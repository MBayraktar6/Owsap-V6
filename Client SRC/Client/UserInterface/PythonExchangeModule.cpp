#include "StdAfx.h"
#include "PythonExchange.h"

PyObject* exchangeInitTrading(PyObject* poSelf, PyObject* poArgs)
{
	CPythonExchange::Instance().End();
	return Py_BuildNone();
}

PyObject* exchangeisTrading(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("b", CPythonExchange::Instance().IsTrading());
}

PyObject* exchangeSetElkMode(PyObject* poTarget, PyObject* poArgs)
{
	bool bElkMode;
	if (!PyTuple_GetBoolean(poArgs, 0, &bElkMode))
		return Py_BuildException();

	CPythonExchange::Instance().SetElkMode(bElkMode);
	return Py_BuildNone();
}

PyObject* exchangeGetElkMode(PyObject* poTarget, PyObject* poArgs)
{
	return Py_BuildValue("b", CPythonExchange::Instance().GetElkMode());
}

PyObject* exchangeGetNameFromSelf(PyObject* poTarget, PyObject* poArgs)
{
	return Py_BuildValue("s", CPythonExchange::Instance().GetNameFromSelf());
}

PyObject* exchangeGetNameFromTarget(PyObject* poTarget, PyObject* poArgs)
{
	return Py_BuildValue("s", CPythonExchange::Instance().GetNameFromTarget());
}

PyObject* exchangeGetLevelFromTarget(PyObject* poTarget, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonExchange::Instance().GetLevelFromTarget());
}

PyObject* exchangeGetElkFromSelf(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonExchange::Instance().GetElkFromSelf());
}

PyObject* exchangeGetElkFromTarget(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonExchange::Instance().GetElkFromTarget());
}

#if defined(ENABLE_CHEQUE_SYSTEM)
PyObject* exchangeGetChequeFromSelf(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonExchange::Instance().GetChequeFromSelf());
}

PyObject* exchangeGetChequeFromTarget(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonExchange::Instance().GetChequeFromTarget());
}
#endif

PyObject* exchangeGetItemVnumFromSelf(PyObject* poSelf, PyObject* poArgs)
{
	BYTE bPos;
	if (!PyTuple_GetByte(poArgs, 0, &bPos))
		return Py_BuildException();

	return Py_BuildValue("i", CPythonExchange::Instance().GetItemVnumFromSelf(bPos));
}

PyObject* exchangeGetItemVnumFromTarget(PyObject* poTarget, PyObject* poArgs)
{
	BYTE bPos;
	if (!PyTuple_GetByte(poArgs, 0, &bPos))
		return Py_BuildException();

	return Py_BuildValue("i", CPythonExchange::Instance().GetItemVnumFromTarget(bPos));
}

PyObject* exchangeGetItemCountFromSelf(PyObject* poSelf, PyObject* poArgs)
{
	BYTE bPos;
	if (!PyTuple_GetByte(poArgs, 0, &bPos))
		return Py_BuildException();

	return Py_BuildValue("i", CPythonExchange::Instance().GetItemCountFromSelf(bPos));
}

PyObject* exchangeGetItemCountFromTarget(PyObject* poTarget, PyObject* poArgs)
{
	BYTE bPos;
	if (!PyTuple_GetByte(poArgs, 0, &bPos))
		return Py_BuildException();

	return Py_BuildValue("i", CPythonExchange::Instance().GetItemCountFromTarget(bPos));
}

PyObject* exchangeGetAcceptFromSelf(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonExchange::Instance().GetAcceptFromSelf());
}

PyObject* exchangeGetAcceptFromTarget(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonExchange::Instance().GetAcceptFromTarget());
}

PyObject* exchangeGetItemMetinSocketFromSelf(PyObject* poTarget, PyObject* poArgs)
{
	BYTE bPos;
	if (!PyTuple_GetByte(poArgs, 0, &bPos))
		return Py_BuildException();

	BYTE bSocketPos;
	if (!PyTuple_GetByte(poArgs, 1, &bSocketPos))
		return Py_BuildException();

	return Py_BuildValue("i", CPythonExchange::Instance().GetItemMetinSocketFromSelf(bPos, bSocketPos));
}

PyObject* exchangeGetItemMetinSocketFromTarget(PyObject* poTarget, PyObject* poArgs)
{
	BYTE bPos;
	if (!PyTuple_GetByte(poArgs, 0, &bPos))
		return Py_BuildException();

	BYTE bSocketPos;
	if (!PyTuple_GetByte(poArgs, 1, &bSocketPos))
		return Py_BuildException();

	return Py_BuildValue("i", CPythonExchange::Instance().GetItemMetinSocketFromTarget(bPos, bSocketPos));
}

PyObject* exchangeGetItemAttributeFromSelf(PyObject* poTarget, PyObject* poArgs)
{
	BYTE bPos;
	if (!PyTuple_GetByte(poArgs, 0, &bPos))
		return Py_BuildException();

	BYTE bAttrPos;
	if (!PyTuple_GetByte(poArgs, 1, &bAttrPos))
		return Py_BuildException();

	POINT_TYPE wType; POINT_VALUE lValue;
	CPythonExchange::Instance().GetItemAttributeFromSelf(bPos, bAttrPos, &wType, &lValue);

	return Py_BuildValue("ii", wType, lValue);
}

PyObject* exchangeGetItemAttributeFromTarget(PyObject* poTarget, PyObject* poArgs)
{
	BYTE bPos;
	if (!PyTuple_GetByte(poArgs, 0, &bPos))
		return Py_BuildException();

	BYTE bAttrPos;
	if (!PyTuple_GetByte(poArgs, 1, &bAttrPos))
		return Py_BuildException();

	POINT_TYPE wType; POINT_VALUE lValue;
	CPythonExchange::Instance().GetItemAttributeFromTarget(bPos, bAttrPos, &wType, &lValue);

	return Py_BuildValue("ii", wType, lValue);
}

#if defined(ENABLE_CHANGE_LOOK_SYSTEM)
PyObject* exchangeGetChangeLookVnumFromSelf(PyObject* poSelf, PyObject* poArgs)
{
	BYTE bPos;
	if (!PyTuple_GetByte(poArgs, 0, &bPos))
		return Py_BuildException();

	return Py_BuildValue("i", CPythonExchange::Instance().GetItemChangeLookVnumToSelf(bPos));
}

PyObject* exchangeGetChangeLookVnumFromTarget(PyObject* poSelf, PyObject* poArgs)
{
	BYTE bPos;
	if (!PyTuple_GetByte(poArgs, 0, &bPos))
		return Py_BuildException();

	return Py_BuildValue("i", CPythonExchange::Instance().GetItemChangeLookVnumToTarget(bPos));
}
#endif

#if defined(ENABLE_REFINE_ELEMENT_SYSTEM)
PyObject* exchangeGetItemRefineElementFromSelf(PyObject* poTarget, PyObject* poArgs)
{
	BYTE bPos;
	if (!PyTuple_GetByte(poArgs, 0, &bPos))
		return Py_BuildException();

	const TPlayerItemRefineElement* pTable = CPythonExchange::Instance().GetItemRefineElementFromSelf(bPos);
	if (pTable == nullptr)
		return Py_BuildValue("(iiOO)", 0, 0, Py_BuildValue("iii", 0, 0, 0), Py_BuildValue("iii", 0, 0, 0));

	return Py_BuildValue("(iiOO)", pTable->wApplyType, pTable->bGrade,
		Py_BuildValue("iii", pTable->abValue[0], pTable->abValue[1], pTable->abValue[2]),
		Py_BuildValue("iii", pTable->abBonusValue[0], pTable->abBonusValue[1], pTable->abBonusValue[2]));
}

PyObject* exchangeGetItemRefineElementFromTarget(PyObject* poTarget, PyObject* poArgs)
{
	BYTE bPos;
	if (!PyTuple_GetByte(poArgs, 0, &bPos))
		return Py_BuildException();

	const TPlayerItemRefineElement* pTable = CPythonExchange::Instance().GetItemRefineElementFromTarget(bPos);
	if (pTable == nullptr)
		return Py_BuildValue("(iiOO)", 0, 0, Py_BuildValue("iii", 0, 0, 0), Py_BuildValue("iii", 0, 0, 0));

	return Py_BuildValue("(iiOO)", pTable->wApplyType, pTable->bGrade,
		Py_BuildValue("iii", pTable->abValue[0], pTable->abValue[1], pTable->abValue[2]),
		Py_BuildValue("iii", pTable->abBonusValue[0], pTable->abBonusValue[1], pTable->abBonusValue[2]));
}
#endif

#if defined(ENABLE_APPLY_RANDOM)
PyObject* exchangeGetItemApplyRandomFromSelf(PyObject* poTarget, PyObject* poArgs)
{
	BYTE bPos;
	if (!PyTuple_GetByte(poArgs, 0, &bPos))
		return Py_BuildException();

	BYTE bApplyPos;
	if (!PyTuple_GetByte(poArgs, 1, &bApplyPos))
		return Py_BuildException();

	POINT_TYPE wType; POINT_VALUE lValue;
	CPythonExchange::Instance().GetItemApplyRandomFromSelf(bPos, bApplyPos, &wType, &lValue);

	return Py_BuildValue("ii", wType, lValue);
}

PyObject* exchangeGetItemApplyRandomFromTarget(PyObject* poTarget, PyObject* poArgs)
{
	BYTE bPos;
	if (!PyTuple_GetByte(poArgs, 0, &bPos))
		return Py_BuildException();

	BYTE bApplyPos;
	if (!PyTuple_GetByte(poArgs, 1, &bApplyPos))
		return Py_BuildException();

	POINT_TYPE wType; POINT_VALUE lValue;
	CPythonExchange::Instance().GetItemApplyRandomFromTarget(bPos, bApplyPos, &wType, &lValue);

	return Py_BuildValue("ii", wType, lValue);
}
#endif

#if defined(ENABLE_SET_ITEM)
PyObject* exchangeGetItemSetValueFromSelf(PyObject* poSelf, PyObject* poArgs)
{
	BYTE bPos;
	if (!PyTuple_GetByte(poArgs, 0, &bPos))
		return Py_BuildException();

	return Py_BuildValue("i", CPythonExchange::Instance().GetItemSetValueFromSelf(bPos));
}

PyObject* exchangeGetItemSetValueFromTarget(PyObject* poSelf, PyObject* poArgs)
{
	BYTE bPos;
	if (!PyTuple_GetByte(poArgs, 0, &bPos))
		return Py_BuildException();

	return Py_BuildValue("i", CPythonExchange::Instance().GetItemSetValueFromTarget(bPos));
}
#endif

void initTrade()
{
	static PyMethodDef s_methods[] =
	{
		{ "InitTrading", exchangeInitTrading, METH_VARARGS },
		{ "isTrading", exchangeisTrading, METH_VARARGS },

		{ "SetElkMode", exchangeSetElkMode, METH_VARARGS },
		{ "GetElkMode", exchangeGetElkMode, METH_VARARGS },

		{ "GetNameFromSelf", exchangeGetNameFromSelf, METH_VARARGS },
		{ "GetNameFromTarget", exchangeGetNameFromTarget, METH_VARARGS },
		{ "GetLevelFromTarget", exchangeGetLevelFromTarget, METH_VARARGS },

		{ "GetElkFromSelf", exchangeGetElkFromSelf, METH_VARARGS },
		{ "GetElkFromTarget", exchangeGetElkFromTarget, METH_VARARGS },

#if defined(ENABLE_CHEQUE_SYSTEM)
		{ "GetChequeFromSelf", exchangeGetChequeFromSelf, METH_VARARGS },
		{ "GetChequeFromTarget", exchangeGetChequeFromTarget, METH_VARARGS },
#endif

		{ "GetItemVnumFromSelf", exchangeGetItemVnumFromSelf, METH_VARARGS },
		{ "GetItemVnumFromTarget", exchangeGetItemVnumFromTarget, METH_VARARGS },

		{ "GetItemCountFromSelf", exchangeGetItemCountFromSelf, METH_VARARGS },
		{ "GetItemCountFromTarget", exchangeGetItemCountFromTarget, METH_VARARGS },

		{ "GetAcceptFromSelf", exchangeGetAcceptFromSelf, METH_VARARGS },
		{ "GetAcceptFromTarget", exchangeGetAcceptFromTarget, METH_VARARGS },

		{ "GetItemMetinSocketFromSelf", exchangeGetItemMetinSocketFromSelf, METH_VARARGS },
		{ "GetItemMetinSocketFromTarget", exchangeGetItemMetinSocketFromTarget, METH_VARARGS },

		{ "GetItemAttributeFromSelf", exchangeGetItemAttributeFromSelf, METH_VARARGS },
		{ "GetItemAttributeFromTarget", exchangeGetItemAttributeFromTarget, METH_VARARGS },

#if defined(ENABLE_CHANGE_LOOK_SYSTEM)
		{ "GetChangeLookVnumFromSelf", exchangeGetChangeLookVnumFromSelf, METH_VARARGS },
		{ "GetChangeLookVnumFromTarget", exchangeGetChangeLookVnumFromTarget, METH_VARARGS },
#endif

#if defined(ENABLE_REFINE_ELEMENT_SYSTEM)
		{ "GetItemRefineElementFromSelf", exchangeGetItemRefineElementFromSelf, METH_VARARGS },
		{ "GetItemRefineElementFromTarget", exchangeGetItemRefineElementFromTarget, METH_VARARGS },
#endif

#if defined(ENABLE_APPLY_RANDOM)
		{ "GetItemApplyRandomFromSelf", exchangeGetItemApplyRandomFromSelf, METH_VARARGS },
		{ "GetItemApplyRandomFromTarget", exchangeGetItemApplyRandomFromTarget, METH_VARARGS },
#endif

#if defined(ENABLE_SET_ITEM)
		{ "GetItemSetValueFromSelf", exchangeGetItemSetValueFromSelf, METH_VARARGS },
		{ "GetItemSetValueFromTarget", exchangeGetItemSetValueFromTarget, METH_VARARGS },
#endif

		{ NULL, NULL },
	};

	PyObject* poModule = Py_InitModule("exchange", s_methods);
	PyModule_AddIntConstant(poModule, "EXCHANGE_ITEM_MAX_NUM", CPythonExchange::EXCHANGE_ITEM_MAX_NUM);
}
