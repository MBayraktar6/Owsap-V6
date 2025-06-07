/**
* Filename: minigame_yutnori.cpp
* Author: Owsap
**/

#include "stdafx.h"

#if defined(__MINI_GAME_YUTNORI__)
#include "minigame_yutnori.h"

#include "char.h"
#include "desc.h"
#include "char_manager.h"
#include "questmanager.h"
#include "sectree_manager.h"
#include "unique_item.h"
#include "db.h"
#include "config.h"

CMiniGameYutnori::CMiniGameYutnori(LPCHARACTER pChar) : m_pChar(pChar)
{
	m_bFirstThrow = true;
	m_bReThrow = false;

	m_wScore = YUTNORI_INIT_SCORE;
	m_bRemainCount = YUTNORI_INIT_REMAIN_COUNT;
	m_bRecvReward = false;
	m_bYutProb = YUTNORI_YUTSEM1;

	m_bPC = true;

	m_bPCYut = 0;
	memset(&m_bPCUnitPos, 0, sizeof(m_bPCUnitPos));
	memset(&m_bPCUnitLastPos, 0, sizeof(m_bPCUnitLastPos));

	m_bCOMYut = 0;
	memset(&m_bCOMUnitPos, 0, sizeof(m_bCOMUnitPos));
	memset(&m_bCOMUnitLastPos, 0, sizeof(m_bCOMUnitLastPos));
	m_bComNextUnitIndex = false;

	SendPacket(YUTNORI_GC_SUBHEADER_START);
}

CMiniGameYutnori::~CMiniGameYutnori()
{
	m_bFirstThrow = false;
	m_bReThrow = false;

	m_wScore = 0;
	m_bRemainCount = 0;
	m_bRecvReward = false;
	m_bYutProb = 0;

	m_bPC = false;

	m_bPCYut = 0;
	memset(&m_bPCUnitPos, 0, sizeof(m_bPCUnitPos));
	memset(&m_bPCUnitLastPos, 0, sizeof(m_bPCUnitLastPos));

	m_bCOMYut = 0;
	memset(&m_bCOMUnitPos, 0, sizeof(m_bCOMUnitPos));
	memset(&m_bCOMUnitLastPos, 0, sizeof(m_bCOMUnitLastPos));
	m_bComNextUnitIndex = false;

	SendPacket(YUTNORI_GC_SUBHEADER_STOP);
}

void CMiniGameYutnori::Giveup()
{
	m_pChar->SetMiniGameYutnori(nullptr);
}

void CMiniGameYutnori::SetProb(BYTE bProbIndex)
{
	if (bProbIndex >= YUTNORI_YUTSEM_MAX)
		return;

	m_bYutProb = bProbIndex;

	TPacketGCMiniGameYutnoriSetProb SetProbPacket(bProbIndex);
	SendPacket(YUTNORI_GC_SUBHEADER_SET_PROB, &SetProbPacket);
}

void CMiniGameYutnori::Throw(bool bPC)
{
	if (bPC && !m_bPC)
		return;

	if (!bPC && m_bPC)
		return;

	if (m_bFirstThrow)
	{
		BYTE bRandomYut = __GetRandomYut(false, true);

		TPacketGCMiniGameYutnoriThrow ThrowPacket(bPC, bRandomYut);
		SendPacket(YUTNORI_GC_SUBHEADER_THROW, &ThrowPacket);

		if (bPC)
		{
			m_bPC = false;
			m_bPCYut = bRandomYut;

			__PushNextTurn(false, YUTNORI_BEFORE_TURN_SELECT);
		}
		else
		{
			m_bFirstThrow = false;
			m_bPC = (m_bPCYut < bRandomYut);

			__PushNextTurn(m_bPC, YUTNORI_AFTER_TURN_SELECT);
		}
	}
	else
	{
		bool bReThrow = m_bReThrow;

		if (m_bReThrow)
			m_bReThrow = false;

		if (bPC && !bReThrow)
		{
			__UpdateScore(false, false);
			__UpdateRemainCount();
		}

		bool bExcludeYutSem6 = __IsExcludeYutSem6(bPC);
		BYTE bRandomYut = __GetRandomYut(bReThrow, bExcludeYutSem6, bPC ? m_bYutProb : YUTNORI_YUTSEM_MAX);

		// Avoid getting the same Yut again.
		BYTE bLastYut = bPC ? m_bPCYut : m_bCOMYut;
		while (bRandomYut == bLastYut)
		{
			bRandomYut = __GetRandomYut(bReThrow, bExcludeYutSem6, bPC ? m_bYutProb : YUTNORI_YUTSEM_MAX);
		}

		TPacketGCMiniGameYutnoriThrow ThrowPacket(bPC, bRandomYut);
		SendPacket(YUTNORI_GC_SUBHEADER_THROW, &ThrowPacket);

		if (bRandomYut == YUTNORI_YUTSEM6)
		{
			bool bCanMove = __CanMove(bPC);

			m_bPC = bPC ? bCanMove : !bCanMove;

			if (bPC)
				m_bPCYut = bRandomYut;
			else
				m_bCOMYut = bRandomYut;

			__PushNextTurn(bPC ? bCanMove : !bCanMove, bCanMove ? YUTNORI_STATE_MOVE : YUTNORI_STATE_THROW);
		}
		else
		{
			m_bPC = bPC;

			if (bPC)
				m_bPCYut = bRandomYut;
			else
				m_bCOMYut = bRandomYut;

			__PushNextTurn(bPC, YUTNORI_STATE_MOVE);
		}
	}
}

void CMiniGameYutnori::CharClick(BYTE bUnitIndex)
{
	if (!m_bPC)
		return;

	if (bUnitIndex >= YUTNORI_PLAYER_MAX)
		return;

	BYTE bStartIndex = m_bPCUnitPos[bUnitIndex];
	BYTE bDestIndex = bStartIndex;
	BYTE bLastIndex = m_bPCUnitLastPos[bUnitIndex];

	if (bStartIndex == YUTNORI_GOAL_AREA)
		return;

	__GetDestPos(m_bPCYut, &bStartIndex, &bDestIndex, &bLastIndex);

	TPacketGCMiniGameYutnoriAvailableArea AvailableAreaPacket(bUnitIndex, bDestIndex);
	SendPacket(YUTNORI_GC_SUBHEADER_AVAILABLE_AREA, &AvailableAreaPacket);
}

void CMiniGameYutnori::Move(BYTE bUnitIndex)
{
	if (!m_bPC)
		return;

	if (bUnitIndex >= YUTNORI_PLAYER_MAX)
		return;

	BYTE bStartIndex = m_bPCUnitPos[bUnitIndex];
	BYTE bDestIndex = bStartIndex;
	BYTE bLastIndex = m_bPCUnitLastPos[bUnitIndex];

	if (bStartIndex == YUTNORI_GOAL_AREA)
		return;

	__GetDestPos(m_bPCYut, &bStartIndex, &bDestIndex, &bLastIndex);

	bool bIsCatch = false;
	{
		for (BYTE bUnitIdx = 0; bUnitIdx < YUTNORI_PLAYER_MAX; ++bUnitIdx)
		{
			if (m_bCOMUnitPos[bUnitIdx] == bDestIndex && bDestIndex != 0 && bDestIndex != YUTNORI_GOAL_AREA)
			{
				__UpdateScore(true, false);

				m_bCOMUnitPos[bUnitIdx] = 0;
				m_bCOMUnitLastPos[bUnitIdx] = 0;

				TPacketGCMiniGameYutnoriPushCatchYut PushCatchYutPacket(false, bUnitIdx);
				SendPacket(YUTNORI_GC_SUBHEADER_PUSH_CATCH_YUT, &PushCatchYutPacket);

				bIsCatch = true;
			}
		}
	}

	TPacketGCMiniGameYutnoriMove MovePacket(true, bUnitIndex, bIsCatch, bStartIndex, bDestIndex);
	SendPacket(YUTNORI_GC_SUBHEADER_MOVE, &MovePacket);

	if (__IsDouble(m_bPCUnitPos))
	{
		if (bDestIndex == YUTNORI_GOAL_AREA)
			__UpdateScore(true, true);

		for (BYTE bUnitIndx = 0; bUnitIndx < YUTNORI_PLAYER_MAX; ++bUnitIndx)
		{
			m_bPCUnitPos[bUnitIndx] = bDestIndex;
			m_bPCUnitLastPos[bUnitIndx] = bLastIndex;
		}
	}
	else
	{
		if (bDestIndex == YUTNORI_GOAL_AREA)
			__UpdateScore(true, false);

		m_bPCUnitPos[bUnitIndex] = bDestIndex;
		m_bPCUnitLastPos[bUnitIndex] = bLastIndex;
	}

	if (__IsGoalArea(m_bPCUnitPos))
	{
		m_bRecvReward = true;
		__PushNextTurn(true, YUTNORI_STATE_END);
	}
	else
	{
		if (m_bRemainCount == 0 || m_wScore == 0)
		{
			if (m_wScore == 0)
				m_bRecvReward = false;
			else
				m_bRecvReward = true;

			__PushNextTurn(false, YUTNORI_STATE_END);
		}
		else
		{
			if (m_bPCYut == YUTNORI_YUTSEM4 || m_bPCYut == YUTNORI_YUTSEM5)
			{
				m_bReThrow = true;
				__PushNextTurn(true, YUTNORI_STATE_RE_THROW);
			}
			else
			{
				m_bPC = false;
				__PushNextTurn(false, YUTNORI_STATE_THROW);
			}
		}
	}
}

void CMiniGameYutnori::RequestComAction()
{
	if (m_bPC)
		return;

	BYTE bUnitIndex = __GetComUnitIndex();
	if (bUnitIndex == YUTNORI_PLAYER_MAX)
		return;

	m_bComNextUnitIndex = bUnitIndex ? 0 : 1;

	BYTE bStartIndex = m_bCOMUnitPos[bUnitIndex];
	BYTE bDestIndex = bStartIndex;
	BYTE bLastIndex = m_bCOMUnitLastPos[bUnitIndex];

	if (bStartIndex == YUTNORI_GOAL_AREA)
		return;

	__GetDestPos(m_bCOMYut, &bStartIndex, &bDestIndex, &bLastIndex);

	bool bIsCatch = false;
	{
		for (BYTE bUnitIdx = 0; bUnitIdx < YUTNORI_PLAYER_MAX; ++bUnitIdx)
		{
			if (m_bPCUnitPos[bUnitIdx] == bDestIndex && bDestIndex != 0 && bDestIndex != YUTNORI_GOAL_AREA)
			{
				__UpdateScore(false, false);

				m_bPCUnitPos[bUnitIdx] = 0;
				m_bPCUnitLastPos[bUnitIdx] = 0;

				TPacketGCMiniGameYutnoriPushCatchYut PushCatchYutPacket(true, bUnitIdx);
				SendPacket(YUTNORI_GC_SUBHEADER_PUSH_CATCH_YUT, &PushCatchYutPacket);

				bIsCatch = true;
			}
		}
	}

	TPacketGCMiniGameYutnoriMove MovePacket(false, bUnitIndex, bIsCatch, bStartIndex, bDestIndex);
	SendPacket(YUTNORI_GC_SUBHEADER_MOVE, &MovePacket);

	if (__IsDouble(m_bCOMUnitPos))
	{
		if (bDestIndex == YUTNORI_GOAL_AREA)
			__UpdateScore(false, true);

		for (BYTE bUnitIndx = 0; bUnitIndx < YUTNORI_PLAYER_MAX; ++bUnitIndx)
		{
			m_bCOMUnitPos[bUnitIndx] = bDestIndex;
			m_bCOMUnitLastPos[bUnitIndx] = bLastIndex;
		}
	}
	else
	{
		if (bDestIndex == YUTNORI_GOAL_AREA)
			__UpdateScore(false, false);

		m_bCOMUnitPos[bUnitIndex] = bDestIndex;
		m_bCOMUnitLastPos[bUnitIndex] = bLastIndex;
	}

	if (__IsGoalArea(m_bCOMUnitPos))
	{
		m_bRecvReward = true;
		__PushNextTurn(false, YUTNORI_STATE_END);
	}
	else
	{
		if (m_bCOMYut == YUTNORI_YUTSEM4 || m_bCOMYut == YUTNORI_YUTSEM5)
		{
			m_bReThrow = true;
			__PushNextTurn(false, YUTNORI_STATE_RE_THROW);
		}
		else
		{
			m_bPC = true;
			__PushNextTurn(true, YUTNORI_STATE_THROW);
		}
	}
}

void CMiniGameYutnori::Reward()
{
	if (!m_bRecvReward)
		return;

	m_bRecvReward = false;

	const WORD wScore = m_wScore;
	if (wScore < YUTNORI_LOW_TOTAL_SCORE)
	{
#if defined(__WJ_PICKUP_ITEM_EFFECT__)
		m_pChar->AutoGiveItem(YUTNORI_LOW_REWARD, 1, -1, true, true);
#else
		m_pChar->AutoGiveItem(YUTNORI_LOW_REWARD, 1, -1, true);
#endif
	}
	else if (wScore >= YUTNORI_LOW_TOTAL_SCORE && wScore < YUTNORI_MID_TOTAL_SCORE)
	{
#if defined(__WJ_PICKUP_ITEM_EFFECT__)
		m_pChar->AutoGiveItem(YUTNORI_MID_REWARD, 1, -1, true, true);
#else
		m_pChar->AutoGiveItem(YUTNORI_MID_REWARD, 1, -1, true);
#endif
	}
	else if (wScore >= YUTNORI_MID_TOTAL_SCORE)
	{
#if defined(__WJ_PICKUP_ITEM_EFFECT__)
		m_pChar->AutoGiveItem(YUTNORI_HIGH_REWARD, 1, -1, true, true);
#else
		m_pChar->AutoGiveItem(YUTNORI_HIGH_REWARD, 1, -1, true);
#endif
	}

	__UpdateMyScore(m_pChar->GetPlayerID());

	m_pChar->SetMiniGameYutnori(nullptr);
}

void CMiniGameYutnori::__UpdateScore(bool bIncrease, bool bIsDouble)
{
	if (bIncrease)
	{
		if (bIsDouble)
			m_wScore = MAX(0, m_wScore + (YUTNORI_IN_DE_CREASE_SCORE * 2));
		else
			m_wScore = MAX(0, m_wScore + YUTNORI_IN_DE_CREASE_SCORE);
	}
	else
	{
		if (bIsDouble)
			m_wScore = MAX(0, m_wScore - (YUTNORI_IN_DE_CREASE_SCORE * 2));
		else
			m_wScore = MAX(0, m_wScore - YUTNORI_IN_DE_CREASE_SCORE);
	}

	TPacketGCMiniGameYutnoriSetScore SetScorePacket(m_wScore);
	SendPacket(YUTNORI_GC_SUBHEADER_SET_SCORE, &SetScorePacket);
}

void CMiniGameYutnori::__UpdateRemainCount()
{
	m_bRemainCount = MAX(0, m_bRemainCount - 1);

	TPacketGCMiniGameYutnoriSetRemainCount SetRemainCountPacket(m_bRemainCount);
	SendPacket(YUTNORI_GC_SUBHEADER_SET_REMAIN_COUNT, &SetRemainCountPacket);
}

bool CMiniGameYutnori::__IsGoalArea(BYTE* pbUnitPos)
{
	bool bIsGoalArea = false;
	for (BYTE bUnitIndex = 0; bUnitIndex < YUTNORI_PLAYER_MAX; ++bUnitIndex)
	{
		if (pbUnitPos[bUnitIndex] == pbUnitPos[(bUnitIndex + 1) % YUTNORI_PLAYER_MAX] &&
			pbUnitPos[bUnitIndex] == YUTNORI_GOAL_AREA)
		{
			bIsGoalArea = true;
			break;
		}
	}

	return bIsGoalArea;
}

bool CMiniGameYutnori::__IsDouble(BYTE* pbUnitPos)
{
	bool bIsDouble = false;
	for (BYTE bUnitIndex = 0; bUnitIndex < YUTNORI_PLAYER_MAX; ++bUnitIndex)
	{
		if (pbUnitPos[bUnitIndex] == pbUnitPos[(bUnitIndex + 1) % YUTNORI_PLAYER_MAX] &&
			pbUnitPos[bUnitIndex] != 0 && pbUnitPos[bUnitIndex] != YUTNORI_GOAL_AREA)
		{
			bIsDouble = true;
			break;
		}
	}

	return bIsDouble;
}

bool CMiniGameYutnori::__CanMove(bool bPC)
{
	const BYTE* pUnit = bPC ? m_bPCUnitPos : m_bCOMUnitPos;
	bool bCanMove = true;

	for (BYTE bUnitIndex = 0; bUnitIndex < YUTNORI_PLAYER_MAX; ++bUnitIndex)
	{
		if (pUnit[bUnitIndex] == 0 || pUnit[bUnitIndex] == 11)
		{
			bCanMove = false;
			break;
		}
	}

	return bCanMove;
}

bool CMiniGameYutnori::__IsExcludeYutSem6(bool bPC)
{
	const BYTE* pUnit = bPC ? m_bPCUnitPos : m_bCOMUnitPos;
	bool bExcludeYutSem6 = false;

	for (BYTE bUnitIndex = 0; bUnitIndex < YUTNORI_PLAYER_MAX; ++bUnitIndex)
	{
		if (pUnit[bUnitIndex] == YUTNORI_GOAL_AREA || pUnit[bUnitIndex] == YUTNORI_GOAL_AREA - 1)
		{
			bExcludeYutSem6 = true;
			break;
		}
	}

	return bExcludeYutSem6;
}

BYTE CMiniGameYutnori::__GetComUnitIndex()
{
	BYTE bStartIndexUnit1 = m_bCOMUnitPos[0];
	BYTE bStartIndexUnit2 = m_bCOMUnitPos[1];

	BYTE bDestIndexUnit1 = bStartIndexUnit1;
	BYTE bDestIndexUnit2 = bStartIndexUnit2;

	BYTE bLastIndexUnit1 = m_bCOMUnitLastPos[0];
	BYTE bLastIndexUnit2 = m_bCOMUnitLastPos[1];

	// Check if both units are in the goal area.
	if (bStartIndexUnit1 == YUTNORI_GOAL_AREA && bStartIndexUnit2 == YUTNORI_GOAL_AREA)
		return YUTNORI_PLAYER_MAX;

	// Check if both units start at the same position.
	if (bStartIndexUnit1 == bStartIndexUnit2)
		return 0;

	// Check conditions for excluding units based on YUTNORI_YUTSEM6.
	bool bExcludeUnit1 = (m_bCOMYut == YUTNORI_YUTSEM6 && bStartIndexUnit1 == 0) || (bStartIndexUnit1 == YUTNORI_GOAL_AREA);
	bool bExcludeUnit2 = (m_bCOMYut == YUTNORI_YUTSEM6 && bStartIndexUnit2 == 0) || (bStartIndexUnit2 == YUTNORI_GOAL_AREA);

	// Determine the destination indices for each unit.
	for (BYTE bUnitIndex = 0; bUnitIndex < YUTNORI_PLAYER_MAX; ++bUnitIndex)
	{
		BYTE& bStartIndex = (bUnitIndex == 0) ? bStartIndexUnit1 : bStartIndexUnit2;
		BYTE& bDestIndex = (bUnitIndex == 0) ? bDestIndexUnit1 : bDestIndexUnit2;
		BYTE& bLastIndex = (bUnitIndex == 0) ? bLastIndexUnit1 : bLastIndexUnit2;

		__GetDestPos(m_bCOMYut, &bStartIndex, &bDestIndex, &bLastIndex);
	}

	// Check if each unit catches any PC unit.
	bool bCatchUnit1 = (bDestIndexUnit1 == m_bPCUnitPos[0] || bDestIndexUnit1 == m_bPCUnitPos[1]);
	bool bCatchUnit2 = (bDestIndexUnit2 == m_bPCUnitPos[0] || bDestIndexUnit2 == m_bPCUnitPos[1]);

	// Determine the computer's next unit index based on the conditions.
	if (bCatchUnit1 && !bExcludeUnit1)
		return 0;

	if (bCatchUnit2 && !bExcludeUnit2)
		return 1;

	if (m_bComNextUnitIndex == 0 && bExcludeUnit1 && !bExcludeUnit2)
		return 1;

	if (m_bComNextUnitIndex == 1 && !bExcludeUnit1 && bExcludeUnit2)
		return 0;

	if (bExcludeUnit1 && bExcludeUnit2)
		return YUTNORI_PLAYER_MAX;

	return m_bComNextUnitIndex;
}

BYTE CMiniGameYutnori::__GetRandomYut(BYTE bReThrow, BYTE bExcludeYutSem6, BYTE bYutProb)
{
	std::vector<BYTE> vYutIndexes;
	for (BYTE bYutIndex = YUTNORI_YUTSEM1; bYutIndex < YUTNORI_YUTSEM_MAX; ++bYutIndex)
	{
		// Avoid getting YUTSEM4 or YUTSEM5 if it's a re-throw.
		if (bReThrow && (bYutIndex == YUTNORI_YUTSEM4 || bYutIndex == YUTNORI_YUTSEM5))
			continue;

		if (bExcludeYutSem6 && bYutIndex == YUTNORI_YUTSEM6)
			continue;

		// Add an additional chance for the selected Yut.
		if (bYutIndex == bYutProb)
			vYutIndexes.push_back(bYutIndex);

		vYutIndexes.push_back(bYutIndex);
	}

	BYTE bRandomIndex = number(0, vYutIndexes.size() - 1);
	return vYutIndexes[bRandomIndex];
}

void CMiniGameYutnori::__GetDestPos(BYTE bMoveCount, BYTE* bStartIndex, BYTE* bDestIndex, BYTE* bLastIndex)
{
	if (*bStartIndex == 0 || *bDestIndex == 0 || *bLastIndex == 0)
		*bStartIndex = YUTNORI_GOAL_AREA;

	*bDestIndex = *bStartIndex;

	BYTE bNextIndex = *bDestIndex;
	BYTE bPrevIndex = *bDestIndex;

	if (bMoveCount == YUTNORI_YUTSEM6)
	{
		if (*bDestIndex == 20 || *bDestIndex == 21)
			*bDestIndex = 1;
		else if (*bDestIndex == 26 || *bDestIndex == 5)
			*bDestIndex = 6;
		else if (*bDestIndex == 28 || *bDestIndex == 24)
			*bDestIndex = 23;
		else if (*bDestIndex == 23)
			*bDestIndex = (*bDestIndex == 23 && *bLastIndex == 27) ? 27 : 22;
		else if (*bDestIndex == 16)
			*bDestIndex = (*bDestIndex == 16 || *bLastIndex == 29) ? 29 : 17;
		else
			*bDestIndex = (*bDestIndex <= 20) ? *bDestIndex + 1 : *bDestIndex - 1;
	}
	else
	{
		for (BYTE bIndex = 0; bIndex < bMoveCount + 1; ++bIndex)
		{
			if (*bDestIndex == 1)
			{
				bPrevIndex = *bDestIndex;
				*bDestIndex = (bNextIndex == 1) ? 21 : 20;
				*bLastIndex = bPrevIndex;
				continue;
			}
			else if (*bDestIndex == 6)
			{
				bPrevIndex = *bDestIndex;
				*bDestIndex = (bNextIndex == 6) ? 26 : 5;
				*bLastIndex = bPrevIndex;
				continue;
			}
			else if (*bDestIndex == 23)
			{
				bPrevIndex = *bDestIndex;
				*bDestIndex = (bNextIndex == 23 || *bLastIndex == 22) ? 24 : 28;
				*bLastIndex = bPrevIndex;
				continue;
			}
			else if (*bDestIndex == 27)
			{
				bPrevIndex = *bDestIndex;
				*bDestIndex = 23;
				*bLastIndex = bPrevIndex;
				continue;
			}
			else if (*bDestIndex == 29)
			{
				bPrevIndex = *bDestIndex;
				*bDestIndex = 16;
				*bLastIndex = bPrevIndex;
				continue;
			}
			else if (*bDestIndex == 25 || *bDestIndex == 12)
			{
				bPrevIndex = *bDestIndex;
				*bDestIndex = 11;
				*bLastIndex = bPrevIndex;
				break;
			}
			else
			{
				bPrevIndex = *bDestIndex;
				*bDestIndex = (*bDestIndex <= 20) ? *bDestIndex - 1 : *bDestIndex + 1;
				*bLastIndex = bPrevIndex;
				continue;
			}
		}
	}
}

void CMiniGameYutnori::__PushNextTurn(bool bPC, BYTE bState)
{
	TPacketGCMiniGameYutnoriPushNextTurn PushNextTurnPacket(bPC, bState);
	SendPacket(YUTNORI_GC_SUBHEADER_PUSH_NEXT_TURN, &PushNextTurnPacket);
}

void CMiniGameYutnori::SendPacket(BYTE bSubHeader, void* pvData)
{
	if (m_pChar == nullptr)
		return;

	const LPDESC pkDesc = m_pChar->GetDesc();
	if (pkDesc == nullptr)
	{
		sys_err("CMiniGameYutnori::SendPacket - Null DESC! (ch: %s) (%d)",
			m_pChar->GetName(), bSubHeader);
		return;
	}

	TPacketGCMiniGameYutnori Packet;
	Packet.bHeader = HEADER_GC_MINI_GAME_YUTNORI;
	Packet.bSubHeader = bSubHeader;

	const std::unordered_set<BYTE> EmptyData
	{
		YUTNORI_GC_SUBHEADER_START,
		YUTNORI_GC_SUBHEADER_STOP,
	};

	if (pvData == nullptr && EmptyData.find(bSubHeader) == EmptyData.end())
	{
		sys_err("CMiniGameYutnori::SendPacket(bSubHeader=%u) Null Data! Character: %s",
			bSubHeader, m_pChar->GetName());
		return;
	}

	switch (bSubHeader)
	{
		case YUTNORI_GC_SUBHEADER_START:
		case YUTNORI_GC_SUBHEADER_STOP:
		{
			Packet.wSize = sizeof(Packet);
			pkDesc->Packet(&Packet, sizeof(TPacketGCMiniGameYutnori));
		}
		break;

		case YUTNORI_GC_SUBHEADER_SET_PROB:
		{
			const TPacketGCMiniGameYutnoriSetProb* pkData = static_cast<TPacketGCMiniGameYutnoriSetProb*>(pvData);
			if (pkData == nullptr)
			{
				sys_err("CMiniGameYutnori::SendPacket(bSubHeader=%u) Null SetProbPacket! Character: %s",
					bSubHeader, m_pChar->GetName());
				return;
			}

			TPacketGCMiniGameYutnoriSetProb SetProbPacket(pkData->bProbIndex);
			Packet.wSize = sizeof(Packet) + sizeof(SetProbPacket);

			pkDesc->BufferedPacket(&Packet, sizeof(TPacketGCMiniGameYutnori));
			pkDesc->Packet(&SetProbPacket, sizeof(TPacketGCMiniGameYutnoriSetProb));
		}
		break;

		case YUTNORI_GC_SUBHEADER_THROW:
		{
			const TPacketGCMiniGameYutnoriThrow* pkData = static_cast<TPacketGCMiniGameYutnoriThrow*>(pvData);
			if (pkData == nullptr)
			{
				sys_err("CMiniGameYutnori::SendPacket(bSubHeader=%u) Null ThrowYutPacket! Character: %s",
					bSubHeader, m_pChar->GetName());
				return;
			}

			TPacketGCMiniGameYutnoriThrow ThrowPacket(pkData->bPC, pkData->bYut);
			Packet.wSize = sizeof(Packet) + sizeof(ThrowPacket);

			pkDesc->BufferedPacket(&Packet, sizeof(TPacketGCMiniGameYutnori));
			pkDesc->Packet(&ThrowPacket, sizeof(TPacketGCMiniGameYutnoriThrow));
		}
		break;

		case YUTNORI_GC_SUBHEADER_MOVE:
		{
			const TPacketGCMiniGameYutnoriMove* pkData = static_cast<TPacketGCMiniGameYutnoriMove*>(pvData);
			if (pkData == nullptr)
			{
				sys_err("CMiniGameYutnori::SendPacket(bSubHeader=%u) Null MoveYutPacket! Character: %s",
					bSubHeader, m_pChar->GetName());
				return;
			}

			TPacketGCMiniGameYutnoriMove MovePacket(pkData->bPC, pkData->bUnitIndex, pkData->bIsCatch, pkData->bStartIndex, pkData->bDestIndex);
			Packet.wSize = sizeof(Packet) + sizeof(MovePacket);

			pkDesc->BufferedPacket(&Packet, sizeof(TPacketGCMiniGameYutnori));
			pkDesc->Packet(&MovePacket, sizeof(TPacketGCMiniGameYutnoriMove));
		}
		break;

		case YUTNORI_GC_SUBHEADER_AVAILABLE_AREA:
		{
			const TPacketGCMiniGameYutnoriAvailableArea* pkData = static_cast<TPacketGCMiniGameYutnoriAvailableArea*>(pvData);
			if (pkData == nullptr)
			{
				sys_err("CMiniGameYutnori::SendPacket(bSubHeader=%u) Null AvailAreaPacket! Character: %s",
					bSubHeader, m_pChar->GetName());
				return;
			}

			TPacketGCMiniGameYutnoriAvailableArea AvailAreaPacket(pkData->bPlayerIndex, pkData->bAvailableIndex);
			Packet.wSize = sizeof(Packet) + sizeof(AvailAreaPacket);

			pkDesc->BufferedPacket(&Packet, sizeof(TPacketGCMiniGameYutnori));
			pkDesc->Packet(&AvailAreaPacket, sizeof(TPacketGCMiniGameYutnoriAvailableArea));
		}
		break;

		case YUTNORI_GC_SUBHEADER_PUSH_CATCH_YUT:
		{
			const TPacketGCMiniGameYutnoriPushCatchYut* pkData = static_cast<TPacketGCMiniGameYutnoriPushCatchYut*>(pvData);
			if (pkData == nullptr)
			{
				sys_err("CMiniGameYutnori::SendPacket(bSubHeader=%u) Null PushCatchYutPacket! Character: %s",
					bSubHeader, m_pChar->GetName());
				return;
			}

			TPacketGCMiniGameYutnoriPushCatchYut PushCatchYutPacket(pkData->bPC, pkData->bUnitIndex);
			Packet.wSize = sizeof(Packet) + sizeof(PushCatchYutPacket);

			pkDesc->BufferedPacket(&Packet, sizeof(TPacketGCMiniGameYutnori));
			pkDesc->Packet(&PushCatchYutPacket, sizeof(TPacketGCMiniGameYutnoriPushCatchYut));
		}
		break;

		case YUTNORI_GC_SUBHEADER_SET_SCORE:
		{
			const TPacketGCMiniGameYutnoriSetScore* pkData = static_cast<TPacketGCMiniGameYutnoriSetScore*>(pvData);
			if (pkData == nullptr)
			{
				sys_err("CMiniGameYutnori::SendPacket(bSubHeader=%u) Null SetScorePacket! Character: %s",
					bSubHeader, m_pChar->GetName());
				return;
			}

			TPacketGCMiniGameYutnoriSetScore SetScorePacket(pkData->wScore);
			Packet.wSize = sizeof(Packet) + sizeof(SetScorePacket);

			pkDesc->BufferedPacket(&Packet, sizeof(TPacketGCMiniGameYutnori));
			pkDesc->Packet(&SetScorePacket, sizeof(TPacketGCMiniGameYutnoriSetScore));
		}
		break;

		case YUTNORI_GC_SUBHEADER_SET_REMAIN_COUNT:
		{
			const TPacketGCMiniGameYutnoriSetRemainCount* pkData = static_cast<TPacketGCMiniGameYutnoriSetRemainCount*>(pvData);
			if (pkData == nullptr)
			{
				sys_err("CMiniGameYutnori::SendPacket(bSubHeader=%u) Null SetRemainCountPacket! Character: %s",
					bSubHeader, m_pChar->GetName());
				return;
			}

			TPacketGCMiniGameYutnoriSetRemainCount SetRemainCountPacket(pkData->bRemainCount);
			Packet.wSize = sizeof(Packet) + sizeof(SetRemainCountPacket);

			pkDesc->BufferedPacket(&Packet, sizeof(TPacketGCMiniGameYutnori));
			pkDesc->Packet(&SetRemainCountPacket, sizeof(TPacketGCMiniGameYutnoriSetRemainCount));
		}
		break;

		case YUTNORI_GC_SUBHEADER_PUSH_NEXT_TURN:
		{
			const TPacketGCMiniGameYutnoriPushNextTurn* pkData = static_cast<TPacketGCMiniGameYutnoriPushNextTurn*>(pvData);
			if (pkData == nullptr)
			{
				sys_err("CMiniGameYutnori::SendPacket(bSubHeader=%u) Null PushNextTurnPacket! Character: %s",
					bSubHeader, m_pChar->GetName());
				return;
			}

			TPacketGCMiniGameYutnoriPushNextTurn PushNextTurnPacket(pkData->bPC, pkData->bState);
			Packet.wSize = sizeof(Packet) + sizeof(PushNextTurnPacket);

			pkDesc->BufferedPacket(&Packet, sizeof(TPacketGCMiniGameYutnori));
			pkDesc->Packet(&PushNextTurnPacket, sizeof(TPacketGCMiniGameYutnoriPushNextTurn));
		}
		break;
	}
}

void CMiniGameYutnori::__UpdateMyScore(DWORD dwPID)
{
	char szQuery[2048 + 1];
	int iLen = sprintf(szQuery, "INSERT INTO `player`.`minigame_yutnori` (`pid`, `best_score`, `total_score`, `last_play`) ");
	iLen += sprintf(szQuery + iLen, "VALUES (%u, %u, %u, FROM_UNIXTIME(%d)) ", dwPID, m_wScore, m_wScore, std::time(nullptr));
	iLen += sprintf(szQuery + iLen, "ON DUPLICATE KEY UPDATE ");
	iLen += sprintf(szQuery + iLen, "`total_score` = `total_score` + %d,", m_wScore);
	iLen += sprintf(szQuery + iLen, "`best_score` = if (%d > `best_score`, %d, `best_score`) ", m_wScore, m_wScore);
	std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery(szQuery));
}

//////////////////////////////////////////////////////////////////////////

/* static */ void CMiniGameYutnori::Create(LPCHARACTER pChar)
{
	if (!IsActiveEvent())
	{
		Destroy(pChar);
		return;
	}

	if (pChar->GetMiniGameYutnori())
		return;

	if (pChar->GetGold() < YUTNORI_START_GOLD)
		return;

#if defined(__YUTNORI_EVENT_FLAG_RENEWAL__)
	const WORD wYutBoardCount = pChar->GetQuestFlag("minigame_yutnori.board_count");
	if (wYutBoardCount == 0)
		return;

	pChar->SetQuestFlag("minigame_yutnori.board_count", wYutBoardCount - 1);
#else
	if (pChar->CountSpecifyItem(ITEM_VNUM_YUT_BOARD))
		return;

	pChar->RemoveSpecifyItem(ITEM_VNUM_YUT_BOARD, 1);
#endif
	pChar->PointChange(POINT_GOLD, -YUTNORI_START_GOLD, true);

	pChar->SetMiniGameYutnori(new CMiniGameYutnori(pChar));
}

/* static */ void CMiniGameYutnori::Destroy(LPCHARACTER pChar)
{
	pChar->SetMiniGameYutnori(nullptr);
}

/* static */ void CMiniGameYutnori::Analyze(LPCHARACTER pChar, BYTE bSubHeader, BYTE bArgument)
{
	CMiniGameYutnori* pMiniGameYutnori = pChar->GetMiniGameYutnori();
	if (pMiniGameYutnori == nullptr)
	{
		sys_err("CMiniGameYutnori::Analyze - Null MiniGameYutnori! (ch: %s) (%d)",
			pChar->GetName(), bSubHeader);
		return;
	}

	switch (bSubHeader)
	{
		case YUTNORI_CG_SUBHEADER_SET_PROB:
			pMiniGameYutnori->SetProb(bArgument);
			break;

		case YUTNORI_CG_SUBHEADER_CLICK_CHAR:
			pMiniGameYutnori->CharClick(bArgument);
			break;

		case YUTNORI_CG_SUBHEADER_THROW:
			pMiniGameYutnori->Throw(bArgument);
			break;

		case YUTNORI_CG_SUBHEADER_MOVE:
			pMiniGameYutnori->Move(bArgument);
			break;

		case YUTNORI_CG_SUBHEADER_REQUEST_COM_ACTION:
			pMiniGameYutnori->RequestComAction();
			break;

		case YUTNORI_CG_SUBHEADER_REWARD:
			pMiniGameYutnori->Reward();
			break;

		default:
			sys_err("CMiniGameYutnori::Analyze - Unknown SubHeader (ch: %s) (%d)",
				pChar->GetName(), bSubHeader);
			break;
	}
}

/* static */ bool CMiniGameYutnori::IsActiveEvent()
{
	if (quest::CQuestManager::instance().GetEventFlag("mini_game_yutnori"))
		return true;

	return false;
}

/* static */ void CMiniGameYutnori::SpawnEventNPC(bool bSpawn)
{
	CharacterVectorInteractor vChar;
	if (CHARACTER_MANAGER::instance().GetCharactersByRaceNum(YUTNORI_TABLE, vChar))
	{
		std::for_each(vChar.begin(), vChar.end(),
			[](LPCHARACTER pChar) { M2_DESTROY_CHARACTER(pChar); });
	}

	if (bSpawn)
	{
		std::unordered_map<long, std::pair<long, long>> mSpawnPoint
		{
			{ MAP_A1, { 608, 614 } },
			{ MAP_B1, { 596, 608 } },
			{ MAP_C1, { 358, 748 } },
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

			CHARACTER_MANAGER::instance().SpawnMob(YUTNORI_TABLE, lMapIndex,
				xPos, yPos, 0, false, 90, true);
		}
	}
}

#if defined(__YUTNORI_EVENT_FLAG_RENEWAL__)
/* static */ void CMiniGameYutnori::RequestQuestFlag(LPCHARACTER pChar, BYTE bSubHeader)
{
	const LPDESC pDesc = pChar->GetDesc();
	if (pDesc == nullptr)
	{
		sys_err("CMiniGameYutnori::RequestQuestFlag - Null DESC! (ch: %s) (%d)",
			pChar->GetName(), bSubHeader);
		return;
	}

	const WORD wYutPieceCount = pChar->GetQuestFlag("minigame_yutnori.piece_count");
	const WORD wYutBoardCount = pChar->GetQuestFlag("minigame_yutnori.board_count");

	TPacketGCMiniGameYutnori Packet;
	Packet.bHeader = HEADER_GC_MINI_GAME_YUTNORI;
	Packet.bSubHeader = bSubHeader;

	TPacketGCMiniGameYutnoriQuestFlag QuestFlagPacket(wYutPieceCount, wYutBoardCount);
	Packet.wSize = sizeof(Packet) + sizeof(QuestFlagPacket);

	pDesc->BufferedPacket(&Packet, sizeof(TPacketGCMiniGameYutnori));
	pDesc->Packet(&QuestFlagPacket, sizeof(TPacketGCMiniGameYutnoriQuestFlag));
}

/* static */ bool CMiniGameYutnori::UpdateQuestFlag(LPCHARACTER pChar)
{
	if (!IsActiveEvent())
		return false;

	const WORD wYutPieceCount = pChar->GetQuestFlag("minigame_yutnori.piece_count");
	const WORD wYutBoardCount = pChar->GetQuestFlag("minigame_yutnori.board_count");

	if (wYutBoardCount >= YUTNORI_BOARD_COUNT_MAX)
	{
		RequestQuestFlag(pChar, YUTNORI_GC_SUBHEADER_NO_MORE_GAIN);
		return false;
	}

	if (wYutPieceCount + 1 >= YUTNORI_PIECE_COUNT_MAX)
	{
		pChar->SetQuestFlag("minigame_yutnori.piece_count", 0);
		pChar->SetQuestFlag("minigame_yutnori.board_count", wYutBoardCount + 1);

		RequestQuestFlag(pChar, YUTNORI_GC_SUBHEADER_SET_YUT_BOARD_FLAG);
	}
	else
	{
		pChar->SetQuestFlag("minigame_yutnori.piece_count", wYutPieceCount + 1);

		RequestQuestFlag(pChar, YUTNORI_GC_SUBHEADER_SET_YUT_PIECE_FLAG);
	}

	return true;
}
#endif

/* static */ void CMiniGameYutnori::GetScoreTable(lua_State* L, bool bTotal)
{
	DWORD dwIndex = 1;
	lua_newtable(L);

	char szQuery[2048 + 1];
	int iLen = 0;

	iLen += sprintf(szQuery + iLen, "SELECT ");
	iLen += sprintf(szQuery + iLen, " `player`.`name`, ");
	iLen += bTotal ? sprintf(szQuery + iLen, "`total_score` ") : sprintf(szQuery + iLen, "`best_score` ");
	iLen += sprintf(szQuery + iLen, "FROM `minigame_yutnori` ");
	iLen += sprintf(szQuery + iLen, "LEFT JOIN (SELECT `id`, `name` AS `name` FROM `player`) AS `player` ON `player`.`id` = `minigame_yutnori`.`pid` ");
	iLen += bTotal ? sprintf(szQuery + iLen, "ORDER BY `total_score` DESC ") : sprintf(szQuery + iLen, "ORDER BY `best_score` DESC ");
	iLen += sprintf(szQuery + iLen, "LIMIT 10 ");

	std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery(szQuery));
	const SQLResult* pkRes = pMsg->Get();
	if (pkRes && pkRes->uiNumRows > 0)
	{
		MYSQL_ROW RowData;
		while ((RowData = mysql_fetch_row(pkRes->pSQLResult)))
		{
			if (RowData[0][0] == '[') // GM
				continue;

			DWORD dwScore;
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

/* static */ DWORD CMiniGameYutnori::GetMyScoreValue(lua_State* L, DWORD dwPID, bool bTotal)
{
	char szQuery[2048 + 1];
	int iLen = 0;

	iLen += sprintf(szQuery + iLen, "SELECT");
	iLen += bTotal ? sprintf(szQuery + iLen, "`total_score` ") : sprintf(szQuery + iLen, "`best_score` ");
	iLen += sprintf(szQuery + iLen, "FROM `minigame_yutnori` WHERE `pid` = %u", dwPID);

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

#endif // __MINI_GAME_YUTNORI__
