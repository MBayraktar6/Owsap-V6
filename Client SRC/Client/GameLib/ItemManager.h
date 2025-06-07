#pragma once

#include "ItemData.h"
#include "DragonSoulTable.h"

class CItemManager : public CSingleton<CItemManager>
{
public:
	enum EItemDescCol
	{
		ITEMDESC_COL_VNUM,
		ITEMDESC_COL_NAME,
		ITEMDESC_COL_DESC,
		ITEMDESC_COL_SUMM,
		ITEMDESC_COL_NUM,
	};

#if defined(ENABLE_ACCE_COSTUME_SYSTEM) || defined(ENABLE_AURA_COSTUME_SYSTEM)
	enum EItemScaleCol
	{
		ITEMSCALE_COL_VNUM,
		ITEMSCALE_COL_JOB,
		ITEMSCALE_COL_SEX,
		ITEMSCALE_COL_SCALE_X,
		ITEMSCALE_COL_SCALE_Y,
		ITEMSCALE_COL_SCALE_Z,
		ITEMSCALE_COL_PARTICLE_SCALE,
		ITEMSCALE_COL_NUM,
	};
#endif

public:
	typedef std::map<DWORD, CItemData*> TItemMap;
	typedef std::map<std::string, CItemData*> TItemNameMap;

public:
	CItemManager();
	virtual ~CItemManager();

	void Destroy();

	BOOL SelectItemData(DWORD dwIndex);
	CItemData* GetSelectedItemDataPointer();

	BOOL GetItemDataPointer(DWORD dwItemID, CItemData** ppItemData);

	/////
	bool LoadItemDesc(const char* c_szFileName);
	bool LoadItemList(const char* c_szFileName);
	bool LoadItemTable(const char* c_szFileName);
	CItemData* MakeItemData(DWORD dwIndex);
#if defined(ENABLE_ACCE_COSTUME_SYSTEM) || defined(ENABLE_AURA_COSTUME_SYSTEM)
	bool LoadItemScale(const char* c_szFileName);
#endif

protected:
	TItemMap m_ItemMap;
	std::vector<CItemData*>  m_vec_ItemRange;
	CItemData* m_pSelectedItemData;

public:
	bool LoadDragonSoulTable(const char* c_szFileName);
	float GetDSSetWeight(BYTE bDSType, BYTE bDSGrade) const;
	int GetDSBasicApplyCount(BYTE bDSType, BYTE bDSGrade) const;
	POINT_VALUE GetDSBasicApplyValue(BYTE bDSType, POINT_TYPE wApplyType) const;
	POINT_VALUE GetDSAdditionalApplyValue(BYTE bDSType, POINT_TYPE wApplyType) const;

protected:
	CDragonSoulTable* m_pDSTable;
};
