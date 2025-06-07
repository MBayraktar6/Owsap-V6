/**
* Filename: ingame_event_manager.h
* Author: Owsap
**/

#if !defined(__INC_INGAME_EVENT_MANAGER_H__) && defined(__INGAME_EVENT_MANAGER__)
#define __INC_INGAME_EVENT_MANAGER_H__

using InGameEventProcessFunc = void(*)(LPCHARACTER, const std::string&, int);
class InGameEventProcessor
{
public:
	static void Process(const std::string& rkEventName, int iValue, InGameEventProcessFunc pFunc);
};

class CInGameEventManager : public singleton<CInGameEventManager>
{
public:
	enum EInGameEventType
	{
		INGAME_EVENT_TYPE_NONE,
		INGAME_EVENT_TYPE_MYSTERY_BOX1,
		INGAME_EVENT_TYPE_MYSTERY_BOX2,
		//INGAME_EVENT_TYPE_LUCKY_EVENT,
		//INGAME_EVENT_TYPE_ATTENDANCE,
#if defined(__MINI_GAME_RUMI__)
		INGAME_EVENT_TYPE_OKEY,
		INGAME_EVENT_TYPE_OKEY_NORMAL,
#endif
#if defined(__XMAS_EVENT_2008__) || defined(__XMAS_EVENT_2012__)
		INGAME_EVENT_TYPE_NEW_XMAS_EVENT,
#endif
#if defined(__MINI_GAME_YUTNORI__)
		INGAME_EVENT_TYPE_YUTNORI,
#endif
		//INGAME_EVENT_TYPE_MONSTERBACK_EVENT,
		//INGAME_EVENT_TYPE_MONSTERBACK_10TH_EVENT,
#if defined(__EASTER_EVENT__)
		INGAME_EVENT_TYPE_EASTER_EVENT,
#endif
		INGAME_EVENT_TYPE_ICECREAM_EVENT,
		INGAME_EVENT_TYPE_RAMADAN_EVENT,
#if defined(__HALLOWEEN_EVENT_2014__)
		INGAME_EVENT_TYPE_HALLOWEEN_EVENT,
#endif
		INGAME_EVENT_TYPE_FOOTBALL_EVENT,
		INGAME_EVENT_TYPE_OLYMPIC_EVENT,
		INGAME_EVENT_TYPE_VALENTINE_DAY_EVENT,
		//INGAME_EVENT_TYPE_FISH,
#if defined(__FLOWER_EVENT__)
		INGAME_EVENT_TYPE_FLOWER_EVENT,
#endif
#if defined(__MINI_GAME_CATCH_KING__)
		INGAME_EVENT_TYPE_CATCHKING,
#endif
		//INGAME_EVENT_TYPE_MINIBOSS,
#if defined(__SUMMER_EVENT_ROULETTE__)
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
#if defined(__SNOWFLAKE_STICK_EVENT__)
		INGAME_EVENT_TYPE_SNOWFLAKE_STICK_EVENT,
#endif
		//INGAME_EVENT_TYPE_TREASURE_HUNT,
		INGAME_EVENT_TYPE_MAX
	};

	typedef std::unordered_map<std::string, BYTE> TInGameEventNameMap;

public:
	CInGameEventManager();
	~CInGameEventManager();

	void Destroy();

	BYTE GetInGameEventType(const std::string strEventName) const;

	void SetInGameEvent(const std::string& stEventName, int iValue, int iPrevValue);
	void BroadcastInGameEventOnLogin(LPCHARACTER pChar);

	void UpdateInGameEvent();

private:
	TInGameEventNameMap m_map_InGameEventName;

#if defined(__EVENT_BANNER_FLAG__)
public:
	typedef std::unordered_map<DWORD, std::string> BannerMapType;

	bool InitializeBanners();
	bool SpawnBanners(int iEnable, const char* c_szBannerName = NULL);

protected:
	BannerMapType m_map_BannerType;
	bool m_bIsLoadedBanners;
#endif
};
#endif
