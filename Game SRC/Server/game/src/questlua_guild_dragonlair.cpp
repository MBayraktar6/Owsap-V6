/**
* Filename: questlua_guild_dragonlair.cpp
* Author: Owsap
**/

#include "stdafx.h"

#if defined(__GUILD_DRAGONLAIR_SYSTEM__)
#include "questmanager.h"
#include "guild_dragonlair.h"
#include "char.h"
#include "char_manager.h"
#include "guild.h"
#include "guild_manager.h"

namespace quest
{
	// Syntax in LUA: guild_dragonlair.find_guild(arg1)
	// arg1: number (guild id)
	int guild_dragonlair_find_guild(lua_State* L)
	{
		if (!lua_isnumber(L, 1))
		{
			sys_err("QUEST: Wrong argument");
			return 0;
		}
		const DWORD dwGuildID = (DWORD)lua_tonumber(L, 1);
		lua_pushboolean(L, CGuildDragonLairManager::Instance().FindGuild(dwGuildID));
		return 1;
	}

	// Syntax in LUA: guild_dragonlair.can_register()
	int guild_dragonlair_can_register(lua_State* L)
	{
		const LPCHARACTER pCharacter = CQuestManager::Instance().GetCurrentCharacterPtr();
		lua_pushboolean(L, pCharacter != NULL ? CGuildDragonLairManager::Instance().CanRegister(pCharacter) : false);
		return 1;
	}

	// Syntax in LUA: guild_dragonlair.register(arg1, arg2)
	// arg1: number (lair type)
	// arg2: boolean (ticket)
	int guild_dragonlair_register_guild(lua_State* L)
	{
		if (!lua_isnumber(L, 1) && !lua_isnumber(L, 2))
		{
			sys_err("QUEST: Wrong argument");
			return 0;
		}

		BYTE bType = static_cast<BYTE>(lua_tonumber(L, 1));
		bool bTicket = static_cast<bool>(lua_tonumber(L, 2));

		const LPCHARACTER pCharacter = CQuestManager::Instance().GetCurrentCharacterPtr();
		lua_pushboolean(L, pCharacter ? CGuildDragonLairManager::Instance().RegisterGuild(pCharacter, bType, bTicket) : false);
		return 1;
	}

	// Syntax in LUA: guild_dragonlair.enter_guild(arg1)
	// arg1: number (lair type)
	int guild_dragonlair_enter_guild(lua_State* L)
	{
		if (!lua_isnumber(L, 1))
		{
			sys_err("QUEST: Wrong argument");
			return 0;
		}

		BYTE bType = static_cast<BYTE>(lua_tonumber(L, 1));

		const LPCHARACTER pCharacter = CQuestManager::Instance().GetCurrentCharacterPtr();
		lua_pushboolean(L, pCharacter ? CGuildDragonLairManager::Instance().EnterGuild(pCharacter, bType) : false);
		return 1;
	}

	// Syntax in LUA: guild_dragonlair.exit()
	int guild_dragonlair_exit(lua_State* L)
	{
		const LPCHARACTER pCharacter = CQuestManager::Instance().GetCurrentCharacterPtr();
		if (pCharacter != NULL) CGuildDragonLairManager::Instance().Exit(pCharacter);
		return 0;
	}

	// Syntax in LUA: guild_dragonlair.start()
	int guild_dragonlair_start(lua_State* L)
	{
		const LPCHARACTER pCharacter = CQuestManager::Instance().GetCurrentCharacterPtr();
		lua_pushboolean(L, pCharacter != NULL ? CGuildDragonLairManager::Instance().Start(pCharacter) : false);
		return 1;
	}

	// Syntax in LUA: guild_dragonlair.cancel()
	int guild_dragonlair_cancel(lua_State* L)
	{
		const LPCHARACTER pCharacter = CQuestManager::Instance().GetCurrentCharacterPtr();
		lua_pushboolean(L, pCharacter != NULL ? CGuildDragonLairManager::Instance().Cancel(pCharacter) : false);
		return 1;
	}

	// Syntax in LUA: guild_dragonlair.cancel_guild(arg1)
	// arg1: string (guild name)
	int guild_dragonlair_cancel_guild(lua_State* L)
	{
		if (!lua_isstring(L, 1))
		{
			sys_err("QUEST: Wrong argument");
			return 0;
		}
		const char* szpGuildName = (const char*)lua_tostring(L, 1);
		lua_pushboolean(L, CGuildDragonLairManager::Instance().CancelGuild(szpGuildName));
		return 1;
	}

	// Syntax in LUA: guild_dragonlair.is_exit()
	int guild_dragonlair_is_exit(lua_State* L)
	{
		const LPCHARACTER pCharacter = CQuestManager::Instance().GetCurrentCharacterPtr();
		lua_pushboolean(L, pCharacter != NULL ? CGuildDragonLairManager::Instance().IsExit(pCharacter) : false);
		return 1;
	}

	// Syntax in LUA: guild_dragonlair.get_guild_stage(arg1)
	// arg1: number (guild id)
	int guild_dragonlair_get_guild_stage(lua_State* L)
	{
		if (!lua_isnumber(L, 1))
		{
			sys_err("QUEST: Wrong argument");
			return 0;
		}
		const DWORD dwGuildID = (DWORD)lua_tonumber(L, 1);
		lua_pushnumber(L, CGuildDragonLairManager::Instance().GetGuildStage(dwGuildID));
		return 1;
	}

	// Syntax in LUA: guild_dragonlair.get_guild_member_count(arg1)
	// arg1: number (guild id)
	int guild_dragonlair_get_guild_member_count(lua_State* L)
	{
		if (!lua_isnumber(L, 1))
		{
			sys_err("QUEST: Wrong argument");
			return 0;
		}
		const DWORD dwGuildID = (DWORD)lua_tonumber(L, 1);
		lua_pushnumber(L, CGuildDragonLairManager::Instance().GetGuildMemberCount(dwGuildID));
		return 1;
	}

	// Syntax in LUA: guild_dragonlair.get_stage(arg1)
	// arg1: number (guild id)
	int guild_dragonlair_get_stage(lua_State* L)
	{
		const LPCHARACTER pCharacter = CQuestManager::Instance().GetCurrentCharacterPtr();
		lua_pushnumber(L, pCharacter != NULL ? CGuildDragonLairManager::Instance().GetStage(pCharacter) : false);
		return 1;
	}

	// Syntax in LUA: guild_dragonlair.give_reward(arg1)
	// arg1: number (item vnum)
	int guild_dragonlair_give_reward(lua_State* L)
	{
		if (!lua_isnumber(L, 1))
		{
			sys_err("QUEST: Wrong argument");
			return 0;
		}
		const LPCHARACTER pCharacter = CQuestManager::Instance().GetCurrentCharacterPtr();
		const DWORD dwRewardVnum = static_cast<DWORD>(lua_tonumber(L, 1));
		lua_pushboolean(L, pCharacter != NULL ? CGuildDragonLairManager::Instance().GiveReward(pCharacter, dwRewardVnum) : false);
		return 1;
	}

	// Syntax in LUA: guild_dragonlair.get_reward()
	int guild_dragonlair_get_reward(lua_State* L)
	{
		const LPCHARACTER pCharacter = CQuestManager::Instance().GetCurrentCharacterPtr();
		lua_pushboolean(L, pCharacter != NULL ? CGuildDragonLairManager::Instance().GetReward(pCharacter) : false);
		return 1;
	}

	// Syntax in LUA: guild_dragonlair.is_lair_map(arg1)
	// arg1: number (map index)
	int guild_dragonlair_is_red_dragonlair(lua_State* L)
	{
		if (!lua_isnumber(L, 1))
		{
			sys_err("QUEST: Wrong argument");
			return 0;
		}
		const long lMapIndex = static_cast<long>(lua_tonumber(L, 1));
		lua_pushboolean(L, CGuildDragonLairManager::Instance().IsRedDragonLair(lMapIndex));
		return 1;
	}

#if defined(__GUILD_DRAGONLAIR_PARTY_SYSTEM__)
	// Syntax in LUA: guild_dragonlair.find_party(arg1)
	// arg1: number (leader pid)
	int guild_dragonlair_find_party(lua_State* L)
	{
		if (!lua_isnumber(L, 1))
		{
			sys_err("QUEST: Wrong argument");
			return 0;
		}
		const DWORD dwLeaderPID = (DWORD)lua_tonumber(L, 1);
		lua_pushboolean(L, CGuildDragonLairManager::Instance().FindParty(dwLeaderPID));
		return 1;
	}

	// Syntax in LUA: guild_dragonlair.enter_party(arg1)
	// arg1: number (lair type)
	int guild_dragonlair_enter_party(lua_State* L)
	{
		if (!lua_isnumber(L, 1))
		{
			sys_err("QUEST: Wrong argument");
			return 0;
		}

		BYTE bType = static_cast<BYTE>(lua_tonumber(L, 1));

		const LPCHARACTER pCharacter = CQuestManager::Instance().GetCurrentCharacterPtr();
		lua_pushboolean(L, pCharacter ? CGuildDragonLairManager::Instance().EnterParty(pCharacter, bType) : false);
		return 1;
	}

	// Syntax in LUA: guild_dragonlair.get_party_stage(arg1)
	// arg1: number (leader pid)
	int guild_dragonlair_get_party_stage(lua_State* L)
	{
		if (!lua_isnumber(L, 1))
		{
			sys_err("QUEST: Wrong argument");
			return 0;
		}
		const DWORD dwLeaderPID = (DWORD)lua_tonumber(L, 1);
		lua_pushnumber(L, CGuildDragonLairManager::Instance().GetPartyStage(dwLeaderPID));
		return 1;
	}

	// Syntax in LUA: guild_dragonlair.get_party_member_count(arg1)
	// arg1: number (leader pid)
	int guild_dragonlair_get_party_member_count(lua_State* L)
	{
		if (!lua_isnumber(L, 1))
		{
			sys_err("QUEST: Wrong argument");
			return 0;
		}
		const DWORD dwLeaderPID = (DWORD)lua_tonumber(L, 1);
		lua_pushnumber(L, CGuildDragonLairManager::Instance().GetPartyMemberCount(dwLeaderPID));
		return 1;
	}
#endif

	void RegisterGuildDragonLairFunctionTable()
	{
		luaL_reg guild_dragonlair[] =
		{
			{ "find_guild", guild_dragonlair_find_guild },
			{ "can_register", guild_dragonlair_can_register },
			{ "register_guild", guild_dragonlair_register_guild },
			{ "enter_guild", guild_dragonlair_enter_guild },
			{ "exit", guild_dragonlair_exit },
			{ "start", guild_dragonlair_start },
			{ "cancel", guild_dragonlair_cancel },
			{ "cancel_guild", guild_dragonlair_cancel_guild },
			{ "is_exit", guild_dragonlair_is_exit },
			{ "get_guild_stage", guild_dragonlair_get_guild_stage },
			{ "get_guild_member_count", guild_dragonlair_get_guild_member_count },
			{ "get_stage", guild_dragonlair_get_stage },
			{ "give_reward", guild_dragonlair_give_reward },
			{ "get_reward", guild_dragonlair_get_reward },
			{ "is_red_dragonlair", guild_dragonlair_is_red_dragonlair },
#if defined(__GUILD_DRAGONLAIR_PARTY_SYSTEM__)
			{ "find_party", guild_dragonlair_find_party },
			{ "enter_party", guild_dragonlair_enter_party },
			{ "get_party_stage", guild_dragonlair_get_party_stage },
			{ "get_party_member_count", guild_dragonlair_get_party_member_count },
#endif
			{ NULL, NULL }
		};

		CQuestManager::instance().AddLuaFunctionTable("guild_dragonlair", guild_dragonlair);
	}
}
#endif
