#include "stdafx.h"
#include "PythonShop.h"

#if defined(ENABLE_SET_ITEM)
#	include "PythonItem.h"
#endif
#include "PythonNetworkStream.h"
#if defined(ENABLE_PRIVATESHOP_SEARCH_SYSTEM)
#	include "../GameLib/ItemManager.h"
#endif

/*
BOOL CPythonShop::GetSlotItemID(DWORD dwSlotPos, DWORD* pdwItemID)
{
	if (!CheckSlotIndex(dwSlotPos))
		return FALSE;
	const TShopItemData* itemData;
	if (!GetItemData(dwSlotPos, &itemData))
		return FALSE;
	*pdwItemID = itemData->vnum;
	return TRUE;
}
*/

void CPythonShop::SetTabCoinType(BYTE tabIdx, BYTE coinType)
{
	if (tabIdx >= m_bTabCount)
	{
		TraceError("Out of Index. tabIdx(%d) must be less than %d.", tabIdx, SHOP_TAB_COUNT_MAX);
		return;
	}
	m_aShoptabs[tabIdx].coinType = coinType;
}

BYTE CPythonShop::GetTabCoinType(BYTE tabIdx)
{
	if (tabIdx >= m_bTabCount)
	{
		TraceError("Out of Index. tabIdx(%d) must be less than %d.", tabIdx, SHOP_TAB_COUNT_MAX);
		return 0xff;
	}
	return m_aShoptabs[tabIdx].coinType;
}

void CPythonShop::SetTabName(BYTE tabIdx, const char* name)
{
	if (tabIdx >= m_bTabCount)
	{
		TraceError("Out of Index. tabIdx(%d) must be less than %d.", tabIdx, SHOP_TAB_COUNT_MAX);
		return;
	}
	m_aShoptabs[tabIdx].name = name;
}

const char* CPythonShop::GetTabName(BYTE tabIdx)
{
	if (tabIdx >= m_bTabCount)
	{
		TraceError("Out of Index. tabIdx(%d) must be less than %d.", tabIdx, SHOP_TAB_COUNT_MAX);
		return NULL;
	}

	return m_aShoptabs[tabIdx].name.c_str();
}

void CPythonShop::SetItemData(DWORD dwIndex, const TShopItemData& c_rShopItemData)
{
	BYTE tabIdx = dwIndex / SHOP_HOST_ITEM_MAX_NUM;
	DWORD dwSlotPos = dwIndex % SHOP_HOST_ITEM_MAX_NUM;

	SetItemData(tabIdx, dwSlotPos, c_rShopItemData);
}

BOOL CPythonShop::GetItemData(DWORD dwIndex, const TShopItemData** c_ppItemData)
{
	BYTE tabIdx = dwIndex / SHOP_HOST_ITEM_MAX_NUM;
	DWORD dwSlotPos = dwIndex % SHOP_HOST_ITEM_MAX_NUM;

	return GetItemData(tabIdx, dwSlotPos, c_ppItemData);
}

void CPythonShop::SetItemData(BYTE tabIdx, DWORD dwSlotPos, const TShopItemData& c_rShopItemData)
{
	if (tabIdx >= SHOP_TAB_COUNT_MAX || dwSlotPos >= SHOP_HOST_ITEM_MAX_NUM)
	{
		TraceError("Out of Index. tabIdx(%d) must be less than %d. dwSlotPos(%d) must be less than %d", tabIdx, SHOP_TAB_COUNT_MAX, dwSlotPos, SHOP_HOST_ITEM_MAX_NUM);
		return;
	}

	m_aShoptabs[tabIdx].items[dwSlotPos] = c_rShopItemData;
}

BOOL CPythonShop::GetItemData(BYTE tabIdx, DWORD dwSlotPos, const TShopItemData** c_ppItemData)
{
	if (tabIdx >= SHOP_TAB_COUNT_MAX || dwSlotPos >= SHOP_HOST_ITEM_MAX_NUM)
	{
		TraceError("Out of Index. tabIdx(%d) must be less than %d. dwSlotPos(%d) must be less than %d", tabIdx, SHOP_TAB_COUNT_MAX, dwSlotPos, SHOP_HOST_ITEM_MAX_NUM);
		return FALSE;
	}

	*c_ppItemData = &m_aShoptabs[tabIdx].items[dwSlotPos];

	return TRUE;
}

/*
BOOL CPythonShop::CheckSlotIndex(DWORD dwSlotPos)
{
	if (dwSlotPos >= SHOP_HOST_ITEM_MAX_NUM * SHOP_TAB_COUNT_MAX)
		return FALSE;

	return TRUE;
}
*/

void CPythonShop::ClearPrivateShopStock()
{
	m_PrivateShopItemStock.clear();
}

void CPythonShop::AddPrivateShopItemStock(TItemPos ItemPos, BYTE bDisplayPos, DWORD dwPrice
#if defined(ENABLE_CHEQUE_SYSTEM)
	, DWORD dwCheque
#endif
)
{
	DelPrivateShopItemStock(ItemPos);

	TShopItemTable SellingItem;
	SellingItem.vnum = 0;
	SellingItem.count = 0;
	SellingItem.pos = ItemPos;
	SellingItem.price = dwPrice;
#if defined(ENABLE_CHEQUE_SYSTEM)
	SellingItem.cheque = dwCheque;
#endif
	SellingItem.display_pos = bDisplayPos;
	m_PrivateShopItemStock.insert(std::make_pair(ItemPos, SellingItem));
}
void CPythonShop::DelPrivateShopItemStock(TItemPos ItemPos)
{
	if (m_PrivateShopItemStock.end() == m_PrivateShopItemStock.find(ItemPos))
		return;

	m_PrivateShopItemStock.erase(ItemPos);
}

int CPythonShop::GetPrivateShopItemPrice(TItemPos ItemPos)
{
	TPrivateShopItemStock::iterator itor = m_PrivateShopItemStock.find(ItemPos);

	if (m_PrivateShopItemStock.end() == itor)
		return 0;

	TShopItemTable& rShopItemTable = itor->second;
	return rShopItemTable.price;
}

#if defined(ENABLE_CHEQUE_SYSTEM)
int CPythonShop::GetPrivateShopItemCheque(TItemPos ItemPos)
{
	TPrivateShopItemStock::iterator itor = m_PrivateShopItemStock.find(ItemPos);

	if (m_PrivateShopItemStock.end() == itor)
		return 0;

	TShopItemTable& rShopItemTable = itor->second;
	return rShopItemTable.cheque;
}
#endif

struct ItemStockSortFunc
{
	bool operator() (TShopItemTable& rkLeft, TShopItemTable& rkRight)
	{
		return rkLeft.display_pos < rkRight.display_pos;
	}
};
void CPythonShop::BuildPrivateShop(const char* c_szName)
{
	std::vector<TShopItemTable> ItemStock;
	ItemStock.reserve(m_PrivateShopItemStock.size());

	TPrivateShopItemStock::iterator itor = m_PrivateShopItemStock.begin();
	for (; itor != m_PrivateShopItemStock.end(); ++itor)
	{
		ItemStock.push_back(itor->second);
	}

	std::sort(ItemStock.begin(), ItemStock.end(), ItemStockSortFunc());

	CPythonNetworkStream::Instance().SendBuildPrivateShopPacket(c_szName, ItemStock);
}

void CPythonShop::Open(BOOL isPrivateShop, BOOL isMainPrivateShop)
{
	m_isShoping = TRUE;
	m_isPrivateShop = isPrivateShop;
	m_isMainPlayerPrivateShop = isMainPrivateShop;
}

void CPythonShop::Close()
{
	m_isShoping = FALSE;
	m_isPrivateShop = FALSE;
	m_isMainPlayerPrivateShop = FALSE;
}

BOOL CPythonShop::IsOpen()
{
	return m_isShoping;
}

BOOL CPythonShop::IsPrivateShop()
{
	return m_isPrivateShop;
}

BOOL CPythonShop::IsMainPlayerPrivateShop()
{
	return m_isMainPlayerPrivateShop;
}

void CPythonShop::Clear()
{
	m_isShoping = FALSE;
	m_isPrivateShop = FALSE;
	m_isMainPlayerPrivateShop = FALSE;
	m_isNameDialogOpen = false;

	ClearPrivateShopStock();
	m_bTabCount = 1;

	for (int i = 0; i < SHOP_TAB_COUNT_MAX; i++)
		memset(m_aShoptabs[i].items, 0, sizeof(TShopItemData) * SHOP_HOST_ITEM_MAX_NUM);
}

CPythonShop::CPythonShop(void)
{
	Clear();
}

CPythonShop::~CPythonShop(void)
{
#if defined(ENABLE_PRIVATESHOP_SEARCH_SYSTEM)
	ClearShopSearchData();
#endif
}

PyObject* shopOpen(PyObject* poSelf, PyObject* poArgs)
{
	int isPrivateShop = false;
	PyTuple_GetInteger(poArgs, 0, &isPrivateShop);

	int isMainPrivateShop = false;
	PyTuple_GetInteger(poArgs, 1, &isMainPrivateShop);

	CPythonShop& rkShop = CPythonShop::Instance();
	rkShop.Open(isPrivateShop, isMainPrivateShop);
	return Py_BuildNone();
}

PyObject* shopClose(PyObject* poSelf, PyObject* poArgs)
{
	CPythonShop& rkShop = CPythonShop::Instance();
	rkShop.Close();
	return Py_BuildNone();
}

PyObject* shopIsOpen(PyObject* poSelf, PyObject* poArgs)
{
	CPythonShop& rkShop = CPythonShop::Instance();
	return Py_BuildValue("i", rkShop.IsOpen());
}

PyObject* shopIsPrviateShop(PyObject* poSelf, PyObject* poArgs)
{
	CPythonShop& rkShop = CPythonShop::Instance();
	return Py_BuildValue("i", rkShop.IsPrivateShop());
}

PyObject* shopIsMainPlayerPrivateShop(PyObject* poSelf, PyObject* poArgs)
{
	CPythonShop& rkShop = CPythonShop::Instance();
	return Py_BuildValue("i", rkShop.IsMainPlayerPrivateShop());
}

PyObject* shopGetItemID(PyObject* poSelf, PyObject* poArgs)
{
	int nPos;
	if (!PyTuple_GetInteger(poArgs, 0, &nPos))
		return Py_BuildException();

	const TShopItemData* c_pItemData;
	if (CPythonShop::Instance().GetItemData(nPos, &c_pItemData))
		return Py_BuildValue("i", c_pItemData->dwVnum);

	return Py_BuildValue("i", 0);
}

PyObject* shopGetItemCount(PyObject* poSelf, PyObject* poArgs)
{
	int iIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iIndex))
		return Py_BuildException();

	const TShopItemData* c_pItemData;
	if (CPythonShop::Instance().GetItemData(iIndex, &c_pItemData))
		return Py_BuildValue("i", c_pItemData->dwCount);

	return Py_BuildValue("i", 0);
}

PyObject* shopGetItemPrice(PyObject* poSelf, PyObject* poArgs)
{
	int iIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iIndex))
		return Py_BuildException();

	const TShopItemData* c_pItemData;
	if (CPythonShop::Instance().GetItemData(iIndex, &c_pItemData))
		return Py_BuildValue("i", c_pItemData->dwPrice);

	return Py_BuildValue("i", 0);
}

#if defined(ENABLE_CHEQUE_SYSTEM)
PyObject* shopGetItemCheque(PyObject* poSelf, PyObject* poArgs)
{
	int iIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iIndex))
		return Py_BuildException();

	const TShopItemData* c_pItemData;
	if (CPythonShop::Instance().GetItemData(iIndex, &c_pItemData))
		return Py_BuildValue("i", c_pItemData->dwCheque);

	return Py_BuildValue("i", 0);
}
#endif

PyObject* shopGetItemMetinSocket(PyObject* poSelf, PyObject* poArgs)
{
	int iIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iIndex))
		return Py_BuildException();
	int iMetinSocketIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &iMetinSocketIndex))
		return Py_BuildException();

	const TShopItemData* c_pItemData;
	if (CPythonShop::Instance().GetItemData(iIndex, &c_pItemData))
		return Py_BuildValue("i", c_pItemData->alSockets[iMetinSocketIndex]);

	return Py_BuildValue("i", 0);
}

#if defined(ENABLE_APPLY_RANDOM)
PyObject* shopGetItemApplyRandom(PyObject* poSelf, PyObject* poArgs)
{
	int iIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iIndex))
		return Py_BuildException();

	int iApplySlotIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &iApplySlotIndex))
		return Py_BuildException();

	if (iApplySlotIndex >= 0 && iApplySlotIndex < ITEM_APPLY_RANDOM_SLOT_MAX_NUM)
	{
		const TShopItemData* c_pItemData;
		if (CPythonShop::Instance().GetItemData(iIndex, &c_pItemData))
			return Py_BuildValue("ii", c_pItemData->aApplyRandom[iApplySlotIndex].wType, c_pItemData->aApplyRandom[iApplySlotIndex].lValue);
	}

	return Py_BuildValue("ii", 0, 0);
}
#endif

PyObject* shopGetItemAttribute(PyObject* poSelf, PyObject* poArgs)
{
	int iIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iIndex))
		return Py_BuildException();
	int iAttrSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &iAttrSlotIndex))
		return Py_BuildException();

	if (iAttrSlotIndex >= 0 && iAttrSlotIndex < ITEM_ATTRIBUTE_SLOT_MAX_NUM)
	{
		const TShopItemData* c_pItemData;
		if (CPythonShop::Instance().GetItemData(iIndex, &c_pItemData))
			return Py_BuildValue("ii", c_pItemData->aAttr[iAttrSlotIndex].wType, c_pItemData->aAttr[iAttrSlotIndex].lValue);
	}

	return Py_BuildValue("ii", 0, 0);
}

PyObject* shopClearPrivateShopStock(PyObject* poSelf, PyObject* poArgs)
{
	CPythonShop::Instance().ClearPrivateShopStock();
	return Py_BuildNone();
}

PyObject* shopAddPrivateShopItemStock(PyObject* poSelf, PyObject* poArgs)
{
	BYTE bItemWindowType;
	if (!PyTuple_GetInteger(poArgs, 0, &bItemWindowType))
		return Py_BuildException();

	WORD wItemSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &wItemSlotIndex))
		return Py_BuildException();

	int iDisplaySlotIndex;
	if (!PyTuple_GetInteger(poArgs, 2, &iDisplaySlotIndex))
		return Py_BuildException();

	int iPrice;
	if (!PyTuple_GetInteger(poArgs, 3, &iPrice))
		return Py_BuildException();

#if defined(ENABLE_CHEQUE_SYSTEM)
	int iCheque;
	if (!PyTuple_GetInteger(poArgs, 4, &iCheque))
		return Py_BuildException();
#endif

	CPythonShop::Instance().AddPrivateShopItemStock(TItemPos(bItemWindowType, wItemSlotIndex), iDisplaySlotIndex, iPrice
#if defined(ENABLE_CHEQUE_SYSTEM)
		, iCheque
#endif
	);

	return Py_BuildNone();
}

PyObject* shopDelPrivateShopItemStock(PyObject* poSelf, PyObject* poArgs)
{
	BYTE bItemWindowType;
	if (!PyTuple_GetInteger(poArgs, 0, &bItemWindowType))
		return Py_BuildException();

	WORD wItemSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &wItemSlotIndex))
		return Py_BuildException();

	CPythonShop::Instance().DelPrivateShopItemStock(TItemPos(bItemWindowType, wItemSlotIndex));
	return Py_BuildNone();
}

PyObject* shopGetPrivateShopItemPrice(PyObject* poSelf, PyObject* poArgs)
{
	BYTE bItemWindowType;
	if (!PyTuple_GetInteger(poArgs, 0, &bItemWindowType))
		return Py_BuildException();

	WORD wItemSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &wItemSlotIndex))
		return Py_BuildException();

	int iValue = CPythonShop::Instance().GetPrivateShopItemPrice(TItemPos(bItemWindowType, wItemSlotIndex));
	return Py_BuildValue("i", iValue);
}

#if defined(ENABLE_CHEQUE_SYSTEM)
PyObject* shopGetPrivateShopItemCheque(PyObject* poSelf, PyObject* poArgs)
{
	BYTE bItemWindowType;
	if (!PyTuple_GetInteger(poArgs, 0, &bItemWindowType))
		return Py_BuildException();

	WORD wItemSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &wItemSlotIndex))
		return Py_BuildException();

	int iValue = CPythonShop::Instance().GetPrivateShopItemCheque(TItemPos(bItemWindowType, wItemSlotIndex));
	return Py_BuildValue("i", iValue);
}
#endif

PyObject* shopBuildPrivateShop(PyObject* poSelf, PyObject* poArgs)
{
	char* szName;
	if (!PyTuple_GetString(poArgs, 0, &szName))
		return Py_BuildException();

	CPythonShop::Instance().BuildPrivateShop(szName);
	return Py_BuildNone();
}

PyObject* shopGetTabCount(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonShop::Instance().GetTabCount());
}

PyObject* shopGetTabName(PyObject* poSelf, PyObject* poArgs)
{
	BYTE bTabIdx;
	if (!PyTuple_GetInteger(poArgs, 0, &bTabIdx))
		return Py_BuildException();

	return Py_BuildValue("s", CPythonShop::Instance().GetTabName(bTabIdx));
}

PyObject* shopGetTabCoinType(PyObject* poSelf, PyObject* poArgs)
{
	BYTE bTabIdx;
	if (!PyTuple_GetInteger(poArgs, 0, &bTabIdx))
		return Py_BuildException();

	return Py_BuildValue("i", CPythonShop::Instance().GetTabCoinType(bTabIdx));
}

#if defined(ENABLE_SHOPEX_RENEWAL)
PyObject* shopGetItemPriceType(PyObject* poSelf, PyObject* poArgs)
{
	int nPos;
	if (!PyTuple_GetInteger(poArgs, 0, &nPos))
		return Py_BuildException();

	const TShopItemData* c_pItemData;
	if (CPythonShop::Instance().GetItemData(nPos, &c_pItemData))
		return Py_BuildValue("i", c_pItemData->bPriceType);

	return Py_BuildValue("i", SHOP_COIN_TYPE_GOLD);
}

PyObject* shopGetItemPriceVnum(PyObject* poSelf, PyObject* poArgs)
{
	int nPos;
	if (!PyTuple_GetInteger(poArgs, 0, &nPos))
		return Py_BuildException();

	const TShopItemData* c_pItemData;
	if (CPythonShop::Instance().GetItemData(nPos, &c_pItemData))
		return Py_BuildValue("i", c_pItemData->dwPriceVnum);

	return Py_BuildValue("i", 0);
}
#endif

#if defined(ENABLE_PRIVATESHOP_SEARCH_SYSTEM)
void CPythonShop::ClearShopSearchData()
{
	for (auto obj : vShopSearch)
		if (obj) delete obj;
	vShopSearch.clear();
	ShopSearchChangePage(1);
}

void CPythonShop::ShopSearchChangePage(int iPage)
{
	iShopSearchPage = iPage;
}

void CPythonShop::SetShopSearchItemData(ShopSearchData* sShopData)
{
	vShopSearch.push_back(sShopData);
}

ShopSearchData* CPythonShop::GetShopSearchItemData(DWORD dwIndex)
{
	dwIndex += (iShopSearchPage - 1) * 10;
	if (dwIndex >= vShopSearch.size())
		return nullptr;

	return vShopSearch.at(dwIndex);
}

void CPythonShop::SortShopSearchData()
{
	// Sort by ASC
	// 1 : WON | 2 : GOLD
	std::sort(vShopSearch.begin(), vShopSearch.end(), [](const ShopSearchData* a, const ShopSearchData* b)
		{
#if defined(ENABLE_CHEQUE_SYSTEM)
			return (a->item.dwCheque < b->item.dwCheque) || (a->item.dwCheque == b->item.dwCheque && a->item.dwPrice < b->item.dwPrice);
#else
			return (a->item.dwPrice < b->item.dwPrice);
#endif
		});
}

PyObject* shopGetPrivateShopSearchResult(PyObject* poSelf, PyObject* poArgs)
{
	int iLine;
	if (!PyTuple_GetInteger(poArgs, 0, &iLine))
		return Py_BuildException();

	const ShopSearchData* pkShopSearchData = CPythonShop::Instance().GetShopSearchItemData(iLine);
	if (pkShopSearchData)
	{
		CItemData* pItemData;
		if (CItemManager::Instance().GetItemDataPointer(pkShopSearchData->item.dwVnum, &pItemData))
		{
#if defined(ENABLE_SET_ITEM)
			const char* szItemName = CPythonItem::Instance().GetPreName(pkShopSearchData->item.bSetValue, pItemData->GetName());
#else
			const char* szItemName = pItemData->GetName();
#endif
			return Py_BuildValue("ssii"
#if defined(ENABLE_CHEQUE_SYSTEM)
				"i"
#endif
				, szItemName
				, pkShopSearchData->name.c_str()
				, pkShopSearchData->item.dwCount
				, pkShopSearchData->item.dwPrice
#if defined(ENABLE_CHEQUE_SYSTEM)
				, pkShopSearchData->item.dwCheque
#endif
			);
		}
	}

	return Py_BuildValue("ssii"
#if defined(ENABLE_CHEQUE_SYSTEM)
		"i"
#endif
		, ""
		, ""
		, 0
		, 0
#if defined(ENABLE_CHEQUE_SYSTEM)
		, 0
#endif
	);
}

#if defined(ENABLE_APPLY_RANDOM)
PyObject* shopGetPrivateShopItemApplyRandom(PyObject* poSelf, PyObject* poArgs)
{
	int iIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iIndex))
		return Py_BuildException();

	int iApplySlotIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &iApplySlotIndex))
		return Py_BuildException();

	if (iApplySlotIndex >= 0 && iApplySlotIndex < ITEM_APPLY_RANDOM_SLOT_MAX_NUM)
	{
		const ShopSearchData* pkShopSearchData = CPythonShop::Instance().GetShopSearchItemData(iIndex);
		if (pkShopSearchData)
			return Py_BuildValue("ii", pkShopSearchData->item.aApplyRandom[iApplySlotIndex].wType, pkShopSearchData->item.aApplyRandom[iApplySlotIndex].lValue);
	}

	return Py_BuildValue("ii", 0, 0);
}
#endif

PyObject* shopGetPrivateShopSelectItemAttribute(PyObject* poSelf, PyObject* poArgs)
{
	int iIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iIndex))
		return Py_BuildException();

	int iAttrSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &iAttrSlotIndex))
		return Py_BuildException();

	if (iAttrSlotIndex >= 0 && iAttrSlotIndex < ITEM_ATTRIBUTE_SLOT_MAX_NUM)
	{
		const ShopSearchData* pkShopSearchData = CPythonShop::Instance().GetShopSearchItemData(iIndex);
		if (pkShopSearchData)
			return Py_BuildValue("ii", pkShopSearchData->item.aAttr[iAttrSlotIndex].wType, pkShopSearchData->item.aAttr[iAttrSlotIndex].lValue);
	}

	return Py_BuildValue("ii", 0, 0);
}

PyObject* shopGetPrivateShopSelectItemMetinSocket(PyObject* poSelf, PyObject* poArgs)
{
	int iIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iIndex))
		return Py_BuildException();

	int iMetinSocketIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &iMetinSocketIndex))
		return Py_BuildException();

	if (iMetinSocketIndex >= 0 && iMetinSocketIndex < ITEM_SOCKET_SLOT_MAX_NUM)
	{
		const ShopSearchData* pkShopSearchData = CPythonShop::Instance().GetShopSearchItemData(iIndex);
		if (pkShopSearchData)
			return Py_BuildValue("i", pkShopSearchData->item.alSockets[iMetinSocketIndex]);
	}

	return Py_BuildValue("i", 0);
}

PyObject* shopGetPrivateShopSelectItemVnum(PyObject* poSelf, PyObject* poArgs)
{
	int iIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iIndex))
		return Py_BuildException();

	const ShopSearchData* pkShopSearchData = CPythonShop::Instance().GetShopSearchItemData(iIndex);
	if (pkShopSearchData)
		return Py_BuildValue("i", pkShopSearchData->item.dwVnum);

	return Py_BuildValue("i", 0);
}

PyObject* shopGetPrivateShopSearchResultMaxCount(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonShop::Instance().GetShopSearchResultCount());
}

PyObject* shopGetPrivateShopSearchResultPage(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonShop::Instance().GetShopSearchPage());
}

PyObject* shopGetPrivateShopSelectItemChrVID(PyObject* poSelf, PyObject* poArgs)
{
	int iIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iIndex))
		return Py_BuildException();

	const ShopSearchData* pkShopSearchData = CPythonShop::Instance().GetShopSearchItemData(iIndex);
	if (pkShopSearchData)
		return Py_BuildValue("i", pkShopSearchData->dwShopVID);

	return Py_BuildValue("i", 0);
}

#if defined(ENABLE_CHANGE_LOOK_SYSTEM)
PyObject* shopGetPrivateShopItemChangeLookVnum(PyObject* poSelf, PyObject* poArgs)
{
	int iIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iIndex))
		return Py_BuildException();

	const ShopSearchData* pkShopSearchData = CPythonShop::Instance().GetShopSearchItemData(iIndex);
	if (pkShopSearchData)
		return Py_BuildValue("i", pkShopSearchData->item.dwTransmutationVnum);

	return Py_BuildValue("i", 0);
}
#endif

#if defined(ENABLE_SET_ITEM)
PyObject* shopGetPrivateShopItemSetValue(PyObject* poSelf, PyObject* poArgs)
{
	int iIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iIndex))
		return Py_BuildException();

	const ShopSearchData* pkShopSearchData = CPythonShop::Instance().GetShopSearchItemData(iIndex);
	if (pkShopSearchData)
		return Py_BuildValue("i", pkShopSearchData->item.bSetValue);

	return Py_BuildValue("i", 0);
}
#endif

#if defined(ENABLE_REFINE_ELEMENT_SYSTEM)
PyObject* shopGetPrivateShopItemRefineElement(PyObject* poSelf, PyObject* poArgs)
{
	int iIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iIndex))
		return Py_BuildException();

	const ShopSearchData* pkShopSearchData = CPythonShop::Instance().GetShopSearchItemData(iIndex);
	if (!pkShopSearchData)
		return Py_BuildValue("(iiOO)", 0, 0, Py_BuildValue("iii", 0, 0, 0), Py_BuildValue("iii", 0, 0, 0));

	const TPlayerItemRefineElement& kTable = pkShopSearchData->item.RefineElement;
	return Py_BuildValue("(iiOO)", kTable.wApplyType, kTable.bGrade,
		Py_BuildValue("iii", kTable.abValue[0], kTable.abValue[1], kTable.abValue[2]),
		Py_BuildValue("iii", kTable.abBonusValue[0], kTable.abBonusValue[1], kTable.abBonusValue[2]));
}
#endif
#endif

#if defined(ENABLE_CHANGE_LOOK_SYSTEM)
PyObject* shopGetItemChangeLookVnum(PyObject* poSelf, PyObject* poArgs)
{
	int iIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iIndex))
		return Py_BuildException();

	const TShopItemData* c_pShopItemData;
	if (CPythonShop::Instance().GetItemData(iIndex, &c_pShopItemData))
		return Py_BuildValue("i", c_pShopItemData->dwTransmutationVnum);

	return Py_BuildValue("i", 0);
}
#endif

#if defined(ENABLE_SET_ITEM)
PyObject* shopGetItemSetValue(PyObject* poSelf, PyObject* poArgs)
{
	int iIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iIndex))
		return Py_BuildException();

	const TShopItemData* c_pShopItemData;
	if (CPythonShop::Instance().GetItemData(iIndex, &c_pShopItemData))
		return Py_BuildValue("i", c_pShopItemData->bSetValue);

	return Py_BuildValue("i", 0);
}
#endif

#if defined(ENABLE_REFINE_ELEMENT_SYSTEM)
PyObject* shopGetItemRefineElement(PyObject* poSelf, PyObject* poArgs)
{
	int iIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iIndex))
		return Py_BuildException();

	const TShopItemData* c_pShopItemData;
	if (!CPythonShop::Instance().GetItemData(iIndex, &c_pShopItemData))
		return Py_BuildValue("(iiOO)", 0, 0, Py_BuildValue("iii", 0, 0, 0), Py_BuildValue("iii", 0, 0, 0));

	const TPlayerItemRefineElement& kTable = c_pShopItemData->RefineElement;
	return Py_BuildValue("(iiOO)", kTable.wApplyType, kTable.bGrade,
		Py_BuildValue("iii", kTable.abValue[0], kTable.abValue[1], kTable.abValue[2]),
		Py_BuildValue("iii", kTable.abBonusValue[0], kTable.abBonusValue[1], kTable.abBonusValue[2]));
}
#endif

PyObject* shopGetNameDialogOpen(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonShop::Instance().GetNameDialogOpen());
}

PyObject* shopSetNameDialogOpen(PyObject* poSelf, PyObject* poArgs)
{
	bool bOpen;
	if (!PyTuple_GetBoolean(poArgs, 0, &bOpen))
		return Py_BuildException();

	CPythonShop::Instance().SetNameDialogOpen(bOpen);
	return Py_BuildNone();
}

void initshop()
{
	static PyMethodDef s_methods[] =
	{
		// Shop
		{ "Open", shopOpen, METH_VARARGS },
		{ "Close", shopClose, METH_VARARGS },
		{ "IsOpen", shopIsOpen, METH_VARARGS },
		{ "IsPrivateShop", shopIsPrviateShop, METH_VARARGS },
		{ "IsMainPlayerPrivateShop", shopIsMainPlayerPrivateShop, METH_VARARGS },
		{ "GetItemID", shopGetItemID, METH_VARARGS },
		{ "GetItemCount", shopGetItemCount, METH_VARARGS },
		{ "GetItemPrice", shopGetItemPrice, METH_VARARGS },
#if defined(ENABLE_CHEQUE_SYSTEM)
		{ "GetItemCheque", shopGetItemCheque, METH_VARARGS },
#endif
		{ "GetItemMetinSocket", shopGetItemMetinSocket, METH_VARARGS },
#if defined(ENABLE_APPLY_RANDOM)
		{ "GetItemApplyRandom", shopGetItemApplyRandom, METH_VARARGS },
#endif
		{ "GetItemAttribute", shopGetItemAttribute, METH_VARARGS },
		{ "GetTabCount", shopGetTabCount, METH_VARARGS },
		{ "GetTabName", shopGetTabName, METH_VARARGS },
		{ "GetTabCoinType", shopGetTabCoinType, METH_VARARGS },
#if defined(ENABLE_CHANGE_LOOK_SYSTEM)
		{ "GetItemChangeLookVnum", shopGetItemChangeLookVnum, METH_VARARGS },
#endif
#if defined(ENABLE_SET_ITEM)
		{ "GetItemSetValue", shopGetItemSetValue, METH_VARARGS },
#endif
#if defined(ENABLE_REFINE_ELEMENT_SYSTEM)
		{ "GetItemRefineElement", shopGetItemRefineElement, METH_VARARGS },
#endif

#if defined(ENABLE_SHOPEX_RENEWAL)
		// ShopEx
		{ "GetItemPriceType", shopGetItemPriceType, METH_VARARGS },
		{ "GetItemPriceVnum", shopGetItemPriceVnum, METH_VARARGS },
#endif

		// Private Shop
		{ "ClearPrivateShopStock", shopClearPrivateShopStock, METH_VARARGS },
		{ "AddPrivateShopItemStock", shopAddPrivateShopItemStock, METH_VARARGS },
		{ "DelPrivateShopItemStock", shopDelPrivateShopItemStock, METH_VARARGS },
		{ "GetPrivateShopItemPrice", shopGetPrivateShopItemPrice, METH_VARARGS },
#if defined(ENABLE_CHEQUE_SYSTEM)
		{ "GetPrivateShopItemCheque", shopGetPrivateShopItemCheque, METH_VARARGS },
#endif
		{ "BuildPrivateShop", shopBuildPrivateShop, METH_VARARGS },

#if defined(ENABLE_PRIVATESHOP_SEARCH_SYSTEM)
		// Private Shop Search
		{ "GetPrivateShopSearchResult", shopGetPrivateShopSearchResult, METH_VARARGS },
#	if defined(ENABLE_APPLY_RANDOM)
		{ "GetPrivateShopItemApplyRandom", shopGetPrivateShopItemApplyRandom, METH_VARARGS },
#	endif
		{ "GetPrivateShopSelectItemAttribute", shopGetPrivateShopSelectItemAttribute, METH_VARARGS },
		{ "GetPrivateShopSelectItemMetinSocket", shopGetPrivateShopSelectItemMetinSocket, METH_VARARGS },
		{ "GetPrivateShopSelectItemVnum", shopGetPrivateShopSelectItemVnum, METH_VARARGS },
		{ "GetPrivateShopSearchResultMaxCount", shopGetPrivateShopSearchResultMaxCount, METH_VARARGS },
		{ "GetPrivateShopSearchResultPage", shopGetPrivateShopSearchResultPage, METH_VARARGS },
		{ "GetPrivateShopSelectItemChrVID", shopGetPrivateShopSelectItemChrVID, METH_VARARGS },
#	if defined(ENABLE_CHANGE_LOOK_SYSTEM)
		{ "GetPrivateShopItemChangeLookVnum", shopGetPrivateShopItemChangeLookVnum, METH_VARARGS },
#	endif
#	if defined(ENABLE_SET_ITEM)
		{ "GetPrivateShopItemSetValue", shopGetPrivateShopItemSetValue, METH_VARARGS },
#	endif
#	if defined(ENABLE_REFINE_ELEMENT_SYSTEM)
		{ "GetPrivateShopItemRefineElement", shopGetPrivateShopItemRefineElement, METH_VARARGS },
#	endif
#endif

		{ "SetNameDialogOpen", shopSetNameDialogOpen, METH_VARARGS },
		{ "GetNameDialogOpen", shopGetNameDialogOpen, METH_VARARGS },

		{ NULL, NULL, NULL },
	};
	PyObject* poModule = Py_InitModule("shop", s_methods);

	PyModule_AddIntConstant(poModule, "SHOP_SLOT_COUNT", SHOP_HOST_ITEM_MAX_NUM);
#if defined(ENABLE_MYSHOP_EXPANSION)
	PyModule_AddIntConstant(poModule, "SHOP_HOST_ITEM_MAX", SHOP_HOST_ITEM_MAX);
#endif

	PyModule_AddIntConstant(poModule, "SHOP_COIN_TYPE_GOLD", SHOP_COIN_TYPE_GOLD);
	PyModule_AddIntConstant(poModule, "SHOP_COIN_TYPE_SECONDARY_COIN", SHOP_COIN_TYPE_SECONDARY_COIN);
	//PyModule_AddIntConstant(poModule, "SHOP_COIN_TYPE_BATTLE_POINT", SHOP_COIN_TYPE_BATTLE_POINT);
	//PyModule_AddIntConstant(poModule, "SHOP_COIN_TYPE_10TH_EVENT", SHOP_COIN_TYPE_10TH_EVENT);
	//PyModule_AddIntConstant(poModule, "SHOP_COIN_TYPE_MEDAL_OF_HONOR", SHOP_COIN_TYPE_MEDAL_OF_HONOR);
	//PyModule_AddIntConstant(poModule, "SHOP_COIN_TYPE_BNW", SHOP_COIN_TYPE_BNW);
	//PyModule_AddIntConstant(poModule, "SHOP_COIN_TYPE_TREE_FRUIT", SHOP_COIN_TYPE_TREE_FRUIT);
#if defined(ENABLE_SHOPEX_RENEWAL)
	PyModule_AddIntConstant(poModule, "SHOP_COIN_TYPE_ITEM", SHOP_COIN_TYPE_ITEM);
#endif
}
