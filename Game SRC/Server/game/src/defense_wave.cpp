/**
* Filename: defense_wave.cpp
* Author: Owsap
**/

#include "stdafx.h"

#if defined(__DEFENSE_WAVE__)
#include "defense_wave.h"
#include "char.h"
#include "char_manager.h"
#include "questmanager.h"

#include "utils.h"
#include "config.h"
#include "mob_manager.h"
#include "desc.h"
#include "skill.h"
#include "item.h"
#include "party.h"
#include "start_position.h"

const bool speed_test = false;

EVENTINFO(DefenseWaveEventInfo)
{
	LPDEFENSE_WAVE pDefenseWave;
	bool bPrepare;
	int iPassesPerSec;
	BYTE bNextWave;
	BYTE bIndex;
	char szUniqueKey[EVENT_FLAG_NAME_MAX_LEN];
	DefenseWaveEventInfo()
		: pDefenseWave(NULL)
		, bPrepare(false)
		, iPassesPerSec(0)
		, bNextWave(0)
		, bIndex(CDefenseWave::LASER_MAX_NUM)
	{
		memset(szUniqueKey, 0, sizeof(szUniqueKey));
	}
};

EVENTFUNC(DefenseWaveDestroyEvent)
{
	DefenseWaveEventInfo* pEventInfo = dynamic_cast<DefenseWaveEventInfo*>(event->info);
	if (pEventInfo == NULL)
	{
		sys_err("DefenseWaveDestroyEvent: <Factor> NULL Pointer");
		return 0;
	}

	LPDEFENSE_WAVE pDefenseWave = pEventInfo->pDefenseWave;
	if (pDefenseWave == NULL)
	{
		sys_err("DefenseWaveDestroyEvent: <Factor> NULL CDefenseWave");
		return 0;
	}

	if (pEventInfo->bPrepare == false)
	{
		pEventInfo->bPrepare = true;


		pDefenseWave->ExitAll();
		return PASSES_PER_SEC(5);
	}
	else
	{
		CDefenseWaveManager::Instance().Destroy(pDefenseWave->GetId());
		return 0;
	}
}

EVENTFUNC(DefenseWavePurgeEvent)
{
	DefenseWaveEventInfo* pEventInfo = dynamic_cast<DefenseWaveEventInfo*>(event->info);
	if (pEventInfo == NULL)
	{
		sys_err("DefenseWavePurgeEvent: <Factor> NULL Pointer");
		return 0;
	}

	LPDEFENSE_WAVE pDefenseWave = pEventInfo->pDefenseWave;
	if (pDefenseWave == NULL)
	{
		sys_err("DefenseWavePurgeEvent: <Factor> NULL CDefenseWave");
		return 0;
	}

	pDefenseWave->PurgeUnique(pEventInfo->szUniqueKey);
	return 0;
}

EVENTFUNC(DefenseWaveEvent)
{
	DefenseWaveEventInfo* pEventInfo = dynamic_cast<DefenseWaveEventInfo*>(event->info);
	if (pEventInfo == NULL)
	{
		sys_err("DefenseWaveEvent: <Factor> NULL Pointer");
		return 0;
	}

	LPDEFENSE_WAVE pDefenseWave = pEventInfo->pDefenseWave;
	if (pDefenseWave == NULL)
	{
		sys_err("DefenseWaveEvent: <Factor> NULL CDefenseWave");
		return 0;
	}

	int iPassesPerSec = pEventInfo->iPassesPerSec;
	if (iPassesPerSec < 1)
	{
		if (pEventInfo->bPrepare == false)
			pDefenseWave->SetWave(pEventInfo->bNextWave);
		else
			pDefenseWave->PrepareWave(pEventInfo->bNextWave);
		return 0;
	}

	if (pEventInfo->bPrepare == false)
	{
		if (iPassesPerSec <= 10 || (iPassesPerSec % 10 == 0 && iPassesPerSec < 60))
			pDefenseWave->ChatPacket(CHAT_TYPE_INFO, LC_STRING("The sea battle starts in %d sec.", iPassesPerSec));
	}
	else
	{
		if (iPassesPerSec <= 10 || iPassesPerSec == 30)
			pDefenseWave->ChatPacket(CHAT_TYPE_INFO, LC_STRING("The next wave begins in %d sec.", iPassesPerSec));
	}

	--pEventInfo->iPassesPerSec;
	return PASSES_PER_SEC(1);
}

EVENTFUNC(DefenseWaveSpawnEvent)
{
	DefenseWaveEventInfo* pEventInfo = dynamic_cast<DefenseWaveEventInfo*>(event->info);
	if (pEventInfo == NULL)
	{
		sys_err("DefenseWaveSpawnEvent: <Factor> NULL Pointer");
		return 0;
	}

	LPDEFENSE_WAVE pDefenseWave = pEventInfo->pDefenseWave;
	if (pDefenseWave == NULL)
	{
		sys_err("DefenseWaveSpawnEvent: <Factor> NULL CDefenseWave");
		return 0;
	}

	const std::array<PIXEL_POSITION, CDefenseWave::LASER_MAX_NUM> aSpawnPos
	{
		PIXEL_POSITION { 405, 385 },
		PIXEL_POSITION { 406, 418 },
		PIXEL_POSITION { 365, 418 },
		PIXEL_POSITION { 365, 386 },
	};

	BYTE bWave = pDefenseWave->GetWave();
	for (BYTE bIndex = 0; bIndex < CDefenseWave::LASER_MAX_NUM; ++bIndex)
	{
		std::string n = "HydraSpawn" + std::to_string(bIndex + 1);
		if (pDefenseWave->GetUnique(n) != NULL)
		{
			if (bWave == CDefenseWave::WAVE2)
			{
				for (int j = 0; j < 3; ++j)
					pDefenseWave->SpawnMob(3950, aSpawnPos[bIndex].x, aSpawnPos[bIndex].y, 0);
			}
			else if (bWave == CDefenseWave::WAVE3)
			{
				for (int j = 0; j < 3; ++j)
					pDefenseWave->SpawnMob(3951, aSpawnPos[bIndex].x, aSpawnPos[bIndex].y, 0);
			}
			else if (bWave == CDefenseWave::WAVE4)
			{
				for (int j = 0; j < 4; ++j)
					pDefenseWave->SpawnMob(3952, aSpawnPos[bIndex].x, aSpawnPos[bIndex].y, 0);
			}
		}
	}

	return PASSES_PER_SEC(30);
}

EVENTFUNC(DefenseWaveEggEvent)
{
	DefenseWaveEventInfo* pEventInfo = dynamic_cast<DefenseWaveEventInfo*>(event->info);
	if (pEventInfo == NULL)
	{
		sys_err("DefenseWaveEggEvent: <Factor> NULL Pointer");
		return 0;
	}

	LPDEFENSE_WAVE pDefenseWave = pEventInfo->pDefenseWave;
	if (pDefenseWave == NULL)
	{
		sys_err("DefenseWaveEggEvent: <Factor> NULL CDefenseWave");
		return 0;
	}

	pDefenseWave->SpawnEgg();
	return PASSES_PER_SEC(speed_test ? 3 : 40);
}

EVENTFUNC(DefenseWaveHydraSpawnEvent)
{
	DefenseWaveEventInfo* pEventInfo = dynamic_cast<DefenseWaveEventInfo*>(event->info);
	if (pEventInfo == NULL)
	{
		sys_err("DefenseWaveHydraSpawnEvent: <Factor> NULL Pointer");
		return 0;
	}

	LPDEFENSE_WAVE pDefenseWave = pEventInfo->pDefenseWave;
	if (pDefenseWave == NULL)
	{
		sys_err("DefenseWaveHydraSpawnEvent: <Factor> NULL CDefenseWave");
		return 0;
	}

	pDefenseWave->SpawnHydraSpawn(pEventInfo->bIndex);
	return 0;
}

EVENTFUNC(DefenseWaveLaserEvent)
{
	DefenseWaveEventInfo* pEventInfo = dynamic_cast<DefenseWaveEventInfo*>(event->info);
	if (pEventInfo == NULL)
	{
		sys_err("DefenseWaveLaserEvent: <Factor> NULL Pointer");
		return 0;
	}

	LPDEFENSE_WAVE pDefenseWave = pEventInfo->pDefenseWave;
	if (pDefenseWave == NULL)
	{
		sys_err("DefenseWaveLaserEvent: <Factor> NULL CDefenseWave");
		return 0;
	}

	pDefenseWave->TriggerLaser(pEventInfo->bIndex);
	return PASSES_PER_SEC(15);
}

EVENTFUNC(DefenseWaveDebuffEvent)
{
	DefenseWaveEventInfo* pEventInfo = dynamic_cast<DefenseWaveEventInfo*>(event->info);
	if (pEventInfo == NULL)
	{
		sys_err("DefenseWaveDebuffEvent: <Factor> NULL Pointer");
		return 0;
	}

	LPDEFENSE_WAVE pDefenseWave = pEventInfo->pDefenseWave;
	if (pDefenseWave == NULL)
	{
		sys_err("DefenseWaveDebuffEvent: <Factor> NULL CDefenseWave");
		return 0;
	}

	pDefenseWave->Debuff();
	return PASSES_PER_SEC(10);
}

struct FDefenseWaveExit
{
	void operator() (LPENTITY pEntity)
	{
		if (pEntity && pEntity->IsType(ENTITY_CHARACTER))
		{
			LPCHARACTER pCharacter = dynamic_cast<LPCHARACTER>(pEntity);
			if (pCharacter && pCharacter->IsPC())
			{
				if (pCharacter->GetDefenseWave())
					pCharacter->GetDefenseWave()->Exit(pCharacter);
			}
		}
	}
};

struct FDefenseWaveChat
{
	FDefenseWaveChat(BYTE bChatType, const char* pszText)
		: m_bChatType(bChatType), m_pszText(pszText) {}

	void operator() (LPENTITY pEntity)
	{
		if (pEntity && pEntity->IsType(ENTITY_CHARACTER))
		{
			LPCHARACTER pCharacter = dynamic_cast<LPCHARACTER>(pEntity);
			if (pCharacter && pCharacter->IsPC())
			{
				pCharacter->ChatPacket(m_bChatType, "%s", m_pszText);
			}
		}
	}

	BYTE m_bChatType;
	const char* m_pszText;
};

struct FDefenseWaveJump
{
	FDefenseWaveJump(long lMapIndex, long lX, long lY)
		: m_lMapIndex(lMapIndex), m_lX(lX), m_lY(lY) {}

	void operator() (LPENTITY pEntity)
	{
		if (pEntity && pEntity->IsType(ENTITY_CHARACTER))
		{
			LPCHARACTER pCharacter = dynamic_cast<LPCHARACTER>(pEntity);
			if (pCharacter && pCharacter->IsPC())
			{
				if (pCharacter->GetMapIndex() == m_lMapIndex)
				{
					pCharacter->Show(m_lMapIndex, m_lX, m_lY, 0);
					pCharacter->Stop();
				}
				else
				{
					pCharacter->WarpSet(m_lX, m_lY, m_lMapIndex);
				}
			}
		}
	}

	long m_lMapIndex;
	long m_lX;
	long m_lY;
};

struct FDefenseWaveClearAll
{
	FDefenseWaveClearAll() {}

	void operator() (LPENTITY pEntity)
	{
		if (pEntity && pEntity->IsType(ENTITY_CHARACTER))
		{
			LPCHARACTER pCharacter = dynamic_cast<LPCHARACTER>(pEntity);
			if (pCharacter && (pCharacter->IsMonster() || pCharacter->IsStone()))
			{
				if (pCharacter->IsMonster())
				{
					if (m_bIgnoreMobRank.find(pCharacter->GetMobRank()) != m_bIgnoreMobRank.end())
						return;

					if (m_bIgnoreMobRace.find(pCharacter->GetRaceNum()) != m_bIgnoreMobRace.end())
						return;
				}

				pCharacter->Dead(NULL, true);
			}
		}
	}

	void IgnoreMobRank(BYTE bRank)
	{
		m_bIgnoreMobRank.emplace(bRank);
	}

	void IgnoreMobRace(DWORD dwRaceNum)
	{
		m_bIgnoreMobRace.emplace(dwRaceNum);
	}

	bool m_bPurge;
	std::unordered_set<BYTE> m_bIgnoreMobRank;
	std::unordered_set<DWORD> m_bIgnoreMobRace;
};

struct FDefenseWaveAllianceTarget
{
	FDefenseWaveAllianceTarget(DWORD dwVID, int iHP, int iMaxHP)
		: m_dwVID(dwVID), m_iHP(iHP), m_iMaxHP(iMaxHP)
	{
	}

	void operator() (LPENTITY pEntity)
	{
		if (pEntity && pEntity->IsType(ENTITY_CHARACTER))
		{
			LPCHARACTER pCharacter = dynamic_cast<LPCHARACTER>(pEntity);
			if (pCharacter && pCharacter->GetDesc())
			{
				TPacketGCTarget Packet;
				Packet.header = HEADER_GC_TARGET;
				Packet.dwVID = m_dwVID;
				Packet.iMinHP = m_iHP;
				Packet.iMaxHP = m_iMaxHP;
				Packet.bAlliance = true;
				pCharacter->GetDesc()->Packet(&Packet, sizeof(TPacketGCTarget));
			}
		}
	}

	DWORD m_dwVID;
	int m_iHP; int m_iMaxHP;
};

struct FDefenseWaveDebuff
{
	FDefenseWaveDebuff() {}

	void operator() (LPENTITY pEntity)
	{
		if (pEntity && pEntity->IsType(ENTITY_CHARACTER))
		{
			LPCHARACTER pCharacter = dynamic_cast<LPCHARACTER>(pEntity);
			if (pCharacter && (pCharacter->IsMonster() || pCharacter->IsStone()))
			{
				pCharacter->RemoveGoodAffect();
				pCharacter->AddAffect(SKILL_PABEOB, POINT_NONE, 0, AFF_PABEOP, 25, 0, true);
			}
		}
	}
};

///////////////////////////////////////////////////////////////////////////////////////////
// CDefenseWave
///////////////////////////////////////////////////////////////////////////////////////////

CDefenseWave::CDefenseWave(DWORD dwID, long lOriginalMapIndex, long lPrivateMapIndex)
	: CDungeon(dwID, lOriginalMapIndex, lPrivateMapIndex)
{
	m_bWave = WAVE0;

	m_pDestroyEvent = NULL;
	m_pPurgeEvent = NULL;
	m_pWaveEvent = NULL;
	m_pEggEvent = NULL;

	for (BYTE bIndex = 0; bIndex < LASER_MAX_NUM; ++bIndex)
		m_pHydraSpawnEvent[bIndex] = NULL;

	for (BYTE bIndex = 0; bIndex < LASER_MAX_NUM; ++bIndex)
		m_pLaserEvent[bIndex] = NULL;

	m_pSpawnEvent = NULL;
	m_pDebuffEvent = NULL;

	m_dwStartTime = 0;
	m_iLastAllianceHPPct = 0;
}

CDefenseWave::~CDefenseWave()
{
	Destroy();
}

void CDefenseWave::Initialize()
{
	SetUnique("ShipMast", SpawnMob(20434, 385, 400, 1)->GetVID()); // 기둥돛
	SetUnique("ShipWheel", SpawnMob(20436, 385, 367, 1)->GetVID()); // 조타대

	CreateWalls(); // 보이지 않는 벽

	DefenseWaveEventInfo* pDestroyEventInfo = AllocEventInfo<DefenseWaveEventInfo>();
	pDestroyEventInfo->pDefenseWave = this;
	SetDestroyEvent(event_create(DefenseWaveDestroyEvent, pDestroyEventInfo, PASSES_PER_SEC(speed_test ? 60 : 600)));
}

void CDefenseWave::Destroy()
{
	m_bWave = WAVE0;

	CancelEvents();

	m_dwStartTime = 0;
	m_iLastAllianceHPPct = 0;
}

void CDefenseWave::ChatPacket(BYTE bChatType, const char* szText)
{
	LPSECTREE_MAP pSectreeMap = SECTREE_MANAGER::instance().GetMap(GetMapIndex());
	if (pSectreeMap == NULL)
	{
		sys_err("Cannot find map by index %ld", GetMapIndex());
		return;
	}

	FDefenseWaveChat f(bChatType, szText);
	pSectreeMap->for_each(f);
}

bool CDefenseWave::ChangeAllianceHP(BYTE bHealthPct)
{
	LPSECTREE_MAP pSectreeMap = SECTREE_MANAGER::Instance().GetMap(GetMapIndex());
	if (pSectreeMap == NULL)
	{
		sys_err("Cannot find map by index %ld", GetMapIndex());
		return false;
	}

	LPCHARACTER pShipMast = GetUnique("ShipMast");
	if (pShipMast == NULL)
		return false;

	int iHP = (bHealthPct * pShipMast->GetMaxHP()) / 100;
	pShipMast->SetHP(pShipMast->GetHP() + iHP);

	ChatPacket(CHAT_TYPE_BIG_NOTICE, LC_STRING("%s's remaining HP: %d%%",
		LC_MOB(pShipMast->GetRaceNum()), pShipMast->GetHPPct()));

	FDefenseWaveAllianceTarget f(pShipMast->GetVID(), pShipMast->GetHP(), pShipMast->GetMaxHP());
	pSectreeMap->for_each(f);

	return true;
}

void CDefenseWave::Debuff()
{
	LPSECTREE_MAP pSectreeMap = SECTREE_MANAGER::instance().GetMap(GetMapIndex());
	if (pSectreeMap == NULL)
	{
		sys_err("Cannot find map by index %ld", GetMapIndex());
		return;
	}

	FDefenseWaveDebuff f;
	pSectreeMap->for_each(f);
}

bool CDefenseWave::Enter(LPCHARACTER pChar)
{
	PIXEL_POSITION WarpPosition;
	BYTE bEmpire = pChar->GetEmpire();

	if (SECTREE_MANAGER::Instance().GetRecallPositionByEmpire(GetMapIndex(), bEmpire, WarpPosition))
		return pChar->WarpSet(WarpPosition.x, WarpPosition.y, GetMapIndex());

	return pChar->WarpSet(EMPIRE_START_X(bEmpire), EMPIRE_START_Y(bEmpire));
}

bool CDefenseWave::Exit(LPCHARACTER pChar)
{
	PIXEL_POSITION WarpPosition;
	BYTE bEmpire = pChar->GetEmpire();

	if (SECTREE_MANAGER::Instance().GetRecallPositionByEmpire(MAP_CAPEDRAGONHEAD, bEmpire, WarpPosition))
		return pChar->WarpSet(WarpPosition.x, WarpPosition.y);

	return pChar->WarpSet(EMPIRE_START_X(bEmpire), EMPIRE_START_Y(bEmpire));
}

void CDefenseWave::ExitAll()
{
	LPSECTREE_MAP pSectreeMap = SECTREE_MANAGER::instance().GetMap(GetMapIndex());
	if (pSectreeMap == NULL)
	{
		sys_err("Cannot find map by index %ld", GetMapIndex());
		return;
	}

	FDefenseWaveExit f;
	pSectreeMap->for_each(f);
}

void CDefenseWave::Start()
{
	m_dwStartTime = std::time(NULL);

	LPSECTREE_MAP pSectreeMap = SECTREE_MANAGER::instance().GetMap(GetMapIndex());
	if (pSectreeMap == NULL)
	{
		sys_err("Cannot find map by index %ld", GetMapIndex());
		return;
	}

	LPCHARACTER pChar = GetUnique("ShipMast");
	if (pChar != NULL)
	{
		FDefenseWaveAllianceTarget f(pChar->GetVID(), pChar->GetHP(), pChar->GetMaxHP());
		pSectreeMap->for_each(f);
	}
	else
	{
		sys_log(0, "Cannot find ally character (ship mast), destroying instance...");

		DefenseWaveEventInfo* pDestroyEventInfo = AllocEventInfo<DefenseWaveEventInfo>();
		pDestroyEventInfo->pDefenseWave = this;
		SetDestroyEvent(event_create(DefenseWaveDestroyEvent, pDestroyEventInfo, PASSES_PER_SEC(10)));
	}

	PrepareWave(WAVE1);
}

LPCHARACTER CDefenseWave::SpawnMob(DWORD dwVnum, int iX, int iY, int iDir, bool bSpawnMotion, bool bSetVictim)
{
	LPSECTREE_MAP pSectreeMap = SECTREE_MANAGER::instance().GetMap(GetMapIndex());
	if (pSectreeMap == NULL)
	{
		sys_err("Cannot find map by index %ld", GetMapIndex());
		return NULL;
	}

	long lX = pSectreeMap->m_setting.iBaseX + iX * 100;
	long lY = pSectreeMap->m_setting.iBaseY + iY * 100;
	int iRot = iDir == 0 ? -1 : (iDir - 1) * 45;

	const CMob* pMob = CMobManager::Instance().Get(dwVnum);
	if (!pMob)
	{
		sys_err("Cannot find mob data for vnum %u", dwVnum);
		return NULL;
	}

	LPCHARACTER pChar = CHARACTER_MANAGER::Instance().CreateCharacter(pMob->m_table.szLocaleName);
	if (pChar == NULL)
	{
		sys_log(0, "Cannot create new character %u", dwVnum);
		return NULL;
	}

	if (iRot == -1)
		iRot = number(0, 360);

	pChar->SetProto(pMob);
	pChar->SetRotation(static_cast<float>(iRot));
	pChar->SetDungeon(this);
	pChar->SetDefenseWave(this);

	if (pChar->IsMonster() && bSetVictim)
		pChar->SetVictim(GetUnique("ShipMast"));

	if (!pChar->Show(GetMapIndex(), lX, lY, 0, bSpawnMotion))
	{
		M2_DESTROY_CHARACTER(pChar);
		sys_log(0, "Cannot show monster monster %u", dwVnum);
		return NULL;
	}

	return pChar;
}

void CDefenseWave::SpawnHydraSpawn(BYTE bIndex)
{
	LPSECTREE_MAP pSectreeMap = SECTREE_MANAGER::instance().GetMap(GetMapIndex());
	if (pSectreeMap == NULL)
	{
		sys_err("Cannot find map by index %ld", GetMapIndex());
		return;
	}

	const std::array<PIXEL_POSITION, LASER_MAX_NUM> pos
	{
		PIXEL_POSITION { 396, 389 },
		PIXEL_POSITION { 396, 411 },
		PIXEL_POSITION { 374, 411 },
		PIXEL_POSITION { 374, 389 },
	};

	std::string name = "HydraSpawn" + std::to_string(bIndex + 1);
	LPCHARACTER pHydraSpawn = GetUnique(name);
	if (NULL == pHydraSpawn)
	{
		DWORD dwRaceNum = 3957;
		if (m_bWave == WAVE3)
			dwRaceNum = 3958;
		else if (m_bWave == WAVE4)
			dwRaceNum = 3959;

		if (m_bWave == WAVE2 && bIndex >= 1)
			return;

		if (m_bWave == WAVE3 && bIndex >= 2)
			return;

		if (m_bWave == WAVE4 && bIndex >= 4)
			return;

		pHydraSpawn = SpawnMob(dwRaceNum, pos[bIndex].x, pos[bIndex].y, 0);
		SetUnique(name.c_str(), pHydraSpawn->GetVID());
	}
}

void CDefenseWave::SpawnEgg()
{
	LPSECTREE_MAP pSectreeMap = SECTREE_MANAGER::instance().GetMap(GetMapIndex());
	if (pSectreeMap == NULL)
	{
		sys_err("Cannot find map by index %ld", GetMapIndex());
		return;
	}

	LPCHARACTER pShipMast = GetUnique("ShipMast");
	if (pShipMast == NULL)
		return;

	PIXEL_POSITION pos;
	pos.x = (pShipMast->GetX() - pSectreeMap->m_setting.iBaseX) / 100;
	pos.y = (pShipMast->GetY() - pSectreeMap->m_setting.iBaseY) / 100;

	if (NULL == GetUnique("HydraEgg"))
	{
		int x, y;
		do
		{
			x = pos.x + number(-8, 8);
			y = pos.y + number(-8, 8);

			if (x == pos.x && y == pos.y)
				continue;

		} while (DISTANCE_APPROX(pos.x - x, pos.y - y) < 5);

		SetUnique("HydraEgg", SpawnMob(20432, x, y, 0)->GetVID()); // 히드라알
	}

	// Purge Event
	DefenseWaveEventInfo* pDestroyEventInfo = AllocEventInfo<DefenseWaveEventInfo>();
	pDestroyEventInfo->pDefenseWave = this;
	strlcpy(pDestroyEventInfo->szUniqueKey, "HydraEgg", sizeof(pDestroyEventInfo->szUniqueKey));
	SetPurgeEvent(event_create(DefenseWavePurgeEvent, pDestroyEventInfo, PASSES_PER_SEC(speed_test ? 3 : 8)));
}

void CDefenseWave::TriggerLaser(BYTE bLaserIdx)
{
	if (bLaserIdx >= LASER_MAX_NUM)
		return;

	LPCHARACTER pHydraSpawn = GetUnique("HydraSpawn" + std::to_string(bLaserIdx + 1));
	if (pHydraSpawn && pHydraSpawn->FindAffect(AFFECT_DEFENSEWAVE_LASER) == NULL)
	{
		pHydraSpawn->AddAffect(AFFECT_DEFENSEWAVE_LASER, POINT_NONE, 0, AFF_NONE, INFINITE_AFFECT_DURATION, 0, true);
		pHydraSpawn->EffectPacket(SE_DEFENSE_WAVE_LASER);
	}
}

void CDefenseWave::CheckAffect(LPCHARACTER pChar, long x, long y)
{
	if (pChar == NULL)
		return;

	if (pChar->IsPC() == false)
		return;

	if (m_bWave == WAVE0 || m_bWave == WAVE1)
		return;

	LPSECTREE_MAP pSectreeMap = SECTREE_MANAGER::instance().GetMap(GetMapIndex());
	if (pSectreeMap == NULL)
	{
		sys_err("Cannot find map by index %ld", GetMapIndex());
		return;
	}

	const std::array<PIXEL_POSITION, LASER_MAX_NUM> aLaserPosition
	{
		PIXEL_POSITION { 390, 394 },
		PIXEL_POSITION { 390, 405 },
		PIXEL_POSITION { 379, 405 },
		PIXEL_POSITION { 379, 394 }
	};

	long lCharPixelPosX = (x - pSectreeMap->m_setting.iBaseX) / 100;
	long lCharPixelPosY = (y - pSectreeMap->m_setting.iBaseY) / 100;

	for (BYTE bIndex = 0; bIndex < LASER_MAX_NUM; ++bIndex)
	{
		if (lCharPixelPosX == aLaserPosition[bIndex].x && lCharPixelPosY == aLaserPosition[bIndex].y)
		{
			LPCHARACTER pHydraSpawn = GetUnique("HydraSpawn" + std::to_string(bIndex + 1));
			if (pHydraSpawn && pHydraSpawn->FindAffect(AFFECT_DEFENSEWAVE_LASER))
			{
				pHydraSpawn->RemoveAffect(AFFECT_DEFENSEWAVE_LASER);
				pHydraSpawn->ViewReencode();

				pChar->AddAffect(AFFECT_DEFENSEWAVE_LASER, POINT_NONE, 0, AFF_NONE, 15, 0, true);

				// Laser Event
				DefenseWaveEventInfo* pLaserEventInfo = AllocEventInfo<DefenseWaveEventInfo>();
				pLaserEventInfo->pDefenseWave = this;
				pLaserEventInfo->bIndex = bIndex;
				SetLaserEvent(bIndex, event_create(DefenseWaveLaserEvent, pLaserEventInfo, PASSES_PER_SEC(15)));
			}
		}
	}
}

bool CDefenseWave::Damage(LPCHARACTER pCharVictim)
{
	if (pCharVictim == NULL)
		return false;

	DWORD dwRaceNum = pCharVictim->GetRaceNum();

	if (CDefenseWaveManager::Instance().IsLeftHydra(dwRaceNum))
		return false;

	if (CDefenseWaveManager::Instance().IsRightHydra(dwRaceNum))
		return false;

	if (GetWave() == WAVE4 && CDefenseWaveManager::Instance().IsHydra(dwRaceNum))
	{
		LPCHARACTER pChar = GetUnique("Hydra");
		if (pChar != NULL && pChar->GetHPPct() <= 50)
		{
			if (m_pDebuffEvent != NULL)
			{
				DefenseWaveEventInfo* pDebuffEventInfo = AllocEventInfo<DefenseWaveEventInfo>();
				pDebuffEventInfo->pDefenseWave = this;
				SetDebuffEvent(event_create(DefenseWaveDebuffEvent, pDebuffEventInfo, PASSES_PER_SEC(1)));
			}
		}
	}

	if (CDefenseWaveManager::Instance().IsShipMast(dwRaceNum))
	{
		LPSECTREE_MAP pSectreeMap = SECTREE_MANAGER::instance().GetMap(GetMapIndex());
		if (pSectreeMap == NULL)
		{
			sys_err("Cannot find map by index %ld", GetMapIndex());
			return false;
		}

		LPCHARACTER pChar = GetUnique("ShipMast");
		if (pChar != NULL)
		{
			int iHPPct = pChar->GetHPPct();
			if ((iHPPct == 30 || iHPPct == 10 || (iHPPct >= 1 && iHPPct <= 5)) && iHPPct != m_iLastAllianceHPPct)
			{
				m_iLastAllianceHPPct = iHPPct;

				ChatPacket(CHAT_TYPE_BIG_NOTICE, LC_STRING("%s's remaining HP: %d%%",
					LC_MOB(pChar->GetRaceNum()), iHPPct));
			}

			FDefenseWaveAllianceTarget f(pChar->GetVID(), pChar->GetHP(), pChar->GetMaxHP());
			pSectreeMap->for_each(f);
		}
	}

	return true;
}

void CDefenseWave::DeadCharacter(LPCHARACTER pCharVictim)
{
	if (pCharVictim == NULL)
		return;

	if (IsUnique(pCharVictim) == false)
		return;

	LPSECTREE_MAP pSectreeMap = SECTREE_MANAGER::instance().GetMap(GetMapIndex());
	if (pSectreeMap == NULL)
	{
		sys_err("Cannot find map by index %ld", GetMapIndex());
		return;
	}

	if (pCharVictim == GetUnique("ShipMast"))
	{
		CDungeon::DeadCharacter(pCharVictim);
		ClearWave();

		ChatPacket(CHAT_TYPE_BIG_NOTICE,
			LC_STRING("SOS! The mast has been smashed and your ship is sinking. You will now be teleported to Cape Dragon Fire."));

		DefenseWaveEventInfo* pDestroyEventInfo = AllocEventInfo<DefenseWaveEventInfo>();
		pDestroyEventInfo->pDefenseWave = this;
		SetDestroyEvent(event_create(DefenseWaveDestroyEvent, pDestroyEventInfo, PASSES_PER_SEC(10)));
	}
	else if (pCharVictim == GetUnique("Hydra"))
	{
		CDungeon::DeadCharacter(pCharVictim);
		ClearWave();

		ChatPacket(CHAT_TYPE_BIG_NOTICE,
			LC_STRING("You have successfully protected the mast. Keep it up! Get ready for the next wave."));

		DefenseWaveEventInfo* pWaveEventInfo = AllocEventInfo<DefenseWaveEventInfo>();
		pWaveEventInfo->pDefenseWave = this;
		pWaveEventInfo->bPrepare = true;

		BYTE bWave = GetWave();
		switch (bWave)
		{
			case WAVE2:
				pWaveEventInfo->bNextWave = WAVE3;
				break;

			case WAVE3:
				pWaveEventInfo->bNextWave = WAVE4;
				break;

			case WAVE4:
			{
#if defined(__RANKING_SYSTEM__)
				if (GetParty())
				{
					TRankingData RankingData = {};
					RankingData.dwRecord0 = std::time(NULL) - m_dwStartTime;
					RankingData.dwStartTime = m_dwStartTime;
					CRanking::Register(GetParty()->GetLeaderPID(), CRanking::TYPE_RK_PARTY, CRanking::PARTY_RK_CATEGORY_DEFENSE_WAVE, RankingData);
				}
#endif

				RemoveWalls();

				if (NULL == GetUnique("HydraReward"))
					SetUnique("HydraReward", SpawnMob(3965, 385, 416, 1)->GetVID()); // 히드라보상

				if (NULL == GetUnique("Portal"))
					SetUnique("Portal", SpawnMob(3949, 385, 450, 1)->GetVID()); // 포탈

				ChatPacket(CHAT_TYPE_BIG_NOTICE,
					LC_STRING("Land sighted! You have successfully protected the mast and reached the new continent after a long and weary journey. Continue to the portal."));

				DefenseWaveEventInfo* pDestroyEventInfo = AllocEventInfo<DefenseWaveEventInfo>();
				pDestroyEventInfo->pDefenseWave = this;
				pDestroyEventInfo->iPassesPerSec = speed_test ? 10 : 300;
				SetDestroyEvent(event_create(DefenseWaveDestroyEvent, pDestroyEventInfo, PASSES_PER_SEC(speed_test ? 10 : 300)));

				ChatPacket(CHAT_TYPE_INFO, LC_STRING("Hurry! You have %d minutes to travel through the portal to reach the mainland.",
					(pWaveEventInfo->iPassesPerSec / 60)));
			}
			break;

			default:
				sys_err("Unknown Wave %u", bWave);
				break;
		}

		SetWaveEvent(event_create(DefenseWaveEvent, pWaveEventInfo, PASSES_PER_SEC(6)));
	}
	else if (pCharVictim == GetUnique("HydraEgg"))
	{
		CDungeon::DeadCharacter(pCharVictim);

		TMobIgnoreList MobIgnoreList;
		MobIgnoreList.AddRankToIgnore(MOB_RANK_KING);
		MobIgnoreList.AddRaceToIgnore(3956);
		ClearMobs(MobIgnoreList);
	}
	
	for (BYTE bIndex = 0; bIndex < LASER_MAX_NUM; ++bIndex)
	{
		std::string name = "HydraSpawn" + std::to_string(bIndex + 1);
		LPCHARACTER pHydraSpawn = GetUnique(name);
		if (pCharVictim == pHydraSpawn)
		{
			DefenseWaveEventInfo* pDestroyEventInfo = AllocEventInfo<DefenseWaveEventInfo>();
			pDestroyEventInfo->pDefenseWave = this;
			pDestroyEventInfo->bIndex = bIndex;
			SetHydraSpawnEvent(bIndex, event_create(DefenseWaveHydraSpawnEvent, pDestroyEventInfo, PASSES_PER_SEC(30)));
		}
	}

	CDungeon::DeadCharacter(pCharVictim);
}

void CDefenseWave::CreateWalls()
{
	// 보이지 않는 벽 (back)
	if (NULL == GetUnique("LeftBackWall"))
		SetUnique("LeftBackWall", SpawnMob(3970, 400, 372, 1)->GetVID());
	if (NULL == GetUnique("RightBackWall"))
		SetUnique("RightBackWall", SpawnMob(3970, 369, 372, 1)->GetVID());

	// 보이지 않는 벽 (front)
	if (NULL == GetUnique("LeftFrontWall"))
		SetUnique("LeftFrontWall", SpawnMob(3970, 400, 435, 1)->GetVID());
	if (NULL == GetUnique("RightFrontWall"))
		SetUnique("RightFrontWall", SpawnMob(3970, 369, 435, 1)->GetVID());
}

void CDefenseWave::RemoveWalls()
{
	RemoveBackWalls();
	RemoveFrontWalls();
}

void CDefenseWave::RemoveBackWalls()
{
	PurgeUnique("LeftBackWall");
	PurgeUnique("RightBackWall");
}

void CDefenseWave::RemoveFrontWalls()
{
	PurgeUnique("LeftFrontWall");
	PurgeUnique("RightFrontWall");
}

void CDefenseWave::PrepareWave(BYTE bWave)
{
	ClearWave();

	DefenseWaveEventInfo* pWaveEventInfo = AllocEventInfo<DefenseWaveEventInfo>();
	pWaveEventInfo->pDefenseWave = this;
	pWaveEventInfo->iPassesPerSec = (bWave == WAVE1) ? (speed_test ? 5 : 60) : (speed_test ? 5 : 10);
	pWaveEventInfo->bNextWave = bWave;

	switch (bWave)
	{
		case WAVE1:
		{
			DWORD dwCurrentTime = get_dword_time();

			// 히드라(left)
			if (NULL == GetUnique("HydraLeft"))
			{
				LPCHARACTER pChar = SpawnMob(3961, 385, 378, 5, false, false); // 히드라(left)
				if (pChar != NULL)
				{
					SetUnique("HydraLeft", pChar->GetVID());
					pChar->SendMovePacket(FUNC_MOB_SKILL, 2, pChar->GetX(), pChar->GetY(), 0, dwCurrentTime);

					DefenseWaveEventInfo* pDestroyEventInfo = AllocEventInfo<DefenseWaveEventInfo>();
					pDestroyEventInfo->pDefenseWave = this;
					strlcpy(pDestroyEventInfo->szUniqueKey, "HydraLeft", sizeof(pDestroyEventInfo->szUniqueKey));
					SetPurgeEvent(event_create(DefenseWavePurgeEvent, pDestroyEventInfo, PASSES_PER_SEC(4)));
				}
			}

			// 히드라(right)
			if (NULL == GetUnique("HydraRight1"))
				SetUnique("HydraRight1", SpawnMob(3964, 378, 443, 5)->GetVID());
			if (NULL == GetUnique("HydraRight2"))
				SetUnique("HydraRight2", SpawnMob(3964, 385, 439, 5)->GetVID());
			if (NULL == GetUnique("HydraRight3"))
				SetUnique("HydraRight3", SpawnMob(3964, 392, 443, 5)->GetVID());

			ChatPacket(CHAT_TYPE_BIG_NOTICE, LC_STRING("We're under attack from sea monsters! Get ready to fight!"));
			ChatPacket(CHAT_TYPE_BIG_NOTICE, LC_STRING("Wave %d: For %d sec. you must protect the %s with all your strength.",
				bWave, 120, LC_MOB(GetUnique("ShipMast")->GetRaceNum())));

			if (pWaveEventInfo->iPassesPerSec % 60 == 0)
			{
				ChatPacket(CHAT_TYPE_INFO, LC_STRING("Hurry! You have %d minutes to travel through the portal to reach the mainland.",
					(pWaveEventInfo->iPassesPerSec / 60)));
			}
		}
		break;

		case WAVE2:
		case WAVE3:
		case WAVE4:
		{
			CreateWalls();
			JumpToStartLocation();

			if (NULL == GetUnique("Hydra"))
			{
				if (bWave == WAVE4)
				{
					PurgeUnique("HydraRight3"); // 히드라(right)
					SetUnique("Hydra", SpawnMob(3962, 385, 373, 1, false, false)->GetVID()); // 히드라3
				}
				else if (bWave == WAVE3)
				{
					PurgeUnique("HydraRight2"); // 히드라(right)
					SetUnique("Hydra", SpawnMob(3961, 385, 373, 1, false, false)->GetVID()); // 히드라2
				}
				else if (bWave == WAVE2)
				{
					PurgeUnique("HydraRight1"); // 히드라(right)
					SetUnique("Hydra", SpawnMob(3960, 385, 373, 1, false, false)->GetVID()); // 히드라1
				}
			}

			ChatPacket(CHAT_TYPE_BIG_NOTICE, LC_STRING("They're attacking again! Ready your weapons!"));
			ChatPacket(CHAT_TYPE_BIG_NOTICE, LC_STRING("Wave %d: While protecting the %s, defeat %s.",
				bWave, LC_MOB(GetUnique("ShipMast")->GetRaceNum()), LC_MOB(GetUnique("Hydra")->GetRaceNum())));
		}
		break;

		default:
			sys_err("Unknown Wave %u", bWave);
			return;
	}

	SetWaveEvent(event_create(DefenseWaveEvent, pWaveEventInfo, PASSES_PER_SEC(1)));
}

void CDefenseWave::SetWave(BYTE bWave)
{
	CancelEvents();
	RemoveBackWalls();

	m_bWave = bWave;

	switch (bWave)
	{
		case WAVE1:
		case WAVE2:
		case WAVE3:
		case WAVE4:
		{
			if (bWave == WAVE1)
			{
				DefenseWaveEventInfo* pWaveEventInfo = AllocEventInfo<DefenseWaveEventInfo>();
				pWaveEventInfo->pDefenseWave = this;
				pWaveEventInfo->iPassesPerSec = speed_test ? 5 : 120;
				pWaveEventInfo->bNextWave = WAVE2;
				pWaveEventInfo->bPrepare = true;
				SetWaveEvent(event_create(DefenseWaveEvent, pWaveEventInfo, PASSES_PER_SEC(1)));
			}
			else
			{
				if (GetUnique("Hydra"))
					GetUnique("Hydra")->SetVictim(GetUnique("ShipMast"));

				// Egg Event
				DefenseWaveEventInfo* pEggEventInfo = AllocEventInfo<DefenseWaveEventInfo>();
				pEggEventInfo->pDefenseWave = this;
				SetEggEvent(event_create(DefenseWaveEggEvent, pEggEventInfo, PASSES_PER_SEC(speed_test ? 3 : 30)));

				for (BYTE bIndex = 0; bIndex < LASER_MAX_NUM; ++bIndex)
				{
					// Hydra Spawn Event
					DefenseWaveEventInfo* pHydraEventInfo = AllocEventInfo<DefenseWaveEventInfo>();
					pHydraEventInfo->pDefenseWave = this;
					pHydraEventInfo->bIndex = bIndex;
					SetHydraSpawnEvent(bIndex, event_create(DefenseWaveHydraSpawnEvent, pHydraEventInfo, PASSES_PER_SEC(1)));

					// Laser Event
					DefenseWaveEventInfo* pLaserEventInfo = AllocEventInfo<DefenseWaveEventInfo>();
					pLaserEventInfo->pDefenseWave = this;
					pLaserEventInfo->bIndex = bIndex;
					SetLaserEvent(bIndex, event_create(DefenseWaveLaserEvent, pLaserEventInfo, PASSES_PER_SEC(15)));
				}

				// Spawn Event
				DefenseWaveEventInfo* pSpawnEventInfo = AllocEventInfo<DefenseWaveEventInfo>();
				pSpawnEventInfo->pDefenseWave = this;
				SetSpawnEvent(event_create(DefenseWaveSpawnEvent, pSpawnEventInfo, PASSES_PER_SEC(10)));
			}

			char szRegenFileName[FILE_MAX_LEN];
			snprintf(szRegenFileName, sizeof(szRegenFileName), "data/dungeon/defense_wave/wave%d.txt", bWave);
			SpawnRegen(szRegenFileName, false);
			SpawnRegen("data/dungeon/defense_wave/boss.txt", true);
		}
		break;
	}
}

void CDefenseWave::ClearWave()
{
	CancelEvents();
	ClearRegen();

	TMobIgnoreList MobIgnoreList;
	MobIgnoreList.AddRankToIgnore(MOB_RANK_KING);
	ClearMobs(MobIgnoreList);
}

void CDefenseWave::ClearMobs(const TMobIgnoreList& rkMobIgnoreList)
{
	LPSECTREE_MAP pSectreeMap = SECTREE_MANAGER::instance().GetMap(GetMapIndex());
	if (pSectreeMap == NULL)
	{
		sys_err("Cannot find map by index %ld", GetMapIndex());
		return;
	}

	FDefenseWaveClearAll f;
	for (BYTE bMobRank : rkMobIgnoreList.GetIgnoreMobRank())
		f.IgnoreMobRank(bMobRank);
	for (DWORD dwRaceNum : rkMobIgnoreList.GetIgnoreMobRace())
		f.IgnoreMobRace(dwRaceNum);

	pSectreeMap->for_each(f);
}

void CDefenseWave::JumpToStartLocation()
{
	long lMapIndex = GetMapIndex();

	LPSECTREE_MAP pSectreeMap = SECTREE_MANAGER::instance().GetMap(lMapIndex);
	if (pSectreeMap == NULL)
	{
		sys_err("Cannot find map by index %ld", lMapIndex);
		return;
	}

	PIXEL_POSITION WarpPosition;
	if (SECTREE_MANAGER::Instance().GetRecallPositionByEmpire(lMapIndex, 0, WarpPosition))
	{
		FDefenseWaveJump f(lMapIndex, WarpPosition.x, WarpPosition.y);
		pSectreeMap->for_each(f);
	}
	else
	{
		sys_log(0, "Cannot find recall position by empire %ld", lMapIndex);
	}
}

void CDefenseWave::SetDestroyEvent(LPEVENT pEvent)
{
	if (m_pDestroyEvent != NULL)
		event_cancel(&m_pDestroyEvent);

	m_pDestroyEvent = pEvent;
}

void CDefenseWave::SetPurgeEvent(LPEVENT pEvent)
{
	if (m_pPurgeEvent != NULL)
		event_cancel(&m_pPurgeEvent);

	m_pPurgeEvent = pEvent;
}

void CDefenseWave::SetWaveEvent(LPEVENT pEvent)
{
	if (m_pWaveEvent != NULL)
		event_cancel(&m_pWaveEvent);

	m_pWaveEvent = pEvent;
}

void CDefenseWave::SetEggEvent(LPEVENT pEvent)
{
	if (m_pEggEvent != NULL)
		event_cancel(&m_pEggEvent);

	m_pEggEvent = pEvent;
}

void CDefenseWave::SetHydraSpawnEvent(BYTE bIndex, LPEVENT pEvent)
{
	if (bIndex >= LASER_MAX_NUM)
		return;

	if (m_pHydraSpawnEvent[bIndex] != NULL)
		event_cancel(&m_pHydraSpawnEvent[bIndex]);

	m_pHydraSpawnEvent[bIndex] = pEvent;
}

void CDefenseWave::SetLaserEvent(BYTE bIndex, LPEVENT pEvent)
{
	if (bIndex >= LASER_MAX_NUM)
		return;

	if (m_pLaserEvent[bIndex] != NULL)
		event_cancel(&m_pLaserEvent[bIndex]);

	m_pLaserEvent[bIndex] = pEvent;
}

void CDefenseWave::SetSpawnEvent(LPEVENT pEvent)
{
	if (m_pSpawnEvent != NULL)
		event_cancel(&m_pSpawnEvent);

	m_pSpawnEvent = pEvent;
}

void CDefenseWave::SetDebuffEvent(LPEVENT pEvent)
{
	if (m_pDebuffEvent != NULL)
		event_cancel(&m_pDebuffEvent);

	m_pDebuffEvent = pEvent;
}

void CDefenseWave::CancelEvents()
{
	SetDestroyEvent(NULL);
	SetPurgeEvent(NULL);
	SetWaveEvent(NULL);
	SetEggEvent(NULL);

	for (BYTE bIndex = 0; bIndex < LASER_MAX_NUM; ++bIndex)
		SetHydraSpawnEvent(bIndex, NULL);

	for (BYTE bIndex = 0; bIndex < LASER_MAX_NUM; ++bIndex)
		SetLaserEvent(bIndex, NULL);

	SetSpawnEvent(NULL);
	SetDebuffEvent(NULL);
}

///////////////////////////////////////////////////////////////////////////////////////////
// CDefenseWaveManager
///////////////////////////////////////////////////////////////////////////////////////////

CDefenseWaveManager::CDefenseWaveManager()
{
}

CDefenseWaveManager::~CDefenseWaveManager()
{
}

LPDEFENSE_WAVE CDefenseWaveManager::Create(long lOriginalMapIndex)
{
	LPDUNGEON pDungeon = CDungeonManager::Instance().Create(lOriginalMapIndex, DUNGEON_TYPE_DEFENSE_WAVE);
	if (pDungeon == NULL)
	{
		sys_err("Failed to create dungeon");
		return NULL;
	}

	LPDEFENSE_WAVE pDefenseWave = dynamic_cast<LPDEFENSE_WAVE>(pDungeon);
	if (pDefenseWave == NULL)
	{
		sys_err("Failed to create dungoen: Invalid cast type");
		return NULL;
	}

	m_map_pDefenseWave.emplace(pDefenseWave->GetId(), pDefenseWave);
	m_map_pMapDefenseWave.emplace(pDefenseWave->GetMapIndex(), pDefenseWave);

	return pDefenseWave;
}

void CDefenseWaveManager::Destroy(DWORD dwDungeonID)
{
	LPDEFENSE_WAVE pDefenseWave = Find(dwDungeonID);
	if (pDefenseWave == NULL)
		return;

	long lPrivateMapIndex = pDefenseWave->GetMapIndex();
	m_map_pDefenseWave.erase(dwDungeonID);
	m_map_pMapDefenseWave.erase(lPrivateMapIndex);

	CDungeonManager::Instance().Destroy(dwDungeonID);
}

LPDEFENSE_WAVE CDefenseWaveManager::Find(DWORD dwDungeonID) const
{
	LPDUNGEON pDungeon = CDungeonManager::Instance().Find(dwDungeonID);
	return dynamic_cast<LPDEFENSE_WAVE>(pDungeon);
}

LPDEFENSE_WAVE CDefenseWaveManager::FindByMapIndex(long lMapIndex) const
{
	LPDUNGEON pDungeon = CDungeonManager::Instance().FindByMapIndex(lMapIndex);
	return dynamic_cast<LPDEFENSE_WAVE>(pDungeon);
}

LPDEFENSE_WAVE CDefenseWaveManager::GetDungeon(LPDUNGEON pDungeon) const
{
	return dynamic_cast<LPDEFENSE_WAVE>(pDungeon);
}

bool CDefenseWaveManager::Enter(LPCHARACTER pChar, long lMapIndex)
{
	if (pChar == NULL)
		return false;

	LPPARTY pParty = pChar->GetParty();
	if (pParty == NULL)
		return false;

	LPDEFENSE_WAVE pDefenseWave = FindByMapIndex(pParty->GetFlag("dungeon_index"));
	if (pDefenseWave == NULL)
	{
		if (pParty->GetLeaderPID() != pChar->GetPlayerID())
			return false;

		pDefenseWave = Create(lMapIndex);
		if (pDefenseWave == NULL)
			return false;

		pDefenseWave->SetParty(pParty);
		pDefenseWave->Initialize();

		// 던전과 파티에 서로에 대한 정보를 저장한다.
		pParty->SetFlag("dungeon_index", pDefenseWave->GetMapIndex());
		pDefenseWave->SetFlag("party_leader_pid", pParty->GetLeaderPID());

		pParty->ChatPacketToAllMember(CHAT_TYPE_NOTICE,
			LC_STRING("The 'Ship Defence' dungeon is ready. The fisherman will let you board now."));
	}

	PIXEL_POSITION WarpPosition;
	if (SECTREE_MANAGER::Instance().GetRecallPositionByEmpire(lMapIndex, pChar->GetEmpire(), WarpPosition))
		return pChar->WarpSet(WarpPosition.x, WarpPosition.y, pDefenseWave->GetMapIndex());

	return false;
}

bool CDefenseWaveManager::Exit(LPCHARACTER pChar, long lMapIndex)
{
	PIXEL_POSITION WarpPosition;
	BYTE bEmpire = pChar->GetEmpire();

	if (SECTREE_MANAGER::Instance().GetRecallPositionByEmpire(lMapIndex, bEmpire, WarpPosition))
		return pChar->WarpSet(WarpPosition.x, WarpPosition.y);

	return pChar->WarpSet(EMPIRE_START_X(bEmpire), EMPIRE_START_Y(bEmpire));
}

bool CDefenseWaveManager::CanAttack(LPCHARACTER pCharAttacker, LPCHARACTER pCharVictim)
{
	if (pCharAttacker == NULL || pCharVictim == NULL)
		return false;

	if (pCharAttacker == pCharVictim)
		return false;

	if (IsLeftHydra(pCharAttacker->GetRaceNum()))
		return false;

	if (IsRightHydra(pCharAttacker->GetRaceNum()))
		return false;

	return true;
}

bool CDefenseWaveManager::IsShipMast(DWORD dwRaceNum) const
{
	return dwRaceNum == 20434;
}

bool CDefenseWaveManager::IsShipWheel(DWORD dwRaceNum) const
{
	return dwRaceNum == 20436;
}

bool CDefenseWaveManager::IsWoodRepair(DWORD dwRaceNum) const
{
	return dwRaceNum == 20437;
}

bool CDefenseWaveManager::IsHydraSpawn(DWORD dwRaceNum) const
{
	return dwRaceNum == 3957 || dwRaceNum == 3958 || dwRaceNum == 3959;
}

bool CDefenseWaveManager::IsLeftHydra(DWORD dwRaceNum) const
{
	return dwRaceNum == 3963;
}

bool CDefenseWaveManager::IsRightHydra(DWORD dwRaceNum) const
{
	return dwRaceNum == 3964;
}

bool CDefenseWaveManager::IsHydra(DWORD dwRaceNum) const
{
	return dwRaceNum == 3960 || dwRaceNum == 3961 || dwRaceNum == 3962;
}

bool CDefenseWaveManager::IsDefenseWaveMap(long lMapIndex) const
{
	return lMapIndex >= (MAP_DEFENSEWAVE * 10000) && lMapIndex < ((MAP_DEFENSEWAVE + 1) * 10000);
}
#endif // __DEFENSE_WAVE__
