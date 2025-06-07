/**
* Filename: PythonInGameEventSystem.h
* Author: Owsap
**/

#pragma once

#if !defined(__INC_INGAME_EVENT_SYSTEM_MANAGER_H__) && defined(ENABLE_EVENT_BANNER)
#define __INC_INGAME_EVENT_SYSTEM_MANAGER_H__

class CPythonInGameEventSystemManager : public CSingleton<CPythonInGameEventSystemManager>
{
public:
	enum EInGameEventType
	{
		INGAME_EVENT_TYPE_NONE,
		INGAME_EVENT_TYPE_MYSTERY_BOX1,
		INGAME_EVENT_TYPE_MYSTERY_BOX2,
		//INGAME_EVENT_TYPE_LUCKY_EVENT,
		//INGAME_EVENT_TYPE_ATTENDANCE,
#if defined(ENABLE_MINI_GAME_RUMI)
		INGAME_EVENT_TYPE_OKEY,
		INGAME_EVENT_TYPE_OKEY_NORMAL,
#endif
		INGAME_EVENT_TYPE_NEW_XMAS_EVENT,
#if defined(ENABLE_MINI_GAME_YUTNORI)
		INGAME_EVENT_TYPE_YUTNORI,
#endif
		//INGAME_EVENT_TYPE_MONSTERBACK_EVENT,
		//INGAME_EVENT_TYPE_MONSTERBACK_10TH_EVENT,
		INGAME_EVENT_TYPE_EASTER_EVENT,
		INGAME_EVENT_TYPE_ICECREAM_EVENT,
		INGAME_EVENT_TYPE_RAMADAN_EVENT,
		INGAME_EVENT_TYPE_HALLOWEEN_EVENT,
		INGAME_EVENT_TYPE_FOOTBALL_EVENT,
		INGAME_EVENT_TYPE_OLYMPIC_EVENT,
		INGAME_EVENT_TYPE_VALENTINE_DAY_EVENT,
		//INGAME_EVENT_TYPE_FISH,
#if defined(ENABLE_FLOWER_EVENT)
		INGAME_EVENT_TYPE_FLOWER_EVENT,
#endif
#if defined(ENABLE_MINI_GAME_CATCH_KING)
		INGAME_EVENT_TYPE_CATCHKING,
#endif
		//INGAME_EVENT_TYPE_MINIBOSS,
#if defined(ENABLE_SUMMER_EVENT_ROULETTE)
		INGAME_EVENT_TYPE_ROULETTE,
#endif
		//INGAME_EVENT_TYPE_BLACK_AND_WHITE,
		//INGAME_EVENT_TYPE_WORLD_BOSS,
		//INGAME_EVENT_TYPE_BATTLE_ROYALE,
		//INGAME_EVENT_TYPE_METINSTONE_RAIN_EVENT,
		//INGAME_EVENT_TYPE_OTHER_WORLD,
		//INGAME_EVENT_TYPE_OTHER_WORLD_EVE,
		//INGAME_EVENT_TYPE_GOLDEN_LAND_EVENT,
		//INGAME_EVENT_TYPE_SPORTS_MATCH_EVENT,
#if defined(ENABLE_SNOWFLAKE_STICK_EVENT)
		INGAME_EVENT_TYPE_SNOWFLAKE_STICK_EVENT,
#endif
		//INGAME_EVENT_TYPE_TREASURE_HUNT,
		INGAME_EVENT_TYPE_MAX
	};

	enum EInGameEventReward
	{
		INGAME_EVENT_REWARD_LIST_MIN = 3,
		INGAME_EVENT_REWARD_LIST_MAX = 100,
	};

	typedef struct SInGameEventRewardData
	{
		int iItemIndex;
		int iItemCount;
		SInGameEventRewardData()
		{
			iItemIndex = 0;
			iItemCount = 0;
		}
	} TInGameEventRewardData;

	typedef std::vector<TInGameEventRewardData> TInGameEventRewardVector;

	typedef struct SInGameEventData
	{
		bool bEnable;
		time_t lStartTime;
		time_t lEndTime;
		TInGameEventRewardVector vRewardData;
		SInGameEventData()
		{
			bEnable = false;
			lStartTime = 0;
			lEndTime = 0;
			vRewardData.clear();
		}
	} TInGameEventData;

	typedef std::unordered_map<std::string, BYTE> TInGameEventNameMap;
	typedef std::unordered_map<BYTE, TInGameEventData> TInGameEventDataMap;

public:
	CPythonInGameEventSystemManager();
	~CPythonInGameEventSystemManager();

	void Destroy();
	void Clear();

	void SetIngameEventHandler(PyObject* poHandler);
	void DestroyInGameEventHandler();

	bool LoadInGameEventData(const char* c_szFileName);
	BYTE GetInGameEventType(const std::string strEventName) const;

	bool GetInGameEventRewardData(BYTE bInGameEventType, BYTE bSlotPos, TInGameEventRewardData& rkData);
	size_t GetEventRewardCount(BYTE bInGameEventType) const;

	void SetInGameEventEnable(BYTE bInGameEventType, int iEnable);
	bool GetInGameEventEnable(BYTE bInGameEventType);

	BYTE GetInGameEventCount() const;
	bool GetInGameEventData(BYTE bInGameEventType, TInGameEventData& rkData);

private:
	PyObject* m_poIngameEventHandler;
	TInGameEventNameMap m_map_InGameEventName;
	TInGameEventDataMap m_map_InGameEventData;
};
#endif // ENABLE_EVENT_BANNER
