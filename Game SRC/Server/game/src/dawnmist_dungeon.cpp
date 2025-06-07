/**
* Filename: dawnmist_dungeon.cpp
* Author: Owsap
**/

#include "stdafx.h"

#if defined(__DAWNMIST_DUNGEON__)
#include "dawnmist_dungeon.h"
#include "char.h"
#include "char_manager.h"
#include "party.h"
#include "config.h"

bool CDawnMistDungeon::CanSpawnHealerGroup(const LPCHARACTER pkLeader)
{
	int iPercent = 0;
	if (pkLeader->GetMaxHP() >= 0)
		iPercent = (pkLeader->GetHP() * 100) / pkLeader->GetMaxHP();

	int iMaxSP = pkLeader->GetMaxSP();
	if (iMaxSP >= MAX_HEAL_STAGE)
		return false;

	return iPercent <= HEAL_ON_HP_PCT[iMaxSP];
}

void CDawnMistDungeon::SpawnHealerGroup(const LPCHARACTER pkLeader)
{
	const DWORD dwHealerNum = GetBossHealer(pkLeader->GetRaceNum());
	if (dwHealerNum == 0)
		return;

	LPPARTY pParty = pkLeader->GetParty();
	if (pParty == nullptr)
		pParty = CPartyManager::Instance().CreateParty(pkLeader);

	int sx = 0, sy = 0, ex = 0, ey = 0;
	int range = HEALER_MAX_RANGE;
	for (BYTE bIndex = 0; bIndex < HEALER_COUNT; ++bIndex)
	{
		BYTE bSpawnAttemptCount = 0;
		bool bNextHealer = false;

		sx = pkLeader->GetX() - range;
		sy = pkLeader->GetY() - range;
		ex = pkLeader->GetX() + range;
		ey = pkLeader->GetY() + range;

		while (!bNextHealer && bSpawnAttemptCount < 16)
		{
			LPCHARACTER pkHealer = CHARACTER_MANAGER::Instance().SpawnMobRange(dwHealerNum,
				pkLeader->GetMapIndex(),
				sx, sy,
				ex, ey,
				false, true);

			if (NULL != pkHealer)
			{
				pParty->Join(pkHealer->GetVID());
				pParty->Link(pkHealer);
				bNextHealer = true;
			}
			else
			{
				range = number(HEALER_MIN_RANGE, HEALER_MAX_RANGE);
				sx = pkLeader->GetX() - range;
				sy = pkLeader->GetY() - range;
				ex = pkLeader->GetX() + range;
				ey = pkLeader->GetY() + range;

				bSpawnAttemptCount++;
			}
		}
	}

	pkLeader->SetMaxSP(pkLeader->GetMaxSP() + 1);
}

bool CDawnMistDungeon::IsBoss(const LPCHARACTER pkChar)
{
	const DWORD dwRaceNum = pkChar->GetRaceNum();
	const long lMapIndex = pkChar->GetMapIndex();

	if (dwRaceNum == BOSS_VNUM && (lMapIndex >= MAP_INDEX * 10000 && lMapIndex < (MAP_INDEX + 1) * 10000))
		return true;

#if defined(__LABYRINTH_DUNGEON__)
	if (dwRaceNum == TIME_RIFT_BOSS_VNUM && (lMapIndex >= TIME_RIFT_MAP_INDEX * 10000 && lMapIndex < (TIME_RIFT_MAP_INDEX + 1) * 10000))
		return true;

	if (dwRaceNum == REDUX_BOSS_VNUM && (lMapIndex >= REDUX_MAP_INDEX * 10000 && lMapIndex < (REDUX_MAP_INDEX + 1) * 10000))
		return true;
#endif

	return false;
}

DWORD CDawnMistDungeon::GetBossHealer(const DWORD dwRaceNum) const
{
	if (dwRaceNum == BOSS_VNUM)
		return HEALER_VNUM;

#if defined(__LABYRINTH_DUNGEON__)
	if (dwRaceNum == TIME_RIFT_BOSS_VNUM)
		return TIME_RIFT_HEALER_VNUM;

	if (dwRaceNum == REDUX_BOSS_VNUM)
		return REDUX_HEALER_VNUM;
#endif

	return 0;
}
#endif
