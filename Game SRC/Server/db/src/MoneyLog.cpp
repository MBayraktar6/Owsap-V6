#include "stdafx.h"
#include "MoneyLog.h"
#include "ClientManager.h"
#include "Peer.h"

CMoneyLog::CMoneyLog()
{
}

CMoneyLog::~CMoneyLog()
{
}

void CMoneyLog::Save()
{
	CPeer* peer = CClientManager::instance().GetAnyPeer();
	if (!peer)
		return;

	for (BYTE bType = 0; bType < MONEY_LOG_TYPE_MAX_NUM; bType++)
	{
		MoneyLogMap::iterator it = m_MoneyLogContainer[bType].begin();
		for (; it != m_MoneyLogContainer[bType].end(); ++it)
		{
			// bType;
			TPacketMoneyLog p;
			p.type = bType;
			p.vnum = it->first;
			p.gold = it->second.iGold;
#if defined(__CHEQUE_SYSTEM__)
			p.cheque = it->second.iCheque;
#endif
			peer->EncodeHeader(HEADER_DG_MONEY_LOG, 0, sizeof(p));
			peer->Encode(&p, sizeof(p));
		}
		m_MoneyLogContainer[bType].clear();
	}
}

void CMoneyLog::AddLog(BYTE bType, DWORD dwVnum, int iGold
#if defined(__CHEQUE_SYSTEM__)
	, int iCheque
#endif
)
{
	m_MoneyLogContainer[bType][dwVnum].iGold += iGold;
#if defined(__CHEQUE_SYSTEM__)
	m_MoneyLogContainer[bType][dwVnum].iCheque += iCheque;
#endif
}
