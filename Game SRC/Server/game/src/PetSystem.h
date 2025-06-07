#if !defined(__INC_PET_SYSTEM_H__) && defined(__PET_SYSTEM__)
#define __INC_PET_SYSTEM_H__

class CHARACTER;

// TODO: 펫으로서의 능력치? 라던가 친밀도, 배고픔 기타등등... 수치
struct SPetAbility
{
};

/**
*/
class CPetActor //: public CHARACTER
{
public:
	enum EPetOptions
	{
		EPetOption_Followable = 1 << 0,
		EPetOption_Mountable = 1 << 1,
		EPetOption_Summonable = 1 << 2,
		EPetOption_Combatable = 1 << 3,
#if defined(__PET_LOOT__)
		EPetOption_Lootable = 1 << 4,
#endif
	};

protected:
	friend class CPetSystem;

	CPetActor(LPCHARACTER owner, DWORD vnum, DWORD options = EPetOption_Followable | EPetOption_Summonable);
	//CPetActor(LPCHARACTER owner, DWORD vnum, const SPetAbility& petAbility, DWORD options = EPetOption_Followable | EPetOption_Summonable);

	virtual ~CPetActor();

	virtual bool Update(DWORD deltaTime);

protected:
	virtual bool _UpdateFollowAI(); ///< 주인을 따라다니는 AI 처리
	virtual bool _UpdatAloneActionAI(float fMinDist, float fMaxDist); ///< 주인 근처에서 혼자 노는 AI 처리
#if defined(__PET_LOOT_AI__)
	virtual void _UpdateLoot(float fMaxDist = 3000.0f);
#endif

	/// @TODO
	//virtual bool _UpdateCombatAI();

private:
	bool Follow(float fMinDist = 50.f);

#if defined(__PET_LOOT_AI__)
public:
	bool Loot(DWORD dwVID, float fMinDist = 300.f);
#endif

public:
	LPCHARACTER GetCharacter() const { return m_pkChar; }
	LPCHARACTER GetOwner() const { return m_pkOwner; }
	DWORD GetVID() const { return m_dwVID; }
	DWORD GetVnum() const { return m_dwVnum; }

	bool HasOption(EPetOptions option) const { return m_dwOptions & option; }

	void SetName(const char* petName);

	bool Mount();
	void Unmount();

	DWORD Summon(const char* petName, LPITEM pSummonItem, bool bSpawnFar = false);
	void Unsummon();

	bool IsSummoned() const { return 0 != m_pkChar; }
	void SetSummonItem(LPITEM pItem);
	DWORD GetSummonItemVID() { return m_dwSummonItemVID; }
	DWORD GetSummonItemVnum() { return m_dwSummonItemVnum; }
	// 버프 주는 함수와 거두는 함수.
	// 이게 좀 괴랄한게, 서버가 ㅄ라서, 
	// POINT_MOV_SPEED, POINT_ATT_SPEED, POINT_CAST_SPEED는 PointChange()란 함수만 써서 변경해 봐야 소용이 없는게,
	// PointChange() 이후에 어디선가 ComputePoints()를 하면 싹다 초기화되고, 
	// 더 웃긴건, ComputePoints()를 부르지 않으면 클라의 POINT는 전혀 변하지 않는다는 거다.
	// 그래서 버프를 주는 것은 ComputePoints() 내부에서 petsystem->RefreshBuff()를 부르도록 하였고,
	// 버프를 빼는 것은 ClearBuff()를 부르고, ComputePoints를 하는 것으로 한다.
	void GiveBuff();
	void ClearBuff();
	bool CheckBuff(const CPetActor* pPetActor);

private:
	DWORD m_dwVnum;
	DWORD m_dwVID;
	DWORD m_dwOptions;
	DWORD m_dwLastActionTime;
	DWORD m_dwSummonItemVID;
	DWORD m_dwSummonItemVnum;

	short m_originalMoveSpeed;

	std::string m_name;

	LPCHARACTER m_pkChar; // Instance of pet(CHARACTER)
	LPCHARACTER m_pkOwner;

	//SPetAbility m_petAbility; // 능력치
};

class CPetSystem
{
public:
	typedef std::unordered_map<DWORD, CPetActor*> TPetActorMap; /// <VNUM, PetActor> map. (한 캐릭터가 같은 vnum의 펫을 여러개 가질 일이 있을까..??)

public:
	CPetSystem(LPCHARACTER owner);
	virtual ~CPetSystem();

	CPetActor* GetByVID(DWORD vid) const;
	CPetActor* GetByVnum(DWORD vnum) const;
	DWORD GetVnum() const;
	DWORD GetSummonItemVID() const;
	DWORD GetSummonItemVnum() const;

	bool Update(DWORD deltaTime);
	void Destroy();

	size_t CountSummoned() const; ///< 현재 소환된(실체화 된 캐릭터가 있는) 펫의 개수

public:
	void SetUpdatePeriod(DWORD ms);

	CPetActor* Summon(DWORD mobVnum, LPITEM pSummonItem, const char* petName, bool bSpawnFar, DWORD options = CPetActor::EPetOption_Followable | CPetActor::EPetOption_Summonable);

	void Unsummon(DWORD mobVnum, bool bDeleteFromList = false);
	void Unsummon(CPetActor* petActor, bool bDeleteFromList = false);

	// TODO: 진짜 펫 시스템이 들어갈 때 구현. (캐릭터가 보유한 펫의 정보를 추가할 때 라던가...)
	CPetActor* AddPet(DWORD mobVnum, const char* petName, const SPetAbility& ability, DWORD options = CPetActor::EPetOption_Followable | CPetActor::EPetOption_Summonable | CPetActor::EPetOption_Combatable);

	void DeletePet(DWORD mobVnum);
	void DeletePet(CPetActor* petActor);
	void RefreshBuff();

#if defined(__PET_LOOT__)
	bool SummonedLooter() const;
	bool LootItem(const LPITEM pItem);
#endif

private:
	TPetActorMap m_petActorMap;
	LPCHARACTER m_pkOwner; ///< 펫 시스템의 Owner
	DWORD m_dwUpdatePeriod; ///< 업데이트 주기 (ms단위)
	DWORD m_dwLastUpdateTime;
	LPEVENT m_pkPetSystemUpdateEvent;
};

#endif // __INC_PET_SYSTEM_H__
