#include "stdafx.h"
#include "questlua.h"
#include "questmanager.h"
#include "desc_client.h"
#include "char.h"
#include "item_manager.h"
#include "item.h"
#include "cmd.h"
#include "packet.h"
#include "utils.h"
#include "config.h"

#if defined(__DICE_SYSTEM__)
#	include "party.h"
#endif

#if defined(__SUMMER_EVENT_ROULETTE__)
#	include "minigame_roulette.h"
#endif

#if defined(__GUILD_DRAGONLAIR_SYSTEM__)
#	include "db.h"
#endif

#undef sys_err
#ifndef __WIN32__
#	define sys_err(fmt, args...) quest::CQuestManager::instance().QuestError(__FUNCTION__, __LINE__, fmt, ##args)
#else
#	define sys_err(fmt, ...) quest::CQuestManager::instance().QuestError(__FUNCTION__, __LINE__, fmt, __VA_ARGS__)
#endif

extern ACMD(do_in_game_mall);

namespace quest
{
	int game_set_event_flag(lua_State* L)
	{
		CQuestManager& q = CQuestManager::instance();

		if (lua_isstring(L, 1) && lua_isnumber(L, 2))
			q.RequestSetEventFlag(lua_tostring(L, 1), (int)lua_tonumber(L, 2));

		return 0;
	}

	int game_get_event_flag(lua_State* L)
	{
		CQuestManager& q = CQuestManager::instance();

		if (lua_isstring(L, 1))
			lua_pushnumber(L, q.GetEventFlag(lua_tostring(L, 1)));
		else
			lua_pushnumber(L, 0);

		return 1;
	}

	int game_request_make_guild(lua_State* L)
	{
		CQuestManager& q = CQuestManager::instance();
		LPDESC d = q.GetCurrentCharacterPtr()->GetDesc();
		if (d)
		{
			BYTE header = HEADER_GC_REQUEST_MAKE_GUILD;
			d->Packet(&header, 1);
		}
		return 0;
	}

	int game_get_safebox_level(lua_State* L)
	{
		CQuestManager& q = CQuestManager::instance();
		lua_pushnumber(L, q.GetCurrentCharacterPtr()->GetSafeboxSize() / SAFEBOX_PAGE_SIZE);
		return 1;
	}

	int game_set_safebox_level(lua_State* L)
	{
		CQuestManager& q = CQuestManager::instance();

		//q.GetCurrentCharacterPtr()->ChangeSafeboxSize(3*(int)lua_tonumber(L,-1));
		TSafeboxChangeSizePacket p;
		p.dwID = q.GetCurrentCharacterPtr()->GetDesc()->GetAccountTable().id;
		p.bSize = (int)lua_tonumber(L, -1);
		db_clientdesc->DBPacket(HEADER_GD_SAFEBOX_CHANGE_SIZE, q.GetCurrentCharacterPtr()->GetDesc()->GetHandle(), &p, sizeof(p));

		q.GetCurrentCharacterPtr()->SetSafeboxSize(SAFEBOX_PAGE_SIZE * (int)lua_tonumber(L, -1));
		return 0;
	}

	int game_open_safebox(lua_State* /*L*/)
	{
		CQuestManager& q = CQuestManager::instance();
		LPCHARACTER ch = q.GetCurrentCharacterPtr();
		ch->SetSafeboxOpenPosition();
		ch->ChatPacket(CHAT_TYPE_COMMAND, "ShowMeSafeboxPassword");
		return 0;
	}

	int game_open_mall(lua_State* /*L*/)
	{
		CQuestManager& q = CQuestManager::instance();
		LPCHARACTER ch = q.GetCurrentCharacterPtr();
		ch->SetSafeboxOpenPosition();
		ch->ChatPacket(CHAT_TYPE_COMMAND, "ShowMeMallPassword");
		return 0;
	}

	int game_drop_item(lua_State* L)
	{
		//
		// Syntax: game.drop_item(50050, 1)
		//
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		DWORD item_vnum = (DWORD)lua_tonumber(L, 1);
		int count = (int)lua_tonumber(L, 2);
		long x = ch->GetX();
		long y = ch->GetY();

		LPITEM item = ITEM_MANAGER::instance().CreateItem(item_vnum, count);

		if (!item)
		{
			sys_err("cannot create item vnum %d count %d", item_vnum, count);
			return 0;
		}

		PIXEL_POSITION pos;
		pos.x = x + number(-200, 200);
		pos.y = y + number(-200, 200);

		item->AddToGround(ch->GetMapIndex(), pos);
		item->StartDestroyEvent();

		return 0;
	}

	int game_drop_item_with_ownership(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		LPITEM item = NULL;
		switch (lua_gettop(L))
		{
			case 1:
				item = ITEM_MANAGER::instance().CreateItem((DWORD)lua_tonumber(L, 1));
				break;
			case 2:
			case 3:
				item = ITEM_MANAGER::instance().CreateItem((DWORD)lua_tonumber(L, 1), (int)lua_tonumber(L, 2));
				break;
			default:
				return 0;
		}

		if (item == NULL)
			return 0;

#if defined(__PET_SYSTEM__) && defined(__PET_LOOT__)
		if (ch->GetPetSystem() && ch->GetPetSystem()->LootItem(item))
		{
			ch->AutoGiveItem(item, true, true
#if defined(__WJ_PICKUP_ITEM_EFFECT__)
				, true
#endif
			);
			return 0;
		}
#endif

		if (lua_isnumber(L, 3))
		{
			int sec = (int)lua_tonumber(L, 3);
			if (sec <= 0)
			{
				item->SetOwnership(ch);
			}
			else
			{
				item->SetOwnership(ch, sec);
			}
		}
		else
			item->SetOwnership(ch);

		PIXEL_POSITION pos;
		pos.x = ch->GetX() + number(-200, 200);
		pos.y = ch->GetY() + number(-200, 200);

		item->AddToGround(ch->GetMapIndex(), pos);
		item->StartDestroyEvent();

		return 0;
	}

#if defined(__DICE_SYSTEM__)
	int game_drop_item_with_ownership_and_dice(lua_State* L)
	{
		LPITEM item = NULL;
		switch (lua_gettop(L))
		{
			case 1:
				item = ITEM_MANAGER::instance().CreateItem((DWORD)lua_tonumber(L, 1));
				break;
			case 2:
			case 3:
				item = ITEM_MANAGER::instance().CreateItem((DWORD)lua_tonumber(L, 1), (int)lua_tonumber(L, 2));
				break;
			default:
				return 0;
		}

		if (item == NULL)
		{
			return 0;
		}

		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		FPartyDropDiceRoll f(item, ch);
		f.Process(NULL);

#if defined(__PET_SYSTEM__) && defined(__PET_LOOT__)
		if ((f.GetItemOwner() == ch) && (ch->GetPetSystem() && ch->GetPetSystem()->LootItem(item)))
		{
			ch->AutoGiveItem(item, true, true
#if defined(__WJ_PICKUP_ITEM_EFFECT__)
				, true
#endif
			);
			return 0;
		}
#endif

		if (lua_isnumber(L, 3))
		{
			int sec = (int)lua_tonumber(L, 3);
			if (sec <= 0)
			{
				item->SetOwnership(ch);
			}
			else
			{
				item->SetOwnership(ch, sec);
			}
		}
		else
			item->SetOwnership(ch);

		PIXEL_POSITION pos;
		pos.x = ch->GetX() + number(-200, 200);
		pos.y = ch->GetY() + number(-200, 200);

		item->AddToGround(ch->GetMapIndex(), pos);
		item->StartDestroyEvent();

		return 0;
	}
#endif

	int game_web_mall(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		if (ch != NULL)
		{
			do_in_game_mall(ch, const_cast<char*>(""), 0, 0);
		}
		return 0;
	}

#if defined(__MINI_GAME_RUMI__)
	int game_get_minigame_rumi_score(lua_State* L)
	{
		bool bTotal = lua_toboolean(L, 1);
		CMiniGameRumi::GetScoreTable(L, bTotal);
		return 1;
	}

	int game_get_minigame_rumi_my_score(lua_State* L)
	{
		LPCHARACTER pChar = CQuestManager::instance().GetCurrentCharacterPtr();
		bool bTotal = lua_toboolean(L, 1);
		lua_pushnumber(L, pChar ? CMiniGameRumi::GetMyScoreValue(L, pChar->GetPlayerID(), bTotal) : 0);
		return 1;
	}
#endif

#if defined(__MINI_GAME_YUTNORI__)
	int game_get_minigame_yutnori_score(lua_State* L)
	{
		bool bTotal = lua_toboolean(L, 1);
		CMiniGameYutnori::GetScoreTable(L, bTotal);
		return 1;
	}

	int game_get_minigame_yutnori_my_score(lua_State* L)
	{
		LPCHARACTER pChar = CQuestManager::instance().GetCurrentCharacterPtr();
		bool bTotal = lua_toboolean(L, 1);
		lua_pushnumber(L, pChar ? CMiniGameYutnori::GetMyScoreValue(L, pChar->GetPlayerID(), bTotal) : 0);
		return 1;
	}
#endif

#if defined(__MINI_GAME_CATCH_KING__)
	int game_get_catchking_score(lua_State* L)
	{
		const bool c_bTotal = lua_toboolean(L, 1);
		CMiniGameCatchKing::GetScore(L, c_bTotal);
		return 1;
	}

	int game_get_catchking_myscore(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		lua_pushnumber(L, CMiniGameCatchKing::GetMyScore(ch));
		return 1;
	}
#endif

	int game_get_config(lua_State* L)
	{
		if (lua_isstring(L, 1))
		{
			bool bValue = 0;
			const char* szGameConfig = lua_tostring(L, 1);

			/* config_init */
			if (strcmp(szGameConfig, "create_with_full_set") == 0)
				bValue = g_bCreateWithFullSet;

			lua_pushnumber(L, bValue);
		}
		else
			lua_pushnumber(L, 0);

		return 1;
	}

#if defined(__MOVE_COSTUME_ATTR__)
	int game_open_item_comb(lua_State* L)
	{
		const LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		if (ch)
			ch->OpenItemComb();

		return 0;
	}
#endif

#if defined(__MAILBOX__)
	int game_open_mailbox(lua_State* L)
	{
		CMailBox::Open(CQuestManager::instance().GetCurrentCharacterPtr());
		return 0;
	}

	int game_send_gm_mail(lua_State* L)
	{
		if (false == (lua_isstring(L, 1) && lua_isstring(L, 2) && lua_isstring(L, 3)
			&& lua_isnumber(L, 4) && lua_isnumber(L, 5) && lua_isnumber(L, 6) && lua_isnumber(L, 7)))
		{
			sys_err("Wrong argument.");
			return 0;
		}

		const char* const cPlayerName = lua_tostring(L, 1);
		const char* const cTitle = lua_tostring(L, 2);
		const char* const cMessage = lua_tostring(L, 3);
		const DWORD dwItemVnum = static_cast<DWORD>(lua_tonumber(L, 4));
		const DWORD dwItemCount = static_cast<DWORD>(lua_tonumber(L, 5));
		const int iYang = static_cast<int>(lua_tonumber(L, 6));
		const int iWon = static_cast<int>(lua_tonumber(L, 7));

		CMailBox::SendGMMail(cPlayerName, cTitle, cMessage, dwItemVnum, dwItemCount, iYang, iWon);
		return 0;
	}
#endif

#if defined(__ACCE_COSTUME_SYSTEM__)
	int game_open_acce_refine(lua_State* L)
	{
		const LPCHARACTER pChar = CQuestManager::instance().GetCurrentCharacterPtr();
		if (pChar == nullptr)
			return 0;

		if (lua_isnumber(L, 1))
		{
			const BYTE bType = static_cast<BYTE>(lua_tonumber(L, 1));
			pChar->AcceRefineWindowOpen(CQuestManager::instance().GetCurrentNPCCharacterPtr(), bType);
		}
		return 0;
	}
#endif

#if defined(__AURA_COSTUME_SYSTEM__)
	int game_open_aura_refine(lua_State* L)
	{
		const LPCHARACTER pChar = CQuestManager::instance().GetCurrentCharacterPtr();
		if (pChar == nullptr)
			return 0;

		if (lua_isnumber(L, 1))
		{
			const BYTE bType = static_cast<BYTE>(lua_tonumber(L, 1));
			pChar->OpenAuraRefineWindow(CQuestManager::instance().GetCurrentNPCCharacterPtr(), bType);
		}

		return 0;
	}
#endif

#if defined(__CHANGE_LOOK_SYSTEM__)
	int game_open_changelook(lua_State* L)
	{
		const LPCHARACTER pChar = CQuestManager::instance().GetCurrentCharacterPtr();
		if (pChar == nullptr)
			return 0;

		if (lua_isnumber(L, 1))
		{
			const BYTE bType = static_cast<BYTE>(lua_tonumber(L, 1));
			CChangeLook::Open(pChar, bType);
		}
		return 0;
	}
#endif

#if defined(__RANKING_SYSTEM__)
	int game_open_ranking(lua_State* L)
	{
		if (lua_isnumber(L, 1) && lua_isnumber(L, 2))
		{
			const LPCHARACTER c_lpCh = CQuestManager::instance().GetCurrentCharacterPtr();
			const BYTE c_bType = lua_tonumber(L, 1);
			const BYTE c_bCategory = lua_tonumber(L, 2);
			CRanking::Open(c_lpCh, c_bType, c_bCategory);
		}
		return 0;
	}
#endif

#if defined(__GEM_SHOP__)
	int game_open_gem_shop(lua_State* L)
	{
#if defined(__CONQUEROR_LEVEL__)
		bool bSpecial = lua_isboolean(L, 1) ? lua_toboolean(L, 1) : false;
		CGemShop::Open(CQuestManager::instance().GetCurrentCharacterPtr(), bSpecial);
#else
		CGemShop::Open(CQuestManager::instance().GetCurrentCharacterPtr());
#endif
		return 0;
	}
#endif

#if defined(__CUBE_RENEWAL__)
	int game_open_cube(lua_State* L)
	{
		CCubeManager::Instance().OpenCube(CQuestManager::instance().GetCurrentCharacterPtr());
		return 0;
	}
#endif

#if defined(__GUILD_DRAGONLAIR_SYSTEM__)
	int game_open_guild_dragonlair_ranking(lua_State* L)
	{
		if (!lua_isnumber(L, 1) && !lua_isnumber(L, 2))
		{
			sys_err("QUEST: Wrong argument");
			return 0;
		}

		BYTE bType = static_cast<BYTE>(lua_tonumber(L, 1));
		DWORD dwLimit = static_cast<DWORD>(lua_tonumber(L, 2));

		const LPCHARACTER pCharacter = CQuestManager::instance().GetCurrentCharacterPtr();
		if (pCharacter == NULL || pCharacter->GetDesc() == NULL)
			return 0;

		char szQuery[1024];
		snprintf(szQuery, sizeof(szQuery), "SELECT"
			" `ranking`.`type`,"
			" `guild`.`id`,"
			" `guild`.`name`,"
			" `ranking`.`member_count`,"
			" `ranking`.`time`"
			" FROM `guild_dragonlair_ranking%s` AS `ranking`"
			" JOIN `guild%s` AS `guild` ON `ranking`.`guild_id` = `guild`.`id`"
			" WHERE `ranking`.`type` = %u"
			" ORDER BY `ranking`.`time` ASC LIMIT %u;"
			, get_table_postfix()
			, get_table_postfix()
			, bType
			, dwLimit
		);

		std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery(szQuery));
		if (pMsg->uiSQLErrno)
			return 0;

		TEMP_BUFFER TempBuffer;
		MYSQL_ROW RowData;

		while ((RowData = mysql_fetch_row(pMsg->Get()->pSQLResult)))
		{
			BYTE bIndex = 0;
			TPacketGCGuildDragonLairRanking DataTable;

			str_to_number(DataTable.bType, RowData[bIndex++]);
			str_to_number(DataTable.dwGuildID, RowData[bIndex++]);
			strlcpy(DataTable.szGuildName, RowData[bIndex++], sizeof(DataTable.szGuildName));
			str_to_number(DataTable.bMemberCount, RowData[bIndex++]);
			str_to_number(DataTable.dwTime, RowData[bIndex++]);

			TempBuffer.write(&DataTable, sizeof(DataTable));
		}

		TPacketGCGuildDragonLair Packet;
		Packet.bHeader = HEADER_GC_GUILD_DRAGONLAIR;
		Packet.bSubHeader = GUILD_DRAGONLAIR_GC_SUBHEADER_RANKING;
		Packet.wSize = sizeof(Packet) + TempBuffer.size();
		Packet.bType = bType;
		if (TempBuffer.size())
		{
			pCharacter->GetDesc()->BufferedPacket(&Packet, sizeof(Packet));
			pCharacter->GetDesc()->Packet(TempBuffer.read_peek(), TempBuffer.size());
		}
		else
			pCharacter->GetDesc()->Packet(&Packet, sizeof(Packet));

		return 0;
	}
#endif

#if defined(__SUMMER_EVENT_ROULETTE__)
	int game_open_minigame_roulette(lua_State* L)
	{
		const LPCHARACTER pNPC = CQuestManager::instance().GetCurrentNPCCharacterPtr();
		const LPCHARACTER pChar = CQuestManager::instance().GetCurrentCharacterPtr();
		if (lua_isnumber(L, 1) && (pNPC && pChar))
		{
			const BYTE bType = static_cast<BYTE>(lua_tonumber(L, 1));
			CMiniGameRoulette::Open(pNPC, pChar, bType);
		}
		return 0;
	}

	int game_get_minigame_roulette_score(lua_State* L)
	{
		CMiniGameRoulette::GetScoreTable(L);
		return 1;
	}

	int game_get_minigame_roulette_my_score(lua_State* L)
	{
		LPCHARACTER pChar = CQuestManager::instance().GetCurrentCharacterPtr();
		lua_pushnumber(L, pChar ? CMiniGameRoulette::GetMyScoreValue(L, pChar->GetPlayerID()) : 0);
		return 1;
	}
#endif

	void RegisterGameFunctionTable()
	{
		luaL_reg game_functions[] =
		{
			{ "get_safebox_level", game_get_safebox_level },
			{ "request_make_guild", game_request_make_guild },
			{ "set_safebox_level", game_set_safebox_level },
			{ "open_safebox", game_open_safebox },
			{ "open_mall", game_open_mall },
			{ "get_event_flag", game_get_event_flag },
			{ "set_event_flag", game_set_event_flag },
			{ "drop_item", game_drop_item },
			{ "drop_item_with_ownership", game_drop_item_with_ownership },
#if defined(__DICE_SYSTEM__)
			{ "drop_item_with_ownership_and_dice", game_drop_item_with_ownership_and_dice },
#endif
			{ "open_web_mall", game_web_mall },
#if defined(__MINI_GAME_RUMI__)
			{ "get_minigame_rumi_score", game_get_minigame_rumi_score },
			{ "get_minigame_rumi_my_score", game_get_minigame_rumi_my_score },
#endif
#if defined(__MINI_GAME_YUTNORI__)
			{ "get_minigame_yutnori_score", game_get_minigame_yutnori_score },
			{ "get_minigame_yutnori_my_score", game_get_minigame_yutnori_my_score },
#endif
#if defined(__MINI_GAME_CATCH_KING__)
			{ "get_catchking_score", game_get_catchking_score },
			{ "get_catchking_myscore", game_get_catchking_myscore },
#endif
			{ "get_config", game_get_config },
#if defined(__MOVE_COSTUME_ATTR__)
			{ "open_item_comb", game_open_item_comb },
#endif
#if defined(__MAILBOX__)
			{ "open_mailbox", game_open_mailbox },
			{ "send_gm_mail", game_send_gm_mail },
#endif
#if defined(__ACCE_COSTUME_SYSTEM__)
			{ "open_acce_refine", game_open_acce_refine },
#endif
#if defined(__AURA_COSTUME_SYSTEM__)
			{ "open_aura_refine", game_open_aura_refine },
#endif
#if defined(__CHANGE_LOOK_SYSTEM__)
			{ "open_changelook", game_open_changelook },
#endif
#if defined(__RANKING_SYSTEM__)
			{ "open_ranking", game_open_ranking },
#endif
#if defined(__GEM_SHOP__)
			{ "open_gem_shop", game_open_gem_shop },
#endif
#if defined(__CUBE_RENEWAL__)
			{ "open_cube", game_open_cube },
#endif
#if defined(__GUILD_DRAGONLAIR_SYSTEM__)
			{ "open_guild_dragonlair_ranking", game_open_guild_dragonlair_ranking },
#endif
#if defined(__SUMMER_EVENT_ROULETTE__)
			{ "open_minigame_roulette", game_open_minigame_roulette },
			{ "get_minigame_roulette_score", game_get_minigame_roulette_score },
			{ "get_minigame_roulette_my_score", game_get_minigame_roulette_my_score },
#endif
			{ NULL, NULL }
		};

		CQuestManager::instance().AddLuaFunctionTable("game", game_functions);
	}
}
