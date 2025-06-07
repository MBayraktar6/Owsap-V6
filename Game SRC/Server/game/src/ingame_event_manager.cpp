/**
* Filename: ingame_event_manager.cpp
* Author: Owsap
**/

#include "stdafx.h"

#if defined(__INGAME_EVENT_MANAGER__)
#include "ingame_event_manager.h"

#include "char.h"
#include "desc.h"
#include "desc_manager.h"

#include "questmanager.h"
#include "xmas_event.h"

struct SInGameEventProcessor
{
	SInGameEventProcessor(InGameEventProcessFunc pFunc, const std::string& rkEventName, int iValue) :
		m_pFunc(pFunc), m_stEventName(rkEventName), m_iValue(iValue) {}

	void operator () (LPDESC pDesc) const
	{
		if (NULL == pDesc)
			return;

		const LPCHARACTER pChar = pDesc->GetCharacter();
		if (NULL == pChar)
			return;

		m_pFunc(pChar, m_stEventName, m_iValue);
	}

	InGameEventProcessFunc m_pFunc;
	std::string m_stEventName;
	int m_iValue;
};

void InGameEventProcessor::Process(const std::string& rkEventName, int iValue, InGameEventProcessFunc pFunc)
{
	const DESC_MANAGER::DESC_SET& it = DESC_MANAGER::instance().GetClientSet();
	std::for_each(it.begin(), it.end(), SInGameEventProcessor(pFunc, rkEventName, iValue));
}

void CommandEventProcess(LPCHARACTER pChar, const std::string& rkEventName, int iValue)
{
	pChar->ChatPacket(CHAT_TYPE_COMMAND, "%s %d", rkEventName.c_str(), iValue);
}

#if defined(__SNOWFLAKE_STICK_EVENT__)
void SnowflakeStickEventProcess(LPCHARACTER pChar, const std::string& rkEventName, int iValue)
{
	CSnowflakeStickEvent::Process(pChar, SNOWFLAKE_STICK_EVENT_GC_SUBHEADER_ENABLE, iValue);
}
#endif

CInGameEventManager::CInGameEventManager()
{
	m_map_InGameEventName["mystery_box_drop"] = INGAME_EVENT_TYPE_MYSTERY_BOX1;
	m_map_InGameEventName["mystery_box_drop_2"] = INGAME_EVENT_TYPE_MYSTERY_BOX2;
	//m_map_InGameEventName["e_buff_spawn"] = INGAME_EVENT_TYPE_LUCKY_EVENT;
	//m_map_InGameEventName["attendance"] = INGAME_EVENT_TYPE_ATTENDANCE;
#if defined(__MINI_GAME_RUMI__)
	m_map_InGameEventName["mini_game_okey"] = INGAME_EVENT_TYPE_OKEY;
	m_map_InGameEventName["mini_game_okey_normal"] = INGAME_EVENT_TYPE_OKEY_NORMAL;
#endif
#if defined(__XMAS_EVENT_2012__)
	m_map_InGameEventName["new_xmas_event"] = INGAME_EVENT_TYPE_NEW_XMAS_EVENT;
#endif
#if defined(__MINI_GAME_YUTNORI__)
	m_map_InGameEventName["mini_game_yutnori"] = INGAME_EVENT_TYPE_YUTNORI;
#endif
	//m_map_InGameEventName["e_monsterback"] = INGAME_EVENT_TYPE_MONSTERBACK_EVENT;
	//m_map_InGameEventName["e_monsterback_10th"] = INGAME_EVENT_TYPE_MONSTERBACK_10TH_EVENT;
#if defined(__EASTER_EVENT__)
	m_map_InGameEventName["easter_drop"] = INGAME_EVENT_TYPE_EASTER_EVENT;
#endif
	m_map_InGameEventName["e_summer_event"] = INGAME_EVENT_TYPE_ICECREAM_EVENT;
	m_map_InGameEventName["ramadan_drop"] = INGAME_EVENT_TYPE_RAMADAN_EVENT;
#if defined(__HALLOWEEN_EVENT_2014__)
	m_map_InGameEventName["halloween_box"] = INGAME_EVENT_TYPE_HALLOWEEN_EVENT;
#endif
	m_map_InGameEventName["football_drop"] = INGAME_EVENT_TYPE_FOOTBALL_EVENT;
	m_map_InGameEventName["medal_part_drop"] = INGAME_EVENT_TYPE_OLYMPIC_EVENT;
#if defined(__2016_VALENTINE_EVENT__)
	m_map_InGameEventName["valentine_drop"] = INGAME_EVENT_TYPE_VALENTINE_DAY_EVENT;
#endif
	//m_map_InGameEventName["fish_event"] = INGAME_EVENT_TYPE_FISH;
#if defined(__FLOWER_EVENT__)
	m_map_InGameEventName["e_flower_drop"] = INGAME_EVENT_TYPE_FLOWER_EVENT;
#endif
#if defined(__MINI_GAME_CATCH_KING__)
	m_map_InGameEventName["mini_game_catchking"] = INGAME_EVENT_TYPE_CATCHKING;
#endif
	//m_map_InGameEventName["md_start"] = INGAME_EVENT_TYPE_MINIBOSS;
#if defined(__SUMMER_EVENT_ROULETTE__)
	m_map_InGameEventName["e_late_summer"] = INGAME_EVENT_TYPE_ROULETTE;
#endif
	//m_map_InGameEventName["mini_game_bnw"] = INGAME_EVENT_TYPE_BLACK_AND_WHITE;
	//m_map_InGameEventName["world_boss"] = INGAME_EVENT_TYPE_WORLD_BOSS;
	//m_map_InGameEventName["battle_royale"] = INGAME_EVENT_TYPE_BATTLE_ROYALE;
	//m_map_InGameEventName["metinstone_rain_event"] = INGAME_EVENT_TYPE_METINSTONE_RAIN_EVENT;
	//m_map_InGameEventName["e_otherworld_drop_1"] = INGAME_EVENT_TYPE_OTHER_WORLD;
	//m_map_InGameEventName["e_otherworld_drop_2"] = INGAME_EVENT_TYPE_OTHER_WORLD_EVE;
	//m_map_InGameEventName["golden_land"] = INGAME_EVENT_TYPE_GOLDEN_LAND_EVENT;
	//m_map_InGameEventName["sports_match"] = INGAME_EVENT_TYPE_SPORTS_MATCH_EVENT;
#if defined(__SNOWFLAKE_STICK_EVENT__)
	m_map_InGameEventName["snowflake_stick_event"] = INGAME_EVENT_TYPE_SNOWFLAKE_STICK_EVENT;
#endif
	//m_map_InGameEventName["treasure_hunt"] = INGAME_EVENT_TYPE_TREASURE_HUNT;

#if defined(__EVENT_BANNER_FLAG__)
	m_bIsLoadedBanners = false;
#endif
}

CInGameEventManager::~CInGameEventManager()
{
	Destroy();
}

void CInGameEventManager::Destroy()
{
	m_map_InGameEventName.clear();
#if defined(__EVENT_BANNER_FLAG__)
	m_bIsLoadedBanners = false;
	m_map_BannerType.clear();
#endif
}

BYTE CInGameEventManager::GetInGameEventType(const std::string strEventName) const
{
	TInGameEventNameMap::const_iterator it = m_map_InGameEventName.find(strEventName);
	return (it != m_map_InGameEventName.end() ? it->second : INGAME_EVENT_TYPE_NONE);
}

void CInGameEventManager::SetInGameEvent(const std::string& stEventName, int iValue, int iPrevValue)
{
	const BYTE bInGameEventType = GetInGameEventType(stEventName);
	if (bInGameEventType == INGAME_EVENT_TYPE_NONE)
		return;

	if (iValue == iPrevValue)
		return;

	switch (bInGameEventType)
	{
#if defined(__XMAS_EVENT_2012__)
		case INGAME_EVENT_TYPE_NEW_XMAS_EVENT:
		{
			xmas::SpawnNewSanta(iValue > 0 ? true : false);
			xmas::SpawnNewXmasTree(iValue > 0 ? true : false);

			InGameEventProcessor::Process(stEventName, iValue, CommandEventProcess);
		}
		break;
#endif

#if defined(__MINI_GAME_RUMI__)
		case INGAME_EVENT_TYPE_OKEY:
		case INGAME_EVENT_TYPE_OKEY_NORMAL:
		{
			// Only display the event NPC if the event is enabled "mini_game_okey_reward", and
			// remove the event NPC when "mini_game_okey_reward_reward" ends @ UpdateInGameEvent
			if (iValue > 0) CMiniGameRumi::SpawnEventNPC(true);
			InGameEventProcessor::Process(stEventName, iValue, CommandEventProcess);
		}
		break;
#endif
		
#if defined(__MINI_GAME_YUTNORI__)
		case INGAME_EVENT_TYPE_YUTNORI:
		{
			// Only display the event NPC if the event is enabled "mini_game_yutnori", and
			// remove the event NPC when "mini_game_yutnori_reward" ends @ UpdateInGameEvent
			if (iValue > 0) CMiniGameYutnori::SpawnEventNPC(true);
			InGameEventProcessor::Process(stEventName, iValue, CommandEventProcess);
		}
		break;
#endif

#if defined(__FLOWER_EVENT__)
		case INGAME_EVENT_TYPE_FLOWER_EVENT:
			InGameEventProcessor::Process(stEventName, iValue, CommandEventProcess);
			break;
#endif

#if defined(__MINI_GAME_CATCH_KING__)
		case INGAME_EVENT_TYPE_CATCHKING:
		{
			CMiniGameCatchKing::SpawnEventNPC(iValue > 0 ? true : false);
			InGameEventProcessor::Process(stEventName, iValue, CommandEventProcess);
		}
		break;
#endif

#if defined(__SUMMER_EVENT_ROULETTE__)
		case INGAME_EVENT_TYPE_ROULETTE:
		{
			CMiniGameRoulette::SpawnEventNPC(iValue > 0 ? true : false);
			InGameEventProcessor::Process(stEventName, iValue, CommandEventProcess);
		}
		break;
#endif

#if defined(__SNOWFLAKE_STICK_EVENT__)
		case INGAME_EVENT_TYPE_SNOWFLAKE_STICK_EVENT:
			InGameEventProcessor::Process(stEventName, iValue, SnowflakeStickEventProcess);
			break;
#endif

		default:
			InGameEventProcessor::Process(stEventName, iValue, CommandEventProcess);
			break;
	}
}

void CInGameEventManager::UpdateInGameEvent()
{
#if defined(__MINI_GAME_RUMI__)
	int iMiniGameOkey = quest::CQuestManager::instance().GetEventFlag("mini_game_okey");
	int iMiniGameOkeyNormal = quest::CQuestManager::instance().GetEventFlag("mini_game_okey_normal");
	int iMiniGameOkeyReward = quest::CQuestManager::instance().GetEventFlag("mini_game_okey_reward");

	if (std::time(nullptr) > iMiniGameOkey && iMiniGameOkey != 0)
	{
		quest::CQuestManager::instance().RequestSetEventFlag("mini_game_okey", 0);
		quest::CQuestManager::instance().RequestSetEventFlag("mini_game_okey_reward",
			std::time(nullptr) + CMiniGameRumi::RUMI_REWARD_COOLDOWN);

		SendNotice(LC_STRING("[Okey Event] The Okey Event is over!"));
		SendNotice(LC_STRING("[Okey Event] If you make it into the top rankings, you have 7 days to collect your prize."));
	}

	if (std::time(nullptr) > iMiniGameOkeyNormal && iMiniGameOkeyNormal != 0)
	{
		quest::CQuestManager::instance().RequestSetEventFlag("mini_game_okey_normal", 0);
		quest::CQuestManager::instance().RequestSetEventFlag("mini_game_okey_reward",
			std::time(nullptr) + CMiniGameRumi::RUMI_REWARD_COOLDOWN);

		SendNotice(LC_STRING("[Okey Event] The Okey Event is over!"));
		SendNotice(LC_STRING("[Okey Event] If you make it into the top rankings, you have 7 days to collect your prize."));
	}

	if (std::time(nullptr) > iMiniGameOkeyReward && iMiniGameOkeyReward != 0)
	{
		CMiniGameRumi::SpawnEventNPC(false);
		quest::CQuestManager::instance().RequestSetEventFlag("mini_game_okey_reward", 0);
	}
#endif

#if defined(__MINI_GAME_YUTNORI__)
	int iMiniGameYutnori = quest::CQuestManager::instance().GetEventFlag("mini_game_yutnori");
	int iMiniGameYutnoriReward = quest::CQuestManager::instance().GetEventFlag("mini_game_yutnori_reward");

	if (std::time(nullptr) > iMiniGameYutnori && iMiniGameYutnori != 0)
	{
		quest::CQuestManager::instance().RequestSetEventFlag("mini_game_yutnori", 0);
		quest::CQuestManager::instance().RequestSetEventFlag("mini_game_yutnori_reward",
			std::time(nullptr) + CMiniGameYutnori::YUTNORI_REWARD_COOLDOWN);

		SendNotice(LC_STRING("Yut Nori has finished."));
		SendNotice(LC_STRING("Go to the Yut Nori Table to collect your reward."));
	}

	if (std::time(nullptr) > iMiniGameYutnoriReward && iMiniGameYutnoriReward != 0)
	{
		CMiniGameYutnori::SpawnEventNPC(false);
		quest::CQuestManager::instance().RequestSetEventFlag("mini_game_yutnori_reward", 0);
	}
#endif

#if defined(__MINI_GAME_CATCH_KING__)
	int iMiniGameCatchKing = quest::CQuestManager::instance().GetEventFlag("mini_game_catchking");
	if (std::time(nullptr) > iMiniGameCatchKing && iMiniGameCatchKing != 0)
	{
		quest::CQuestManager::instance().RequestSetEventFlag("mini_game_catchking", 0);
		SendNotice(LC_STRING("[Catch the King] Catch the King has ended."));
	}
#endif

#if defined(__SUMMER_EVENT_ROULETTE__)
	int iMiniGameRoulette = quest::CQuestManager::instance().GetEventFlag("e_late_summer");
	if (std::time(nullptr) > iMiniGameRoulette && iMiniGameRoulette != 0)
		quest::CQuestManager::instance().RequestSetEventFlag("e_late_summer", 0);
#endif

#if defined(__SNOWFLAKE_STICK_EVENT__)
	int iSnowflakeStickEvent = quest::CQuestManager::instance().GetEventFlag("snowflake_stick_event");
	if (std::time(nullptr) > iSnowflakeStickEvent && iSnowflakeStickEvent != 0)
		quest::CQuestManager::instance().RequestSetEventFlag("snowflake_stick_event", 0);
#endif
}

void CInGameEventManager::BroadcastInGameEventOnLogin(LPCHARACTER pChar)
{
	for (const TInGameEventNameMap::value_type& it : m_map_InGameEventName)
	{
		const std::string& stEventFlagName = it.first;
		int iEventFlagValue = quest::CQuestManager::instance().GetEventFlag(stEventFlagName.c_str());
		if (iEventFlagValue != 0)
			pChar->ChatPacket(CHAT_TYPE_COMMAND, "%s %d", stEventFlagName.c_str(), iEventFlagValue);
	}
}

#if defined(__EVENT_BANNER_FLAG__)
#include "CsvReader.h"
#include "char_manager.h"
#include "config.h"
#include "regen.h"

bool CInGameEventManager::InitializeBanners()
{
	if (m_bIsLoadedBanners)
		return false;

	const char* c_szFileName = "data/banner/list.txt";

	cCsvTable nameData;
	if (!nameData.Load(c_szFileName, '\t'))
	{
		sys_log(0, "%s couldn't be loaded or its format is incorrect.", c_szFileName);
		return false;
	}
	else
	{
		nameData.Next();
		while (nameData.Next())
		{
			if (nameData.ColCount() < 2)
				continue;

			m_map_BannerType.emplace(std::make_pair(atoi(nameData.AsStringByIndex(0)), nameData.AsStringByIndex(1)));
		}
	}
	nameData.Destroy();

	m_bIsLoadedBanners = true;

	DWORD dwFlagVnum = quest::CQuestManager::instance().GetEventFlag("banner");
	if (dwFlagVnum > 0)
		SpawnBanners(dwFlagVnum);

	return true;
}

bool CInGameEventManager::SpawnBanners(int iEnable, const char* c_szBannerName)
{
	if (!m_bIsLoadedBanners)
		InitializeBanners();

	bool bDestroy = true;
	bool bSpawn = false;

	DWORD dwBannerVnum = 0;
	std::string strBannerName;

	if (!c_szBannerName)
	{
		BannerMapType::const_iterator it = m_map_BannerType.find(iEnable);
		if (it == m_map_BannerType.end())
			return false;

		dwBannerVnum = it->first;
		strBannerName = it->second;
	}
	else
	{
		for (BannerMapType::const_iterator it = m_map_BannerType.begin(); m_map_BannerType.end() != it; ++it)
		{
			if (!strcmp(it->second.c_str(), c_szBannerName))
			{
				dwBannerVnum = it->first;
				strBannerName = it->second;
				break;
			}
		}
	}

	if (dwBannerVnum == 0 || strBannerName.empty())
		return false;

	if (iEnable > 0)
		bSpawn = true;

	if (bDestroy)
	{
		quest::CQuestManager::instance().RequestSetEventFlag("banner", 0);

		CharacterVectorInteractor i;
		CHARACTER_MANAGER::instance().GetCharactersByRaceNum(dwBannerVnum, i);

		for (CharacterVectorInteractor::iterator it = i.begin(); it != i.end(); it++)
		{
			M2_DESTROY_CHARACTER(*it);
		}
	}

	if (bSpawn)
	{
		quest::CQuestManager::instance().RequestSetEventFlag("banner", dwBannerVnum);

		if (map_allow_find(MAP_A1))
		{
			std::string strBannerFile = "data/banner/a/" + strBannerName + ".txt";
			if (LPSECTREE_MAP pkMap = SECTREE_MANAGER::instance().GetMap(MAP_A1))
				regen_do(strBannerFile.c_str(), MAP_A1, pkMap->m_setting.iBaseX, pkMap->m_setting.iBaseY, NULL, false);
		}

		if (map_allow_find(MAP_B1))
		{
			std::string strBannerFile = "data/banner/b/" + strBannerName + ".txt";
			if (LPSECTREE_MAP pkMap = SECTREE_MANAGER::instance().GetMap(MAP_B1))
				regen_do(strBannerFile.c_str(), MAP_B1, pkMap->m_setting.iBaseX, pkMap->m_setting.iBaseY, NULL, false);
		}

		if (map_allow_find(MAP_C1))
		{
			std::string strBannerFile = "data/banner/c/" + strBannerName + ".txt";
			if (LPSECTREE_MAP pkMap = SECTREE_MANAGER::instance().GetMap(MAP_C1))
				regen_do(strBannerFile.c_str(), MAP_C1, pkMap->m_setting.iBaseX, pkMap->m_setting.iBaseY, NULL, false);
		}
	}

	return true;
}
#endif
#endif // __INGAME_EVENT_MANAGER__
