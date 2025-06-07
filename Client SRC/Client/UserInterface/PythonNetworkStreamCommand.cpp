#include "StdAfx.h"

#include "PythonNetworkStream.h"
#include "PythonNonPlayer.h"

#include "AbstractApplication.h"
#include "AbstractPlayer.h"
#include "AbstractCharacterManager.h"
#include "AbstractChat.h"

#include "InstanceBase.h"

#if defined(ENABLE_METINSTONE_SWAP)
#	include "MetinstoneSwapManager.h"
#endif

#if defined(ENABLE_EVENT_BANNER)
#	include "PythonInGameEventSystemManager.h"
#endif

#define ishan(ch) (((ch) & 0xE0) > 0x90)
#define ishanasc(ch) (isascii(ch) || ishan(ch))
#define ishanalp(ch) (isalpha(ch) || ishan(ch))
#define isnhdigit(ch) (!ishan(ch) && isdigit(ch))
#define isnhspace(ch) (!ishan(ch) && isspace(ch))

#define LOWER(c) (((c) >= 'A' && (c) <= 'Z') ? ((c) + ('a' - 'A')) : (c))
#define UPPER(c) (((c) >= 'a' && (c) <= 'z') ? ((c) + ('A' - 'a')) : (c))

void SkipSpaces(char** string)
{
	for (; **string != '\0' && isnhspace((unsigned char)**string); ++(*string));
}

char* OneArgument(char* argument, char* first_arg)
{
	char mark = FALSE;

	if (!argument)
	{
		*first_arg = '\0';
		return NULL;
	}

	SkipSpaces(&argument);

	while (*argument)
	{
		if (*argument == '\"')
		{
			mark = !mark;
			++argument;
			continue;
		}

		if (!mark && isnhspace((unsigned char)*argument))
			break;

		*(first_arg++) = LOWER(*argument);
		++argument;
	}

	*first_arg = '\0';

	SkipSpaces(&argument);
	return (argument);
}

void AppendMonsterList(const CPythonNonPlayer::TMobTableList& c_rMobTableList, const char* c_szHeader, int iType)
{
	DWORD dwMonsterCount = 0;
	std::string strMonsterList = c_szHeader;

	CPythonNonPlayer::TMobTableList::const_iterator itor = c_rMobTableList.begin();
	for (; itor != c_rMobTableList.end(); ++itor)
	{
		const CPythonNonPlayer::TMobTable* c_pMobTable = *itor;
		if (iType == c_pMobTable->bRank)
		{
			if (dwMonsterCount != 0)
				strMonsterList += ", ";
			strMonsterList += c_pMobTable->szLocaleName;
			if (++dwMonsterCount > 5)
				break;
		}
	}
	if (dwMonsterCount > 0)
	{
		IAbstractChat& rkChat = IAbstractChat::GetSingleton();
		rkChat.AppendChat(CHAT_TYPE_INFO, strMonsterList.c_str());
	}
}

bool CPythonNetworkStream::ClientCommand(const char* c_szCommand)
{
	return false;
}

bool SplitToken(const char* c_szLine, CTokenVector* pstTokenVector, const char* c_szDelimeter = " ")
{
	pstTokenVector->reserve(10);
	pstTokenVector->clear();

	std::string stToken;
	std::string strLine = c_szLine;

	DWORD basePos = 0;

	do
	{
		int beginPos = strLine.find_first_not_of(c_szDelimeter, basePos);
		if (beginPos < 0)
			return false;

		int endPos;

		if (strLine[beginPos] == '"')
		{
			++beginPos;
			endPos = strLine.find_first_of("\"", beginPos);

			if (endPos < 0)
				return false;

			basePos = endPos + 1;
		}
		else
		{
			endPos = strLine.find_first_of(c_szDelimeter, beginPos);
			basePos = endPos;
		}

		pstTokenVector->push_back(strLine.substr(beginPos, endPos - beginPos));

		// 추가 코드. 맨뒤에 탭이 있는 경우를 체크한다. - [levites]
		if (int(strLine.find_first_not_of(c_szDelimeter, basePos)) < 0)
			break;
	} while (basePos < strLine.length());

	return true;
}

void CPythonNetworkStream::ServerCommand(char* c_szCommand)
{
	// #0000811: [M2EU] 콘솔창 기능 차단 
	if (strcmpi(c_szCommand, "ConsoleEnable") == 0)
		return;

	if (m_apoPhaseWnd[PHASE_WINDOW_GAME])
	{
		bool isTrue;
		if (PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME],
			"BINARY_ServerCommand_Run",
			Py_BuildValue("(s)", c_szCommand),
			&isTrue
		))
		{
			if (isTrue)
				return;
		}
	}
	else if (m_poSerCommandParserWnd)
	{
		bool isTrue;
		if (PyCallClassMemberFunc(m_poSerCommandParserWnd,
			"BINARY_ServerCommand_Run",
			Py_BuildValue("(s)", c_szCommand),
			&isTrue
		))
		{
			if (isTrue)
				return;
		}
	}

	CTokenVector TokenVector;
	if (!SplitToken(c_szCommand, &TokenVector))
		return;

	if (TokenVector.empty())
		return;

	const char* szCmd = TokenVector[0].c_str();

#if defined(ENABLE_EVENT_BANNER)
	// NOTE : Instead of creating an additional packet to enable events, we will just
	// take advantage of the command packet and filter the existing events, making it
	// easier to control. However, this code below is subject to change in the future
	// to its own packet.
	BYTE bInGameEventType = CPythonInGameEventSystemManager::Instance().GetInGameEventType(szCmd);
	if (bInGameEventType != CPythonInGameEventSystemManager::INGAME_EVENT_TYPE_NONE)
	{
		if (2 != TokenVector.size())
		{
			TraceError("CPythonNetworkStream::ServerCommand(c_szCommand=%s) - Strange Parameter Count : %d", c_szCommand, TokenVector.size());
			return;
		}

		int iFlag = atoi(TokenVector[1].c_str());
		CPythonInGameEventSystemManager::Instance().SetInGameEventEnable(bInGameEventType, iFlag);
		return;
	}
#endif

	if (!strcmpi(szCmd, "quit"))
	{
		PostQuitMessage(0);
	}
	else if (!strcmpi(szCmd, "BettingMoney"))
	{
		if (2 != TokenVector.size())
		{
			TraceError("CPythonNetworkStream::ServerCommand(c_szCommand=%s) - Strange Parameter Count : %s", c_szCommand);
			return;
		}

		// UINT uMoney = atoi(TokenVector[1].c_str());
	}
#if defined(ENABLE_SEND_TARGET_INFO)
	else if (!strcmpi(szCmd, "RefreshMonsterDropInfo"))
	{
		if (TokenVector.size() != 2)
		{
			TraceError("CPythonNetworkStream::ServerCommand(c_szCommand=%s) - Strange Parameter Count : %s", c_szCommand);
			return;
		}

		const DWORD dwRace = atoi(TokenVector[1].c_str());
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_RefreshTargetMonsterDropInfo", Py_BuildValue("(i)", dwRace));
	}
#endif
	// GIFT NOTIFY
	else if (!strcmpi(szCmd, "gift"))
	{
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "Gift_Show", Py_BuildValue("()"));
	}
#if defined(ENABLE_GEM_SYSTEM)
	else if (!strcmpi(szCmd, "BINARY_OpenSelectItemWindowEx"))
	{
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_OpenSelectItemWindowEx", Py_BuildValue("()"));
	}
	else if (!strcmpi(szCmd, "BINARY_RefreshSelectItemWindowEx"))
	{
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_RefreshSelectItemWindowEx", Py_BuildValue("()"));
	}
#endif
#if !defined(ENABLE_CUBE_RENEWAL)
	// CUBE
	else if (!strcmpi(szCmd, "cube"))
	{
		if (TokenVector.size() < 2)
		{
			TraceError("CPythonNetworkStream::ServerCommand(c_szCommand=%s) - Strange Parameter Count : %s", c_szCommand);
			return;
		}

		if ("open" == TokenVector[1])
		{
			if (4 > TokenVector.size())
			{
				TraceError("CPythonNetworkStream::ServerCommand(c_szCommand=%s) - Strange Parameter Count : %s", c_szCommand);
				return;
			}

			DWORD npcVNUM = (DWORD)atoi(TokenVector[2].c_str());
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_Cube_Open", Py_BuildValue("(ii)", npcVNUM, std::stoi(TokenVector[3])));
		}
		else if ("close" == TokenVector[1])
		{
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_Cube_Close", Py_BuildValue("()"));
		}
		else if ("info" == TokenVector[1])
		{
			if (5 != TokenVector.size())
			{
				TraceError("CPythonNetworkStream::ServerCommand(c_szCommand=%s) - Strange Parameter Count : %s", c_szCommand);
				return;
			}

			UINT gold = atoi(TokenVector[2].c_str());
			UINT itemVnum = atoi(TokenVector[3].c_str());
			UINT count = atoi(TokenVector[4].c_str());
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_Cube_UpdateInfo", Py_BuildValue("(iii)", gold, itemVnum, count));
		}
		else if ("success" == TokenVector[1])
		{
			if (4 != TokenVector.size())
			{
				TraceError("CPythonNetworkStream::ServerCommand(c_szCommand=%s) - Strange Parameter Count : %s", c_szCommand);
				return;
			}

			UINT itemVnum = atoi(TokenVector[2].c_str());
			UINT count = atoi(TokenVector[3].c_str());
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_Cube_Succeed", Py_BuildValue("(ii)", itemVnum, count));
		}
		else if ("fail" == TokenVector[1])
		{
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_Cube_Failed", Py_BuildValue("()"));
		}
		else if ("r_list" == TokenVector[1])
		{
			// result list (/cube r_list npcVNUM resultCount resultText)
			// 20383 4 72723,1/72725,1/72730.1/50001,5 <- 이런식으로 "/" 문자로 구분된 리스트를 줌
			if (5 != TokenVector.size())
			{
				TraceError("CPythonNetworkStream::ServerCommand(c_szCommand=%s) - Strange Parameter Count : %d", c_szCommand, 5);
				return;
			}

			DWORD npcVNUM = (DWORD)atoi(TokenVector[2].c_str());

			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_Cube_ResultList", Py_BuildValue("(is)", npcVNUM, TokenVector[4].c_str()));
		}
		else if ("m_info" == TokenVector[1])
		{
			// material list (/cube m_info requestStartIndex resultCount MaterialText)
			// ex) requestStartIndex: 0, resultCount : 5 - 해당 NPC가 만들수 있는 아이템 중 0~4번째에 해당하는 아이템을 만드는 데 필요한 모든 재료들이 MaterialText에 들어있음
			// 위 예시처럼 아이템이 다수인 경우 구분자 "@" 문자를 사용
			// 0 5 125,1|126,2|127,2|123,5&555,5&555,4/120000 <- 이런식으로 서버에서 클라로 리스트를 줌

			if (5 != TokenVector.size())
			{
				TraceError("CPythonNetworkStream::ServerCommand(c_szCommand=%s) - Strange Parameter Count : %d", c_szCommand, 5);
				return;
			}

			UINT requestStartIndex = (UINT)atoi(TokenVector[2].c_str());
			UINT resultCount = (UINT)atoi(TokenVector[3].c_str());

			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_Cube_MaterialInfo", Py_BuildValue("(iis)", requestStartIndex, resultCount, TokenVector[4].c_str()));
		}
	}
	// CUEBE_END
#endif
	else if (!strcmpi(szCmd, "ObserverCount"))
	{
		if (2 != TokenVector.size())
		{
			TraceError("CPythonNetworkStream::ServerCommand(c_szCommand=%s) - Strange Parameter Count : %s", c_szCommand);
			return;
		}

		UINT uObserverCount = atoi(TokenVector[1].c_str());

		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME],
			"BINARY_BettingGuildWar_UpdateObserverCount",
			Py_BuildValue("(i)", uObserverCount)
		);
	}
	else if (!strcmpi(szCmd, "ObserverMode"))
	{
		if (2 != TokenVector.size())
		{
			TraceError("CPythonNetworkStream::ServerCommand(c_szCommand=%s) - Strange Parameter Count : %s", c_szCommand);
			return;
		}

		UINT uMode = atoi(TokenVector[1].c_str());

		IAbstractPlayer& rkPlayer = IAbstractPlayer::GetSingleton();
		rkPlayer.SetObserverMode(uMode ? true : false);

		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME],
			"BINARY_BettingGuildWar_SetObserverMode",
			Py_BuildValue("(i)", uMode)
		);
	}
	else if (!strcmpi(szCmd, "ObserverTeamInfo"))
	{
	}
#if defined(ENABLE_LOCALE_CLIENT)
	else if (!strcmpi(szCmd, "language_change"))
	{
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "LanguageChange", Py_BuildValue("()"));
	}
#endif
#if defined(ENABLE_METINSTONE_SWAP)
	else if (!strcmpi(szCmd, "metinstone_swap"))
	{
		if (2 != TokenVector.size())
		{
			TraceError("CPythonNetworkStream::ServerCommand(c_szCommand=%s) - Strange Parameter Count : %s", c_szCommand);
			return;
		}

		MetinStoneSwap::Manager::Instance().SetEventFlag(atoi(TokenVector[1].c_str()));
	}
#endif
	else if (!strcmpi(szCmd, "StoneDetect"))
	{
		if (4 != TokenVector.size())
		{
			TraceError("CPythonNetworkStream::ServerCommand(c_szCommand=%s) - Strange Parameter Count : %s", c_szCommand);
			return;
		}

		// vid distance(1-3) angle(0-360)
		DWORD dwVID = atoi(TokenVector[1].c_str());
		BYTE byDistance = atoi(TokenVector[2].c_str());
		float fAngle = atof(TokenVector[3].c_str());
		fAngle = fmod(540.0f - fAngle, 360.0f);
		Tracef("StoneDetect [VID:%d] [Distance:%d] [Angle:%d->%f]\n", dwVID, byDistance, atoi(TokenVector[3].c_str()), fAngle);

		IAbstractCharacterManager& rkChrMgr = IAbstractCharacterManager::GetSingleton();

		CInstanceBase* pInstance = rkChrMgr.GetInstancePtr(dwVID);
		if (!pInstance)
		{
			TraceError("CPythonNetworkStream::ServerCommand(c_szCommand=%s) - Not Exist Instance", c_szCommand);
			return;
		}

		TPixelPosition PixelPosition;
		D3DXVECTOR3 v3Rotation(0.0f, 0.0f, fAngle);
		pInstance->NEW_GetPixelPosition(&PixelPosition);

		PixelPosition.y *= -1.0f;

		switch (byDistance)
		{
		case 0:
			CEffectManager::Instance().RegisterEffect("d:/ymir work/effect/etc/firecracker/find_out.mse");
			CEffectManager::Instance().CreateEffect("d:/ymir work/effect/etc/firecracker/find_out.mse", PixelPosition, v3Rotation);
			break;
		case 1:
			CEffectManager::Instance().RegisterEffect("d:/ymir work/effect/etc/compass/appear_small.mse");
			CEffectManager::Instance().CreateEffect("d:/ymir work/effect/etc/compass/appear_small.mse", PixelPosition, v3Rotation);
			break;
		case 2:
			CEffectManager::Instance().RegisterEffect("d:/ymir work/effect/etc/compass/appear_middle.mse");
			CEffectManager::Instance().CreateEffect("d:/ymir work/effect/etc/compass/appear_middle.mse", PixelPosition, v3Rotation);
			break;
		case 3:
			CEffectManager::Instance().RegisterEffect("d:/ymir work/effect/etc/compass/appear_large.mse");
			CEffectManager::Instance().CreateEffect("d:/ymir work/effect/etc/compass/appear_large.mse", PixelPosition, v3Rotation);
			break;
		default:
			TraceError("CPythonNetworkStream::ServerCommand(c_szCommand=%s) - Strange Distance", c_szCommand);
			break;
		}

#ifdef _DEBUG
		IAbstractChat& rkChat = IAbstractChat::GetSingleton();
		rkChat.AppendChat(CHAT_TYPE_INFO, c_szCommand);
#endif
	}
	else if (!strcmpi(szCmd, "StartStaminaConsume"))
	{
		if (3 != TokenVector.size())
		{
			TraceError("CPythonNetworkStream::ServerCommand(c_szCommand=%s) - Strange Parameter Count : %s", c_szCommand);
			return;
		}

		DWORD dwConsumePerSec = atoi(TokenVector[1].c_str());
		DWORD dwCurrentStamina = atoi(TokenVector[2].c_str());

		IAbstractPlayer& rPlayer = IAbstractPlayer::GetSingleton();
		rPlayer.StartStaminaConsume(dwConsumePerSec, dwCurrentStamina);
	}
	else if (!strcmpi(szCmd, "StopStaminaConsume"))
	{
		if (2 != TokenVector.size())
		{
			TraceError("CPythonNetworkStream::ServerCommand(c_szCommand=%s) - Strange Parameter Count : %d", c_szCommand, TokenVector.size());
			return;
		}

		DWORD dwCurrentStamina = atoi(TokenVector[1].c_str());

		IAbstractPlayer& rPlayer = IAbstractPlayer::GetSingleton();
		rPlayer.StopStaminaConsume(dwCurrentStamina);
	}
	else if (!strcmpi(szCmd, "messenger_auth"))
	{
		const std::string& c_rstrName = TokenVector[1].c_str();
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OnMessengerAddFriendQuestion", Py_BuildValue("(s)", c_rstrName.c_str()));
	}
	else if (!strcmpi(szCmd, "combo"))
	{
		int iFlag = atoi(TokenVector[1].c_str());
		IAbstractPlayer& rPlayer = IAbstractPlayer::GetSingleton();
		rPlayer.SetComboSkillFlag(iFlag);
		m_bComboSkillFlag = iFlag ? true : false;
	}
	else if (!strcmpi(szCmd, "setblockmode"))
	{
		int iFlag = atoi(TokenVector[1].c_str());
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OnBlockMode", Py_BuildValue("(i)", iFlag));
	}
	else
	{
		TraceError("Unknown Server Command %s | %s", c_szCommand, szCmd);
	}
}
