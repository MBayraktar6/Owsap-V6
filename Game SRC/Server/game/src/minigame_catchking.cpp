#include "stdafx.h"

#if defined(__MINI_GAME_CATCH_KING__)
#include "minigame_catchking.h"

#include "char.h"
#include "desc.h"
#include "char_manager.h"
#include "desc_manager.h"
#include "sectree_manager.h"
#include "questmanager.h"
#include "config.h"
#include "db.h"
#include "unique_item.h"

CMiniGameCatchKing::CMiniGameCatchKing() {}
CMiniGameCatchKing::~CMiniGameCatchKing() {}

int CMiniGameCatchKing::Process(LPCHARACTER pkChar, const char* c_pszData, size_t uiBytes)
{
	const TPacketCGMiniGameCatchKing* pkData = reinterpret_cast<const TPacketCGMiniGameCatchKing*>(c_pszData);
	if (uiBytes < sizeof(TPacketCGMiniGameCatchKing))
		return -1;

	uiBytes -= sizeof(TPacketCGMiniGameCatchKing);
	switch (pkData->bSubHeader)
	{
		case CATCHKING_CG_START:
			StartGame(pkChar, pkData->bSubArgument);
			return 0;

		case CATCHKING_CG_CLICK_HAND:
			DeckCardClick(pkChar);
			return 0;

		case CATCHKING_CG_CLICK_CARD:
			FieldCardClick(pkChar, pkData->bSubArgument);
			return 0;

		case CATCHKING_CG_REWARD:
			GetReward(pkChar);
			return 0;

#if defined(__CATCH_KING_EVENT_FLAG_RENEWAL__)
		case CATCHKING_CG_REQUEST_QUEST_FLAG:
			RequestQuestFlag(pkChar, CATCHKING_GC_SET_QUEST_FLAG);
			break;
#endif

		default:
			sys_err("CMiniGameCatchKing::Process : Unknown SubHeader %d : %s", pkData->bSubHeader, pkChar->GetName());
			break;
	}

	return 0;
}

/* static */ bool CMiniGameCatchKing::IsActiveEvent()
{
	if (quest::CQuestManager::instance().GetEventFlag("mini_game_catchking"))
		return true;

	return false;
}

/* static */ void CMiniGameCatchKing::SpawnEventNPC(bool bSpawn)
{
	CharacterVectorInteractor vChar;
	if (CHARACTER_MANAGER::instance().GetCharactersByRaceNum(20506, vChar))
	{
		std::for_each(vChar.begin(), vChar.end(), [](LPCHARACTER ch) { M2_DESTROY_CHARACTER(ch); });
	}

	if (bSpawn)
	{
		std::map<long, std::pair<long, long>> vMap
		{
			{ MAP_A1, { 608, 623 } },
			{ MAP_B1, { 596, 614 } },
			{ MAP_C1, { 350, 738 } },
		};

		auto it = vMap.begin();
		for (; it != vMap.end(); ++it)
		{
			if (!map_allow_find(it->first))
				continue;

			const LPSECTREE_MAP c_lpSectree = SECTREE_MANAGER::instance().GetMap(it->first);
			if (c_lpSectree == nullptr)
				continue;

			CHARACTER_MANAGER::instance().SpawnMob(20506, it->first,
				c_lpSectree->m_setting.iBaseX + it->second.first * 100,
				c_lpSectree->m_setting.iBaseY + it->second.second * 100,
				0, false, 90, true);
		}
	}
}

/* static */ void CMiniGameCatchKing::StartGame(LPCHARACTER pkChar, BYTE bSetCount)
{
	if (pkChar == NULL)
		return;

	if (!pkChar->GetDesc())
		return;

	if (pkChar->MiniGameCatchKingGetGameStatus() == true)
		return;

	if (!IsActiveEvent())
		return;

	if (bSetCount < 1 || bSetCount > 5)
		return;

	if (pkChar->GetGold() < (30000 * bSetCount))
	{
		pkChar->ChatPacket(CHAT_TYPE_INFO, LC_STRING("[Catch the King] You don't have a complete King Deck or you have insufficient Yang."));
		return;
	}

#if defined(__CATCH_KING_EVENT_FLAG_RENEWAL__)
	const WORD wPackCount = pkChar->GetQuestFlag("minigame_catchking.pack_count");
	if (wPackCount < bSetCount)
		return;

	pkChar->SetQuestFlag("minigame_catchking.pack_count", wPackCount - bSetCount);
#else
	if (pkChar->CountSpecifyItem(ITEM_VNUM_CATCH_KING_PACK) < bSetCount)
		return;

	pkChar->RemoveSpecifyItem(ITEM_VNUM_CATCH_KING_PACK, bSetCount);
#endif
	pkChar->PointChange(POINT_GOLD, -(30000 * bSetCount));

	std::vector<TCatchKingCard> vFieldCards;

	std::srand(unsigned(std::time(0)));

	for (int i = 0; i < 25; i++)
	{
		if (i >= 0 && i < 7)
			vFieldCards.push_back(TCatchKingCard(1, false));
		else if (i >= 7 && i < 11)
			vFieldCards.push_back(TCatchKingCard(2, false));
		else if (i >= 11 && i < 16)
			vFieldCards.push_back(TCatchKingCard(3, false));
		else if (i >= 16 && i < 21)
			vFieldCards.push_back(TCatchKingCard(4, false));
		else if (i >= 21 && i < 24)
			vFieldCards.push_back(TCatchKingCard(5, false));
		else if (i >= 24)
			vFieldCards.push_back(TCatchKingCard(6, false)); // 6 = K
	}

	//std::random_shuffle(m_vecFieldCards.begin(), m_vecFieldCards.end());

	std::random_device rd;
	std::mt19937 g(rd());
	std::shuffle(vFieldCards.begin(), vFieldCards.end(), g);

	pkChar->MiniGameCatchKingSetFieldCards(vFieldCards);

	pkChar->MiniGameCatchKingSetBetNumber(bSetCount);
	pkChar->MiniGameCatchKingSetHandCardLeft(12);
	pkChar->MiniGameCatchKingSetGameStatus(true);

	DWORD dwBigScore = GetMyScore(pkChar);

	TPacketGCMiniGameCatchKing packet;
	packet.bHeader = HEADER_GC_MINI_GAME_CATCH_KING;
	packet.bSubHeader = CATCHKING_GC_START;

	packet.wSize = sizeof(packet) + sizeof(dwBigScore);

	pkChar->GetDesc()->BufferedPacket(&packet, sizeof(TPacketGCMiniGameCatchKing));
	pkChar->GetDesc()->Packet(&dwBigScore, sizeof(DWORD));
}

/* static */ void CMiniGameCatchKing::DeckCardClick(LPCHARACTER pkChar)
{
	if (pkChar == NULL)
		return;

	if (!pkChar->GetDesc())
		return;

	if (!IsActiveEvent())
		return;

	if (pkChar->MiniGameCatchKingGetGameStatus() == false)
		return;

	if (pkChar->MiniGameCatchKingGetHandCard())
		return;

	BYTE bCardLeft = pkChar->MiniGameCatchKingGetHandCardLeft();

	if (bCardLeft)
	{
		if (bCardLeft <= 12 && bCardLeft > 7)
			pkChar->MiniGameCatchKingSetHandCard(1);
		else if (bCardLeft <= 7 && bCardLeft > 5)
			pkChar->MiniGameCatchKingSetHandCard(2);
		else if (bCardLeft <= 5 && bCardLeft > 3)
			pkChar->MiniGameCatchKingSetHandCard(3);
		else if (bCardLeft == 3)
			pkChar->MiniGameCatchKingSetHandCard(4);
		else if (bCardLeft == 2)
			pkChar->MiniGameCatchKingSetHandCard(5);
		else if (bCardLeft == 1)
			pkChar->MiniGameCatchKingSetHandCard(6);
	}
	else
		return;

	BYTE bCardInHand = pkChar->MiniGameCatchKingGetHandCard();

	if (!bCardInHand)
		return;

	pkChar->MiniGameCatchKingSetHandCardLeft(bCardLeft - 1);

	TPacketGCMiniGameCatchKing packet;
	packet.bHeader = HEADER_GC_MINI_GAME_CATCH_KING;
	packet.bSubHeader = CATCHKING_GC_SET_CARD;

	packet.wSize = sizeof(packet) + sizeof(bCardInHand);

	pkChar->GetDesc()->BufferedPacket(&packet, sizeof(TPacketGCMiniGameCatchKing));
	pkChar->GetDesc()->Packet(&bCardInHand, sizeof(BYTE));
}

/* static */ void CMiniGameCatchKing::FieldCardClick(LPCHARACTER pkChar, BYTE bFieldPos)
{
	if (pkChar == NULL)
		return;

	if (!pkChar->GetDesc())
		return;

	if (!IsActiveEvent())
		return;

	if (pkChar->MiniGameCatchKingGetGameStatus() == false)
		return;

	if (bFieldPos < 0 || bFieldPos > 24)
		return;

	BYTE bHandCard = pkChar->MiniGameCatchKingGetHandCard();
	TCatchKingCard filedCard = pkChar->m_vecCatchKingFieldCards[bFieldPos];

	if (!bHandCard)
		return;

	if (filedCard.bIsExposed == true)
		return;

	DWORD dwPoints = 0;
	bool bDestroyCard = false;
	bool bKeepFieldCard = false;
	bool bGetReward = false;

	if (bHandCard < 5)
	{
		if (bHandCard < filedCard.bIndex)
		{
			dwPoints = 0;
			bDestroyCard = true;
			bKeepFieldCard = false;
		}
		else if (bHandCard == filedCard.bIndex)
		{
			dwPoints = filedCard.bIndex * 10;
			bDestroyCard = true;
			bKeepFieldCard = true;
		}
		else
		{
			dwPoints = filedCard.bIndex * 10;
			bDestroyCard = false;
			bKeepFieldCard = true;
		}
	}

	int checkPos[8];
	checkPos[0] = bFieldPos - 5;
	checkPos[1] = bFieldPos + 5;
	checkPos[2] = (bFieldPos % 10 == 4 || bFieldPos % 10 == 9) ? -1 : (bFieldPos + 1);
	checkPos[3] = (bFieldPos % 10 == 0 || bFieldPos % 10 == 5) ? -1 : (bFieldPos - 1);
	checkPos[4] = (bFieldPos % 10 == 4 || bFieldPos % 10 == 9) ? -1 : (bFieldPos - 5 + 1);
	checkPos[5] = (bFieldPos % 10 == 4 || bFieldPos % 10 == 9) ? -1 : (bFieldPos + 5 + 1);
	checkPos[6] = (bFieldPos % 10 == 0 || bFieldPos % 10 == 5) ? -1 : (bFieldPos - 5 - 1);
	checkPos[7] = (bFieldPos % 10 == 0 || bFieldPos % 10 == 5) ? -1 : (bFieldPos + 5 - 1);

	bool isFiveNearby = false;

	for (int i = 0; i < 25; i++)
	{
		if (isFiveNearby)
			break;

		for (int j = 0; j < sizeof(checkPos) / sizeof(checkPos[0]); j++)
		{
			if (checkPos[j] < 0 || checkPos[j] >= 25)
				continue;

			if (i == checkPos[j] && pkChar->m_vecCatchKingFieldCards[i].bIndex == 5)
			{
				isFiveNearby = true;
				break;
			}
		}
	}

	if (bHandCard == 5)
	{
		if (isFiveNearby)
		{
			dwPoints = 0;
			bDestroyCard = true;
			bKeepFieldCard = (bHandCard >= filedCard.bIndex) ? true : false;
		}
		else
		{
			dwPoints = (bHandCard >= filedCard.bIndex) ? filedCard.bIndex * 10 : 0;
			bDestroyCard = (bHandCard > filedCard.bIndex) ? false : true;
			bKeepFieldCard = (bHandCard >= filedCard.bIndex) ? true : false;
		}
	}

	if (bHandCard == 6)
	{
		dwPoints = (bHandCard == filedCard.bIndex) ? 100 : 0;
		bDestroyCard = true;
		bKeepFieldCard = (bHandCard == filedCard.bIndex) ? true : false;
	}

	if (bKeepFieldCard)
	{
		pkChar->m_vecCatchKingFieldCards[bFieldPos].bIsExposed = true;
	}

	int checkRowPos[4];
	checkRowPos[0] = 5 * (bFieldPos / 5);
	checkRowPos[1] = 4 + (5 * (bFieldPos / 5));
	checkRowPos[2] = bFieldPos - (5 * (bFieldPos / 5));
	checkRowPos[3] = bFieldPos + 20 - (5 * (bFieldPos / 5));

	bool isHorizontalRow = true;
	bool isVerticalRow = true;

	for (int row = checkRowPos[0]; row <= checkRowPos[1]; row += 1)
	{
		if (!pkChar->m_vecCatchKingFieldCards[row].bIsExposed)
		{
			isHorizontalRow = false;
			break;
		}
	}

	for (int col = checkRowPos[2]; col <= checkRowPos[3]; col += 5)
	{
		if (!pkChar->m_vecCatchKingFieldCards[col].bIsExposed)
		{
			isVerticalRow = false;
			break;
		}
	}

	dwPoints += isHorizontalRow ? 10 : 0;
	dwPoints += isVerticalRow ? 10 : 0;

	if (dwPoints)
	{
		pkChar->MiniGameCatchKingSetScore(pkChar->MiniGameCatchKingGetScore() + dwPoints);
	}

	bool isTheEnd = false;

	if (bDestroyCard)
	{
		bGetReward = (bHandCard == 6 && pkChar->MiniGameCatchKingGetScore() >= 10) ? true : false;
		isTheEnd = (bHandCard == 6) ? true : false;
		pkChar->MiniGameCatchKingSetHandCard(0);
	}

	BYTE bRowType = 0;
	if (isHorizontalRow && !isVerticalRow)
	{
		bRowType = 1;
	}
	else if (!isHorizontalRow && isVerticalRow)
	{
		bRowType = 2;
	}
	else if (isHorizontalRow && isVerticalRow)
	{
		bRowType = 3;
	}

	TPacketGCMiniGameCatchKing packet;
	packet.bHeader = HEADER_GC_MINI_GAME_CATCH_KING;
	packet.bSubHeader = CATCHKING_GC_RESULT_FIELD;

	TPacketGCMiniGameCatchKingResult packetSecond;
	packetSecond.dwPoints = pkChar->MiniGameCatchKingGetScore();
	packetSecond.bRowType = bRowType;
	packetSecond.bCardPos = bFieldPos;
	packetSecond.bCardValue = filedCard.bIndex;
	packetSecond.bKeepFieldCard = bKeepFieldCard;
	packetSecond.bDestroyHandCard = bDestroyCard;
	packetSecond.bGetReward = bGetReward;
	packetSecond.bIsFiveNearBy = isFiveNearby;

	packet.wSize = sizeof(packet) + sizeof(packetSecond);

	pkChar->GetDesc()->BufferedPacket(&packet, sizeof(TPacketGCMiniGameCatchKing));
	pkChar->GetDesc()->Packet(&packetSecond, sizeof(TPacketGCMiniGameCatchKingResult));

	if (isTheEnd)
	{
		for (int i = 0; i < 25; i++)
		{
			if (!pkChar->m_vecCatchKingFieldCards[i].bIsExposed)
			{
				TPacketGCMiniGameCatchKing packet;
				packet.bHeader = HEADER_GC_MINI_GAME_CATCH_KING;
				packet.bSubHeader = CATCHKING_GC_SET_END_CARD;

				TPacketGCMiniGameCatchKingSetEndCard packetSecond;
				packetSecond.bCardPos = i;
				packetSecond.bCardValue = pkChar->m_vecCatchKingFieldCards[i].bIndex;

				packet.wSize = sizeof(packet) + sizeof(packetSecond);

				pkChar->GetDesc()->BufferedPacket(&packet, sizeof(TPacketGCMiniGameCatchKing));
				pkChar->GetDesc()->Packet(&packetSecond, sizeof(TPacketGCMiniGameCatchKingSetEndCard));
			}
		}
	}
}

/* static */ void CMiniGameCatchKing::GetReward(LPCHARACTER pkChar)
{
	if (pkChar == NULL)
		return;

	if (!pkChar->GetDesc())
		return;

	if (!IsActiveEvent())
		return;

	if (pkChar->MiniGameCatchKingGetGameStatus() == false)
		return;

	if (pkChar->MiniGameCatchKingGetHandCard())
		return;

	if (pkChar->MiniGameCatchKingGetHandCardLeft())
		return;

	DWORD dwRewardVnum = 0;
	BYTE bReturnCode = 0;
	DWORD dwScore = pkChar->MiniGameCatchKingGetScore();

	if (dwScore >= 10 && dwScore < 400)
		dwRewardVnum = 50930;
	else if (dwScore >= 400 && dwScore < 550)
		dwRewardVnum = 50929;
	else if (dwScore >= 550)
		dwRewardVnum = 50928;

	RegisterScore(pkChar, dwScore);

	if (dwRewardVnum)
	{
		pkChar->MiniGameCatchKingSetScore(0);
		pkChar->AutoGiveItem(dwRewardVnum, pkChar->MiniGameCatchKingGetBetNumber());
		pkChar->MiniGameCatchKingSetBetNumber(0);
		pkChar->MiniGameCatchKingSetGameStatus(false);
		pkChar->m_vecCatchKingFieldCards.clear();

		bReturnCode = 0;
	}
	else
	{
		bReturnCode = 1;
	}

	TPacketGCMiniGameCatchKing packet;
	packet.bHeader = HEADER_GC_MINI_GAME_CATCH_KING;
	packet.bSubHeader = CATCHKING_GC_REWARD;

	packet.wSize = sizeof(packet) + sizeof(bReturnCode);

	pkChar->GetDesc()->BufferedPacket(&packet, sizeof(TPacketGCMiniGameCatchKing));
	pkChar->GetDesc()->Packet(&bReturnCode, sizeof(BYTE));
}

/* static */ void CMiniGameCatchKing::RegisterScore(LPCHARACTER pkChar, DWORD dwScore)
{
	if (pkChar == NULL)
		return;

	if (!pkChar->GetDesc())
		return;

	char querySelect[256];

	snprintf(querySelect, sizeof(querySelect),
		"SELECT max_score FROM log.catck_king_event WHERE name = '%s' LIMIT 1;", pkChar->GetName());

	std::unique_ptr<SQLMsg> pSelectMsg(DBManager::instance().DirectQuery(querySelect));
	SQLResult* resSelect = pSelectMsg->Get();

	if (resSelect && resSelect->uiNumRows > 0)
	{
		DWORD maxScore;
		MYSQL_ROW row = mysql_fetch_row(resSelect->pSQLResult);
		str_to_number(maxScore, row[0]);

		if (dwScore > maxScore)
			DBManager::instance().DirectQuery("UPDATE log.catck_king_event SET max_score = %d, total_score = total_score + %d WHERE name = '%s';", dwScore, dwScore, pkChar->GetName());
		else
			DBManager::instance().DirectQuery("UPDATE log.catck_king_event SET total_score = total_score + %d WHERE name = '%s';", dwScore, pkChar->GetName());
	}
	else
	{
		DBManager::instance().DirectQuery("REPLACE INTO log.catck_king_event (name, empire, max_score, total_score) VALUES ('%s', %d, %d, %d);",
			pkChar->GetName(), pkChar->GetEmpire(), dwScore, dwScore);
	}
}

/* static */ int CMiniGameCatchKing::GetScore(lua_State* L, bool isTotal)
{
	DWORD index = 1;
	lua_newtable(L);

	char querySelect[256];

	if (isTotal)
	{
		snprintf(querySelect, sizeof(querySelect),
			"SELECT name, empire, total_score FROM log.catck_king_event ORDER BY total_score DESC LIMIT 10;");
	}
	else
	{
		snprintf(querySelect, sizeof(querySelect),
			"SELECT name, empire, max_score FROM log.catck_king_event ORDER BY max_score DESC LIMIT 10;");
	}

	std::unique_ptr<SQLMsg> pSelectMsg(DBManager::instance().DirectQuery(querySelect));
	SQLResult* resSelect = pSelectMsg->Get();

	if (resSelect && resSelect->uiNumRows > 0)
	{
		for (uint i = 0; i < resSelect->uiNumRows; i++)
		{
			MYSQL_ROW row = mysql_fetch_row(resSelect->pSQLResult);
			BYTE bEmpire;
			DWORD dwScore;

			str_to_number(bEmpire, row[1]);
			str_to_number(dwScore, row[2]);

			lua_newtable(L);

			lua_pushstring(L, row[0]);
			lua_rawseti(L, -2, 1);

			lua_pushnumber(L, bEmpire);
			lua_rawseti(L, -2, 2);

			lua_pushnumber(L, dwScore);
			lua_rawseti(L, -2, 3);

			lua_rawseti(L, -2, index++);
		}
	}

	return 0;
}

/* static */ int CMiniGameCatchKing::GetMyScore(LPCHARACTER pkChar, bool isTotal)
{
	if (pkChar == NULL)
		return 0;

	if (!pkChar->GetDesc())
		return 0;

	char querySelect[256];

	if (isTotal)
		snprintf(querySelect, sizeof(querySelect), "SELECT total_score FROM log.catck_king_event WHERE name = '%s' LIMIT 1;", pkChar->GetName());
	else
		snprintf(querySelect, sizeof(querySelect), "SELECT max_score FROM log.catck_king_event WHERE name = '%s' LIMIT 1;", pkChar->GetName());

	std::unique_ptr<SQLMsg> pSelectMsg(DBManager::instance().DirectQuery(querySelect));
	SQLResult* resSelect = pSelectMsg->Get();

	if (resSelect && resSelect->uiNumRows > 0)
	{
		DWORD dwScore = 0;
		MYSQL_ROW row = mysql_fetch_row(resSelect->pSQLResult);

		str_to_number(dwScore, row[0]);

		return dwScore;
	}

	return 0;
}

#if defined(__CATCH_KING_EVENT_FLAG_RENEWAL__)
/* static */ bool CMiniGameCatchKing::UpdateQuestFlag(LPCHARACTER pChar)
{
	if (!IsActiveEvent())
		return false;

	const WORD wPieceCount = pChar->GetQuestFlag("minigame_catchking.piece_count");
	const WORD wPackCount = pChar->GetQuestFlag("minigame_catchking.pack_count");

	if (wPackCount >= CATCH_KING_PACK_COUNT_MAX)
	{
		RequestQuestFlag(pChar, CATCHKING_GC_NO_MORE_GAIN);
		return false;
	}

	if (wPieceCount + 1 >= CATCH_KING_PIECE_COUNT_MAX)
	{
		pChar->SetQuestFlag("minigame_catchking.piece_count", 0);
		pChar->SetQuestFlag("minigame_catchking.pack_count", wPackCount + 1);

		RequestQuestFlag(pChar, CATCHKING_GC_SET_CARD_FLAG);
	}
	else
	{
		pChar->SetQuestFlag("minigame_catchking.piece_count", wPieceCount + 1);

		RequestQuestFlag(pChar, CATCHKING_GC_SET_CARD_PIECE_FLAG);
	}

	return true;
}

/* static */ void CMiniGameCatchKing::RequestQuestFlag(LPCHARACTER pChar, BYTE bSubHeader)
{
	const LPDESC pkDesc = pChar->GetDesc();
	if (pkDesc == nullptr)
	{
		sys_err("CMiniGameCatchKing::RequestQuestFlag - Null DESC! (ch: %s) (%d)",
			pChar->GetName(), bSubHeader);
		return;
	}

	const WORD wPieceCount = pChar->GetQuestFlag("minigame_catchking.piece_count");
	const WORD wPackCount = pChar->GetQuestFlag("minigame_catchking.pack_count");

	TPacketGCMiniGameCatchKing Packet;
	Packet.bHeader = HEADER_GC_MINI_GAME_CATCH_KING;
	Packet.bSubHeader = bSubHeader;

	TPacketGCMiniGameCatchKingQuestFlag QuestFlagPacket(wPieceCount, wPackCount);
	Packet.wSize = sizeof(Packet) + sizeof(QuestFlagPacket);

	pkDesc->BufferedPacket(&Packet, sizeof(TPacketGCMiniGameCatchKing));
	pkDesc->Packet(&QuestFlagPacket, sizeof(TPacketGCMiniGameCatchKingQuestFlag));
}
#endif
#endif
