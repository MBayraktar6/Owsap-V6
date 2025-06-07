/**
* Filename: minigame_rumi.cpp
* Author: Owsap
**/

#include "stdafx.h"

#if defined(__MINI_GAME_RUMI__)
#include "minigame_rumi.h"

#include "char.h"
#include "desc.h"
#include "db.h"
#include "config.h"
#include "char_manager.h"
#include "sectree_manager.h"
#include "questmanager.h"

CMiniGameRumi::CMiniGameRumi(LPCHARACTER pChar) : m_pChar(pChar)
{
	m_vecDeck.clear();
	m_mapHand.clear();
	m_vecGrave.clear();

	m_bState = RUMI_STATE_PLAY;
	m_bCardCount = 0;
	m_wScore = 0;

	SendPacket(RUMI_GC_SUBHEADER_START);

	__ShuffleDeck();
}

CMiniGameRumi::~CMiniGameRumi()
{
	m_vecDeck.clear();
	m_mapHand.clear();
	m_vecGrave.clear();

	m_bState = RUMI_STATE_NONE;
	m_bCardCount = 0;
	m_wScore = 0;

	SendPacket(RUMI_GC_SUBHEADER_END);
}

BYTE CMiniGameRumi::GetDeckCount() const
{
	return static_cast<BYTE>(m_vecDeck.size());
}

INT CMiniGameRumi::GetEmptyHandPosition() const
{
	INT iHandPosition = __GetEmptyHandPosition();
	if (iHandPosition == -1)
		return -1;

	return iHandPosition;
}

void CMiniGameRumi::ClickDeckCard()
{
	if (m_vecDeck.empty())
		return;

	if (m_bCardCount >= RUMI_HAND_CARD_INDEX_MAX)
		return;

	// Find the first available position in hand.
	INT iHandPosition = __GetEmptyHandPosition();
	if (iHandPosition == -1)
		return;

	static std::mt19937 rng(std::random_device{}());
	std::uniform_int_distribution<std::size_t> dist(0, m_vecDeck.size() - 1);
	std::size_t nIndex = dist(rng);

	// Get the selected card. <color, number>
	PairedCard Card = m_vecDeck[nIndex];

	// Add the card to the hand.
	m_mapHand[iHandPosition] = Card;

	{
		TPacketGCMiniGameRumiMoveCard MoveCardPacket = {};
		MoveCardPacket.bSrcPos = RUMI_DECK_CARD;

		MoveCardPacket.bDstPos = RUMI_HAND_CARD;
		MoveCardPacket.bDstIndex = iHandPosition;
		MoveCardPacket.bDstColor = Card.first;
		MoveCardPacket.bDstNumber = Card.second;

		SendPacket(RUMI_GC_SUBHEADER_MOVE_CARD, &MoveCardPacket);
	}

	// Remove the card from the deck.
	m_vecDeck.erase(m_vecDeck.begin() + nIndex);
	m_bCardCount += 1;
}

void CMiniGameRumi::ClickHandCard(BOOL bUseCard, BYTE bIndex)
{
	if (m_bCardCount <= 0 || m_mapHand.empty())
		return;

	if (bUseCard)
	{
		HandMap::iterator it = m_mapHand.find(bIndex);
		if (it != m_mapHand.end())
		{
			// Find the first available position in the field.
			INT iFieldPosition = __GetEmptyFieldPosition();
			if (iFieldPosition == -1)
				return;

			// Add the card to the field.
			m_mapField[iFieldPosition] = it->second;

			// Server Process
			{
				TPacketGCMiniGameRumiMoveCard MoveCardPacket = {};

				MoveCardPacket.bSrcPos = RUMI_HAND_CARD;
				MoveCardPacket.bSrcIndex = bIndex;
				MoveCardPacket.bSrcColor = it->second.first;
				MoveCardPacket.bSrcNumber = it->second.second;

				MoveCardPacket.bDstPos = RUMI_FIELD_CARD;
				MoveCardPacket.bDstIndex = iFieldPosition;
				MoveCardPacket.bDstColor = it->second.first;
				MoveCardPacket.bDstNumber = it->second.second;

				SendPacket(RUMI_GC_SUBHEADER_MOVE_CARD, &MoveCardPacket);
			}

			// Remove the card from the players hand.
			m_mapHand.erase(it);

			// Check if the field cards combine.
			if (m_mapField.size() == RUMI_FIELD_CARD_INDEX_MAX)
			{
				if (__CheckCombination())
				{
					m_bCardCount -= 3;
				}
				else
				{
					// Add the cards back to the hand.
					for (BYTE bIndex = 0; bIndex < RUMI_FIELD_CARD_INDEX_MAX; ++bIndex)
					{
						ClickFieldCard(bIndex);
					}
				}
			}
		}
	}
	else if (bUseCard == false)
	{
		HandMap::iterator it = m_mapHand.find(bIndex);
		if (it != m_mapHand.end())
		{
			// Add the card to the grave.
			m_vecGrave.emplace_back(it->second);

			// Server Process
			{
				TPacketGCMiniGameRumiMoveCard MoveCardPacket = {};

				MoveCardPacket.bSrcPos = RUMI_HAND_CARD;
				MoveCardPacket.bSrcIndex = bIndex;
				MoveCardPacket.bSrcColor = it->second.first;
				MoveCardPacket.bSrcNumber = it->second.second;

				MoveCardPacket.bDstPos = RUMI_NONE_POS;

				SendPacket(RUMI_GC_SUBHEADER_MOVE_CARD, &MoveCardPacket);
			}

			// Remove the card from the players hand.
			m_mapHand.erase(it);
			m_bCardCount -= 1;
		}
		else if (m_mapField.find(bIndex) != m_mapField.end())
		{
			m_pChar->ChatPacket(CHAT_TYPE_INFO, LC_STRING("You cannot discard cards when you have already selected some."));
			return;
		}
		else
		{
			// card already in grave?
		}
	}
}

void CMiniGameRumi::ClickFieldCard(BYTE bIndex)
{
	if (m_mapField.empty())
		return;

	FieldMap::iterator it = m_mapField.find(bIndex);
	if (it != m_mapField.end())
	{
		// Find the first available position in hand.
		INT iHandPosition = __GetEmptyHandPosition();
		if (iHandPosition == -1)
			return;

		// Add the card back to the hand.
		m_mapHand[iHandPosition] = it->second;

		// Update
		{
			TPacketGCMiniGameRumiMoveCard MoveCardPacket = {};

			MoveCardPacket.bSrcPos = RUMI_FIELD_CARD;
			MoveCardPacket.bSrcIndex = bIndex;
			MoveCardPacket.bSrcColor = it->second.first;
			MoveCardPacket.bSrcNumber = it->second.second;

			MoveCardPacket.bDstPos = RUMI_HAND_CARD;
			MoveCardPacket.bDstIndex = iHandPosition;
			MoveCardPacket.bDstColor = it->second.first;
			MoveCardPacket.bDstNumber = it->second.second;

			SendPacket(RUMI_GC_SUBHEADER_MOVE_CARD, &MoveCardPacket);
		}

		// Remove the card from the field.
		m_mapField.erase(it);
	}
}

void CMiniGameRumi::Reward()
{
	if (m_bState != RUMI_STATE_PLAY)
		return;

	int iMiniGameOkeyNormal = quest::CQuestManager::instance().GetEventFlag("mini_game_okey_normal");

	DWORD dwRewardVnum = 0;
	if (m_wScore >= RUMI_MID_TOTAL_SCORE)
	{
		dwRewardVnum = (iMiniGameOkeyNormal > 0) ? RUMI_NORMAL_HIGH_REWARD : RUMI_HIGH_REWARD;
	}
	else if (m_wScore >= RUMI_LOW_TOTAL_SCORE && m_wScore < RUMI_MID_TOTAL_SCORE)
	{
		dwRewardVnum = (iMiniGameOkeyNormal > 0) ? RUMI_NORMAL_MID_REWARD : RUMI_MID_REWARD;
	}
	else if (m_wScore >= 0 && m_wScore < RUMI_LOW_TOTAL_SCORE)
	{
		dwRewardVnum = (iMiniGameOkeyNormal > 0) ? RUMI_NORMAL_LOW_REWARD : RUMI_LOW_REWARD;
	}

	const TItemTable* pkTable = ITEM_MANAGER::instance().GetTable(dwRewardVnum);
	if (!pkTable)
	{
		sys_err("CMiniGameRumi::Reward - Failed to get item table. (ch: %s) (%d)",
			m_pChar->GetName(), dwRewardVnum);
		return;
	}

	bool bEnoughInventoryForItem = m_pChar->GetEmptyInventory(pkTable->bSize) != -1;
	if (!bEnoughInventoryForItem)
	{
		m_pChar->ChatPacket(CHAT_TYPE_INFO, LC_STRING("You can only receive your prize once you have made space in your inventory."));
		return;
	}

#if defined(__WJ_PICKUP_ITEM_EFFECT__)
	m_pChar->AutoGiveItem(dwRewardVnum, 1, -1, true, true);
#else
	m_pChar->AutoGiveItem(dwRewardVnum, 1, -1, true);
#endif

	UpdateMyScore(m_pChar->GetPlayerID());

	m_pChar->SetMiniGameRumi(nullptr);
}

void CMiniGameRumi::UpdateMyScore(DWORD dwPID)
{
	char szQuery[2048 + 1];
	int iLen = sprintf(szQuery, "INSERT INTO `minigame_rumi%s` (`pid`, `best_score`, `total_score`, `last_play`) ", get_table_postfix());
	iLen += sprintf(szQuery + iLen, "VALUES (%u, %u, %u, FROM_UNIXTIME(%d)) ", dwPID, m_wScore, m_wScore, std::time(nullptr));
	iLen += sprintf(szQuery + iLen, "ON DUPLICATE KEY UPDATE ");
	iLen += sprintf(szQuery + iLen, "`total_score` = `total_score` + %d, ", m_wScore);
	iLen += sprintf(szQuery + iLen, "`best_score` = if (%d > `best_score`, %d, `best_score`) ", m_wScore, m_wScore);
	DBManager::instance().DirectQuery(szQuery);
}

void CMiniGameRumi::SendPacket(BYTE bSubHeader, void* pvData)
{
	if (m_pChar == nullptr)
		return;

	const LPDESC pkDesc = m_pChar->GetDesc();
	if (pkDesc == nullptr)
	{
		sys_err("CMiniGameRumi::SendPacket - Null DESC! (ch: %s) (%d)",
			m_pChar->GetName(), bSubHeader);
		return;
	}

	TPacketGCMiniGameRumi Packet;
	Packet.bHeader = HEADER_GC_MINI_GAME_RUMI;
	Packet.bSubHeader = bSubHeader;

	const std::unordered_set<BYTE> EmptyData
	{
		RUMI_GC_SUBHEADER_END,
		RUMI_GC_SUBHEADER_START,
	};

	if (pvData == nullptr && EmptyData.find(bSubHeader) == EmptyData.end())
	{
		sys_err("CMiniGameRumi::SendPacket(bSubHeader=%u) Null Data! Character: %s",
			bSubHeader, m_pChar->GetName());
		return;
	}

	switch (bSubHeader)
	{
		case RUMI_GC_SUBHEADER_END:
		case RUMI_GC_SUBHEADER_START:
		{
			Packet.wSize = sizeof(Packet);
			pkDesc->Packet(&Packet, sizeof(TPacketGCMiniGameRumi));
		}
		break;

		case RUMI_GC_SUBHEADER_SET_DECK:
		{
			const TPacketGCMiniGameRumiSetDeck* pkData = static_cast<TPacketGCMiniGameRumiSetDeck*>(pvData);
			if (pkData == nullptr)
			{
				sys_err("CMiniGameRumi::SendPacket(bSubHeader=%u) Null SetDeckPacket! Character: %s",
					bSubHeader, m_pChar->GetName());
				return;
			}

			TPacketGCMiniGameRumiSetDeck SetDeckPacket(pkData->bDeckCount);
			Packet.wSize = sizeof(Packet) + sizeof(SetDeckPacket);

			pkDesc->BufferedPacket(&Packet, sizeof(TPacketGCMiniGameRumi));
			pkDesc->Packet(&SetDeckPacket, sizeof(TPacketGCMiniGameRumiSetDeck));
		}
		break;

		case RUMI_GC_SUBHEADER_MOVE_CARD:
		{
			const TPacketGCMiniGameRumiMoveCard* pkData = static_cast<TPacketGCMiniGameRumiMoveCard*>(pvData);
			if (pkData == nullptr)
			{
				sys_err("CMiniGameRumi::SendPacket(bSubHeader=%u) Null MoveCardPacket! Character: %s",
					bSubHeader, m_pChar->GetName());
				return;
			}

			TPacketGCMiniGameRumiMoveCard MoveCardPacket;
			MoveCardPacket.bSrcPos = pkData->bSrcPos;
			MoveCardPacket.bSrcIndex = pkData->bSrcIndex;
			MoveCardPacket.bSrcColor = pkData->bSrcColor;
			MoveCardPacket.bSrcNumber = pkData->bSrcNumber;
			MoveCardPacket.bDstPos = pkData->bDstPos;
			MoveCardPacket.bDstIndex = pkData->bDstIndex;
			MoveCardPacket.bDstColor = pkData->bDstColor;
			MoveCardPacket.bDstNumber = pkData->bDstNumber;

			Packet.wSize = sizeof(Packet) + sizeof(MoveCardPacket);

			pkDesc->BufferedPacket(&Packet, sizeof(TPacketGCMiniGameRumi));
			pkDesc->Packet(&MoveCardPacket, sizeof(TPacketGCMiniGameRumiMoveCard));
		}
		break;

		case RUMI_GC_SUBHEADER_SET_SCORE:
		{
			const TPacketGCMiniGameRumiSetScore* pkData = static_cast<TPacketGCMiniGameRumiSetScore*>(pvData);
			if (pkData == nullptr)
			{
				sys_err("CMiniGameRumi::SendPacket(bSubHeader=%u) Null SetScorePacket! Character: %s",
					bSubHeader, m_pChar->GetName());
				return;
			}

			TPacketGCMiniGameRumiSetScore SetScorePacket(pkData->wScore, pkData->wTotalScore);
			Packet.wSize = sizeof(Packet) + sizeof(SetScorePacket);

			pkDesc->BufferedPacket(&Packet, sizeof(TPacketGCMiniGameRumi));
			pkDesc->Packet(&SetScorePacket, sizeof(TPacketGCMiniGameRumiSetScore));
		}
		break;
	}
}

void CMiniGameRumi::__ShuffleDeck()
{
	std::array<BYTE, RUMI_CARD_COLOR_MAX> aCardColor = { RUMI_RED_CARD, RUMI_BLUE_CARD, RUMI_YELLOW_CARD };
	for (BYTE bColor : aCardColor)
	{
		for (BYTE bCardNumber = 1; bCardNumber <= RUMI_CARD_NUMBER_END; bCardNumber++)
		{
			m_vecDeck.emplace_back(bColor, bCardNumber);
		}
	}

	static std::mt19937 rng(std::random_device{}());
	std::shuffle(m_vecDeck.begin(), m_vecDeck.end(), rng);

	TPacketGCMiniGameRumiSetDeck SetDeckPacket(static_cast<BYTE>(m_vecDeck.size()));
	SendPacket(RUMI_GC_SUBHEADER_SET_DECK, &SetDeckPacket);
}

static bool __ContainsNumbers(const std::vector<BYTE>& rkNumVec, const std::vector<BYTE>& rkSearchNumVec)
{
	return std::all_of(rkSearchNumVec.begin(), rkSearchNumVec.end(),
		[&](BYTE bNumber)
		{
			return std::find(rkNumVec.begin(), rkNumVec.end(), bNumber) != rkNumVec.end();
		});
}

bool CMiniGameRumi::__CheckCombination()
{
	if (m_mapField.empty())
		return false;

	if (m_mapField.size() < RUMI_FIELD_CARD_INDEX_MAX)
		return false;

	std::vector<BYTE> vCardColor, vCardNumber;
	for (const auto& it : m_mapField)
	{
		PairedCard Card = it.second;
		vCardColor.push_back(Card.first);
		vCardNumber.push_back(Card.second);
	}

	WORD wScore = 0;

	const bool bSameNumber = std::all_of(vCardNumber.cbegin() + 1, vCardNumber.cend(),
		[&](BYTE bNumber) { return bNumber == vCardNumber.front(); });

	if (bSameNumber)
	{
		const BYTE bCardNumber = vCardNumber.front();
		wScore = (bCardNumber * 10) + 10;
	}
	else
	{
		const bool bSameCardColors = std::all_of(vCardColor.cbegin() + 1, vCardColor.cend(),
			[&](BYTE bColor) { return bColor == vCardColor.front(); });

		for (BYTE bCardNumber = 1; bCardNumber <= 6; ++bCardNumber)
		{
			std::vector<BYTE> vSearchNumber
			{
				static_cast<BYTE>(bCardNumber),
				static_cast<BYTE>(bCardNumber + 1),
				static_cast<BYTE>(bCardNumber + 2)
			};

			if (__ContainsNumbers(vCardNumber, vSearchNumber))
			{
				wScore = (bCardNumber * 10);
				break;
			}
		}

		if (wScore > 0 && bSameCardColors)
			wScore += 40;
	}

	if (wScore > 0)
	{
		m_mapField.clear();
		m_wScore += wScore;

		TPacketGCMiniGameRumiSetScore SetScorePacket(wScore, m_wScore);
		SendPacket(RUMI_GC_SUBHEADER_SET_SCORE, &SetScorePacket);

		return true;
	}

	return false;
}

INT CMiniGameRumi::__GetEmptyHandPosition() const
{
	if (m_mapHand.size() < RUMI_HAND_CARD_INDEX_MAX)
	{
		INT bPosition = 0;
		while (m_mapHand.count(bPosition) > 0)
			bPosition++;

		return bPosition;
	}
	return -1;
}

INT CMiniGameRumi::__GetEmptyFieldPosition() const
{
	if (m_mapField.size() < RUMI_FIELD_CARD_INDEX_MAX)
	{
		INT bPosition = 0;
		while (m_mapField.count(bPosition) > 0)
			bPosition++;

		return bPosition;
	}
	return -1;
}

/* static */ void CMiniGameRumi::StartGame(LPCHARACTER pChar)
{
	if (!IsActiveEvent())
	{
		EndGame(pChar);
		return;
	}

	if (pChar->GetMiniGameRumi())
		return;

	if (pChar->GetGold() < RUMI_START_GOLD)
	{
		pChar->ChatPacket(CHAT_TYPE_INFO, LC_STRING("You don't have enough Yang."));
		return;
	}

#if defined(__OKEY_EVENT_FLAG_RENEWAL__)
	const WORD wCardCount = pChar->GetQuestFlag("minigame_rumi.card_count");
	if (wCardCount == 0)
		return;

	pChar->SetQuestFlag("minigame_rumi.card_count", wCardCount - 1);
#else
	if (pChar->CountSpecifyItem(ITEM_VNUM_RUMI_CARD_PACK))
		return;

	pChar->RemoveSpecifyItem(ITEM_VNUM_RUMI_CARD_PACK, 1);
#endif
	pChar->PointChange(POINT_GOLD, -RUMI_START_GOLD, true);

	pChar->SetMiniGameRumi(new CMiniGameRumi(pChar));
	pChar->ChatPacket(CHAT_TYPE_INFO, LC_STRING("Your game of okey has started!"));
}

/* static */ void CMiniGameRumi::EndGame(LPCHARACTER pChar)
{
	CMiniGameRumi* pMiniGameRumi = pChar->GetMiniGameRumi();
	if (pMiniGameRumi)
		pMiniGameRumi->Reward();
}

/* static */ void CMiniGameRumi::Analyze(LPCHARACTER pChar, BYTE bSubHeader, BOOL bUseCard, BYTE bIndex)
{
	CMiniGameRumi* pMiniGameRumi = pChar->GetMiniGameRumi();
	if (pMiniGameRumi == nullptr)
	{
		sys_err("CMiniGameRumi::Analyze - Null MiniGameRumi! (ch: %s) (%d)",
			pChar->GetName(), bSubHeader);
		return;
	}

	switch (bSubHeader)
	{
		case RUMI_CG_SUBHEADER_DECK_CARD_CLICK:
		{
			if (pMiniGameRumi->GetDeckCount() == 0)
			{
				pChar->ChatPacket(CHAT_TYPE_INFO, LC_STRING("You cannot draw any more cards."));
				return;
			}

			if (pMiniGameRumi->GetEmptyHandPosition() == -1)
			{
				pChar->ChatPacket(CHAT_TYPE_INFO, LC_STRING("You cannot draw any more cards when you have already selected some."));
				return;
			}

#if defined(__RUMI_DEALER__)
			for (BYTE bIndex = 0; bIndex < RUMI_HAND_CARD_INDEX_MAX; ++bIndex)
				pMiniGameRumi->ClickDeckCard();
#else
			pMiniGameRumi->ClickDeckCard();
#endif
		}
		break;

		case RUMI_CG_SUBHEADER_HAND_CARD_CLICK:
			pMiniGameRumi->ClickHandCard(bUseCard, bIndex);
			break;

		case RUMI_CG_SUBHEADER_FIELD_CARD_CLICK:
			pMiniGameRumi->ClickFieldCard(bIndex);
			break;

		default:
			sys_err("CMiniGameRumi::Analyze - Unknown SubHeader (ch: %s) (%d)",
				pChar->GetName(), bSubHeader);
			break;
	}
}

/* static */ bool CMiniGameRumi::IsActiveEvent()
{
	if (quest::CQuestManager::instance().GetEventFlag("mini_game_okey"))
		return true;

	if (quest::CQuestManager::instance().GetEventFlag("mini_game_okey_normal"))
		return true;

	return false;
}

/* static */ void CMiniGameRumi::SpawnEventNPC(bool bSpawn)
{
	CharacterVectorInteractor vChar;
	if (CHARACTER_MANAGER::instance().GetCharactersByRaceNum(RUMI_TABLE, vChar))
	{
		std::for_each(vChar.begin(), vChar.end(),
			[](LPCHARACTER pChar) { M2_DESTROY_CHARACTER(pChar); });
	}

	if (bSpawn)
	{
		std::unordered_map<long, std::pair<long, long>> mSpawnPoint
		{
			{ MAP_A1, { 607, 619 } },
			{ MAP_B1, { 595, 613 } },
			{ MAP_C1, { 353, 741 } },
		};

		for (const auto& it : mSpawnPoint)
		{
			long lMapIndex = it.first;
			const auto& rkPos = it.second;

			if (!map_allow_find(lMapIndex))
				continue;

			const LPSECTREE_MAP pkSectreeMap = SECTREE_MANAGER::instance().GetMap(lMapIndex);
			if (pkSectreeMap == nullptr)
				continue;

			long xPos = pkSectreeMap->m_setting.iBaseX + rkPos.first * 100;
			long yPos = pkSectreeMap->m_setting.iBaseY + rkPos.second * 100;

			CHARACTER_MANAGER::instance().SpawnMob(RUMI_TABLE, lMapIndex,
				xPos, yPos, 0, false, 90, true);
		}
	}
}

/* static */ void CMiniGameRumi::RequestQuestFlag(LPCHARACTER pChar, BYTE bSubHeader)
{
	const LPDESC pkDesc = pChar->GetDesc();
	if (pkDesc == nullptr)
	{
		sys_err("CMiniGameRumi::RequestQuestFlag - Null DESC! (ch: %s) (%d)",
			pChar->GetName(), bSubHeader);
		return;
	}

	const WORD wCardPieceCount = pChar->GetQuestFlag("minigame_rumi.card_piece_count");
	const WORD wCardCount = pChar->GetQuestFlag("minigame_rumi.card_count");

	TPacketGCMiniGameRumi Packet;
	Packet.bHeader = HEADER_GC_MINI_GAME_RUMI;
	Packet.bSubHeader = bSubHeader;

	TPacketGCMiniGameRumiQuestFlag QuestFlagPacket(wCardPieceCount, wCardCount);
	Packet.wSize = sizeof(Packet) + sizeof(QuestFlagPacket);

	pkDesc->BufferedPacket(&Packet, sizeof(TPacketGCMiniGameRumi));
	pkDesc->Packet(&QuestFlagPacket, sizeof(TPacketGCMiniGameRumiQuestFlag));
}

/* static */ bool CMiniGameRumi::UpdateQuestFlag(LPCHARACTER pChar)
{
	if (!IsActiveEvent())
		return false;

	const WORD wCardPieceCount = pChar->GetQuestFlag("minigame_rumi.card_piece_count");
	const WORD wCardCount = pChar->GetQuestFlag("minigame_rumi.card_count");

	if (wCardCount >= RUMI_CARD_COUNT_MAX)
	{
		RequestQuestFlag(pChar, RUMI_GC_SUBHEADER_NO_MORE_GAIN);
		return false;
	}

	if (wCardPieceCount + 1 >= RUMI_CARD_PIECE_COUNT_MAX)
	{
		pChar->SetQuestFlag("minigame_rumi.card_piece_count", 0);
		pChar->SetQuestFlag("minigame_rumi.card_count", wCardCount + 1);

		RequestQuestFlag(pChar, RUMI_GC_SUBHEADER_SET_CARD_FLAG);
	}
	else
	{
		pChar->SetQuestFlag("minigame_rumi.card_piece_count", wCardPieceCount + 1);

		RequestQuestFlag(pChar, RUMI_GC_SUBHEADER_SET_CARD_PIECE_FLAG);
	}

	return true;
}

/* static */ void CMiniGameRumi::GetScoreTable(lua_State* L, bool bTotal)
{
	DWORD dwIndex = 1;
	lua_newtable(L);

	char szQuery[2048 + 1];
	int iLen = 0;

	iLen += sprintf(szQuery + iLen, "SELECT ");
	iLen += sprintf(szQuery + iLen, "`player`.`name`, ");
	iLen += bTotal ? sprintf(szQuery + iLen, "`total_score` ") : sprintf(szQuery + iLen, "`best_score` ");
	iLen += sprintf(szQuery + iLen, "FROM `minigame_rumi` ");
	iLen += sprintf(szQuery + iLen, "LEFT JOIN (SELECT `id`, `name` AS `name` FROM `player`) AS `player` ON `player`.`id` = `minigame_rumi`.`pid` ");
	iLen += bTotal ? sprintf(szQuery + iLen, "ORDER BY `total_score` DESC ") : sprintf(szQuery + iLen, "ORDER BY `best_score` DESC ");
	iLen += sprintf(szQuery + iLen, "LIMIT 10 ");

	if (iLen >= sizeof(szQuery))
		return;

	std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery(szQuery));
	SQLResult* pRes = pMsg->Get();
	if (pRes && pRes->uiNumRows > 0)
	{
		MYSQL_ROW RowData;
		while ((RowData = mysql_fetch_row(pRes->pSQLResult)))
		{
			if (RowData[0] == NULL || RowData[1] == NULL)
				continue;

			if (RowData[0][0] == '[') // GM
				continue;

			DWORD dwScore = 0;
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

/* static */ DWORD CMiniGameRumi::GetMyScoreValue(lua_State* L, DWORD dwPID, bool bTotal)
{
	char szQuery[2048 + 1];
	int iLen = 0;

	iLen += sprintf(szQuery + iLen, "SELECT ");
	iLen += bTotal ? sprintf(szQuery + iLen, "`total_score` ") : sprintf(szQuery + iLen, "`best_score` ");
	iLen += sprintf(szQuery + iLen, "FROM `minigame_rumi` WHERE `pid` = %u ", dwPID);

	std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery(szQuery));
	const SQLResult* pkRes = pMsg->Get();
	if (pkRes && pkRes->uiNumRows > 0)
	{
		MYSQL_ROW RowData = mysql_fetch_row(pkRes->pSQLResult);

		DWORD dwScore;
		str_to_number(dwScore, RowData[0]);

		return dwScore;
	}

	return 0;
}
#endif // __MINI_GAME_RUMI__
