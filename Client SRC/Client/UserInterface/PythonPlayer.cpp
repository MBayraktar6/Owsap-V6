#include "StdAfx.h"
#include "PythonPlayerEventHandler.h"
#include "PythonApplication.h"
#include "PythonItem.h"
#include "../EterBase/Timer.h"

#include "AbstractPlayer.h"
#include "../EterPythonLib/PythonSlotWindow.h"

const DWORD POINT_MAGIC_NUMBER = 0xe73ac1da;

void CPythonPlayer::SPlayerStatus::SetPoint(POINT_TYPE wType, POINT_VALUE lValue)
{
	m_alPoint[wType] = lValue ^ POINT_MAGIC_NUMBER;
}

POINT_VALUE CPythonPlayer::SPlayerStatus::GetPoint(POINT_TYPE wType)
{
	return m_alPoint[wType] ^ POINT_MAGIC_NUMBER;
}

bool CPythonPlayer::AffectIndexToSkillIndex(DWORD dwAffectIndex, DWORD* pdwSkillIndex)
{
	if (m_kMap_dwAffectIndexToSkillIndex.end() == m_kMap_dwAffectIndexToSkillIndex.find(dwAffectIndex))
		return false;

	*pdwSkillIndex = m_kMap_dwAffectIndexToSkillIndex[dwAffectIndex];
	return true;
}

bool CPythonPlayer::AffectIndexToSkillSlotIndex(UINT uAffect, DWORD* pdwSkillSlotIndex)
{
	DWORD dwSkillIndex = m_kMap_dwAffectIndexToSkillIndex[uAffect];

	return GetSkillSlotIndex(dwSkillIndex, pdwSkillSlotIndex);
}

bool CPythonPlayer::__GetPickedActorPtr(CInstanceBase** ppkInstPicked)
{
	CPythonCharacterManager& rkChrMgr = CPythonCharacterManager::Instance();
	CInstanceBase* pkInstPicked = rkChrMgr.OLD_GetPickedInstancePtr();
	if (!pkInstPicked)
		return false;

	*ppkInstPicked = pkInstPicked;
	return true;
}

bool CPythonPlayer::__GetPickedActorID(DWORD* pdwActorID)
{
	CPythonCharacterManager& rkChrMgr = CPythonCharacterManager::Instance();
	return rkChrMgr.OLD_GetPickedInstanceVID(pdwActorID);
}

bool CPythonPlayer::__GetPickedItemID(DWORD* pdwItemID)
{
	CPythonItem& rkItemMgr = CPythonItem::Instance();
	return rkItemMgr.GetPickedItemID(pdwItemID);
}

bool CPythonPlayer::__GetPickedGroundPos(TPixelPosition* pkPPosPicked)
{
	CPythonBackground& rkBG = CPythonBackground::Instance();

	TPixelPosition kPPosPicked;
	if (rkBG.GetPickingPoint(pkPPosPicked))
	{
		pkPPosPicked->y = -pkPPosPicked->y;
		return true;
	}

	return false;
}

void CPythonPlayer::NEW_GetMainActorPosition(TPixelPosition* pkPPosActor)
{
	TPixelPosition kPPosMainActor;

	IAbstractPlayer& rkPlayer = IAbstractPlayer::GetSingleton();
	CInstanceBase* pInstance = rkPlayer.NEW_GetMainActorPtr();
	if (pInstance)
	{
		pInstance->NEW_GetPixelPosition(pkPPosActor);
	}
	else
	{
		CPythonApplication::Instance().GetCenterPosition(pkPPosActor);
	}
}

bool CPythonPlayer::RegisterEffect(DWORD dwEID, const char* c_szFileName, bool isCache)
{
	if (dwEID >= EFFECT_NUM)
		return false;

	CEffectManager& rkEftMgr = CEffectManager::Instance();
	rkEftMgr.RegisterEffect2(c_szFileName, &m_adwEffect[dwEID], isCache);
	return true;
}

void CPythonPlayer::NEW_ShowEffect(int dwEID, TPixelPosition kPPosDst)
{
	if (dwEID >= EFFECT_NUM)
		return;

	D3DXVECTOR3 kD3DVt3Pos(kPPosDst.x, -kPPosDst.y, kPPosDst.z);
	D3DXVECTOR3 kD3DVt3Dir(0.0f, 0.0f, 1.0f);

	CEffectManager& rkEftMgr = CEffectManager::Instance();
	rkEftMgr.CreateEffect(m_adwEffect[dwEID], kD3DVt3Pos, kD3DVt3Dir);
}

CInstanceBase* CPythonPlayer::NEW_FindActorPtr(DWORD dwVID)
{
	CPythonCharacterManager& rkChrMgr = CPythonCharacterManager::Instance();
	return rkChrMgr.GetInstancePtr(dwVID);
}

CInstanceBase* CPythonPlayer::NEW_GetMainActorPtr()
{
	return NEW_FindActorPtr(m_dwMainCharacterIndex);
}

///////////////////////////////////////////////////////////////////////////////////////////

void CPythonPlayer::Update()
{
	NEW_RefreshMouseWalkingDirection();

	CPythonPlayerEventHandler& rkPlayerEventHandler = CPythonPlayerEventHandler::GetSingleton();
	rkPlayerEventHandler.FlushVictimList();

	if (m_isDestPosition)
	{
		CInstanceBase* pInstance = NEW_GetMainActorPtr();
		if (pInstance)
		{
			TPixelPosition PixelPosition;
			pInstance->NEW_GetPixelPosition(&PixelPosition);

			if (abs(int(PixelPosition.x) - m_ixDestPos) + abs(int(PixelPosition.y) - m_iyDestPos) < 10000)
			{
				m_isDestPosition = FALSE;
			}
			else
			{
				if (CTimer::Instance().GetCurrentMillisecond() - m_iLastAlarmTime > 20000)
				{
					AlarmHaveToGo();
				}
			}
		}
	}

	if (m_isConsumingStamina)
	{
		float fElapsedTime = CTimer::Instance().GetElapsedSecond();
		m_fCurrentStamina -= (fElapsedTime * m_fConsumeStaminaPerSec);

		SetStatus(POINT_STAMINA, DWORD(m_fCurrentStamina));

		PyCallClassMemberFunc(m_ppyGameWindow, "RefreshStamina", Py_BuildValue("()"));
	}

	__Update_AutoAttack();
	__Update_NotifyGuildAreaEvent();
}

bool CPythonPlayer::__IsUsingChargeSkill()
{
	CInstanceBase* pkInstMain = NEW_GetMainActorPtr();
	if (!pkInstMain)
		return false;

	if (__CheckDashAffect(*pkInstMain))
		return true;

	if (MODE_USE_SKILL != m_eReservedMode)
		return false;

	if (m_dwSkillSlotIndexReserved >= SKILL_MAX_NUM)
		return false;

	TSkillInstance& rkSkillInst = m_playerStatus.aSkill[m_dwSkillSlotIndexReserved];

	CPythonSkill::TSkillData* pSkillData;
	if (!CPythonSkill::Instance().GetSkillData(rkSkillInst.dwIndex, &pSkillData))
		return false;

	return pSkillData->IsChargeSkill() ? true : false;
}

bool CPythonPlayer::__IsHorseSkill(DWORD dwSkillIndex)
{
	return dwSkillIndex >= 137 && dwSkillIndex <= 140;
}

void CPythonPlayer::__Update_AutoAttack()
{
	if (0 == m_dwAutoAttackTargetVID)
		return;

	CInstanceBase* pkInstMain = NEW_GetMainActorPtr();
	if (!pkInstMain)
		return;

	// 탄환격 쓰고 달려가는 도중에는 스킵
	if (__IsUsingChargeSkill())
		return;

	CInstanceBase* pkInstVictim = NEW_FindActorPtr(m_dwAutoAttackTargetVID);
	if (!pkInstVictim)
	{
		__ClearAutoAttackTargetActorID();
	}
	else
	{
		if (pkInstVictim->IsDead())
		{
			__ClearAutoAttackTargetActorID();
		}
		else if (pkInstMain->IsMountingHorse() && !pkInstMain->CanAttackHorseLevel())
		{
			__ClearAutoAttackTargetActorID();
		}
		else if (pkInstMain->IsAttackableInstance(*pkInstVictim))
		{
			// 20190220.Owsap : Fix auto attack with no arrows (Movement bug floating)
			if (pkInstMain->IsBowMode())
			{
				if (!__CanShot(*pkInstMain, *pkInstVictim))
					return;
			}

			if (pkInstMain->IsSleep())
			{
				//TraceError("SKIP_AUTO_ATTACK_IN_SLEEPING");
			}
			else
			{
				__ReserveClickActor(m_dwAutoAttackTargetVID);
			}
		}
	}
}

void CPythonPlayer::__Update_NotifyGuildAreaEvent()
{
	CInstanceBase* pkInstMain = NEW_GetMainActorPtr();
	if (pkInstMain)
	{
		TPixelPosition kPixelPosition;
		pkInstMain->NEW_GetPixelPosition(&kPixelPosition);

		DWORD dwAreaID = CPythonMiniMap::Instance().GetGuildAreaID(
			ULONG(kPixelPosition.x), ULONG(kPixelPosition.y));

		if (dwAreaID != m_inGuildAreaID)
		{
			if (0xffffffff != dwAreaID)
			{
				PyCallClassMemberFunc(m_ppyGameWindow, "BINARY_Guild_EnterGuildArea", Py_BuildValue("(i)", dwAreaID));
			}
			else
			{
				PyCallClassMemberFunc(m_ppyGameWindow, "BINARY_Guild_ExitGuildArea", Py_BuildValue("(i)", dwAreaID));
			}

			m_inGuildAreaID = dwAreaID;
		}
	}
}

void CPythonPlayer::SetMainCharacterIndex(int iIndex)
{
	m_dwMainCharacterIndex = iIndex;

	CInstanceBase* pkInstMain = NEW_GetMainActorPtr();
	if (pkInstMain)
	{
		CPythonPlayerEventHandler& rkPlayerEventHandler = CPythonPlayerEventHandler::GetSingleton();
		pkInstMain->SetEventHandler(&rkPlayerEventHandler);
	}
}

DWORD CPythonPlayer::GetMainCharacterIndex()
{
	return m_dwMainCharacterIndex;
}

bool CPythonPlayer::IsMainCharacterIndex(DWORD dwIndex)
{
	return (m_dwMainCharacterIndex == dwIndex);
}

DWORD CPythonPlayer::GetGuildID()
{
	CInstanceBase* pkInstMain = NEW_GetMainActorPtr();
	if (!pkInstMain)
		return 0xffffffff;

	return pkInstMain->GetGuildID();
}

void CPythonPlayer::SetWeaponPower(DWORD dwMinPower, DWORD dwMaxPower, DWORD dwMinMagicPower, DWORD dwMaxMagicPower, DWORD dwAddPower)
{
	m_dwWeaponMinPower = dwMinPower;
	m_dwWeaponMaxPower = dwMaxPower;
	m_dwWeaponMinMagicPower = dwMinMagicPower;
	m_dwWeaponMaxMagicPower = dwMaxMagicPower;
	m_dwWeaponAddPower = dwAddPower;

	__UpdateBattleStatus();
}

#if defined(ENABLE_ACCE_COSTUME_SYSTEM)
void CPythonPlayer::SetAccePower(DWORD dwMinPower, DWORD dwMaxPower, DWORD dwMinMagicPower, DWORD dwMaxMagicPower)
{
	m_dwAcceMinPower = dwMinPower;
	m_dwAcceMaxPower = dwMaxPower;
	m_dwAcceMinMagicPower = dwMinMagicPower;
	m_dwAcceMaxMagicPower = dwMaxMagicPower;

	__UpdateBattleStatus();
}
#endif

void CPythonPlayer::SetRace(DWORD dwRace)
{
	m_dwRace = dwRace;
}

DWORD CPythonPlayer::GetRace()
{
	return m_dwRace;
}

DWORD CPythonPlayer::__GetRaceStat()
{
	switch (GetRace())
	{
		case MAIN_RACE_WARRIOR_M:
		case MAIN_RACE_WARRIOR_W:
			return GetStatus(POINT_ST);

		case MAIN_RACE_ASSASSIN_M:
		case MAIN_RACE_ASSASSIN_W:
			return GetStatus(POINT_DX);

		case MAIN_RACE_SURA_M:
		case MAIN_RACE_SURA_W:
			return GetStatus(POINT_ST);

		case MAIN_RACE_SHAMAN_M:
		case MAIN_RACE_SHAMAN_W:
			return GetStatus(POINT_IQ);

		case MAIN_RACE_WOLFMAN_M:
			return GetStatus(POINT_DX);
	}

	return GetStatus(POINT_ST);
}

DWORD CPythonPlayer::__GetLevelAtk()
{
	return 2 * GetStatus(POINT_LEVEL);
}

DWORD CPythonPlayer::__GetStatAtk()
{
	switch (GetRace())
	{
		case MAIN_RACE_WARRIOR_M:
		case MAIN_RACE_WARRIOR_W:
			return (4 * __GetRaceStat() /*POINT_ST*/ + 2 * GetStatus(POINT_DX)) / 3;

		case MAIN_RACE_ASSASSIN_M:
		case MAIN_RACE_ASSASSIN_W:
			return (5 * __GetRaceStat() /*POINT_DX*/ + 1 * GetStatus(POINT_ST)) / 3;

		case MAIN_RACE_SURA_M:
		case MAIN_RACE_SURA_W:
			return (4 * __GetRaceStat() /*POINT_ST*/ + 2 * GetStatus(POINT_DX)) / 3;

		case MAIN_RACE_SHAMAN_M:
		case MAIN_RACE_SHAMAN_W:
			return (5 * __GetRaceStat() /*POINT_IQ*/ + 1 * GetStatus(POINT_DX)) / 3;

		case MAIN_RACE_WOLFMAN_M:
			return (7 * __GetRaceStat() /*POINT_DX*/ + 2 * GetStatus(POINT_HT)) / 3;
	}

	return (2 * GetStatus(POINT_ST));
}

DWORD CPythonPlayer::__GetWeaponAtk(DWORD dwWeaponPower)
{
	return 2 * dwWeaponPower;
}

DWORD CPythonPlayer::__GetTotalAtk(DWORD dwWeaponPower, DWORD dwRefineBonus)
{
	DWORD dwLvAtk = __GetLevelAtk();
	DWORD dwStAtk = __GetStatAtk();

	/////

	DWORD dwWepAtk;
	DWORD dwTotalAtk;

	if (LocaleService_IsCHEONMA())
	{
		dwWepAtk = __GetWeaponAtk(dwWeaponPower + dwRefineBonus);
		dwTotalAtk = dwLvAtk + (dwStAtk + dwWepAtk) * (GetStatus(POINT_DX) + 210) / 300;
	}
	else
	{
		int hr = __GetHitRate();
		dwWepAtk = __GetWeaponAtk(dwWeaponPower + dwRefineBonus);
		dwTotalAtk = dwLvAtk + (dwStAtk + dwWepAtk) * hr / 100;
	}

	return dwTotalAtk;
}

DWORD CPythonPlayer::__GetHitRate()
{
	int src = 0;

	if (LocaleService_IsCHEONMA())
	{
		src = GetStatus(POINT_DX);
	}
	else
	{
		src = (GetStatus(POINT_DX) * 4 + GetStatus(POINT_LEVEL) * 2) / 6;
	}

	return 100 * (min(90, src) + 210) / 300;
}

DWORD CPythonPlayer::__GetEvadeRate()
{
	return 30 * (2 * GetStatus(POINT_DX) + 5) / (GetStatus(POINT_DX) + 95);
}

void CPythonPlayer::__UpdateBattleStatus()
{
	m_playerStatus.SetPoint(POINT_NONE, 0);
	m_playerStatus.SetPoint(POINT_EVADE_RATE, __GetEvadeRate());
	m_playerStatus.SetPoint(POINT_HIT_RATE, __GetHitRate());
	m_playerStatus.SetPoint(POINT_MIN_WEP, m_dwWeaponMinPower + m_dwWeaponAddPower);
	m_playerStatus.SetPoint(POINT_MAX_WEP, m_dwWeaponMaxPower + m_dwWeaponAddPower);
	m_playerStatus.SetPoint(POINT_MIN_MAGIC_WEP, m_dwWeaponMinMagicPower + m_dwWeaponAddPower);
	m_playerStatus.SetPoint(POINT_MAX_MAGIC_WEP, m_dwWeaponMaxMagicPower + m_dwWeaponAddPower);
	m_playerStatus.SetPoint(POINT_MIN_ATK, __GetTotalAtk(m_dwWeaponMinPower, m_dwWeaponAddPower));
	m_playerStatus.SetPoint(POINT_MAX_ATK, __GetTotalAtk(m_dwWeaponMaxPower, m_dwWeaponAddPower));

#if defined(ENABLE_ACCE_COSTUME_SYSTEM)
	m_playerStatus.SetPoint(POINT_MIN_WEP, m_playerStatus.GetPoint(POINT_MIN_WEP) + m_dwAcceMinPower);
	m_playerStatus.SetPoint(POINT_MAX_WEP, m_playerStatus.GetPoint(POINT_MAX_WEP) + m_dwAcceMaxPower);

	m_playerStatus.SetPoint(POINT_MIN_MAGIC_WEP, m_playerStatus.GetPoint(POINT_MIN_MAGIC_WEP) + m_dwAcceMinMagicPower);
	m_playerStatus.SetPoint(POINT_MAX_MAGIC_WEP, m_playerStatus.GetPoint(POINT_MAX_MAGIC_WEP) + m_dwAcceMaxMagicPower);

	m_playerStatus.SetPoint(POINT_MIN_ATK, m_playerStatus.GetPoint(POINT_MIN_ATK) + m_dwAcceMinPower);
	m_playerStatus.SetPoint(POINT_MAX_ATK, m_playerStatus.GetPoint(POINT_MAX_ATK) + m_dwAcceMaxPower);
#endif
}

void CPythonPlayer::SetStatus(POINT_TYPE wType, POINT_VALUE lValue)
{
	if (wType >= POINT_MAX_NUM)
	{
		assert(!" CPythonPlayer::SetStatus - Strange Status Type!");
		Tracef("CPythonPlayer::SetStatus - Set Status Type Error\n");
		return;
	}

	if (wType == POINT_LEVEL)
	{
		CInstanceBase* pkPlayer = NEW_GetMainActorPtr();
		if (pkPlayer)
			pkPlayer->UpdateTextTailLevel(lValue);
	}

#if defined(ENABLE_CONQUEROR_LEVEL)
	if (wType == POINT_CONQUEROR_LEVEL && lValue > 0)
	{
		CInstanceBase* pkPlayer = NEW_GetMainActorPtr();
		if (pkPlayer)
			pkPlayer->UpdateTextTailConquerorLevel(lValue);
	}
#endif

	switch (wType)
	{
		case POINT_MIN_WEP:
		case POINT_MAX_WEP:
		case POINT_MIN_ATK:
		case POINT_MAX_ATK:
		case POINT_HIT_RATE:
		case POINT_EVADE_RATE:
		case POINT_LEVEL:
		case POINT_ST:
		case POINT_DX:
		case POINT_IQ:
#if defined(ENABLE_CONQUEROR_LEVEL)
		case POINT_CONQUEROR_LEVEL:
		case POINT_SUNGMA_STR:
		case POINT_SUNGMA_HP:
		case POINT_SUNGMA_MOVE:
		case POINT_SUNGMA_IMMUNE:
#endif
			m_playerStatus.SetPoint(wType, lValue);
			__UpdateBattleStatus();
			break;

		default:
			m_playerStatus.SetPoint(wType, lValue);
			break;
	}
}

POINT_VALUE CPythonPlayer::GetStatus(POINT_TYPE wType)
{
	if (wType >= POINT_MAX_NUM)
	{
		assert(!" CPythonPlayer::GetStatus - Strange Status Type!");
		Tracef("CPythonPlayer::GetStatus - Get Status Type Error\n");
		return 0;
	}

	return m_playerStatus.GetPoint(wType);
}

const char* CPythonPlayer::GetName()
{
	return m_stName.c_str();
}

void CPythonPlayer::SetName(const char* name)
{
	m_stName = name;
}

void CPythonPlayer::NotifyDeletingCharacterInstance(DWORD dwVID)
{
	if (m_dwMainCharacterIndex == dwVID)
		m_dwMainCharacterIndex = 0;
}

void CPythonPlayer::NotifyCharacterDead(DWORD dwVID)
{
	if (__IsSameTargetVID(dwVID))
		SetTarget(0);
}

void CPythonPlayer::NotifyCharacterUpdate(DWORD dwVID)
{
	if (__IsSameTargetVID(dwVID))
	{
		CInstanceBase* pMainInstance = NEW_GetMainActorPtr();
		CInstanceBase* pTargetInstance = CPythonCharacterManager::Instance().GetInstancePtr(dwVID);
		if (pMainInstance && pTargetInstance)
		{
			if (!pMainInstance->IsTargetableInstance(*pTargetInstance))
			{
				SetTarget(0);
				PyCallClassMemberFunc(m_ppyGameWindow, "CloseTargetBoard", Py_BuildValue("()"));
			}
			else
			{
				PyCallClassMemberFunc(m_ppyGameWindow, "RefreshTargetBoardByVID", Py_BuildValue("(i)", dwVID));
			}
		}
	}
}

void CPythonPlayer::NotifyDeadMainCharacter()
{
	__ClearReservedAction();
	__ClearAutoAttackTargetActorID();
}

void CPythonPlayer::NotifyChangePKMode()
{
	PyCallClassMemberFunc(m_ppyGameWindow, "OnChangePKMode", Py_BuildValue("()"));
}

void CPythonPlayer::MoveItemData(TItemPos SrcCell, TItemPos DstCell)
{
	if (!SrcCell.IsValidCell() || !DstCell.IsValidCell())
		return;

	TItemData src_item(*GetItemData(SrcCell));
	TItemData dst_item(*GetItemData(DstCell));
	SetItemData(DstCell, src_item);
	SetItemData(SrcCell, dst_item);
}

const TItemData* CPythonPlayer::GetItemData(TItemPos Cell) const
{
	if (!Cell.IsValidCell())
		return NULL;

	switch (Cell.window_type)
	{
		case INVENTORY:
			return &m_playerStatus.aInventoryItem[Cell.cell];

		case EQUIPMENT:
			return &m_playerStatus.aEquipmentItem[Cell.cell];

#if defined(ENABLE_DRAGON_SOUL_SYSTEM)
		case DRAGON_SOUL_INVENTORY:
			return &m_playerStatus.aDragonSoulInventoryItem[Cell.cell];
#endif

		case BELT_INVENTORY:
			return &m_playerStatus.aBeltInventoryItem[Cell.cell];

#if defined(ENABLE_ATTR_6TH_7TH)
		case NPC_STORAGE:
			return &m_playerStatus.aAttr67AddItem;
#endif

		default:
			return NULL;
	}
}

void CPythonPlayer::SetItemData(TItemPos Cell, const TItemData& c_rkItemInst)
{
	if (!Cell.IsValidCell())
		return;

	if (c_rkItemInst.dwVnum != 0)
	{
		CItemData* pItemData;
		if (!CItemManager::Instance().GetItemDataPointer(c_rkItemInst.dwVnum, &pItemData))
		{
			TraceError("CPythonPlayer::SetItemData(window_type : %d, dwSlotIndex=%d, itemIndex=%d) - Failed to item data\n", Cell.window_type, Cell.cell, c_rkItemInst.dwVnum);
			return;
		}
	}

	switch (Cell.window_type)
	{
		case INVENTORY:
			m_playerStatus.aInventoryItem[Cell.cell] = c_rkItemInst;
			break;

		case EQUIPMENT:
			m_playerStatus.aEquipmentItem[Cell.cell] = c_rkItemInst;
			break;

#if defined(ENABLE_DRAGON_SOUL_SYSTEM)
		case DRAGON_SOUL_INVENTORY:
			m_playerStatus.aDragonSoulInventoryItem[Cell.cell] = c_rkItemInst;
			break;
#endif

		case BELT_INVENTORY:
			m_playerStatus.aBeltInventoryItem[Cell.cell] = c_rkItemInst;
			break;

#if defined(ENABLE_ATTR_6TH_7TH)
		case NPC_STORAGE:
			m_playerStatus.aAttr67AddItem = c_rkItemInst;
			break;
#endif
	}
}

DWORD CPythonPlayer::GetItemIndex(TItemPos Cell)
{
	if (!Cell.IsValidCell())
		return 0;

	return GetItemData(Cell)->dwVnum;
}

DWORD CPythonPlayer::GetItemFlags(TItemPos Cell)
{
	if (!Cell.IsValidCell())
		return 0;
	const TItemData* pItem = GetItemData(Cell);
	assert(pItem != NULL);
	return pItem->dwFlags;
}

DWORD CPythonPlayer::GetItemAntiFlags(TItemPos Cell)
{
	if (!Cell.IsValidCell())
		return 0;

	const TItemData* pItem = GetItemData(Cell);
	assert(pItem != NULL);
	return pItem->dwAntiFlags;
}

BYTE CPythonPlayer::GetItemTypeBySlot(TItemPos Cell)
{
	if (!Cell.IsValidCell())
		return 0;

	CItemData* pItemDataPtr = NULL;
	if (CItemManager::Instance().GetItemDataPointer(GetItemIndex(Cell), &pItemDataPtr))
		return pItemDataPtr->GetType();
	else
	{
		TraceError("FAILED\t\tCPythonPlayer::GetItemTypeBySlot()\t\tCell(%d, %d) item is null", Cell.window_type, Cell.cell);
		return 0;
	}
}

BYTE CPythonPlayer::GetItemSubTypeBySlot(TItemPos Cell)
{
	if (!Cell.IsValidCell())
		return 0;

	CItemData* pItemDataPtr = NULL;
	if (CItemManager::Instance().GetItemDataPointer(GetItemIndex(Cell), &pItemDataPtr))
		return pItemDataPtr->GetSubType();
	else
	{
		TraceError("FAILED\t\tCPythonPlayer::GetItemSubTypeBySlot()\t\tCell(%d, %d) item is null", Cell.window_type, Cell.cell);
		return 0;
	}
}

DWORD CPythonPlayer::GetItemCount(TItemPos Cell)
{
	if (!Cell.IsValidCell())
		return 0;
	const TItemData* pItem = GetItemData(Cell);
	if (pItem == NULL)
		return 0;
	else
		return pItem->dwCount;
}

DWORD CPythonPlayer::GetItemCountByVnum(DWORD dwVnum
#if defined(ENABLE_CUBE_RENEWAL) && defined(ENABLE_SET_ITEM)
	, bool bIgnoreSetValue
#endif
)
{
	DWORD dwCount = 0;

	for (const TItemData& c_rItemData : m_playerStatus.aInventoryItem)
	{
#if defined(ENABLE_CUBE_RENEWAL) && defined(ENABLE_SET_ITEM)
		if (bIgnoreSetValue && c_rItemData.bSetValue)
			continue;
#endif

		if (c_rItemData.dwVnum == dwVnum)
			dwCount += c_rItemData.dwCount;
	}

	return dwCount;
}

DWORD CPythonPlayer::GetItemMetinSocket(TItemPos Cell, DWORD dwMetinSocketIndex)
{
	if (!Cell.IsValidCell())
		return 0;

	if (dwMetinSocketIndex >= ITEM_SOCKET_SLOT_MAX_NUM)
		return 0;

	return GetItemData(Cell)->alSockets[dwMetinSocketIndex];
}

#if defined(ENABLE_APPLY_RANDOM)
void CPythonPlayer::GetItemApplyRandom(TItemPos Cell, BYTE bIndex, POINT_TYPE* pwType, POINT_VALUE* plValue)
{
	*pwType = 0;
	*plValue = 0;

	if (!Cell.IsValidCell())
		return;

	if (bIndex >= ITEM_APPLY_RANDOM_SLOT_MAX_NUM)
		return;

	*pwType = GetItemData(Cell)->aApplyRandom[bIndex].wType;
	*plValue = GetItemData(Cell)->aApplyRandom[bIndex].lValue;
}
#endif

void CPythonPlayer::GetItemAttribute(TItemPos Cell, BYTE bIndex, POINT_TYPE* pwType, POINT_VALUE* plValue)
{
	*pwType = 0;
	*plValue = 0;

	if (!Cell.IsValidCell())
		return;

	if (bIndex >= ITEM_ATTRIBUTE_SLOT_MAX_NUM)
		return;

	*pwType = GetItemData(Cell)->aAttr[bIndex].wType;
	*plValue = GetItemData(Cell)->aAttr[bIndex].lValue;
}

#if defined(ENABLE_CHANGED_ATTR)
void CPythonPlayer::GetItemChangedAttribute(DWORD dwAttrSlotIndex, POINT_TYPE* pwType, POINT_VALUE* plValue)
{
	*pwType = 0;
	*plValue = 0;

	if (dwAttrSlotIndex >= ITEM_ATTRIBUTE_SLOT_MAX_NUM)
		return;

	*pwType = m_SelectAttrArr[dwAttrSlotIndex].wType;
	*plValue = m_SelectAttrArr[dwAttrSlotIndex].lValue;
}

void CPythonPlayer::SetSelectAttr(const TPlayerItemAttribute* attr)
{
	memcpy(m_SelectAttrArr, attr, sizeof(m_SelectAttrArr));
}
#endif

void CPythonPlayer::SetItemCount(TItemPos Cell, DWORD dwCount)
{
	if (!Cell.IsValidCell())
		return;

	(const_cast <TItemData*>(GetItemData(Cell)))->dwCount = dwCount;
	PyCallClassMemberFunc(m_ppyGameWindow, "RefreshInventory", Py_BuildValue("()"));
}

#if defined(ENABLE_SOULBIND_SYSTEM)
void CPythonPlayer::SealItem(TItemPos Cell, long lSealDate)
{
	const TItemData* pItem = GetItemData(Cell);
	if (!pItem)
	{
		TraceError("FAILED\t\tCPythonPlayer::SealItem()\t\tCell(%d, %d) item is null", Cell.window_type, Cell.cell);
		return;
	}

	(const_cast <TItemData*>(pItem))->lSealDate = lSealDate;
}

bool CPythonPlayer::IsSealedItemBySlot(TItemPos Cell)
{
	const TItemData* pItem = GetItemData(Cell);
	if (!pItem)
	{
		TraceError("FAILED\t\tCPythonPlayer::IsSealedItemBySlot()\t\tCell(%d, %d) item is null", Cell.window_type, Cell.cell);
		return false;
	}

	return (pItem->lSealDate != CItemData::E_SEAL_DATE_DEFAULT_TIMESTAMP || pItem->lSealDate == CItemData::U_SEAL_DATE_DEFAULT_TIMESTAMP);
}
#endif

void CPythonPlayer::SetItemMetinSocket(TItemPos Cell, DWORD dwMetinSocketIndex, DWORD dwMetinNumber)
{
	if (!Cell.IsValidCell())
		return;
	if (dwMetinSocketIndex >= ITEM_SOCKET_SLOT_MAX_NUM)
		return;

	(const_cast <TItemData*>(GetItemData(Cell)))->alSockets[dwMetinSocketIndex] = dwMetinNumber;
}

#if defined(ENABLE_APPLY_RANDOM)
void CPythonPlayer::SetItemApplyRandom(TItemPos Cell, BYTE bIndex, POINT_TYPE wType, POINT_VALUE lValue)
{
	if (!Cell.IsValidCell())
		return;

	if (bIndex >= ITEM_APPLY_RANDOM_SLOT_MAX_NUM)
		return;

	(const_cast <TItemData*>(GetItemData(Cell)))->aApplyRandom[bIndex].wType = wType;
	(const_cast <TItemData*>(GetItemData(Cell)))->aApplyRandom[bIndex].lValue = lValue;
}
#endif

void CPythonPlayer::SetItemAttribute(TItemPos Cell, BYTE bIndex, POINT_TYPE wType, POINT_VALUE lValue)
{
	if (!Cell.IsValidCell())
		return;

	if (bIndex >= ITEM_ATTRIBUTE_SLOT_MAX_NUM)
		return;

	(const_cast<TItemData*>(GetItemData(Cell)))->aAttr[bIndex].wType = wType;
	(const_cast<TItemData*>(GetItemData(Cell)))->aAttr[bIndex].lValue = lValue;
}

int CPythonPlayer::GetQuickPage()
{
	return m_playerStatus.lQuickPageIndex;
}

void CPythonPlayer::SetQuickPage(BYTE nQuickPageIndex)
{
	if (nQuickPageIndex < 0)
	{
		m_playerStatus.lQuickPageIndex = QUICKSLOT_MAX_LINE + nQuickPageIndex;
	}
	else if (nQuickPageIndex >= QUICKSLOT_MAX_LINE)
	{
		m_playerStatus.lQuickPageIndex = nQuickPageIndex % QUICKSLOT_MAX_LINE;
	}
	else
	{
		m_playerStatus.lQuickPageIndex = nQuickPageIndex;
	}

	PyCallClassMemberFunc(m_ppyGameWindow, "RefreshInventory", Py_BuildValue("()"));
}

BYTE CPythonPlayer::LocalQuickSlotIndexToGlobalQuickSlotIndex(BYTE bLocalSlotIndex)
{
	return m_playerStatus.lQuickPageIndex * QUICKSLOT_MAX_COUNT_PER_LINE + bLocalSlotIndex;
}

void CPythonPlayer::GetGlobalQuickSlotData(BYTE bGlobalSlotIndex, BYTE* pbWndType, WORD* pwWndItemPos)
{
	TQuickSlot& rkQuickSlot = __RefGlobalQuickSlot(bGlobalSlotIndex);
	*pbWndType = rkQuickSlot.Type;
	*pwWndItemPos = rkQuickSlot.Position;
}

void CPythonPlayer::GetLocalQuickSlotData(BYTE bSlotPos, BYTE* pbWndType, WORD* pwWndItemPos)
{
	TQuickSlot& rkQuickSlot = __RefLocalQuickSlot(bSlotPos);
	*pbWndType = rkQuickSlot.Type;
	*pwWndItemPos = rkQuickSlot.Position;
}

TQuickSlot& CPythonPlayer::__RefLocalQuickSlot(BYTE SlotIndex)
{
	return __RefGlobalQuickSlot(LocalQuickSlotIndexToGlobalQuickSlotIndex(SlotIndex));
}

TQuickSlot& CPythonPlayer::__RefGlobalQuickSlot(BYTE SlotIndex)
{
	if (SlotIndex < 0 || SlotIndex >= QUICKSLOT_MAX_NUM)
	{
		static TQuickSlot s_kQuickSlot;
		s_kQuickSlot.Type = 0;
		s_kQuickSlot.Position = 0;
		return s_kQuickSlot;
	}

	return m_playerStatus.aQuickSlot[SlotIndex];
}

void CPythonPlayer::RemoveQuickSlotByValue(BYTE bType, WORD wPosition)
{
	for (BYTE i = 0; i < QUICKSLOT_MAX_NUM; ++i)
	{
		if (bType == m_playerStatus.aQuickSlot[i].Type)
			if (wPosition == m_playerStatus.aQuickSlot[i].Position)
				CPythonNetworkStream::Instance().SendQuickSlotDelPacket(i);
	}
}

char CPythonPlayer::IsItem(TItemPos Cell)
{
	if (!Cell.IsValidCell())
		return 0;

	return 0 != GetItemData(Cell)->dwVnum;
}

void CPythonPlayer::RequestMoveGlobalQuickSlotToLocalQuickSlot(BYTE bGlobalSrcSlotIndex, BYTE bLocalDstSlotIndex)
{
	BYTE bGlobalDstSlotIndex = LocalQuickSlotIndexToGlobalQuickSlotIndex(bLocalDstSlotIndex);

	CPythonNetworkStream& rkNetStream = CPythonNetworkStream::Instance();
	rkNetStream.SendQuickSlotMovePacket(bGlobalSrcSlotIndex, bGlobalDstSlotIndex);
}

void CPythonPlayer::RequestAddLocalQuickSlot(BYTE bLocalSlotIndex, BYTE bWndType, WORD wWndItemPos)
{
	if (bLocalSlotIndex >= QUICKSLOT_MAX_COUNT_PER_LINE)
		return;

	BYTE bGlobalSlotIndex = LocalQuickSlotIndexToGlobalQuickSlotIndex(bLocalSlotIndex);

	CPythonNetworkStream& rkNetStream = CPythonNetworkStream::Instance();
	rkNetStream.SendQuickSlotAddPacket(bGlobalSlotIndex, bWndType, wWndItemPos);
}

void CPythonPlayer::RequestAddToEmptyLocalQuickSlot(BYTE bWndType, WORD wWndItemPos)
{
	for (BYTE i = 0; i < QUICKSLOT_MAX_COUNT_PER_LINE; ++i)
	{
		TQuickSlot& rkQuickSlot = __RefLocalQuickSlot(i);

		if (0 == rkQuickSlot.Type)
		{
			BYTE bGlobalQuickSlotIndex = LocalQuickSlotIndexToGlobalQuickSlotIndex(i);
			CPythonNetworkStream& rkNetStream = CPythonNetworkStream::Instance();
			rkNetStream.SendQuickSlotAddPacket(bGlobalQuickSlotIndex, bWndType, wWndItemPos);
			return;
		}
	}
}

void CPythonPlayer::RequestDeleteGlobalQuickSlot(BYTE bGlobalSlotIndex)
{
	if (bGlobalSlotIndex >= QUICKSLOT_MAX_COUNT)
		return;

	CPythonNetworkStream& rkNetStream = CPythonNetworkStream::Instance();
	rkNetStream.SendQuickSlotDelPacket(bGlobalSlotIndex);
}

void CPythonPlayer::UseLocalQuickSlot(BYTE bLocalSlotIndex)
{
	if (GetDefaultCodePage() == CP_ARABIC)
	{
		if (bLocalSlotIndex > 3)
			bLocalSlotIndex = 11 - bLocalSlotIndex;
		else
			bLocalSlotIndex = 3 - bLocalSlotIndex;
	}

	if (bLocalSlotIndex < QUICKSLOT_MAX_COUNT_PER_LINE)
	{
		BYTE bRegisteredSlotType = 0;
		WORD wRegisteredSlotPos = 0;
		GetLocalQuickSlotData(bLocalSlotIndex, &bRegisteredSlotType, &wRegisteredSlotPos);
		RequestUseLocalQuickSlot(bLocalSlotIndex);
	}
}

void CPythonPlayer::RequestUseLocalQuickSlot(BYTE bLocalSlotIndex)
{
	if (bLocalSlotIndex >= QUICKSLOT_MAX_COUNT_PER_LINE)
		return;

	BYTE bRegisteredSlotType;
	WORD wRegisteredSlotPos;
	GetLocalQuickSlotData(bLocalSlotIndex, &bRegisteredSlotType, &wRegisteredSlotPos);

	switch (bRegisteredSlotType)
	{
		case SLOT_TYPE_INVENTORY:
		{
			PyCallClassMemberFunc(m_ppyGameWindow, "UseInventoryItemInQuickSlot", Py_BuildValue("(i)", wRegisteredSlotPos));
			break;
		}
		case SLOT_TYPE_SKILL:
		{
			ClickSkillSlot(wRegisteredSlotPos);
			break;
		}
		case SLOT_TYPE_EMOTION:
		{
			PyCallClassMemberFunc(m_ppyGameWindow, "BINARY_ActEmotion", Py_BuildValue("(i)", wRegisteredSlotPos));
			break;
		}
	}
}

void CPythonPlayer::AddQuickSlot(BYTE QuickSlotIndex, BYTE IconType, WORD IconPosition)
{
	if (QuickSlotIndex < 0 || QuickSlotIndex >= QUICKSLOT_MAX_NUM)
		return;

	m_playerStatus.aQuickSlot[QuickSlotIndex].Type = IconType;
	m_playerStatus.aQuickSlot[QuickSlotIndex].Position = IconPosition;
}

void CPythonPlayer::DeleteQuickSlot(BYTE QuickSlotIndex)
{
	if (QuickSlotIndex < 0 || QuickSlotIndex >= QUICKSLOT_MAX_NUM)
		return;

	m_playerStatus.aQuickSlot[QuickSlotIndex].Type = 0;
	m_playerStatus.aQuickSlot[QuickSlotIndex].Position = 0;
}

void CPythonPlayer::MoveQuickSlot(BYTE Source, BYTE Target)
{
	if (Source < 0 || Source >= QUICKSLOT_MAX_NUM)
		return;

	if (Target < 0 || Target >= QUICKSLOT_MAX_NUM)
		return;

	TQuickSlot& rkSrcSlot = __RefGlobalQuickSlot(Source);
	TQuickSlot& rkDstSlot = __RefGlobalQuickSlot(Target);

	std::swap(rkSrcSlot, rkDstSlot);
}

#if defined(ENABLE_NEW_EQUIPMENT_SYSTEM)
bool CPythonPlayer::IsBeltInventorySlot(TItemPos Cell)
{
	return Cell.IsBeltInventoryCell();
}
#endif

bool CPythonPlayer::IsInventorySlot(TItemPos Cell)
{
	return !Cell.IsEquipCell() && Cell.IsValidCell();
}

bool CPythonPlayer::IsEquipmentSlot(TItemPos Cell)
{
	return Cell.IsEquipCell();
}

bool CPythonPlayer::IsEquipItemInSlot(TItemPos Cell)
{
	if (!Cell.IsEquipCell())
	{
		return false;
	}

	const TItemData* pData = GetItemData(Cell);

	if (NULL == pData)
	{
		return false;
	}

	DWORD dwItemIndex = pData->dwVnum;

	CItemManager::Instance().SelectItemData(dwItemIndex);
	CItemData* pItemData = CItemManager::Instance().GetSelectedItemDataPointer();
	if (!pItemData)
	{
		TraceError("Failed to find ItemData - CPythonPlayer::IsEquipItem(window_type=%d, iSlotindex=%d)\n", Cell.window_type, Cell.cell);
		return false;
	}

	return pItemData->IsEquipment() ? true : false;
}


void CPythonPlayer::SetSkill(DWORD dwSlotIndex, DWORD dwSkillIndex)
{
	if (dwSlotIndex >= SKILL_MAX_NUM)
		return;

	m_playerStatus.aSkill[dwSlotIndex].dwIndex = dwSkillIndex;
	m_skillSlotDict[dwSkillIndex] = dwSlotIndex;
}

int CPythonPlayer::GetSkillIndex(DWORD dwSlotIndex)
{
	if (dwSlotIndex >= SKILL_MAX_NUM)
		return 0;

	return m_playerStatus.aSkill[dwSlotIndex].dwIndex;
}

bool CPythonPlayer::GetSkillSlotIndex(DWORD dwSkillIndex, DWORD* pdwSlotIndex)
{
	std::map<DWORD, DWORD>::iterator f = m_skillSlotDict.find(dwSkillIndex);
	if (m_skillSlotDict.end() == f)
	{
		return false;
	}

	*pdwSlotIndex = f->second;

	return true;
}

int CPythonPlayer::GetSkillGrade(DWORD dwSlotIndex)
{
	if (dwSlotIndex >= SKILL_MAX_NUM)
		return 0;

	return m_playerStatus.aSkill[dwSlotIndex].iGrade;
}

int CPythonPlayer::GetSkillLevel(DWORD dwSlotIndex)
{
	if (dwSlotIndex >= SKILL_MAX_NUM)
		return 0;

	return m_playerStatus.aSkill[dwSlotIndex].iLevel;
}

float CPythonPlayer::GetSkillCurrentEfficientPercentage(DWORD dwSlotIndex)
{
	if (dwSlotIndex >= SKILL_MAX_NUM)
		return 0;

	return m_playerStatus.aSkill[dwSlotIndex].fcurEfficientPercentage;
}

float CPythonPlayer::GetSkillNextEfficientPercentage(DWORD dwSlotIndex)
{
	if (dwSlotIndex >= SKILL_MAX_NUM)
		return 0;

	return m_playerStatus.aSkill[dwSlotIndex].fnextEfficientPercentage;
}

void CPythonPlayer::SetSkillLevel(DWORD dwSlotIndex, DWORD dwSkillLevel)
{
	assert(!"CPythonPlayer::SetSkillLevel - 사용하지 않는 함수");
	if (dwSlotIndex >= SKILL_MAX_NUM)
		return;

	m_playerStatus.aSkill[dwSlotIndex].iGrade = -1;
	m_playerStatus.aSkill[dwSlotIndex].iLevel = dwSkillLevel;
}

void CPythonPlayer::SetSkillLevel_(DWORD dwSkillIndex, DWORD dwSkillGrade, DWORD dwSkillLevel)
{
	DWORD dwSlotIndex;
	if (!GetSkillSlotIndex(dwSkillIndex, &dwSlotIndex))
		return;

	if (dwSlotIndex >= SKILL_MAX_NUM)
		return;

	switch (dwSkillGrade)
	{
		case 0:
			m_playerStatus.aSkill[dwSlotIndex].iGrade = dwSkillGrade;
			m_playerStatus.aSkill[dwSlotIndex].iLevel = dwSkillLevel;
			break;
		case 1:
			m_playerStatus.aSkill[dwSlotIndex].iGrade = dwSkillGrade;
			m_playerStatus.aSkill[dwSlotIndex].iLevel = dwSkillLevel - 19;
			break;
		case 2:
			m_playerStatus.aSkill[dwSlotIndex].iGrade = dwSkillGrade;
			m_playerStatus.aSkill[dwSlotIndex].iLevel = dwSkillLevel - 29;
			break;
		case 3:
			m_playerStatus.aSkill[dwSlotIndex].iGrade = dwSkillGrade;
			m_playerStatus.aSkill[dwSlotIndex].iLevel = dwSkillLevel - 39;
			break;
		default:
			break;
	}

	const DWORD SKILL_MAX_LEVEL = 40;

	if (dwSkillLevel > SKILL_MAX_LEVEL)
	{
		m_playerStatus.aSkill[dwSlotIndex].fcurEfficientPercentage = 0.0f;
		m_playerStatus.aSkill[dwSlotIndex].fnextEfficientPercentage = 0.0f;

		TraceError("CPythonPlayer::SetSkillLevel(SlotIndex=%d, SkillLevel=%d)", dwSlotIndex, dwSkillLevel);
		return;
	}

	m_playerStatus.aSkill[dwSlotIndex].fcurEfficientPercentage = LocaleService_GetSkillPower(dwSkillLevel) / 100.0f;
	m_playerStatus.aSkill[dwSlotIndex].fnextEfficientPercentage = LocaleService_GetSkillPower(dwSkillLevel + 1) / 100.0f;
}

void CPythonPlayer::SetSkillCoolTime(DWORD dwSkillIndex)
{
	DWORD dwSlotIndex;
	if (!GetSkillSlotIndex(dwSkillIndex, &dwSlotIndex))
	{
		Tracenf("CPythonPlayer::SetSkillCoolTime(dwSkillIndex=%d) - FIND SLOT ERROR", dwSkillIndex);
		return;
	}

	if (dwSlotIndex >= SKILL_MAX_NUM)
	{
		Tracenf("CPythonPlayer::SetSkillCoolTime(dwSkillIndex=%d) - dwSlotIndex=%d/%d OUT OF RANGE", dwSkillIndex, dwSlotIndex, SKILL_MAX_NUM);
		return;
	}

	m_playerStatus.aSkill[dwSlotIndex].isCoolTime = true;
}

void CPythonPlayer::EndSkillCoolTime(DWORD dwSkillIndex)
{
	DWORD dwSlotIndex;
	if (!GetSkillSlotIndex(dwSkillIndex, &dwSlotIndex))
	{
		Tracenf("CPythonPlayer::EndSkillCoolTime(dwSkillIndex=%d) - FIND SLOT ERROR", dwSkillIndex);
		return;
	}

	if (dwSlotIndex >= SKILL_MAX_NUM)
	{
		Tracenf("CPythonPlayer::EndSkillCoolTime(dwSkillIndex=%d) - dwSlotIndex=%d/%d OUT OF RANGE", dwSkillIndex, dwSlotIndex, SKILL_MAX_NUM);
		return;
	}

	m_playerStatus.aSkill[dwSlotIndex].isCoolTime = false;
}

float CPythonPlayer::GetSkillCoolTime(DWORD dwSlotIndex)
{
	if (dwSlotIndex >= SKILL_MAX_NUM)
		return 0.0f;

	return m_playerStatus.aSkill[dwSlotIndex].fCoolTime;
}

float CPythonPlayer::GetSkillElapsedCoolTime(DWORD dwSlotIndex)
{
	if (dwSlotIndex >= SKILL_MAX_NUM)
		return 0.0f;

	return CTimer::Instance().GetCurrentSecond() - m_playerStatus.aSkill[dwSlotIndex].fLastUsedTime;
}

void CPythonPlayer::__ActivateSkillSlot(DWORD dwSlotIndex)
{
	if (dwSlotIndex >= SKILL_MAX_NUM)
	{
		Tracenf("CPythonPlayer::ActivavteSkill(dwSlotIndex=%d/%d) - OUT OF RANGE", dwSlotIndex, SKILL_MAX_NUM);
		return;
	}

	m_playerStatus.aSkill[dwSlotIndex].bActive = TRUE;
	PyCallClassMemberFunc(m_ppyGameWindow, "ActivateSkillSlot", Py_BuildValue("(i)", dwSlotIndex));
}

void CPythonPlayer::__DeactivateSkillSlot(DWORD dwSlotIndex)
{
	if (dwSlotIndex >= SKILL_MAX_NUM)
	{
		Tracenf("CPythonPlayer::DeactivavteSkill(dwSlotIndex=%d/%d) - OUT OF RANGE", dwSlotIndex, SKILL_MAX_NUM);
		return;
	}

	m_playerStatus.aSkill[dwSlotIndex].bActive = FALSE;
	PyCallClassMemberFunc(m_ppyGameWindow, "DeactivateSkillSlot", Py_BuildValue("(i)", dwSlotIndex));
}

BOOL CPythonPlayer::IsSkillCoolTime(DWORD dwSlotIndex)
{
	if (!__CheckRestSkillCoolTime(dwSlotIndex))
		return FALSE;

	return TRUE;
}

BOOL CPythonPlayer::IsSkillActive(DWORD dwSlotIndex)
{
	if (dwSlotIndex >= SKILL_MAX_NUM)
		return FALSE;

	return m_playerStatus.aSkill[dwSlotIndex].bActive;
}

BOOL CPythonPlayer::IsToggleSkill(DWORD dwSlotIndex)
{
	if (dwSlotIndex >= SKILL_MAX_NUM)
		return FALSE;

	DWORD dwSkillIndex = m_playerStatus.aSkill[dwSlotIndex].dwIndex;

	CPythonSkill::TSkillData* pSkillData;
	if (!CPythonSkill::Instance().GetSkillData(dwSkillIndex, &pSkillData))
		return FALSE;

	return pSkillData->IsToggleSkill();
}

void CPythonPlayer::SetPlayTime(DWORD dwPlayTime)
{
	m_dwPlayTime = dwPlayTime;
}

DWORD CPythonPlayer::GetPlayTime()
{
	return m_dwPlayTime;
}

void CPythonPlayer::SendClickItemPacket(DWORD dwIID)
{
	if (IsObserverMode())
		return;

	static DWORD s_dwNextTCPTime = 0;

	DWORD dwCurTime = ELTimer_GetMSec();

	if (dwCurTime >= s_dwNextTCPTime)
	{
		s_dwNextTCPTime = dwCurTime + 500;

		const char* c_szOwnerName;
		if (!CPythonItem::Instance().GetOwnership(dwIID, &c_szOwnerName))
			return;

		if (strlen(c_szOwnerName) > 0)
			if (0 != strcmp(c_szOwnerName, GetName()))
			{
				CItemData* pItemData;
				if (!CItemManager::Instance().GetItemDataPointer(CPythonItem::Instance().GetVirtualNumberOfGroundItem(dwIID), &pItemData))
				{
					Tracenf("CPythonPlayer::SendClickItemPacket(dwIID=%d) : Non-exist item.", dwIID);
					return;
				}

				if (!IsPartyMemberByName(c_szOwnerName) || pItemData->IsAntiFlag(CItemData::ITEM_ANTIFLAG_DROP | CItemData::ITEM_ANTIFLAG_GIVE))
				{
					PyCallClassMemberFunc(m_ppyGameWindow, "OnCannotPickItem", Py_BuildValue("()"));
					return;
				}
			}

		CPythonNetworkStream& rkNetStream = CPythonNetworkStream::Instance();
		rkNetStream.SendItemPickUpPacket(dwIID);
	}
}

void CPythonPlayer::__SendClickActorPacket(CInstanceBase& rkInstVictim)
{
	// 말을 타고 광산을 캐는 것에 대한 예외 처리
	CInstanceBase* pkInstMain = NEW_GetMainActorPtr();
	if (pkInstMain)
		if (pkInstMain->IsHoldingPickAxe())
			if (pkInstMain->IsMountingHorse())
				if (rkInstVictim.IsResource())
				{
					PyCallClassMemberFunc(m_ppyGameWindow, "OnCannotMining", Py_BuildValue("()"));
					return;
				}

	static DWORD s_dwNextTCPTime = 0;

	DWORD dwCurTime = ELTimer_GetMSec();

	if (dwCurTime >= s_dwNextTCPTime)
	{
		s_dwNextTCPTime = dwCurTime + 1000;

		CPythonNetworkStream& rkNetStream = CPythonNetworkStream::Instance();

		DWORD dwVictimVID = rkInstVictim.GetVirtualID();
		rkNetStream.SendOnClickPacket(dwVictimVID);
	}
}

void CPythonPlayer::ActEmotion(DWORD dwEmotionID)
{
	CInstanceBase* pkInstTarget = __GetAliveTargetInstancePtr();
	if (!pkInstTarget)
	{
		PyCallClassMemberFunc(m_ppyGameWindow, "OnCannotShotError", Py_BuildValue("(is)", GetMainCharacterIndex(), "NEED_TARGET"));
		return;
	}

	CPythonNetworkStream::Instance().SendCommandPacket(_getf("/kiss %s", pkInstTarget->GetNameString()));
}

void CPythonPlayer::StartEmotionProcess()
{
	__ClearReservedAction();
	__ClearAutoAttackTargetActorID();

	m_bisProcessingEmotion = TRUE;
}

void CPythonPlayer::EndEmotionProcess()
{
	m_bisProcessingEmotion = FALSE;
}

BOOL CPythonPlayer::__IsProcessingEmotion()
{
	return m_bisProcessingEmotion;
}

#if defined(ENABLE_DRAGON_SOUL_SYSTEM)
// Dragon Soul
bool CPythonPlayer::IsDSPageFull(const BYTE bPageIndex)
{
	if (bPageIndex < 0 || bPageIndex >= DS_DECK_MAX_NUM)
		return false;

	const DWORD dwAffectType = CInstanceBase::NEW_AFFECT_DRAGON_SOUL_DECK1 + bPageIndex;
	if (GetAffectDataIndex(dwAffectType, 0) == -1)
		return false;

	BYTE bSlotCount = 0;
	const DWORD dwSlotStartIndex = c_DragonSoul_Equip_Start + (bPageIndex * CItemData::DS_SLOT_NUM_TYPES);
	const DWORD dwSlotEndIndex = dwSlotStartIndex + CItemData::DS_SLOT_NUM_TYPES;

	for (DWORD dwSlotIndex = dwSlotStartIndex; dwSlotIndex < dwSlotEndIndex; ++dwSlotIndex)
	{
		const TItemPos Cell = TItemPos(INVENTORY, dwSlotIndex);
		const DWORD dwItemIndex = GetItemIndex(Cell);

		CItemManager::Instance().SelectItemData(dwItemIndex);
		const CItemData* pItemData = CItemManager::Instance().GetSelectedItemDataPointer();
		if (!pItemData)
			continue;

		bool isNoLimit = true;
		for (BYTE i = 0; i < CItemData::ITEM_LIMIT_MAX_NUM; ++i)
		{
			CItemData::TItemLimit ItemLimit;
			if (pItemData->GetLimit(i, &ItemLimit) && ItemLimit.bType == CItemData::LIMIT_TIMER_BASED_ON_WEAR)
			{
				isNoLimit = false;
				const DWORD dwRemainTime = GetItemMetinSocket(Cell, 0);
				if (dwRemainTime > 0)
				{
					bSlotCount++;
					break;
				}
			}
		}

		if (isNoLimit)
			bSlotCount++;
	}

	return (bSlotCount == CItemData::DS_SLOT_NUM_TYPES);
}
#endif

// Dungeon
void CPythonPlayer::SetDungeonDestinationPosition(int ix, int iy)
{
	m_isDestPosition = TRUE;
	m_ixDestPos = ix;
	m_iyDestPos = iy;

	AlarmHaveToGo();
}

void CPythonPlayer::AlarmHaveToGo()
{
	m_iLastAlarmTime = CTimer::Instance().GetCurrentMillisecond();

	/////

	CInstanceBase* pInstance = NEW_GetMainActorPtr();
	if (!pInstance)
		return;

	TPixelPosition PixelPosition;
	pInstance->NEW_GetPixelPosition(&PixelPosition);

	float fAngle = GetDegreeFromPosition2(PixelPosition.x, PixelPosition.y, float(m_ixDestPos), float(m_iyDestPos));
	fAngle = fmod(540.0f - fAngle, 360.0f);
	D3DXVECTOR3 v3Rotation(0.0f, 0.0f, fAngle);

	PixelPosition.y *= -1.0f;

	CEffectManager::Instance().RegisterEffect("d:/ymir work/effect/etc/compass/appear_middle.mse");
	CEffectManager::Instance().CreateEffect("d:/ymir work/effect/etc/compass/appear_middle.mse", PixelPosition, v3Rotation);
}

// Party
void CPythonPlayer::ExitParty()
{
	m_PartyMemberMap.clear();

	CPythonCharacterManager::Instance().RefreshAllPCTextTail();
}

void CPythonPlayer::AppendPartyMember(DWORD dwPID, const char* c_szName)
{
	m_PartyMemberMap.insert(std::make_pair(dwPID, TPartyMemberInfo(dwPID, c_szName)));
}

void CPythonPlayer::LinkPartyMember(DWORD dwPID, DWORD dwVID)
{
	TPartyMemberInfo* pPartyMemberInfo;
	if (!GetPartyMemberPtr(dwPID, &pPartyMemberInfo))
	{
		TraceError(" CPythonPlayer::LinkPartyMember(dwPID=%d, dwVID=%d) - Failed to find party member", dwPID, dwVID);
		return;
	}

	pPartyMemberInfo->dwVID = dwVID;

	CInstanceBase* pInstance = NEW_FindActorPtr(dwVID);
	if (pInstance)
		pInstance->RefreshTextTail();
}

void CPythonPlayer::UnlinkPartyMember(DWORD dwPID)
{
	TPartyMemberInfo* pPartyMemberInfo;
	if (!GetPartyMemberPtr(dwPID, &pPartyMemberInfo))
	{
		TraceError(" CPythonPlayer::UnlinkPartyMember(dwPID=%d) - Failed to find party member", dwPID);
		return;
	}

	pPartyMemberInfo->dwVID = 0;
#if defined(WJ_SHOW_PARTY_ON_MINIMAP)
	pPartyMemberInfo->lX = 0;
	pPartyMemberInfo->lY = 0;
	pPartyMemberInfo->fMinimapX = 0.0f;
	pPartyMemberInfo->fMinimapY = 0.0f;
#endif
}

void CPythonPlayer::UpdatePartyMemberInfo(DWORD dwPID, BYTE byState, BYTE byHPPercentage)
{
	TPartyMemberInfo* pPartyMemberInfo;
	if (!GetPartyMemberPtr(dwPID, &pPartyMemberInfo))
	{
		TraceError(" CPythonPlayer::UpdatePartyMemberInfo(dwPID=%d, byState=%d, byHPPercentage=%d) - Failed to find character", dwPID, byState, byHPPercentage);
		return;
	}

	pPartyMemberInfo->byState = byState;
	pPartyMemberInfo->byHPPercentage = byHPPercentage;
}

void CPythonPlayer::UpdatePartyMemberAffect(DWORD dwPID, BYTE byAffectSlotIndex, short sAffectNumber)
{
	if (byAffectSlotIndex >= PARTY_AFFECT_SLOT_MAX_NUM)
	{
		TraceError(" CPythonPlayer::UpdatePartyMemberAffect(dwPID=%d, byAffectSlotIndex=%d, sAffectNumber=%d) - Strange affect slot index", dwPID, byAffectSlotIndex, sAffectNumber);
		return;
	}

	TPartyMemberInfo* pPartyMemberInfo;
	if (!GetPartyMemberPtr(dwPID, &pPartyMemberInfo))
	{
		TraceError(" CPythonPlayer::UpdatePartyMemberAffect(dwPID=%d, byAffectSlotIndex=%d, sAffectNumber=%d) - Failed to find character", dwPID, byAffectSlotIndex, sAffectNumber);
		return;
	}

	pPartyMemberInfo->sAffects[byAffectSlotIndex] = sAffectNumber;
}

void CPythonPlayer::RemovePartyMember(DWORD dwPID)
{
	DWORD dwVID = 0;
	TPartyMemberInfo* pPartyMemberInfo;
	if (GetPartyMemberPtr(dwPID, &pPartyMemberInfo))
	{
		dwVID = pPartyMemberInfo->dwVID;
	}

	m_PartyMemberMap.erase(dwPID);

	if (dwVID > 0)
	{
		CInstanceBase* pInstance = NEW_FindActorPtr(dwVID);
		if (pInstance)
			pInstance->RefreshTextTail();
	}
}

bool CPythonPlayer::IsPartyMemberByVID(DWORD dwVID)
{
	std::map<DWORD, TPartyMemberInfo>::iterator itor = m_PartyMemberMap.begin();
	for (; itor != m_PartyMemberMap.end(); ++itor)
	{
		TPartyMemberInfo& rPartyMemberInfo = itor->second;
		if (dwVID == rPartyMemberInfo.dwVID)
			return true;
	}

	return false;
}

bool CPythonPlayer::IsPartyMemberByName(const char* c_szName)
{
	std::map<DWORD, TPartyMemberInfo>::iterator itor = m_PartyMemberMap.begin();
	for (; itor != m_PartyMemberMap.end(); ++itor)
	{
		TPartyMemberInfo& rPartyMemberInfo = itor->second;
		if (0 == rPartyMemberInfo.strName.compare(c_szName))
			return true;
	}

	return false;
}

bool CPythonPlayer::GetPartyMemberPtr(DWORD dwPID, TPartyMemberInfo** ppPartyMemberInfo)
{
	std::map<DWORD, TPartyMemberInfo>::iterator itor = m_PartyMemberMap.find(dwPID);

	if (m_PartyMemberMap.end() == itor)
		return false;

	*ppPartyMemberInfo = &(itor->second);

	return true;
}

bool CPythonPlayer::PartyMemberPIDToVID(DWORD dwPID, DWORD* pdwVID)
{
	std::map<DWORD, TPartyMemberInfo>::iterator itor = m_PartyMemberMap.find(dwPID);

	if (m_PartyMemberMap.end() == itor)
		return false;

	const TPartyMemberInfo& c_rPartyMemberInfo = itor->second;
	*pdwVID = c_rPartyMemberInfo.dwVID;

	return true;
}

bool CPythonPlayer::PartyMemberVIDToPID(DWORD dwVID, DWORD* pdwPID)
{
	std::map<DWORD, TPartyMemberInfo>::iterator itor = m_PartyMemberMap.begin();
	for (; itor != m_PartyMemberMap.end(); ++itor)
	{
		TPartyMemberInfo& rPartyMemberInfo = itor->second;
		if (dwVID == rPartyMemberInfo.dwVID)
		{
			*pdwPID = rPartyMemberInfo.dwPID;
			return true;
		}
	}

	return false;
}

bool CPythonPlayer::IsSamePartyMember(DWORD dwVID1, DWORD dwVID2)
{
	return (IsPartyMemberByVID(dwVID1) && IsPartyMemberByVID(dwVID2));
}

#if defined(WJ_SHOW_PARTY_ON_MINIMAP)
void CPythonPlayer::UpdatePartyMemberPosition(DWORD dwPID, long lX, long lY)
{
	CPythonPlayer::TPartyMemberInfo* pPartyMemberInfo;
	if (!CPythonPlayer::Instance().GetPartyMemberPtr(dwPID, &pPartyMemberInfo))
		return;

	pPartyMemberInfo->lX = lX;
	pPartyMemberInfo->lY = lY;
}

void CPythonPlayer::UpdatePartyMemberMinimapPosition(DWORD dwPID, float fX, float fY)
{
	CPythonPlayer::TPartyMemberInfo* pPartyMemberInfo;
	if (!CPythonPlayer::Instance().GetPartyMemberPtr(dwPID, &pPartyMemberInfo))
		return;

	pPartyMemberInfo->fMinimapX = fX;
	pPartyMemberInfo->fMinimapY = fY;
}

std::size_t CPythonPlayer::GetPartyMemberCount()
{
	return m_PartyMemberMap.size();
}

DWORD CPythonPlayer::GetPartyMemberPIDByIndex(std::size_t nIndex)
{
	if (nIndex >= m_PartyMemberMap.size())
		return 0;

	std::map<DWORD, TPartyMemberInfo>::const_iterator it = m_PartyMemberMap.begin();
	std::advance(it, nIndex);
	return it->first;
}

bool CPythonPlayer::GetPartyMemberPosition(DWORD dwPID, D3DXVECTOR2* v2Position)
{
	std::map<DWORD, TPartyMemberInfo>::const_iterator it = m_PartyMemberMap.find(dwPID);
	if (it == m_PartyMemberMap.end())
		return false;

	v2Position->x = it->second.lX;
	v2Position->y = it->second.lY;
	return true;
}

bool CPythonPlayer::GetPartyMemberMinimapPosition(DWORD dwPID, D3DXVECTOR2* v2Position)
{
	std::map<DWORD, TPartyMemberInfo>::const_iterator it = m_PartyMemberMap.find(dwPID);
	if (it == m_PartyMemberMap.end())
		return false;

	v2Position->x = it->second.fMinimapX;
	v2Position->y = it->second.fMinimapY;
	return true;
}

bool CPythonPlayer::IsPartyPositionExist(DWORD dwPID)
{
	std::map<DWORD, TPartyMemberInfo>::const_iterator it = m_PartyMemberMap.find(dwPID);
	if (it == m_PartyMemberMap.end())
		return false;

	return it->second.lX != 0 || it->second.lY != 0;
}

std::string CPythonPlayer::GetPartyMemberName(DWORD dwPID)
{
	std::map<DWORD, TPartyMemberInfo>::const_iterator it = m_PartyMemberMap.find(dwPID);
	if (it == m_PartyMemberMap.end())
		return "";

	return it->second.strName;
}
#endif

// PVP
void CPythonPlayer::RememberChallengeInstance(DWORD dwVID)
{
	m_RevengeInstanceSet.erase(dwVID);
	m_ChallengeInstanceSet.insert(dwVID);
}

void CPythonPlayer::RememberRevengeInstance(DWORD dwVID)
{
	m_ChallengeInstanceSet.erase(dwVID);
	m_RevengeInstanceSet.insert(dwVID);
}

void CPythonPlayer::RememberCantFightInstance(DWORD dwVID)
{
	m_CantFightInstanceSet.insert(dwVID);
}

void CPythonPlayer::ForgetInstance(DWORD dwVID)
{
	m_ChallengeInstanceSet.erase(dwVID);
	m_RevengeInstanceSet.erase(dwVID);
	m_CantFightInstanceSet.erase(dwVID);
}

bool CPythonPlayer::IsChallengeInstance(DWORD dwVID)
{
	return m_ChallengeInstanceSet.end() != m_ChallengeInstanceSet.find(dwVID);
}

bool CPythonPlayer::IsRevengeInstance(DWORD dwVID)
{
	return m_RevengeInstanceSet.end() != m_RevengeInstanceSet.find(dwVID);
}

bool CPythonPlayer::IsCantFightInstance(DWORD dwVID)
{
	return m_CantFightInstanceSet.end() != m_CantFightInstanceSet.find(dwVID);
}

void CPythonPlayer::OpenPrivateShop()
{
	m_isOpenPrivateShop = TRUE;
}

void CPythonPlayer::ClosePrivateShop()
{
	m_isOpenPrivateShop = FALSE;
}

bool CPythonPlayer::IsOpenPrivateShop()
{
	return m_isOpenPrivateShop;
}

bool CPythonPlayer::IsDead()
{
	CInstanceBase* pMainInstance = CPythonCharacterManager::Instance().GetMainInstancePtr();
	if (!pMainInstance)
		return false;

	return pMainInstance->IsDead();
}

bool CPythonPlayer::IsPoly()
{
	CInstanceBase* pMainInstance = CPythonCharacterManager::Instance().GetMainInstancePtr();
	if (!pMainInstance)
		return false;

	return pMainInstance->IsPoly();
}

void CPythonPlayer::SetObserverMode(bool isEnable)
{
	m_isObserverMode = isEnable;
}

bool CPythonPlayer::IsObserverMode()
{
	return m_isObserverMode;
}

BOOL CPythonPlayer::__ToggleCoolTime()
{
	m_sysIsCoolTime = 1 - m_sysIsCoolTime;
	return m_sysIsCoolTime;
}

BOOL CPythonPlayer::__ToggleLevelLimit()
{
	m_sysIsLevelLimit = 1 - m_sysIsLevelLimit;
	return m_sysIsLevelLimit;
}

void CPythonPlayer::StartStaminaConsume(DWORD dwConsumePerSec, DWORD dwCurrentStamina)
{
	m_isConsumingStamina = TRUE;
	m_fConsumeStaminaPerSec = float(dwConsumePerSec);
	m_fCurrentStamina = float(dwCurrentStamina);

	SetStatus(POINT_STAMINA, dwCurrentStamina);
}

void CPythonPlayer::StopStaminaConsume(DWORD dwCurrentStamina)
{
	m_isConsumingStamina = FALSE;
	m_fConsumeStaminaPerSec = 0.0f;
	m_fCurrentStamina = float(dwCurrentStamina);

	SetStatus(POINT_STAMINA, dwCurrentStamina);
}

DWORD CPythonPlayer::GetPKMode()
{
	CInstanceBase* pInstance = NEW_GetMainActorPtr();
	if (!pInstance)
		return 0;

	return pInstance->GetPKMode();
}

#if defined(ENABLE_KEYCHANGE_SYSTEM)
void CPythonPlayer::OpenKeyChangeWindow()
{
	if (!m_isOpenKeySettingWindow)
		PyCallClassMemberFunc(m_ppyGameWindow, "OpenKeyChangeWindow", Py_BuildValue("()"));
}
#endif

void CPythonPlayer::SetGameWindow(PyObject* ppyObject)
{
	m_ppyGameWindow = ppyObject;
}

void CPythonPlayer::NEW_ClearSkillData(bool bAll)
{
	std::map<DWORD, DWORD>::iterator it;

	for (it = m_skillSlotDict.begin(); it != m_skillSlotDict.end();)
	{
		if (bAll || __GetSkillType(it->first) == CPythonSkill::SKILL_TYPE_ACTIVE)
			it = m_skillSlotDict.erase(it);
		else
			++it;
	}

	for (int i = 0; i < SKILL_MAX_NUM; ++i)
	{
		ZeroMemory(&m_playerStatus.aSkill[i], sizeof(TSkillInstance));
	}

	for (int j = 0; j < SKILL_MAX_NUM; ++j)
	{
		// 2004.09.30.myevan.스킬갱신시 스킬 포인트업[+] 버튼이 안나와 처리
		m_playerStatus.aSkill[j].iGrade = 0;
		m_playerStatus.aSkill[j].fcurEfficientPercentage = 0.0f;
		m_playerStatus.aSkill[j].fnextEfficientPercentage = 0.05f;
	}

	if (m_ppyGameWindow)
		PyCallClassMemberFunc(m_ppyGameWindow, "BINARY_CheckGameButton", Py_BuildNone());
}

void CPythonPlayer::ClearSkillDict()
{
	// ClearSkillDict
	m_skillSlotDict.clear();

	// Game End - Player Data Reset
	m_isOpenPrivateShop = false;
	m_isObserverMode = false;
	m_isOpenSafeBox = false;
	m_isOpenMall = false;

	m_isConsumingStamina = FALSE;
	m_fConsumeStaminaPerSec = 0.0f;
	m_fCurrentStamina = 0.0f;

	__ClearAutoAttackTargetActorID();

	m_vecAffectData.clear();
}

void CPythonPlayer::Clear()
{
	memset(&m_playerStatus, 0, sizeof(m_playerStatus));
	NEW_ClearSkillData(true);

#if defined(ENABLE_EXPRESSING_EMOTION)
	m_map_kSpecialAction.clear();
#endif

	m_bisProcessingEmotion = FALSE;

	m_dwSendingTargetVID = 0;
	m_fTargetUpdateTime = 0.0f;

	// Test Code for Status Interface
	m_stName = "";
	m_dwMainCharacterIndex = 0;
	m_dwRace = 0;

	// Weapon Power
	m_dwWeaponMinPower = 0;
	m_dwWeaponMaxPower = 0;
	m_dwWeaponMinMagicPower = 0;
	m_dwWeaponMaxMagicPower = 0;
	m_dwWeaponAddPower = 0;

#if defined(ENABLE_ACCE_COSTUME_SYSTEM)
	// Acce Power
	m_dwAcceMinPower = 0;
	m_dwAcceMaxPower = 0;
	m_dwAcceMinMagicPower = 0;
	m_dwAcceMaxMagicPower = 0;
#endif

	/////
	m_MovingCursorPosition = TPixelPosition(0, 0, 0);
	m_fMovingCursorSettingTime = 0.0f;

	m_eReservedMode = MODE_NONE;
	m_fReservedDelayTime = 0.0f;
	m_kPPosReserved = TPixelPosition(0, 0, 0);
	m_dwVIDReserved = 0;
	m_dwIIDReserved = 0;
	m_dwSkillSlotIndexReserved = 0;
	m_dwSkillRangeReserved = 0;

	m_isUp = false;
	m_isDown = false;
	m_isLeft = false;
	m_isRight = false;
	m_isSmtMov = false;
	m_isDirMov = false;
	m_isDirKey = false;
	m_isAtkKey = false;

	m_isCmrRot = true;
	m_fCmrRotSpd = 20.0f;

	m_iComboOld = 0;

	m_dwVIDPicked = 0;
	m_dwIIDPicked = 0;

	m_dwcurSkillSlotIndex = DWORD(-1);

	m_dwTargetVID = 0;
	m_dwTargetEndTime = 0;

	m_PartyMemberMap.clear();

	m_ChallengeInstanceSet.clear();
	m_RevengeInstanceSet.clear();

	m_isOpenPrivateShop = false;
	m_isObserverMode = false;
	m_isOpenSafeBox = false;
	m_isOpenMall = false;

#if defined(ENABLE_CUBE_RENEWAL)
	m_bCubeWindowOpen = false;
#endif

#if defined(ENABLE_FISHING_GAME)
	m_bIsOpenFishingGameWindow = false;
#endif
#if defined(ENABLE_MINI_GAME_RUMI)
	m_bRumiGame = false;
	m_bMiniGameOkeyNormal = false;
#endif
#if defined(ENABLE_MINI_GAME_CATCH_KING)
	m_bCatchKingGame = false;
#endif
#if defined(ENABLE_SNOWFLAKE_STICK_EVENT)
	m_dwSnowflakeStickEvent = 0;
#endif
#if defined(ENABLE_FLOWER_EVENT)
	m_dwFlowerEvent = 0;
#endif

	m_isConsumingStamina = FALSE;
	m_fConsumeStaminaPerSec = 0.0f;
	m_fCurrentStamina = 0.0f;

	m_inGuildAreaID = 0xffffffff;

#if defined(ENABLE_KEYCHANGE_SYSTEM)
	m_isOpenKeySettingWindow = FALSE;
	m_keySettingMap.clear();
	m_bAutoRun = false;
#endif

	__ClearAutoAttackTargetActorID();

#if defined(ENABLE_EXTEND_INVEN_SYSTEM)
	m_bExtendInvenStage = 0;
	m_wExtendInvenMax = 0;
#endif

#if defined(ENABLE_ACCE_COSTUME_SYSTEM)
	m_AcceItemInstanceVector.clear();
	m_AcceItemInstanceVector.resize(ACCE_SLOT_MAX);
	for (BYTE bSlotIndex = 0; bSlotIndex < m_AcceItemInstanceVector.size(); ++bSlotIndex)
	{
		TItemData& rkAcceItemInstance = m_AcceItemInstanceVector[bSlotIndex];
		ZeroMemory(&rkAcceItemInstance, sizeof(TItemData));
	}

	m_bAcceRefineWindowOpen = false;
	m_bAcceRefineWindowType = ACCE_SLOT_TYPE_MAX;

	for (BYTE bSlotIndex = 0; bSlotIndex < ACCE_SLOT_MAX; ++bSlotIndex)
		m_AcceRefineActivatedCell[bSlotIndex] = NPOS;
#endif

#if defined(ENABLE_AURA_COSTUME_SYSTEM)
	m_AuraItemInstanceVector.clear();
	m_AuraItemInstanceVector.resize(AURA_SLOT_MAX);
	for (BYTE bSlotIndex = 0; bSlotIndex < m_AuraItemInstanceVector.size(); ++bSlotIndex)
	{
		TItemData& rkAuraItemInstance = m_AuraItemInstanceVector[bSlotIndex];
		ZeroMemory(&rkAuraItemInstance, sizeof(TItemData));
	}

	m_bAuraRefineWindowOpen = false;
	m_bAuraRefineWindowType = AURA_WINDOW_TYPE_MAX;

	for (BYTE bSlotIndex = 0; bSlotIndex < AURA_SLOT_MAX; ++bSlotIndex)
		m_AuraRefineActivatedCell[bSlotIndex] = NPOS;

	ZeroMemory(&m_bAuraRefineInfo, sizeof(TAuraRefineInfo));
#endif

#if defined(ENABLE_CHANGE_LOOK_SYSTEM)
	ClearChangeLook();
	m_bIsChangeLookWindowOpen = false;
	m_bChangeLookWindowType = static_cast<decltype(m_bChangeLookWindowType)>(EChangeLookType::CHANGE_LOOK_TYPE_ITEM);
#endif

#if defined(ENABLE_MOVE_COSTUME_ATTR)
	memset(m_MoveCostumeAttrArr, -1, sizeof(m_MoveCostumeAttrArr));
#endif
#if defined(ENABLE_CHANGED_ATTR)
	memset(m_SelectAttrArr, 0, sizeof(m_SelectAttrArr));
#endif

#if defined(ENABLE_GEM_SYSTEM)
	ClearGemShop();
#endif
}

#if defined(ENABLE_SKILL_COOLTIME_UPDATE)
void CPythonPlayer::ResetSkillCoolTimes()
{
	if (!CPythonNetworkStream::Instance().GetMainActorSkillGroup())
		return;

	for (int i = 0; i < SKILL_MAX_NUM; ++i)
	{
		TSkillInstance& rkSkillInst = m_playerStatus.aSkill[i];
		if (!rkSkillInst.fLastUsedTime && !rkSkillInst.fCoolTime)
			continue;

		rkSkillInst.fLastUsedTime = rkSkillInst.fCoolTime = 0.0f;
		PyCallClassMemberFunc(m_ppyGameWindow, "SkillClearCoolTime", Py_BuildValue("(i)", i));
	}
}
#endif

void CPythonPlayer::AddAffect(DWORD dwType, TPacketAffectElement kElem)
{
	int iAffIndex = GetAffectDataIndex(dwType, kElem.wApplyOn);
	if (iAffIndex != -1)
		m_vecAffectData.at(iAffIndex) = kElem;
	else
		m_vecAffectData.emplace_back(kElem);
}

void CPythonPlayer::RemoveAffect(DWORD dwType, DWORD dwApplyOn)
{
	TAffectDataVector::iterator it = m_vecAffectData.begin();
	for (; it != m_vecAffectData.end(); ++it)
	{
		TPacketAffectElement kElem = *it;
		if (kElem.dwType == dwType && (dwApplyOn == 0 || dwApplyOn == kElem.wApplyOn))
		{
			m_vecAffectData.erase(it);
			break;
		}
	}
}

int CPythonPlayer::GetAffectDataIndex(DWORD dwType, DWORD dwApplyOn)
{
	int ret = -1, i = 0;
	TAffectDataVector::iterator it = m_vecAffectData.begin();
	for (; it != m_vecAffectData.end(); ++it, ++i)
	{
		TPacketAffectElement kElem = *it;
		if (kElem.dwType == dwType && (dwApplyOn == 0 || dwApplyOn == kElem.wApplyOn))
		{
			ret = i;
			break;
		}
	}
	return ret;
}

long CPythonPlayer::GetAffectDuration(DWORD dwType)
{
	TAffectDataVector::iterator it = m_vecAffectData.begin();
	for (; it != m_vecAffectData.end(); ++it)
	{
		TPacketAffectElement kElem = *it;
		if (kElem.dwType == dwType)
			return kElem.lDuration;
	}
	return 0;
}

CPythonPlayer::TAffectDataVector CPythonPlayer::GetAffectDataVector(DWORD dwType)
{
	TAffectDataVector vAffect;
	TAffectDataVector::iterator it = m_vecAffectData.begin();
	for (; it != m_vecAffectData.end(); ++it)
	{
		const TPacketAffectElement kElem = *it;
		if (kElem.dwType == dwType)
			vAffect.push_back(kElem);
	}
	return vAffect;
}

/*
TPacketAffectElement CPythonPlayer::GetAffectData(DWORD dwType, BYTE bApplyOn)
{
	TPacketAffectElement ret;
	memset(&ret, 0, sizeof(TPacketAffectElement));
	for (TAffectDataVector::iterator it = m_mapAffectData.begin(); it != m_mapAffectData.end(); ++it)
	{
		TPacketAffectElement elem = *it;
		if (elem.dwType == dwType && (bApplyOn == 0 || bApplyOn == elem.bPointIdxApplyOn))
		{
			ret = elem;
			break;
		}
	}
	return ret;
}
*/

#if defined(ENABLE_ACCE_COSTUME_SYSTEM)
void CPythonPlayer::__ClearAcceRefineWindow()
{
	for (BYTE bSlotIndex = 0; bSlotIndex < ACCE_SLOT_MAX; ++bSlotIndex)
	{
		if (!m_AcceRefineActivatedCell[bSlotIndex].IsNPOS())
		{
#if defined(WJ_ENABLE_PICKUP_ITEM_EFFECT)
			PyCallClassMemberFunc(m_ppyGameWindow, "DeactivateSlot", Py_BuildValue("(ii)", m_AcceRefineActivatedCell[bSlotIndex].cell, UI::ESlotHilight::HILIGHTSLOT_ACCE));
#endif
			m_AcceRefineActivatedCell[bSlotIndex] = NPOS;
		}
	}
}

void CPythonPlayer::SetAcceRefineWindowType(BYTE bType)
{
	m_bAcceRefineWindowType = bType;
	m_bAcceRefineWindowOpen = ACCE_SLOT_TYPE_MAX != bType;

	if (!m_bAcceRefineWindowOpen)
		__ClearAcceRefineWindow();
}

bool CPythonPlayer::IsAcceWindowEmpty()
{
	for (BYTE bSlotIndex = 0; bSlotIndex < static_cast<BYTE>(m_AcceItemInstanceVector.size()); ++bSlotIndex)
	{
		if (m_AcceItemInstanceVector[bSlotIndex].dwVnum)
			return false;
	}

	return true;
}

void CPythonPlayer::SetAcceItemData(BYTE bSlotIndex, const TItemData& rItemInstance)
{
	if (bSlotIndex >= m_AcceItemInstanceVector.size())
	{
		TraceError("CPythonPlayer::SetAcceItemData(bSlotIndex=%u) - Strange slot index", bSlotIndex);
		return;
	}

	m_AcceItemInstanceVector[bSlotIndex] = rItemInstance;

#if defined(WJ_ENABLE_PICKUP_ITEM_EFFECT)
	if (bSlotIndex != ACCE_SLOT_RESULT)
		PyCallClassMemberFunc(m_ppyGameWindow, "ActivateSlot", Py_BuildValue("(ii)", m_AcceRefineActivatedCell[bSlotIndex].cell, UI::ESlotHilight::HILIGHTSLOT_ACCE));
#endif
}

void CPythonPlayer::DelAcceItemData(BYTE bSlotIndex)
{
	if (bSlotIndex >= ACCE_SLOT_MAX || bSlotIndex >= static_cast<BYTE>(m_AcceItemInstanceVector.size()))
	{
		TraceError("CPythonPlayer::DelAcceItemData(bSlotIndex=%d) - Strange slot index", bSlotIndex);
		return;
	}

	TItemData& rInstance = m_AcceItemInstanceVector[bSlotIndex];
	CItemData* pItemData = nullptr;
	if (CItemManager::Instance().GetItemDataPointer(rInstance.dwVnum, &pItemData))
	{
		if (bSlotIndex == ACCE_SLOT_LEFT)
			DelAcceItemData(ACCE_SLOT_RESULT);

		if (pItemData->GetType() == CItemData::ITEM_TYPE_WEAPON || pItemData->GetType() == CItemData::ITEM_TYPE_ARMOR)
			DelAcceItemData(ACCE_SLOT_RESULT);

		if (bSlotIndex != ACCE_SLOT_RESULT)
		{
#if defined(WJ_ENABLE_PICKUP_ITEM_EFFECT)
			PyCallClassMemberFunc(m_ppyGameWindow, "DeactivateSlot", Py_BuildValue("(ii)", m_AcceRefineActivatedCell[bSlotIndex].cell, UI::ESlotHilight::HILIGHTSLOT_ACCE));
#endif
			m_AcceRefineActivatedCell[bSlotIndex] = NPOS;
		}

		ZeroMemory(&rInstance, sizeof(TItemData));
	}
}

BYTE CPythonPlayer::FindMoveAcceItemSlot()
{
	for (BYTE bSlotIndex = 0; bSlotIndex < static_cast<BYTE>(m_AcceItemInstanceVector.size()); ++bSlotIndex)
	{
		if (!m_AcceItemInstanceVector[bSlotIndex].dwVnum)
			return bSlotIndex;
	}

	return ACCE_SLOT_MAX;
}

BYTE CPythonPlayer::GetAcceCurrentItemCount()
{
	BYTE bCount = 0;
	for (BYTE bSlotIndex = 0; bSlotIndex < ACCE_SLOT_MAX; ++bSlotIndex)
	{
		if (m_AcceItemInstanceVector[bSlotIndex].dwVnum)
			++bCount;
	}

	return bCount;
}

BOOL CPythonPlayer::GetAcceItemDataPtr(BYTE bSlotIndex, TItemData** ppInstance)
{
	if (bSlotIndex >= m_AcceItemInstanceVector.size())
	{
		TraceError("CPythonPlayer::GetAcceItemDataPtr(bSlotIndex=%u) - Strange slot index", bSlotIndex);
		return FALSE;
	}

	*ppInstance = &m_AcceItemInstanceVector[bSlotIndex];

	return TRUE;
}

void CPythonPlayer::SetActivatedAcceSlot(BYTE bSlotIndex, TItemPos ItemPos)
{
	if (bSlotIndex >= ACCE_SLOT_MAX)
		return;

	m_AcceRefineActivatedCell[bSlotIndex] = ItemPos;
}

BYTE CPythonPlayer::FindActivatedAcceSlot(TItemPos ItemCell)
{
	for (BYTE bSlotIndex = ACCE_SLOT_LEFT; bSlotIndex < ACCE_SLOT_MAX; ++bSlotIndex)
	{
		if (m_AcceRefineActivatedCell[bSlotIndex] == ItemCell)
			return bSlotIndex;
	}

	return ACCE_SLOT_MAX;
}

TItemPos CPythonPlayer::FindUsingAcceSlot(BYTE bSlotIndex)
{
	if (bSlotIndex >= ACCE_SLOT_MAX)
		return NPOS;

	return m_AcceRefineActivatedCell[bSlotIndex];
}
#endif

#if defined(ENABLE_AURA_COSTUME_SYSTEM)
void CPythonPlayer::__ClearAuraRefineWindow()
{
	for (BYTE bSlotIndex = 0; bSlotIndex < AURA_SLOT_MAX; ++bSlotIndex)
	{
		if (!m_AuraRefineActivatedCell[bSlotIndex].IsNPOS())
		{
#if defined(WJ_ENABLE_PICKUP_ITEM_EFFECT)
			PyCallClassMemberFunc(m_ppyGameWindow, "DeactivateSlot", Py_BuildValue("(ii)", m_AuraRefineActivatedCell[bSlotIndex].cell, UI::ESlotHilight::HILIGHTSLOT_AURA));
#endif
			m_AuraRefineActivatedCell[bSlotIndex] = NPOS;
		}
	}
}

void CPythonPlayer::SetAuraWindowType(BYTE bType)
{
	m_bAuraRefineWindowType = bType;
	m_bAuraRefineWindowOpen = AURA_WINDOW_TYPE_MAX != bType;

	if (!m_bAuraRefineWindowOpen)
		__ClearAuraRefineWindow();
}

bool CPythonPlayer::IsAuraSlotEmpty()
{
	for (BYTE bSlotIndex = 0; bSlotIndex < static_cast<BYTE>(m_AuraItemInstanceVector.size()); ++bSlotIndex)
	{
		if (m_AuraItemInstanceVector[bSlotIndex].dwVnum)
			return false;
	}

	return true;
}

void CPythonPlayer::SetAuraRefineInfo(BYTE bAuraRefineInfoSlot, BYTE bAuraRefineInfoLevel, BYTE bAuraRefineInfoExpPercent)
{
	if (bAuraRefineInfoSlot >= AURA_REFINE_INFO_SLOT_MAX)
		return;

	m_bAuraRefineInfo[bAuraRefineInfoSlot].bAuraRefineInfoLevel = bAuraRefineInfoLevel;
	m_bAuraRefineInfo[bAuraRefineInfoSlot].bAuraRefineInfoExpPercent = bAuraRefineInfoExpPercent;
}

BYTE CPythonPlayer::GetAuraRefineInfoLevel(BYTE bAuraRefineInfoSlot)
{
	if (bAuraRefineInfoSlot >= AURA_REFINE_INFO_SLOT_MAX)
		return 0;

	return m_bAuraRefineInfo[bAuraRefineInfoSlot].bAuraRefineInfoLevel;
}

BYTE CPythonPlayer::GetAuraRefineInfoExpPct(BYTE bAuraRefineInfoSlot)
{
	if (bAuraRefineInfoSlot >= AURA_REFINE_INFO_SLOT_MAX)
		return 0;

	return m_bAuraRefineInfo[bAuraRefineInfoSlot].bAuraRefineInfoExpPercent;
}

void CPythonPlayer::SetAuraItemData(BYTE bSlotIndex, const TItemData& rItemInstance)
{
	if (bSlotIndex >= m_AuraItemInstanceVector.size())
	{
		TraceError("CPythonPlayer::SetAuraItemData(bSlotIndex=%u) - Strange slot index", bSlotIndex);
		return;
	}

	m_AuraItemInstanceVector[bSlotIndex] = rItemInstance;

#if defined(WJ_ENABLE_PICKUP_ITEM_EFFECT)
	if (bSlotIndex != AURA_SLOT_RESULT)
		PyCallClassMemberFunc(m_ppyGameWindow, "ActivateSlot", Py_BuildValue("(ii)", m_AuraRefineActivatedCell[bSlotIndex].cell, UI::ESlotHilight::HILIGHTSLOT_AURA));
#endif
}

void CPythonPlayer::DelAuraItemData(BYTE bSlotIndex)
{
	if (bSlotIndex >= AURA_SLOT_MAX || bSlotIndex >= static_cast<BYTE>(m_AuraItemInstanceVector.size()))
	{
		TraceError("CPythonPlayer::DelAuraItemData(bSlotIndex=%u) - Strange slot index", bSlotIndex);
		return;
	}

	TItemData& rInstance = m_AuraItemInstanceVector[bSlotIndex];
	CItemData* pItemData = nullptr;
	if (CItemManager::Instance().GetItemDataPointer(rInstance.dwVnum, &pItemData))
	{
		if (bSlotIndex == AURA_SLOT_MAIN || bSlotIndex == AURA_SLOT_SUB)
			DelAuraItemData(AURA_SLOT_RESULT);

		if (bSlotIndex != AURA_SLOT_RESULT)
		{
#if defined(WJ_ENABLE_PICKUP_ITEM_EFFECT)
			PyCallClassMemberFunc(m_ppyGameWindow, "DeactivateSlot", Py_BuildValue("(ii)", m_AuraRefineActivatedCell[bSlotIndex].cell, UI::ESlotHilight::HILIGHTSLOT_AURA));
#endif
			m_AuraRefineActivatedCell[bSlotIndex] = NPOS;
		}

		ZeroMemory(&rInstance, sizeof(TItemData));
	}
}

BYTE CPythonPlayer::FindMoveAuraItemSlot()
{
	for (BYTE bSlotIndex = 0; bSlotIndex < static_cast<BYTE>(m_AuraItemInstanceVector.size()); ++bSlotIndex)
	{
		if (!m_AuraItemInstanceVector[bSlotIndex].dwVnum)
			return bSlotIndex;
	}

	return AURA_SLOT_MAX;
}

BYTE CPythonPlayer::GetAuraCurrentItemSlotCount()
{
	BYTE bCount = 0;
	for (BYTE bSlotIndex = 0; bSlotIndex < AURA_SLOT_MAX; ++bSlotIndex)
	{
		if (m_AuraItemInstanceVector[bSlotIndex].dwVnum)
			++bCount;
	}

	return bCount;
}

BOOL CPythonPlayer::GetAuraItemDataPtr(BYTE bSlotIndex, TItemData** ppInstance)
{
	if (bSlotIndex >= m_AuraItemInstanceVector.size())
	{
		TraceError("CPythonPlayer::GetAuraItemDataPtr(bSlotIndex=%u) - Strange slot index", bSlotIndex);
		return FALSE;
	}

	*ppInstance = &m_AuraItemInstanceVector[bSlotIndex];

	return TRUE;
}

void CPythonPlayer::SetActivatedAuraSlot(BYTE bSlotIndex, TItemPos ItemPos)
{
	if (bSlotIndex >= AURA_SLOT_MAX)
		return;

	m_AuraRefineActivatedCell[bSlotIndex] = ItemPos;
}

BYTE CPythonPlayer::FindActivatedAuraSlot(TItemPos ItemCell)
{
	for (BYTE bSlotIndex = AURA_SLOT_MAIN; bSlotIndex < AURA_SLOT_MAX; ++bSlotIndex)
	{
		if (m_AuraRefineActivatedCell[bSlotIndex] == ItemCell)
			return bSlotIndex;
	}

	return AURA_SLOT_MAX;
}

TItemPos CPythonPlayer::FindUsingAuraSlot(BYTE bSlotIndex)
{
	if (bSlotIndex >= AURA_SLOT_MAX)
		return NPOS;

	return m_AuraRefineActivatedCell[bSlotIndex];
}
#endif

#if defined(ENABLE_CHANGE_LOOK_SYSTEM)
void CPythonPlayer::SetItemTransmutationVnum(TItemPos& rItemPos, const DWORD c_dwVnum)
{
	if (!rItemPos.IsValidCell())
		return;

	const TItemData* c_pItemData = GetItemData(rItemPos);
	if (c_pItemData == nullptr)
		return;

	const_cast<TItemData*>(c_pItemData)->dwTransmutationVnum = c_dwVnum;
	PyCallClassMemberFunc(m_ppyGameWindow, "RefreshInventory", Py_BuildValue("()"));
}

DWORD CPythonPlayer::GetItemChangeLookVnum(TItemPos& rItemPos) const
{
	if (!rItemPos.IsValidCell())
		return 0;

	const TItemData* c_pItemData = GetItemData(rItemPos);
	return c_pItemData ? c_pItemData->dwTransmutationVnum : 0;
}

void CPythonPlayer::ClearChangeLook(const bool c_bClearEffect)
{
	for (BYTE bSlotIndex = 0; bSlotIndex < static_cast<BYTE>(EChangeLookSlots::CHANGE_LOOK_SLOT_MAX); bSlotIndex++)
		DelChangeLookItemData(bSlotIndex, c_bClearEffect);

	DelChangeLookFreeItemData(c_bClearEffect);
}

void CPythonPlayer::SetChangeLookWindowOpen(const bool c_bOpen)
{
	m_bIsChangeLookWindowOpen = c_bOpen;
	CPythonPlayer::Instance().ClearChangeLook(true);
}

bool CPythonPlayer::GetChangeLookWindowOpen() const
{
	return m_bIsChangeLookWindowOpen;
}

void CPythonPlayer::SetChangeLookWindowType(const BYTE c_bType)
{
	m_bChangeLookWindowType = c_bType;
}

BYTE CPythonPlayer::GetChangeLookWindowType() const
{
	return m_bChangeLookWindowType;
}

void CPythonPlayer::SetChangeLookItemData(const TPacketGCChangeLookSet& rData)
{
	if (rData.bSlotIndex >= static_cast<BYTE>(EChangeLookSlots::CHANGE_LOOK_SLOT_MAX))
		return;

	m_ChangeLookSlot[rData.bSlotIndex] = rData;
#if defined(WJ_ENABLE_PICKUP_ITEM_EFFECT)
	PyCallClassMemberFunc(m_ppyGameWindow, "ActivateSlot", Py_BuildValue("(ii)", rData.wCell, UI::ESlotHilight::HILIGHTSLOT_CHANGE_LOOK));
#endif
}

TPacketGCChangeLookSet* CPythonPlayer::GetChangeLookItemData(const BYTE c_bSlotIndex)
{
	if (c_bSlotIndex >= static_cast<BYTE>(EChangeLookSlots::CHANGE_LOOK_SLOT_MAX))
		return nullptr;

	return &m_ChangeLookSlot[c_bSlotIndex];
}

void CPythonPlayer::DelChangeLookItemData(const BYTE c_bSlotIndex, const bool c_bClearEffect)
{
	if (c_bSlotIndex >= static_cast<BYTE>(EChangeLookSlots::CHANGE_LOOK_SLOT_MAX))
		return;

	TPacketGCChangeLookSet& rData = m_ChangeLookSlot[c_bSlotIndex];

#if defined(WJ_ENABLE_PICKUP_ITEM_EFFECT)
	if (c_bClearEffect && rData.wCell < c_Inventory_Slot_Count)
		PyCallClassMemberFunc(m_ppyGameWindow, "DeactivateSlot", Py_BuildValue("(ii)", rData.wCell, UI::ESlotHilight::HILIGHTSLOT_CHANGE_LOOK));
#endif

	ZeroMemory(&rData, sizeof(rData));
	rData.wCell = c_Inventory_Slot_Count;
}

void CPythonPlayer::SetChangeLookFreeItemData(const TPacketGCChangeLookSet& rData)
{
	m_ChangeLookFreeYangItemSlot = rData;
#if defined(WJ_ENABLE_PICKUP_ITEM_EFFECT)
	PyCallClassMemberFunc(m_ppyGameWindow, "ActivateSlot", Py_BuildValue("(ii)", rData.wCell, UI::ESlotHilight::HILIGHTSLOT_CHANGE_LOOK));
#endif
}

TPacketGCChangeLookSet* CPythonPlayer::GetChangeLookFreeItemData()
{
	return &m_ChangeLookFreeYangItemSlot;
}

void CPythonPlayer::DelChangeLookFreeItemData(const bool c_bClearEffect)
{
#if defined(WJ_ENABLE_PICKUP_ITEM_EFFECT)
	if (c_bClearEffect && m_ChangeLookFreeYangItemSlot.wCell < c_Inventory_Slot_Count)
		PyCallClassMemberFunc(m_ppyGameWindow, "DeactivateSlot", Py_BuildValue("(ii)", m_ChangeLookFreeYangItemSlot.wCell, UI::ESlotHilight::HILIGHTSLOT_CHANGE_LOOK));
#endif

	ZeroMemory(&m_ChangeLookFreeYangItemSlot, sizeof(m_ChangeLookFreeYangItemSlot));
	m_ChangeLookFreeYangItemSlot.wCell = c_Inventory_Slot_Count;
}
#endif

#if defined(ENABLE_SET_ITEM)
void CPythonPlayer::SetItemSetValue(TItemPos& rItemPos, const BYTE bSetValue)
{
	if (!rItemPos.IsValidCell())
		return;

	const TItemData* c_pItemData = GetItemData(rItemPos);
	if (c_pItemData == nullptr)
		return;

	const_cast<TItemData*>(c_pItemData)->bSetValue = bSetValue;
	PyCallClassMemberFunc(m_ppyGameWindow, "RefreshInventory", Py_BuildValue("()"));
}

BYTE CPythonPlayer::GetItemSetValue(TItemPos& rItemPos) const
{
	if (!rItemPos.IsValidCell())
		return 0;

	const TItemData* c_pItemData = GetItemData(rItemPos);
	return c_pItemData ? c_pItemData->bSetValue : 0;
}
#endif

#if defined(ENABLE_REFINE_ELEMENT_SYSTEM)
void CPythonPlayer::SetItemRefineElement(TItemPos& rItemPos, const TPlayerItemRefineElement& c_rkTable)
{
	if (!rItemPos.IsValidCell())
		return;

	const TItemData* c_pItemData = GetItemData(rItemPos);
	if (c_pItemData == nullptr)
		return;

	const_cast<TItemData*>(c_pItemData)->RefineElement = c_rkTable;
}

const TPlayerItemRefineElement* CPythonPlayer::GetItemRefineElement(TItemPos& rItemPos) const
{
	if (!rItemPos.IsValidCell())
		return 0;

	const TItemData* pItemData = GetItemData(rItemPos);
	if (pItemData == nullptr)
		return nullptr;

	return &pItemData->RefineElement;
}
#endif

#if defined(ENABLE_MOVE_COSTUME_ATTR)
void CPythonPlayer::SetItemCombinationWindowActivedItemSlot(const BYTE bSlotType, const int iSlotIndex)
{
	m_MoveCostumeAttrArr[bSlotType] = iSlotIndex;
}

short CPythonPlayer::GetInvenSlotAttachedToConbWindowSlot(const BYTE bSlotType) const
{
	return m_MoveCostumeAttrArr[bSlotType];
}

CItemData* CPythonPlayer::GetConbMediumItemData() const
{
	const short sMediumItemIndex = m_MoveCostumeAttrArr[ECombSlotType::COMB_WND_SLOT_MEDIUM];
	if (sMediumItemIndex == -1)
		return nullptr;

	const TItemData* pItemData = CPythonPlayer::Instance().GetItemData(TItemPos(INVENTORY, sMediumItemIndex));
	if (pItemData == nullptr)
		return nullptr;

	CItemData* pItemDataPtr = nullptr;
	if (CItemManager::Instance().GetItemDataPointer(pItemData->dwVnum, &pItemDataPtr) == false)
		return nullptr;

	return pItemDataPtr;
}
#endif

#if defined(ENABLE_GEM_SYSTEM)
void CPythonPlayer::GemShop(TGemShopTable& rTable)
{
	m_rGemShopTable = rTable;
}

void CPythonPlayer::ClearGemShop()
{
	ZeroMemory(&m_rGemShopTable, sizeof(m_rGemShopTable));
}

const TGemShopItem& CPythonPlayer::GetGemShopItem(BYTE bSlotIndex)
{
	return m_rGemShopTable.GemShopItem[bSlotIndex];
}

long CPythonPlayer::GetGemShopRefreshTime() const
{
	return m_rGemShopTable.lRefreshTime;
}

BYTE CPythonPlayer::GetGemShopOpenSlotCount() const
{
	return m_rGemShopTable.bEnabledSlots;
}
#endif

#if defined(ENABLE_EXPRESSING_EMOTION)
void CPythonPlayer::ClearSpecialActionSlot(BYTE bSlotIndex)
{
	TSpecialActionMap::iterator it = m_map_kSpecialAction.find(bSlotIndex);
	if (it != m_map_kSpecialAction.end())
		m_map_kSpecialAction.erase(it);
}

void CPythonPlayer::SetSpecialActionSlot(BYTE bSlotIndex, BYTE bEmotionIndex, DWORD dwEmotionDuration)
{
	if (bSlotIndex < SPECIAL_ACTION_START_INDEX)
		return;

	m_map_kSpecialAction[bSlotIndex] = std::make_pair(bEmotionIndex, dwEmotionDuration);
}

INT CPythonPlayer::GetSpecialActionSlot(BYTE bSlotIndex) const
{
	TSpecialActionMap::const_iterator it = m_map_kSpecialAction.find(bSlotIndex);
	return (it != m_map_kSpecialAction.end() ? it->second.first : 0);
}

void CPythonPlayer::FindSpecialActionSlot(BYTE bEmotionIndex, BYTE* pbSlotIndex, DWORD* pdwEmotionDuration) const
{
	*pbSlotIndex = 0;
	*pdwEmotionDuration = 0;

	for (const TSpecialActionMap::value_type& it : m_map_kSpecialAction)
	{
		if (it.second.first == bEmotionIndex)
		{
			*pbSlotIndex = it.first;
			*pdwEmotionDuration = it.second.second;
		}
	}
}
#endif

#if defined(ENABLE_MINI_GAME_YUTNORI)
void CPythonPlayer::YutnoriNotifyMotionDone()
{
	PyCallClassMemberFunc(m_ppyGameWindow, "YutnoriProcess", Py_BuildValue("(ii)", 10, 0));
}
#endif

#if defined(ENABLE_LEFT_SEAT)
void CPythonPlayer::LoadLeftSeatData()
{
	PyCallClassMemberFunc(m_ppyGameWindow, "LoadLeftSeatWaitTimeIndexData", Py_BuildValue("()"));
}
#endif

CPythonPlayer::CPythonPlayer(void)
{
	SetMovableGroundDistance(40.0f);

	// AffectIndex To SkillIndex
	m_kMap_dwAffectIndexToSkillIndex.insert(std::make_pair(int(CInstanceBase::AFFECT_JEONGWI), 3));
	m_kMap_dwAffectIndexToSkillIndex.insert(std::make_pair(int(CInstanceBase::AFFECT_GEOMGYEONG), 4));
	m_kMap_dwAffectIndexToSkillIndex.insert(std::make_pair(int(CInstanceBase::AFFECT_CHEONGEUN), 19));
	m_kMap_dwAffectIndexToSkillIndex.insert(std::make_pair(int(CInstanceBase::AFFECT_GYEONGGONG), 49));
	m_kMap_dwAffectIndexToSkillIndex.insert(std::make_pair(int(CInstanceBase::AFFECT_EUNHYEONG), 34));
	m_kMap_dwAffectIndexToSkillIndex.insert(std::make_pair(int(CInstanceBase::AFFECT_GONGPO), 64));
	m_kMap_dwAffectIndexToSkillIndex.insert(std::make_pair(int(CInstanceBase::AFFECT_JUMAGAP), 65));
	m_kMap_dwAffectIndexToSkillIndex.insert(std::make_pair(int(CInstanceBase::AFFECT_HOSIN), 94));
	m_kMap_dwAffectIndexToSkillIndex.insert(std::make_pair(int(CInstanceBase::AFFECT_BOHO), 95));
	m_kMap_dwAffectIndexToSkillIndex.insert(std::make_pair(int(CInstanceBase::AFFECT_KWAESOK), 110));
	m_kMap_dwAffectIndexToSkillIndex.insert(std::make_pair(int(CInstanceBase::AFFECT_GICHEON), 96));
	m_kMap_dwAffectIndexToSkillIndex.insert(std::make_pair(int(CInstanceBase::AFFECT_JEUNGRYEOK), 111));
	m_kMap_dwAffectIndexToSkillIndex.insert(std::make_pair(int(CInstanceBase::AFFECT_PABEOP), 66));
	m_kMap_dwAffectIndexToSkillIndex.insert(std::make_pair(int(CInstanceBase::AFFECT_FALLEN_CHEONGEUN), 19));
	/////
	m_kMap_dwAffectIndexToSkillIndex.insert(std::make_pair(int(CInstanceBase::AFFECT_GWIGEOM), 63));
	m_kMap_dwAffectIndexToSkillIndex.insert(std::make_pair(int(CInstanceBase::AFFECT_MUYEONG), 78));
	m_kMap_dwAffectIndexToSkillIndex.insert(std::make_pair(int(CInstanceBase::AFFECT_HEUKSIN), 79));

	m_kMap_dwAffectIndexToSkillIndex.insert(std::make_pair(int(CInstanceBase::AFFECT_RED_POSSESSION), 174));
	m_kMap_dwAffectIndexToSkillIndex.insert(std::make_pair(int(CInstanceBase::AFFECT_BLUE_POSSESSION), 175));

	m_ppyGameWindow = NULL;

	m_sysIsCoolTime = TRUE;
	m_sysIsLevelLimit = TRUE;
	m_dwPlayTime = 0;

	m_aeMBFButton[MBT_LEFT] = CPythonPlayer::MBF_SMART;
	m_aeMBFButton[MBT_RIGHT] = CPythonPlayer::MBF_CAMERA;
	m_aeMBFButton[MBT_MIDDLE] = CPythonPlayer::MBF_CAMERA;

	memset(m_adwEffect, 0, sizeof(m_adwEffect));

	m_isDestPosition = FALSE;
	m_ixDestPos = 0;
	m_iyDestPos = 0;
	m_iLastAlarmTime = 0;

	Clear();
}

CPythonPlayer::~CPythonPlayer(void)
{
}
