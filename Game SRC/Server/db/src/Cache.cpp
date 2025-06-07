#include "stdafx.h"
#include "Cache.h"

#include "QID.h"
#include "ClientManager.h"
#include "Main.h"

extern CPacketInfo g_item_info;
extern int g_iPlayerCacheFlushSeconds;
extern int g_iItemCacheFlushSeconds;
extern int g_test_server;
// MYSHOP_PRICE_LIST
extern int g_iItemPriceListTableCacheFlushSeconds;
// END_OF_MYSHOP_PRICE_LIST

extern int g_item_count;

CItemCache::CItemCache()
{
	m_expireTime = MIN(1800, g_iItemCacheFlushSeconds);
}

CItemCache::~CItemCache()
{
}

// 이거 이상한데...
// Delete를 했으면, Cache도 해제해야 하는것 아닌가???
// 근데 Cache를 해제하는 부분이 없어.
// 못 찾은 건가?
// 이렇게 해놓으면, 계속 시간이 될 때마다 아이템을 계속 지워...
// 이미 사라진 아이템인데... 확인사살??????
// fixme
// by rtsummit
void CItemCache::Delete()
{
	if (m_data.dwVnum == 0)
		return;

	//char szQuery[QUERY_MAX_LEN];
	//szQuery[QUERY_MAX_LEN] = '\0';
	if (g_test_server)
		sys_log(0, "ItemCache::Delete : DELETE %u", m_data.dwID);

	m_data.dwVnum = 0;
	m_bNeedQuery = true;
	m_lastUpdateTime = time(0);
	OnFlush();

	//m_bNeedQuery = false;
	//m_lastUpdateTime = time(0) - m_expireTime;
}

void CItemCache::OnFlush()
{
	if (m_data.dwVnum == 0) // vnum 이 0이면 삭제하라고 표시된 것이다.
	{
		char szQuery[QUERY_MAX_LEN];
		snprintf(szQuery, sizeof(szQuery), "DELETE FROM item%s WHERE `id` = %u", GetTablePostfix(), m_data.dwID);
		CDBManager::instance().ReturnQuery(szQuery, QID_ITEM_DESTROY, 0, NULL);

		if (g_test_server)
			sys_log(0, "ItemCache::Flush : DELETE %u %s", m_data.dwID, szQuery);
	}
	else
	{
		long alSockets[ITEM_SOCKET_MAX_NUM];
		TPlayerItemAttribute aAttr[ITEM_ATTRIBUTE_MAX_NUM];

#if defined(__ITEM_APPLY_RANDOM__)
		TPlayerItemAttribute aApplyRandom[ITEM_APPLY_MAX_NUM];
		bool isApplyRandom = false;
#endif

		bool isSocket = false, isAttr = false;

		memset(&alSockets, 0, sizeof(long) * ITEM_SOCKET_MAX_NUM);
		memset(&aAttr, 0, sizeof(TPlayerItemAttribute) * ITEM_ATTRIBUTE_MAX_NUM);
#if defined(__ITEM_APPLY_RANDOM__)
		memset(&aApplyRandom, 0, sizeof(TPlayerItemAttribute) * ITEM_APPLY_MAX_NUM);
#endif

		TPlayerItem* p = &m_data;

		if (memcmp(alSockets, p->alSockets, sizeof(long) * ITEM_SOCKET_MAX_NUM))
			isSocket = true;

		if (memcmp(aAttr, p->aAttr, sizeof(TPlayerItemAttribute) * ITEM_ATTRIBUTE_MAX_NUM))
			isAttr = true;

#if defined(__ITEM_APPLY_RANDOM__)
		if (memcmp(aApplyRandom, p->aApplyRandom, sizeof(TPlayerItemAttribute) * ITEM_APPLY_MAX_NUM))
			isApplyRandom = true;
#endif

		char szColumns[QUERY_MAX_LEN];
		char szValues[QUERY_MAX_LEN];
		char szUpdate[QUERY_MAX_LEN];

		int iLen = snprintf(szColumns, sizeof(szColumns), "`id`, `owner_id`, `window`, `pos`, `vnum`, `count`");
		int iValueLen = snprintf(szValues, sizeof(szValues),
			"%u, %u, %u, %u, %u, %u"
			, p->dwID, p->dwOwner, p->bWindow, p->wPos, p->dwVnum, p->dwCount
		);
		int iUpdateLen = snprintf(szUpdate, sizeof(szUpdate),
			"`owner_id` = %u, `window` = %u, `pos` = %u, `vnum` = %u, `count` = %u"
			, p->dwOwner, p->bWindow, p->wPos, p->dwCount, p->dwVnum
		);

#if defined(__SOUL_BIND_SYSTEM__)
		iLen += snprintf(szColumns + iLen, sizeof(szColumns) - iLen, ", `soulbind`");
		iValueLen += snprintf(szValues + iValueLen, sizeof(szValues) - iValueLen, ", %ld", p->lSealDate);
		iUpdateLen += snprintf(szUpdate + iUpdateLen, sizeof(szUpdate) - iUpdateLen, ", `soulbind` = %ld", p->lSealDate);
#endif

		if (isSocket)
		{
			iLen += snprintf(szColumns + iLen, sizeof(szColumns) - iLen,
				", `socket0`"
				", `socket1`"
				", `socket2`"
#if defined(__ITEM_SOCKET6__)
				", `socket3`"
				", `socket4`"
				", `socket5`"
#endif
			);
			iValueLen += snprintf(szValues + iValueLen, sizeof(szValues) - iValueLen,
				", %lu"
				", %lu"
				", %lu"
#if defined(__ITEM_SOCKET6__)
				", %lu"
				", %lu"
				", %lu"
#endif
				, p->alSockets[0]
				, p->alSockets[1]
				, p->alSockets[2]
#if defined(__ITEM_SOCKET6__)
				, p->alSockets[3]
				, p->alSockets[4]
				, p->alSockets[5]
#endif
			);
			iUpdateLen += snprintf(szUpdate + iUpdateLen, sizeof(szUpdate) - iUpdateLen,
				", `socket0` = %lu"
				", `socket1` = %lu"
				", `socket2` = %lu"
#if defined(__ITEM_SOCKET6__)
				", `socket3` = %lu"
				", `socket4` = %lu"
				", `socket5` = %lu"
#endif
				, p->alSockets[0]
				, p->alSockets[1]
				, p->alSockets[2]
#if defined(__ITEM_SOCKET6__)
				, p->alSockets[3]
				, p->alSockets[4]
				, p->alSockets[5]
#endif
			);
		}

		if (isAttr)
		{
			iLen += snprintf(szColumns + iLen, sizeof(szColumns) - iLen,
				", `attrtype0`, `attrvalue0`"
				", `attrtype1`, `attrvalue1`"
				", `attrtype2`, `attrvalue2`"
				", `attrtype3`, `attrvalue3`"
				", `attrtype4`, `attrvalue4`"
				", `attrtype5`, `attrvalue5`"
				", `attrtype6`, `attrvalue6`"
			);

			iValueLen += snprintf(szValues + iValueLen, sizeof(szValues) - iValueLen,
				", %u, %ld"
				", %u, %ld"
				", %u, %ld"
				", %u, %ld"
				", %u, %ld"
				", %u, %ld"
				", %u, %ld"
				, p->aAttr[0].wType, p->aAttr[0].lValue
				, p->aAttr[1].wType, p->aAttr[1].lValue
				, p->aAttr[2].wType, p->aAttr[2].lValue
				, p->aAttr[3].wType, p->aAttr[3].lValue
				, p->aAttr[4].wType, p->aAttr[4].lValue
				, p->aAttr[5].wType, p->aAttr[5].lValue
				, p->aAttr[6].wType, p->aAttr[6].lValue
			);

			iUpdateLen += snprintf(szUpdate + iUpdateLen, sizeof(szUpdate) - iUpdateLen,
				", `attrtype0` = %u, `attrvalue0` = %ld"
				", `attrtype1` = %u, `attrvalue1` = %ld"
				", `attrtype2` = %u, `attrvalue2` = %ld"
				", `attrtype3` = %u, `attrvalue3` = %ld"
				", `attrtype4` = %u, `attrvalue4` = %ld"
				", `attrtype5` = %u, `attrvalue5` = %ld"
				", `attrtype6` = %u, `attrvalue6` = %ld"
				, p->aAttr[0].wType, p->aAttr[0].lValue
				, p->aAttr[1].wType, p->aAttr[1].lValue
				, p->aAttr[2].wType, p->aAttr[2].lValue
				, p->aAttr[3].wType, p->aAttr[3].lValue
				, p->aAttr[4].wType, p->aAttr[4].lValue
				, p->aAttr[5].wType, p->aAttr[5].lValue
				, p->aAttr[6].wType, p->aAttr[6].lValue
			);
		}

#if defined(__CHANGE_LOOK_SYSTEM__)
		iLen += snprintf(szColumns + iLen, sizeof(szColumns) - iLen, ", `transmutation`");
		iValueLen += snprintf(szValues + iValueLen, sizeof(szValues) - iValueLen, ", %u", p->dwTransmutationVnum);
		iUpdateLen += snprintf(szUpdate + iUpdateLen, sizeof(szUpdate) - iUpdateLen, ", `transmutation` = %u", p->dwTransmutationVnum);
#endif

#if defined(__REFINE_ELEMENT_SYSTEM__)
		iLen += snprintf(szColumns + iLen, sizeof(szColumns) - iLen,
			", `refine_element_apply_type`"
			", `refine_element_grade`"
			", `refine_element_value0`, `refine_element_value1`, `refine_element_value2`"
			", `refine_element_bonus_value0`, `refine_element_bonus_value1`, `refine_element_bonus_value2`"
		);
		iValueLen += snprintf(szValues + iValueLen, sizeof(szValues) - iValueLen,
			", %d"
			", %d"
			", %d, %d, %d"
			", %d, %d, %d"
			, p->RefineElement.wApplyType
			, p->RefineElement.bGrade
			, p->RefineElement.abValue[0], p->RefineElement.abValue[1], p->RefineElement.abValue[2]
			, p->RefineElement.abBonusValue[0], p->RefineElement.abBonusValue[1], p->RefineElement.abBonusValue[2]
		);
		iUpdateLen += snprintf(szUpdate + iUpdateLen, sizeof(szUpdate) - iUpdateLen,
			", `refine_element_apply_type` = %d"
			", `refine_element_grade` = %d"
			", `refine_element_value0` = %d, `refine_element_value1` = %d, `refine_element_value2` = %d"
			", `refine_element_bonus_value0` = %d, `refine_element_bonus_value1` = %d, `refine_element_bonus_value2` = %d"
			, p->RefineElement.wApplyType
			, p->RefineElement.bGrade
			, p->RefineElement.abValue[0], p->RefineElement.abValue[1], p->RefineElement.abValue[2]
			, p->RefineElement.abBonusValue[0], p->RefineElement.abBonusValue[1], p->RefineElement.abBonusValue[2]
		);
#endif

#if defined(__ITEM_APPLY_RANDOM__)
		if (isApplyRandom)
		{
			iLen += snprintf(szColumns + iLen, sizeof(szColumns) - iLen,
				", `apply_type0`, `apply_value0`, `apply_path0`"
				", `apply_type1`, `apply_value1`, `apply_path1`"
				", `apply_type2`, `apply_value2`, `apply_path2`"
				", `apply_type3`, `apply_value3`, `apply_path3`"
			);

			iValueLen += snprintf(szValues + iValueLen, sizeof(szValues) - iValueLen,
				", %u, %ld, %d"
				", %u, %ld, %d"
				", %u, %ld, %d"
				", %u, %ld, %d"
				, p->aApplyRandom[0].wType, p->aApplyRandom[0].lValue, p->aApplyRandom[0].bPath
				, p->aApplyRandom[1].wType, p->aApplyRandom[1].lValue, p->aApplyRandom[1].bPath
				, p->aApplyRandom[2].wType, p->aApplyRandom[2].lValue, p->aApplyRandom[2].bPath
				, p->aApplyRandom[3].wType, p->aApplyRandom[3].lValue, p->aApplyRandom[3].bPath
			);

			iUpdateLen += snprintf(szUpdate + iUpdateLen, sizeof(szUpdate) - iUpdateLen,
				", `apply_type0` = %u, `apply_value0` = %ld, `apply_path0` = %d"
				", `apply_type1` = %u, `apply_value1` = %ld, `apply_path1` = %d"
				", `apply_type2` = %u, `apply_value2` = %ld, `apply_path2` = %d"
				", `apply_type3` = %u, `apply_value3` = %ld, `apply_path3` = %d"
				, p->aApplyRandom[0].wType, p->aApplyRandom[0].lValue, p->aApplyRandom[3].bPath
				, p->aApplyRandom[1].wType, p->aApplyRandom[1].lValue, p->aApplyRandom[3].bPath
				, p->aApplyRandom[2].wType, p->aApplyRandom[2].lValue, p->aApplyRandom[3].bPath
				, p->aApplyRandom[3].wType, p->aApplyRandom[3].lValue, p->aApplyRandom[3].bPath
			);
		}
#endif

#if defined(__SET_ITEM__)
		iLen += snprintf(szColumns + iLen, sizeof(szColumns) - iLen, ", `set_value`");
		iValueLen += snprintf(szValues + iValueLen, sizeof(szValues) - iValueLen, ", %u", p->bSetValue);
		iUpdateLen += snprintf(szUpdate + iUpdateLen, sizeof(szUpdate) - iUpdateLen, ", `set_value` = %u", p->bSetValue);
#endif

		char szItemQuery[QUERY_MAX_LEN + QUERY_MAX_LEN];
		snprintf(szItemQuery, sizeof(szItemQuery), "REPLACE INTO item%s (%s) VALUES(%s)", GetTablePostfix(), szColumns, szValues);

		if (g_test_server)
			sys_log(0, "ItemCache::Flush :REPLACE (%s)", szItemQuery);

		CDBManager::instance().ReturnQuery(szItemQuery, QID_ITEM_SAVE, 0, NULL);

		//g_item_info.Add(p->vnum);
		++g_item_count;
	}

	m_bNeedQuery = false;
}

//
// CPlayerTableCache
//
CPlayerTableCache::CPlayerTableCache()
{
	m_expireTime = MIN(1800, g_iPlayerCacheFlushSeconds);
}

CPlayerTableCache::~CPlayerTableCache()
{
}

void CPlayerTableCache::OnFlush()
{
	if (g_test_server)
		sys_log(0, "PlayerTableCache::Flush : %s", m_data.name);

	char szQuery[QUERY_MAX_LEN];
	CreatePlayerSaveQuery(szQuery, sizeof(szQuery), &m_data);
	CDBManager::instance().ReturnQuery(szQuery, QID_PLAYER_SAVE, 0, NULL);
}

// MYSHOP_PRICE_LIST
//
// CItemPriceListTableCache class implementation
//

const int CItemPriceListTableCache::s_nMinFlushSec = 1800;

CItemPriceListTableCache::CItemPriceListTableCache()
{
	m_expireTime = MIN(s_nMinFlushSec, g_iItemPriceListTableCacheFlushSeconds);
}

CItemPriceListTableCache::~CItemPriceListTableCache()
{
}

void CItemPriceListTableCache::UpdateList(const TItemPriceListTable* pUpdateList)
{
	//
	// 이미 캐싱된 아이템과 중복된 아이템을 찾고 중복되지 않는 이전 정보는 tmpvec 에 넣는다.
	//

	std::vector<TItemPriceInfo> tmpvec;

	for (uint idx = 0; idx < m_data.byCount; ++idx)
	{
		const TItemPriceInfo* pos = pUpdateList->aPriceInfo;
		for (; pos != pUpdateList->aPriceInfo + pUpdateList->byCount && m_data.aPriceInfo[idx].dwVnum != pos->dwVnum; ++pos);

		if (pos == pUpdateList->aPriceInfo + pUpdateList->byCount)
			tmpvec.push_back(m_data.aPriceInfo[idx]);
	}

	//
	// pUpdateList 를 m_data 에 복사하고 남은 공간을 tmpvec 의 앞에서 부터 남은 만큼 복사한다.
	//

	if (pUpdateList->byCount > SHOP_PRICELIST_MAX_NUM)
	{
		sys_err("Count overflow!");
		return;
	}

	m_data.byCount = pUpdateList->byCount;

	thecore_memcpy(m_data.aPriceInfo, pUpdateList->aPriceInfo, sizeof(TItemPriceInfo) * pUpdateList->byCount);

	int nDeletedNum; // 삭제된 가격정보의 갯수

	if (pUpdateList->byCount < SHOP_PRICELIST_MAX_NUM)
	{
		size_t sizeAddOldDataSize = SHOP_PRICELIST_MAX_NUM - pUpdateList->byCount;

		if (tmpvec.size() < sizeAddOldDataSize)
			sizeAddOldDataSize = tmpvec.size();

		if (!tmpvec.empty())
			thecore_memcpy(m_data.aPriceInfo + pUpdateList->byCount, &tmpvec[0], sizeof(TItemPriceInfo) * sizeAddOldDataSize);

		m_data.byCount += static_cast<BYTE>(sizeAddOldDataSize);

		nDeletedNum = static_cast<UINT>(tmpvec.size() - sizeAddOldDataSize);
	}
	else
		nDeletedNum = tmpvec.size();

	m_bNeedQuery = true;

	sys_log(0, "ItemPriceListTableCache::UpdateList : OwnerID[%u] Update [%u] Items, Delete [%u] Items, Total [%u] Items",
		m_data.dwOwnerID, pUpdateList->byCount, nDeletedNum, m_data.byCount);
}

void CItemPriceListTableCache::OnFlush()
{
	char szQuery[QUERY_MAX_LEN];

	//
	// 이 캐시의 소유자에 대한 기존에 DB 에 저장된 아이템 가격정보를 모두 삭제한다.
	//

	snprintf(szQuery, sizeof(szQuery), "DELETE FROM myshop_pricelist%s WHERE `owner_id` = %u", GetTablePostfix(), m_data.dwOwnerID);
	CDBManager::instance().ReturnQuery(szQuery, QID_ITEMPRICE_DESTROY, 0, NULL);

	//
	// 캐시의 내용을 모두 DB 에 쓴다.
	//

	for (int idx = 0; idx < m_data.byCount; ++idx)
	{
		snprintf(szQuery, sizeof(szQuery), "REPLACE myshop_pricelist%s (`owner_id`, `item_vnum`, `price`) VALUES(%u, %u, %u)",
			GetTablePostfix(), m_data.dwOwnerID, m_data.aPriceInfo[idx].dwVnum, m_data.aPriceInfo[idx].dwPrice);
		CDBManager::instance().ReturnQuery(szQuery, QID_ITEMPRICE_SAVE, 0, NULL);
	}

	sys_log(0, "ItemPriceListTableCache::Flush : OwnerID[%u] Update [%u]Items", m_data.dwOwnerID, m_data.byCount);

	m_bNeedQuery = false;
}
// END_OF_MYSHOP_PRICE_LIST
