#include "stdafx.h"
#include "config.h"
#include "xmas_event.h"
#include "desc.h"
#include "desc_manager.h"
#include "sectree_manager.h"
#include "char.h"
#include "char_manager.h"
#include "questmanager.h"

#if defined(__SNOWFLAKE_STICK_EVENT__)
#	include "item.h"
#	include "unique_item.h"
#	include "unique_mob.h"
#	include "config.h"
#	include "utils.h"
#endif

namespace xmas
{
#if defined(__XMAS_EVENT_2008__)
	void ProcessEventFlag(const std::string& name, int prev_value, int value)
	{
		if (name == "xmas_snow" || name == "xmas_boom" || name == "xmas_song" || name == "xmas_tree")
		{
			// 뿌려준다
			const DESC_MANAGER::DESC_SET& c_rDescSet = DESC_MANAGER::instance().GetClientSet();

			for (auto it = c_rDescSet.begin(); it != c_rDescSet.end(); ++it)
			{
				LPCHARACTER ch = (*it)->GetCharacter();
				if (ch)
					ch->ChatPacket(CHAT_TYPE_COMMAND, "%s %d", name.c_str(), value);
			}

			if (name == "xmas_boom")
			{
				if (value && !prev_value)
					SpawnEventHelper(true);
				else if (!value && prev_value)
					SpawnEventHelper(false);
			}
			else if (name == "xmas_tree")
			{
				if (value > 0 && prev_value == 0)
				{
					// 없으면 만들어준다
					CharacterVectorInteractor vec;
					if (!CHARACTER_MANAGER::instance().GetCharactersByRaceNum(MOB_XMAS_TREE_VNUM, vec))
						CHARACTER_MANAGER::instance().SpawnMob(MOB_XMAS_TREE_VNUM, MAP_N_SNOWM_01, 76500 + 358400, 60900 + 153600, 0, false, -1);
				}
				else if (prev_value > 0 && value == 0)
				{
					// 있으면 지워준다
					CharacterVectorInteractor vec;
					if (CHARACTER_MANAGER::instance().GetCharactersByRaceNum(MOB_XMAS_TREE_VNUM, vec))
					{
						CharacterVectorInteractor::iterator it = vec.begin();
						while (it != vec.end())
							M2_DESTROY_CHARACTER(*it++);
					}
				}
			}
		}
		else if (name == "xmas_santa")
		{
			switch (value)
			{
				case 0:
				{
					// 있으면 지우는 코드
					CharacterVectorInteractor vec;
					if (CHARACTER_MANAGER::instance().GetCharactersByRaceNum(MOB_XMAS_SANTA_VNUM, vec))
					{
						CharacterVectorInteractor::iterator it = vec.begin();
						while (it != vec.end())
							M2_DESTROY_CHARACTER(*it++);
					}
				}
				break;

				case 1:
				{
					// 내가 서한산이면 산타 없으면 만들고 상태를 2로 만든다.
					if (map_allow_find(61))
					{
						quest::CQuestManager::instance().RequestSetEventFlag("xmas_santa", 2);

						CharacterVectorInteractor vec;
						if (!CHARACTER_MANAGER::instance().GetCharactersByRaceNum(MOB_XMAS_SANTA_VNUM, vec))
							CHARACTER_MANAGER::instance().SpawnMobRandomPosition(MOB_XMAS_SANTA_VNUM, MAP_N_SNOWM_01);
					}
				}
				break;

				case 2:
					break;
			}
		}
	}

	EVENTINFO(spawn_santa_info)
	{
		long map_index;
		spawn_santa_info()
			: map_index(0)
		{
		}
	};

	EVENTFUNC(spawn_santa_event)
	{
		spawn_santa_info* info = dynamic_cast<spawn_santa_info*>(event->info);

		if (info == NULL)
		{
			sys_err("spawn_santa_event> <Factor> Null pointer");
			return 0;
		}

		long lMapIndex = info->map_index;

		if (quest::CQuestManager::instance().GetEventFlag("xmas_santa") == 0)
			return 0;

		//CharacterVectorInteractor vec;
		//if (CHARACTER_MANAGER::instance().GetCharactersByRaceNum(MOB_XMAS_SANTA_VNUM, vec))
		//	return 0;

		if (CHARACTER_MANAGER::instance().SpawnMobRandomPosition(xmas::MOB_XMAS_SANTA_VNUM, lMapIndex))
		{
			sys_log(0, "santa comes to town!");
			return 0;
		}

		return PASSES_PER_SEC(5);
	}

	void SpawnSanta(long lMapIndex, int iTimeGapSec)
	{
		if (test_server)
		{
			iTimeGapSec /= 60;
		}

		sys_log(0, "santa respawn time = %d", iTimeGapSec);
		spawn_santa_info* info = AllocEventInfo<spawn_santa_info>();

		info->map_index = lMapIndex;

		event_create(spawn_santa_event, info, PASSES_PER_SEC(iTimeGapSec));
	}

	void SpawnEventHelper(bool spawn)
	{
		if (spawn == true)
		{
			// 없으면 만들어준다
			static SNPCPosition positions[] = {
				{ 1, 615, 618 },
				{ 3, 500, 625 },
				{ 21, 598, 665 },
				{ 23, 476, 360 },
				{ 41, 318, 629 },
				{ 43, 478, 375 },
				{ 0, 0, 0 },
			};

			const SNPCPosition* p = positions;
			while (p->map_index)
			{
				if (map_allow_find(p->map_index))
				{
					PIXEL_POSITION posBase;
					if (!SECTREE_MANAGER::instance().GetMapBasePositionByMapIndex(p->map_index, posBase))
					{
						sys_err("cannot get map base position %d", p->map_index);
						p++;
						continue;
					}

					CHARACTER_MANAGER::instance().SpawnMob(MOB_XMAS_FIRWORK_SELLER_VNUM, p->map_index, posBase.x + p->x * 100, posBase.y + p->y * 100, 0, false, -1);
				}
				p++;
			}
		}
		else
		{
			// 있으면 지워준다
			CharacterVectorInteractor vec;
			if (CHARACTER_MANAGER::instance().GetCharactersByRaceNum(MOB_XMAS_FIRWORK_SELLER_VNUM, vec))
			{
				CharacterVectorInteractor::iterator it = vec.begin();
				while (it != vec.end())
					M2_DESTROY_CHARACTER(*it++);
			}
		}
	}
#endif

#if defined(__XMAS_EVENT_2012__)
	void SpawnNewSanta(const bool c_bSpawn)
	{
		if (c_bSpawn == true)
		{
			// 없으면 만들어준다
			static SNPCPosition positions[] = {
				{ 1, 608, 617 },
				{ 21, 596, 610 },
				{ 41, 357, 743 },
				{ 0, 0, 0 },
			};

			SNPCPosition* p = positions;
			while (p->map_index)
			{
				if (map_allow_find(p->map_index))
				{
					PIXEL_POSITION posBase;
					if (!SECTREE_MANAGER::instance().GetMapBasePositionByMapIndex(p->map_index, posBase))
					{
						sys_err("cannot get map base position %d", p->map_index);
						p++;
						continue;
					}

					CHARACTER_MANAGER::instance().SpawnMob(MOB_NEW_XMAS_SANTA_VNUM, p->map_index, posBase.x + p->x * 100, posBase.y + p->y * 100, 0, false, -1);
				}
				p++;
			}
		}
		else
		{
			// 있으면 지워준다
			CharacterVectorInteractor vec;
			if (CHARACTER_MANAGER::instance().GetCharactersByRaceNum(MOB_NEW_XMAS_SANTA_VNUM, vec))
			{
				CharacterVectorInteractor::iterator it = vec.begin();
				while (it != vec.end())
					M2_DESTROY_CHARACTER(*it++);
			}
		}
	}

	void SpawnNewXmasTree(const bool c_bSpawn)
	{
		if (c_bSpawn == true)
		{
			// 없으면 만들어준다
			static SNPCPosition positions[] = {
				{ 1, 625, 676 },
				{ 21, 542, 551 },
				{ 41, 442, 717 },
				{ 0, 0, 0 },
			};

			const SNPCPosition* p = positions;
			while (p->map_index)
			{
				if (map_allow_find(p->map_index))
				{
					PIXEL_POSITION pos;
					if (!SECTREE_MANAGER::instance().GetMapBasePositionByMapIndex(p->map_index, pos))
					{
						sys_err("cannot get map base position %d", p->map_index);
						p++;
						continue;
					}

					CHARACTER_MANAGER::instance().SpawnMob(MOB_NEW_XMAS_TREE_VNUM, p->map_index, pos.x + p->x * 100, pos.y + p->y * 100, 0, false, -1);
				}
				p++;
			}
		}
		else
		{
			// 있으면 지워준다
			CharacterVectorInteractor vec;
			if (CHARACTER_MANAGER::instance().GetCharactersByRaceNum(MOB_NEW_XMAS_TREE_VNUM, vec))
			{
				CharacterVectorInteractor::iterator it = vec.begin();
				while (it != vec.end())
					M2_DESTROY_CHARACTER(*it++);
			}
		}
	}
#endif
};

#if defined(__SNOWFLAKE_STICK_EVENT__)
/**
* Filename: xmas_event.cpp
* Author: Owsap
**/

std::string CSnowflakeStickEvent::m_str_QuestName = "snowflake_stick_event";
CSnowflakeStickEvent::TQuestDataMap CSnowflakeStickEvent::m_map_QuestData
{
	{ SNOWFLAKE_STICK_EVENT_QUEST_FLAG_SNOW_BALL, "snow_ball" },
	{ SNOWFLAKE_STICK_EVENT_QUEST_FLAG_TREE_BRANCH, "tree_branch" },
	{ SNOWFLAKE_STICK_EVENT_QUEST_FLAG_EXCHANGE_STICK, "exchange_stick" },
	{ SNOWFLAKE_STICK_EVENT_QUEST_FLAG_EXCHANGE_STICK_COOLDOWN, "exchange_stick_cooldown" },
	{ SNOWFLAKE_STICK_EVENT_QUEST_FLAG_EXCHANGE_PET, "exchange_pet" },
	{ SNOWFLAKE_STICK_EVENT_QUEST_FLAG_EXCHANGE_MOUNT, "exchange_mount" },
	{ SNOWFLAKE_STICK_EVENT_QUEST_FLAG_USE_STICK_COOLDOWN, "use_stick_cooldown" },
};

CSnowflakeStickEvent::CSnowflakeStickEvent() = default;
CSnowflakeStickEvent::~CSnowflakeStickEvent()
{
	m_map_QuestData.clear();
}

/* static */ DWORD CSnowflakeStickEvent::__GetQuestFlag(const LPCHARACTER& rChar, const BYTE bQuestFlagType)
{
	if (m_map_QuestData.empty() || bQuestFlagType >= SNOWFLAKE_STICK_EVENT_QUEST_FLAG_MAX_NUM)
		return 0;

	char szBuf[255] = { 0 };
	snprintf(szBuf, sizeof(szBuf), "%s.%s", m_str_QuestName.c_str(), m_map_QuestData[bQuestFlagType].c_str());
	szBuf[sizeof(szBuf) - 1] = '\0';

	return rChar->GetQuestFlag(szBuf);
}

/* static */ void CSnowflakeStickEvent::__SetQuestFlag(const LPCHARACTER& rChar, const BYTE bQuestFlagType, const INT iValue)
{
	if (m_map_QuestData.empty() || bQuestFlagType >= SNOWFLAKE_STICK_EVENT_QUEST_FLAG_MAX_NUM)
		return;

	char szBuf[255] = { 0 };
	snprintf(szBuf, sizeof(szBuf), "%s.%s", m_str_QuestName.c_str(), m_map_QuestData[bQuestFlagType].c_str());
	szBuf[sizeof(szBuf) - 1] = '\0';

	rChar->SetQuestFlag(szBuf, iValue);
}

/* static */ void CSnowflakeStickEvent::Reward(LPCHARACTER pChar)
{
	if (!test_server && quest::CQuestManager::instance().GetEventFlag("new_xmas_event") == 0)
		return;

	if (std::time(nullptr) > quest::CQuestManager::instance().GetEventFlag("snowflake_stick_event"))
		return;

	if (!pChar->FindAffect(AFFECT_SNOWFLAKE_STICK_EVENT_SNOWFLAKE_BUFF))
		return;

	if (!test_server && pChar->GetLevel() < SNOWFLAKE_STICK_EVENT_REFERENCE_LEVEL)
		return;

	BYTE bSnowBall = __GetQuestFlag(pChar, SNOWFLAKE_STICK_EVENT_QUEST_FLAG_SNOW_BALL);
	BYTE bTreeBranch = __GetQuestFlag(pChar, SNOWFLAKE_STICK_EVENT_QUEST_FLAG_TREE_BRANCH);

	// NOTE : 50% chance of rewarding a snowball or a tree branch.
	if (number(1, 100) <= 50)
	{
		if (bSnowBall < EXCHANGE_STICK_MAX_MATERIAL_COUNT)
		{
			__SetQuestFlag(pChar, SNOWFLAKE_STICK_EVENT_QUEST_FLAG_SNOW_BALL, bSnowBall + 1);
			Process(pChar, SNOWFLAKE_STICK_EVENT_GC_SUBHEADER_ADD_SNOW_BALL);
		}
		else
		{
			Process(pChar, SNOWFLAKE_STICK_EVENT_GC_SUBHEADER_MESSAGE_SNOW_BALL_MAX);

			// NOTE : If the maximum number of snowballs is reached, give a tree branch.
			if (bTreeBranch < EXCHANGE_STICK_MAX_MATERIAL_COUNT)
			{
				__SetQuestFlag(pChar, SNOWFLAKE_STICK_EVENT_QUEST_FLAG_TREE_BRANCH, bTreeBranch + 1);
				Process(pChar, SNOWFLAKE_STICK_EVENT_GC_SUBHEADER_ADD_TREE_BRANCH);
			}
		}
	}
	else
	{
		if (bTreeBranch < EXCHANGE_STICK_MAX_MATERIAL_COUNT)
		{
			__SetQuestFlag(pChar, SNOWFLAKE_STICK_EVENT_QUEST_FLAG_TREE_BRANCH, bTreeBranch + 1);
			Process(pChar, SNOWFLAKE_STICK_EVENT_GC_SUBHEADER_ADD_TREE_BRANCH);
		}
		else
		{
			Process(pChar, SNOWFLAKE_STICK_EVENT_GC_SUBHEADER_MESSAGE_TREE_BRANCH_MAX);

			// NOTE : If the maximum number of tree branches is reached, give a snowball.
			if (bSnowBall < EXCHANGE_STICK_MAX_MATERIAL_COUNT)
			{
				__SetQuestFlag(pChar, SNOWFLAKE_STICK_EVENT_QUEST_FLAG_SNOW_BALL, bSnowBall + 1);
				Process(pChar, SNOWFLAKE_STICK_EVENT_GC_SUBHEADER_ADD_SNOW_BALL);
			}
		}
	}
}

/* static */ void CSnowflakeStickEvent::Process(LPCHARACTER pChar, BYTE bSubHeader, INT iValue)
{
	if (pChar == nullptr)
	{
		sys_err("CSnowflakeStickEvent::Process(pChar=NULL, bSubHeader=%d): NULL CHAR", bSubHeader);
		return;
	}

	const LPDESC pDesc = pChar->GetDesc();
	if (pDesc == nullptr)
	{
		sys_err("CSnowflakeStickEvent::Process(pChar=%p(name=%s), bSubHeader=%d): NULL DESC",
			get_pointer(pChar), pChar->GetName(), bSubHeader);
		return;
	}

	const DWORD dwCurrentTime = std::time(nullptr);
	const DWORD dwExchangeStickCooldown = __GetQuestFlag(pChar, SNOWFLAKE_STICK_EVENT_QUEST_FLAG_EXCHANGE_STICK_COOLDOWN);
	if (dwCurrentTime > dwExchangeStickCooldown && dwExchangeStickCooldown != 0)
		Reset(pChar, true);
	else if (iValue == 0)
		Reset(pChar);

	TPacketGCSnowflakeStickEvent Packet(bSubHeader);
	if (bSubHeader == SNOWFLAKE_STICK_EVENT_GC_SUBHEADER_EVENT_INFO)
	{
		for (const TQuestDataMap::value_type& it : m_map_QuestData)
			Packet.dwValue[it.first] = __GetQuestFlag(pChar, it.first);
	}
	else if (bSubHeader == SNOWFLAKE_STICK_EVENT_GC_SUBHEADER_ENABLE)
	{
		pChar->ChatPacket(CHAT_TYPE_COMMAND, "snowflake_stick_event %d", iValue);
		Packet.dwValue[0] = static_cast<DWORD>(iValue);
	}
	else if (bSubHeader == SNOWFLAKE_STICK_EVENT_GC_SUBHEADER_MESSAGE_USE_STICK_FAILED && iValue > 0)
	{
		Packet.dwValue[0] = static_cast<DWORD>(iValue);
	}

	pDesc->Packet(&Packet, sizeof(Packet));
}

/* static */ void CSnowflakeStickEvent::Exchange(LPCHARACTER pChar, BYTE bSubHeader)
{
	const DWORD dwCurrentTime = std::time(nullptr);
	if (dwCurrentTime > quest::CQuestManager::instance().GetEventFlag("snowflake_stick_event"))
		return;

	if (!test_server && pChar->GetLevel() < SNOWFLAKE_STICK_EVENT_REFERENCE_LEVEL)
		return;

	const LPDESC pDesc = pChar->GetDesc();
	if (pDesc == nullptr)
	{
		sys_err("CSnowflakeStickEvent::Exchange(pChar=%p(name=%s), bSubHeader=%d): NULL DESC",
			get_pointer(pChar), pChar->GetName(), bSubHeader);
		return;
	}

	switch (bSubHeader)
	{
		case SNOWFLAKE_STICK_EVENT_CG_SUBHEADER_EXCHANGE_STICK:
		{
			const BYTE bSnowBall = __GetQuestFlag(pChar, SNOWFLAKE_STICK_EVENT_QUEST_FLAG_SNOW_BALL);
			const BYTE bTreeBranch = __GetQuestFlag(pChar, SNOWFLAKE_STICK_EVENT_QUEST_FLAG_TREE_BRANCH);
			if (bSnowBall < EXCHANGE_STICK_NEED_MATERIAL_COUNT || bTreeBranch < EXCHANGE_STICK_NEED_MATERIAL_COUNT)
				return;

			const DWORD dwExchangeStickCooldown = __GetQuestFlag(pChar, SNOWFLAKE_STICK_EVENT_QUEST_FLAG_EXCHANGE_STICK_COOLDOWN);
			if (dwCurrentTime > dwExchangeStickCooldown && dwExchangeStickCooldown != 0)
				Reset(pChar, true);
			else if(dwExchangeStickCooldown > dwCurrentTime)
				return;

			const BYTE bExchangeStick = __GetQuestFlag(pChar, SNOWFLAKE_STICK_EVENT_QUEST_FLAG_EXCHANGE_STICK);
			if (bExchangeStick >= EXCHANGE_STICK_COUNT_MAX)
				return;

			const LPITEM pRewardItem = ITEM_MANAGER::Instance().CreateItem(ITEM_VNUM_SNOWFLAKE_STICK);
			if (pRewardItem == nullptr)
			{
				sys_err("CSnowflakeStickEvent::Exchange(pChar=%p(name=%s), bSubHeader=%d): Cannot create item %u",
					get_pointer(pChar), pChar->GetName(), ITEM_VNUM_SNOWFLAKE_STICK);
				return;
			}

			const INT iEmptyPos = pChar->GetEmptyInventory(pRewardItem->GetSize());
			if (iEmptyPos == -1)
			{
				pChar->ChatPacket(CHAT_TYPE_INFO, LC_STRING("There isn't enough space in your inventory."));
				M2_DESTROY_ITEM(pRewardItem);
				return;
			}

			__SetQuestFlag(pChar, SNOWFLAKE_STICK_EVENT_QUEST_FLAG_SNOW_BALL, bSnowBall - EXCHANGE_STICK_NEED_MATERIAL_COUNT);
			__SetQuestFlag(pChar, SNOWFLAKE_STICK_EVENT_QUEST_FLAG_TREE_BRANCH, bTreeBranch - EXCHANGE_STICK_NEED_MATERIAL_COUNT);
			__SetQuestFlag(pChar, SNOWFLAKE_STICK_EVENT_QUEST_FLAG_EXCHANGE_STICK, bExchangeStick + 1);

			if (bExchangeStick == EXCHANGE_STICK_COUNT_MAX - 1)
			{
				if (test_server)
					__SetQuestFlag(pChar, SNOWFLAKE_STICK_EVENT_QUEST_FLAG_EXCHANGE_STICK_COOLDOWN, dwCurrentTime + 60);
				else
					__SetQuestFlag(pChar, SNOWFLAKE_STICK_EVENT_QUEST_FLAG_EXCHANGE_STICK_COOLDOWN, dwCurrentTime + EXCHANGE_STICK_COUNT_INIT_COOLTIME);
			}

#if defined(__WJ_PICKUP_ITEM_EFFECT__)
			pChar->AutoGiveItem(pRewardItem, true, true, true);
#else
			pChar->AutoGiveItem(pRewardItem, true, true);
#endif
			ITEM_MANAGER::Instance().FlushDelayedSave(pRewardItem);
		}
		break;

		case SNOWFLAKE_STICK_EVENT_CG_SUBHEADER_EXCHANGE_PET:
		{
			const BYTE bExchangePet = __GetQuestFlag(pChar, SNOWFLAKE_STICK_EVENT_QUEST_FLAG_EXCHANGE_PET);

			if (bExchangePet >= EXCHANGE_PET_COUNT_MAX)
				return;

			if (pChar->CountSpecifyItem(ITEM_VNUM_SNOWFLAKE_STICK) < EXCHANGE_PET_NEED_MATERIAL_COUNT)
				return;

			const LPITEM pRewardItem = ITEM_MANAGER::Instance().CreateItem(ITEM_VNUM_SNOWFLAKE_STICK_EVENT_PET);
			if (pRewardItem == nullptr)
			{
				sys_err("CSnowflakeStickEvent::Exchange(pChar=%p(name=%s), bSubHeader=%d): Cannot create item %u",
					get_pointer(pChar), pChar->GetName(), ITEM_VNUM_SNOWFLAKE_STICK_EVENT_PET);
				return;
			}

			const INT iEmptyPos = pChar->GetEmptyInventory(pRewardItem->GetSize());
			if (iEmptyPos == -1)
			{
				pChar->ChatPacket(CHAT_TYPE_INFO, LC_STRING("There isn't enough space in your inventory."));
				M2_DESTROY_ITEM(pRewardItem);
				return;
			}

			__SetQuestFlag(pChar, SNOWFLAKE_STICK_EVENT_QUEST_FLAG_EXCHANGE_PET, bExchangePet + 1);

			pChar->RemoveSpecifyItem(ITEM_VNUM_SNOWFLAKE_STICK, EXCHANGE_PET_NEED_MATERIAL_COUNT);
#if defined(__WJ_PICKUP_ITEM_EFFECT__)
			pChar->AutoGiveItem(pRewardItem, true, true, true);
#else
			pChar->AutoGiveItem(pRewardItem, true, true);
#endif
			ITEM_MANAGER::Instance().FlushDelayedSave(pRewardItem);
		}
		break;

		case SNOWFLAKE_STICK_EVENT_CG_SUBHEADER_EXCHANGE_MOUNT:
		{
			const BYTE bExchangeMount = __GetQuestFlag(pChar, SNOWFLAKE_STICK_EVENT_QUEST_FLAG_EXCHANGE_MOUNT);
			if (bExchangeMount >= EXCHANGE_MOUNT_COUNT_MAX)
				return;

			if (pChar->CountSpecifyItem(ITEM_VNUM_SNOWFLAKE_STICK) < EXCHANGE_MOUNT_NEED_MATERIAL_COUNT)
				return;

			const LPITEM pRewardItem = ITEM_MANAGER::Instance().CreateItem(ITEM_VNUM_SNOWFLAKE_STICK_EVENT_MOUNT);
			if (pRewardItem == nullptr)
			{
				sys_err("CSnowflakeStickEvent::Exchange(pChar=%p(name=%s), bSubHeader=%d): Cannot create item %u",
					get_pointer(pChar), pChar->GetName(), ITEM_VNUM_SNOWFLAKE_STICK_EVENT_MOUNT);
				return;
			}

			const INT iEmptyPos = pChar->GetEmptyInventory(pRewardItem->GetSize());
			if (iEmptyPos == -1)
			{
				pChar->ChatPacket(CHAT_TYPE_INFO, LC_STRING("There isn't enough space in your inventory."));
				M2_DESTROY_ITEM(pRewardItem);
				return;
			}

			__SetQuestFlag(pChar, SNOWFLAKE_STICK_EVENT_QUEST_FLAG_EXCHANGE_MOUNT, bExchangeMount + 1);

			pChar->RemoveSpecifyItem(ITEM_VNUM_SNOWFLAKE_STICK, EXCHANGE_MOUNT_NEED_MATERIAL_COUNT);
#if defined(__WJ_PICKUP_ITEM_EFFECT__)
			pChar->AutoGiveItem(pRewardItem, true, true, true);
#else
			pChar->AutoGiveItem(pRewardItem, true, true);
#endif
			ITEM_MANAGER::Instance().FlushDelayedSave(pRewardItem);

		}
		break;

		default:
			sys_err("CSnowflakeStickEvent::Exchange(pChar=%p(name=%s), bSubHeader=%d): Unknown subheader",
				get_pointer(pChar), pChar->GetName(), bSubHeader);
			break;
	}

	Process(pChar, SNOWFLAKE_STICK_EVENT_GC_SUBHEADER_EVENT_INFO);
}

/* static */ void CSnowflakeStickEvent::Reset(LPCHARACTER pChar, bool bExchangeCooldown)
{
	if (bExchangeCooldown)
	{
		__SetQuestFlag(pChar, SNOWFLAKE_STICK_EVENT_QUEST_FLAG_EXCHANGE_STICK, 0);
		__SetQuestFlag(pChar, SNOWFLAKE_STICK_EVENT_QUEST_FLAG_EXCHANGE_STICK_COOLDOWN, 0);
	}
	else
	{
		for (const TQuestDataMap::value_type& it : m_map_QuestData)
			__SetQuestFlag(pChar, it.first, 0);

		pChar->RemoveAffect(AFFECT_SNOWFLAKE_STICK_EVENT_RANK_BUFF);
		pChar->RemoveAffect(AFFECT_SNOWFLAKE_STICK_EVENT_SNOWFLAKE_BUFF);
	}
}

/* static */ void CSnowflakeStickEvent::EnterGame(LPCHARACTER pChar)
{
	INT iEventFlag = quest::CQuestManager::instance().GetEventFlag("snowflake_stick_event");
	if (std::time(nullptr) > iEventFlag)
	{
		if (pChar->FindAffect(AFFECT_SNOWFLAKE_STICK_EVENT_SNOWFLAKE_BUFF))
			pChar->RemoveAffect(AFFECT_SNOWFLAKE_STICK_EVENT_SNOWFLAKE_BUFF);
	}
	else
		Process(pChar, SNOWFLAKE_STICK_EVENT_GC_SUBHEADER_ENABLE, iEventFlag);
}

struct FuncSwapRace
{
	LPCHARACTER m_pChar;
	BYTE m_bSwapCount;
	FuncSwapRace(LPCHARACTER pChar) :
		m_pChar(pChar), m_bSwapCount(0)
	{}

	void operator()(LPENTITY pEntity)
	{
		if (m_bSwapCount >= CSnowflakeStickEvent::SNOWFLAKE_STICK_POLY_LIMIT)
			return;

		if (pEntity->IsType(ENTITY_CHARACTER))
		{
			const LPCHARACTER pTarget = dynamic_cast<LPCHARACTER>(pEntity);
			if (pTarget == nullptr)
				return;

			if (!pTarget->IsMonster())
				return;

			if (pTarget->GetMobRank() >= MOB_RANK_BOSS)
				return;

#if defined(__RACE_SWAP__)
			if (pTarget->GetEventRaceNum() == EUniqueMob::MOB_XMAS_SNOWMAN)
				return;
#else
			if (pTarget->GetRaceNum() == EUniqueMob::MOB_XMAS_SNOWMAN)
				return;
#endif

			float fDist = DISTANCE_APPROX(m_pChar->GetX() - pTarget->GetX(), m_pChar->GetY() - pTarget->GetY());
			if (fDist > static_cast<float>(CSnowflakeStickEvent::SNOWFLAKE_STICK_POLY_DISTANCE))
				return;

			pTarget->SetAggressive(false);
			pTarget->SetVictim(NULL);
			pTarget->SetNoMove();
			pTarget->Stop();

			// NOTE : We don't want to transform (poly) the monster completely into another
			// one as this will change its level and status, the solution to this is only
			// change the model as how the metin stone swap system works.
#if defined(__RACE_SWAP__)
			pTarget->SetEventRaceNum(EUniqueMob::MOB_XMAS_SNOWMAN);
#else
			pTarget->SetPolymorph(EUniqueMob::MOB_XMAS_SNOWMAN, true);
#endif

			m_bSwapCount++;
		}
	}

	int GetTotalSwapLimit() const
	{
		return m_bSwapCount;
	}
};

/* static */ bool CSnowflakeStickEvent::UseStick(LPCHARACTER pChar)
{
	const DWORD dwCurrentTime = std::time(nullptr);
	if (dwCurrentTime > quest::CQuestManager::instance().GetEventFlag("snowflake_stick_event"))
		return false;

	const DWORD dwUseStickCooldown = __GetQuestFlag(pChar, SNOWFLAKE_STICK_EVENT_QUEST_FLAG_USE_STICK_COOLDOWN);
	if (dwUseStickCooldown > dwCurrentTime)
	{
		CSnowflakeStickEvent::Process(pChar, SNOWFLAKE_STICK_EVENT_GC_SUBHEADER_MESSAGE_USE_STICK_FAILED, dwUseStickCooldown);
		return false;
	}

	LPSECTREE pSec = pChar->GetSectree();
	if (pSec == nullptr)
	{
		sys_err("CSnowflakeStickEvent::UseStick(pChar=%p(name=%s)): Cannot get sectree",
			get_pointer(pChar), pChar->GetName());
		return false;
	}

	FuncSwapRace f(pChar);
	pSec->ForEachAround(f);

	if (test_server)
		__SetQuestFlag(pChar, SNOWFLAKE_STICK_EVENT_QUEST_FLAG_USE_STICK_COOLDOWN, dwCurrentTime + 60);
	else
		__SetQuestFlag(pChar, SNOWFLAKE_STICK_EVENT_QUEST_FLAG_USE_STICK_COOLDOWN, dwCurrentTime + SNOWFLAKE_STICK_USE_COOLTIME);

	if (f.GetTotalSwapLimit() == 0)
		CSnowflakeStickEvent::Process(pChar, SNOWFLAKE_STICK_EVENT_GC_SUBHEADER_MESSAGE_USE_STICK_FAILED);

	return true;
}
#endif
