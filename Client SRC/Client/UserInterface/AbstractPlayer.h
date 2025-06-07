#pragma once

#include "AbstractSingleton.h"

class CInstanceBase;

class IAbstractPlayer : public TAbstractSingleton<IAbstractPlayer>
{
public:
	IAbstractPlayer() {}
	virtual ~IAbstractPlayer() {}

	virtual DWORD GetMainCharacterIndex() = 0;
	virtual void SetMainCharacterIndex(int iIndex) = 0;
	virtual bool IsMainCharacterIndex(DWORD dwIndex) = 0;

	virtual POINT_VALUE GetStatus(POINT_TYPE wType) = 0;

	virtual const char* GetName() = 0;
	virtual DWORD GetGuildID() = 0;

	virtual void SetRace(DWORD dwRace) = 0;

	virtual void StartStaminaConsume(DWORD dwConsumePerSec, DWORD dwCurrentStamina) = 0;
	virtual void StopStaminaConsume(DWORD dwCurrentStamina) = 0;

	virtual bool IsPartyMemberByVID(DWORD dwVID) = 0;
	virtual bool PartyMemberVIDToPID(DWORD dwVID, DWORD* pdwPID) = 0;
	virtual bool IsSamePartyMember(DWORD dwVID1, DWORD dwVID2) = 0;

	virtual void SetItemData(TItemPos itemPos, const TItemData& c_rkItemInst) = 0;
	virtual void SetItemCount(TItemPos itemPos, DWORD dwCount) = 0;
#if defined(ENABLE_SOULBIND_SYSTEM)
	virtual void SealItem(TItemPos Cell, long lSealDate) = 0;
#endif

	virtual void SetItemMetinSocket(TItemPos itemPos, DWORD dwMetinSocketIndex, DWORD dwMetinNumber) = 0;
#if defined(ENABLE_APPLY_RANDOM)
	virtual void SetItemApplyRandom(TItemPos Cell, BYTE bIndex, POINT_TYPE wType, POINT_VALUE lValue) = 0;
#endif
	virtual void SetItemAttribute(TItemPos Cell, BYTE bIndex, POINT_TYPE wType, POINT_VALUE lValue) = 0;
#if defined(ENABLE_CHANGE_LOOK_SYSTEM)
	virtual void SetItemTransmutationVnum(TItemPos& rItemPos, const DWORD c_dwVnum) = 0;
#endif
#if defined(ENABLE_SET_ITEM)
	virtual void SetItemSetValue(TItemPos& rItemPos, const BYTE c_bSetValue) = 0;
#endif
#if defined(ENABLE_REFINE_ELEMENT_SYSTEM)
	virtual void SetItemRefineElement(TItemPos& rItemPos, const TPlayerItemRefineElement& c_rkTable) = 0;
#endif

	virtual DWORD GetItemIndex(TItemPos itemPos) = 0;
	virtual DWORD GetItemFlags(TItemPos itemPos) = 0;
	virtual DWORD GetItemCount(TItemPos itemPos) = 0;

	virtual bool IsEquipItemInSlot(TItemPos itemPos) = 0;

	virtual void AddQuickSlot(BYTE QuickslotIndex, BYTE IconType, WORD IconPosition) = 0;
	virtual void DeleteQuickSlot(BYTE QuickslotIndex) = 0;
	virtual void MoveQuickSlot(BYTE Source, BYTE Target) = 0;

	virtual void SetWeaponPower(DWORD dwMinPower, DWORD dwMaxPower, DWORD dwMinMagicPower, DWORD dwMaxMagicPower, DWORD dwAddPower) = 0;
#if defined(ENABLE_ACCE_COSTUME_SYSTEM)
	virtual void SetAccePower(DWORD dwMinPower, DWORD dwMaxPower, DWORD dwMinMagicPower, DWORD dwMaxMagicPower) = 0;
#endif

	virtual void SetTarget(DWORD dwVID, BOOL bForceChange = TRUE) = 0;
	virtual void NotifyCharacterUpdate(DWORD dwVID) = 0;
	virtual void NotifyCharacterDead(DWORD dwVID) = 0;
	virtual void NotifyDeletingCharacterInstance(DWORD dwVID) = 0;
	virtual void NotifyChangePKMode() = 0;

	virtual void SetObserverMode(bool isEnable) = 0;
	virtual void SetComboSkillFlag(BOOL bFlag) = 0;

	virtual void StartEmotionProcess() = 0;
	virtual void EndEmotionProcess() = 0;

	virtual CInstanceBase* NEW_GetMainActorPtr() = 0;
};
