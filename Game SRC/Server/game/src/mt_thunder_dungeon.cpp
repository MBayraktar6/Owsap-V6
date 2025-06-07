/**
* Filename: mt_thunder_dungeon.cpp
* Author: Owsap
**/

#include "stdafx.h"

#if defined(__MT_THUNDER_DUNGEON__)
#include "mt_thunder_dungeon.h"

#include "char.h"
#include "char_manager.h"
#include "sectree_manager.h"

#include "config.h"
#include "utils.h"

CMTThunderDungeon::CMTThunderDungeon()
{
	m_vGuardianPos.clear();

	m_dwGuardianVID = 0;
	m_dwPortalVID = 0;

	m_pGuardianMoveEvent = nullptr;
	m_pPortalEvent = nullptr;
}

CMTThunderDungeon::~CMTThunderDungeon()
{
	Destroy();
}

void CMTThunderDungeon::Initialize()
{
	const LPSECTREE_MAP pkSectree = SECTREE_MANAGER::instance().GetMap(MAP_INDEX);
	if (pkSectree == nullptr)
		return;

	const int iBaseX = pkSectree->m_setting.iBaseX;
	const int iBaseY = pkSectree->m_setting.iBaseY;

	m_vGuardianPos.reserve(MAP_ROOMS);
	for (const PIXEL_POSITION& rkPos : MAP_ROOM_POS)
		m_vGuardianPos.emplace_back(PIXEL_POSITION{ iBaseX + rkPos.x * 100, iBaseY + rkPos.y * 100, 0 });

	SpawnGuardian();
}

void CMTThunderDungeon::Destroy()
{
	m_vGuardianPos.clear();

	m_dwGuardianVID = 0;
	m_dwPortalVID = 0;

	if (m_pGuardianMoveEvent)
	{
		event_cancel(&m_pGuardianMoveEvent);
		m_pGuardianMoveEvent = nullptr;
	}

	if (m_pPortalEvent)
	{
		event_cancel(&m_pPortalEvent);
		m_pPortalEvent = nullptr;
	}
}

EVENTINFO(GuardianEventInfo)
{
	DWORD dwNextMoveTime;
};

EVENTFUNC(GuardianEvent)
{
	GuardianEventInfo* pInfo = dynamic_cast<GuardianEventInfo*>(event->info);
	if (pInfo == nullptr)
	{
		sys_err("GuardianEvent: <Factor> Null pointer");
		return 0;
	}

	const DWORD dwCurrentTime = time(nullptr);
	if (pInfo->dwNextMoveTime <= dwCurrentTime)
	{
		pInfo->dwNextMoveTime = dwCurrentTime + CMTThunderDungeon::GUARDIAN_MOVE_SEC;
		CMTThunderDungeon::Instance().MoveGuardian();
	}

	return PASSES_PER_SEC(CMTThunderDungeon::GUARDIAN_CHECK_PULSE);
}

void CMTThunderDungeon::SpawnGuardian()
{
	static std::mt19937 rng(std::random_device{}());
	std::uniform_int_distribution<size_t> dist(0, m_vGuardianPos.size() - 1);
	size_t nIndex = dist(rng);

	const PIXEL_POSITION& rkPos = m_vGuardianPos[nIndex];
	const LPCHARACTER pkGuardian = CHARACTER_MANAGER::instance().SpawnMob(GUARDIAN_VNUM, MAP_INDEX, rkPos.x, rkPos.y, rkPos.z);
	if (pkGuardian == nullptr)
	{
		sys_err("CMTThunderDungeon::SpawnGuardian(): Failed to spawn guardian %u at %d, %d", GUARDIAN_VNUM, rkPos.x, rkPos.y);
		return;
	}

	if (test_server)
	{
		char szBuf[256] = {};
		snprintf(szBuf, sizeof(szBuf), "<Ochao Temple> %s spawned at (%d, %d)", LC_MOB(pkGuardian->GetRaceNum()), rkPos.x, rkPos.y);
		SendNoticeMap(szBuf, MAP_INDEX, false);
	}

	pkGuardian->SetLastAttacked(0);
	m_dwGuardianVID = pkGuardian->GetVID();

	if (m_pGuardianMoveEvent)
	{
		event_cancel(&m_pGuardianMoveEvent);
		m_pGuardianMoveEvent = nullptr;
	}

	GuardianEventInfo* pEventInfo = AllocEventInfo<GuardianEventInfo>();
	pEventInfo->dwNextMoveTime = time(nullptr) + GUARDIAN_MOVE_SEC;
	m_pGuardianMoveEvent = event_create(GuardianEvent, pEventInfo, PASSES_PER_SEC(GUARDIAN_CHECK_PULSE));
}

void CMTThunderDungeon::MoveGuardian()
{
	const LPCHARACTER pkGuardian = CHARACTER_MANAGER::instance().Find(m_dwGuardianVID);
	if ((pkGuardian) && (pkGuardian->GetMapIndex() == MAP_INDEX))
		M2_DESTROY_CHARACTER(pkGuardian);

	if (test_server)
	{
		char szBuf[256] = {};
		snprintf(szBuf, sizeof(szBuf), "<Ochao Temple> %s is moving again!", LC_MOB(GUARDIAN_VNUM));
		SendNoticeMap(szBuf, MAP_INDEX, false);
	}

	SpawnGuardian();
}

EVENTINFO(PortalEventInfo)
{
	DWORD dwPortalVID;
};

EVENTFUNC(PortalEvent)
{
	PortalEventInfo* pInfo = dynamic_cast<PortalEventInfo*>(event->info);
	if (pInfo == nullptr)
	{
		sys_err("PortalEvent: <Factor> Null pointer");
		return 0;
	}

	if (test_server)
	{
		char szBuf[256] = {};
		snprintf(szBuf, sizeof(szBuf), "<Ochao Temple> %s vanished!", LC_MOB(CMTThunderDungeon::PORTAL_VNUM));
		SendNoticeMap(szBuf, CMTThunderDungeon::MAP_INDEX, false);
	}

	const LPCHARACTER pkPortal = CHARACTER_MANAGER::instance().Find(pInfo->dwPortalVID);
	if ((pkPortal) && (pkPortal->GetMapIndex() == CMTThunderDungeon::MAP_INDEX))
		M2_DESTROY_CHARACTER(pkPortal);

	CMTThunderDungeon::Instance().MoveGuardian();
	return 0;
}

void CMTThunderDungeon::SpawnPortal(const LPCHARACTER pkGuardian)
{
	if (pkGuardian == nullptr)
		return;

	if (pkGuardian->GetVID() != m_dwGuardianVID)
		return;

	if (m_pGuardianMoveEvent)
	{
		event_cancel(&m_pGuardianMoveEvent);
		m_pGuardianMoveEvent = nullptr;
	}

	const PIXEL_POSITION kPos = { pkGuardian->GetX(), pkGuardian->GetY(), pkGuardian->GetZ() };
	const LPCHARACTER pkPortal = CHARACTER_MANAGER::instance().SpawnMob(PORTAL_VNUM, MAP_INDEX, kPos.x, kPos.y, kPos.z);
	if (pkPortal == nullptr)
	{
		sys_err("CMTThunderDungeon::SpawnPortal(): Failed to spawn portal %u at %d, %d", PORTAL_VNUM, kPos.x, kPos.y);
		return;
	}

	if (test_server)
	{
		char szBuf[256] = {};
		snprintf(szBuf, sizeof(szBuf), "<Ochao Temple> %s spawned at (%d, %d)", LC_MOB(PORTAL_VNUM), kPos.x, kPos.y);
		SendNoticeMap(szBuf, MAP_INDEX, false);
	}

	m_dwPortalVID = pkPortal->GetVID();

	if (m_pPortalEvent)
	{
		event_cancel(&m_pPortalEvent);
		m_pPortalEvent = nullptr;
	}

	PortalEventInfo* pEventInfo = AllocEventInfo<PortalEventInfo>();
	pEventInfo->dwPortalVID = m_dwPortalVID;
	m_pPortalEvent = event_create(PortalEvent, pEventInfo, PASSES_PER_SEC(PORTAL_OPEN_SEC));
}
#endif
