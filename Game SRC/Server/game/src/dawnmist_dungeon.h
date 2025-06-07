/**
* Filename: dawnmist_dungeon.h
* Author: Owsap
**/

#if !defined(__INC_DAWNMIST_DUNGEON__) && defined(__DAWNMIST_DUNGEON__)
#define __INC_DAWNMIST_DUNGEON__

class CDawnMistDungeon : public singleton<CDawnMistDungeon>
{
public:
	static constexpr long MAP_INDEX = 353;
#if defined(__LABYRINTH_DUNGEON__)
	static constexpr long TIME_RIFT_MAP_INDEX = 367;
	static constexpr long REDUX_MAP_INDEX = 371;
#endif

	static constexpr DWORD BOSS_VNUM = 6192;
	static constexpr DWORD HEALER_VNUM = 6409;

#if defined(__LABYRINTH_DUNGEON__)
	static constexpr DWORD TIME_RIFT_BOSS_VNUM = 4403;
	static constexpr DWORD TIME_RIFT_HEALER_VNUM = 4266;

	static constexpr DWORD REDUX_BOSS_VNUM = 4413;
	static constexpr DWORD REDUX_HEALER_VNUM = 4346;
#endif

	static constexpr BYTE HEALER_COUNT = 4;
	static constexpr BYTE HEALING_PULSE = 5;

	static constexpr WORD HEALER_MIN_RANGE = 300;
	static constexpr WORD HEALER_MAX_RANGE = 900;

	static constexpr BYTE MAX_HEAL_STAGE = 4;
	static constexpr BYTE HEAL_ON_HP_PCT[MAX_HEAL_STAGE] = { 75, 50, 25, 10 };

	static constexpr PIXEL_POSITION HEALER_POSITION[MAX_HEAL_STAGE]
	{
		{ 776500, 1503700 }, // Healer 1
		{ 773100, 1503700 }, // Healer 2
		{ 773200, 1500400 }, // Healer 3
		{ 776500, 1500400 }, // Healer 4
	};

	static constexpr BYTE SPECIAL_SKILL_INDEX = 2;
	static constexpr int SPECIAL_SKILL_VNUM = 306;

public:
	CDawnMistDungeon() = default;
	virtual ~CDawnMistDungeon() = default;

public:
	struct FPartyHealer
	{
		void operator() (const LPENTITY pkEntity);
	};

	bool CanSpawnHealerGroup(const LPCHARACTER pkLeader);
	void SpawnHealerGroup(const LPCHARACTER pkLeader);

	bool IsBoss(const LPCHARACTER pkChar);
	DWORD GetBossHealer(const DWORD dwRaceNum) const;
};
#endif // __INC_DAWNMIST_DUNGEON__ / __DAWNMIST_DUNGEON__
