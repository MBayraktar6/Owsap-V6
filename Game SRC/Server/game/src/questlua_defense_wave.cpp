/**
* Filename: questlua_defense_wave.cpp
* Author: Owsap
**/

#include "stdafx.h"

#if defined(__DEFENSE_WAVE__)
#include "questmanager.h"
#include "defense_wave.h"
#include "party.h"

namespace quest
{
	int dw_enter(lua_State* L)
	{
		if (!lua_isnumber(L, 1))
		{
			sys_err("QUEST: Wrong argument");
			return 0;
		}

		LPCHARACTER pChar = CQuestManager::Instance().GetCurrentCharacterPtr();
		if (pChar == NULL)
		{
			lua_pushboolean(L, false);
			return 1;
		}

		lua_pushboolean(L, CDefenseWaveManager::Instance().Enter(pChar,
			static_cast<DWORD>(lua_tonumber(L, 1))));
		return 1;
	}

	int dw_exit(lua_State* L)
	{
		if (!lua_isnumber(L, 1))
		{
			sys_err("QUEST: Wrong argument");
			return 0;
		}

		LPCHARACTER pChar = CQuestManager::Instance().GetCurrentCharacterPtr();
		if (pChar == NULL)
		{
			lua_pushboolean(L, false);
			return 1;
		}

		lua_pushboolean(L, CDefenseWaveManager::Instance().Exit(pChar,
			static_cast<DWORD>(lua_tonumber(L, 1))));
		return 1;
	}

	int dw_start(lua_State* L)
	{
		LPCHARACTER pChar = CQuestManager::Instance().GetCurrentCharacterPtr();
		if (pChar == NULL)
			return 0;

		LPDEFENSE_WAVE pDefenseWave = pChar->GetDefenseWave();
		if (pDefenseWave == NULL)
			return 0;

		pDefenseWave->Start();
		return 0;
	}

	int dw_find(lua_State* L)
	{
		if (!lua_isnumber(L, 1))
		{
			sys_err("QUEST: Wrong argument");
			return 0;
		}

		DWORD dwMapIndex = static_cast<DWORD>(lua_tonumber(L, 1));

		LPDEFENSE_WAVE pDefenseWave = CDefenseWaveManager::Instance().FindByMapIndex(dwMapIndex);
		lua_pushboolean(L, pDefenseWave ? true : false);
		return 1;
	}

	int dw_is_started(lua_State* L)
	{
		LPCHARACTER pChar = CQuestManager::Instance().GetCurrentCharacterPtr();
		if (pChar == NULL)
		{
			lua_pushboolean(L, false);
			return 1;
		}

		LPDEFENSE_WAVE pDefenseWave = pChar->GetDefenseWave();
		if (pDefenseWave == NULL)
		{
			lua_pushboolean(L, false);
			return 1;
		}

		lua_pushboolean(L, pDefenseWave->GetWave() > CDefenseWave::WAVE0);
		return 1;
	}

	int dw_set_alliance_hp_pct(lua_State* L)
	{
		if (!lua_isnumber(L, 1))
		{
			sys_err("QUEST: Wrong argument");
			return 0;
		}

		LPCHARACTER pChar = CQuestManager::Instance().GetCurrentCharacterPtr();
		if (pChar == NULL)
			return 0;

		const BYTE bHealthPct = static_cast<BYTE>(lua_tonumber(L, 1));
		LPDEFENSE_WAVE pDefenseWave = pChar->GetDefenseWave();
		if (pDefenseWave == NULL)
			return 0;

		if (pDefenseWave->ChangeAllianceHP(bHealthPct))
			pChar->AddAffect(AFFECT_STUN, POINT_NONE, 0, AFF_STUN, 5, 0, true);

		return 0;
	}

	void RegisterDefenseWaveFunctionTable()
	{
		luaL_reg dw_functions[] =
		{
			{ "enter", dw_enter },
			{ "exit", dw_exit },
			{ "start", dw_start },
			{ "find", dw_find },
			{ "is_started", dw_is_started },
			{ "set_alliance_hp_pct", dw_set_alliance_hp_pct },
			{ NULL, NULL }
		};

		CQuestManager::instance().AddLuaFunctionTable("dw", dw_functions);
	}
}
#endif // __DEFENSE_WAVE__
