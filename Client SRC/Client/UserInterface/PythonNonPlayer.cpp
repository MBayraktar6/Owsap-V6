#include "stdafx.h"
#include "../EterPack/EterPackManager.h"
#include "PythoNnonPlayer.h"
#include "InstanceBase.h"
#include "PythonCharacterManager.h"

bool CPythonNonPlayer::LoadNonPlayerData(const char* c_szFileName)
{
	CMappedFile file;
	LPCVOID pvData;

	Tracef("CPythonNonPlayer::LoadNonPlayerData: %s, sizeof(TMobTable)=%u\n", c_szFileName, sizeof(TMobTable));

	if (!CEterPackManager::Instance().Get(file, c_szFileName, &pvData))
		return false;

	DWORD dwFourCC, dwElements, dwDataSize;

	file.Read(&dwFourCC, sizeof(DWORD));

	if (dwFourCC != MAKEFOURCC('M', 'M', 'P', 'T'))
	{
		TraceError("CPythonNonPlayer::LoadNonPlayerData: invalid Mob proto type %s", c_szFileName);
		return false;
	}

	file.Read(&dwElements, sizeof(DWORD));
	file.Read(&dwDataSize, sizeof(DWORD));

	std::unique_ptr<BYTE[]> pbData = std::make_unique<BYTE[]>(dwDataSize);
	file.Read(pbData.get(), dwDataSize);

	CLZObject zObj;
	if (!CLZO::Instance().Decompress(zObj, pbData.get(), ETERPACK_MOB_PROTOCOL))
		return false;

	if ((zObj.GetSize() % sizeof(TMobTable)) != 0)
	{
		TraceError("CPythonNonPlayer::LoadNonPlayerData: invalid size %u check data format.", zObj.GetSize());
		return false;
	}

	TMobTable* pTable = (TMobTable*)zObj.GetBuffer();
	for (DWORD i = 0; i < dwElements; ++i, ++pTable)
	{
		TMobTable* pNonPlayerData = new TMobTable;

		memcpy(pNonPlayerData, pTable, sizeof(TMobTable));

		//TraceError("%d : %s type[%d] color[%d]", pNonPlayerData->dwVnum, pNonPlayerData->szLocaleName, pNonPlayerData->bType, pNonPlayerData->dwMonsterColor);
		m_NonPlayerDataMap.emplace(TNonPlayerDataMap::value_type(pNonPlayerData->dwVnum, pNonPlayerData));
	}

	return true;
}

bool CPythonNonPlayer::GetName(DWORD dwVnum, const char** c_pszName)
{
	const TMobTable* p = GetTable(dwVnum);

	if (!p)
		return false;

	*c_pszName = p->szLocaleName;

	return true;
}

BYTE CPythonNonPlayer::GetRank(DWORD dwVnum)
{
	const CPythonNonPlayer::TMobTable* c_pTable = GetTable(dwVnum);
	if (!c_pTable)
		return 0;

	return c_pTable->bRank;
}

bool CPythonNonPlayer::GetInstanceType(DWORD dwVnum, BYTE* pbType)
{
	const TMobTable* p = GetTable(dwVnum);

	// dwVnum를 찾을 수 없으면 플레이어 캐릭터로 간주 한다. 문제성 코드 -_- [cronan]
	if (!p)
		return false;

	*pbType = p->bType;

	return true;
}

CPythonNonPlayer::TMobTable* CPythonNonPlayer::GetTable(DWORD dwVnum)
{
	TNonPlayerDataMap::iterator itor = m_NonPlayerDataMap.find(dwVnum);

	if (itor == m_NonPlayerDataMap.end())
		return NULL;

	return itor->second;
}

BYTE CPythonNonPlayer::GetEventType(DWORD dwVnum)
{
	const TMobTable* p = GetTable(dwVnum);

	if (!p)
	{
		//Tracef("CPythonNonPlayer::GetEventType - Failed to find virtual number\n");
		return ON_CLICK_EVENT_NONE;
	}

	return p->bOnClickType;
}

BYTE CPythonNonPlayer::GetEventTypeByVID(DWORD dwVID)
{
	CInstanceBase* pInstance = CPythonCharacterManager::Instance().GetInstancePtr(dwVID);

	if (NULL == pInstance)
	{
		//Tracef("CPythonNonPlayer::GetEventTypeByVID - There is no Virtual Number\n");
		return ON_CLICK_EVENT_NONE;
	}

	WORD dwVnum = pInstance->GetVirtualNumber();
	return GetEventType(dwVnum);
}

const char* CPythonNonPlayer::GetMonsterName(DWORD dwVnum)
{
	const CPythonNonPlayer::TMobTable* c_pTable = GetTable(dwVnum);
	if (!c_pTable)
	{
		static const char* sc_szEmpty = "";
		return sc_szEmpty;
	}

	return c_pTable->szLocaleName;
}

DWORD CPythonNonPlayer::GetMonsterColor(DWORD dwVnum)
{
	const CPythonNonPlayer::TMobTable* c_pTable = GetTable(dwVnum);
	if (!c_pTable)
		return 0;

	return c_pTable->dwMobColor;
}

void CPythonNonPlayer::GetMatchableMobList(int iLevel, int iInterval, TMobTableList* pMobTableList)
{
	/*
	pMobTableList->clear();

	TNonPlayerDataMap::iterator itor = m_NonPlayerDataMap.begin();
	for (; itor != m_NonPlayerDataMap.end(); ++itor)
	{
		TMobTable* pMobTable = itor->second;

		int iLowerLevelLimit = iLevel - iInterval;
		int iUpperLevelLimit = iLevel + iInterval;

		if ((pMobTable->abLevelRange[0] >= iLowerLevelLimit && pMobTable->abLevelRange[0] <= iUpperLevelLimit) ||
			(pMobTable->abLevelRange[1] >= iLowerLevelLimit && pMobTable->abLevelRange[1] <= iUpperLevelLimit))
		{
			pMobTableList->push_back(pMobTable);
		}
	}
	*/
}

#if defined(ENABLE_SEND_TARGET_INFO)
BOOL CPythonNonPlayer::SelectMobData(DWORD dwRaceVnum)
{
	CPythonNonPlayer::TMobTable* pTable = GetTable(dwRaceVnum);
	if (NULL == pTable)
		return FALSE;

	m_pSelectedMobData = pTable;
	return TRUE;
}

CPythonNonPlayer::TMobTable* CPythonNonPlayer::GetSelectedMobDataPointer()
{
	return m_pSelectedMobData;
}

// Monster Item Drop
void CPythonNonPlayer::AddItemDrop(const DWORD dwRaceVnum, const DWORD dwVnum, const WORD wCount)
{
	TMobDropItemMap::iterator it = m_MobItemDropMap.find(dwRaceVnum);
	if (it != m_MobItemDropMap.end())
	{
		TMobDropItemVec& rVec = it->second;
		const TMobDropItemVec::iterator vFind = std::find_if(rVec.begin(), rVec.end(),
			[dwVnum](const SMobItemDrop& rItem) { return rItem.dwVnum == dwVnum; });

		if (vFind != rVec.end())
			return;

		it->second.push_back({ dwVnum, wCount });
	}
	else
		m_MobItemDropMap[dwRaceVnum].push_back({ dwVnum, wCount });
}

void CPythonNonPlayer::ClearItemDrop()
{
	for (TMobDropItemMap::value_type& it : m_MobItemDropMap)
		it.second.clear();

	m_MobItemDropMap.clear();
}

CPythonNonPlayer::TMobDropItemVec* CPythonNonPlayer::GetItemDropVec(const DWORD dwVnum)
{
	TMobDropItemMap::iterator it = m_MobItemDropMap.find(dwVnum);
	if (it != m_MobItemDropMap.end())
		return &(it->second);

	return nullptr;
}
#endif

BYTE CPythonNonPlayer::GetMonsterScalePercent(DWORD dwVnum)
{
	const CPythonNonPlayer::TMobTable* c_pTable = GetTable(dwVnum);
	if (!c_pTable)
		return 100;

	return MINMAX(50, c_pTable->bScale, 200);
}

float CPythonNonPlayer::GetMonsterHitRange(DWORD dwVnum)
{
	const CPythonNonPlayer::TMobTable* c_pTable = GetTable(dwVnum);
	if (!c_pTable)
		return 70.0f;

	if (c_pTable->fHitRange)
		return c_pTable->fHitRange;

	return 100.0f;
}

void CPythonNonPlayer::Clear()
{
#if defined(ENABLE_SEND_TARGET_INFO)
	m_pSelectedMobData = NULL;
#endif
}

void CPythonNonPlayer::Destroy()
{
	stl_wipe_second(m_NonPlayerDataMap);
	m_NonPlayerDataMap.clear();

#if defined(ENABLE_SEND_TARGET_INFO)
	m_pSelectedMobData = NULL;
	m_MobItemDropMap.clear();
#endif
}

CPythonNonPlayer::CPythonNonPlayer()
{
	Clear();
}

CPythonNonPlayer::~CPythonNonPlayer(void)
{
	Destroy();
}
