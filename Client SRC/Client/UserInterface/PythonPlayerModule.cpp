#include "StdAfx.h"
#include "PythonPlayer.h"
#include "PythonApplication.h"
#include "Locale_inc.h"

extern const DWORD c_iSkillIndex_Tongsol = 121;
extern const DWORD c_iSkillIndex_Combo = 122;
extern const DWORD c_iSkillIndex_Fishing = 123;
extern const DWORD c_iSkillIndex_Mining = 124;
extern const DWORD c_iSkillIndex_Making = 125;
extern const DWORD c_iSkillIndex_Language1 = 126;
extern const DWORD c_iSkillIndex_Language2 = 127;
extern const DWORD c_iSkillIndex_Language3 = 128;
extern const DWORD c_iSkillIndex_Polymorph = 129;
extern const DWORD c_iSkillIndex_Riding = 130;
extern const DWORD c_iSkillIndex_Summon = 131;
extern const DWORD c_iSkillIndex_AutoAttack = 132;
#if defined(ENABLE_PARTY_PROFICY)
extern const DWORD c_iSkillIndex_RoleProficiency = 133;
#endif
#if defined(ENABLE_PARTY_INSIGHT)
extern const DWORD c_iSkillIndex_InSight = 134;
#endif
extern const DWORD c_iSkillIndex_Hit = 246;

std::map<int, CGraphicImage*> m_kMap_iEmotionIndex_pkIconImage;

extern int TWOHANDED_WEWAPON_ATT_SPEED_DECREASE_VALUE;

#if defined(ENABLE_NEW_EQUIPMENT_SYSTEM)
class CBeltInventoryHelper
{
public:
	typedef BYTE TGradeUnit;

	static TGradeUnit GetBeltGradeByRefineLevel(int refineLevel)
	{
		static TGradeUnit beltGradeByLevelTable[] =
		{
			0, // 벨트+0
			1, // +1
			1, // +2
			2, // +3
			2, // +4,
			3, // +5
			4, // +6,
			5, // +7,
			6, // +8,
			7, // +9
		};

		return beltGradeByLevelTable[refineLevel];
	}

	// 현재 벨트 레벨을 기준으로, 어떤 셀들을 이용할 수 있는지 리턴
	static const TGradeUnit* GetAvailableRuleTableByGrade()
	{
		/**
			벨트는 총 +0 ~ +9 레벨을 가질 수 있으며, 레벨에 따라 7단계 등급으로 구분되어 인벤토리가 활성 화 된다.
			벨트 레벨에 따른 사용 가능한 셀은 아래 그림과 같음. 현재 등급 >= 활성가능 등급이면 사용 가능.
			(단, 현재 레벨이 0이면 무조건 사용 불가, 괄호 안의 숫자는 등급)

				2(1)  4(2)  6(4)  8(6)
				5(3)  5(3)  6(4)  8(6)
				7(5)  7(5)  7(5)  8(6)
				9(7)  9(7)  9(7)  9(7)

			벨트 인벤토리의 크기는 4x4 (16칸)
		*/

		static TGradeUnit availableRuleByGrade[c_Belt_Inventory_Slot_Count] = {
			1, 2, 4, 6,
			3, 3, 4, 6,
			5, 5, 5, 6,
			7, 7, 7, 7
		};

		return availableRuleByGrade;
	}

	static bool IsAvailableCell(WORD cell, int beltGrade /*int beltLevel*/)
	{
		// 기획 또 바뀜.. 아놔...
		//const TGradeUnit beltGrade = GetBeltGradeByRefineLevel(beltLevel);
		const TGradeUnit* ruleTable = GetAvailableRuleTableByGrade();

		return ruleTable[cell] <= beltGrade;
	}

};
#endif

PyObject* playerPickCloseItem(PyObject* poSelf, PyObject* poArgs)
{
	CPythonPlayer::Instance().PickCloseItem();
	return Py_BuildNone();
}

PyObject* playerSetGameWindow(PyObject* poSelf, PyObject* poArgs)
{
	PyObject* pyHandle;
	if (!PyTuple_GetObject(poArgs, 0, &pyHandle))
		return Py_BadArgument();

	CPythonPlayer& rkPlayer = CPythonPlayer::Instance();
	rkPlayer.SetGameWindow(pyHandle);
	return Py_BuildNone();
}

PyObject* playerSetQuickCameraMode(PyObject* poSelf, PyObject* poArgs)
{
	int nIsEnable;
	if (!PyTuple_GetInteger(poArgs, 0, &nIsEnable))
		return Py_BadArgument();

	CPythonPlayer& rkPlayer = CPythonPlayer::Instance();
	rkPlayer.SetQuickCameraMode(nIsEnable ? true : false);

	return Py_BuildNone();
}

// Test Code
PyObject* playerSetMainCharacterIndex(PyObject* poSelf, PyObject* poArgs)
{
	int iVID;
	if (!PyTuple_GetInteger(poArgs, 0, &iVID))
		return Py_BuildException();

	CPythonPlayer::Instance().SetMainCharacterIndex(iVID);
	CPythonCharacterManager::Instance().SetMainInstance(iVID);

	return Py_BuildNone();
}
// Test Code

PyObject* playerGetMainCharacterIndex(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonPlayer::Instance().GetMainCharacterIndex());
}

PyObject* playerGetMainCharacterName(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("s", CPythonPlayer::Instance().GetName());
}

PyObject* playerGetMainCharacterPosition(PyObject* poSelf, PyObject* poArgs)
{
	TPixelPosition kPPosMainActor;
	CPythonPlayer& rkPlayer = CPythonPlayer::Instance();
	rkPlayer.NEW_GetMainActorPosition(&kPPosMainActor);
	return Py_BuildValue("fff", kPPosMainActor.x, kPPosMainActor.y, kPPosMainActor.z);
}

PyObject* playerIsMainCharacterIndex(PyObject* poSelf, PyObject* poArgs)
{
	int iVID;
	if (!PyTuple_GetInteger(poArgs, 0, &iVID))
		return Py_BuildException();

	return Py_BuildValue("i", CPythonPlayer::Instance().IsMainCharacterIndex(iVID));
}

PyObject* playerCanAttackInstance(PyObject* poSelf, PyObject* poArgs)
{
	int iVID;
	if (!PyTuple_GetInteger(poArgs, 0, &iVID))
		return Py_BuildException();

	CInstanceBase* pMainInstance = CPythonPlayer::Instance().NEW_GetMainActorPtr();
	CInstanceBase* pTargetInstance = CPythonCharacterManager::Instance().GetInstancePtr(iVID);
	if (!pMainInstance)
		return Py_BuildValue("i", 0);
	if (!pTargetInstance)
		return Py_BuildValue("i", 0);

	return Py_BuildValue("i", pMainInstance->IsAttackableInstance(*pTargetInstance));
}

PyObject* playerIsActingTargetEmotion(PyObject* poSelf, PyObject* poArgs)
{
	CInstanceBase* pMainInstance = CPythonPlayer::Instance().NEW_GetMainActorPtr();
	if (!pMainInstance)
		return Py_BuildValue("i", 0);

	return Py_BuildValue("i", pMainInstance->IsActingTargetEmotion());
}

PyObject* playerIsActingEmotion(PyObject* poSelf, PyObject* poArgs)
{
	CInstanceBase* pMainInstance = CPythonPlayer::Instance().NEW_GetMainActorPtr();
	if (!pMainInstance)
		return Py_BuildValue("i", 0);

	return Py_BuildValue("i", pMainInstance->IsActingEmotion());
}

PyObject* playerIsPVPInstance(PyObject* poSelf, PyObject* poArgs)
{
	int iVID;
	if (!PyTuple_GetInteger(poArgs, 0, &iVID))
		return Py_BuildException();

	CInstanceBase* pMainInstance = CPythonPlayer::Instance().NEW_GetMainActorPtr();
	CInstanceBase* pTargetInstance = CPythonCharacterManager::Instance().GetInstancePtr(iVID);
	if (!pMainInstance)
		return Py_BuildValue("i", 0);
	if (!pTargetInstance)
		return Py_BuildValue("i", 0);

	return Py_BuildValue("i", pMainInstance->IsPVPInstance(*pTargetInstance));
}

PyObject* playerIsSameEmpire(PyObject* poSelf, PyObject* poArgs)
{
	int iVID;
	if (!PyTuple_GetInteger(poArgs, 0, &iVID))
		return Py_BuildException();

	CInstanceBase* pMainInstance = CPythonPlayer::Instance().NEW_GetMainActorPtr();
	CInstanceBase* pTargetInstance = CPythonCharacterManager::Instance().GetInstancePtr(iVID);
	if (!pMainInstance)
		return Py_BuildValue("i", FALSE);
	if (!pTargetInstance)
		return Py_BuildValue("i", FALSE);

	return Py_BuildValue("i", pMainInstance->IsSameEmpire(*pTargetInstance));
}

PyObject* playerIsChallengeInstance(PyObject* poSelf, PyObject* poArgs)
{
	int iVID;
	if (!PyTuple_GetInteger(poArgs, 0, &iVID))
		return Py_BuildException();

	return Py_BuildValue("i", CPythonPlayer::Instance().IsChallengeInstance(iVID));
}

PyObject* playerIsRevengeInstance(PyObject* poSelf, PyObject* poArgs)
{
	int iVID;
	if (!PyTuple_GetInteger(poArgs, 0, &iVID))
		return Py_BuildException();

	return Py_BuildValue("i", CPythonPlayer::Instance().IsRevengeInstance(iVID));
}

PyObject* playerIsCantFightInstance(PyObject* poSelf, PyObject* poArgs)
{
	int iVID;
	if (!PyTuple_GetInteger(poArgs, 0, &iVID))
		return Py_BuildException();

	return Py_BuildValue("i", CPythonPlayer::Instance().IsCantFightInstance(iVID));
}

PyObject* playerGetCharacterDistance(PyObject* poSelf, PyObject* poArgs)
{
	int iVID;
	if (!PyTuple_GetInteger(poArgs, 0, &iVID))
		return Py_BuildException();

	CInstanceBase* pMainInstance = CPythonPlayer::Instance().NEW_GetMainActorPtr();
	CInstanceBase* pTargetInstance = CPythonCharacterManager::Instance().GetInstancePtr(iVID);
	if (!pMainInstance)
		return Py_BuildValue("f", -1.0f);
	if (!pTargetInstance)
		return Py_BuildValue("f", -1.0f);

	return Py_BuildValue("f", pMainInstance->GetDistance(pTargetInstance));
}

PyObject* playerIsInSafeArea(PyObject* poSelf, PyObject* poArgs)
{
	CInstanceBase* pMainInstance = CPythonPlayer::Instance().NEW_GetMainActorPtr();
	if (!pMainInstance)
		return Py_BuildValue("i", FALSE);

	return Py_BuildValue("i", pMainInstance->IsInSafe());
}

PyObject* playerIsMountingHorse(PyObject* poSelf, PyObject* poArgs)
{
	CInstanceBase* pMainInstance = CPythonPlayer::Instance().NEW_GetMainActorPtr();
	if (!pMainInstance)
		return Py_BuildValue("i", FALSE);

	return Py_BuildValue("i", pMainInstance->IsMountingHorse());
}

PyObject* playerIsObserverMode(PyObject* poSelf, PyObject* poArgs)
{
	CPythonPlayer& rkPlayer = CPythonPlayer::Instance();
	return Py_BuildValue("i", rkPlayer.IsObserverMode());
}

PyObject* playerActEmotion(PyObject* poSelf, PyObject* poArgs)
{
	int iVID;
	if (!PyTuple_GetInteger(poArgs, 0, &iVID))
		return Py_BuildException();

	CPythonPlayer& rkPlayer = CPythonPlayer::Instance();
	rkPlayer.ActEmotion(iVID);
	return Py_BuildNone();
}

PyObject* playerShowPlayer(PyObject* poSelf, PyObject* poArgs)
{
	CInstanceBase* pMainInstance = CPythonPlayer::Instance().NEW_GetMainActorPtr();
	if (pMainInstance)
		pMainInstance->GetGraphicThingInstanceRef().Show();
	return Py_BuildNone();
}

PyObject* playerHidePlayer(PyObject* poSelf, PyObject* poArgs)
{
	CInstanceBase* pMainInstance = CPythonPlayer::Instance().NEW_GetMainActorPtr();
	if (pMainInstance)
		pMainInstance->GetGraphicThingInstanceRef().Hide();
	return Py_BuildNone();
}

PyObject* playerComboAttack(PyObject* poSelf, PyObject* poArgs)
{
	CPythonPlayer::Instance().NEW_Attack();
	return Py_BuildNone();
}

PyObject* playerRegisterEffect(PyObject* poSelf, PyObject* poArgs)
{
	int iEft;
	if (!PyTuple_GetInteger(poArgs, 0, &iEft))
		return Py_BadArgument();

	char* szFileName;
	if (!PyTuple_GetString(poArgs, 1, &szFileName))
		return Py_BadArgument();

	CPythonPlayer& rkPlayer = CPythonPlayer::Instance();
	if (!rkPlayer.RegisterEffect(iEft, szFileName, false))
		return Py_BuildException("CPythonPlayer::RegisterEffect(eEft=%d, szFileName=%s", iEft, szFileName);

	return Py_BuildNone();
}

PyObject* playerRegisterCacheEffect(PyObject* poSelf, PyObject* poArgs)
{
	int iEft;
	if (!PyTuple_GetInteger(poArgs, 0, &iEft))
		return Py_BadArgument();

	char* szFileName;
	if (!PyTuple_GetString(poArgs, 1, &szFileName))
		return Py_BadArgument();

	CPythonPlayer& rkPlayer = CPythonPlayer::Instance();
	if (!rkPlayer.RegisterEffect(iEft, szFileName, true))
		return Py_BuildException("CPythonPlayer::RegisterEffect(eEft=%d, szFileName=%s", iEft, szFileName);

	return Py_BuildNone();
}

PyObject* playerSetAttackKeyState(PyObject* poSelf, PyObject* poArgs)
{
	int isPressed;
	if (!PyTuple_GetInteger(poArgs, 0, &isPressed))
		return Py_BuildException("playerSetAttackKeyState(isPressed) - There is no first argument");

	CPythonPlayer& rkPlayer = CPythonPlayer::Instance();
	rkPlayer.SetAttackKeyState(isPressed ? true : false);
	return Py_BuildNone();
}

PyObject* playerSetSingleDIKKeyState(PyObject* poSelf, PyObject* poArgs)
{
	int eDIK;
	if (!PyTuple_GetInteger(poArgs, 0, &eDIK))
		return Py_BuildException("playerSetSingleDIKKeyState(eDIK, isPressed) - There is no first argument");

	int isPressed;
	if (!PyTuple_GetInteger(poArgs, 1, &isPressed))
		return Py_BuildException("playerSetSingleDIKKeyState(eDIK, isPressed) - There is no second argument");

	CPythonPlayer& rkPlayer = CPythonPlayer::Instance();
	rkPlayer.NEW_SetSingleDIKKeyState(eDIK, isPressed ? true : false);
	return Py_BuildNone();
}

PyObject* playerEndKeyWalkingImmediately(PyObject* poSelf, PyObject* poArgs)
{
	CPythonPlayer::Instance().NEW_Stop();
	return Py_BuildNone();
}

PyObject* playerStartMouseWalking(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildNone();
}

PyObject* playerEndMouseWalking(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildNone();
}

PyObject* playerResetCameraRotation(PyObject* poSelf, PyObject* poArgs)
{
	CPythonPlayer::Instance().NEW_ResetCameraRotation();
	return Py_BuildNone();
}

PyObject* playerSetAutoCameraRotationSpeed(PyObject* poSelf, PyObject* poArgs)
{
	float fCmrRotSpd;
	if (!PyTuple_GetFloat(poArgs, 0, &fCmrRotSpd))
		return Py_BuildException();

	CPythonPlayer& rkPlayer = CPythonPlayer::Instance();
	rkPlayer.NEW_SetAutoCameraRotationSpeed(fCmrRotSpd);
	return Py_BuildNone();
}

PyObject* playerSetMouseState(PyObject* poSelf, PyObject* poArgs)
{
	int eMBT;
	if (!PyTuple_GetInteger(poArgs, 0, &eMBT))
		return Py_BuildException();

	int eMBS;
	if (!PyTuple_GetInteger(poArgs, 1, &eMBS))
		return Py_BuildException();

	CPythonPlayer& rkPlayer = CPythonPlayer::Instance();
	rkPlayer.NEW_SetMouseState(eMBT, eMBS);

	return Py_BuildNone();
}

PyObject* playerSetMouseFunc(PyObject* poSelf, PyObject* poArgs)
{
	int eMBT;
	if (!PyTuple_GetInteger(poArgs, 0, &eMBT))
		return Py_BuildException();

	int eMBS;
	if (!PyTuple_GetInteger(poArgs, 1, &eMBS))
		return Py_BuildException();

	CPythonPlayer& rkPlayer = CPythonPlayer::Instance();
	rkPlayer.NEW_SetMouseFunc(eMBT, eMBS);

	return Py_BuildNone();
}

PyObject* playerGetMouseFunc(PyObject* poSelf, PyObject* poArgs)
{
	int eMBT;
	if (!PyTuple_GetInteger(poArgs, 0, &eMBT))
		return Py_BuildException();

	CPythonPlayer& rkPlayer = CPythonPlayer::Instance();
	return Py_BuildValue("i", rkPlayer.NEW_GetMouseFunc(eMBT));
}

PyObject* playerSetMouseMiddleButtonState(PyObject* poSelf, PyObject* poArgs)
{
	int eMBS;
	if (!PyTuple_GetInteger(poArgs, 0, &eMBS))
		return Py_BuildException();

	CPythonPlayer& rkPlayer = CPythonPlayer::Instance();
	rkPlayer.NEW_SetMouseMiddleButtonState(eMBS);

	return Py_BuildNone();
}

///////////////////////////////////////////////////////////////////////////////////////////////////

PyObject* playerGetName(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("s", CPythonPlayer::Instance().GetName());
}

PyObject* playerGetRace(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonPlayer::Instance().GetRace());
}

PyObject* playerGetJob(PyObject* poSelf, PyObject* poArgs)
{
	int race = CPythonPlayer::Instance().GetRace();
	int job = RaceToJob(race);
	return Py_BuildValue("i", job);
}

PyObject* playerGetPlayTime(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonPlayer::Instance().GetPlayTime());
}

PyObject* playerSetPlayTime(PyObject* poSelf, PyObject* poArgs)
{
	int iTime;
	if (!PyTuple_GetInteger(poArgs, 0, &iTime))
		return Py_BuildException();

	CPythonPlayer::Instance().SetPlayTime(iTime);
	return Py_BuildNone();
}

PyObject* playerIsSkillCoolTime(PyObject* poSelf, PyObject* poArgs)
{
	int iSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iSlotIndex))
		return Py_BuildException();

	return Py_BuildValue("i", CPythonPlayer::Instance().IsSkillCoolTime(iSlotIndex));
}

PyObject* playerGetSkillCoolTime(PyObject* poSelf, PyObject* poArgs)
{
	int iSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iSlotIndex))
		return Py_BuildException();

	float fCoolTime = CPythonPlayer::Instance().GetSkillCoolTime(iSlotIndex);
	float fElapsedCoolTime = CPythonPlayer::Instance().GetSkillElapsedCoolTime(iSlotIndex);
	return Py_BuildValue("ff", fCoolTime, fElapsedCoolTime);
}

PyObject* playerIsSkillActive(PyObject* poSelf, PyObject* poArgs)
{
	int iSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iSlotIndex))
		return Py_BuildException();

	return Py_BuildValue("i", CPythonPlayer::Instance().IsSkillActive(iSlotIndex));
}

PyObject* playerUseGuildSkill(PyObject* poSelf, PyObject* poArgs)
{
	int iSkillSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iSkillSlotIndex))
		return Py_BuildException();

	CPythonPlayer::Instance().UseGuildSkill(iSkillSlotIndex);
	return Py_BuildNone();
}

PyObject* playerAffectIndexToSkillIndex(PyObject* poSelf, PyObject* poArgs)
{
	int iAffectIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iAffectIndex))
		return Py_BuildException();

	DWORD dwSkillIndex;
	if (!CPythonPlayer::Instance().AffectIndexToSkillIndex(iAffectIndex, &dwSkillIndex))
		return Py_BuildValue("i", 0);

	return Py_BuildValue("i", dwSkillIndex);
}

PyObject* playerGetEXP(PyObject* poSelf, PyObject* poArgs)
{
	DWORD dwEXP = CPythonPlayer::Instance().GetStatus(POINT_EXP);
	return Py_BuildValue("l", dwEXP);
}

PyObject* playerGetStatus(PyObject* poSelf, PyObject* poArgs)
{
	int iType;
	if (!PyTuple_GetInteger(poArgs, 0, &iType))
		return Py_BuildException();

	long iValue = CPythonPlayer::Instance().GetStatus(iType);

	if (POINT_ATT_SPEED == iType)
	{
		CInstanceBase* pInstance = CPythonPlayer::Instance().NEW_GetMainActorPtr();
		if (pInstance && (CItemData::WEAPON_TWO_HANDED == pInstance->GetWeaponType()))
		{
			iValue -= TWOHANDED_WEWAPON_ATT_SPEED_DECREASE_VALUE;
		}
	}

	return Py_BuildValue("i", iValue);
}

PyObject* playerSetStatus(PyObject* poSelf, PyObject* poArgs)
{
	int iType;
	if (!PyTuple_GetInteger(poArgs, 0, &iType))
		return Py_BuildException();

	int iValue;
	if (!PyTuple_GetInteger(poArgs, 1, &iValue))
		return Py_BuildException();

	CPythonPlayer::Instance().SetStatus(iType, iValue);
	return Py_BuildNone();
}

PyObject* playerGetElk(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonPlayer::Instance().GetStatus(POINT_GOLD));
}

#if defined(ENABLE_CHEQUE_SYSTEM)
PyObject* playerGetCheque(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonPlayer::Instance().GetStatus(POINT_CHEQUE));
}
#endif

#if defined(ENABLE_GEM_SYSTEM)
PyObject* playerGetGem(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonPlayer::Instance().GetStatus(POINT_GEM));
}
#endif

PyObject* playerGetGuildID(PyObject* poSelf, PyObject* poArgs)
{
	CInstanceBase* pInstance = CPythonPlayer::Instance().NEW_GetMainActorPtr();
	if (!pInstance)
		return Py_BuildValue("i", 0);
	return Py_BuildValue("i", pInstance->GetGuildID());
}

PyObject* playerGetGuildName(PyObject* poSelf, PyObject* poArgs)
{
	CInstanceBase* pInstance = CPythonPlayer::Instance().NEW_GetMainActorPtr();
	if (pInstance)
	{
		DWORD dwID = pInstance->GetGuildID();
		std::string strName;
		if (CPythonGuild::Instance().GetGuildName(dwID, &strName))
			return Py_BuildValue("s", strName.c_str());
	}
	return Py_BuildValue("s", "");
}

PyObject* playerGetAlignmentData(PyObject* poSelf, PyObject* poArgs)
{
	CInstanceBase* pInstance = CPythonPlayer::Instance().NEW_GetMainActorPtr();
	int iAlignmentPoint = 0;
	int iAlignmentGrade = 4;
	if (pInstance)
	{
		iAlignmentPoint = pInstance->GetAlignment();
		iAlignmentGrade = pInstance->GetAlignmentGrade();
	}
	return Py_BuildValue("ii", iAlignmentPoint, iAlignmentGrade);
}

PyObject* playerSetSkill(PyObject* poSelf, PyObject* poArgs)
{
	int iSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iSlotIndex))
		return Py_BuildException();

	int iSkillIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &iSkillIndex))
		return Py_BuildException();

	CPythonPlayer::Instance().SetSkill(iSlotIndex, iSkillIndex);
	return Py_BuildNone();
}

PyObject* playerGetSkillIndex(PyObject* poSelf, PyObject* poArgs)
{
	int iSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iSlotIndex))
		return Py_BuildException();

	return Py_BuildValue("i", CPythonPlayer::Instance().GetSkillIndex(iSlotIndex));
}

PyObject* playerGetSkillSlotIndex(PyObject* poSelf, PyObject* poArgs)
{
	int iSkillIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iSkillIndex))
		return Py_BuildException();

	DWORD dwSlotIndex;
	if (!CPythonPlayer::Instance().GetSkillSlotIndex(iSkillIndex, &dwSlotIndex))
		return Py_BuildException();

	return Py_BuildValue("i", dwSlotIndex);
}

PyObject* playerGetSkillGrade(PyObject* poSelf, PyObject* poArgs)
{
	int iSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iSlotIndex))
		return Py_BuildException();

	return Py_BuildValue("i", CPythonPlayer::Instance().GetSkillGrade(iSlotIndex));
}

PyObject* playerGetSkillLevel(PyObject* poSelf, PyObject* poArgs)
{
	int iSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iSlotIndex))
		return Py_BuildException();

	return Py_BuildValue("i", CPythonPlayer::Instance().GetSkillLevel(iSlotIndex));
}

PyObject* playerGetSkillCurrentEfficientPercentage(PyObject* poSelf, PyObject* poArgs)
{
	int iSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iSlotIndex))
		return Py_BuildException();

	return Py_BuildValue("f", CPythonPlayer::Instance().GetSkillCurrentEfficientPercentage(iSlotIndex));
}
PyObject* playerGetSkillNextEfficientPercentage(PyObject* poSelf, PyObject* poArgs)
{
	int iSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iSlotIndex))
		return Py_BuildException();

	return Py_BuildValue("f", CPythonPlayer::Instance().GetSkillNextEfficientPercentage(iSlotIndex));
}

PyObject* playerClickSkillSlot(PyObject* poSelf, PyObject* poArgs)
{
	int iSkillSlot;
	if (!PyTuple_GetInteger(poArgs, 0, &iSkillSlot))
		return Py_BadArgument();

	CPythonPlayer::Instance().ClickSkillSlot(iSkillSlot);

	return Py_BuildNone();
}

PyObject* playerChangeCurrentSkillNumberOnly(PyObject* poSelf, PyObject* poArgs)
{
	int iSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iSlotIndex))
		return Py_BadArgument();

	CPythonPlayer::Instance().ChangeCurrentSkillNumberOnly(iSlotIndex);

	return Py_BuildNone();
}

PyObject* playerClearSkillDict(PyObject* poSelf, PyObject* poArgs)
{
	CPythonPlayer::Instance().ClearSkillDict();
	return Py_BuildNone();
}

PyObject* playerMoveItem(PyObject* poSelf, PyObject* poArgs)
{
	TItemPos srcCell;
	TItemPos dstCell;
	switch (PyTuple_Size(poArgs))
	{
		case 2:
			//int iSourceSlotIndex;
			if (!PyTuple_GetInteger(poArgs, 0, &srcCell.cell))
				return Py_BuildException();
			if (!PyTuple_GetInteger(poArgs, 1, &dstCell.cell))
				return Py_BuildException();
			break;
		case 4:
			if (!PyTuple_GetByte(poArgs, 0, &srcCell.window_type))
				return Py_BuildException();
			if (!PyTuple_GetInteger(poArgs, 1, &srcCell.cell))
				return Py_BuildException();
			if (!PyTuple_GetByte(poArgs, 2, &dstCell.window_type))
				return Py_BuildException();
			if (!PyTuple_GetInteger(poArgs, 3, &dstCell.cell))
				return Py_BuildException();
		default:
			return Py_BuildException();
	}

	CPythonPlayer::Instance().MoveItemData(srcCell, dstCell);
	return Py_BuildNone();
}

PyObject* playerSendClickItemPacket(PyObject* poSelf, PyObject* poArgs)
{
	int ivid;
	if (!PyTuple_GetInteger(poArgs, 0, &ivid))
		return Py_BuildException();

	CPythonPlayer::Instance().SendClickItemPacket(ivid);
	return Py_BuildNone();
}

PyObject* playerGetItemIndex(PyObject* poSelf, PyObject* poArgs)
{
	switch (PyTuple_Size(poArgs))
	{
		case 1:
		{
			int iSlotIndex;
			if (!PyTuple_GetInteger(poArgs, 0, &iSlotIndex))
				return Py_BuildException();

			int ItemIndex = CPythonPlayer::Instance().GetItemIndex(TItemPos(INVENTORY, iSlotIndex));
			return Py_BuildValue("i", ItemIndex);
		}
		case 2:
		{
			TItemPos Cell;
			if (!PyTuple_GetByte(poArgs, 0, &Cell.window_type))
				return Py_BuildException();
			if (!PyTuple_GetInteger(poArgs, 1, &Cell.cell))
				return Py_BuildException();

			int ItemIndex = CPythonPlayer::Instance().GetItemIndex(Cell);
			return Py_BuildValue("i", ItemIndex);
		}
		default:
			return Py_BuildException();

	}
}

PyObject* playerGetItemFlags(PyObject* poSelf, PyObject* poArgs)
{
	switch (PyTuple_Size(poArgs))
	{
		case 1:
		{
			int iSlotIndex;
			if (!PyTuple_GetInteger(poArgs, 0, &iSlotIndex))
				return Py_BuildException();

			DWORD flags = CPythonPlayer::Instance().GetItemFlags(TItemPos(INVENTORY, iSlotIndex));
			return Py_BuildValue("i", flags);
		}
		case 2:
		{
			TItemPos Cell;
			if (!PyTuple_GetByte(poArgs, 0, &Cell.window_type))
				return Py_BuildException();

			if (!PyTuple_GetInteger(poArgs, 1, &Cell.cell))
				return Py_BuildException();

			DWORD flags = CPythonPlayer::Instance().GetItemFlags(Cell);
			return Py_BuildValue("i", flags);
		}
		default:
			return Py_BuildException();
	}
}

PyObject* playerIsAntiFlagBySlot(PyObject* poSelf, PyObject* poArgs)
{
	TItemPos Cell;
	int antiflag;
	switch (PyTuple_Size(poArgs))
	{
		case 2:
			if (!PyTuple_GetInteger(poArgs, 0, &Cell.cell))
				return Py_BadArgument();
			if (!PyTuple_GetInteger(poArgs, 1, &antiflag))
				return Py_BadArgument();
			break;

		case 3:
			if (!PyTuple_GetInteger(poArgs, 0, &Cell.window_type))
				return Py_BadArgument();
			if (!PyTuple_GetInteger(poArgs, 1, &Cell.cell))
				return Py_BadArgument();
			if (!PyTuple_GetInteger(poArgs, 2, &antiflag))
				return Py_BadArgument();
			break;
		default:
			return Py_BuildException();
	}

	DWORD itemAntiFlags = CPythonPlayer::Instance().GetItemAntiFlags(Cell);
	return Py_BuildValue("i", IS_SET(itemAntiFlags, antiflag) != 0 ? TRUE : FALSE);
}

PyObject* playerGetItemTypeBySlot(PyObject* poSelf, PyObject* poArgs)
{
	TItemPos Cell;
	switch (PyTuple_Size(poArgs))
	{
		case 1:
			if (!PyTuple_GetInteger(poArgs, 0, &Cell.cell))
				return Py_BadArgument();
			break;
		case 2:
			if (!PyTuple_GetByte(poArgs, 0, &Cell.window_type))
				return Py_BadArgument();
			if (!PyTuple_GetInteger(poArgs, 1, &Cell.cell))
				return Py_BadArgument();
			break;
		default:
			return Py_BuildException();
	}

	return Py_BuildValue("i", CPythonPlayer::Instance().GetItemTypeBySlot(Cell));
}

PyObject* playerGetItemSubTypeBySlot(PyObject* poSelf, PyObject* poArgs)
{
	TItemPos Cell;
	switch (PyTuple_Size(poArgs))
	{
		case 1:
			if (!PyTuple_GetInteger(poArgs, 0, &Cell.cell))
				return Py_BadArgument();
			break;
		case 2:
			if (!PyTuple_GetByte(poArgs, 0, &Cell.window_type))
				return Py_BadArgument();
			if (!PyTuple_GetInteger(poArgs, 1, &Cell.cell))
				return Py_BadArgument();
			break;
		default:
			return Py_BuildException();
	}

	return Py_BuildValue("i", CPythonPlayer::Instance().GetItemSubTypeBySlot(Cell));
}

PyObject* playerIsSameItemVnum(PyObject* poSelf, PyObject* poArgs)
{
	TItemPos Cell;
	int iItemVNum;
	switch (PyTuple_Size(poArgs))
	{
		case 2:
			if (!PyTuple_GetInteger(poArgs, 0, &iItemVNum))
				return Py_BadArgument();
			if (!PyTuple_GetInteger(poArgs, 1, &Cell.cell))
				return Py_BadArgument();
			break;
		case 3:
			if (!PyTuple_GetInteger(poArgs, 0, &iItemVNum))
				return Py_BadArgument();
			if (!PyTuple_GetInteger(poArgs, 1, &Cell.window_type))
				return Py_BadArgument();
			if (!PyTuple_GetInteger(poArgs, 2, &Cell.cell))
				return Py_BadArgument();
			break;
		default:
			return Py_BuildException();
	}

	return Py_BuildValue("i", (iItemVNum == CPythonPlayer::Instance().GetItemIndex(Cell)) ? TRUE : FALSE);
}

PyObject* playerGetItemCount(PyObject* poSelf, PyObject* poArgs)
{
	switch (PyTuple_Size(poArgs))
	{
		case 1:
		{
			int iSlotIndex;
			if (!PyTuple_GetInteger(poArgs, 0, &iSlotIndex))
				return Py_BuildException();

			int ItemNum = CPythonPlayer::Instance().GetItemCount(TItemPos(INVENTORY, iSlotIndex));
			return Py_BuildValue("i", ItemNum);
		}
		case 2:
		{
			TItemPos Cell;
			if (!PyTuple_GetByte(poArgs, 0, &Cell.window_type))
				return Py_BuildException();

			if (!PyTuple_GetInteger(poArgs, 1, &Cell.cell))
				return Py_BuildException();

			int ItemNum = CPythonPlayer::Instance().GetItemCount(Cell);

			return Py_BuildValue("i", ItemNum);
		}
		default:
			return Py_BuildException();

	}
}

PyObject* playerSetItemCount(PyObject* poSelf, PyObject* poArgs)
{
	switch (PyTuple_Size(poArgs))
	{
		case 2:
		{
			int iSlotIndex;
			if (!PyTuple_GetInteger(poArgs, 0, &iSlotIndex))
				return Py_BuildException();

			INT iCount;
			if (!PyTuple_GetInteger(poArgs, 1, &iCount))
				return Py_BuildException();

			if (0 == iCount)
				return Py_BuildException();

			CPythonPlayer::Instance().SetItemCount(TItemPos(INVENTORY, iSlotIndex), iCount);
			return Py_BuildNone();
		}
		case 3:
		{
			TItemPos Cell;
			if (!PyTuple_GetByte(poArgs, 0, &Cell.window_type))
				return Py_BuildException();

			if (!PyTuple_GetInteger(poArgs, 1, &Cell.cell))
				return Py_BuildException();

			INT iCount;
			if (!PyTuple_GetInteger(poArgs, 2, &iCount))
				return Py_BuildException();

			CPythonPlayer::Instance().SetItemCount(Cell, iCount);

			return Py_BuildNone();
		}
		default:
			return Py_BuildException();

	}
}

PyObject* playerGetItemCountByVnum(PyObject* poSelf, PyObject* poArgs)
{
	int iVnum;
	if (!PyTuple_GetInteger(poArgs, 0, &iVnum))
		return Py_BuildException();

#if defined(ENABLE_CUBE_RENEWAL) && defined(ENABLE_SET_ITEM)
	bool bIgnoreSetValue;
	if (!PyTuple_GetBoolean(poArgs, 1, &bIgnoreSetValue))
		bIgnoreSetValue = false;

	int iCount = CPythonPlayer::Instance().GetItemCountByVnum(iVnum, bIgnoreSetValue);
#else
	int iCount = CPythonPlayer::Instance().GetItemCountByVnum(iVnum);
#endif
	return Py_BuildValue("i", iCount);
}

PyObject* playerGetItemMetinSocket(PyObject* poSelf, PyObject* poArgs)
{
	TItemPos Cell;
	int iMetinSocketIndex;

	switch (PyTuple_Size(poArgs))
	{
		case 2:
			if (!PyTuple_GetInteger(poArgs, 0, &Cell.cell))
				return Py_BuildException();

			if (!PyTuple_GetInteger(poArgs, 1, &iMetinSocketIndex))
				return Py_BuildException();

			break;
		case 3:
			if (!PyTuple_GetByte(poArgs, 0, &Cell.window_type))
				return Py_BuildException();
			if (!PyTuple_GetInteger(poArgs, 1, &Cell.cell))
				return Py_BuildException();
			if (!PyTuple_GetInteger(poArgs, 2, &iMetinSocketIndex))
				return Py_BuildException();

			break;

		default:
			return Py_BuildException();

	}
	int nMetinSocketValue = CPythonPlayer::Instance().GetItemMetinSocket(Cell, iMetinSocketIndex);
	return Py_BuildValue("i", nMetinSocketValue);
}

#if defined(ENABLE_APPLY_RANDOM)
PyObject* playerGetItemApplyRandom(PyObject* poSelf, PyObject* poArgs)
{
	TItemPos Cell;
	int iApplySlotIndex;
	switch (PyTuple_Size(poArgs))
	{
		case 2:
			if (!PyTuple_GetInteger(poArgs, 0, &Cell.cell))
				return Py_BuildException();

			if (!PyTuple_GetInteger(poArgs, 1, &iApplySlotIndex))
				return Py_BuildException();

			break;
		case 3:
			if (!PyTuple_GetByte(poArgs, 0, &Cell.window_type))
				return Py_BuildException();

			if (!PyTuple_GetInteger(poArgs, 1, &Cell.cell))
				return Py_BuildException();

			if (!PyTuple_GetInteger(poArgs, 2, &iApplySlotIndex))
				return Py_BuildException();
			break;
		default:
			return Py_BuildException();
	}

	POINT_TYPE wType; POINT_VALUE lValue;
	CPythonPlayer::Instance().GetItemApplyRandom(Cell, iApplySlotIndex, &wType, &lValue);

	return Py_BuildValue("ii", wType, lValue);
}
#endif

PyObject* playerGetItemAttribute(PyObject* poSelf, PyObject* poArgs)
{
	TItemPos Cell;
	//int iSlotPos;
	int iAttributeSlotIndex;
	switch (PyTuple_Size(poArgs))
	{
		case 2:
			if (!PyTuple_GetInteger(poArgs, 0, &Cell.cell))
				return Py_BuildException();

			if (!PyTuple_GetInteger(poArgs, 1, &iAttributeSlotIndex))
				return Py_BuildException();

			break;
		case 3:
			if (!PyTuple_GetByte(poArgs, 0, &Cell.window_type))
				return Py_BuildException();

			if (!PyTuple_GetInteger(poArgs, 1, &Cell.cell))
				return Py_BuildException();

			if (!PyTuple_GetInteger(poArgs, 2, &iAttributeSlotIndex))
				return Py_BuildException();
			break;
		default:
			return Py_BuildException();
	}

	POINT_TYPE wType; POINT_VALUE lValue;
	CPythonPlayer::Instance().GetItemAttribute(Cell, iAttributeSlotIndex, &wType, &lValue);

	return Py_BuildValue("ii", wType, lValue);
}

PyObject* playerGetItemLink(PyObject* poSelf, PyObject* poArgs)
{
	TItemPos Cell;

	switch (PyTuple_Size(poArgs))
	{
		case 1:
			if (!PyTuple_GetInteger(poArgs, 0, &Cell.cell))
				return Py_BuildException();
			break;
		case 2:
			if (!PyTuple_GetByte(poArgs, 0, &Cell.window_type))
				return Py_BuildException();
			if (!PyTuple_GetInteger(poArgs, 1, &Cell.cell))
				return Py_BuildException();
			break;
		default:
			return Py_BuildException();
	}

	const TItemData* pPlayerItem = CPythonPlayer::Instance().GetItemData(Cell);
	CItemData* pItemData = NULL;
	char buf[1024];

	if (pPlayerItem && CItemManager::Instance().GetItemDataPointer(pPlayerItem->dwVnum, &pItemData))
	{
		char itemlink[256];
		bool isAttr = false;

		int len = snprintf(itemlink, sizeof(itemlink), "item"
			":%x" // HYPER_LINK_ITEM_VNUM
#if defined(ENABLE_SET_ITEM)
			":%d" // HYPER_LINK_ITEM_PRE_SET_VALUE
#endif
			":%x" // HYPER_LINK_ITEM_FLAGS
			":%x" // HYPER_LINK_ITEM_SOCKET0
			":%x" // HYPER_LINK_ITEM_SOCKET1
			":%x" // HYPER_LINK_ITEM_SOCKET2
#if defined(ENABLE_ITEM_SOCKET6)
			":%x" // HYPER_LINK_ITEM_SOCKET3
			":%x" // HYPER_LINK_ITEM_SOCKET4
			":%x" // HYPER_LINK_ITEM_SOCKET5
#endif
			, pPlayerItem->dwVnum
#if defined(ENABLE_SET_ITEM)
			, pPlayerItem->bSetValue
#endif
			, pPlayerItem->dwFlags
			, pPlayerItem->alSockets[0]
			, pPlayerItem->alSockets[1]
			, pPlayerItem->alSockets[2]
#if defined(ENABLE_ITEM_SOCKET6)
			, pPlayerItem->alSockets[3]
			, pPlayerItem->alSockets[4]
			, pPlayerItem->alSockets[5]
#endif
		);

#if defined(ENABLE_CHANGE_LOOK_SYSTEM)
		len += snprintf(itemlink + len, sizeof(itemlink) - len, ":%x", pPlayerItem->dwTransmutationVnum);
#endif

#if defined(ENABLE_REFINE_ELEMENT_SYSTEM)
		len += snprintf(itemlink + len, sizeof(itemlink) - len,
			":%x" // HYPER_LINK_ITEM_REFINE_ELEMENT_APPLY_TYPE
			":%x" // HYPER_LINK_ITEM_REFINE_ELEMENT_GRADE
			":%x" // HYPER_LINK_ITEM_REFINE_ELEMENT_VALUE0
			":%x" // HYPER_LINK_ITEM_REFINE_ELEMENT_VALUE1
			":%x" // HYPER_LINK_ITEM_REFINE_ELEMENT_VALUE2
			":%x" // HYPER_LINK_ITEM_REFINE_ELEMENT_BONUS_VALUE0
			":%x" // HYPER_LINK_ITEM_REFINE_ELEMENT_BONUS_VALUE1
			":%x" // HYPER_LINK_ITEM_REFINE_ELEMENT_BONUS_VALUE2
			, pPlayerItem->RefineElement.wApplyType
			, pPlayerItem->RefineElement.bGrade
			, pPlayerItem->RefineElement.abValue[0], pPlayerItem->RefineElement.abValue[1], pPlayerItem->RefineElement.abValue[2]
			, pPlayerItem->RefineElement.abBonusValue[0], pPlayerItem->RefineElement.abBonusValue[1], pPlayerItem->RefineElement.abBonusValue[2]
		);
#endif

#if defined(ENABLE_APPLY_RANDOM)
		for (const TPlayerItemAttribute& rkRandomAttr : pPlayerItem->aApplyRandom)
		{
			len += snprintf(itemlink + len, sizeof(itemlink) - len, ":%x:%ld",
				rkRandomAttr.wType, rkRandomAttr.lValue);
		}
#endif

		for (const TPlayerItemAttribute& rkAttr : pPlayerItem->aAttr)
		{
			if (rkAttr.wType != 0)
			{
				len += snprintf(itemlink + len, sizeof(itemlink) - len, ":%x:%d",
					rkAttr.wType, rkAttr.lValue);
				isAttr = true;
			}
		}

#if defined(ENABLE_SET_ITEM)
		const char* szItemName = CPythonItem::Instance().GetPreName(pPlayerItem->bSetValue, pItemData->GetName());
#else
		const char* szItemName = pItemData->GetName();
#endif

#if defined(ENABLE_LOCALE_CLIENT)
		if (isAttr)
			// "item:번호:플래그:소켓0:소켓1:소켓2"
			snprintf(buf, sizeof(buf), "|cffffc700|H%s|h[%s]|h|r", itemlink, szItemName);
		else
			snprintf(buf, sizeof(buf), "|cfff1e6c0|H%s|h[%s]|h|r", itemlink, szItemName);
#else
		if (GetDefaultCodePage() == CP_ARABIC)
		{
			if (isAttr)
				// "item:번호:플래그:소켓0:소켓1:소켓2"
				snprintf(buf, sizeof(buf), " |h|r[%s]|cffffc700|H%s|h", szItemName, itemlink);
			else
				snprintf(buf, sizeof(buf), " |h|r[%s]|cfff1e6c0|H%s|h", szItemName, itemlink);
		}
		else
		{
			if (isAttr)
				// "item:번호:플래그:소켓0:소켓1:소켓2"
				snprintf(buf, sizeof(buf), "|cffffc700|H%s|h[%s]|h|r", itemlink, szItemName);
			else
				snprintf(buf, sizeof(buf), "|cfff1e6c0|H%s|h[%s]|h|r", itemlink, szItemName);
		}
#endif
	}
	else
		buf[0] = '\0';

	return Py_BuildValue("s", buf);
}

PyObject* playerGetISellItemPrice(PyObject* poSelf, PyObject* poArgs)
{
	TItemPos Cell;

	switch (PyTuple_Size(poArgs))
	{
		case 1:
			if (!PyTuple_GetInteger(poArgs, 0, &Cell.cell))
				return Py_BuildException();
			break;
		case 2:
			if (!PyTuple_GetByte(poArgs, 0, &Cell.window_type))
				return Py_BuildException();
			if (!PyTuple_GetInteger(poArgs, 1, &Cell.cell))
				return Py_BuildException();
			break;
		default:
			return Py_BuildException();
	}
	CItemData* pItemData;

	if (!CItemManager::Instance().GetItemDataPointer(CPythonPlayer::Instance().GetItemIndex(Cell), &pItemData))
		return Py_BuildValue("i", 0);

	int iPrice;

	if (pItemData->IsFlag(CItemData::ITEM_FLAG_COUNT_PER_1GOLD))
		iPrice = CPythonPlayer::Instance().GetItemCount(Cell) / pItemData->GetISellItemPrice();
	else
		iPrice = pItemData->GetISellItemPrice() * CPythonPlayer::Instance().GetItemCount(Cell);

	iPrice /= 5;
	return Py_BuildValue("i", iPrice);
}

PyObject* playerGetQuickPage(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonPlayer::Instance().GetQuickPage());
}

PyObject* playerSetQuickPage(PyObject* poSelf, PyObject* poArgs)
{
	int iPageIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iPageIndex))
		return Py_BuildException();

	CPythonPlayer::Instance().SetQuickPage(iPageIndex);
	return Py_BuildNone();
}

PyObject* playerLocalQuickSlotIndexToGlobalQuickSlotIndex(PyObject* poSelf, PyObject* poArgs)
{
	int iLocalSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iLocalSlotIndex))
		return Py_BuildException();

	CPythonPlayer& rkPlayer = CPythonPlayer::Instance();
	return Py_BuildValue("i", rkPlayer.LocalQuickSlotIndexToGlobalQuickSlotIndex(iLocalSlotIndex));
}

PyObject* playerGetLocalQuickSlot(PyObject* poSelf, PyObject* poArgs)
{
	int iSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iSlotIndex))
		return Py_BuildException();

	BYTE bWndType;
	WORD wWndItemPos;

	CPythonPlayer& rkPlayer = CPythonPlayer::Instance();
	rkPlayer.GetLocalQuickSlotData(iSlotIndex, &bWndType, &wWndItemPos);

	return Py_BuildValue("ii", bWndType, wWndItemPos);
}

PyObject* playerGetGlobalQuickSlot(PyObject* poSelf, PyObject* poArgs)
{
	int iSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iSlotIndex))
		return Py_BuildException();

	BYTE bWndType;
	WORD wWndItemPos;

	CPythonPlayer& rkPlayer = CPythonPlayer::Instance();
	rkPlayer.GetGlobalQuickSlotData(iSlotIndex, &bWndType, &wWndItemPos);

	return Py_BuildValue("ii", bWndType, wWndItemPos);
}

PyObject* playerRequestAddLocalQuickSlot(PyObject* poSelf, PyObject* poArgs)
{
	int nSlotIndex;
	int nWndType;
	int nWndItemPos;

	if (!PyTuple_GetInteger(poArgs, 0, &nSlotIndex))
		return Py_BuildException();

	if (!PyTuple_GetInteger(poArgs, 1, &nWndType))
		return Py_BuildException();

	if (!PyTuple_GetInteger(poArgs, 2, &nWndItemPos))
		return Py_BuildException();

	CPythonPlayer& rkPlayer = CPythonPlayer::Instance();
	rkPlayer.RequestAddLocalQuickSlot(nSlotIndex, nWndType, nWndItemPos);

	return Py_BuildNone();
}

PyObject* playerRequestAddToEmptyLocalQuickSlot(PyObject* poSelf, PyObject* poArgs)
{
	int nWndType;
	if (!PyTuple_GetInteger(poArgs, 0, &nWndType))
		return Py_BuildException();

	int nWndItemPos;
	if (!PyTuple_GetInteger(poArgs, 1, &nWndItemPos))
		return Py_BuildException();

	CPythonPlayer& rkPlayer = CPythonPlayer::Instance();
	rkPlayer.RequestAddToEmptyLocalQuickSlot(nWndType, nWndItemPos);

	return Py_BuildNone();
}

PyObject* playerRequestDeleteGlobalQuickSlot(PyObject* poSelf, PyObject* poArgs)
{
	int nGlobalSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &nGlobalSlotIndex))
		return Py_BuildException();

	CPythonPlayer& rkPlayer = CPythonPlayer::Instance();
	rkPlayer.RequestDeleteGlobalQuickSlot(nGlobalSlotIndex);
	return Py_BuildNone();
}

PyObject* playerRequestMoveGlobalQuickSlotToLocalQuickSlot(PyObject* poSelf, PyObject* poArgs)
{
	int nGlobalSrcSlotIndex;
	int nLocalDstSlotIndex;

	if (!PyTuple_GetInteger(poArgs, 0, &nGlobalSrcSlotIndex))
		return Py_BuildException();

	if (!PyTuple_GetInteger(poArgs, 1, &nLocalDstSlotIndex))
		return Py_BuildException();

	CPythonPlayer& rkPlayer = CPythonPlayer::Instance();
	rkPlayer.RequestMoveGlobalQuickSlotToLocalQuickSlot(nGlobalSrcSlotIndex, nLocalDstSlotIndex);
	return Py_BuildNone();
}

PyObject* playerRequestUseLocalQuickSlot(PyObject* poSelf, PyObject* poArgs)
{
	int iLocalPosition;
	if (!PyTuple_GetInteger(poArgs, 0, &iLocalPosition))
		return Py_BuildException();

	CPythonPlayer::Instance().RequestUseLocalQuickSlot(iLocalPosition);

	return Py_BuildNone();
}

PyObject* playerRemoveQuickSlotByValue(PyObject* poSelf, PyObject* poArgs)
{
	int iType;
	if (!PyTuple_GetInteger(poArgs, 0, &iType))
		return Py_BuildException();

	int iPosition;
	if (!PyTuple_GetInteger(poArgs, 1, &iPosition))
		return Py_BuildException();

	CPythonPlayer::Instance().RemoveQuickSlotByValue(iType, iPosition);

	return Py_BuildNone();
}

PyObject* playerisItem(PyObject* poSelf, PyObject* poArgs)
{
	int iSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iSlotIndex))
		return Py_BuildException();

	char Flag = CPythonPlayer::Instance().IsItem(TItemPos(INVENTORY, iSlotIndex));

	return Py_BuildValue("i", Flag);
}

#if defined(ENABLE_NEW_EQUIPMENT_SYSTEM)
PyObject* playerIsBeltInventorySlot(PyObject* poSelf, PyObject* poArgs)
{
	int iSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iSlotIndex))
		return Py_BuildException();

	char Flag = CPythonPlayer::Instance().IsBeltInventorySlot(TItemPos(INVENTORY, iSlotIndex));

	return Py_BuildValue("i", Flag);
}
#endif

PyObject* playerIsEquipmentSlot(PyObject* poSelf, PyObject* poArgs)
{
	BYTE bWindowType;
	if (!PyTuple_GetInteger(poArgs, 0, &bWindowType))
		return Py_BuildException();

	int iSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &iSlotIndex))
		return Py_BuildException();

	if (bWindowType != EQUIPMENT || iSlotIndex >= c_Equipment_Slot_Count)
		return Py_BuildValue("i", 0);

	return Py_BuildValue("i", 0);
}

PyObject* playerIsDSEquipmentSlot(PyObject* poSelf, PyObject* poArgs)
{
	BYTE bWindowType;
	if (!PyTuple_GetInteger(poArgs, 0, &bWindowType))
		return Py_BuildException();

	int iSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &iSlotIndex))
		return Py_BuildException();

	if (bWindowType != EQUIPMENT || iSlotIndex < c_Wear_Max
		|| iSlotIndex >= c_Equipment_Slot_Count)
		return Py_BuildValue("i", 0);

	return Py_BuildValue("i", 1);
}

PyObject* playerIsCostumeSlot(PyObject* poSelf, PyObject* poArgs)
{
	int iSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iSlotIndex))
		return Py_BuildException();

#if defined(ENABLE_COSTUME_SYSTEM)
	if (iSlotIndex >= c_Costume_Slot_Start)
		if (iSlotIndex <= c_Costume_Slot_End)
			return Py_BuildValue("i", 1);
#endif

	return Py_BuildValue("i", 0);
}

PyObject* playerIsOpenPrivateShop(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonPlayer::Instance().IsOpenPrivateShop());
}

PyObject* playerIsDead(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonPlayer::Instance().IsDead());
}

PyObject* playerIsPoly(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonPlayer::Instance().IsPoly());
}

PyObject* playerIsOpenSafeBox(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonPlayer::Instance().IsOpenSafeBox());
}

PyObject* playerSetOpenSafeBox(PyObject* poSelf, PyObject* poArgs)
{
	bool bOpen;
	if (!PyTuple_GetBoolean(poArgs, 0, &bOpen))
		return Py_BadArgument();

	CPythonPlayer::Instance().SetOpenSafeBox(bOpen);
	return Py_BuildNone();
}

PyObject* playerIsOpenMall(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonPlayer::Instance().IsOpenMall());
}

PyObject* playerSetOpenMall(PyObject* poSelf, PyObject* poArgs)
{
	bool bOpen;
	if (!PyTuple_GetBoolean(poArgs, 0, &bOpen))
		return Py_BadArgument();

	CPythonPlayer::Instance().SetOpenMall(bOpen);
	return Py_BuildNone();
}

PyObject* playerIsValuableItem(PyObject* poSelf, PyObject* poArgs)
{
	TItemPos SlotIndex;

	switch (PyTuple_Size(poArgs))
	{
		case 1:
			if (!PyTuple_GetInteger(poArgs, 0, &SlotIndex.cell))
				return Py_BuildException();
			break;
		case 2:
			if (!PyTuple_GetInteger(poArgs, 0, &SlotIndex.window_type))
				return Py_BuildException();
			if (!PyTuple_GetInteger(poArgs, 1, &SlotIndex.cell))
				return Py_BuildException();
			break;
		default:
			return Py_BuildException();
	}

	DWORD dwItemIndex = CPythonPlayer::Instance().GetItemIndex(SlotIndex);
	CItemManager::Instance().SelectItemData(dwItemIndex);
	CItemData* pItemData = CItemManager::Instance().GetSelectedItemDataPointer();
	if (!pItemData)
		return Py_BuildException("Can't find item data");

	BOOL hasMetinSocket = false;
	BOOL isHighPrice = false;

	for (int i = 0; i < METIN_SOCKET_COUNT; ++i)
		if (CPythonPlayer::METIN_SOCKET_TYPE_NONE != CPythonPlayer::Instance().GetItemMetinSocket(SlotIndex, i))
			hasMetinSocket = true;

	DWORD dwValue = pItemData->GetISellItemPrice();
	if (dwValue > 5000)
		isHighPrice = true;

	return Py_BuildValue("i", hasMetinSocket || isHighPrice);
}

int GetItemGrade(const char* c_szItemName)
{
	std::string strName = c_szItemName;
	if (strName.empty())
		return 0;

	char chGrade = strName[strName.length() - 1];
	if (chGrade < '0' || chGrade > '9')
		chGrade = '0';

	int iGrade = chGrade - '0';
	return iGrade;
}

PyObject* playerGetItemGrade(PyObject* poSelf, PyObject* poArgs)
{
	TItemPos SlotIndex;
	switch (PyTuple_Size(poArgs))
	{
		case 1:
			if (!PyTuple_GetInteger(poArgs, 0, &SlotIndex.cell))
				return Py_BuildException();
			break;
		case 2:
			if (!PyTuple_GetInteger(poArgs, 0, &SlotIndex.window_type))
				return Py_BuildException();
			if (!PyTuple_GetInteger(poArgs, 1, &SlotIndex.cell))
				return Py_BuildException();
			break;
		default:
			return Py_BuildException();
	}

	int iItemIndex = CPythonPlayer::Instance().GetItemIndex(SlotIndex);
	CItemManager::Instance().SelectItemData(iItemIndex);
	CItemData* pItemData = CItemManager::Instance().GetSelectedItemDataPointer();
	if (!pItemData)
		return Py_BuildException("Can't find item data");

	return Py_BuildValue("i", GetItemGrade(pItemData->GetName()));
}

enum
{
	REFINE_SCROLL_TYPE_MAKE_SOCKET = 1,
	REFINE_SCROLL_TYPE_UP_GRADE = 2,
};

enum
{
	REFINE_CANT,
	REFINE_OK,
	REFINE_ALREADY_MAX_SOCKET_COUNT,
	REFINE_NEED_MORE_GOOD_SCROLL,
	REFINE_CANT_MAKE_SOCKET_ITEM,
	REFINE_NOT_NEXT_GRADE_ITEM,
	REFINE_CANT_REFINE_METIN_TO_EQUIPMENT,
	REFINE_CANT_REFINE_ROD,
#if defined(ENABLE_AURA_COSTUME_SYSTEM)
	REFINE_CANT_REFINE_AURA_ITEM,
#endif
};

PyObject* playerCanRefine(PyObject* poSelf, PyObject* poArgs)
{
	int iScrollItemIndex;
	TItemPos TargetSlotIndex;

	switch (PyTuple_Size(poArgs))
	{
		case 2:
			if (!PyTuple_GetInteger(poArgs, 0, &iScrollItemIndex))
				return Py_BadArgument();
			if (!PyTuple_GetInteger(poArgs, 1, &TargetSlotIndex.cell))
				return Py_BadArgument();
			break;
		case 3:
			if (!PyTuple_GetInteger(poArgs, 0, &iScrollItemIndex))
				return Py_BadArgument();
			if (!PyTuple_GetInteger(poArgs, 1, &TargetSlotIndex.window_type))
				return Py_BadArgument();
			if (!PyTuple_GetInteger(poArgs, 2, &TargetSlotIndex.cell))
				return Py_BadArgument();
			break;
		default:
			return Py_BadArgument();
	}

	if (CPythonPlayer::Instance().IsEquipmentSlot(TargetSlotIndex))
		return Py_BuildValue("i", REFINE_CANT_REFINE_METIN_TO_EQUIPMENT);

	// Scroll
	CItemManager::Instance().SelectItemData(iScrollItemIndex);
	CItemData* pScrollItemData = CItemManager::Instance().GetSelectedItemDataPointer();
	if (!pScrollItemData)
		return Py_BuildValue("i", REFINE_CANT);

	int iScrollType = pScrollItemData->GetType();
	int iScrollSubType = pScrollItemData->GetSubType();

	if (iScrollType != CItemData::ITEM_TYPE_USE)
		return Py_BuildValue("i", REFINE_CANT);

	if (iScrollSubType != CItemData::USE_TUNING)
		return Py_BuildValue("i", REFINE_CANT);

	// Target Item
	int iTargetItemIndex = CPythonPlayer::Instance().GetItemIndex(TargetSlotIndex);
	CItemManager::Instance().SelectItemData(iTargetItemIndex);
	CItemData* pTargetItemData = CItemManager::Instance().GetSelectedItemDataPointer();
	if (!pTargetItemData)
		return Py_BuildValue("i", REFINE_CANT);

	int iTargetType = pTargetItemData->GetType();
	//int iTargetSubType = pTargetItemData->GetSubType();

	if (CItemData::ITEM_TYPE_ROD == iTargetType || CItemData::ITEM_TYPE_PICK == iTargetType)
		return Py_BuildValue("i", REFINE_CANT_REFINE_ROD);

#if defined(ENABLE_STONE_OF_BLESS)
	if ((pScrollItemData->GetValue(0) == 7) && (pTargetItemData->GetLevelLimit() <= 80))
		return Py_BuildValue("i", REFINE_CANT);
#endif

#if defined(ENABLE_SOUL_SYSTEM)
	if ((pScrollItemData->GetValue(0) == 8 || pScrollItemData->GetValue(0) == 9) && (CItemData::ITEM_TYPE_SOUL != iTargetType))
		return Py_BuildValue("i", REFINE_CANT);

	if ((pScrollItemData->GetValue(0) != 8 && pScrollItemData->GetValue(0) != 9) && (CItemData::ITEM_TYPE_SOUL == iTargetType))
		return Py_BuildValue("i", REFINE_CANT);
#endif

#if defined(ENABLE_AURA_COSTUME_SYSTEM)
	if (pTargetItemData->GetType() == CItemData::ITEM_TYPE_COSTUME && pTargetItemData->GetSubType() == CItemData::COSTUME_AURA)
		return Py_BuildValue("i", REFINE_CANT_REFINE_AURA_ITEM);
#endif

	if (pTargetItemData->HasNextGrade())
		return Py_BuildValue("i", REFINE_OK);
	else
		return Py_BuildValue("i", REFINE_NOT_NEXT_GRADE_ITEM);

	return Py_BuildValue("i", REFINE_CANT);
}

enum
{
	ATTACH_METIN_CANT,
	ATTACH_METIN_OK,
	ATTACH_METIN_NOT_MATCHABLE_ITEM,
	ATTACH_METIN_NO_MATCHABLE_SOCKET,
	ATTACH_METIN_NOT_EXIST_GOLD_SOCKET,
	ATTACH_METIN_CANT_ATTACH_TO_EQUIPMENT,
};

PyObject* playerCanAttachMetin(PyObject* poSelf, PyObject* poArgs)
{
	int iMetinItemID;
	TItemPos TargetSlotIndex;

	switch (PyTuple_Size(poArgs))
	{
		case 2:
		{
			if (!PyTuple_GetInteger(poArgs, 0, &iMetinItemID))
				return Py_BuildException();
			if (!PyTuple_GetInteger(poArgs, 1, &TargetSlotIndex.cell))
				return Py_BuildException();
		}
		break;

		case 3:
		{
			if (!PyTuple_GetInteger(poArgs, 0, &iMetinItemID))
				return Py_BuildException();
			if (!PyTuple_GetInteger(poArgs, 1, &TargetSlotIndex.window_type))
				return Py_BuildException();
			if (!PyTuple_GetInteger(poArgs, 2, &TargetSlotIndex.cell))
				return Py_BuildException();
		}
		break;
		default:
			return Py_BuildException();
	}

	if (CPythonPlayer::Instance().IsEquipmentSlot(TargetSlotIndex))
		return Py_BuildValue("i", ATTACH_METIN_CANT_ATTACH_TO_EQUIPMENT);

	CItemData* pMetinItemData;
	if (!CItemManager::Instance().GetItemDataPointer(iMetinItemID, &pMetinItemData))
		return Py_BuildException("can't find item data");

	const DWORD dwTargetItemIndex = CPythonPlayer::Instance().GetItemIndex(TargetSlotIndex);
	CItemData* pTargetItemData;
	if (!CItemManager::Instance().GetItemDataPointer(dwTargetItemIndex, &pTargetItemData))
		return Py_BuildException("can't find item data");

	const DWORD dwMetinWearFlags = pMetinItemData->GetWearFlags();
	const DWORD dwTargetWearFlags = pTargetItemData->GetWearFlags();
	if (0 == (dwMetinWearFlags & dwTargetWearFlags))
		return Py_BuildValue("i", ATTACH_METIN_NOT_MATCHABLE_ITEM);

	if (CItemData::ITEM_TYPE_ROD == pTargetItemData->GetType() || CItemData::ITEM_TYPE_PICK == pTargetItemData->GetType())
		return Py_BuildValue("i", ATTACH_METIN_CANT);

	BOOL bNotExistGoldSocket = FALSE;

	int iSubType = pMetinItemData->GetSubType();
	for (int i = 0; i < ITEM_METIN_SOCKET_MAX; ++i)
	{
		DWORD dwSocketType = CPythonPlayer::Instance().GetItemMetinSocket(TargetSlotIndex, i);
		if (CItemData::METIN_NORMAL == iSubType || CItemData::METIN_SUNGMA)
		{
			if (CPythonPlayer::METIN_SOCKET_TYPE_SILVER == dwSocketType || CPythonPlayer::METIN_SOCKET_TYPE_GOLD == dwSocketType)
				return Py_BuildValue("i", ATTACH_METIN_OK);
		}
	}

	if (bNotExistGoldSocket)
		return Py_BuildValue("i", ATTACH_METIN_NOT_EXIST_GOLD_SOCKET);

	return Py_BuildValue("i", ATTACH_METIN_NO_MATCHABLE_SOCKET);
}

enum
{
	DETACH_METIN_CANT,
	DETACH_METIN_OK,
};

PyObject* playerCanDetach(PyObject* poSelf, PyObject* poArgs)
{
	int iScrollItemIndex;
	TItemPos TargetSlotIndex;

	switch (PyTuple_Size(poArgs))
	{
		case 2:
			if (!PyTuple_GetInteger(poArgs, 0, &iScrollItemIndex))
				return Py_BadArgument();
			if (!PyTuple_GetInteger(poArgs, 1, &TargetSlotIndex.cell))
				return Py_BadArgument();
			break;
		case 3:
			if (!PyTuple_GetInteger(poArgs, 0, &iScrollItemIndex))
				return Py_BadArgument();
			if (!PyTuple_GetInteger(poArgs, 1, &TargetSlotIndex.window_type))
				return Py_BadArgument();
			if (!PyTuple_GetInteger(poArgs, 2, &TargetSlotIndex.cell))
				return Py_BadArgument();
			break;
		default:
			return Py_BadArgument();
	}

	// Scroll
	CItemManager::Instance().SelectItemData(iScrollItemIndex);
	CItemData* pScrollItemData = CItemManager::Instance().GetSelectedItemDataPointer();
	if (!pScrollItemData)
		return Py_BuildException("Can't find item data");

	int iScrollType = pScrollItemData->GetType();
	int iScrollSubType = pScrollItemData->GetSubType();

	if (iScrollType != CItemData::ITEM_TYPE_USE)
		return Py_BuildValue("i", DETACH_METIN_CANT);

	// Target Item
	int iTargetItemIndex = CPythonPlayer::Instance().GetItemIndex(TargetSlotIndex);
	CItemManager::Instance().SelectItemData(iTargetItemIndex);
	CItemData* pTargetItemData = CItemManager::Instance().GetSelectedItemDataPointer();
	if (!pTargetItemData)
		return Py_BuildException("Can't find item data");

	int iTargetType = pTargetItemData->GetType();
	int iTargetSubType = pTargetItemData->GetSubType();

	if (CPythonPlayer::Instance().IsEquipmentSlot(TargetSlotIndex))
		return Py_BuildValue("i", DETACH_METIN_CANT);

#if defined(ENABLE_ACCE_COSTUME_SYSTEM)
	// Acce Reverse Scroll
	if (CPythonItem::Instance().IsAcceScroll(iScrollItemIndex) != FALSE)
	{
		if (!pTargetItemData->IsCostumeAcce())
			return Py_BuildValue("i", DETACH_METIN_CANT);

		if (CPythonPlayer::Instance().GetItemMetinSocket(TargetSlotIndex, ITEM_SOCKET_ACCE_DRAIN_ITEM_VNUM) > 0)
			return Py_BuildValue("i", DETACH_METIN_OK);
		else
			return Py_BuildValue("i", DETACH_METIN_CANT);
	}
#endif

	if (!pTargetItemData->IsRemovableSocket())
		return Py_BuildValue("i", DETACH_METIN_CANT);

	if (iScrollSubType == CItemData::USE_DETACHMENT || iScrollItemIndex == ITEM_VNUM_SCROLL_OF_CORRECTION)
	{
		if (pTargetItemData->IsFlag(CItemData::ITEM_FLAG_REFINEABLE))
		{
			for (int iSlotCount = 0; iSlotCount < METIN_SOCKET_COUNT; ++iSlotCount)
			{
				DWORD dwItemMetinSocket = CPythonPlayer::Instance().GetItemMetinSocket(TargetSlotIndex, iSlotCount);
				if (dwItemMetinSocket > 2 && dwItemMetinSocket != 28960 /* ERROR_METIN_STONE */)
				{
					return Py_BuildValue("i", DETACH_METIN_OK);
				}
			}
		}
	}

	return Py_BuildValue("i", DETACH_METIN_CANT);
}

PyObject* playerCanUnlock(PyObject* poSelf, PyObject* poArgs)
{
	int iKeyItemIndex;
	TItemPos TargetSlotIndex;

	switch (PyTuple_Size(poArgs))
	{
		case 2:
			if (!PyTuple_GetInteger(poArgs, 0, &iKeyItemIndex))
				return Py_BadArgument();
			if (!PyTuple_GetInteger(poArgs, 1, &TargetSlotIndex.cell))
				return Py_BadArgument();
			break;
		case 3:
			if (!PyTuple_GetInteger(poArgs, 0, &iKeyItemIndex))
				return Py_BadArgument();
			if (!PyTuple_GetInteger(poArgs, 1, &TargetSlotIndex.window_type))
				return Py_BadArgument();
			if (!PyTuple_GetInteger(poArgs, 2, &TargetSlotIndex.cell))
				return Py_BadArgument();
			break;
		default:
			return Py_BadArgument();
	}

	// Key
	CItemManager::Instance().SelectItemData(iKeyItemIndex);
	CItemData* pKeyItemData = CItemManager::Instance().GetSelectedItemDataPointer();
	if (!pKeyItemData)
		return Py_BuildException("Can't find item data");
	int iKeyType = pKeyItemData->GetType();
	if (iKeyType != CItemData::ITEM_TYPE_TREASURE_KEY)
		return Py_BuildValue("i", FALSE);

	// Target Item
	int iTargetItemIndex = CPythonPlayer::Instance().GetItemIndex(TargetSlotIndex);
	CItemManager::Instance().SelectItemData(iTargetItemIndex);
	CItemData* pTargetItemData = CItemManager::Instance().GetSelectedItemDataPointer();
	if (!pTargetItemData)
		return Py_BuildException("Can't find item data");
	int iTargetType = pTargetItemData->GetType();
	if (iTargetType != CItemData::ITEM_TYPE_TREASURE_BOX)
		return Py_BuildValue("i", FALSE);

	return Py_BuildValue("i", TRUE);
}

PyObject* playerIsRefineGradeScroll(PyObject* poSelf, PyObject* poArgs)
{
	TItemPos ScrollSlotIndex;
	switch (PyTuple_Size(poArgs))
	{
		case 1:
			if (!PyTuple_GetInteger(poArgs, 0, &ScrollSlotIndex.cell))
				return Py_BuildException();
		case 2:
			if (!PyTuple_GetInteger(poArgs, 0, &ScrollSlotIndex.window_type))
				return Py_BuildException();
			if (!PyTuple_GetInteger(poArgs, 1, &ScrollSlotIndex.cell))
				return Py_BuildException();
		default:
			return Py_BuildException();
	}

	int iScrollItemIndex = CPythonPlayer::Instance().GetItemIndex(ScrollSlotIndex);
	CItemManager::Instance().SelectItemData(iScrollItemIndex);
	CItemData* pScrollItemData = CItemManager::Instance().GetSelectedItemDataPointer();
	if (!pScrollItemData)
		return Py_BuildException("Can't find item data");

	return Py_BuildValue("i", REFINE_SCROLL_TYPE_UP_GRADE == pScrollItemData->GetValue(0));
}

PyObject* playerUpdate(PyObject* poSelf, PyObject* poArgs)
{
	CPythonPlayer::Instance().Update();
	return Py_BuildNone();
}

PyObject* playerRender(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildNone();
}

PyObject* playerClear(PyObject* poSelf, PyObject* poArgs)
{
	CPythonPlayer::Instance().Clear();
	return Py_BuildNone();
}

PyObject* playerClearTarget(PyObject* poSelf, PyObject* poArgs)
{
	CPythonPlayer::Instance().SetTarget(0);
	return Py_BuildNone();
}

PyObject* playerSetTarget(PyObject* poSelf, PyObject* poArgs)
{
	int iVID;
	if (!PyTuple_GetInteger(poArgs, 0, &iVID))
		return Py_BuildException();

	CPythonPlayer::Instance().SetTarget(iVID);
	return Py_BuildNone();
}

PyObject* playerOpenCharacterMenu(PyObject* poSelf, PyObject* poArgs)
{
	int iVID;
	if (!PyTuple_GetInteger(poArgs, 0, &iVID))
		return Py_BuildException();

	CPythonPlayer::Instance().OpenCharacterMenu(iVID);
	return Py_BuildNone();
}

PyObject* playerIsPartyMember(PyObject* poSelf, PyObject* poArgs)
{
	int iVID;
	if (!PyTuple_GetInteger(poArgs, 0, &iVID))
		return Py_BuildException();

	return Py_BuildValue("i", CPythonPlayer::Instance().IsPartyMemberByVID(iVID));
}

PyObject* playerIsPartyLeader(PyObject* poSelf, PyObject* poArgs)
{
	int iVID;
	if (!PyTuple_GetInteger(poArgs, 0, &iVID))
		return Py_BuildException();

	DWORD dwPID;
	if (!CPythonPlayer::Instance().PartyMemberVIDToPID(iVID, &dwPID))
		return Py_BuildValue("i", FALSE);

	CPythonPlayer::TPartyMemberInfo* pPartyMemberInfo;
	if (!CPythonPlayer::Instance().GetPartyMemberPtr(dwPID, &pPartyMemberInfo))
		return Py_BuildValue("i", FALSE);

	return Py_BuildValue("i", CPythonPlayer::PARTY_ROLE_LEADER == pPartyMemberInfo->byState);
}

PyObject* playerIsPartyLeaderByPID(PyObject* poSelf, PyObject* poArgs)
{
	int iPID;
	if (!PyTuple_GetInteger(poArgs, 0, &iPID))
		return Py_BuildException();

	CPythonPlayer::TPartyMemberInfo* pPartyMemberInfo;
	if (!CPythonPlayer::Instance().GetPartyMemberPtr(iPID, &pPartyMemberInfo))
		return Py_BuildValue("i", FALSE);

	return Py_BuildValue("i", CPythonPlayer::PARTY_ROLE_LEADER == pPartyMemberInfo->byState);
}

PyObject* playerGetPartyMemberHPPercentage(PyObject* poSelf, PyObject* poArgs)
{
	int iPID;
	if (!PyTuple_GetInteger(poArgs, 0, &iPID))
		return Py_BuildException();

	CPythonPlayer::TPartyMemberInfo* pPartyMemberInfo;
	if (!CPythonPlayer::Instance().GetPartyMemberPtr(iPID, &pPartyMemberInfo))
		return Py_BuildValue("i", FALSE);

	return Py_BuildValue("i", pPartyMemberInfo->byHPPercentage);
}

PyObject* playerGetPartyMemberState(PyObject* poSelf, PyObject* poArgs)
{
	int iPID;
	if (!PyTuple_GetInteger(poArgs, 0, &iPID))
		return Py_BuildException();

	CPythonPlayer::TPartyMemberInfo* pPartyMemberInfo;
	if (!CPythonPlayer::Instance().GetPartyMemberPtr(iPID, &pPartyMemberInfo))
		return Py_BuildValue("i", FALSE);

	return Py_BuildValue("i", pPartyMemberInfo->byState);
}

PyObject* playerGetPartyMemberAffects(PyObject* poSelf, PyObject* poArgs)
{
	int iPID;
	if (!PyTuple_GetInteger(poArgs, 0, &iPID))
		return Py_BuildException();

	CPythonPlayer::TPartyMemberInfo* pPartyMemberInfo;
	if (!CPythonPlayer::Instance().GetPartyMemberPtr(iPID, &pPartyMemberInfo))
		return Py_BuildValue("i", FALSE);

	return Py_BuildValue("iiiiiii", pPartyMemberInfo->sAffects[0],
		pPartyMemberInfo->sAffects[1],
		pPartyMemberInfo->sAffects[2],
		pPartyMemberInfo->sAffects[3],
		pPartyMemberInfo->sAffects[4],
		pPartyMemberInfo->sAffects[5],
		pPartyMemberInfo->sAffects[6]);
}

PyObject* playerRemovePartyMember(PyObject* poSelf, PyObject* poArgs)
{
	int iPID;
	if (!PyTuple_GetInteger(poArgs, 0, &iPID))
		return Py_BuildException();

	CPythonPlayer::Instance().RemovePartyMember(iPID);
	return Py_BuildNone();
}

PyObject* playerPartyMemberVIDToPID(PyObject* poSelf, PyObject* poArgs)
{
	int iVID;
	if (!PyTuple_GetInteger(poArgs, 0, &iVID))
		return Py_BuildException();

	DWORD dwPID;
	if (!CPythonPlayer::Instance().PartyMemberVIDToPID(iVID, &dwPID))
		return Py_BuildValue("i", FALSE);

	return Py_BuildValue("i", dwPID);
}

PyObject* playerExitParty(PyObject* poSelf, PyObject* poArgs)
{
	CPythonPlayer::Instance().ExitParty();
	return Py_BuildNone();
}

PyObject* playerGetPKMode(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonPlayer::Instance().GetPKMode());
}

PyObject* playerSetWeaponAttackBonusFlag(PyObject* poSelf, PyObject* poArgs)
{
	int iFlag;
	if (!PyTuple_GetInteger(poArgs, 0, &iFlag))
		return Py_BuildException();

	return Py_BuildNone();
}

PyObject* playerToggleCoolTime(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonPlayer::Instance().__ToggleCoolTime());
}

PyObject* playerToggleLevelLimit(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonPlayer::Instance().__ToggleLevelLimit());
}

PyObject* playerGetTargetVID(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonPlayer::Instance().GetTargetVID());
}

PyObject* playerRegisterEmotionIcon(PyObject* poSelf, PyObject* poArgs)
{
	int iIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iIndex))
		return Py_BuildException();

	char* szFileName;
	if (!PyTuple_GetString(poArgs, 1, &szFileName))
		return Py_BuildException();

	CGraphicImage* pImage = (CGraphicImage*)CResourceManager::Instance().GetResourcePointer(szFileName);
	m_kMap_iEmotionIndex_pkIconImage.insert(std::make_pair(iIndex, pImage));

	return Py_BuildNone();
}

PyObject* playerClearEmotionIcon(PyObject* poSelf, PyObject* poArgs)
{
	int iIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iIndex))
		return Py_BuildException();

	m_kMap_iEmotionIndex_pkIconImage.erase(iIndex);

	return Py_BuildNone();
}

PyObject* playerGetEmotionIconImage(PyObject* poSelf, PyObject* poArgs)
{
	int iIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iIndex))
		return Py_BuildException();

	if (m_kMap_iEmotionIndex_pkIconImage.end() == m_kMap_iEmotionIndex_pkIconImage.find(iIndex))
		return Py_BuildValue("i", 0);

	return Py_BuildValue("i", m_kMap_iEmotionIndex_pkIconImage[iIndex]);
}

PyObject* playerSetItemData(PyObject* poSelf, PyObject* poArgs)
{
	int iSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iSlotIndex))
		return Py_BuildException();

	int iVirtualID;
	if (!PyTuple_GetInteger(poArgs, 1, &iVirtualID))
		return Py_BuildException();

	int iNum;
	if (!PyTuple_GetInteger(poArgs, 2, &iNum))
		return Py_BuildException();

	TItemData kItemInst;
	ZeroMemory(&kItemInst, sizeof(kItemInst));
	kItemInst.dwVnum = iVirtualID;
	kItemInst.dwCount = iNum;
	CPythonPlayer::Instance().SetItemData(TItemPos(INVENTORY, iSlotIndex), kItemInst);
	return Py_BuildNone();
}

PyObject* playerSetItemMetinSocket(PyObject* poSelf, PyObject* poArgs)
{
	TItemPos ItemPos;
	int iMetinSocketNumber;
	int iNum;

	switch (PyTuple_Size(poArgs))
	{
		case 3:
			if (!PyTuple_GetInteger(poArgs, 0, &ItemPos.cell))
				return Py_BuildException();

			if (!PyTuple_GetInteger(poArgs, 1, &iMetinSocketNumber))
				return Py_BuildException();

			if (!PyTuple_GetInteger(poArgs, 2, &iNum))
				return Py_BuildException();

			break;
		case 4:
			if (!PyTuple_GetInteger(poArgs, 0, &ItemPos.window_type))
				return Py_BuildException();
			if (!PyTuple_GetInteger(poArgs, 1, &ItemPos.cell))
				return Py_BuildException();
			if (!PyTuple_GetInteger(poArgs, 2, &iMetinSocketNumber))
				return Py_BuildException();
			if (!PyTuple_GetInteger(poArgs, 3, &iNum))
				return Py_BuildException();

			break;
		default:
			return Py_BuildException();
	}

	CPythonPlayer::Instance().SetItemMetinSocket(ItemPos, iMetinSocketNumber, iNum);
	return Py_BuildNone();
}

PyObject* playerSetItemAttribute(PyObject* poSelf, PyObject* poArgs)
{
	TItemPos ItemPos;
	int iAttributeSlotIndex;
	int iAttributeType;
	int iAttributeValue;

	switch (PyTuple_Size(poArgs))
	{
		case 4:
			if (!PyTuple_GetInteger(poArgs, 0, &ItemPos.cell))
				return Py_BuildException();

			if (!PyTuple_GetInteger(poArgs, 1, &iAttributeSlotIndex))
				return Py_BuildException();

			if (!PyTuple_GetInteger(poArgs, 2, &iAttributeType))
				return Py_BuildException();

			if (!PyTuple_GetInteger(poArgs, 3, &iAttributeValue))
				return Py_BuildException();
			break;
		case 5:
			if (!PyTuple_GetInteger(poArgs, 0, &ItemPos.window_type))
				return Py_BuildException();

			if (!PyTuple_GetInteger(poArgs, 1, &ItemPos.cell))
				return Py_BuildException();

			if (!PyTuple_GetInteger(poArgs, 2, &iAttributeSlotIndex))
				return Py_BuildException();

			if (!PyTuple_GetInteger(poArgs, 3, &iAttributeType))
				return Py_BuildException();

			if (!PyTuple_GetInteger(poArgs, 4, &iAttributeValue))
				return Py_BuildException();
			break;
	}

	CPythonPlayer::Instance().SetItemAttribute(ItemPos, iAttributeSlotIndex, iAttributeType, iAttributeValue);
	return Py_BuildNone();
}

PyObject* playerGetAutoPotionInfo(PyObject* poSelf, PyObject* poArgs)
{
	BYTE bPotionType;
	if (!PyTuple_GetByte(poArgs, 0, &bPotionType))
		return Py_BadArgument();

	CPythonPlayer* pPlayer = CPythonPlayer::InstancePtr();
	const CPythonPlayer::SAutoPotionInfo& rkPotionInfo = pPlayer->GetAutoPotionInfo(bPotionType);
	return Py_BuildValue("ii", rkPotionInfo.Cell.window_type, rkPotionInfo.Cell.cell);
}

PyObject* playerSetAutoPotionInfo(PyObject* poSelf, PyObject* poArgs)
{
	BYTE bPotionType;
	if (!PyTuple_GetByte(poArgs, 0, &bPotionType))
		return Py_BadArgument();

	CPythonPlayer* pPlayer = CPythonPlayer::InstancePtr();
	CPythonPlayer::SAutoPotionInfo& rPotionInfo = pPlayer->GetAutoPotionInfo(bPotionType);

	if (!PyTuple_GetByte(poArgs, 1, &rPotionInfo.Cell.window_type))
		return Py_BuildException();

	if (!PyTuple_GetInteger(poArgs, 2, &rPotionInfo.Cell.cell))
		return Py_BuildException();

	return Py_BuildNone();
}

PyObject* playerSlotTypeToInvenType(PyObject* poSelf, PyObject* poArgs)
{
	int iSlotType = 0;
	if (!PyTuple_GetInteger(poArgs, 0, &iSlotType))
		return Py_BadArgument();

	return Py_BuildValue("i", SlotTypeToInvenType((BYTE)iSlotType));
}

PyObject* playerWindowTypeToSlotType(PyObject* poSelf, PyObject* poArgs)
{
	int iWndType = 0;
	if (!PyTuple_GetInteger(poArgs, 0, &iWndType))
		return Py_BadArgument();

	return Py_BuildValue("i", WindowTypeToSlotType((BYTE)iWndType));
}

#if defined(ENABLE_NEW_EQUIPMENT_SYSTEM)
// 플레이어가 벨트를 착용 중인지?
PyObject* playerIsEquippingBelt(PyObject* poSelf, PyObject* poArgs)
{
	const CPythonPlayer* player = CPythonPlayer::InstancePtr();
	bool bEquipping = false;

	const TItemData* data = player->GetItemData(TItemPos(EQUIPMENT, c_Equipment_Slot_Belt));

	if (NULL != data)
		bEquipping = 0 < data->dwCount;

	return Py_BuildValue("b", bEquipping);
}

// 검사하려는 벨트 인벤토리 Cell이 사용 가능한 칸인지? (사용가능 여부는 착용 중인 벨트의 강화 정도에 따라 달라짐)
PyObject* playerIsAvailableBeltInventoryCell(PyObject* poSelf, PyObject* poArgs)
{
	const CPythonPlayer* player = CPythonPlayer::InstancePtr();
	const TItemData* pData = player->GetItemData(TItemPos(EQUIPMENT, c_Equipment_Slot_Belt));

	if (NULL == pData || 0 == pData->dwCount)
		return Py_BuildValue("b", false);

	CItemManager::Instance().SelectItemData(pData->dwVnum);
	CItemData* pItem = CItemManager::Instance().GetSelectedItemDataPointer();

	long beltGrade = pItem->GetValue(0);

	int pos = 0;
	if (!PyTuple_GetInteger(poArgs, 0, &pos))
		return Py_BadArgument();

	//return Py_BuildValue("b", CBeltInventoryHelper::IsAvailableCell(pos - c_Belt_Inventory_Slot_Start, GetItemGrade(pItem->GetName())));
	return Py_BuildValue("b", CBeltInventoryHelper::IsAvailableCell(pos - c_Belt_Inventory_Slot_Start, beltGrade));
}
#endif

#if defined(ENABLE_DRAGON_SOUL_SYSTEM)
// 용혼석 강화
PyObject* playerSendDragonSoulRefine(PyObject* poSelf, PyObject* poArgs)
{
	BYTE bSubHeader;
	PyObject* pDic;
	TItemPos RefineItemPoses[DS_REFINE_WINDOW_MAX_NUM];
	if (!PyTuple_GetByte(poArgs, 0, &bSubHeader))
		return Py_BuildException();
	switch (bSubHeader)
	{
		case DS_SUB_HEADER_CLOSE:
			break;

		case DS_SUB_HEADER_DO_UPGRADE:
		case DS_SUB_HEADER_DO_IMPROVEMENT:
		case DS_SUB_HEADER_DO_REFINE:
#if defined(ENABLE_DS_CHANGE_ATTR)
		case DS_SUB_HEADER_DO_CHANGE_ATTR:
#endif
		{
			if (!PyTuple_GetObject(poArgs, 1, &pDic))
				return Py_BuildException();
			int pos = 0;
			PyObject* key, * value;
			int size = PyDict_Size(pDic);

			while (PyDict_Next(pDic, &pos, &key, &value))
			{
				int i = PyInt_AsLong(key);
				if (i > DS_REFINE_WINDOW_MAX_NUM)
					return Py_BuildException();

				if (!PyTuple_GetByte(value, 0, &RefineItemPoses[i].window_type)
					|| !PyTuple_GetInteger(value, 1, &RefineItemPoses[i].cell))
				{
					return Py_BuildException();
				}
			}
		}
		break;
	}

	CPythonNetworkStream& rns = CPythonNetworkStream::Instance();
	rns.SendDragonSoulRefinePacket(bSubHeader, RefineItemPoses);

	return Py_BuildNone();
}
#endif

#if defined(ENABLE_MOVE_COSTUME_ATTR)
PyObject* playerSetItemCombinationWindowActivedItemSlot(PyObject* poSelf, PyObject* poArgs)
{
	BYTE bSlotType;
	if (!PyTuple_GetByte(poArgs, 0, &bSlotType))
		return Py_BadArgument();

	int iSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &iSlotIndex))
		return Py_BadArgument();

	if (bSlotType >= ECombSlotType::COMB_WND_SLOT_MAX)
		return Py_BuildException();

	CPythonPlayer::Instance().SetItemCombinationWindowActivedItemSlot(bSlotType, iSlotIndex);

	return Py_BuildNone();
}

PyObject* playerGetInvenSlotAttachedToConbWindowSlot(PyObject* poSelf, PyObject* poArgs)
{
	BYTE bSlotType;
	if (!PyTuple_GetByte(poArgs, 0, &bSlotType))
		return Py_BadArgument();

	if (bSlotType >= ECombSlotType::COMB_WND_SLOT_MAX)
		return Py_BuildException();

	return Py_BuildValue("i", CPythonPlayer::Instance().GetInvenSlotAttachedToConbWindowSlot(bSlotType));
}

PyObject* playerCanAttachToCombMediumSlot(PyObject* poSelf, PyObject* poArgs)
{
	BYTE bSlotType;
	if (!PyTuple_GetByte(poArgs, 0, &bSlotType))
		return Py_BadArgument();

	TItemPos Cell;
	if (!PyTuple_GetByte(poArgs, 1, &Cell.window_type))
		return Py_BuildException();
	if (!PyTuple_GetInteger(poArgs, 2, &Cell.cell))
		return Py_BuildException();

	if (bSlotType != ECombSlotType::COMB_WND_SLOT_MEDIUM || Cell.window_type != INVENTORY)
		return Py_BuildValue("b", false);

	const TItemData* pItem = CPythonPlayer::Instance().GetItemData(Cell);
	if (pItem == nullptr)
		return Py_BuildValue("b", false);

	CItemData* pItemData = nullptr;
	if (CItemManager::Instance().GetItemDataPointer(pItem->dwVnum, &pItemData) == false)
		return Py_BuildValue("b", false);

	bool bRet = false;
	if (pItemData->GetType() == CItemData::EItemTypes::ITEM_TYPE_MEDIUM)
	{
		switch (pItemData->GetSubType())
		{
			case CItemData::EMediumSubTypes::MEDIUM_MOVE_COSTUME_ATTR:
#if defined(ENABLE_ACCE_COSTUME_SYSTEM)
			case CItemData::EMediumSubTypes::MEDIUM_MOVE_ACCE_ATTR:
#endif
				bRet = true;
				break;
			default:
				break;
		}
	}

	return Py_BuildValue("b", bRet);
}

PyObject* playerCanAttachToCombItemSlot(PyObject* poSelf, PyObject* poArgs)
{
	BYTE bSlotType;
	if (!PyTuple_GetByte(poArgs, 0, &bSlotType))
		return Py_BadArgument();

	TItemPos Cell;
	if (!PyTuple_GetByte(poArgs, 1, &Cell.window_type))
		return Py_BuildException();
	if (!PyTuple_GetInteger(poArgs, 2, &Cell.cell))
		return Py_BuildException();

	if ((bSlotType != ECombSlotType::COMB_WND_SLOT_BASE && bSlotType != ECombSlotType::COMB_WND_SLOT_MATERIAL)
		|| Cell.window_type != INVENTORY)
		return Py_BuildValue("b", false);

	const TItemData* pItem = CPythonPlayer::Instance().GetItemData(Cell);
	if (pItem == nullptr)
		return Py_BuildValue("b", false);

	CItemData* pItemData = nullptr;
	if (CItemManager::Instance().GetItemDataPointer(pItem->dwVnum, &pItemData) == false)
		return Py_BuildValue("b", false);

	if (pItemData->GetType() != CItemData::EItemTypes::ITEM_TYPE_COSTUME)
		return Py_BuildValue("b", false);

	const CItemData* pMediumItemData = CPythonPlayer::Instance().GetConbMediumItemData();
	if (pMediumItemData == nullptr)
		return Py_BuildValue("b", false);

	if (pMediumItemData->GetSubType() == CItemData::MEDIUM_MOVE_COSTUME_ATTR)
	{
#if defined(ENABLE_MOUNT_COSTUME_SYSTEM)
		if (pItemData->GetSubType() == CItemData::COSTUME_MOUNT)
			return Py_BuildValue("b", false);
#endif

#if defined(ENABLE_ACCE_COSTUME_SYSTEM)
		if (pItemData->GetSubType() == CItemData::COSTUME_ACCE)
			return Py_BuildValue("b", false);
#endif

#if defined(ENABLE_AURA_COSTUME_SYSTEM)
		if (pItemData->GetSubType() == CItemData::COSTUME_AURA)
			return Py_BuildValue("b", false);
#endif;

		bool bHasAttr = false;
		for (BYTE i = 0; i < ITEM_ATTRIBUTE_SLOT_MAX_NUM; i++)
		{
			if (pItem->aAttr[i].wType != 0)
			{
				bHasAttr = true;
				break;
			}
		}

		if (bHasAttr == false)
			return Py_BuildValue("b", false);
	}
#if defined(ENABLE_ACCE_COSTUME_SYSTEM)
	else if (pMediumItemData->GetSubType() == CItemData::MEDIUM_MOVE_ACCE_ATTR)
	{
		if (pItemData->GetSubType() != CItemData::COSTUME_ACCE)
			return Py_BuildValue("b", false);
	}
#endif

	for (BYTE i : { ECombSlotType::COMB_WND_SLOT_BASE, ECombSlotType::COMB_WND_SLOT_MATERIAL })
	{
		const short sOtherItemIndex = CPythonPlayer::Instance().GetInvenSlotAttachedToConbWindowSlot(i);
		if (sOtherItemIndex == -1)
			continue;

		const TItemData* pOtherItem = CPythonPlayer::Instance().GetItemData(TItemPos(INVENTORY, sOtherItemIndex));
		if (pOtherItem == nullptr)
			continue;

		CItemData* pOtherItemData = nullptr;
		if (CItemManager::Instance().GetItemDataPointer(pOtherItem->dwVnum, &pOtherItemData) == false)
			continue;

		if (pOtherItemData->GetSubType() != pItemData->GetSubType())
			return Py_BuildValue("b", false);
	}

	return Py_BuildValue("b", true);
}

PyObject* playerGetConbWindowSlotByAttachedInvenSlot(PyObject* poSelf, PyObject* poArgs)
{
	int iSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iSlotIndex))
		return Py_BadArgument();

	BYTE bRet = 0;
	while (bRet < ECombSlotType::COMB_WND_SLOT_MAX
		&& CPythonPlayer::Instance().GetInvenSlotAttachedToConbWindowSlot(bRet) != iSlotIndex)
		bRet++;

	return Py_BuildValue("i", bRet);
}
#endif

#if defined(ENABLE_CHANGED_ATTR)
PyObject* playerGetItemChangedAttribute(PyObject* poSelf, PyObject* poArgs)
{
	int iAttributeSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iAttributeSlotIndex))
		return Py_BuildException();

	POINT_TYPE wType; POINT_VALUE lValue;
	CPythonPlayer::Instance().GetItemChangedAttribute(iAttributeSlotIndex, &wType, &lValue);

	return Py_BuildValue("ii", wType, lValue);
}
#endif

#if defined(ENABLE_SOULBIND_SYSTEM)
PyObject* playerCanSealItem(PyObject* poSelf, PyObject* poArgs)
{
	int iItemVID;
	TItemPos DestCell;
	switch (PyTuple_Size(poArgs))
	{
		case 2:
		{
			if (!PyTuple_GetInteger(poArgs, 0, &iItemVID))
				return Py_BadArgument();
			if (!PyTuple_GetInteger(poArgs, 1, &DestCell.cell))
				return Py_BadArgument();
		}
		break;

		case 3:
		{
			if (!PyTuple_GetInteger(poArgs, 0, &iItemVID))
				return Py_BadArgument();
			if (!PyTuple_GetInteger(poArgs, 1, &DestCell.window_type))
				return Py_BadArgument();
			if (!PyTuple_GetInteger(poArgs, 2, &DestCell.cell))
				return Py_BadArgument();
		}
		break;

		default:
			return Py_BuildException();
	}

	CItemManager::Instance().SelectItemData(iItemVID);
	CItemData* pItemData = CItemManager::Instance().GetSelectedItemDataPointer();
	if (pItemData == nullptr)
		return Py_BuildException("Can't find item data");

	CItemManager::Instance().SelectItemData(CPythonPlayer::Instance().GetItemIndex(DestCell));
	CItemData* pDestItemData = CItemManager::Instance().GetSelectedItemDataPointer();
	if (!pDestItemData)
		return Py_BuildException("Can't find item data");

	const TItemData* pDestItem = CPythonPlayer::Instance().GetItemData(DestCell);
	if (!pDestItem)
		return Py_BuildException("Cell(%d, %d) item is null", DestCell.window_type, DestCell.cell);

	if (pItemData->GetType() == CItemData::ITEM_TYPE_USE && pItemData->GetIndex() == CItemData::SEAL_ITEM_BINDING_VNUM)
	{
		if (pDestItemData->CanSealItem() && !pDestItem->lSealDate)
			return Py_BuildValue("i", TRUE);
	}
	else if (pItemData->GetType() == CItemData::ITEM_TYPE_USE && (pItemData->GetIndex() == CItemData::SEAL_ITEM_UNBINDING_VNUM
#if defined(ENABLE_UN_SEAL_SCROLL_PLUS)
		|| pItemData->GetIndex() == CItemData::SEAL_ITEM_UNBINDING_VNUM_MALL
#endif
		))
	{
		if (pDestItem->lSealDate)
			return Py_BuildValue("i", TRUE);
	}

	return Py_BuildValue("i", FALSE);
}

PyObject* playerGetItemSealDate(PyObject* poSelf, PyObject* poArgs)
{
	TItemPos Cell;
	if (!PyTuple_GetByte(poArgs, 0, &Cell.window_type))
		return Py_BuildException();
	if (!PyTuple_GetInteger(poArgs, 1, &Cell.cell))
		return Py_BuildException();

	TItemData* pPlayerItem = nullptr;
	if (Cell.window_type == SAFEBOX)
		CPythonSafeBox::Instance().GetItemDataPtr(Cell.cell, &pPlayerItem);
	else
		pPlayerItem = const_cast<TItemData*>(CPythonPlayer::Instance().GetItemData(Cell));

	if (pPlayerItem)
	{
		long lSealDate = pPlayerItem->lSealDate;
		if (CItemData::U_SEAL_DATE_DEFAULT_TIMESTAMP != lSealDate && lSealDate < CPythonApplication::Instance().GetServerTimeStamp())
			return Py_BuildValue("i", CItemData::E_SEAL_DATE_DEFAULT_TIMESTAMP);

		return Py_BuildValue("i", lSealDate);
	}

	return Py_BuildValue("i", CItemData::E_SEAL_DATE_DEFAULT_TIMESTAMP);
}

PyObject* playerGetItemUnSealLeftTime(PyObject* poSelf, PyObject* poArgs)
{
	TItemPos Cell;
	if (!PyTuple_GetByte(poArgs, 0, &Cell.window_type))
		return Py_BuildException();
	if (!PyTuple_GetInteger(poArgs, 1, &Cell.cell))
		return Py_BuildException();

	TItemData* pPlayerItem = nullptr;
	if (Cell.window_type == SAFEBOX)
		CPythonSafeBox::Instance().GetItemDataPtr(Cell.cell, &pPlayerItem);
	else
	{
		pPlayerItem = const_cast<TItemData*>(CPythonPlayer::Instance().GetItemData(Cell));
		if (pPlayerItem != NULL)
		{
			long lSealDate = pPlayerItem->lSealDate;
			if (lSealDate != CItemData::U_SEAL_DATE_DEFAULT_TIMESTAMP && lSealDate != 0 && lSealDate > CPythonApplication::Instance().GetServerTimeStamp())
				return Py_BuildValue("ii", ((lSealDate - CPythonApplication::Instance().GetServerTimeStamp()) / 3600), (lSealDate - CPythonApplication::Instance().GetServerTimeStamp()) % 3600 / 60);
		}
	}

	return Py_BuildValue("ii", 0, 0);
}

PyObject* playerIsSealedItemBySlot(PyObject* poSelf, PyObject* poArgs)
{
	TItemPos Cell;

	if (!PyTuple_GetByte(poArgs, 0, &Cell.window_type))
		return Py_BuildException();

	if (!PyTuple_GetInteger(poArgs, 1, &Cell.cell))
		return Py_BuildException();

	return Py_BuildValue("b", CPythonPlayer::Instance().IsSealedItemBySlot(Cell));
}
#endif

PyObject* playerGetLevel(PyObject* poSelf, PyObject* poArgs)
{
	DWORD dwLevel = CPythonPlayer::Instance().GetStatus(POINT_LEVEL);
	return Py_BuildValue("l", dwLevel);
}

#if defined(ENABLE_LOADING_TIP)
PyObject* playerGetLoadingTip(PyObject* poSelf, PyObject* poArgs)
{
	int iMapIndex = CPythonNetworkStream::Instance().GetMapIndex();
	return Py_BuildValue("s", CPythonLocale::Instance().GetLoadingTipVnum(iMapIndex));
}
#endif

PyObject* playerCheckAffect(PyObject* poSelf, PyObject* poArgs)
{
	DWORD dwType;
	if (!PyTuple_GetUnsignedLong(poArgs, 0, &dwType))
		return Py_BadArgument();

	BYTE bApplyOn;
	if (!PyTuple_GetByte(poArgs, 1, &bApplyOn))
		return Py_BadArgument();

	int iAffIndex = CPythonPlayer::Instance().GetAffectDataIndex(dwType, bApplyOn);
	return Py_BuildValue("b", iAffIndex != -1);
}

PyObject* playerSetParalysis(PyObject* poSelf, PyObject* poArgs)
{
	int isParalysis;
	if (!PyTuple_GetInteger(poArgs, 0, &isParalysis))
		return Py_BuildException();

	CInstanceBase* pMainInstance = CPythonPlayer::Instance().NEW_GetMainActorPtr();
	if (pMainInstance)
		pMainInstance->GetGraphicThingInstanceRef().SetParalysis(isParalysis ? true : false);

	return Py_BuildNone();
}

#if defined(ENABLE_KEYCHANGE_SYSTEM)
PyObject* playerOpenKeyChangeWindow(PyObject* poSelf, PyObject* poArgs)
{
	CPythonPlayer::Instance().OpenKeyChangeWindow();
	return Py_BuildNone();
}

PyObject* playerIsOpenKeySettingWindow(PyObject* poSelf, PyObject* poArgs)
{
	BOOL bSet = 0;
	if (!PyTuple_GetInteger(poArgs, 0, &bSet))
		return Py_BadArgument();

	CPythonPlayer::Instance().IsOpenKeySettingWindow(bSet);
	return Py_BuildNone();
}

PyObject* playerKeySettingClear(PyObject* poSelf, PyObject* poArgs)
{
	CPythonPlayer::Instance().KeySettingClear();
	return Py_BuildNone();
}

PyObject* playerKeySetting(PyObject* poSelf, PyObject* poArgs)
{
	int iKey = 0;
	if (!PyTuple_GetInteger(poArgs, 0, &iKey))
		return Py_BadArgument();

	int iKeyFunction = 0;
	if (!PyTuple_GetInteger(poArgs, 1, &iKeyFunction))
		return Py_BadArgument();

	CPythonPlayer::Instance().KeySetting(iKey, iKeyFunction);
	return Py_BuildNone();
}

PyObject* playerOnKeyDown(PyObject* poSelf, PyObject* poArgs)
{
	int iKey = 0;
	if (!PyTuple_GetInteger(poArgs, 0, &iKey))
		return Py_BadArgument();

	CPythonPlayer::Instance().OnKeyDown(iKey);
	return Py_BuildNone();
}

PyObject* playerOnKeyUp(PyObject* poSelf, PyObject* poArgs)
{
	int iKey = 0;
	if (!PyTuple_GetInteger(poArgs, 0, &iKey))
		return Py_BadArgument();

	CPythonPlayer::Instance().OnKeyUp(iKey);
	return Py_BuildNone();
}
#endif

#if defined(ENABLE_ACCE_COSTUME_SYSTEM)
PyObject* playerSetAcceRefineWindowOpen(PyObject* poSelf, PyObject* poArgs)
{
	BYTE bType;
	if (!PyTuple_GetByte(poArgs, 0, &bType))
		return Py_BuildException();

	CPythonPlayer::Instance().SetAcceRefineWindowType(bType);
	return Py_BuildNone();
}

PyObject* playerGetAcceRefineWindowOpen(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("b", CPythonPlayer::Instance().GetAcceRefineWindowOpen());
}

PyObject* playerGetAcceRefineWindowType(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonPlayer::Instance().GetAcceRefineWindowType());
}

PyObject* playerFindUsingAcceSlot(PyObject* poSelf, PyObject* poArgs)
{
	BYTE bAcceSlotPos;
	if (!PyTuple_GetInteger(poArgs, 0, &bAcceSlotPos))
		return Py_BuildException();

	TItemPos ItemCell = CPythonPlayer::Instance().FindUsingAcceSlot(bAcceSlotPos);
	return Py_BuildValue("(ii)", ItemCell.window_type, ItemCell.cell);
}

PyObject* playerFindActivatedAcceSlot(PyObject* poSelf, PyObject* poArgs)
{
	TItemPos ItemCell;
	if (!PyTuple_GetByte(poArgs, 0, &ItemCell.window_type))
		return Py_BuildException();
	if (!PyTuple_GetInteger(poArgs, 1, &ItemCell.cell))
		return Py_BuildException();

	return Py_BuildValue("i", CPythonPlayer::Instance().FindActivatedAcceSlot(ItemCell));
}

PyObject* playerFindMoveAcceItemSlot(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonPlayer::Instance().FindMoveAcceItemSlot());
}

PyObject* playerSetAcceActivatedItemSlot(PyObject* poSelf, PyObject* poArgs)
{
	BYTE bAcceSlotPos;
	if (!PyTuple_GetByte(poArgs, 0, &bAcceSlotPos))
		return Py_BuildException();

	TItemPos ItemCell;
	if (!PyTuple_GetByte(poArgs, 1, &ItemCell.window_type))
		return Py_BuildException();
	if (!PyTuple_GetInteger(poArgs, 2, &ItemCell.cell))
		return Py_BuildException();

	CPythonPlayer::Instance().SetActivatedAcceSlot(bAcceSlotPos, ItemCell);
	return Py_BuildNone();
}

PyObject* playerGetAcceCurrentItemCount(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonPlayer::Instance().GetAcceCurrentItemCount());
}

PyObject* playerGetAcceItemSize(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", ACCE_SLOT_MAX);
}

PyObject* playerIsAcceWindowEmpty(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonPlayer::Instance().IsAcceWindowEmpty());
}

PyObject* playerGetAcceItemID(PyObject* poSelf, PyObject* poArgs)
{
	int iSlotPos;
	if (!PyTuple_GetInteger(poArgs, 0, &iSlotPos))
		return Py_BadArgument();

	TItemData* pAcceItemInstance;
	if (!CPythonPlayer::Instance().GetAcceItemDataPtr(iSlotPos, &pAcceItemInstance))
		return Py_BuildException();

	return Py_BuildValue("i", pAcceItemInstance->dwVnum);
}

PyObject* playerGetAcceItemFlags(PyObject* poSelf, PyObject* poArgs)
{
	int iSlotPos;
	if (!PyTuple_GetInteger(poArgs, 0, &iSlotPos))
		return Py_BadArgument();

	TItemData* pAcceItemInstance;
	if (!CPythonPlayer::Instance().GetAcceItemDataPtr(iSlotPos, &pAcceItemInstance))
		return Py_BuildException();

	return Py_BuildValue("i", pAcceItemInstance->dwFlags);
}

PyObject* playerGetAcceItemMetinSocket(PyObject* poSelf, PyObject* poArgs)
{
	int iSlotPos;
	if (!PyTuple_GetInteger(poArgs, 0, &iSlotPos))
		return Py_BadArgument();

	int iMetinSocketIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &iMetinSocketIndex))
		return Py_BadArgument();

	TItemData* pAcceItemInstance;
	if (!CPythonPlayer::Instance().GetAcceItemDataPtr(iSlotPos, &pAcceItemInstance))
		return Py_BuildException();

	if (iMetinSocketIndex >= ITEM_SOCKET_SLOT_MAX_NUM || iMetinSocketIndex < 0)
		return Py_BuildException();

	return Py_BuildValue("i", pAcceItemInstance->alSockets[iMetinSocketIndex]);
}

PyObject* playerGetAcceItemAttribute(PyObject* poSelf, PyObject* poArgs)
{
	int iSlotPos;
	if (!PyTuple_GetInteger(poArgs, 0, &iSlotPos))
		return Py_BadArgument();

	int iAttributeSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &iAttributeSlotIndex))
		return Py_BadArgument();

	TItemData* pAcceItemInstance;
	if (!CPythonPlayer::Instance().GetAcceItemDataPtr(iSlotPos, &pAcceItemInstance) || (iAttributeSlotIndex >= ITEM_ATTRIBUTE_SLOT_MAX_NUM || iAttributeSlotIndex < 0))
		return Py_BuildValue("ii", 0, 0);

	TPlayerItemAttribute kAttr = pAcceItemInstance->aAttr[iAttributeSlotIndex];
	return Py_BuildValue("ii", kAttr.wType, kAttr.lValue);
}

#if defined(ENABLE_CHANGE_LOOK_SYSTEM)
PyObject* playerGetAcceWindowChangeLookVnum(PyObject* poSelf, PyObject* poArgs)
{
	int iSlotPos;
	if (!PyTuple_GetInteger(poArgs, 0, &iSlotPos))
		return Py_BadArgument();

	TItemData* pAcceItemInstance;
	if (!CPythonPlayer::Instance().GetAcceItemDataPtr(iSlotPos, &pAcceItemInstance))
		return Py_BuildException();

	return Py_BuildValue("i", pAcceItemInstance->dwTransmutationVnum);
}
#endif

#if defined(ENABLE_APPLY_RANDOM)
PyObject* playerGetAcceItemApplyRandom(PyObject* poSelf, PyObject* poArgs)
{
	int iSlotPos;
	if (!PyTuple_GetInteger(poArgs, 0, &iSlotPos))
		return Py_BadArgument();

	int iApplyIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &iApplyIndex))
		return Py_BadArgument();

	TItemData* pAcceItemInstance;
	if (!CPythonPlayer::Instance().GetAcceItemDataPtr(iSlotPos, &pAcceItemInstance) || (iApplyIndex >= ITEM_APPLY_RANDOM_SLOT_MAX_NUM || iApplyIndex < 0))
		return Py_BuildValue("ii", 0, 0);

	TPlayerItemAttribute kApplyRandom = pAcceItemInstance->aApplyRandom[iApplyIndex];
	return Py_BuildValue("ii", kApplyRandom.wType, kApplyRandom.lValue);
}
#endif

#if defined(ENABLE_SET_ITEM)
PyObject* playerGetAcceItemSetValue(PyObject* poSelf, PyObject* poArgs)
{
	int iSlotPos;
	if (!PyTuple_GetInteger(poArgs, 0, &iSlotPos))
		return Py_BadArgument();

	TItemData* pAcceItemInstance;
	if (!CPythonPlayer::Instance().GetAcceItemDataPtr(iSlotPos, &pAcceItemInstance))
		return Py_BuildException();

	return Py_BuildValue("i", pAcceItemInstance->bSetValue);
}
#endif

#if defined(ENABLE_REFINE_ELEMENT_SYSTEM)
PyObject* playerGetAcceItemRefineElement(PyObject* poSelf, PyObject* poArgs)
{
	int iSlotPos;
	if (!PyTuple_GetInteger(poArgs, 0, &iSlotPos))
		return Py_BadArgument();

	TItemData* pAcceItemInstance;
	if (!CPythonPlayer::Instance().GetAcceItemDataPtr(iSlotPos, &pAcceItemInstance))
		return Py_BuildValue("(iiOO)", 0, 0, Py_BuildValue("iii", 0, 0, 0), Py_BuildValue("iii", 0, 0, 0));

	const TPlayerItemRefineElement& kTable = pAcceItemInstance->RefineElement;
	return Py_BuildValue("(iiOO)", kTable.wApplyType, kTable.bGrade,
		Py_BuildValue("iii", kTable.abValue[0], kTable.abValue[1], kTable.abValue[2]),
		Py_BuildValue("iii", kTable.abBonusValue[0], kTable.abBonusValue[1], kTable.abBonusValue[2]));
}
#endif

PyObject* playerCanAcceClearItem(PyObject* poSelf, PyObject* poArgs)
{
	if (PyTuple_Size(poArgs) != 3)
		return Py_BuildException();

	int iScrollItemIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iScrollItemIndex))
		return Py_BadArgument();

	TItemPos TargetSlotIndex;
	if (!PyTuple_GetByte(poArgs, 1, &TargetSlotIndex.window_type))
		return Py_BuildException();
	if (!PyTuple_GetInteger(poArgs, 2, &TargetSlotIndex.cell))
		return Py_BuildException();

	if (!CPythonItem::Instance().IsAcceScroll(iScrollItemIndex))
		return Py_BuildValue("i", DETACH_METIN_CANT);

	// Scroll
	CItemManager::Instance().SelectItemData(iScrollItemIndex);
	CItemData* pScrollItemData = CItemManager::Instance().GetSelectedItemDataPointer();
	if (!pScrollItemData)
		return Py_BuildException("Can't find item data");

	if (pScrollItemData->GetType() != CItemData::ITEM_TYPE_USE || pScrollItemData->GetSubType() != CItemData::USE_SPECIAL)
		return Py_BuildValue("i", DETACH_METIN_CANT);

	// Target Item
	int iTargetItemIndex = CPythonPlayer::Instance().GetItemIndex(TargetSlotIndex);
	CItemManager::Instance().SelectItemData(iTargetItemIndex);
	CItemData* pTargetItemData = CItemManager::Instance().GetSelectedItemDataPointer();
	if (!pTargetItemData)
		return Py_BuildException("Can't find item data");

	if (CPythonPlayer::Instance().IsEquipmentSlot(TargetSlotIndex))
		return Py_BuildValue("i", DETACH_METIN_CANT);

	if (pTargetItemData->GetType() != CItemData::ITEM_TYPE_COSTUME || pTargetItemData->GetSubType() != CItemData::COSTUME_ACCE)
		return Py_BuildValue("i", DETACH_METIN_CANT);

	const TItemData* pPlayerItem = CPythonPlayer::Instance().GetItemData(TargetSlotIndex);
	if (pPlayerItem)
	{
		if (pPlayerItem->alSockets[ITEM_SOCKET_ACCE_DRAIN_ITEM_VNUM] > 0)
			return Py_BuildValue("i", DETACH_METIN_OK);
		else
			return Py_BuildValue("i", DETACH_METIN_CANT);
	}

	return Py_BuildValue("i", DETACH_METIN_CANT);
}
#endif

#if defined(ENABLE_AURA_COSTUME_SYSTEM)
PyObject* playerGetAuraItemID(PyObject* poSelf, PyObject* poArgs)
{
	int iSlotPos;
	if (!PyTuple_GetInteger(poArgs, 0, &iSlotPos))
		return Py_BadArgument();

	TItemData* pAuraItemInstance;
	if (!CPythonPlayer::Instance().GetAuraItemDataPtr(iSlotPos, &pAuraItemInstance))
		return Py_BuildException();

	return Py_BuildValue("i", pAuraItemInstance->dwVnum);
}

PyObject* playerGetAuraItemCount(PyObject* poSelf, PyObject* poArgs)
{
	int iSlotPos;
	if (!PyTuple_GetInteger(poArgs, 0, &iSlotPos))
		return Py_BadArgument();

	TItemData* pAuraItemInstance;
	if (!CPythonPlayer::Instance().GetAuraItemDataPtr(iSlotPos, &pAuraItemInstance))
		return Py_BuildException();

	return Py_BuildValue("i", pAuraItemInstance->dwCount);
}

PyObject* playerGetAuraSlotItemMetinSocket(PyObject* poSelf, PyObject* poArgs)
{
	int iSlotPos;
	if (!PyTuple_GetInteger(poArgs, 0, &iSlotPos))
		return Py_BadArgument();

	int iMetinSocketIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &iMetinSocketIndex))
		return Py_BadArgument();

	TItemData* pAuraItemInstance;
	if (!CPythonPlayer::Instance().GetAuraItemDataPtr(iSlotPos, &pAuraItemInstance))
		return Py_BuildException();

	if (iMetinSocketIndex >= ITEM_SOCKET_SLOT_MAX_NUM || iMetinSocketIndex < 0)
		return Py_BuildException();

	return Py_BuildValue("i", pAuraItemInstance->alSockets[iMetinSocketIndex]);
}

PyObject* playerGetAuraSlotItemAttribute(PyObject* poSelf, PyObject* poArgs)
{
	int iSlotPos;
	if (!PyTuple_GetInteger(poArgs, 0, &iSlotPos))
		return Py_BadArgument();

	int iAttributeSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &iAttributeSlotIndex))
		return Py_BadArgument();

	TItemData* pAuraItemInstance;
	if (!CPythonPlayer::Instance().GetAuraItemDataPtr(iSlotPos, &pAuraItemInstance) || (iAttributeSlotIndex >= ITEM_ATTRIBUTE_SLOT_MAX_NUM || iAttributeSlotIndex < 0))
		return Py_BuildValue("ii", 0, 0);

	TPlayerItemAttribute kAttr = pAuraItemInstance->aAttr[iAttributeSlotIndex];
	return Py_BuildValue("ii", kAttr.wType, kAttr.lValue);
}

PyObject* playerGetAuraSlotItemFlags(PyObject* poSelf, PyObject* poArgs)
{
	int iSlotPos;
	if (!PyTuple_GetInteger(poArgs, 0, &iSlotPos))
		return Py_BadArgument();

	TItemData* pAuraItemInstance;
	if (!CPythonPlayer::Instance().GetAuraItemDataPtr(iSlotPos, &pAuraItemInstance))
		return Py_BuildException();

	return Py_BuildValue("i", pAuraItemInstance->dwFlags);
}

#if defined(ENABLE_APPLY_RANDOM)
PyObject* playerGetAuraSlotItemApplyRandom(PyObject* poSelf, PyObject* poArgs)
{
	int iSlotPos;
	if (!PyTuple_GetInteger(poArgs, 0, &iSlotPos))
		return Py_BadArgument();

	int iApplyIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &iApplyIndex))
		return Py_BadArgument();

	TItemData* pAuraItemInstance;
	if (!CPythonPlayer::Instance().GetAuraItemDataPtr(iSlotPos, &pAuraItemInstance) || (iApplyIndex >= ITEM_APPLY_RANDOM_SLOT_MAX_NUM || iApplyIndex < 0))
		return Py_BuildValue("ii", 0, 0);

	TPlayerItemAttribute kApplyRandom = pAuraItemInstance->aApplyRandom[iApplyIndex];
	return Py_BuildValue("ii", kApplyRandom.wType, kApplyRandom.lValue);
}
#endif

#if defined(ENABLE_SET_ITEM)
PyObject* playerGetAuraSlotItemSetValue(PyObject* poSelf, PyObject* poArgs)
{
	int iSlotPos;
	if (!PyTuple_GetInteger(poArgs, 0, &iSlotPos))
		return Py_BadArgument();

	TItemData* pAuraItemInstance;
	if (!CPythonPlayer::Instance().GetAuraItemDataPtr(iSlotPos, &pAuraItemInstance))
		return Py_BuildException();

	return Py_BuildValue("i", pAuraItemInstance->bSetValue);
}
#endif

PyObject* playerSetAuraWindowOpen(PyObject* poSelf, PyObject* poArgs)
{
	BYTE bType;
	if (!PyTuple_GetByte(poArgs, 0, &bType))
		return Py_BuildException();

	CPythonPlayer::Instance().SetAuraWindowType(bType);
	return Py_BuildNone();
}

PyObject* playerIsAuraWindowOpen(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonPlayer::Instance().IsAuraWindowOpen());
}

PyObject* playerGetAuraWindowType(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonPlayer::Instance().GetAuraWindowType());
}

PyObject* playerIsAuraSlotEmpty(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonPlayer::Instance().IsAuraSlotEmpty());
}

PyObject* playerGetAuraCurrentItemSlotCount(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonPlayer::Instance().GetAuraCurrentItemSlotCount());
}

PyObject* playerSetAuraActivatedItemSlot(PyObject* poSelf, PyObject* poArgs)
{
	BYTE bAuraSlotPos;
	if (!PyTuple_GetByte(poArgs, 0, &bAuraSlotPos))
		return Py_BuildException();

	TItemPos ItemCell;
	if (!PyTuple_GetByte(poArgs, 1, &ItemCell.window_type))
		return Py_BuildException();
	if (!PyTuple_GetInteger(poArgs, 2, &ItemCell.cell))
		return Py_BuildException();

	CPythonPlayer::Instance().SetActivatedAuraSlot(bAuraSlotPos, ItemCell);
	return Py_BuildNone();
}

PyObject* playerFindUsingAuraSlot(PyObject* poSelf, PyObject* poArgs)
{
	BYTE bAuraSlotPos;
	if (!PyTuple_GetInteger(poArgs, 0, &bAuraSlotPos))
		return Py_BuildException();

	TItemPos ItemCell = CPythonPlayer::Instance().FindUsingAuraSlot(bAuraSlotPos);
	return Py_BuildValue("(ii)", ItemCell.window_type, ItemCell.cell);
}

PyObject* playerFindActivatedAuraSlot(PyObject* poSelf, PyObject* poArgs)
{
	TItemPos ItemCell;
	if (!PyTuple_GetByte(poArgs, 0, &ItemCell.window_type))
		return Py_BuildException();
	if (!PyTuple_GetInteger(poArgs, 1, &ItemCell.cell))
		return Py_BuildException();

	return Py_BuildValue("i", CPythonPlayer::Instance().FindActivatedAuraSlot(ItemCell));
}

PyObject* playerFindMoveAuraItemSlot(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonPlayer::Instance().FindMoveAuraItemSlot());
}

PyObject* playerGetAuraRefineInfo(PyObject* poSelf, PyObject* poArgs)
{
	BYTE bLevel;
	if (!PyTuple_GetByte(poArgs, 0, &bLevel))
		return Py_BuildException();

	BYTE bRefineInfoIndex;
	if (!PyTuple_GetByte(poArgs, 1, &bRefineInfoIndex))
		return Py_BuildException();

	const int* info = GetAuraRefineInfo(bLevel);
	if (!info || bRefineInfoIndex < AURA_REFINE_INFO_STEP || bRefineInfoIndex >= AURA_REFINE_INFO_MAX)
		return Py_BuildValue("i", 0);

	return Py_BuildValue("i", info[bRefineInfoIndex]);
}

PyObject* playerGetAuraRefineInfoExpPer(PyObject* poSelf, PyObject* poArgs)
{
	BYTE bAuraRefineInfoSlot;
	if (!PyTuple_GetInteger(poArgs, 0, &bAuraRefineInfoSlot))
		return Py_BuildException();

	return Py_BuildValue("i", CPythonPlayer::Instance().GetAuraRefineInfoExpPct(bAuraRefineInfoSlot));
}

PyObject* playerGetAuraRefineInfoLevel(PyObject* poSelf, PyObject* poArgs)
{
	BYTE bAuraRefineInfoSlot;
	if (!PyTuple_GetInteger(poArgs, 0, &bAuraRefineInfoSlot))
		return Py_BuildException();

	return Py_BuildValue("i", CPythonPlayer::Instance().GetAuraRefineInfoLevel(bAuraRefineInfoSlot));
}

PyObject* playerCanAuraClearItem(PyObject* poSelf, PyObject* poArgs)
{
	if (PyTuple_Size(poArgs) != 3)
		return Py_BuildException();

	int iScrollItemIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iScrollItemIndex))
		return Py_BadArgument();

	TItemPos TargetSlotIndex;
	if (!PyTuple_GetByte(poArgs, 1, &TargetSlotIndex.window_type))
		return Py_BuildException();
	if (!PyTuple_GetInteger(poArgs, 2, &TargetSlotIndex.cell))
		return Py_BuildException();

	if (iScrollItemIndex != ITEM_VNUM_AURA_CLEAR)
		return Py_BuildValue("i", DETACH_METIN_CANT);

	// Scroll
	CItemManager::Instance().SelectItemData(iScrollItemIndex);
	CItemData* pScrollItemData = CItemManager::Instance().GetSelectedItemDataPointer();
	if (!pScrollItemData)
		return Py_BuildException("Can't find item data");

	if (pScrollItemData->GetType() != CItemData::ITEM_TYPE_USE || pScrollItemData->GetSubType() != CItemData::USE_SPECIAL)
		return Py_BuildValue("i", DETACH_METIN_CANT);

	// Target Item
	int iTargetItemIndex = CPythonPlayer::Instance().GetItemIndex(TargetSlotIndex);
	CItemManager::Instance().SelectItemData(iTargetItemIndex);
	CItemData* pTargetItemData = CItemManager::Instance().GetSelectedItemDataPointer();
	if (!pTargetItemData)
		return Py_BuildException("Can't find item data");

	if (CPythonPlayer::Instance().IsEquipmentSlot(TargetSlotIndex))
		return Py_BuildValue("i", DETACH_METIN_CANT);

	if (pTargetItemData->GetType() != CItemData::ITEM_TYPE_COSTUME || pTargetItemData->GetSubType() != CItemData::COSTUME_AURA)
		return Py_BuildValue("i", DETACH_METIN_CANT);

	const TItemData* pPlayerItem = CPythonPlayer::Instance().GetItemData(TargetSlotIndex);
	if (pPlayerItem)
	{
		if (pPlayerItem->alSockets[ITEM_SOCKET_AURA_DRAIN_ITEM_VNUM] > 0)
			return Py_BuildValue("i", DETACH_METIN_OK);
		else
			return Py_BuildValue("i", DETACH_METIN_CANT);
	}

	return Py_BuildValue("i", DETACH_METIN_CANT);
}
#endif

#if defined(ENABLE_CHANGE_LOOK_SYSTEM)
PyObject* playerGetChangeLookVnum(PyObject* poSelf, PyObject* poArgs)
{
	TItemPos Cell;
	if (!PyTuple_GetByte(poArgs, 0, &Cell.window_type))
		return Py_BuildException();
	if (!PyTuple_GetInteger(poArgs, 1, &Cell.cell))
		return Py_BuildException();

	return Py_BuildValue("i", CPythonPlayer::Instance().GetItemChangeLookVnum(Cell));
}

PyObject* playerSetChangeLookWindow(PyObject* poSelf, PyObject* poArgs)
{
	bool bOpen;
	if (!PyTuple_GetBoolean(poArgs, 0, &bOpen))
		return Py_BadArgument();

	CPythonPlayer::Instance().SetChangeLookWindowOpen(bOpen);
	return Py_BuildNone();
}

PyObject* playerGetChangeLookWindowOpen(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("b", CPythonPlayer::Instance().GetChangeLookWindowOpen());
}

PyObject* playerSetChangeLookWindowType(PyObject* poSelf, PyObject* poArgs)
{
	BYTE bType;
	if (!PyTuple_GetInteger(poArgs, 0, &bType))
		return Py_BadArgument();

	CPythonPlayer::Instance().SetChangeLookWindowType(bType);
	return Py_BuildNone();
}

PyObject* playerGetChangeLookItemID(PyObject* poSelf, PyObject* poArgs)
{
	BYTE bSlotIndex;
	if (!PyTuple_GetByte(poArgs, 0, &bSlotIndex))
		return Py_BadArgument();

	if (bSlotIndex >= static_cast<BYTE>(EChangeLookSlots::CHANGE_LOOK_SLOT_MAX))
		return Py_BuildException();

	const TPacketGCChangeLookSet* c_pData = CPythonPlayer::Instance().GetChangeLookItemData(bSlotIndex);
	if (!c_pData)
		return Py_BuildValue("i", 0);

	const TItemData* c_pItemData = CPythonPlayer::Instance().GetItemData(TItemPos(INVENTORY, c_pData->wCell));
	if (!c_pItemData)
		return Py_BuildValue("i", 0);

	return Py_BuildValue("i", c_pItemData->dwVnum);
}

PyObject* playerGetChangeLookItemInvenSlot(PyObject* poSelf, PyObject* poArgs)
{
	BYTE bSlotIndex;
	if (!PyTuple_GetByte(poArgs, 0, &bSlotIndex))
		return Py_BadArgument();

	if (bSlotIndex >= static_cast<BYTE>(EChangeLookSlots::CHANGE_LOOK_SLOT_MAX))
		return Py_BuildException();

	const TPacketGCChangeLookSet* c_pData = CPythonPlayer::Instance().GetChangeLookItemData(bSlotIndex);
	if (c_pData)
		return Py_BuildValue("i", c_pData->wCell);

	return Py_BuildValue("i", 0);
}

PyObject* playerGetChangeLookFreeYangItemID(PyObject* poSelf, PyObject* poArgs)
{
	const TPacketGCChangeLookSet* c_pData = CPythonPlayer::Instance().GetChangeLookFreeItemData();
	if (!c_pData)
		return Py_BuildValue("i", 0);

	const TItemData* c_pItemData = CPythonPlayer::Instance().GetItemData(TItemPos(INVENTORY, c_pData->wCell));
	if (!c_pItemData)
		return Py_BuildValue("i", 0);

	return Py_BuildValue("i", c_pItemData->dwVnum);
}

PyObject* playerGetChangeLookFreeYangInvenSlotPos(PyObject* poSelf, PyObject* poArgs)
{
	const TPacketGCChangeLookSet* c_pData = CPythonPlayer::Instance().GetChangeLookFreeItemData();
	if (c_pData)
		return Py_BuildValue("i", c_pData->wCell);

	return Py_BuildValue("i", 0);
}

PyObject* playerCanChangeLookClearItem(PyObject* poSelf, PyObject* poArgs)
{
	int iItemVID;
	if (!PyTuple_GetInteger(poArgs, 0, &iItemVID))
		return Py_BadArgument();

	TItemPos Cell;
	if (!PyTuple_GetByte(poArgs, 1, &Cell.window_type))
		return Py_BuildException();
	if (!PyTuple_GetInteger(poArgs, 2, &Cell.cell))
		return Py_BuildException();

	CItemManager::Instance().SelectItemData(iItemVID);
	CItemData* pSrcItemData = CItemManager::Instance().GetSelectedItemDataPointer();
	if (!pSrcItemData)
		return Py_BuildException("Can't find select item data");

	CItemManager::Instance().SelectItemData(CPythonPlayer::Instance().GetItemIndex(Cell));
	CItemData* pDestItemData = CItemManager::Instance().GetSelectedItemDataPointer();
	if (!pDestItemData)
		return Py_BuildException("Can't find select item data");

	if (pSrcItemData->GetIndex() == static_cast<DWORD>(EChangeLookItems::CHANGE_LOOK_MOUNT_REVERSAL))
	{
		if (!pDestItemData->IsCostumeMount())
			return Py_BuildValue("b", false);
	}
	else
	{
		if (pDestItemData->IsCostumeMount())
			return Py_BuildValue("b", false);
	}

	const TItemData* c_pDestItemData = CPythonPlayer::Instance().GetItemData(Cell);
	if (c_pDestItemData)
		return Py_BuildValue("b", c_pDestItemData->dwTransmutationVnum != 0);

	return Py_BuildValue("b", false);
}

PyObject* playerGetChangeWIndowChangeLookVnum(PyObject* poSelf, PyObject* poArgs)
{
	BYTE bSlotIndex;
	if (!PyTuple_GetByte(poArgs, 0, &bSlotIndex))
		return Py_BadArgument();

	if (bSlotIndex >= static_cast<BYTE>(EChangeLookSlots::CHANGE_LOOK_SLOT_MAX))
		return Py_BuildException();

	const TPacketGCChangeLookSet* c_pData = CPythonPlayer::Instance().GetChangeLookItemData(bSlotIndex);
	if (!c_pData)
		return Py_BuildValue("i", 0);

	const TItemData* c_pItemData = CPythonPlayer::Instance().GetItemData(TItemPos(INVENTORY, c_pData->wCell));
	if (!c_pItemData)
		return Py_BuildValue("i", 0);

	return Py_BuildValue("i", c_pItemData->dwTransmutationVnum);
}

PyObject* playerGetChangeChangeLookPrice(PyObject* poSelf, PyObject* poArgs)
{
	const BYTE c_bType = CPythonPlayer::Instance().GetChangeLookWindowType();
	switch (static_cast<EChangeLookType>(c_bType))
	{
		case EChangeLookType::CHANGE_LOOK_TYPE_ITEM:
			return Py_BuildValue("i", static_cast<DWORD>(EChangeLookPrice::CHANGE_LOOK_ITEM_PRICE));
		case EChangeLookType::CHANGE_LOOK_TYPE_MOUNT:
			return Py_BuildValue("i", static_cast<DWORD>(EChangeLookPrice::CHANGE_LOOK_MOUNT_PRICE));
	}
	return Py_BuildValue("i", 0);
}
#endif

PyObject* playerGetAffectData(PyObject* poSelf, PyObject* poArgs)
{
	int iAffectType;
	if (!PyTuple_GetInteger(poArgs, 0, &iAffectType))
		return Py_BadArgument();

	PyObject* poDict = PyDict_New();
	if (poDict == NULL)
		return Py_BuildException();

	for (const CPythonPlayer::TAffectDataVector::value_type& it : CPythonPlayer::Instance().GetAffectDataVector(iAffectType))
	{
		PyDict_SetItem(poDict,
			Py_BuildValue("i", PointTypeToApplyType(it.wApplyOn)),
			Py_BuildValue("i", it.lApplyValue));
	}

	PyObject* pResult = Py_BuildValue("O", poDict);
	if (!pResult)
	{
		Py_DECREF(poDict);
		return Py_BuildException();
	}

	return pResult;
}

PyObject* playerEmptyAffectData(PyObject* poSelf, PyObject* poArgs)
{
	int iAffectType;
	if (!PyTuple_GetInteger(poArgs, 0, &iAffectType))
		return Py_BadArgument();

	return Py_BuildValue("b", CPythonPlayer::Instance().GetAffectDataIndex(iAffectType, 0) == -1);
}

#if defined(ENABLE_SET_ITEM)
PyObject* playerGetItemSetValue(PyObject* poSelf, PyObject* poArgs)
{
	TItemPos Cell;
	switch (PyTuple_Size(poArgs))
	{
		case 1:
			if (!PyTuple_GetInteger(poArgs, 0, &Cell.cell))
				return Py_BadArgument();
			break;
		case 2:
			if (!PyTuple_GetByte(poArgs, 0, &Cell.window_type))
				return Py_BadArgument();
			if (!PyTuple_GetInteger(poArgs, 1, &Cell.cell))
				return Py_BadArgument();
			break;
		default:
			return Py_BuildException();
	}

	return Py_BuildValue("i", CPythonPlayer::Instance().GetItemSetValue(Cell));
}

PyObject* playerCanSetItemClear(PyObject* poSelf, PyObject* poArgs)
{
	int iScrollVNum;
	TItemPos Cell;
	switch (PyTuple_Size(poArgs))
	{
		case 2:
			if (!PyTuple_GetInteger(poArgs, 0, &iScrollVNum))
				return Py_BadArgument();
			if (!PyTuple_GetInteger(poArgs, 1, &Cell.cell))
				return Py_BadArgument();
			break;

		case 3:
			if (!PyTuple_GetInteger(poArgs, 0, &iScrollVNum))
				return Py_BadArgument();
			if (!PyTuple_GetInteger(poArgs, 1, &Cell.window_type))
				return Py_BadArgument();
			if (!PyTuple_GetInteger(poArgs, 2, &Cell.cell))
				return Py_BadArgument();
			break;

		default:
			return Py_BuildException();
	}

	// Scroll
	CItemManager::Instance().SelectItemData(iScrollVNum);
	CItemData* pSealScrollItemData = CItemManager::Instance().GetSelectedItemDataPointer();
	if (!pSealScrollItemData)
		return Py_BuildException("Can't find item data");

	// Destination ItemData
	CItemManager::Instance().SelectItemData(CPythonPlayer::Instance().GetItemIndex(Cell));
	CItemData* pDestItemData = CItemManager::Instance().GetSelectedItemDataPointer();
	if (!pDestItemData)
		return Py_BuildException("Can't find item data");

	// Destination Item
	const TItemData* pDestItem = CPythonPlayer::Instance().GetItemData(Cell);
	if (!pDestItem)
		return Py_BuildException("Cell(%d, %d) item is null", Cell.window_type, Cell.cell);

	// Bind-Item
	if ((pSealScrollItemData->GetType() == CItemData::ITEM_TYPE_USE) || (pSealScrollItemData->GetSubType() == CItemData::USE_SPECIAL) || (pSealScrollItemData->GetIndex() == ITEM_VNUM_SET_CLEAR_SCROLL))
	{
		if (pDestItem->bSetValue)
			return Py_BuildValue("i", TRUE);
	}

	return Py_BuildValue("i", FALSE);
}
#endif

#if defined(ENABLE_REFINE_ELEMENT_SYSTEM)
PyObject* playerGetItemRefineElement(PyObject* poSelf, PyObject* poArgs)
{
	TItemPos Cell;
	switch (PyTuple_Size(poArgs))
	{
		case 1:
			if (!PyTuple_GetInteger(poArgs, 0, &Cell.cell))
				return Py_BadArgument();
			break;
		case 2:
			if (!PyTuple_GetByte(poArgs, 0, &Cell.window_type))
				return Py_BadArgument();
			if (!PyTuple_GetInteger(poArgs, 1, &Cell.cell))
				return Py_BadArgument();
			break;
		default:
			return Py_BuildException();
	}

	const TPlayerItemRefineElement* pTable = CPythonPlayer::Instance().GetItemRefineElement(Cell);
	if (pTable == nullptr)
		return Py_BuildValue("(iiOO)", 0, 0, Py_BuildValue("iii", 0, 0, 0), Py_BuildValue("iii", 0, 0, 0));

	return Py_BuildValue("(iiOO)", pTable->wApplyType, pTable->bGrade,
		Py_BuildValue("iii", pTable->abValue[0], pTable->abValue[1], pTable->abValue[2]),
		Py_BuildValue("iii", pTable->abBonusValue[0], pTable->abBonusValue[1], pTable->abBonusValue[2]));
}
#endif

#if defined(ENABLE_MONSTER_CARD)
PyObject* playerIllustrationSelectModel(PyObject* poSelf, PyObject* poArgs)
{
	int iVnum;
	if (!PyTuple_GetInteger(poArgs, 0, &iVnum))
		return Py_BadArgument();

	return Py_BuildValue("b", CPythonIllustratedManager::Instance().SelectModel(iVnum));
}

PyObject* playerIllustrationShow(PyObject* poSelf, PyObject* poArgs)
{
	bool isShow;
	if (!PyTuple_GetBoolean(poArgs, 0, &isShow))
		return Py_BadArgument();

	CPythonIllustratedManager::Instance().SetShow(isShow);
	return Py_BuildNone();
}

PyObject* playerIllustrationChangeMotion(PyObject* poSelf, PyObject* poArgs)
{
	int iVnum;
	if (!PyTuple_GetInteger(poArgs, 0, &iVnum))
		return Py_BadArgument();

	CPythonIllustratedManager::Instance().ChangeMotion(iVnum);
	return Py_BuildNone();
}

PyObject* playerIllustrationModelRotation(PyObject* poSelf, PyObject* poArgs)
{
	float fRot;
	if (!PyTuple_GetFloat(poArgs, 0, &fRot))
		return Py_BadArgument();

	CPythonIllustratedManager::Instance().ModelRotation(fRot);
	return Py_BuildNone();
}

PyObject* playerIllustrationModelUpDown(PyObject* poSelf, PyObject* poArgs)
{
	bool bUp;
	if (!PyTuple_GetBoolean(poArgs, 0, &bUp))
		return Py_BadArgument();

	CPythonIllustratedManager::Instance().ModelUpDown(bUp);
	return Py_BuildNone();
}

PyObject* playerIllustrationModelZoom(PyObject* poSelf, PyObject* poArgs)
{
	bool bZoom;
	if (!PyTuple_GetBoolean(poArgs, 0, &bZoom))
		return Py_BadArgument();

	CPythonIllustratedManager::Instance().ModelZoom(bZoom);
	return Py_BuildNone();
}

PyObject* playerIllustrationModelViewReset(PyObject* poSelf, PyObject* poArgs)
{
	CPythonIllustratedManager::Instance().ModelViewReset();
	return Py_BuildNone();
}
#endif

#if defined(ENABLE_MYSHOP_DECO)
PyObject* playerMyShopDecoShow(PyObject* poSelf, PyObject* poArgs)
{
	bool bShow;
	if (!PyTuple_GetBoolean(poArgs, 0, &bShow))
		return Py_BadArgument();

	CPythonMyShopDecoManager::Instance().SetShow(bShow);
	return Py_BuildNone();
}

PyObject* playerSelectShopModel(PyObject* poSelf, PyObject* poArgs)
{
	int iVnum;
	if (!PyTuple_GetInteger(poArgs, 0, &iVnum))
		return Py_BadArgument();

	CPythonMyShopDecoManager::Instance().SelectModel(iVnum);
	return Py_BuildNone();
}
#endif

#if defined(ENABLE_MINI_GAME_RUMI)
// Rumi Game
PyObject* playerSetRumiGame(PyObject* poSelf, PyObject* poArgs)
{
	bool bEnable;
	if (!PyTuple_GetBoolean(poArgs, 0, &bEnable))
		return Py_BadArgument();

	CPythonPlayer::Instance().SetRumiGame(bEnable);
	return Py_BuildNone();
}

PyObject* playerGetRumiGame(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("b", CPythonPlayer::Instance().GetRumiGame());
}

PyObject* playerSetMiniGameOkeyNormal(PyObject* poSelf, PyObject* poArgs)
{
	bool bEnable;
	if (!PyTuple_GetBoolean(poArgs, 0, &bEnable))
		return Py_BadArgument();

	CPythonPlayer::Instance().SetMiniGameOkeyNormal(bEnable);
	return Py_BuildNone();
}

PyObject* playerGetMiniGameOkeyNormal(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("b", CPythonPlayer::Instance().GetMiniGameOkeyNormal());
}
#endif

#if defined(ENABLE_MINI_GAME_YUTNORI)
PyObject* playerYutnoriShow(PyObject* poSelf, PyObject* poArgs)
{
	bool bShow;
	if (!PyTuple_GetBoolean(poArgs, 0, &bShow))
		return Py_BadArgument();

	CPythonYutnoriManager::Instance().SetShow(bShow);
	return Py_BuildNone();
}

PyObject* playerYutnoriChangeMotion(PyObject* poSelf, PyObject* poArgs)
{
	int iMotionIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iMotionIndex))
		return Py_BadArgument();

	CPythonYutnoriManager::Instance().ChangeMotion(iMotionIndex);
	return Py_BuildNone();
}
#endif

#if defined(ENABLE_MINI_GAME_CATCH_KING)
// Catch King Game
PyObject* playerSetCatchKingGame(PyObject* poSelf, PyObject* poArgs)
{
	bool bEnable;
	if (!PyTuple_GetBoolean(poArgs, 0, &bEnable))
		return Py_BadArgument();

	CPythonPlayer::Instance().SetCatchKingGame(bEnable);
	return Py_BuildNone();
}

PyObject* playerGetCatchKingGame(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("b", CPythonPlayer::Instance().GetCatchKingGame());
}
#endif

#if defined(ENABLE_LUCKY_BOX)
PyObject* playerSendLuckyBoxAction(PyObject* poSelf, PyObject* poArgs)
{
	BYTE bAction;
	if (!PyTuple_GetByte(poArgs, 0, &bAction))
		return Py_BuildException();

	CPythonNetworkStream::Instance().SendLuckyBoxActionPacket(bAction);
	return Py_BuildNone();
}
#endif

#if defined(ENABLE_FISHING_GAME)
PyObject* playerFishingGameGoal(PyObject* poSelf, PyObject* poArgs)
{
	BYTE bCount;
	if (!PyTuple_GetInteger(poArgs, 0, &bCount))
		return Py_BadArgument();

	CPythonNetworkStream& rkNetStream = CPythonNetworkStream::Instance();
	rkNetStream.SendFishingGamePacket(FISHING_GAME_SUBHEADER_GOAL, bCount);
	return Py_BuildNone();
}

PyObject* playerFishingGameQuit(PyObject* poSelf, PyObject* poArgs)
{
	CPythonNetworkStream& rkNetStream = CPythonNetworkStream::Instance();
	rkNetStream.SendFishingGamePacket(FISHING_GAME_SUBHEADER_QUIT);
	return Py_BuildNone();
}
#endif

#if defined(ENABLE_GEM_SYSTEM)
PyObject* playerGetGemShopItemID(PyObject* poSelf, PyObject* poArgs)
{
	BYTE bSlotIndex;
	if (!PyTuple_GetByte(poArgs, 0, &bSlotIndex))
		return Py_BadArgument();

	if (bSlotIndex > GEM_SHOP_SLOT_COUNT)
		return Py_BuildValue("iibi", 0, 0, false, 0);

	const TGemShopItem& c_rTable = CPythonPlayer::Instance().GetGemShopItem(bSlotIndex);
	return Py_BuildValue("iibi", c_rTable.dwItemVnum, c_rTable.dwPrice, c_rTable.bEnable, c_rTable.bCount);
}

PyObject* playerGetGemShopOpenSlotCount(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonPlayer::Instance().GetGemShopOpenSlotCount());
}

PyObject* playerGetGemShopOpenSlotItemCount(PyObject* poSelf, PyObject* poArgs)
{
	BYTE bOpenSlotCount;
	if (!PyTuple_GetByte(poArgs, 0, &bOpenSlotCount))
		return Py_BadArgument();

	BYTE bOpenSlotItemCount = 0;
	switch (bOpenSlotCount)
	{
		case 3: bOpenSlotItemCount = 1; break;
		case 4: bOpenSlotItemCount = 2; break;
		case 5: bOpenSlotItemCount = 4; break;
		case 6:
		case 7:
		case 8:
			bOpenSlotItemCount = 8;
			break;
		default:
			bOpenSlotItemCount = 0;
			break;
	}

	return Py_BuildValue("i", bOpenSlotItemCount);
}

PyObject* playerGetGemShopRefreshTime(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("l", CPythonPlayer::Instance().GetGemShopRefreshTime());
}

PyObject* playerSetGemShopWindowOpen(PyObject* poSelf, PyObject* poArgs)
{
	bool bOpen;
	if (!PyTuple_GetBoolean(poArgs, 0, &bOpen))
		return Py_BadArgument();

	CPythonPlayer& rkPlayer = CPythonPlayer::Instance();
	rkPlayer.SetGemShopWindowOpen(bOpen);

	return Py_BuildNone();
}

PyObject* playerIsGemShopWindowOpen(PyObject* poSelf, PyObject* poArgs)
{
	CPythonPlayer& rkPlayer = CPythonPlayer::Instance();
	return Py_BuildValue("b", rkPlayer.IsGemShopWindowOpen());
}
#endif

PyObject* playerNPOS(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("(ii)", NPOS.window_type, NPOS.cell);
}

#if defined(ENABLE_EXTEND_INVEN_SYSTEM)
PyObject* playerGetExtendInvenStage(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonPlayer::Instance().GetExtendInvenStage());
}

PyObject* playerGetExtendInvenMax(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonPlayer::Instance().GetExtendInvenMax());
}
#endif

#if defined(ENABLE_EXPRESSING_EMOTION)
PyObject* playerClearSpecialActionSlot(PyObject* poSelf, PyObject* poArgs)
{
	BYTE bSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &bSlotIndex))
		return Py_BuildException();

	CPythonPlayer& rkPlayer = CPythonPlayer::Instance();
	rkPlayer.ClearSpecialActionSlot(bSlotIndex);
	return Py_BuildNone();
}

PyObject* playerSetSpecialActionSlot(PyObject* poSelf, PyObject* poArgs)
{
	BYTE bSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &bSlotIndex))
		return Py_BuildException();

	BYTE bEmotionIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &bEmotionIndex))
		return Py_BuildException();

	DWORD dwEmotionIndex;
	if (!PyTuple_GetUnsignedLong(poArgs, 2, &dwEmotionIndex))
		return Py_BuildException();

	CPythonPlayer& rkPlayer = CPythonPlayer::Instance();
	rkPlayer.SetSpecialActionSlot(bSlotIndex, bEmotionIndex, dwEmotionIndex);
	return Py_BuildNone();
}

PyObject* playerGetSpecialActionSlot(PyObject* poSelf, PyObject* poArgs)
{
	BYTE bSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &bSlotIndex))
		return Py_BuildException();

	return Py_BuildValue("i", CPythonPlayer::Instance().GetSpecialActionSlot(bSlotIndex));
}

PyObject* playerFindSpecialActionSlot(PyObject* poSelf, PyObject* poArgs)
{
	BYTE bEmotionIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &bEmotionIndex))
		return Py_BuildException();

	BYTE bSlotIndex; DWORD dwEmotionDuration;
	CPythonPlayer::Instance().FindSpecialActionSlot(bEmotionIndex, &bSlotIndex, &dwEmotionDuration);
	return Py_BuildValue("ii", bSlotIndex, dwEmotionDuration);
}
#endif

#if defined(ENABLE_CUBE_RENEWAL)
PyObject* playerGetCubeItem(PyObject* poSelf, PyObject* poArgs)
{
	int iNPCVnum;
	if (!PyTuple_GetInteger(poArgs, 0, &iNPCVnum))
		return Py_BuildException();

	PyObject* poCubeList = PyList_New(0);
	if (!poCubeList)
		return Py_BuildException();

	CPythonCubeManager& rkCubeMgr = CPythonCubeManager::Instance();
	for (const CPythonCubeManager::CubeDataPtr& it : rkCubeMgr.GetCubeData(iNPCVnum))
	{
		PyObject* poCubeDict = PyDict_New();
		if (!poCubeDict)
		{
			Py_DECREF(poCubeList);
			return Py_BuildException();
		}

		PyDict_SetItemString(poCubeDict, "index", Py_BuildValue("i", it->iIndex));
		PyDict_SetItemString(poCubeDict, "npc", Py_BuildValue("i", iNPCVnum));

		PyObject* poCubeItemList = PyList_New(0);
		if (!poCubeItemList)
		{
			Py_DECREF(poCubeDict);
			Py_DECREF(poCubeList);
			return Py_BuildException();
		}

		for (const CPythonCubeManager::SCubeValue& rkValue : it->vItem)
		{
			PyObject* poCubeItemDict = PyDict_New();
			if (!poCubeItemDict)
			{
				Py_DECREF(poCubeItemList);
				Py_DECREF(poCubeDict);
				Py_DECREF(poCubeList);
				return Py_BuildException();
			}

			PyDict_SetItemString(poCubeItemDict, "vnum", Py_BuildValue("i", rkValue.dwVnum));
			PyDict_SetItemString(poCubeItemDict, "count", Py_BuildValue("i", rkValue.iCount));
			PyList_Append(poCubeItemList, poCubeItemDict);
		}

		PyDict_SetItemString(poCubeDict, "item", poCubeItemList);
		Py_DECREF(poCubeItemList);

		PyObject* poCubeRewardDict = PyDict_New();
		if (!poCubeRewardDict)
		{
			Py_DECREF(poCubeDict);
			Py_DECREF(poCubeList);
			return Py_BuildException();
		}

		PyDict_SetItemString(poCubeRewardDict, "vnum", Py_BuildValue("i", it->Reward.dwVnum));
		PyDict_SetItemString(poCubeRewardDict, "count", Py_BuildValue("i", it->Reward.iCount));
		PyDict_SetItemString(poCubeDict, "reward", poCubeRewardDict);
		Py_DECREF(poCubeRewardDict);

		PyDict_SetItemString(poCubeDict, "percent", Py_BuildValue("i", it->iPercent));
		PyDict_SetItemString(poCubeDict, "gold", Py_BuildValue("i", it->iGold));
		PyDict_SetItemString(poCubeDict, "gem", Py_BuildValue("i", it->iGem));
#if defined(ENABLE_SET_ITEM)
		PyDict_SetItemString(poCubeDict, "set_value", Py_BuildValue("i", it->iSetValue));
		PyDict_SetItemString(poCubeDict, "not_remove", Py_BuildValue("i", it->dwNotRemove));
#endif
		PyDict_SetItemString(poCubeDict, "category", Py_BuildValue("i", it->iCategory));
		PyDict_SetItemString(poCubeDict, "subcategory", Py_BuildValue("i", it->iSubCategory));

		PyList_Append(poCubeList, poCubeDict);
		Py_DECREF(poCubeDict);
	}

	return poCubeList;
}

PyObject* playerGetCubeListSize(PyObject* poSelf, PyObject* poArgs)
{
	int iNPCVnum;
	if (!PyTuple_GetInteger(poArgs, 0, &iNPCVnum))
		return Py_BuildException();

	CPythonCubeManager& rkCubeMgr = CPythonCubeManager::Instance();
	return Py_BuildValue("i", rkCubeMgr.GetCubeData(iNPCVnum).size());
}

PyObject* playerSetCubeWindowOpen(PyObject* poSelf, PyObject* poArgs)
{
	bool bSet;
	if (!PyTuple_GetBoolean(poArgs, 0, &bSet))
		return Py_BuildException();

	CPythonPlayer& rkPlayer = CPythonPlayer::Instance();
	rkPlayer.SetCubeWindowOpen(bSet);
	return Py_BuildNone();
}

PyObject* playerIsCubeSetAddCategory(PyObject* poSelf, PyObject* poArgs)
{
	BYTE bCategory;
	if (!PyTuple_GetByte(poArgs, 0, &bCategory))
		return Py_BuildException();

	CPythonCubeManager& rkCubeMgr = CPythonCubeManager::Instance();
	return Py_BuildValue("b", rkCubeMgr.IsCubeSetAddCategory(bCategory));
}
#endif

#if defined(ENABLE_SNOWFLAKE_STICK_EVENT)
// Snowflake Stick Event
PyObject* playerSetSnowflakeStickEvent(PyObject* poSelf, PyObject* poArgs)
{
	int iEndTime;
	if (!PyTuple_GetInteger(poArgs, 0, &iEndTime))
		return Py_BadArgument();

	CPythonPlayer::Instance().SetSnowflakeStickEvent(iEndTime);
	return Py_BuildNone();
}

PyObject* playerGetSnowflakeStickEvent(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonPlayer::Instance().GetSnowflakeStickEvent());
}
#endif

#if defined(ENABLE_GAME_OPTION_ESCAPE)
PyObject* playerCanEscapeState(PyObject* poSelf, PyObject* poArgs)
{
	if (CPythonPlayer::Instance().IsDead() || CPythonPlayer::Instance().IsPoly())
		return Py_BuildValue("b", false);

	return Py_BuildValue("b", true);
}
#endif

#if defined(ENABLE_FLOWER_EVENT)
PyObject* playerSetFlowerEventEnable(PyObject* poSelf, PyObject* poArgs)
{
	bool bEnable;
	if (!PyTuple_GetBoolean(poArgs, 0, &bEnable))
		return Py_BuildException();

	CPythonPlayer::Instance().SetFlowerEventEnable(bEnable);
	return Py_BuildNone();
}

PyObject* playerGetFlowerEventEnable(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("b", CPythonPlayer::Instance().GetFlowerEventEnable());
}
#endif

void initPlayer()
{
	static PyMethodDef s_methods[] =
	{
		{ "GetAutoPotionInfo", playerGetAutoPotionInfo, METH_VARARGS },
		{ "SetAutoPotionInfo", playerSetAutoPotionInfo, METH_VARARGS },

		{ "PickCloseItem", playerPickCloseItem, METH_VARARGS },
		{ "SetGameWindow", playerSetGameWindow, METH_VARARGS },
		{ "RegisterEffect", playerRegisterEffect, METH_VARARGS },
		{ "RegisterCacheEffect", playerRegisterCacheEffect, METH_VARARGS },
		{ "SetMouseState", playerSetMouseState, METH_VARARGS },
		{ "SetMouseFunc", playerSetMouseFunc, METH_VARARGS },
		{ "GetMouseFunc", playerGetMouseFunc, METH_VARARGS },
		{ "SetMouseMiddleButtonState", playerSetMouseMiddleButtonState, METH_VARARGS },
		{ "SetMainCharacterIndex", playerSetMainCharacterIndex, METH_VARARGS },
		{ "GetMainCharacterIndex", playerGetMainCharacterIndex, METH_VARARGS },
		{ "GetMainCharacterName", playerGetMainCharacterName, METH_VARARGS },
		{ "GetMainCharacterPosition", playerGetMainCharacterPosition, METH_VARARGS },
		{ "IsMainCharacterIndex", playerIsMainCharacterIndex, METH_VARARGS },
		{ "CanAttackInstance", playerCanAttackInstance, METH_VARARGS },
		{ "IsActingTargetEmotion", playerIsActingTargetEmotion, METH_VARARGS },
		{ "IsActingEmotion", playerIsActingEmotion, METH_VARARGS },
		{ "IsPVPInstance", playerIsPVPInstance, METH_VARARGS },
		{ "IsSameEmpire", playerIsSameEmpire, METH_VARARGS },
		{ "IsChallengeInstance", playerIsChallengeInstance, METH_VARARGS },
		{ "IsRevengeInstance", playerIsRevengeInstance, METH_VARARGS },
		{ "IsCantFightInstance", playerIsCantFightInstance, METH_VARARGS },
		{ "GetCharacterDistance", playerGetCharacterDistance, METH_VARARGS },
		{ "IsInSafeArea", playerIsInSafeArea, METH_VARARGS },
		{ "IsMountingHorse", playerIsMountingHorse, METH_VARARGS },
		{ "IsObserverMode", playerIsObserverMode, METH_VARARGS },
		{ "ActEmotion", playerActEmotion, METH_VARARGS },

		{ "ShowPlayer", playerShowPlayer, METH_VARARGS },
		{ "HidePlayer", playerHidePlayer, METH_VARARGS },

		{ "ComboAttack", playerComboAttack, METH_VARARGS },

		{ "SetAutoCameraRotationSpeed", playerSetAutoCameraRotationSpeed, METH_VARARGS },
		{ "SetAttackKeyState", playerSetAttackKeyState, METH_VARARGS },
		{ "SetSingleDIKKeyState", playerSetSingleDIKKeyState, METH_VARARGS },
		{ "EndKeyWalkingImmediately", playerEndKeyWalkingImmediately, METH_VARARGS },
		{ "StartMouseWalking", playerStartMouseWalking, METH_VARARGS },
		{ "EndMouseWalking", playerEndMouseWalking, METH_VARARGS },
		{ "ResetCameraRotation", playerResetCameraRotation, METH_VARARGS },
		{ "SetQuickCameraMode", playerSetQuickCameraMode, METH_VARARGS },

		///////////////////////////////////////////////////////////////////////////////////////////

		{ "SetSkill", playerSetSkill, METH_VARARGS },
		{ "GetSkillIndex", playerGetSkillIndex, METH_VARARGS },
		{ "GetSkillSlotIndex", playerGetSkillSlotIndex, METH_VARARGS },
		{ "GetSkillGrade", playerGetSkillGrade, METH_VARARGS },
		{ "GetSkillLevel", playerGetSkillLevel, METH_VARARGS },
		{ "GetSkillCurrentEfficientPercentage", playerGetSkillCurrentEfficientPercentage, METH_VARARGS },
		{ "GetSkillNextEfficientPercentage", playerGetSkillNextEfficientPercentage, METH_VARARGS },
		{ "ClickSkillSlot", playerClickSkillSlot, METH_VARARGS },
		{ "ChangeCurrentSkillNumberOnly", playerChangeCurrentSkillNumberOnly, METH_VARARGS },
		{ "ClearSkillDict", playerClearSkillDict, METH_VARARGS },

		{ "GetItemIndex", playerGetItemIndex, METH_VARARGS },
		{ "GetItemFlags", playerGetItemFlags, METH_VARARGS },
		{ "IsAntiFlagBySlot", playerIsAntiFlagBySlot, METH_VARARGS },
		{ "GetItemTypeBySlot", playerGetItemTypeBySlot, METH_VARARGS },
		{ "GetItemSubTypeBySlot", playerGetItemSubTypeBySlot, METH_VARARGS },
		{ "IsSameItemVnum", playerIsSameItemVnum, METH_VARARGS },
		{ "GetItemCount", playerGetItemCount, METH_VARARGS },
		{ "GetItemCountByVnum", playerGetItemCountByVnum, METH_VARARGS },
		{ "GetItemMetinSocket", playerGetItemMetinSocket, METH_VARARGS },
#if defined(ENABLE_APPLY_RANDOM)
		{ "GetItemApplyRandom", playerGetItemApplyRandom, METH_VARARGS },
#endif
		{ "GetItemAttribute", playerGetItemAttribute, METH_VARARGS },
		{ "GetISellItemPrice", playerGetISellItemPrice, METH_VARARGS },
		{ "MoveItem", playerMoveItem, METH_VARARGS },
		{ "SendClickItemPacket", playerSendClickItemPacket, METH_VARARGS },

#if defined(ENABLE_SOULBIND_SYSTEM)
		{ "CanSealItem", playerCanSealItem, METH_VARARGS },
		{ "GetItemSealDate", playerGetItemSealDate, METH_VARARGS },
		{ "GetItemUnSealLeftTime", playerGetItemUnSealLeftTime, METH_VARARGS },
		{ "IsSealedItemBySlot", playerIsSealedItemBySlot, METH_VARARGS },
#endif

		///////////////////////////////////////////////////////////////////////////////////////////

		{ "GetName", playerGetName, METH_VARARGS },
		{ "GetLevel", playerGetLevel, METH_VARARGS },
		{ "GetJob", playerGetJob, METH_VARARGS },
		{ "GetRace", playerGetRace, METH_VARARGS },
		{ "GetPlayTime", playerGetPlayTime, METH_VARARGS },
		{ "SetPlayTime", playerSetPlayTime, METH_VARARGS },

		{ "IsSkillCoolTime", playerIsSkillCoolTime, METH_VARARGS },
		{ "GetSkillCoolTime", playerGetSkillCoolTime, METH_VARARGS },
		{ "IsSkillActive", playerIsSkillActive, METH_VARARGS },
		{ "UseGuildSkill", playerUseGuildSkill, METH_VARARGS },
		{ "AffectIndexToSkillIndex", playerAffectIndexToSkillIndex, METH_VARARGS },
		{ "GetEXP", playerGetEXP, METH_VARARGS },
		{ "GetStatus", playerGetStatus, METH_VARARGS },
		{ "SetStatus", playerSetStatus, METH_VARARGS },
		{ "GetElk", playerGetElk, METH_VARARGS },
#if defined(ENABLE_CHEQUE_SYSTEM)
		{ "GetCheque", playerGetCheque, METH_VARARGS },
#endif
#if defined(ENABLE_GEM_SYSTEM)
		{ "GetGem", playerGetGem, METH_VARARGS },
#endif
		{ "GetMoney", playerGetElk, METH_VARARGS },
		{ "GetGuildID", playerGetGuildID, METH_VARARGS },
		{ "GetGuildName", playerGetGuildName, METH_VARARGS },
		{ "GetAlignmentData", playerGetAlignmentData, METH_VARARGS },
		{ "RequestAddLocalQuickSlot", playerRequestAddLocalQuickSlot, METH_VARARGS },
		{ "RequestAddToEmptyLocalQuickSlot", playerRequestAddToEmptyLocalQuickSlot, METH_VARARGS },
		{ "RequestDeleteGlobalQuickSlot", playerRequestDeleteGlobalQuickSlot, METH_VARARGS },
		{ "RequestMoveGlobalQuickSlotToLocalQuickSlot", playerRequestMoveGlobalQuickSlotToLocalQuickSlot, METH_VARARGS },
		{ "RequestUseLocalQuickSlot", playerRequestUseLocalQuickSlot, METH_VARARGS },
		{ "LocalQuickSlotIndexToGlobalQuickSlotIndex", playerLocalQuickSlotIndexToGlobalQuickSlotIndex, METH_VARARGS },

		{ "GetQuickPage", playerGetQuickPage, METH_VARARGS },
		{ "SetQuickPage", playerSetQuickPage, METH_VARARGS },
		{ "GetLocalQuickSlot", playerGetLocalQuickSlot, METH_VARARGS },
		{ "GetGlobalQuickSlot", playerGetGlobalQuickSlot, METH_VARARGS },
		{ "RemoveQuickSlotByValue", playerRemoveQuickSlotByValue, METH_VARARGS },

		{ "isItem", playerisItem, METH_VARARGS },
		{ "IsEquipmentSlot", playerIsEquipmentSlot, METH_VARARGS },
		{ "IsDSEquipmentSlot", playerIsDSEquipmentSlot, METH_VARARGS },
		{ "IsCostumeSlot", playerIsCostumeSlot, METH_VARARGS },
		{ "IsValuableItem", playerIsValuableItem, METH_VARARGS },
		{ "IsOpenPrivateShop", playerIsOpenPrivateShop, METH_VARARGS },
		{ "IsDead", playerIsDead, METH_VARARGS },
		{ "IsPoly", playerIsPoly, METH_VARARGS },
		{ "IsOpenSafeBox", playerIsOpenSafeBox, METH_VARARGS },
		{ "SetOpenSafeBox", playerSetOpenSafeBox, METH_VARARGS },
		{ "IsOpenMall", playerIsOpenMall, METH_VARARGS },
		{ "SetOpenMall", playerSetOpenMall, METH_VARARGS },

#if defined(ENABLE_NEW_EQUIPMENT_SYSTEM)
		{ "IsBeltInventorySlot", playerIsBeltInventorySlot, METH_VARARGS },
		{ "IsEquippingBelt", playerIsEquippingBelt, METH_VARARGS },
		{ "IsAvailableBeltInventoryCell", playerIsAvailableBeltInventoryCell, METH_VARARGS },
#endif

		// Refine
		{ "GetItemGrade", playerGetItemGrade, METH_VARARGS },
		{ "CanRefine", playerCanRefine, METH_VARARGS },
		{ "CanDetach", playerCanDetach, METH_VARARGS },
		{ "CanUnlock", playerCanUnlock, METH_VARARGS },
		{ "CanAttachMetin", playerCanAttachMetin, METH_VARARGS },
		{ "IsRefineGradeScroll", playerIsRefineGradeScroll, METH_VARARGS },

		{ "ClearTarget", playerClearTarget, METH_VARARGS },
		{ "SetTarget", playerSetTarget, METH_VARARGS },
		{ "OpenCharacterMenu", playerOpenCharacterMenu, METH_VARARGS },

		{ "Update", playerUpdate, METH_VARARGS },
		{ "Render", playerRender, METH_VARARGS },
		{ "Clear", playerClear, METH_VARARGS },

		// Party
		{ "IsPartyMember", playerIsPartyMember, METH_VARARGS },
		{ "IsPartyLeader", playerIsPartyLeader, METH_VARARGS },
		{ "IsPartyLeaderByPID", playerIsPartyLeaderByPID, METH_VARARGS },
		{ "GetPartyMemberHPPercentage",	playerGetPartyMemberHPPercentage, METH_VARARGS },
		{ "GetPartyMemberState", playerGetPartyMemberState, METH_VARARGS },
		{ "GetPartyMemberAffects", playerGetPartyMemberAffects, METH_VARARGS },
		{ "RemovePartyMember", playerRemovePartyMember, METH_VARARGS },
		{ "ExitParty", playerExitParty, METH_VARARGS },
		{ "PartyMemberVIDToPID", playerPartyMemberVIDToPID, METH_VARARGS },

		// PK Mode
		{ "GetPKMode", playerGetPKMode, METH_VARARGS },

		// Emotion
		{ "RegisterEmotionIcon", playerRegisterEmotionIcon, METH_VARARGS },
		{ "GetEmotionIconImage", playerGetEmotionIconImage, METH_VARARGS },
		{ "ClearEmotionIcon", playerClearEmotionIcon, METH_VARARGS },

		// For System
		{ "SetWeaponAttackBonusFlag", playerSetWeaponAttackBonusFlag, METH_VARARGS },
		{ "ToggleCoolTime", playerToggleCoolTime, METH_VARARGS },
		{ "ToggleLevelLimit", playerToggleLevelLimit, METH_VARARGS },
		{ "GetTargetVID", playerGetTargetVID, METH_VARARGS },

		{ "SetItemData", playerSetItemData, METH_VARARGS },
		{ "SetItemMetinSocket", playerSetItemMetinSocket, METH_VARARGS },
		{ "SetItemAttribute", playerSetItemAttribute, METH_VARARGS },
		{ "SetItemCount", playerSetItemCount, METH_VARARGS },

		{ "GetItemLink", playerGetItemLink, METH_VARARGS },
		{ "SlotTypeToInvenType", playerSlotTypeToInvenType, METH_VARARGS },
#if defined(ENABLE_DRAGON_SOUL_SYSTEM)
		{ "SendDragonSoulRefine", playerSendDragonSoulRefine, METH_VARARGS },
#endif

#if defined(ENABLE_MOVE_COSTUME_ATTR)
		// Item Combination
		{ "SetItemCombinationWindowActivedItemSlot", playerSetItemCombinationWindowActivedItemSlot, METH_VARARGS },
		{ "GetInvenSlotAttachedToConbWindowSlot", playerGetInvenSlotAttachedToConbWindowSlot, METH_VARARGS },
		{ "CanAttachToCombMediumSlot", playerCanAttachToCombMediumSlot, METH_VARARGS },
		{ "CanAttachToCombItemSlot", playerCanAttachToCombItemSlot, METH_VARARGS },
		{ "GetConbWindowSlotByAttachedInvenSlot", playerGetConbWindowSlotByAttachedInvenSlot, METH_VARARGS },
#endif

#if defined(ENABLE_CHANGED_ATTR)
		// Select Attribute
		{ "GetItemChangedAttribute", playerGetItemChangedAttribute, METH_VARARGS },
#endif

#if defined(ENABLE_LOADING_TIP)
		{ "GetLoadingTip", playerGetLoadingTip, METH_VARARGS },
#endif

		{ "CheckAffect", playerCheckAffect, METH_VARARGS },

		{ "SetParalysis", playerSetParalysis, METH_VARARGS },

#if defined(ENABLE_KEYCHANGE_SYSTEM)
		// Keyboard Controls
		{ "OpenKeyChangeWindow", playerOpenKeyChangeWindow , METH_VARARGS },
		{ "IsOpenKeySettingWindow", playerIsOpenKeySettingWindow, METH_VARARGS },
		{ "KeySettingClear", playerKeySettingClear, METH_VARARGS },
		{ "KeySetting", playerKeySetting, METH_VARARGS },
		{ "OnKeyDown", playerOnKeyDown, METH_VARARGS },
		{ "OnKeyUp", playerOnKeyUp, METH_VARARGS },
#endif

#if defined(ENABLE_ACCE_COSTUME_SYSTEM)
		// Acce Refine System
		{ "SetAcceRefineWindowOpen", playerSetAcceRefineWindowOpen, METH_VARARGS },
		{ "GetAcceRefineWindowOpen", playerGetAcceRefineWindowOpen, METH_VARARGS },
		{ "GetAcceRefineWindowType", playerGetAcceRefineWindowType, METH_VARARGS },

		{ "FindUsingAcceSlot", playerFindUsingAcceSlot, METH_VARARGS },
		{ "FindActivatedAcceSlot", playerFindActivatedAcceSlot, METH_VARARGS },
		{ "FindMoveAcceItemSlot", playerFindMoveAcceItemSlot, METH_VARARGS },

		{ "GetAcceCurrentItemCount", playerGetAcceCurrentItemCount, METH_VARARGS },
		{ "GetAcceItemSize", playerGetAcceItemSize, METH_VARARGS },
		//{ "GetItemAcceDrainRateGrade", playerGetItemAcceDrainRateGrade, METH_VARARGS },

		{ "SetAcceActivatedItemSlot", playerSetAcceActivatedItemSlot, METH_VARARGS },
		{ "IsAcceWindowEmpty", playerIsAcceWindowEmpty, METH_VARARGS },

		{ "GetAcceItemID", playerGetAcceItemID, METH_VARARGS },
		{ "GetAcceItemFlags", playerGetAcceItemFlags, METH_VARARGS },
		{ "GetAcceItemMetinSocket", playerGetAcceItemMetinSocket, METH_VARARGS },
		{ "GetAcceItemAttribute", playerGetAcceItemAttribute, METH_VARARGS },
#if defined(ENABLE_CHANGE_LOOK_SYSTEM)
		{ "GetAcceWindowChangeLookVnum", playerGetAcceWindowChangeLookVnum, METH_VARARGS },
#endif
#if defined(ENABLE_APPLY_RANDOM)
		{ "GetAcceItemApplyRandom", playerGetAcceItemApplyRandom, METH_VARARGS },
#endif
#if defined(ENABLE_SET_ITEM)
		{ "GetAcceItemSetValue", playerGetAcceItemSetValue, METH_VARARGS },
#endif
#if defined(ENABLE_REFINE_ELEMENT_SYSTEM)
		{ "GetAcceItemRefineElement", playerGetAcceItemRefineElement, METH_VARARGS },
#endif
		{ "CanAcceClearItem", playerCanAcceClearItem, METH_VARARGS },
#endif

#if defined(ENABLE_AURA_COSTUME_SYSTEM)
		// Aura Refine
		{ "SetAuraWindowOpen", playerSetAuraWindowOpen, METH_VARARGS },
		{ "IsAuraWindowOpen", playerIsAuraWindowOpen, METH_VARARGS },

		{ "GetAuraWindowType", playerGetAuraWindowType, METH_VARARGS },
		{ "IsAuraSlotEmpty", playerIsAuraSlotEmpty, METH_VARARGS },

		{ "GetAuraCurrentItemSlotCount", playerGetAuraCurrentItemSlotCount, METH_VARARGS },
		{ "SetAuraActivatedItemSlot", playerSetAuraActivatedItemSlot, METH_VARARGS },

		{ "FindActivatedAuraSlot", playerFindActivatedAuraSlot, METH_VARARGS },
		{ "FindUsingAuraSlot", playerFindUsingAuraSlot, METH_VARARGS },
		{ "FindMoveAuraItemSlot", playerFindMoveAuraItemSlot, METH_VARARGS },

		{ "GetAuraRefineInfo", playerGetAuraRefineInfo, METH_VARARGS },
		{ "GetAuraRefineInfoExpPer", playerGetAuraRefineInfoExpPer, METH_VARARGS },
		{ "GetAuraRefineInfoLevel", playerGetAuraRefineInfoLevel, METH_VARARGS },

		{ "GetAuraItemID", playerGetAuraItemID, METH_VARARGS },
		{ "GetAuraItemCount", playerGetAuraItemCount, METH_VARARGS },
		{ "GetAuraSlotItemMetinSocket", playerGetAuraSlotItemMetinSocket, METH_VARARGS },
		{ "GetAuraSlotItemAttribute", playerGetAuraSlotItemAttribute, METH_VARARGS },
		{ "GetAuraSlotItemFlags", playerGetAuraSlotItemFlags, METH_VARARGS },
#if defined(ENABLE_APPLY_RANDOM)
		{ "GetAuraSlotItemApplyRandom", playerGetAuraSlotItemApplyRandom, METH_VARARGS },
#endif
#if defined(ENABLE_SET_ITEM)
		{ "GetAuraSlotItemSetValue", playerGetAuraSlotItemSetValue, METH_VARARGS },
#endif
		{ "CanAuraClearItem", playerCanAuraClearItem, METH_VARARGS },
#endif

#if defined(ENABLE_CHANGE_LOOK_SYSTEM)
		// Change Look
		{ "GetChangeLookVnum", playerGetChangeLookVnum, METH_VARARGS },
		{ "SetChangeLookWindow", playerSetChangeLookWindow, METH_VARARGS },
		{ "GetChangeLookWindowOpen", playerGetChangeLookWindowOpen, METH_VARARGS },
		{ "SetChangeLookWindowType", playerSetChangeLookWindowType, METH_VARARGS },
		{ "GetChangeLookItemID", playerGetChangeLookItemID, METH_VARARGS },
		{ "GetChangeLookItemInvenSlot", playerGetChangeLookItemInvenSlot, METH_VARARGS },
		{ "GetChangeLookFreeYangItemID", playerGetChangeLookFreeYangItemID, METH_VARARGS },
		{ "GetChangeLookFreeYangInvenSlotPos", playerGetChangeLookFreeYangInvenSlotPos, METH_VARARGS },
		{ "CanChangeLookClearItem", playerCanChangeLookClearItem, METH_VARARGS },
		{ "GetChangeWIndowChangeLookVnum", playerGetChangeWIndowChangeLookVnum, METH_VARARGS },
		{ "GetChangeChangeLookPrice", playerGetChangeChangeLookPrice, METH_VARARGS },
#endif

		{ "GetAffectData", playerGetAffectData, METH_VARARGS },
		{ "EmptyAffectData", playerEmptyAffectData, METH_VARARGS },

#if defined(ENABLE_SET_ITEM)
		// Set Item
		{ "GetItemSetValue", playerGetItemSetValue, METH_VARARGS },
		{ "GetSetItemEffect", playerGetAffectData, METH_VARARGS },
		{ "EmptySetItemEffect", playerEmptyAffectData, METH_VARARGS },
		{ "CanSetItemClear", playerCanSetItemClear, METH_VARARGS },
#endif

#if defined(ENABLE_REFINE_ELEMENT_SYSTEM)
		// Item Refine Element
		{ "GetItemRefineElement", playerGetItemRefineElement, METH_VARARGS },
#endif

#if defined(ENABLE_MONSTER_CARD)
		// Monster Card
		{ "IllustrationSelectModel", playerIllustrationSelectModel, METH_VARARGS },
		{ "IllustrationShow", playerIllustrationShow, METH_VARARGS },
		{ "IllustrationChangeMotion", playerIllustrationChangeMotion, METH_VARARGS },
		{ "IllustrationModelRotation", playerIllustrationModelRotation, METH_VARARGS },
		{ "IllustrationModelUpDown", playerIllustrationModelUpDown, METH_VARARGS },
		{ "IllustrationModelZoom", playerIllustrationModelZoom, METH_VARARGS },
		{ "IllustrationModelViewReset", playerIllustrationModelViewReset, METH_VARARGS },
#endif

#if defined(ENABLE_MYSHOP_DECO)
		// MyShop Decoration
		{ "MyShopDecoShow", playerMyShopDecoShow, METH_VARARGS },
		{ "SelectShopModel", playerSelectShopModel, METH_VARARGS },
#endif

#if defined(ENABLE_MINI_GAME_RUMI)
		{ "SetRumiGame", playerSetRumiGame, METH_VARARGS },
		{ "GetRumiGame", playerGetRumiGame, METH_VARARGS },
		{ "SetMiniGameOkeyNormal", playerSetMiniGameOkeyNormal, METH_VARARGS },
		{ "GetMiniGameOkeyNormal", playerGetMiniGameOkeyNormal, METH_VARARGS },
#endif

#if defined(ENABLE_MINI_GAME_YUTNORI)
		{ "YutnoriShow", playerYutnoriShow, METH_VARARGS },
		{ "YutnoriChangeMotion", playerYutnoriChangeMotion, METH_VARARGS },
#endif

#if defined(ENABLE_MINI_GAME_CATCH_KING)
		// Catch King Game
		{ "SetCatchKingGame", playerSetCatchKingGame, METH_VARARGS },
		{ "GetCatchKingGame", playerGetCatchKingGame, METH_VARARGS },
#endif

#if defined(ENABLE_LUCKY_BOX)
		// Lucky Box
		{ "SendLuckyBoxAction", playerSendLuckyBoxAction, METH_VARARGS },
#endif

#if defined(ENABLE_FISHING_GAME)
		// Fishing Game
		{ "FishingGameGoal", playerFishingGameGoal, METH_VARARGS },
		{ "FishingGameQuit", playerFishingGameQuit, METH_VARARGS },
#endif

#if defined(ENABLE_GEM_SYSTEM)
		// Gem (Gaya) Shop
		{ "GetGemShopItemID", playerGetGemShopItemID, METH_VARARGS },
		{ "GetGemShopOpenSlotCount", playerGetGemShopOpenSlotCount, METH_VARARGS },
		{ "GetGemShopOpenSlotItemCount", playerGetGemShopOpenSlotItemCount, METH_VARARGS },
		{ "GetGemShopRefreshTime", playerGetGemShopRefreshTime, METH_VARARGS },
		{ "SetGemShopWindowOpen", playerSetGemShopWindowOpen, METH_VARARGS },
		{ "IsGemShopWindowOpen", playerIsGemShopWindowOpen, METH_VARARGS },
#endif

		{ "NPOS", playerNPOS, METH_VARARGS },
		{ "WindowTypeToSlotType", playerWindowTypeToSlotType, METH_VARARGS },

#if defined(ENABLE_EXTEND_INVEN_SYSTEM)
		{ "GetExtendInvenStage", playerGetExtendInvenStage, METH_VARARGS },
		{ "GetExtendInvenMax", playerGetExtendInvenMax, METH_VARARGS },
#endif

#if defined(ENABLE_EXPRESSING_EMOTION)
		{ "ClearSpecialActionSlot", playerClearSpecialActionSlot, METH_VARARGS },
		{ "SetSpecialActionSlot", playerSetSpecialActionSlot, METH_VARARGS },
		{ "GetSpecialActionSlot", playerGetSpecialActionSlot, METH_VARARGS },
		{ "FindSpecialActionSlot", playerFindSpecialActionSlot, METH_VARARGS },
#endif

#if defined(ENABLE_CUBE_RENEWAL)
		{ "GetCubeItem", playerGetCubeItem, METH_VARARGS },
		{ "GetCubeListSize", playerGetCubeListSize, METH_VARARGS },
		{ "SetCubeWindowOpen", playerSetCubeWindowOpen, METH_VARARGS },
		{ "IsCubeSetAddCategory", playerIsCubeSetAddCategory, METH_VARARGS },
#endif

#if defined(ENABLE_SNOWFLAKE_STICK_EVENT)
		// Snowflake Stick Event
		{ "SetSnowflakeStickEvent", playerSetSnowflakeStickEvent, METH_VARARGS },
		{ "GetSnowflakeStickEvent", playerGetSnowflakeStickEvent, METH_VARARGS },
#endif

#if defined(ENABLE_GAME_OPTION_ESCAPE)
		{ "CanEscapeState", playerCanEscapeState, METH_VARARGS },
#endif

#if defined(ENABLE_FLOWER_EVENT)
		// Flower Event
		{ "SetFlowerEventEnable", playerSetFlowerEventEnable, METH_VARARGS },
		{ "GetFlowerEventEnable", playerGetFlowerEventEnable, METH_VARARGS },
#endif

		{ NULL, NULL, NULL },
	};

	PyObject* poModule = Py_InitModule("player", s_methods);

	PyModule_AddIntConstant(poModule, "GOLD_MAX", GOLD_MAX);
#if defined(ENABLE_CHEQUE_SYSTEM)
	PyModule_AddIntConstant(poModule, "CHEQUE_MAX", CHEQUE_MAX);
#endif
#if defined(ENABLE_GEM_SYSTEM)
	PyModule_AddIntConstant(poModule, "GEM_MAX", GEM_MAX);
#endif

	PyModule_AddIntConstant(poModule, "LEVEL", POINT_LEVEL);
	PyModule_AddIntConstant(poModule, "VOICE", POINT_VOICE);
	PyModule_AddIntConstant(poModule, "EXP", POINT_EXP);
	PyModule_AddIntConstant(poModule, "NEXT_EXP", POINT_NEXT_EXP);
	PyModule_AddIntConstant(poModule, "HP", POINT_HP);
	PyModule_AddIntConstant(poModule, "MAX_HP", POINT_MAX_HP);
	PyModule_AddIntConstant(poModule, "SP", POINT_SP);
	PyModule_AddIntConstant(poModule, "MAX_SP", POINT_MAX_SP);
	PyModule_AddIntConstant(poModule, "STAMINA", POINT_STAMINA);
	PyModule_AddIntConstant(poModule, "MAX_STAMINA", POINT_MAX_STAMINA);
	PyModule_AddIntConstant(poModule, "ELK", POINT_GOLD);
	PyModule_AddIntConstant(poModule, "ST", POINT_ST);
	PyModule_AddIntConstant(poModule, "HT", POINT_HT);
	PyModule_AddIntConstant(poModule, "DX", POINT_DX);
	PyModule_AddIntConstant(poModule, "IQ", POINT_IQ);
	PyModule_AddIntConstant(poModule, "ATT_POWER", POINT_ATT_POWER);
	PyModule_AddIntConstant(poModule, "ATT_MIN", POINT_MIN_ATK);
	PyModule_AddIntConstant(poModule, "ATT_MAX", POINT_MAX_ATK);
	PyModule_AddIntConstant(poModule, "MIN_MAGIC_WEP", POINT_MIN_MAGIC_WEP);
	PyModule_AddIntConstant(poModule, "MAX_MAGIC_WEP", POINT_MAX_MAGIC_WEP);
	PyModule_AddIntConstant(poModule, "ATT_SPEED", POINT_ATT_SPEED);
	PyModule_AddIntConstant(poModule, "ATT_BONUS", POINT_ATT_GRADE_BONUS);
	PyModule_AddIntConstant(poModule, "EVADE_RATE", POINT_EVADE_RATE);
	PyModule_AddIntConstant(poModule, "MOVING_SPEED", POINT_MOV_SPEED);
	PyModule_AddIntConstant(poModule, "DEF_GRADE", POINT_DEF_GRADE);
	PyModule_AddIntConstant(poModule, "DEF_BONUS", POINT_DEF_GRADE_BONUS);
	PyModule_AddIntConstant(poModule, "CASTING_SPEED", POINT_CASTING_SPEED);
	PyModule_AddIntConstant(poModule, "MAG_ATT", POINT_MAGIC_ATT_GRADE);
	PyModule_AddIntConstant(poModule, "MAG_DEF", POINT_MAGIC_DEF_GRADE);
	PyModule_AddIntConstant(poModule, "EMPIRE_POINT", POINT_EMPIRE_POINT);
	PyModule_AddIntConstant(poModule, "STAT", POINT_STAT);
	PyModule_AddIntConstant(poModule, "SKILL_PASSIVE", POINT_SUB_SKILL);
	PyModule_AddIntConstant(poModule, "SKILL_SUPPORT", POINT_SUB_SKILL);
	PyModule_AddIntConstant(poModule, "SKILL_ACTIVE", POINT_SKILL);
	PyModule_AddIntConstant(poModule, "SKILL_HORSE", POINT_HORSE_SKILL);
	PyModule_AddIntConstant(poModule, "PLAYTIME", POINT_PLAYTIME);
	PyModule_AddIntConstant(poModule, "BOW_DISTANCE", POINT_BOW_DISTANCE);
	PyModule_AddIntConstant(poModule, "HP_RECOVERY", POINT_HP_RECOVERY);
	PyModule_AddIntConstant(poModule, "SP_RECOVERY", POINT_SP_RECOVERY);
	PyModule_AddIntConstant(poModule, "ATTACKER_BONUS", POINT_PARTY_ATT_GRADE);
	PyModule_AddIntConstant(poModule, "MAX_NUM", POINT_MAX_NUM);
	////
	PyModule_AddIntConstant(poModule, "POINT_NONE", POINT_NONE);
	PyModule_AddIntConstant(poModule, "POINT_GOLD", POINT_GOLD);
	PyModule_AddIntConstant(poModule, "POINT_CRITICAL_PCT", POINT_CRITICAL_PCT);
	PyModule_AddIntConstant(poModule, "POINT_PENETRATE_PCT", POINT_PENETRATE_PCT);
	PyModule_AddIntConstant(poModule, "POINT_ATTBONUS_HUMAN", POINT_ATTBONUS_HUMAN);
	PyModule_AddIntConstant(poModule, "POINT_ATTBONUS_ANIMAL", POINT_ATTBONUS_ANIMAL);
	PyModule_AddIntConstant(poModule, "POINT_ATTBONUS_ORC", POINT_ATTBONUS_ORC);
	PyModule_AddIntConstant(poModule, "POINT_ATTBONUS_MILGYO", POINT_ATTBONUS_MILGYO);
	PyModule_AddIntConstant(poModule, "POINT_ATTBONUS_UNDEAD", POINT_ATTBONUS_UNDEAD);
	PyModule_AddIntConstant(poModule, "POINT_ATTBONUS_DEVIL", POINT_ATTBONUS_DEVIL);
	PyModule_AddIntConstant(poModule, "POINT_ATTBONUS_INSECT", POINT_ATTBONUS_INSECT);
	PyModule_AddIntConstant(poModule, "POINT_ATTBONUS_DESERT", POINT_ATTBONUS_DESERT);
	PyModule_AddIntConstant(poModule, "POINT_ATTBONUS_MONSTER", POINT_ATTBONUS_MONSTER);
	PyModule_AddIntConstant(poModule, "POINT_ATTBONUS_WARRIOR", POINT_ATTBONUS_WARRIOR);
	PyModule_AddIntConstant(poModule, "POINT_ATTBONUS_ASSASSIN", POINT_ATTBONUS_ASSASSIN);
	PyModule_AddIntConstant(poModule, "POINT_ATTBONUS_SURA", POINT_ATTBONUS_SURA);
	PyModule_AddIntConstant(poModule, "POINT_ATTBONUS_SHAMAN", POINT_ATTBONUS_SHAMAN);
	PyModule_AddIntConstant(poModule, "POINT_RESIST_FIRE", POINT_RESIST_FIRE);
	PyModule_AddIntConstant(poModule, "POINT_RESIST_ELEC", POINT_RESIST_ELEC);
	PyModule_AddIntConstant(poModule, "POINT_RESIST_MAGIC", POINT_RESIST_MAGIC);
	PyModule_AddIntConstant(poModule, "POINT_RESIST_WIND", POINT_RESIST_WIND);
	PyModule_AddIntConstant(poModule, "POINT_MALL_ATTBONUS", POINT_MALL_ATTBONUS);
	PyModule_AddIntConstant(poModule, "POINT_MALL_DEFBONUS", POINT_MALL_DEFBONUS);
	PyModule_AddIntConstant(poModule, "POINT_MALL_EXPBONUS", POINT_MALL_EXPBONUS);
	PyModule_AddIntConstant(poModule, "POINT_MALL_ITEMBONUS", POINT_MALL_ITEMBONUS);
	PyModule_AddIntConstant(poModule, "POINT_MALL_GOLDBONUS", POINT_MALL_GOLDBONUS);
	PyModule_AddIntConstant(poModule, "POINT_MAX_HP_PCT", POINT_MAX_HP_PCT);
	PyModule_AddIntConstant(poModule, "POINT_MAX_SP_PCT", POINT_MAX_SP_PCT);
	PyModule_AddIntConstant(poModule, "POINT_SKILL_DAMAGE_BONUS", POINT_SKILL_DAMAGE_BONUS);
	PyModule_AddIntConstant(poModule, "POINT_NORMAL_HIT_DAMAGE_BONUS", POINT_NORMAL_HIT_DAMAGE_BONUS);
	PyModule_AddIntConstant(poModule, "POINT_SKILL_DEFEND_BONUS", POINT_SKILL_DEFEND_BONUS);
	PyModule_AddIntConstant(poModule, "POINT_NORMAL_HIT_DEFEND_BONUS", POINT_NORMAL_HIT_DEFEND_BONUS);
	PyModule_AddIntConstant(poModule, "POINT_PC_BANG_EXP_BONUS", POINT_PC_BANG_EXP_BONUS);
	PyModule_AddIntConstant(poModule, "POINT_PC_BANG_DROP_BONUS", POINT_PC_BANG_DROP_BONUS);
	PyModule_AddIntConstant(poModule, "POINT_COSTUME_ATTR_BONUS", POINT_COSTUME_ATTR_BONUS);
	PyModule_AddIntConstant(poModule, "POINT_MELEE_MAGIC_ATT_BONUS_PER", POINT_MELEE_MAGIC_ATT_BONUS_PER);
	PyModule_AddIntConstant(poModule, "POINT_RESIST_ICE", POINT_RESIST_ICE);
	PyModule_AddIntConstant(poModule, "POINT_RESIST_EARTH", POINT_RESIST_EARTH);
	PyModule_AddIntConstant(poModule, "POINT_RESIST_DARK", POINT_RESIST_DARK);
	PyModule_AddIntConstant(poModule, "POINT_ATTBONUS_WOLFMAN", POINT_ATTBONUS_WOLFMAN);
#if defined(ENABLE_CHEQUE_SYSTEM)
	PyModule_AddIntConstant(poModule, "POINT_CHEQUE", POINT_CHEQUE);
#endif
	PyModule_AddIntConstant(poModule, "POINT_RESIST_HUMAN", POINT_RESIST_HUMAN);
	PyModule_AddIntConstant(poModule, "POINT_ENCHANT_ELECT", POINT_ENCHANT_ELECT);
	PyModule_AddIntConstant(poModule, "POINT_ENCHANT_FIRE", POINT_ENCHANT_FIRE);
	PyModule_AddIntConstant(poModule, "POINT_ENCHANT_ICE", POINT_ENCHANT_ICE);
	PyModule_AddIntConstant(poModule, "POINT_ENCHANT_WIND", POINT_ENCHANT_WIND);
	PyModule_AddIntConstant(poModule, "POINT_ENCHANT_EARTH", POINT_ENCHANT_EARTH);
	PyModule_AddIntConstant(poModule, "POINT_ENCHANT_DARK", POINT_ENCHANT_DARK);
	PyModule_AddIntConstant(poModule, "POINT_ATTBONUS_CZ", POINT_ATTBONUS_CZ);
#if defined(ENABLE_GEM_SYSTEM)
	PyModule_AddIntConstant(poModule, "POINT_GEM", POINT_GEM);
#endif
	PyModule_AddIntConstant(poModule, "POINT_PREMIUM_EXPBONUS", POINT_PREMIUM_EXPBONUS);
	PyModule_AddIntConstant(poModule, "POINT_PRIVILEGE_EXPBONUS", POINT_PRIVILEGE_EXPBONUS);
	PyModule_AddIntConstant(poModule, "POINT_MARRIAGE_EXPBONUS", POINT_MARRIAGE_EXPBONUS);
	PyModule_AddIntConstant(poModule, "POINT_DEVILTOWER_EXPBONUS", POINT_DEVILTOWER_EXPBONUS);
	PyModule_AddIntConstant(poModule, "POINT_PREMIUM_ITEMBONUS", POINT_PREMIUM_ITEMBONUS);
	PyModule_AddIntConstant(poModule, "POINT_PRIVILEGE_ITEMBONUS", POINT_PRIVILEGE_ITEMBONUS);
	PyModule_AddIntConstant(poModule, "POINT_PREMIUM_GOLDBONUS", POINT_PREMIUM_GOLDBONUS);
	PyModule_AddIntConstant(poModule, "POINT_PRIVILEGE_GOLDBONUS", POINT_PRIVILEGE_GOLDBONUS);
	PyModule_AddIntConstant(poModule, "POINT_ATTBONUS_STONE", POINT_ATTBONUS_STONE);
	PyModule_AddIntConstant(poModule, "POINT_MEDAL_OF_HONOR", POINT_MEDAL_OF_HONOR);
	PyModule_AddIntConstant(poModule, "POINT_ALL_STAT_BONUS", POINT_ALL_STAT_BONUS);
	PyModule_AddIntConstant(poModule, "POINT_SUNGMA_STR", POINT_SUNGMA_STR);
	PyModule_AddIntConstant(poModule, "POINT_SUNGMA_HP", POINT_SUNGMA_HP);
	PyModule_AddIntConstant(poModule, "POINT_SUNGMA_MOVE", POINT_SUNGMA_MOVE);
	PyModule_AddIntConstant(poModule, "POINT_SUNGMA_IMMUNE", POINT_SUNGMA_IMMUNE);
	PyModule_AddIntConstant(poModule, "POINT_CONQUEROR_LEVEL", POINT_CONQUEROR_LEVEL);
	PyModule_AddIntConstant(poModule, "POINT_CONQUEROR_LEVEL_STEP", POINT_CONQUEROR_LEVEL_STEP);
	PyModule_AddIntConstant(poModule, "POINT_CONQUEROR_EXP", POINT_CONQUEROR_EXP);
	PyModule_AddIntConstant(poModule, "POINT_CONQUEROR_NEXT_EXP", POINT_CONQUEROR_NEXT_EXP);
	PyModule_AddIntConstant(poModule, "POINT_CONQUEROR_POINT", POINT_CONQUEROR_POINT);
	PyModule_AddIntConstant(poModule, "POINT_HIT_PCT", POINT_HIT_PCT);
	PyModule_AddIntConstant(poModule, "POINT_ATTBONUS_PER_HUMAN", POINT_ATTBONUS_PER_HUMAN);
	PyModule_AddIntConstant(poModule, "POINT_ATTBONUS_PER_ANIMAL", POINT_ATTBONUS_PER_ANIMAL);
	PyModule_AddIntConstant(poModule, "POINT_ATTBONUS_PER_ORC", POINT_ATTBONUS_PER_ORC);
	PyModule_AddIntConstant(poModule, "POINT_ATTBONUS_PER_MILGYO", POINT_ATTBONUS_PER_MILGYO);
	PyModule_AddIntConstant(poModule, "POINT_ATTBONUS_PER_UNDEAD", POINT_ATTBONUS_PER_UNDEAD);
	PyModule_AddIntConstant(poModule, "POINT_ATTBONUS_PER_DEVIL", POINT_ATTBONUS_PER_DEVIL);
	PyModule_AddIntConstant(poModule, "POINT_ENCHANT_PER_ELECT", POINT_ENCHANT_PER_ELECT);
	PyModule_AddIntConstant(poModule, "POINT_ENCHANT_PER_FIRE", POINT_ENCHANT_PER_FIRE);
	PyModule_AddIntConstant(poModule, "POINT_ENCHANT_PER_ICE", POINT_ENCHANT_PER_ICE);
	PyModule_AddIntConstant(poModule, "POINT_ENCHANT_PER_WIND", POINT_ENCHANT_PER_WIND);
	PyModule_AddIntConstant(poModule, "POINT_ENCHANT_PER_EARTH", POINT_ENCHANT_PER_EARTH);
	PyModule_AddIntConstant(poModule, "POINT_ENCHANT_PER_DARK", POINT_ENCHANT_PER_DARK);
	PyModule_AddIntConstant(poModule, "POINT_ATTBONUS_PER_CZ", POINT_ATTBONUS_PER_CZ);
	PyModule_AddIntConstant(poModule, "POINT_ATTBONUS_PER_INSECT", POINT_ATTBONUS_PER_INSECT);
	PyModule_AddIntConstant(poModule, "POINT_ATTBONUS_PER_DESERT", POINT_ATTBONUS_PER_DESERT);
	PyModule_AddIntConstant(poModule, "POINT_ATTBONUS_PER_STONE", POINT_ATTBONUS_PER_STONE);
	PyModule_AddIntConstant(poModule, "POINT_ATTBONUS_PER_MONSTER", POINT_ATTBONUS_PER_MONSTER);
	PyModule_AddIntConstant(poModule, "POINT_RESIST_PER_HUMAN", POINT_RESIST_PER_HUMAN);
	PyModule_AddIntConstant(poModule, "POINT_RESIST_PER_ICE", POINT_RESIST_PER_ICE);
	PyModule_AddIntConstant(poModule, "POINT_RESIST_PER_DARK", POINT_RESIST_PER_DARK);
	PyModule_AddIntConstant(poModule, "POINT_RESIST_PER_EARTH", POINT_RESIST_PER_EARTH);
	PyModule_AddIntConstant(poModule, "POINT_RESIST_PER_FIRE", POINT_RESIST_PER_FIRE);
	PyModule_AddIntConstant(poModule, "POINT_RESIST_PER_ELEC", POINT_RESIST_PER_ELEC);
	PyModule_AddIntConstant(poModule, "POINT_RESIST_PER_MAGIC", POINT_RESIST_PER_MAGIC);
	PyModule_AddIntConstant(poModule, "POINT_RESIST_PER_WIND", POINT_RESIST_PER_WIND);
	PyModule_AddIntConstant(poModule, "POINT_SUNGMA_PER_STR", POINT_SUNGMA_PER_STR);
	PyModule_AddIntConstant(poModule, "POINT_SUNGMA_PER_HP", POINT_SUNGMA_PER_HP);
	PyModule_AddIntConstant(poModule, "POINT_SUNGMA_PER_MOVE", POINT_SUNGMA_PER_MOVE);
	PyModule_AddIntConstant(poModule, "POINT_SUNGMA_PER_IMMUNE", POINT_SUNGMA_PER_IMMUNE);
	PyModule_AddIntConstant(poModule, "POINT_MONSTER_DEFEND_BONUS", POINT_MONSTER_DEFEND_BONUS);

	PyModule_AddIntConstant(poModule, "ENERGY", POINT_ENERGY);
	PyModule_AddIntConstant(poModule, "ENERGY_END_TIME", POINT_ENERGY_END_TIME);

#if defined(ENABLE_CHEQUE_SYSTEM)
	PyModule_AddIntConstant(poModule, "ITEM_CHEQUE", -1);
	PyModule_AddIntConstant(poModule, "CHEQUE", POINT_CHEQUE);
#endif

#if defined(ENABLE_GEM_SYSTEM)
	PyModule_AddIntConstant(poModule, "GEM", POINT_GEM);
#endif

	PyModule_AddIntConstant(poModule, "SKILL_GRADE_NORMAL", CPythonPlayer::SKILL_NORMAL);
	PyModule_AddIntConstant(poModule, "SKILL_GRADE_MASTER", CPythonPlayer::SKILL_MASTER);
	PyModule_AddIntConstant(poModule, "SKILL_GRADE_GRAND_MASTER", CPythonPlayer::SKILL_GRAND_MASTER);
	PyModule_AddIntConstant(poModule, "SKILL_GRADE_PERFECT_MASTER", CPythonPlayer::SKILL_PERFECT_MASTER);

	PyModule_AddIntConstant(poModule, "CATEGORY_ACTIVE", CPythonPlayer::CATEGORY_ACTIVE);
	PyModule_AddIntConstant(poModule, "CATEGORY_PASSIVE", CPythonPlayer::CATEGORY_PASSIVE);

	PyModule_AddIntConstant(poModule, "SHOW_UI_WINDOW_LIMIT_RANGE", c_iShow_UI_Window_Limit_Range);

	PyModule_AddIntConstant(poModule, "INVENTORY_PAGE_SIZE", c_Inventory_Page_Size);
	PyModule_AddIntConstant(poModule, "INVENTORY_PAGE_COUNT", c_Inventory_Page_Count);
	PyModule_AddIntConstant(poModule, "INVENTORY_SLOT_COUNT", c_Inventory_Slot_Count);
	PyModule_AddIntConstant(poModule, "ITEM_SLOT_COUNT", c_Inventory_Slot_Count);
	PyModule_AddIntConstant(poModule, "EQUIPMENT_SLOT_START", c_Equipment_Slot_Start);
	PyModule_AddIntConstant(poModule, "EQUIPMENT_PAGE_COUNT", c_Equipment_Slot_Count);
	PyModule_AddIntConstant(poModule, "WEAR_MAX", c_Wear_Max);

#if defined(ENABLE_SKILLBOOK_COMB_SYSTEM)
	PyModule_AddIntConstant(poModule, "SKILLBOOK_COMB_SLOT_MAX", c_SkillBook_Comb_Size);
#endif

	PyModule_AddIntConstant(poModule, "MBF_SKILL", CPythonPlayer::MBF_SKILL);
	PyModule_AddIntConstant(poModule, "MBF_ATTACK", CPythonPlayer::MBF_ATTACK);
	PyModule_AddIntConstant(poModule, "MBF_CAMERA", CPythonPlayer::MBF_CAMERA);
	PyModule_AddIntConstant(poModule, "MBF_SMART", CPythonPlayer::MBF_SMART);
	PyModule_AddIntConstant(poModule, "MBF_MOVE", CPythonPlayer::MBF_MOVE);
	PyModule_AddIntConstant(poModule, "MBF_AUTO", CPythonPlayer::MBF_AUTO);
	PyModule_AddIntConstant(poModule, "MBS_PRESS", CPythonPlayer::MBS_PRESS);
	PyModule_AddIntConstant(poModule, "MBS_CLICK", CPythonPlayer::MBS_CLICK);
	PyModule_AddIntConstant(poModule, "MBT_RIGHT", CPythonPlayer::MBT_RIGHT);
	PyModule_AddIntConstant(poModule, "MBT_LEFT", CPythonPlayer::MBT_LEFT);

	// Public code with server


#if defined(ENABLE_CHANGE_LOOK_SYSTEM)
	PyModule_AddIntConstant(poModule, "CHANGE_LOOK_TYPE_ITEM", static_cast<BYTE>(EChangeLookType::CHANGE_LOOK_TYPE_ITEM));
	PyModule_AddIntConstant(poModule, "CHANGE_LOOK_TYPE_MOUNT", static_cast<BYTE>(EChangeLookType::CHANGE_LOOK_TYPE_MOUNT));
#endif

	PyModule_AddIntConstant(poModule, "RESERVED_WINDOW", RESERVED_WINDOW);
	PyModule_AddIntConstant(poModule, "INVENTORY", INVENTORY);
	PyModule_AddIntConstant(poModule, "EQUIPMENT", EQUIPMENT);
	PyModule_AddIntConstant(poModule, "SAFEBOX", SAFEBOX);
	PyModule_AddIntConstant(poModule, "MALL", MALL);
	PyModule_AddIntConstant(poModule, "DRAGON_SOUL_INVENTORY", DRAGON_SOUL_INVENTORY);
	PyModule_AddIntConstant(poModule, "BELT_INVENTORY", BELT_INVENTORY);
	PyModule_AddIntConstant(poModule, "GUILDBANK", GUILDBANK);
	PyModule_AddIntConstant(poModule, "MAIL", MAIL);
	PyModule_AddIntConstant(poModule, "NPC_STORAGE", NPC_STORAGE);
	PyModule_AddIntConstant(poModule, "PREMIUM_PRIVATE_SHOP", PREMIUM_PRIVATE_SHOP);
	PyModule_AddIntConstant(poModule, "ACCEREFINE", ACCEREFINE);
	//PyModule_AddIntConstant(poModule, "GROUND", GROUND);
	PyModule_AddIntConstant(poModule, "PET_FEED", PET_FEED);
	PyModule_AddIntConstant(poModule, "CHANGELOOK", CHANGELOOK);
	PyModule_AddIntConstant(poModule, "AURA_REFINE", AURA_REFINE);
	PyModule_AddIntConstant(poModule, "CUBE_WINDOW", CUBE_WINDOW);

	PyModule_AddIntConstant(poModule, "ITEM_MONEY", -1);

	PyModule_AddIntConstant(poModule, "SKILL_SLOT_COUNT", SKILL_MAX_NUM);

	PyModule_AddIntConstant(poModule, "EFFECT_PICK", CPythonPlayer::EFFECT_PICK);

	PyModule_AddIntConstant(poModule, "METIN_SOCKET_TYPE_NONE", CPythonPlayer::METIN_SOCKET_TYPE_NONE);
	PyModule_AddIntConstant(poModule, "METIN_SOCKET_TYPE_SILVER", CPythonPlayer::METIN_SOCKET_TYPE_SILVER);
	PyModule_AddIntConstant(poModule, "METIN_SOCKET_TYPE_GOLD", CPythonPlayer::METIN_SOCKET_TYPE_GOLD);
	PyModule_AddIntConstant(poModule, "ITEM_METIN_SOCKET_MAX", ITEM_METIN_SOCKET_MAX);
	PyModule_AddIntConstant(poModule, "METIN_SOCKET_MAX_NUM", ITEM_SOCKET_SLOT_MAX_NUM);
	PyModule_AddIntConstant(poModule, "ATTRIBUTE_SLOT_MAX_NUM", ITEM_ATTRIBUTE_SLOT_MAX_NUM);
#if defined(ENABLE_APPLY_RANDOM)
	PyModule_AddIntConstant(poModule, "APPLY_RANDOM_SLOT_MAX_NUM", ITEM_APPLY_RANDOM_SLOT_MAX_NUM);
#endif

	PyModule_AddIntConstant(poModule, "REFINE_CANT", REFINE_CANT);
	PyModule_AddIntConstant(poModule, "REFINE_OK", REFINE_OK);
	PyModule_AddIntConstant(poModule, "REFINE_ALREADY_MAX_SOCKET_COUNT", REFINE_ALREADY_MAX_SOCKET_COUNT);
	PyModule_AddIntConstant(poModule, "REFINE_NEED_MORE_GOOD_SCROLL", REFINE_NEED_MORE_GOOD_SCROLL);
	PyModule_AddIntConstant(poModule, "REFINE_CANT_MAKE_SOCKET_ITEM", REFINE_CANT_MAKE_SOCKET_ITEM);
	PyModule_AddIntConstant(poModule, "REFINE_NOT_NEXT_GRADE_ITEM", REFINE_NOT_NEXT_GRADE_ITEM);
	PyModule_AddIntConstant(poModule, "REFINE_CANT_REFINE_METIN_TO_EQUIPMENT", REFINE_CANT_REFINE_METIN_TO_EQUIPMENT);
	PyModule_AddIntConstant(poModule, "REFINE_CANT_REFINE_ROD", REFINE_CANT_REFINE_ROD);
	PyModule_AddIntConstant(poModule, "ATTACH_METIN_CANT", ATTACH_METIN_CANT);
	PyModule_AddIntConstant(poModule, "ATTACH_METIN_OK", ATTACH_METIN_OK);
	PyModule_AddIntConstant(poModule, "ATTACH_METIN_NOT_MATCHABLE_ITEM", ATTACH_METIN_NOT_MATCHABLE_ITEM);
	PyModule_AddIntConstant(poModule, "ATTACH_METIN_NO_MATCHABLE_SOCKET", ATTACH_METIN_NO_MATCHABLE_SOCKET);
	PyModule_AddIntConstant(poModule, "ATTACH_METIN_NOT_EXIST_GOLD_SOCKET", ATTACH_METIN_NOT_EXIST_GOLD_SOCKET);
	PyModule_AddIntConstant(poModule, "ATTACH_METIN_CANT_ATTACH_TO_EQUIPMENT", ATTACH_METIN_CANT_ATTACH_TO_EQUIPMENT);
	PyModule_AddIntConstant(poModule, "DETACH_METIN_CANT", DETACH_METIN_CANT);
	PyModule_AddIntConstant(poModule, "DETACH_METIN_OK", DETACH_METIN_OK);

	// Party
	PyModule_AddIntConstant(poModule, "PARTY_STATE_NORMAL", CPythonPlayer::PARTY_ROLE_NORMAL);
	PyModule_AddIntConstant(poModule, "PARTY_STATE_LEADER", CPythonPlayer::PARTY_ROLE_LEADER);
	PyModule_AddIntConstant(poModule, "PARTY_STATE_ATTACKER", CPythonPlayer::PARTY_ROLE_ATTACKER);
	PyModule_AddIntConstant(poModule, "PARTY_STATE_TANKER", CPythonPlayer::PARTY_ROLE_TANKER);
	PyModule_AddIntConstant(poModule, "PARTY_STATE_BUFFER", CPythonPlayer::PARTY_ROLE_BUFFER);
	PyModule_AddIntConstant(poModule, "PARTY_STATE_SKILL_MASTER", CPythonPlayer::PARTY_ROLE_SKILL_MASTER);
	PyModule_AddIntConstant(poModule, "PARTY_STATE_BERSERKER", CPythonPlayer::PARTY_ROLE_BERSERKER);
	PyModule_AddIntConstant(poModule, "PARTY_STATE_DEFENDER", CPythonPlayer::PARTY_ROLE_DEFENDER);
	PyModule_AddIntConstant(poModule, "PARTY_STATE_MAX_NUM", CPythonPlayer::PARTY_ROLE_MAX_NUM);

	// Skill Index
	PyModule_AddIntConstant(poModule, "SKILL_INDEX_TONGSOL", c_iSkillIndex_Tongsol);
	PyModule_AddIntConstant(poModule, "SKILL_INDEX_FISHING", c_iSkillIndex_Fishing);
	PyModule_AddIntConstant(poModule, "SKILL_INDEX_MINING", c_iSkillIndex_Mining);
	PyModule_AddIntConstant(poModule, "SKILL_INDEX_MAKING", c_iSkillIndex_Making);
	PyModule_AddIntConstant(poModule, "SKILL_INDEX_COMBO", c_iSkillIndex_Combo);
	PyModule_AddIntConstant(poModule, "SKILL_INDEX_LANGUAGE1", c_iSkillIndex_Language1);
	PyModule_AddIntConstant(poModule, "SKILL_INDEX_LANGUAGE2", c_iSkillIndex_Language2);
	PyModule_AddIntConstant(poModule, "SKILL_INDEX_LANGUAGE3", c_iSkillIndex_Language3);
	PyModule_AddIntConstant(poModule, "SKILL_INDEX_POLYMORPH", c_iSkillIndex_Polymorph);
	PyModule_AddIntConstant(poModule, "SKILL_INDEX_RIDING", c_iSkillIndex_Riding);
	PyModule_AddIntConstant(poModule, "SKILL_INDEX_SUMMON", c_iSkillIndex_Summon);
	PyModule_AddIntConstant(poModule, "SKILL_INDEX_AUTO_ATTACK", c_iSkillIndex_AutoAttack);
#if defined(ENABLE_PARTY_PROFICY)
	PyModule_AddIntConstant(poModule, "SKILL_INDEX_ROLE_PROFICIENCY", c_iSkillIndex_RoleProficiency);
#endif
#if defined(ENABLE_PARTY_INSIGHT)
	PyModule_AddIntConstant(poModule, "SKILL_INDEX_INSIGHT", c_iSkillIndex_InSight);
#endif
	PyModule_AddIntConstant(poModule, "SKILL_INDEX_HIT", c_iSkillIndex_Hit);

	// PK Mode
	PyModule_AddIntConstant(poModule, "PK_MODE_PEACE", PK_MODE_PEACE);
	PyModule_AddIntConstant(poModule, "PK_MODE_REVENGE", PK_MODE_REVENGE);
	PyModule_AddIntConstant(poModule, "PK_MODE_FREE", PK_MODE_FREE);
	PyModule_AddIntConstant(poModule, "PK_MODE_PROTECT", PK_MODE_PROTECT);
	PyModule_AddIntConstant(poModule, "PK_MODE_GUILD", PK_MODE_GUILD);
	PyModule_AddIntConstant(poModule, "PK_MODE_MAX_NUM", PK_MODE_MAX_NUM);

	// Block Mode
	PyModule_AddIntConstant(poModule, "BLOCK_EXCHANGE", BLOCK_EXCHANGE);
	PyModule_AddIntConstant(poModule, "BLOCK_PARTY", BLOCK_PARTY_INVITE);
	PyModule_AddIntConstant(poModule, "BLOCK_GUILD", BLOCK_GUILD_INVITE);
	PyModule_AddIntConstant(poModule, "BLOCK_WHISPER", BLOCK_WHISPER);
	PyModule_AddIntConstant(poModule, "BLOCK_FRIEND", BLOCK_MESSENGER_INVITE);
	PyModule_AddIntConstant(poModule, "BLOCK_PARTY_REQUEST", BLOCK_PARTY_REQUEST);

	// Party
	PyModule_AddIntConstant(poModule, "PARTY_EXP_NON_DISTRIBUTION", PARTY_EXP_DISTRIBUTION_NON_PARITY);
	PyModule_AddIntConstant(poModule, "PARTY_EXP_DISTRIBUTION_PARITY", PARTY_EXP_DISTRIBUTION_PARITY);

	// Emotion
	PyModule_AddIntConstant(poModule, "EMOTION_CLAP", EMOTION_CLAP);
	PyModule_AddIntConstant(poModule, "EMOTION_CHEERS_1", EMOTION_CHEERS_1);
	PyModule_AddIntConstant(poModule, "EMOTION_CHEERS_2", EMOTION_CHEERS_2);
	PyModule_AddIntConstant(poModule, "EMOTION_DANCE_1", EMOTION_DANCE_1);
	PyModule_AddIntConstant(poModule, "EMOTION_DANCE_2", EMOTION_DANCE_2);
	PyModule_AddIntConstant(poModule, "EMOTION_DANCE_3", EMOTION_DANCE_3);
	PyModule_AddIntConstant(poModule, "EMOTION_DANCE_4", EMOTION_DANCE_4);
	PyModule_AddIntConstant(poModule, "EMOTION_DANCE_5", EMOTION_DANCE_5);
	PyModule_AddIntConstant(poModule, "EMOTION_DANCE_6", EMOTION_DANCE_6); // PSY 강남스타일
	PyModule_AddIntConstant(poModule, "EMOTION_CONGRATULATION", EMOTION_CONGRATULATION);
	PyModule_AddIntConstant(poModule, "EMOTION_FORGIVE", EMOTION_FORGIVE);
	PyModule_AddIntConstant(poModule, "EMOTION_ANGRY", EMOTION_ANGRY);
	PyModule_AddIntConstant(poModule, "EMOTION_ATTRACTIVE", EMOTION_ATTRACTIVE);
	PyModule_AddIntConstant(poModule, "EMOTION_SAD", EMOTION_SAD);
	PyModule_AddIntConstant(poModule, "EMOTION_SHY", EMOTION_SHY);
	PyModule_AddIntConstant(poModule, "EMOTION_CHEERUP", EMOTION_CHEERUP);
	PyModule_AddIntConstant(poModule, "EMOTION_BANTER", EMOTION_BANTER);
	PyModule_AddIntConstant(poModule, "EMOTION_JOY", EMOTION_JOY);

	PyModule_AddIntConstant(poModule, "EMOTION_KISS", EMOTION_KISS);
	PyModule_AddIntConstant(poModule, "EMOTION_FRENCH_KISS", EMOTION_FRENCH_KISS);
	PyModule_AddIntConstant(poModule, "EMOTION_SLAP", EMOTION_SLAP);

	//// 자동물약 타입
	PyModule_AddIntConstant(poModule, "AUTO_POTION_TYPE_HP", CPythonPlayer::AUTO_POTION_TYPE_HP);
	PyModule_AddIntConstant(poModule, "AUTO_POTION_TYPE_SP", CPythonPlayer::AUTO_POTION_TYPE_SP);

#if defined(ENABLE_DRAGON_SOUL_SYSTEM)
	// 용혼석
	PyModule_AddIntConstant(poModule, "DRAGON_SOUL_PAGE_SIZE", c_DragonSoul_Inventory_Box_Size);
	PyModule_AddIntConstant(poModule, "DRAGON_SOUL_PAGE_COUNT", DRAGON_SOUL_GRADE_MAX);
	PyModule_AddIntConstant(poModule, "DRAGON_SOUL_SLOT_COUNT", c_DragonSoul_Inventory_Count);
	PyModule_AddIntConstant(poModule, "DRAGON_SOUL_EQUIPMENT_SLOT_START", c_DragonSoul_Equip_Start);
	PyModule_AddIntConstant(poModule, "DRAGON_SOUL_EQUIPMENT_SLOT_END", c_DragonSoul_Equip_End);
	PyModule_AddIntConstant(poModule, "DRAGON_SOUL_EQUIPMENT_PAGE_COUNT", DS_DECK_MAX_NUM);
	PyModule_AddIntConstant(poModule, "DRAGON_SOUL_EQUIPMENT_FIRST_SIZE", CItemData::DS_SLOT_NUM_TYPES);

	PyModule_AddIntConstant(poModule, "DRAGON_SOUL_GRADE_MYTH", DRAGON_SOUL_GRADE_MYTH);

	// 용혼석 개량창
	PyModule_AddIntConstant(poModule, "DRAGON_SOUL_REFINE_CLOSE", DS_SUB_HEADER_CLOSE);
	PyModule_AddIntConstant(poModule, "DS_SUB_HEADER_DO_UPGRADE", DS_SUB_HEADER_DO_UPGRADE);
	PyModule_AddIntConstant(poModule, "DS_SUB_HEADER_DO_IMPROVEMENT", DS_SUB_HEADER_DO_IMPROVEMENT);
	PyModule_AddIntConstant(poModule, "DS_SUB_HEADER_DO_REFINE", DS_SUB_HEADER_DO_REFINE);
#if defined(ENABLE_DS_CHANGE_ATTR)
	PyModule_AddIntConstant(poModule, "DS_SUB_HEADER_DO_CHANGE_ATTR", DS_SUB_HEADER_DO_CHANGE_ATTR);
#endif
#endif // ENABLE_DRAGON_SOUL_SYSTEM

#if defined(ENABLE_MOVE_COSTUME_ATTR)
	PyModule_AddIntConstant(poModule, "COMB_WND_SLOT_MEDIUM", ECombSlotType::COMB_WND_SLOT_MEDIUM);
	PyModule_AddIntConstant(poModule, "COMB_WND_SLOT_BASE", ECombSlotType::COMB_WND_SLOT_BASE);
	PyModule_AddIntConstant(poModule, "COMB_WND_SLOT_MATERIAL", ECombSlotType::COMB_WND_SLOT_MATERIAL);
	PyModule_AddIntConstant(poModule, "COMB_WND_SLOT_RESULT", ECombSlotType::COMB_WND_SLOT_RESULT);
	PyModule_AddIntConstant(poModule, "COMB_WND_SLOT_MAX", ECombSlotType::COMB_WND_SLOT_MAX);
#endif

#if defined(WJ_ENABLE_TRADABLE_ICON)
	PyModule_AddIntConstant(poModule, "ON_TOP_WND_NONE", ON_TOP_WND_NONE);
	PyModule_AddIntConstant(poModule, "ON_TOP_WND_SHOP", ON_TOP_WND_SHOP);
	PyModule_AddIntConstant(poModule, "ON_TOP_WND_EXCHANGE", ON_TOP_WND_EXCHANGE);
	PyModule_AddIntConstant(poModule, "ON_TOP_WND_SAFEBOX", ON_TOP_WND_SAFEBOX);
	PyModule_AddIntConstant(poModule, "ON_TOP_WND_PRIVATE_SHOP", ON_TOP_WND_PRIVATE_SHOP);
#if defined(ENABLE_MOVE_COSTUME_ATTR)
	PyModule_AddIntConstant(poModule, "ON_TOP_WND_ITEM_COMB", ON_TOP_WND_ITEM_COMB);
#endif
	//#if defined(ENABLE_GROWTH_PET_SYSTEM)
	PyModule_AddIntConstant(poModule, "ON_TOP_WND_PET_FEED", ON_TOP_WND_PET_FEED);
	//#endif
	PyModule_AddIntConstant(poModule, "ON_TOP_WND_FISH_EVENT", ON_TOP_WND_FISH_EVENT);
	PyModule_AddIntConstant(poModule, "ON_TOP_WND_MAILBOX", ON_TOP_WND_MAILBOX);
	//#if defined(ENABLE_GROWTH_PET_SYSTEM)
	PyModule_AddIntConstant(poModule, "ON_TOP_WND_PET_ATTR_CHANGE", ON_TOP_WND_PET_ATTR_CHANGE);
	//#endif
#if defined(ENABLE_LUCKY_BOX)
	PyModule_AddIntConstant(poModule, "ON_TOP_WND_LUCKY_BOX", ON_TOP_WND_LUCKY_BOX);
#endif
	PyModule_AddIntConstant(poModule, "ON_TOP_WND_ATTR67", ON_TOP_WND_ATTR67);
	//#if defined(ENABLE_GROWTH_PET_SYSTEM) && defined(ENABLE_PET_PRIMIUM_FEEDSTUFF)
	PyModule_AddIntConstant(poModule, "ON_TOP_WND_PET_PRIMIUM_FEEDSTUFF", ON_TOP_WND_PET_PRIMIUM_FEEDSTUFF);
	//#endif
	PyModule_AddIntConstant(poModule, "ON_TOP_WND_PASSIVE_ATTR", ON_TOP_WND_PASSIVE_ATTR);
#endif

#if defined(ENABLE_KEYCHANGE_SYSTEM)
	PyModule_AddIntConstant(poModule, "KEY_MOVE_UP_1", KEY_MOVE_UP_1);
	PyModule_AddIntConstant(poModule, "KEY_MOVE_DOWN_1", KEY_MOVE_DOWN_1);
	PyModule_AddIntConstant(poModule, "KEY_MOVE_LEFT_1", KEY_MOVE_LEFT_1);
	PyModule_AddIntConstant(poModule, "KEY_MOVE_RIGHT_1", KEY_MOVE_RIGHT_1);
	PyModule_AddIntConstant(poModule, "KEY_MOVE_UP_2", KEY_MOVE_UP_2);
	PyModule_AddIntConstant(poModule, "KEY_MOVE_DOWN_2", KEY_MOVE_DOWN_2);
	PyModule_AddIntConstant(poModule, "KEY_MOVE_LEFT_2", KEY_MOVE_LEFT_2);
	PyModule_AddIntConstant(poModule, "KEY_MOVE_RIGHT_2", KEY_MOVE_RIGHT_2);
	PyModule_AddIntConstant(poModule, "KEY_CAMERA_ROTATE_POSITIVE_1", KEY_CAMERA_ROTATE_POSITIVE_1);
	PyModule_AddIntConstant(poModule, "KEY_CAMERA_ROTATE_NEGATIVE_1", KEY_CAMERA_ROTATE_NEGATIVE_1);
	PyModule_AddIntConstant(poModule, "KEY_CAMERA_ZOOM_POSITIVE_1", KEY_CAMERA_ZOOM_POSITIVE_1);
	PyModule_AddIntConstant(poModule, "KEY_CAMERA_ZOOM_NEGATIVE_1", KEY_CAMERA_ZOOM_NEGATIVE_1);
	PyModule_AddIntConstant(poModule, "KEY_CAMERA_PITCH_POSITIVE_1", KEY_CAMERA_PITCH_POSITIVE_1);
	PyModule_AddIntConstant(poModule, "KEY_CAMERA_PITCH_NEGATIVE_1", KEY_CAMERA_PITCH_NEGATIVE_1);
	PyModule_AddIntConstant(poModule, "KEY_CAMERA_ROTATE_POSITIVE_2", KEY_CAMERA_ROTATE_POSITIVE_2);
	PyModule_AddIntConstant(poModule, "KEY_CAMERA_ROTATE_NEGATIVE_2", KEY_CAMERA_ROTATE_NEGATIVE_2);
	PyModule_AddIntConstant(poModule, "KEY_CAMERA_ZOOM_POSITIVE_2", KEY_CAMERA_ZOOM_POSITIVE_2);
	PyModule_AddIntConstant(poModule, "KEY_CAMERA_ZOOM_NEGATIVE_2", KEY_CAMERA_ZOOM_NEGATIVE_2);
	PyModule_AddIntConstant(poModule, "KEY_CAMERA_PITCH_POSITIVE_2", KEY_CAMERA_PITCH_POSITIVE_2);
	PyModule_AddIntConstant(poModule, "KEY_CAMERA_PITCH_NEGATIVE_2", KEY_CAMERA_PITCH_NEGATIVE_2);
	PyModule_AddIntConstant(poModule, "KEY_ROOTING_1", KEY_ROOTING_1);
	PyModule_AddIntConstant(poModule, "KEY_ROOTING_2", KEY_ROOTING_2);
	PyModule_AddIntConstant(poModule, "KEY_ATTACK", KEY_ATTACK);
	PyModule_AddIntConstant(poModule, "KEY_RIDEMYHORS", KEY_RIDEMYHORS);
	PyModule_AddIntConstant(poModule, "KEY_FEEDMYHORS", KEY_FEEDMYHORS);
	PyModule_AddIntConstant(poModule, "KEY_BYEMYHORS", KEY_BYEMYHORS);
	PyModule_AddIntConstant(poModule, "KEY_RIDEHORS", KEY_RIDEHORS);
	PyModule_AddIntConstant(poModule, "KEY_EMOTION1", KEY_EMOTION1);
	PyModule_AddIntConstant(poModule, "KEY_EMOTION2", KEY_EMOTION2);
	PyModule_AddIntConstant(poModule, "KEY_EMOTION3", KEY_EMOTION3);
	PyModule_AddIntConstant(poModule, "KEY_EMOTION4", KEY_EMOTION4);
	PyModule_AddIntConstant(poModule, "KEY_EMOTION5", KEY_EMOTION5);
	PyModule_AddIntConstant(poModule, "KEY_EMOTION6", KEY_EMOTION6);
	PyModule_AddIntConstant(poModule, "KEY_EMOTION7", KEY_EMOTION7);
	PyModule_AddIntConstant(poModule, "KEY_EMOTION8", KEY_EMOTION8);
	PyModule_AddIntConstant(poModule, "KEY_EMOTION9", KEY_EMOTION9);
	PyModule_AddIntConstant(poModule, "KEY_SLOT_1", KEY_SLOT_1);
	PyModule_AddIntConstant(poModule, "KEY_SLOT_2", KEY_SLOT_2);
	PyModule_AddIntConstant(poModule, "KEY_SLOT_3", KEY_SLOT_3);
	PyModule_AddIntConstant(poModule, "KEY_SLOT_4", KEY_SLOT_4);
	PyModule_AddIntConstant(poModule, "KEY_SLOT_5", KEY_SLOT_5);
	PyModule_AddIntConstant(poModule, "KEY_SLOT_6", KEY_SLOT_6);
	PyModule_AddIntConstant(poModule, "KEY_SLOT_7", KEY_SLOT_7);
	PyModule_AddIntConstant(poModule, "KEY_SLOT_8", KEY_SLOT_8);
	PyModule_AddIntConstant(poModule, "KEY_SLOT_CHANGE_1", KEY_SLOT_CHANGE_1);
	PyModule_AddIntConstant(poModule, "KEY_SLOT_CHANGE_2", KEY_SLOT_CHANGE_2);
	PyModule_AddIntConstant(poModule, "KEY_SLOT_CHANGE_3", KEY_SLOT_CHANGE_3);
	PyModule_AddIntConstant(poModule, "KEY_SLOT_CHANGE_4", KEY_SLOT_CHANGE_4);
	PyModule_AddIntConstant(poModule, "KEY_OPEN_STATE", KEY_OPEN_STATE);
	PyModule_AddIntConstant(poModule, "KEY_OPEN_SKILL", KEY_OPEN_SKILL);
	PyModule_AddIntConstant(poModule, "KEY_OPEN_QUEST", KEY_OPEN_QUEST);
	PyModule_AddIntConstant(poModule, "KEY_OPEN_INVENTORY", KEY_OPEN_INVENTORY);
	PyModule_AddIntConstant(poModule, "KEY_OPEN_DDS", KEY_OPEN_DDS);
	PyModule_AddIntConstant(poModule, "KEY_OPEN_MINIMAP", KEY_OPEN_MINIMAP);
	PyModule_AddIntConstant(poModule, "KEY_OPEN_LOGCHAT", KEY_OPEN_LOGCHAT);
	PyModule_AddIntConstant(poModule, "KEY_OPEN_PET", KEY_OPEN_PET);
	PyModule_AddIntConstant(poModule, "KEY_OPEN_GUILD", KEY_OPEN_GUILD);
	PyModule_AddIntConstant(poModule, "KEY_OPEN_MESSENGER", KEY_OPEN_MESSENGER);
	PyModule_AddIntConstant(poModule, "KEY_OPEN_HELP", KEY_OPEN_HELP);
	PyModule_AddIntConstant(poModule, "KEY_OPEN_ACTION", KEY_OPEN_ACTION);
	PyModule_AddIntConstant(poModule, "KEY_SCROLL_ONOFF", KEY_SCROLL_ONOFF);
	PyModule_AddIntConstant(poModule, "KEY_PLUS_MINIMAP", KEY_PLUS_MINIMAP);
	PyModule_AddIntConstant(poModule, "KEY_MIN_MINIMAP", KEY_MIN_MINIMAP);
	PyModule_AddIntConstant(poModule, "KEY_SCREENSHOT", KEY_SCREENSHOT);
	PyModule_AddIntConstant(poModule, "KEY_SHOW_NAME", KEY_SHOW_NAME);
	PyModule_AddIntConstant(poModule, "KEY_OPEN_AUTO", KEY_OPEN_AUTO);
	PyModule_AddIntConstant(poModule, "KEY_AUTO_RUN", KEY_AUTO_RUN);
	PyModule_AddIntConstant(poModule, "KEY_NEXT_TARGET", KEY_NEXT_TARGET);
	PyModule_AddIntConstant(poModule, "KEY_MONSTER_CARD", KEY_MONSTER_CARD);
	PyModule_AddIntConstant(poModule, "KEY_PARTY_MATCH", KEY_PARTY_MATCH);
	PyModule_AddIntConstant(poModule, "KEY_SELECT_DSS_1", KEY_SELECT_DSS_1);
	PyModule_AddIntConstant(poModule, "KEY_SELECT_DSS_2", KEY_SELECT_DSS_2);
	PyModule_AddIntConstant(poModule, "KEY_OPEN_IN_GAME_EVENT", KEY_OPEN_IN_GAME_EVENT);
	PyModule_AddIntConstant(poModule, "KEY_PASSIVE_ATTR1", KEY_PASSIVE_ATTR1);
	PyModule_AddIntConstant(poModule, "KEY_PASSIVE_ATTR2", KEY_PASSIVE_ATTR2);

	PyModule_AddIntConstant(poModule, "KEY_ADDKEYBUFFERCONTROL", KEY_ADDKEYBUFFERCONTROL);
	PyModule_AddIntConstant(poModule, "KEY_ADDKEYBUFFERALT", KEY_ADDKEYBUFFERALT);
	PyModule_AddIntConstant(poModule, "KEY_ADDKEYBUFFERSHIFT", KEY_ADDKEYBUFFERSHIFT);
#endif

#if defined(ENABLE_CONQUEROR_LEVEL)
	PyModule_AddIntConstant(poModule, "LEVEL_TYPE_BASE", LEVEL_TYPE_BASE);
	PyModule_AddIntConstant(poModule, "LEVEL_TYPE_CONQUEROR", LEVEL_TYPE_CONQUEROR);
	PyModule_AddIntConstant(poModule, "LEVEL_TYPE_MAX", LEVEL_TYPE_MAX);
#endif

#if defined(ENABLE_EVENT_BANNER)
#if defined(ENABLE_MINI_GAME_RUMI)
	PyModule_AddIntConstant(poModule, "MINIGAME_RUMI", CPythonPlayer::MINIGAME_RUMI);
#endif
#if defined(ENABLE_MINI_GAME_YUTNORI)
	PyModule_AddIntConstant(poModule, "MINIGAME_YUTNORI", CPythonPlayer::MINIGAME_YUTNORI);
#endif
#if defined(ENABLE_MINI_GAME_CATCH_KING)
	PyModule_AddIntConstant(poModule, "MINIGAME_CATCHKING", CPythonPlayer::MINIGAME_CATCHKING);
#endif
#if defined(ENABLE_SUMMER_EVENT_ROULETTE)
	PyModule_AddIntConstant(poModule, "MINIGAME_ROULETTE", CPythonPlayer::MINIGAME_ROULETTE);
#endif
#if defined(ENABLE_FLOWER_EVENT)
	PyModule_AddIntConstant(poModule, "FLOWER_EVENT", CPythonPlayer::FLOWER_EVENT);
#endif
#if defined(ENABLE_SNOWFLAKE_STICK_EVENT)
	PyModule_AddIntConstant(poModule, "SNOWFLAKE_STICK_EVENT", CPythonPlayer::SNOWFLAKE_STICK_EVENT);
#endif
	PyModule_AddIntConstant(poModule, "MINIGAME_TYPE_MAX", CPythonPlayer::MINIGAME_TYPE_MAX);
#endif

#if defined(ENABLE_LUCKY_BOX)
	PyModule_AddIntConstant(poModule, "LUCKY_BOX_ACTION_RETRY", ELUCKY_BOX_ACTION::LUCKY_BOX_ACTION_RETRY);
	PyModule_AddIntConstant(poModule, "LUCKY_BOX_ACTION_RECEIVE", ELUCKY_BOX_ACTION::LUCKY_BOX_ACTION_RECEIVE);
#endif

#if defined(ENABLE_ACCE_COSTUME_SYSTEM)
	PyModule_AddIntConstant(poModule, "ACCE_SLOT_TYPE_COMBINE", ACCE_SLOT_TYPE_COMBINE);
	PyModule_AddIntConstant(poModule, "ACCE_SLOT_TYPE_ABSORB", ACCE_SLOT_TYPE_ABSORB);

	PyModule_AddIntConstant(poModule, "ACCE_SLOT_LEFT", ACCE_SLOT_LEFT);
	PyModule_AddIntConstant(poModule, "ACCE_SLOT_RIGHT", ACCE_SLOT_RIGHT);
	PyModule_AddIntConstant(poModule, "ACCE_SLOT_MAX", ACCE_SLOT_MAX);
	PyModule_AddIntConstant(poModule, "ACCE_MAX_DRAINRATE", ACCE_DRAIN_RATE_MAX);
#endif

	PyModule_AddIntConstant(poModule, "SLOT_TYPE_NONE", SLOT_TYPE_NONE);
	PyModule_AddIntConstant(poModule, "SLOT_TYPE_INVENTORY", SLOT_TYPE_INVENTORY);
	PyModule_AddIntConstant(poModule, "SLOT_TYPE_SKILL", SLOT_TYPE_SKILL);
	PyModule_AddIntConstant(poModule, "SLOT_TYPE_EMOTION", SLOT_TYPE_EMOTION);
	PyModule_AddIntConstant(poModule, "SLOT_TYPE_SHOP", SLOT_TYPE_SHOP);
	PyModule_AddIntConstant(poModule, "SLOT_TYPE_EXCHANGE_OWNER", SLOT_TYPE_EXCHANGE_OWNER);
	PyModule_AddIntConstant(poModule, "SLOT_TYPE_EXCHANGE_TARGET", SLOT_TYPE_EXCHANGE_TARGET);
	PyModule_AddIntConstant(poModule, "SLOT_TYPE_QUICK_SLOT", SLOT_TYPE_QUICK_SLOT);
	PyModule_AddIntConstant(poModule, "SLOT_TYPE_SAFEBOX", SLOT_TYPE_SAFEBOX);
	PyModule_AddIntConstant(poModule, "SLOT_TYPE_GUILDBANK", SLOT_TYPE_GUILDBANK);
	PyModule_AddIntConstant(poModule, "SLOT_TYPE_ACCE", SLOT_TYPE_ACCE);
	PyModule_AddIntConstant(poModule, "SLOT_TYPE_PRIVATE_SHOP", SLOT_TYPE_PRIVATE_SHOP);
	PyModule_AddIntConstant(poModule, "SLOT_TYPE_MALL", SLOT_TYPE_MALL);
	PyModule_AddIntConstant(poModule, "SLOT_TYPE_DRAGON_SOUL_INVENTORY", SLOT_TYPE_DRAGON_SOUL_INVENTORY);
	PyModule_AddIntConstant(poModule, "SLOT_TYPE_PET_FEED_WINDOW", SLOT_TYPE_PET_FEED_WINDOW);
	PyModule_AddIntConstant(poModule, "SLOT_TYPE_EQUIPMENT", SLOT_TYPE_EQUIPMENT);
	PyModule_AddIntConstant(poModule, "SLOT_TYPE_BELT_INVENTORY", SLOT_TYPE_BELT_INVENTORY);
	PyModule_AddIntConstant(poModule, "SLOT_TYPE_AUTO", SLOT_TYPE_AUTO);
	PyModule_AddIntConstant(poModule, "SLOT_TYPE_CHANGE_LOOK", SLOT_TYPE_CHANGE_LOOK);
	PyModule_AddIntConstant(poModule, "SLOT_TYPE_FISH_EVENT", SLOT_TYPE_FISH_EVENT);
	PyModule_AddIntConstant(poModule, "SLOT_TYPE_AURA", SLOT_TYPE_AURA);
	PyModule_AddIntConstant(poModule, "SLOT_TYPE_PREMIUM_PRIVATE_SHOP", SLOT_TYPE_PREMIUM_PRIVATE_SHOP);
	PyModule_AddIntConstant(poModule, "SLOT_TYPE_GOLDEN_LAND_FRUIT", SLOT_TYPE_GOLDEN_LAND_FRUIT);

#if defined(ENABLE_EXTEND_INVEN_SYSTEM)
	PyModule_AddIntConstant(poModule, "EX_INVENTORY_PAGE_COUNT", EX_INVENTORY_PAGE_COUNT);
	PyModule_AddIntConstant(poModule, "EX_INVENTORY_PAGE_START", EX_INVENTORY_PAGE_START);
	PyModule_AddIntConstant(poModule, "EX_INVENTORY_STAGE_MAX", EX_INVENTORY_STAGE_MAX);

	PyModule_AddIntConstant(poModule, "EX_INVEN_FAIL_FALL_SHORT", EX_INVEN_FAIL_FALL_SHORT);
	PyModule_AddIntConstant(poModule, "EX_INVEN_SUCCESS", EX_INVEN_SUCCESS);
	PyModule_AddIntConstant(poModule, "EX_INVEN_FAIL_FOURTH_PAGE_STAGE_MAX", EX_INVEN_FAIL_FOURTH_PAGE_STAGE_MAX);
#endif

#if defined(ENABLE_CLIENT_TIMER)
	PyModule_AddIntConstant(poModule, "CLIENT_TIMER_SUBHEADER_GC_SET", CLIENT_TIMER_SUBHEADER_GC_SET);
	PyModule_AddIntConstant(poModule, "CLIENT_TIMER_SUBHEADER_GC_DELETE", CLIENT_TIMER_SUBHEADER_GC_DELETE);

	PyModule_AddIntConstant(poModule, "ECLIENT_TIMER_END_TIME", ECLIENT_TIMER_END_TIME);
	PyModule_AddIntConstant(poModule, "ECLIENT_TIMER_ALARM_SECOND", ECLIENT_TIMER_ALARM_SECOND);
	PyModule_AddIntConstant(poModule, "ECLIENT_TIMER_MAX", ECLIENT_TIMER_MAX);
#endif

	PyModule_AddIntConstant(poModule, "EMOTION_CLAP", EMOTION_CLAP);
	PyModule_AddIntConstant(poModule, "EMOTION_CONGRATULATION", EMOTION_CONGRATULATION);
	PyModule_AddIntConstant(poModule, "EMOTION_FORGIVE", EMOTION_FORGIVE);
	PyModule_AddIntConstant(poModule, "EMOTION_ANGRY", EMOTION_ANGRY);
	PyModule_AddIntConstant(poModule, "EMOTION_ATTRACTIVE", EMOTION_ATTRACTIVE);
	PyModule_AddIntConstant(poModule, "EMOTION_SAD", EMOTION_SAD);
	PyModule_AddIntConstant(poModule, "EMOTION_SHY", EMOTION_SHY);
	PyModule_AddIntConstant(poModule, "EMOTION_CHEERUP", EMOTION_CHEERUP);
	PyModule_AddIntConstant(poModule, "EMOTION_BANTER", EMOTION_BANTER);
	PyModule_AddIntConstant(poModule, "EMOTION_JOY", EMOTION_JOY);
	PyModule_AddIntConstant(poModule, "EMOTION_CHEERS_1", EMOTION_CHEERS_1);
	PyModule_AddIntConstant(poModule, "EMOTION_CHEERS_2", EMOTION_CHEERS_2);
	PyModule_AddIntConstant(poModule, "EMOTION_DANCE_1", EMOTION_DANCE_1);
	PyModule_AddIntConstant(poModule, "EMOTION_DANCE_2", EMOTION_DANCE_2);
	PyModule_AddIntConstant(poModule, "EMOTION_DANCE_3", EMOTION_DANCE_3);
	PyModule_AddIntConstant(poModule, "EMOTION_DANCE_4", EMOTION_DANCE_4);
	PyModule_AddIntConstant(poModule, "EMOTION_DANCE_5", EMOTION_DANCE_5);
	PyModule_AddIntConstant(poModule, "EMOTION_DANCE_6", EMOTION_DANCE_6);
	PyModule_AddIntConstant(poModule, "EMOTION_KISS", EMOTION_KISS);
	PyModule_AddIntConstant(poModule, "EMOTION_FRENCH_KISS", EMOTION_FRENCH_KISS);
	PyModule_AddIntConstant(poModule, "EMOTION_SLAP", EMOTION_SLAP);
#if defined(ENABLE_EXPRESSING_EMOTION)
	PyModule_AddIntConstant(poModule, "EMOTION_PUSH_UP", EMOTION_PUSH_UP);
	PyModule_AddIntConstant(poModule, "EMOTION_DANCE_7", EMOTION_DANCE_7);
	PyModule_AddIntConstant(poModule, "EMOTION_EXERCISE", EMOTION_EXERCISE);
	PyModule_AddIntConstant(poModule, "EMOTION_DOZE", EMOTION_DOZE);
	PyModule_AddIntConstant(poModule, "EMOTION_SELFIE", EMOTION_SELFIE);
	PyModule_AddIntConstant(poModule, "EMOTION_CHARGING", EMOTION_CHARGING);
	PyModule_AddIntConstant(poModule, "EMOTION_NOSAY", EMOTION_NOSAY);
	PyModule_AddIntConstant(poModule, "EMOTION_WEATHER_1", EMOTION_WEATHER_1);
	PyModule_AddIntConstant(poModule, "EMOTION_WEATHER_2", EMOTION_WEATHER_2);
	PyModule_AddIntConstant(poModule, "EMOTION_WEATHER_3", EMOTION_WEATHER_3);
	PyModule_AddIntConstant(poModule, "EMOTION_HUNGRY", EMOTION_HUNGRY);
	PyModule_AddIntConstant(poModule, "EMOTION_SIREN", EMOTION_SIREN);
	PyModule_AddIntConstant(poModule, "EMOTION_LETTER", EMOTION_LETTER);
	PyModule_AddIntConstant(poModule, "EMOTION_CALL", EMOTION_CALL);
	PyModule_AddIntConstant(poModule, "EMOTION_CELEBRATION", EMOTION_CELEBRATION);
	PyModule_AddIntConstant(poModule, "EMOTION_ALCOHOL", EMOTION_ALCOHOL);
	PyModule_AddIntConstant(poModule, "EMOTION_BUSY", EMOTION_BUSY);
	PyModule_AddIntConstant(poModule, "EMOTION_WHIRL", EMOTION_WHIRL);
#endif

#if defined(ENABLE_CUBE_RENEWAL)
	PyModule_AddIntConstant(poModule, "CUBEITEM_NPC_VNUM", CPythonCubeManager::CUBEITEM_NPC_VNUM);
	PyModule_AddIntConstant(poModule, "CUBEITEM_REWARD_ITEM", CPythonCubeManager::CUBEITEM_REWARD_ITEM);
	PyModule_AddIntConstant(poModule, "CUBEITEM_MATERIAL_ITEM", CPythonCubeManager::CUBEITEM_MATERIAL_ITEM);
	PyModule_AddIntConstant(poModule, "CUBEITEM_GOLD", CPythonCubeManager::CUBEITEM_GOLD);
	PyModule_AddIntConstant(poModule, "CUBEITEM_CATEGORY", CPythonCubeManager::CUBEITEM_CATEGORY);
	PyModule_AddIntConstant(poModule, "CUBEITEM_PROBABILITY", CPythonCubeManager::CUBEITEM_PROBABILITY);
	PyModule_AddIntConstant(poModule, "CUBEITEM_GEM", CPythonCubeManager::CUBEITEM_GEM);
	PyModule_AddIntConstant(poModule, "CUBEITEM_SET_VALUE", CPythonCubeManager::CUBEITEM_SET_VALUE);
	PyModule_AddIntConstant(poModule, "CUBEITEM_SUB_CATEGORY", CPythonCubeManager::CUBEITEM_SUB_CATEGORY);

	PyModule_AddIntConstant(poModule, "CUBE_ARMOR", CPythonCubeManager::CUBE_ARMOR);
	PyModule_AddIntConstant(poModule, "CUBE_WEAPON", CPythonCubeManager::CUBE_WEAPON);
	PyModule_AddIntConstant(poModule, "CUBE_ACCESSORY", CPythonCubeManager::CUBE_ACCESSORY);
	PyModule_AddIntConstant(poModule, "CUBE_BELT", CPythonCubeManager::CUBE_BELT);
	PyModule_AddIntConstant(poModule, "CUBE_EVENT", CPythonCubeManager::CUBE_EVENT);
	PyModule_AddIntConstant(poModule, "CUBE_ETC", CPythonCubeManager::CUBE_ETC);
	PyModule_AddIntConstant(poModule, "CUBE_JOB", CPythonCubeManager::CUBE_JOB);
	PyModule_AddIntConstant(poModule, "CUBE_SETADD_WEAPON", CPythonCubeManager::CUBE_SETADD_WEAPON);
	PyModule_AddIntConstant(poModule, "CUBE_SETADD_ARMOR_BODY", CPythonCubeManager::CUBE_SETADD_ARMOR_BODY);
	PyModule_AddIntConstant(poModule, "CUBE_SETADD_ARMOR_HELMET", CPythonCubeManager::CUBE_SETADD_ARMOR_HELMET);
	PyModule_AddIntConstant(poModule, "CUBE_PET", CPythonCubeManager::CUBE_PET);
	PyModule_AddIntConstant(poModule, "CUBE_SKILL_BOOK", CPythonCubeManager::CUBE_SKILL_BOOK);
	PyModule_AddIntConstant(poModule, "CUBE_ARMOR_GLOVE", CPythonCubeManager::CUBE_ARMOR_GLOVE);
	PyModule_AddIntConstant(poModule, "CUBE_ALCHEMY", CPythonCubeManager::CUBE_ALCHEMY);
	PyModule_AddIntConstant(poModule, "CUBE_CATEGORY_MAX", CPythonCubeManager::CUBE_CATEGORY_MAX);

	PyModule_AddIntConstant(poModule, "CUBE_MIN_ITEM_ATTR_WARNING", CPythonCubeManager::CUBE_MIN_ITEM_ATTR_WARNING);
	PyModule_AddIntConstant(poModule, "CUBE_MAX_MAKE_QUANTITY", CPythonCubeManager::CUBE_MAX_MAKE_QUANTITY);
	PyModule_AddIntConstant(poModule, "CUBE_MAX_MATERIAL_QUANTITY", CPythonCubeManager::CUBE_MAX_MATERIAL_QUANTITY);
	PyModule_AddIntConstant(poModule, "CUBE_MAX_MATERIAL_COUNT", CPythonCubeManager::CUBE_MAX_MATERIAL_COUNT);
#endif

#if defined(ENABLE_SET_ITEM)
	PyModule_AddIntConstant(poModule, "SET_ITEM_SET_VALUE_NONE", SET_ITEM_SET_VALUE_NONE);
	PyModule_AddIntConstant(poModule, "SET_ITEM_SET_VALUE_1", SET_ITEM_SET_VALUE_1);
	PyModule_AddIntConstant(poModule, "SET_ITEM_SET_VALUE_2", SET_ITEM_SET_VALUE_2);
	PyModule_AddIntConstant(poModule, "SET_ITEM_SET_VALUE_3", SET_ITEM_SET_VALUE_3);
	PyModule_AddIntConstant(poModule, "SET_ITEM_SET_VALUE_4", SET_ITEM_SET_VALUE_4);
	PyModule_AddIntConstant(poModule, "SET_ITEM_SET_VALUE_5", SET_ITEM_SET_VALUE_5);
	PyModule_AddIntConstant(poModule, "SET_ITEM_SET_VALUE_MAX", SET_ITEM_SET_VALUE_MAX);
#endif

#if defined(ENABLE_AURA_COSTUME_SYSTEM)
	PyModule_AddIntConstant(poModule, "AURA_MAX_LEVEL", AURA_MAX_LEVEL);

	PyModule_AddIntConstant(poModule, "AURA_SLOT_MAIN", AURA_SLOT_MAIN);
	PyModule_AddIntConstant(poModule, "AURA_SLOT_SUB", AURA_SLOT_SUB);
	PyModule_AddIntConstant(poModule, "AURA_SLOT_RESULT", AURA_SLOT_RESULT);
	PyModule_AddIntConstant(poModule, "AURA_SLOT_MAX", AURA_SLOT_MAX);

	PyModule_AddIntConstant(poModule, "AURA_WINDOW_TYPE_ABSORB", AURA_WINDOW_TYPE_ABSORB);
	PyModule_AddIntConstant(poModule, "AURA_WINDOW_TYPE_GROWTH", AURA_WINDOW_TYPE_GROWTH);
	PyModule_AddIntConstant(poModule, "AURA_WINDOW_TYPE_EVOLVE", AURA_WINDOW_TYPE_EVOLVE);
	PyModule_AddIntConstant(poModule, "AURA_WINDOW_TYPE_MAX", AURA_WINDOW_TYPE_MAX);

	PyModule_AddIntConstant(poModule, "AURA_REFINE_INFO_SLOT_CURRENT", AURA_REFINE_INFO_SLOT_CURRENT);
	PyModule_AddIntConstant(poModule, "AURA_REFINE_INFO_SLOT_NEXT", AURA_REFINE_INFO_SLOT_NEXT);
	PyModule_AddIntConstant(poModule, "AURA_REFINE_INFO_SLOT_EVOLVED", AURA_REFINE_INFO_SLOT_EVOLVED);
	PyModule_AddIntConstant(poModule, "AURA_WINDOW_TYPE_MAX", AURA_WINDOW_TYPE_MAX);
#endif

#if defined(ENABLE_REFINE_ELEMENT_SYSTEM)
	PyModule_AddIntConstant(poModule, "REFINE_ELEMENT_GC_OPEN", REFINE_ELEMENT_GC_OPEN);
	PyModule_AddIntConstant(poModule, "REFINE_ELEMENT_GC_RESULT", REFINE_ELEMENT_GC_RESULT);

	PyModule_AddIntConstant(poModule, "REFINE_ELEMENT_UPGRADE", REFINE_ELEMENT_UPGRADE);
	PyModule_AddIntConstant(poModule, "REFINE_ELEMENT_DOWNGRADE", REFINE_ELEMENT_DOWNGRADE);
	PyModule_AddIntConstant(poModule, "REFINE_ELEMENT_CHANGE", REFINE_ELEMENT_CHANGE);
#endif

#if defined(ENABLE_MINI_GAME_RUMI) && defined(ENABLE_OKEY_EVENT_FLAG_RENEWAL)
	PyModule_AddIntConstant(poModule, "RUMI_GC_SUBHEADER_SET_CARD_PIECE_FLAG", RUMI_GC_SUBHEADER_SET_CARD_PIECE_FLAG);
	PyModule_AddIntConstant(poModule, "RUMI_GC_SUBHEADER_SET_CARD_FLAG", RUMI_GC_SUBHEADER_SET_CARD_FLAG);
	PyModule_AddIntConstant(poModule, "RUMI_GC_SUBHEADER_SET_QUEST_FLAG", RUMI_GC_SUBHEADER_SET_QUEST_FLAG);
	PyModule_AddIntConstant(poModule, "RUMI_GC_SUBHEADER_NO_MORE_GAIN", RUMI_GC_SUBHEADER_NO_MORE_GAIN);
#endif

#if defined(ENABLE_MINI_GAME_YUTNORI) && defined(ENABLE_YUTNORI_EVENT_FLAG_RENEWAL)
	PyModule_AddIntConstant(poModule, "YUTNORI_GC_SUBHEADER_SET_YUT_PIECE_FLAG", YUTNORI_GC_SUBHEADER_SET_YUT_PIECE_FLAG);
	PyModule_AddIntConstant(poModule, "YUTNORI_GC_SUBHEADER_SET_YUT_BOARD_FLAG", YUTNORI_GC_SUBHEADER_SET_YUT_BOARD_FLAG);
	PyModule_AddIntConstant(poModule, "YUTNORI_GC_SUBHEADER_SET_QUEST_FLAG", YUTNORI_GC_SUBHEADER_SET_QUEST_FLAG);
	PyModule_AddIntConstant(poModule, "YUTNORI_GC_SUBHEADER_NO_MORE_GAIN", YUTNORI_GC_SUBHEADER_NO_MORE_GAIN);
#endif

#if defined(ENABLE_MINI_GAME_CATCH_KING) && defined(ENABLE_CATCH_KING_EVENT_FLAG_RENEWAL)
	PyModule_AddIntConstant(poModule, "CATCHKING_GC_SET_CARD_PIECE_FLAG", CATCHKING_GC_SET_CARD_PIECE_FLAG);
	PyModule_AddIntConstant(poModule, "CATCHKING_GC_SET_CARD_FLAG", CATCHKING_GC_SET_CARD_FLAG);
	PyModule_AddIntConstant(poModule, "CATCHKING_GC_SET_QUEST_FLAG", CATCHKING_GC_SET_QUEST_FLAG);
	PyModule_AddIntConstant(poModule, "CATCHKING_GC_NO_MORE_GAIN", CATCHKING_GC_NO_MORE_GAIN);
#endif

#if defined(ENABLE_LEFT_SEAT)
	PyModule_AddIntConstant(poModule, "LEFT_SEAT_TIME_10_MIN", LEFT_SEAT_TIME_10_MIN);
	PyModule_AddIntConstant(poModule, "LEFT_SEAT_TIME_30_MIN", LEFT_SEAT_TIME_30_MIN);
	PyModule_AddIntConstant(poModule, "LEFT_SEAT_TIME_90_MIN", LEFT_SEAT_TIME_90_MIN);
	PyModule_AddIntConstant(poModule, "LEFT_SEAT_TIME_MAX", LEFT_SEAT_TIME_MAX);

	PyModule_AddIntConstant(poModule, "LEFT_SEAT_LOGOUT_TIME_30_MIN", LEFT_SEAT_LOGOUT_TIME_30_MIN);
	PyModule_AddIntConstant(poModule, "LEFT_SEAT_LOGOUT_TIME_60_MIN", LEFT_SEAT_LOGOUT_TIME_60_MIN);
	PyModule_AddIntConstant(poModule, "LEFT_SEAT_LOGOUT_TIME_120_MIN", LEFT_SEAT_LOGOUT_TIME_120_MIN);
	PyModule_AddIntConstant(poModule, "LEFT_SEAT_LOGOUT_TIME_180_MIN", LEFT_SEAT_LOGOUT_TIME_180_MIN);
	PyModule_AddIntConstant(poModule, "LEFT_SEAT_LOGOUT_TIME_OFF", LEFT_SEAT_LOGOUT_TIME_OFF);
	PyModule_AddIntConstant(poModule, "LEFT_SEAT_LOGOUT_TIME_MAX", LEFT_SEAT_LOGOUT_TIME_MAX);
#endif

	PyModule_AddIntConstant(poModule, "DEAD_DIALOG_NONE", DEAD_DIALOG_NONE);
	PyModule_AddIntConstant(poModule, "DEAD_DIALOG_NORMAL", DEAD_DIALOG_NORMAL);
	PyModule_AddIntConstant(poModule, "DEAD_DIALOG_GIVE_UP", DEAD_DIALOG_GIVE_UP);

#if defined(ENABLE_SUMMER_EVENT_ROULETTE)
	PyModule_AddIntConstant(poModule, "ROULETTE_GC_OPEN", ROULETTE_GC_OPEN);
	PyModule_AddIntConstant(poModule, "ROULETTE_GC_START", ROULETTE_GC_START);
	PyModule_AddIntConstant(poModule, "ROULETTE_GC_REQUEST", ROULETTE_GC_REQUEST);
	PyModule_AddIntConstant(poModule, "ROULETTE_GC_END", ROULETTE_GC_END);
	PyModule_AddIntConstant(poModule, "ROULETTE_GC_CLOSE", ROULETTE_GC_CLOSE);
#endif

#if defined(ENABLE_FLOWER_EVENT)
	PyModule_AddIntConstant(poModule, "SHOOT_ENVELOPE", SHOOT_ENVELOPE);
	PyModule_AddIntConstant(poModule, "SHOOT_CHRYSANTHEMUM", SHOOT_CHRYSANTHEMUM);
	PyModule_AddIntConstant(poModule, "SHOOT_MAY_BELL", SHOOT_MAY_BELL);
	PyModule_AddIntConstant(poModule, "SHOOT_DAFFODIL", SHOOT_DAFFODIL);
	PyModule_AddIntConstant(poModule, "SHOOT_LILY", SHOOT_LILY);
	PyModule_AddIntConstant(poModule, "SHOOT_SUNFLOWER", SHOOT_SUNFLOWER);
	PyModule_AddIntConstant(poModule, "SHOOT_TYPE_MAX", SHOOT_TYPE_MAX);

	PyModule_AddIntConstant(poModule, "FLOWER_EVENT_SHOOT_ENVELOPE_NEED_COUNT", FLOWER_EVENT_SHOOT_ENVELOPE_NEED_COUNT);
	PyModule_AddIntConstant(poModule, "FLOWER_EVENT_SHOOT_NEED_COUNT", FLOWER_EVENT_SHOOT_NEED_COUNT);
	PyModule_AddIntConstant(poModule, "FLOWER_EVENT_NEED_INVENTORY_SPACE", FLOWER_EVENT_NEED_INVENTORY_SPACE);
	PyModule_AddIntConstant(poModule, "FLOWER_EVENT_EXCHANGE_COOLTIME_SEC", FLOWER_EVENT_EXCHANGE_COOLTIME_SEC);
	PyModule_AddIntConstant(poModule, "MAX_FLOWER_EVENT_ITEM_COUNT", MAX_FLOWER_EVENT_ITEM_COUNT);

	PyModule_AddIntConstant(poModule, "FLOWER_EVENT_CHAT_TYPE_NOT_ENOUGH_SHOOT_COUNT", FLOWER_EVENT_CHAT_TYPE_NOT_ENOUGH_SHOOT_COUNT);
	PyModule_AddIntConstant(poModule, "FLOWER_EVENT_CHAT_TYPE_NOT_ENOUGH_EVENTORY_SPACE", FLOWER_EVENT_CHAT_TYPE_NOT_ENOUGH_EVENTORY_SPACE);
	PyModule_AddIntConstant(poModule, "FLOWER_EVENT_CHAT_TYPE_NOT_ENOUGH_SHOOT_ENVELOPE", FLOWER_EVENT_CHAT_TYPE_NOT_ENOUGH_SHOOT_ENVELOPE);
	PyModule_AddIntConstant(poModule, "FLOWER_EVENT_CHAT_TYPE_GET_SHOOT_ENVELOPE", FLOWER_EVENT_CHAT_TYPE_GET_SHOOT_ENVELOPE);
	PyModule_AddIntConstant(poModule, "FLOWER_EVENT_CHAT_TYPE_GET_SHOOT_CHRYSANTHEMUM", FLOWER_EVENT_CHAT_TYPE_GET_SHOOT_CHRYSANTHEMUM);
	PyModule_AddIntConstant(poModule, "FLOWER_EVENT_CHAT_TYPE_GET_SHOOT_MAY_BELL", FLOWER_EVENT_CHAT_TYPE_GET_SHOOT_MAY_BELL);
	PyModule_AddIntConstant(poModule, "FLOWER_EVENT_CHAT_TYPE_GET_SHOOT_DAFFODIL", FLOWER_EVENT_CHAT_TYPE_GET_SHOOT_DAFFODIL);
	PyModule_AddIntConstant(poModule, "FLOWER_EVENT_CHAT_TYPE_GET_SHOOT_LILY", FLOWER_EVENT_CHAT_TYPE_GET_SHOOT_LILY);
	PyModule_AddIntConstant(poModule, "FLOWER_EVENT_CHAT_TYPE_GET_SHOOT_SUNFLOWER", FLOWER_EVENT_CHAT_TYPE_GET_SHOOT_SUNFLOWER);
	PyModule_AddIntConstant(poModule, "FLOWER_EVENT_CHAT_TYPE_ITEM_FULL_AND_NOT_USE", FLOWER_EVENT_CHAT_TYPE_ITEM_FULL_AND_NOT_USE);
	PyModule_AddIntConstant(poModule, "FLOWER_EVENT_CHAT_TYPE_GET_SHOOT_CHRYSANTHEMUM_COUNT", FLOWER_EVENT_CHAT_TYPE_GET_SHOOT_CHRYSANTHEMUM_COUNT);
	PyModule_AddIntConstant(poModule, "FLOWER_EVENT_CHAT_TYPE_GET_SHOOT_MAY_BELL_COUNT", FLOWER_EVENT_CHAT_TYPE_GET_SHOOT_MAY_BELL_COUNT);
	PyModule_AddIntConstant(poModule, "FLOWER_EVENT_CHAT_TYPE_GET_SHOOT_DAFFODIL_COUNT", FLOWER_EVENT_CHAT_TYPE_GET_SHOOT_DAFFODIL_COUNT);
	PyModule_AddIntConstant(poModule, "FLOWER_EVENT_CHAT_TYPE_GET_SHOOT_LILY_COUNT", FLOWER_EVENT_CHAT_TYPE_GET_SHOOT_LILY_COUNT);
	PyModule_AddIntConstant(poModule, "FLOWER_EVENT_CHAT_TYPE_GET_SHOOT_SUNFLOWER_COUNT", FLOWER_EVENT_CHAT_TYPE_GET_SHOOT_SUNFLOWER_COUNT);
	PyModule_AddIntConstant(poModule, "FLOWER_EVENT_CHAT_TYPE_ENVELOPE_MAX", FLOWER_EVENT_CHAT_TYPE_ENVELOPE_MAX);
	PyModule_AddIntConstant(poModule, "FLOWER_EVENT_CHAT_TYPE_MAX", FLOWER_EVENT_CHAT_TYPE_MAX);
#endif
}
