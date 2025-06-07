/**
* Filename: Ranking.cpp
* Author: Owsap
**/

#include "stdafx.h"

#if defined(__RANKING_SYSTEM__)
#include "Ranking.h"

#include "char.h"
#include "char_manager.h"
#include "party.h"
#include "db.h"
#include "buffer_manager.h"
#include "desc_client.h"
#include "p2p.h"
#include "guild.h"
#include "config.h"

// NOTE : It was decided to use direct query because if the cache method is used then
// the table returned will be always static, this means that if the table was updated
// via cache, the SQL ORDER BY function would have no effect, causing high scores to
// not return properly. To have a fluid table return we must use a direct query or
// create a vector using sort algorithm, which I will do in the future.
// 20220917.Owsap

void GetQuery(char* szQuery, size_t nBufferSize, BYTE bType, BYTE bCategory, DWORD dwPID = 0)
{
#if defined(__GUILD_DRAGONLAIR_PARTY_SYSTEM__)
	if (bCategory >= CRanking::PARTY_RK_CATEGORY_GUILD_DRAGONLAIR_RED_ALL && bCategory <= CRanking::PARTY_RK_CATEGORY_GUILD_DRAGONLAIR_RED_PAST_WEEK)
		bCategory = CRanking::PARTY_RK_CATEGORY_GUILD_DRAGONLAIR_RED_ALL;
#endif

	int iLen = snprintf(szQuery, nBufferSize, "SELECT");

	for (BYTE bMemberIndex = 0; bMemberIndex < PARTY_MAX_MEMBER; bMemberIndex++)
	{
		if (iLen >= nBufferSize)
			return;

		iLen += snprintf(szQuery + iLen, nBufferSize - iLen,
			" `member%d`.`member_name%d`,", bMemberIndex, bMemberIndex);
	}

	iLen += snprintf(szQuery + iLen, nBufferSize - iLen,
		" `record0`, `record1`, UNIX_TIMESTAMP(`start_time`), `empire`, `guild`.`guild_name`");
	iLen += snprintf(szQuery + iLen, nBufferSize - iLen,
		" FROM `ranking%s` AS `ranking`", get_table_postfix());

	for (BYTE bMemberIndex = 0; bMemberIndex < PARTY_MAX_MEMBER; bMemberIndex++)
	{
		if (iLen >= nBufferSize)
			return;

		iLen += snprintf(szQuery + iLen, nBufferSize - iLen,
			" LEFT JOIN (SELECT `id`, `name` AS `member_name%d` FROM `player%s`) AS `member%d` ON `member%d`.`id` = `ranking`.`member_pid%d`",
			bMemberIndex, get_table_postfix(), bMemberIndex, bMemberIndex, bMemberIndex);
	}

	iLen += snprintf(szQuery + iLen, nBufferSize - iLen,
		" LEFT JOIN (SELECT `id`, `name` AS `guild_name` FROM `guild%s`) AS `guild` ON `guild`.`id` = `ranking`.`guild_id`",
		get_table_postfix());
	iLen += snprintf(szQuery + iLen, nBufferSize - iLen,
		" WHERE `ranking`.`type` = %d AND `ranking`.`category` = %d", bType, bCategory);

	if (dwPID > 0)
	{
		if (iLen >= nBufferSize)
			return;

		iLen += snprintf(szQuery + iLen, nBufferSize - iLen,
			" AND `member_pid0` = %d", dwPID);
	}

	if (iLen >= nBufferSize)
		return;

	switch (bType)
	{
		case CRanking::TYPE_RK_SOLO:
		{
			switch (bCategory)
			{
				default:
					iLen += snprintf(szQuery + iLen, nBufferSize - iLen, " ORDER BY `ranking`.`record0` DESC, `ranking`.`start_time`");
			}
		}
		break;

		case CRanking::TYPE_RK_PARTY:
		{
			switch (bCategory)
			{
#if defined(__GUILD_DRAGONLAIR_PARTY_SYSTEM__)
				case CRanking::PARTY_RK_CATEGORY_GUILD_DRAGONLAIR_RED_ALL:
					iLen += snprintf(szQuery + iLen, nBufferSize - iLen,
						" ORDER BY `ranking`.`record0` ASC, `ranking`.`start_time`");
					break;

				case CRanking::PARTY_RK_CATEGORY_GUILD_DRAGONLAIR_RED_NOW_WEEK:
					iLen += snprintf(szQuery + iLen, nBufferSize - iLen,
						" AND YEARWEEK(`ranking`.`start_time`, 1) = YEARWEEK(NOW(), 1)");
					iLen += snprintf(szQuery + iLen, nBufferSize - iLen, " ORDER BY `ranking`.`record0` ASC, `ranking`.`start_time`");
					break;

				case CRanking::PARTY_RK_CATEGORY_GUILD_DRAGONLAIR_RED_PAST_WEEK:
					iLen += snprintf(szQuery + iLen, nBufferSize - iLen,
						" AND YEARWEEK(`ranking`.`start_time`, 1) = YEARWEEK(NOW(), 1) - 1");
					iLen += snprintf(szQuery + iLen, nBufferSize - iLen, " ORDER BY `ranking`.`record0` ASC, `ranking`.`start_time`");
					break;
#endif

#if defined(__DEFENSE_WAVE__)
				case CRanking::PARTY_RK_CATEGORY_DEFENSE_WAVE:
					iLen += snprintf(szQuery + iLen, nBufferSize - iLen,
						" ORDER BY `ranking`.`record0` ASC, `ranking`.`start_time`");
					break;
#endif

				default:
					iLen += snprintf(szQuery + iLen, nBufferSize - iLen,
						" ORDER BY `ranking`.`record0` ASC, `ranking`.`start_time`");
			}
		}
		break;

		default:
			break;
	}
}

/* static */ void CRanking::Open(LPCHARACTER pChar, BYTE bType, BYTE bCategory)
{
	if (pChar == nullptr)
		return;

	const LPDESC pDesc = pChar->GetDesc();
	if (pDesc == nullptr)
		return;

	if (bType > TYPE_RK_MAX)
		return;

	if (bType == TYPE_RK_SOLO && bCategory > SOLO_RK_CATEGORY_MAX)
		return;

	if (bType == TYPE_RK_PARTY && bCategory > PARTY_RK_CATEGORY_MAX)
		return;

	char szQuery[2048];
	GetQuery(szQuery, sizeof(szQuery), bType, bCategory);

	std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery(szQuery));
	const SQLResult* pRes = pMsg->Get();

	TEMP_BUFFER TmpBuffer;
	if (pRes->uiNumRows > 0)
	{
		MYSQL_ROW RowData;
		while ((RowData = mysql_fetch_row(pMsg->Get()->pSQLResult)))
		{
			BYTE bColumn = 0;
			SRankingData RankingData = {};

			for (BYTE bMemberIndex = 0; bMemberIndex < PARTY_MAX_MEMBER; bMemberIndex++)
			{
				BYTE bCurrentColumn = bColumn++;
				std::memcpy(RankingData.Member[bMemberIndex].szName, RowData[bCurrentColumn] ? RowData[bCurrentColumn] : "", sizeof(RankingData.Member[bMemberIndex].szName));
			}

			str_to_number(RankingData.dwRecord0, RowData[bColumn++]);
			str_to_number(RankingData.dwRecord1, RowData[bColumn++]);
			str_to_number(RankingData.dwStartTime, RowData[bColumn++]);
			str_to_number(RankingData.bEmpire, RowData[bColumn++]);

			BYTE bCurrentColumn = bColumn++;
			strlcpy(RankingData.szGuildName, RowData[bCurrentColumn] ? RowData[bCurrentColumn] : "", sizeof(RankingData.szGuildName));

			TmpBuffer.write(&RankingData, sizeof(RankingData));
		}
	}
	else
	{
		SRankingData RankingData = {};
		TmpBuffer.write(&RankingData, sizeof(RankingData));
	}

	TPacketGCRanking Packet;
	Packet.bHeader = HEADER_GC_RANKING;
	Packet.wSize = sizeof(Packet) + TmpBuffer.size();
	Packet.bType = bType;
	Packet.bCategory = bCategory;

	if (TmpBuffer.size())
	{
		pDesc->BufferedPacket(&Packet, sizeof(Packet));
		pDesc->Packet(TmpBuffer.read_peek(), TmpBuffer.size());
	}
	else
		pDesc->Packet(&Packet, sizeof(Packet));
}

/* static */ void CRanking::Register(DWORD dwPID, BYTE bType, BYTE bCategory, const TRankingData& rTable)
{
	LPCHARACTER pChar = CHARACTER_MANAGER::instance().FindByPID(dwPID);
	if (pChar == nullptr)
	{
		const CCI* pCCI = P2P_MANAGER::instance().FindByPID(dwPID);
		if (pCCI == nullptr)
			return;

		pChar = pCCI->pkDesc ? pCCI->pkDesc->GetCharacter() : nullptr;
		if (pChar == nullptr)
			return;
	}

	if (bType > TYPE_RK_MAX)
		return;

	if (bType == TYPE_RK_SOLO && bCategory > SOLO_RK_CATEGORY_MAX)
		return;

	if (bType == TYPE_RK_PARTY && bCategory > PARTY_RK_CATEGORY_MAX)
		return;

	switch (bType)
	{
		case TYPE_RK_SOLO:
		{
			switch (bCategory)
			{
				default:
					break;
			}
		}
		break;

		case TYPE_RK_PARTY:
		{
			const LPPARTY pParty = pChar->GetParty();
			if (pParty == nullptr)
				return;

			const DWORD dwLeaderPID = pParty->GetLeaderPID();
			if (dwLeaderPID != pChar->GetPlayerID())
				return;

			std::vector<DWORD> vMemberPID;
			vMemberPID.reserve(PARTY_MAX_MEMBER);

			auto FCheckMembePID = [&vMemberPID](const LPCHARACTER pMemberChar)
				{
					if (pMemberChar && pMemberChar->GetDesc())
						vMemberPID.emplace_back(pMemberChar->GetPlayerID());
				};
			pParty->ForEachOnlineMember(FCheckMembePID);

			char szQuery[2048];
			int iLen = snprintf(szQuery, sizeof(szQuery),
				"REPLACE INTO `ranking%s` (`type`, `category`, ", get_table_postfix());

			for (BYTE bMemberIndex = 0; bMemberIndex < PARTY_MAX_MEMBER; bMemberIndex++)
				iLen += snprintf(szQuery + iLen, sizeof(szQuery) - iLen,
					"`member_pid%d`, ", bMemberIndex);

			iLen += snprintf(szQuery + iLen, sizeof(szQuery) - iLen,
				"`record0`, `record1`, `start_time`, `empire`) VALUES (%d, %d, ", bType, bCategory);

			for (BYTE bMemberIndex = 0; bMemberIndex < PARTY_MAX_MEMBER; bMemberIndex++)
				iLen += snprintf(szQuery + iLen, sizeof(szQuery) - iLen,
					"%d, ",
					bMemberIndex >= vMemberPID.size() ? 0 : vMemberPID.at(bMemberIndex));

			iLen += snprintf(szQuery + iLen, sizeof(szQuery) - iLen,
				"%d, %d, FROM_UNIXTIME(%d), %d)",
				rTable.dwRecord0, vMemberPID.size(), rTable.dwStartTime, pChar->GetEmpire());

			DBManager::instance().DirectQuery(szQuery);
		}
		break;
	}
}

/* static */ void CRanking::Delete(BYTE bType, BYTE bCategory)
{
	char szQuery[1024];
	snprintf(szQuery, sizeof(szQuery), "DELETE FROM `ranking%s` WHERE `type` = %d AND `category` = %d",
		get_table_postfix(), bType, bCategory);
	DBManager::instance().DirectQuery(szQuery);
}

/* static */ void CRanking::DeleteByPID(DWORD dwPID, BYTE bType, BYTE bCategory)
{
	char szQuery[1024];
	snprintf(szQuery, sizeof(szQuery), "DELETE FROM `ranking%s` WHERE `type` = %d AND `category` = %d AND `member_pid0` = %d",
		get_table_postfix(), bType, bCategory, dwPID);
	DBManager::instance().DirectQuery(szQuery);
}
#endif
