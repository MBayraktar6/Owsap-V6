#include "StdAfx.h"

#include "../EterLib/ResourceManager.h"
#if defined(ENABLE_AURA_COSTUME_SYSTEM)
#	include "../EffectLib/EffectManager.h"
#endif
#include "ItemData.h"

CDynamicPool<CItemData> CItemData::ms_kPool;

extern DWORD GetDefaultCodePage();

CItemData* CItemData::New()
{
	return ms_kPool.Alloc();
}

void CItemData::Delete(CItemData* pkItemData)
{
	pkItemData->Clear();
	ms_kPool.Free(pkItemData);
}

void CItemData::DestroySystem()
{
	ms_kPool.Destroy();
}

CGraphicThing* CItemData::GetModelThing()
{
	return m_pModelThing;
}

CGraphicThing* CItemData::GetSubModelThing()
{
	if (m_pSubModelThing)
		return m_pSubModelThing;
	else
		return m_pModelThing;
}

CGraphicThing* CItemData::GetDropModelThing()
{
	return m_pDropModelThing;
}

CGraphicSubImage* CItemData::GetIconImage()
{
	if (m_pIconImage == NULL && m_strIconFileName.empty() == false)
		__SetIconImage(m_strIconFileName.c_str());
	return m_pIconImage;
}

DWORD CItemData::GetLODModelThingCount()
{
	return m_pLODModelThingVector.size();
}

BOOL CItemData::GetLODModelThingPointer(DWORD dwIndex, CGraphicThing** ppModelThing)
{
	if (dwIndex >= m_pLODModelThingVector.size())
		return FALSE;

	*ppModelThing = m_pLODModelThingVector[dwIndex];

	return TRUE;
}

DWORD CItemData::GetAttachingDataCount()
{
	return m_AttachingDataVector.size();
}

BOOL CItemData::GetCollisionDataPointer(DWORD dwIndex, const NRaceData::TAttachingData** c_ppAttachingData)
{
	if (dwIndex >= GetAttachingDataCount())
		return FALSE;

	if (NRaceData::ATTACHING_DATA_TYPE_COLLISION_DATA != m_AttachingDataVector[dwIndex].dwType)
		return FALSE;

	*c_ppAttachingData = &m_AttachingDataVector[dwIndex];
	return TRUE;
}

BOOL CItemData::GetAttachingDataPointer(DWORD dwIndex, const NRaceData::TAttachingData** c_ppAttachingData)
{
	if (dwIndex >= GetAttachingDataCount())
		return FALSE;

	*c_ppAttachingData = &m_AttachingDataVector[dwIndex];
	return TRUE;
}

void CItemData::SetSummary(const std::string& c_rstSumm)
{
	m_strSummary = c_rstSumm;
}

void CItemData::SetDescription(const std::string& c_rstDesc)
{
	m_strDescription = c_rstDesc;
}
/*
BOOL CItemData::LoadItemData(const char * c_szFileName)
{
	CTextFileLoader TextFileLoader;

	if (!TextFileLoader.Load(c_szFileName))
	{
		//Lognf(1, "CItemData::LoadItemData(c_szFileName=%s) - FAILED", c_szFileName);
		return FALSE;
	}

	TextFileLoader.SetTop();

	TextFileLoader.GetTokenString("modelfilename", &m_strModelFileName);
	TextFileLoader.GetTokenString("submodelfilename", &m_strSubModelFileName);
	TextFileLoader.GetTokenString("dropmodelfilename", &m_strDropModelFileName);
	TextFileLoader.GetTokenString("iconimagefilename", &m_strIconFileName);

	char szDescriptionKey[32+1];
	_snprintf(szDescriptionKey, 32, "%ddescription", GetDefaultCodePage());
	if (!TextFileLoader.GetTokenString(szDescriptionKey, &m_strDescription))
	{
		TextFileLoader.GetTokenString("description", &m_strDescription);
	}

	// LOD Model File Name List
	CTokenVector * pLODModelList;
	if (TextFileLoader.GetTokenVector("lodmodellist", &pLODModelList))
	{
		m_strLODModelFileNameVector.clear();
		m_strLODModelFileNameVector.resize(pLODModelList->size());

		for (DWORD i = 0; i < pLODModelList->size(); ++i)
		{
			m_strLODModelFileNameVector[i] = pLODModelList->at(0);
		}
	}

	// Attaching Data
	// Item 에 Attaching Data 일단 없음.
//	if (TextFileLoader.SetChildNode("attachingdata"))
//	{
//		if (!NRaceData::LoadAttachingData(TextFileLoader, &m_AttachingDataVector))
//			return FALSE;
//
//		TextFileLoader.SetParentNode();
//	}

	__LoadFiles();

	return TRUE;
}
*/
void CItemData::SetDefaultItemData(const char* c_szIconFileName, const char* c_szModelFileName)
{
	if (c_szModelFileName)
	{
		m_strModelFileName = c_szModelFileName;
		m_strDropModelFileName = c_szModelFileName;
	}
	else
	{
		m_strModelFileName = "";
		m_strDropModelFileName = "d:/ymir work/item/etc/item_bag.gr2";
	}
	m_strIconFileName = c_szIconFileName;

	m_strSubModelFileName = "";
	m_strDescription = "";
	m_strSummary = "";
	memset(m_ItemTable.alSockets, 0, sizeof(m_ItemTable.alSockets));

	__LoadFiles();
}

void CItemData::__LoadFiles()
{
	// Model File Name
	if (!m_strModelFileName.empty())
		m_pModelThing = (CGraphicThing*)CResourceManager::Instance().GetResourcePointer(m_strModelFileName.c_str());

	if (!m_strSubModelFileName.empty())
		m_pSubModelThing = (CGraphicThing*)CResourceManager::Instance().GetResourcePointer(m_strSubModelFileName.c_str());

	if (!m_strDropModelFileName.empty())
		m_pDropModelThing = (CGraphicThing*)CResourceManager::Instance().GetResourcePointer(m_strDropModelFileName.c_str());


	if (!m_strLODModelFileNameVector.empty())
	{
		m_pLODModelThingVector.clear();
		m_pLODModelThingVector.resize(m_strLODModelFileNameVector.size());

		for (DWORD i = 0; i < m_strLODModelFileNameVector.size(); ++i)
		{
			const std::string& c_rstrLODModelFileName = m_strLODModelFileNameVector[i];
			m_pLODModelThingVector[i] = (CGraphicThing*)CResourceManager::Instance().GetResourcePointer(c_rstrLODModelFileName.c_str());
		}
	}
}

void CItemData::__SetIconImage(const char* c_szFileName)
{
	if (!CResourceManager::Instance().IsFileExist(c_szFileName))
	{
		TraceError("%s 파일이 없습니다.CItemData::__SetIconImage", c_szFileName);
		m_pIconImage = NULL;
	}
	else if (m_pIconImage == NULL)
		m_pIconImage = (CGraphicSubImage*)CResourceManager::Instance().GetResourcePointer(c_szFileName);
}

void CItemData::SetItemTableData(TItemTable* pItemTable)
{
	memcpy(&m_ItemTable, pItemTable, sizeof(TItemTable));
}

const CItemData::TItemTable* CItemData::GetTable() const
{
	return &m_ItemTable;
}

DWORD CItemData::GetIndex() const
{
	return m_ItemTable.dwVnum;
}

const char* CItemData::GetName() const
{
	return m_ItemTable.szLocaleName;
}

const char* CItemData::GetDescription() const
{
	return m_strDescription.c_str();
}

const char* CItemData::GetSummary() const
{
	return m_strSummary.c_str();
}

BYTE CItemData::GetType() const
{
	return m_ItemTable.bType;
}

BYTE CItemData::GetSubType() const
{
	return m_ItemTable.bSubType;
}

uint64_t CItemData::GetAntiFlags() const
{
	return m_ItemTable.ullAntiFlags;
}

#define DEF_STR(x) #x

const char* CItemData::GetUseTypeString() const
{
	if (GetType() != CItemData::ITEM_TYPE_USE)
		return "NOT_USE_TYPE";

	switch (GetSubType())
	{
		case USE_TUNING:
			return DEF_STR(USE_TUNING);
		case USE_DETACHMENT:
			return DEF_STR(USE_DETACHMENT);
		case USE_CLEAN_SOCKET:
			return DEF_STR(USE_CLEAN_SOCKET);
		case USE_CHANGE_ATTRIBUTE:
			return DEF_STR(USE_CHANGE_ATTRIBUTE);
#if defined(ENABLE_ATTR_6TH_7TH)
		case USE_CHANGE_ATTRIBUTE2:
			return DEF_STR(USE_CHANGE_ATTRIBUTE2);
#endif
		case USE_ADD_ATTRIBUTE:
			return DEF_STR(USE_ADD_ATTRIBUTE);
		case USE_ADD_ATTRIBUTE2:
			return DEF_STR(USE_ADD_ATTRIBUTE2);
		case USE_ADD_ACCESSORY_SOCKET:
			return DEF_STR(USE_ADD_ACCESSORY_SOCKET);
		case USE_PUT_INTO_ACCESSORY_SOCKET:
			return DEF_STR(USE_PUT_INTO_ACCESSORY_SOCKET);
		case USE_PUT_INTO_BELT_SOCKET:
			return DEF_STR(USE_PUT_INTO_BELT_SOCKET);
		case USE_PUT_INTO_RING_SOCKET:
			return DEF_STR(USE_PUT_INTO_RING_SOCKET);
#if defined(ENABLE_MOVE_COSTUME_ATTR)
		case USE_RESET_COSTUME_ATTR:
			return DEF_STR(USE_RESET_COSTUME_ATTR);
		case USE_CHANGE_COSTUME_ATTR:
			return DEF_STR(USE_CHANGE_COSTUME_ATTR);
#endif
#if defined(ENABLE_CHANGED_ATTR)
		case USE_SELECT_ATTRIBUTE:
			return DEF_STR(USE_SELECT_ATTRIBUTE);
#endif
#if defined(ENABLE_REFINE_ELEMENT_SYSTEM)
		case USE_ELEMENT_UPGRADE:
			return DEF_STR(USE_ELEMENT_UPGRADE);
		case USE_ELEMENT_DOWNGRADE:
			return DEF_STR(USE_ELEMENT_DOWNGRADE);
		case USE_ELEMENT_CHANGE:
			return DEF_STR(USE_ELEMENT_CHANGE);
#endif
	}
	return "USE_UNKNOWN_TYPE";
}


DWORD CItemData::GetWeaponType() const
{
#if defined(ENABLE_WEAPON_COSTUME_SYSTEM)
	if (m_ItemTable.bType == ITEM_TYPE_COSTUME && m_ItemTable.bSubType == COSTUME_WEAPON)
	{
		return m_ItemTable.alValues[3];
	}
#endif

	return m_ItemTable.bSubType;
}

BYTE CItemData::GetSize() const
{
	return m_ItemTable.bSize;
}

BOOL CItemData::IsAntiFlag(uint64_t dwFlag) const
{
	return (dwFlag & m_ItemTable.ullAntiFlags) != 0;
}

BOOL CItemData::IsFlag(DWORD dwFlag) const
{
	return (dwFlag & m_ItemTable.dwFlags) != 0;
}

BOOL CItemData::IsWearableFlag(DWORD dwFlag) const
{
	return (dwFlag & m_ItemTable.dwWearFlags) != 0;
}

#include "../GameLib/ItemData.h"
BOOL CItemData::IsWearableByJob(BYTE bJob) const
{
	const std::map<BYTE, BOOL> bFlagList{
		{ NRaceData::EJobs::JOB_WARRIOR, !IsAntiFlag(CItemData::ITEM_ANTIFLAG_WARRIOR) },
		{ NRaceData::EJobs::JOB_ASSASSIN, !IsAntiFlag(CItemData::ITEM_ANTIFLAG_ASSASSIN) },
		{ NRaceData::EJobs::JOB_SURA, !IsAntiFlag(CItemData::ITEM_ANTIFLAG_SURA) },
		{ NRaceData::EJobs::JOB_SHAMAN, !IsAntiFlag(CItemData::ITEM_ANTIFLAG_SHAMAN) },
#if defined(ENABLE_WOLFMAN_CHARACTER)
		{ NRaceData::EJobs::JOB_WOLFMAN, !IsAntiFlag(CItemData::ITEM_ANTIFLAG_WOLFMAN) },
#endif
	};

	for (const auto& it : bFlagList)
	{
		if (it.second && it.first == bJob)
			return TRUE;
	}

	return FALSE;
}

BOOL CItemData::HasNextGrade() const
{
	return 0 != m_ItemTable.dwRefinedVnum;
}

DWORD CItemData::GetRefineVNum() const
{
	return m_ItemTable.dwRefinedVnum;
}

DWORD CItemData::Get67AttrMaterial() const
{
	return m_ItemTable.dw67AttrMaterial;
}

DWORD CItemData::GetWearFlags() const
{
	return m_ItemTable.dwWearFlags;
}

DWORD CItemData::GetIBuyItemPrice() const
{
	return m_ItemTable.dwIBuyItemPrice;
}

DWORD CItemData::GetISellItemPrice() const
{
	return m_ItemTable.dwISellItemPrice;
}

long CItemData::GetLevelLimit() const
{
	for (int lv = 0; lv < ITEM_LIMIT_MAX_NUM; lv++)
	{
		if (m_ItemTable.aLimits[lv].bType == LIMIT_LEVEL)
			return m_ItemTable.aLimits[lv].lValue;
	}
	return 0;
}

BOOL CItemData::GetLimit(BYTE byIndex, TItemLimit* pItemLimit) const
{
	if (byIndex >= ITEM_LIMIT_MAX_NUM)
	{
		assert(byIndex < ITEM_LIMIT_MAX_NUM);
		return FALSE;
	}

	*pItemLimit = m_ItemTable.aLimits[byIndex];

	return TRUE;
}

BOOL CItemData::GetApply(BYTE byIndex, TItemApply* pItemApply) const
{
	if (byIndex >= ITEM_APPLY_MAX_NUM)
	{
		assert(byIndex < ITEM_APPLY_MAX_NUM);
		return FALSE;
	}

	*pItemApply = m_ItemTable.aApplies[byIndex];
	return TRUE;
}

POINT_VALUE CItemData::FindApplyValue(POINT_TYPE wApplyType)
{
	for (int i = 0; 0 < ITEM_APPLY_MAX_NUM; i++)
	{
		if (m_ItemTable.aApplies[i].wType == wApplyType)
			return m_ItemTable.aApplies[i].lValue;
	}

	return 0;
}

long CItemData::GetValue(BYTE byIndex) const
{
	if (byIndex >= ITEM_VALUES_MAX_NUM)
	{
		assert(byIndex < ITEM_VALUES_MAX_NUM);
		return 0;
	}

	return m_ItemTable.alValues[byIndex];
}

long CItemData::SetSocket(BYTE byIndex, DWORD value)
{
	if (byIndex >= ITEM_SOCKET_MAX_NUM)
	{
		assert(byIndex < ITEM_SOCKET_MAX_NUM);
		return -1;
	}

	return m_ItemTable.alSockets[byIndex] = value;
}

long CItemData::GetSocket(BYTE byIndex) const
{
	if (byIndex >= ITEM_SOCKET_MAX_NUM)
	{
		assert(byIndex < ITEM_SOCKET_MAX_NUM);
		return -1;
	}

	return m_ItemTable.alSockets[byIndex];
}

// 서버와 동일 서버 함수 변경시 같이 변경!!(이후에 합친다)
// SocketCount = 1 이면 초급무기
// SocketCount = 2 이면 중급무기
// SocketCount = 3 이면 고급무기
int CItemData::GetSocketCount() const
{
	return m_ItemTable.bGainSocketPct;
}

DWORD CItemData::GetIconNumber() const
{
	return m_ItemTable.dwVnum;
	// !@#
	//	return m_ItemTable.dwIconNumber;
}

UINT CItemData::GetSpecularPoweru() const
{
	return m_ItemTable.bSpecular;
}

float CItemData::GetSpecularPowerf() const
{
	UINT uSpecularPower = GetSpecularPoweru();

	return float(uSpecularPower) / 100.0f;
}

// refine 값은 아이템번호 끝자리와 일치한다-_-(테이블이용으로 바꿀 예정)
UINT CItemData::GetRefine() const
{
	const char* refine = strrchr(GetName(), '+');
	if (!refine)
		return 0;

	return atoi(refine + 1);
}

BOOL CItemData::IsEquipment() const
{
	switch (GetType())
	{
		case ITEM_TYPE_WEAPON:
		case ITEM_TYPE_ARMOR:
		case ITEM_TYPE_BELT:
		case ITEM_TYPE_RING:
		case ITEM_TYPE_COSTUME:
			return true;
	}

	return false;
}

#if defined(ENABLE_SOULBIND_SYSTEM)
bool CItemData::CanSealItem() const
{
	switch (GetType())
	{
		case ITEM_TYPE_WEAPON:
		{
			switch (GetSubType())
			{
				case WEAPON_ARROW:
				case WEAPON_MOUNT_SPEAR:
#if defined(ENABLE_QUIVER_SYSTEM)
				case WEAPON_QUIVER:
#endif
				case WEAPON_BOUQUET:
					return false;
				default:
					return true;
			}
		}

		case ITEM_TYPE_ARMOR:
		case ITEM_TYPE_BELT:
#if defined(ENABLE_DRAGON_SOUL_SEAL)
		case ITEM_TYPE_DS:
#endif
			return true;

		case ITEM_TYPE_COSTUME:
		{
			switch (GetSubType())
			{
#if defined(ENABLE_MOUNT_COSTUME_SYSTEM)
				case COSTUME_MOUNT:
#endif
					return false;
				default:
					return true;
			}
		}

		default:
			return false;
	}
}
#endif

#if defined(ENABLE_ACCE_COSTUME_SYSTEM) || defined(ENABLE_AURA_COSTUME_SYSTEM)
void CItemData::SetItemTableScaleData(BYTE bJob, BYTE bSex, float fScaleX, float fScaleY, float fScaleZ, float fScaleParticle)
{
	m_ItemScaleTable.v3Scale[bSex][bJob].x = fScaleX;
	m_ItemScaleTable.v3Scale[bSex][bJob].y = fScaleY;
	m_ItemScaleTable.v3Scale[bSex][bJob].z = fScaleZ;
	m_ItemScaleTable.fScaleParticle[bSex][bJob] = fScaleParticle;
}

D3DXVECTOR3& CItemData::GetItemScaleVector(BYTE bJob, BYTE bSex)
{
	return m_ItemScaleTable.v3Scale[bSex][bJob];
}

float CItemData::GetItemParticleScale(BYTE byJob, BYTE bySex)
{
	return m_ItemScaleTable.fScaleParticle[bySex][byJob];
}
#endif

#if defined(ENABLE_AURA_COSTUME_SYSTEM)
void CItemData::SetAuraEffectID(const char* szAuraEffectPath)
{
	if (szAuraEffectPath)
		CEffectManager::Instance().RegisterEffect2(szAuraEffectPath, &m_dwAuraEffectID);
}
#endif

void CItemData::Clear()
{
	m_strSummary = "";
	m_strModelFileName = "";
	m_strSubModelFileName = "";
	m_strDropModelFileName = "";
	m_strIconFileName = "";
	m_strLODModelFileNameVector.clear();

	m_pModelThing = NULL;
	m_pSubModelThing = NULL;
	m_pDropModelThing = NULL;
	m_pIconImage = NULL;
	m_pLODModelThingVector.clear();

	memset(&m_ItemTable, 0, sizeof(m_ItemTable));
#if defined(ENABLE_ACCE_COSTUME_SYSTEM)
	memset(&m_ItemScaleTable, 0, sizeof(m_ItemScaleTable));
#endif
#if defined(ENABLE_AURA_COSTUME_SYSTEM)
	m_dwAuraEffectID = 0;
#endif
}

CItemData::CItemData()
{
	Clear();
}

CItemData::~CItemData()
{
}
