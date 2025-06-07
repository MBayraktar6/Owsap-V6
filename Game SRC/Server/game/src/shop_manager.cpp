#include "stdafx.h"
#include "../../libgame/include/grid.h"
#include "constants.h"
#include "utils.h"
#include "config.h"
#include "shop.h"
#include "desc.h"
#include "desc_manager.h"
#include "char.h"
#include "char_manager.h"
#include "item.h"
#include "item_manager.h"
#include "buffer_manager.h"
#include "packet.h"
#include "log.h"
#include "db.h"
#include "questmanager.h"
#include "monarch.h"
#include "mob_manager.h"
#include "locale_service.h"
#include "desc_client.h"
#include "shop_manager.h"
#include "group_text_parse_tree.h"
#include "shopEx.h"
#include "shop_manager.h"
#include <cctype>

#if defined(__PRIVATESHOP_SEARCH_SYSTEM__)
#	include "unique_item.h"
#	include "target.h"
#endif

CShopManager::CShopManager()
{
}

CShopManager::~CShopManager()
{
	Destroy();
}

bool CShopManager::Initialize(TShopTable* table, int size)
{
	if (!m_map_pkShop.empty())
		return false;

	int i;

	for (i = 0; i < size; ++i, ++table)
	{
		LPSHOP shop = M2_NEW CShop;

		if (!shop->Create(table->dwVnum, table->dwNPCVnum, table->items))
		{
			M2_DELETE(shop);
			continue;
		}

		m_map_pkShop.insert(TShopMap::value_type(table->dwVnum, shop));
		m_map_pkShopByNPCVnum.insert(TShopMap::value_type(table->dwNPCVnum, shop));
	}

	char szShopTableExFileName[256];
	snprintf(szShopTableExFileName, sizeof(szShopTableExFileName),
		"%s/shop_table_ex.txt", LocaleService_GetBasePath().c_str());

	return ReadShopTableEx(szShopTableExFileName);
}

void CShopManager::Destroy()
{
	TShopMap::iterator it = m_map_pkShop.begin();
	std::unordered_set<decltype(m_map_pkShop)::value_type::second_type> collector; // avoid delete duplicate objects(unique key)
	while (it != m_map_pkShop.end())
	{
		//delete it->second;
		collector.insert(it->second);
		++it;
	}
	for (auto& v : collector)
		delete v;

	m_map_pkShop.clear();
}

LPSHOP CShopManager::Get(DWORD dwVnum)
{
	TShopMap::const_iterator it = m_map_pkShop.find(dwVnum);

	if (it == m_map_pkShop.end())
		return NULL;

	return (it->second);
}

LPSHOP CShopManager::GetByNPCVnum(DWORD dwVnum)
{
	TShopMap::const_iterator it = m_map_pkShopByNPCVnum.find(dwVnum);

	if (it == m_map_pkShopByNPCVnum.end())
		return NULL;

	return (it->second);
}

/*
* 인터페이스 함수들
*/

// 상점 거래를 시작
bool CShopManager::StartShopping(LPCHARACTER pkChr, LPCHARACTER pkChrShopKeeper, int iShopVnum)
{
#if defined(__QUEST_EVENT_BUY_SELL__)
	if (pkChr->GetShop())
		return false;
#endif

	if (pkChr->GetShopOwner() == pkChrShopKeeper)
		return false;

	// this method is only for NPC
	if (pkChrShopKeeper->IsPC())
		return false;

	// PREVENT_TRADE_WINDOW
	if (pkChr->PreventTradeWindow(WND_SHOPOWNER, true/*except*/))
	{
		pkChr->ChatPacket(CHAT_TYPE_INFO, LC_STRING("다른 거래창이 열린상태에서는 상점거래를 할수 가 없습니다."));
		return false;
	}
	// END_PREVENT_TRADE_WINDOW

	long distance = DISTANCE_APPROX(pkChr->GetX() - pkChrShopKeeper->GetX(), pkChr->GetY() - pkChrShopKeeper->GetY());

	if (distance >= SHOP_MAX_DISTANCE)
	{
		sys_log(1, "SHOP: TOO_FAR: %s distance %d", pkChr->GetName(), distance);
		return false;
	}

	LPSHOP pkShop;

	if (iShopVnum)
		pkShop = Get(iShopVnum);
	else
		pkShop = GetByNPCVnum(pkChrShopKeeper->GetRaceNum());

	if (!pkShop)
	{
		sys_log(1, "SHOP: NO SHOP");
		return false;
	}

	bool bOtherEmpire = false;

	if (pkChr->GetEmpire() != pkChrShopKeeper->GetEmpire())
		bOtherEmpire = true;

	pkShop->AddGuest(pkChr, pkChrShopKeeper->GetVID(), bOtherEmpire);
	pkChr->SetShopOwner(pkChrShopKeeper);
	sys_log(0, "SHOP: START: %s", pkChr->GetName());
	return true;
}

LPSHOP CShopManager::FindPCShop(DWORD dwVID)
{
	TShopMap::iterator it = m_map_pkShopByPC.find(dwVID);

	if (it == m_map_pkShopByPC.end())
		return NULL;

	return it->second;
}

LPSHOP CShopManager::CreatePCShop(LPCHARACTER ch, TShopItemTable* pTable, BYTE bItemCount)
{
	if (FindPCShop(ch->GetVID()))
		return NULL;

	LPSHOP pkShop = M2_NEW CShop;
	pkShop->SetPCShop(ch);
	pkShop->SetShopItems(pTable, bItemCount);

	m_map_pkShopByPC.insert(TShopMap::value_type(ch->GetVID(), pkShop));
	return pkShop;
}

void CShopManager::DestroyPCShop(LPCHARACTER ch)
{
	LPSHOP pkShop = FindPCShop(ch->GetVID());

	if (!pkShop)
		return;

	// PREVENT_ITEM_COPY;
	ch->SetMyShopTime();
	// END_PREVENT_ITEM_COPY

	m_map_pkShopByPC.erase(ch->GetVID());
	M2_DELETE(pkShop);
}

// 상점 거래를 종료
void CShopManager::StopShopping(LPCHARACTER ch)
{
	LPSHOP shop;

	if (!(shop = ch->GetShop()))
		return;

	// PREVENT_ITEM_COPY;
	ch->SetMyShopTime();
	// END_PREVENT_ITEM_COPY

	shop->RemoveGuest(ch);
	sys_log(0, "SHOP: END: %s", ch->GetName());
}

// 아이템 구입
void CShopManager::Buy(LPCHARACTER ch, BYTE pos)
{
	if (!ch->GetShop())
		return;

	if (!ch->GetShopOwner())
		return;

	if (DISTANCE_APPROX(ch->GetX() - ch->GetShopOwner()->GetX(), ch->GetY() - ch->GetShopOwner()->GetY()) > 2000)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_STRING("상점과의 거리가 너무 멀어 물건을 살 수 없습니다."));
		return;
	}

	CShop* pkShop = ch->GetShop();
	if (!pkShop)
		return;

	// PREVENT_ITEM_COPY
	ch->SetMyShopTime();
	// END_PREVENT_ITEM_COPY

	int ret = pkShop->Buy(ch, pos);

	if (SHOP_SUBHEADER_GC_OK != ret) // 문제가 있었으면 보낸다.
	{
		TPacketGCShop pack;

		pack.header = HEADER_GC_SHOP;
		pack.subheader = ret;
		pack.size = sizeof(TPacketGCShop);

		ch->GetDesc()->Packet(&pack, sizeof(pack));
	}
}

void CShopManager::Sell(LPCHARACTER ch, WORD wCell, WORD wCount, BYTE bType)
{
	if (!ch->GetShop())
		return;

	if (!ch->GetShopOwner())
		return;

	if (!ch->CanHandleItem())
		return;

	if (ch->GetShop()->IsPCShop())
		return;

	if (DISTANCE_APPROX(ch->GetX() - ch->GetShopOwner()->GetX(), ch->GetY() - ch->GetShopOwner()->GetY()) > 2000)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_STRING("상점과의 거리가 너무 멀어 물건을 팔 수 없습니다."));
		return;
	}

	LPITEM item = ch->GetInventoryItem(wCell);

	if (!item)
		return;

	if (item->IsEquipped() == true)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_STRING("착용 중인 아이템은 판매할 수 없습니다."));
		return;
	}

	if (true == item->isLocked())
		return;

#if defined(__SOUL_BIND_SYSTEM__)
	if (item->IsSealed())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_STRING("You cannot sell a soulbound item."));
		return;
	}
#endif

	if (IS_SET(item->GetAntiFlag(), ITEM_ANTIFLAG_SELL))
		return;

	DWORD dwPrice;

	if (wCount == 0 || wCount > item->GetCount())
		wCount = item->GetCount();

	dwPrice = item->GetShopSellPrice();

	if (IS_SET(item->GetFlag(), ITEM_FLAG_COUNT_PER_1GOLD))
	{
		if (dwPrice == 0)
			dwPrice = wCount;
		else
			dwPrice = wCount / dwPrice;
	}
	else
		dwPrice *= wCount;

	dwPrice /= 5;

	// 세금 계산
	DWORD dwTax = 0;
	int iVal = 3;

	dwTax = dwPrice * iVal / 100;
	dwPrice -= dwTax;

	if (test_server)
		sys_log(0, "Sell Item price id %d %s itemid %d", ch->GetPlayerID(), ch->GetName(), item->GetID());

	const int64_t nTotalMoney = static_cast<int64_t>(ch->GetGold()) + static_cast<int64_t>(dwPrice);
	if (GOLD_MAX <= nTotalMoney)
	{
		sys_err("[OVERFLOW_GOLD] id %u name %s gold %d", ch->GetPlayerID(), ch->GetName(), ch->GetGold());
		ch->ChatPacket(CHAT_TYPE_INFO, LC_STRING("20억냥이 초과하여 물품을 팔수 없습니다."));
		return;
	}

	// 20050802.myevan.상점 판매 로그에 아이템 ID 추가
	sys_log(0, "SHOP: SELL: %s item name: %s(x%d):%u price: %u", ch->GetName(), item->GetName(), wCount, item->GetID(), dwPrice);

	if (iVal > 0)
		ch->ChatPacket(CHAT_TYPE_INFO, LC_STRING("판매금액의 %d %% 가 세금으로 나가게됩니다", iVal));

	DBManager::instance().SendMoneyLog(MONEY_LOG_SHOP, item->GetVnum(), dwPrice);

#if defined(__QUEST_EVENT_BUY_SELL__)
	CShop* pkShop = ch->GetShop();
	if (pkShop)
	{
		ch->SetQuestNPCID(ch->GetShopOwner() ? ch->GetShopOwner()->GetVID() : 0);
		quest::CQuestManager::instance().SellItem(ch->GetPlayerID(), pkShop->GetNPCVnum(), item);
	}
#endif

	if (wCount == item->GetCount())
		ITEM_MANAGER::instance().RemoveItem(item, "SELL");
	else
		item->SetCount(item->GetCount() - wCount);

	// 군주 시스템 : 세금 징수
	CMonarch::instance().SendtoDBAddMoney(dwTax, ch->GetEmpire(), ch);

	ch->PointChange(POINT_GOLD, dwPrice, false);
}

bool CompareShopItemName(const SShopItemTable& lhs, const SShopItemTable& rhs)
{
	TItemTable* lItem = ITEM_MANAGER::instance().GetTable(lhs.vnum);
	TItemTable* rItem = ITEM_MANAGER::instance().GetTable(rhs.vnum);
	if (lItem && rItem)
		return strcmp(lItem->szLocaleName, rItem->szLocaleName) < 0;
	else
		return true;
}

#if defined(__SHOPEX_RENEWAL__)
bool CompareShopItemVnum(const SShopItemTable& lhs, const SShopItemTable& rhs)
{
	return lhs.vnum > rhs.vnum;
}

bool CompareShopItemPrice(const SShopItemTable& lhs, const SShopItemTable& rhs)
{
	return lhs.price > rhs.price;
}

bool CompareShopItemType(const SShopItemTable& lhs, const SShopItemTable& rhs)
{
	const TItemTable* lItem = ITEM_MANAGER::instance().GetTable(lhs.vnum);
	const TItemTable* rItem = ITEM_MANAGER::instance().GetTable(rhs.vnum);
	return (lItem && rItem) ? lItem->bType > rItem->bType : true;
}
#endif

static bool iequals(const std::string& a, const std::string& b)
{
	return std::equal(a.begin(), a.end(),
		b.begin(), b.end(),
		[](char a, char b) {
			return tolower(a) == tolower(b);
		});
}

bool ConvertToShopItemTable(IN CGroupNode* pNode, OUT TShopTableEx& shopTable)
{
	if (!pNode->GetValue("vnum", 0, shopTable.dwVnum))
	{
		sys_err("Group %s does not have vnum.", pNode->GetNodeName().c_str());
		return false;
	}

	if (!pNode->GetValue("name", 0, shopTable.name))
	{
		sys_err("Group %s does not have name.", pNode->GetNodeName().c_str());
		return false;
	}

	if (shopTable.name.length() >= SHOP_TAB_NAME_MAX)
	{
		sys_err("Shop name length must be less than %d. Error in Group %s, name %s", SHOP_TAB_NAME_MAX, pNode->GetNodeName().c_str(), shopTable.name.c_str());
		return false;
	}

	std::string stCoinType;
	if (!pNode->GetValue("cointype", 0, stCoinType))
	{
		stCoinType = "Gold";
	}

	if (iequals(stCoinType, "Gold"))
	{
		shopTable.coinType = SHOP_COIN_TYPE_GOLD;
	}
	else if (iequals(stCoinType, "SecondaryCoin"))
	{
		shopTable.coinType = SHOP_COIN_TYPE_SECONDARY_COIN;
	}
	else
	{
		sys_err("Group %s has undefine cointype(%s).", pNode->GetNodeName().c_str(), stCoinType.c_str());
		return false;
	}

	CGroupNode* pItemGroup = pNode->GetChildNode("items");
	if (!pItemGroup)
	{
		sys_err("Group %s does not have 'group items'.", pNode->GetNodeName().c_str());
		return false;
	}

	size_t itemGroupSize = pItemGroup->GetRowCount();
	std::vector<TShopItemTable> shopItems(itemGroupSize);
	if (itemGroupSize >= SHOP_HOST_ITEM_MAX_NUM)
	{
		sys_err("count(%d) of rows of group items of group %s must be smaller than %d", itemGroupSize, pNode->GetNodeName().c_str(), SHOP_HOST_ITEM_MAX_NUM);
		return false;
	}

	for (size_t i = 0; i < itemGroupSize; i++)
	{
		if (!pItemGroup->GetValue(i, "vnum", shopItems[i].vnum))
		{
			sys_err("row(%d) of group items of group %s does not have vnum column", i, pNode->GetNodeName().c_str());
			return false;
		}

		if (!pItemGroup->GetValue(i, "count", shopItems[i].count))
		{
			sys_err("row(%d) of group items of group %s does not have count column", i, pNode->GetNodeName().c_str());
			return false;
		}

		if (!pItemGroup->GetValue(i, "price", shopItems[i].price))
		{
			sys_err("row(%d) of group items of group %s does not have price column", i, pNode->GetNodeName().c_str());
			return false;
		}

		// NOTE : Set the item price to the default item proto shop buy price.
		if (shopItems[i].price == 0)
		{
			TItemTable* itemTable = ITEM_MANAGER::instance().GetTable(shopItems[i].vnum);
			if (itemTable != NULL)
				shopItems[i].price = itemTable->dwShopBuyPrice * shopItems[i].count;
		}

#if defined(__SHOPEX_RENEWAL__)
		if (shopItems[i].bPriceType >= SHOP_COIN_MAX_TYPE)
		{
			sys_err("row(%d) of group items of group %s price_type is wrong!", i, pNode->GetNodeName().c_str());
			return false;
		}

		char getval[20];
		for (int j = 0; j < ITEM_SOCKET_MAX_NUM; j++)
		{
			snprintf(getval, sizeof(getval), "socket%d", j);
			if (!pItemGroup->GetValue(i, getval, shopItems[i].alSockets[j]))
			{
				sys_err("row(%d) stage %d of group items of group %s does not have socket column", i, j, pNode->GetNodeName().c_str());
				return false;
			}
		}

		if (pItemGroup->GetValue(i, "price_type", shopItems[i].bPriceType) &&
			pItemGroup->GetValue(i, "price_vnum", shopItems[i].dwPriceVnum) &&
			shopItems[i].bPriceType == SHOP_COIN_TYPE_ITEM)
		{
			if (!ITEM_MANAGER::instance().GetTable(shopItems[i].dwPriceVnum))
			{
				sys_err("CANNOT GET ITEM PROTO %d", shopItems[i].dwPriceVnum);
				return false;
			}
		}
#endif
	}

	std::string stSort;
	if (!pNode->GetValue("sort", 0, stSort))
		stSort = "None";

	if (iequals(stSort, "Asc"))
		std::sort(shopItems.begin(), shopItems.end(), CompareShopItemName);
	else if (iequals(stSort, "Desc"))
		std::sort(shopItems.rbegin(), shopItems.rend(), CompareShopItemName);
#if defined(__SHOPEX_RENEWAL__)
	else if (iequals(stSort, "Vnum"))
		std::sort(shopItems.rbegin(), shopItems.rend(), CompareShopItemVnum);
	else if (iequals(stSort, "Price"))
		std::sort(shopItems.begin(), shopItems.end(), CompareShopItemPrice);
	else if (iequals(stSort, "Name"))
		std::sort(shopItems.begin(), shopItems.end(), CompareShopItemName);
	else if (iequals(stSort, "Type"))
		std::sort(shopItems.begin(), shopItems.end(), CompareShopItemType);
#endif

	CGrid grid = CGrid(CShop::SHOP_GRID_WIDTH, CShop::SHOP_GRID_HEIGHT);
	int iPos;

	memset(&shopTable.items[0], 0, sizeof(shopTable.items));

	for (size_t i = 0; i < shopItems.size(); i++)
	{
		TItemTable* item_table = ITEM_MANAGER::instance().GetTable(shopItems[i].vnum);
		if (!item_table)
		{
			sys_err("vnum(%d) of group items of group %s does not exist", shopItems[i].vnum, pNode->GetNodeName().c_str());
			return false;
		}

		iPos = grid.FindBlank(1, item_table->bSize);

		grid.Put(iPos, 1, item_table->bSize);
		shopTable.items[iPos] = shopItems[i];
	}

	shopTable.bItemCount = shopItems.size();
	return true;
}

bool CShopManager::ReadShopTableEx(const char* stFileName)
{
	// file 유무 체크.
	// 없는 경우는 에러로 처리하지 않는다.
	FILE* fp = fopen(stFileName, "rb");
	if (NULL == fp)
		return true;
	fclose(fp);

	CGroupTextParseTreeLoader loader;
	if (!loader.Load(stFileName))
	{
		sys_err("%s Load fail.", stFileName);
		return false;
	}

	CGroupNode* pShopNPCGroup = loader.GetGroup("shopnpc");
	if (NULL == pShopNPCGroup)
	{
		sys_err("Group ShopNPC is not exist.");
		return false;
	}

	typedef std::multimap <DWORD, TShopTableEx> TMapNPCshop;
	TMapNPCshop map_npcShop;

#if defined(__SHOPEX_RENEWAL__)
	{
		std::unordered_set<CShop*> v;
		// include unordered_set
		auto ExDelete = [&v](TShopMap& c)
		{
			for (auto it = c.begin(); !c.empty() && it != c.end();)
			{
				const auto shop = it->second;
				if (shop && shop->IsShopEx())
				{
					it = c.erase(it);
					v.insert(shop);
				}
				else
					++it;
			}
		};
		ExDelete(m_map_pkShopByNPCVnum);
		ExDelete(m_map_pkShop);
		for (const auto& del : v)
			delete del;
	}
#endif

	for (size_t i = 0; i < pShopNPCGroup->GetRowCount(); i++)
	{
		DWORD npcVnum;
		std::string shopName;
		if (!pShopNPCGroup->GetValue(i, "npc", npcVnum) || !pShopNPCGroup->GetValue(i, "group", shopName))
		{
			sys_err("Invalid row(%d). Group ShopNPC rows must have 'npc', 'group' columns", i);
			return false;
		}
		std::transform(shopName.begin(), shopName.end(), shopName.begin(), (int(*)(int))std::tolower);
		CGroupNode* pShopGroup = loader.GetGroup(shopName.c_str());
		if (!pShopGroup)
		{
			sys_err("Group %s is not exist.", shopName.c_str());
			return false;
		}
		TShopTableEx table;
		if (!ConvertToShopItemTable(pShopGroup, table))
		{
			sys_err("Cannot read Group %s.", shopName.c_str());
			return false;
		}
		if (m_map_pkShopByNPCVnum.find(npcVnum) != m_map_pkShopByNPCVnum.end())
		{
			sys_err("%d cannot have both original shop and extended shop", npcVnum);
			return false;
		}

		map_npcShop.insert(TMapNPCshop::value_type(npcVnum, table));
	}

	for (TMapNPCshop::iterator it = map_npcShop.begin(); it != map_npcShop.end(); ++it)
	{
		DWORD npcVnum = it->first;
		TShopTableEx& table = it->second;
		if (m_map_pkShop.find(table.dwVnum) != m_map_pkShop.end())
		{
			sys_err("Shop vnum(%d) already exists", table.dwVnum);
			return false;
		}
		TShopMap::iterator shop_it = m_map_pkShopByNPCVnum.find(npcVnum);

		LPSHOPEX pkShopEx = NULL;
		if (m_map_pkShopByNPCVnum.end() == shop_it)
		{
			pkShopEx = M2_NEW CShopEx;
			pkShopEx->Create(0, npcVnum);
			m_map_pkShopByNPCVnum.insert(TShopMap::value_type(npcVnum, pkShopEx));
		}
		else
		{
			pkShopEx = dynamic_cast <CShopEx*> (shop_it->second);
			if (NULL == pkShopEx)
			{
				sys_err("WTF!!! It can't be happend. NPC(%d) Shop is not extended version.", shop_it->first);
				return false;
			}
		}

		if (pkShopEx->GetTabCount() >= SHOP_TAB_COUNT_MAX)
		{
			sys_err("ShopEx cannot have tab more than %d", SHOP_TAB_COUNT_MAX);
			return false;
		}

		if (m_map_pkShop.find(table.dwVnum) != m_map_pkShop.end())
		{
			sys_err("Shop vnum(%d) already exist.", table.dwVnum);
			return false;
		}
		m_map_pkShop.insert(TShopMap::value_type(table.dwVnum, pkShopEx));
		pkShopEx->AddShopTable(table);
	}

	return true;
}

#if defined(__PRIVATESHOP_SEARCH_SYSTEM__)
void CShopManager::ShopSearchProcess(LPCHARACTER ch, const TPacketCGPrivateShopSearch* p)
{
	if (ch == NULL || ch->GetDesc() == NULL || p == NULL)
		return;

	if (ch->PreventTradeWindow(WND_SHOPSEARCH, true/*except*/))
		return;

	TEMP_BUFFER buf;

	for (std::map<DWORD, CShop*>::const_iterator it = m_map_pkShopByPC.begin(); it != m_map_pkShopByPC.end(); ++it)
	{
		CShop* tShopTable = it->second;
		if (tShopTable == NULL)
			continue;

		LPCHARACTER GetOwner = tShopTable->GetShopOwner();
		if (GetOwner == NULL || ch == GetOwner)
			continue;

		const std::vector<CShop::SHOP_ITEM>& vItemVec = tShopTable->GetItemVector();
		for (std::vector<CShop::SHOP_ITEM>::const_iterator ShopIter = vItemVec.begin(); ShopIter != vItemVec.end(); ++ShopIter)
		{
			LPITEM item = ShopIter->pkItem;
			if (item == NULL)
				continue;

			/* First n character equals(case insensitive) */
			if (strncasecmp(item->GetName(), p->szItemName, strlen(p->szItemName)))
				continue;

			if ((p->iMinRefine <= item->GetRefineLevel() && p->iMaxRefine >= item->GetRefineLevel()) == false)
				continue;

			if ((p->iMinLevel <= item->GetLevelLimit() && p->iMaxLevel >= item->GetLevelLimit()) == false)
				continue;

			if ((p->iMinGold <= ShopIter->price && p->iMaxGold >= ShopIter->price) == false)
				continue;

#if defined(__CHEQUE_SYSTEM__)
			if ((p->iMinCheque <= ShopIter->cheque && p->iMaxCheque >= ShopIter->cheque) == false)
				continue;
#endif

			if (p->bMaskType != ITEM_NONE && p->bMaskType != item->GetType()) // ITEM_NONE: All Categories
				continue;

			if (p->iMaskSub != -1 && p->iMaskSub != item->GetSubType()) // -1: No SubType Check
				continue;

			switch (p->bJob)
			{
				case JOB_WARRIOR:
					if (item->GetAntiFlag() & ITEM_ANTIFLAG_WARRIOR)
						continue;
					break;

				case JOB_ASSASSIN:
					if (item->GetAntiFlag() & ITEM_ANTIFLAG_ASSASSIN)
						continue;
					break;

				case JOB_SHAMAN:
					if (item->GetAntiFlag() & ITEM_ANTIFLAG_SHAMAN)
						continue;
					break;

				case JOB_SURA:
					if (item->GetAntiFlag() & ITEM_ANTIFLAG_SURA)
						continue;
					break;

#if defined(__WOLFMAN_CHARACTER__)
				case JOB_WOLFMAN:
					if (item->GetAntiFlag() & ITEM_ANTIFLAG_WOLFMAN)
						continue;
					break;
#endif
			}

			TPacketGCPrivateShopSearchItem pack2;
			pack2.item.dwVnum = ShopIter->vnum;
			pack2.item.dwCount = ShopIter->count;
			pack2.item.dwPrice = ShopIter->price;
#if defined(__CHEQUE_SYSTEM__)
			pack2.item.dwCheque = ShopIter->cheque;
#endif
			pack2.item.bDisplayPos = static_cast<BYTE>(std::distance(vItemVec.begin(), ShopIter));
			pack2.dwShopPID = GetOwner->GetPlayerID();
			pack2.dwShopVID = GetOwner->GetVID();
			std::memcpy(&pack2.szSellerName, GetOwner->GetName(), sizeof(pack2.szSellerName));
			std::memcpy(&pack2.item.alSockets, item->GetSockets(), sizeof(pack2.item.alSockets));
#if defined(__CHANGE_LOOK_SYSTEM__)
			pack2.item.dwTransmutationVnum = item->GetTransmutationVnum();
#endif
#if defined(__REFINE_ELEMENT_SYSTEM__)
			std::memcpy(&pack2.item.RefineElement, item->GetRefineElement(), sizeof(pack2.item.RefineElement));
#endif
#if defined(__ITEM_APPLY_RANDOM__)
			std::memcpy(&pack2.item.aApplyRandom, item->GetRandomApplies(), sizeof(pack2.item.aApplyRandom));
#endif
#if defined(__SET_ITEM__)
			pack2.item.bSetValue = item->GetItemSetValue();
#endif
			std::memcpy(&pack2.item.aAttr, item->GetAttributes(), sizeof(pack2.item.aAttr));
			buf.write(&pack2, sizeof(pack2));
		}
	}

	if (buf.size() <= 0)
		return;

	TPacketGCPrivateShopSearch pack;
	pack.header = HEADER_GC_PRIVATESHOP_SEARCH;
	pack.size = static_cast<WORD>(sizeof(pack) + buf.size());
	ch->GetDesc()->BufferedPacket(&pack, sizeof(pack));
	ch->GetDesc()->Packet(buf.read_peek(), buf.size());
}

void CShopManager::ShopSearchBuy(LPCHARACTER ch, const TPacketCGPrivateShopSearchBuyItem* p)
{
	if (ch == NULL || ch->GetDesc() == NULL || p == NULL)
		return;

	if (ch->PreventTradeWindow(WND_SHOPSEARCH, true/*except*/))
		return;

	LPCHARACTER ShopCH = CHARACTER_MANAGER::instance().FindByPID(p->dwShopPID);
	if (ShopCH == NULL)
		return;

	if (ch == ShopCH)
		return;

	CShop* pkShop = ShopCH->GetMyShop();
	if (pkShop == NULL || pkShop->IsPCShop() == false)
		return;

	const BYTE bState = ch->GetPrivateShopSearchState();
	switch (bState)
	{
		case SHOP_SEARCH_LOOKING:
		{
			if (ch->CountSpecifyItem(ITEM_PRIVATESHOP_SEARCH_LOOKING_GLASS) == 0)
				return;

			if (ch->GetMapIndex() != ShopCH->GetMapIndex())
				return;

			const DWORD dwSellerVID(ShopCH->GetVID());
			if (CTargetManager::instance().GetTargetInfo(ch->GetPlayerID(), TARGET_TYPE_VID_SHOP_SEARCH, dwSellerVID))
				CTargetManager::instance().DeleteTarget(ch->GetPlayerID(), SHOP_SEARCH_INDEX, "__SHOPSEARCH_TARGET__");

			CTargetManager::Instance().CreateTarget(ch->GetPlayerID(), SHOP_SEARCH_INDEX, "__SHOPSEARCH_TARGET__", TARGET_TYPE_VID_SHOP_SEARCH, dwSellerVID, 0, ch->GetMapIndex(), "Shop Search", 1);
		}
		break;

		case SHOP_SEARCH_TRADING:
		{
			if (ch->CountSpecifyItem(ITEM_PRIVATESHOP_SEARCH_TRADING_GLASS) == 0)
				return;

			ch->SetMyShopTime();
			int ret = pkShop->Buy(ch, p->bPos, true);

			if (SHOP_SUBHEADER_GC_OK != ret)
			{
				TPacketGCShop pack;
				pack.header = HEADER_GC_SHOP;
				pack.subheader = static_cast<BYTE>(ret);
				pack.size = sizeof(TPacketGCShop);
				ch->GetDesc()->Packet(&pack, sizeof(pack));
			}
		}
		break;

		default:
			sys_err("ShopSearchBuy ch(%s) wrong state(%d)", ch->GetName(), bState);
			break;
	}
}
#endif
