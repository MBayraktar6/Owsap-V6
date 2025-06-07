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
#include "shopEx.h"
#include "group_text_parse_tree.h"

bool CShopEx::Create(DWORD dwVnum, DWORD dwNPCVnum)
{
	m_dwVnum = dwVnum;
	m_dwNPCVnum = dwNPCVnum;
	return true;
}

bool CShopEx::AddShopTable(TShopTableEx& shopTable)
{
	for (auto it = m_vec_shopTabs.begin(); it != m_vec_shopTabs.end(); it++)
	{
		const TShopTableEx& _shopTable = *it;
		if (0 != _shopTable.dwVnum && _shopTable.dwVnum == shopTable.dwVnum)
			return false;
		if (0 != _shopTable.dwNPCVnum && _shopTable.dwNPCVnum == shopTable.dwVnum)
			return false;
	}
	m_vec_shopTabs.push_back(shopTable);
	return true;
}

bool CShopEx::AddGuest(LPCHARACTER ch, DWORD owner_vid, bool bOtherEmpire)
{
	if (!ch)
		return false;

	if (ch->GetExchange())
		return false;

	if (ch->GetShop())
		return false;

	ch->SetShop(this);

	m_map_guest.insert(GuestMapType::value_type(ch, bOtherEmpire));

	TPacketGCShop pack;

	pack.header = HEADER_GC_SHOP;
	pack.subheader = SHOP_SUBHEADER_GC_START_EX;

	TPacketGCShopStartEx pack2;

	memset(&pack2, 0, sizeof(pack2));

	pack2.owner_vid = owner_vid;
	pack2.shop_tab_count = m_vec_shopTabs.size();

#if defined(__SHOPEX_RENEWAL__)
	char temp[8096 * 3]; // 최대 1728 * 3
#else
	char temp[8096]; // 최대 1728 * 3
#endif
	char* buf = &temp[0];
	size_t size = 0;

	for (auto it = m_vec_shopTabs.begin(); it != m_vec_shopTabs.end(); it++)
	{
		const TShopTableEx& shop_tab = *it;
		TPacketGCShopStartEx::TSubPacketShopTab pack_tab;
		pack_tab.coin_type = shop_tab.coinType;
		memcpy(pack_tab.name, shop_tab.name.c_str(), SHOP_TAB_NAME_MAX);

		for (BYTE i = 0; i < SHOP_HOST_ITEM_MAX_NUM; i++)
		{
			pack_tab.items[i].dwVnum = shop_tab.items[i].vnum;
			pack_tab.items[i].dwCount = shop_tab.items[i].count;
			switch (shop_tab.coinType)
			{
			case SHOP_COIN_TYPE_GOLD:
				if (bOtherEmpire) // no empire price penalty for pc shop
				{
#if defined(__SHOPEX_EMPIRE_TAX__)
					pack_tab.items[i].dwPrice = shop_tab.items[i].price * 3;
#else
					pack_tab.items[i].dwPrice = shop_tab.items[i].price;
#endif
				}
				else
					pack_tab.items[i].dwPrice = shop_tab.items[i].price;
				break;
			case SHOP_COIN_TYPE_SECONDARY_COIN:
				pack_tab.items[i].dwPrice = shop_tab.items[i].price;
				break;
			}
#if defined(__SHOPEX_RENEWAL__)
			pack_tab.items[i].bPriceType = shop_tab.items[i].bPriceType;
			pack_tab.items[i].dwPriceVnum = shop_tab.items[i].dwPriceVnum;
			thecore_memcpy(pack_tab.items[i].alSockets, shop_tab.items[i].alSockets, sizeof(pack_tab.items[i].alSockets));
#else
			memset(pack_tab.items[i].aAttr, 0, sizeof(pack_tab.items[i].aAttr));
			memset(pack_tab.items[i].alSockets, 0, sizeof(pack_tab.items[i].alSockets));
#endif
#if defined(__CHANGE_LOOK_SYSTEM__)
			pack_tab.items[i].dwTransmutationVnum = 0;
#endif
#if defined(__SET_ITEM__)
			pack_tab.items[i].bSetValue = 0;
#endif
		}

		memcpy(buf, &pack_tab, sizeof(pack_tab));
		buf += sizeof(pack_tab);
		size += sizeof(pack_tab);
	}

	pack.size = sizeof(pack) + sizeof(pack2) + size;

	ch->GetDesc()->BufferedPacket(&pack, sizeof(TPacketGCShop));
	ch->GetDesc()->BufferedPacket(&pack2, sizeof(TPacketGCShopStartEx));
	ch->GetDesc()->Packet(temp, size);

	return true;
}

#if defined(__PRIVATESHOP_SEARCH_SYSTEM__)
int CShopEx::Buy(LPCHARACTER ch, BYTE pos, bool bIsShopSearch)
#else
int CShopEx::Buy(LPCHARACTER ch, BYTE pos)
#endif
{
	BYTE tabIdx = pos / SHOP_HOST_ITEM_MAX_NUM;
	BYTE slotPos = pos % SHOP_HOST_ITEM_MAX_NUM;

	if (tabIdx >= GetTabCount())
	{
		sys_log(0, "ShopEx::Buy : invalid position %d : %s", pos, ch->GetName());
		return SHOP_SUBHEADER_GC_INVALID_POS;
	}

	sys_log(0, "ShopEx::Buy : name %s pos %d", ch->GetName(), pos);

	GuestMapType::iterator it = m_map_guest.find(ch);

	if (it == m_map_guest.end())
		return SHOP_SUBHEADER_GC_END;

	TShopTableEx& shopTab = m_vec_shopTabs[tabIdx];
	TShopItemTable& r_item = shopTab.items[slotPos];

	if (r_item.price <= 0)
	{
		LogManager::instance().HackLog("SHOP_BUY_GOLD_OVERFLOW", ch);
		return SHOP_SUBHEADER_GC_NOT_ENOUGH_MONEY;
	}

	unsigned long long dwPrice = r_item.price;

#if !defined(__SHOPEX_RENEWAL__)
	switch (shopTab.coinType)
	{
	case SHOP_COIN_TYPE_GOLD:
		if (it->second) // other empire, price is triple
			dwPrice *= 3;

		if (ch->GetGold() < dwPrice)
		{
			sys_log(1, "ShopEx::Buy : Not enough money : %s has %llu, price %llu", ch->GetName(), ch->GetGold(), dwPrice);
			return SHOP_SUBHEADER_GC_NOT_ENOUGH_MONEY;
		}
		break;
	case SHOP_COIN_TYPE_SECONDARY_COIN:
	{
		int count = ch->CountSpecifyTypeItem(ITEM_SECONDARY_COIN);
		if (count < dwPrice)
		{
			sys_log(1, "ShopEx::Buy : Not enough myeongdojun : %s has %llu, price %llu", ch->GetName(), count, dwPrice);
			return SHOP_SUBHEADER_GC_NOT_ENOUGH_MONEY_EX;
		}
	}
	break;
	}
#else
	switch (r_item.bPriceType)
	{
		case SHOP_COIN_TYPE_GOLD:
		{
#if defined(__SHOPEX_EMPIRE_TAX__)
			if (it->second) // if other empire, price is triple
				dwPrice *= 3;
#endif
			if (static_cast<decltype(dwPrice)>(ch->GetGold()) < dwPrice)
				return SHOP_SUBHEADER_GC_NOT_ENOUGH_MONEY;
		}
		break;

		case SHOP_COIN_TYPE_SECONDARY_COIN:
			if (static_cast<decltype(dwPrice)>(ch->CountSpecifyTypeItem(ITEM_SECONDARY_COIN)) < dwPrice)
				return SHOP_SUBHEADER_GC_NOT_ENOUGH_MONEY_EX;
			break;

		case SHOP_COIN_TYPE_ITEM:
			if (static_cast<decltype(dwPrice)>(ch->CountSpecifyItem(r_item.dwPriceVnum)) < dwPrice)
				return SHOP_SUBHEADER_GC_NOT_ENOUGH_ITEM;
			break;
	}
#endif

	LPITEM item;

	item = ITEM_MANAGER::instance().CreateItem(r_item.vnum, r_item.count);

	if (!item)
		return SHOP_SUBHEADER_GC_SOLD_OUT;

	int iEmptyPos;
#if defined(__DRAGON_SOUL_SYSTEM__)
	if (item->IsDragonSoul())
	{
		iEmptyPos = ch->GetEmptyDragonSoulInventory(item);
	}
	else
#endif
	{
		iEmptyPos = ch->GetEmptyInventory(item->GetSize());
	}

	if (iEmptyPos < 0)
	{
		sys_log(1, "ShopEx::Buy : Inventory full : %s size %d", ch->GetName(), item->GetSize());
		M2_DESTROY_ITEM(item);
		return SHOP_SUBHEADER_GC_INVENTORY_FULL;
	}

#if defined(__SHOPEX_RENEWAL__)
	switch (r_item.bPriceType)
	{
	case SHOP_COIN_TYPE_GOLD:
		ch->PointChange(POINT_GOLD, -dwPrice, false);
		break;
	case SHOP_COIN_TYPE_SECONDARY_COIN:
		ch->RemoveSpecifyTypeItem(ITEM_SECONDARY_COIN, dwPrice);
		break;
	case SHOP_COIN_TYPE_ITEM:
		ch->RemoveSpecifyItem(r_item.dwPriceVnum, dwPrice);
		break;
	}
	if (!std::all_of(std::begin(r_item.alSockets), std::end(r_item.alSockets), [](const long& s) { return !s; }))
		item->SetSockets(r_item.alSockets);
#else
	switch (shopTab.coinType)
	{
	case SHOP_COIN_TYPE_GOLD:
		ch->PointChange(POINT_GOLD, -dwPrice, false);
		break;
	case SHOP_COIN_TYPE_SECONDARY_COIN:
		ch->RemoveSpecifyTypeItem(ITEM_SECONDARY_COIN, dwPrice);
		break;
	}
#endif

#if defined(__DRAGON_SOUL_SYSTEM__)
	if (item->IsDragonSoul())
		item->AddToCharacter(ch, TItemPos(DRAGON_SOUL_INVENTORY, iEmptyPos));
	else
#endif
		item->AddToCharacter(ch, TItemPos(INVENTORY, iEmptyPos));

	ITEM_MANAGER::instance().FlushDelayedSave(item);
	LogManager::instance().ItemLog(ch, item, "BUY", item->GetName());

	if (item->GetVnum() >= 80003 && item->GetVnum() <= 80007)
	{
		LogManager::instance().GoldBarLog(ch->GetPlayerID(), item->GetID(), PERSONAL_SHOP_BUY, "");
	}

	DBManager::instance().SendMoneyLog(MONEY_LOG_SHOP, item->GetVnum(), -dwPrice);

	if (item)
		sys_log(0, "ShopEx: BUY: name %s %s(x %d):%u price %llu", ch->GetName(), item->GetName(), item->GetCount(), item->GetID(), dwPrice);

	ch->Save();

	return (SHOP_SUBHEADER_GC_OK);
}
