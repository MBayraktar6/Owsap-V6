#pragma once

#include "AbstractPlayer.h"
#include "Packet.h"
#include "PythonSkill.h"

class CInstanceBase;

/*
* 메인 캐릭터 (자신이 조정하는 캐릭터) 가 가진 정보들을 관리한다.
*
* 2003-01-12 Levites 본래는 CPythonCharacter가 가지고 있었지만 규모가 너무 커져 버린데다
* 위치도 애매해서 따로 분리
* 2003-07-19 Levites 메인 캐릭터의 이동 처리 CharacterInstance에서 떼어다 붙임
* 기존의 데이타 보존의 역할에서 완벽한 메인 플레이어 제어 클래스로
* 탈바꿈 함.
*/

class CPythonPlayer : public CSingleton<CPythonPlayer>, public IAbstractPlayer
{
public:
	enum
	{
		CATEGORY_NONE = 0,
		CATEGORY_ACTIVE = 1,
		CATEGORY_PASSIVE = 2,
		CATEGORY_MAX_NUM = 3,

		STATUS_INDEX_ST = 1,
		STATUS_INDEX_DX = 2,
		STATUS_INDEX_IQ = 3,
		STATUS_INDEX_HT = 4,
	};

	enum
	{
		MBT_LEFT,
		MBT_RIGHT,
		MBT_MIDDLE,
		MBT_NUM,
	};

	enum
	{
		MBF_SMART,
		MBF_MOVE,
		MBF_CAMERA,
		MBF_ATTACK,
		MBF_SKILL,
		MBF_AUTO,
	};

	enum
	{
		MBS_CLICK,
		MBS_PRESS,
	};

	enum EMode
	{
		MODE_NONE,
		MODE_CLICK_POSITION,
		MODE_CLICK_ITEM,
		MODE_CLICK_ACTOR,
		MODE_USE_SKILL,
	};

	enum EEffect
	{
		EFFECT_PICK,
		EFFECT_NUM,
	};

	enum EMetinSocketType
	{
		METIN_SOCKET_TYPE_NONE,
		METIN_SOCKET_TYPE_SILVER,
		METIN_SOCKET_TYPE_GOLD,
	};

#if defined(ENABLE_EVENT_BANNER)
	enum EMiniGameType
	{
#if defined(ENABLE_MINI_GAME_RUMI)
		MINIGAME_RUMI,
#endif
		//MINIGAME_ATTENDANCE,
		//MINIGAME_FISH,
		//MINIGAME_MONSTERBACK,
#if defined(ENABLE_MINI_GAME_YUTNORI)
		MINIGAME_YUTNORI,
#endif
#if defined(ENABLE_MINI_GAME_CATCH_KING)
		MINIGAME_CATCHKING,
#endif
		//MINIGAME_BNW,
		//BATTLE_ROYALE_EVENT,
#if defined(ENABLE_SUMMER_EVENT_ROULETTE)
		MINIGAME_ROULETTE,
#endif
		//WORLD_BOSS_EVENT,
#if defined(ENABLE_FLOWER_EVENT)
		FLOWER_EVENT,
#endif
		//OTHER_WORLD_EVENT,
		//RAMADAN_EVENT,
		//ICECREAM_EVENT,
		//GOLDEN_LAND_EVENT,
#if defined(ENABLE_SNOWFLAKE_STICK_EVENT)
		SNOWFLAKE_STICK_EVENT,
#endif
		//SPORTS_MATCH_EVENT,
		//VALENTINE_DAY_EVENT,
		//FOOTBALL_EVENT,
		//UNK19
		//TREASURE_HUNT_EVENT,
		MINIGAME_TYPE_MAX
	};
#endif

	typedef struct SSkillInstance
	{
		DWORD dwIndex;
		int iType;
		int iGrade;
		int iLevel;
		float fcurEfficientPercentage;
		float fnextEfficientPercentage;
		BOOL isCoolTime;

		float fCoolTime; // NOTE : 쿨타임 중인 스킬 슬롯을
		float fLastUsedTime; // 퀵창에 등록할 때 사용하는 변수
		BOOL bActive;
	} TSkillInstance;

	enum EKeyBoard_UD
	{
		KEYBOARD_UD_NONE,
		KEYBOARD_UD_UP,
		KEYBOARD_UD_DOWN,
	};

	enum EKeyBoard_LR
	{
		KEYBOARD_LR_NONE,
		KEYBOARD_LR_LEFT,
		KEYBOARD_LR_RIGHT,
	};

	enum
	{
		DIR_UP,
		DIR_DOWN,
		DIR_LEFT,
		DIR_RIGHT,
	};

	typedef struct SPlayerStatus
	{
		TItemData aInventoryItem[c_Inventory_Slot_Count];
		TItemData aEquipmentItem[c_Equipment_Slot_Count];
#if defined(ENABLE_DRAGON_SOUL_SYSTEM)
		TItemData aDragonSoulInventoryItem[c_DragonSoul_Inventory_Count];
#endif
		TItemData aBeltInventoryItem[c_Belt_Inventory_Slot_Count];
#if defined(ENABLE_ATTR_6TH_7TH)
		TItemData aAttr67AddItem;
#endif

		TQuickSlot aQuickSlot[QUICKSLOT_MAX_NUM];
		TSkillInstance aSkill[SKILL_MAX_NUM];
		POINT_VALUE m_alPoint[POINT_MAX_NUM];
		long lQuickPageIndex;

		void SetPoint(POINT_TYPE wType, POINT_VALUE lValue);
		POINT_VALUE GetPoint(POINT_TYPE wType);
	} TPlayerStatus;

	typedef struct SPartyMemberInfo
	{
#if defined(WJ_SHOW_PARTY_ON_MINIMAP)
		SPartyMemberInfo(DWORD _dwPID, const char* c_szName) : dwPID(_dwPID), strName(c_szName), dwVID(0), lX(0), lY(0), fMinimapX(0.0f), fMinimapY(0.0f) {}
#else
		SPartyMemberInfo(DWORD _dwPID, const char* c_szName) : dwPID(_dwPID), strName(c_szName), dwVID(0) {}
#endif

		DWORD dwVID;
		DWORD dwPID;
		std::string strName;
		BYTE byState;
		BYTE byHPPercentage;
		short sAffects[PARTY_AFFECT_SLOT_MAX_NUM];
#if defined(WJ_SHOW_PARTY_ON_MINIMAP)
		long lX;
		long lY;
		float fMinimapX;
		float fMinimapY;
#endif
	} TPartyMemberInfo;

	enum EPartyRole
	{
		PARTY_ROLE_NORMAL,
		PARTY_ROLE_LEADER,
		PARTY_ROLE_ATTACKER,
		PARTY_ROLE_TANKER,
		PARTY_ROLE_BUFFER,
		PARTY_ROLE_SKILL_MASTER,
		PARTY_ROLE_BERSERKER,
		PARTY_ROLE_DEFENDER,
		PARTY_ROLE_MAX_NUM,
	};

	enum
	{
		SKILL_NORMAL,
		SKILL_MASTER,
		SKILL_GRAND_MASTER,
		SKILL_PERFECT_MASTER,
	};

	// 자동물약 상태 관련 특화 구조체.. 이런식의 특화 처리 작업을 안 하려고 최대한 노력했지만 실패하고 결국 특화처리.
	struct SAutoPotionInfo
	{
		TItemPos Cell; // 사용중인 아이템의 인벤토리상 슬롯 인덱스
	};

	enum EAutoPotionType
	{
		AUTO_POTION_TYPE_HP = 0,
		AUTO_POTION_TYPE_SP = 1,
		AUTO_POTION_TYPE_NUM
	};

public:
	CPythonPlayer(void);
	virtual ~CPythonPlayer(void);

	void PickCloseMoney();
	void PickCloseItem();

	void SetGameWindow(PyObject* ppyObject);

	void SetObserverMode(bool isEnable);
	bool IsObserverMode();

	void SetQuickCameraMode(bool isEnable);

	void SetAttackKeyState(bool isPress);

	void NEW_GetMainActorPosition(TPixelPosition* pkPPosActor);

	bool RegisterEffect(DWORD dwEID, const char* c_szEftFileName, bool isCache);

	bool NEW_SetMouseState(int eMBType, int eMBState);
	bool NEW_SetMouseFunc(int eMBType, int eMBFunc);
	int NEW_GetMouseFunc(int eMBT);
	void NEW_SetMouseMiddleButtonState(int eMBState);

	void NEW_SetAutoCameraRotationSpeed(float fRotSpd);
	void NEW_ResetCameraRotation();

	void NEW_SetSingleDirKeyState(int eDirKey, bool isPress);
	void NEW_SetSingleDIKKeyState(int eDIKKey, bool isPress);
	void NEW_SetMultiDirKeyState(bool isLeft, bool isRight, bool isUp, bool isDown);

	void NEW_Attack();
	void NEW_Fishing();
	bool NEW_CancelFishing();

	void NEW_LookAtFocusActor();
	bool NEW_IsAttackableDistanceFocusActor();

	bool NEW_MoveToDestPixelPositionDirection(const TPixelPosition& c_rkPPosDst);
	bool NEW_MoveToMousePickedDirection();
	bool NEW_MoveToMouseScreenDirection();
	bool NEW_MoveToDirection(float fDirRot);
	void NEW_Stop();

	// Reserved
	bool NEW_IsEmptyReservedDelayTime(float fElapsedtime); // 네이밍 교정 논의 필요 - [levites]

	// Dungeon
	void SetDungeonDestinationPosition(int ix, int iy);
	void AlarmHaveToGo();

	CInstanceBase* NEW_FindActorPtr(DWORD dwVID);
	CInstanceBase* NEW_GetMainActorPtr();

	// flying target set
	void Clear();
	void ClearSkillDict(); // 없어지거나 ClearGame 쪽으로 포함될 함수
	void NEW_ClearSkillData(bool bAll = false);

	void Update();

	// Play Time
	DWORD GetPlayTime();
	void SetPlayTime(DWORD dwPlayTime);

	// System
	void SetMainCharacterIndex(int iIndex);

	DWORD GetMainCharacterIndex();
	bool IsMainCharacterIndex(DWORD dwIndex);
	DWORD GetGuildID();
	void NotifyDeletingCharacterInstance(DWORD dwVID);
	void NotifyCharacterDead(DWORD dwVID);
	void NotifyCharacterUpdate(DWORD dwVID);
	void NotifyDeadMainCharacter();
	void NotifyChangePKMode();

#if defined(ENABLE_SKILL_COOLTIME_UPDATE)
	void ResetSkillCoolTimes();
#endif

	// Player Status
	const char* GetName();
	void SetName(const char* name);

	void SetRace(DWORD dwRace);
	DWORD GetRace();

	void SetWeaponPower(DWORD dwMinPower, DWORD dwMaxPower, DWORD dwMinMagicPower, DWORD dwMaxMagicPower, DWORD dwAddPower);
#if defined(ENABLE_ACCE_COSTUME_SYSTEM)
	void SetAccePower(DWORD dwMinPower, DWORD dwMaxPower, DWORD dwMinMagicPower, DWORD dwMaxMagicPower);
#endif

	void SetStatus(POINT_TYPE wType, POINT_VALUE lValue);
	POINT_VALUE GetStatus(POINT_TYPE wType);

	// Item
	void MoveItemData(TItemPos SrcCell, TItemPos DstCell);
	void SetItemData(TItemPos Cell, const TItemData& c_rkItemInst);
	const TItemData* GetItemData(TItemPos Cell) const;
	void SetItemCount(TItemPos Cell, DWORD dwCount);

#if defined(ENABLE_SOULBIND_SYSTEM)
	void SealItem(TItemPos Cell, long lSealDate);
	bool IsSealedItemBySlot(TItemPos Cell);
#endif

	void SetItemMetinSocket(TItemPos Cell, DWORD dwMetinSocketIndex, DWORD dwMetinNumber);
#if defined(ENABLE_APPLY_RANDOM)
	void SetItemApplyRandom(TItemPos Cell, BYTE bIndex, POINT_TYPE wType, POINT_VALUE lValue);
#endif

	void SetItemAttribute(TItemPos Cell, BYTE bIndex, POINT_TYPE wType, POINT_VALUE lValue);
	DWORD GetItemIndex(TItemPos Cell);
	DWORD GetItemFlags(TItemPos Cell);
	DWORD GetItemAntiFlags(TItemPos Cell);
	BYTE GetItemTypeBySlot(TItemPos Cell);
	BYTE GetItemSubTypeBySlot(TItemPos Cell);
	DWORD GetItemCount(TItemPos Cell);
	DWORD GetItemCountByVnum(DWORD dwVnum
#if defined(ENABLE_CUBE_RENEWAL) && defined(ENABLE_SET_ITEM)
		, bool bIgnoreSetValue
#endif
	);
	DWORD GetItemMetinSocket(TItemPos Cell, DWORD dwMetinSocketIndex);
#if defined(ENABLE_APPLY_RANDOM)
	void GetItemApplyRandom(TItemPos Cell, BYTE bIndex, POINT_TYPE* pwType, POINT_VALUE* plValue);
#endif
	void GetItemAttribute(TItemPos Cell, BYTE bIndex, POINT_TYPE* pwType, POINT_VALUE* plValue);
	void SendClickItemPacket(DWORD dwIID);

	void RequestAddLocalQuickSlot(BYTE bLocalSlotIndex, BYTE bWndType, WORD wWndItemPos);
	void RequestAddToEmptyLocalQuickSlot(BYTE bWndType, WORD wWndItemPos);
	void RequestMoveGlobalQuickSlotToLocalQuickSlot(BYTE bGlobalSrcSlotIndex, BYTE bLocalDstSlotIndex);
	void RequestDeleteGlobalQuickSlot(BYTE bGlobalSlotIndex);
	void UseLocalQuickSlot(BYTE bLocalSlotIndex);
	void RequestUseLocalQuickSlot(BYTE bLocalSlotIndex);
	BYTE LocalQuickSlotIndexToGlobalQuickSlotIndex(BYTE bLocalSlotIndex);

	void GetGlobalQuickSlotData(BYTE bGlobalSlotIndex, BYTE* pbWndType, WORD* pwWndItemPos);
	void GetLocalQuickSlotData(BYTE bSlotPos, BYTE* pbWndType, WORD* pwWndItemPos);
	void RemoveQuickSlotByValue(BYTE bType, WORD wPosition);

	char IsItem(TItemPos SlotIndex);

#if defined(ENABLE_NEW_EQUIPMENT_SYSTEM)
	bool IsBeltInventorySlot(TItemPos Cell);
#endif
	bool IsInventorySlot(TItemPos SlotIndex);
	bool IsEquipmentSlot(TItemPos SlotIndex);

	bool IsEquipItemInSlot(TItemPos iSlotIndex);

	// Quickslot
	int GetQuickPage();
	void SetQuickPage(BYTE nPageIndex);
	void AddQuickSlot(BYTE QuickslotIndex, BYTE IconType, WORD IconPosition);
	void DeleteQuickSlot(BYTE QuickslotIndex);
	void MoveQuickSlot(BYTE Source, BYTE Target);

	// Skill
	void SetSkill(DWORD dwSlotIndex, DWORD dwSkillIndex);
	bool GetSkillSlotIndex(DWORD dwSkillIndex, DWORD* pdwSlotIndex);
	int GetSkillIndex(DWORD dwSlotIndex);
	int GetSkillGrade(DWORD dwSlotIndex);
	int GetSkillLevel(DWORD dwSlotIndex);
	float GetSkillCurrentEfficientPercentage(DWORD dwSlotIndex);
	float GetSkillNextEfficientPercentage(DWORD dwSlotIndex);
	void SetSkillLevel(DWORD dwSlotIndex, DWORD dwSkillLevel);
	void SetSkillLevel_(DWORD dwSkillIndex, DWORD dwSkillGrade, DWORD dwSkillLevel);
	BOOL IsToggleSkill(DWORD dwSlotIndex);
	void ClickSkillSlot(DWORD dwSlotIndex);
	void ChangeCurrentSkillNumberOnly(DWORD dwSlotIndex);
	bool FindSkillSlotIndexBySkillIndex(DWORD dwSkillIndex, DWORD* pdwSkillSlotIndex);

	void SetSkillCoolTime(DWORD dwSkillIndex);
	void EndSkillCoolTime(DWORD dwSkillIndex);

	float GetSkillCoolTime(DWORD dwSlotIndex);
	float GetSkillElapsedCoolTime(DWORD dwSlotIndex);
	BOOL IsSkillActive(DWORD dwSlotIndex);
	BOOL IsSkillCoolTime(DWORD dwSlotIndex);
	void UseGuildSkill(DWORD dwSkillSlotIndex);
	bool AffectIndexToSkillSlotIndex(UINT uAffect, DWORD* pdwSkillSlotIndex);
	bool AffectIndexToSkillIndex(DWORD dwAffectIndex, DWORD* pdwSkillIndex);

	void SetAffect(UINT uAffect);
	void ResetAffect(UINT uAffect);
	void ClearAffects();

	// Target
	void SetTarget(DWORD dwVID, BOOL bForceChange = TRUE);
	void OpenCharacterMenu(DWORD dwVictimActorID);
	DWORD GetTargetVID();
#if defined(ENABLE_TAB_NEXT_TARGET)
	void SelectNearTarget();
#endif

	// Party
	void ExitParty();
	void AppendPartyMember(DWORD dwPID, const char* c_szName);
	void LinkPartyMember(DWORD dwPID, DWORD dwVID);
	void UnlinkPartyMember(DWORD dwPID);
	void UpdatePartyMemberInfo(DWORD dwPID, BYTE byState, BYTE byHPPercentage);
	void UpdatePartyMemberAffect(DWORD dwPID, BYTE byAffectSlotIndex, short sAffectNumber);
	void RemovePartyMember(DWORD dwPID);
	bool IsPartyMemberByVID(DWORD dwVID);
	bool IsPartyMemberByName(const char* c_szName);
	bool GetPartyMemberPtr(DWORD dwPID, TPartyMemberInfo** ppPartyMemberInfo);
	bool PartyMemberPIDToVID(DWORD dwPID, DWORD* pdwVID);
	bool PartyMemberVIDToPID(DWORD dwVID, DWORD* pdwPID);
	bool IsSamePartyMember(DWORD dwVID1, DWORD dwVID2);

#if defined(WJ_SHOW_PARTY_ON_MINIMAP)
	void UpdatePartyMemberPosition(DWORD dwPID, long lX, long lY);
	void UpdatePartyMemberMinimapPosition(DWORD dwPID, float fX, float fY);

	std::size_t GetPartyMemberCount();
	DWORD GetPartyMemberPIDByIndex(std::size_t nIndex);

	bool GetPartyMemberPosition(DWORD dwPID, D3DXVECTOR2* v2Position);
	bool GetPartyMemberMinimapPosition(DWORD dwPID, D3DXVECTOR2* v2Position);
	bool IsPartyPositionExist(DWORD dwPID);

	std::string	GetPartyMemberName(DWORD dwPID);
#endif

	// Fight
	void RememberChallengeInstance(DWORD dwVID);
	void RememberRevengeInstance(DWORD dwVID);
	void RememberCantFightInstance(DWORD dwVID);
	void ForgetInstance(DWORD dwVID);
	bool IsChallengeInstance(DWORD dwVID);
	bool IsRevengeInstance(DWORD dwVID);
	bool IsCantFightInstance(DWORD dwVID);

	// Private Shop
	void OpenPrivateShop();
	void ClosePrivateShop();
	bool IsOpenPrivateShop();

	bool IsDead();
	bool IsPoly();

	// Safebox
	void SetOpenSafeBox(bool bOpen) noexcept { m_isOpenSafeBox = bOpen; };
	bool IsOpenSafeBox() const noexcept { return m_isOpenSafeBox; };

	// Mall
	void SetOpenMall(bool bOpen) noexcept { m_isOpenMall = bOpen; };
	bool IsOpenMall() const noexcept { return m_isOpenMall; };

#if defined(ENABLE_GEM_SYSTEM)
	// Gem (Gaya) Market
public:
	void SetGemShopWindowOpen(bool bOpen) { m_bIsGemShopWindowOpen = bOpen; }
	bool IsGemShopWindowOpen() const { return m_bIsGemShopWindowOpen; }

	void GemShop(TGemShopTable& rTable);
	void ClearGemShop();

	const TGemShopTable& GetGemShopTable() { return m_rGemShopTable; }
	const TGemShopItem& GetGemShopItem(BYTE bSlotIndex);

	long GetGemShopRefreshTime() const;
	BYTE GetGemShopOpenSlotCount() const;

protected:
	bool m_bIsGemShopWindowOpen;
	TGemShopTable m_rGemShopTable;
#endif

#if defined(ENABLE_ACCE_COSTUME_SYSTEM)
	// Acce Costume System
public:
	void SetAcceRefineWindowType(BYTE bType);
	BYTE GetAcceRefineWindowType() const { return m_bAcceRefineWindowType; };

	bool GetAcceRefineWindowOpen() const { return m_bAcceRefineWindowOpen; }
	bool IsAcceWindowEmpty();

	void SetAcceItemData(BYTE bSlotIndex, const TItemData& rItemInstance);
	void DelAcceItemData(BYTE bSlotIndex);

	BYTE FindMoveAcceItemSlot();
	BYTE GetAcceCurrentItemCount();

	BOOL GetAcceItemDataPtr(BYTE bSlotIndex, TItemData** ppInstance);

	void SetActivatedAcceSlot(BYTE bSlotIndex, TItemPos ItemPos);
	BYTE FindActivatedAcceSlot(TItemPos ItemCell);
	TItemPos FindUsingAcceSlot(BYTE bSlotIndex);

private:
	std::vector<TItemData> m_AcceItemInstanceVector;

protected:
	bool m_bAcceRefineWindowOpen;
	BYTE m_bAcceRefineWindowType;
	TItemPos m_AcceRefineActivatedCell[ACCE_SLOT_MAX];

	void __ClearAcceRefineWindow();
#endif

#if defined(ENABLE_AURA_COSTUME_SYSTEM)
	// Aura Costume System
public:
	void SetAuraWindowType(BYTE bType);
	BYTE GetAuraWindowType() const { return m_bAuraRefineWindowType; };

	bool IsAuraWindowOpen() const { return m_bAuraRefineWindowOpen; }
	bool IsAuraSlotEmpty();

	void SetAuraRefineInfo(BYTE bAuraRefineInfoSlot, BYTE bAuraRefineInfoLevel, BYTE bAuraRefineInfoExpPercent);
	BYTE GetAuraRefineInfoLevel(BYTE bAuraRefineInfoSlot);
	BYTE GetAuraRefineInfoExpPct(BYTE bAuraRefineInfoSlot);

	void SetAuraItemData(BYTE bSlotIndex, const TItemData& rItemInstance);
	void DelAuraItemData(BYTE bSlotIndex);

	BYTE FindMoveAuraItemSlot();
	BYTE GetAuraCurrentItemSlotCount();

	BOOL GetAuraItemDataPtr(BYTE bSlotIndex, TItemData** ppInstance);

	void SetActivatedAuraSlot(BYTE bSlotIndex, TItemPos ItemPos);
	BYTE FindActivatedAuraSlot(TItemPos ItemCell);
	TItemPos FindUsingAuraSlot(BYTE bSlotIndex);

private:
	typedef struct SAuraRefineInfo
	{
		BYTE bAuraRefineInfoLevel;
		BYTE bAuraRefineInfoExpPercent;
	} TAuraRefineInfo;

	std::vector<TItemData> m_AuraItemInstanceVector;

protected:
	bool m_bAuraRefineWindowOpen;
	BYTE m_bAuraRefineWindowType;
	TItemPos m_AuraRefineActivatedCell[AURA_SLOT_MAX];
	TAuraRefineInfo m_bAuraRefineInfo[AURA_REFINE_INFO_SLOT_MAX];

	void __ClearAuraRefineWindow();
#endif

#if defined(ENABLE_CHANGE_LOOK_SYSTEM)
	// Change Look System
public:
	void ClearChangeLook(const bool bClearEffect = false);

	void SetItemTransmutationVnum(TItemPos& Cell, const DWORD blVnum);
	DWORD GetItemChangeLookVnum(TItemPos& Cell) const;

	void SetChangeLookWindowOpen(const bool c_bOpen);
	bool GetChangeLookWindowOpen() const;

	void SetChangeLookWindowType(const BYTE c_bType);
	BYTE GetChangeLookWindowType() const;

	void SetChangeLookItemData(const TPacketGCChangeLookSet& rData);
	TPacketGCChangeLookSet* GetChangeLookItemData(const BYTE c_bSlotIndex);
	void DelChangeLookItemData(const BYTE c_bSlotIndex, const bool c_bClearEffect = false);

	void SetChangeLookFreeItemData(const TPacketGCChangeLookSet& rData);
	TPacketGCChangeLookSet* GetChangeLookFreeItemData();
	void DelChangeLookFreeItemData(const bool c_bClearEffect = false);

protected:
	bool m_bIsChangeLookWindowOpen;
	BYTE m_bChangeLookWindowType;
	TPacketGCChangeLookSet m_ChangeLookSlot[static_cast<BYTE>(EChangeLookSlots::CHANGE_LOOK_SLOT_MAX)];
	TPacketGCChangeLookSet m_ChangeLookFreeYangItemSlot;
#endif

#if defined(ENABLE_SET_ITEM)
public:
	void SetItemSetValue(TItemPos& Cell, const BYTE bSetValue);
	BYTE GetItemSetValue(TItemPos& Cell) const;
#endif

#if defined(ENABLE_REFINE_ELEMENT_SYSTEM)
public:
	void SetItemRefineElement(TItemPos& rItemPos, const TPlayerItemRefineElement& c_rkTable);
	const TPlayerItemRefineElement* GetItemRefineElement(TItemPos& rItemPos) const;
#endif

#if defined(ENABLE_MINI_GAME_RUMI)
	// Rumi Game
public:
	void SetRumiGame(bool bEnable) { m_bRumiGame = bEnable; }
	bool GetRumiGame() const { return m_bRumiGame; }

	void SetMiniGameOkeyNormal(bool bEnable) { m_bMiniGameOkeyNormal = bEnable; }
	bool GetMiniGameOkeyNormal() const { return m_bMiniGameOkeyNormal; }

protected:
	bool m_bRumiGame;
	bool m_bMiniGameOkeyNormal;
#endif

#if defined(ENABLE_MINI_GAME_CATCH_KING)
	// Catch King Game
public:
	void SetCatchKingGame(const bool c_bEnable) { m_bCatchKingGame = c_bEnable; }
	bool GetCatchKingGame() const { return m_bCatchKingGame; }
protected:
	bool m_bCatchKingGame;
#endif

#if defined(ENABLE_KEYCHANGE_SYSTEM)
	// Keyboard Controls
public:
	void OpenKeyChangeWindow();
	void IsOpenKeySettingWindow(BOOL bSet) { m_isOpenKeySettingWindow = bSet; };

	void KeySetting(int iKey, int iKeyFunction) { m_keySettingMap[iKey] = iKeyFunction; }
	void KeySettingClear() { m_keySettingMap.clear(); }

	void OnKeyDown(int iKey);
	void OnKeyUp(int iKey);

	using KeySettingMap = std::map<int, int>;

private:
	int m_iKeyBuffer;
	BOOL m_isOpenKeySettingWindow;
	KeySettingMap m_keySettingMap;
	bool m_bAutoRun;
#endif

public:
	// Stamina
	void StartStaminaConsume(DWORD dwConsumePerSec, DWORD dwCurrentStamina);
	void StopStaminaConsume(DWORD dwCurrentStamina);

	// PK Mode
	DWORD GetPKMode();

	// Combo
	void SetComboSkillFlag(BOOL bFlag);

	// System
	void SetMovableGroundDistance(float fDistance);

	// Emotion
	void ActEmotion(DWORD dwEmotionID);
	void StartEmotionProcess();
	void EndEmotionProcess();

#if defined(ENABLE_DRAGON_SOUL_SYSTEM)
	// Dragon Soul
	bool IsDSPageFull(BYTE page);
#endif

#if defined(ENABLE_MOVE_COSTUME_ATTR)
	// Item Combination
	void SetItemCombinationWindowActivedItemSlot(const BYTE bSlotType, const int iSlotIndex);
	short GetInvenSlotAttachedToConbWindowSlot(const BYTE bSlotType) const;
	CItemData* GetConbMediumItemData() const;
#endif

#if defined(ENABLE_CHANGED_ATTR)
	// Select Attribute
	void GetItemChangedAttribute(DWORD dwAttrSlotIndex, POINT_TYPE* pwType, POINT_VALUE* plValue);
	void SetSelectAttr(const TPlayerItemAttribute* attr);
#endif

	// Function Only For Console System
	BOOL __ToggleCoolTime();
	BOOL __ToggleLevelLimit();

#if defined(ENABLE_GRAPHIC_ON_OFF)
	bool GetPickedActorID(DWORD* pdwActorID) { return __GetPickedActorID(pdwActorID); }
#endif

	__inline const SAutoPotionInfo& GetAutoPotionInfo(BYTE bType) const { return m_kAutoPotionInfo[bType]; }
	__inline SAutoPotionInfo& GetAutoPotionInfo(BYTE bType) { return m_kAutoPotionInfo[bType]; }
	__inline void SetAutoPotionInfo(BYTE bType, const SAutoPotionInfo& rkTable) { m_kAutoPotionInfo[bType] = rkTable; }

protected:
	TQuickSlot& __RefLocalQuickSlot(BYTE SlotIndex);
	TQuickSlot& __RefGlobalQuickSlot(BYTE SlotIndex);

	DWORD __GetLevelAtk();
	DWORD __GetStatAtk();
	DWORD __GetWeaponAtk(DWORD dwWeaponPower);
	DWORD __GetTotalAtk(DWORD dwWeaponPower, DWORD dwRefineBonus);
	DWORD __GetRaceStat();
	DWORD __GetHitRate();
	DWORD __GetEvadeRate();

	void __UpdateBattleStatus();

	void __DeactivateSkillSlot(DWORD dwSlotIndex);
	void __ActivateSkillSlot(DWORD dwSlotIndex);

	void __OnPressSmart(CInstanceBase& rkInstMain, bool isAuto);
	void __OnClickSmart(CInstanceBase& rkInstMain, bool isAuto);

	void __OnPressItem(CInstanceBase& rkInstMain, DWORD dwPickedItemID);
	void __OnPressActor(CInstanceBase& rkInstMain, DWORD dwPickedActorID, bool isAuto);
	void __OnPressGround(CInstanceBase& rkInstMain, const TPixelPosition& c_rkPPosPickedGround);
	void __OnPressScreen(CInstanceBase& rkInstMain);

	void __OnClickActor(CInstanceBase& rkInstMain, DWORD dwPickedActorID, bool isAuto);
	void __OnClickItem(CInstanceBase& rkInstMain, DWORD dwPickedItemID);
	void __OnClickGround(CInstanceBase& rkInstMain, const TPixelPosition& c_rkPPosPickedGround);

	bool __IsMovableGroundDistance(CInstanceBase& rkInstMain, const TPixelPosition& c_rkPPosPickedGround);

	bool __GetPickedActorPtr(CInstanceBase** pkInstPicked);

	bool __GetPickedActorID(DWORD* pdwActorID);
	bool __GetPickedItemID(DWORD* pdwItemID);
	bool __GetPickedGroundPos(TPixelPosition* pkPPosPicked);

	void __ClearReservedAction();
	void __ReserveClickItem(DWORD dwItemID);
	void __ReserveClickActor(DWORD dwActorID);
	void __ReserveClickGround(const TPixelPosition& c_rkPPosPickedGround);
	void __ReserveUseSkill(DWORD dwActorID, DWORD dwSkillSlotIndex, DWORD dwRange);

	void __ReserveProcess_ClickActor();

	void __ShowPickedEffect(const TPixelPosition& c_rkPPosPickedGround);
	void __SendClickActorPacket(CInstanceBase& rkInstVictim);

	void __ClearAutoAttackTargetActorID();
	void __SetAutoAttackTargetActorID(DWORD dwActorID);

	void NEW_ShowEffect(int dwEID, TPixelPosition kPPosDst);

	void NEW_SetMouseSmartState(int eMBS, bool isAuto);
	void NEW_SetMouseMoveState(int eMBS);
	void NEW_SetMouseCameraState(int eMBS);
	void NEW_GetMouseDirRotation(float fScrX, float fScrY, float* pfDirRot);
	void NEW_GetMultiKeyDirRotation(bool isLeft, bool isRight, bool isUp, bool isDown, float* pfDirRot);

	float GetDegreeFromDirection(int iUD, int iLR);
	float GetDegreeFromPosition(int ix, int iy, int iHalfWidth, int iHalfHeight);

	bool CheckCategory(int iCategory);
	bool CheckAbilitySlot(int iSlotIndex);

	void RefreshKeyWalkingDirection();
	void NEW_RefreshMouseWalkingDirection();

	// Instances
	void RefreshInstances();

	bool __CanShot(CInstanceBase& rkInstMain, CInstanceBase& rkInstTarget);
	bool __CanUseSkill(DWORD dwSkillIndex);

	bool __CanMove();

	bool __CanAttack();
	bool __CanChangeTarget();

	bool __CheckSkillUsable(DWORD dwSlotIndex);
	void __UseCurrentSkill();
	void __UseChargeSkill(DWORD dwSkillSlotIndex);
	bool __UseSkill(DWORD dwSlotIndex);
	bool __CheckSpecialSkill(DWORD dwSkillIndex);

	bool __CheckRestSkillCoolTime(DWORD dwSkillSlotIndex);
	bool __CheckShortLife(TSkillInstance& rkSkillInst, CPythonSkill::TSkillData& rkSkillData);
	bool __CheckShortMana(TSkillInstance& rkSkillInst, CPythonSkill::TSkillData& rkSkillData);
	bool __CheckShortArrow(TSkillInstance& rkSkillInst, CPythonSkill::TSkillData& rkSkillData);
	bool __CheckDashAffect(CInstanceBase& rkInstMain);

	void __SendUseSkill(DWORD dwSkillSlotIndex, DWORD dwTargetVID);
	void __RunCoolTime(DWORD dwSkillSlotIndex);

	BYTE __GetSkillType(DWORD dwSkillSlotIndex);

	bool __IsReservedUseSkill(DWORD dwSkillSlotIndex);
	bool __IsMeleeSkill(CPythonSkill::TSkillData& rkSkillData);
	bool __IsChargeSkill(CPythonSkill::TSkillData& rkSkillData);
	DWORD __GetSkillTargetRange(CPythonSkill::TSkillData& rkSkillData);
	bool __SearchNearTarget();
	bool __IsUsingChargeSkill();
	bool __IsHorseSkill(DWORD dwSkillIndex);

	bool __ProcessEnemySkillTargetRange(CInstanceBase& rkInstMain, CInstanceBase& rkInstTarget, CPythonSkill::TSkillData& rkSkillData, DWORD dwSkillSlotIndex);

	// Item
	bool __HasEnoughArrow();
	bool __HasItem(DWORD dwItemID);
	DWORD __GetPickableDistance();

	// Target
	CInstanceBase* __GetTargetActorPtr();
	void __ClearTarget();
	DWORD __GetTargetVID();
	void __SetTargetVID(DWORD dwVID);
	bool __IsSameTargetVID(DWORD dwVID);
	bool __IsTarget();
	bool __ChangeTargetToPickedInstance();

	CInstanceBase* __GetSkillTargetInstancePtr(CPythonSkill::TSkillData& rkSkillData);
	CInstanceBase* __GetAliveTargetInstancePtr();
	CInstanceBase* __GetDeadTargetInstancePtr();

	BOOL __IsRightButtonSkillMode();

	// Update
	void __Update_AutoAttack();
	void __Update_NotifyGuildAreaEvent();

	// Emotion
	BOOL __IsProcessingEmotion();

protected:
	PyObject* m_ppyGameWindow;

	// Client Player Data
	std::map<DWORD, DWORD> m_skillSlotDict;
	std::string m_stName;
	DWORD m_dwMainCharacterIndex;
	DWORD m_dwRace;

	// Weapon Power
	DWORD m_dwWeaponMinPower;
	DWORD m_dwWeaponMaxPower;
	DWORD m_dwWeaponMinMagicPower;
	DWORD m_dwWeaponMaxMagicPower;
	DWORD m_dwWeaponAddPower;

#if defined(ENABLE_ACCE_COSTUME_SYSTEM)
	// Acce Power
	DWORD m_dwAcceMinPower;
	DWORD m_dwAcceMaxPower;
	DWORD m_dwAcceMinMagicPower;
	DWORD m_dwAcceMaxMagicPower;
#endif

	// Todo
	DWORD m_dwSendingTargetVID;
	float m_fTargetUpdateTime;

	// Attack
	DWORD m_dwAutoAttackTargetVID;

	// NEW_Move
	EMode m_eReservedMode;
	float m_fReservedDelayTime;

	float m_fMovDirRot;

	bool m_isUp;
	bool m_isDown;
	bool m_isLeft;
	bool m_isRight;
	bool m_isAtkKey;
	bool m_isDirKey;
	bool m_isCmrRot;
	bool m_isSmtMov;
	bool m_isDirMov;

	float m_fCmrRotSpd;

	TPlayerStatus m_playerStatus;

	UINT m_iComboOld;
	DWORD m_dwVIDReserved;
	DWORD m_dwIIDReserved;

	DWORD m_dwcurSkillSlotIndex;
	DWORD m_dwSkillSlotIndexReserved;
	DWORD m_dwSkillRangeReserved;

	TPixelPosition m_kPPosInstPrev;
	TPixelPosition m_kPPosReserved;

	// Emotion
	BOOL m_bisProcessingEmotion;

	// Dungeon
	BOOL m_isDestPosition;
	int m_ixDestPos;
	int m_iyDestPos;
	int m_iLastAlarmTime;

	// Party
	std::map<DWORD, TPartyMemberInfo> m_PartyMemberMap;

	// PVP
	std::set<DWORD> m_ChallengeInstanceSet;
	std::set<DWORD> m_RevengeInstanceSet;
	std::set<DWORD> m_CantFightInstanceSet;

	// Private Shop
	bool m_isOpenPrivateShop;
	bool m_isObserverMode;

	// SafeBox & Mall
	bool m_isOpenSafeBox;
	bool m_isOpenMall;

	// Stamina
	BOOL m_isConsumingStamina;
	float m_fCurrentStamina;
	float m_fConsumeStaminaPerSec;

	// Guild
	DWORD m_inGuildAreaID;

	// System
	BOOL m_sysIsCoolTime;
	BOOL m_sysIsLevelLimit;

protected:
	// Game Cursor Data
	TPixelPosition m_MovingCursorPosition;
	float m_fMovingCursorSettingTime;
	DWORD m_adwEffect[EFFECT_NUM];

	DWORD m_dwVIDPicked;
	DWORD m_dwIIDPicked;
	int m_aeMBFButton[MBT_NUM];

	DWORD m_dwTargetVID;
	DWORD m_dwTargetEndTime;
	DWORD m_dwPlayTime;

	SAutoPotionInfo m_kAutoPotionInfo[AUTO_POTION_TYPE_NUM];

#if defined(ENABLE_MOVE_COSTUME_ATTR)
	short m_MoveCostumeAttrArr[ECombSlotType::COMB_WND_SLOT_MAX];
#endif

#if defined(ENABLE_CHANGED_ATTR)
	TPlayerItemAttribute m_SelectAttrArr[ITEM_ATTRIBUTE_SLOT_MAX_NUM];
#endif

protected:
	float MOVABLE_GROUND_DISTANCE;

private:
	std::map<DWORD, DWORD> m_kMap_dwAffectIndexToSkillIndex;

public:
	void AddAffect(DWORD dwType, TPacketAffectElement kElem);
	void RemoveAffect(DWORD dwType, DWORD dwApplyOn);

	int GetAffectDataIndex(DWORD dwType, DWORD dwApplyOn);
	long GetAffectDuration(DWORD dwType);

	typedef std::vector<TPacketAffectElement> TAffectDataVector;
	TAffectDataVector GetAffectDataVector(DWORD dwType);

protected:
	TAffectDataVector m_vecAffectData;

#if defined(ENABLE_FISHING_GAME)
public:
	// Fish Game
	void SetFishingGameWindowOpen(bool bFlag) { m_bIsOpenFishingGameWindow = bFlag; };
	bool GetFishingGameWindowOpen() { return m_bIsOpenFishingGameWindow; }
protected:
	bool m_bIsOpenFishingGameWindow;
#endif

#if defined(ENABLE_EXTEND_INVEN_SYSTEM)
public:
	void SetExtendInvenStage(BYTE bInvenStage) { m_bExtendInvenStage = bInvenStage; };
	BYTE GetExtendInvenStage() const { return m_bExtendInvenStage; };

	void SetExtendInvenMax(WORD wInvenMax) { m_wExtendInvenMax = wInvenMax; };
	WORD GetExtendInvenMax() const { return m_wExtendInvenMax; };

protected:
	BYTE m_bExtendInvenStage;
	WORD m_wExtendInvenMax;
#endif

#if defined(ENABLE_EXPRESSING_EMOTION)
public:
	using TSpecialActionInfo = std::pair<BYTE, DWORD>;
	using TSpecialActionMap = std::map<BYTE, TSpecialActionInfo>;

	void ClearSpecialActionSlot(BYTE bSlotIndex);
	void SetSpecialActionSlot(BYTE bSlotIndex, BYTE bEmotionIndex, DWORD dwEmotionDuration);

	INT GetSpecialActionSlot(BYTE bSlotIndex) const;
	void FindSpecialActionSlot(BYTE bEmotionIndex, BYTE* pbSlotIndex, DWORD* pdwEmotionDuration) const;

protected:
	TSpecialActionMap m_map_kSpecialAction;
#endif

#if defined(ENABLE_CUBE_RENEWAL)
public:
	void SetCubeWindowOpen(bool bOpen) { m_bCubeWindowOpen = bOpen; }
	bool IsCubeWindowOpen() const { return m_bCubeWindowOpen; }
private:
	bool m_bCubeWindowOpen;
#endif

#if defined(ENABLE_SNOWFLAKE_STICK_EVENT)
	// Snowflake Stick Event
public:
	void SetSnowflakeStickEvent(DWORD dwEndTime) { m_dwSnowflakeStickEvent = dwEndTime; }
	DWORD GetSnowflakeStickEvent() const { return m_dwSnowflakeStickEvent; }
protected:
	DWORD m_dwSnowflakeStickEvent;
#endif

#if defined(ENABLE_FLOWER_EVENT)
	// Flower Event
public:
	void SetFlowerEventEnable(DWORD dwEnable) { m_dwFlowerEvent = dwEnable; }
	DWORD GetFlowerEventEnable() const { return m_dwFlowerEvent; }
protected:
	int m_dwFlowerEvent;
#endif

#if defined(__IMGUI__)
public:
	void SetAutoAttackTargetActorID(DWORD dwActorID) { __SetAutoAttackTargetActorID(dwActorID); }
#endif

#if defined(ENABLE_MINI_GAME_YUTNORI)
public:
	void YutnoriNotifyMotionDone();
#endif

#if defined(ENABLE_LEFT_SEAT)
public:
	void LoadLeftSeatData();
#endif
};

extern const int c_iFastestSendingCount;
extern const int c_iSlowestSendingCount;
extern const float c_fFastestSendingDelay;
extern const float c_fSlowestSendingDelay;
extern const float c_fRotatingStepTime;

extern const float c_fComboDistance;
extern const float c_fPickupDistance;
extern const float c_fClickDistance;
