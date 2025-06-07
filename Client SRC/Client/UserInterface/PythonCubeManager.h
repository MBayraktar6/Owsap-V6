/**
* Filename: PythonCubeManager.h
* Author: Owsap
**/

#pragma once

#if defined(ENABLE_CUBE_RENEWAL)
class CPythonCubeManager : public CSingleton<CPythonCubeManager>
{
public:
	enum ECubeItem : BYTE
	{
		CUBEITEM_NPC_VNUM,
		CUBEITEM_REWARD_ITEM,
		CUBEITEM_MATERIAL_ITEM,
		CUBEITEM_GOLD,
		CUBEITEM_CATEGORY,
		CUBEITEM_PROBABILITY,
		CUBEITEM_GEM,
		CUBEITEM_SET_VALUE,
		CUBEITEM_SUB_CATEGORY
	};

	enum ECubeCategory : BYTE
	{
		CUBE_ARMOR,
		CUBE_WEAPON,
		CUBE_ACCESSORY,
		CUBE_BELT,
		CUBE_EVENT,
		CUBE_ETC,
		CUBE_JOB,
		CUBE_SETADD_WEAPON,
		CUBE_SETADD_ARMOR_BODY,
		CUBE_SETADD_ARMOR_HELMET,
		CUBE_PET,
		CUBE_SKILL_BOOK,
		CUBE_ARMOR_GLOVE,
		CUBE_ALCHEMY,
		CUBE_CATEGORY_MAX
	};

	enum ECubeMisc
	{
		CUBE_MIN_ITEM_ATTR_WARNING = 1,
		CUBE_MAX_MAKE_QUANTITY = 9999,
		CUBE_MAX_MATERIAL_QUANTITY = 999,
		CUBE_MAX_MATERIAL_COUNT = 5,
		CUBE_MAX_DISTANCE = 1000,
	};

	using CategoryNameMap = std::unordered_map<std::string, ECubeCategory>;
	CategoryNameMap g_map_CubeCategoryName
	{
		{ "ARMOR",CUBE_ARMOR },
		{ "WEAPON", CUBE_WEAPON },
		{ "ACCESSORY", CUBE_ACCESSORY },
		{ "BELT", CUBE_BELT },
		{ "EVENT", CUBE_EVENT },
		{ "ETC", CUBE_ETC },
		{ "JOB", CUBE_JOB },
		{ "SETADD_WEAPON", CUBE_SETADD_WEAPON },
		{ "SETADD_ARMOR", CUBE_SETADD_ARMOR_BODY },
		{ "SETADD_HELMET", CUBE_SETADD_ARMOR_HELMET },
		{ "PET", CUBE_PET },
		{ "SKILL_BOOK", CUBE_SKILL_BOOK },
		{ "ARMOR_GLOVE", CUBE_ARMOR_GLOVE },
		{ "ALCHEMY", CUBE_ALCHEMY },
	};

	struct SCubeValue
	{
		DWORD dwVnum;
		UINT iCount;
		bool operator == (const SCubeValue& b)
		{
			return (this->iCount == b.iCount) && (this->dwVnum == b.dwVnum);
		}
	};

	struct SCubeData
	{
		UINT iIndex;

		DWORD dwNPCVnum;
		std::vector<SCubeValue> vItem;
		SCubeValue Reward;

		UINT iPercent;
		UINT iGold;
		UINT iGem;
#if defined(ENABLE_SET_ITEM)
		UINT iSetValue;
		DWORD dwNotRemove;
#endif

		INT iCategory;
		INT iSubCategory;

		SCubeData()
		{
			iIndex = 0;

			dwNPCVnum = 0;
			vItem.clear();
			Reward = {};

			iPercent = 0;
			iGold = 0;
			iGem = 0;
#if defined(ENABLE_SET_ITEM)
			iSetValue = SET_ITEM_SET_VALUE_NONE;
			dwNotRemove = 0;
#endif

			iCategory = CUBE_ETC;
			iSubCategory = CUBE_ETC;
		}
	};

	using CubeDataPtr = std::shared_ptr<SCubeData>;

public:
	CPythonCubeManager();
	virtual ~CPythonCubeManager();

	bool LoadCubeTable(const char* szFileName);
	bool CheckCubeData(const CubeDataPtr& pCubeData);

	using CubeDataVector = std::vector<CubeDataPtr>;
	CubeDataVector GetCubeData(const DWORD dwNPCVnum);

	INT GetCubeCategory(const std::string& rkCategory) const;
	bool IsCubeSetAddCategory(const BYTE bCategory) const;

	DWORD GetFileCrc() const { return m_dwFileCrc; }

private:
	CubeDataVector m_vCubeData;
	DWORD m_dwFileCrc;
};
#endif
