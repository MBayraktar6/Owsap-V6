/**
* Filename: minigame_yutnori.h
* Author: Owsap
**/

#if !defined(__INC_MINI_GAME_YUTNORI_H__) && defined(__MINI_GAME_YUTNORI__)
#define __INC_MINI_GAME_YUTNORI_H__

#include "questlua.h"

class CMiniGameYutnori
{
public:
	CMiniGameYutnori(LPCHARACTER pChar);
	~CMiniGameYutnori();

	static void Create(LPCHARACTER pChar);
	static void Destroy(LPCHARACTER pChar);
	static void Analyze(LPCHARACTER pChar, BYTE bSubHeader, BYTE bArgument);
	
	static bool IsActiveEvent();
	static void SpawnEventNPC(bool bSpawn);

#if defined(__YUTNORI_EVENT_FLAG_RENEWAL__)
	static void RequestQuestFlag(LPCHARACTER pChar, BYTE bSubHeader);
	static bool UpdateQuestFlag(LPCHARACTER pChar);
#endif

	static void GetScoreTable(lua_State* L, bool bTotal);
	static DWORD GetMyScoreValue(lua_State* L, DWORD dwPID, bool bTotal);

public:
	enum EYutnoriYutSem
	{
		YUTNORI_YUTSEM1,
		YUTNORI_YUTSEM2,
		YUTNORI_YUTSEM3,
		YUTNORI_YUTSEM4,
		YUTNORI_YUTSEM5,
		YUTNORI_YUTSEM6,
		YUTNORI_YUTSEM_MAX
	};

	enum EYutnoriState
	{
		YUTNORI_STATE_THROW, // 윷을 던져야 하는 상태
		YUTNORI_STATE_RE_THROW, // 윷,모가 나와 다시 던지는 상태
		YUTNORI_STATE_MOVE, // 윷말을 이동해야 하는 상태
		YUTNORI_BEFORE_TURN_SELECT, // 턴 결정 전
		YUTNORI_AFTER_TURN_SELECT, // 턴이 결정된 후
		YUTNORI_STATE_END // 게임이 종료
	};

	enum EYutnoriScore
	{
		YUTNORI_IN_DE_CREASE_SCORE = 10,
		YUTNORI_LOW_TOTAL_SCORE = 150,
		YUTNORI_MID_TOTAL_SCORE = 220,
	};

	enum EYutnoriReward
	{
		YUTNORI_HIGH_REWARD = 83030,
		YUTNORI_MID_REWARD = 83031,
		YUTNORI_LOW_REWARD = 50922,
	};

	enum EYutnoriGame
	{
		YUTNORI_PLAYER_MAX = 2,
		YUTNORI_GOAL_AREA = 11,
		YUTNORI_REWARD_COOLDOWN = 604800,

		YUTNORI_INIT_SCORE = 250,
		YUTNORI_INIT_REMAIN_COUNT = 20,

		YUTNORI_TABLE = 20502,
		YUTNORI_START_GOLD = 30000,
#if defined(__YUTNORI_EVENT_FLAG_RENEWAL__)
		YUTNORI_PIECE_COUNT_MAX = 28,
		YUTNORI_BOARD_COUNT_MAX = 999,
#endif
	};

public:
	void Giveup();
	void SetProb(BYTE bProbIndex);
	void Throw(bool bPC);
	void CharClick(BYTE bUnitIndex);
	void Move(BYTE bUnitIndex);
	void RequestComAction();
	void Reward();

	void SendPacket(BYTE bSubHeader, void* pvData = nullptr);

private:
	void __UpdateScore(bool bIncrease, bool bIsDouble);
	void __UpdateRemainCount();
	void __PushNextTurn(bool bPC, BYTE bState);
	void __UpdateMyScore(DWORD dwPID);

	bool __IsGoalArea(BYTE* pbUnitPos);
	bool __IsDouble(BYTE* pbUnitPos);
	bool __CanMove(bool bPC);
	bool __IsExcludeYutSem6(bool bPC);

	BYTE __GetComUnitIndex();
	BYTE __GetRandomYut(BYTE bReThrow, BYTE bExcludeYutSem6, BYTE bYutProb = YUTNORI_YUTSEM_MAX);
	void __GetDestPos(BYTE bMoveCount, BYTE* bStartIndex, BYTE* bDestIndex, BYTE* bLastDestIndex);

private:
	LPCHARACTER m_pChar;

	BYTE m_bFirstThrow;
	BYTE m_bReThrow;

	WORD m_wScore;
	BYTE m_bRemainCount;
	BYTE m_bRecvReward;

	BYTE m_bYutProb;
	BYTE m_bPC;

	BYTE m_bPCYut;
	BYTE m_bPCUnitPos[YUTNORI_PLAYER_MAX];
	BYTE m_bPCUnitLastPos[YUTNORI_PLAYER_MAX];

	BYTE m_bCOMYut;
	BYTE m_bCOMUnitPos[YUTNORI_PLAYER_MAX];
	BYTE m_bCOMUnitLastPos[YUTNORI_PLAYER_MAX];
	BYTE m_bComNextUnitIndex;
};
#endif // __MINI_GAME_YUTNORI__
