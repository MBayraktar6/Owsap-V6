#include "stdafx.h"
#include "ClientManager.h"
#include "Main.h"
#include "Config.h"
#include "DBManager.h"
#include "QID.h"

void CClientManager::LoadEventFlag()
{
	char szQuery[1024];
	snprintf(szQuery, sizeof(szQuery), "SELECT `szName`, `lValue` FROM quest%s WHERE `dwPID` = 0", GetTablePostfix());
	std::unique_ptr<SQLMsg> pmsg(CDBManager::instance().DirectQuery(szQuery));

	SQLResult* pRes = pmsg->Get();
	if (pRes && pRes->uiNumRows)
	{
		MYSQL_ROW row;
		while ((row = mysql_fetch_row(pRes->pSQLResult)))
		{
			TPacketSetEventFlag p;
			strlcpy(p.szFlagName, row[0], sizeof(p.szFlagName));
			str_to_number(p.lValue, row[1]);
			sys_log(0, "EventFlag Load %s %d", p.szFlagName, p.lValue);
			m_map_lEventFlag.insert(std::make_pair(std::string(p.szFlagName), p.lValue));
			ForwardPacket(HEADER_DG_SET_EVENT_FLAG, &p, sizeof(TPacketSetEventFlag));
		}
	}
}

void CClientManager::SetEventFlag(TPacketSetEventFlag* p)
{
	ForwardPacket(HEADER_DG_SET_EVENT_FLAG, p, sizeof(TPacketSetEventFlag));

	bool bChanged = false;

	TEventFlagMap::iterator it = m_map_lEventFlag.find(p->szFlagName);
	if (it == m_map_lEventFlag.end())
	{
		bChanged = true;
		m_map_lEventFlag.insert(std::make_pair(std::string(p->szFlagName), p->lValue));
	}
	else if (it->second != p->lValue)
	{
		bChanged = true;
		it->second = p->lValue;
	}

	if (bChanged)
	{
		char szQuery[1024];
		snprintf(szQuery, sizeof(szQuery),
			"REPLACE INTO quest%s (`dwPID`, `szName`, `szState`, `lValue`) VALUES(0, '%s', '', %ld)",
			GetTablePostfix(), p->szFlagName, p->lValue);
		szQuery[1023] = '\0';

		// CDBManager::instance().ReturnQuery(szQuery, QID_QUEST_SAVE, 0, NULL);
		CDBManager::instance().AsyncQuery(szQuery);
		sys_log(0, "HEADER_GD_SET_EVENT_FLAG : Changed CClientmanager::SetEventFlag(%s %d) ", p->szFlagName, p->lValue);
		return;
	}
	sys_log(0, "HEADER_GD_SET_EVENT_FLAG : No Changed CClientmanager::SetEventFlag(%s %d) ", p->szFlagName, p->lValue);
}

void CClientManager::SendEventFlagsOnSetup(CPeer* peer)
{
	TEventFlagMap::iterator it = m_map_lEventFlag.begin();
	for (; it != m_map_lEventFlag.end(); ++it)
	{
		TPacketSetEventFlag p;
		strlcpy(p.szFlagName, it->first.c_str(), sizeof(p.szFlagName));
		p.lValue = it->second;
		peer->EncodeHeader(HEADER_DG_SET_EVENT_FLAG, 0, sizeof(TPacketSetEventFlag));
		peer->Encode(&p, sizeof(TPacketSetEventFlag));
	}
}

#if defined(__GUILD_EVENT_FLAG__) //&& defined(__GUILD_RENEWAL__)
void CClientManager::LoadGuildEventFlag()
{
	char szQuery[1024];
	snprintf(szQuery, sizeof(szQuery), "SELECT `dwGuildID`, `szFlagName`, `lValue` FROM `guild_event_flag%s`", GetTablePostfix());
	std::unique_ptr<SQLMsg> pMsg(CDBManager::instance().DirectQuery(szQuery));

	SQLResult* pRes = pMsg->Get();
	if (pRes && pRes->uiNumRows)
	{
		MYSQL_ROW RowData;
		while ((RowData = mysql_fetch_row(pRes->pSQLResult)))
		{
			TPacketSetGuildEventFlag p;
			str_to_number(p.dwGuildID, RowData[0]);
			strlcpy(p.szFlagName, RowData[1], sizeof(p.szFlagName));
			str_to_number(p.lValue, RowData[2]);

			sys_log(0, "GuildEventFlag Load %u %s %ld", p.dwGuildID, p.szFlagName, p.lValue);
			m_map_lGuildEventFlag[p.dwGuildID].emplace(std::make_pair(std::string(p.szFlagName), p.lValue));

			ForwardPacket(HEADER_DG_GUILD_EVENT_FLAG, &p, sizeof(TPacketSetGuildEventFlag));
		}
	}
}

void CClientManager::GuildSetEventFlag(TPacketSetGuildEventFlag* pTable)
{
	ForwardPacket(HEADER_DG_GUILD_EVENT_FLAG, pTable, sizeof(TPacketSetGuildEventFlag));

	bool bChanged = false;

	TGuildEventFlagMap::iterator itGuild = m_map_lGuildEventFlag.find(pTable->dwGuildID);
	if (itGuild != m_map_lGuildEventFlag.end())
	{
		TEventFlagMap::iterator itEventFlag = itGuild->second.find(pTable->szFlagName);
		if (itEventFlag == itGuild->second.end())
		{
			bChanged = true;
			m_map_lGuildEventFlag[pTable->dwGuildID][pTable->szFlagName] = pTable->lValue;
		}
		else if (itEventFlag->second != pTable->lValue)
		{
			bChanged = true;
			itEventFlag->second = pTable->lValue;
		}
	}
	else
	{
		bChanged = true;
		m_map_lGuildEventFlag[pTable->dwGuildID][pTable->szFlagName] = pTable->lValue;
	}

	if (bChanged)
	{
		char szQuery[1024];
		snprintf(szQuery, sizeof(szQuery),
			"INSERT INTO `guild_event_flag%s` (dwGuildID, szFlagName, lValue) "
			"VALUES(%u, '%s', %ld) "
			"ON DUPLICATE KEY UPDATE szFlagName = '%s', lValue = %ld",
			GetTablePostfix(), pTable->dwGuildID, pTable->szFlagName, pTable->lValue,
			pTable->szFlagName, pTable->lValue);
		szQuery[1023] = '\0';

		CDBManager::Instance().AsyncQuery(szQuery);
		sys_log(0, "HEADER_GD_GUILD_EVENT_FLAG : Changed CClientmanager::GuildSetEventFlag(%u %s %ld) ",
			pTable->dwGuildID, pTable->szFlagName, pTable->lValue);

		return;
	}
}

void CClientManager::SendGuildEventFlagsOnSetup(CPeer* pPeer)
{
	TGuildEventFlagMap::iterator itGuild = m_map_lGuildEventFlag.begin();
	for (; itGuild != m_map_lGuildEventFlag.end(); ++itGuild)
	{
		TEventFlagMap::iterator itEventFlag = itGuild->second.begin();
		for (; itEventFlag != itGuild->second.end(); ++itEventFlag)
		{
			TPacketSetGuildEventFlag p;
			p.dwGuildID = itGuild->first;
			strlcpy(p.szFlagName, itEventFlag->first.c_str(), sizeof(p.szFlagName));
			p.lValue = itEventFlag->second;

			pPeer->EncodeHeader(HEADER_DG_GUILD_EVENT_FLAG, 0, sizeof(TPacketSetGuildEventFlag));
			pPeer->Encode(&p, sizeof(TPacketSetGuildEventFlag));
		}
	}
}
#endif
