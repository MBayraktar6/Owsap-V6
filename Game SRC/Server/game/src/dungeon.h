#ifndef __INC_DUNGEON_H__
#define __INC_DUNGEON_H__

#include "sectree_manager.h"

class CParty;

typedef std::map<std::string, int> FlagMap;
typedef std::vector<LPREGEN> RegenVector;

#if defined(__DUNGEON_RENEWAL__)
enum EDungeonType
{
	DUNGEON_TYPE_DEFAULT,
#if defined(__DEFENSE_WAVE__)
	DUNGEON_TYPE_DEFENSE_WAVE,
#endif
};
#endif

class CDungeon
{
	typedef std::unordered_map<LPPARTY, int> TPartyMap;
	typedef std::map<std::string, LPCHARACTER> TUniqueMobMap;

public:
	// <Factor> Non-persistent identifier type
	typedef uint32_t IdType;

#if defined(__DUNGEON_RENEWAL__) || defined(__DEFENSE_WAVE__)
	virtual ~CDungeon();
#else
	~CDungeon();
#endif

	// <Factor>
	IdType GetId() const { return m_id; }

	// DUNGEON_NOTICE
	void Notice(const char* msg);
	void MissionMessage(const char* msg);
	void SubMissionMessage(const char* msg);
	// END_OF_DUNGEON_NOTICE

#if defined(__DUNGEON_RENEWAL__) || defined(__DEFENSE_WAVE__)
	virtual void JoinParty(LPPARTY pParty);
	virtual void QuitParty(LPPARTY pParty);
#else
	void JoinParty(LPPARTY pParty);
	void QuitParty(LPPARTY pParty);
#endif

	void Join(LPCHARACTER ch);

#if defined(__DUNGEON_RENEWAL__) || defined(__DEFENSE_WAVE__)
	virtual void IncMember(LPCHARACTER ch);
	virtual void DecMember(LPCHARACTER ch);
#else
	void IncMember(LPCHARACTER ch);
	void DecMember(LPCHARACTER ch);
#endif

#if defined(__DUNGEON_RENEWAL__)
	void Destroy();
#endif

	// DUNGEON_KILL_ALL_BUG_FIX
	void Purge();
	void KillAll();
	// END_OF_DUNGEON_KILL_ALL_BUG_FIX

	void IncMonster() { m_iMonsterCount++; /*sys_log(0, "MonsterCount %d", m_iMonsterCount);*/ }
	void DecMonster() { m_iMonsterCount--; CheckEliminated(); }
	int CountMonster() { return m_iMonsterCount; } // 데이터로 리젠한 몬스터의 수
	int CountRealMonster(); // 실제로 맵상에 있는 몬스터

#if defined(__DUNGEON_RENEWAL__) || defined(__DEFENSE_WAVE__)
	virtual void IncPartyMember(LPPARTY pParty, LPCHARACTER ch);
	virtual void DecPartyMember(LPPARTY pParty, LPCHARACTER ch);
#else
	void IncPartyMember(LPPARTY pParty, LPCHARACTER ch);
	void DecPartyMember(LPPARTY pParty, LPCHARACTER ch);
#endif

	void IncKillCount(LPCHARACTER pkKiller, LPCHARACTER pkVictim);
	int GetKillMobCount();
	int GetKillStoneCount();
	bool IsUsePotion();
	bool IsUseRevive();
	void UsePotion(LPCHARACTER ch);
	void UseRevive(LPCHARACTER ch);

	long GetMapIndex() { return m_lMapIndex; }

	void Spawn(DWORD vnum, const char* pos);
	LPCHARACTER SpawnMob(DWORD vnum, int x, int y, int dir = 0);
	LPCHARACTER SpawnMob_ac_dir(DWORD vnum, int x, int y, int dir = 0);
	LPCHARACTER SpawnGroup(DWORD vnum, long x, long y, float radius, bool bAggressive = false, int count = 1);

	void SpawnNameMob(DWORD vnum, int x, int y, const char* name);
	void SpawnGotoMob(long lFromX, long lFromY, long lToX, long lToY);

	void SpawnRegen(const char* filename, bool bOnce = true);
	void AddRegen(LPREGEN regen);
	void ClearRegen();
	bool IsValidRegen(LPREGEN regen, size_t regen_id);

	void SetUnique(const char* key, DWORD vid);
	void SpawnMoveUnique(const char* key, DWORD vnum, const char* pos_from, const char* pos_to);
	void SpawnMoveGroup(DWORD vnum, const char* pos_from, const char* pos_to, int count = 1);
	void SpawnUnique(const char* key, DWORD vnum, const char* pos);
	void SpawnStoneDoor(const char* key, const char* pos);
	void SpawnWoodenDoor(const char* key, const char* pos);

	void KillUnique(const std::string& key);
	void PurgeUnique(const std::string& key);
	bool IsUniqueDead(const std::string& key);
	float GetUniqueHpPerc(const std::string& key);
	DWORD GetUniqueVID(const std::string& key);
	bool IsUnique(LPCHARACTER pChar);
	LPCHARACTER GetUnique(const std::string& key);

#if defined(__DUNGEON_RENEWAL__) || defined(__DEFENSE_WAVE__)
	virtual void DeadCharacter(LPCHARACTER ch);
#else
	void DeadCharacter(LPCHARACTER ch);
#endif

	void UniqueSetMaxHP(const std::string& key, DWORD dwMaxHP);
	void UniqueSetHP(const std::string& key, int iHP);
	void UniqueSetDefGrade(const std::string& key, int iGrade);

	void SendDestPositionToParty(LPPARTY pParty, long x, long y);

	void CheckEliminated();

	void JumpAll(long lFromMapIndex, int x, int y);
	void WarpAll(long lFromMapIndex, int x, int y);
	void JumpParty(LPPARTY pParty, long lFromMapIndex, int x, int y);

#if defined(__DUNGEON_RENEWAL__)
	void SetParty(LPPARTY pParty);
	LPPARTY GetParty() const { return m_pParty; }
#endif

	void ExitAll();
	void ExitAllToStartPosition();
	void JumpToEliminateLocation();
	void SetExitAllAtEliminate(long time);
	void SetWarpAtEliminate(long time, long lMapIndex, int x, int y, const char* regen_file);

	int GetFlag(std::string name);
	void SetFlag(std::string name, int value);
	void SetWarpLocation(long map_index, int x, int y);

	// item group은 item_vnum과 item_count로 구성.
	typedef std::vector <std::pair <DWORD, int>> ItemGroup;
	void CreateItemGroup(std::string& group_name, ItemGroup& item_group);
	const ItemGroup* GetItemGroup(std::string& group_name);
	//void InsertItemGroup(std::string& group_name, DWORD item_vnum);

	template <class Func> Func ForEachMember(Func f);

	bool IsAllPCNearTo(int x, int y, int dist);

#if defined(__DUNGEON_RENEWAL__) || defined(__DEFENSE_WAVE__)
public:
#else
protected:
#endif
	CDungeon(IdType id, long lOriginalMapIndex, long lMapIndex);

protected:
	void Initialize();
	void CheckDestroy();

private:
	IdType m_id; // <Factor>
	DWORD m_lOrigMapIndex;
	DWORD m_lMapIndex;

	CHARACTER_SET m_set_pkCharacter;
	FlagMap m_map_Flag;
	typedef std::map<std::string, ItemGroup> ItemGroupMap;
	ItemGroupMap m_map_ItemGroup;
	TPartyMap m_map_pkParty;
	TAreaMap& m_map_Area;
	TUniqueMobMap m_map_UniqueMob;

	int m_iMobKill;
	int m_iStoneKill;
	bool m_bUsePotion;
	bool m_bUseRevive;

	int m_iMonsterCount;

	bool m_bExitAllAtEliminate;
	bool m_bWarpAtEliminate;

	// 적 전멸시 워프하는 위치
	int m_iWarpDelay;
	long m_lWarpMapIndex;
	long m_lWarpX;
	long m_lWarpY;
	std::string m_stRegenFile;

	RegenVector m_regen;

	LPEVENT deadEvent;
	// <Factor>
	LPEVENT exit_all_event_;
	LPEVENT jump_to_event_;
	size_t regen_id_;

	friend class CDungeonManager;
	friend EVENTFUNC(dungeon_dead_event);
	// <Factor>
	friend EVENTFUNC(dungeon_exit_all_event);
	friend EVENTFUNC(dungeon_jump_to_event);

	// 파티 단위 던전 입장을 위한 임시 변수.
	// m_map_pkParty는 관리가 부실하여 사용할 수 없다고 판단하여,
	// 임시로 한 파티에 대한 관리를 하는 변수 생성.

	LPPARTY m_pParty;

public:
	void SetPartyNull();
};

class CDungeonManager : public singleton<CDungeonManager>
{
	typedef std::map<CDungeon::IdType, LPDUNGEON> TDungeonMap;
	typedef std::map<long, LPDUNGEON> TMapDungeon;

public:
	CDungeonManager();
	virtual ~CDungeonManager();

	LPDUNGEON Create(long lOriginalMapIndex
#if defined(__DUNGEON_RENEWAL__) || defined(__DEFENSE_WAVE__)
		, EDungeonType eType = DUNGEON_TYPE_DEFAULT
#endif
	);
	void Destroy(CDungeon::IdType dungeon_id);
	LPDUNGEON Find(CDungeon::IdType dungeon_id);
	LPDUNGEON FindByMapIndex(long lMapIndex);

private:
	TDungeonMap m_map_pkDungeon;
	TMapDungeon m_map_pkMapDungeon;

	// <Factor> Introduced unsigned 32-bit dungeon identifier
	CDungeon::IdType next_id_;
};

#endif // __INC_DUNGEON_H__
