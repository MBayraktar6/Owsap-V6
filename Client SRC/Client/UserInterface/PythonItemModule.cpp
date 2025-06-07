#include "StdAfx.h"
#include "PythonItem.h"

#include "../GameLib/ItemManager.h"

#include "InstanceBase.h"
#include "AbstractApplication.h"
#include "PythonPlayer.h"

extern int TWOHANDED_WEWAPON_ATT_SPEED_DECREASE_VALUE;

PyObject* itemSetUseSoundFileName(PyObject* poSelf, PyObject* poArgs)
{
	int iUseSound;
	if (!PyTuple_GetInteger(poArgs, 0, &iUseSound))
		return Py_BadArgument();

	char* szFileName;
	if (!PyTuple_GetString(poArgs, 1, &szFileName))
		return Py_BadArgument();

	CPythonItem& rkItem = CPythonItem::Instance();
	rkItem.SetUseSoundFileName(iUseSound, szFileName);
	return Py_BuildNone();
}

PyObject* itemSetDropSoundFileName(PyObject* poSelf, PyObject* poArgs)
{
	int iDropSound;
	if (!PyTuple_GetInteger(poArgs, 0, &iDropSound))
		return Py_BadArgument();

	char* szFileName;
	if (!PyTuple_GetString(poArgs, 1, &szFileName))
		return Py_BadArgument();

	CPythonItem& rkItem = CPythonItem::Instance();
	rkItem.SetDropSoundFileName(iDropSound, szFileName);
	return Py_BuildNone();
}

PyObject* itemSelectItem(PyObject* poSelf, PyObject* poArgs)
{
	int iIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iIndex))
		return Py_BadArgument();

	if (!CItemManager::Instance().SelectItemData(iIndex))
	{
		//TraceError("Cannot find item by %d", iIndex);
		CItemManager::Instance().SelectItemData(60001);
	}

	return Py_BuildNone();
}

PyObject* itemGetVnum(PyObject* poSelf, PyObject* poArgs)
{
	CItemData* pItemData = CItemManager::Instance().GetSelectedItemDataPointer();
	if (!pItemData)
		return Py_BuildException("no selected item data");

	return Py_BuildValue("i", pItemData->GetIndex());
}

PyObject* itemGetItemName(PyObject* poSelf, PyObject* poArgs)
{
	CItemData* pItemData = CItemManager::Instance().GetSelectedItemDataPointer();
	if (!pItemData)
		return Py_BuildException("no selected item data");

	return Py_BuildValue("s", pItemData->GetName());
}

PyObject* itemIsWeddingItem(PyObject* poSelf, PyObject* poArgs)
{
	DWORD dwItemIndex;
	switch (PyTuple_Size(poArgs))
	{
		case 0:
		{
			CItemData* pItemData = CItemManager::Instance().GetSelectedItemDataPointer();
			if (!pItemData)
				return Py_BuildException("No item selected!");
			dwItemIndex = pItemData->GetIndex();
			break;
		}
		case 1:
		{
			if (!PyTuple_GetUnsignedLong(poArgs, 0, &dwItemIndex))
				return Py_BuildException();
			break;
		}
		default:
			return Py_BuildException();
	}

	//if ((dwItemIndex == 50201 || dwItemIndex == 50202) || (dwItemIndex >= 11901 && dwItemIndex <= 11914))
	//	return Py_BuildValue("i", TRUE);

	switch (dwItemIndex)
	{
		case CItemData::WEDDING_TUXEDO1:
		case CItemData::WEDDING_TUXEDO2:
		case CItemData::WEDDING_TUXEDO3:
		case CItemData::WEDDING_TUXEDO4:
		case CItemData::WEDDING_BRIDE_DRESS1:
		case CItemData::WEDDING_BRIDE_DRESS2:
		case CItemData::WEDDING_BRIDE_DRESS3:
		case CItemData::WEDDING_BRIDE_DRESS4:
		case CItemData::WEDDING_BOUQUET1:
		case CItemData::WEDDING_BOUQUET2:
			return Py_BuildValue("i", TRUE);
			break;
	}

	return Py_BuildValue("i", FALSE);
}

#if defined(ENABLE_MOVE_COSTUME_ATTR)
PyObject* itemGetItemNameByVnum(PyObject* poSelf, PyObject* poArgs)
{
	DWORD dwVirtualNumber;
	if (!PyTuple_GetUnsignedLong(poArgs, 0, &dwVirtualNumber))
		return Py_BadArgument();

	CItemData* pItemData;
	if (!CItemManager::Instance().GetItemDataPointer(dwVirtualNumber, &pItemData))
		return Py_BuildException();

	return Py_BuildValue("s", pItemData->GetName());
}
#endif

#if defined(ENABLE_SET_ITEM)
PyObject* itemGetItemNameBySetValue(PyObject* poSelf, PyObject* poArgs)
{
	int iSetValue;
	if (!PyTuple_GetInteger(poArgs, 0, &iSetValue))
		return Py_BadArgument();

	CItemData* pItemData = CItemManager::Instance().GetSelectedItemDataPointer();
	if (!pItemData)
		return Py_BuildException("no selected item data");

	return Py_BuildValue("s", CPythonItem::Instance().GetPreName(iSetValue, pItemData->GetName()));
}
#endif

PyObject* itemGetItemDescription(PyObject* poSelf, PyObject* poArgs)
{
	CItemData* pItemData = CItemManager::Instance().GetSelectedItemDataPointer();
	if (!pItemData)
		return Py_BuildException("no selected item data");

	return Py_BuildValue("s", pItemData->GetDescription());
}

PyObject* itemGetItemSummary(PyObject* poSelf, PyObject* poArgs)
{
	CItemData* pItemData = CItemManager::Instance().GetSelectedItemDataPointer();
	if (!pItemData)
		return Py_BuildException("no selected item data");

	return Py_BuildValue("s", pItemData->GetSummary());
}

PyObject* itemGetIconImage(PyObject* poSelf, PyObject* poArgs)
{
	CItemData* pItemData = CItemManager::Instance().GetSelectedItemDataPointer();
	if (!pItemData)
		return Py_BuildException("no selected item data");

	/*
	if (CItemData::ITEM_TYPE_SKILLBOOK == pItemData->GetType())
	{
		char szItemName[64 + 1];
		_snprintf(szItemName, "d:/ymir work/ui/items/etc/book_%02d.sub", );
		CGraphicImage* pImage = (CGraphicImage*)CResourceManager::Instance().GetResourcePointer(szItemName);
	}
	*/

	return Py_BuildValue("i", pItemData->GetIconImage());
}

PyObject* itemGetIconImageFileName(PyObject* poSelf, PyObject* poArgs)
{
	CItemData* pItemData = CItemManager::Instance().GetSelectedItemDataPointer();
	if (!pItemData)
		return Py_BuildException("no selected item data");

	CGraphicSubImage* pImage = pItemData->GetIconImage();
	if (!pImage)
		return Py_BuildValue("s", "Noname");

	return Py_BuildValue("s", pImage->GetFileName());
}

PyObject* itemGetItemSize(PyObject* poSelf, PyObject* poArgs)
{
	CItemData* pItemData = CItemManager::Instance().GetSelectedItemDataPointer();
	if (!pItemData)
		return Py_BuildException("no selected item data");

	return Py_BuildValue("(ii)", 1, pItemData->GetSize());
}

PyObject* itemGetItemType(PyObject* poSelf, PyObject* poArgs)
{
	CItemData* pItemData = CItemManager::Instance().GetSelectedItemDataPointer();
	if (!pItemData)
		return Py_BuildException("no selected item data");

	return Py_BuildValue("i", pItemData->GetType());
}

PyObject* itemGetItemSubType(PyObject* poSelf, PyObject* poArgs)
{
	CItemData* pItemData = CItemManager::Instance().GetSelectedItemDataPointer();
	if (!pItemData)
		return Py_BuildException("no selected item data");

	return Py_BuildValue("i", pItemData->GetSubType());
}

PyObject* itemGetIBuyItemPrice(PyObject* poSelf, PyObject* poArgs)
{
	CItemData* pItemData = CItemManager::Instance().GetSelectedItemDataPointer();

	if (!pItemData)
		return Py_BuildException("no selected item data");

	return Py_BuildValue("i", pItemData->GetIBuyItemPrice());
}

PyObject* itemGetISellItemPrice(PyObject* poSelf, PyObject* poArgs)
{
	CItemData* pItemData = CItemManager::Instance().GetSelectedItemDataPointer();

	if (!pItemData)
		return Py_BuildException("no selected item data");

	return Py_BuildValue("i", pItemData->GetISellItemPrice());
}

PyObject* itemIsAntiFlag(PyObject* poSelf, PyObject* poArgs)
{
	int iFlag;
	if (!PyTuple_GetInteger(poArgs, 0, &iFlag))
		return Py_BadArgument();

	CItemData* pItemData = CItemManager::Instance().GetSelectedItemDataPointer();
	if (!pItemData)
		return Py_BuildException("no selected item data");

	return Py_BuildValue("i", pItemData->IsAntiFlag(iFlag));
}

PyObject* itemIsWearableByJob(PyObject* poSelf, PyObject* poArgs)
{
	BYTE bJob;
	if (!PyTuple_GetInteger(poArgs, 0, &bJob))
		return Py_BadArgument();

	CItemData* pItemData = CItemManager::Instance().GetSelectedItemDataPointer();
	if (!pItemData)
		return Py_BuildException("no selected item data");

	return Py_BuildValue("b", pItemData->IsWearableByJob(bJob));
}

PyObject* itemIsFlag(PyObject* poSelf, PyObject* poArgs)
{
	int iFlag;
	if (!PyTuple_GetInteger(poArgs, 0, &iFlag))
		return Py_BadArgument();

	CItemData* pItemData = CItemManager::Instance().GetSelectedItemDataPointer();
	if (!pItemData)
		return Py_BuildException("no selected item data");

	return Py_BuildValue("i", pItemData->IsFlag(iFlag));
}

PyObject* itemIsWearableFlag(PyObject* poSelf, PyObject* poArgs)
{
	int iFlag;
	if (!PyTuple_GetInteger(poArgs, 0, &iFlag))
		return Py_BadArgument();

	CItemData* pItemData = CItemManager::Instance().GetSelectedItemDataPointer();
	if (!pItemData)
		return Py_BuildException("no selected item data");

	return Py_BuildValue("i", pItemData->IsWearableFlag(iFlag));
}

PyObject* itemIs1GoldItem(PyObject* poSelf, PyObject* poArgs)
{
	CItemData* pItemData = CItemManager::Instance().GetSelectedItemDataPointer();
	if (!pItemData)
		return Py_BuildException("no selected item data");

	return Py_BuildValue("i", pItemData->IsFlag(CItemData::ITEM_FLAG_COUNT_PER_1GOLD));
}

#if defined(ENABLE_REFINE_ELEMENT_SYSTEM)
PyObject* itemGetRefineLevel(PyObject* poSelf, PyObject* poArgs)
{
	CItemData* pItemData = CItemManager::Instance().GetSelectedItemDataPointer();
	if (!pItemData)
		return Py_BuildException("no selected item data");

	return Py_BuildValue("i", pItemData->GetRefine());
}
#endif

PyObject* itemGetLimit(PyObject* poSelf, PyObject* poArgs)
{
	int iValueIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iValueIndex))
		return Py_BadArgument();

	CItemData* pItemData = CItemManager::Instance().GetSelectedItemDataPointer();
	if (!pItemData)
		return Py_BuildException("Not yet select item data");

	CItemData::TItemLimit ItemLimit;
	if (!pItemData->GetLimit(iValueIndex, &ItemLimit))
		return Py_BuildException();

	return Py_BuildValue("ii", ItemLimit.bType, ItemLimit.lValue);
}

PyObject* itemGetAffect(PyObject* poSelf, PyObject* poArgs)
{
	int iValueIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iValueIndex))
		return Py_BadArgument();

	CItemData* pItemData = CItemManager::Instance().GetSelectedItemDataPointer();
	if (!pItemData)
		return Py_BuildException("Not yet select item data");

	CItemData::TItemApply ItemApply;
	if (!pItemData->GetApply(iValueIndex, &ItemApply))
		return Py_BuildException();

	if ((CItemData::APPLY_ATT_SPEED == ItemApply.wType) && (CItemData::ITEM_TYPE_WEAPON == pItemData->GetType()) && (CItemData::WEAPON_TWO_HANDED == pItemData->GetSubType()))
	{
		ItemApply.lValue -= TWOHANDED_WEWAPON_ATT_SPEED_DECREASE_VALUE;
	}

#if defined(ENABLE_MOUNT_COSTUME_SYSTEM)
	if (CItemData::APPLY_MOUNT == ItemApply.wType)
		return Py_BuildValue("ii", 0, 0);
#endif

	return Py_BuildValue("ii", ItemApply.wType, ItemApply.lValue);
}

PyObject* itemGetValue(PyObject* poSelf, PyObject* poArgs)
{
	int iValueIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iValueIndex))
		return Py_BadArgument();

	CItemData* pItemData = CItemManager::Instance().GetSelectedItemDataPointer();
	if (!pItemData)
		return Py_BuildException("Not yet select item data");

	return Py_BuildValue("i", pItemData->GetValue(iValueIndex));
}

PyObject* itemGetSocket(PyObject* poSelf, PyObject* poArgs)
{
	int iValueIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iValueIndex))
		return Py_BadArgument();

	CItemData* pItemData = CItemManager::Instance().GetSelectedItemDataPointer();
	if (!pItemData)
		return Py_BuildException("Not yet select item data");

	return Py_BuildValue("i", pItemData->GetSocket(iValueIndex));
}

PyObject* itemGetIconInstance(PyObject* poSelf, PyObject* poArgs)
{
	CItemData* pItemData = CItemManager::Instance().GetSelectedItemDataPointer();
	if (!pItemData)
		return Py_BuildException("Not yet select item data");

	CGraphicSubImage* pImage = pItemData->GetIconImage();
	if (!pImage)
		return Py_BuildException("Cannot get icon image by %d", pItemData->GetIndex());

	CGraphicImageInstance* pImageInstance = CGraphicImageInstance::New();
	pImageInstance->SetImagePointer(pImage);

	return Py_BuildValue("i", pImageInstance);
}

PyObject* itemDeleteIconInstance(PyObject* poSelf, PyObject* poArgs)
{
	int iHandle;
	if (!PyTuple_GetInteger(poArgs, 0, &iHandle))
		return Py_BadArgument();

	CGraphicImageInstance::Delete((CGraphicImageInstance*)iHandle);

	return Py_BuildNone();
}

PyObject* itemIsEquipmentVID(PyObject* poSelf, PyObject* poArgs)
{
	int iItemVID;
	if (!PyTuple_GetInteger(poArgs, 0, &iItemVID))
		return Py_BadArgument();

	CItemManager::Instance().SelectItemData(iItemVID);
	CItemData* pItemData = CItemManager::Instance().GetSelectedItemDataPointer();
	if (!pItemData)
		return Py_BuildException("Not yet select item data");

	return Py_BuildValue("i", pItemData->IsEquipment());
}

// 2005.05.20.myevan.통합 USE_TYPE 체크
PyObject* itemGetUseType(PyObject* poSelf, PyObject* poArgs)
{
	int iItemVID;
	if (!PyTuple_GetInteger(poArgs, 0, &iItemVID))
		return Py_BadArgument();

	CItemManager::Instance().SelectItemData(iItemVID);
	CItemData* pItemData = CItemManager::Instance().GetSelectedItemDataPointer();
	if (!pItemData)
		return Py_BuildException("Can't find select item data");

	return Py_BuildValue("s", pItemData->GetUseTypeString());
}

PyObject* itemIsRefineScroll(PyObject* poSelf, PyObject* poArgs)
{
	int iItemIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iItemIndex))
		return Py_BadArgument();

	CItemManager::Instance().SelectItemData(iItemIndex);
	CItemData* pItemData = CItemManager::Instance().GetSelectedItemDataPointer();
	if (!pItemData)
		return Py_BuildException("Can't find select item data");

	if (pItemData->GetType() != CItemData::ITEM_TYPE_USE)
		return Py_BuildValue("i", FALSE);

	switch (pItemData->GetSubType())
	{
		case CItemData::USE_TUNING:
			return Py_BuildValue("i", TRUE);
	}

	return Py_BuildValue("i", FALSE);
}

PyObject* itemIsDetachScroll(PyObject* poSelf, PyObject* poArgs)
{
	int iItemIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iItemIndex))
		return Py_BadArgument();

	CItemManager::Instance().SelectItemData(iItemIndex);
	CItemData* pItemData = CItemManager::Instance().GetSelectedItemDataPointer();
	if (!pItemData)
		return Py_BuildException("Can't find select item data");

	int iType = pItemData->GetType();
	int iSubType = pItemData->GetSubType();

	if (iType == CItemData::ITEM_TYPE_USE)
	{
		if (iSubType == CItemData::USE_DETACHMENT)
			return Py_BuildValue("i", TRUE);

#if defined(ENABLE_ACCE_COSTUME_SYSTEM)
		if (CPythonItem::Instance().IsAcceScroll(iItemIndex))
			return Py_BuildValue("i", TRUE);
#endif

		if (iItemIndex == ITEM_VNUM_SCROLL_OF_CORRECTION)
			return Py_BuildValue("i", TRUE);
	}

	return Py_BuildValue("i", FALSE);
}

PyObject* itemCanAddToQuickSlotItem(PyObject* poSelf, PyObject* poArgs)
{
	int iItemIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iItemIndex))
		return Py_BadArgument();

	CItemData* pItemData;
	if (!CItemManager::Instance().GetItemDataPointer(iItemIndex, &pItemData))
		return Py_BuildValue("b", false);

	if (CItemData::ITEM_TYPE_USE == pItemData->GetType() || CItemData::ITEM_TYPE_QUEST == pItemData->GetType())
		return Py_BuildValue("b", true);

	return Py_BuildValue("b", false);
}

PyObject* itemIsKey(PyObject* poSelf, PyObject* poArgs)
{
	int iItemIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iItemIndex))
		return Py_BadArgument();

	CItemManager::Instance().SelectItemData(iItemIndex);
	CItemData* pItemData = CItemManager::Instance().GetSelectedItemDataPointer();
	if (!pItemData)
		return Py_BuildException("Can't find select item data");

	if (CItemData::ITEM_TYPE_TREASURE_KEY == pItemData->GetType())
	{
		return Py_BuildValue("i", TRUE);
	}

	return Py_BuildValue("i", FALSE);
}

PyObject* itemIsMetin(PyObject* poSelf, PyObject* poArgs)
{
	int iItemIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iItemIndex))
		return Py_BadArgument();

	CItemManager::Instance().SelectItemData(iItemIndex);
	CItemData* pItemData = CItemManager::Instance().GetSelectedItemDataPointer();
	if (!pItemData)
		return Py_BuildException("Can't find select item data");

	if (CItemData::ITEM_TYPE_METIN == pItemData->GetType())
	{
		return Py_BuildValue("i", TRUE);
	}

	return Py_BuildValue("i", FALSE);
}

PyObject* itemRender(PyObject* poSelf, PyObject* poArgs)
{
	CPythonItem::Instance().Render();
	return Py_BuildNone();
}

PyObject* itemUpdate(PyObject* poSelf, PyObject* poArgs)
{
	IAbstractApplication& rkApp = IAbstractApplication::GetSingleton();

	POINT ptMouse;
	rkApp.GetMousePosition(&ptMouse);

	CPythonItem::Instance().Update(ptMouse);
	return Py_BuildNone();
}

PyObject* itemCreateItem(PyObject* poSelf, PyObject* poArgs)
{
	int iVirtualID;
	if (!PyTuple_GetInteger(poArgs, 0, &iVirtualID))
		return Py_BadArgument();
	int iVirtualNumber;
	if (!PyTuple_GetInteger(poArgs, 1, &iVirtualNumber))
		return Py_BadArgument();

	float x;
	if (!PyTuple_GetFloat(poArgs, 2, &x))
		return Py_BadArgument();
	float y;
	if (!PyTuple_GetFloat(poArgs, 3, &y))
		return Py_BadArgument();
	float z;
	if (!PyTuple_GetFloat(poArgs, 4, &z))
		return Py_BadArgument();

	bool bDrop = true;
	PyTuple_GetBoolean(poArgs, 5, &bDrop);

#if defined(ENABLE_SET_ITEM)
	CPythonItem::Instance().CreateItem(iVirtualID, iVirtualNumber, x, y, z, 0, bDrop);
#else
	CPythonItem::Instance().CreateItem(iVirtualID, iVirtualNumber, x, y, z, bDrop);
#endif

	return Py_BuildNone();
}

PyObject* itemDeleteItem(PyObject* poSelf, PyObject* poArgs)
{
	int iVirtualID;
	if (!PyTuple_GetInteger(poArgs, 0, &iVirtualID))
		return Py_BadArgument();

	CPythonItem::Instance().DeleteItem(iVirtualID);
	return Py_BuildNone();
}

PyObject* itemPick(PyObject* poSelf, PyObject* poArgs)
{
	DWORD dwItemID;
	if (CPythonItem::Instance().GetPickedItemID(&dwItemID))
		return Py_BuildValue("i", dwItemID);
	else
		return Py_BuildValue("i", -1);
}

PyObject* itemLoadItemTable(PyObject* poSelf, PyObject* poArgs)
{
	char* szFileName;
	if (!PyTuple_GetString(poArgs, 0, &szFileName))
		return Py_BadArgument();

	CItemManager::Instance().LoadItemTable(szFileName);
	return Py_BuildNone();
}

#if defined(ENABLE_DETAILS_UI)
PyObject* itemGetPointApply(PyObject* poSelf, PyObject* poArgs)
{
	POINT_TYPE wPointType = 0;
	if (!PyTuple_GetInteger(poArgs, 0, &wPointType))
		return Py_BadArgument();

	return Py_BuildValue("i", PointTypeToApplyType(wPointType));
}

PyObject* itemGetApplyPoint(PyObject* poSelf, PyObject* poArgs)
{
	POINT_TYPE wApplyType = 0;
	if (!PyTuple_GetInteger(poArgs, 0, &wApplyType))
		return Py_BadArgument();

	return Py_BuildValue("i", ApplyTypeToPointType(wApplyType));
}
#endif

#if defined(ENABLE_SOULBIND_SYSTEM)
PyObject* itemIsSealScroll(PyObject* poSelf, PyObject* poArgs)
{
	int iItemIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iItemIndex))
		return Py_BadArgument();

	CItemManager::Instance().SelectItemData(iItemIndex);
	CItemData* pItemData = CItemManager::Instance().GetSelectedItemDataPointer();
	if (!pItemData)
		return Py_BuildException("Can't find select item data");

	BYTE bType = pItemData->GetType();
	BYTE bSubType = pItemData->GetSubType();

	if (bType == CItemData::ITEM_TYPE_USE)
	{
		switch (bSubType)
		{
			case CItemData::USE_BIND:
			case CItemData::USE_UNBIND:
				return Py_BuildValue("b", true);
		}
	}

	if (iItemIndex == CItemData::SEAL_ITEM_BINDING_VNUM)
		return Py_BuildValue("b", true);
	else if (iItemIndex == CItemData::SEAL_ITEM_UNBINDING_VNUM)
		return Py_BuildValue("b", true);
#if defined(ENABLE_UN_SEAL_SCROLL_PLUS)
	else if (iItemIndex == CItemData::SEAL_ITEM_UNBINDING_VNUM_MALL)
		return Py_BuildValue("b", true);
#endif

	return Py_BuildValue("b", false);
}

PyObject* itemGetUnlimitedSealDate(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CItemData::U_SEAL_DATE_DEFAULT_TIMESTAMP);
}
#endif

#if defined(ENABLE_SOUL_SYSTEM)
PyObject* itemIsSoulItem(PyObject* poSelf, PyObject* poArgs)
{
	int iItemIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iItemIndex))
		return Py_BadArgument();

	CItemData* pItemData;
	if (!CItemManager::Instance().GetItemDataPointer(iItemIndex, &pItemData))
		return Py_BuildValue("b", false);

	if (pItemData->GetType() == CItemData::ITEM_TYPE_SOUL)
		return Py_BuildValue("b", true);

	return Py_BuildValue("b", false);
}
#endif

PyObject* itemIsPetPay(PyObject* poSelf, PyObject* poArgs)
{
	int iItemIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iItemIndex))
		return Py_BadArgument();

	CItemData* pItemData;
	if (!CItemManager::Instance().GetItemDataPointer(iItemIndex, &pItemData))
		return Py_BuildValue("b", false);

	if (pItemData->GetType() == CItemData::ITEM_TYPE_PET
		&& pItemData->GetSubType() == CItemData::PET_PAY)
		return Py_BuildValue("b", true);

	return Py_BuildValue("b", false);
}

PyObject* itemGetDSSetWeight(PyObject* poSelf, PyObject* poArgs)
{
	BYTE bDSType;
	if (!PyTuple_GetInteger(poArgs, 0, &bDSType))
		return Py_BadArgument();

	BYTE bDSGrade = 0;
	if (!PyTuple_GetInteger(poArgs, 1, &bDSGrade))
		return Py_BadArgument();

	return Py_BuildValue("f", CItemManager::Instance().GetDSSetWeight(bDSType, bDSGrade));
}

PyObject* itemGetDSBasicApplyCount(PyObject* poSelf, PyObject* poArgs)
{
	BYTE bDSType;
	if (!PyTuple_GetInteger(poArgs, 0, &bDSType))
		return Py_BadArgument();

	BYTE bDSGrade = 0;
	if (!PyTuple_GetInteger(poArgs, 1, &bDSGrade))
		return Py_BadArgument();

	return Py_BuildValue("i", CItemManager::Instance().GetDSBasicApplyCount(bDSType, bDSGrade));
}

PyObject* itemGetDSBasicApplyValue(PyObject* poSelf, PyObject* poArgs)
{
	BYTE bDSType;
	if (!PyTuple_GetInteger(poArgs, 0, &bDSType))
		return Py_BadArgument();

	POINT_TYPE wApplyType;
	if (!PyTuple_GetInteger(poArgs, 1, &wApplyType))
		return Py_BadArgument();

	return Py_BuildValue("i", CItemManager::Instance().GetDSBasicApplyValue(bDSType, wApplyType));
}

PyObject* itemGetDSAdditionalApplyValue(PyObject* poSelf, PyObject* poArgs)
{
	BYTE bDSType;
	if (!PyTuple_GetInteger(poArgs, 0, &bDSType))
		return Py_BadArgument();

	POINT_TYPE wApplyType;
	if (!PyTuple_GetInteger(poArgs, 1, &wApplyType))
		return Py_BadArgument();

	return Py_BuildValue("i", CItemManager::Instance().GetDSAdditionalApplyValue(bDSType, wApplyType));
}

PyObject* itemIsItemUsedForDragonSoul(PyObject* poSelf, PyObject* poArgs)
{
	int iSrcItemIndex = 0;
	TItemPos SrcPos(INVENTORY, 0), DestPos(DRAGON_SOUL_INVENTORY, 0);
	switch (PyTuple_Size(poArgs))
	{
		case 1:
			if (!PyTuple_GetInteger(poArgs, 0, &iSrcItemIndex))
				return Py_BuildException();
			break;
		case 4:
			if (!PyTuple_GetInteger(poArgs, 0, &SrcPos.window_type))
				return Py_BuildException();
			if (!PyTuple_GetInteger(poArgs, 1, &SrcPos.cell))
				return Py_BuildException();
			if (!PyTuple_GetInteger(poArgs, 2, &DestPos.window_type))
				return Py_BuildException();
			if (!PyTuple_GetInteger(poArgs, 3, &DestPos.cell))
				return Py_BuildException();
			break;
		default:
			return Py_BuildException();
	}

	if (PyTuple_Size(poArgs) > 1)
		iSrcItemIndex = CPythonPlayer::Instance().GetItemIndex(SrcPos);

	CItemManager::Instance().SelectItemData(iSrcItemIndex);
	CItemData* pSrcItemData = CItemManager::Instance().GetSelectedItemDataPointer();
	if (!pSrcItemData)
		return Py_BuildException("Can't find item data");

	if (pSrcItemData->GetType() == CItemData::ITEM_TYPE_USE)
	{
		switch (pSrcItemData->GetSubType())
		{
			case CItemData::USE_TIME_CHARGE_PER:
			case CItemData::USE_TIME_CHARGE_FIX:
			{
				if (DestPos.window_type == DRAGON_SOUL_INVENTORY || DestPos.window_type == EQUIPMENT)
					return Py_BuildValue("b", true);

				return Py_BuildValue("b", false);
			}
		}
	}

#if defined(ENABLE_SOULBIND_SYSTEM) && defined(ENABLE_DRAGON_SOUL_SEAL)
	TItemData* pPlayerItem = const_cast<TItemData*>(CPythonPlayer::Instance().GetItemData(DestPos));
	long lSealDate = (pPlayerItem ? pPlayerItem->lSealDate : CItemData::U_SEAL_DATE_DEFAULT_TIMESTAMP);

	if (pSrcItemData->GetIndex() == CItemData::SEAL_ITEM_BINDING_VNUM && lSealDate == CItemData::E_SEAL_DATE_DEFAULT_TIMESTAMP)
	{
		if (pPlayerItem)
		{
			const BYTE bGradeIdx = ((pPlayerItem->dwVnum / 1000) % 10);
			if (bGradeIdx >= DRAGON_SOUL_GRADE_ANCIENT)
				return Py_BuildValue("b", true);
		}
	}

	if ((pSrcItemData->GetIndex() == CItemData::SEAL_ITEM_UNBINDING_VNUM
#if defined(ENABLE_UN_SEAL_SCROLL_PLUS)
		|| pSrcItemData->GetIndex() == CItemData::SEAL_ITEM_UNBINDING_VNUM_MALL
#endif
		) && lSealDate != CItemData::E_SEAL_DATE_DEFAULT_TIMESTAMP)
		return Py_BuildValue("b", true);
#endif

	if (pSrcItemData->GetType() == CItemData::ITEM_TYPE_EXTRACT)
	{
#if defined(ENABLE_SOULBIND_SYSTEM) && defined(ENABLE_DRAGON_SOUL_SEAL)
		if (lSealDate != CItemData::E_SEAL_DATE_DEFAULT_TIMESTAMP)
			return Py_BuildValue("b", false);
#endif

		if (pSrcItemData->GetSubType() == CItemData::EXTRACT_DRAGON_SOUL && DestPos.window_type == EQUIPMENT)
			return Py_BuildValue("b", true);

		if (pSrcItemData->GetSubType() == CItemData::EXTRACT_DRAGON_HEART && DestPos.window_type == DRAGON_SOUL_INVENTORY)
			return Py_BuildValue("b", true);
	}

	return Py_BuildValue("b", false);
}

#if defined(ENABLE_ACCE_COSTUME_SYSTEM)
PyObject* itemIsAcceScroll(PyObject* poSelf, PyObject* poArgs)
{
	int iItemVID;
	if (!PyTuple_GetInteger(poArgs, 0, &iItemVID))
		return Py_BadArgument();

	CItemManager::Instance().SelectItemData(iItemVID);
	CItemData* pItemData = CItemManager::Instance().GetSelectedItemDataPointer();
	if (!pItemData)
		return Py_BuildException("Can't find select item data");

	return Py_BuildValue("b", CPythonItem::Instance().IsAcceScroll(pItemData->GetIndex()));
}
#endif

#if defined(ENABLE_CHANGE_LOOK_SYSTEM)
PyObject* itemIsPossibleChangeLookLeft(PyObject* poSelf, PyObject* poArgs)
{
	int iItemVID;
	if (!PyTuple_GetInteger(poArgs, 0, &iItemVID))
		return Py_BadArgument();

	CItemManager::Instance().SelectItemData(iItemVID);
	CItemData* pItemData = CItemManager::Instance().GetSelectedItemDataPointer();
	if (!pItemData)
		return Py_BuildException("Can't find select item data");

	return Py_BuildValue("b", CPythonItem::Instance().CanAddChangeLookItem(pItemData, nullptr));
}

PyObject* itemIsPossibleChangeLookRight(PyObject* poSelf, PyObject* poArgs)
{
	int iLeftItemVID;
	if (!PyTuple_GetInteger(poArgs, 0, &iLeftItemVID))
		return Py_BadArgument();

	int iRightItemVID;
	if (!PyTuple_GetInteger(poArgs, 1, &iRightItemVID))
		return Py_BadArgument();

	CItemManager::Instance().SelectItemData(iLeftItemVID);
	CItemData* pLeftItemData = CItemManager::Instance().GetSelectedItemDataPointer();

	CItemManager::Instance().SelectItemData(iRightItemVID);
	CItemData* pRightItemData = CItemManager::Instance().GetSelectedItemDataPointer();

	return Py_BuildValue("b", CPythonItem::Instance().CanAddChangeLookItem(pLeftItemData, pRightItemData));
}

PyObject* itemIsChangeLookFreePassYangItem(PyObject* poSelf, PyObject* poArgs)
{
	DWORD dwVirtualNumber;
	if (!PyTuple_GetUnsignedLong(poArgs, 0, &dwVirtualNumber))
		return Py_BadArgument();

	return Py_BuildValue("b", CPythonItem::Instance().CanAddChangeLookFreeItem(dwVirtualNumber));
}

PyObject* itemIsChangeLookClearScroll(PyObject* poSelf, PyObject* poArgs)
{
	int iItemVID;
	if (!PyTuple_GetInteger(poArgs, 0, &iItemVID))
		return Py_BadArgument();

	CItemManager::Instance().SelectItemData(iItemVID);
	CItemData* pItemData = CItemManager::Instance().GetSelectedItemDataPointer();
	if (!pItemData)
		return Py_BuildException("Can't find select item data");

	return Py_BuildValue("b", CPythonItem::Instance().IsChangeLookClearScrollItem(pItemData->GetIndex()));
}
#endif

#if defined(ENABLE_SET_ITEM)
PyObject* itemIsSetClearScroll(PyObject* poSelf, PyObject* poArgs)
{
	int iItemVID;
	if (!PyTuple_GetInteger(poArgs, 0, &iItemVID))
		return Py_BadArgument();

	CItemManager::Instance().SelectItemData(iItemVID);
	CItemData* pItemData = CItemManager::Instance().GetSelectedItemDataPointer();
	if (!pItemData)
		return Py_BuildException("Can't find select item data");

	return Py_BuildValue("b", pItemData->GetIndex() == ITEM_VNUM_SET_CLEAR_SCROLL);
}
#endif

#if defined(ENABLE_ATTR_6TH_7TH)
PyObject* itemGetAttr67MaterialVnum(PyObject* poSelf, PyObject* poArgs)
{
	CItemData* pItemData = CItemManager::Instance().GetSelectedItemDataPointer();
	if (!pItemData)
		return Py_BuildException("Selected item data pointer is null. (not yet selected?)");

	return Py_BuildValue("i", pItemData->Get67AttrMaterial());
}
#endif

#if defined(ENABLE_SET_ITEM)
PyObject* itemSetPreName(PyObject* poSelf, PyObject* poArgs)
{
	int iType;
	if (!PyTuple_GetInteger(poArgs, 0, &iType))
		return Py_BadArgument();

	char* szPreName;
	if (!PyTuple_GetString(poArgs, 1, &szPreName))
		return Py_BadArgument();

	CPythonItem& rkItem = CPythonItem::Instance();
	rkItem.SetPreName(iType, szPreName);
	return Py_BuildNone();
}
#endif

PyObject* itemCheckAffect(PyObject* poSelf, PyObject* poArgs)
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

PyObject* itemGetAffectDuration(PyObject* poSelf, PyObject* poArgs)
{
	DWORD dwType;
	if (!PyTuple_GetUnsignedLong(poArgs, 0, &dwType))
		return Py_BadArgument();

	return Py_BuildValue("l", CPythonPlayer::Instance().GetAffectDuration(dwType));
}

PyObject* itemGetRefinedVnum(PyObject* poSelf, PyObject* poArgs)
{
	CItemData* pItemData = CItemManager::Instance().GetSelectedItemDataPointer();
	if (!pItemData)
		return Py_BuildException("no selected item data");

	return Py_BuildValue("i", pItemData->GetRefineVNum());
}

void initItem()
{
	static PyMethodDef s_methods[] =
	{
		{ "SetUseSoundFileName", itemSetUseSoundFileName, METH_VARARGS },
		{ "SetDropSoundFileName", itemSetDropSoundFileName, METH_VARARGS },
		{ "SelectItem", itemSelectItem, METH_VARARGS },

		{ "GetVnum", itemGetVnum, METH_VARARGS },
		{ "IsWeddingItem", itemIsWeddingItem, METH_VARARGS },
		{ "GetItemName", itemGetItemName, METH_VARARGS },
#if defined(ENABLE_MOVE_COSTUME_ATTR)
		{ "GetItemNameByVnum", itemGetItemNameByVnum, METH_VARARGS },
#endif
#if defined(ENABLE_SET_ITEM)
		{ "GetItemNameBySetValue", itemGetItemNameBySetValue, METH_VARARGS },
#endif
		{ "GetItemDescription", itemGetItemDescription, METH_VARARGS },
		{ "GetItemSummary", itemGetItemSummary, METH_VARARGS },
		{ "GetIconImage", itemGetIconImage, METH_VARARGS },
		{ "GetIconImageFileName", itemGetIconImageFileName, METH_VARARGS },
		{ "GetItemSize", itemGetItemSize, METH_VARARGS },
		{ "GetItemType", itemGetItemType, METH_VARARGS },
		{ "GetItemSubType", itemGetItemSubType, METH_VARARGS },
		{ "GetIBuyItemPrice", itemGetIBuyItemPrice, METH_VARARGS },
		{ "GetISellItemPrice", itemGetISellItemPrice, METH_VARARGS },
		{ "IsAntiFlag", itemIsAntiFlag, METH_VARARGS },
		{ "IsWearableByJob", itemIsWearableByJob, METH_VARARGS },
		{ "IsFlag", itemIsFlag, METH_VARARGS },
		{ "IsWearableFlag", itemIsWearableFlag, METH_VARARGS },
		{ "Is1GoldItem", itemIs1GoldItem, METH_VARARGS },
#if defined(ENABLE_REFINE_ELEMENT_SYSTEM)
		{ "GetRefineLevel", itemGetRefineLevel, METH_VARARGS },
#endif
		{ "GetLimit", itemGetLimit, METH_VARARGS },
		{ "GetAffect", itemGetAffect, METH_VARARGS },
		{ "GetValue", itemGetValue, METH_VARARGS },
		{ "GetSocket", itemGetSocket, METH_VARARGS },
		{ "GetIconInstance", itemGetIconInstance, METH_VARARGS },
		{ "GetUseType", itemGetUseType, METH_VARARGS },
		{ "DeleteIconInstance", itemDeleteIconInstance, METH_VARARGS },
		{ "IsEquipmentVID", itemIsEquipmentVID, METH_VARARGS },
		{ "IsRefineScroll", itemIsRefineScroll, METH_VARARGS },
		{ "IsDetachScroll", itemIsDetachScroll, METH_VARARGS },
		{ "IsKey", itemIsKey, METH_VARARGS },
		{ "IsMetin", itemIsMetin, METH_VARARGS },
		{ "CanAddToQuickSlotItem", itemCanAddToQuickSlotItem, METH_VARARGS },

		{ "Update", itemUpdate, METH_VARARGS },
		{ "Render", itemRender, METH_VARARGS },
		{ "CreateItem", itemCreateItem, METH_VARARGS },
		{ "DeleteItem", itemDeleteItem, METH_VARARGS },
		{ "Pick", itemPick, METH_VARARGS },

		{ "LoadItemTable", itemLoadItemTable, METH_VARARGS },

#if defined(ENABLE_DETAILS_UI)
		{ "GetPointApply", itemGetPointApply, METH_VARARGS },
		{ "GetApplyPoint", itemGetApplyPoint, METH_VARARGS },
#endif

#if defined(ENABLE_SOULBIND_SYSTEM)
		{ "IsSealScroll", itemIsSealScroll, METH_VARARGS },
		//{ "IsUnSealScrollPlus", itemIsUnSealScrollPlus, METH_VARARGS },
		{ "GetUnlimitedSealDate", itemGetUnlimitedSealDate, METH_VARARGS },
#endif

#if defined(ENABLE_SOUL_SYSTEM)
		{ "IsSoulItem", itemIsSoulItem, METH_VARARGS },
#endif

#if defined(ENABLE_DS_SET)
		{ "GetDSSetWeight", itemGetDSSetWeight, METH_VARARGS },
		{ "GetDSBasicApplyCount", itemGetDSBasicApplyCount, METH_VARARGS },
		{ "GetDSBasicApplyValue", itemGetDSBasicApplyValue, METH_VARARGS },
		{ "GetDSAdditionalApplyValue", itemGetDSAdditionalApplyValue, METH_VARARGS },
#endif
		{ "IsItemUsedForDragonSoul", itemIsItemUsedForDragonSoul, METH_VARARGS },

		{ "IsPetPay", itemIsPetPay, METH_VARARGS },

#if defined(ENABLE_ACCE_COSTUME_SYSTEM)
		{ "IsAcceScroll", itemIsAcceScroll, METH_VARARGS },
#endif

#if defined(ENABLE_CHANGE_LOOK_SYSTEM)
		{ "IsPossibleChangeLookLeft", itemIsPossibleChangeLookLeft, METH_VARARGS },
		{ "IsPossibleChangeLookRight", itemIsPossibleChangeLookRight, METH_VARARGS },
		{ "IsChangeLookFreePassYangItem", itemIsChangeLookFreePassYangItem, METH_VARARGS },
		{ "IsChangeLookClearScroll", itemIsChangeLookClearScroll, METH_VARARGS },
#endif

#if defined(ENABLE_SET_ITEM)
		{ "IsSetClearScroll", itemIsSetClearScroll, METH_VARARGS },
#endif

		{ "CheckAffect", itemCheckAffect, METH_VARARGS },
		{ "GetAffectDuration", itemGetAffectDuration, METH_VARARGS },
		{ "GetRefinedVnum", itemGetRefinedVnum, METH_VARARGS },

#if defined(ENABLE_ATTR_6TH_7TH)
		{ "GetAttr67MaterialVnum", itemGetAttr67MaterialVnum, METH_VARARGS },
#endif

#if defined(ENABLE_SET_ITEM)
		{ "SetPreName", itemSetPreName, METH_VARARGS },
#endif

		{ NULL, NULL, NULL },
	};

	PyObject* poModule = Py_InitModule("item", s_methods);

	PyModule_AddIntConstant(poModule, "USESOUND_ACCESSORY", CPythonItem::USESOUND_ACCESSORY);
	PyModule_AddIntConstant(poModule, "USESOUND_ARMOR", CPythonItem::USESOUND_ARMOR);
	PyModule_AddIntConstant(poModule, "USESOUND_BOW", CPythonItem::USESOUND_BOW);
	PyModule_AddIntConstant(poModule, "USESOUND_DEFAULT", CPythonItem::USESOUND_DEFAULT);
	PyModule_AddIntConstant(poModule, "USESOUND_WEAPON", CPythonItem::USESOUND_WEAPON);
	PyModule_AddIntConstant(poModule, "USESOUND_POTION", CPythonItem::USESOUND_POTION);
	PyModule_AddIntConstant(poModule, "USESOUND_PORTAL", CPythonItem::USESOUND_PORTAL);

	PyModule_AddIntConstant(poModule, "DROPSOUND_ACCESSORY", CPythonItem::DROPSOUND_ACCESSORY);
	PyModule_AddIntConstant(poModule, "DROPSOUND_ARMOR", CPythonItem::DROPSOUND_ARMOR);
	PyModule_AddIntConstant(poModule, "DROPSOUND_BOW", CPythonItem::DROPSOUND_BOW);
	PyModule_AddIntConstant(poModule, "DROPSOUND_DEFAULT", CPythonItem::DROPSOUND_DEFAULT);
	PyModule_AddIntConstant(poModule, "DROPSOUND_WEAPON", CPythonItem::DROPSOUND_WEAPON);

	PyModule_AddIntConstant(poModule, "EQUIPMENT_BODY", c_Equipment_Slot_Body);
	PyModule_AddIntConstant(poModule, "EQUIPMENT_HEAD", c_Equipment_Slot_Head);
	PyModule_AddIntConstant(poModule, "EQUIPMENT_SHOES", c_Equipment_Slot_Foots);
	PyModule_AddIntConstant(poModule, "EQUIPMENT_WRIST", c_Equipment_Slot_Wrist);
	PyModule_AddIntConstant(poModule, "EQUIPMENT_WEAPON", c_Equipment_Slot_Weapon);
	PyModule_AddIntConstant(poModule, "EQUIPMENT_NECK", c_Equipment_Slot_Neck);
	PyModule_AddIntConstant(poModule, "EQUIPMENT_EAR", c_Equipment_Slot_Ear);
	PyModule_AddIntConstant(poModule, "EQUIPMENT_ARROW", c_Equipment_Slot_Arrow);
	PyModule_AddIntConstant(poModule, "EQUIPMENT_SHIELD", c_Equipment_Slot_Shield);
	PyModule_AddIntConstant(poModule, "EQUIPMENT_BELT", c_Equipment_Slot_Belt);
#if defined(ENABLE_PENDANT)
	PyModule_AddIntConstant(poModule, "EQUIPMENT_PENDANT", c_Equipment_Slot_Pendant);
#endif
#if defined(ENABLE_GLOVE_SYSTEM)
	PyModule_AddIntConstant(poModule, "EQUIPMENT_GLOVE", c_Equipment_Slot_Glove);
#endif
	PyModule_AddIntConstant(poModule, "EQUIPMENT_UNIQUE1", c_Equipment_Slot_Unique1);
	PyModule_AddIntConstant(poModule, "EQUIPMENT_UNIQUE2", c_Equipment_Slot_Unique2);

	PyModule_AddIntConstant(poModule, "ITEM_TYPE_NONE", CItemData::ITEM_TYPE_NONE);
	PyModule_AddIntConstant(poModule, "ITEM_TYPE_WEAPON", CItemData::ITEM_TYPE_WEAPON);
	PyModule_AddIntConstant(poModule, "ITEM_TYPE_ARMOR", CItemData::ITEM_TYPE_ARMOR);
	PyModule_AddIntConstant(poModule, "ITEM_TYPE_USE", CItemData::ITEM_TYPE_USE);
	PyModule_AddIntConstant(poModule, "ITEM_TYPE_AUTOUSE", CItemData::ITEM_TYPE_AUTOUSE);
	PyModule_AddIntConstant(poModule, "ITEM_TYPE_MATERIAL", CItemData::ITEM_TYPE_MATERIAL);
	PyModule_AddIntConstant(poModule, "ITEM_TYPE_SPECIAL", CItemData::ITEM_TYPE_SPECIAL);
	PyModule_AddIntConstant(poModule, "ITEM_TYPE_TOOL", CItemData::ITEM_TYPE_TOOL);
	PyModule_AddIntConstant(poModule, "ITEM_TYPE_LOTTERY", CItemData::ITEM_TYPE_LOTTERY);
	PyModule_AddIntConstant(poModule, "ITEM_TYPE_ELK", CItemData::ITEM_TYPE_ELK);
	PyModule_AddIntConstant(poModule, "ITEM_TYPE_METIN", CItemData::ITEM_TYPE_METIN);
	PyModule_AddIntConstant(poModule, "ITEM_TYPE_CONTAINER", CItemData::ITEM_TYPE_CONTAINER);
	PyModule_AddIntConstant(poModule, "ITEM_TYPE_FISH", CItemData::ITEM_TYPE_FISH);
	PyModule_AddIntConstant(poModule, "ITEM_TYPE_ROD", CItemData::ITEM_TYPE_ROD);
	PyModule_AddIntConstant(poModule, "ITEM_TYPE_RESOURCE", CItemData::ITEM_TYPE_RESOURCE);
	PyModule_AddIntConstant(poModule, "ITEM_TYPE_CAMPFIRE", CItemData::ITEM_TYPE_CAMPFIRE);
	PyModule_AddIntConstant(poModule, "ITEM_TYPE_UNIQUE", CItemData::ITEM_TYPE_UNIQUE);
	PyModule_AddIntConstant(poModule, "ITEM_TYPE_SKILLBOOK", CItemData::ITEM_TYPE_SKILLBOOK);
	PyModule_AddIntConstant(poModule, "ITEM_TYPE_QUEST", CItemData::ITEM_TYPE_QUEST);
	PyModule_AddIntConstant(poModule, "ITEM_TYPE_POLYMORPH", CItemData::ITEM_TYPE_POLYMORPH);
	PyModule_AddIntConstant(poModule, "ITEM_TYPE_TREASURE_BOX", CItemData::ITEM_TYPE_TREASURE_BOX);
	PyModule_AddIntConstant(poModule, "ITEM_TYPE_TREASURE_KEY", CItemData::ITEM_TYPE_TREASURE_KEY);
	PyModule_AddIntConstant(poModule, "ITEM_TYPE_SKILLFORGET", CItemData::ITEM_TYPE_SKILLFORGET);
	PyModule_AddIntConstant(poModule, "ITEM_TYPE_GIFTBOX", CItemData::ITEM_TYPE_GIFTBOX);
	PyModule_AddIntConstant(poModule, "ITEM_TYPE_PICK", CItemData::ITEM_TYPE_PICK);
	PyModule_AddIntConstant(poModule, "ITEM_TYPE_HAIR", CItemData::ITEM_TYPE_HAIR);
	PyModule_AddIntConstant(poModule, "ITEM_TYPE_TOTEM", CItemData::ITEM_TYPE_TOTEM);
	PyModule_AddIntConstant(poModule, "ITEM_TYPE_BLEND", CItemData::ITEM_TYPE_BLEND);
#if defined(ENABLE_COSTUME_SYSTEM)
	PyModule_AddIntConstant(poModule, "ITEM_TYPE_COSTUME", CItemData::ITEM_TYPE_COSTUME);
#endif
	PyModule_AddIntConstant(poModule, "ITEM_TYPE_DS", CItemData::ITEM_TYPE_DS);
	PyModule_AddIntConstant(poModule, "ITEM_TYPE_SPECIAL_DS", CItemData::ITEM_TYPE_SPECIAL_DS);
	PyModule_AddIntConstant(poModule, "ITEM_TYPE_EXTRACT", CItemData::ITEM_TYPE_EXTRACT);
	PyModule_AddIntConstant(poModule, "ITEM_TYPE_SECONDARY_COIN", CItemData::ITEM_TYPE_SECONDARY_COIN);
	PyModule_AddIntConstant(poModule, "ITEM_TYPE_RING", CItemData::ITEM_TYPE_RING);
	PyModule_AddIntConstant(poModule, "ITEM_TYPE_BELT", CItemData::ITEM_TYPE_BELT);
	PyModule_AddIntConstant(poModule, "ITEM_TYPE_PET", CItemData::ITEM_TYPE_PET);
#if defined(ENABLE_MOVE_COSTUME_ATTR)
	PyModule_AddIntConstant(poModule, "ITEM_TYPE_MEDIUM", CItemData::ITEM_TYPE_MEDIUM);
#endif
#if defined(ENABLE_GACHA)
	PyModule_AddIntConstant(poModule, "ITEM_TYPE_GACHA", CItemData::ITEM_TYPE_GACHA);
#endif
	PyModule_AddIntConstant(poModule, "ITEM_TYPE_PASSIVE", CItemData::ITEM_TYPE_PASSIVE);
	PyModule_AddIntConstant(poModule, "ITEM_TYPE_MERCENARY", CItemData::ITEM_TYPE_MERCENARY);
	PyModule_AddIntConstant(poModule, "ITEM_TYPE_ALCHEMY", CItemData::ITEM_TYPE_ALCHEMY);
#if defined(ENABLE_SOUL_SYSTEM)
	PyModule_AddIntConstant(poModule, "ITEM_TYPE_SOUL", CItemData::ITEM_TYPE_SOUL);
#endif

	PyModule_AddIntConstant(poModule, "WEAPON_SWORD", CItemData::WEAPON_SWORD);
	PyModule_AddIntConstant(poModule, "WEAPON_DAGGER", CItemData::WEAPON_DAGGER);
	PyModule_AddIntConstant(poModule, "WEAPON_BOW", CItemData::WEAPON_BOW);
	PyModule_AddIntConstant(poModule, "WEAPON_TWO_HANDED", CItemData::WEAPON_TWO_HANDED);
	PyModule_AddIntConstant(poModule, "WEAPON_BELL", CItemData::WEAPON_BELL);
	PyModule_AddIntConstant(poModule, "WEAPON_FAN", CItemData::WEAPON_FAN);
	PyModule_AddIntConstant(poModule, "WEAPON_ARROW", CItemData::WEAPON_ARROW);
	PyModule_AddIntConstant(poModule, "WEAPON_MOUNT_SPEAR", CItemData::WEAPON_MOUNT_SPEAR);
	PyModule_AddIntConstant(poModule, "WEAPON_CLAW", CItemData::WEAPON_CLAW);
#if defined(ENABLE_QUIVER_SYSTEM)
	PyModule_AddIntConstant(poModule, "WEAPON_QUIVER", CItemData::WEAPON_QUIVER);
#endif
	PyModule_AddIntConstant(poModule, "WEAPON_NUM_TYPES", CItemData::WEAPON_NUM_TYPES);

	PyModule_AddIntConstant(poModule, "ARMOR_BODY", CItemData::ARMOR_BODY);
	PyModule_AddIntConstant(poModule, "ARMOR_HEAD", CItemData::ARMOR_HEAD);
	PyModule_AddIntConstant(poModule, "ARMOR_SHIELD", CItemData::ARMOR_SHIELD);
	PyModule_AddIntConstant(poModule, "ARMOR_WRIST", CItemData::ARMOR_WRIST);
	PyModule_AddIntConstant(poModule, "ARMOR_FOOTS", CItemData::ARMOR_FOOTS);
	PyModule_AddIntConstant(poModule, "ARMOR_NECK", CItemData::ARMOR_NECK);
	PyModule_AddIntConstant(poModule, "ARMOR_EAR", CItemData::ARMOR_EAR);
#if defined(ENABLE_PENDANT)
	PyModule_AddIntConstant(poModule, "ARMOR_PENDANT", CItemData::ARMOR_PENDANT);
#endif
#if defined(ENABLE_GLOVE_SYSTEM)
	PyModule_AddIntConstant(poModule, "ARMOR_GLOVE", CItemData::ARMOR_GLOVE);
#endif

	PyModule_AddIntConstant(poModule, "USE_POTION", CItemData::USE_POTION);
	PyModule_AddIntConstant(poModule, "USE_TALISMAN", CItemData::USE_TALISMAN);
	PyModule_AddIntConstant(poModule, "USE_TUNING", CItemData::USE_TUNING);
	PyModule_AddIntConstant(poModule, "USE_MOVE", CItemData::USE_MOVE);
	PyModule_AddIntConstant(poModule, "USE_TREASURE_BOX", CItemData::USE_TREASURE_BOX);
	PyModule_AddIntConstant(poModule, "USE_MONEYBAG", CItemData::USE_MONEYBAG);
	PyModule_AddIntConstant(poModule, "USE_BAIT", CItemData::USE_BAIT);
	PyModule_AddIntConstant(poModule, "USE_ABILITY_UP", CItemData::USE_ABILITY_UP);
	PyModule_AddIntConstant(poModule, "USE_AFFECT", CItemData::USE_AFFECT);
	PyModule_AddIntConstant(poModule, "USE_CREATE_STONE", CItemData::USE_CREATE_STONE);
	PyModule_AddIntConstant(poModule, "USE_SPECIAL", CItemData::USE_SPECIAL);
	PyModule_AddIntConstant(poModule, "USE_POTION_NODELAY", CItemData::USE_POTION_NODELAY);
	PyModule_AddIntConstant(poModule, "USE_CLEAR", CItemData::USE_CLEAR);
	PyModule_AddIntConstant(poModule, "USE_INVISIBILITY", CItemData::USE_INVISIBILITY);
	PyModule_AddIntConstant(poModule, "USE_DETACHMENT", CItemData::USE_DETACHMENT);
#if defined(ENABLE_PRIVATESHOP_SEARCH_SYSTEM)
	PyModule_AddIntConstant(poModule, "USE_CLEAN_SOCKET", CItemData::USE_CLEAN_SOCKET);
#endif
	PyModule_AddIntConstant(poModule, "USE_ADD_ACCESSORY_SOCKET", CItemData::USE_ADD_ACCESSORY_SOCKET);
	PyModule_AddIntConstant(poModule, "USE_PUT_INTO_ACCESSORY_SOCKET", CItemData::USE_PUT_INTO_ACCESSORY_SOCKET);
	PyModule_AddIntConstant(poModule, "USE_TIME_CHARGE_PER", CItemData::USE_TIME_CHARGE_PER);
	PyModule_AddIntConstant(poModule, "USE_TIME_CHARGE_FIX", CItemData::USE_TIME_CHARGE_FIX);
#if defined(ENABLE_REFINE_ELEMENT_SYSTEM)
	PyModule_AddIntConstant(poModule, "USE_ELEMENT_UPGRADE", CItemData::USE_ELEMENT_UPGRADE);
	PyModule_AddIntConstant(poModule, "USE_ELEMENT_DOWNGRADE", CItemData::USE_ELEMENT_DOWNGRADE);
	PyModule_AddIntConstant(poModule, "USE_ELEMENT_CHANGE", CItemData::USE_ELEMENT_CHANGE);
#endif
	PyModule_AddIntConstant(poModule, "USE_CALL", CItemData::USE_CALL);
	PyModule_AddIntConstant(poModule, "USE_POTION_TOWER", CItemData::USE_POTION_TOWER);
	PyModule_AddIntConstant(poModule, "USE_POTION_NODELAY_TOWER", CItemData::USE_POTION_NODELAY_TOWER);
	PyModule_AddIntConstant(poModule, "USE_REMOVE_AFFECT", CItemData::USE_REMOVE_AFFECT);
	PyModule_AddIntConstant(poModule, "USE_EMOTION_TOWER", CItemData::USE_EMOTION_TOWER);
	PyModule_AddIntConstant(poModule, "USE_SECRET_DUNGEON_SCROLL", CItemData::USE_SECRET_DUNGEON_SCROLL);

	PyModule_AddIntConstant(poModule, "MATERIAL_LEATHER", CItemData::MATERIAL_LEATHER);
	PyModule_AddIntConstant(poModule, "MATERIAL_BLOOD", CItemData::MATERIAL_BLOOD);
	PyModule_AddIntConstant(poModule, "MATERIAL_ROOT", CItemData::MATERIAL_ROOT);
	PyModule_AddIntConstant(poModule, "MATERIAL_NEEDLE", CItemData::MATERIAL_NEEDLE);
	PyModule_AddIntConstant(poModule, "MATERIAL_JEWEL", CItemData::MATERIAL_JEWEL);
	PyModule_AddIntConstant(poModule, "MATERIAL_DS_REFINE_NORMAL", CItemData::MATERIAL_DS_REFINE_NORMAL);
	PyModule_AddIntConstant(poModule, "MATERIAL_DS_REFINE_BLESSED", CItemData::MATERIAL_DS_REFINE_BLESSED);
	PyModule_AddIntConstant(poModule, "MATERIAL_DS_REFINE_HOLLY", CItemData::MATERIAL_DS_REFINE_HOLLY);
#if defined(ENABLE_DS_CHANGE_ATTR)
	PyModule_AddIntConstant(poModule, "MATERIAL_DS_CHANGE_ATTR", CItemData::MATERIAL_DS_CHANGE_ATTR);
#endif
	PyModule_AddIntConstant(poModule, "MATERIAL_PASSIVE_WEAPON", CItemData::MATERIAL_PASSIVE_WEAPON);
	PyModule_AddIntConstant(poModule, "MATERIAL_PASSIVE_ARMOR", CItemData::MATERIAL_PASSIVE_ARMOR);
	PyModule_AddIntConstant(poModule, "MATERIAL_PASSIVE_ACCE", CItemData::MATERIAL_PASSIVE_ACCE);
	PyModule_AddIntConstant(poModule, "MATERIAL_PASSIVE_ELEMENT", CItemData::MATERIAL_PASSIVE_ELEMENT);

	PyModule_AddIntConstant(poModule, "METIN_NORMAL", CItemData::METIN_NORMAL);
	PyModule_AddIntConstant(poModule, "METIN_SUNGMA", CItemData::METIN_SUNGMA);

	PyModule_AddIntConstant(poModule, "RESOURCE_FISHBONE", CItemData::RESOURCE_FISHBONE);
	PyModule_AddIntConstant(poModule, "RESOURCE_WATERSTONEPIECE", CItemData::RESOURCE_WATERSTONEPIECE);
	PyModule_AddIntConstant(poModule, "RESOURCE_WATERSTONE", CItemData::RESOURCE_WATERSTONE);
	PyModule_AddIntConstant(poModule, "RESOURCE_BLOOD_PEARL", CItemData::RESOURCE_BLOOD_PEARL);
	PyModule_AddIntConstant(poModule, "RESOURCE_BLUE_PEARL", CItemData::RESOURCE_BLUE_PEARL);
	PyModule_AddIntConstant(poModule, "RESOURCE_WHITE_PEARL", CItemData::RESOURCE_WHITE_PEARL);
	PyModule_AddIntConstant(poModule, "RESOURCE_BUCKET", CItemData::RESOURCE_BUCKET);
	PyModule_AddIntConstant(poModule, "RESOURCE_CRYSTAL", CItemData::RESOURCE_CRYSTAL);
	PyModule_AddIntConstant(poModule, "RESOURCE_GEM", CItemData::RESOURCE_GEM);
	PyModule_AddIntConstant(poModule, "RESOURCE_STONE", CItemData::RESOURCE_STONE);
	PyModule_AddIntConstant(poModule, "RESOURCE_METIN", CItemData::RESOURCE_METIN);
	PyModule_AddIntConstant(poModule, "RESOURCE_ORE", CItemData::RESOURCE_ORE);
#if defined(ENABLE_AURA_COSTUME_SYSTEM)
	PyModule_AddIntConstant(poModule, "RESOURCE_AURA", CItemData::RESOURCE_AURA);
#endif

	PyModule_AddIntConstant(poModule, "QUEST_NONE", CItemData::QUEST_NONE);
	PyModule_AddIntConstant(poModule, "QUEST_PET_PAY", CItemData::QUEST_PET_PAY);
	PyModule_AddIntConstant(poModule, "QUEST_WARP", CItemData::QUEST_WARP);
	PyModule_AddIntConstant(poModule, "QUEST_GEM_BAG", CItemData::QUEST_GEM_BAG);

#if defined(ENABLE_COSTUME_SYSTEM)
	PyModule_AddIntConstant(poModule, "COSTUME_TYPE_BODY", CItemData::COSTUME_BODY);
	PyModule_AddIntConstant(poModule, "COSTUME_TYPE_HAIR", CItemData::COSTUME_HAIR);
#if defined(ENABLE_MOUNT_COSTUME_SYSTEM)
	PyModule_AddIntConstant(poModule, "COSTUME_TYPE_MOUNT", CItemData::COSTUME_MOUNT);
#endif
#if defined(ENABLE_ACCE_COSTUME_SYSTEM)
	PyModule_AddIntConstant(poModule, "COSTUME_TYPE_ACCE", CItemData::COSTUME_ACCE);
#endif
#if defined(ENABLE_WEAPON_COSTUME_SYSTEM)
	PyModule_AddIntConstant(poModule, "COSTUME_TYPE_WEAPON", CItemData::COSTUME_WEAPON);
#endif
#if defined(ENABLE_AURA_COSTUME_SYSTEM)
	PyModule_AddIntConstant(poModule, "COSTUME_TYPE_AURA", CItemData::COSTUME_AURA);
#endif

	// 인벤토리 및 장비창에서의 슬롯 번호
	PyModule_AddIntConstant(poModule, "COSTUME_SLOT_START", c_Costume_Slot_Start);
	PyModule_AddIntConstant(poModule, "COSTUME_SLOT_COUNT", c_Costume_Slot_Count);
	PyModule_AddIntConstant(poModule, "COSTUME_SLOT_BODY", c_Costume_Slot_Body);
	PyModule_AddIntConstant(poModule, "COSTUME_SLOT_HAIR", c_Costume_Slot_Hair);
#if defined(ENABLE_MOUNT_COSTUME_SYSTEM)
	PyModule_AddIntConstant(poModule, "COSTUME_SLOT_MOUNT", c_Costume_Slot_Mount);
#endif
#if defined(ENABLE_ACCE_COSTUME_SYSTEM)
	PyModule_AddIntConstant(poModule, "COSTUME_SLOT_ACCE", c_Costume_Slot_Acce);
#endif
#if defined(ENABLE_AURA_COSTUME_SYSTEM)
	PyModule_AddIntConstant(poModule, "COSTUME_SLOT_AURA", c_Costume_Slot_Aura);
#endif
#if defined(ENABLE_WEAPON_COSTUME_SYSTEM)
	PyModule_AddIntConstant(poModule, "COSTUME_SLOT_WEAPON", c_Costume_Slot_Weapon);
#endif
	PyModule_AddIntConstant(poModule, "COSTUME_SLOT_END", c_Costume_Slot_End);
#endif

#if defined(ENABLE_NEW_EQUIPMENT_SYSTEM)
	PyModule_AddIntConstant(poModule, "BELT_INVENTORY_SLOT_START", c_Belt_Inventory_Slot_Start);
	PyModule_AddIntConstant(poModule, "BELT_INVENTORY_SLOT_COUNT", c_Belt_Inventory_Slot_Count);
	PyModule_AddIntConstant(poModule, "BELT_INVENTORY_SLOT_END", c_Belt_Inventory_Slot_End);
#endif

#if defined(ENABLE_DRAGON_SOUL_SYSTEM)
	PyModule_AddIntConstant(poModule, "DS_SLOT1", CItemData::DS_SLOT1);
	PyModule_AddIntConstant(poModule, "DS_SLOT2", CItemData::DS_SLOT2);
	PyModule_AddIntConstant(poModule, "DS_SLOT3", CItemData::DS_SLOT3);
	PyModule_AddIntConstant(poModule, "DS_SLOT4", CItemData::DS_SLOT4);
	PyModule_AddIntConstant(poModule, "DS_SLOT5", CItemData::DS_SLOT5);
	PyModule_AddIntConstant(poModule, "DS_SLOT6", CItemData::DS_SLOT6);
	PyModule_AddIntConstant(poModule, "DS_SLOT7", CItemData::DS_SLOT7);
	PyModule_AddIntConstant(poModule, "DS_SLOT_NUM_TYPES", CItemData::DS_SLOT_NUM_TYPES);
#endif

#if defined(ENABLE_DRAGON_SOUL_SYSTEM) && defined(ENABLE_PRIVATESHOP_SEARCH_SYSTEM)
	PyModule_AddIntConstant(poModule, "DRAGON_STONE_DRAGON_DIAMOND", CItemData::DS_SLOT1);
	PyModule_AddIntConstant(poModule, "DRAGON_STONE_DRAGON_RUBY", CItemData::DS_SLOT2);
	PyModule_AddIntConstant(poModule, "DRAGON_STONE_DRAGON_JADE", CItemData::DS_SLOT3);
	PyModule_AddIntConstant(poModule, "DRAGON_STONE_DRAGON_SAPPHIRE", CItemData::DS_SLOT4);
	PyModule_AddIntConstant(poModule, "DRAGON_STONE_DRAGON_GARNET", CItemData::DS_SLOT5);
	PyModule_AddIntConstant(poModule, "DRAGON_STONE_DRAGON_ONYX", CItemData::DS_SLOT6);
	PyModule_AddIntConstant(poModule, "DRAGON_STONE_DRAGON_AMETHYST", CItemData::DS_SLOT7);
#endif

	PyModule_AddIntConstant(poModule, "EXTRACT_DRAGON_SOUL", CItemData::EXTRACT_DRAGON_SOUL);
	PyModule_AddIntConstant(poModule, "EXTRACT_DRAGON_HEART", CItemData::EXTRACT_DRAGON_HEART);

//#if defined(ENABLE_GROWTH_PET_SYSTEM)
	PyModule_AddIntConstant(poModule, "PET_EGG", CItemData::PET_EGG);
	PyModule_AddIntConstant(poModule, "PET_UPBRINGING", CItemData::PET_UPBRINGING);
	PyModule_AddIntConstant(poModule, "PET_BAG", CItemData::PET_BAG);
	PyModule_AddIntConstant(poModule, "PET_FEEDSTUFF", CItemData::PET_FEEDSTUFF);
	PyModule_AddIntConstant(poModule, "PET_SKILL", CItemData::PET_SKILL);
	PyModule_AddIntConstant(poModule, "PET_SKILL_DEL_BOOK", CItemData::PET_SKILL_DEL_BOOK);
	PyModule_AddIntConstant(poModule, "PET_NAME_CHANGE", CItemData::PET_NAME_CHANGE);
	PyModule_AddIntConstant(poModule, "PET_EXPFOOD", CItemData::PET_EXPFOOD);
	PyModule_AddIntConstant(poModule, "PET_SKILL_ALL_DEL_BOOK", CItemData::PET_SKILL_ALL_DEL_BOOK);
	PyModule_AddIntConstant(poModule, "PET_EXPFOOD_PER", CItemData::PET_EXPFOOD_PER);
//#if defined(ENABLE_PET_ATTR_DETERMINE)
	PyModule_AddIntConstant(poModule, "PET_ATTR_DETERMINE", CItemData::PET_ATTR_DETERMINE);
	PyModule_AddIntConstant(poModule, "PET_ATTR_CHANGE", CItemData::PET_ATTR_CHANGE);
//#endif
//#endif
	PyModule_AddIntConstant(poModule, "PET_PAY", CItemData::PET_PAY);
//#if defined(ENABLE_GROWTH_PET_SYSTEM) && defined(ENABLE_PET_PRIMIUM_FEEDSTUFF)
	PyModule_AddIntConstant(poModule, "PET_PRIMIUM_FEEDSTUFF", CItemData::PET_PRIMIUM_FEEDSTUFF);
//#endif

#if defined(ENABLE_MOVE_COSTUME_ATTR)
	PyModule_AddIntConstant(poModule, "MEDIUM_MOVE_COSTUME_ATTR", CItemData::EMediumSubTypes::MEDIUM_MOVE_COSTUME_ATTR);
#if defined(ENABLE_ACCE_COSTUME_SYSTEM)
	PyModule_AddIntConstant(poModule, "MEDIUM_MOVE_ACCE_ATTR", CItemData::EMediumSubTypes::MEDIUM_MOVE_ACCE_ATTR);
#endif
#endif

#if defined(ENABLE_GACHA)
	PyModule_AddIntConstant(poModule, "USE_GACHA", CItemData::USE_GACHA);
#if defined(ENABLE_LUCKY_BOX)
	PyModule_AddIntConstant(poModule, "GEM_LUCKY_BOX_GACHA", CItemData::GEM_LUCKY_BOX_GACHA);
	PyModule_AddIntConstant(poModule, "SPECIAL_LUCKY_BOX_GACHA", CItemData::SPECIAL_LUCKY_BOX_GACHA);
#endif
#endif

	PyModule_AddIntConstant(poModule, "PASSIVE_JOB", CItemData::PASSIVE_JOB);

#if defined(ENABLE_SOUL_SYSTEM)
	PyModule_AddIntConstant(poModule, "RED_SOUL", CItemData::RED_SOUL);
	PyModule_AddIntConstant(poModule, "BLUE_SOUL", CItemData::BLUE_SOUL);
#endif

	PyModule_AddIntConstant(poModule, "ITEM_FLAG_REFINEABLE", CItemData::ITEM_FLAG_REFINEABLE);
	PyModule_AddIntConstant(poModule, "ITEM_FLAG_SAVE", CItemData::ITEM_FLAG_SAVE);
	PyModule_AddIntConstant(poModule, "ITEM_FLAG_STACKABLE", CItemData::ITEM_FLAG_STACKABLE);
	PyModule_AddIntConstant(poModule, "ITEM_FLAG_COUNT_PER_1GOLD", CItemData::ITEM_FLAG_COUNT_PER_1GOLD);
	PyModule_AddIntConstant(poModule, "ITEM_FLAG_SLOW_QUERY", CItemData::ITEM_FLAG_SLOW_QUERY);
	PyModule_AddIntConstant(poModule, "ITEM_FLAG_UNIQUE", CItemData::ITEM_FLAG_UNIQUE);
	PyModule_AddIntConstant(poModule, "ITEM_FLAG_MAKECOUNT", CItemData::ITEM_FLAG_MAKECOUNT);
	PyModule_AddIntConstant(poModule, "ITEM_FLAG_IRREMOVABLE", CItemData::ITEM_FLAG_IRREMOVABLE);
	PyModule_AddIntConstant(poModule, "ITEM_FLAG_CONFIRM_WHEN_USE", CItemData::ITEM_FLAG_CONFIRM_WHEN_USE);
	PyModule_AddIntConstant(poModule, "ITEM_FLAG_QUEST_USE", CItemData::ITEM_FLAG_QUEST_USE);
	PyModule_AddIntConstant(poModule, "ITEM_FLAG_QUEST_USE_MULTIPLE", CItemData::ITEM_FLAG_QUEST_USE_MULTIPLE);
	PyModule_AddIntConstant(poModule, "ITEM_FLAG_LOG", CItemData::ITEM_FLAG_LOG);
	PyModule_AddIntConstant(poModule, "ITEM_FLAG_APPLICABLE", CItemData::ITEM_FLAG_APPLICABLE);

	PyModule_AddIntConstant(poModule, "ANTIFLAG_FEMALE", CItemData::ITEM_ANTIFLAG_FEMALE);
	PyModule_AddIntConstant(poModule, "ANTIFLAG_MALE", CItemData::ITEM_ANTIFLAG_MALE);
	PyModule_AddIntConstant(poModule, "ANTIFLAG_WARRIOR", CItemData::ITEM_ANTIFLAG_WARRIOR);
	PyModule_AddIntConstant(poModule, "ANTIFLAG_ASSASSIN", CItemData::ITEM_ANTIFLAG_ASSASSIN);
	PyModule_AddIntConstant(poModule, "ANTIFLAG_SURA", CItemData::ITEM_ANTIFLAG_SURA);
	PyModule_AddIntConstant(poModule, "ANTIFLAG_SHAMAN", CItemData::ITEM_ANTIFLAG_SHAMAN);
	PyModule_AddIntConstant(poModule, "ANTIFLAG_GET", CItemData::ITEM_ANTIFLAG_GET);
	PyModule_AddIntConstant(poModule, "ANTIFLAG_DROP", CItemData::ITEM_ANTIFLAG_DROP);
	PyModule_AddIntConstant(poModule, "ANTIFLAG_SELL", CItemData::ITEM_ANTIFLAG_SELL);
	PyModule_AddIntConstant(poModule, "ANTIFLAG_EMPIRE_A", CItemData::ITEM_ANTIFLAG_EMPIRE_A);
	PyModule_AddIntConstant(poModule, "ANTIFLAG_EMPIRE_B", CItemData::ITEM_ANTIFLAG_EMPIRE_B);
	PyModule_AddIntConstant(poModule, "ANTIFLAG_EMPIRE_C", CItemData::ITEM_ANTIFLAG_EMPIRE_C);
	PyModule_AddIntConstant(poModule, "ANTIFLAG_SAVE", CItemData::ITEM_ANTIFLAG_SAVE);
	PyModule_AddIntConstant(poModule, "ANTIFLAG_GIVE", CItemData::ITEM_ANTIFLAG_GIVE);
	PyModule_AddIntConstant(poModule, "ANTIFLAG_PKDROP", CItemData::ITEM_ANTIFLAG_PKDROP);
	PyModule_AddIntConstant(poModule, "ANTIFLAG_STACK", CItemData::ITEM_ANTIFLAG_STACK);
	PyModule_AddIntConstant(poModule, "ANTIFLAG_MYSHOP", CItemData::ITEM_ANTIFLAG_MYSHOP);
	PyModule_AddIntConstant(poModule, "ANTIFLAG_SAFEBOX", CItemData::ITEM_ANTIFLAG_SAFEBOX);

	PyModule_AddIntConstant(poModule, "ITEM_ANTIFLAG_FEMALE", CItemData::ITEM_ANTIFLAG_FEMALE);
	PyModule_AddIntConstant(poModule, "ITEM_ANTIFLAG_MALE", CItemData::ITEM_ANTIFLAG_MALE);
	PyModule_AddIntConstant(poModule, "ITEM_ANTIFLAG_WARRIOR", CItemData::ITEM_ANTIFLAG_WARRIOR);
	PyModule_AddIntConstant(poModule, "ITEM_ANTIFLAG_ASSASSIN", CItemData::ITEM_ANTIFLAG_ASSASSIN);
	PyModule_AddIntConstant(poModule, "ITEM_ANTIFLAG_SURA", CItemData::ITEM_ANTIFLAG_SURA);
	PyModule_AddIntConstant(poModule, "ITEM_ANTIFLAG_SHAMAN", CItemData::ITEM_ANTIFLAG_SHAMAN);
	PyModule_AddIntConstant(poModule, "ITEM_ANTIFLAG_GET", CItemData::ITEM_ANTIFLAG_GET);
	PyModule_AddIntConstant(poModule, "ITEM_ANTIFLAG_DROP", CItemData::ITEM_ANTIFLAG_DROP);
	PyModule_AddIntConstant(poModule, "ITEM_ANTIFLAG_SELL", CItemData::ITEM_ANTIFLAG_SELL);
	PyModule_AddIntConstant(poModule, "ITEM_ANTIFLAG_EMPIRE_A", CItemData::ITEM_ANTIFLAG_EMPIRE_A);
	PyModule_AddIntConstant(poModule, "ITEM_ANTIFLAG_EMPIRE_B", CItemData::ITEM_ANTIFLAG_EMPIRE_B);
	PyModule_AddIntConstant(poModule, "ITEM_ANTIFLAG_EMPIRE_C", CItemData::ITEM_ANTIFLAG_EMPIRE_C);
	PyModule_AddIntConstant(poModule, "ITEM_ANTIFLAG_SAVE", CItemData::ITEM_ANTIFLAG_SAVE);
	PyModule_AddIntConstant(poModule, "ITEM_ANTIFLAG_GIVE", CItemData::ITEM_ANTIFLAG_GIVE);
	PyModule_AddIntConstant(poModule, "ITEM_ANTIFLAG_PKDROP", CItemData::ITEM_ANTIFLAG_PKDROP);
	PyModule_AddIntConstant(poModule, "ITEM_ANTIFLAG_STACK", CItemData::ITEM_ANTIFLAG_STACK);
	PyModule_AddIntConstant(poModule, "ITEM_ANTIFLAG_MYSHOP", CItemData::ITEM_ANTIFLAG_MYSHOP);
	PyModule_AddIntConstant(poModule, "ITEM_ANTIFLAG_SAFEBOX", CItemData::ITEM_ANTIFLAG_SAFEBOX);
	PyModule_AddIntConstant(poModule, "ITEM_ANTIFLAG_WOLFMAN", CItemData::ITEM_ANTIFLAG_WOLFMAN);
	PyModule_AddIntConstant(poModule, "ITEM_ANTIFLAG_RT_REMOVE", CItemData::ITEM_ANTIFLAG_RT_REMOVE);
	PyModule_AddIntConstant(poModule, "ITEM_ANTIFLAG_QUICKSLOT", CItemData::ITEM_ANTIFLAG_QUICKSLOT);
#if defined(ENABLE_CHANGE_LOOK_SYSTEM)
	PyModule_AddIntConstant(poModule, "ITEM_ANTIFLAG_CHANGELOOK", CItemData::ITEM_ANTIFLAG_CHANGELOOK);
#endif
	PyModule_AddIntConstant(poModule, "ITEM_ANTIFLAG_REINFORCE", CItemData::ITEM_ANTIFLAG_REINFORCE);
	PyModule_AddIntConstant(poModule, "ITEM_ANTIFLAG_ENCHANT", CItemData::ITEM_ANTIFLAG_ENCHANT);
	PyModule_AddIntConstant(poModule, "ITEM_ANTIFLAG_ENERGY", CItemData::ITEM_ANTIFLAG_ENERGY);
	PyModule_AddIntConstant(poModule, "ITEM_ANTIFLAG_PETFEED", CItemData::ITEM_ANTIFLAG_PETFEED);
	PyModule_AddIntConstant(poModule, "ITEM_ANTIFLAG_APPLY", CItemData::ITEM_ANTIFLAG_APPLY);
#if defined(ENABLE_ACCE_COSTUME_SYSTEM)
	PyModule_AddIntConstant(poModule, "ITEM_ANTIFLAG_ACCE", CItemData::ITEM_ANTIFLAG_ACCE);
#endif
#if defined(ENABLE_MAILBOX)
	PyModule_AddIntConstant(poModule, "ITEM_ANTIFLAG_MAIL", CItemData::ITEM_ANTIFLAG_MAIL);
#endif
	PyModule_AddIntConstant(poModule, "ITEM_ANTIFLAG_DESTROY", CItemData::ITEM_ANTIFLAG_DESTROY);

	PyModule_AddIntConstant(poModule, "WEARABLE_BODY", CItemData::WEARABLE_BODY);
	PyModule_AddIntConstant(poModule, "WEARABLE_HEAD", CItemData::WEARABLE_HEAD);
	PyModule_AddIntConstant(poModule, "WEARABLE_FOOTS", CItemData::WEARABLE_FOOTS);
	PyModule_AddIntConstant(poModule, "WEARABLE_WRIST", CItemData::WEARABLE_WRIST);
	PyModule_AddIntConstant(poModule, "WEARABLE_WEAPON", CItemData::WEARABLE_WEAPON);
	PyModule_AddIntConstant(poModule, "WEARABLE_NECK", CItemData::WEARABLE_NECK);
	PyModule_AddIntConstant(poModule, "WEARABLE_EAR", CItemData::WEARABLE_EAR);
	PyModule_AddIntConstant(poModule, "WEARABLE_UNIQUE", CItemData::WEARABLE_UNIQUE);
	PyModule_AddIntConstant(poModule, "WEARABLE_SHIELD", CItemData::WEARABLE_SHIELD);
	PyModule_AddIntConstant(poModule, "WEARABLE_ARROW", CItemData::WEARABLE_ARROW);
#if defined(ENABLE_PENDANT)
	PyModule_AddIntConstant(poModule, "WEARABLE_PENDANT", CItemData::WEARABLE_PENDANT);
#endif
#if defined(ENABLE_GLOVE_SYSTEM)
	PyModule_AddIntConstant(poModule, "WEARABLE_GLOVE", CItemData::WEARABLE_GLOVE);
#endif

	PyModule_AddIntConstant(poModule, "LIMIT_NONE", CItemData::LIMIT_NONE);
	PyModule_AddIntConstant(poModule, "LIMIT_LEVEL", CItemData::LIMIT_LEVEL);
	PyModule_AddIntConstant(poModule, "LIMIT_STR", CItemData::LIMIT_STR);
	PyModule_AddIntConstant(poModule, "LIMIT_DEX", CItemData::LIMIT_DEX);
	PyModule_AddIntConstant(poModule, "LIMIT_INT", CItemData::LIMIT_INT);
	PyModule_AddIntConstant(poModule, "LIMIT_CON", CItemData::LIMIT_CON);
	PyModule_AddIntConstant(poModule, "LIMIT_REAL_TIME", CItemData::LIMIT_REAL_TIME);
	PyModule_AddIntConstant(poModule, "LIMIT_REAL_TIME_START_FIRST_USE", CItemData::LIMIT_REAL_TIME_START_FIRST_USE);
	PyModule_AddIntConstant(poModule, "LIMIT_TIMER_BASED_ON_WEAR", CItemData::LIMIT_TIMER_BASED_ON_WEAR);
	PyModule_AddIntConstant(poModule, "LIMIT_NEWWORLD_LEVEL", CItemData::LIMIT_NEWWORLD_LEVEL);
	PyModule_AddIntConstant(poModule, "LIMIT_DURATION", CItemData::LIMIT_DURATION);
	PyModule_AddIntConstant(poModule, "LIMIT_TYPE_MAX_NUM", CItemData::LIMIT_MAX_NUM);
	PyModule_AddIntConstant(poModule, "LIMIT_MAX_NUM", CItemData::ITEM_LIMIT_MAX_NUM);

	PyModule_AddIntConstant(poModule, "REFINE_TYPE_CHUKBOK", CItemData::REFINE_TYPE_CHUKBOK);
	PyModule_AddIntConstant(poModule, "REFINE_TYPE_HYUNIRON", CItemData::REFINE_TYPE_HYUNIRON);
	PyModule_AddIntConstant(poModule, "REFINE_TYPE_YONGSIN", CItemData::REFINE_TYPE_YONGSIN);
	PyModule_AddIntConstant(poModule, "REFINE_TYPE_MUSIN", CItemData::REFINE_TYPE_MUSIN);
	PyModule_AddIntConstant(poModule, "REFINE_TYPE_YAGONG", CItemData::REFINE_TYPE_YAGONG);
	PyModule_AddIntConstant(poModule, "REFINE_TYPE_MEMO", CItemData::REFINE_TYPE_MEMO);
	PyModule_AddIntConstant(poModule, "REFINE_TYPE_BDRAGON", CItemData::REFINE_TYPE_BDRAGON);
#if defined(ENABLE_STONE_OF_BLESS)
	PyModule_AddIntConstant(poModule, "REFINE_TYPE_STONE_OF_BLESS", CItemData::REFINE_TYPE_STONE_OF_BLESS);
#endif
#if defined(ENABLE_SOUL_SYSTEM)
	PyModule_AddIntConstant(poModule, "REFINE_TYPE_SOUL_AWAKE", CItemData::REFINE_TYPE_SOUL_AWAKE);
	PyModule_AddIntConstant(poModule, "REFINE_TYPE_SOUL_EVOLVE", CItemData::REFINE_TYPE_SOUL_EVOLVE);
#endif

	PyModule_AddIntConstant(poModule, "ITEM_APPLY_MAX_NUM", CItemData::ITEM_APPLY_MAX_NUM);
	PyModule_AddIntConstant(poModule, "ITEM_SOCKET_MAX_NUM", CItemData::ITEM_SOCKET_MAX_NUM);

#if defined(ENABLE_REFINE_ELEMENT_SYSTEM)
	PyModule_AddIntConstant(poModule, "REFINE_ELEMENT_MAX", REFINE_ELEMENT_MAX);
#endif

#if defined(ENABLE_SOULBIND_SYSTEM)
	PyModule_AddIntConstant(poModule, "E_SEAL_DATE_DEFAULT_TIMESTAMP", CItemData::E_SEAL_DATE_DEFAULT_TIMESTAMP);
#endif

#if defined(ENABLE_APPLY_RANDOM)
	PyModule_AddIntConstant(poModule, "ITEM_SOCKET_DEF_MINMAX_RANDOM", CItemData::ITEM_SOCKET_DEF_MINMAX_RANDOM);
	PyModule_AddIntConstant(poModule, "ITEM_SOCKET_BELT_DEF_MINMAX_RANDOM", CItemData::ITEM_SOCKET_BELT_DEF_MINMAX_RANDOM);
	PyModule_AddIntConstant(poModule, "ITEM_SOCKET_MTK_MINMAX_RANDOM", CItemData::ITEM_SOCKET_MTK_MINMAX_RANDOM);
	PyModule_AddIntConstant(poModule, "ITEM_SOCKET_ATK_MINMAX_RANDOM", CItemData::ITEM_SOCKET_ATK_MINMAX_RANDOM);
	PyModule_AddIntConstant(poModule, "ITEM_VALUE_MINMAX_RANDOM_DIVISION_VALUE", CItemData::ITEM_VALUE_MINMAX_RANDOM_DIVISION_VALUE);
#endif

#if defined(ENABLE_AURA_COSTUME_SYSTEM)
	PyModule_AddIntConstant(poModule, "ITEM_SOCKET_AURA_DRAIN_ITEM_VNUM", ITEM_SOCKET_AURA_DRAIN_ITEM_VNUM);
	PyModule_AddIntConstant(poModule, "ITEM_SOCKET_AURA_LEVEL_VALUE", ITEM_SOCKET_AURA_LEVEL_VALUE);

	PyModule_AddIntConstant(poModule, "AURA_GRADE_NONE", CItemData::AURA_GRADE_NONE);
	PyModule_AddIntConstant(poModule, "AURA_GRADE_ORDINARY", CItemData::AURA_GRADE_ORDINARY);
	PyModule_AddIntConstant(poModule, "AURA_GRADE_SIMPLE", CItemData::AURA_GRADE_SIMPLE);
	PyModule_AddIntConstant(poModule, "AURA_GRADE_NOBLE", CItemData::AURA_GRADE_NOBLE);
	PyModule_AddIntConstant(poModule, "AURA_GRADE_SPARKLING", CItemData::AURA_GRADE_SPARKLING);
	PyModule_AddIntConstant(poModule, "AURA_GRADE_MAGNIFICENT", CItemData::AURA_GRADE_MAGNIFICENT);
	PyModule_AddIntConstant(poModule, "AURA_GRADE_RADIANT", CItemData::AURA_GRADE_RADIANT);
	PyModule_AddIntConstant(poModule, "AURA_GRADE_MAX_NUM", CItemData::AURA_GRADE_MAX_NUM);

	PyModule_AddIntConstant(poModule, "AURA_REFINE_INFO_STEP", AURA_REFINE_INFO_STEP);
	PyModule_AddIntConstant(poModule, "AURA_REFINE_INFO_LEVEL_MIN", AURA_REFINE_INFO_LEVEL_MIN);
	PyModule_AddIntConstant(poModule, "AURA_REFINE_INFO_LEVEL_MAX", AURA_REFINE_INFO_LEVEL_MAX);
	PyModule_AddIntConstant(poModule, "AURA_REFINE_INFO_NEED_EXP", AURA_REFINE_INFO_NEED_EXP);
	PyModule_AddIntConstant(poModule, "AURA_REFINE_INFO_MATERIAL_VNUM", AURA_REFINE_INFO_MATERIAL_VNUM);
	PyModule_AddIntConstant(poModule, "AURA_REFINE_INFO_MATERIAL_COUNT", AURA_REFINE_INFO_MATERIAL_COUNT);
	PyModule_AddIntConstant(poModule, "AURA_REFINE_INFO_NEED_GOLD", AURA_REFINE_INFO_NEED_GOLD);
	PyModule_AddIntConstant(poModule, "AURA_REFINE_INFO_EVOLVE_PCT", AURA_REFINE_INFO_EVOLVE_PCT);
#endif

	PyModule_AddIntConstant(poModule, "APPLY_NONE", CItemData::APPLY_NONE);
	PyModule_AddIntConstant(poModule, "APPLY_STR", CItemData::APPLY_STR);
	PyModule_AddIntConstant(poModule, "APPLY_DEX", CItemData::APPLY_DEX);
	PyModule_AddIntConstant(poModule, "APPLY_CON", CItemData::APPLY_CON);
	PyModule_AddIntConstant(poModule, "APPLY_INT", CItemData::APPLY_INT);
	PyModule_AddIntConstant(poModule, "APPLY_MAX_HP", CItemData::APPLY_MAX_HP);
	PyModule_AddIntConstant(poModule, "APPLY_MAX_SP", CItemData::APPLY_MAX_SP);
	PyModule_AddIntConstant(poModule, "APPLY_HP_REGEN", CItemData::APPLY_HP_REGEN);
	PyModule_AddIntConstant(poModule, "APPLY_SP_REGEN", CItemData::APPLY_SP_REGEN);
	PyModule_AddIntConstant(poModule, "APPLY_DEF_GRADE_BONUS", CItemData::APPLY_DEF_GRADE_BONUS);
	PyModule_AddIntConstant(poModule, "APPLY_ATT_GRADE_BONUS", CItemData::APPLY_ATT_GRADE_BONUS);
	PyModule_AddIntConstant(poModule, "APPLY_ATT_SPEED", CItemData::APPLY_ATT_SPEED);
	PyModule_AddIntConstant(poModule, "APPLY_MOV_SPEED", CItemData::APPLY_MOV_SPEED);
	PyModule_AddIntConstant(poModule, "APPLY_CAST_SPEED", CItemData::APPLY_CAST_SPEED);
	PyModule_AddIntConstant(poModule, "APPLY_MAGIC_ATT_GRADE", CItemData::APPLY_MAGIC_ATT_GRADE);
	PyModule_AddIntConstant(poModule, "APPLY_MAGIC_DEF_GRADE", CItemData::APPLY_MAGIC_DEF_GRADE);
	PyModule_AddIntConstant(poModule, "APPLY_SKILL", CItemData::APPLY_SKILL);
	PyModule_AddIntConstant(poModule, "APPLY_ATTBONUS_ANIMAL", CItemData::APPLY_ATTBONUS_ANIMAL);
	PyModule_AddIntConstant(poModule, "APPLY_ATTBONUS_UNDEAD", CItemData::APPLY_ATTBONUS_UNDEAD);
	PyModule_AddIntConstant(poModule, "APPLY_ATTBONUS_DEVIL", CItemData::APPLY_ATTBONUS_DEVIL);
	PyModule_AddIntConstant(poModule, "APPLY_ATTBONUS_HUMAN", CItemData::APPLY_ATTBONUS_HUMAN);
	PyModule_AddIntConstant(poModule, "APPLY_BOW_DISTANCE", CItemData::APPLY_BOW_DISTANCE);
	PyModule_AddIntConstant(poModule, "APPLY_RESIST_BOW", CItemData::APPLY_RESIST_BOW);
	PyModule_AddIntConstant(poModule, "APPLY_RESIST_FIRE", CItemData::APPLY_RESIST_FIRE);
	PyModule_AddIntConstant(poModule, "APPLY_RESIST_ELEC", CItemData::APPLY_RESIST_ELEC);
	PyModule_AddIntConstant(poModule, "APPLY_RESIST_MAGIC", CItemData::APPLY_RESIST_MAGIC);
	PyModule_AddIntConstant(poModule, "APPLY_POISON_PCT", CItemData::APPLY_POISON_PCT);
	PyModule_AddIntConstant(poModule, "APPLY_SLOW_PCT", CItemData::APPLY_SLOW_PCT);
	PyModule_AddIntConstant(poModule, "APPLY_STUN_PCT", CItemData::APPLY_STUN_PCT);
	PyModule_AddIntConstant(poModule, "APPLY_CRITICAL_PCT", CItemData::APPLY_CRITICAL_PCT); // n% 확률로 두배 타격
	PyModule_AddIntConstant(poModule, "APPLY_PENETRATE_PCT", CItemData::APPLY_PENETRATE_PCT); // n% 확률로 적의 방어력 무시
	PyModule_AddIntConstant(poModule, "APPLY_ATTBONUS_ORC", CItemData::APPLY_ATTBONUS_ORC); // 웅귀에게 n% 추가 데미지
	PyModule_AddIntConstant(poModule, "APPLY_ATTBONUS_MILGYO", CItemData::APPLY_ATTBONUS_MILGYO); // 밀교에게 n% 추가 데미지
	PyModule_AddIntConstant(poModule, "APPLY_ATTBONUS_UNDEAD", CItemData::APPLY_ATTBONUS_UNDEAD); // 시체에게 n% 추가 데미지
	PyModule_AddIntConstant(poModule, "APPLY_ATTBONUS_DEVIL", CItemData::APPLY_ATTBONUS_DEVIL); // 악마에게 n% 추가 데미지
	PyModule_AddIntConstant(poModule, "APPLY_STEAL_HP", CItemData::APPLY_STEAL_HP); // n% 확률로 타격의 10% 를 생명력으로 흡수
	PyModule_AddIntConstant(poModule, "APPLY_STEAL_SP", CItemData::APPLY_STEAL_SP); // n% 확률로 타격의 10% 를 정신력으로 흡수
	PyModule_AddIntConstant(poModule, "APPLY_MANA_BURN_PCT", CItemData::APPLY_MANA_BURN_PCT); // n% 확률로 상대의 마나를 깎는다
	PyModule_AddIntConstant(poModule, "APPLY_DAMAGE_SP_RECOVER", CItemData::APPLY_DAMAGE_SP_RECOVER); // n% 확률로 정신력 2 회복
	PyModule_AddIntConstant(poModule, "APPLY_BLOCK", CItemData::APPLY_BLOCK); // n% 확률로 물리공격 완벽 방어
	PyModule_AddIntConstant(poModule, "APPLY_DODGE", CItemData::APPLY_DODGE); // n% 확률로 물리공격 완벽 회피
	PyModule_AddIntConstant(poModule, "APPLY_RESIST_SWORD", CItemData::APPLY_RESIST_SWORD); // 한손검에 의한 피해를 n% 감소
	PyModule_AddIntConstant(poModule, "APPLY_RESIST_TWOHAND", CItemData::APPLY_RESIST_TWOHAND); // 양손검에 의한 피해를 n% 감소
	PyModule_AddIntConstant(poModule, "APPLY_RESIST_DAGGER", CItemData::APPLY_RESIST_DAGGER); // 단도에 의한 피해를 n% 감소
	PyModule_AddIntConstant(poModule, "APPLY_RESIST_BELL", CItemData::APPLY_RESIST_BELL); // 방울에 의한 피해를 n% 감소
	PyModule_AddIntConstant(poModule, "APPLY_RESIST_FAN", CItemData::APPLY_RESIST_FAN); // 부채에 의한 피해를 n% 감소
	PyModule_AddIntConstant(poModule, "APPLY_RESIST_WIND", CItemData::APPLY_RESIST_WIND); // 바람에 의한 피해를 n% 감소
	PyModule_AddIntConstant(poModule, "APPLY_REFLECT_MELEE", CItemData::APPLY_REFLECT_MELEE); // 근접 타격 n% 를 적에게 되돌린다
	PyModule_AddIntConstant(poModule, "APPLY_REFLECT_CURSE", CItemData::APPLY_REFLECT_CURSE); // 적이 나에게 저주 사용시 n% 확률로 되돌린다
	PyModule_AddIntConstant(poModule, "APPLY_POISON_REDUCE", CItemData::APPLY_POISON_REDUCE); // 독에 의한 데미지 감소
	PyModule_AddIntConstant(poModule, "APPLY_KILL_SP_RECOVER", CItemData::APPLY_KILL_SP_RECOVER); // 적을 죽였을때 n% 확률로 정신력 10 회복
	PyModule_AddIntConstant(poModule, "APPLY_EXP_DOUBLE_BONUS", CItemData::APPLY_EXP_DOUBLE_BONUS); // n% 확률로 경험치 획득량 2배
	PyModule_AddIntConstant(poModule, "APPLY_GOLD_DOUBLE_BONUS", CItemData::APPLY_GOLD_DOUBLE_BONUS); // n% 확률로 돈 획득량 2배
	PyModule_AddIntConstant(poModule, "APPLY_ITEM_DROP_BONUS", CItemData::APPLY_ITEM_DROP_BONUS); // n% 확률로 아이템 획득량 2배
	PyModule_AddIntConstant(poModule, "APPLY_POTION_BONUS", CItemData::APPLY_POTION_BONUS); // 물약 복용시 n% 만큼 성능 증대
	PyModule_AddIntConstant(poModule, "APPLY_KILL_HP_RECOVER", CItemData::APPLY_KILL_HP_RECOVER); // 죽일때마다 생명력 회복 
	PyModule_AddIntConstant(poModule, "APPLY_IMMUNE_STUN", CItemData::APPLY_IMMUNE_STUN); // 기절 하지 않는다
	PyModule_AddIntConstant(poModule, "APPLY_IMMUNE_SLOW", CItemData::APPLY_IMMUNE_SLOW); // 느려지지 않는다
	PyModule_AddIntConstant(poModule, "APPLY_IMMUNE_FALL", CItemData::APPLY_IMMUNE_FALL); // 넘어지지 않는다
	PyModule_AddIntConstant(poModule, "APPLY_MAX_STAMINA", CItemData::APPLY_MAX_STAMINA); // 최대 스테미너 증가
	PyModule_AddIntConstant(poModule, "APPLY_ATTBONUS_WARRIOR", CItemData::APPLY_ATTBONUS_WARRIOR);
	PyModule_AddIntConstant(poModule, "APPLY_ATTBONUS_ASSASSIN", CItemData::APPLY_ATTBONUS_ASSASSIN);
	PyModule_AddIntConstant(poModule, "APPLY_ATTBONUS_SURA", CItemData::APPLY_ATTBONUS_SURA);
	PyModule_AddIntConstant(poModule, "APPLY_ATTBONUS_SHAMAN", CItemData::APPLY_ATTBONUS_SHAMAN);
	PyModule_AddIntConstant(poModule, "APPLY_ATTBONUS_MONSTER", CItemData::APPLY_ATTBONUS_MONSTER);
	PyModule_AddIntConstant(poModule, "APPLY_MALL_ATTBONUS", CItemData::APPLY_MALL_ATTBONUS);
	PyModule_AddIntConstant(poModule, "APPLY_MALL_DEFBONUS", CItemData::APPLY_MALL_DEFBONUS);
	PyModule_AddIntConstant(poModule, "APPLY_MALL_EXPBONUS", CItemData::APPLY_MALL_EXPBONUS);
	PyModule_AddIntConstant(poModule, "APPLY_MALL_ITEMBONUS", CItemData::APPLY_MALL_ITEMBONUS);
	PyModule_AddIntConstant(poModule, "APPLY_MALL_GOLDBONUS", CItemData::APPLY_MALL_GOLDBONUS);
	PyModule_AddIntConstant(poModule, "APPLY_MAX_HP_PCT", CItemData::APPLY_MAX_HP_PCT);
	PyModule_AddIntConstant(poModule, "APPLY_MAX_SP_PCT", CItemData::APPLY_MAX_SP_PCT);
	PyModule_AddIntConstant(poModule, "APPLY_SKILL_DAMAGE_BONUS", CItemData::APPLY_SKILL_DAMAGE_BONUS);
	PyModule_AddIntConstant(poModule, "APPLY_NORMAL_HIT_DAMAGE_BONUS", CItemData::APPLY_NORMAL_HIT_DAMAGE_BONUS);
	PyModule_AddIntConstant(poModule, "APPLY_SKILL_DEFEND_BONUS", CItemData::APPLY_SKILL_DEFEND_BONUS);
	PyModule_AddIntConstant(poModule, "APPLY_NORMAL_HIT_DEFEND_BONUS", CItemData::APPLY_NORMAL_HIT_DEFEND_BONUS);
	PyModule_AddIntConstant(poModule, "APPLY_PC_BANG_EXP_BONUS", CItemData::APPLY_PC_BANG_EXP_BONUS);
	PyModule_AddIntConstant(poModule, "APPLY_PC_BANG_DROP_BONUS", CItemData::APPLY_PC_BANG_DROP_BONUS);
	PyModule_AddIntConstant(poModule, "APPLY_RESIST_WARRIOR", CItemData::APPLY_RESIST_WARRIOR);
	PyModule_AddIntConstant(poModule, "APPLY_RESIST_ASSASSIN", CItemData::APPLY_RESIST_ASSASSIN);
	PyModule_AddIntConstant(poModule, "APPLY_RESIST_SURA", CItemData::APPLY_RESIST_SURA);
	PyModule_AddIntConstant(poModule, "APPLY_RESIST_SHAMAN", CItemData::APPLY_RESIST_SHAMAN);
	PyModule_AddIntConstant(poModule, "APPLY_ENERGY", CItemData::APPLY_ENERGY); // 기력
	PyModule_AddIntConstant(poModule, "APPLY_COSTUME_ATTR_BONUS", CItemData::APPLY_COSTUME_ATTR_BONUS);
	PyModule_AddIntConstant(poModule, "APPLY_MAGIC_ATTBONUS_PER", CItemData::APPLY_MAGIC_ATTBONUS_PER);
	PyModule_AddIntConstant(poModule, "APPLY_MELEE_MAGIC_ATTBONUS_PER", CItemData::APPLY_MELEE_MAGIC_ATTBONUS_PER);
	PyModule_AddIntConstant(poModule, "APPLY_RESIST_ICE", CItemData::APPLY_RESIST_ICE);
	PyModule_AddIntConstant(poModule, "APPLY_RESIST_EARTH", CItemData::APPLY_RESIST_EARTH);
	PyModule_AddIntConstant(poModule, "APPLY_RESIST_DARK", CItemData::APPLY_RESIST_DARK);
	PyModule_AddIntConstant(poModule, "APPLY_ANTI_CRITICAL_PCT", CItemData::APPLY_ANTI_CRITICAL_PCT);
	PyModule_AddIntConstant(poModule, "APPLY_ANTI_PENETRATE_PCT", CItemData::APPLY_ANTI_PENETRATE_PCT);
	PyModule_AddIntConstant(poModule, "APPLY_BLEEDING_PCT", CItemData::APPLY_BLEEDING_PCT);
	PyModule_AddIntConstant(poModule, "APPLY_BLEEDING_REDUCE", CItemData::APPLY_BLEEDING_REDUCE);
	PyModule_AddIntConstant(poModule, "APPLY_ATTBONUS_WOLFMAN", CItemData::APPLY_ATTBONUS_WOLFMAN);
	PyModule_AddIntConstant(poModule, "APPLY_RESIST_WOLFMAN", CItemData::APPLY_RESIST_WOLFMAN);
	PyModule_AddIntConstant(poModule, "APPLY_RESIST_CLAW", CItemData::APPLY_RESIST_CLAW);
	PyModule_AddIntConstant(poModule, "APPLY_ACCEDRAIN_RATE", CItemData::APPLY_ACCEDRAIN_RATE);
	PyModule_AddIntConstant(poModule, "APPLY_RESIST_MAGIC_REDUCTION", CItemData::APPLY_RESIST_MAGIC_REDUCTION);
	PyModule_AddIntConstant(poModule, "APPLY_ENCHANT_ELECT", CItemData::APPLY_ENCHANT_ELECT);
	PyModule_AddIntConstant(poModule, "APPLY_ENCHANT_FIRE", CItemData::APPLY_ENCHANT_FIRE);
	PyModule_AddIntConstant(poModule, "APPLY_ENCHANT_ICE", CItemData::APPLY_ENCHANT_ICE);
	PyModule_AddIntConstant(poModule, "APPLY_ENCHANT_WIND", CItemData::APPLY_ENCHANT_WIND);
	PyModule_AddIntConstant(poModule, "APPLY_ENCHANT_EARTH", CItemData::APPLY_ENCHANT_EARTH);
	PyModule_AddIntConstant(poModule, "APPLY_ENCHANT_DARK", CItemData::APPLY_ENCHANT_DARK);
	PyModule_AddIntConstant(poModule, "APPLY_ATTBONUS_CZ", CItemData::APPLY_ATTBONUS_CZ);
	PyModule_AddIntConstant(poModule, "APPLY_ATTBONUS_INSECT", CItemData::APPLY_ATTBONUS_INSECT);
	PyModule_AddIntConstant(poModule, "APPLY_ATTBONUS_DESERT", CItemData::APPLY_ATTBONUS_DESERT);
	PyModule_AddIntConstant(poModule, "APPLY_ATTBONUS_SWORD", CItemData::APPLY_ATTBONUS_SWORD);
	PyModule_AddIntConstant(poModule, "APPLY_ATTBONUS_TWOHAND", CItemData::APPLY_ATTBONUS_TWOHAND);
	PyModule_AddIntConstant(poModule, "APPLY_ATTBONUS_DAGGER", CItemData::APPLY_ATTBONUS_DAGGER);
	PyModule_AddIntConstant(poModule, "APPLY_ATTBONUS_BELL", CItemData::APPLY_ATTBONUS_BELL);
	PyModule_AddIntConstant(poModule, "APPLY_ATTBONUS_FAN", CItemData::APPLY_ATTBONUS_FAN);
	PyModule_AddIntConstant(poModule, "APPLY_ATTBONUS_BOW", CItemData::APPLY_ATTBONUS_BOW);
	PyModule_AddIntConstant(poModule, "APPLY_ATTBONUS_CLAW", CItemData::APPLY_ATTBONUS_CLAW);
	PyModule_AddIntConstant(poModule, "APPLY_RESIST_HUMAN", CItemData::APPLY_RESIST_HUMAN);
	PyModule_AddIntConstant(poModule, "APPLY_RESIST_MOUNT_FALL", CItemData::APPLY_RESIST_MOUNT_FALL);
	PyModule_AddIntConstant(poModule, "APPLY_RESIST_FIST", CItemData::APPLY_RESIST_FIST);
	PyModule_AddIntConstant(poModule, "APPLY_MOUNT", CItemData::APPLY_MOUNT);
	PyModule_AddIntConstant(poModule, "APPLY_SKILL_DAMAGE_SAMYEON", CItemData::APPLY_SKILL_DAMAGE_SAMYEON);
	PyModule_AddIntConstant(poModule, "APPLY_SKILL_DAMAGE_TANHWAN", CItemData::APPLY_SKILL_DAMAGE_TANHWAN);
	PyModule_AddIntConstant(poModule, "APPLY_SKILL_DAMAGE_PALBANG", CItemData::APPLY_SKILL_DAMAGE_PALBANG);
	PyModule_AddIntConstant(poModule, "APPLY_SKILL_DAMAGE_GIGONGCHAM", CItemData::APPLY_SKILL_DAMAGE_GIGONGCHAM);
	PyModule_AddIntConstant(poModule, "APPLY_SKILL_DAMAGE_GYOKSAN", CItemData::APPLY_SKILL_DAMAGE_GYOKSAN);
	PyModule_AddIntConstant(poModule, "APPLY_SKILL_DAMAGE_GEOMPUNG", CItemData::APPLY_SKILL_DAMAGE_GEOMPUNG);
	PyModule_AddIntConstant(poModule, "APPLY_SKILL_DAMAGE_AMSEOP", CItemData::APPLY_SKILL_DAMAGE_AMSEOP);
	PyModule_AddIntConstant(poModule, "APPLY_SKILL_DAMAGE_GUNGSIN", CItemData::APPLY_SKILL_DAMAGE_GUNGSIN);
	PyModule_AddIntConstant(poModule, "APPLY_SKILL_DAMAGE_CHARYUN", CItemData::APPLY_SKILL_DAMAGE_CHARYUN);
	PyModule_AddIntConstant(poModule, "APPLY_SKILL_DAMAGE_SANGONG", CItemData::APPLY_SKILL_DAMAGE_SANGONG);
	PyModule_AddIntConstant(poModule, "APPLY_SKILL_DAMAGE_YEONSA", CItemData::APPLY_SKILL_DAMAGE_YEONSA);
	PyModule_AddIntConstant(poModule, "APPLY_SKILL_DAMAGE_KWANKYEOK", CItemData::APPLY_SKILL_DAMAGE_KWANKYEOK);
	PyModule_AddIntConstant(poModule, "APPLY_SKILL_DAMAGE_GIGUNG", CItemData::APPLY_SKILL_DAMAGE_GIGUNG);
	PyModule_AddIntConstant(poModule, "APPLY_SKILL_DAMAGE_HWAJO", CItemData::APPLY_SKILL_DAMAGE_HWAJO);
	PyModule_AddIntConstant(poModule, "APPLY_SKILL_DAMAGE_SWAERYUNG", CItemData::APPLY_SKILL_DAMAGE_SWAERYUNG);
	PyModule_AddIntConstant(poModule, "APPLY_SKILL_DAMAGE_YONGKWON", CItemData::APPLY_SKILL_DAMAGE_YONGKWON);
	PyModule_AddIntConstant(poModule, "APPLY_SKILL_DAMAGE_PABEOB", CItemData::APPLY_SKILL_DAMAGE_PABEOB);
	PyModule_AddIntConstant(poModule, "APPLY_SKILL_DAMAGE_MARYUNG", CItemData::APPLY_SKILL_DAMAGE_MARYUNG);
	PyModule_AddIntConstant(poModule, "APPLY_SKILL_DAMAGE_HWAYEOMPOK", CItemData::APPLY_SKILL_DAMAGE_HWAYEOMPOK);
	PyModule_AddIntConstant(poModule, "APPLY_SKILL_DAMAGE_MAHWAN", CItemData::APPLY_SKILL_DAMAGE_MAHWAN);
	PyModule_AddIntConstant(poModule, "APPLY_SKILL_DAMAGE_BIPABU", CItemData::APPLY_SKILL_DAMAGE_BIPABU);
	PyModule_AddIntConstant(poModule, "APPLY_SKILL_DAMAGE_YONGBI", CItemData::APPLY_SKILL_DAMAGE_YONGBI);
	PyModule_AddIntConstant(poModule, "APPLY_SKILL_DAMAGE_PAERYONG", CItemData::APPLY_SKILL_DAMAGE_PAERYONG);
	PyModule_AddIntConstant(poModule, "APPLY_SKILL_DAMAGE_NOEJEON", CItemData::APPLY_SKILL_DAMAGE_NOEJEON);
	PyModule_AddIntConstant(poModule, "APPLY_SKILL_DAMAGE_BYEURAK", CItemData::APPLY_SKILL_DAMAGE_BYEURAK);
	PyModule_AddIntConstant(poModule, "APPLY_SKILL_DAMAGE_CHAIN", CItemData::APPLY_SKILL_DAMAGE_CHAIN);
	PyModule_AddIntConstant(poModule, "APPLY_SKILL_DAMAGE_CHAYEOL", CItemData::APPLY_SKILL_DAMAGE_CHAYEOL);
	PyModule_AddIntConstant(poModule, "APPLY_SKILL_DAMAGE_SALPOONG", CItemData::APPLY_SKILL_DAMAGE_SALPOONG);
	PyModule_AddIntConstant(poModule, "APPLY_SKILL_DAMAGE_GONGDAB", CItemData::APPLY_SKILL_DAMAGE_GONGDAB);
	PyModule_AddIntConstant(poModule, "APPLY_SKILL_DAMAGE_PASWAE", CItemData::APPLY_SKILL_DAMAGE_PASWAE);
	PyModule_AddIntConstant(poModule, "APPLY_NORMAL_HIT_DEFEND_BONUS_BOSS_OR_MORE", CItemData::APPLY_NORMAL_HIT_DEFEND_BONUS_BOSS_OR_MORE);
	PyModule_AddIntConstant(poModule, "APPLY_SKILL_DEFEND_BONUS_BOSS_OR_MORE", CItemData::APPLY_SKILL_DEFEND_BONUS_BOSS_OR_MORE);
	PyModule_AddIntConstant(poModule, "APPLY_NORMAL_HIT_DAMAGE_BONUS_BOSS_OR_MORE", CItemData::APPLY_NORMAL_HIT_DAMAGE_BONUS_BOSS_OR_MORE);
	PyModule_AddIntConstant(poModule, "APPLY_SKILL_DAMAGE_BONUS_BOSS_OR_MORE", CItemData::APPLY_SKILL_DAMAGE_BONUS_BOSS_OR_MORE);
	PyModule_AddIntConstant(poModule, "APPLY_HIT_BUFF_ENCHANT_FIRE", CItemData::APPLY_HIT_BUFF_ENCHANT_FIRE);
	PyModule_AddIntConstant(poModule, "APPLY_HIT_BUFF_ENCHANT_ICE", CItemData::APPLY_HIT_BUFF_ENCHANT_ICE);
	PyModule_AddIntConstant(poModule, "APPLY_HIT_BUFF_ENCHANT_ELEC", CItemData::APPLY_HIT_BUFF_ENCHANT_ELEC);
	PyModule_AddIntConstant(poModule, "APPLY_HIT_BUFF_ENCHANT_WIND", CItemData::APPLY_HIT_BUFF_ENCHANT_WIND);
	PyModule_AddIntConstant(poModule, "APPLY_HIT_BUFF_ENCHANT_DARK", CItemData::APPLY_HIT_BUFF_ENCHANT_DARK);
	PyModule_AddIntConstant(poModule, "APPLY_HIT_BUFF_ENCHANT_EARTH", CItemData::APPLY_HIT_BUFF_ENCHANT_EARTH);
	PyModule_AddIntConstant(poModule, "APPLY_HIT_BUFF_RESIST_FIRE", CItemData::APPLY_HIT_BUFF_RESIST_FIRE);
	PyModule_AddIntConstant(poModule, "APPLY_HIT_BUFF_RESIST_ICE", CItemData::APPLY_HIT_BUFF_RESIST_ICE);
	PyModule_AddIntConstant(poModule, "APPLY_HIT_BUFF_RESIST_ELEC", CItemData::APPLY_HIT_BUFF_RESIST_ELEC);
	PyModule_AddIntConstant(poModule, "APPLY_HIT_BUFF_RESIST_WIND", CItemData::APPLY_HIT_BUFF_RESIST_WIND);
	PyModule_AddIntConstant(poModule, "APPLY_HIT_BUFF_RESIST_DARK", CItemData::APPLY_HIT_BUFF_RESIST_DARK);
	PyModule_AddIntConstant(poModule, "APPLY_HIT_BUFF_RESIST_EARTH", CItemData::APPLY_HIT_BUFF_RESIST_EARTH);
	PyModule_AddIntConstant(poModule, "APPLY_USE_SKILL_CHEONGRANG_MOV_SPEED", CItemData::APPLY_USE_SKILL_CHEONGRANG_MOV_SPEED);
	PyModule_AddIntConstant(poModule, "APPLY_USE_SKILL_CHEONGRANG_CASTING_SPEED", CItemData::APPLY_USE_SKILL_CHEONGRANG_CASTING_SPEED);
	PyModule_AddIntConstant(poModule, "APPLY_USE_SKILL_CHAYEOL_CRITICAL_PCT", CItemData::APPLY_USE_SKILL_CHAYEOL_CRITICAL_PCT);
	PyModule_AddIntConstant(poModule, "APPLY_USE_SKILL_SANGONG_ATT_GRADE_BONUS", CItemData::APPLY_USE_SKILL_SANGONG_ATT_GRADE_BONUS);
	PyModule_AddIntConstant(poModule, "APPLY_USE_SKILL_GIGUNG_ATT_GRADE_BONUS", CItemData::APPLY_USE_SKILL_GIGUNG_ATT_GRADE_BONUS);
	PyModule_AddIntConstant(poModule, "APPLY_USE_SKILL_JEOKRANG_DEF_BONUS", CItemData::APPLY_USE_SKILL_JEOKRANG_DEF_BONUS);
	PyModule_AddIntConstant(poModule, "APPLY_USE_SKILL_GWIGEOM_DEF_BONUS", CItemData::APPLY_USE_SKILL_GWIGEOM_DEF_BONUS);
	PyModule_AddIntConstant(poModule, "APPLY_USE_SKILL_TERROR_ATT_GRADE_BONUS", CItemData::APPLY_USE_SKILL_TERROR_ATT_GRADE_BONUS);
	PyModule_AddIntConstant(poModule, "APPLY_USE_SKILL_MUYEONG_ATT_GRADE_BONUS", CItemData::APPLY_USE_SKILL_MUYEONG_ATT_GRADE_BONUS);
	PyModule_AddIntConstant(poModule, "APPLY_USE_SKILL_MANASHILED_CASTING_SPEED", CItemData::APPLY_USE_SKILL_MANASHILED_CASTING_SPEED);
	PyModule_AddIntConstant(poModule, "APPLY_USE_SKILL_HOSIN_DEF_BONUS", CItemData::APPLY_USE_SKILL_HOSIN_DEF_BONUS);
	PyModule_AddIntConstant(poModule, "APPLY_USE_SKILL_GICHEON_ATT_GRADE_BONUS", CItemData::APPLY_USE_SKILL_GICHEON_ATT_GRADE_BONUS);
	PyModule_AddIntConstant(poModule, "APPLY_USE_SKILL_JEONGEOP_ATT_GRADE_BONUS", CItemData::APPLY_USE_SKILL_JEONGEOP_ATT_GRADE_BONUS);
	PyModule_AddIntConstant(poModule, "APPLY_USE_SKILL_JEUNGRYEOK_DEF_BONUS", CItemData::APPLY_USE_SKILL_JEUNGRYEOK_DEF_BONUS);
	PyModule_AddIntConstant(poModule, "APPLY_USE_SKILL_GIHYEOL_ATT_GRADE_BONUS", CItemData::APPLY_USE_SKILL_GIHYEOL_ATT_GRADE_BONUS);
	PyModule_AddIntConstant(poModule, "APPLY_USE_SKILL_CHUNKEON_CASTING_SPEED", CItemData::APPLY_USE_SKILL_CHUNKEON_CASTING_SPEED);
	PyModule_AddIntConstant(poModule, "APPLY_USE_SKILL_NOEGEOM_ATT_GRADE_BONUS", CItemData::APPLY_USE_SKILL_NOEGEOM_ATT_GRADE_BONUS);
	PyModule_AddIntConstant(poModule, "APPLY_SKILL_DURATION_INCREASE_EUNHYUNG", CItemData::APPLY_SKILL_DURATION_INCREASE_EUNHYUNG);
	PyModule_AddIntConstant(poModule, "APPLY_SKILL_DURATION_INCREASE_GYEONGGONG", CItemData::APPLY_SKILL_DURATION_INCREASE_GYEONGGONG);
	PyModule_AddIntConstant(poModule, "APPLY_SKILL_DURATION_INCREASE_GEOMKYUNG", CItemData::APPLY_SKILL_DURATION_INCREASE_GEOMKYUNG);
	PyModule_AddIntConstant(poModule, "APPLY_SKILL_DURATION_INCREASE_JEOKRANG", CItemData::APPLY_SKILL_DURATION_INCREASE_JEOKRANG);
	PyModule_AddIntConstant(poModule, "APPLY_USE_SKILL_PALBANG_HP_ABSORB", CItemData::APPLY_USE_SKILL_PALBANG_HP_ABSORB);
	PyModule_AddIntConstant(poModule, "APPLY_USE_SKILL_AMSEOP_HP_ABSORB", CItemData::APPLY_USE_SKILL_AMSEOP_HP_ABSORB);
	PyModule_AddIntConstant(poModule, "APPLY_USE_SKILL_YEONSA_HP_ABSORB", CItemData::APPLY_USE_SKILL_YEONSA_HP_ABSORB);
	PyModule_AddIntConstant(poModule, "APPLY_USE_SKILL_YONGBI_HP_ABSORB", CItemData::APPLY_USE_SKILL_YONGBI_HP_ABSORB);
	PyModule_AddIntConstant(poModule, "APPLY_USE_SKILL_CHAIN_HP_ABSORB", CItemData::APPLY_USE_SKILL_CHAIN_HP_ABSORB);
	PyModule_AddIntConstant(poModule, "APPLY_USE_SKILL_PASWAE_SP_ABSORB", CItemData::APPLY_USE_SKILL_PASWAE_SP_ABSORB);
	PyModule_AddIntConstant(poModule, "APPLY_USE_SKILL_GIGONGCHAM_STUN", CItemData::APPLY_USE_SKILL_GIGONGCHAM_STUN);
	PyModule_AddIntConstant(poModule, "APPLY_USE_SKILL_CHARYUN_STUN", CItemData::APPLY_USE_SKILL_CHARYUN_STUN);
	PyModule_AddIntConstant(poModule, "APPLY_USE_SKILL_PABEOB_STUN", CItemData::APPLY_USE_SKILL_PABEOB_STUN);
	PyModule_AddIntConstant(poModule, "APPLY_USE_SKILL_MAHWAN_STUN", CItemData::APPLY_USE_SKILL_MAHWAN_STUN);
	PyModule_AddIntConstant(poModule, "APPLY_USE_SKILL_GONGDAB_STUN", CItemData::APPLY_USE_SKILL_GONGDAB_STUN);
	PyModule_AddIntConstant(poModule, "APPLY_USE_SKILL_SAMYEON_STUN", CItemData::APPLY_USE_SKILL_SAMYEON_STUN);
	PyModule_AddIntConstant(poModule, "APPLY_USE_SKILL_GYOKSAN_KNOCKBACK", CItemData::APPLY_USE_SKILL_GYOKSAN_KNOCKBACK);
	PyModule_AddIntConstant(poModule, "APPLY_USE_SKILL_SEOMJEON_KNOCKBACK", CItemData::APPLY_USE_SKILL_SEOMJEON_KNOCKBACK);
	PyModule_AddIntConstant(poModule, "APPLY_USE_SKILL_SWAERYUNG_KNOCKBACK", CItemData::APPLY_USE_SKILL_SWAERYUNG_KNOCKBACK);
	PyModule_AddIntConstant(poModule, "APPLY_USE_SKILL_HWAYEOMPOK_KNOCKBACK", CItemData::APPLY_USE_SKILL_HWAYEOMPOK_KNOCKBACK);
	PyModule_AddIntConstant(poModule, "APPLY_USE_SKILL_GONGDAB_KNOCKBACK", CItemData::APPLY_USE_SKILL_GONGDAB_KNOCKBACK);
	PyModule_AddIntConstant(poModule, "APPLY_USE_SKILL_KWANKYEOK_KNOCKBACK", CItemData::APPLY_USE_SKILL_KWANKYEOK_KNOCKBACK);
	PyModule_AddIntConstant(poModule, "APPLY_USE_SKILL_SAMYEON_NEXT_COOLTIME_DECREASE_10PER", CItemData::APPLY_USE_SKILL_SAMYEON_NEXT_COOLTIME_DECREASE_10PER);
	PyModule_AddIntConstant(poModule, "APPLY_USE_SKILL_GEOMPUNG_NEXT_COOLTIME_DECREASE_10PER", CItemData::APPLY_USE_SKILL_GEOMPUNG_NEXT_COOLTIME_DECREASE_10PER);
	PyModule_AddIntConstant(poModule, "APPLY_USE_SKILL_GUNGSIN_NEXT_COOLTIME_DECREASE_10PER", CItemData::APPLY_USE_SKILL_GUNGSIN_NEXT_COOLTIME_DECREASE_10PER);
	PyModule_AddIntConstant(poModule, "APPLY_USE_SKILL_KWANKYEOK_NEXT_COOLTIME_DECREASE_10PER", CItemData::APPLY_USE_SKILL_KWANKYEOK_NEXT_COOLTIME_DECREASE_10PER);
	PyModule_AddIntConstant(poModule, "APPLY_USE_SKILL_YONGKWON_NEXT_COOLTIME_DECREASE_10PER", CItemData::APPLY_USE_SKILL_YONGKWON_NEXT_COOLTIME_DECREASE_10PER);
	PyModule_AddIntConstant(poModule, "APPLY_USE_SKILL_MARYUNG_NEXT_COOLTIME_DECREASE_10PER", CItemData::APPLY_USE_SKILL_MARYUNG_NEXT_COOLTIME_DECREASE_10PER);
	PyModule_AddIntConstant(poModule, "APPLY_USE_SKILL_BIPABU_NEXT_COOLTIME_DECREASE_10PER", CItemData::APPLY_USE_SKILL_BIPABU_NEXT_COOLTIME_DECREASE_10PER);
	PyModule_AddIntConstant(poModule, "APPLY_USE_SKILL_NOEJEON_NEXT_COOLTIME_DECREASE_10PER", CItemData::APPLY_USE_SKILL_NOEJEON_NEXT_COOLTIME_DECREASE_10PER);
	PyModule_AddIntConstant(poModule, "APPLY_USE_SKILL_SALPOONG_NEXT_COOLTIME_DECREASE_10PER", CItemData::APPLY_USE_SKILL_SALPOONG_NEXT_COOLTIME_DECREASE_10PER);
	PyModule_AddIntConstant(poModule, "APPLY_USE_SKILL_PASWAE_NEXT_COOLTIME_DECREASE_10PER", CItemData::APPLY_USE_SKILL_PASWAE_NEXT_COOLTIME_DECREASE_10PER);
	PyModule_AddIntConstant(poModule, "APPLY_ATTBONUS_STONE", CItemData::APPLY_ATTBONUS_STONE);
	PyModule_AddIntConstant(poModule, "APPLY_DAMAGE_HP_RECOVERY", CItemData::APPLY_DAMAGE_HP_RECOVERY);
	PyModule_AddIntConstant(poModule, "APPLY_DAMAGE_SP_RECOVERY", CItemData::APPLY_DAMAGE_SP_RECOVERY);
	PyModule_AddIntConstant(poModule, "APPLY_ALIGNMENT_DAMAGE_BONUS", CItemData::APPLY_ALIGNMENT_DAMAGE_BONUS);
	PyModule_AddIntConstant(poModule, "APPLY_NORMAL_DAMAGE_GUARD", CItemData::APPLY_NORMAL_DAMAGE_GUARD);
	PyModule_AddIntConstant(poModule, "APPLY_MORE_THEN_HP90_DAMAGE_REDUCE", CItemData::APPLY_MORE_THEN_HP90_DAMAGE_REDUCE);
	PyModule_AddIntConstant(poModule, "APPLY_USE_SKILL_TUSOK_HP_ABSORB", CItemData::APPLY_USE_SKILL_TUSOK_HP_ABSORB);
	PyModule_AddIntConstant(poModule, "APPLY_USE_SKILL_PAERYONG_HP_ABSORB", CItemData::APPLY_USE_SKILL_PAERYONG_HP_ABSORB);
	PyModule_AddIntConstant(poModule, "APPLY_USE_SKILL_BYEURAK_HP_ABSORB", CItemData::APPLY_USE_SKILL_BYEURAK_HP_ABSORB);
	PyModule_AddIntConstant(poModule, "APPLY_FIRST_ATTRIBUTE_BONUS", CItemData::APPLY_FIRST_ATTRIBUTE_BONUS);
	PyModule_AddIntConstant(poModule, "APPLY_SECOND_ATTRIBUTE_BONUS", CItemData::APPLY_SECOND_ATTRIBUTE_BONUS);
	PyModule_AddIntConstant(poModule, "APPLY_THIRD_ATTRIBUTE_BONUS", CItemData::APPLY_THIRD_ATTRIBUTE_BONUS);
	PyModule_AddIntConstant(poModule, "APPLY_FOURTH_ATTRIBUTE_BONUS", CItemData::APPLY_FOURTH_ATTRIBUTE_BONUS);
	PyModule_AddIntConstant(poModule, "APPLY_FIFTH_ATTRIBUTE_BONUS", CItemData::APPLY_FIFTH_ATTRIBUTE_BONUS);
	PyModule_AddIntConstant(poModule, "APPLY_USE_SKILL_SAMYEON_NEXT_COOLTIME_DECREASE_20PER", CItemData::APPLY_USE_SKILL_SAMYEON_NEXT_COOLTIME_DECREASE_20PER);
	PyModule_AddIntConstant(poModule, "APPLY_USE_SKILL_GEOMPUNG_NEXT_COOLTIME_DECREASE_20PER", CItemData::APPLY_USE_SKILL_GEOMPUNG_NEXT_COOLTIME_DECREASE_20PER);
	PyModule_AddIntConstant(poModule, "APPLY_USE_SKILL_GUNGSIN_NEXT_COOLTIME_DECREASE_20PER", CItemData::APPLY_USE_SKILL_GUNGSIN_NEXT_COOLTIME_DECREASE_20PER);
	PyModule_AddIntConstant(poModule, "APPLY_USE_SKILL_KWANKYEOK_NEXT_COOLTIME_DECREASE_20PER", CItemData::APPLY_USE_SKILL_KWANKYEOK_NEXT_COOLTIME_DECREASE_20PER);
	PyModule_AddIntConstant(poModule, "APPLY_USE_SKILL_YONGKWON_NEXT_COOLTIME_DECREASE_20PER", CItemData::APPLY_USE_SKILL_YONGKWON_NEXT_COOLTIME_DECREASE_20PER);
	PyModule_AddIntConstant(poModule, "APPLY_USE_SKILL_MARYUNG_NEXT_COOLTIME_DECREASE_20PER", CItemData::APPLY_USE_SKILL_MARYUNG_NEXT_COOLTIME_DECREASE_20PER);
	PyModule_AddIntConstant(poModule, "APPLY_USE_SKILL_BIPABU_NEXT_COOLTIME_DECREASE_20PER", CItemData::APPLY_USE_SKILL_BIPABU_NEXT_COOLTIME_DECREASE_20PER);
	PyModule_AddIntConstant(poModule, "APPLY_USE_SKILL_NOEJEON_NEXT_COOLTIME_DECREASE_20PER", CItemData::APPLY_USE_SKILL_NOEJEON_NEXT_COOLTIME_DECREASE_20PER);
	PyModule_AddIntConstant(poModule, "APPLY_USE_SKILL_SALPOONG_NEXT_COOLTIME_DECREASE_20PER", CItemData::APPLY_USE_SKILL_SALPOONG_NEXT_COOLTIME_DECREASE_20PER);
	PyModule_AddIntConstant(poModule, "APPLY_USE_SKILL_PASWAE_NEXT_COOLTIME_DECREASE_20PER", CItemData::APPLY_USE_SKILL_PASWAE_NEXT_COOLTIME_DECREASE_20PER);
	PyModule_AddIntConstant(poModule, "APPLY_USE_SKILL_CHAYEOL_HP_ABSORB", CItemData::APPLY_USE_SKILL_CHAYEOL_HP_ABSORB);
	PyModule_AddIntConstant(poModule, "APPLY_SUNGMA_STR", CItemData::APPLY_SUNGMA_STR);
	PyModule_AddIntConstant(poModule, "APPLY_SUNGMA_HP", CItemData::APPLY_SUNGMA_HP);
	PyModule_AddIntConstant(poModule, "APPLY_SUNGMA_MOVE", CItemData::APPLY_SUNGMA_MOVE);
	PyModule_AddIntConstant(poModule, "APPLY_SUNGMA_IMMUNE", CItemData::APPLY_SUNGMA_IMMUNE);
	PyModule_AddIntConstant(poModule, "APPLY_HIT_PCT", CItemData::APPLY_HIT_PCT);
	PyModule_AddIntConstant(poModule, "APPLY_RANDOM", CItemData::APPLY_RANDOM);
	PyModule_AddIntConstant(poModule, "APPLY_ATTBONUS_PER_HUMAN", CItemData::APPLY_ATTBONUS_PER_HUMAN);
	PyModule_AddIntConstant(poModule, "APPLY_ATTBONUS_PER_ANIMAL", CItemData::APPLY_ATTBONUS_PER_ANIMAL);
	PyModule_AddIntConstant(poModule, "APPLY_ATTBONUS_PER_ORC", CItemData::APPLY_ATTBONUS_PER_ORC);
	PyModule_AddIntConstant(poModule, "APPLY_ATTBONUS_PER_MILGYO", CItemData::APPLY_ATTBONUS_PER_MILGYO);
	PyModule_AddIntConstant(poModule, "APPLY_ATTBONUS_PER_UNDEAD", CItemData::APPLY_ATTBONUS_PER_UNDEAD);
	PyModule_AddIntConstant(poModule, "APPLY_ATTBONUS_PER_DEVIL", CItemData::APPLY_ATTBONUS_PER_DEVIL);
	PyModule_AddIntConstant(poModule, "APPLY_ENCHANT_PER_ELECT", CItemData::APPLY_ENCHANT_PER_ELECT);
	PyModule_AddIntConstant(poModule, "APPLY_ENCHANT_PER_FIRE", CItemData::APPLY_ENCHANT_PER_FIRE);
	PyModule_AddIntConstant(poModule, "APPLY_ENCHANT_PER_ICE", CItemData::APPLY_ENCHANT_PER_ICE);
	PyModule_AddIntConstant(poModule, "APPLY_ENCHANT_PER_WIND", CItemData::APPLY_ENCHANT_PER_WIND);
	PyModule_AddIntConstant(poModule, "APPLY_ENCHANT_PER_EARTH", CItemData::APPLY_ENCHANT_PER_EARTH);
	PyModule_AddIntConstant(poModule, "APPLY_ENCHANT_PER_DARK", CItemData::APPLY_ENCHANT_PER_DARK);
	PyModule_AddIntConstant(poModule, "APPLY_ATTBONUS_PER_CZ", CItemData::APPLY_ATTBONUS_PER_CZ);
	PyModule_AddIntConstant(poModule, "APPLY_ATTBONUS_PER_INSECT", CItemData::APPLY_ATTBONUS_PER_INSECT);
	PyModule_AddIntConstant(poModule, "APPLY_ATTBONUS_PER_DESERT", CItemData::APPLY_ATTBONUS_PER_DESERT);
	PyModule_AddIntConstant(poModule, "APPLY_ATTBONUS_PER_STONE", CItemData::APPLY_ATTBONUS_PER_STONE);
	PyModule_AddIntConstant(poModule, "APPLY_ATTBONUS_PER_MONSTER", CItemData::APPLY_ATTBONUS_PER_MONSTER);
	PyModule_AddIntConstant(poModule, "APPLY_RESIST_PER_HUMAN", CItemData::APPLY_RESIST_PER_HUMAN);
	PyModule_AddIntConstant(poModule, "APPLY_RESIST_PER_ICE", CItemData::APPLY_RESIST_PER_ICE);
	PyModule_AddIntConstant(poModule, "APPLY_RESIST_PER_DARK", CItemData::APPLY_RESIST_PER_DARK);
	PyModule_AddIntConstant(poModule, "APPLY_RESIST_PER_EARTH", CItemData::APPLY_RESIST_PER_EARTH);
	PyModule_AddIntConstant(poModule, "APPLY_RESIST_PER_FIRE", CItemData::APPLY_RESIST_PER_FIRE);
	PyModule_AddIntConstant(poModule, "APPLY_RESIST_PER_ELEC", CItemData::APPLY_RESIST_PER_ELEC);
	PyModule_AddIntConstant(poModule, "APPLY_RESIST_PER_MAGIC", CItemData::APPLY_RESIST_PER_MAGIC);
	PyModule_AddIntConstant(poModule, "APPLY_RESIST_PER_WIND", CItemData::APPLY_RESIST_PER_WIND);
	PyModule_AddIntConstant(poModule, "APPLY_HIT_BUFF_SUNGMA_STR", CItemData::APPLY_HIT_BUFF_SUNGMA_STR);
	PyModule_AddIntConstant(poModule, "APPLY_HIT_BUFF_SUNGMA_MOVE", CItemData::APPLY_HIT_BUFF_SUNGMA_MOVE);
	PyModule_AddIntConstant(poModule, "APPLY_HIT_BUFF_SUNGMA_HP", CItemData::APPLY_HIT_BUFF_SUNGMA_HP);
	PyModule_AddIntConstant(poModule, "APPLY_HIT_BUFF_SUNGMA_IMMUNE", CItemData::APPLY_HIT_BUFF_SUNGMA_IMMUNE);
	PyModule_AddIntConstant(poModule, "APPLY_MOUNT_MELEE_MAGIC_ATTBONUS_PER", CItemData::APPLY_MOUNT_MELEE_MAGIC_ATTBONUS_PER);
	PyModule_AddIntConstant(poModule, "APPLY_DISMOUNT_MOVE_SPEED_BONUS_PER", CItemData::APPLY_DISMOUNT_MOVE_SPEED_BONUS_PER);
	PyModule_AddIntConstant(poModule, "APPLY_HIT_AUTO_HP_RECOVERY", CItemData::APPLY_HIT_AUTO_HP_RECOVERY);
	PyModule_AddIntConstant(poModule, "APPLY_HIT_AUTO_SP_RECOVERY", CItemData::APPLY_HIT_AUTO_SP_RECOVERY);
	PyModule_AddIntConstant(poModule, "APPLY_USE_SKILL_COOLTIME_DECREASE_ALL", CItemData::APPLY_USE_SKILL_COOLTIME_DECREASE_ALL);
	PyModule_AddIntConstant(poModule, "APPLY_HIT_STONE_ATTBONUS_STONE", CItemData::APPLY_HIT_STONE_ATTBONUS_STONE);
	PyModule_AddIntConstant(poModule, "APPLY_HIT_STONE_DEF_GRADE_BONUS", CItemData::APPLY_HIT_STONE_DEF_GRADE_BONUS);
	PyModule_AddIntConstant(poModule, "APPLY_KILL_BOSS_ITEM_BONUS", CItemData::APPLY_KILL_BOSS_ITEM_BONUS);
	PyModule_AddIntConstant(poModule, "APPLY_MOB_HIT_MOB_AGGRESSIVE", CItemData::APPLY_MOB_HIT_MOB_AGGRESSIVE);
	PyModule_AddIntConstant(poModule, "APPLY_NO_DEATH_AND_HP_RECOVERY30", CItemData::APPLY_NO_DEATH_AND_HP_RECOVERY30);
	PyModule_AddIntConstant(poModule, "APPLY_AUTO_PICKUP", CItemData::APPLY_AUTO_PICKUP);
	PyModule_AddIntConstant(poModule, "APPLY_MOUNT_NO_KNOCKBACK", CItemData::APPLY_MOUNT_NO_KNOCKBACK);
	PyModule_AddIntConstant(poModule, "APPLY_SUNGMA_PER_STR", CItemData::APPLY_SUNGMA_PER_STR);
	PyModule_AddIntConstant(poModule, "APPLY_SUNGMA_PER_HP", CItemData::APPLY_SUNGMA_PER_HP);
	PyModule_AddIntConstant(poModule, "APPLY_SUNGMA_PER_MOVE", CItemData::APPLY_SUNGMA_PER_MOVE);
	PyModule_AddIntConstant(poModule, "APPLY_SUNGMA_PER_IMMUNE", CItemData::APPLY_SUNGMA_PER_IMMUNE);
	PyModule_AddIntConstant(poModule, "APPLY_IMMUNE_POISON100", CItemData::APPLY_IMMUNE_POISON100);
	PyModule_AddIntConstant(poModule, "APPLY_IMMUNE_BLEEDING100", CItemData::APPLY_IMMUNE_BLEEDING100);
	PyModule_AddIntConstant(poModule, "APPLY_MONSTER_DEFEND_BONUS", CItemData::APPLY_MONSTER_DEFEND_BONUS);

	PyModule_AddIntConstant(poModule, "ITEM_VNUM_SCROLL_OF_CORRECTION", ITEM_VNUM_SCROLL_OF_CORRECTION);
#if defined(ENABLE_MINI_GAME_RUMI)
	PyModule_AddIntConstant(poModule, "ITEM_VNUM_RUMI_CARD_PIECE", ITEM_VNUM_RUMI_CARD_PIECE);
	PyModule_AddIntConstant(poModule, "ITEM_VNUM_RUMI_CARD_PACK", ITEM_VNUM_RUMI_CARD_PACK);
#endif
#if defined(ENABLE_MINI_GAME_YUTNORI)
	PyModule_AddIntConstant(poModule, "ITEM_VNUM_YUT_PIECE", ITEM_VNUM_YUT_PIECE);
	PyModule_AddIntConstant(poModule, "ITEM_VNUM_YUT_BOARD", ITEM_VNUM_YUT_BOARD);
#endif
#if defined(ENABLE_MINI_GAME_CATCH_KING)
	PyModule_AddIntConstant(poModule, "ITEM_VNUM_CATCH_KING_PIECE", ITEM_VNUM_CATCH_KING_PIECE);
	PyModule_AddIntConstant(poModule, "ITEM_VNUM_CATCH_KING_PACK", ITEM_VNUM_CATCH_KING_PACK);
#endif
#if defined(ENABLE_AURA_COSTUME_SYSTEM)
	PyModule_AddIntConstant(poModule, "ITEM_VNUM_AURA_CLEAR", ITEM_VNUM_AURA_CLEAR);
#endif
#if defined(ENABLE_SNOWFLAKE_STICK_EVENT)
	PyModule_AddIntConstant(poModule, "ITEM_VNUM_SNOWFLAKE_STICK", ITEM_VNUM_SNOWFLAKE_STICK);
	PyModule_AddIntConstant(poModule, "ITEM_VNUM_SNOWFLAKE_STICK_EVENT_MOUNT", ITEM_VNUM_SNOWFLAKE_STICK_EVENT_MOUNT);
	PyModule_AddIntConstant(poModule, "ITEM_VNUM_SNOWFLAKE_STICK_EVENT_PET", ITEM_VNUM_SNOWFLAKE_STICK_EVENT_PET);
#endif
#if defined(ENABLE_GEM_CONVERTER)
	PyModule_AddIntConstant(poModule, "ITEM_VNUM_GEM_CONVERTER", ITEM_VNUM_GEM_CONVERTER);
	PyModule_AddIntConstant(poModule, "ITEM_VNUM_GEM_STONE", ITEM_VNUM_GEM_STONE);
#endif
}
