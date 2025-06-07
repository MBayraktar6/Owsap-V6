/**
* Filename: PythonMailBox.cpp
* Author: blackdragonx61 (aka. Mali)
**/

#include "StdAfx.h"
#if defined(ENABLE_MAILBOX)
#include "PythonMailBox.h"

CPythonMailBox::CPythonMailBox()
{
}

CPythonMailBox::~CPythonMailBox()
{
	Destroy();
}

void CPythonMailBox::Destroy()
{
	if (vecMail.empty())
		return;

	for (SMailBox* mail : vecMail)
		delete mail;

	vecMail.clear();
}

const CPythonMailBox::MailVec& CPythonMailBox::GetMailVec() const
{
	return vecMail;
}

void CPythonMailBox::ResetAddData(const BYTE Index)
{
	CPythonMailBox::SMailBox* mail = GetMail(Index);
	if (mail == nullptr)
		return;

	mail->bIsConfirm = true;
	mail->ResetAddData();
}

void CPythonMailBox::AddMail(CPythonMailBox::SMailBox* mail)
{
	vecMail.emplace_back(mail);
}

// SMailBox:

CPythonMailBox::SMailBox::SMailBox(const time_t _SendTime, const time_t _DeleteTime, const char* _Title,
	const bool _IsGM, const bool _IsItem, const bool _IsConfirm) :
	Sendtime(_SendTime),
	Deletetime(_DeleteTime),
	sTitle(_Title),
	bIsGMPost(_IsGM),
	bIsItemExist(_IsItem),
	bIsConfirm(_IsConfirm),
	AddData(nullptr)
{
}

CPythonMailBox::SMailBox::~SMailBox()
{
	ResetAddData();
}

void CPythonMailBox::SMailBox::ResetAddData()
{
	if (AddData == nullptr)
		return;

	delete AddData;
	AddData = nullptr;
}

CPythonMailBox::SMailBox* CPythonMailBox::GetMail(const BYTE Index)
{
	const MailVec& mail = GetMailVec();
	if (Index >= mail.size())
		return nullptr;

	return mail.at(Index);
}

// SMailBoxAddData:

CPythonMailBox::SMailBoxAddData::SMailBoxAddData(const char* _From, const char* _Message, const int _Yang, const int _Won,
	const DWORD _ItemVnum, const DWORD _ItemCount, const long* _Sockets, const TPlayerItemAttribute* _Attrs
#if defined(ENABLE_CHANGE_LOOK_SYSTEM)
	, const DWORD _ChangeLookVnum
#endif
#if defined(ENABLE_REFINE_ELEMENT_SYSTEM)
	, const TPlayerItemRefineElement* _RefineElement
#endif
#if defined(ENABLE_APPLY_RANDOM)
	, const TPlayerItemAttribute* _ApplyRandom
#endif
#if defined(ENABLE_SET_ITEM)
	, const BYTE _SetValue
#endif
) :
	sFrom(_From),
	sMessage(_Message),
	iYang(_Yang),
	iWon(_Won),
	ItemVnum(_ItemVnum),
	ItemCount(_ItemCount)
#if defined(ENABLE_CHANGE_LOOK_SYSTEM)
	, ChangeLookVnum(_ChangeLookVnum)
#endif
#if defined(ENABLE_SET_ITEM)
	, SetValue(_SetValue)
#endif
{
	std::memcpy(alSockets, _Sockets, sizeof(alSockets));
	std::memcpy(aAttr, _Attrs, sizeof(aAttr));
#if defined(ENABLE_REFINE_ELEMENT_SYSTEM)
	std::memcpy(&RefineElement, _RefineElement, sizeof(RefineElement));
#endif
#if defined(ENABLE_APPLY_RANDOM)
	std::memcpy(aApplyRandom, _ApplyRandom, sizeof(aApplyRandom));
#endif
}

CPythonMailBox::SMailBoxAddData::~SMailBoxAddData()
{
}

CPythonMailBox::SMailBoxAddData* CPythonMailBox::GetMailAddData(const BYTE Index)
{
	const CPythonMailBox::SMailBox* mail = GetMail(Index);
	if (mail == nullptr)
		return nullptr;

	return mail->AddData;
}

// Methods
PyObject* mailGetMailAddData(PyObject* poSelf, PyObject* poArgs)
{
	// (from_name, message, yang, cheque)

	BYTE Index;
	if (!PyTuple_GetByte(poArgs, 0, &Index))
		return Py_BadArgument();

	const CPythonMailBox::SMailBoxAddData* addData = CPythonMailBox::Instance().GetMailAddData(Index);
	if (addData == nullptr)
		return Py_BuildNone();

	return Py_BuildValue("ssii", addData->sFrom.c_str(), addData->sMessage.c_str(), addData->iYang, addData->iWon);
}

PyObject* mailGetMailData(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildNone();
}

PyObject* mailGetMailDict(PyObject* poSelf, PyObject* poArgs)
{
	// (index, send_time, delete_time, title, is_gm_post, is_item_exist, is_confirm)

	PyObject* dict = PyDict_New();

	BYTE idx = 0;
	for (CPythonMailBox::SMailBox* mail : CPythonMailBox::Instance().GetMailVec())
	{
		PyDict_SetItem(dict, Py_BuildValue("i", idx), Py_BuildValue("illsiii", idx, mail->Sendtime, mail->Deletetime, mail->sTitle.c_str(),
			mail->bIsGMPost, mail->bIsItemExist, mail->bIsConfirm));

		idx++;
	}

	return dict;
}

PyObject* mailGetMailItemData(PyObject* poSelf, PyObject* poArgs)
{
	BYTE Index;
	if (!PyTuple_GetByte(poArgs, 0, &Index))
		return Py_BadArgument();

	const CPythonMailBox::SMailBoxAddData* addData = CPythonMailBox::Instance().GetMailAddData(Index);
	if (addData == nullptr || addData->ItemVnum == 0)
		return Py_BuildNone();

	return Py_BuildValue("ii", addData->ItemVnum, addData->ItemCount);
}

PyObject* mailGetMailItemAttribute(PyObject* poSelf, PyObject* poArgs)
{
	BYTE Index;
	if (!PyTuple_GetByte(poArgs, 0, &Index))
		return Py_BadArgument();
	BYTE AttrSlotIndex;
	if (!PyTuple_GetByte(poArgs, 1, &AttrSlotIndex))
		return Py_BadArgument();

	if ((AttrSlotIndex >= 0 && AttrSlotIndex < ITEM_ATTRIBUTE_SLOT_MAX_NUM) == false)
		return Py_BuildException();

	const CPythonMailBox::SMailBoxAddData* addData = CPythonMailBox::Instance().GetMailAddData(Index);
	if (addData == nullptr || addData->ItemVnum == 0)
		return Py_BuildNone();

	return Py_BuildValue("ii", addData->aAttr[AttrSlotIndex].wType, addData->aAttr[AttrSlotIndex].lValue);
}

PyObject* mailGetMailItemMetinSocket(PyObject* poSelf, PyObject* poArgs)
{
	BYTE Index;
	if (!PyTuple_GetByte(poArgs, 0, &Index))
		return Py_BadArgument();
	BYTE SocketIndex;
	if (!PyTuple_GetByte(poArgs, 1, &SocketIndex))
		return Py_BadArgument();

	if (SocketIndex >= ITEM_SOCKET_SLOT_MAX_NUM)
		return Py_BuildException();

	const CPythonMailBox::SMailBoxAddData* addData = CPythonMailBox::Instance().GetMailAddData(Index);
	if (addData == nullptr || addData->ItemVnum == 0)
		return Py_BuildNone();

	return Py_BuildValue("i", addData->alSockets[SocketIndex]);
}

#if defined(ENABLE_CHANGE_LOOK_SYSTEM)
PyObject* mailGetItemChangeLookVnum(PyObject* poSelf, PyObject* poArgs)
{
	BYTE Index;
	if (!PyTuple_GetByte(poArgs, 0, &Index))
		return Py_BadArgument();

	const CPythonMailBox::SMailBoxAddData* addData = CPythonMailBox::Instance().GetMailAddData(Index);
	if (addData == nullptr || addData->ItemVnum == 0)
		return Py_BuildNone();

	return Py_BuildValue("i", addData->ChangeLookVnum);
}
#endif

#if defined(ENABLE_REFINE_ELEMENT_SYSTEM)
PyObject* mailGetItemRefineElement(PyObject* poSelf, PyObject* poArgs)
{
	BYTE Index;
	if (!PyTuple_GetByte(poArgs, 0, &Index))
		return Py_BadArgument();

	const CPythonMailBox::SMailBoxAddData* addData = CPythonMailBox::Instance().GetMailAddData(Index);
	if (addData == nullptr || addData->ItemVnum == 0)
		return Py_BuildValue("(iiOO)", 0, 0, Py_BuildValue("iii", 0, 0, 0), Py_BuildValue("iii", 0, 0, 0));

	const TPlayerItemRefineElement& kTable = addData->RefineElement;
	return Py_BuildValue("(iiOO)", kTable.wApplyType, kTable.bGrade,
		Py_BuildValue("iii", kTable.abValue[0], kTable.abValue[1], kTable.abValue[2]),
		Py_BuildValue("iii", kTable.abBonusValue[0], kTable.abBonusValue[1], kTable.abBonusValue[2]));
}
#endif

#if defined(ENABLE_APPLY_RANDOM)
PyObject* mailGetItemApplyRandom(PyObject* poSelf, PyObject* poArgs)
{
	BYTE Index;
	if (!PyTuple_GetByte(poArgs, 0, &Index))
		return Py_BadArgument();

	BYTE ApplySlotIndex;
	if (!PyTuple_GetByte(poArgs, 1, &ApplySlotIndex))
		return Py_BadArgument();

	if ((ApplySlotIndex >= 0 && ApplySlotIndex < ITEM_APPLY_RANDOM_SLOT_MAX_NUM) == false)
		return Py_BuildException();

	const CPythonMailBox::SMailBoxAddData* addData = CPythonMailBox::Instance().GetMailAddData(Index);
	if (addData == nullptr || addData->ItemVnum == 0)
		return Py_BuildNone();

	return Py_BuildValue("ii", addData->aApplyRandom[ApplySlotIndex].wType, addData->aApplyRandom[ApplySlotIndex].lValue);
}
#endif

#if defined(ENABLE_SET_ITEM)
PyObject* mailGetItemSetValue(PyObject* poSelf, PyObject* poArgs)
{
	BYTE bIndex;
	if (!PyTuple_GetByte(poArgs, 0, &bIndex))
		return Py_BadArgument();

	const CPythonMailBox::SMailBoxAddData* addData = CPythonMailBox::Instance().GetMailAddData(bIndex);
	if (addData == nullptr || addData->ItemVnum == 0)
		return Py_BuildNone();

	return Py_BuildValue("i", addData->SetValue);
}
#endif

void initmail()
{
	static PyMethodDef s_methods[] =
	{
		{ "GetMailAddData", mailGetMailAddData, METH_VARARGS },
		{ "GetMailData", mailGetMailData, METH_VARARGS },
		{ "GetMailDict", mailGetMailDict, METH_VARARGS },
		{ "GetMailItemAttribute", mailGetMailItemAttribute, METH_VARARGS },
		{ "GetMailItemData", mailGetMailItemData, METH_VARARGS },
		{ "GetMailItemMetinSocket", mailGetMailItemMetinSocket, METH_VARARGS },
#if defined(ENABLE_CHANGE_LOOK_SYSTEM)
		{ "GetItemChangeLookVnum", mailGetItemChangeLookVnum, METH_VARARGS },
#endif
#if defined(ENABLE_REFINE_ELEMENT_SYSTEM)
		{ "GetItemRefineElement", mailGetItemRefineElement, METH_VARARGS },
#endif
#if defined(ENABLE_APPLY_RANDOM)
		{ "GetItemApplyRandom", mailGetItemApplyRandom, METH_VARARGS },
#endif
#if defined(ENABLE_SET_ITEM)
		{ "GetItemSetValue", mailGetItemSetValue, METH_VARARGS },
#endif
		{ NULL, NULL, NULL },
	};

	PyObject* poModule = Py_InitModule("mail", s_methods);

	/* GC */
	PyModule_AddIntConstant(poModule, "MAILBOX_GC_OPEN", CPythonMailBox::EMAILBOX_GC::MAILBOX_GC_OPEN);
	PyModule_AddIntConstant(poModule, "MAILBOX_GC_POST_WRITE_CONFIRM", CPythonMailBox::EMAILBOX_GC::MAILBOX_GC_POST_WRITE_CONFIRM);
	PyModule_AddIntConstant(poModule, "MAILBOX_GC_POST_WRITE", CPythonMailBox::EMAILBOX_GC::MAILBOX_GC_POST_WRITE);
	PyModule_AddIntConstant(poModule, "MAILBOX_GC_SET", CPythonMailBox::EMAILBOX_GC::MAILBOX_GC_SET);
	PyModule_AddIntConstant(poModule, "MAILBOX_GC_ADD_DATA", CPythonMailBox::EMAILBOX_GC::MAILBOX_GC_ADD_DATA);
	PyModule_AddIntConstant(poModule, "MAILBOX_GC_POST_GET_ITEMS", CPythonMailBox::EMAILBOX_GC::MAILBOX_GC_POST_GET_ITEMS);
	PyModule_AddIntConstant(poModule, "MAILBOX_GC_POST_DELETE", CPythonMailBox::EMAILBOX_GC::MAILBOX_GC_POST_DELETE);
	PyModule_AddIntConstant(poModule, "MAILBOX_GC_POST_ALL_DELETE", CPythonMailBox::EMAILBOX_GC::MAILBOX_GC_POST_ALL_DELETE);
	PyModule_AddIntConstant(poModule, "MAILBOX_GC_POST_ALL_GET_ITEMS", CPythonMailBox::EMAILBOX_GC::MAILBOX_GC_POST_ALL_GET_ITEMS);
	PyModule_AddIntConstant(poModule, "MAILBOX_GC_UNREAD_DATA", CPythonMailBox::EMAILBOX_GC::MAILBOX_GC_UNREAD_DATA);
	PyModule_AddIntConstant(poModule, "MAILBOX_GC_ITEM_EXPIRE", CPythonMailBox::EMAILBOX_GC::MAILBOX_GC_ITEM_EXPIRE);
	PyModule_AddIntConstant(poModule, "MAILBOX_GC_CLOSE", CPythonMailBox::EMAILBOX_GC::MAILBOX_GC_CLOSE);
	PyModule_AddIntConstant(poModule, "MAILBOX_GC_SYSTEM_CLOSE", CPythonMailBox::EMAILBOX_GC::MAILBOX_GC_SYSTEM_CLOSE);
	/* GC END */

	/* POST_ALL_DELETE */
	PyModule_AddIntConstant(poModule, "POST_ALL_DELETE_FAIL", CPythonMailBox::EMAILBOX_POST_ALL_DELETE::POST_ALL_DELETE_FAIL);
	PyModule_AddIntConstant(poModule, "POST_ALL_DELETE_OK", CPythonMailBox::EMAILBOX_POST_ALL_DELETE::POST_ALL_DELETE_OK);
	PyModule_AddIntConstant(poModule, "POST_ALL_DELETE_FAIL_EMPTY", CPythonMailBox::EMAILBOX_POST_ALL_DELETE::POST_ALL_DELETE_FAIL_EMPTY);
	PyModule_AddIntConstant(poModule, "POST_ALL_DELETE_FAIL_DONT_EXIST", CPythonMailBox::EMAILBOX_POST_ALL_DELETE::POST_ALL_DELETE_FAIL_DONT_EXIST);
	/* POST_ALL_DELETE END */

	/* POST_ALL_GET_ITEMS */
	PyModule_AddIntConstant(poModule, "POST_ALL_GET_ITEMS_FAIL", CPythonMailBox::EMAILBOX_POST_ALL_GET_ITEMS::POST_ALL_GET_ITEMS_FAIL);
	PyModule_AddIntConstant(poModule, "POST_ALL_GET_ITEMS_OK", CPythonMailBox::EMAILBOX_POST_ALL_GET_ITEMS::POST_ALL_GET_ITEMS_OK);
	PyModule_AddIntConstant(poModule, "POST_ALL_GET_ITEMS_EMPTY", CPythonMailBox::EMAILBOX_POST_ALL_GET_ITEMS::POST_ALL_GET_ITEMS_EMPTY);
	PyModule_AddIntConstant(poModule, "POST_ALL_GET_ITEMS_FAIL_DONT_EXIST", CPythonMailBox::EMAILBOX_POST_ALL_GET_ITEMS::POST_ALL_GET_ITEMS_FAIL_DONT_EXIST);
	PyModule_AddIntConstant(poModule, "POST_ALL_GET_ITEMS_FAIL_CANT_GET", CPythonMailBox::EMAILBOX_POST_ALL_GET_ITEMS::POST_ALL_GET_ITEMS_FAIL_CANT_GET);
	PyModule_AddIntConstant(poModule, "POST_ALL_GET_ITEMS_FAIL_USE_TIME_LIMIT", CPythonMailBox::EMAILBOX_POST_ALL_GET_ITEMS::POST_ALL_GET_ITEMS_FAIL_USE_TIME_LIMIT);
	/* POST_ALL_GET_ITEMS END */

	/* POST_DELETE_FAIL */
	PyModule_AddIntConstant(poModule, "POST_DELETE_FAIL", CPythonMailBox::EMAILBOX_POST_DELETE_FAIL::POST_DELETE_FAIL);
	PyModule_AddIntConstant(poModule, "POST_DELETE_OK", CPythonMailBox::EMAILBOX_POST_DELETE_FAIL::POST_DELETE_OK);
	PyModule_AddIntConstant(poModule, "POST_DELETE_FAIL_EXIST_ITEMS", CPythonMailBox::EMAILBOX_POST_DELETE_FAIL::POST_DELETE_FAIL_EXIST_ITEMS);
	PyModule_AddIntConstant(poModule, "POST_TIME_OUT_DELETE", CPythonMailBox::EMAILBOX_POST_DELETE_FAIL::POST_TIME_OUT_DELETE);
	/* POST_DELETE_FAIL END */

	/* POST_GET_ITEMS */
	PyModule_AddIntConstant(poModule, "POST_GET_ITEMS_FAIL", CPythonMailBox::EMAILBOX_POST_GET_ITEMS::POST_GET_ITEMS_FAIL);
	PyModule_AddIntConstant(poModule, "POST_GET_ITEMS_OK", CPythonMailBox::EMAILBOX_POST_GET_ITEMS::POST_GET_ITEMS_OK);
	PyModule_AddIntConstant(poModule, "POST_GET_ITEMS_NONE", CPythonMailBox::EMAILBOX_POST_GET_ITEMS::POST_GET_ITEMS_NONE);
	PyModule_AddIntConstant(poModule, "POST_GET_ITEMS_NOT_ENOUGHT_INVENTORY", CPythonMailBox::EMAILBOX_POST_GET_ITEMS::POST_GET_ITEMS_NOT_ENOUGHT_INVENTORY);
	PyModule_AddIntConstant(poModule, "POST_GET_ITEMS_YANG_OVERFLOW", CPythonMailBox::EMAILBOX_POST_GET_ITEMS::POST_GET_ITEMS_YANG_OVERFLOW);
	PyModule_AddIntConstant(poModule, "POST_GET_ITEMS_WON_OVERFLOW", CPythonMailBox::EMAILBOX_POST_GET_ITEMS::POST_GET_ITEMS_WON_OVERFLOW);
	PyModule_AddIntConstant(poModule, "POST_GET_ITEMS_FAIL_BLOCK_CHAR", CPythonMailBox::EMAILBOX_POST_GET_ITEMS::POST_GET_ITEMS_FAIL_BLOCK_CHAR);
	PyModule_AddIntConstant(poModule, "POST_GET_ITEMS_USE_TIME_LIMIT", CPythonMailBox::EMAILBOX_POST_GET_ITEMS::POST_GET_ITEMS_USE_TIME_LIMIT);
	PyModule_AddIntConstant(poModule, "POST_GET_ITEMS_RESULT_MAX", CPythonMailBox::EMAILBOX_POST_GET_ITEMS::POST_GET_ITEMS_RESULT_MAX);
	/* POST_GET_ITEMS END */

	/* POST_WRITE */
	PyModule_AddIntConstant(poModule, "POST_WRITE_FAIL", CPythonMailBox::EMAILBOX_POST_WRITE::POST_WRITE_FAIL);
	PyModule_AddIntConstant(poModule, "POST_WRITE_OK", CPythonMailBox::EMAILBOX_POST_WRITE::POST_WRITE_OK);
	PyModule_AddIntConstant(poModule, "POST_WRITE_INVALID_NAME", CPythonMailBox::EMAILBOX_POST_WRITE::POST_WRITE_INVALID_NAME);
	PyModule_AddIntConstant(poModule, "POST_WRITE_TARGET_BLOCKED", CPythonMailBox::EMAILBOX_POST_WRITE::POST_WRITE_TARGET_BLOCKED);
	PyModule_AddIntConstant(poModule, "POST_WRITE_BLOCKED_ME", CPythonMailBox::EMAILBOX_POST_WRITE::POST_WRITE_BLOCKED_ME);
	PyModule_AddIntConstant(poModule, "POST_WRITE_FULL_MAILBOX", CPythonMailBox::EMAILBOX_POST_WRITE::POST_WRITE_FULL_MAILBOX);
	PyModule_AddIntConstant(poModule, "POST_WRITE_WRONG_TITLE", CPythonMailBox::EMAILBOX_POST_WRITE::POST_WRITE_WRONG_TITLE);
	PyModule_AddIntConstant(poModule, "POST_WRITE_YANG_NOT_ENOUGHT", CPythonMailBox::EMAILBOX_POST_WRITE::POST_WRITE_YANG_NOT_ENOUGHT);
	PyModule_AddIntConstant(poModule, "POST_WRITE_WON_NOT_ENOUGHT", CPythonMailBox::EMAILBOX_POST_WRITE::POST_WRITE_WON_NOT_ENOUGHT);
	PyModule_AddIntConstant(poModule, "POST_WRITE_WRONG_MESSAGE", CPythonMailBox::EMAILBOX_POST_WRITE::POST_WRITE_WRONG_MESSAGE);
	PyModule_AddIntConstant(poModule, "POST_WRITE_WRONG_ITEM", CPythonMailBox::EMAILBOX_POST_WRITE::POST_WRITE_WRONG_ITEM);
	PyModule_AddIntConstant(poModule, "POST_WRITE_LEVEL_NOT_ENOUGHT", CPythonMailBox::EMAILBOX_POST_WRITE::POST_WRITE_LEVEL_NOT_ENOUGHT);
	PyModule_AddIntConstant(poModule, "POST_WRITE_USE_TIME_LIMIT", CPythonMailBox::EMAILBOX_POST_WRITE::POST_WRITE_USE_TIME_LIMIT);
	/* POST_WRITE END */
}
#endif
