#include "StdAfx.h"
#include "PythonExchange.h"

CPythonExchange::CPythonExchange()
{
	Clear();
	m_bIsTrading = false;
	m_bElkMode = false;
	// Clear로 옴겨놓으면 안됨. 
	// trade_start 페킷이 오면 Clear를 실행하는데
	// m_bElkMode는 클리어 되선 안됨.;  
}

CPythonExchange::~CPythonExchange() = default;

void CPythonExchange::Clear()
{
	std::memset(&m_kSelf, 0, sizeof(m_kSelf));
	std::memset(&m_kVictim, 0, sizeof(m_kVictim));
}

void CPythonExchange::Start()
{
	m_bIsTrading = true;
}

void CPythonExchange::End()
{
	m_bIsTrading = false;
}

bool CPythonExchange::IsTrading()
{
	return m_bIsTrading;
}

// Interface
void CPythonExchange::SetAcceptToSelf(BYTE bAccept)
{
	m_kSelf.bAccept = bAccept ? true : false;
}

bool CPythonExchange::GetAcceptFromSelf()
{
	return m_kSelf.bAccept ? true : false;
}

void CPythonExchange::SetAcceptToTarget(BYTE bAccept)
{
	m_kVictim.bAccept = bAccept ? true : false;
}

bool CPythonExchange::GetAcceptFromTarget()
{
	return m_kVictim.bAccept ? true : false;
}

void CPythonExchange::SetElkMode(bool bValue)
{
	m_bElkMode = bValue;
}

bool CPythonExchange::GetElkMode()
{
	return m_bElkMode;
}

void CPythonExchange::SetSelfName(const char* szName)
{
	strncpy(m_kSelf.szName, szName, CHARACTER_NAME_MAX_LEN);
}

const char* CPythonExchange::GetNameFromSelf()
{
	return m_kSelf.szName;
}

void CPythonExchange::SetTargetName(const char* szName)
{
	strncpy(m_kVictim.szName, szName, CHARACTER_NAME_MAX_LEN);
}

const char* CPythonExchange::GetNameFromTarget()
{
	return m_kVictim.szName;
}

void CPythonExchange::SetLevelToTarget(BYTE bLevel)
{
	m_kVictim.bLevel = bLevel;
}

BYTE CPythonExchange::GetLevelFromTarget() const
{
	return m_kVictim.bLevel;
}

void CPythonExchange::SetElkToSelf(DWORD dwElk)
{
	m_kSelf.dwElk = dwElk;
}

DWORD CPythonExchange::GetElkFromSelf() const
{
	return m_kSelf.dwElk;
}

void CPythonExchange::SetElkToTarget(DWORD dwElk)
{
	m_kVictim.dwElk = dwElk;
}

DWORD CPythonExchange::GetElkFromTarget() const
{
	return m_kVictim.dwElk;
}

#if defined(ENABLE_CHEQUE_SYSTEM)
void CPythonExchange::SetChequeToSelf(DWORD dwCheque)
{
	m_kSelf.dwCheque = dwCheque;
}

DWORD CPythonExchange::GetChequeFromSelf() const
{
	return m_kSelf.dwCheque;
}

void CPythonExchange::SetChequeToTarget(DWORD dwCheque)
{
	m_kVictim.dwCheque = dwCheque;
}

DWORD CPythonExchange::GetChequeFromTarget() const
{
	return m_kVictim.dwCheque;
}
#endif

void CPythonExchange::SetItemToSelf(BYTE bPos, DWORD dwVnum, WORD wCount)
{
	if (bPos >= EXCHANGE_ITEM_MAX_NUM)
		return;

	m_kSelf.dwItemVnum[bPos] = dwVnum;
	m_kSelf.wItemCount[bPos] = wCount;
}

void CPythonExchange::SetItemToTarget(BYTE bPos, DWORD dwVnum, WORD wCount)
{
	if (bPos >= EXCHANGE_ITEM_MAX_NUM)
		return;

	m_kVictim.dwItemVnum[bPos] = dwVnum;
	m_kVictim.wItemCount[bPos] = wCount;
}

void CPythonExchange::DelItemOfSelf(BYTE bPos)
{
	if (bPos >= EXCHANGE_ITEM_MAX_NUM)
		return;

	m_kSelf.dwItemVnum[bPos] = 0;
	m_kSelf.wItemCount[bPos] = 0;
}

void CPythonExchange::DelItemOfTarget(BYTE bPos)
{
	if (bPos >= EXCHANGE_ITEM_MAX_NUM)
		return;

	m_kVictim.dwItemVnum[bPos] = 0;
	m_kVictim.wItemCount[bPos] = 0;
}

DWORD CPythonExchange::GetItemVnumFromSelf(BYTE bPos) const
{
	if (bPos >= EXCHANGE_ITEM_MAX_NUM)
		return 0;

	return m_kSelf.dwItemVnum[bPos];
}

DWORD CPythonExchange::GetItemVnumFromTarget(BYTE bPos) const
{
	if (bPos >= EXCHANGE_ITEM_MAX_NUM)
		return 0;

	return m_kVictim.dwItemVnum[bPos];
}

WORD CPythonExchange::GetItemCountFromSelf(BYTE bPos) const
{
	if (bPos >= EXCHANGE_ITEM_MAX_NUM)
		return 0;

	return m_kSelf.wItemCount[bPos];
}

WORD CPythonExchange::GetItemCountFromTarget(BYTE bPos) const
{
	if (bPos >= EXCHANGE_ITEM_MAX_NUM)
		return 0;

	return m_kVictim.wItemCount[bPos];
}

void CPythonExchange::SetItemMetinSocketToSelf(BYTE bPos, BYTE bSocketPos, long lValue)
{
	if (bPos >= EXCHANGE_ITEM_MAX_NUM)
		return;

	m_kSelf.aItemSocket[bPos][bSocketPos] = lValue;
}

long CPythonExchange::GetItemMetinSocketFromSelf(BYTE bPos, BYTE bSocketPos) const
{
	if (bPos >= EXCHANGE_ITEM_MAX_NUM)
		return 0;

	return m_kSelf.aItemSocket[bPos][bSocketPos];
}

void CPythonExchange::SetItemMetinSocketToTarget(BYTE bPos, BYTE bSocketPos, long lValue)
{
	if (bPos >= EXCHANGE_ITEM_MAX_NUM)
		return;

	m_kVictim.aItemSocket[bPos][bSocketPos] = lValue;
}

long CPythonExchange::GetItemMetinSocketFromTarget(BYTE bPos, BYTE bSocketPos) const
{
	if (bPos >= EXCHANGE_ITEM_MAX_NUM)
		return 0;

	return m_kVictim.aItemSocket[bPos][bSocketPos];
}

void CPythonExchange::SetItemAttributeToSelf(BYTE bPos, BYTE bAttrPos, POINT_TYPE wType, POINT_VALUE lValue)
{
	if (bPos >= EXCHANGE_ITEM_MAX_NUM)
		return;

	m_kSelf.aItemAttr[bPos][bAttrPos].wType = wType;
	m_kSelf.aItemAttr[bPos][bAttrPos].lValue = lValue;
}

void CPythonExchange::GetItemAttributeFromSelf(BYTE bPos, BYTE bAttrPos, POINT_TYPE* pwType, POINT_VALUE* plValue)
{
	if (bPos >= EXCHANGE_ITEM_MAX_NUM)
		return;

	*pwType = m_kSelf.aItemAttr[bPos][bAttrPos].wType;
	*plValue = m_kSelf.aItemAttr[bPos][bAttrPos].lValue;
}

void CPythonExchange::SetItemAttributeToTarget(BYTE bPos, BYTE bAttrPos, POINT_TYPE wType, POINT_VALUE lValue)
{
	if (bPos >= EXCHANGE_ITEM_MAX_NUM)
		return;

	m_kVictim.aItemAttr[bPos][bAttrPos].wType = wType;
	m_kVictim.aItemAttr[bPos][bAttrPos].lValue = lValue;
}

void CPythonExchange::GetItemAttributeFromTarget(BYTE bPos, BYTE bAttrPos, POINT_TYPE* pwType, POINT_VALUE* plValue)
{
	if (bPos >= EXCHANGE_ITEM_MAX_NUM)
		return;

	*pwType = m_kVictim.aItemAttr[bPos][bAttrPos].wType;
	*plValue = m_kVictim.aItemAttr[bPos][bAttrPos].lValue;
}

#if defined(ENABLE_CHANGE_LOOK_SYSTEM)
void CPythonExchange::SetItemChangeLookVnumToSelf(BYTE bPos, DWORD dwVnum)
{
	if (bPos >= EXCHANGE_ITEM_MAX_NUM)
		return;

	m_kSelf.dwTransmutationVnum[bPos] = dwVnum;
}

DWORD CPythonExchange::GetItemChangeLookVnumToSelf(BYTE bPos) const
{
	if (bPos >= EXCHANGE_ITEM_MAX_NUM)
		return 0;

	return m_kSelf.dwTransmutationVnum[bPos];
}

void CPythonExchange::SetItemChangeLookVnumToTarget(BYTE bPos, DWORD dwVnum)
{
	if (bPos >= EXCHANGE_ITEM_MAX_NUM)
		return;

	m_kVictim.dwTransmutationVnum[bPos] = dwVnum;
}

DWORD CPythonExchange::GetItemChangeLookVnumToTarget(BYTE bPos) const
{
	if (bPos >= EXCHANGE_ITEM_MAX_NUM)
		return 0;

	return m_kVictim.dwTransmutationVnum[bPos];
}
#endif

#if defined(ENABLE_REFINE_ELEMENT_SYSTEM)
void CPythonExchange::SetItemRefineElementToSelf(BYTE bPos, const TPlayerItemRefineElement& rkTable)
{
	if (bPos >= EXCHANGE_ITEM_MAX_NUM)
		return;

	m_kSelf.sItemRefineElement[bPos] = rkTable;
}

const TPlayerItemRefineElement* const CPythonExchange::GetItemRefineElementFromSelf(BYTE bPos)
{
	if (bPos >= EXCHANGE_ITEM_MAX_NUM)
		return nullptr;

	return &m_kSelf.sItemRefineElement[bPos];
}

void CPythonExchange::SetItemRefineElementToTarget(BYTE bPos, const TPlayerItemRefineElement& rkTable)
{
	if (bPos >= EXCHANGE_ITEM_MAX_NUM)
		return;

	m_kVictim.sItemRefineElement[bPos] = rkTable;
}

const TPlayerItemRefineElement* const CPythonExchange::GetItemRefineElementFromTarget(BYTE bPos)
{
	if (bPos >= EXCHANGE_ITEM_MAX_NUM)
		return nullptr;

	return &m_kVictim.sItemRefineElement[bPos];
}
#endif

#if defined(ENABLE_APPLY_RANDOM)
void CPythonExchange::SetItemApplyRandomToSelf(BYTE bPos, BYTE bApplyPos, POINT_TYPE wType, POINT_VALUE lValue)
{
	if (bPos >= EXCHANGE_ITEM_MAX_NUM)
		return;

	m_kSelf.aItemApplyRandom[bPos][bApplyPos].wType = wType;
	m_kSelf.aItemApplyRandom[bPos][bApplyPos].lValue = lValue;
}

void CPythonExchange::GetItemApplyRandomFromSelf(BYTE bPos, BYTE bApplyPos, POINT_TYPE* pwType, POINT_VALUE* plValue)
{
	if (bPos >= EXCHANGE_ITEM_MAX_NUM)
		return;

	*pwType = m_kSelf.aItemApplyRandom[bPos][bApplyPos].wType;
	*plValue = m_kSelf.aItemApplyRandom[bPos][bApplyPos].lValue;
}

void CPythonExchange::SetItemApplyRandomToTarget(BYTE bPos, BYTE bApplyPos, POINT_TYPE wType, POINT_VALUE lValue)
{
	if (bPos >= EXCHANGE_ITEM_MAX_NUM)
		return;

	m_kVictim.aItemApplyRandom[bPos][bApplyPos].wType = wType;
	m_kVictim.aItemApplyRandom[bPos][bApplyPos].lValue = lValue;
}

void CPythonExchange::GetItemApplyRandomFromTarget(BYTE bPos, BYTE bApplyPos, POINT_TYPE* pwType, POINT_VALUE* plValue)
{
	if (bPos >= EXCHANGE_ITEM_MAX_NUM)
		return;

	*pwType = m_kVictim.aItemApplyRandom[bPos][bApplyPos].wType;
	*plValue = m_kVictim.aItemApplyRandom[bPos][bApplyPos].lValue;
}
#endif

#if defined(ENABLE_SET_ITEM)
void CPythonExchange::SetItemSetValueToSelf(BYTE bPos, BYTE bSetValue)
{
	if (bPos >= EXCHANGE_ITEM_MAX_NUM)
		return;

	m_kSelf.bSetValue[bPos] = bSetValue;
}

BYTE CPythonExchange::GetItemSetValueFromSelf(BYTE bPos) const
{
	if (bPos >= EXCHANGE_ITEM_MAX_NUM)
		return 0;

	return m_kSelf.bSetValue[bPos];
}

void CPythonExchange::SetItemSetValueToTarget(BYTE bPos, BYTE bSetValue)
{
	if (bPos >= EXCHANGE_ITEM_MAX_NUM)
		return;

	m_kVictim.bSetValue[bPos] = bSetValue;
}

BYTE CPythonExchange::GetItemSetValueFromTarget(BYTE bPos) const
{
	if (bPos >= EXCHANGE_ITEM_MAX_NUM)
		return 0;

	return m_kVictim.bSetValue[bPos];
}
#endif
