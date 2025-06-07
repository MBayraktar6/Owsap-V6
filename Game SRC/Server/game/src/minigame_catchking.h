#pragma once

#if !defined(__INC_MINI_GAME_CATCH_KING_H__) && defined(__MINI_GAME_CATCH_KING__)
#define __INC_MINI_GAME_CATCH_KING_H__

#include "questlua.h"

class CMiniGameCatchKing
{
public:
#if defined(__CATCH_KING_EVENT_FLAG_RENEWAL__)
	enum
	{
		CATCH_KING_PIECE_COUNT_MAX = 25,
		CATCH_KING_PACK_COUNT_MAX = 999,
	};

	static void RequestQuestFlag(LPCHARACTER pChar, BYTE bSubHeader);
	static bool UpdateQuestFlag(LPCHARACTER pChar);
#endif

	CMiniGameCatchKing();
	~CMiniGameCatchKing();

	static int Process(LPCHARACTER pkChar, const char* c_pszData, size_t uiBytes);

	static bool IsActiveEvent();
	static void SpawnEventNPC(bool bSpawn);

	static void StartGame(LPCHARACTER pkChar, BYTE bSetCount);
	static void DeckCardClick(LPCHARACTER pkChar);
	static void FieldCardClick(LPCHARACTER pkChar, BYTE bFieldPos);
	static void GetReward(LPCHARACTER pkChar);

	static void RegisterScore(LPCHARACTER pkChar, DWORD dwScore);

	static int GetScore(lua_State* L, bool isTotal);
	static int GetMyScore(LPCHARACTER pkChar, bool isTotal = false);
};

#endif // __MINI_GAME_CATCH_KING__
