/**
* Filename: defense_wave.h
* Author: Owsap
**/

#if !defined(__INC_DEFENSE_WAVE_H__) && defined(__DEFENSE_WAVE__)
#define __INC_DEFENSE_WAVE_H__

#include "dungeon.h"

class CDefenseWave : public CDungeon
{
public:
	enum EWaves
	{
		WAVE0,
		WAVE1,
		WAVE2,
		WAVE3,
		WAVE4,
		WAVE_MAX_NUM,
	};

	enum EMisc
	{
		LASER_MAX_NUM = 4,
		SPAWN_WOOD_REPAIR = true,
	};

	typedef struct SMobIgnoreList
	{
	public:
		void AddRankToIgnore(BYTE bMonRank) { m_bIgnoreMobRank.emplace(bMonRank); }
		void AddRaceToIgnore(DWORD dwRaceNum) { m_bIgnoreMobRace.emplace(dwRaceNum); }

		const std::unordered_set<BYTE>& GetIgnoreMobRank() const { return m_bIgnoreMobRank; }
		const std::unordered_set<DWORD>& GetIgnoreMobRace() const { return m_bIgnoreMobRace; }

	private:
		std::unordered_set<BYTE> m_bIgnoreMobRank;
		std::unordered_set<DWORD> m_bIgnoreMobRace;
	} TMobIgnoreList;

public:
	CDefenseWave(DWORD dwID, long lOriginalMapIndex, long lPrivateMapIndex);
	virtual ~CDefenseWave();

	void Initialize();
	void Destroy();

	void ChatPacket(BYTE bChatType, const char* szText);

	bool ChangeAllianceHP(BYTE bHealthPct);
	void Debuff();

	bool Enter(LPCHARACTER pChar);
	bool Exit(LPCHARACTER pChar);
	void ExitAll();
	void Start();

	LPCHARACTER SpawnMob(DWORD dwVnum, int iX, int iY, int iDir = 0, bool bSpawnMotion = false, bool bSetVictim = true);

	void SpawnHydraSpawn(BYTE bIndex);
	void SpawnEgg();

	void TriggerLaser(BYTE bLaserIdx);
	void CheckAffect(LPCHARACTER pChar, long x, long y);

	bool Damage(LPCHARACTER pCharVictim);
	void DeadCharacter(LPCHARACTER pCharVictim);

	void CreateWalls();
	void RemoveWalls();
	void RemoveBackWalls();
	void RemoveFrontWalls();

	void PrepareWave(BYTE bWave);
	void SetWave(BYTE bWave);
	BYTE GetWave() const { return m_bWave; }

	void ClearWave();
	void ClearMobs(const TMobIgnoreList& rkIgnoreMobList);

	void JumpToStartLocation();

	void SetDestroyEvent(LPEVENT pEvent);
	void SetPurgeEvent(LPEVENT pEvent);
	void SetWaveEvent(LPEVENT pEvent);
	void SetEggEvent(LPEVENT pEvent);
	void SetHydraSpawnEvent(BYTE bIndex, LPEVENT pEvent);
	void SetLaserEvent(BYTE bIndex, LPEVENT pEvent);
	void SetSpawnEvent(LPEVENT pEvent);
	void SetDebuffEvent(LPEVENT pEvent);
	void CancelEvents();

	void JoinParty(LPPARTY pParty) {};
	void QuitParty(LPPARTY pParty) {};

	void IncPartyMember(LPCHARACTER pChar) {};
	void DecPartyMember(LPCHARACTER pChar) {};

	void IncMember(LPCHARACTER pChar) {};
	void DecMember(LPCHARACTER pChar) {};

private:
	BYTE m_bWave;

	LPEVENT m_pDestroyEvent;
	LPEVENT m_pPurgeEvent;
	LPEVENT m_pWaveEvent;
	LPEVENT m_pEggEvent;
	LPEVENT m_pHydraSpawnEvent[LASER_MAX_NUM];
	LPEVENT m_pLaserEvent[LASER_MAX_NUM];
	LPEVENT m_pSpawnEvent;
	LPEVENT m_pDebuffEvent;

	DWORD m_dwStartTime;
	int m_iLastAllianceHPPct;
};

class CDefenseWaveManager : public singleton<CDefenseWaveManager>
{
	typedef std::unordered_map<DWORD, LPDEFENSE_WAVE> TDefenseWave;
	typedef std::unordered_map<long, LPDEFENSE_WAVE> TMapDefenseWave;

public:
	CDefenseWaveManager();
	virtual ~CDefenseWaveManager();

	LPDEFENSE_WAVE Create(long lOriginalMapIndex);
	void Destroy(DWORD dwDungeonID);

	LPDEFENSE_WAVE Find(DWORD dwDungeonID) const;
	LPDEFENSE_WAVE FindByMapIndex(long lMapIndex) const;
	LPDEFENSE_WAVE GetDungeon(LPDUNGEON pDungeon) const;

	bool Enter(LPCHARACTER pChar, long lMapIndex);
	bool Exit(LPCHARACTER pChar, long lMapIndex);

	bool CanAttack(LPCHARACTER pCharAttacker, LPCHARACTER pCharVictim);

	bool IsShipMast(DWORD dwRaceNum) const;
	bool IsShipWheel(DWORD dwRaceNum) const;
	bool IsWoodRepair(DWORD dwRaceNum) const;

	bool IsHydraSpawn(DWORD dwRaceNum) const;
	bool IsLeftHydra(DWORD dwRaceNum) const;
	bool IsRightHydra(DWORD dwRaceNum) const;
	bool IsHydra(DWORD dwRaceNum) const;

	bool IsDefenseWaveMap(long lMapIndex) const;

private:
	TDefenseWave m_map_pDefenseWave;
	TMapDefenseWave m_map_pMapDefenseWave;
};
#endif // __INC_DEFENSE_WAVE_H__
