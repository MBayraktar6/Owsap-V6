#pragma once

namespace BlueDragon
{
	constexpr DWORD BossVnum = 2493;
	constexpr BYTE StoneCount = 4;
	constexpr DWORD MapIndex = 208;
	constexpr DWORD StoneVnum[StoneCount] = { 8031, 8032, 8033, 8034 };

#if defined(__LABYRINTH_DUNGEON__)
	constexpr DWORD TimeRift_BossVnum = 4400;
	constexpr DWORD TimeRift_MapIndex = 364;
	constexpr DWORD TimeRift_StoneVnum[StoneCount] = { 8120, 8121, 8122, 8123 };

	constexpr DWORD Redux_BossVnum = 4410;
	constexpr DWORD Redux_MapIndex = 368;
	constexpr DWORD Redux_StoneVnum[StoneCount] = { 8130, 8131, 8132, 8133 };
#endif
}

extern int BlueDragon_StateBattle(LPCHARACTER);
extern time_t UseBlueDragonSkill(LPCHARACTER, unsigned int idx);
extern int BlueDragon_Damage(LPCHARACTER me, LPCHARACTER attacker, int dam);
#if defined(__BLUE_DRAGON_RENEWAL__)
extern bool BlueDragon_Block(long idx);
extern bool BlueDragon_IsBoss(DWORD vnum);
#endif
#if defined(__LABYRINTH_DUNGEON__)
extern std::string BlueDragon_GetVnumFieldByBossVnum(DWORD vnum);
#endif
