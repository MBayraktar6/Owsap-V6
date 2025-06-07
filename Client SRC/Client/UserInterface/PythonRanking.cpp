/**
* Filename: PythonRanking.cpp
* Author: Owsap
**/

#include "StdAfx.h"

#if defined(ENABLE_RANKING_SYSTEM)
#include "PythonRanking.h"
#include "AbstractPlayer.h"

CPythonRanking::CPythonRanking()
{
	Clear();
}

CPythonRanking::~CPythonRanking()
{
	Clear();
}

void CPythonRanking::Add(const BYTE c_bType, const BYTE c_bCategory, SRankingData& rTable)
{
	if (c_bType == TYPE_RK_SOLO && c_bCategory > SOLO_RK_CATEGORY_MAX)
		return;

	if (c_bType == TYPE_RK_PARTY && c_bCategory > PARTY_RK_CATEGORY_MAX)
		return;

	m_map_vecRankingData[c_bType][c_bCategory].emplace_back(std::make_shared<SRankingData>(rTable));
}

void CPythonRanking::Clear()
{
	for (BYTE bType = 0; bType < TYPE_RK_MAX; bType++)
		m_map_vecRankingData[bType].clear();
}

INT CPythonRanking::GetMySoloRankingPosition(const BYTE c_bCategory) const
{
	RankingCategoryMap RKMap = m_map_vecRankingData[TYPE_RK_SOLO];
	if (RKMap.empty())
		return 0;

	RankingCategoryMap::const_iterator it = RKMap.find(c_bCategory);
	if (it != RKMap.end())
	{
		bool bFound = false;
		RankingDataVector vRKData = it->second;
		RankingDataVector::iterator it = std::find_if(vRKData.begin(), vRKData.end(), [&bFound](const std::shared_ptr<SRankingData>& rTable)
			{
				for (BYTE bMemberIndex = 0; bMemberIndex < PARTY_MAX_MEMBER; bMemberIndex++)
				{
					const char* c_pszMemberName = rTable->Member[bMemberIndex].szName;
					if (strcmp(rTable->Member[bMemberIndex].szName, IAbstractPlayer::GetSingleton().GetName()) == 0)
					{
						bFound = true;
						break;
					}
				}
		return bFound;
			}
		);
		return (it == vRKData.end()) ? -1 : (std::distance(vRKData.begin(), it));
	}

	return 0;
}

TRankingData* CPythonRanking::GetHighRankingInfoSolo(const BYTE c_bCategory, const INT c_iLine) const
{
	RankingCategoryMap RKMap = m_map_vecRankingData[TYPE_RK_SOLO];
	if (RKMap.empty())
		return nullptr;

	RankingCategoryMap::const_iterator it = RKMap.find(c_bCategory);
	if (it != RKMap.end())
		return (c_iLine >= it->second.size()) ? nullptr : it->second.at(c_iLine).get();
	return nullptr;
}

INT CPythonRanking::GetMyPartyRankingPosition(const BYTE c_bCategory) const
{
	RankingCategoryMap RKMap = m_map_vecRankingData[TYPE_RK_PARTY];
	if (RKMap.empty())
		return 0;

	RankingCategoryMap::const_iterator it = RKMap.find(c_bCategory);
	if (it != RKMap.end())
	{
		bool bFound = false;
		RankingDataVector vRKData = it->second;
		RankingDataVector::iterator it = std::find_if(vRKData.begin(), vRKData.end(), [&bFound](const std::shared_ptr<SRankingData>& rTable)
			{
				for (BYTE bMemberIndex = 0; bMemberIndex < PARTY_MAX_MEMBER; bMemberIndex++)
				{
					const char* c_pszMemberName = rTable->Member[bMemberIndex].szName;
					if (strcmp(rTable->Member[bMemberIndex].szName, IAbstractPlayer::GetSingleton().GetName()) == 0)
					{
						bFound = true;
						break;
					}
				}
				return bFound;
			}
		);
		return (it == vRKData.end()) ? -1 : (std::distance(vRKData.begin(), it));
	}

	return 0;
}

std::size_t CPythonRanking::GetRankingInfoSoloSize(const BYTE c_bCategory) const
{
	RankingCategoryMap RKMap = m_map_vecRankingData[TYPE_RK_SOLO];
	if (RKMap.empty())
		return 0;

	RankingCategoryMap::const_iterator it = RKMap.find(c_bCategory);
	if (it != RKMap.end())
		return it->second.size();

	return 0;
}

TRankingData* CPythonRanking::GetHighRankingInfoParty(const BYTE c_bCategory, const INT c_iLine) const
{
	RankingCategoryMap RKMap = m_map_vecRankingData[TYPE_RK_PARTY];
	if (RKMap.empty())
		return nullptr;

	RankingCategoryMap::const_iterator it = RKMap.find(c_bCategory);
	if (it != RKMap.end())
		return (c_iLine >= it->second.size()) ? nullptr : it->second.at(c_iLine).get();
	return nullptr;
}

static std::string GetMemberNames(SRankingData* pTable)
{
	if (pTable == nullptr)
		return "";

	std::string strMemberNames;
	strMemberNames.reserve((CHARACTER_NAME_MAX_LEN + 1) * PARTY_MAX_MEMBER);

	strMemberNames += pTable->Member[0].szName;

	for (BYTE bMemberIndex = 1; bMemberIndex < PARTY_MAX_MEMBER; ++bMemberIndex)
	{
		const char* c_pszMemberName = pTable->Member[bMemberIndex].szName;
		if ((c_pszMemberName != nullptr) && (c_pszMemberName[0] != '\0'))
			strMemberNames += ", " + std::string(c_pszMemberName);
	}

	return strMemberNames;
}

////////////////////////////////////////////////////////////////////////////////
// Modules
//
// Solo (Unimplemented)
PyObject* rkGetHighRankingInfoSolo(PyObject* poSelf, PyObject* poArgs)
{
	BYTE bCategory;
	if (!PyTuple_GetByte(poArgs, 0, &bCategory))
		return Py_BadArgument();

	INT iLine;
	if (!PyTuple_GetInteger(poArgs, 1, &iLine))
		return Py_BadArgument();

	SRankingData* pRKData = CPythonRanking::Instance().GetHighRankingInfoSolo(bCategory, iLine);
	if (pRKData == nullptr)
		return Py_BuildValue("siiiis", "", 0, 0, 0, 0, "");

	return Py_BuildValue("siiiis",
		pRKData->Member[0].szName,
		pRKData->dwRecord0,
		pRKData->dwRecord1,
		pRKData->dwStartTime,
		pRKData->bEmpire,
		pRKData->szGuildName
	);
}

PyObject* rkGetMyRankingInfoSolo(PyObject* poSelf, PyObject* poArgs)
{
	BYTE bCategory;
	if (!PyTuple_GetByte(poArgs, 0, &bCategory))
		return Py_BadArgument();

	CPythonRanking& rkRanking = CPythonRanking::Instance();
	INT iMyPartyRKPosition = rkRanking.GetMySoloRankingPosition(bCategory);

	SRankingData* pRKData = rkRanking.GetHighRankingInfoSolo(bCategory, iMyPartyRKPosition);
	if (pRKData == nullptr || iMyPartyRKPosition < 0)
		return Py_BuildValue("isiiiis", 0, "", 0, 0, 0, 0, "");

	return Py_BuildValue("isiiiis",
		iMyPartyRKPosition + 1,
		pRKData->Member[0].szName,
		pRKData->dwRecord0,
		pRKData->dwRecord1,
		pRKData->dwStartTime,
		pRKData->bEmpire,
		pRKData->szGuildName
	);
}

PyObject* rkGetRankingInfoSoloSize(PyObject* poSelf, PyObject* poArgs)
{
	BYTE bCategory;
	if (!PyTuple_GetByte(poArgs, 0, &bCategory))
		return Py_BadArgument();

	CPythonRanking& rkRanking = CPythonRanking::Instance();
	return Py_BuildValue("i", rkRanking.GetRankingInfoSoloSize(bCategory));
}

//
// Party
PyObject* rkGetHighRankingInfoParty(PyObject* poSelf, PyObject* poArgs)
{
	BYTE bCategory;
	if (!PyTuple_GetByte(poArgs, 0, &bCategory))
		return Py_BadArgument();

	INT iLine;
	if (!PyTuple_GetInteger(poArgs, 1, &iLine))
		return Py_BadArgument();

	SRankingData* pRKData = CPythonRanking::Instance().GetHighRankingInfoParty(bCategory, iLine);
	if (pRKData == nullptr)
		return Py_BuildValue("siiii", "", 0, 0, 0, 0);

	return Py_BuildValue("siiii", pRKData->Member[0].szName, pRKData->dwRecord0, pRKData->dwRecord1, pRKData->dwStartTime, pRKData->bEmpire);
}
PyObject* rkGetPartyMemberName(PyObject* poSelf, PyObject* poArgs)
{
	BYTE bCategory;
	if (!PyTuple_GetByte(poArgs, 0, &bCategory))
		return Py_BadArgument();

	INT iLine;
	if (!PyTuple_GetInteger(poArgs, 1, &iLine))
		return Py_BadArgument();

	SRankingData* pRKData = CPythonRanking::Instance().GetHighRankingInfoParty(bCategory, iLine);
	return Py_BuildValue("s", GetMemberNames(pRKData).c_str());
}
PyObject* rkGetMyRankingInfoParty(PyObject* poSelf, PyObject* poArgs)
{
	BYTE bCategory;
	if (!PyTuple_GetByte(poArgs, 0, &bCategory))
		return Py_BadArgument();

	CPythonRanking& rkRanking = CPythonRanking::Instance();
	INT iMyPartyRKPosition = rkRanking.GetMyPartyRankingPosition(bCategory);

	SRankingData* pRKData = rkRanking.GetHighRankingInfoParty(bCategory, iMyPartyRKPosition);
	if (pRKData == nullptr || iMyPartyRKPosition < 0)
		return Py_BuildValue("isiiii", 0, "", 0, 0, 0, 0);

	return Py_BuildValue("isiiii", iMyPartyRKPosition + 1, pRKData->Member[0].szName, pRKData->dwRecord0, pRKData->dwRecord1, pRKData->dwStartTime, pRKData->bEmpire);
}
PyObject* rkGetMyPartyMemberName(PyObject* poSelf, PyObject* poArgs)
{
	BYTE bCategory;
	if (!PyTuple_GetByte(poArgs, 0, &bCategory))
		return Py_BadArgument();

	CPythonRanking& rkRanking = CPythonRanking::Instance();
	INT iMyPartyRKPosition = rkRanking.GetMyPartyRankingPosition(bCategory);

	SRankingData* pRKData = rkRanking.GetHighRankingInfoParty(bCategory, iMyPartyRKPosition);
	return Py_BuildValue("s", GetMemberNames(pRKData).c_str());
}
//
// Guild (Unimplemented)
PyObject* rkGetHighRankingInfoSoloWithGuildName(PyObject* poSelf, PyObject* poArgs) { return Py_BuildNone(); }
PyObject* rkGetMyRankingInfoSoloWithGuildName(PyObject* poSelf, PyObject* poArgs) { return Py_BuildNone(); }

void initranking()
{
	static PyMethodDef s_methods[] =
	{
		// Solo
		{ "GetHighRankingInfoSolo", rkGetHighRankingInfoSolo, METH_VARARGS },
		{ "GetMyRankingInfoSolo", rkGetMyRankingInfoSolo, METH_VARARGS },
		{ "GetRankingInfoSoloSize", rkGetRankingInfoSoloSize, METH_VARARGS },

		// Party
		{ "GetHighRankingInfoParty", rkGetHighRankingInfoParty, METH_VARARGS },
		{ "GetPartyMemberName", rkGetPartyMemberName, METH_VARARGS },
		{ "GetMyRankingInfoParty", rkGetMyRankingInfoParty, METH_VARARGS },
		{ "GetMyPartyMemberName", rkGetMyPartyMemberName, METH_VARARGS },

		// Guild
		{ "GetHighRankingInfoSoloWithGuildName", rkGetHighRankingInfoSoloWithGuildName, METH_VARARGS },
		{ "GetMyRankingInfoSoloWithGuildName", rkGetMyRankingInfoSoloWithGuildName, METH_VARARGS },

		{ NULL, NULL, NULL },
	};

	PyObject* poModule = Py_InitModule("ranking", s_methods);

	// Ranking Types
	PyModule_AddIntConstant(poModule, "TYPE_RK_SOLO", CPythonRanking::TYPE_RK_SOLO);
	PyModule_AddIntConstant(poModule, "TYPE_RK_PARTY", CPythonRanking::TYPE_RK_PARTY);
	PyModule_AddIntConstant(poModule, "TYPE_RK_MAX", CPythonRanking::TYPE_RK_MAX);

	// Solo Ranking
	PyModule_AddIntConstant(poModule, "SOLO_RK_CATEGORY_BF_WEAK", CPythonRanking::SOLO_RK_CATEGORY_BF_WEAK);
	PyModule_AddIntConstant(poModule, "SOLO_RK_CATEGORY_BF_TOTAL", CPythonRanking::SOLO_RK_CATEGORY_BF_TOTAL);
	PyModule_AddIntConstant(poModule, "SOLO_RK_CATEGORY_MD_RED", CPythonRanking::SOLO_RK_CATEGORY_MD_RED);
	PyModule_AddIntConstant(poModule, "SOLO_RK_CATEGORY_MD_BLUE", CPythonRanking::SOLO_RK_CATEGORY_MD_BLUE);
	PyModule_AddIntConstant(poModule, "SOLO_RK_CATEGORY_BNW", CPythonRanking::SOLO_RK_CATEGORY_BNW);
	PyModule_AddIntConstant(poModule, "SOLO_RK_CATEGORY_BATTLE_ROYALE", CPythonRanking::SOLO_RK_CATEGORY_BATTLE_ROYALE);
	PyModule_AddIntConstant(poModule, "SOLO_RK_CATEGORY_WORLD_BOSS", CPythonRanking::SOLO_RK_CATEGORY_WORLD_BOSS);
	PyModule_AddIntConstant(poModule, "SOLO_RK_CATEGORY_MAX", CPythonRanking::SOLO_RK_CATEGORY_MAX);

	// Party Ranking
#if defined(ENABLE_GUILD_DRAGONLAIR_PARTY_SYSTEM)
	PyModule_AddIntConstant(poModule, "PARTY_RK_CATEGORY_GUILD_DRAGONLAIR_RED_ALL", CPythonRanking::PARTY_RK_CATEGORY_GUILD_DRAGONLAIR_RED_ALL);
	PyModule_AddIntConstant(poModule, "PARTY_RK_CATEGORY_GUILD_DRAGONLAIR_RED_NOW_WEEK", CPythonRanking::PARTY_RK_CATEGORY_GUILD_DRAGONLAIR_RED_NOW_WEEK);
	PyModule_AddIntConstant(poModule, "PARTY_RK_CATEGORY_GUILD_DRAGONLAIR_RED_PAST_WEEK", CPythonRanking::PARTY_RK_CATEGORY_GUILD_DRAGONLAIR_RED_PAST_WEEK);
#endif
	PyModule_AddIntConstant(poModule, "PARTY_RK_CATEGORY_CZ_MOUSE", CPythonRanking::PARTY_RK_CATEGORY_CZ_MOUSE);
	PyModule_AddIntConstant(poModule, "PARTY_RK_CATEGORY_CZ_COW", CPythonRanking::PARTY_RK_CATEGORY_CZ_COW);
	PyModule_AddIntConstant(poModule, "PARTY_RK_CATEGORY_CZ_TIGER", CPythonRanking::PARTY_RK_CATEGORY_CZ_TIGER);
	PyModule_AddIntConstant(poModule, "PARTY_RK_CATEGORY_CZ_RABBIT", CPythonRanking::PARTY_RK_CATEGORY_CZ_RABBIT);
	PyModule_AddIntConstant(poModule, "PARTY_RK_CATEGORY_CZ_DRAGON", CPythonRanking::PARTY_RK_CATEGORY_CZ_DRAGON);
	PyModule_AddIntConstant(poModule, "PARTY_RK_CATEGORY_CZ_SNAKE", CPythonRanking::PARTY_RK_CATEGORY_CZ_SNAKE);
	PyModule_AddIntConstant(poModule, "PARTY_RK_CATEGORY_CZ_HORSE", CPythonRanking::PARTY_RK_CATEGORY_CZ_HORSE);
	PyModule_AddIntConstant(poModule, "PARTY_RK_CATEGORY_CZ_SHEEP", CPythonRanking::PARTY_RK_CATEGORY_CZ_SHEEP);
	PyModule_AddIntConstant(poModule, "PARTY_RK_CATEGORY_CZ_MONKEY", CPythonRanking::PARTY_RK_CATEGORY_CZ_MONKEY);
	PyModule_AddIntConstant(poModule, "PARTY_RK_CATEGORY_CZ_CHICKEN", CPythonRanking::PARTY_RK_CATEGORY_CZ_CHICKEN);
	PyModule_AddIntConstant(poModule, "PARTY_RK_CATEGORY_CZ_DOG", CPythonRanking::PARTY_RK_CATEGORY_CZ_DOG);
	PyModule_AddIntConstant(poModule, "PARTY_RK_CATEGORY_CZ_PIG", CPythonRanking::PARTY_RK_CATEGORY_CZ_PIG);
#if defined(ENABLE_DEFENSE_WAVE)
	PyModule_AddIntConstant(poModule, "PARTY_RK_CATEGORY_DEFENSE_WAVE", CPythonRanking::PARTY_RK_CATEGORY_DEFENSE_WAVE);
#endif
}
#endif
