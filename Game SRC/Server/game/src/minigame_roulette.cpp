/**
* Filename: minigame_roulette.cpp
* Author: Owsap
* Description: Mini-Game Roulette (Late Summer Event)
**/

#include "stdafx.h"

#if defined(__SUMMER_EVENT_ROULETTE__)
#include "minigame_roulette.h"

#include "char.h"
#include "item.h"
#include "desc.h"
#include "char_manager.h"
#include "questmanager.h"
#include "sectree_manager.h"
#include "config.h"
#include "utils.h"
#include "db.h"
#include "log.h"

#if defined(__MAILBOX__)
#	include "MailBox.h"
#endif

static const char* gs_szQuestFlag_SoulCount = "minigame_roulette.souls";
static const char* gs_szQuestFlag_UsedSoulCount = "minigame_roulette.used_souls";
static const char* gs_szQuestFlag_SpecialExpireTime = "minigame_roulette.ritual_expire_time";

EVENTINFO(MiniGameRouletteEventInfo)
{
	CMiniGameRoulette* pMiniGameRoulette;
	MiniGameRouletteEventInfo()
		: pMiniGameRoulette(NULL)
	{}
};

EVENTFUNC(MiniGameSpecialRouletteEvent)
{
	MiniGameRouletteEventInfo* pEventInfo = dynamic_cast<MiniGameRouletteEventInfo*>(event->info);
	if (pEventInfo == NULL)
	{
		sys_err("MiniGameSpecialRouletteEvent: <Factor> Null pointer.");
		return 0;
	}

	CMiniGameRoulette* pMiniGameRoulette = pEventInfo->pMiniGameRoulette;
	if (pMiniGameRoulette == NULL)
	{
		sys_err("MiniGameSpecialRouletteEvent: <Factor> Null instance.");
		return 0;
	}

	LPCHARACTER pChar = pMiniGameRoulette->GetCharacter();
	if (pChar == NULL)
	{
		sys_err("MiniGameSpecialRouletteEvent: <Factor> Null CHARACTER (pc).");
		return 0;
	}

	LPCHARACTER pNPC = pMiniGameRoulette->GetRouletteCharacter();
	if (pNPC == NULL)
	{
		sys_err("MiniGameSpecialRouletteEvent: <Factor> Null CHARACTER (npc).");
		return 0;
	}

	if (pChar->GetQuestNPC() != pNPC)
	{
		sys_err("MiniGameSpecialRouletteEvent: <Factor> Quest NPC != Roulette Character");
		return 0;
	}

	pNPC->ViewReencode();
	return 0;
}

bool CRouletteManager::ReadRouletteTableFile(const char* c_pszFileName)
{
	m_MapperMap.clear();
	m_ItemMap.clear();

	CTextFileLoader TextFileLoader;
	if (!TextFileLoader.Load(c_pszFileName))
		return false;

	if (!__LoadRouletteMapper(TextFileLoader))
		return false;

	if (!__LoadRouletteItem(TextFileLoader))
		return false;

	return true;
}

bool CRouletteManager::__LoadRouletteMapper(CTextFileLoader& TextFileLoader)
{
	if (!TextFileLoader.SetChildNode("roulettemapper"))
	{
		sys_err("ReadRouletteTableFile : Cannot find `RouletteMapper` node.");
		return false;
	}

	TTokenVector* pToken = NULL;
	for (BYTE bRowIndex = 1; bRowIndex < 256; ++bRowIndex)
	{
		if (!TextFileLoader.GetTokenVector(std::to_string(bRowIndex), &pToken))
			break;

		if (pToken == NULL || pToken->size() < 4)
		{
			sys_err("ReadRouletteTableFile : Invalid token vector at row %u", bRowIndex);
			continue;
		}

		TMapper Mapper;
		Mapper.strGroupName = pToken->at(0);
		std::transform(Mapper.strGroupName.begin(), Mapper.strGroupName.end(), Mapper.strGroupName.begin(), ::tolower);

		if (!str_to_number(Mapper.bType, pToken->at(1).c_str()) ||
			!str_to_number(Mapper.iPrice, pToken->at(2).c_str()) ||
			!str_to_number(Mapper.iSoulCount, pToken->at(3).c_str()))
		{
			sys_err("ReadRouletteTableFile : Error parsing values at row %u", bRowIndex);
			continue;
		}

		m_MapperMap.insert(std::make_pair(bRowIndex, Mapper));
	}

	if (m_MapperMap.empty())
	{
		sys_err("ReadRouletteTableFile : Empty `RouletteMapper`");
		TextFileLoader.SetParentNode();
		return false;
	}

	TextFileLoader.SetParentNode();
	return true;
}

bool CRouletteManager::__LoadRouletteItem(CTextFileLoader& TextFileLoader)
{
	if (!TextFileLoader.SetChildNode("rouletteitem"))
	{
		sys_err("ReadRouletteTableFile : Cannot find `RouletteItem` node.");
		return false;
	}

	TTokenVector* pToken = NULL;
	for (const TMapperMap::value_type& it : m_MapperMap)
	{
		const BYTE bMappperIndex = it.first;
		const TMapper& Mappper = it.second;

		if (!TextFileLoader.SetChildNode(Mappper.strGroupName.c_str()))
		{
			sys_err("ReadRouletteTableFile : Cannot find `%s` node in group `RouletteItem`.", Mappper.strGroupName.c_str());
			return false;
		}

		for (BYTE bRowIndex = 1; bRowIndex <= ROULETTE_ITEM_MAX; ++bRowIndex)
		{
			if (!TextFileLoader.GetTokenVector(std::to_string(bRowIndex), &pToken))
				break;

			if (pToken == NULL || pToken->size() < 3)
			{
				sys_err("ReadRouletteTableFile : Invalid token vector at row %u in group `%s`",
					bRowIndex, Mappper.strGroupName.c_str());
				continue;
			}

			TItem Item;
			if (!str_to_number(Item.dwVnum, pToken->at(0).c_str()) ||
				!str_to_number(Item.bCount, pToken->at(1).c_str()) ||
				!str_to_number(Item.dwWeight, pToken->at(2).c_str()))
			{
				sys_err("ReadRouletteTableFile : Error parsing values at row %u in group `%s`",
					bRowIndex, Mappper.strGroupName.c_str());
				continue;
			}

			m_ItemMap[bMappperIndex].push_back(Item);
		}

		TextFileLoader.SetParentNode();
	}

	if (m_ItemMap.empty())
	{
		sys_err("ReadRouletteTableFile : Empty `RouletteItem`");
		TextFileLoader.SetParentNode();
		return false;
	}

	TextFileLoader.SetParentNode();
	return true;
}

BYTE CRouletteManager::GetRandomMapperIndex(BYTE bType) const
{
	TMapperMap MapperFilter;
	for (const TMapperMap::value_type& it : m_MapperMap)
	{
		const TMapper& Mapper = it.second;
		if (Mapper.bType == bType)
			MapperFilter.insert(std::make_pair(it.first, Mapper));
	}

	if (MapperFilter.empty())
	{
		sys_err("CRouletteManager::GetRandomMapper(bType=%u): Cannot find any Mappers!", bType);
		return 0;
	}

	return number(MapperFilter.begin()->first, MapperFilter.rbegin()->first);
}

const CRouletteManager::TMapper* CRouletteManager::GetMapper(BYTE bRowIndex) const
{
	TMapperMap::const_iterator it = m_MapperMap.find(bRowIndex);
	if (it != m_MapperMap.end())
		return &(it->second);

	return NULL;
}

BYTE CRouletteManager::GetRandomItemIndex(BYTE bMapperIndex) const
{
	TItemMap::const_iterator it = m_ItemMap.find(bMapperIndex);
	if (it == m_ItemMap.end() || it->second.empty())
		return ROULETTE_ITEM_MAX;

	const std::vector<TItem>& vItem = it->second;

	DWORD dwTotalWeight = 0;
	for (const TItem& Item : vItem)
		dwTotalWeight += Item.dwWeight;

	DWORD dwRandomWeight = number(1, dwTotalWeight);
	for (size_t i = 0; i < vItem.size(); ++i)
	{
		if (dwRandomWeight <= vItem[i].dwWeight)
			return static_cast<BYTE>(i);

		dwRandomWeight -= vItem[i].dwWeight;
	}

	return ROULETTE_ITEM_MAX;
}

const CRouletteManager::TItem* CRouletteManager::GetItem(BYTE bMapperIndex, BYTE bRowIndex) const
{
	TItemMap::const_iterator it = m_ItemMap.find(bMapperIndex);
	if (it != m_ItemMap.end())
	{
		const std::vector<TItem>& Item = it->second;
		if (bRowIndex < Item.size())
			return &(Item[bRowIndex]);
	}
	return NULL;
}

const CRouletteManager::TItemVec* CRouletteManager::GetItemVec(BYTE bMapperIndex) const
{
	TItemMap::const_iterator it = m_ItemMap.find(bMapperIndex);
	if (it != m_ItemMap.end())
		return &(it->second);
	return NULL;
}

////////////////////////////////////////////////////////////////////

CMiniGameRoulette::CMiniGameRoulette(LPCHARACTER pNPC, LPCHARACTER pChar, BYTE bType) :
	m_pNPC(pNPC), m_pChar(pChar), m_bType(bType), m_bIsSpinning(false), m_bSpinCount(0), m_pSpecialRouletteEvent(NULL)
{
	Initialize();
}

CMiniGameRoulette::~CMiniGameRoulette()
{
	Destroy();
}

/* static */ bool CMiniGameRoulette::IsActiveEvent()
{
	if (quest::CQuestManager::instance().GetEventFlag("e_late_summer"))
		return true;

	return false;
}

/* static */ bool CMiniGameRoulette::IsSpecialRoulette(LPCHARACTER pChar)
{
	if (pChar->GetQuestFlag(gs_szQuestFlag_SpecialExpireTime) > get_global_time())
		return true;

	return false;
}

/* static */ void CMiniGameRoulette::SpawnEventNPC(bool bSpawn)
{
	CharacterVectorInteractor vChar;
	if (CHARACTER_MANAGER::instance().GetCharactersByRaceNum(EVENT_NPC, vChar))
	{
		std::for_each(vChar.begin(), vChar.end(),
			[](LPCHARACTER pChar) { M2_DESTROY_CHARACTER(pChar); });
	}

	if (bSpawn)
	{
		struct pixel_pos { long xPos, yPos; int iDir; };
		std::unordered_map<long, pixel_pos> mSpawnPoint
		{
			{ MAP_A1, { 658, 534, 1 } },
			{ MAP_B1, { 602, 698, 4 } },
			{ MAP_C1, { 363, 702, 5 } },
			{ MAP_CAPEDRAGONHEAD, { 510, 1197, 1 } },
#if defined(__CONQUEROR_LEVEL__)
			{ MAP_EASTPLAIN_01, { 553, 672, 1 } },
#endif
		};

		for (const auto& it : mSpawnPoint)
		{
			long lMapIndex = it.first;
			const auto& pos = it.second;

			if (!map_allow_find(lMapIndex))
				continue;

			const LPSECTREE_MAP pSectreeMap = SECTREE_MANAGER::instance().GetMap(lMapIndex);
			if (pSectreeMap == NULL)
				continue;

			long xPos = pSectreeMap->m_setting.iBaseX + pos.xPos * 100;
			long yPos = pSectreeMap->m_setting.iBaseY + pos.yPos * 100;

			CHARACTER_MANAGER::Instance().SpawnMob(EVENT_NPC, lMapIndex,
				xPos, yPos, 0, false, pos.iDir == 0 ? -1 : (pos.iDir - 1) * 45);
		}
	}
}

struct SLateSummerEvent
{
	SLateSummerEvent(LPCHARACTER pChar) : m_pChar(pChar) {}
	bool UpdateAffect(LPAFFECT pAffect, int iMaxLimit)
	{
		if (pAffect == NULL)
			return false;

		if (pAffect->lApplyValue >= iMaxLimit - 1)
		{
			m_pChar->RemoveAffect(pAffect);
			m_pChar->SetQuestFlag(gs_szQuestFlag_SoulCount, m_pChar->GetQuestFlag(gs_szQuestFlag_SoulCount) + iMaxLimit);
			m_pChar->ChatPacket(CHAT_TYPE_INFO, LC_STRING("You have collected the maximum number of souls. Visit the Altar of Blood and carry out the ritual!"));
		}
		else
		{
			pAffect->bUpdate = true;
			++pAffect->lApplyValue;
			return true;
		}

		return false;
	}

	LPCHARACTER m_pChar;
};

/* static */ void CMiniGameRoulette::UpdateQuestFlag(LPCHARACTER pChar)
{
	SLateSummerEvent LateSummerEvent(pChar);

	LPAFFECT pAffect = pChar->FindAffect(AFFECT_LATE_SUMMER_EVENT_BUFF);
	if (!LateSummerEvent.UpdateAffect(pAffect, BUFF_SOUL_COUNT))
	{
		pAffect = pChar->FindAffect(AFFECT_LATE_SUMMER_EVENT_PRIMIUM_BUFF);
		LateSummerEvent.UpdateAffect(pAffect, PREMIUM_BUFF_SOUL_COUNT);
	}

	if (pAffect && pAffect->bUpdate)
		SendAffectAddPacket(pChar->GetDesc(), pAffect);

	if (pChar->GetQuestFlag(gs_szQuestFlag_SoulCount) < INT_MAX)
		pChar->ChatPacket(CHAT_TYPE_INFO, LC_STRING("You have captured a soul."));
}

/* static */ void CMiniGameRoulette::GetScoreTable(lua_State* L)
{
	DWORD dwIndex = 1;
	lua_newtable(L);

	char szQuery[2048 + 1];
	int iLen = 0;

	iLen += sprintf(szQuery + iLen, "SELECT `player`.`name`, `score` FROM `minigame_roulette%s` ", get_table_postfix());
	iLen += sprintf(szQuery + iLen,
		"LEFT JOIN (SELECT `id`, `name` AS `name` FROM `player%s`) AS `player` ON `player`.`id` = `minigame_roulette%s`.`pid` ",
		get_table_postfix(), get_table_postfix());
	iLen += sprintf(szQuery + iLen, "ORDER BY `score` DESC LIMIT 10");

	std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery(szQuery));
	const SQLResult* pRes = pMsg->Get();
	if (pRes && pRes->uiNumRows > 0)
	{
		MYSQL_ROW RowData;
		while ((RowData = mysql_fetch_row(pRes->pSQLResult)))
		{
			if (RowData[0][0] == '[') // GM
				continue;

			DWORD dwScore;
			str_to_number(dwScore, RowData[1]);

			if (dwScore == 0)
				continue;

			lua_newtable(L);

			// Name
			lua_pushstring(L, RowData[0]);
			lua_rawseti(L, -2, 1);

			// Score
			lua_pushnumber(L, dwScore);
			lua_rawseti(L, -2, 2);

			lua_rawseti(L, -2, dwIndex++);
		}
	}
}

/* static */ DWORD CMiniGameRoulette::GetMyScoreValue(lua_State* L, DWORD dwPID)
{
	char szQuery[2048 + 1];
	int iLen = 0;

	iLen += sprintf(szQuery + iLen,
		"SELECT `score` FROM `minigame_roulette%s` WHERE `pid` = %u ", get_table_postfix(), dwPID);

	std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery(szQuery));
	const SQLResult* pRes = pMsg->Get();
	if (pRes && pRes->uiNumRows > 0)
	{
		MYSQL_ROW RowData = mysql_fetch_row(pRes->pSQLResult);

		DWORD dwScore;
		str_to_number(dwScore, RowData[0]);

		return dwScore;
	}

	return 0;
}

/* static */ void CMiniGameRoulette::Open(LPCHARACTER pNPC, LPCHARACTER pChar, BYTE bType)
{
	if (pNPC == NULL)
	{
		sys_err("CMiniGameRoulette::Open(pNPC=%p, pChar=%p, bType=%d): NULL CHAR %s",
			get_pointer(pNPC), get_pointer(pChar), pChar->GetName());
		return;
	}

	int iDist = DISTANCE_APPROX(pChar->GetX() - pNPC->GetX(), pChar->GetY() - pNPC->GetY());
	if (iDist >= WINDOW_OPENER_MAX_DISTANCE)
		return;

	if (IsActiveEvent() == false)
		return;

	if (pChar->GetMiniGameRoulette())
		return;

	CMiniGameRoulette* pMiniGameRoulette = M2_NEW CMiniGameRoulette(pNPC, pChar, bType);
	if (pMiniGameRoulette == NULL)
		return;

	pMiniGameRoulette->SendPacket(ROULETTE_GC_OPEN);
	pChar->SetMiniGameRoulette(pMiniGameRoulette);
}

void CMiniGameRoulette::Initialize()
{
	const BYTE bMapperIndex = m_pChar->GetMiniGameRoulette_RewardMapperNum();

	m_bMapperIndex = bMapperIndex != 0 ? bMapperIndex : CRouletteManager::Instance().GetRandomMapperIndex(m_bType);
	m_pChar->SetMiniGameRoulette_RewardMapperNum(m_bMapperIndex);

	m_bItemIndex = ROULETTE_ITEM_MAX;

	const CRouletteManager::TMapper* pMapper = CRouletteManager::Instance().GetMapper(m_bMapperIndex);
	if (pMapper == NULL)
	{
		CloseError("CMiniGameRoulette::Initialize(): Failed to find TMapper for bMapperNum: %d", m_bMapperIndex);
		return;
	}

	m_iPrice = pMapper->iPrice;
	m_iSoulCount = pMapper->iSoulCount;

	if (m_bType == TYPE_SPECIAL)
	{
		int iSpecialRouletteTime = m_pChar->GetQuestFlag(gs_szQuestFlag_SpecialExpireTime);
		if (iSpecialRouletteTime > get_global_time())
		{
			MiniGameRouletteEventInfo* pSpecialRouletteEvent = AllocEventInfo<MiniGameRouletteEventInfo>();
			pSpecialRouletteEvent->pMiniGameRoulette = this;
			SetSpecialRouletteEvent(event_create(MiniGameSpecialRouletteEvent, pSpecialRouletteEvent, PASSES_PER_SEC(iSpecialRouletteTime - get_global_time())));
		}
	}
}

void CMiniGameRoulette::Destroy()
{
	if (m_bIsSpinning)
		Recover();

	m_bType = TYPE_NORMAL;
	m_bMapperIndex = 0;
	m_bItemIndex = ROULETTE_ITEM_MAX;
	m_iPrice = 0;
	m_iSoulCount = 0;

	m_pChar = NULL;
	m_pNPC = NULL;

	m_bIsSpinning = false;
	m_bSpinCount = 0;

	SetSpecialRouletteEvent(NULL);
}

void CMiniGameRoulette::Start()
{
	if (m_bIsSpinning)
		return;

	if (m_bType == TYPE_SPECIAL)
	{
		if (get_global_time() > m_pChar->GetQuestFlag(gs_szQuestFlag_SpecialExpireTime))
		{
			m_pChar->ChatPacket(CHAT_TYPE_INFO, LC_STRING("The Ritual of Blood is over. Try again another time."));
			Close(true); // NOTE : Force close window if the ritual of blood if over.
			return;
		}
	}

	// NOTE : Always check if there is any available position, all rewards are size 1.
	const int iEmptyPos = m_pChar->GetEmptyInventory(1);
	if (iEmptyPos == -1)
	{
		m_pChar->ChatPacket(CHAT_TYPE_INFO, LC_STRING("[Ritual of Blood] You don't have enough space in your inventory."));
		return;
	}

	if (m_pChar->GetGold() < m_iPrice)
	{
		m_pChar->ChatPacket(CHAT_TYPE_INFO, LC_STRING("[Ritual of Blood] You don't have enough Yang."));
		return;
	}

	int iSoulCount = m_pChar->GetQuestFlag(gs_szQuestFlag_SoulCount);
	int iUsedSoulCount = m_pChar->GetQuestFlag(gs_szQuestFlag_UsedSoulCount);

	if (iSoulCount < m_iSoulCount)
	{
		m_pChar->ChatPacket(CHAT_TYPE_INFO, LC_STRING("[Ritual of Blood] You don't have enough souls."));
		return;
	}

	m_bItemIndex = CRouletteManager::Instance().GetRandomItemIndex(m_bMapperIndex);
	if (m_bItemIndex == ROULETTE_ITEM_MAX)
	{
		CloseError("CMiniGameRoulette::Start: Failed to get item index for mapper index: %d", m_bMapperIndex);
		return;
	}

	m_pChar->PointChange(POINT_GOLD, -m_iPrice);
	m_pChar->SetQuestFlag(gs_szQuestFlag_SoulCount, iSoulCount - m_iSoulCount);
	m_pChar->SetQuestFlag(gs_szQuestFlag_UsedSoulCount, iUsedSoulCount + m_iSoulCount);

	m_bIsSpinning = true;
	m_bSpinCount += 1;

	SendPacket(ROULETTE_GC_START);
}

void CMiniGameRoulette::Request()
{
	SendPacket(ROULETTE_GC_REQUEST);
}

void CMiniGameRoulette::End()
{
	if (!m_bIsSpinning)
		return;

	int iUsedSoulCount = m_pChar->GetQuestFlag(gs_szQuestFlag_UsedSoulCount);
	if (iUsedSoulCount >= SOUL_MAX_NUM)
	{
		if (m_pNPC == m_pChar->GetQuestNPC())
		{
			TPacketGCSpecialEffect p;
			p.bHeader = HEADER_GC_SEPCIAL_EFFECT;
			p.bEffectNum = SE_SPECIAL_ROULETTE;
			p.dwVID = m_pNPC->GetVID();
			m_pChar->GetDesc()->Packet(&p, sizeof(p));
		}

		m_pChar->ChatPacket(CHAT_TYPE_INFO, LC_STRING("The Ritual of Blood has started. Buy special items to get better rewards."));
		m_pChar->SetQuestFlag(gs_szQuestFlag_SpecialExpireTime, get_global_time() + (test_server ? 60 : SPECIAL_ROULETTE_EXPIRE_TIME));
		m_pChar->SetQuestFlag(gs_szQuestFlag_UsedSoulCount, 0);
	}

	const CRouletteManager::TItem* pItemData = CRouletteManager::Instance().GetItem(m_bMapperIndex, m_bItemIndex);
	if (pItemData == NULL)
	{
		CloseError("CMiniGameRoulette::Start: Failed to get item index for mapper index: %d", m_bMapperIndex);
		return;
	}

	if (m_pChar->AutoGiveItem(pItemData->dwVnum, pItemData->bCount, -1, true
#if defined(__WJ_PICKUP_ITEM_EFFECT__)
		, true /* isHighLight */
#endif
#if defined(__NEW_USER_CARE__)
		, false /* bSystemDrop */
#endif
	) == NULL)
	{
#if defined(__MAILBOX__)
		CMailBox::SendGMMail(m_pChar->GetName(),
			LC_STRING("Reward for Ritual of Blood"),
			"",
			pItemData->dwVnum, pItemData->bCount);

		m_pChar->ChatPacket(CHAT_TYPE_INFO, LC_STRING("Insufficient space in your inventory. The reward will be sent to your mailbox."));
#else
		sys_log(0, "ROULETTE ITEM: Cannot find empty position for item %u (ch : %s)", pItemData->dwVnum, m_pChar->GetName());

		char szQuery[2048 + 1];
		snprintf(szQuery, sizeof(szQuery), "INSERT INTO `item_award%s` (`login`, `vnum`, `count`, `given_time`) VALUES('%s', %u, %u, NOW())",
			get_table_postfix(), m_pChar->GetDesc()->GetAccountTable().login, pItemData->dwVnum, pItemData->bCount);

		DBManager::instance().DirectQuery(szQuery);
#endif
	}

	m_bItemIndex = ROULETTE_ITEM_MAX;
	m_bIsSpinning = false;

	UpdateMyScore(m_pChar->GetPlayerID());
}

void CMiniGameRoulette::Close(bool bForce)
{
	if (bForce)
		SendPacket(ROULETTE_GC_CLOSE);

	if (m_pChar)
		m_pChar->SetMiniGameRoulette(NULL);
}

void CMiniGameRoulette::CloseError(const char* format, ...)
{
	char buf[255 + 1];
	va_list args;

	va_start(args, format);
	vsnprintf(buf, sizeof(buf), format, args);
	va_end(args);

	sys_err(buf);

	Close(true);
}

void CMiniGameRoulette::Recover()
{
	if (m_bIsSpinning == false)
		return;

	const CRouletteManager::TItem* pItemData = CRouletteManager::Instance().GetItem(m_bMapperIndex, m_bItemIndex);
	if (pItemData == NULL)
	{
		sys_err("CMiniGameRoulette::Recover : Item Data (%u) NULL (ch : %s)",
			m_bMapperIndex, m_pChar->GetName());
		return;
	}

#if defined(__MAILBOX__)
	CMailBox::SendGMMail(m_pChar->GetName(), LC_STRING("Reward for Ritual of Blood"), "", pItemData->dwVnum, pItemData->bCount);
#else
	char szQuery[2048 + 1];
	snprintf(szQuery, sizeof(szQuery), "INSERT INTO `item_award%s` (`login`, `vnum`, `count`, `given_time`) VALUES('%s', %u, %u, NOW())",
		get_table_postfix(), m_pChar->GetDesc()->GetAccountTable().login, pItemData->dwVnum, pItemData->bCount);

	DBManager::instance().DirectQuery(szQuery);
#endif
}

void CMiniGameRoulette::SendPacket(BYTE bSubHeader)
{
	if (m_pChar == NULL)
		return;

	LPDESC pDesc = m_pChar->GetDesc();
	if (pDesc == NULL)
		return;

	TPacketGCMiniGameRoulette MiniGameRoulette;
	MiniGameRoulette.bHeader = HEADER_GC_MINI_GAME_ROULETTE;
	MiniGameRoulette.bSubHeader = bSubHeader;
	MiniGameRoulette.bResult = m_bItemIndex;
	if (m_bType == TYPE_SPECIAL)
		MiniGameRoulette.dwExpireTime = m_pChar->GetQuestFlag(gs_szQuestFlag_SpecialExpireTime);
	else
		MiniGameRoulette.dwExpireTime = 0;

	switch (bSubHeader)
	{
		case ROULETTE_GC_OPEN:
		{
			const CRouletteManager::TItemVec* pItemVec = CRouletteManager::Instance().GetItemVec(m_bMapperIndex);
			if (pItemVec == NULL)
			{
				CloseError("CMiniGameRoulette::Initialize: pItemVec == NULL! (ch : %s)", m_pChar->GetName());
				return;
			}

			for (size_t i = 0; i < pItemVec->size(); i++)
			{
				MiniGameRoulette.ItemData[i].dwVnum = (*pItemVec)[i].dwVnum;
				MiniGameRoulette.ItemData[i].bCount = (*pItemVec)[i].bCount;
			}
		}
		break;

		case ROULETTE_GC_START:
		case ROULETTE_GC_REQUEST:
		case ROULETTE_GC_END:
		case ROULETTE_GC_CLOSE:
			break;

		default:
			sys_log(0, "CMiniGameRoulette::SendPacket(bSubHeader=%u): Unknown SubHeader! (ch : %s)",
				bSubHeader, m_pChar->GetName());
			break;
	}

	pDesc->Packet(&MiniGameRoulette, sizeof(MiniGameRoulette));
}

void CMiniGameRoulette::SetSpecialRouletteEvent(LPEVENT pEvent)
{
	if (m_pSpecialRouletteEvent != NULL)
		event_cancel(&m_pSpecialRouletteEvent);

	m_pSpecialRouletteEvent = pEvent;
}

void CMiniGameRoulette::UpdateMyScore(DWORD dwPID)
{
	char szQuery[2048 + 1];
	int iLen = sprintf(szQuery, "INSERT INTO `minigame_roulette%s` (`pid`, `score`, `last_play`) ", get_table_postfix());
	iLen += sprintf(szQuery + iLen, "VALUES (%u, %u, FROM_UNIXTIME(%d)) ", dwPID, m_iSoulCount, std::time(nullptr));
	iLen += sprintf(szQuery + iLen, "ON DUPLICATE KEY UPDATE `score` = `score` + %d", m_iSoulCount);
	DBManager::instance().DirectQuery(szQuery);
}
#endif // __SUMMER_EVENT_ROULETTE__
