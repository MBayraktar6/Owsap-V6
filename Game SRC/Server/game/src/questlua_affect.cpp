#include "stdafx.h"
#include "config.h"
#include "questmanager.h"
#include "sectree_manager.h"
#include "char.h"
#include "affect.h"
#include "db.h"

namespace quest
{
	//
	// "affect" Lua functions
	//
	int affect_add(lua_State* L)
	{
		if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2) || !lua_isnumber(L, 3))
		{
			sys_err("invalid argument");
			return 0;
		}

		CQuestManager& q = CQuestManager::instance();

		POINT_TYPE applyOn = (POINT_TYPE)lua_tonumber(L, 1);

		LPCHARACTER ch = q.GetCurrentCharacterPtr();

		if (applyOn >= MAX_APPLY_NUM || applyOn < 1)
		{
			sys_err("apply is out of range : %d", applyOn);
			return 0;
		}

		if (ch->FindAffect(AFFECT_QUEST_START_IDX, applyOn)) // 퀘스트로 인해 같은 곳에 효과가 걸려있으면 스킵
			return 0;

		long value = (long)lua_tonumber(L, 2);
		long duration = (long)lua_tonumber(L, 3);

		ch->AddAffect(AFFECT_QUEST_START_IDX, aApplyInfo[applyOn].wPointType, value, 0, duration, 0, false);

		return 0;
	}

	int affect_remove(lua_State* L)
	{
		CQuestManager& q = CQuestManager::instance();
		int iType;

		if (lua_isnumber(L, 1))
		{
			iType = (int)lua_tonumber(L, 1);

			if (iType == 0)
				iType = q.GetCurrentPC()->GetCurrentQuestIndex() + AFFECT_QUEST_START_IDX;
		}
		else
			iType = q.GetCurrentPC()->GetCurrentQuestIndex() + AFFECT_QUEST_START_IDX;

		q.GetCurrentCharacterPtr()->RemoveAffect(iType);

		return 0;
	}

	int affect_remove_bad(lua_State* L) // 나쁜 효과를 없앰
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		ch->RemoveBadAffect();
		return 0;
	}

	int affect_remove_good(lua_State* L) // 좋은 효과를 없앰
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		ch->RemoveGoodAffect();
		return 0;
	}

	int affect_add_hair(lua_State* L)
	{
		if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2) || !lua_isnumber(L, 3))
		{
			sys_err("invalid argument");
			return 0;
		}

		CQuestManager& q = CQuestManager::instance();

		POINT_TYPE applyOn = (POINT_TYPE)lua_tonumber(L, 1);

		LPCHARACTER ch = q.GetCurrentCharacterPtr();

		if (applyOn >= MAX_APPLY_NUM || applyOn < 1)
		{
			sys_err("apply is out of range : %d", applyOn);
			return 0;
		}

		long value = (long)lua_tonumber(L, 2);
		long duration = (long)lua_tonumber(L, 3);

		ch->AddAffect(AFFECT_HAIR, aApplyInfo[applyOn].wPointType, value, 0, duration, 0, false);

		return 0;
	}

	int affect_remove_hair(lua_State* L) // 헤어 효과를 없앤다.
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		CAffect* pkAff = ch->FindAffect(AFFECT_HAIR);

		if (pkAff != NULL)
		{
			lua_pushnumber(L, pkAff->lDuration);
			ch->RemoveAffect(pkAff);
		}
		else
		{
			lua_pushnumber(L, 0);
		}

		return 1;
	}

	// 현재 캐릭터가 AFFECT_TYPE affect를 갖고있으면 bApplyOn 값을 반환하고 없으면 nil을 반환하는 함수.
	// usage : applyOn = affect.get_apply(AFFECT_TYPE) 
	int affect_get_apply_on(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		if (!lua_isnumber(L, 1))
		{
			sys_err("invalid argument");
			return 0;
		}

		DWORD affectType = lua_tonumber(L, 1);

		CAffect* pkAff = ch->FindAffect(affectType);

		if (pkAff != NULL)
			lua_pushnumber(L, pkAff->wApplyOn);
		else
			lua_pushnil(L);

		return 1;
	}

	int affect_get_apply_value(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		if (!lua_isnumber(L, 1))
		{
			sys_err("invalid argument");
			return 0;
		}

		DWORD affectType = lua_tonumber(L, 1);

		CAffect* pkAff = ch->FindAffect(affectType);

		if (pkAff != NULL)
			lua_pushnumber(L, pkAff->lApplyValue);
		else
			lua_pushnil(L);

		return 1;
	}

	int affect_add_collect(lua_State* L)
	{
		if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2) || !lua_isnumber(L, 3))
		{
			sys_err("invalid argument");
			return 0;
		}

		CQuestManager& q = CQuestManager::instance();

		POINT_TYPE applyOn = lua_tonumber(L, 1);

		LPCHARACTER ch = q.GetCurrentCharacterPtr();

		if (applyOn >= MAX_APPLY_NUM || applyOn < 1)
		{
			sys_err("apply is out of range : %d", applyOn);
			return 0;
		}

		POINT_VALUE value = (POINT_VALUE)lua_tonumber(L, 2);
		POINT_VALUE duration = (POINT_VALUE)lua_tonumber(L, 3);

		ch->AddAffect(AFFECT_COLLECT, aApplyInfo[applyOn].wPointType, value, 0, duration, 0, false);

		return 0;
	}

	int affect_add_collect_point(lua_State* L)
	{
		if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2) || !lua_isnumber(L, 3))
		{
			sys_err("invalid argument");
			return 0;
		}

		CQuestManager& q = CQuestManager::instance();

		POINT_TYPE point_type = (POINT_TYPE)lua_tonumber(L, 1);

		LPCHARACTER ch = q.GetCurrentCharacterPtr();

		if (point_type >= POINT_MAX_NUM || point_type < 1)
		{
			sys_err("point is out of range : %d", point_type);
			return 0;
		}

		long value = (long)lua_tonumber(L, 2);
		long duration = (long)lua_tonumber(L, 3);

		ch->AddAffect(AFFECT_COLLECT, point_type, value, 0, duration, 0, false);

		return 0;
	}

	int affect_remove_collect(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		if (ch != NULL)
		{
			POINT_TYPE wApplyType = lua_tonumber(L, 1);
			if (wApplyType >= MAX_APPLY_NUM)
				return 0;

			wApplyType = aApplyInfo[wApplyType].wPointType;
			POINT_VALUE lApplyValue = (POINT_VALUE)lua_tonumber(L, 2);

			const std::list<CAffect*>& rList = ch->GetAffectContainer();
			const CAffect* pAffect = NULL;

			for (std::list<CAffect*>::const_iterator iter = rList.begin(); iter != rList.end(); ++iter)
			{
				pAffect = *iter;

				if (pAffect->dwType == AFFECT_COLLECT)
				{
					if (pAffect->wApplyOn == wApplyType && pAffect->lApplyValue == lApplyValue)
					{
						break;
					}
				}

				pAffect = NULL;
			}

			if (pAffect != NULL)
			{
				ch->RemoveAffect(const_cast<CAffect*>(pAffect));
			}
		}

		return 0;
	}

	int affect_remove_all_collect(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		if (ch != NULL)
		{
			ch->RemoveAffect(AFFECT_COLLECT);
		}

		return 0;
	}

	int affect_find(lua_State* L)
	{
		if (!lua_isnumber(L, 1))
		{
			sys_err("Invalid arguments: expecting number");
			lua_pushboolean(L, false);
			return 1;
		}

		const DWORD affect_type = static_cast<DWORD>(lua_tonumber(L, 1));
		if (affect_type >= AFFECT_MAX)
		{
			sys_err("Affect type is out of range: %u", affect_type);
			lua_pushboolean(L, false);
			return 1;
		}

		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		if (ch == nullptr)
		{
			sys_err("Failed to retrieve current character pointer");
			lua_pushboolean(L, false);
			return 1;
		}

		lua_pushboolean(L, ch->FindAffect(affect_type) ? true : false);
		return 1;
	}

	int affect_add_buff(lua_State* L)
	{
		if (!lua_isnumber(L, 1) || !lua_istable(L, 2) || !lua_isnumber(L, 3) || !lua_isnumber(L, 4) || !lua_isboolean(L, 5))
		{
			sys_err("Invalid arguments: expecting number, table, number, number");
			lua_pushboolean(L, false);
			return 1;
		}

		const DWORD affect_type = static_cast<DWORD>(lua_tonumber(L, 1));
		if (affect_type >= AFFECT_MAX)
		{
			sys_err("Affect type is out of range: %u", affect_type);
			lua_pushboolean(L, false);
			return 1;
		}

		std::vector<std::pair<POINT_TYPE, POINT_VALUE>> point_vec;

		int i = 1;
		while (true)
		{
			lua_rawgeti(L, 2, i);

			if (lua_isnil(L, -1))
			{
				lua_pop(L, 1);
				break;
			}

			if (!lua_istable(L, -1))
			{
				sys_err("Invalid table structure at index %d", i);
				lua_pop(L, 1);
				++i;
				continue;
			}

			lua_rawgeti(L, -1, 1);
			POINT_TYPE point_type = static_cast<POINT_TYPE>(lua_tonumber(L, -1));
			lua_pop(L, 1);

			lua_rawgeti(L, -1, 2);
			POINT_VALUE point_value = static_cast<POINT_VALUE>(lua_tonumber(L, -1));
			lua_pop(L, 1);

			point_vec.push_back(std::make_pair(point_type, point_value));

			lua_pop(L, 1);
			++i;
		}

		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		if (ch == nullptr)
		{
			sys_err("Failed to retrieve current character pointer");
			lua_pushboolean(L, false);
			return 1;
		}

		const DWORD affect_flag = static_cast<DWORD>(lua_tonumber(L, 3));
		const long affect_duration = static_cast<long>(lua_tonumber(L, 4));
		const bool affect_override = static_cast<bool>(lua_toboolean(L, 5));
		
#if defined(__AFFECT_RENEWAL__)
		bool bRealTime = false;
		if (lua_isboolean(L, 6))
			bRealTime = lua_toboolean(L, 6);
#endif

		if (ch->FindAffect(affect_type) && !affect_override)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_STRING("This effect is already activated."));
			lua_pushboolean(L, false);
			return 1;
		}

		for (const auto& it : point_vec)
		{
			const POINT_TYPE point_type = it.first;
			const POINT_VALUE point_value = it.second;

			if (point_type >= POINT_MAX_NUM)
			{
				sys_err("Point type is out of range: %d", point_type);
				continue;
			}

#if defined(__AFFECT_RENEWAL__)
			if (bRealTime)
				ch->AddRealTimeAffect(affect_type, point_type, point_value, affect_flag, affect_duration, 0, affect_override, true);
			else
				ch->AddAffect(affect_type, point_type, point_value, affect_flag, affect_duration, 0, affect_override, true);
#else
			ch->AddAffect(affect_type, point_type, point_value, affect_flag, affect_duration, 0, affect_override, true);
#endif
		}

		lua_pushboolean(L, true);
		return 1;
	}

	int affect_remove_buff(lua_State* L)
	{
		if (!lua_isnumber(L, 1))
		{
			sys_err("Invalid arguments: expecting number");
			return 0;
		}

		const DWORD affect_type = static_cast<DWORD>(lua_tonumber(L, 1));
		if (affect_type >= AFFECT_MAX)
		{
			sys_err("Affect type is out of range: %u", affect_type);
			return 0;
		}

		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		if (ch == nullptr)
		{
			sys_err("Failed to retrieve current character pointer");
			return 0;
		}

		ch->RemoveAffect(affect_type);
		return 0;
	}

	void RegisterAffectFunctionTable()
	{
		luaL_reg affect_functions[] =
		{
			{ "add", affect_add },
			{ "remove", affect_remove },
			{ "remove_bad", affect_remove_bad },
			{ "remove_good", affect_remove_good },
			{ "add_hair", affect_add_hair },
			{ "remove_hair", affect_remove_hair },
			{ "add_collect", affect_add_collect },
			{ "add_collect_point", affect_add_collect_point },
			{ "remove_collect", affect_remove_collect },
			{ "remove_all_collect", affect_remove_all_collect },
			{ "get_apply_on", affect_get_apply_on },
			{ "get_apply_value", affect_get_apply_value },
			{ "find", affect_find },
			{ "add_buff", affect_add_buff },
			{ "remove_buff", affect_remove_buff },
			{ NULL, NULL }
		};

		CQuestManager::instance().AddLuaFunctionTable("affect", affect_functions);
	}
};
