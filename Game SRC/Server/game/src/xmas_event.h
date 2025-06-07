#ifndef __INC_XMAS_EVENT_H__
#define __INC_XMAS_EVENT_H__

struct SNPCPosition
{
	long map_index;
	int x;
	int y;
};

namespace xmas
{
	enum EXmasNPC
	{
		MOB_XMAS_SANTA_VNUM = 20031, // ªÍ≈∏
		MOB_XMAS_TREE_VNUM = 20032,
		MOB_XMAS_FIRWORK_SELLER_VNUM = 9004,
#if defined(__XMAS_EVENT_2012__)
		MOB_NEW_XMAS_SANTA_VNUM = 20126,
		MOB_NEW_XMAS_TREE_VNUM = 20384,
#endif
	};

#if defined(__XMAS_EVENT_2008__)
	void ProcessEventFlag(const std::string& name, int prev_value, int value);
	void SpawnSanta(long lMapIndex, int iTimeGapSec);
	void SpawnEventHelper(bool spawn);
#endif

#if defined(__XMAS_EVENT_2012__)
	void SpawnNewSanta(const bool c_bSpawn);
	void SpawnNewXmasTree(const bool c_bSpawn);
#endif
}

#if defined(__SNOWFLAKE_STICK_EVENT__)
/**
* Filename: xmas_event.h
* Author: Owsap
**/

class CSnowflakeStickEvent
{
public:
	enum EMisc
	{
		SNOWFLAKE_STICK_EVENT_REFERENCE_LEVEL = 60,
		SNOWFLAKE_STICK_USE_COOLTIME = 1800,
		SNOWFLAKE_STICK_POLY_DISTANCE = 1000,
		SNOWFLAKE_STICK_POLY_LIMIT = 8,
		EXCHANGE_STICK_NEED_MATERIAL_COUNT = 12,
		EXCHANGE_STICK_MAX_MATERIAL_COUNT = 99,
		EXCHANGE_STICK_COUNT_INIT_COOLTIME = 86400,
		EXCHANGE_PET_NEED_MATERIAL_COUNT = 15,
		EXCHANGE_MOUNT_NEED_MATERIAL_COUNT = 15,
		EXCHANGE_STICK_COUNT_MAX = 5,
		EXCHANGE_PET_COUNT_MAX = 3,
		EXCHANGE_MOUNT_COUNT_MAX = 3,
	};

	enum ERankBuff
	{
		RANK_BUFF_TYPE_S_PAWN,
		RANK_BUFF_TYPE_KNIGHT,
		RANK_BUFF_TYPE_S_KNIGHT,
		RANK_BUFF_TYPE_STONE
	};

	using TQuestFlagData = struct SQuestFlagData
	{
		BYTE bQuestFlagType;
		const char* szQuestFlag;
	};

public:
	CSnowflakeStickEvent();
	~CSnowflakeStickEvent();

	static void Reward(LPCHARACTER pChar);
	static void Exchange(LPCHARACTER pChar, BYTE bSubHeader);
	static void Process(LPCHARACTER pChar, BYTE bSubHeader, INT iValue = -1);

	static void Reset(LPCHARACTER pChar, bool bExchangeCooldown = false);
	static void EnterGame(LPCHARACTER pChar);
	static bool UseStick(LPCHARACTER pChar);

private:
	static DWORD __GetQuestFlag(const LPCHARACTER& rChar, const BYTE bQuestFlagType);
	static void __SetQuestFlag(const LPCHARACTER& rChar, const BYTE bQuestFlag, const INT iValue);

	using TQuestDataMap = std::map<BYTE, std::string>;
	static std::string m_str_QuestName;
	static TQuestDataMap m_map_QuestData;
};
#endif // __SNOWFLAKE_STICK_EVENT__
#endif // __INC_XMAS_EVENT_H__
