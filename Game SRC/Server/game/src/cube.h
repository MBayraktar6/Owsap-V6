#ifndef __INC_CUBE_H__
#define __INC_CUBE_H__

#if defined(__CUBE_RENEWAL__)
/**
* Filename: cube.h
* Author: Owsap
**/

class CCubeManager : public singleton<CCubeManager>
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
		UINT iSetValue;
		DWORD dwNotRemove;

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
			iSetValue = 0;
			dwNotRemove = 0;

			iCategory = CUBE_ETC;
			iSubCategory = CUBE_ETC;
		}
	};

	using CubeDataPtr = std::unique_ptr<SCubeData>;

public:
	CCubeManager();
	virtual ~CCubeManager();

	void Initialize();

	void LoadCubeTable(const char* szFileName);
	bool CheckCubeData(const CubeDataPtr& pkCubeData);
	bool CheckValidNPC(const DWORD dwNPCVnum);

	void OpenCube(const LPCHARACTER pChar);
	void CloseCube(const LPCHARACTER pChar);
	void MakeCube(const LPCHARACTER pChar, const UINT iCubeIndex, INT iQuantity, const INT iImproveItemPos);

	void Process(LPDESC pDesc, BYTE bSubHeader, DWORD dwNPCVnum = 0, bool bSuccess = false);

	using CubeDataVector = std::vector<CubeDataPtr>;
	const SCubeData& GetCubeData(const DWORD dwNPCVnum, const UINT iCubeIndex);

	INT GetCubeCategory(const std::string& rkCategory) const;
	bool IsCubeSetAddCategory(const BYTE bCategory) const;

	DWORD GetFileCrc() const { return m_dwFileCrc; }

private:
	CubeDataVector m_vCubeData;
	DWORD m_dwFileCrc;
};
#else
/**
* File : cube.h
* Date : 2006.11.20
* Author : mhh
* Description : 큐브시스템
**/

#define CUBE_MAX_NUM 24 // OLD:INVENTORY_MAX_NUM
#define CUBE_MAX_DISTANCE 1000

struct CUBE_VALUE
{
	DWORD vnum;
	int count;

	bool operator == (const CUBE_VALUE& b)
	{
		return (this->count == b.count) && (this->vnum == b.vnum);
	}
};

struct CUBE_DATA
{
	std::vector<WORD> npc_vnum;
	std::vector<CUBE_VALUE> item;
	std::vector<CUBE_VALUE> reward;
	int percent;
	unsigned int gold; // 제조시 필요한 금액
#if defined(__CUBE_RENEWAL__)
	DWORD not_remove;
#endif

	CUBE_DATA();

	bool can_make_item(LPITEM* items, WORD npc_vnum);
	CUBE_VALUE* reward_value();
	void remove_material(LPCHARACTER ch);
};

void Cube_init();
bool Cube_load(const char* file);

bool Cube_make(LPCHARACTER ch);

void Cube_clean_item(LPCHARACTER ch);
void Cube_open(LPCHARACTER ch);
void Cube_close(LPCHARACTER ch);

void Cube_show_list(LPCHARACTER ch);
void Cube_add_item(LPCHARACTER ch, int cube_index, int inven_index);
void Cube_delete_item(LPCHARACTER ch, int cube_index);

void Cube_request_result_list(LPCHARACTER ch);
void Cube_request_material_info(LPCHARACTER ch, int request_start_index, int request_count = 1);

// test print code
void Cube_print();

bool Cube_InformationInitialize();

#endif
#endif // __INC_CUBE_H__
