#include <map>
#include "stdafx.h"
#include "ClientManager.h"
#include "Main.h"
#include "Monarch.h"
#include "CsvReader.h"
#include "ProtoReader.h"

using namespace std;

extern int g_test_server;
extern std::string g_stLocaleNameColumn;

bool CClientManager::InitializeTables()
{
	if (!InitializeMobTable())
	{
		sys_err("InitializeMobTable FAILED");
		return false;
	}

	if (!InitializeItemTable())
	{
		sys_err("InitializeItemTable FAILED");
		return false;
	}

	if (!InitializeShopTable())
	{
		sys_err("InitializeShopTable FAILED");
		return false;
	}

#if defined(__GEM_SHOP__)
	if (!InitializeGemShopTable())
	{
		sys_err("InitializeGemShopTable FAILED");
		return false;
	}
#endif

	if (!InitializeSkillTable())
	{
		sys_err("InitializeSkillTable FAILED");
		return false;
	}

	if (!InitializeRefineTable())
	{
		sys_err("InitializeRefineTable FAILED");
		return false;
	}

	if (!InitializeItemAttrTable())
	{
		sys_err("InitializeItemAttrTable FAILED");
		return false;
	}

	if (!InitializeItemRareTable())
	{
		sys_err("InitializeItemRareTable FAILED");
		return false;
	}

	if (!InitializeBanwordTable())
	{
		sys_err("InitializeBanwordTable FAILED");
		return false;
	}

	if (!InitializeLandTable())
	{
		sys_err("InitializeLandTable FAILED");
		return false;
	}

	if (!InitializeObjectProto())
	{
		sys_err("InitializeObjectProto FAILED");
		return false;
	}

	if (!InitializeObjectTable())
	{
		sys_err("InitializeObjectTable FAILED");
		return false;
	}

	if (!InitializeMonarch())
	{
		sys_err("InitializeMonarch FAILED");
		return false;
	}

#if defined(__MAILBOX__)
	if (!InitializeMailBoxTable())
	{
		sys_err("InitializeMailBoxTable FAILED");
		return false;
	}
#endif

#if defined(__EXPRESSING_EMOTIONS__)
	if (!InitializeEmoteTable())
	{
		sys_err("InitializeEmoteTable FAILED");
		return false;
	}
#endif

	return true;
}

bool CClientManager::InitializeRefineTable()
{
	char query[2048];

	snprintf(query, sizeof(query),
		"SELECT `id`, `cost`, `prob`, `vnum0`, `count0`, `vnum1`, `count1`, `vnum2`, `count2`, `vnum3`, `count3`, `vnum4`, `count4` FROM refine_proto%s",
		GetTablePostfix());

	std::unique_ptr<SQLMsg> pkMsg(CDBManager::instance().DirectQuery(query));
	SQLResult* pRes = pkMsg->Get();

	if (!pRes->uiNumRows)
		return true;

	if (m_pRefineTable)
	{
		sys_log(0, "RELOAD: refine_proto");
		delete[] m_pRefineTable;
		m_pRefineTable = NULL;
	}

	m_iRefineTableSize = pRes->uiNumRows;

	m_pRefineTable = new TRefineTable[m_iRefineTableSize];
	memset(m_pRefineTable, 0, sizeof(TRefineTable) * m_iRefineTableSize);

	TRefineTable* prt = m_pRefineTable;
	MYSQL_ROW data;

	while ((data = mysql_fetch_row(pRes->pSQLResult)))
	{
		// const char* s_szQuery = "SELECT src_vnum, result_vnum, cost, prob, "
		// "vnum0, count0, vnum1, count1, vnum2, count2, vnum3, count3, vnum4, count4 "

		int col = 0;
		// prt->src_vnum = atoi(data[col++]);
		// prt->result_vnum = atoi(data[col++]);
		str_to_number(prt->id, data[col++]);
		str_to_number(prt->cost, data[col++]);
		str_to_number(prt->prob, data[col++]);

		prt->material_count = REFINE_MATERIAL_MAX_NUM;

		for (int i = 0; i < REFINE_MATERIAL_MAX_NUM; i++)
		{
			str_to_number(prt->materials[i].vnum, data[col++]);
			str_to_number(prt->materials[i].count, data[col++]);
			if (prt->materials[i].vnum == 0)
			{
				prt->material_count = i;
				break;
			}
		}

		sys_log(0, "REFINE: id %ld cost %d prob %d mat1 %lu cnt1 %d", prt->id, prt->cost, prt->prob, prt->materials[0].vnum, prt->materials[0].count);

		prt++;
	}
	return true;
}

class FCompareVnum
{
public:
	bool operator () (const TEntityTable& a, const TEntityTable& b) const
	{
		return (a.dwVnum < b.dwVnum);
	}
};

bool CClientManager::InitializeMobTable()
{
	map<int, const char*> localMap;

	cCsvTable nameData;
	if (!nameData.Load("mob_names.txt", '\t'))
	{
		fprintf(stderr, "Could not load mob_names.txt\n");
	}
	else
	{
		nameData.Next();
		while (nameData.Next())
		{
			if (nameData.ColCount() < 2)
				continue;

			localMap[atoi(nameData.AsStringByIndex(0))] = nameData.AsStringByIndex(1);
		}
	}
	//________________________________________________//

	cCsvTable data;
	if (!data.Load("mob_proto.txt", '\t'))
	{
		fprintf(stderr, "Could not load mob_proto.txt. Wrong file format?\n");
		return false;
	}
	data.Next();

	if (!m_vec_mobTable.empty())
	{
		sys_log(0, "RELOAD: mob_proto");
		m_vec_mobTable.clear();
	}
	m_vec_mobTable.resize(data.m_File.GetRowCount() - 1);

	memset(&m_vec_mobTable[0], 0, sizeof(TMobTable) * m_vec_mobTable.size());
	TMobTable* mob_table = &m_vec_mobTable[0];

	while (data.Next())
	{
		if (!Set_Proto_Mob_Table(mob_table, data, localMap))
		{
			fprintf(stderr, "Could not process entry.\n");
		}

		sys_log(1, "MOB #%-5d %-24s %-24s level: %-3u rank: %u empire: %d", mob_table->dwVnum, mob_table->szName, mob_table->szLocaleName, mob_table->bLevel, mob_table->bRank, mob_table->bEmpire);
		++mob_table;
	}

	sort(m_vec_mobTable.begin(), m_vec_mobTable.end(), FCompareVnum());
	return true;
}

bool CClientManager::InitializeShopTable()
{
	MYSQL_ROW data;
	int col;

	static const char* s_szQuery =
		"SELECT "
		"shop.vnum, "
		"shop.npc_vnum, "
		"shop_item.item_vnum, "
		"shop_item.count "
		"FROM shop LEFT JOIN shop_item "
		"ON shop.vnum = shop_item.shop_vnum ORDER BY shop.vnum, shop_item.item_vnum";

	std::unique_ptr<SQLMsg> pkMsg2(CDBManager::instance().DirectQuery(s_szQuery));

	// shopÀÇ vnumÀº ÀÖ´Âµ¥ shop_item ÀÌ ¾øÀ»°æ¿ì... ½ÇÆÐ·Î Ã³¸®µÇ´Ï ÁÖÀÇ ¿ä¸Á.
	// °íÃ³¾ßÇÒºÎºÐ
	SQLResult* pRes2 = pkMsg2->Get();

	if (!pRes2->uiNumRows)
	{
		sys_err("InitializeShopTable : Table count is zero.");
		return false;
	}

	std::map<int, TShopTable*> map_shop;

	if (m_pShopTable)
	{
		delete[](m_pShopTable);
		m_pShopTable = NULL;
	}

	TShopTable* shop_table = m_pShopTable;

	while ((data = mysql_fetch_row(pRes2->pSQLResult)))
	{
		col = 0;

		int iShopVnum = 0;
		str_to_number(iShopVnum, data[col++]);

		if (map_shop.end() == map_shop.find(iShopVnum))
		{
			shop_table = new TShopTable;
			memset(shop_table, 0, sizeof(TShopTable));
			shop_table->dwVnum = iShopVnum;

			map_shop[iShopVnum] = shop_table;
		}
		else
			shop_table = map_shop[iShopVnum];

		str_to_number(shop_table->dwNPCVnum, data[col++]);

		if (!data[col]) // ¾ÆÀÌÅÛÀÌ ÇÏ³ªµµ ¾øÀ¸¸é NULLÀÌ ¸®ÅÏ µÇ¹Ç·Î..
			continue;

		TShopItemTable* pItem = &shop_table->items[shop_table->bItemCount];

		str_to_number(pItem->vnum, data[col++]);
		str_to_number(pItem->count, data[col++]);

		++shop_table->bItemCount;
	}

	m_pShopTable = new TShopTable[map_shop.size()];
	m_iShopTableSize = map_shop.size();

	int i = 0;
	auto it = map_shop.begin();
	while (it != map_shop.end())
	{
		thecore_memcpy((m_pShopTable + i), (it++)->second, sizeof(TShopTable));
		sys_log(0, "SHOP: #%d items: %d", (m_pShopTable + i)->dwVnum, (m_pShopTable + i)->bItemCount);
		++i;
	}

	return true;
}

bool CClientManager::InitializeQuestItemTable()
{
	using namespace std;

	static const char* s_szQuery = "SELECT vnum, name, %s FROM quest_item_proto ORDER BY vnum";

	char query[1024];
	snprintf(query, sizeof(query), s_szQuery, g_stLocaleNameColumn.c_str());

	std::unique_ptr<SQLMsg> pkMsg(CDBManager::instance().DirectQuery(query));
	SQLResult* pRes = pkMsg->Get();

	if (!pRes->uiNumRows)
	{
		sys_err("query error or no rows: %s", query);
		return false;
	}

	MYSQL_ROW row;

	while ((row = mysql_fetch_row(pRes->pSQLResult)))
	{
		int col = 0;

		TItemTable tbl;
		memset(&tbl, 0, sizeof(tbl));

		str_to_number(tbl.dwVnum, row[col++]);

		if (row[col])
			strlcpy(tbl.szName, row[col], sizeof(tbl.szName));

		col++;

		if (row[col])
			strlcpy(tbl.szLocaleName, row[col], sizeof(tbl.szLocaleName));

		col++;

		if (m_map_itemTableByVnum.find(tbl.dwVnum) != m_map_itemTableByVnum.end())
		{
			sys_err("QUEST_ITEM_ERROR! %lu vnum already exist! (name %s)", tbl.dwVnum, tbl.szLocaleName);
			continue;
		}

		tbl.bType = ITEM_QUEST; // quest_item_proto Å×ÀÌºí¿¡ ÀÖ´Â °ÍµéÀº ¸ðµÎ ITEM_QUEST À¯Çü
		tbl.bSize = 1;

		m_vec_itemTable.push_back(tbl);
	}

	return true;
}

bool CClientManager::InitializeItemTable()
{
	map<int, const char*> localMap;
	cCsvTable nameData;
	if (!nameData.Load("item_names.txt", '\t'))
	{
		fprintf(stderr, "Could not load item_names.txt.\n");
	}
	else
	{
		nameData.Next();
		while (nameData.Next())
		{
			if (nameData.ColCount() < 2)
				continue;

			localMap[atoi(nameData.AsStringByIndex(0))] = nameData.AsStringByIndex(1);
		}
	}
	//_________________________________________________________________//

	cCsvTable data;
	if (!data.Load("item_proto.txt", '\t'))
	{
		fprintf(stderr, "Could not load item_proto.txt. Wrong file format?\n");
		return false;
	}
	data.Next();

	if (!m_vec_itemTable.empty())
	{
		sys_log(0, "RELOAD: item_proto");
		m_vec_itemTable.clear();
		m_map_itemTableByVnum.clear();
	}

	data.Destroy();
	if (!data.Load("item_proto.txt", '\t'))
	{
		fprintf(stderr, "Could not load item_proto.txt. Wrong file format?\n");
		return false;
	}
	data.Next();

	m_vec_itemTable.resize(data.m_File.GetRowCount() - 1);
	memset(&m_vec_itemTable[0], 0, sizeof(TItemTable) * m_vec_itemTable.size());

	TItemTable* item_table = &m_vec_itemTable[0];

	while (data.Next())
	{
		if (!Set_Proto_Item_Table(item_table, data, localMap))
		{
			fprintf(stderr, "Invalid item table. VNUM: %d\n", item_table->dwVnum);
		}

		m_map_itemTableByVnum.insert(std::map<DWORD, TItemTable*>::value_type(item_table->dwVnum, item_table));
		++item_table;
	}
	//_______________________________________________________________________//

	m_map_itemTableByVnum.clear();

	auto it = m_vec_itemTable.begin();
	while (it != m_vec_itemTable.end())
	{
		TItemTable* item_table = &(*(it++));

		sys_log(1, "ITEM: #%-5lu %-24s %-24s VAL: %ld %ld %ld %ld %ld %ld WEAR %lu ANTI %llu IMMUNE %lu REFINE %lu REFINE_SET %u ATTR67_MATERIAL %u MAGIC_PCT %u",
			item_table->dwVnum,
			item_table->szName,
			item_table->szLocaleName,
			item_table->alValues[0],
			item_table->alValues[1],
			item_table->alValues[2],
			item_table->alValues[3],
			item_table->alValues[4],
			item_table->alValues[5],
			item_table->dwWearFlags,
			item_table->ullAntiFlags,
			item_table->dwImmuneFlag,
			item_table->dwRefinedVnum,
			item_table->wRefineSet,
			item_table->dw67AttrMaterial,
			item_table->bAlterToMagicItemPct);

		m_map_itemTableByVnum.insert(std::map<DWORD, TItemTable*>::value_type(item_table->dwVnum, item_table));
	}
	sort(m_vec_itemTable.begin(), m_vec_itemTable.end(), FCompareVnum());
	return true;
}

bool CClientManager::InitializeSkillTable()
{
	char query[4096];
	snprintf(query, sizeof(query),
		"SELECT `dwVnum`, `szName`, `bType`, `bMaxLevel`, `dwSplashRange`, "
		"`szPointOn`, `szPointPoly`, `szSPCostPoly`, `szDurationPoly`, `szDurationSPCostPoly`, "
		"`szCooldownPoly`, `szMasterBonusPoly`, `setFlag`+0, `setAffectFlag`+0, "
		"`szPointOn2`, `szPointPoly2`, `szDurationPoly2`, `setAffectFlag2`+0, "
		"`szPointOn3`, `szPointPoly3`, `szDurationPoly3`, `szGrandMasterAddSPCostPoly`, "
		"`bLevelStep`, `bLevelLimit`, `prerequisiteSkillVnum`, `prerequisiteSkillLevel`, `iMaxHit`, `szSplashAroundDamageAdjustPoly`, `eSkillType`+0, `dwTargetRange` "
		"FROM skill_proto%s ORDER BY `dwVnum`",
		GetTablePostfix());

	std::unique_ptr<SQLMsg> pkMsg(CDBManager::instance().DirectQuery(query));
	SQLResult* pRes = pkMsg->Get();

	if (!pRes->uiNumRows)
	{
		sys_err("no result from skill_proto");
		return false;
	}

	if (!m_vec_skillTable.empty())
	{
		sys_log(0, "RELOAD: skill_proto");
		m_vec_skillTable.clear();
	}

	m_vec_skillTable.reserve(pRes->uiNumRows);

	MYSQL_ROW data;
	int col;

	while ((data = mysql_fetch_row(pRes->pSQLResult)))
	{
		TSkillTable t;
		memset(&t, 0, sizeof(t));

		col = 0;

		str_to_number(t.dwVnum, data[col++]);
		strlcpy(t.szName, data[col++], sizeof(t.szName));
		str_to_number(t.bType, data[col++]);
		str_to_number(t.bMaxLevel, data[col++]);
		str_to_number(t.dwSplashRange, data[col++]);

		strlcpy(t.szPointOn, data[col++], sizeof(t.szPointOn));
		strlcpy(t.szPointPoly, data[col++], sizeof(t.szPointPoly));
		strlcpy(t.szSPCostPoly, data[col++], sizeof(t.szSPCostPoly));
		strlcpy(t.szDurationPoly, data[col++], sizeof(t.szDurationPoly));
		strlcpy(t.szDurationSPCostPoly, data[col++], sizeof(t.szDurationSPCostPoly));
		strlcpy(t.szCooldownPoly, data[col++], sizeof(t.szCooldownPoly));
		strlcpy(t.szMasterBonusPoly, data[col++], sizeof(t.szMasterBonusPoly));

		str_to_number(t.dwFlag, data[col++]);
		str_to_number(t.dwAffectFlag, data[col++]);

		strlcpy(t.szPointOn2, data[col++], sizeof(t.szPointOn2));
		strlcpy(t.szPointPoly2, data[col++], sizeof(t.szPointPoly2));
		strlcpy(t.szDurationPoly2, data[col++], sizeof(t.szDurationPoly2));
		str_to_number(t.dwAffectFlag2, data[col++]);

		// ADD_GRANDMASTER_SKILL
		strlcpy(t.szPointOn3, data[col++], sizeof(t.szPointOn3));
		strlcpy(t.szPointPoly3, data[col++], sizeof(t.szPointPoly3));
		strlcpy(t.szDurationPoly3, data[col++], sizeof(t.szDurationPoly3));

		strlcpy(t.szGrandMasterAddSPCostPoly, data[col++], sizeof(t.szGrandMasterAddSPCostPoly));
		// END_OF_ADD_GRANDMASTER_SKILL

		str_to_number(t.bLevelStep, data[col++]);
		str_to_number(t.bLevelLimit, data[col++]);
		str_to_number(t.preSkillVnum, data[col++]);
		str_to_number(t.preSkillLevel, data[col++]);

		str_to_number(t.lMaxHit, data[col++]);

		strlcpy(t.szSplashAroundDamageAdjustPoly, data[col++], sizeof(t.szSplashAroundDamageAdjustPoly));

		str_to_number(t.bSkillAttrType, data[col++]);
		str_to_number(t.dwTargetRange, data[col++]);

		sys_log(0, "SKILL: #%d %s flag %u point %s affect %u cooldown %s", t.dwVnum, t.szName, t.dwFlag, t.szPointOn, t.dwAffectFlag, t.szCooldownPoly);

		m_vec_skillTable.push_back(t);
	}

	return true;
}

bool CClientManager::InitializeBanwordTable()
{
	m_vec_banwordTable.clear();

	std::unique_ptr<SQLMsg> pkMsg(CDBManager::instance().DirectQuery("SELECT word FROM banword"));

	SQLResult* pRes = pkMsg->Get();

	if (pRes->uiNumRows == 0)
		return true;

	MYSQL_ROW data;

	while ((data = mysql_fetch_row(pRes->pSQLResult)))
	{
		TBanwordTable t;

		if (data[0])
		{
			strlcpy(t.szWord, data[0], sizeof(t.szWord));
			m_vec_banwordTable.push_back(t);
		}
	}

	sys_log(0, "BANWORD: total %d", m_vec_banwordTable.size());
	return true;
}

bool CClientManager::InitializeItemAttrTable()
{
	char query[4096];
	snprintf(query, sizeof(query),
		"SELECT `apply`, `apply`+0, `prob`, "
		"`lv1`, `lv2`, `lv3`, `lv4`, `lv5`, "
#if defined(__ATTR_6TH_7TH__)
		"`lv6`, `lv7`, `lv8`, `lv9`, `lv10`, "
#endif
		"`weapon`, `body`, `wrist`, `foots`, `neck`, `head`, `shield`, `ear`"
#if defined(__PENDANT_SYSTEM__)
		", `pendant`"
#endif
#if defined(__GLOVE_SYSTEM__)
		", `glove`"
#endif
#if defined(__COSTUME_SYSTEM__)
		", `costume_body`"
		", `costume_hair`"
#if defined(__WEAPON_COSTUME_SYSTEM__)
		", `costume_weapon`"
#endif
#endif
		" FROM item_attr%s ORDER BY `apply`",
		GetTablePostfix()
	);

	std::unique_ptr<SQLMsg> pkMsg(CDBManager::instance().DirectQuery(query));
	SQLResult* pRes = pkMsg->Get();

	if (!pRes->uiNumRows)
	{
		sys_err("no result from item_attr");
		return false;
	}

	if (!m_vec_itemAttrTable.empty())
	{
		sys_log(0, "RELOAD: item_attr");
		m_vec_itemAttrTable.clear();
	}

	m_vec_itemAttrTable.reserve(pRes->uiNumRows);

	MYSQL_ROW data;

	while ((data = mysql_fetch_row(pRes->pSQLResult)))
	{
		TItemAttrTable t;

		memset(&t, 0, sizeof(TItemAttrTable));

		int col = 0;

		strlcpy(t.szApply, data[col++], sizeof(t.szApply));
		str_to_number(t.wApplyIndex, data[col++]);
		str_to_number(t.dwProb, data[col++]);
		str_to_number(t.lValues[0], data[col++]);
		str_to_number(t.lValues[1], data[col++]);
		str_to_number(t.lValues[2], data[col++]);
		str_to_number(t.lValues[3], data[col++]);
		str_to_number(t.lValues[4], data[col++]);
#if defined(__ATTR_6TH_7TH__)
		str_to_number(t.lValues[5], data[col++]);
		str_to_number(t.lValues[6], data[col++]);
		str_to_number(t.lValues[7], data[col++]);
		str_to_number(t.lValues[8], data[col++]);
		str_to_number(t.lValues[9], data[col++]);
#endif
		str_to_number(t.bMaxLevelBySet[ATTRIBUTE_SET_WEAPON], data[col++]);
		str_to_number(t.bMaxLevelBySet[ATTRIBUTE_SET_BODY], data[col++]);
		str_to_number(t.bMaxLevelBySet[ATTRIBUTE_SET_WRIST], data[col++]);
		str_to_number(t.bMaxLevelBySet[ATTRIBUTE_SET_FOOTS], data[col++]);
		str_to_number(t.bMaxLevelBySet[ATTRIBUTE_SET_NECK], data[col++]);
		str_to_number(t.bMaxLevelBySet[ATTRIBUTE_SET_HEAD], data[col++]);
		str_to_number(t.bMaxLevelBySet[ATTRIBUTE_SET_SHIELD], data[col++]);
		str_to_number(t.bMaxLevelBySet[ATTRIBUTE_SET_EAR], data[col++]);
#if defined(__PENDANT_SYSTEM__)
		str_to_number(t.bMaxLevelBySet[ATTRIBUTE_SET_PENDANT], data[col++]);
#endif
#if defined(__GLOVE_SYSTEM__)
		str_to_number(t.bMaxLevelBySet[ATTRIBUTE_SET_GLOVE], data[col++]);
#endif
#if defined(__COSTUME_SYSTEM__)
		str_to_number(t.bMaxLevelBySet[ATTRIBUTE_SET_COSTUME_BODY], data[col++]);
		str_to_number(t.bMaxLevelBySet[ATTRIBUTE_SET_COSTUME_HAIR], data[col++]);
#if defined(__WEAPON_COSTUME_SYSTEM__)
		str_to_number(t.bMaxLevelBySet[ATTRIBUTE_SET_COSTUME_WEAPON], data[col++]);
#endif
#endif

		if (!m_bWolfmanCharacter)
		{
			const char* szWolfApply[6] = {
				"BLEEDING_REDUCE",
				"BLEEDING_PCT",
				"ATT_BONUS_TO_WOLFMAN",
				"RESIST_WOLFMAN"
				"RESIST_CLAW",
#if defined(__ELEMENT_SYSTEM__)
				"RESIST_CLAW_REDUCTION",
#endif
			};
			for (int i = 0; i < _countof(szWolfApply); ++i)
				if (strcmp(t.szApply, szWolfApply[i]) == 0)
					continue;
		}

		sys_log(0, "ITEM_RARE: %-20s %4lu { %3d %3d %3d %3d %3d "
#if defined(__ATTR_6TH_7TH__)
			"%3d %3d %3d %3d %3d"
#endif
			"} { %d %d %d %d %d %d %d "
#if defined(__PENDANT_SYSTEM__)
			"%d "
#endif
#if defined(__GLOVE_SYSTEM__)
			"%d "
#endif
#if defined(__COSTUME_SYSTEM__)
			"%d "
			"%d "
#if defined(__WEAPON_COSTUME_SYSTEM__)
			"%d "
#endif
#endif
			"}"
			, t.szApply
			, t.dwProb
			, t.lValues[0], t.lValues[1], t.lValues[2], t.lValues[3], t.lValues[4]
#if defined(__ATTR_6TH_7TH__)
			, t.lValues[5], t.lValues[6], t.lValues[7], t.lValues[8], t.lValues[9]
#endif
			, t.bMaxLevelBySet[ATTRIBUTE_SET_WEAPON]
			, t.bMaxLevelBySet[ATTRIBUTE_SET_BODY]
			, t.bMaxLevelBySet[ATTRIBUTE_SET_WRIST]
			, t.bMaxLevelBySet[ATTRIBUTE_SET_FOOTS]
			, t.bMaxLevelBySet[ATTRIBUTE_SET_NECK]
			, t.bMaxLevelBySet[ATTRIBUTE_SET_HEAD]
			, t.bMaxLevelBySet[ATTRIBUTE_SET_SHIELD]
			, t.bMaxLevelBySet[ATTRIBUTE_SET_EAR]
#if defined(__PENDANT_SYSTEM__)
			, t.bMaxLevelBySet[ATTRIBUTE_SET_PENDANT]
#endif
#if defined(__GLOVE_SYSTEM__)
			, t.bMaxLevelBySet[ATTRIBUTE_SET_GLOVE]
#endif
#if defined(__COSTUME_SYSTEM__)
			, t.bMaxLevelBySet[ATTRIBUTE_SET_COSTUME_BODY]
			, t.bMaxLevelBySet[ATTRIBUTE_SET_COSTUME_HAIR]
#if defined(__WEAPON_COSTUME_SYSTEM__)
			, t.bMaxLevelBySet[ATTRIBUTE_SET_COSTUME_WEAPON]
#endif
#endif
		);

		m_vec_itemAttrTable.push_back(t);
	}

	return true;
}

bool CClientManager::InitializeItemRareTable()
{
	char query[4096];
	snprintf(query, sizeof(query),
		"SELECT `apply`, `apply`+0, `prob`, "
		"`lv1`, `lv2`, `lv3`, `lv4`, `lv5`, "
#if defined(__ATTR_6TH_7TH__)
		"`lv6`, `lv7`, `lv8`, `lv9`, `lv10`, "
#endif
		"`weapon`, `body`, `wrist`, `foots`, `neck`, `head`, `shield`, `ear`"
#if defined(__PENDANT_SYSTEM__)
		", `pendant`"
#endif
#if defined(__GLOVE_SYSTEM__)
		", `glove`"
#endif
		" FROM item_attr_rare%s ORDER BY `apply`",
		GetTablePostfix()
	);

	std::unique_ptr<SQLMsg> pkMsg(CDBManager::instance().DirectQuery(query));
	SQLResult* pRes = pkMsg->Get();

	if (!pRes->uiNumRows)
	{
		sys_err("no result from item_attr_rare");
		return false;
	}

	if (!m_vec_itemRareTable.empty())
	{
		sys_log(0, "RELOAD: item_attr_rare");
		m_vec_itemRareTable.clear();
	}

	m_vec_itemRareTable.reserve(pRes->uiNumRows);

	MYSQL_ROW data;

	while ((data = mysql_fetch_row(pRes->pSQLResult)))
	{
		TItemAttrTable t;

		memset(&t, 0, sizeof(TItemAttrTable));

		int col = 0;

		strlcpy(t.szApply, data[col++], sizeof(t.szApply));
		str_to_number(t.wApplyIndex, data[col++]);
		str_to_number(t.dwProb, data[col++]);
		str_to_number(t.lValues[0], data[col++]);
		str_to_number(t.lValues[1], data[col++]);
		str_to_number(t.lValues[2], data[col++]);
		str_to_number(t.lValues[3], data[col++]);
		str_to_number(t.lValues[4], data[col++]);
#if defined(__ATTR_6TH_7TH__)
		str_to_number(t.lValues[5], data[col++]);
		str_to_number(t.lValues[6], data[col++]);
		str_to_number(t.lValues[7], data[col++]);
		str_to_number(t.lValues[8], data[col++]);
		str_to_number(t.lValues[9], data[col++]);
#endif
		str_to_number(t.bMaxLevelBySet[ATTRIBUTE_SET_WEAPON], data[col++]);
		str_to_number(t.bMaxLevelBySet[ATTRIBUTE_SET_BODY], data[col++]);
		str_to_number(t.bMaxLevelBySet[ATTRIBUTE_SET_WRIST], data[col++]);
		str_to_number(t.bMaxLevelBySet[ATTRIBUTE_SET_FOOTS], data[col++]);
		str_to_number(t.bMaxLevelBySet[ATTRIBUTE_SET_NECK], data[col++]);
		str_to_number(t.bMaxLevelBySet[ATTRIBUTE_SET_HEAD], data[col++]);
		str_to_number(t.bMaxLevelBySet[ATTRIBUTE_SET_SHIELD], data[col++]);
		str_to_number(t.bMaxLevelBySet[ATTRIBUTE_SET_EAR], data[col++]);
#if defined(__PENDANT_SYSTEM__)
		str_to_number(t.bMaxLevelBySet[ATTRIBUTE_SET_PENDANT], data[col++]);
#endif
#if defined(__GLOVE_SYSTEM__)
		str_to_number(t.bMaxLevelBySet[ATTRIBUTE_SET_GLOVE], data[col++]);
#endif

		sys_log(0, "ITEM_RARE: %-20s %4lu { %3d %3d %3d %3d %3d "
#if defined(__ATTR_6TH_7TH__)
			"%3d %3d %3d %3d %3d"
#endif
			"} { %d %d %d %d %d %d %d "
#if defined(__PENDANT_SYSTEM__)
			"%d "
#endif
#if defined(__GLOVE_SYSTEM__)
			"%d "
#endif
			"}"
			, t.szApply, t.dwProb
			, t.lValues[0], t.lValues[1], t.lValues[2], t.lValues[3], t.lValues[4]
#if defined(__ATTR_6TH_7TH__)
			, t.lValues[5], t.lValues[6], t.lValues[7], t.lValues[8], t.lValues[9]
#endif
			, t.bMaxLevelBySet[ATTRIBUTE_SET_WEAPON]
			, t.bMaxLevelBySet[ATTRIBUTE_SET_BODY]
			, t.bMaxLevelBySet[ATTRIBUTE_SET_WRIST]
			, t.bMaxLevelBySet[ATTRIBUTE_SET_FOOTS]
			, t.bMaxLevelBySet[ATTRIBUTE_SET_NECK]
			, t.bMaxLevelBySet[ATTRIBUTE_SET_HEAD]
			, t.bMaxLevelBySet[ATTRIBUTE_SET_SHIELD]
			, t.bMaxLevelBySet[ATTRIBUTE_SET_EAR]
#if defined(__PENDANT_SYSTEM__)
			, t.bMaxLevelBySet[ATTRIBUTE_SET_PENDANT]
#endif
#if defined(__GLOVE_SYSTEM__)
			, t.bMaxLevelBySet[ATTRIBUTE_SET_GLOVE]
#endif
		);

		m_vec_itemRareTable.push_back(t);
	}

	return true;
}

bool CClientManager::InitializeLandTable()
{
	using namespace building;

	char query[4096];

	snprintf(query, sizeof(query),
		"SELECT `id`, `map_index`, `x`, `y`, `width`, `height`, `guild_id`, `guild_level_limit`, `price` "
		"FROM land%s WHERE `enable` = 'YES' ORDER BY `id`",
		GetTablePostfix());

	std::unique_ptr<SQLMsg> pkMsg(CDBManager::instance().DirectQuery(query));
	SQLResult* pRes = pkMsg->Get();

	if (!m_vec_kLandTable.empty())
	{
		sys_log(0, "RELOAD: land");
		m_vec_kLandTable.clear();
	}

	m_vec_kLandTable.reserve(pRes->uiNumRows);

	MYSQL_ROW data;

	if (pRes->uiNumRows > 0)
		while ((data = mysql_fetch_row(pRes->pSQLResult)))
		{
			TLand t;

			memset(&t, 0, sizeof(t));

			int col = 0;

			str_to_number(t.dwID, data[col++]);
			str_to_number(t.lMapIndex, data[col++]);
			str_to_number(t.x, data[col++]);
			str_to_number(t.y, data[col++]);
			str_to_number(t.width, data[col++]);
			str_to_number(t.height, data[col++]);
			str_to_number(t.dwGuildID, data[col++]);
			str_to_number(t.bGuildLevelLimit, data[col++]);
			str_to_number(t.dwPrice, data[col++]);

			sys_log(0, "LAND: %lu map %-4ld %7ldx%-7ld w %-4ld h %-4ld", t.dwID, t.lMapIndex, t.x, t.y, t.width, t.height);

			m_vec_kLandTable.push_back(t);
		}

	return true;
}

void parse_pair_number_string(const char* c_pszString, std::vector<std::pair<int, int> >& vec)
{
	// format: 10,1/20,3/300,50
	const char* t = c_pszString;
	const char* p = strchr(t, '/');
	std::pair<int, int> k;

	char szNum[32 + 1];
	char* comma;

	while (p)
	{
		if (isnhdigit(*t))
		{
			strlcpy(szNum, t, MIN(sizeof(szNum), (p - t) + 1));

			comma = strchr(szNum, ',');

			if (comma)
			{
				*comma = '\0';
				str_to_number(k.second, comma + 1);
			}
			else
				k.second = 0;

			str_to_number(k.first, szNum);
			vec.push_back(k);
		}

		t = p + 1;
		p = strchr(t, '/');
	}

	if (isnhdigit(*t))
	{
		strlcpy(szNum, t, sizeof(szNum));

		comma = strchr(const_cast<char*>(t), ',');

		if (comma)
		{
			*comma = '\0';
			str_to_number(k.second, comma + 1);
		}
		else
			k.second = 0;

		str_to_number(k.first, szNum);
		vec.push_back(k);
	}
}

bool CClientManager::InitializeObjectProto()
{
	using namespace building;

	char query[4096];
	snprintf(query, sizeof(query),
		"SELECT `vnum`, `price`, `materials`, `upgrade_vnum`, `upgrade_limit_time`, `life`, `reg_1`, `reg_2`, `reg_3`, `reg_4`, `npc`, `group_vnum`, `dependent_group` "
		"FROM object_proto%s ORDER BY `vnum`",
		GetTablePostfix());

	std::unique_ptr<SQLMsg> pkMsg(CDBManager::instance().DirectQuery(query));
	SQLResult* pRes = pkMsg->Get();

	if (!m_vec_kObjectProto.empty())
	{
		sys_log(0, "RELOAD: object_proto");
		m_vec_kObjectProto.clear();
	}

	m_vec_kObjectProto.reserve(MAX(0, pRes->uiNumRows));

	MYSQL_ROW data;

	if (pRes->uiNumRows > 0)
	{
		while ((data = mysql_fetch_row(pRes->pSQLResult)))
		{
			TObjectProto t;

			memset(&t, 0, sizeof(t));

			int col = 0;

			str_to_number(t.dwVnum, data[col++]);
			str_to_number(t.dwPrice, data[col++]);

			std::vector<std::pair<int, int> > vec;
			parse_pair_number_string(data[col++], vec);

			for (unsigned int i = 0; i < OBJECT_MATERIAL_MAX_NUM && i < vec.size(); ++i)
			{
				std::pair<int, int>& r = vec[i];

				t.kMaterials[i].dwItemVnum = r.first;
				t.kMaterials[i].dwCount = r.second;
			}

			str_to_number(t.dwUpgradeVnum, data[col++]);
			str_to_number(t.dwUpgradeLimitTime, data[col++]);
			str_to_number(t.lLife, data[col++]);
			str_to_number(t.lRegion[0], data[col++]);
			str_to_number(t.lRegion[1], data[col++]);
			str_to_number(t.lRegion[2], data[col++]);
			str_to_number(t.lRegion[3], data[col++]);

			// ADD_BUILDING_NPC
			str_to_number(t.dwNPCVnum, data[col++]);
			str_to_number(t.dwGroupVnum, data[col++]);
			str_to_number(t.dwDependOnGroupVnum, data[col++]);

			t.lNPCX = 0;
			t.lNPCY = MAX(t.lRegion[1], t.lRegion[3]) + 300;
			// END_OF_ADD_BUILDING_NPC

			sys_log(0, "OBJ_PROTO: vnum %lu price %lu mat %lu %lu",
				t.dwVnum, t.dwPrice, t.kMaterials[0].dwItemVnum, t.kMaterials[0].dwCount);

			m_vec_kObjectProto.push_back(t);
		}
	}

	return true;
}

bool CClientManager::InitializeObjectTable()
{
	using namespace building;

	char query[4096];
	snprintf(query, sizeof(query), "SELECT `id`, `land_id`, `vnum`, `map_index`, `x`, `y`, `x_rot`, `y_rot`, `z_rot`, `life` FROM object%s ORDER BY `id`", GetTablePostfix());

	std::unique_ptr<SQLMsg> pkMsg(CDBManager::instance().DirectQuery(query));
	SQLResult* pRes = pkMsg->Get();

	if (!m_map_pkObjectTable.empty())
	{
		sys_log(0, "RELOAD: object");
		m_map_pkObjectTable.clear();
	}

	MYSQL_ROW data;

	if (pRes->uiNumRows > 0)
		while ((data = mysql_fetch_row(pRes->pSQLResult)))
		{
			TObject* k = new TObject;

			memset(k, 0, sizeof(TObject));

			int col = 0;

			str_to_number(k->dwID, data[col++]);
			str_to_number(k->dwLandID, data[col++]);
			str_to_number(k->dwVnum, data[col++]);
			str_to_number(k->lMapIndex, data[col++]);
			str_to_number(k->x, data[col++]);
			str_to_number(k->y, data[col++]);
			str_to_number(k->xRot, data[col++]);
			str_to_number(k->yRot, data[col++]);
			str_to_number(k->zRot, data[col++]);
			str_to_number(k->lLife, data[col++]);

			sys_log(0, "OBJ: %lu vnum %lu map %-4ld %7ldx%-7ld life %ld",
				k->dwID, k->dwVnum, k->lMapIndex, k->x, k->y, k->lLife);

			m_map_pkObjectTable.insert(std::make_pair(k->dwID, k));
		}

	return true;
}

bool CClientManager::InitializeMonarch()
{
	CMonarch::instance().LoadMonarch();

	return true;
}

#if defined(__EXPRESSING_EMOTIONS__)
bool CClientManager::InitializeEmoteTable()
{
	if (m_map_kPlayerEmote.empty() == false)
		return true;

	char szQuery[1024];
	sprintf(szQuery, "SELECT `pid`, `vnum`, UNIX_TIMESTAMP(`duration`) FROM emotions%s", GetTablePostfix());

	std::unique_ptr<SQLMsg> pkMsg(CDBManager::instance().DirectQuery(szQuery));
	const SQLResult* pkRes = pkMsg->Get();
	if (!pkRes->uiNumRows)
		return true;

	const DWORD dwCurrentTime = static_cast<DWORD>(std::time(nullptr));

	MYSQL_ROW kData;
	while ((kData = mysql_fetch_row(pkRes->pSQLResult)))
	{
		BYTE bColumn = 0; TPacketGDEmote kTable;

		str_to_number(kTable.dwPID, kData[bColumn++]);
		str_to_number(kTable.dwVnum, kData[bColumn++]);
		str_to_number(kTable.dwDuration, kData[bColumn++]);

		if (dwCurrentTime > kTable.dwDuration)
			continue;
	
		m_map_kPlayerEmote[kTable.dwPID].emplace_back(kTable);
	}

	return true;
}
#endif

#if defined(__MAILBOX__)
bool CClientManager::InitializeMailBoxTable()
{
	if (m_map_mailbox.empty() == false)
		return true;

	char szQuery[QUERY_MAX_LEN];
	int len = sprintf(szQuery, "SELECT `name`, `who`, `title`, `message`, `gm`, `confirm`, `send_time`, `delete_time`"
		", `gold`, `won`, `vnum`, `count`"
	);

	for (BYTE i = 0; i < ITEM_SOCKET_MAX_NUM; i++)
		len += sprintf(szQuery + len, ", `socket%u`", i);

	for (BYTE i = 0; i < ITEM_ATTRIBUTE_MAX_NUM; i++)
		len += sprintf(szQuery + len, ", `attrtype%u`, `attrvalue%u`", i, i);

#if defined(__CHANGE_LOOK_SYSTEM__)
	len += sprintf(szQuery + len, ", `changelookvnum`");
#endif

#if defined(__REFINE_ELEMENT_SYSTEM__)
	len += sprintf(szQuery + len,
		", `refine_element_apply_type`"
		", `refine_element_grade`"
		", `refine_element_value0`, `refine_element_value1`, `refine_element_value2`"
		", `refine_element_bonus_value0`, `refine_element_bonus_value1`, `refine_element_bonus_value2`"
	);
#endif

#if defined(__ITEM_APPLY_RANDOM__)
	for (BYTE i = 0; i < ITEM_APPLY_MAX_NUM; i++)
		len += sprintf(szQuery + len, ", `apply_type%u`, `apply_value%u`, `apply_path%u`", i, i, i);
#endif

#if defined(__SET_ITEM__)
	len += sprintf(szQuery + len, ", `set_value`");
#endif

	len += sprintf(szQuery + len, " FROM `mailbox%s`", GetTablePostfix());

	std::unique_ptr<SQLMsg> pkMsg(CDBManager::instance().DirectQuery(szQuery));
	const SQLResult* pRes = pkMsg->Get();
	if (!pRes->uiNumRows)
		return true;

	MYSQL_ROW data;
	while ((data = mysql_fetch_row(pRes->pSQLResult)))
	{
		uint8_t col = 0;
		SMailBoxTable mail;

		auto name = data[col++];
		mail.bIsDeleted = false;
		mail.AddData.bHeader = 0;
		mail.AddData.Index = 0;

		std::memcpy(mail.szName, name, sizeof(mail.szName));
		std::memcpy(mail.AddData.szFrom, data[col++], sizeof(mail.AddData.szFrom));
		std::memcpy(mail.Message.szTitle, data[col++], sizeof(mail.Message.szTitle));
		std::memcpy(mail.AddData.szMessage, data[col++], sizeof(mail.AddData.szMessage));
		str_to_number(mail.Message.bIsGMPost, data[col++]);
		str_to_number(mail.Message.bIsConfirm, data[col++]);
		str_to_number(mail.Message.SendTime, data[col++]);
		str_to_number(mail.Message.DeleteTime, data[col++]);
		str_to_number(mail.AddData.iYang, data[col++]);
		str_to_number(mail.AddData.iWon, data[col++]);
		str_to_number(mail.AddData.dwItemVnum, data[col++]);
		str_to_number(mail.AddData.dwItemCount, data[col++]);

		mail.Message.bIsItemExist = mail.AddData.dwItemVnum > 0 || mail.AddData.iYang > 0 || mail.AddData.iWon > 0;

		for (BYTE i = 0; i < ITEM_SOCKET_MAX_NUM; i++)
			str_to_number(mail.AddData.alSockets[i], data[col++]);

		for (BYTE i = 0; i < ITEM_ATTRIBUTE_MAX_NUM; i++)
		{
			str_to_number(mail.AddData.aAttr[i].wType, data[col++]);
			str_to_number(mail.AddData.aAttr[i].lValue, data[col++]);
		}

#if defined(__CHANGE_LOOK_SYSTEM__)
		str_to_number(mail.AddData.dwChangeLookVnum, data[col++]);
#endif

#if defined(__REFINE_ELEMENT_SYSTEM__)
		str_to_number(mail.AddData.RefineElement.wApplyType, data[col++]);
		str_to_number(mail.AddData.RefineElement.bGrade, data[col++]);
		for (BYTE i = 0; i < REFINE_ELEMENT_MAX; i++)
			str_to_number(mail.AddData.RefineElement.abValue[i], data[col++]);
		for (BYTE i = 0; i < REFINE_ELEMENT_MAX; i++)
			str_to_number(mail.AddData.RefineElement.abBonusValue[i], data[col++]);
#endif

#if defined(__ITEM_APPLY_RANDOM__)
		for (BYTE i = 0; i < ITEM_APPLY_MAX_NUM; i++)
		{
			str_to_number(mail.AddData.aApplyRandom[i].wType, data[col++]);
			str_to_number(mail.AddData.aApplyRandom[i].lValue, data[col++]);
			str_to_number(mail.AddData.aApplyRandom[i].bPath, data[col++]);
		}
#endif

#if defined(__SET_ITEM__)
		str_to_number(mail.AddData.bSetValue, data[col++]);
#endif

		m_map_mailbox[name].emplace_back(mail);
	}

	return true;
}
#endif

#if defined(__GEM_SHOP__)
bool CClientManager::InitializeGemShopTable()
{
	// Load Default Gem Shop Table
	{
		if (m_mapGemShopTable.empty() == false)
			return true;

		char szQuery[1024];
		int iLen = sprintf(szQuery, "SELECT `pid`, UNIX_TIMESTAMP(`refresh_time`), `enabled_slots`");

		for (BYTE i = 0; i < GEM_SHOP_SLOT_COUNT; i++)
			iLen += sprintf(szQuery + iLen, ", `slot%d_enable`, `slot%d_vnum`, `slot%d_count`, `slot%d_price`", i, i, i, i);

		iLen += sprintf(szQuery + iLen, " FROM `gem_shop%s`", GetTablePostfix());

		std::unique_ptr<SQLMsg> pMsg(CDBManager::instance().DirectQuery(szQuery));
		const SQLResult* c_pRes = pMsg->Get();
		if (!c_pRes->uiNumRows)
			return true;

		MYSQL_ROW RowData;
		while ((RowData = mysql_fetch_row(c_pRes->pSQLResult)))
		{
			uint8_t iColumn = 0;
			SGemShopTable GemShopTable;
#if defined(__CONQUEROR_LEVEL__)
			GemShopTable.bSpecial = false;
#endif

			str_to_number(GemShopTable.dwPID, RowData[iColumn++]);
			str_to_number(GemShopTable.lRefreshTime, RowData[iColumn++]);
			str_to_number(GemShopTable.bEnabledSlots, RowData[iColumn++]);
			for (BYTE i = 0; i < GEM_SHOP_SLOT_COUNT; i++)
			{
				str_to_number(GemShopTable.GemShopItem[i].bEnable, RowData[iColumn++]);
				str_to_number(GemShopTable.GemShopItem[i].dwItemVnum, RowData[iColumn++]);
				str_to_number(GemShopTable.GemShopItem[i].bCount, RowData[iColumn++]);
				str_to_number(GemShopTable.GemShopItem[i].dwPrice, RowData[iColumn++]);
			}
			m_mapGemShopTable.emplace(std::make_pair(GemShopTable.dwPID, GemShopTable));
		}
	}

#	if defined(__CONQUEROR_LEVEL__)
	// Load Port (Special) Gem Shop Table
	{
		if (m_mapGemShopSpecialTable.empty() == false)
			return true;

		char szQuery[1024];
		int iLen = sprintf(szQuery, "SELECT `pid`, UNIX_TIMESTAMP(`refresh_time`), `enabled_slots`");

		for (BYTE i = 0; i < GEM_SHOP_SLOT_COUNT; i++)
			iLen += sprintf(szQuery + iLen, ", `slot%d_enable`, `slot%d_vnum`, `slot%d_count`, `slot%d_price`", i, i, i, i);

		iLen += sprintf(szQuery + iLen, " FROM `gem_shop_port%s`", GetTablePostfix());

		std::unique_ptr<SQLMsg> pMsg(CDBManager::instance().DirectQuery(szQuery));
		const SQLResult* c_pRes = pMsg->Get();
		if (!c_pRes->uiNumRows)
			return true;

		MYSQL_ROW RowData;
		while ((RowData = mysql_fetch_row(c_pRes->pSQLResult)))
		{
			uint8_t iColumn = 0;
			SGemShopTable GemShopTable;
			GemShopTable.bSpecial = true;

			str_to_number(GemShopTable.dwPID, RowData[iColumn++]);
			str_to_number(GemShopTable.lRefreshTime, RowData[iColumn++]);
			str_to_number(GemShopTable.bEnabledSlots, RowData[iColumn++]);
			for (BYTE i = 0; i < GEM_SHOP_SLOT_COUNT; i++)
			{
				str_to_number(GemShopTable.GemShopItem[i].bEnable, RowData[iColumn++]);
				str_to_number(GemShopTable.GemShopItem[i].dwItemVnum, RowData[iColumn++]);
				str_to_number(GemShopTable.GemShopItem[i].bCount, RowData[iColumn++]);
				str_to_number(GemShopTable.GemShopItem[i].dwPrice, RowData[iColumn++]);
			}
			m_mapGemShopSpecialTable.emplace(std::make_pair(GemShopTable.dwPID, GemShopTable));
		}
	}
#	endif

	return true;
}
#endif
