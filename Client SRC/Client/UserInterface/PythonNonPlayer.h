#pragma once

#include "StdAfx.h"

/*
* NPC 데이터 프로토 타잎을 관리 한다.
*/

class CPythonNonPlayer : public CSingleton<CPythonNonPlayer>
{
public:
	enum EClickEvent
	{
		ON_CLICK_EVENT_NONE,
		ON_CLICK_EVENT_SHOP,
		ON_CLICK_EVENT_TALK,
		ON_CLICK_EVENT_MAX_NUM,
	};

	enum EMobRaceFlags
	{
		RACE_FLAG_ANIMAL = (1 << 0),
		RACE_FLAG_UNDEAD = (1 << 1),
		RACE_FLAG_DEVIL = (1 << 2),
		RACE_FLAG_HUMAN = (1 << 3),
		RACE_FLAG_ORC = (1 << 4),
		RACE_FLAG_MILGYO = (1 << 5),
		RACE_FLAG_INSECT = (1 << 6),
		RACE_FLAG_DESERT = (1 << 7),
		RACE_FLAG_TREE = (1 << 8),
		RACE_FLAG_DECO = (1 << 9),
		RACE_FLAG_HIDE = (1 << 10),
		RACE_FLAG_ATT_CZ = (1 << 11),
		RACE_FLAG_AWEAKEN = (1 << 12),
		RACE_FLAG_SUNGMAHEE = (1 << 13),
		RACE_FLAG_OUTPOST = (1 << 14),
	};

	enum EMobEnchants
	{
		MOB_ENCHANT_CURSE,
		MOB_ENCHANT_SLOW,
		MOB_ENCHANT_POISON,
		MOB_ENCHANT_STUN,
		MOB_ENCHANT_CRITICAL,
		MOB_ENCHANT_PENETRATE,
		MOB_ENCHANTS_MAX_NUM
	};

	enum EMobResists
	{
		MOB_RESIST_FIST,

		MOB_RESIST_SWORD,
		MOB_RESIST_TWOHAND,
		MOB_RESIST_DAGGER,
		MOB_RESIST_BELL,
		MOB_RESIST_FAN,
		MOB_RESIST_BOW,
		MOB_RESIST_CLAW,

		MOB_RESIST_FIRE,
		MOB_RESIST_ELECT,
		MOB_RESIST_MAGIC,
		MOB_RESIST_WIND,

		MOB_RESIST_POISON,
		MOB_RESIST_BLEEDING,

		MOB_RESISTS_MAX_NUM
	};

#if defined(ENABLE_ELEMENT_ADD)
	enum EMobElements
	{
		MOB_ELEMENT_ELECT,
		MOB_ELEMENT_FIRE,
		MOB_ELEMENT_ICE,
		MOB_ELEMENT_WIND,
		MOB_ELEMENT_EARTH,
		MOB_ELEMENT_DARK,
		MOB_ELEMENT_MAX_NUM
	};
#endif

#define MOB_SKILL_MAX_NUM 5

#pragma pack(push)
#pragma pack(1)
	typedef struct SMobSkillLevel
	{
		DWORD dwVnum;
		BYTE bLevel;
	} TMobSkillLevel;

	typedef struct SEntityTable
	{
		DWORD dwVnum;
	} TEntityTable;

	typedef struct SMobTable : public SEntityTable
	{
		char szName[CHARACTER_NAME_MAX_LEN + 1];
		char szLocaleName[CHARACTER_NAME_MAX_LEN + 1];

		BYTE bType; // Monster, NPC
		BYTE bRank; // PAWN, KNIGHT, KING
		BYTE bBattleType; // MELEE, etc..
		BYTE bLevel;
		BYTE bScale;
		BYTE bSize;

		DWORD dwGoldMin;
		DWORD dwGoldMax;
		DWORD dwExp;
		DWORD dwSungMaExp;
		DWORD dwMaxHP;
		BYTE bRegenCycle;
		BYTE bRegenPercent;
		WORD wDef;

		DWORD dwAIFlag;
		DWORD dwRaceFlag;
		DWORD dwImmuneFlag;

		BYTE bStr, bDex, bCon, bInt;
		BYTE bSungMaStr, bSungMaDex, bSungMaCon, bSungMaInt;
		DWORD dwDamageRange[2];

		short sAttackSpeed;
		short sMovingSpeed;

		BYTE bAggressiveHPPct;
		WORD wAggressiveSight;
		WORD wAttackRange;

		char cEnchants[MOB_ENCHANTS_MAX_NUM];
		char cResists[MOB_RESISTS_MAX_NUM];
#if defined(ENABLE_ELEMENT_ADD)
		char cElements[MOB_ELEMENT_MAX_NUM];
		char cResistDark, cResistIce, cResistEarth;
#endif

		DWORD dwResurrectionVnum;
		DWORD dwDropItemVnum;

		BYTE bMountCapacity;
		BYTE bOnClickType;

		BYTE bEmpire;
		char szFolder[CHARACTER_FOLDER_MAX_LEN + 1];

		float fDamMultiply;

		DWORD dwSummonVnum;
		DWORD dwDrainSP;
		DWORD dwMobColor;
		DWORD dwPolymorphItemVnum;

		TMobSkillLevel Skills[MOB_SKILL_MAX_NUM];

		BYTE bBerserkPoint;
		BYTE bStoneSkinPoint;
		BYTE bGodSpeedPoint;
		BYTE bDeathBlowPoint;
		BYTE bRevivePoint;

		BYTE bHealPoint;

		BYTE bRAttSpeedPoint;
		BYTE bRCastSpeedPoint;

		BYTE bRHPRegenPoint;

		float fHitRange;
	} TMobTable;
#pragma pack(pop)

	typedef std::list<TMobTable*> TMobTableList;
	typedef std::map<DWORD, TMobTable*> TNonPlayerDataMap;

public:
	CPythonNonPlayer(void);
	virtual ~CPythonNonPlayer(void);

	void Clear();
	void Destroy();

	bool LoadNonPlayerData(const char* c_szFileName);

	TMobTable* GetTable(DWORD dwVnum);
	bool GetName(DWORD dwVnum, const char** c_pszName);
	BYTE GetRank(DWORD dwVnum);
	bool GetInstanceType(DWORD dwVnum, BYTE* pbType);
	BYTE GetEventType(DWORD dwVnum);
	BYTE GetEventTypeByVID(DWORD dwVID);
	DWORD GetMonsterColor(DWORD dwVnum);
	const char* GetMonsterName(DWORD dwVnum);

	BYTE GetMonsterScalePercent(DWORD dwVnum);
	float GetMonsterHitRange(DWORD dwVnum);

	// Function for outer
	void GetMatchableMobList(int iLevel, int iInterval, TMobTableList* pMobTableList);

protected:
	TNonPlayerDataMap m_NonPlayerDataMap;

#if defined(ENABLE_SEND_TARGET_INFO)
public:
	enum EMonsterItemDrop
	{
		MAX_MOB_ITEM_DROP_GRID_SLOT_X = 5,
		MAX_MOB_ITEM_DROP_GRID_SLOT_Y = 8,
		MAX_MOB_ITEM_DROP_GRID_SIZE = MAX_MOB_ITEM_DROP_GRID_SLOT_X * MAX_MOB_ITEM_DROP_GRID_SLOT_Y,
	};

	struct SMobItemDrop
	{
		DWORD dwVnum;
		WORD wCount;
	};

	using TMobDropItemVec = std::vector<SMobItemDrop>;
	using TMobDropItemMap = std::unordered_map<DWORD, TMobDropItemVec>;

public:
	BOOL SelectMobData(DWORD dwRaceVnum);
	TMobTable* GetSelectedMobDataPointer();

	void AddItemDrop(const DWORD dwRaceVnum, const DWORD dwVnum, const WORD wCount);
	void ClearItemDrop();

	TMobDropItemVec* GetItemDropVec(const DWORD dwVnum);

	void SetDropMetinStone(bool bDropMetinStone) { m_bDropMetinStone = bDropMetinStone; }
	bool GetDropMetinStone() const { return m_bDropMetinStone; }

protected:
	TMobTable* m_pSelectedMobData;
	TMobDropItemMap m_MobItemDropMap;
	bool m_bDropMetinStone;
#endif
};
