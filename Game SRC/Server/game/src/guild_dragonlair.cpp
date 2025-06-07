/**
* Filename: guild_dragonlair.cpp
* Author: Owsap
**/

#include "stdafx.h"

#if defined(__GUILD_DRAGONLAIR_SYSTEM__)
#include "guild_dragonlair.h"
#include "char.h"
#include "char_manager.h"
#include "guild.h"
#include "guild_manager.h"
#include "sectree_manager.h"
#include "utils.h"
#include "config.h"
#include "regen.h"
#include "unique_item.h"
#include "item.h"
#include "start_position.h"
#include "questmanager.h"
#include "db.h"

#if defined(__GUILD_DRAGONLAIR_PARTY_SYSTEM__)
#	include "party.h"
#	include "desc.h"
#endif

EVENTINFO(GuildDragonLairEventInfo)
{
	CGuildDragonLair* pGuildDragonLair;
	GuildDragonLairEventInfo()
		: pGuildDragonLair(NULL)
	{}
};

EVENTINFO(GuildDragonLairStoneEventInfo)
{
	CGuildDragonLair* pGuildDragonLair;
	bool bMobSkill;
	GuildDragonLairStoneEventInfo()
		: pGuildDragonLair(NULL)
		, bMobSkill(false)
	{}
};

EVENTINFO(GuildDragonLairSpawnEventInfo)
{
	CGuildDragonLair* pGuildDragonLair;
	bool bFirstSpawn;
	GuildDragonLairSpawnEventInfo()
		: pGuildDragonLair(NULL)
		, bFirstSpawn(true)
	{}
};

EVENTINFO(GuildDragonLairExitEventInfo)
{
	CGuildDragonLair* pGuildDragonLair;
	BYTE bStep;
	GuildDragonLairExitEventInfo()
		: pGuildDragonLair(NULL)
		, bStep(0)
	{}
};

EVENTFUNC(GuildDragonLairEndEvent)
{
	GuildDragonLairEventInfo* pEventInfo = dynamic_cast<GuildDragonLairEventInfo*>(event->info);
	if (pEventInfo == NULL)
	{
		sys_err("GuildDragonLairEndEvent: <Factor> Null pointer.");
		return 0;
	}

	CGuildDragonLair* pGuildDragonLair = pEventInfo->pGuildDragonLair;
	if (pGuildDragonLair == NULL)
	{
		sys_err("GuildDragonLairEndEvent: <Factor> Null instance.");
		return 0;
	}

	LPSECTREE_MAP pSectree = pGuildDragonLair->GetSectree();
	if (pSectree == NULL)
	{
		sys_err("GuildDragonLairEndEvent: <Factor> Null sectree.");
		return 0;
	}

	if (pGuildDragonLair->End(false) == false)
	{
		sys_err("GuildDragonLairEndEvent: <Factor> Cannot end.");
		return 0;
	}

	return 0;
}

EVENTFUNC(GuildDragonLairGateEvent)
{
	GuildDragonLairEventInfo* pEventInfo = dynamic_cast<GuildDragonLairEventInfo*>(event->info);
	if (pEventInfo == NULL)
	{
		sys_err("GuildDragonLairGateEvent: <Factor> Null pointer.");
		return 0;
	}

	CGuildDragonLair* pGuildDragonLair = pEventInfo->pGuildDragonLair;
	if (pGuildDragonLair == NULL)
	{
		sys_err("GuildDragonLairGateEvent: <Factor> Null instance.");
		return 0;
	}

	pGuildDragonLair->KillUnique("Gate");
	return 0;
};

EVENTFUNC(GuildDragonLairStoneEvent)
{
	GuildDragonLairStoneEventInfo* pEventInfo = dynamic_cast<GuildDragonLairStoneEventInfo*>(event->info);
	if (pEventInfo == NULL)
	{
		sys_err("GuildDragonLairStoneEvent: <Factor> Null pointer.");
		return 0;
	}

	CGuildDragonLair* pGuildDragonLair = pEventInfo->pGuildDragonLair;
	if (pEventInfo == NULL)
	{
		sys_err("GuildDragonLairStoneEvent: <Factor> Null instance.");
		return 0;
	}

	LPSECTREE_MAP pSectree = pGuildDragonLair->GetSectree();
	if (pSectree == NULL)
	{
		sys_err("GuildDragonLairStoneEvent: <Factor> Null sectree.");
		return 0;
	}

	if (pGuildDragonLair->GetStage() == GUILD_DRAGONLAIR_STAGE1 || pGuildDragonLair->GetStage() == GUILD_DRAGONLAIR_STAGE2)
	{
		if (pEventInfo->bMobSkill == false)
		{
			const int iBaseX = pSectree->m_setting.iBaseX + GuildDragonLair_GetFactor(3, "PixelPosition", "King", "x") * 100;
			const int iBaseY = pSectree->m_setting.iBaseY + GuildDragonLair_GetFactor(3, "PixelPosition", "King", "y") * 100;

			DWORD dwCurrentTime = get_dword_time();
			for (BYTE bStoneNum = 0; bStoneNum < GUILD_DRAGONLAIR_STONE_COUNT; ++bStoneNum)
			{
				LPCHARACTER pStone = pGuildDragonLair->GetUnique("Stone" + std::to_string(bStoneNum));
				if (pStone)
				{
					pStone->SetRotationToXY(iBaseX, iBaseY);
					pStone->SendMovePacket(FUNC_MOB_SKILL, 0, pStone->GetX(), pStone->GetY(), 0, dwCurrentTime);
				}
			}

			pEventInfo->bMobSkill = true;
		}
		else
		{
			pGuildDragonLair->SetStage(pGuildDragonLair->GetStage() + 1);
			return 0;
		}
	}

	return PASSES_PER_SEC(GuildDragonLair_GetFactor(2, "Event", "RestAfterSpecialAttack"));
};

EVENTFUNC(GuildDragonLairSpawnEvent)
{
	GuildDragonLairSpawnEventInfo* pEventInfo = dynamic_cast<GuildDragonLairSpawnEventInfo*>(event->info);
	if (pEventInfo == NULL)
	{
		sys_err("GuildDragonLairSpawnEvent: <Factor> Null pointer.");
		return 0;
	}

	CGuildDragonLair* pGuildDragonLair = pEventInfo->pGuildDragonLair;
	if (pGuildDragonLair == NULL)
	{
		sys_err("GuildDragonLairSpawnEvent: <Factor> Null instance.");
		return 0;
	}

	const char* szDungeonType = pGuildDragonLair->GetDungeonType();
	const BYTE bStage = pGuildDragonLair->GetStage();

	if (bStage == GUILD_DRAGONLAIR_STAGE_NONE || bStage > GUILD_DRAGONLAIR_STAGE4)
		return 0;

	DWORD dwMobRaceNum = static_cast<DWORD>(GuildDragonLair_GetIndexFactor2(szDungeonType, "Stage", bStage, "MonsterVnum"));
	size_t iMobCount = GuildDragonLair_GetIndexFactor2(szDungeonType, "Stage", bStage, "MonsterCount");
	size_t iMobRespawnTime = GuildDragonLair_GetIndexFactor2(szDungeonType, "Stage", bStage, "MonsterRespawnTime");

	size_t iCurMobCount = SECTREE_MANAGER::Instance().GetMonsterCountInMap(pGuildDragonLair->GetMapIndex(), dwMobRaceNum);
	if (iCurMobCount < iMobCount)
	{
		size_t iSpawnCount = iMobCount - iCurMobCount;
		for (size_t iStoneNum = 0; iStoneNum < iSpawnCount; ++iStoneNum)
		{
			int iX = GuildDragonLair_GetIndexFactor2("PixelPosition", "Stone", (iStoneNum % GUILD_DRAGONLAIR_STONE_COUNT) + 1, "x");
			int iY = GuildDragonLair_GetIndexFactor2("PixelPosition", "Stone", (iStoneNum % GUILD_DRAGONLAIR_STONE_COUNT) + 1, "y");

			if (bStage == GUILD_DRAGONLAIR_STAGE1 && pEventInfo->bFirstSpawn)
			{
				iY += number(3, 10);
				pEventInfo->bFirstSpawn = false;
			}

			pGuildDragonLair->Spawn(dwMobRaceNum, iX, iY, 0);
		}
	}

	if (bStage == GUILD_DRAGONLAIR_STAGE3 && pEventInfo->bFirstSpawn)
	{
		pGuildDragonLair->SetBossKillCount(0);

		for (BYTE bStoneNum = 0; bStoneNum < GUILD_DRAGONLAIR_STONE_COUNT; ++bStoneNum)
		{
			int iX = GuildDragonLair_GetIndexFactor2("PixelPosition", "Stone", (bStoneNum + 1), "x");
			int iY = GuildDragonLair_GetIndexFactor2("PixelPosition", "Stone", (bStoneNum + 1), "y");

			iY += number(3, 10);

			pGuildDragonLair->Spawn(GuildDragonLair_GetFactor(2, pGuildDragonLair->GetDungeonType(), "BossVnum"), iX, iY, 0);
			pGuildDragonLair->Spawn(GuildDragonLair_GetFactor(2, pGuildDragonLair->GetDungeonType(), "KnightVnum"), iX, iY, 0);
			pGuildDragonLair->Spawn(GuildDragonLair_GetFactor(2, pGuildDragonLair->GetDungeonType(), "KnightVnum"), iX, iY, 0);
			pGuildDragonLair->Spawn(GuildDragonLair_GetFactor(2, pGuildDragonLair->GetDungeonType(), "KnightVnum"), iX, iY, 0);
		}

		pEventInfo->bFirstSpawn = false;
	}

	return PASSES_PER_SEC(iMobRespawnTime);
};

EVENTFUNC(GuildDragonLairEggEvent)
{
	GuildDragonLairEventInfo* pEventInfo = dynamic_cast<GuildDragonLairEventInfo*>(event->info);
	if (pEventInfo == NULL)
	{
		sys_err("GuildDragonLairEggEvent: <Factor> Null pointer.");
		return 0;
	}

	CGuildDragonLair* pGuildDragonLair = pEventInfo->pGuildDragonLair;
	if (pGuildDragonLair == NULL)
	{
		sys_err("GuildDragonLairEggEvent: <Factor> Null instance.");
		return 0;
	}

	LPSECTREE_MAP pSectree = pGuildDragonLair->GetSectree();
	if (pSectree == NULL)
	{
		sys_err("GuildDragonLairEggEvent: <Factor> Null sectree.");
		return 0;
	}

	const char* szDungeonType = pGuildDragonLair->GetDungeonType();
	const BYTE bStage = pGuildDragonLair->GetStage();

	if (bStage == GUILD_DRAGONLAIR_STAGE_NONE || bStage > GUILD_DRAGONLAIR_STAGE4)
		return 0;

	DWORD dwEggVnum = static_cast<DWORD>(GuildDragonLair_GetFactor(2, szDungeonType, "EggVnum"));
	size_t iRespawnTime = GuildDragonLair_GetIndexFactor2(szDungeonType, "Stage", bStage, "EggRespawnTime");

	if (bStage == GUILD_DRAGONLAIR_STAGE2 || bStage == GUILD_DRAGONLAIR_STAGE3 || bStage == GUILD_DRAGONLAIR_STAGE4)
	{
		size_t iEggCount = GuildDragonLair_GetFactor(1, "EggCount");
		size_t iCurEggCount = SECTREE_MANAGER::Instance().GetMonsterCountInMap(pGuildDragonLair->GetMapIndex(), dwEggVnum);
		if (iCurEggCount < iEggCount)
		{
			pGuildDragonLair->SetEggKillCount(0);

			size_t iSpawnCount = iEggCount - iCurEggCount;
			for (size_t iStoneNum = 0; iStoneNum < iSpawnCount; ++iStoneNum)
			{
				int iX = GuildDragonLair_GetIndexFactor2("PixelPosition", "Stone", (iStoneNum % GUILD_DRAGONLAIR_STONE_COUNT) + 1, "x");
				int iY = GuildDragonLair_GetIndexFactor2("PixelPosition", "Stone", (iStoneNum % GUILD_DRAGONLAIR_STONE_COUNT) + 1, "y");

				iY += number(3, 10);

				pGuildDragonLair->Spawn(dwEggVnum, iX, iY, 0);
			}

			pGuildDragonLair->LockStones();
		}
	}

	return PASSES_PER_SEC(iRespawnTime);
}

EVENTFUNC(GuildDragonLairExitEvent)
{
	GuildDragonLairExitEventInfo* pEventInfo = dynamic_cast<GuildDragonLairExitEventInfo*>(event->info);
	if (pEventInfo == NULL)
	{
		sys_err("GuildDragonLairExitEvent: <Factor> Null pointer.");
		return 0;
	}

	CGuildDragonLair* pGuildDragonLair = pEventInfo->pGuildDragonLair;
	if (pGuildDragonLair == NULL)
	{
		sys_err("GuildDragonLairExitEvent: <Factor> Null instance.");
		return 0;
	}

	if (pEventInfo->bStep == 0)
	{
		++pEventInfo->bStep;

		pGuildDragonLair->ExitAll();
		return PASSES_PER_SEC(5);
	}
	else
	{
		CGuildDragonLairManager::Instance().DestroyPrivateMap(pGuildDragonLair->GetMapIndex());
		return 0;
	}

	return 0;
}

struct FChatPacket
{
	FChatPacket(BYTE bChatType, const char* pszText) : m_bChatType(bChatType), m_pszText(pszText) {}
	void operator() (LPENTITY pEntity)
	{
		if (pEntity && pEntity->IsType(ENTITY_CHARACTER))
		{
			const LPCHARACTER pChar = dynamic_cast<LPCHARACTER>(pEntity);
			if (pChar && pChar->GetDesc())
			{
				pChar->ChatPacket(m_bChatType, "%s", m_pszText);
			}
		}
	}
	BYTE m_bChatType;
	const char* m_pszText;
};

struct FKillMonster
{
	FKillMonster() {};
	void operator () (LPENTITY pEntity)
	{
		if (pEntity && pEntity->IsType(ENTITY_CHARACTER))
		{
			const LPCHARACTER pChar = dynamic_cast<LPCHARACTER>(pEntity);
			if (pChar == NULL)
				return;

			const DWORD dwRaceNum = pChar->GetRaceNum();
			if (CGuildDragonLairManager::Instance().IsKing(dwRaceNum)
				|| CGuildDragonLairManager::Instance().IsStone(dwRaceNum))
				return;

			if (pChar->IsMonster() || pChar->IsStone())
				pChar->Dead();
		}
	}
};

struct FExitAll
{
	FExitAll() {}
	void operator()(LPENTITY pEntity)
	{
		if (pEntity && pEntity->IsType(ENTITY_CHARACTER))
		{
			const LPCHARACTER pChar = dynamic_cast<LPCHARACTER>(pEntity);
			if (pChar && pChar->GetDesc())
			{
				CGuildDragonLair* pGuildDragonLair = pChar->GetGuildDragonLair();
				if (pGuildDragonLair)
					pGuildDragonLair->Exit(pChar);
			}
		}
	}
};

#if defined(__GUILD_DRAGONLAIR_PARTY_SYSTEM__)
struct FGuildDragonLairPacket
{
	FGuildDragonLairPacket(BYTE bSubHeader, DWORD dwFirstRankingTime = 0) :
		m_bSubHeader(bSubHeader), m_dwFirstRankingTime(dwFirstRankingTime) {}
	void operator()(LPENTITY pEntity)
	{
		if (pEntity && pEntity->IsType(ENTITY_CHARACTER))
		{
			const LPCHARACTER pChar = dynamic_cast<LPCHARACTER>(pEntity);
			if (pChar != NULL && pChar->GetDesc())
			{
				TPacketGCGuildDragonLair GuildDragonLairPacket;
				GuildDragonLairPacket.bHeader = HEADER_GC_GUILD_DRAGONLAIR;
				GuildDragonLairPacket.wSize = sizeof(TPacketGCGuildDragonLair);
				GuildDragonLairPacket.bSubHeader = m_bSubHeader;
				GuildDragonLairPacket.dwFirstRankingTime = m_dwFirstRankingTime;
				pChar->GetDesc()->Packet(&GuildDragonLairPacket, sizeof(GuildDragonLairPacket));
			}
		}
	}
	BYTE m_bSubHeader;
	DWORD m_dwFirstRankingTime;
};

struct FPartySetQuestFlag
{
	FPartySetQuestFlag(const char* szFlagName, int iFlagValue) :
		m_szFlagName(szFlagName), m_iFlagValue(iFlagValue) {}
	void operator()(LPENTITY pEntity)
	{
		if (pEntity && pEntity->IsType(ENTITY_CHARACTER))
		{
			const LPCHARACTER pChar = dynamic_cast<LPCHARACTER>(pEntity);
			if (pChar && pChar->GetDesc() && pChar->GetParty())
			{
				pChar->SetQuestFlag(m_szFlagName, m_iFlagValue);
			}
		}
	}
	const char* m_szFlagName;
	int m_iFlagValue;
};
#endif

///////////////////////////////////////////////////////////////////////////////////////////
// CGuildDragonLair
///////////////////////////////////////////////////////////////////////////////////////////

CGuildDragonLair::CGuildDragonLair(BYTE bType, long lOriginalMapIndex, long lPrivateMapIndex)
	: m_bType(bType), m_lOriginalMapIndex(lOriginalMapIndex), m_lPrivateMapIndex(lPrivateMapIndex)
{
	m_bStage = GUILD_DRAGONLAIR_STAGE_NONE;
	m_pSectree = SECTREE_MANAGER::Instance().GetMap(m_lPrivateMapIndex);

	m_lCenterX = 0;
	m_lCenterY = 0;

	m_set_pChar.clear();

	m_set_dwPID_Exit.clear();
	m_set_dwPID_Stone.clear();
	m_set_dwPID_Reward.clear();

	m_set_dwStoneAffect.clear();

	m_pGuild = NULL;
#if defined(__GUILD_DRAGONLAIR_PARTY_SYSTEM__)
	m_pParty = NULL;
#endif

	m_map_UniqueMob.clear();
	m_bEggKillCount = 0;
	m_bBossKillCount = 0;

	m_dwStartTime = 0;
#if defined(__GUILD_DRAGONLAIR_PARTY_SYSTEM__)
	m_dwFirstRankingTime = 0;
#endif

	m_bSpecialAttack = false;
	m_szDungeonType = "";

	m_pEndEvent = NULL;
	m_pGateEvent = NULL;
	m_pStoneEvent = NULL;
	m_pSpawnEvent = NULL;
	m_pEggEvent = NULL;
	m_pExitEvent = NULL;
}

CGuildDragonLair::~CGuildDragonLair()
{
	CancelAllEvents();
}

void CGuildDragonLair::Initialize()
{
	if (m_pGuild) m_szDungeonType = "Guild";
#if defined(__GUILD_DRAGONLAIR_PARTY_SYSTEM__)
	else if (m_pParty) m_szDungeonType = "Party";
#endif

	LPCHARACTER pNPC = Spawn(GuildDragonLair_GetFactor(2, m_szDungeonType, "WatcherVnum"),
		GuildDragonLair_GetFactor(3, "PixelPosition", "OutsideWatcher", "x"),
		GuildDragonLair_GetFactor(3, "PixelPosition", "OutsideWatcher", "y"),
		GuildDragonLair_GetFactor(3, "PixelPosition", "OutsideWatcher", "dir"));

	if (pNPC != NULL)
		m_map_UniqueMob.insert(std::make_pair("NPC", pNPC));

	LPCHARACTER pGate = Spawn(GuildDragonLair_GetFactor(2, m_szDungeonType, "GateVnum"),
		GuildDragonLair_GetFactor(3, "PixelPosition", "Gate", "x"),
		GuildDragonLair_GetFactor(3, "PixelPosition", "Gate", "y"),
		GuildDragonLair_GetFactor(3, "PixelPosition", "Gate", "dir"));

	if (pGate != NULL)
		m_map_UniqueMob.insert(std::make_pair("Gate", pGate));
}

void CGuildDragonLair::Destroy()
{
	m_bStage = GUILD_DRAGONLAIR_STAGE_NONE;
	m_pSectree = NULL;

	m_lCenterX = 0;
	m_lCenterY = 0;

	m_set_pChar.clear();

	m_set_dwPID_Exit.clear();
	m_set_dwPID_Stone.clear();
	m_set_dwPID_Reward.clear();

	m_set_dwStoneAffect.clear();

	m_pGuild = NULL;
#if defined(__GUILD_DRAGONLAIR_PARTY_SYSTEM__)
	m_pParty = NULL;
#endif

	m_map_UniqueMob.clear();
	m_bEggKillCount = 0;
	m_bBossKillCount = 0;

	m_dwStartTime = 0;
#if defined(__GUILD_DRAGONLAIR_PARTY_SYSTEM__)
	m_dwFirstRankingTime = 0;
#endif

	m_bSpecialAttack = false;
	m_szDungeonType = "";

	m_pEndEvent = NULL;
	m_pGateEvent = NULL;
	m_pStoneEvent = NULL;
	m_pSpawnEvent = NULL;
	m_pEggEvent = NULL;
	m_pExitEvent = NULL;
}

bool CGuildDragonLair::Start()
{
	if (m_bStage != GUILD_DRAGONLAIR_STAGE_NONE)
	{
		sys_log(0, "CGuildDragonLair::Start(): Already started.");
		return false;
	}

	if (m_pSectree == NULL)
	{
		sys_err("CGuildDragonLair::Start(): Null sectree.");
		return false;
	}

	CancelAllEvents();

	GuildDragonLairEventInfo* pGateEventInfo = AllocEventInfo<GuildDragonLairEventInfo>();
	pGateEventInfo->pGuildDragonLair = this;
	SetGateEvent(event_create(GuildDragonLairGateEvent, pGateEventInfo, PASSES_PER_SEC(GuildDragonLair_GetFactor(2, "Event", "OpenGate"))));

	GuildDragonLairEventInfo* pEndEventInfo = AllocEventInfo<GuildDragonLairEventInfo>();
	pEndEventInfo->pGuildDragonLair = this;
	SetEndEvent(event_create(GuildDragonLairEndEvent, pEndEventInfo, PASSES_PER_SEC(GuildDragonLair_GetFactor(2, "Event", "Expire"))));

	m_dwStartTime = get_global_time();
#if defined(__GUILD_DRAGONLAIR_PARTY_SYSTEM__)
	SendFirstRankingTimeStart();
#endif

	LPCHARACTER pKing = Spawn(GuildDragonLair_GetFactor(2, m_szDungeonType, "KingVnum"),
		GuildDragonLair_GetFactor(3, "PixelPosition", "King", "x"),
		GuildDragonLair_GetFactor(3, "PixelPosition", "King", "y"),
		GuildDragonLair_GetFactor(3, "PixelPosition", "King", "dir"));

	if (pKing != NULL)
		m_map_UniqueMob.insert(std::make_pair("King", pKing));

	if (m_pGuild)
	{
		m_pGuild->Chat(LC_STRING("[Guild] The battle against Dragon Queen Meley is starting."));
		m_pGuild->Chat(LC_STRING("[Guild] The gates to Meley's Lair are open. The time limit in the dungeon is one hour."));
	}
#if defined(__GUILD_DRAGONLAIR_PARTY_SYSTEM__)
	else if (m_pParty)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_STRING("[Group] The battle has begun."));
		ChatPacket(CHAT_TYPE_INFO, LC_STRING("[Group] You have 1 hour to complete the dungeon."));
	}
#endif
	else
	{
		ExitAll();
		return false;
	}

	for (BYTE bStoneNum = 0; bStoneNum < GUILD_DRAGONLAIR_STONE_COUNT; ++bStoneNum)
	{
		LPCHARACTER pStone = Spawn(GuildDragonLair_GetFactor(2, m_szDungeonType, "StoneVnum"),
			GuildDragonLair_GetIndexFactor2("PixelPosition", "Stone", (bStoneNum + 1), "x"),
			GuildDragonLair_GetIndexFactor2("PixelPosition", "Stone", (bStoneNum + 1), "y"),
			GuildDragonLair_GetIndexFactor2("PixelPosition", "Stone", (bStoneNum + 1), "dir"));

		if (pStone != NULL)
		{
			pStone->SetRotationToXY(
				m_pSectree->m_setting.iBaseX + GuildDragonLair_GetFactor(3, "PixelPosition", "King", "x") * 100,
				m_pSectree->m_setting.iBaseY + GuildDragonLair_GetFactor(3, "PixelPosition", "King", "y") * 100);

			m_map_UniqueMob.insert(std::make_pair("Stone" + std::to_string(bStoneNum), pStone));
		}
	}

	SetStage(GUILD_DRAGONLAIR_STAGE1);
	return true;
}

bool CGuildDragonLair::Cancel()
{
	if (m_bStage >= GUILD_DRAGONLAIR_STAGE_REWARD)
		return false;

	CancelAllEvents();

	PurgeUnique("Gate");
	for (BYTE bStoneNum = 0; bStoneNum < GUILD_DRAGONLAIR_STONE_COUNT; ++bStoneNum)
		PurgeUnique("Stone" + std::to_string(bStoneNum));
	PurgeUnique("King");

#if defined(__GUILD_DRAGONLAIR_PARTY_SYSTEM__)
	SendFirstRankingTimeFailed();
#endif

	if (m_pGuild)
	{
		m_pGuild->Chat(LC_STRING("[Guild] You have cancelled the fight in Meley's Lair."));
		m_pGuild->Chat(LC_STRING("[Guild] The lair will close in %d min.", 1));
		m_pGuild->ChangeLadderPoint(GuildDragonLair_GetFactor(2, "Guild", "LadderPoint") / 2);
	}
#if defined(__GUILD_DRAGONLAIR_PARTY_SYSTEM__)
	else if (m_pParty)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_STRING("[Group] You have conceded."));
		ChatPacket(CHAT_TYPE_INFO, LC_STRING("[Group] You will be teleported to the city in 1 minute."));
	}
#endif
	else
	{
		ExitAll();
		return false;
	}

	GuildDragonLairExitEventInfo* pExitEventInfo = AllocEventInfo<GuildDragonLairExitEventInfo>();
	pExitEventInfo->pGuildDragonLair = this;
	SetExitEvent(event_create(GuildDragonLairExitEvent, pExitEventInfo, PASSES_PER_SEC(GuildDragonLair_GetFactor(2, "Event", "ExitFailure"))));

	return true;
}

bool CGuildDragonLair::End(bool bSuccess)
{
	CancelAllEvents();

#if defined(__GUILD_DRAGONLAIR_PARTY_SYSTEM__)
	if (bSuccess && CheckFirstRankingTimePassed())
		SendFirstRankingTimeSuccess();
	else
		SendFirstRankingTimeFailed();

	int iMinutes, iSeconds;
	GetElapsedTime(iMinutes, iSeconds);
#endif

	if (m_pGuild)
	{
		if (bSuccess)
		{
			m_pGuild->Chat(LC_STRING("[Guild] You have defeated Meley the Dragon Queen!"));
#if defined(__GUILD_DRAGONLAIR_PARTY_SYSTEM__)
			if (iMinutes != -1 && iSeconds != -1)
				m_pGuild->Chat(LC_STRING("[Guild] Required time: %d min. %d sec.", iMinutes, iSeconds));
#endif
			m_pGuild->Chat(LC_STRING("[Guild] For your own safety, you will be teleported away from the nest in 10 minutes."));
			m_pGuild->ChangeLadderPoint(GuildDragonLair_GetFactor(2, "Guild", "LadderPoint"));

			RegisterGuildRanking(m_bType, m_pGuild->GetID(), static_cast<BYTE>(m_set_pChar.size()), get_global_time() - m_dwStartTime);
		}
		else
		{
			m_pGuild->Chat(LC_STRING("[Guild] Time's up."));
			m_pGuild->Chat(LC_STRING("[Guild] The gates to the lair are closed once more."));
			m_pGuild->Chat(LC_STRING("[Guild] For your own safety, you will be teleported away from the nest in 1 minute."));
			m_pGuild->ChangeLadderPoint(GuildDragonLair_GetFactor(2, "Guild", "LadderPoint") / 2);
		}
	}
#if defined(__GUILD_DRAGONLAIR_PARTY_SYSTEM__)
	else if (m_pParty)
	{
		if (bSuccess)
		{
			ChatPacket(CHAT_TYPE_INFO, LC_STRING("[Group] You have completed your task with distinction!"));
			if (iMinutes != -1 && iSeconds != -1)
				ChatPacket(CHAT_TYPE_INFO, LC_STRING("[Group] Time until teleportation: %d min. %d sec.", iMinutes, iSeconds));
			ChatPacket(CHAT_TYPE_INFO, LC_STRING("[Group] You will be transported from the dungeon in 10 minutes."));

#if defined(__RANKING_SYSTEM__)
			TRankingData RankingData = {};
			RankingData.dwRecord0 = std::time(NULL) - GetStartTime();
			RankingData.dwStartTime = GetStartTime();
			CRanking::Register(m_pParty->GetLeaderPID(), CRanking::TYPE_RK_PARTY, CRanking::PARTY_RK_CATEGORY_GUILD_DRAGONLAIR_RED_ALL, RankingData);
#endif
		}
		else
		{
			ChatPacket(CHAT_TYPE_INFO, LC_STRING("[Group] Your time in the dungeon is up."));
			ChatPacket(CHAT_TYPE_INFO, LC_STRING("[Group] You will be teleported to the city in 1 minute."));
		}
	}
#endif
	else
	{
		ExitAll();
		return false;
	}

	GuildDragonLairExitEventInfo* pExitEventInfo = AllocEventInfo<GuildDragonLairExitEventInfo>();
	pExitEventInfo->pGuildDragonLair = this;
	if (bSuccess)
		SetExitEvent(event_create(GuildDragonLairExitEvent, pExitEventInfo, PASSES_PER_SEC(GuildDragonLair_GetFactor(2, "Event", "ExitSuccess"))));
	else
		SetExitEvent(event_create(GuildDragonLairExitEvent, pExitEventInfo, PASSES_PER_SEC(GuildDragonLair_GetFactor(2, "Event", "ExitFailure"))));

	return true;
}

void CGuildDragonLair::IncMember(LPCHARACTER pChar)
{
	if (pChar == NULL)
		return;

	for (BYTE bStoneNum = 0; bStoneNum < GUILD_DRAGONLAIR_STONE_COUNT; ++bStoneNum)
	{
		LPCHARACTER pStone = GetUnique("Stone" + std::to_string(bStoneNum));
		if (pStone != NULL)
		{
			const CAffect* pAffect = pStone->FindAffect(AFFECT_RED_DRAGONLAIR_STONE);
			if (pAffect && pAffect->lApplyValue)
				pStone->EffectPacket(pAffect->lApplyValue);
		}
	}

	if (m_set_pChar.find(pChar) == m_set_pChar.end())
		m_set_pChar.emplace(pChar);

	if (m_pSectree && m_bStage > GUILD_DRAGONLAIR_STAGE1)
	{
		pChar->Show(m_lPrivateMapIndex,
			m_pSectree->m_setting.iBaseX + GuildDragonLair_GetFactor(3, "PixelPosition", "Inside", "x") * 100,
			m_pSectree->m_setting.iBaseY + GuildDragonLair_GetFactor(3, "PixelPosition", "Inside", "y") * 100,
			GuildDragonLair_GetFactor(3, "PixelPosition", "Inside", "z"));
		pChar->Stop();
	}

#if defined(__GUILD_DRAGONLAIR_PARTY_SYSTEM__)
	SendFirstRankingTime(pChar);
#endif
}

void CGuildDragonLair::DecMember(LPCHARACTER pChar)
{
	if (pChar == NULL)
		return;

	TCharacterSet::iterator it = m_set_pChar.find(pChar);
	if (it == m_set_pChar.end())
		return;

	if (m_bStage != GUILD_DRAGONLAIR_STAGE_NONE)
	{
		const DWORD dwPID = pChar->GetPlayerID();
		if (m_set_dwPID_Exit.find(dwPID) == m_set_dwPID_Exit.end())
			m_set_dwPID_Exit.emplace(dwPID);
	}

	m_set_pChar.erase(it);
}

bool CGuildDragonLair::Exit(LPCHARACTER pChar)
{
	if (pChar == NULL)
		return false;

	const LPSECTREE_MAP pSectree = SECTREE_MANAGER::instance().GetMap(MAP_N_FLAME_01);
	if (pSectree)
	{
		return pChar->WarpSet(
			pSectree->m_setting.iBaseX + GuildDragonLair_GetFactor(3, "PixelPosition", "Outside", "x") * 100,
			pSectree->m_setting.iBaseY + GuildDragonLair_GetFactor(3, "PixelPosition", "Outside", "y") * 100,
			MAP_N_FLAME_01);
	}
	else
	{
		sys_err("CGuildDragonLair::Exit: Null sectree.");
		return false;
	}
}

void CGuildDragonLair::ExitAll()
{
	if (m_pSectree != NULL)
	{
		FExitAll fExitAll;
		m_pSectree->for_each(fExitAll);
	}
}

void CGuildDragonLair::ClearStage()
{
	SetSpawnEvent(NULL);
	SetEggEvent(NULL);

	if (m_pSectree != NULL)
	{
		FKillMonster fKillMonster;
		m_pSectree->for_each(fKillMonster);
	}
}

void CGuildDragonLair::SetStage(BYTE bStage)
{
	if (bStage == m_bStage)
		return;

	m_bStage = bStage;
	m_set_dwStoneAffect.clear();
	m_bSpecialAttack = false;

	switch (bStage)
	{
		case GUILD_DRAGONLAIR_STAGE1:
		{
			GuildDragonLairSpawnEventInfo* pSpawnEventInfo = AllocEventInfo<GuildDragonLairSpawnEventInfo>();
			pSpawnEventInfo->pGuildDragonLair = this;
			SetSpawnEvent(event_create(GuildDragonLairSpawnEvent, pSpawnEventInfo, PASSES_PER_SEC(1)));
		}
		break;

		case GUILD_DRAGONLAIR_STAGE2:
		{
			GuildDragonLairSpawnEventInfo* pSpawnEventInfo = AllocEventInfo<GuildDragonLairSpawnEventInfo>();
			pSpawnEventInfo->pGuildDragonLair = this;
			SetSpawnEvent(event_create(GuildDragonLairSpawnEvent, pSpawnEventInfo, PASSES_PER_SEC(1)));

			GuildDragonLairEventInfo* pEggEventInfo = AllocEventInfo<GuildDragonLairEventInfo>();
			pEggEventInfo->pGuildDragonLair = this;
			SetEggEvent(event_create(GuildDragonLairEggEvent, pEggEventInfo, PASSES_PER_SEC(1)));

			if (GetUnique("Gate") == NULL)
			{
				LPCHARACTER pGate = Spawn(GuildDragonLair_GetFactor(2, m_szDungeonType, "GateVnum"),
					GuildDragonLair_GetFactor(3, "PixelPosition", "Gate", "x"),
					GuildDragonLair_GetFactor(3, "PixelPosition", "Gate", "y"),
					GuildDragonLair_GetFactor(3, "PixelPosition", "Gate", "dir"));

				if (pGate != NULL)
					m_map_UniqueMob.insert(std::make_pair("Gate", pGate));
			}

			const LPCHARACTER pNPC = GetUnique("NPC");
			if (pNPC && pNPC->GetMapIndex() == m_lPrivateMapIndex)
			{
				pNPC->Show(m_lPrivateMapIndex,
					m_pSectree->m_setting.iBaseX + GuildDragonLair_GetFactor(3, "PixelPosition", "InsideWatcher", "x") * 100,
					m_pSectree->m_setting.iBaseY + GuildDragonLair_GetFactor(3, "PixelPosition", "InsideWatcher", "y") * 100,
					GuildDragonLair_GetFactor(3, "PixelPosition", "InsideWatcher", "z"));

				pNPC->SetRotation(GuildDragonLair_GetFactor(3, "PixelPosition", "InsideWatcher", "dir"));
				pNPC->Stop();
			}
		}
		break;

		case GUILD_DRAGONLAIR_STAGE3:
		{
			GuildDragonLairSpawnEventInfo* pSpawnEventInfo = AllocEventInfo<GuildDragonLairSpawnEventInfo>();
			pSpawnEventInfo->pGuildDragonLair = this;
			SetSpawnEvent(event_create(GuildDragonLairSpawnEvent, pSpawnEventInfo, PASSES_PER_SEC(1)));

			GuildDragonLairEventInfo* pEggEventInfo = AllocEventInfo<GuildDragonLairEventInfo>();
			pEggEventInfo->pGuildDragonLair = this;
			SetEggEvent(event_create(GuildDragonLairEggEvent, pEggEventInfo, PASSES_PER_SEC(1)));
		}
		break;

		case GUILD_DRAGONLAIR_STAGE4:
		{
			m_bEggKillCount = GUILD_DRAGONLAIR_STONE_COUNT;

			GuildDragonLairSpawnEventInfo* pSpawnEventInfo = AllocEventInfo<GuildDragonLairSpawnEventInfo>();
			pSpawnEventInfo->pGuildDragonLair = this;
			SetSpawnEvent(event_create(GuildDragonLairSpawnEvent, pSpawnEventInfo, PASSES_PER_SEC(10)));

			GuildDragonLairEventInfo* pEggEventInfo = AllocEventInfo<GuildDragonLairEventInfo>();
			pEggEventInfo->pGuildDragonLair = this;
			SetEggEvent(event_create(GuildDragonLairEggEvent, pEggEventInfo, PASSES_PER_SEC(10)));
		}
		break;

		case GUILD_DRAGONLAIR_STAGE_REWARD:
		{
			ClearStage();

			PurgeUnique("Gate");
			for (BYTE bStoneNum = 0; bStoneNum < GUILD_DRAGONLAIR_STONE_COUNT; ++bStoneNum)
				KillUnique("Stone" + std::to_string(bStoneNum));
			KillUnique("King");

			if (End(true))
			{
				Spawn(GuildDragonLair_GetFactor(2, m_szDungeonType, "ChestVnum"),
					GuildDragonLair_GetFactor(3, "PixelPosition", "Center", "x"),
					GuildDragonLair_GetFactor(3, "PixelPosition", "Center", "y"),
					GuildDragonLair_GetFactor(3, "PixelPosition", "Center", "dir"));
			}
		}
	}
}

void CGuildDragonLair::DeadCharacter(LPCHARACTER pChar)
{
	TUniqueMobMap::iterator it = m_map_UniqueMob.begin();
	while (it != m_map_UniqueMob.end())
	{
		if (it->second == pChar)
		{
			//sys_log(0, "CGuildDragonLair::DeadCharacter(pChar=%p) Dead Unique %s", it->first.c_str());
			m_map_UniqueMob.erase(it);
			break;
		}
		++it;
	}
}

LPCHARACTER CGuildDragonLair::GetUnique(const std::string& strKey) const
{
	TUniqueMobMap::const_iterator it = m_map_UniqueMob.find(strKey);
	if (it == m_map_UniqueMob.end())
	{
		sys_err("CGuildDragonLair::GetUnique(strKey=%s): Unknown Key or Dead!", strKey.c_str());
		return NULL;
	}
	return it->second;
}

bool CGuildDragonLair::IsUnique(LPCHARACTER pChar) const
{
	TUniqueMobMap::const_iterator it = m_map_UniqueMob.begin();
	while (it != m_map_UniqueMob.end())
	{
		if (it->second == pChar)
			return true;

		++it;
	}
	return false;
}

void CGuildDragonLair::KillUnique(const std::string& strKey)
{
	TUniqueMobMap::iterator it = m_map_UniqueMob.find(strKey);
	if (it == m_map_UniqueMob.end())
	{
		sys_err("CGuildDragonLair::KillUnique(strKey=%s): Unknown Key or Dead!", strKey.c_str());
		return;
	}

	LPCHARACTER pChar = it->second;
	if (pChar != NULL)
	{
		if (strKey.find("Stone") != std::string::npos)
		{
			pChar->RemoveAffect(AFFECT_RED_DRAGONLAIR_STONE);
			pChar->ViewReencode();
		}
		pChar->Dead();
	}

	m_map_UniqueMob.erase(it);
}

void CGuildDragonLair::PurgeUnique(const std::string& strKey)
{
	TUniqueMobMap::iterator it = m_map_UniqueMob.find(strKey);
	if (it == m_map_UniqueMob.end())
	{
		sys_err("CGuildDragonLair::PurgeUnique(strKey=%s): Unknown Key or Dead!", strKey.c_str());
		return;
	}

	LPCHARACTER pChar = it->second;
	m_map_UniqueMob.erase(it);

	M2_DESTROY_CHARACTER(pChar);
}

void CGuildDragonLair::SetStoneAffect(LPCHARACTER pStone, BYTE bEffectNum, bool bSave)
{
	if (pStone != NULL)
	{
		pStone->AddAffect(AFFECT_RED_DRAGONLAIR_STONE, POINT_NONE, bEffectNum, AFF_UNBEATABLE, INFINITE_AFFECT_DURATION, 0, true);
		//pStone->ViewReencode();
		pStone->EffectPacket(bEffectNum);

		if (bSave)
			m_set_dwStoneAffect[pStone->GetVID()] = bEffectNum;
	}
}

bool CGuildDragonLair::GetStoneAffect(LPCHARACTER pStone, BYTE bEffectNum) const
{
	CAffect* pAffect = pStone ? pStone->FindAffect(AFFECT_RED_DRAGONLAIR_STONE) : NULL;
	return (pAffect && pAffect->lApplyValue == bEffectNum);
}

size_t CGuildDragonLair::GetAllStoneAffectCount(BYTE bEffectNum) const
{
	size_t nCount = 0;
	for (BYTE bStoneNum = 0; bStoneNum < GUILD_DRAGONLAIR_STONE_COUNT; ++bStoneNum)
	{
		LPCHARACTER pStone = GetUnique("Stone" + std::to_string(bStoneNum));
		if (pStone && pStone->IsAffectFlag(AFF_UNBEATABLE))
		{
			if (GetStoneAffect(pStone, bEffectNum))
				++nCount;
		}
	}
	return nCount;
}

void CGuildDragonLair::LockStones()
{
	for (BYTE bStoneNum = 0; bStoneNum < GUILD_DRAGONLAIR_STONE_COUNT; ++bStoneNum)
	{
		LPCHARACTER pStone = GetUnique("Stone" + std::to_string(bStoneNum));
		if (pStone != NULL)
		{
			const DWORD dwVID = pStone->GetVID();
			if (m_set_dwStoneAffect.find(dwVID) != m_set_dwStoneAffect.end())
			{
				if (m_set_dwStoneAffect[dwVID] == SE_DRAGONLAIR_STONE_UNBEATABLE_1)
					continue;

				if (m_set_dwStoneAffect[dwVID] == SE_DRAGONLAIR_STONE_UNBEATABLE_2 ||
					m_set_dwStoneAffect[dwVID] == SE_DRAGONLAIR_STONE_UNBEATABLE_3)
					SetStoneAffect(pStone, SE_DRAGONLAIR_STONE_UNBEATABLE_2, true);
			}
			else
			{
				const CAffect* pAffect = pStone->FindAffect(AFFECT_RED_DRAGONLAIR_STONE);
				if (pAffect != NULL)
				{
					if (pAffect->lApplyValue == SE_DRAGONLAIR_STONE_UNBEATABLE_2 ||
						pAffect->lApplyValue == SE_DRAGONLAIR_STONE_UNBEATABLE_3)
					{
						SetStoneAffect(pStone, SE_DRAGONLAIR_STONE_UNBEATABLE_2, true);
						continue;
					}
				}
				SetStoneAffect(pStone, SE_DRAGONLAIR_STONE_UNBEATABLE_1, false);
			}
		}
	}

	m_set_dwPID_Stone.clear();
}

void CGuildDragonLair::UnlockStones()
{
	for (BYTE bStoneNum = 0; bStoneNum < GUILD_DRAGONLAIR_STONE_COUNT; ++bStoneNum)
	{
		LPCHARACTER pStone = GetUnique("Stone" + std::to_string(bStoneNum));
		if (pStone != NULL)
		{
			const DWORD dwVID = pStone->GetVID();
			if (m_set_dwStoneAffect.find(dwVID) != m_set_dwStoneAffect.end())
			{
				SetStoneAffect(pStone, m_set_dwStoneAffect[dwVID]);
			}
			else
			{
				pStone->RemoveAffect(AFFECT_RED_DRAGONLAIR_STONE);
				pStone->ViewReencode();
			}
		}
	}
}

bool CGuildDragonLair::Damage(LPCHARACTER pVictim)
{
	if (m_bStage == GUILD_DRAGONLAIR_STAGE_NONE || m_bStage > GUILD_DRAGONLAIR_STAGE4)
		return false;

	if (pVictim == NULL)
		return false;

	if (pVictim->IsAffectFlag(AFF_UNBEATABLE))
		return false;

	int iHP = 0;
	if (m_bStage >= GUILD_DRAGONLAIR_STAGE1 && m_bStage <= GUILD_DRAGONLAIR_STAGE3)
		iHP = ((pVictim->GetMaxHP() * GuildDragonLair_GetIndexFactor2(m_szDungeonType, "Stage", m_bStage, "StoneHealthPct")) / 100);

	if (pVictim->GetHP() <= iHP)
	{
		pVictim->SetHP(iHP);

		if (m_bStage == GUILD_DRAGONLAIR_STAGE1 || m_bStage == GUILD_DRAGONLAIR_STAGE2)
		{
			SetStoneAffect(pVictim, SE_DRAGONLAIR_STONE_UNBEATABLE_1, /* bSave */ true);

			if (GetAllStoneAffectCount(SE_DRAGONLAIR_STONE_UNBEATABLE_1) >= GUILD_DRAGONLAIR_STONE_COUNT)
			{
				ClearStage();
				SetSpecialAttack(true);

				GuildDragonLairStoneEventInfo* pStoneEventInfo = AllocEventInfo<GuildDragonLairStoneEventInfo>();
				pStoneEventInfo->pGuildDragonLair = this;
				SetStoneEvent(event_create(GuildDragonLairStoneEvent, pStoneEventInfo, PASSES_PER_SEC(GuildDragonLair_GetFactor(2, "Event", "SpecialAttack"))));
			}
		}
		else if (m_bStage == GUILD_DRAGONLAIR_STAGE3)
		{
			SetStoneAffect(pVictim, SE_DRAGONLAIR_STONE_UNBEATABLE_2, /* bSave */ true);

			if (GetAllStoneAffectCount(SE_DRAGONLAIR_STONE_UNBEATABLE_2) >= GUILD_DRAGONLAIR_STONE_COUNT)
			{
				ClearStage();

				SetStage(m_bStage + 1);
			}
		}

		return false;
	}

	return true;
}

void CGuildDragonLair::Kill(LPCHARACTER pVictim)
{
	if (pVictim == NULL)
		return;

	if (m_bStage == GUILD_DRAGONLAIR_STAGE_NONE || m_bStage > GUILD_DRAGONLAIR_STAGE4)
		return;

	const DWORD dwRaceNum = pVictim->GetRaceNum();
	if (CGuildDragonLairManager::Instance().IsEgg(dwRaceNum))
	{
		const BYTE bEggCount = static_cast<BYTE>(GuildDragonLair_GetFactor(1, "EggCount"));
		const BYTE bBossCount = static_cast<BYTE>(GuildDragonLair_GetFactor(1, "BossCount"));

		if (m_bEggKillCount < bEggCount)
			m_bEggKillCount += 1;

		if (m_bEggKillCount >= bEggCount)
		{
			if (m_bStage == GUILD_DRAGONLAIR_STAGE3 && m_bBossKillCount >= bBossCount)
			{
				UnlockStones();
			}
			else if (m_bStage == GUILD_DRAGONLAIR_STAGE2 || m_bStage > GUILD_DRAGONLAIR_STAGE3)
			{
				UnlockStones();
			}
		}
	}
	else if (CGuildDragonLairManager::Instance().IsBoss(dwRaceNum))
	{
		const BYTE bEggCount = static_cast<BYTE>(GuildDragonLair_GetFactor(1, "EggCount"));
		const BYTE bBossCount = static_cast<BYTE>(GuildDragonLair_GetFactor(1, "BossCount"));

		if (m_bBossKillCount < bBossCount)
			m_bBossKillCount += 1;

		if (m_bStage == GUILD_DRAGONLAIR_STAGE3)
		{
			if (m_bEggKillCount >= bEggCount &&
				m_bBossKillCount >= bBossCount)
			{
				UnlockStones();
			}
		}
	}
	else if (GuildDragonLair_GetIndexFactor2(m_szDungeonType, "Stage", m_bStage, "MonsterVnum") == dwRaceNum)
	{
		if (number(1, 100) <= GuildDragonLair_GetIndexFactor2(m_szDungeonType, "Stage", m_bStage, "DropPct"))
		{
			LPITEM pItem = ITEM_MANAGER::Instance().CreateItem(ITEM_GUILD_DRAGONLAIR_RING);
			if (pItem == NULL)
				return;

			PIXEL_POSITION DropPos;
			DropPos.x = pVictim->GetX();
			DropPos.y = pVictim->GetY();

			pItem->AddToGround(GetMapIndex(), DropPos);
			pItem->StartDestroyEvent();
		}
	}
}

void CGuildDragonLair::TakeItem(LPCHARACTER pFrom, LPCHARACTER pTo, LPITEM pItem)
{
	if (pFrom == NULL || pTo == NULL || pItem == NULL)
		return;

	if (m_bStage != GUILD_DRAGONLAIR_STAGE4)
		return;

	if (GetStoneAffect(pTo, SE_DRAGONLAIR_STONE_UNBEATABLE_2) == false)
		return;

	if (m_bEggKillCount < GUILD_DRAGONLAIR_STONE_COUNT)
		return;

	const DWORD dwPID = pFrom->GetPlayerID();
	if (m_set_dwPID_Stone.find(dwPID) != m_set_dwPID_Stone.end() && pFrom->GetGMLevel() == GM_PLAYER)
		return;

	m_set_dwPID_Stone.emplace(dwPID);

	ITEM_MANAGER::Instance().RemoveItem(pItem, "GUILD_DRAGONLAIR_STONE");

	SetStoneAffect(pTo, SE_DRAGONLAIR_STONE_UNBEATABLE_3, /*bSave*/ true);
	if (GetAllStoneAffectCount(SE_DRAGONLAIR_STONE_UNBEATABLE_3) >= GUILD_DRAGONLAIR_STONE_COUNT)
	{
		SetStage(m_bStage + 1);
	}
}

bool CGuildDragonLair::GiveReward(LPCHARACTER pChar, DWORD dwVnum)
{
	if (pChar == NULL)
		return false;

	if (m_bStage != GUILD_DRAGONLAIR_STAGE_REWARD)
	{
		sys_log(0, "CGuildDragonLair::GiveReward(pChar=%p, dwVnum=%u): %s trying to get a reward on stage %u.",
			pChar, dwVnum, pChar->GetName(), m_bStage);
		return false;
	}

	const DWORD dwPID = pChar->GetPlayerID();
	if (m_set_dwPID_Reward.find(dwPID) != m_set_dwPID_Reward.end())
		return false;

	LPITEM pRewardItem = pChar->AutoGiveItem(dwVnum, 1, -1, true
#if defined(__WJ_PICKUP_ITEM_EFFECT__)
		, true
#endif
	);

	if (pRewardItem != NULL)
	{
		m_set_dwPID_Reward.emplace(dwPID);
	}

	return true;
}

bool CGuildDragonLair::GetReward(LPCHARACTER pChar)
{
	if (pChar == NULL)
		return false;

	const DWORD dwPID = pChar->GetPlayerID();
	if (m_set_dwPID_Reward.find(dwPID) != m_set_dwPID_Reward.end())
		return true;

	return false;
}

void CGuildDragonLair::ChatPacket(BYTE bChatType, const char* szText)
{
	if (m_pSectree != NULL)
	{
		FChatPacket fChatPacket(bChatType, szText);
		m_pSectree->for_each(fChatPacket);
	}
}

void CGuildDragonLair::SetEndEvent(LPEVENT pEvent)
{
	if (m_pEndEvent != NULL)
		event_cancel(&m_pEndEvent);

	m_pEndEvent = pEvent;
}

void CGuildDragonLair::SetGateEvent(LPEVENT pEvent)
{
	if (m_pGateEvent != NULL)
		event_cancel(&m_pGateEvent);

	m_pGateEvent = pEvent;
}

void CGuildDragonLair::SetStoneEvent(LPEVENT pEvent)
{
	if (m_pStoneEvent != NULL)
		event_cancel(&m_pStoneEvent);

	m_pStoneEvent = pEvent;
}

void CGuildDragonLair::SetSpawnEvent(LPEVENT pEvent)
{
	if (m_pSpawnEvent != NULL)
		event_cancel(&m_pSpawnEvent);

	m_pSpawnEvent = pEvent;
}

void CGuildDragonLair::SetEggEvent(LPEVENT pEvent)
{
	if (m_pEggEvent != NULL)
		event_cancel(&m_pEggEvent);

	m_pEggEvent = pEvent;
}

void CGuildDragonLair::SetExitEvent(LPEVENT pEvent)
{
	if (m_pExitEvent != NULL)
		event_cancel(&m_pExitEvent);

	m_pExitEvent = pEvent;
}

void CGuildDragonLair::CancelAllEvents()
{
	SetEndEvent(NULL);
	SetGateEvent(NULL);
	SetStoneEvent(NULL);
	SetSpawnEvent(NULL);
	SetEggEvent(NULL);
	SetExitEvent(NULL);
}

LPCHARACTER CGuildDragonLair::Spawn(DWORD dwVnum, int iX, int iY, int iDir, bool bSpawnMotion)
{
	if (m_pSectree == NULL)
		return NULL;

	const int iBaseX = m_pSectree->m_setting.iBaseX;
	const int iBaseY = m_pSectree->m_setting.iBaseY;

	if (CGuildDragonLairManager::Instance().IsKing(dwVnum))
	{
		SetCenterPos(
			iBaseX + GuildDragonLair_GetFactor(3, "PixelPosition", "Center", "x") * 100,
			iBaseY + GuildDragonLair_GetFactor(3, "PixelPosition", "Center", "y") * 100);
	}

	LPCHARACTER pMob = CHARACTER_MANAGER::Instance().SpawnMob(dwVnum, m_lPrivateMapIndex,
		iBaseX + iX * 100, iBaseY + iY * 100, 0, bSpawnMotion, iDir == 0 ? -1 : (iDir - 1) * 45);

	if (pMob)
	{
		pMob->SetAggressive();
		pMob->SetGuildDragonLair(this);
	}

	return pMob;
}

void CGuildDragonLair::RegisterGuildRanking(BYTE bType, DWORD dwGuildID, BYTE bMemberCount, DWORD dwTime)
{
	char szQuery[1024];
	snprintf(szQuery, sizeof(szQuery),
		"INSERT INTO `guild_dragonlair_ranking%s` (`type`, `guild_id`, `member_count`, `time`)"
		" VALUES (%u, %u, %u, %u)"
		" ON DUPLICATE KEY UPDATE"
		" `member_count` = VALUES(`member_count`),"
		" `time` = IF(VALUES(`time`) < `time`, VALUES(`time`), `time`);"
		, get_table_postfix()
		, bType
		, dwGuildID
		, bMemberCount
		, dwTime
	);
	DBManager::instance().DirectQuery(szQuery);
}

#if defined(__GUILD_DRAGONLAIR_PARTY_SYSTEM__)
void CGuildDragonLair::GetElapsedTime(int& iMinutes, int& iSeconds)
{
	if (m_dwStartTime)
	{
		const DWORD dwElapsedTime = get_global_time() - m_dwStartTime;

		iMinutes = (dwElapsedTime / 60) % 60;
		iSeconds = dwElapsedTime % 60;
	}
	else
	{
		iMinutes = -1;
		iSeconds = -1;
	}
}

bool CGuildDragonLair::CheckFirstRankingTimePassed()
{
	if (m_dwFirstRankingTime == 0)
		return true;

	if (m_dwStartTime && (get_global_time() - m_dwStartTime < m_dwFirstRankingTime))
		return true;

	return false;
}

void CGuildDragonLair::SendFirstRankingTimeStart()
{
	if (m_pSectree != NULL)
	{
		FGuildDragonLairPacket fStart(GUILD_DRAGONLAIR_GC_SUBHEADER_START);
		m_pSectree->for_each(fStart);
	}
}

void CGuildDragonLair::SendFirstRankingTimeSuccess()
{
	if (m_pSectree != NULL)
	{
		FGuildDragonLairPacket fSuccess(GUILD_DRAGONLAIR_GC_SUBHEADER_SUCCESS);
		m_pSectree->for_each(fSuccess);
	}
}

void CGuildDragonLair::SendFirstRankingTimeFailed()
{
	if (m_pSectree != NULL)
	{
		FGuildDragonLairPacket fSet(GUILD_DRAGONLAIR_GC_SUBHEADER_1ST_GUILD_TEXT, 1);
		m_pSectree->for_each(fSet);

		FGuildDragonLairPacket fStart(GUILD_DRAGONLAIR_GC_SUBHEADER_START);
		m_pSectree->for_each(fStart);
	}
}

void CGuildDragonLair::SendFirstRankingTime(DWORD dwFirstRankingTime)
{
	if (m_pSectree != NULL)
	{
		FGuildDragonLairPacket fSet(GUILD_DRAGONLAIR_GC_SUBHEADER_1ST_GUILD_TEXT, dwFirstRankingTime);
		m_pSectree->for_each(fSet);
	}
}

void CGuildDragonLair::SendFirstRankingTime(LPCHARACTER pChar)
{
	if (pChar == NULL)
		return;

	const LPDESC pDesc = pChar->GetDesc();
	if (pDesc == NULL)
		return;

	const DWORD dwStartTime = m_dwStartTime;
	const DWORD dwFirstRankingTime = m_dwFirstRankingTime;
	const DWORD dwElapsedTime = get_global_time() - m_dwStartTime;

	TPacketGCGuildDragonLair GuildDragonLairPacket;
	GuildDragonLairPacket.bHeader = HEADER_GC_GUILD_DRAGONLAIR;
	GuildDragonLairPacket.wSize = sizeof(TPacketGCGuildDragonLair);

	if (dwFirstRankingTime)
	{
		GuildDragonLairPacket.bSubHeader = GUILD_DRAGONLAIR_GC_SUBHEADER_1ST_GUILD_TEXT;
		if (dwStartTime)
		{
			if (get_global_time() - dwStartTime < dwFirstRankingTime)
				GuildDragonLairPacket.dwFirstRankingTime = dwFirstRankingTime - dwElapsedTime;
			else
				GuildDragonLairPacket.dwFirstRankingTime = 1;
		}
		else
			GuildDragonLairPacket.dwFirstRankingTime = dwFirstRankingTime;
		pDesc->Packet(&GuildDragonLairPacket, sizeof(TPacketGCGuildDragonLair));

		if (m_dwStartTime)
		{
			if (m_bStage == GUILD_DRAGONLAIR_STAGE_REWARD)
			{
				GuildDragonLairPacket.bSubHeader = GUILD_DRAGONLAIR_GC_SUBHEADER_SUCCESS;
				pDesc->Packet(&GuildDragonLairPacket, sizeof(TPacketGCGuildDragonLair));
			}
			else
			{
				GuildDragonLairPacket.bSubHeader = GUILD_DRAGONLAIR_GC_SUBHEADER_START;
				pDesc->Packet(&GuildDragonLairPacket, sizeof(TPacketGCGuildDragonLair));
			}
		}
	}
	else
	{
		GuildDragonLairPacket.bSubHeader = GUILD_DRAGONLAIR_GC_SUBHEADER_1ST_GUILD_TEXT;
		GuildDragonLairPacket.dwFirstRankingTime = 0;
		pDesc->Packet(&GuildDragonLairPacket, sizeof(TPacketGCGuildDragonLair));
	}
}
#endif

///////////////////////////////////////////////////////////////////////////////
// CGuildDragonLairManager
///////////////////////////////////////////////////////////////////////////////////////////

CGuildDragonLairManager::CGuildDragonLairManager()
{
}

CGuildDragonLairManager::~CGuildDragonLairManager()
{
}

CGuildDragonLair* CGuildDragonLairManager::Create(BYTE bType, long lOriginalMapIndex)
{
	const long lPrivateMapIndex = SECTREE_MANAGER::Instance().CreatePrivateMap(lOriginalMapIndex);
	if (lPrivateMapIndex == 0)
	{
		sys_log(0, "CGuildDragonLairManager::Create(bType=%u, lOriginalMapIndex=%ld) : Failed to create private map %ld.",
			bType, lOriginalMapIndex, lPrivateMapIndex);
		return NULL;
	}

	CGuildDragonLair* pGuildDragonLair = M2_NEW CGuildDragonLair(bType, lOriginalMapIndex, lPrivateMapIndex);
	if (pGuildDragonLair)
	{
		m_map_pMapDragonLair.emplace(lPrivateMapIndex, pGuildDragonLair);
		return pGuildDragonLair;
	}

	return NULL;
}

CGuildDragonLair* CGuildDragonLairManager::FindByMapIndex(long lMapIndex) const
{
	TMapDragonLair::const_iterator it = m_map_pMapDragonLair.find(lMapIndex);
	return (it != m_map_pMapDragonLair.end()) ? it->second : NULL;
}

void CGuildDragonLairManager::Destroy()
{
	for (TMapDragonLair::iterator it = m_map_pMapDragonLair.begin();
		it != m_map_pMapDragonLair.end(); )
	{
		CGuildDragonLair* pGuildDragonLair = it->second;
		if (pGuildDragonLair != NULL)
		{
			SECTREE_MANAGER::Instance().DestroyPrivateMap(pGuildDragonLair->GetMapIndex());

			pGuildDragonLair->Destroy();
			M2_DELETE(pGuildDragonLair);
		}
		it = m_map_pMapDragonLair.erase(it);
	}

	m_map_pMapDragonLair.clear();

	m_map_pGuildDragonLair.clear();
#if defined(__GUILD_DRAGONLAIR_PARTY_SYSTEM__)
	m_map_pPartyDragonLair.clear();
#endif
}

void CGuildDragonLairManager::DestroyPrivateMap(long lMapIndex)
{
	CGuildDragonLair* pGuildDragonLair = FindByMapIndex(lMapIndex);
	if (pGuildDragonLair == NULL)
	{
		sys_err("CGuildDragonLairManager::DestroyPrivateMap(lMapIndex=%ld): Failed to find instance with map index %ld.", lMapIndex);
		return;
	}

	m_map_pMapDragonLair.erase(lMapIndex);
	SECTREE_MANAGER::Instance().DestroyPrivateMap(pGuildDragonLair->GetMapIndex());

	if (pGuildDragonLair->GetGuild())
	{
		m_map_pGuildDragonLair.erase(pGuildDragonLair->GetGuild());
	}
#if defined(__GUILD_DRAGONLAIR_PARTY_SYSTEM__)
	else if (pGuildDragonLair->GetParty())
	{
		m_map_pPartyDragonLair.erase(pGuildDragonLair->GetParty());
	}
#endif

	pGuildDragonLair->Destroy();
	M2_DELETE(pGuildDragonLair);
}

CGuildDragonLair* CGuildDragonLairManager::GetGuildDragonLair(LPGUILD pGuild) const
{
	TGuildDragonLair::const_iterator it = m_map_pGuildDragonLair.find(pGuild);
	return it != m_map_pGuildDragonLair.end() ? it->second : NULL;
}

#if defined(__GUILD_DRAGONLAIR_PARTY_SYSTEM__)
CGuildDragonLair* CGuildDragonLairManager::GetGuildDragonLair(LPPARTY pParty) const
{
	TPartyDragonLair::const_iterator it = m_map_pPartyDragonLair.find(pParty);
	return it != m_map_pPartyDragonLair.end() ? it->second : NULL;
}
#endif

bool CGuildDragonLairManager::FindGuild(DWORD dwGuildID)
{
	const LPGUILD pGuild = CGuildManager::Instance().FindGuild(dwGuildID);
	if (pGuild == NULL)
		return false;

	CGuildDragonLair* pGuildDragonLair = GetGuildDragonLair(pGuild);
	if (pGuildDragonLair == NULL)
		return false;

	return true;
}

bool CGuildDragonLairManager::CanRegister(LPCHARACTER pChar)
{
	if (pChar == NULL)
		return false;

	const LPGUILD pGuild = pChar->GetGuild();
	if (pGuild == NULL)
		return false;

	if (pGuild->GetMasterPID() == pChar->GetPlayerID())
		return true;

	if (pGuild->IsGeneralMember(pChar->GetPlayerID()) != 0)
		return true;

	return false;
}

bool CGuildDragonLairManager::RegisterGuild(LPCHARACTER pChar, BYTE bType, bool bUseTicket)
{
	if (pChar == NULL)
		return false;

	const LPGUILD pGuild = pChar->GetGuild();
	if (pGuild == NULL)
		return false;

	const DWORD dwGuildID = pGuild->GetID();
	if (CanRegister(pChar) == false)
		return false;

	CGuildDragonLair* pGuildDragonLair = GetGuildDragonLair(pGuild);
	if (pGuildDragonLair)
		return false;

	int iLadderPoint = GuildDragonLair_GetFactor(2, "Guild", "LadderPoint");
	int iWaitTime = GuildDragonLair_GetFactor(2, "Guild", "WaitTime");
	int iTicketGroup = GuildDragonLair_GetFactor(2, "Guild", "TicketGroup");

	if (bUseTicket)
	{
		const CSpecialItemGroup* pItemGroup = ITEM_MANAGER::instance().GetSpecialItemGroup(iTicketGroup);
		if (pItemGroup == NULL)
		{
			sys_err("CGuildDragonLairManager::RegisterGuild(pChar=%p, bType=%u): Cannot find ticket group %u",
				pChar, bType, bUseTicket, iTicketGroup);
			return false;
		}

		bool bRemoveItem = false;
		for (int iIndex = 0; iIndex < pItemGroup->GetGroupSize(); iIndex++)
		{
			DWORD dwVnum = pItemGroup->GetVnum(iIndex);
			DWORD dwCount = pItemGroup->GetCount(iIndex);

			if (pChar->CountSpecifyItem(dwVnum) >= dwCount)
			{
				pChar->RemoveSpecifyItem(dwVnum, dwCount);
				bRemoveItem = true;
				break;
			}
		}

		if (bRemoveItem == false)
			return false;
	}

#if defined(__GUILD_EVENT_FLAG__) //&& defined(__GUILD_RENEWAL__)
	if (CGuildManager::Instance().GetEventFlag(dwGuildID, "dragonlair_channel") != 0)
		return false;

	if (bUseTicket == false)
	{
		if (get_global_time() - CGuildManager::Instance().GetEventFlag(dwGuildID, "dragonlair_wait_time") < iWaitTime)
			return false;
	}

	if (get_global_time() - CGuildManager::Instance().GetEventFlag(dwGuildID, "create_time") < 86400)
		return false;
#endif

	if (pGuild->GetLevel() < GuildDragonLair_GetFactor(2, "Guild", "GuildLevel"))
		return false;

	if (pGuild->GetNearMemberCount(pChar) < GuildDragonLair_GetFactor(2, "Guild", "MinMember") && pChar->GetGMLevel() == GM_PLAYER)
		return false;

	if (bUseTicket == false)
	{
		if (pGuild->GetLadderPoint() < iLadderPoint)
			return false;
	}

	if (pGuild->UnderAnyWar())
		return false;

	if (bUseTicket == false)
	{
		pGuild->ChangeLadderPoint(-iLadderPoint);
		pGuild->SetLadderPoint(pGuild->GetLadderPoint() - iLadderPoint);
	}

	if (bType == GUILD_DRAGONLAIR_TYPE_RED)
		pGuild->Chat(LC_STRING("[Guild] Successfully registered for Meley's Lair."));

	pGuild->Chat(LC_STRING("[Guild] Visit the Dragon Watcher Dolnarr to enter the dragon cave."));

#if defined(__GUILD_EVENT_FLAG__) //&& defined(__GUILD_RENEWAL__)
	CGuildManager::Instance().RequestSetEventFlag(dwGuildID, "dragonlair_channel", g_bChannel);
	CGuildManager::Instance().RequestSetEventFlag(dwGuildID, "dragonlair_wait_time", time(NULL) + iWaitTime);
#endif

	return true;
}

bool CGuildDragonLairManager::EnterGuild(LPCHARACTER pChar, BYTE bType)
{
	if (pChar == NULL)
		return false;

	const LPGUILD pGuild = pChar->GetGuild();
	if (pGuild == NULL)
		return false;

	if (pChar->GetLevel() < GuildDragonLair_GetFactor(2, "Guild", "PlayerLevel"))
		return false;

	if (pChar->IsRiding())
		return false;

	if (pGuild->IsGeneralMember(pChar->GetPlayerID()) == 0)
		return false;

	long lOriginalMapIndex = 0;
	CGuildDragonLair* pGuildDragonLair = GetGuildDragonLair(pGuild);

	if (pGuildDragonLair == NULL)
	{
		switch (bType)
		{
			case GUILD_DRAGONLAIR_TYPE_RED:
				lOriginalMapIndex = MAP_N_FLAME_DRAGON;
				break;

			default:
			{
				sys_err("CGuildDragonLairManager::EnterGuild(pChar=%p, bType=%u): %s entering unknown lair.",
					pChar, bType, pChar->GetName());
				return false;
			}
		}

		pGuildDragonLair = Create(bType, lOriginalMapIndex);
		if (pGuildDragonLair == NULL)
		{
			sys_err("CGuildDragonLairManager::EnterGuild(pChar=%p, bType=%u): Failed to create instance for guild %s (%u).",
				pChar, bType, pGuild->GetName(), pGuild->GetID());
			return false;
		}

		pGuildDragonLair->SetGuild(pGuild);
#if defined(__GUILD_DRAGONLAIR_PARTY_SYSTEM__)
		pGuildDragonLair->SetFirstRankingTime(GetFirstGuildRankingTime());
#endif
		pGuildDragonLair->Initialize();
		m_map_pGuildDragonLair[pGuild] = pGuildDragonLair;
	}

	if (pGuildDragonLair->IsExit(pChar->GetPlayerID()))
		return false;

	if (pGuildDragonLair->GetMemberCount() >= GuildDragonLair_GetFactor(2, "Guild", "MaxMember"))
		return false;

	if (pGuildDragonLair->GetStage() >= GUILD_DRAGONLAIR_STAGE1)
		return false;

	PIXEL_POSITION WarpPos;
	if (SECTREE_MANAGER::Instance().GetRecallPositionByEmpire(pGuildDragonLair->GetOriginalMapIndex(), pChar->GetEmpire(), WarpPos))
	{
		pChar->WarpSet(WarpPos.x, WarpPos.y, pGuildDragonLair->GetMapIndex());
	}
	else
	{
		sys_err("CGuildDragonLairManager::EnterGuild(pChar=%p, bType=%u): Failed to get map position by %s empire.",
			pChar, bType, pChar->GetName());
		return false;
	}

	return true;
}

BYTE CGuildDragonLairManager::GetGuildStage(DWORD dwGuildID) const
{
	const LPGUILD pGuild = CGuildManager::Instance().FindGuild(dwGuildID);
	if (pGuild == NULL)
		return 0;

	CGuildDragonLair* pGuildDragonLair = GetGuildDragonLair(pGuild);
	if (pGuildDragonLair == NULL)
		return 0;

	return pGuildDragonLair->GetStage();
}

size_t CGuildDragonLairManager::GetGuildMemberCount(DWORD dwGuildID) const
{
	const LPGUILD pGuild = CGuildManager::Instance().FindGuild(dwGuildID);
	if (pGuild == NULL)
		return 0;

	CGuildDragonLair* pGuildDragonLair = GetGuildDragonLair(pGuild);
	if (pGuildDragonLair == NULL)
		return 0;

	return pGuildDragonLair->GetMemberCount();
}

#if defined(__GUILD_DRAGONLAIR_PARTY_SYSTEM__)
bool CGuildDragonLairManager::FindParty(DWORD dwLeaderPID)
{
	const LPPARTY pParty = CPartyManager::Instance().FindParty(dwLeaderPID);
	if (pParty == NULL)
		return false;

	CGuildDragonLair* pGuildDragonLair = GetGuildDragonLair(pParty);
	if (pGuildDragonLair == NULL)
		return false;

	return true;
}

bool CGuildDragonLairManager::EnterParty(LPCHARACTER pChar, BYTE bType)
{
	if (pChar == NULL)
		return false;

	const LPPARTY pParty = pChar->GetParty();
	if (pParty == NULL)
		return false;

	if (pChar->GetLevel() < GuildDragonLair_GetFactor(2, "Party", "PlayerLevel"))
		return false;

	if (pChar->IsRiding())
		return false;

	long lOriginalMapIndex = 0;
	CGuildDragonLair* pGuildDragonLair = GetGuildDragonLair(pParty);

	if (pGuildDragonLair == NULL)
	{
		if (pParty->GetLeaderPID() != pChar->GetPlayerID())
			return false;

		if (pParty->GetNearMemberCount() < GuildDragonLair_GetFactor(2, "Party", "MinMember"))
			return false;

		switch (bType)
		{
			case GUILD_DRAGONLAIR_TYPE_RED:
				lOriginalMapIndex = MAP_N_FLAME_DRAGON;
				break;

			default:
			{
				sys_err("CGuildDragonLairManager::EnterParty(pChar=%p, bType=%u): %s trying to enter unknown lair.",
					pChar, bType, pChar->GetName());
				return false;
			}
		}

		pGuildDragonLair = Create(bType, lOriginalMapIndex);
		if (pGuildDragonLair == NULL)
		{
			sys_err("CGuildDragonLairManager::EnterParty(pChar=%p, bType=%u): Failed to create instance for party leader %s (%u).",
				pChar, bType, pChar->GetName(), pChar->GetPlayerID());
			return false;
		}

		pGuildDragonLair->SetParty(pParty);
#if defined(__GUILD_DRAGONLAIR_PARTY_SYSTEM__)
		pGuildDragonLair->SetFirstRankingTime(GetFirstPartyRankingTime());
#endif
		pGuildDragonLair->Initialize();
		m_map_pPartyDragonLair[pParty] = pGuildDragonLair;

		if (bType == GUILD_DRAGONLAIR_TYPE_RED)
			pParty->ChatPacketToAllMember(CHAT_TYPE_INFO, LC_STRING("[Group] You are now registered for Meley's Lair."));

		pParty->ChatPacketToAllMember(CHAT_TYPE_INFO, LC_STRING("[Group] Please talk to the relevant NPC to enter."));
	}

	if (pGuildDragonLair->IsExit(pChar->GetPlayerID()))
		return false;

	if (pGuildDragonLair->GetMemberCount() >= GuildDragonLair_GetFactor(2, "Party", "MaxMember"))
		return false;

	if (pGuildDragonLair->GetStage() >= GUILD_DRAGONLAIR_STAGE1)
		return false;

	PIXEL_POSITION WarpPos;
	if (SECTREE_MANAGER::Instance().GetRecallPositionByEmpire(pGuildDragonLair->GetOriginalMapIndex(), pChar->GetEmpire(), WarpPos))
	{
		pChar->WarpSet(WarpPos.x, WarpPos.y, pGuildDragonLair->GetMapIndex());
	}
	else
	{
		sys_err("CGuildDragonLairManager::EnterParty(pChar=%p, bType=%u): Failed to get map position by %s empire.",
			pChar, bType, pChar->GetName());
		return false;
	}

	return true;
}

BYTE CGuildDragonLairManager::GetPartyStage(DWORD dwLeaderPID) const
{
	const LPPARTY pParty = CPartyManager::Instance().FindParty(dwLeaderPID);
	if (pParty == NULL)
		return 0;

	CGuildDragonLair* pGuildDragonLair = GetGuildDragonLair(pParty);
	if (pGuildDragonLair == NULL)
		return 0;

	return pGuildDragonLair->GetStage();
}

size_t CGuildDragonLairManager::GetPartyMemberCount(DWORD dwLeaderPID) const
{
	const LPPARTY pParty = CPartyManager::Instance().FindParty(dwLeaderPID);
	if (pParty == NULL)
		return 0;

	CGuildDragonLair* pGuildDragonLair = GetGuildDragonLair(pParty);
	if (pGuildDragonLair == NULL)
		return 0;

	return pGuildDragonLair->GetMemberCount();
}

DWORD CGuildDragonLairManager::GetFirstGuildRankingTime() const
{
	char szQuery[1024];
	snprintf(szQuery, sizeof(szQuery), "SELECT `time` FROM `guild_dragonlair_ranking%s` ORDER BY `time` ASC LIMIT 1;", get_table_postfix());
	std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery(szQuery));
	if (pMsg->Get() && pMsg->Get()->uiNumRows > 0)
	{
		const MYSQL_ROW RowData = mysql_fetch_row(pMsg->Get()->pSQLResult);
		return atoi(RowData[0]);
	}
	return 0;
}

DWORD CGuildDragonLairManager::GetFirstPartyRankingTime() const
{
	char szQuery[1024];
	snprintf(szQuery, sizeof(szQuery), "SELECT `record0` FROM `ranking%s` WHERE `type` = %d AND `category` = %d ORDER BY `record0` ASC LIMIT 1",
		get_table_postfix(), CRanking::TYPE_RK_PARTY, CRanking::PARTY_RK_CATEGORY_GUILD_DRAGONLAIR_RED_ALL);
	std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery(szQuery));
	if (pMsg->Get() && pMsg->Get()->uiNumRows > 0)
	{
		const MYSQL_ROW RowData = mysql_fetch_row(pMsg->Get()->pSQLResult);
		return atoi(RowData[0]);
	}
	return 0;
}
#endif

bool CGuildDragonLairManager::Exit(LPCHARACTER pChar)
{
	if (pChar == NULL)
		return false;

	CGuildDragonLair* pGuildDragonLair = pChar->GetGuildDragonLair();
	if (pGuildDragonLair == NULL)
	{
		const LPSECTREE_MAP pSectree = SECTREE_MANAGER::instance().GetMap(MAP_N_FLAME_01);
		if (pSectree)
		{
			return pChar->WarpSet(
				pSectree->m_setting.iBaseX + GuildDragonLair_GetFactor(3, "PixelPosition", "Outside", "x") * 100,
				pSectree->m_setting.iBaseY + GuildDragonLair_GetFactor(3, "PixelPosition", "Outside", "y") * 100,
				MAP_N_FLAME_01);
		}
		else
		{
			sys_err("CGuildDragonLairManager::Exit(pChar=%p): %s failed to exit. Null sectree.",
				pChar, pChar->GetName());
			return pChar->WarpSet(EMPIRE_START_X(pChar->GetEmpire()), EMPIRE_START_Y(pChar->GetEmpire()));;
		}
	}

	return pGuildDragonLair->Exit(pChar);
}

bool CGuildDragonLairManager::Start(LPCHARACTER pChar)
{
	if (pChar == NULL)
		return false;

	CGuildDragonLair* pGuildDragonLair = pChar->GetGuildDragonLair();
	if (pGuildDragonLair == NULL)
		return false;

	if (pGuildDragonLair->GetGuild())
	{
#if defined(__GUILD_EVENT_FLAG__) //&& defined(__GUILD_RENEWAL__)
		CGuildManager::Instance().RequestSetEventFlag(pGuildDragonLair->GetGuild()->GetID(), "dragonlair_channel", 0);
#endif
	}
#if defined(__GUILD_DRAGONLAIR_PARTY_SYSTEM__)
	else if (pGuildDragonLair->GetParty())
	{
		unsigned int iWaitTime = GuildDragonLair_GetFactor(2, "Party", "WaitTime");

		FPartySetQuestFlag f("guild_dragon_lair.wait_time", std::time(NULL) + iWaitTime);
		pGuildDragonLair->GetParty()->ForEachNearMember(f);
	}
#endif

	return pGuildDragonLair->Start();
}

bool CGuildDragonLairManager::Cancel(LPCHARACTER pChar)
{
	if (pChar == NULL)
		return false;

	CGuildDragonLair* pGuildDragonLair = pChar->GetGuildDragonLair();
	if (pGuildDragonLair == NULL)
		return false;

	return pGuildDragonLair->Cancel();
}

bool CGuildDragonLairManager::CancelGuild(const char* pszGuildName)
{
	const LPGUILD pGuild = CGuildManager::Instance().FindGuildByName(pszGuildName);
	if (pGuild == NULL)
		return false;

	CGuildDragonLair* pGuildDragonLair = GetGuildDragonLair(pGuild);
	if (pGuildDragonLair == NULL)
		return false;

	return pGuildDragonLair->Cancel();
}

bool CGuildDragonLairManager::IsExit(LPCHARACTER pChar) const
{
	if (pChar == NULL)
		return false;

	CGuildDragonLair* pGuildDragonLair = pChar->GetGuildDragonLair();
	if (pGuildDragonLair == NULL)
		return false;

	return pGuildDragonLair->IsExit(pChar->GetPlayerID());
}

BYTE CGuildDragonLairManager::GetStage(LPCHARACTER pChar) const
{
	if (pChar == NULL)
		return 0;

	CGuildDragonLair* pGuildDragonLair = pChar->GetGuildDragonLair();
	if (pGuildDragonLair == NULL)
		return 0;

	return pGuildDragonLair->GetStage();
}

bool CGuildDragonLairManager::GiveReward(LPCHARACTER pChar, DWORD dwVnum)
{
	if (pChar == NULL)
		return false;

	CGuildDragonLair* pGuildDragonLair = pChar->GetGuildDragonLair();
	if (pGuildDragonLair == NULL)
		return false;

	return pGuildDragonLair->GiveReward(pChar, dwVnum);
}

bool CGuildDragonLairManager::GetReward(LPCHARACTER pChar) const
{
	if (pChar == NULL)
		return false;

	CGuildDragonLair* pGuildDragonLair = pChar->GetGuildDragonLair();
	if (pGuildDragonLair == NULL)
		return false;

	return pGuildDragonLair->GetReward(pChar);
}

bool CGuildDragonLairManager::IsUnique(LPCHARACTER pChar) const
{
	if (pChar == NULL)
		return false;

	CGuildDragonLair* pGuildDragonLair = pChar->GetGuildDragonLair();
	if (pGuildDragonLair == NULL)
		return false;

	return pGuildDragonLair->IsUnique(pChar);
}

bool CGuildDragonLairManager::IsStone(DWORD dwRaceNum) const
{
	return dwRaceNum == GuildDragonLair_GetFactor(2, "Guild", "StoneVnum")
#if defined(__GUILD_DRAGONLAIR_PARTY_SYSTEM__)
		|| dwRaceNum == GuildDragonLair_GetFactor(2, "Party", "StoneVnum")
#endif
		;
}

bool CGuildDragonLairManager::IsEgg(DWORD dwRaceNum) const
{
	return dwRaceNum == GuildDragonLair_GetFactor(2, "Guild", "EggVnum")
#if defined(__GUILD_DRAGONLAIR_PARTY_SYSTEM__)
		|| dwRaceNum == GuildDragonLair_GetFactor(2, "Party", "EggVnum")
#endif
		;
}

bool CGuildDragonLairManager::IsBoss(DWORD dwRaceNum) const
{
	return dwRaceNum == GuildDragonLair_GetFactor(2, "Guild", "BossVnum")
#if defined(__GUILD_DRAGONLAIR_PARTY_SYSTEM__)
		|| dwRaceNum == GuildDragonLair_GetFactor(2, "Party", "BossVnum")
#endif
		;
}

bool CGuildDragonLairManager::IsKing(DWORD dwRaceNum) const
{
	return dwRaceNum == GuildDragonLair_GetFactor(2, "Guild", "KingVnum")
#if defined(__GUILD_DRAGONLAIR_PARTY_SYSTEM__)
		|| dwRaceNum == GuildDragonLair_GetFactor(2, "Party", "KingVnum")
#endif
		;
}

bool CGuildDragonLairManager::IsRedDragonLair(long lMapIndex) const
{
	return lMapIndex >= (MAP_N_FLAME_DRAGON * 10000) && lMapIndex < ((MAP_N_FLAME_DRAGON + 1) * 10000);
}

unsigned int GuildDragonLair_GetFactor(const size_t cnt, ...)
{
	lua_State* L = quest::CQuestManager::instance().GetLuaState();

	const int stack_top = lua_gettop(L);

	lua_getglobal(L, "GuildDragonLairSetting");

	if (lua_istable(L, -1) == false)
	{
		lua_settop(L, stack_top);
		return 0;
	}

	va_list vl;
	va_start(vl, cnt);

	for (size_t i = 0; i < cnt; ++i)
	{
		const char* key = va_arg(vl, const char*);

		if (NULL == key)
		{
			va_end(vl);
			lua_settop(L, stack_top);
			sys_err("GuildDragonLair: wrong key list");
			return 0;
		}

		lua_pushstring(L, key);
		lua_gettable(L, -2);

		if (lua_istable(L, -1) == false && i != cnt - 1)
		{
			va_end(vl);
			lua_settop(L, stack_top);
			sys_err("GuildDragonLair: wrong key table %s", key);
			return 0;
		}
	}

	va_end(vl);

	if (lua_isnumber(L, -1) == false)
	{
		lua_settop(L, stack_top);
		sys_err("GuildDragonLair: Last key is not a number.");
		return 0;
	}

	int val = static_cast<int>(lua_tonumber(L, -1));

	lua_settop(L, stack_top);

	return val;
}

unsigned int GuildDragonLair_GetIndexFactor(const char* container, const size_t idx, const char* key)
{
	lua_State* L = quest::CQuestManager::instance().GetLuaState();

	const int stack_top = lua_gettop(L);

	lua_getglobal(L, "GuildDragonLairSetting");

	if (false == lua_istable(L, -1))
	{
		lua_settop(L, stack_top);

		return 0;
	}

	lua_pushstring(L, container);
	lua_gettable(L, -2);

	if (false == lua_istable(L, -1))
	{
		lua_settop(L, stack_top);

		sys_err("GuildDragonLair: no required table %s", key);
		return 0;
	}

	lua_rawgeti(L, -1, idx);

	if (false == lua_istable(L, -1))
	{
		lua_settop(L, stack_top);

		sys_err("GuildDragonLair: wrong table index %s %d", key, idx);
		return 0;
	}

	lua_pushstring(L, key);
	lua_gettable(L, -2);

	if (false == lua_isnumber(L, -1))
	{
		lua_settop(L, stack_top);

		sys_err("GuildDragonLair: no min value set %s", key);
		return 0;
	}

	const unsigned int ret = static_cast<unsigned int>(lua_tonumber(L, -1));

	lua_settop(L, stack_top);

	return ret;
}

unsigned int GuildDragonLair_GetIndexFactor2(const char* container, const char* sub_container, const size_t idx, const char* key)
{
	lua_State* L = quest::CQuestManager::instance().GetLuaState();

	const int stack_top = lua_gettop(L);

	lua_getglobal(L, "GuildDragonLairSetting");

	if (!lua_istable(L, -1))
	{
		lua_settop(L, stack_top);
		return 0;
	}

	lua_pushstring(L, container);
	lua_gettable(L, -2);

	if (!lua_istable(L, -1))
	{
		lua_settop(L, stack_top);
		sys_err("GuildDragonLair: no required table %s", container);
		return 0;
	}

	lua_pushstring(L, sub_container);
	lua_gettable(L, -2);

	if (!lua_istable(L, -1))
	{
		lua_settop(L, stack_top);
		sys_err("GuildDragonLair: no required table %s", sub_container);
		return 0;
	}

	lua_rawgeti(L, -1, idx);

	if (!lua_istable(L, -1))
	{
		lua_settop(L, stack_top);
		sys_err("GuildDragonLair: wrong table index %s %d", sub_container, idx);
		return 0;
	}

	lua_pushstring(L, key);
	lua_gettable(L, -2);

	if (!lua_isnumber(L, -1))
	{
		lua_settop(L, stack_top);
		sys_err("GuildDragonLair: no min value set %s", key);
		return 0;
	}

	const unsigned int ret = static_cast<unsigned int>(lua_tonumber(L, -1));

	lua_settop(L, stack_top);

	return ret;
}
#endif // __GUILD_DRAGONLAIR_SYSTEM__
