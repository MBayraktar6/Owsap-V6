/**
* Filename: minigame_rumi.h
* Author: Owsap
**/

#if !defined(__INC_MINI_GAME_RUMI_H__) && defined(__MINI_GAME_RUMI__)
#define __INC_MINI_GAME_RUMI_H__

#define __RUMI_DEALER__ // Always deal new cards when drawing.

#include "questlua.h"

class CMiniGameRumi
{
public:
	CMiniGameRumi(LPCHARACTER pChar);
	~CMiniGameRumi();

	static void StartGame(LPCHARACTER pChar);
	static void EndGame(LPCHARACTER pChar);
	static void Analyze(LPCHARACTER pChar, BYTE bSubHeader, BOOL bUseCard, BYTE bIndex);

	static bool IsActiveEvent();
	static void SpawnEventNPC(bool bSpawn);

#if defined(__OKEY_EVENT_FLAG_RENEWAL__)
	static void RequestQuestFlag(LPCHARACTER pChar, BYTE bSubHeader);
	static bool UpdateQuestFlag(LPCHARACTER pChar);
#endif

	static void GetScoreTable(lua_State* L, bool bTotal);
	static DWORD GetMyScoreValue(lua_State* L, DWORD dwPID, bool bTotal);

public:
	enum ERumiScore
	{
		RUMI_LOW_TOTAL_SCORE = 300,
		RUMI_MID_TOTAL_SCORE = 400,
		RUMI_LOW_TOTAL_SCORE_NORMAL = RUMI_LOW_TOTAL_SCORE,
	};

	enum ERumiReward
	{
		RUMI_NORMAL_HIGH_REWARD = 50275, // Golden Okey Chest
		RUMI_NORMAL_MID_REWARD = 50276, // Silver Okey Chest
		RUMI_NORMAL_LOW_REWARD = 50277, // Bronze Okey Chest

		RUMI_HIGH_REWARD = 50267, // Merry Golden Okey Chest
		RUMI_MID_REWARD = 50268, // Merry Silver Okey Chest
		RUMI_LOW_REWARD = 50269, // Merry Bronze Okey Chest
	};

	enum ERumiCardPos
	{
		RUMI_NONE_POS,
		RUMI_DECK_CARD,
		RUMI_HAND_CARD,
		RUMI_FIELD_CARD,
		RUMI_CARD_POS_MAX,
	};

	enum ERumiState
	{
		RUMI_STATE_NONE,
		RUMI_STATE_WAITING,
		RUMI_STATE_PLAY,
	};

	enum ERumiGame
	{
		RUMI_DECK_CARD_INDEX_MAX = 3,
		RUMI_HAND_CARD_INDEX_MAX = 5,
		RUMI_FIELD_CARD_INDEX_MAX = 3,

		RUMI_EMPTY_CARD = 0,
		RUMI_RED_CARD = 10,
		RUMI_BLUE_CARD = 20,
		RUMI_YELLOW_CARD = 30,

		RUMI_CARD_COLOR_MAX = 3, // 카드 색상 개수
		RUMI_CARD_NUMBER_END = 8, // 한 색상당 카드 숫자
		RUMI_DECK_COUNT_MAX = RUMI_CARD_COLOR_MAX * RUMI_CARD_NUMBER_END,

		RUMI_TABLE = 20417,
		RUMI_START_GOLD = 30000,
#if defined(__OKEY_EVENT_FLAG_RENEWAL__)
		RUMI_CARD_PIECE_COUNT_MAX = RUMI_DECK_COUNT_MAX,
		RUMI_CARD_COUNT_MAX = 999,
#endif
		RUMI_REWARD_COOLDOWN = 604800,
	};

	BYTE GetDeckCount() const;
	INT GetEmptyHandPosition() const;

	void ClickDeckCard();
	void ClickHandCard(BOOL bUseCard, BYTE bIndex);
	void ClickFieldCard(BYTE bIndex);

	void Reward();
	void UpdateMyScore(DWORD dwPID);

	void SendPacket(BYTE bSubHeader, void* pvData = nullptr);

private:
	using PairedCard = std::pair<BYTE, BYTE>;
	using DeckVector = std::vector<PairedCard>;
	using HandMap = std::map<BYTE, PairedCard>;
	using FieldMap = std::map<BYTE, PairedCard>;
	using GraveVector = std::vector<PairedCard>;

	void __ShuffleDeck();
	bool __CheckCombination();

	INT __GetEmptyHandPosition() const;
	INT __GetEmptyFieldPosition() const;

	DeckVector m_vecDeck;
	HandMap m_mapHand;
	FieldMap m_mapField;
	GraveVector m_vecGrave;

	BYTE m_bState;
	BYTE m_bCardCount;
	WORD m_wScore;

	LPCHARACTER m_pChar;
};
#endif // _INC_MINIGAME_RUMI_H_
