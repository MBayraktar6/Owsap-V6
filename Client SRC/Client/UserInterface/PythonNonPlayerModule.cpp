#include "StdAfx.h"
#include "PythonNonPlayer.h"
#include "InstanceBase.h"
#include "PythonCharacterManager.h"

PyObject* nonplayerGetEventType(PyObject* poSelf, PyObject* poArgs)
{
	int iVirtualNumber;
	if (!PyTuple_GetInteger(poArgs, 0, &iVirtualNumber))
		return Py_BuildException();

	BYTE iType = CPythonNonPlayer::Instance().GetEventType(iVirtualNumber);

	return Py_BuildValue("i", iType);
}

PyObject* nonplayerGetEventTypeByVID(PyObject* poSelf, PyObject* poArgs)
{
	int iVirtualID;
	if (!PyTuple_GetInteger(poArgs, 0, &iVirtualID))
		return Py_BuildException();

	BYTE iType = CPythonNonPlayer::Instance().GetEventTypeByVID(iVirtualID);

	return Py_BuildValue("i", iType);
}

PyObject* nonplayerGetVnumByVID(PyObject* poSelf, PyObject* poArgs)
{
	int iVirtualID;
	if (!PyTuple_GetInteger(poArgs, 0, &iVirtualID))
		return Py_BuildException();

	CInstanceBase* pInstance = CPythonCharacterManager::Instance().GetInstancePtr(iVirtualID);
	if (!pInstance)
		return Py_BuildValue("i", -1);

	const CPythonNonPlayer::TMobTable* pMobTable = CPythonNonPlayer::Instance().GetTable(pInstance->GetVirtualNumber());
	if (!pMobTable)
		return Py_BuildValue("i", -1);

	return Py_BuildValue("i", pMobTable->dwVnum);
}

PyObject* nonplayerGetLevelByVID(PyObject* poSelf, PyObject* poArgs)
{
	int iVirtualID;
	if (!PyTuple_GetInteger(poArgs, 0, &iVirtualID))
		return Py_BuildException();

	CInstanceBase* pInstance = CPythonCharacterManager::Instance().GetInstancePtr(iVirtualID);

	if (!pInstance)
		return Py_BuildValue("i", -1);

	const CPythonNonPlayer::TMobTable* pMobTable = CPythonNonPlayer::Instance().GetTable(pInstance->GetVirtualNumber());

	if (!pMobTable)
		return Py_BuildValue("i", -1);

	float fAverageLevel = pMobTable->bLevel; //(float(pMobTable->abLevelRange[0]) + float(pMobTable->abLevelRange[1])) / 2.0f;
	fAverageLevel = floor(fAverageLevel + 0.5f);
	return Py_BuildValue("i", int(fAverageLevel));
}

PyObject* nonplayerGetGradeByVID(PyObject* poSelf, PyObject* poArgs)
{
	int iVirtualID;
	if (!PyTuple_GetInteger(poArgs, 0, &iVirtualID))
		return Py_BuildException();

	CInstanceBase* pInstance = CPythonCharacterManager::Instance().GetInstancePtr(iVirtualID);

	if (!pInstance)
		return Py_BuildValue("i", -1);

	const CPythonNonPlayer::TMobTable* pMobTable = CPythonNonPlayer::Instance().GetTable(pInstance->GetVirtualNumber());

	if (!pMobTable)
		return Py_BuildValue("i", -1);

	return Py_BuildValue("i", pMobTable->bRank);
}

PyObject* nonplayerGetMonsterName(PyObject* poSelf, PyObject* poArgs)
{
	int iVNum;
	if (!PyTuple_GetInteger(poArgs, 0, &iVNum))
		return Py_BuildException();

	CPythonNonPlayer& rkNonPlayer = CPythonNonPlayer::Instance();
	return Py_BuildValue("s", rkNonPlayer.GetMonsterName(iVNum));
}

PyObject* nonplayerLoadNonPlayerData(PyObject* poSelf, PyObject* poArgs)
{
	char* szFileName;
	if (!PyTuple_GetString(poArgs, 0, &szFileName))
		return Py_BuildException();

	CPythonNonPlayer::Instance().LoadNonPlayerData(szFileName);
	return Py_BuildNone();
}

#if defined(ENABLE_SEND_TARGET_INFO)
PyObject* nonplayerGetRaceByVID(PyObject* poSelf, PyObject* poArgs)
{
	int iVID;
	if (!PyTuple_GetInteger(poArgs, 0, &iVID))
		return Py_BuildException();

	CInstanceBase* pInstance = CPythonCharacterManager::Instance().GetInstancePtr(iVID);
	if (NULL == pInstance)
		return Py_BuildValue("i", -1);

	CPythonNonPlayer::TMobTable* pMobData = CPythonNonPlayer::Instance().GetTable(pInstance->GetVirtualNumber());
	if (NULL == pMobData)
		return Py_BuildValue("i", -1);

	return Py_BuildValue("i", pMobData->dwVnum);
}

PyObject* nonplayerSelectMob(PyObject* poSelf, PyObject* poArgs)
{
	int iVnum;
	if (!PyTuple_GetInteger(poArgs, 0, &iVnum))
		return Py_BadArgument();

	if (!CPythonNonPlayer::Instance().SelectMobData(iVnum))
		TraceError("Cannot select mob data by race %d", iVnum);

	return Py_BuildNone();
}

PyObject* nonplayerIsEnemy(PyObject* poSelf, PyObject* poArgs)
{
	const CPythonNonPlayer::TMobTable* pMobData = CPythonNonPlayer::Instance().GetSelectedMobDataPointer();
	if (NULL == pMobData)
		return Py_BuildException("Cannot select mob data.");

	return Py_BuildValue("b", pMobData->bType == CActorInstance::TYPE_ENEMY);
}

PyObject* nonplayerIsStone(PyObject* poSelf, PyObject* poArgs)
{
	const CPythonNonPlayer::TMobTable* pMobData = CPythonNonPlayer::Instance().GetSelectedMobDataPointer();
	if (NULL == pMobData)
		return Py_BuildException("Cannot select mob data.");

	return Py_BuildValue("b", pMobData->bType == CActorInstance::TYPE_STONE);
}

PyObject* nonplayerGetLevel(PyObject* poSelf, PyObject* poArgs)
{
	const CPythonNonPlayer::TMobTable* pMobData = CPythonNonPlayer::Instance().GetSelectedMobDataPointer();
	if (NULL == pMobData)
		return Py_BuildException("Cannot select mob data.");

	return Py_BuildValue("i", pMobData->bLevel);
}

#if defined(__HIT_RANGE_RENEWAL__)
PyObject* nonplayerGetHitRange(PyObject* poSelf, PyObject* poArgs)
{
	const CPythonNonPlayer::TMobTable* pMobData = CPythonNonPlayer::Instance().GetSelectedMobDataPointer();
	if (NULL == pMobData)
		return Py_BuildException("Cannot select mob data.");

	return Py_BuildValue("f", CPythonNonPlayer::Instance().GetMonsterHitRange(pMobData->dwVnum));
}
#endif

PyObject* nonplayerGetExp(PyObject* poSelf, PyObject* poArgs)
{
	CPythonNonPlayer::TMobTable* pMobData = CPythonNonPlayer::Instance().GetSelectedMobDataPointer();
	if (NULL == pMobData)
		return Py_BuildException("Cannot select mob data.");

	return Py_BuildValue("i", pMobData->dwExp);
}

#if defined(ENABLE_CONQUEROR_LEVEL)
PyObject* nonplayerGetSungMaExp(PyObject* poSelf, PyObject* poArgs)
{
	CPythonNonPlayer::TMobTable* pMobData = CPythonNonPlayer::Instance().GetSelectedMobDataPointer();
	if (NULL == pMobData)
		return Py_BuildException("Cannot select mob data.");

	return Py_BuildValue("i", pMobData->dwSungMaExp);
}
#endif

PyObject* nonplayerGetMaxHP(PyObject* poSelf, PyObject* poArgs)
{
	CPythonNonPlayer::TMobTable* pMobData = CPythonNonPlayer::Instance().GetSelectedMobDataPointer();
	if (NULL == pMobData)
		return Py_BuildException("Cannot select mob data.");

	return Py_BuildValue("i", pMobData->dwMaxHP);
}

PyObject* nonplayerGetRegenCycle(PyObject* poSelf, PyObject* poArgs)
{
	CPythonNonPlayer::TMobTable* pMobData = CPythonNonPlayer::Instance().GetSelectedMobDataPointer();
	if (NULL == pMobData)
		return Py_BuildException("Cannot select mob data.");

	return Py_BuildValue("i", pMobData->bRegenCycle);
}

PyObject* nonplayerGetRegenPercent(PyObject* poSelf, PyObject* poArgs)
{
	CPythonNonPlayer::TMobTable* pMobData = CPythonNonPlayer::Instance().GetSelectedMobDataPointer();
	if (NULL == pMobData)
		return Py_BuildException("Cannot select mob data.");

	return Py_BuildValue("i", pMobData->bRegenPercent);
}

PyObject* nonplayerGetMinGold(PyObject* poSelf, PyObject* poArgs)
{
	CPythonNonPlayer::TMobTable* pMobData = CPythonNonPlayer::Instance().GetSelectedMobDataPointer();
	if (NULL == pMobData)
		return Py_BuildException("Cannot select mob data.");

	return Py_BuildValue("i", pMobData->dwGoldMin);
}

PyObject* nonplayerGetMaxGold(PyObject* poSelf, PyObject* poArgs)
{
	CPythonNonPlayer::TMobTable* pMobData = CPythonNonPlayer::Instance().GetSelectedMobDataPointer();
	if (NULL == pMobData)
		return Py_BuildException("Cannot select mob data.");

	return Py_BuildValue("i", pMobData->dwGoldMax);
}

PyObject* nonplayerGetAIFlag(PyObject* poSelf, PyObject* poArgs)
{
	CPythonNonPlayer::TMobTable* pMobData = CPythonNonPlayer::Instance().GetSelectedMobDataPointer();
	if (NULL == pMobData)
		return Py_BuildException("Cannot select mob data.");

	return Py_BuildValue("i", pMobData->dwAIFlag);
}

PyObject* nonplayerGetRaceFlag(PyObject* poSelf, PyObject* poArgs)
{
	CPythonNonPlayer::TMobTable* pMobData = CPythonNonPlayer::Instance().GetSelectedMobDataPointer();
	if (NULL == pMobData)
		return Py_BuildException("Cannot select mob data.");

	return Py_BuildValue("i", pMobData->dwRaceFlag);
}

PyObject* nonplayerGetImmuneFlag(PyObject* poSelf, PyObject* poArgs)
{
	CPythonNonPlayer::TMobTable* pMobData = CPythonNonPlayer::Instance().GetSelectedMobDataPointer();
	if (NULL == pMobData)
		return Py_BuildException("Cannot select mob data.");

	return Py_BuildValue("i", pMobData->dwImmuneFlag);
}

PyObject* nonplayerGetEnchant(PyObject* poSelf, PyObject* poArgs)
{
	BYTE bEnchantNum;
	if (!PyTuple_GetByte(poArgs, 0, &bEnchantNum))
		return Py_BuildException();

	CPythonNonPlayer::TMobTable* pMobData = CPythonNonPlayer::Instance().GetSelectedMobDataPointer();
	if (NULL == pMobData)
		return Py_BuildException("Cannot select mob data.");

	if (bEnchantNum >= CPythonNonPlayer::MOB_ENCHANTS_MAX_NUM)
		return 0;

	return Py_BuildValue("i", pMobData->cEnchants[bEnchantNum]);
}

PyObject* nonplayerGetResist(PyObject* poSelf, PyObject* poArgs)
{
	BYTE bResistNum;
	if (!PyTuple_GetByte(poArgs, 0, &bResistNum))
		return Py_BuildException();

	CPythonNonPlayer::TMobTable* pMobData = CPythonNonPlayer::Instance().GetSelectedMobDataPointer();
	if (NULL == pMobData)
		return Py_BuildException("Cannot select mob data.");

	if (bResistNum >= CPythonNonPlayer::MOB_RESISTS_MAX_NUM)
		return 0;

	return Py_BuildValue("i", pMobData->cResists[bResistNum]);
}

#if defined(ENABLE_ELEMENT_ADD)
PyObject* nonplayerGetResistDark(PyObject* poSelf, PyObject* poArgs)
{
	CPythonNonPlayer::TMobTable* pMobData = CPythonNonPlayer::Instance().GetSelectedMobDataPointer();
	if (NULL == pMobData)
		return Py_BuildException("Cannot select mob data.");

	return Py_BuildValue("i", pMobData->cResistDark);
}

PyObject* nonplayerGetResistIce(PyObject* poSelf, PyObject* poArgs)
{
	CPythonNonPlayer::TMobTable* pMobData = CPythonNonPlayer::Instance().GetSelectedMobDataPointer();
	if (NULL == pMobData)
		return Py_BuildException("Cannot select mob data.");

	return Py_BuildValue("i", pMobData->cResistIce);
}

PyObject* nonplayerGetResistEarth(PyObject* poSelf, PyObject* poArgs)
{
	CPythonNonPlayer::TMobTable* pMobData = CPythonNonPlayer::Instance().GetSelectedMobDataPointer();
	if (NULL == pMobData)
		return Py_BuildException("Cannot select mob data.");

	return Py_BuildValue("i", pMobData->cResistEarth);
}

PyObject* nonplayerGetElement(PyObject* poSelf, PyObject* poArgs)
{
	BYTE bResistNum;
	if (!PyTuple_GetByte(poArgs, 0, &bResistNum))
		return Py_BuildException();

	CPythonNonPlayer::TMobTable* pMobData = CPythonNonPlayer::Instance().GetSelectedMobDataPointer();
	if (NULL == pMobData)
		return Py_BuildException("Cannot select mob data.");

	if (bResistNum >= CPythonNonPlayer::MOB_ELEMENT_MAX_NUM)
		return 0;

	return Py_BuildValue("i", pMobData->cElements[bResistNum]);
}
#endif

PyObject* nonplayerGetDamage(PyObject* poSelf, PyObject* poArgs)
{
	CPythonNonPlayer::TMobTable* pMobData = CPythonNonPlayer::Instance().GetSelectedMobDataPointer();
	if (NULL == pMobData)
		return Py_BuildException("Cannot select mob data.");

	return Py_BuildValue("ii", pMobData->dwDamageRange[0], pMobData->dwDamageRange[1]);
}

PyObject* nonplayerGetDamMultiply(PyObject* poSelf, PyObject* poArgs)
{
	CPythonNonPlayer::TMobTable* pMobData = CPythonNonPlayer::Instance().GetSelectedMobDataPointer();
	if (NULL == pMobData)
		return Py_BuildException("Cannot select mob data.");

	return Py_BuildValue("f", pMobData->fDamMultiply);
}

PyObject* nonplayerGetST(PyObject* poSelf, PyObject* poArgs)
{
	CPythonNonPlayer::TMobTable* pMobData = CPythonNonPlayer::Instance().GetSelectedMobDataPointer();
	if (NULL == pMobData)
		return Py_BuildException("Cannot select mob data.");

	return Py_BuildValue("i", pMobData->bStr);
}

PyObject* nonplayerGetDX(PyObject* poSelf, PyObject* poArgs)
{
	CPythonNonPlayer::TMobTable* pMobData = CPythonNonPlayer::Instance().GetSelectedMobDataPointer();
	if (NULL == pMobData)
		return Py_BuildException("Cannot select mob data.");

	return Py_BuildValue("i", pMobData->bDex);
}

PyObject* nonplayerGetHT(PyObject* poSelf, PyObject* poArgs)
{
	CPythonNonPlayer::TMobTable* pMobData = CPythonNonPlayer::Instance().GetSelectedMobDataPointer();
	if (NULL == pMobData)
		return Py_BuildException("Cannot select mob data.");

	return Py_BuildValue("i", pMobData->bCon);
}

PyObject* nonplayerGetIQ(PyObject* poSelf, PyObject* poArgs)
{
	CPythonNonPlayer::TMobTable* pMobData = CPythonNonPlayer::Instance().GetSelectedMobDataPointer();
	if (NULL == pMobData)
		return Py_BuildException("Cannot select mob data.");

	return Py_BuildValue("i", pMobData->bInt);
}

// Monster Drop
#include "../EterBase/Grid.h"
#include "../GameLib/ItemManager.h"
static int __CreateDropPage(CPythonNonPlayer::TMobDropItemVec* pVec, PyObject* poList)
{
	int iPageCount(0);
	CGrid Grid(CPythonNonPlayer::MAX_MOB_ITEM_DROP_GRID_SLOT_X, CPythonNonPlayer::MAX_MOB_ITEM_DROP_GRID_SLOT_Y);

	if (pVec && !pVec->empty())
	{
		for (CPythonNonPlayer::TMobDropItemVec::const_iterator it = pVec->begin(); it != pVec->end(); ++it)
		{
			const CPythonNonPlayer::SMobItemDrop& rItem = *it;
			CItemData* pItemData;
			if (!CItemManager::Instance().GetItemDataPointer(rItem.dwVnum, &pItemData))
				continue;

			const BYTE bItemSize = pItemData->GetSize();
			while (true)
			{
				const int iPos = Grid.FindBlank(1, bItemSize);
				if (iPos >= 0)
				{
					Grid.Put(iPos, 1, bItemSize);

					PyObject* pItemTuple = Py_BuildValue("iiii", iPageCount, iPos, rItem.dwVnum, rItem.wCount);
					if (pItemTuple)
					{
						PyList_Append(poList, pItemTuple);
						Py_DECREF(pItemTuple);
					}

					break;
				}
				else
				{
					Grid.Clear();
					++iPageCount;
				}
			}
		}
	}

	return iPageCount;
}

PyObject* nonplayerHasMonsterItemDrop(PyObject* poSelf, PyObject* poArgs)
{
	int iRaceVnum;
	if (!PyTuple_GetInteger(poArgs, 0, &iRaceVnum))
		return Py_BadArgument();

	CPythonNonPlayer::TMobDropItemVec* pVec = CPythonNonPlayer::Instance().GetItemDropVec(iRaceVnum);
	return Py_BuildValue("b", (pVec && !pVec->empty()));
}

PyObject* nonplayerGetMonsterItemDrop(PyObject* poSelf, PyObject* poArgs)
{
	int iRaceVnum;
	if (!PyTuple_GetInteger(poArgs, 0, &iRaceVnum))
		return Py_BadArgument();

	PyObject* poList = PyList_New(0);
	if (!poList)
		return Py_BuildException();

	CPythonNonPlayer::TMobDropItemVec* pVec = CPythonNonPlayer::Instance().GetItemDropVec(iRaceVnum);
	const int iPageCount = __CreateDropPage(pVec, poList);

	PyObject* pResult = Py_BuildValue("iO", iPageCount, poList);
	Py_DECREF(poList);

	if (!pResult)
		return Py_BuildException();

	return pResult;
}

PyObject* nonplayerGetDropMetinStone(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("b", CPythonNonPlayer::Instance().GetDropMetinStone());
}
#endif

void initNonPlayer()
{
	static PyMethodDef s_methods[] =
	{
		{ "GetEventType", nonplayerGetEventType, METH_VARARGS },
		{ "GetEventTypeByVID", nonplayerGetEventTypeByVID, METH_VARARGS },
		{ "GetVnumByVID", nonplayerGetVnumByVID, METH_VARARGS },
		{ "GetLevelByVID", nonplayerGetLevelByVID, METH_VARARGS },
		{ "GetGradeByVID", nonplayerGetGradeByVID, METH_VARARGS },
		{ "GetMonsterName", nonplayerGetMonsterName, METH_VARARGS },

#if defined(ENABLE_SEND_TARGET_INFO)
		{ "GetRaceByVID", nonplayerGetRaceByVID, METH_VARARGS },
		{ "SelectMob", nonplayerSelectMob, METH_VARARGS },

		{ "IsEnemy", nonplayerIsEnemy, METH_VARARGS },
		{ "IsStone", nonplayerIsStone, METH_VARARGS },
		{ "GetLevel", nonplayerGetLevel, METH_VARARGS },
#if defined(__HIT_RANGE_RENEWAL__)
		{ "GetHitRange", nonplayerGetHitRange, METH_VARARGS },
#endif

		{ "GetAIFlag", nonplayerGetAIFlag, METH_VARARGS },
		{ "GetRaceFlag", nonplayerGetRaceFlag, METH_VARARGS },
		{ "GetImmuneFlag", nonplayerGetImmuneFlag, METH_VARARGS },
		{ "GetEnchant", nonplayerGetEnchant, METH_VARARGS },
		{ "GetResist", nonplayerGetResist, METH_VARARGS },
#if defined(ENABLE_ELEMENT_ADD)
		{ "GetResistDark", nonplayerGetResistDark, METH_VARARGS },
		{ "GetResistIce", nonplayerGetResistIce, METH_VARARGS },
		{ "GetResistEarth", nonplayerGetResistEarth, METH_VARARGS },

		{ "GetElement", nonplayerGetElement, METH_VARARGS },
#endif

		{ "GetDamage", nonplayerGetDamage, METH_VARARGS },
		{ "GetDamMultiply", nonplayerGetDamMultiply, METH_VARARGS },

		{ "GetST", nonplayerGetST, METH_VARARGS },
		{ "GetDX", nonplayerGetDX, METH_VARARGS },
		{ "GetHT", nonplayerGetHT, METH_VARARGS },
		{ "GetIQ", nonplayerGetIQ, METH_VARARGS },

		{ "GetMaxHP", nonplayerGetMaxHP, METH_VARARGS },
		{ "GetRegenCycle", nonplayerGetRegenCycle, METH_VARARGS },
		{ "GetRegenPercent", nonplayerGetRegenPercent, METH_VARARGS },

		{ "GetMinGold", nonplayerGetMinGold, METH_VARARGS },
		{ "GetMaxGold", nonplayerGetMaxGold, METH_VARARGS },

		{ "GetExp", nonplayerGetExp, METH_VARARGS },
#if defined(ENABLE_CONQUEROR_LEVEL)
		{ "GetSungMaExp", nonplayerGetSungMaExp, METH_VARARGS },
#endif

		// Monster Item Drop
		{ "HasMonsterItemDrop", nonplayerHasMonsterItemDrop, METH_VARARGS },
		{ "GetMonsterItemDrop", nonplayerGetMonsterItemDrop, METH_VARARGS },
		{ "GetDropMetinStone", nonplayerGetDropMetinStone, METH_VARARGS },
#endif

		{ "LoadNonPlayerData", nonplayerLoadNonPlayerData, METH_VARARGS },

		{ NULL, NULL, NULL },
	};

	PyObject* poModule = Py_InitModule("nonplayer", s_methods);

	PyModule_AddIntConstant(poModule, "ON_CLICK_EVENT_NONE", CPythonNonPlayer::ON_CLICK_EVENT_NONE);
	PyModule_AddIntConstant(poModule, "ON_CLICK_EVENT_SHOP", CPythonNonPlayer::ON_CLICK_EVENT_SHOP);
	PyModule_AddIntConstant(poModule, "ON_CLICK_EVENT_TALK", CPythonNonPlayer::ON_CLICK_EVENT_TALK);

	PyModule_AddIntConstant(poModule, "PAWN", NRaceData::EMobRank::MOB_RANK_PAWN);
	PyModule_AddIntConstant(poModule, "S_PAWN", NRaceData::EMobRank::MOB_RANK_S_PAWN);
	PyModule_AddIntConstant(poModule, "KNIGHT", NRaceData::EMobRank::MOB_RANK_KNIGHT);
	PyModule_AddIntConstant(poModule, "S_KNIGHT", NRaceData::EMobRank::MOB_RANK_S_KNIGHT);
	PyModule_AddIntConstant(poModule, "BOSS", NRaceData::EMobRank::MOB_RANK_BOSS);
	PyModule_AddIntConstant(poModule, "KING", NRaceData::EMobRank::MOB_RANK_KING);

#ifdef WJ_SHOW_MOB_INFO
	PyModule_AddIntConstant(poModule, "AIFLAG_AGGRESSIVE", CInstanceBase::AIFLAG_AGGRESSIVE);
	PyModule_AddIntConstant(poModule, "AIFLAG_NOMOVE", CInstanceBase::AIFLAG_NOMOVE);
	PyModule_AddIntConstant(poModule, "AIFLAG_COWARD", CInstanceBase::AIFLAG_COWARD);
	PyModule_AddIntConstant(poModule, "AIFLAG_NOATTACKSHINSU", CInstanceBase::AIFLAG_NOATTACKSHINSU);
	PyModule_AddIntConstant(poModule, "AIFLAG_NOATTACKCHUNJO", CInstanceBase::AIFLAG_NOATTACKCHUNJO);
	PyModule_AddIntConstant(poModule, "AIFLAG_NOATTACKJINNO", CInstanceBase::AIFLAG_NOATTACKJINNO);
	PyModule_AddIntConstant(poModule, "AIFLAG_ATTACKMOB", CInstanceBase::AIFLAG_ATTACKMOB);
	PyModule_AddIntConstant(poModule, "AIFLAG_BERSERK", CInstanceBase::AIFLAG_BERSERK);
	PyModule_AddIntConstant(poModule, "AIFLAG_STONESKIN", CInstanceBase::AIFLAG_STONESKIN);
	PyModule_AddIntConstant(poModule, "AIFLAG_GODSPEED", CInstanceBase::AIFLAG_GODSPEED);
	PyModule_AddIntConstant(poModule, "AIFLAG_DEATHBLOW", CInstanceBase::AIFLAG_DEATHBLOW);
	PyModule_AddIntConstant(poModule, "AIFLAG_REVIVE", CInstanceBase::AIFLAG_REVIVE);
	PyModule_AddIntConstant(poModule, "AIFLAG_HEALER", CInstanceBase::AIFLAG_HEALER);
	PyModule_AddIntConstant(poModule, "AIFLAG_COUNT", CInstanceBase::AIFLAG_COUNT);
	PyModule_AddIntConstant(poModule, "AIFLAG_NORECOVERY", CInstanceBase::AIFLAG_NORECOVERY);
	PyModule_AddIntConstant(poModule, "AIFLAG_REFLECT", CInstanceBase::AIFLAG_REFLECT);
	PyModule_AddIntConstant(poModule, "AIFLAG_FALL", CInstanceBase::AIFLAG_FALL);
	PyModule_AddIntConstant(poModule, "AIFLAG_VIT", CInstanceBase::AIFLAG_VIT);
	PyModule_AddIntConstant(poModule, "AIFLAG_RATTSPEED", CInstanceBase::AIFLAG_RATTSPEED);
	PyModule_AddIntConstant(poModule, "AIFLAG_RCASTSPEED", CInstanceBase::AIFLAG_RCASTSPEED);
	PyModule_AddIntConstant(poModule, "AIFLAG_RHP_REGEN", CInstanceBase::AIFLAG_RHP_REGEN);
	PyModule_AddIntConstant(poModule, "AIFLAG_TIMEVIT", CInstanceBase::AIFLAG_TIMEVIT);
#endif

#if defined(ENABLE_ELEMENT_ADD)
	PyModule_AddIntConstant(poModule, "MOB_ENCHANT_ELECT", CPythonNonPlayer::MOB_ELEMENT_ELECT);
	PyModule_AddIntConstant(poModule, "MOB_ENCHANT_FIRE", CPythonNonPlayer::MOB_ELEMENT_FIRE);
	PyModule_AddIntConstant(poModule, "MOB_ENCHANT_ICE", CPythonNonPlayer::MOB_ELEMENT_ICE);
	PyModule_AddIntConstant(poModule, "MOB_ENCHANT_WIND", CPythonNonPlayer::MOB_ELEMENT_WIND);
	PyModule_AddIntConstant(poModule, "MOB_ENCHANT_EARTH", CPythonNonPlayer::MOB_ELEMENT_EARTH);
	PyModule_AddIntConstant(poModule, "MOB_ENCHANT_DARK", CPythonNonPlayer::MOB_ELEMENT_DARK);
#endif

	PyModule_AddIntConstant(poModule, "MOB_ENCHANT_CURSE", CPythonNonPlayer::MOB_ENCHANT_CURSE);
	PyModule_AddIntConstant(poModule, "MOB_ENCHANT_SLOW", CPythonNonPlayer::MOB_ENCHANT_SLOW);
	PyModule_AddIntConstant(poModule, "MOB_ENCHANT_POISON", CPythonNonPlayer::MOB_ENCHANT_POISON);
	PyModule_AddIntConstant(poModule, "MOB_ENCHANT_STUN", CPythonNonPlayer::MOB_ENCHANT_STUN);
	PyModule_AddIntConstant(poModule, "MOB_ENCHANT_CRITICAL", CPythonNonPlayer::MOB_ENCHANT_CRITICAL);
	PyModule_AddIntConstant(poModule, "MOB_ENCHANT_PENETRATE", CPythonNonPlayer::MOB_ENCHANT_PENETRATE);
	PyModule_AddIntConstant(poModule, "MOB_ENCHANTS_MAX_NUM", CPythonNonPlayer::MOB_ENCHANTS_MAX_NUM);

	PyModule_AddIntConstant(poModule, "MOB_RESIST_FIST", CPythonNonPlayer::MOB_RESIST_FIST);
	PyModule_AddIntConstant(poModule, "MOB_RESIST_SWORD", CPythonNonPlayer::MOB_RESIST_SWORD);
	PyModule_AddIntConstant(poModule, "MOB_RESIST_TWOHAND", CPythonNonPlayer::MOB_RESIST_TWOHAND);
	PyModule_AddIntConstant(poModule, "MOB_RESIST_DAGGER", CPythonNonPlayer::MOB_RESIST_DAGGER);
	PyModule_AddIntConstant(poModule, "MOB_RESIST_BELL", CPythonNonPlayer::MOB_RESIST_BELL);
	PyModule_AddIntConstant(poModule, "MOB_RESIST_FAN", CPythonNonPlayer::MOB_RESIST_FAN);
	PyModule_AddIntConstant(poModule, "MOB_RESIST_BOW", CPythonNonPlayer::MOB_RESIST_BOW);
	PyModule_AddIntConstant(poModule, "MOB_RESIST_CLAW", CPythonNonPlayer::MOB_RESIST_CLAW);
	PyModule_AddIntConstant(poModule, "MOB_RESIST_FIRE", CPythonNonPlayer::MOB_RESIST_FIRE);
	PyModule_AddIntConstant(poModule, "MOB_RESIST_ELECT", CPythonNonPlayer::MOB_RESIST_ELECT);
	PyModule_AddIntConstant(poModule, "MOB_RESIST_MAGIC", CPythonNonPlayer::MOB_RESIST_MAGIC);
	PyModule_AddIntConstant(poModule, "MOB_RESIST_WIND", CPythonNonPlayer::MOB_RESIST_WIND);
	PyModule_AddIntConstant(poModule, "MOB_RESIST_POISON", CPythonNonPlayer::MOB_RESIST_POISON);
	PyModule_AddIntConstant(poModule, "MOB_RESIST_BLEEDING", CPythonNonPlayer::MOB_RESIST_BLEEDING);
	PyModule_AddIntConstant(poModule, "MOB_RESISTS_MAX_NUM", CPythonNonPlayer::MOB_RESISTS_MAX_NUM);
#if defined(ENABLE_ELEMENT_ADD)
	PyModule_AddIntConstant(poModule, "MOB_ELEMENT_MAX_NUM", CPythonNonPlayer::MOB_ELEMENT_MAX_NUM);
#endif

	PyModule_AddIntConstant(poModule, "RACE_FLAG_ANIMAL", CPythonNonPlayer::RACE_FLAG_ANIMAL);
	PyModule_AddIntConstant(poModule, "RACE_FLAG_UNDEAD", CPythonNonPlayer::RACE_FLAG_UNDEAD);
	PyModule_AddIntConstant(poModule, "RACE_FLAG_DEVIL", CPythonNonPlayer::RACE_FLAG_DEVIL);
	PyModule_AddIntConstant(poModule, "RACE_FLAG_HUMAN", CPythonNonPlayer::RACE_FLAG_HUMAN);
	PyModule_AddIntConstant(poModule, "RACE_FLAG_ORC", CPythonNonPlayer::RACE_FLAG_ORC);
	PyModule_AddIntConstant(poModule, "RACE_FLAG_MILGYO", CPythonNonPlayer::RACE_FLAG_MILGYO);
	PyModule_AddIntConstant(poModule, "RACE_FLAG_INSECT", CPythonNonPlayer::RACE_FLAG_INSECT);
	PyModule_AddIntConstant(poModule, "RACE_FLAG_DESERT", CPythonNonPlayer::RACE_FLAG_DESERT);
	PyModule_AddIntConstant(poModule, "RACE_FLAG_TREE", CPythonNonPlayer::RACE_FLAG_TREE);
	PyModule_AddIntConstant(poModule, "RACE_FLAG_DECO", CPythonNonPlayer::RACE_FLAG_DECO);
	PyModule_AddIntConstant(poModule, "RACE_FLAG_HIDE", CPythonNonPlayer::RACE_FLAG_HIDE);
	PyModule_AddIntConstant(poModule, "RACE_FLAG_ATT_CZ", CPythonNonPlayer::RACE_FLAG_ATT_CZ);
	PyModule_AddIntConstant(poModule, "RACE_FLAG_AWEAKEN", CPythonNonPlayer::RACE_FLAG_AWEAKEN);
	PyModule_AddIntConstant(poModule, "RACE_FLAG_SUNGMAHEE", CPythonNonPlayer::RACE_FLAG_SUNGMAHEE);
	PyModule_AddIntConstant(poModule, "RACE_FLAG_OUTPOST", CPythonNonPlayer::RACE_FLAG_OUTPOST);

#if defined(ENABLE_SEND_TARGET_INFO)
	PyModule_AddIntConstant(poModule, "MAX_MOB_ITEM_DROP_GRID_SLOT_X", CPythonNonPlayer::MAX_MOB_ITEM_DROP_GRID_SLOT_X);
	PyModule_AddIntConstant(poModule, "MAX_MOB_ITEM_DROP_GRID_SLOT_Y", CPythonNonPlayer::MAX_MOB_ITEM_DROP_GRID_SLOT_Y);
	PyModule_AddIntConstant(poModule, "MAX_MOB_ITEM_DROP_GRID_SIZE", CPythonNonPlayer::MAX_MOB_ITEM_DROP_GRID_SIZE);
#endif
}
