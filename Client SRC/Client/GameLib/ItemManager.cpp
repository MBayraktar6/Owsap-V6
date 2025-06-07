#include "StdAfx.h"

#include "../EterPack/EterPackManager.h"
#include "../EterLib/ResourceManager.h"

#include "ItemManager.h"

BOOL CItemManager::SelectItemData(DWORD dwIndex)
{
	TItemMap::iterator f = m_ItemMap.find(dwIndex);

	if (m_ItemMap.end() == f)
	{
		int n = m_vec_ItemRange.size();
		for (int i = 0; i < n; i++)
		{
			CItemData* p = m_vec_ItemRange[i];
			const CItemData::TItemTable* pTable = p->GetTable();
			if ((pTable->dwVnum < dwIndex) &&
				dwIndex < (pTable->dwVnum + pTable->dwVnumRange))
			{
				m_pSelectedItemData = p;
				return true;
			}
		}
		// Tracef("CItemManager::SelectItemData - FIND ERROR [%d]\n", dwIndex);
		return false;
	}

	m_pSelectedItemData = f->second;

	return true;
}

CItemData* CItemManager::GetSelectedItemDataPointer()
{
	return m_pSelectedItemData;
}

BOOL CItemManager::GetItemDataPointer(DWORD dwItemID, CItemData** ppItemData)
{
	if (0 == dwItemID)
		return false;

	TItemMap::iterator f = m_ItemMap.find(dwItemID);

	if (m_ItemMap.end() == f)
	{
		int n = m_vec_ItemRange.size();
		for (int i = 0; i < n; i++)
		{
			CItemData* p = m_vec_ItemRange[i];
			const CItemData::TItemTable* pTable = p->GetTable();
			if ((pTable->dwVnum < dwItemID) &&
				dwItemID < (pTable->dwVnum + pTable->dwVnumRange))
			{
				*ppItemData = p;
				return true;
			}
		}

		Tracef("CItemManager::GetItemDataPointer - FIND ERROR [%d]\n", dwItemID);
		return false;
	}

	*ppItemData = f->second;

	return true;
}

CItemData* CItemManager::MakeItemData(DWORD dwIndex)
{
	TItemMap::iterator f = m_ItemMap.find(dwIndex);

	if (m_ItemMap.end() == f)
	{
		CItemData* pItemData = CItemData::New();

		m_ItemMap.insert(TItemMap::value_type(dwIndex, pItemData));

		return pItemData;
	}

	return f->second;
}

////////////////////////////////////////////////////////////////////////////////////////
// Load Item Table

bool CItemManager::LoadItemList(const char* c_szFileName)
{
	CMappedFile File;
	LPCVOID pData;

	if (!CEterPackManager::Instance().Get(File, c_szFileName, &pData))
		return false;

	CMemoryTextFileLoader textFileLoader;
	textFileLoader.Bind(File.Size(), pData);

	CTokenVector TokenVector;
	for (DWORD i = 0; i < textFileLoader.GetLineCount(); ++i)
	{
		if (!textFileLoader.SplitLine(i, &TokenVector, "\t"))
			continue;

		if (!(TokenVector.size() == 3 || TokenVector.size() == 4))
		{
			TraceError(" CItemManager::LoadItemList(%s) - StrangeLine in %d\n", c_szFileName, i);
			continue;
		}

		const std::string& c_rstrID = TokenVector[0];
		const std::string& c_rstrType = TokenVector[1];
		const std::string& c_rstrIcon = TokenVector[2];

		DWORD dwItemVNum = atoi(c_rstrID.c_str());

		CItemData* pItemData = MakeItemData(dwItemVNum);

		extern BOOL USE_VIETNAM_CONVERT_WEAPON_VNUM;
		if (USE_VIETNAM_CONVERT_WEAPON_VNUM)
		{
			extern DWORD Vietnam_ConvertWeaponVnum(DWORD vnum);
			DWORD dwMildItemVnum = Vietnam_ConvertWeaponVnum(dwItemVNum);
			if (dwMildItemVnum == dwItemVNum)
			{
				if (4 == TokenVector.size())
				{
#if defined(ENABLE_AURA_COSTUME_SYSTEM)
					if (!strcmp(c_rstrType.c_str(), "AURA"))
					{
						const std::string& c_rstrAuraEffectFileName = TokenVector[3];
						pItemData->SetAuraEffectID(c_rstrAuraEffectFileName.c_str());
						pItemData->SetDefaultItemData(c_rstrIcon.c_str());
					}
					else
					{
						const std::string& c_rstrModelFileName = TokenVector[3];
						pItemData->SetDefaultItemData(c_rstrIcon.c_str(), c_rstrModelFileName.c_str());
					}
#else
					const std::string& c_rstrModelFileName = TokenVector[3];
					pItemData->SetDefaultItemData(c_rstrIcon.c_str(), c_rstrModelFileName.c_str());
#endif
				}
				else
				{
					pItemData->SetDefaultItemData(c_rstrIcon.c_str());
				}
			}
			else
			{
				DWORD dwMildBaseVnum = dwMildItemVnum / 10 * 10;
				char szMildIconPath[MAX_PATH];
				sprintf(szMildIconPath, "icon/item/%.5d.tga", dwMildBaseVnum);
				if (4 == TokenVector.size())
				{
					char szMildModelPath[MAX_PATH];
					sprintf(szMildModelPath, "d:/ymir work/item/weapon/%.5d.gr2", dwMildBaseVnum);
					pItemData->SetDefaultItemData(szMildIconPath, szMildModelPath);
				}
				else
				{
					pItemData->SetDefaultItemData(szMildIconPath);
				}
			}
		}
		else
		{
			if (4 == TokenVector.size())
			{
#if defined(ENABLE_AURA_COSTUME_SYSTEM)
				if (!strcmp(c_rstrType.c_str(), "AURA"))
				{
					const std::string& c_rstrAuraEffectFileName = TokenVector[3];
					pItemData->SetAuraEffectID(c_rstrAuraEffectFileName.c_str());
					pItemData->SetDefaultItemData(c_rstrIcon.c_str());
				}
				else
				{
					const std::string& c_rstrModelFileName = TokenVector[3];
					pItemData->SetDefaultItemData(c_rstrIcon.c_str(), c_rstrModelFileName.c_str());
				}
#else
				const std::string& c_rstrModelFileName = TokenVector[3];
				pItemData->SetDefaultItemData(c_rstrIcon.c_str(), c_rstrModelFileName.c_str());
#endif
			}
			else
			{
				pItemData->SetDefaultItemData(c_rstrIcon.c_str());
			}
		}
	}

	return true;
}

const std::string& __SnapString(const std::string& c_rstSrc, std::string& rstTemp)
{
	UINT uSrcLen = c_rstSrc.length();
	if (uSrcLen < 2)
		return c_rstSrc;

	if (c_rstSrc[0] != '"')
		return c_rstSrc;

	UINT uLeftCut = 1;

	UINT uRightCut = uSrcLen;
	if (c_rstSrc[uSrcLen - 1] == '"')
		uRightCut = uSrcLen - 1;

	rstTemp = c_rstSrc.substr(uLeftCut, uRightCut - uLeftCut);
	return rstTemp;
}

bool CItemManager::LoadItemDesc(const char* c_szFileName)
{
	const VOID* pvData;
	CMappedFile kFile;
	if (!CEterPackManager::Instance().Get(kFile, c_szFileName, &pvData))
	{
		Tracenf("CItemManager::LoadItemDesc(c_szFileName=%s) - Load Error", c_szFileName);
		return false;
	}

	CMemoryTextFileLoader kTextFileLoader;
	kTextFileLoader.Bind(kFile.Size(), pvData);

	std::string stTemp;

	CTokenVector kTokenVector;
	for (DWORD i = 0; i < kTextFileLoader.GetLineCount(); ++i)
	{
		if (!kTextFileLoader.SplitLineByTab(i, &kTokenVector))
			continue;

		while (kTokenVector.size() < ITEMDESC_COL_NUM)
			kTokenVector.push_back("");

		//assert(kTokenVector.size()==ITEMDESC_COL_NUM);

		DWORD dwVnum = atoi(kTokenVector[ITEMDESC_COL_VNUM].c_str());
		const std::string& c_rstDesc = kTokenVector[ITEMDESC_COL_DESC];
		const std::string& c_rstSumm = kTokenVector[ITEMDESC_COL_SUMM];
		TItemMap::iterator f = m_ItemMap.find(dwVnum);
		if (m_ItemMap.end() == f)
			continue;

		CItemData* pkItemDataFind = f->second;

		pkItemDataFind->SetDescription(__SnapString(c_rstDesc, stTemp));
		pkItemDataFind->SetSummary(__SnapString(c_rstSumm, stTemp));
	}
	return true;
}

DWORD GetHashCode(const char* pString)
{
	unsigned long i, len;
	unsigned long ch;
	unsigned long result;

	len = strlen(pString);
	result = 5381;
	for (i = 0; i < len; i++)
	{
		ch = (unsigned long)pString[i];
		result = ((result << 5) + result) + ch; // hash * 33 + ch
	}

	return result;
}

bool CItemManager::LoadItemTable(const char* c_szFileName)
{
	CMappedFile file;
	LPCVOID pvData;

	if (!CEterPackManager::Instance().Get(file, c_szFileName, &pvData))
		return false;

	DWORD dwFourCC, dwElements, dwDataSize;
	DWORD dwVersion = 0;
	DWORD dwStride = 0;

	file.Read(&dwFourCC, sizeof(DWORD));

	if (dwFourCC == MAKEFOURCC('M', 'I', 'P', 'X'))
	{
		file.Read(&dwVersion, sizeof(DWORD));
		file.Read(&dwStride, sizeof(DWORD));

		if (dwVersion != 1)
		{
			TraceError("CPythonItem::LoadItemTable: invalid item_proto[%s] VERSION[%d]", c_szFileName, dwVersion);
			return false;
		}
		if (dwStride != sizeof(CItemData::TItemTable))
		{
			TraceError("CPythonItem::LoadItemTable: invalid item_proto[%s] STRIDE[%d] != sizeof(SItemTable)",
				c_szFileName, dwStride, sizeof(CItemData::TItemTable));
			return false;
		}
	}
	else if (dwFourCC != MAKEFOURCC('M', 'I', 'P', 'T'))
	{
		TraceError("CPythonItem::LoadItemTable: invalid item proto type %s", c_szFileName);
		return false;
	}

	file.Read(&dwElements, sizeof(DWORD));
	file.Read(&dwDataSize, sizeof(DWORD));

	std::unique_ptr<BYTE[]> pbData = std::make_unique<BYTE[]>(dwDataSize);
	file.Read(pbData.get(), dwDataSize);

	/////

	CLZObject zObj;
	if (!CLZO::Instance().Decompress(zObj, pbData.get(), ETERPACK_ITEM_PROTOCOL))
		return false;

	/////

	char szName[64 + 1];

	CItemData::TItemTable* table = (CItemData::TItemTable*)zObj.GetBuffer();
	std::map<DWORD, DWORD> itemNameMap;

	for (DWORD i = 0; i < dwElements; ++i, ++table)
	{
		CItemData* pItemData;
		DWORD dwVnum = table->dwVnum;

		TItemMap::iterator f = m_ItemMap.find(dwVnum);
		if (m_ItemMap.end() == f)
		{
			_snprintf(szName, sizeof(szName), "icon/item/%05d.tga", dwVnum);

			if (CResourceManager::Instance().IsFileExist(szName) == false)
			{
				std::map<DWORD, DWORD>::iterator itVnum = itemNameMap.find(GetHashCode(table->szName));

				if (itVnum != itemNameMap.end())
					_snprintf(szName, sizeof(szName), "icon/item/%05d.tga", itVnum->second);
				else
					_snprintf(szName, sizeof(szName), "icon/item/%05d.tga", dwVnum - dwVnum % 10);

				if (CResourceManager::Instance().IsFileExist(szName) == false)
				{
#ifdef _DEBUG
					TraceError("%16s(#%-5d) cannot find icon file. setting to default.", table->szName, dwVnum);
#endif
					const DWORD EmptyBowl = 27995;
					_snprintf(szName, sizeof(szName), "icon/item/%05d.tga", EmptyBowl);
				}
			}

			pItemData = CItemData::New();

			pItemData->SetDefaultItemData(szName);
			m_ItemMap.insert(TItemMap::value_type(dwVnum, pItemData));
		}
		else
		{
			pItemData = f->second;
		}
		if (itemNameMap.find(GetHashCode(table->szName)) == itemNameMap.end())
			itemNameMap.insert(std::map<DWORD, DWORD>::value_type(GetHashCode(table->szName), table->dwVnum));
		pItemData->SetItemTableData(table);
		if (0 != table->dwVnumRange)
		{
			m_vec_ItemRange.push_back(pItemData);
		}
	}

	//!@#
	//	CItemData::TItemTable * table = (CItemData::TItemTable *) zObj.GetBuffer();
	//	for (DWORD i = 0; i < dwElements; ++i, ++table)
	//	{
	//		CItemData * pItemData;
	//		DWORD dwVnum = table->dwVnum;
	//
	//		TItemMap::iterator f = m_ItemMap.find(dwVnum);
	//
	//		if (m_ItemMap.end() == f)
	//		{
	//			pItemData = CItemData::New();
	//
	//			pItemData->LoadItemData(_getf("d:/ymir work/item/%05d.msm", dwVnum));
	//			m_ItemMap.insert(TItemMap::value_type(dwVnum, pItemData));
	//		}
	//		else
	//		{
	//			pItemData = f->second;
	//		}
	//		pItemData->SetItemTableData(table);
	//	}

	return true;
}

#if defined(ENABLE_ACCE_COSTUME_SYSTEM) || defined(ENABLE_AURA_COSTUME_SYSTEM)
bool CItemManager::LoadItemScale(const char* c_szFileName)
{
	CMappedFile File;
	LPCVOID pData;
	if (!CEterPackManager::Instance().Get(File, c_szFileName, &pData))
		return false;

	CMemoryTextFileLoader TextFileLoader;
	TextFileLoader.Bind(File.Size(), pData);

	CTokenVector TokenVector;
	for (DWORD i = 0; i < TextFileLoader.GetLineCount(); ++i)
	{
		if (!TextFileLoader.SplitLine(i, &TokenVector, "\t"))
			continue;

		if (!(TokenVector.size() == 6 || TokenVector.size() == 7))
		{
			TraceError(" CItemManager::LoadItemScale(%s) - Error on line %d\n", c_szFileName, i);
			continue;
		}

		const std::string& c_rstrID = TokenVector[ITEMSCALE_COL_VNUM];
		const std::string& c_rstrJob = TokenVector[ITEMSCALE_COL_JOB];
		const std::string& c_rstrSex = TokenVector[ITEMSCALE_COL_SEX];
		const std::string& c_rstrScaleX = TokenVector[ITEMSCALE_COL_SCALE_X];
		const std::string& c_rstrScaleY = TokenVector[ITEMSCALE_COL_SCALE_Y];
		const std::string& c_rstrScaleZ = TokenVector[ITEMSCALE_COL_SCALE_Z];

		DWORD dwItemVnum = atoi(c_rstrID.c_str());
		BYTE byJob = 0;
		if (!strcmp(c_rstrJob.c_str(), "JOB_WARRIOR")) byJob = NRaceData::JOB_WARRIOR;
		if (!strcmp(c_rstrJob.c_str(), "JOB_ASSASSIN")) byJob = NRaceData::JOB_ASSASSIN;
		if (!strcmp(c_rstrJob.c_str(), "JOB_SURA")) byJob = NRaceData::JOB_SURA;
		if (!strcmp(c_rstrJob.c_str(), "JOB_SHAMAN")) byJob = NRaceData::JOB_SHAMAN;
		if (!strcmp(c_rstrJob.c_str(), "JOB_WOLFMAN")) byJob = NRaceData::JOB_WOLFMAN;
		BYTE bySex = c_rstrSex[0] == 'M';

		float fScaleX = atof(c_rstrScaleX.c_str()) * 0.01f;
		float fScaleY = atof(c_rstrScaleY.c_str()) * 0.01f;
		float fScaleZ = atof(c_rstrScaleZ.c_str()) * 0.01f;
		float fParticleScale = 1.0f;
		if (TokenVector.size() == 7)
		{
			const std::string& c_rstrParticleScale = TokenVector[ITEMSCALE_COL_PARTICLE_SCALE];
			fParticleScale = atof(c_rstrParticleScale.c_str());
		}

		CItemData* pItemData = MakeItemData(dwItemVnum);
		BYTE bGradeMax = 0;

#if defined(ENABLE_ACCE_COSTUME_SYSTEM)
		if (pItemData->GetType() == CItemData::ITEM_TYPE_COSTUME && pItemData->GetSubType() == CItemData::COSTUME_ACCE)
			bGradeMax = CItemData::ACCE_GRADE_MAX_NUM;
#endif

#if defined(ENABLE_AURA_COSTUME_SYSTEM)
		if (pItemData->GetType() == CItemData::ITEM_TYPE_COSTUME && pItemData->GetSubType() == CItemData::COSTUME_AURA)
			bGradeMax = CItemData::AURA_GRADE_MAX_NUM;
#endif

		for (int i = 0; i < bGradeMax; ++i)
		{
			pItemData = MakeItemData(dwItemVnum + i);
			if (pItemData)
				pItemData->SetItemTableScaleData(byJob, bySex, fScaleX, fScaleY, fScaleZ, fParticleScale);
		}
	}

	return true;
}
#endif

float CItemManager::GetDSSetWeight(BYTE bDSType, BYTE bDSGrade) const
{
	float fResult = 0.0f;
	if (NULL != m_pDSTable)
		if (!m_pDSTable->GetWeight(bDSType, /* EDragonSoulStepTypes */ (bDSGrade - 1), 0, 0, fResult))
			TraceError("Failed to get DSSetWeight type(%u) grade(%u)", bDSType, bDSGrade);

	return fResult;
}

int CItemManager::GetDSBasicApplyCount(BYTE bDSType, BYTE bDSGrade) const
{
	int iBasicApplyNum = 0;
	int iAddMin, iAddMax;
	if (NULL != m_pDSTable)
		if (!m_pDSTable->GetApplyNumSettings(bDSType, bDSGrade, iBasicApplyNum, iAddMin, iAddMax))
			TraceError("Failed to get DSBasicApplyCount type(%u) grade(%u)", bDSType, bDSGrade);

	return iBasicApplyNum;
}

POINT_VALUE CItemManager::GetDSBasicApplyValue(BYTE bDSType, POINT_TYPE wApplyType) const
{
	POINT_VALUE lApplyValue = 0;
	CDragonSoulTable::TVecApplys vecBasicApplys;
	if (NULL != m_pDSTable)
	{
		if (!m_pDSTable->GetBasicApplys(bDSType, vecBasicApplys))
		{
			TraceError("There is no BasicApply about %u type dragon soul.", bDSType);
			return 0;
		}
		for (CDragonSoulTable::TVecApplys::const_iterator it = vecBasicApplys.begin(); it != vecBasicApplys.end(); ++it)
		{
			const CDragonSoulTable::SApply& apply = *it;
			if (apply.apply_type == wApplyType)
			{
				lApplyValue = apply.apply_value;
				break;
			}
		}
	}
	return lApplyValue;
}

POINT_VALUE CItemManager::GetDSAdditionalApplyValue(BYTE bDSType, POINT_TYPE wApplyType) const
{
	POINT_VALUE lApplyValue = 0;
	CDragonSoulTable::TVecApplys vecAdditionalApplys;
	if (NULL != m_pDSTable)
	{
		if (!m_pDSTable->GetAdditionalApplys(bDSType, vecAdditionalApplys))
		{
			TraceError("There is no AdditionalApply about %u type dragon soul.", bDSType);
			return 0;
		}
		for (CDragonSoulTable::TVecApplys::const_iterator it = vecAdditionalApplys.begin(); it != vecAdditionalApplys.end(); ++it)
		{
			const CDragonSoulTable::SApply& apply = *it;
			if (apply.apply_type == wApplyType)
			{
				lApplyValue = apply.apply_value;
				break;
			}
		}
	}
	return lApplyValue;
}

bool CItemManager::LoadDragonSoulTable(const char* c_szFileName)
{
	if (NULL != m_pDSTable)
	{
		m_pDSTable->Clear();
		delete m_pDSTable;
		m_pDSTable = NULL;
	}

	m_pDSTable = new CDragonSoulTable;
	return m_pDSTable->ReadDragonSoulTableFile(c_szFileName);
}

void CItemManager::Destroy()
{
	TItemMap::iterator i;
	for (i = m_ItemMap.begin(); i != m_ItemMap.end(); ++i)
		CItemData::Delete(i->second);

	m_ItemMap.clear();

	if (NULL != m_pDSTable)
	{
		m_pDSTable->Clear();
		delete m_pDSTable;
		m_pDSTable = NULL;
	}
}

CItemManager::CItemManager() : m_pSelectedItemData(NULL), m_pDSTable(NULL)
{
}

CItemManager::~CItemManager()
{
	Destroy();
}
