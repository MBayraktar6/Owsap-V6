/**
* Filename: PythonInGameEventSystem.h
* Author: Owsap
**/

#include "StdAfx.h"

#if defined(ENABLE_EVENT_BANNER)
#include "PythonInGameEventSystemManager.h"

#include "../EterPack/EterPackManager.h"
#include "../GameLib/ItemManager.h"

CPythonInGameEventSystemManager::CPythonInGameEventSystemManager()
	: m_poIngameEventHandler(NULL)
{
	m_map_InGameEventName["mystery_box_drop"] = INGAME_EVENT_TYPE_MYSTERY_BOX1;
	m_map_InGameEventName["mystery_box_drop_2"] = INGAME_EVENT_TYPE_MYSTERY_BOX2;
	//m_map_InGameEventName["e_buff_spawn"] = INGAME_EVENT_TYPE_LUCKY_EVENT;
	//m_map_InGameEventName["attendance"] = INGAME_EVENT_TYPE_ATTENDANCE;
#if defined(ENABLE_MINI_GAME_RUMI)
	m_map_InGameEventName["mini_game_okey"] = INGAME_EVENT_TYPE_OKEY;
	m_map_InGameEventName["mini_game_okey_normal"] = INGAME_EVENT_TYPE_OKEY_NORMAL;
#endif
	m_map_InGameEventName["new_xmas_event"] = INGAME_EVENT_TYPE_NEW_XMAS_EVENT;
#if defined(ENABLE_MINI_GAME_YUTNORI)
	m_map_InGameEventName["mini_game_yutnori"] = INGAME_EVENT_TYPE_YUTNORI;
#endif
	//m_map_InGameEventName["e_monsterback"] = INGAME_EVENT_TYPE_MONSTERBACK_EVENT;
	//m_map_InGameEventName["e_monsterback_10th"] = INGAME_EVENT_TYPE_MONSTERBACK_10TH_EVENT;
	m_map_InGameEventName["easter_drop"] = INGAME_EVENT_TYPE_EASTER_EVENT;
	m_map_InGameEventName["e_summer_event"] = INGAME_EVENT_TYPE_ICECREAM_EVENT;
	m_map_InGameEventName["ramadan_drop"] = INGAME_EVENT_TYPE_RAMADAN_EVENT;
	m_map_InGameEventName["halloween_box"] = INGAME_EVENT_TYPE_HALLOWEEN_EVENT;
	m_map_InGameEventName["football_drop"] = INGAME_EVENT_TYPE_FOOTBALL_EVENT;
	m_map_InGameEventName["medal_part_drop"] = INGAME_EVENT_TYPE_OLYMPIC_EVENT;
	m_map_InGameEventName["valentine_drop"] = INGAME_EVENT_TYPE_VALENTINE_DAY_EVENT;
	//m_map_InGameEventName["fish_event"] = INGAME_EVENT_TYPE_FISH;
#if defined(ENABLE_FLOWER_EVENT)
	m_map_InGameEventName["e_flower_drop"] = INGAME_EVENT_TYPE_FLOWER_EVENT;
#endif
#if defined(ENABLE_MINI_GAME_CATCH_KING)
	m_map_InGameEventName["mini_game_catchking"] = INGAME_EVENT_TYPE_CATCHKING;
#endif
	//m_map_InGameEventName["md_start"] = INGAME_EVENT_TYPE_MINIBOSS;
#if defined(ENABLE_SUMMER_EVENT_ROULETTE)
	m_map_InGameEventName["e_late_summer"] = INGAME_EVENT_TYPE_ROULETTE;
#endif
	//m_map_InGameEventName["mini_game_bnw"] = INGAME_EVENT_TYPE_BLACK_AND_WHITE;
	//m_map_InGameEventName["world_boss"] = INGAME_EVENT_TYPE_WORLD_BOSS;
	//m_map_InGameEventName["battle_royale"] = INGAME_EVENT_TYPE_BATTLE_ROYALE;
	//m_map_InGameEventName["metinstone_rain_event"] = INGAME_EVENT_TYPE_METINSTONE_RAIN_EVENT;
	//m_map_InGameEventName["e_otherworld_drop_1"] = INGAME_EVENT_TYPE_OTHER_WORLD;
	//m_map_InGameEventName["e_otherworld_drop_2"] = INGAME_EVENT_TYPE_OTHER_WORLD_EVE;
	//m_map_InGameEventName["golden_land"] = INGAME_EVENT_TYPE_GOLDEN_LAND_EVENT;
	//m_map_InGameEventName["sports_match"] = INGAME_EVENT_TYPE_SPORTS_MATCH_EVENT;
#if defined(ENABLE_SNOWFLAKE_STICK_EVENT)
	m_map_InGameEventName["snowflake_stick_event"] = INGAME_EVENT_TYPE_SNOWFLAKE_STICK_EVENT;
#endif
	//m_map_InGameEventName["treasure_hunt"] = INGAME_EVENT_TYPE_TREASURE_HUNT;
}

CPythonInGameEventSystemManager::~CPythonInGameEventSystemManager()
{
	Destroy();
}

void CPythonInGameEventSystemManager::Destroy()
{
	m_map_InGameEventName.clear();
	m_map_InGameEventData.clear();
}

void CPythonInGameEventSystemManager::Clear()
{
	for (TInGameEventDataMap::value_type& it : m_map_InGameEventData)
	{
		it.second.bEnable = false;
		it.second.lStartTime = 0;
		it.second.lEndTime = 0;
	}
}

void CPythonInGameEventSystemManager::SetIngameEventHandler(PyObject* poHandler)
{
	m_poIngameEventHandler = poHandler;
}

void CPythonInGameEventSystemManager::DestroyInGameEventHandler()
{
	m_poIngameEventHandler = NULL;
}

bool CPythonInGameEventSystemManager::LoadInGameEventData(const char* c_szFileName)
{
	if (!c_szFileName)
		return false;

	CTextFileLoader TextFileLoader;
	if (!TextFileLoader.Load(c_szFileName))
		return false;

	TextFileLoader.SetTop();
	for (DWORD dwNode = 0; dwNode < TextFileLoader.GetChildNodeCount(); ++dwNode)
	{
		if (!TextFileLoader.SetChildNode(dwNode))
		{
			TextFileLoader.SetParentNode();
			continue;
		}

		std::string strNodeName;
		if (!TextFileLoader.GetCurrentNodeName(&strNodeName))
		{
			TextFileLoader.SetParentNode();
			continue;
		}

		BYTE bInGameEventType = GetInGameEventType(strNodeName);
		if (bInGameEventType == 0)
		{
			TextFileLoader.SetParentNode();
			continue;
		}

		TInGameEventRewardVector vInGameEventReward;
		vInGameEventReward.clear();

		TTokenVector* pToken;
		for (int k = 1; k <= 256; ++k)
		{
			char szBuf[4];
			snprintf(szBuf, sizeof(szBuf), "%d", k);

			if (TextFileLoader.GetTokenVector(szBuf, &pToken))
			{
				if (pToken->size() != 2)
				{
					TraceError("Group '%s' check column count of reward key '%d' in event_reward_list.txt.", strNodeName.c_str(), k);
					continue;
				}

				TInGameEventRewardData kInGameEventRewardData;
				kInGameEventRewardData.iItemIndex = atoi(pToken->at(0).c_str());
				kInGameEventRewardData.iItemCount = atoi(pToken->at(1).c_str());

				CItemData* pItemData = NULL;
				if (!CItemManager::Instance().GetItemDataPointer(kInGameEventRewardData.iItemIndex, &pItemData))
				{
					TraceError("Group '%s' invalid vnum '%u' of reward key '%d' in event_reward_list.txt.",
						strNodeName.c_str(), kInGameEventRewardData.iItemIndex, k);
					continue;
				}

				if (kInGameEventRewardData.iItemCount < 1)
				{
					TraceError("Group '%s' invalid reward count '%u' of reward vnum '%u' in event_reward_list.txt.",
						strNodeName.c_str(), kInGameEventRewardData.iItemCount, k);
					continue;
				}

				if (k > INGAME_EVENT_REWARD_LIST_MAX)
				{
					TraceError("The number of '%s' rewards exceed 100 in event_reward_list.txt.", strNodeName.c_str());
					continue;
				}

				vInGameEventReward.emplace_back(kInGameEventRewardData);
			}
		}

		m_map_InGameEventData[bInGameEventType].vRewardData = std::move(vInGameEventReward);
		TextFileLoader.SetParentNode();
	}

	return true;
}

BYTE CPythonInGameEventSystemManager::GetInGameEventType(const std::string strEventName) const
{
	TInGameEventNameMap::const_iterator it = m_map_InGameEventName.find(strEventName);
	return (it != m_map_InGameEventName.end() ? it->second : INGAME_EVENT_TYPE_NONE);
}

bool CPythonInGameEventSystemManager::GetInGameEventRewardData(BYTE bInGameEventType, BYTE bSlotPos, TInGameEventRewardData& rkTable)
{
	TInGameEventDataMap::const_iterator it = m_map_InGameEventData.find(bInGameEventType);
	if (it != m_map_InGameEventData.end() && bSlotPos < it->second.vRewardData.size())
	{
		rkTable = it->second.vRewardData[bSlotPos];
		return true;
	}

	return false;
}

size_t CPythonInGameEventSystemManager::GetEventRewardCount(BYTE bInGameEventType) const
{
	TInGameEventDataMap::const_iterator it = m_map_InGameEventData.find(bInGameEventType);
	return (it != m_map_InGameEventData.end() ? it->second.vRewardData.size() : 0);
}

void CPythonInGameEventSystemManager::SetInGameEventEnable(BYTE bInGameEventType, int iEnable)
{
	TInGameEventDataMap::iterator it = m_map_InGameEventData.find(bInGameEventType);
	if (it != m_map_InGameEventData.end())
	{
		it->second.bEnable = iEnable;
		it->second.lEndTime = iEnable; // NOTE: Currently using activation as the event duration.
	}

	if (NULL != m_poIngameEventHandler)
		PyCallClassMemberFunc(m_poIngameEventHandler, "RefreshInGameEvent", Py_BuildValue("(i)", true));
}

bool CPythonInGameEventSystemManager::GetInGameEventEnable(BYTE bInGameEventType)
{
	TInGameEventDataMap::const_iterator it = m_map_InGameEventData.find(bInGameEventType);
	return (it != m_map_InGameEventData.end() ? it->second.bEnable : false);
}

BYTE CPythonInGameEventSystemManager::GetInGameEventCount() const
{
	BYTE bCount = 0;
	for (TInGameEventDataMap::value_type it : m_map_InGameEventData)
		if (it.second.bEnable)
			++bCount;
	return bCount;
}

bool CPythonInGameEventSystemManager::GetInGameEventData(BYTE bInGameEventType, TInGameEventData& rkTable)
{
	TInGameEventDataMap::const_iterator it = m_map_InGameEventData.find(bInGameEventType);
	if (it != m_map_InGameEventData.end())
	{
		rkTable = it->second;
		return true;
	}

	return false;
}

PyObject* ingameEventSystemSetIngameEventHandler(PyObject* poSelf, PyObject* poArgs)
{
	PyObject* poEventHandler;
	if (!PyTuple_GetObject(poArgs, 0, &poEventHandler))
		return Py_BuildException();

	CPythonInGameEventSystemManager::Instance().SetIngameEventHandler(poEventHandler);
	return Py_BuildNone();
}

PyObject* ingameEventSystemDestroyInGameEventHandler(PyObject* poSelf, PyObject* poArgs)
{
	CPythonInGameEventSystemManager::Instance().DestroyInGameEventHandler();
	return Py_BuildNone();
}

PyObject* ingameEventSystemSetInGameEventEnable(PyObject* poSelf, PyObject* poArgs)
{
	BYTE bInGameEventType;
	if (!PyTuple_GetByte(poArgs, 0, &bInGameEventType))
		return Py_BuildException();

	int iEnable;
	if (!PyTuple_GetInteger(poArgs, 1, &iEnable))
		return Py_BuildException();

	CPythonInGameEventSystemManager::Instance().SetInGameEventEnable(bInGameEventType, iEnable);
	return Py_BuildNone();
}

PyObject* ingameEventSystemGetInGameEventEnable(PyObject* poSelf, PyObject* poArgs)
{
	BYTE bInGameEventType;
	if (!PyTuple_GetByte(poArgs, 0, &bInGameEventType))
		return Py_BuildException();

	return Py_BuildValue("b", CPythonInGameEventSystemManager::Instance().GetInGameEventEnable(bInGameEventType));
}

PyObject* ingameEventSystemGetInGameEventCount(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonInGameEventSystemManager::Instance().GetInGameEventCount());
}

PyObject* ingameEventSystemGetInGameEventData(PyObject* poSelf, PyObject* poArgs)
{
	BYTE bInGameEventType;
	if (!PyTuple_GetByte(poArgs, 0, &bInGameEventType))
		return Py_BuildException();

	CPythonInGameEventSystemManager::TInGameEventData kInGameEventData;
	if (CPythonInGameEventSystemManager::Instance().GetInGameEventData(bInGameEventType, kInGameEventData))
		return Py_BuildValue("(iii)", kInGameEventData.bEnable, kInGameEventData.lStartTime, kInGameEventData.lEndTime);

	return Py_BuildValue("(iii)", 0, 0, 0, 0);
}

PyObject* ingameEventSystemGetInGameEventEndTime(PyObject* poSelf, PyObject* poArgs)
{
	BYTE bInGameEventType;
	if (!PyTuple_GetByte(poArgs, 0, &bInGameEventType))
		return Py_BuildException();

	CPythonInGameEventSystemManager::TInGameEventData kInGameEventData;
	if (CPythonInGameEventSystemManager::Instance().GetInGameEventData(bInGameEventType, kInGameEventData))
		return Py_BuildValue("i", kInGameEventData.lEndTime);

	return Py_BuildValue("i", 0);
}

PyObject* ingameEventSystemGetInGameEventRewardData(PyObject* poSelf, PyObject* poArgs)
{
	BYTE bInGameEventType;
	if (!PyTuple_GetByte(poArgs, 0, &bInGameEventType))
		return Py_BuildException();

	BYTE bSlotPos;
	if (!PyTuple_GetByte(poArgs, 1, &bSlotPos))
		return Py_BuildException();

	CPythonInGameEventSystemManager::TInGameEventRewardData kInGameEventRewardData;
	if (CPythonInGameEventSystemManager::Instance().GetInGameEventRewardData(bInGameEventType, bSlotPos, kInGameEventRewardData))
		return Py_BuildValue("(ii)", kInGameEventRewardData.iItemIndex, kInGameEventRewardData.iItemCount);

	return Py_BuildValue("(ii)", 0, 0);
}

PyObject* ingameEventSystemGetInGameEventRewardCount(PyObject* poSelf, PyObject* poArgs)
{
	BYTE bInGameEventType;
	if (!PyTuple_GetByte(poArgs, 0, &bInGameEventType))
		return Py_BuildException();

	return Py_BuildValue("i", CPythonInGameEventSystemManager::Instance().GetEventRewardCount(bInGameEventType));
}

void initInGameEventSystem()
{
	static PyMethodDef s_methods[] =
	{
		{ "SetIngameEventHandler", ingameEventSystemSetIngameEventHandler, METH_VARARGS },
		{ "DestroyInGameEventHandler", ingameEventSystemDestroyInGameEventHandler, METH_VARARGS },
		{ "SetInGameEventEnable", ingameEventSystemSetInGameEventEnable, METH_VARARGS },
		{ "GetInGameEventEnable", ingameEventSystemGetInGameEventEnable, METH_VARARGS },
		{ "GetInGameEventCount", ingameEventSystemGetInGameEventCount, METH_VARARGS },
		{ "GetInGameEventData", ingameEventSystemGetInGameEventData, METH_VARARGS },
		{ "GetInGameEventEndTime", ingameEventSystemGetInGameEventEndTime, METH_VARARGS },
		{ "GetInGameEventRewardData", ingameEventSystemGetInGameEventRewardData, METH_VARARGS },
		{ "GetInGameEventRewardCount", ingameEventSystemGetInGameEventRewardCount, METH_VARARGS },
		{ NULL, NULL, NULL },
	};

	PyObject* poModule = Py_InitModule("ingameEventSystem", s_methods);
	PyModule_AddIntConstant(poModule, "INGAME_EVENT_TYPE_MYSTERY_BOX1", CPythonInGameEventSystemManager::INGAME_EVENT_TYPE_MYSTERY_BOX1);
	PyModule_AddIntConstant(poModule, "INGAME_EVENT_TYPE_MYSTERY_BOX2", CPythonInGameEventSystemManager::INGAME_EVENT_TYPE_MYSTERY_BOX2);
	//PyModule_AddIntConstant(poModule, "INGAME_EVENT_TYPE_LUCKY_EVENT", CPythonInGameEventSystemManager::INGAME_EVENT_TYPE_LUCKY_EVENT);
	//PyModule_AddIntConstant(poModule, "INGAME_EVENT_TYPE_ATTENDANCE", CPythonInGameEventSystemManager::INGAME_EVENT_TYPE_ATTENDANCE);
#if defined(ENABLE_MINI_GAME_RUMI)
	PyModule_AddIntConstant(poModule, "INGAME_EVENT_TYPE_OKEY", CPythonInGameEventSystemManager::INGAME_EVENT_TYPE_OKEY);
	PyModule_AddIntConstant(poModule, "INGAME_EVENT_TYPE_OKEY_NORMAL", CPythonInGameEventSystemManager::INGAME_EVENT_TYPE_OKEY_NORMAL);
#endif
	PyModule_AddIntConstant(poModule, "INGAME_EVENT_TYPE_NEW_XMAS_EVENT", CPythonInGameEventSystemManager::INGAME_EVENT_TYPE_NEW_XMAS_EVENT);
#if defined(ENABLE_MINI_GAME_YUTNORI)
	PyModule_AddIntConstant(poModule, "INGAME_EVENT_TYPE_YUTNORI", CPythonInGameEventSystemManager::INGAME_EVENT_TYPE_YUTNORI);
#endif
	//PyModule_AddIntConstant(poModule, "INGAME_EVENT_TYPE_MONSTERBACK_EVENT", CPythonInGameEventSystemManager::INGAME_EVENT_TYPE_MONSTERBACK_EVENT);
	//PyModule_AddIntConstant(poModule, "INGAME_EVENT_TYPE_MONSTERBACK_10TH_EVENT", CPythonInGameEventSystemManager::INGAME_EVENT_TYPE_MONSTERBACK_10TH_EVENT);
	PyModule_AddIntConstant(poModule, "INGAME_EVENT_TYPE_EASTER_EVENT", CPythonInGameEventSystemManager::INGAME_EVENT_TYPE_EASTER_EVENT);
	PyModule_AddIntConstant(poModule, "INGAME_EVENT_TYPE_ICECREAM_EVENT", CPythonInGameEventSystemManager::INGAME_EVENT_TYPE_ICECREAM_EVENT);
	PyModule_AddIntConstant(poModule, "INGAME_EVENT_TYPE_RAMADAN_EVENT", CPythonInGameEventSystemManager::INGAME_EVENT_TYPE_RAMADAN_EVENT);
	PyModule_AddIntConstant(poModule, "INGAME_EVENT_TYPE_HALLOWEEN_EVENT", CPythonInGameEventSystemManager::INGAME_EVENT_TYPE_HALLOWEEN_EVENT);
	PyModule_AddIntConstant(poModule, "INGAME_EVENT_TYPE_FOOTBALL_EVENT", CPythonInGameEventSystemManager::INGAME_EVENT_TYPE_FOOTBALL_EVENT);
	PyModule_AddIntConstant(poModule, "INGAME_EVENT_TYPE_OLYMPIC_EVENT", CPythonInGameEventSystemManager::INGAME_EVENT_TYPE_OLYMPIC_EVENT);
	PyModule_AddIntConstant(poModule, "INGAME_EVENT_TYPE_VALENTINE_DAY_EVENT", CPythonInGameEventSystemManager::INGAME_EVENT_TYPE_VALENTINE_DAY_EVENT);
	//PyModule_AddIntConstant(poModule, "INGAME_EVENT_TYPE_FISH", CPythonInGameEventSystemManager::INGAME_EVENT_TYPE_FISH);
#if defined(ENABLE_FLOWER_EVENT)
	PyModule_AddIntConstant(poModule, "INGAME_EVENT_TYPE_FLOWER_EVENT", CPythonInGameEventSystemManager::INGAME_EVENT_TYPE_FLOWER_EVENT);
#endif
#if defined(ENABLE_MINI_GAME_CATCH_KING)
	PyModule_AddIntConstant(poModule, "INGAME_EVENT_TYPE_CATCHKING", CPythonInGameEventSystemManager::INGAME_EVENT_TYPE_CATCHKING);
#endif
	//PyModule_AddIntConstant(poModule, "INGAME_EVENT_TYPE_MINIBOSS", CPythonInGameEventSystemManager::INGAME_EVENT_TYPE_MINIBOSS);
#if defined(ENABLE_SUMMER_EVENT_ROULETTE)
	PyModule_AddIntConstant(poModule, "INGAME_EVENT_TYPE_ROULETTE", CPythonInGameEventSystemManager::INGAME_EVENT_TYPE_ROULETTE);
#endif
	//PyModule_AddIntConstant(poModule, "INGAME_EVENT_TYPE_BLACK_AND_WHITE", CPythonInGameEventSystemManager::INGAME_EVENT_TYPE_BLACK_AND_WHITE);
	//PyModule_AddIntConstant(poModule, "INGAME_EVENT_TYPE_WORLD_BOSS", CPythonInGameEventSystemManager::INGAME_EVENT_TYPE_WORLD_BOSS);
	//PyModule_AddIntConstant(poModule, "INGAME_EVENT_TYPE_BATTLE_ROYALE", CPythonInGameEventSystemManager::INGAME_EVENT_TYPE_BATTLE_ROYALE);
	//PyModule_AddIntConstant(poModule, "INGAME_EVENT_TYPE_METINSTONE_RAIN_EVENT", CPythonInGameEventSystemManager::INGAME_EVENT_TYPE_METINSTONE_RAIN_EVENT);
	//PyModule_AddIntConstant(poModule, "INGAME_EVENT_TYPE_OTHER_WORLD", CPythonInGameEventSystemManager::INGAME_EVENT_TYPE_OTHER_WORLD);
	//PyModule_AddIntConstant(poModule, "INGAME_EVENT_TYPE_OTHER_WORLD_EVE", CPythonInGameEventSystemManager::INGAME_EVENT_TYPE_OTHER_WORLD_EVE);
	//PyModule_AddIntConstant(poModule, "INGAME_EVENT_TYPE_GOLDEN_LAND_EVENT", CPythonInGameEventSystemManager::INGAME_EVENT_TYPE_GOLDEN_LAND_EVENT);
	//PyModule_AddIntConstant(poModule, "INGAME_EVENT_TYPE_SPORTS_MATCH_EVENT", CPythonInGameEventSystemManager::INGAME_EVENT_TYPE_SPORTS_MATCH_EVENT);
#if defined(ENABLE_SNOWFLAKE_STICK_EVENT)
	PyModule_AddIntConstant(poModule, "INGAME_EVENT_TYPE_SNOWFLAKE_STICK_EVENT", CPythonInGameEventSystemManager::INGAME_EVENT_TYPE_SNOWFLAKE_STICK_EVENT);
#endif
	//PyModule_AddIntConstant(poModule, "INGAME_EVENT_TYPE_TREASURE_HUNT", CPythonInGameEventSystemManager::INGAME_EVENT_TYPE_TREASURE_HUNT);
	PyModule_AddIntConstant(poModule, "INGAME_EVENT_TYPE_MAX", CPythonInGameEventSystemManager::INGAME_EVENT_TYPE_MAX);

	PyModule_AddIntConstant(poModule, "INGAME_EVENT_REWARD_LIST_MIN", CPythonInGameEventSystemManager::INGAME_EVENT_REWARD_LIST_MIN);
	PyModule_AddIntConstant(poModule, "INGAME_EVENT_REWARD_LIST_MAX", CPythonInGameEventSystemManager::INGAME_EVENT_REWARD_LIST_MAX);
}
#endif // ENABLE_EVENT_BANNER
