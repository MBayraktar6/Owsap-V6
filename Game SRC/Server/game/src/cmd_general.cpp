#include "stdafx.h"
#ifdef __FreeBSD__
#include <md5.h>
#else
#include "../../libthecore/include/xmd5.h"
#endif

#include "utils.h"
#include "config.h"
#include "desc_client.h"
#include "desc_manager.h"
#include "char.h"
#include "char_manager.h"
#include "motion.h"
#include "packet.h"
#include "affect.h"
#include "pvp.h"
#include "start_position.h"
#include "party.h"
#include "guild_manager.h"
#include "p2p.h"
#include "dungeon.h"
#include "messenger_manager.h"
#include "war_map.h"
#include "questmanager.h"
#include "item_manager.h"
#include "monarch.h"
#include "mob_manager.h"
#include "dev_log.h"
#include "item.h"
#include "arena.h"
#include "buffer_manager.h"
#include "unique_item.h"
#include "threeway_war.h"
#include "log.h"
#include "../../common/VnumHelper.h"
#ifdef ENABLE_QUEEN_NETHIS
#include "SnakeLair.h"
#endif

extern int g_server_id;

extern int g_nPortalLimitTime;

ACMD(do_user_horse_ride)
{
	if (ch->IsObserverMode())
		return;

	if (ch->IsDead() || ch->IsStun())
		return;

	if (ch->IsHorseRiding() == false)
	{
		// 말이 아닌 다른탈것을 타고있다.
		if (ch->GetMountVnum())
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_STRING("이미 탈것을 이용중입니다."));
			return;
		}

		if (ch->GetHorse() == NULL)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_STRING("말을 먼저 소환해주세요."));
			return;
		}

		ch->StartRiding();
	}
	else
	{
		ch->StopRiding();
	}
}

ACMD(do_user_horse_back)
{
	if (ch->GetHorse() != NULL)
	{
		ch->HorseSummon(false);
		ch->ChatPacket(CHAT_TYPE_INFO, LC_STRING("말을 돌려보냈습니다."));
	}
	else if (ch->IsHorseRiding() == true)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_STRING("말에서 먼저 내려야 합니다."));
	}
	else
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_STRING("말을 먼저 소환해주세요."));
	}
}

ACMD(do_user_horse_feed)
{
	// 개인상점을 연 상태에서는 말 먹이를 줄 수 없다.
	if (ch->GetMyShop())
		return;

	if (!ch->GetHorse())
	{
		if (ch->IsHorseRiding() == false)
			ch->ChatPacket(CHAT_TYPE_INFO, LC_STRING("말을 먼저 소환해주세요."));
		else
			ch->ChatPacket(CHAT_TYPE_INFO, LC_STRING("말을 탄 상태에서는 먹이를 줄 수 없습니다."));
		return;
	}

	DWORD dwFood = ch->GetHorseGrade() + 50054 - 1;

	if (ch->CountSpecifyItem(dwFood) > 0)
	{
		ch->RemoveSpecifyItem(dwFood, 1);
		ch->FeedHorse();

		const char* c_szConv = under_han(ITEM_MANAGER::instance().GetTable(dwFood)->szLocaleName) ? LC_STRING("을") : LC_STRING("를");
		ch->ChatPacket(CHAT_TYPE_INFO, LC_STRING("말에게 %s%s 주었습니다.",
			LC_ITEM(ITEM_MANAGER::instance().GetTable(dwFood)->dwVnum), c_szConv));
	}
	else
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_STRING("%s 아이템이 필요합니다", LC_ITEM(ITEM_MANAGER::instance().GetTable(dwFood)->dwVnum)));
	}
}

#define MAX_REASON_LEN 128

EVENTINFO(TimedEventInfo)
{
	DynamicCharacterPtr ch;
	int subcmd;
	int left_second;
	char szReason[MAX_REASON_LEN];

	TimedEventInfo()
		: ch()
		, subcmd(0)
		, left_second(0)
	{
		::memset(szReason, 0, MAX_REASON_LEN);
	}
};

struct SendDisconnectFunc
{
	void operator () (LPDESC d)
	{
		if (d->GetCharacter())
		{
			if (d->GetCharacter()->GetGMLevel() == GM_PLAYER)
				d->GetCharacter()->ChatPacket(CHAT_TYPE_COMMAND, "quit Shutdown(SendDisconnectFunc)");
		}
	}
};

struct DisconnectFunc
{
	void operator () (LPDESC d)
	{
		if (d->GetType() == DESC_TYPE_CONNECTOR)
			return;

		if (d->IsPhase(PHASE_P2P))
			return;

		if (d->GetCharacter())
			d->GetCharacter()->Disconnect("Shutdown(DisconnectFunc)");

		d->SetPhase(PHASE_CLOSE);
	}
};

EVENTINFO(shutdown_event_data)
{
	int seconds;

	shutdown_event_data()
		: seconds(0)
	{
	}
};

EVENTFUNC(shutdown_event)
{
	shutdown_event_data* info = dynamic_cast<shutdown_event_data*>(event->info);

	if (info == NULL)
	{
		sys_err("shutdown_event> <Factor> Null pointer");
		return 0;
	}

	int* pSec = &(info->seconds);

	if (*pSec < 0)
	{
		sys_log(0, "shutdown_event sec %d", *pSec);

		if (--*pSec == -10)
		{
			const DESC_MANAGER::DESC_SET& c_set_desc = DESC_MANAGER::instance().GetClientSet();
			std::for_each(c_set_desc.begin(), c_set_desc.end(), DisconnectFunc());
			return passes_per_sec;
		}
		else if (*pSec < -10)
			return 0;

		return passes_per_sec;
	}
	else if (*pSec == 0)
	{
		const DESC_MANAGER::DESC_SET& c_set_desc = DESC_MANAGER::instance().GetClientSet();
		std::for_each(c_set_desc.begin(), c_set_desc.end(), SendDisconnectFunc());
		g_bNoMoreClient = true;
		--*pSec;
		return passes_per_sec;
	}
	else
	{
		char buf[64];
		snprintf(buf, sizeof(buf), LC_STRING("셧다운이 %d초 남았습니다.", *pSec));
		SendNotice(buf);

		--*pSec;
		return passes_per_sec;
	}
}

void Shutdown(int iSec)
{
	if (g_bNoMoreClient)
	{
		thecore_shutdown();
		return;
	}

	CWarMapManager::instance().OnShutdown();

	char buf[64];
	snprintf(buf, sizeof(buf), LC_STRING("%d초 후 게임이 셧다운 됩니다.", iSec));

	SendNotice(buf);

	shutdown_event_data* info = AllocEventInfo<shutdown_event_data>();
	info->seconds = iSec;

	event_create(shutdown_event, info, 1);
}

ACMD(do_shutdown)
{
	TPacketGGShutdown p;
	p.bHeader = HEADER_GG_SHUTDOWN;
	P2P_MANAGER::instance().Send(&p, sizeof(TPacketGGShutdown));

	Shutdown(10);
}

EVENTFUNC(timed_event)
{
	TimedEventInfo* info = dynamic_cast<TimedEventInfo*>(event->info);

	if (info == NULL)
	{
		sys_err("timed_event> <Factor> Null pointer");
		return 0;
	}

	LPCHARACTER ch = info->ch;

	if (ch == NULL) // <Factor>
		return 0;

	LPDESC d = ch->GetDesc();

	if (info->left_second <= 0)
	{
		ch->m_pkTimedEvent = NULL;

		if (true == LC_IsEurope() || true == LC_IsYMIR() || true == LC_IsKorea())
		{
			switch (info->subcmd)
			{
				case SCMD_LOGOUT:
				case SCMD_QUIT:
				case SCMD_PHASE_SELECT:
#if defined(__LOCALE_CLIENT__)
				case SCMD_LANGUAGE_CHANGE:
#endif
				{
					TPacketNeedLoginLogInfo acc_info;
					acc_info.dwPlayerID = ch->GetDesc()->GetAccountTable().id;
					db_clientdesc->DBPacket(HEADER_GD_VALID_LOGOUT, 0, &acc_info, sizeof(acc_info));

					LogManager::instance().DetailLoginLog(false, ch);
				}
				break;
			}
		}

		switch (info->subcmd)
		{
			case SCMD_LOGOUT:
				if (d)
					d->SetPhase(PHASE_CLOSE);
				break;

			case SCMD_QUIT:
				ch->ChatPacket(CHAT_TYPE_COMMAND, "quit");
				break;

			case SCMD_PHASE_SELECT:
			{
				ch->Disconnect("timed_event - SCMD_PHASE_SELECT");

				if (d)
				{
					d->SetPhase(PHASE_SELECT);
				}
			}
			break;

#if defined(__LOCALE_CLIENT__)
			case SCMD_LANGUAGE_CHANGE:
			{
				ch->ChatPacket(CHAT_TYPE_COMMAND, "language_change");
				ch->Disconnect("timed_event - SCMD_LANGUAGE_CHANGE");

				if (d)
					d->SetPhase(PHASE_SELECT);
			}

			break;
#endif
		}

		return 0;
	}
	else
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_STRING("%d초 남았습니다.", info->left_second));
		--info->left_second;
	}

	return PASSES_PER_SEC(1);
}

ACMD(do_cmd)
{
	// RECALL_DELAY
	/*
	if (ch->m_pkRecallEvent != NULL)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_STRING("취소 되었습니다."));
		event_cancel(&ch->m_pkRecallEvent);
		return;
	}
	*/
	// END_OF_RECALL_DELAY

	if (ch->m_pkTimedEvent)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_STRING("취소 되었습니다."));
		event_cancel(&ch->m_pkTimedEvent);
		return;
	}

	switch (subcmd)
	{
		case SCMD_LOGOUT:
			ch->ChatPacket(CHAT_TYPE_INFO, LC_STRING("로그인 화면으로 돌아 갑니다. 잠시만 기다리세요."));
			break;

		case SCMD_QUIT:
			ch->ChatPacket(CHAT_TYPE_INFO, LC_STRING("게임을 종료 합니다. 잠시만 기다리세요."));
			break;

		case SCMD_PHASE_SELECT:
#if defined(__LOCALE_CLIENT__)
		case SCMD_LANGUAGE_CHANGE:
#endif
			ch->ChatPacket(CHAT_TYPE_INFO, LC_STRING("캐릭터를 전환 합니다. 잠시만 기다리세요."));
			break;
	}

	int nExitLimitTime = 10;

	if (ch->IsHack(false, true, nExitLimitTime) &&
		false == CThreeWayWar::instance().IsSungZiMapIndex(ch->GetMapIndex()) &&
		(!ch->GetWarMap() || ch->GetWarMap()->GetType() == GUILD_WAR_TYPE_FLAG))
	{
		return;
	}

	switch (subcmd)
	{
		case SCMD_LOGOUT:
		case SCMD_QUIT:
		case SCMD_PHASE_SELECT:
#if defined(__LOCALE_CLIENT__)
		case SCMD_LANGUAGE_CHANGE:
#endif
		{
			TimedEventInfo* info = AllocEventInfo<TimedEventInfo>();

			{
				if (ch->IsPosition(POS_FIGHTING))
					info->left_second = 10;
				else
					info->left_second = 3;
			}

			info->ch = ch;
			info->subcmd = subcmd;
			strlcpy(info->szReason, argument, sizeof(info->szReason));

			ch->m_pkTimedEvent = event_create(timed_event, info, 1);
		}
		break;
	}
}

ACMD(do_mount)
{
	/*
	char arg1[256];
	struct action_mount_param param;

	// 이미 타고 있으면
	if (ch->GetMountingChr())
	{
		char arg2[256];
		two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));

		if (!*arg1 || !*arg2)
			return;

		param.x = atoi(arg1);
		param.y = atoi(arg2);
		param.vid = ch->GetMountingChr()->GetVID();
		param.is_unmount = true;

		float distance = DISTANCE_SQRT(param.x - (DWORD)ch->GetX(), param.y - (DWORD)ch->GetY());

		if (distance > 600.0f)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_STRING("좀 더 가까이 가서 내리세요."));
			return;
		}

		action_enqueue(ch, ACTION_TYPE_MOUNT, &param, 0.0f, true);
		return;
	}

	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
		return;

	LPCHARACTER tch = CHARACTER_MANAGER::instance().Find(atoi(arg1));

	if (!tch->IsNPC() || !tch->IsMountable())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_STRING("거기에는 탈 수 없어요."));
		return;
	}

	float distance = DISTANCE_SQRT(tch->GetX() - ch->GetX(), tch->GetY() - ch->GetY());

	if (distance > 600.0f)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_STRING("좀 더 가까이 가서 타세요."));
		return;
	}

	param.vid = tch->GetVID();
	param.is_unmount = false;

	action_enqueue(ch, ACTION_TYPE_MOUNT, &param, 0.0f, true);
	*/
}

ACMD(do_fishing)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
		return;

	ch->SetRotation(atof(arg1));
	ch->fishing();
}

ACMD(do_console)
{
	ch->ChatPacket(CHAT_TYPE_COMMAND, "ConsoleEnable");
}

ACMD(do_restart)
{
	ch->Restart(subcmd);
}

ACMD(do_stat_reset)
{
	ch->PointChange(POINT_STAT_RESET_COUNT, 12 - ch->GetPoint(POINT_STAT_RESET_COUNT));
}

ACMD(do_stat_minus)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
		return;

	if (ch->IsPolymorphed())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_STRING("둔갑 중에는 능력을 올릴 수 없습니다."));
		return;
	}

	if (ch->GetPoint(POINT_STAT_RESET_COUNT) <= 0)
		return;

	if (!strcmp(arg1, "st"))
	{
		if (ch->GetRealPoint(POINT_ST) <= JobInitialPoints[ch->GetJob()].st)
			return;

		ch->SetRealPoint(POINT_ST, ch->GetRealPoint(POINT_ST) - 1);
		ch->SetPoint(POINT_ST, ch->GetPoint(POINT_ST) - 1);
		ch->ComputePoints();
		ch->PointChange(POINT_ST, 0);
	}
	else if (!strcmp(arg1, "dx"))
	{
		if (ch->GetRealPoint(POINT_DX) <= JobInitialPoints[ch->GetJob()].dx)
			return;

		ch->SetRealPoint(POINT_DX, ch->GetRealPoint(POINT_DX) - 1);
		ch->SetPoint(POINT_DX, ch->GetPoint(POINT_DX) - 1);
		ch->ComputePoints();
		ch->PointChange(POINT_DX, 0);
	}
	else if (!strcmp(arg1, "ht"))
	{
		if (ch->GetRealPoint(POINT_HT) <= JobInitialPoints[ch->GetJob()].ht)
			return;

		ch->SetRealPoint(POINT_HT, ch->GetRealPoint(POINT_HT) - 1);
		ch->SetPoint(POINT_HT, ch->GetPoint(POINT_HT) - 1);
		ch->ComputePoints();
		ch->PointChange(POINT_HT, 0);
		ch->PointChange(POINT_MAX_HP, 0);
	}
	else if (!strcmp(arg1, "iq"))
	{
		if (ch->GetRealPoint(POINT_IQ) <= JobInitialPoints[ch->GetJob()].iq)
			return;

		ch->SetRealPoint(POINT_IQ, ch->GetRealPoint(POINT_IQ) - 1);
		ch->SetPoint(POINT_IQ, ch->GetPoint(POINT_IQ) - 1);
		ch->ComputePoints();
		ch->PointChange(POINT_IQ, 0);
		ch->PointChange(POINT_MAX_SP, 0);
	}
	else
		return;

	ch->PointChange(POINT_STAT, +1);
	ch->PointChange(POINT_STAT_RESET_COUNT, -1);
	ch->ComputePoints();
}

ACMD(do_stat)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
		return;

	if (ch->IsPolymorphed())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_STRING("둔갑 중에는 능력을 올릴 수 없습니다."));
		return;
	}

	if (ch->GetPoint(POINT_STAT) <= 0)
		return;

	POINT_TYPE idx = 0;

	if (!strcmp(arg1, "st"))
		idx = POINT_ST;
	else if (!strcmp(arg1, "dx"))
		idx = POINT_DX;
	else if (!strcmp(arg1, "ht"))
		idx = POINT_HT;
	else if (!strcmp(arg1, "iq"))
		idx = POINT_IQ;
	else
		return;

	if (ch->GetRealPoint(idx) >= gPlayerMaxLevelStats)
		return;

	ch->SetRealPoint(idx, ch->GetRealPoint(idx) + 1);
	ch->SetPoint(idx, ch->GetPoint(idx) + 1);
	ch->ComputePoints();
	ch->PointChange(idx, 0);

	if (idx == POINT_IQ)
	{
		ch->PointChange(POINT_MAX_HP, 0);
	}
	else if (idx == POINT_HT)
	{
		ch->PointChange(POINT_MAX_SP, 0);
	}

	ch->PointChange(POINT_STAT, -1);
	ch->ComputePoints();
}

#if defined(__CONQUEROR_LEVEL__)
ACMD(do_conqueror_point)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
		return;

	if (ch->IsPolymorphed())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_STRING("둔갑 중에는 능력을 올릴 수 없습니다."));
		return;
	}

	if (ch->GetPoint(POINT_CONQUEROR_POINT) <= 0)
		return;

	POINT_VALUE idx = 0;
	if (!strcmp(arg1, "sstr"))
		idx = POINT_SUNGMA_STR;
	else if (!strcmp(arg1, "shp"))
		idx = POINT_SUNGMA_HP;
	else if (!strcmp(arg1, "smove"))
		idx = POINT_SUNGMA_MOVE;
	else if (!strcmp(arg1, "simmune"))
		idx = POINT_SUNGMA_IMMUNE;
	else
		return;

	if (ch->GetRealPoint(idx) >= gPlayerMaxLevelStats)
		return;

	ch->SetRealPoint(idx, ch->GetRealPoint(idx) + 1);
	ch->SetPoint(idx, ch->GetPoint(idx) + 1);
	ch->ComputePoints();
	ch->PointChange(idx, 0);

	ch->PointChange(POINT_CONQUEROR_POINT, -1);
	ch->ComputePoints();

	ch->PointsPacket(); // Refresh points.
}
#endif

ACMD(do_pvp)
{
	if (ch->GetArena() != NULL || CArenaManager::instance().IsArenaMap(ch->GetMapIndex()) == true)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_STRING("대련장에서 사용하실 수 없습니다."));
		return;
	}

	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	DWORD vid = 0;
	str_to_number(vid, arg1);

	LPCHARACTER pkVictim = CHARACTER_MANAGER::instance().Find(vid);

	if (!pkVictim)
		return;

	if (pkVictim->IsNPC())
		return;

#if defined(__MESSENGER_BLOCK_SYSTEM__)
	if (CMessengerManager::instance().IsBlocked(ch->GetName(), pkVictim->GetName()))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_STRING("Unblock %s to continue.", pkVictim->GetName()));
		return;
	}
	else if (CMessengerManager::instance().IsBlocked(pkVictim->GetName(), ch->GetName()))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_STRING("%s has blocked you.", pkVictim->GetName()));
		return;
	}
#endif

	if (pkVictim->GetArena() != NULL)
	{
		pkVictim->ChatPacket(CHAT_TYPE_INFO, LC_STRING("상대방이 대련중입니다."));
		return;
	}

	CPVPManager::instance().Insert(ch, pkVictim);
}

ACMD(do_guildskillup)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
		return;

	if (!ch->GetGuild())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_STRING("<길드> 길드에 속해있지 않습니다."));
		return;
	}

	CGuild* g = ch->GetGuild();
	TGuildMember* gm = g->GetMember(ch->GetPlayerID());
	if (gm->grade == GUILD_LEADER_GRADE)
	{
		DWORD vnum = 0;
		str_to_number(vnum, arg1);
		g->SkillLevelUp(vnum);
	}
	else
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_STRING("<길드> 길드 스킬 레벨을 변경할 권한이 없습니다."));
	}
}

ACMD(do_skillup)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
		return;

	DWORD vnum = 0;
	str_to_number(vnum, arg1);

	if (true == ch->CanUseSkill(vnum))
	{
		ch->SkillLevelUp(vnum);
	}
	else
	{
		switch (vnum)
		{
			case SKILL_HORSE_WILDATTACK:
			case SKILL_HORSE_CHARGE:
			case SKILL_HORSE_ESCAPE:
			case SKILL_HORSE_WILDATTACK_RANGE:

			case SKILL_7_A_ANTI_TANHWAN:
			case SKILL_7_B_ANTI_AMSEOP:
			case SKILL_7_C_ANTI_SWAERYUNG:
			case SKILL_7_D_ANTI_YONGBI:

			case SKILL_8_A_ANTI_GIGONGCHAM:
			case SKILL_8_B_ANTI_YEONSA:
			case SKILL_8_C_ANTI_MAHWAN:
			case SKILL_8_D_ANTI_BYEURAK:

			case SKILL_ADD_HP:
			case SKILL_RESIST_PENETRATE:

#if defined(__7AND8TH_SKILLS__)
			case SKILL_ANTI_PALBANG:
			case SKILL_ANTI_AMSEOP:
			case SKILL_ANTI_SWAERYUNG:
			case SKILL_ANTI_YONGBI:
			case SKILL_ANTI_GIGONGCHAM:
			case SKILL_ANTI_HWAJO:
			case SKILL_ANTI_MARYUNG:
			case SKILL_ANTI_BYEURAK:
			case SKILL_ANTI_SALPOONG:
			case SKILL_HELP_PALBANG:
			case SKILL_HELP_AMSEOP:
			case SKILL_HELP_SWAERYUNG:
			case SKILL_HELP_YONGBI:
			case SKILL_HELP_GIGONGCHAM:
			case SKILL_HELP_HWAJO:
			case SKILL_HELP_MARYUNG:
			case SKILL_HELP_BYEURAK:
			case SKILL_HELP_SALPOONG:
#endif
				ch->SkillLevelUp(vnum);
				break;
		}
	}
}

//
// @version 05/06/20 Bang2ni - 커맨드 처리 Delegate to CHARACTER class
//
ACMD(do_safebox_close)
{
	ch->CloseSafebox();
}

//
// @version 05/06/20 Bang2ni - 커맨드 처리 Delegate to CHARACTER class
//
ACMD(do_safebox_password)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));
	ch->ReqSafeboxLoad(arg1);
}

ACMD(do_safebox_change_password)
{
	char arg1[256];
	char arg2[256];

	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));

	if (!*arg1 || strlen(arg1) > 6)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_STRING("<창고> 잘못된 암호를 입력하셨습니다."));
		return;
	}

	if (!*arg2 || strlen(arg2) > 6)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_STRING("<창고> 잘못된 암호를 입력하셨습니다."));
		return;
	}

	if (LC_IsBrazil() == true)
	{
		for (int i = 0; i < 6; ++i)
		{
			if (arg2[i] == '\0')
				break;

			if (isalpha(arg2[i]) == false)
			{
				ch->ChatPacket(CHAT_TYPE_INFO, LC_STRING("<창고> 비밀번호는 영문자만 가능합니다."));
				return;
			}
		}
	}

	TSafeboxChangePasswordPacket p;

	p.dwID = ch->GetDesc()->GetAccountTable().id;
	strlcpy(p.szOldPassword, arg1, sizeof(p.szOldPassword));
	strlcpy(p.szNewPassword, arg2, sizeof(p.szNewPassword));

	db_clientdesc->DBPacket(HEADER_GD_SAFEBOX_CHANGE_PASSWORD, ch->GetDesc()->GetHandle(), &p, sizeof(p));
}

ACMD(do_mall_password)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1 || strlen(arg1) > 6)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_STRING("<창고> 잘못된 암호를 입력하셨습니다."));
		return;
	}

	int iPulse = thecore_pulse();

	if (ch->GetMall())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_STRING("<창고> 창고가 이미 열려있습니다."));
		return;
	}

	if (iPulse - ch->GetMallLoadTime() < passes_per_sec * 10) // 10초에 한번만 요청 가능
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_STRING("<창고> 창고를 닫은지 10초 안에는 열 수 없습니다."));
		return;
	}

	ch->SetMallLoadTime(iPulse);

	TSafeboxLoadPacket p;
	p.dwID = ch->GetDesc()->GetAccountTable().id;
	strlcpy(p.szLogin, ch->GetDesc()->GetAccountTable().login, sizeof(p.szLogin));
	strlcpy(p.szPassword, arg1, sizeof(p.szPassword));

	db_clientdesc->DBPacket(HEADER_GD_MALL_LOAD, ch->GetDesc()->GetHandle(), &p, sizeof(p));
}

ACMD(do_mall_close)
{
	if (ch->GetMall())
	{
		ch->SetMallLoadTime(thecore_pulse());
		ch->CloseMall();
		ch->Save();
	}
}

ACMD(do_ungroup)
{
	if (!ch->GetParty())
		return;

	if (!CPartyManager::instance().IsEnablePCParty())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_STRING("<파티> 서버 문제로 파티 관련 처리를 할 수 없습니다."));
		return;
	}

	if (ch->GetDungeon()
#if defined(__GUILD_DRAGONLAIR_PARTY_SYSTEM__)
		|| ch->GetGuildDragonLair()
#endif
#if defined(__DEFENSE_WAVE__)
		|| ch->GetDefenseWave()
#endif
		)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_STRING("<파티> 던전 안에서는 파티에서 나갈 수 없습니다."));
		return;
	}

#ifdef ENABLE_QUEEN_NETHIS
	if (SnakeLair::CSnk::instance().IsSnakeMap(ch->GetMapIndex()))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_STRING("<파티> 던전 안에서는 파티에서 나갈 수 없습니다."));
		return;
	}
#endif

	LPPARTY pParty = ch->GetParty();

	if (pParty->GetMemberCount() == 2)
	{
		// party disband
		CPartyManager::instance().DeleteParty(pParty);
	}
	else
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_STRING("<파티> 파티에서 나가셨습니다."));
		//pParty->SendPartyRemoveOneToAll(ch);
		pParty->Quit(ch->GetPlayerID());
		//pParty->SendPartyRemoveAllToOne(ch);
	}
}

ACMD(do_close_shop)
{
	if (ch->GetMyShop())
	{
		ch->CloseMyShop();
		return;
	}
}

ACMD(do_set_walk_mode)
{
	ch->SetNowWalking(true);
	ch->SetWalking(true);
}

ACMD(do_set_run_mode)
{
	ch->SetNowWalking(false);
	ch->SetWalking(false);
}

ACMD(do_war)
{
	// 내 길드 정보를 얻어오고
	CGuild* g = ch->GetGuild();

	if (!g)
		return;

	// 전쟁중인지 체크한번!
	if (g->UnderAnyWar())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_STRING("<길드> 이미 다른 전쟁에 참전 중 입니다."));
		return;
	}

	// 파라메터를 두배로 나누고
	char arg1[256], arg2[256];
	int type = GUILD_WAR_TYPE_FIELD;
	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));

	if (!*arg1)
		return;

	if (*arg2)
	{
		str_to_number(type, arg2);

		if (type < 0 || type >= GUILD_WAR_TYPE_MAX_NUM)
			type = GUILD_WAR_TYPE_FIELD;
	}

	// 길드의 마스터 아이디를 얻어온뒤
	DWORD gm_pid = g->GetMasterPID();

	// 마스터인지 체크(길전은 길드장만이 가능)
	if (gm_pid != ch->GetPlayerID())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_STRING("<길드> 길드전에 대한 권한이 없습니다."));
		return;
	}

	// 상대 길드를 얻어오고
	CGuild* opp_g = CGuildManager::instance().FindGuildByName(arg1);

	if (!opp_g)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_STRING("<길드> 그런 길드가 없습니다."));
		return;
	}

	// 상대길드와의 상태 체크
	switch (g->GetGuildWarState(opp_g->GetID()))
	{
		case GUILD_WAR_NONE:
		{
			if (opp_g->UnderAnyWar())
			{
				ch->ChatPacket(CHAT_TYPE_INFO, LC_STRING("<길드> 상대방 길드가 이미 전쟁 중 입니다."));
				return;
			}

			int iWarPrice = KOR_aGuildWarInfo[type].iWarPrice;

			if (g->GetGuildMoney() < iWarPrice)
			{
				ch->ChatPacket(CHAT_TYPE_INFO, LC_STRING("<길드> 전비가 부족하여 길드전을 할 수 없습니다."));
				return;
			}

			if (opp_g->GetGuildMoney() < iWarPrice)
			{
				ch->ChatPacket(CHAT_TYPE_INFO, LC_STRING("<길드> 상대방 길드의 전비가 부족하여 길드전을 할 수 없습니다."));
				return;
			}
		}
		break;

		case GUILD_WAR_SEND_DECLARE:
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_STRING("이미 선전포고 중인 길드입니다."));
			return;
		}
		break;

		case GUILD_WAR_RECV_DECLARE:
		{
			if (opp_g->UnderAnyWar())
			{
				ch->ChatPacket(CHAT_TYPE_INFO, LC_STRING("<길드> 상대방 길드가 이미 전쟁 중 입니다."));
				g->RequestRefuseWar(opp_g->GetID());
				return;
			}
		}
		break;

		case GUILD_WAR_RESERVE:
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_STRING("<길드> 이미 전쟁이 예약된 길드 입니다."));
			return;
		}
		break;

		case GUILD_WAR_END:
			return;

		default:
			ch->ChatPacket(CHAT_TYPE_INFO, LC_STRING("<길드> 이미 전쟁 중인 길드입니다."));
			g->RequestRefuseWar(opp_g->GetID());
			return;
	}

	if (!g->CanStartWar(type))
	{
		// 길드전을 할 수 있는 조건을 만족하지않는다.
		if (g->GetLadderPoint() == 0)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_STRING("<길드> 레더 점수가 모자라서 길드전을 할 수 없습니다."));
			sys_log(0, "GuildWar.StartError.NEED_LADDER_POINT");
		}
		else if (g->GetMemberCount() < GUILD_WAR_MIN_MEMBER_COUNT)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_STRING("<길드> 길드전을 하기 위해선 최소한 %d명이 있어야 합니다.", GUILD_WAR_MIN_MEMBER_COUNT));
			sys_log(0, "GuildWar.StartError.NEED_MINIMUM_MEMBER[%d]", GUILD_WAR_MIN_MEMBER_COUNT);
		}
		else
		{
			sys_log(0, "GuildWar.StartError.UNKNOWN_ERROR");
		}
		return;
	}

	// 필드전 체크만 하고 세세한 체크는 상대방이 승낙할때 한다.
	if (!opp_g->CanStartWar(GUILD_WAR_TYPE_FIELD))
	{
		if (opp_g->GetLadderPoint() == 0)
			ch->ChatPacket(CHAT_TYPE_INFO, LC_STRING("<길드> 상대방 길드의 레더 점수가 모자라서 길드전을 할 수 없습니다."));
		else if (opp_g->GetMemberCount() < GUILD_WAR_MIN_MEMBER_COUNT)
			ch->ChatPacket(CHAT_TYPE_INFO, LC_STRING("<길드> 상대방 길드의 길드원 수가 부족하여 길드전을 할 수 없습니다."));
		return;
	}

	do
	{
		if (g->GetMasterCharacter() != NULL)
			break;

		CCI* pCCI = P2P_MANAGER::instance().FindByPID(g->GetMasterPID());

		if (pCCI != NULL)
			break;

		ch->ChatPacket(CHAT_TYPE_INFO, LC_STRING("<길드> 상대방 길드의 길드장이 접속중이 아닙니다."));
		g->RequestRefuseWar(opp_g->GetID());
		return;

	} while (false);

	do
	{
		if (opp_g->GetMasterCharacter() != NULL)
			break;

		CCI* pCCI = P2P_MANAGER::instance().FindByPID(opp_g->GetMasterPID());

		if (pCCI != NULL)
			break;

		ch->ChatPacket(CHAT_TYPE_INFO, LC_STRING("<길드> 상대방 길드의 길드장이 접속중이 아닙니다."));
		g->RequestRefuseWar(opp_g->GetID());
		return;

	} while (false);

	g->RequestDeclareWar(opp_g->GetID(), type);
}

ACMD(do_nowar)
{
	CGuild* g = ch->GetGuild();
	if (!g)
		return;

	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
		return;

	DWORD gm_pid = g->GetMasterPID();

	if (gm_pid != ch->GetPlayerID())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_STRING("<길드> 길드전에 대한 권한이 없습니다."));
		return;
	}

	CGuild* opp_g = CGuildManager::instance().FindGuildByName(arg1);

	if (!opp_g)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_STRING("<길드> 그런 길드가 없습니다."));
		return;
	}

	g->RequestRefuseWar(opp_g->GetID());
}

ACMD(do_detaillog)
{
	ch->DetailLog();
}

ACMD(do_monsterlog)
{
	ch->ToggleMonsterLog();
}

ACMD(do_pkmode)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
		return;

	BYTE mode = 0;
	str_to_number(mode, arg1);

	if (mode == PK_MODE_PROTECT)
		return;

	if (ch->GetLevel() < PK_PROTECT_LEVEL && mode != 0)
		return;

	ch->SetPKMode(mode);
}

ACMD(do_messenger_auth)
{
	if (ch->GetArena())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_STRING("대련장에서 사용하실 수 없습니다."));
		return;
	}

	char arg1[256], arg2[256];
	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));

	if (!*arg1 || !*arg2)
		return;

	char answer = LOWER(*arg1);

	if (answer != 'y')
	{
		LPCHARACTER tch = CHARACTER_MANAGER::instance().FindPC(arg2);

		if (tch)
			tch->ChatPacket(CHAT_TYPE_INFO, LC_STRING("%s 님으로 부터 친구 등록을 거부 당했습니다.", ch->GetName()));
	}

	CMessengerManager::instance().AuthToAdd(ch->GetName(), arg2, answer == 'y' ? false : true); // DENY
}

ACMD(do_setblockmode)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (*arg1)
	{
		BYTE flag = 0;
		str_to_number(flag, arg1);
		ch->SetBlockMode(flag);
	}
}

ACMD(do_unmount)
{
#if defined(__MOUNT_COSTUME_SYSTEM__)
	if (const LPITEM pCostumeMount = ch->GetWear(WEAR_COSTUME_MOUNT))
		if (ch->UnequipItem(pCostumeMount) == false)
			return;
#endif
	ch->UnMount(true);
}

ACMD(do_observer_exit)
{
	if (ch->IsObserverMode())
	{
		if (ch->GetWarMap())
			ch->SetWarMap(NULL);

		if (ch->GetArena() != NULL || ch->GetArenaObserverMode() == true)
		{
			ch->SetArenaObserverMode(false);

			if (ch->GetArena() != NULL)
				ch->GetArena()->RemoveObserver(ch->GetPlayerID());

			ch->SetArena(NULL);
			ch->WarpSet(ARENA_RETURN_POINT_X(ch->GetEmpire()), ARENA_RETURN_POINT_Y(ch->GetEmpire()));
		}
		else
		{
			ch->ExitToSavedLocation();
		}
		ch->SetObserverMode(false);
	}
#ifdef ENABLE_QUEEN_NETHIS
	if (ch->IsSnakeMap())
	{
		switch (subcmd)
		{
			case SCMD_RESTART_TOWN:
				sys_log(0, "do_restart: restart town");
				PIXEL_POSITION pos;

				ch->RestartAtSamePos();

				ch->PointChange(POINT_HP, ch->GetMaxHP() - ch->GetHP());
				ch->PointChange(POINT_SP, ch->GetMaxSP() - ch->GetSP());
				ch->ReviveInvisible(3);
				break;

			case SCMD_RESTART_HERE:
				sys_log(0, "do_restart: restart here");
				ch->RestartAtSamePos();
				ch->PointChange(POINT_HP, ch->GetMaxHP() - ch->GetHP());
				ch->PointChange(POINT_SP, ch->GetMaxSP() - ch->GetSP());
				ch->ReviveInvisible(3);
				break;
		}

		return;
	}
#endif
}

ACMD(do_party_request)
{
	if (ch->GetArena())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_STRING("대련장에서 사용하실 수 없습니다."));
		return;
	}

	if (ch->GetParty())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_STRING("이미 파티에 속해 있으므로 가입신청을 할 수 없습니다."));
		return;
	}

	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
		return;

	DWORD vid = 0;
	str_to_number(vid, arg1);
	LPCHARACTER tch = CHARACTER_MANAGER::instance().Find(vid);

	if (tch)
		if (!ch->RequestToParty(tch))
			ch->ChatPacket(CHAT_TYPE_COMMAND, "PartyRequestDenied");
}

ACMD(do_party_request_accept)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
		return;

	DWORD vid = 0;
	str_to_number(vid, arg1);
	LPCHARACTER tch = CHARACTER_MANAGER::instance().Find(vid);

	if (tch)
		ch->AcceptToParty(tch);
}

ACMD(do_party_request_deny)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
		return;

	DWORD vid = 0;
	str_to_number(vid, arg1);
	LPCHARACTER tch = CHARACTER_MANAGER::instance().Find(vid);

	if (tch)
		ch->DenyToParty(tch);
}

ACMD(do_monarch_warpto)
{
	if (true == LC_IsYMIR() || true == LC_IsKorea())
		return;

	if (!CMonarch::instance().IsMonarch(ch->GetPlayerID(), ch->GetEmpire()))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_STRING("군주만이 사용 가능한 기능입니다"));
		return;
	}

	//군주 쿨타임 검사
	if (!ch->IsMCOK(CHARACTER::MI_WARP))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_STRING("%d 초간 쿨타임이 적용중입니다.", ch->GetMCLTime(CHARACTER::MI_WARP)));
		return;
	}

	//군주 몹 소환 비용 
	const int WarpPrice = 10000;

	//군주 국고 검사 
	if (!CMonarch::instance().IsMoneyOk(WarpPrice, ch->GetEmpire()))
	{
		int NationMoney = CMonarch::instance().GetMoney(ch->GetEmpire());
		ch->ChatPacket(CHAT_TYPE_INFO, LC_STRING("국고에 돈이 부족합니다. 현재 : %u 필요금액 : %u", NationMoney, WarpPrice));
		return;
	}

	int x = 0, y = 0;
	char arg1[256];

	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_STRING("사용법: warpto <character name>"));
		return;
	}

	LPCHARACTER tch = CHARACTER_MANAGER::instance().FindPC(arg1);

	if (!tch)
	{
		CCI* pkCCI = P2P_MANAGER::instance().Find(arg1);

		if (pkCCI)
		{
			if (pkCCI->bEmpire != ch->GetEmpire())
			{
				ch->ChatPacket(CHAT_TYPE_INFO, LC_STRING("타제국 유저에게는 이동할수 없습니다"));
				return;
			}

			if (pkCCI->bChannel != g_bChannel)
			{
				ch->ChatPacket(CHAT_TYPE_INFO, LC_STRING("해당 유저는 %d 채널에 있습니다. (현재 채널 %d)", pkCCI->bChannel, g_bChannel));
				return;
			}
			if (!IsMonarchWarpZone(pkCCI->lMapIndex))
			{
				ch->ChatPacket(CHAT_TYPE_INFO, LC_STRING("해당 지역으로 이동할 수 없습니다."));
				return;
			}

			PIXEL_POSITION pos;

			if (!SECTREE_MANAGER::instance().GetCenterPositionOfMap(pkCCI->lMapIndex, pos))
				ch->ChatPacket(CHAT_TYPE_INFO, "Cannot find map (index %d)", pkCCI->lMapIndex);
			else
			{
				//ch->ChatPacket(CHAT_TYPE_INFO, "You warp to (%d, %d)", pos.x, pos.y);
				ch->ChatPacket(CHAT_TYPE_INFO, LC_STRING("%s 에게로 이동합니다", arg1));
				ch->WarpSet(pos.x, pos.y);

				//군주 돈 삭감
				CMonarch::instance().SendtoDBDecMoney(WarpPrice, ch->GetEmpire(), ch);

				//쿨타임 초기화
				ch->SetMC(CHARACTER::MI_WARP);
			}
		}
		else if (NULL == CHARACTER_MANAGER::instance().FindPC(arg1))
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "There is no one by that name");
		}

		return;
	}
	else
	{
		if (tch->GetEmpire() != ch->GetEmpire())
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_STRING("타제국 유저에게는 이동할수 없습니다"));
			return;
		}
		if (!IsMonarchWarpZone(tch->GetMapIndex()))
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_STRING("해당 지역으로 이동할 수 없습니다."));
			return;
		}
		x = tch->GetX();
		y = tch->GetY();
	}

	ch->ChatPacket(CHAT_TYPE_INFO, LC_STRING("%s 에게로 이동합니다", arg1));
	ch->WarpSet(x, y);
	ch->Stop();

	//군주 돈 삭감
	CMonarch::instance().SendtoDBDecMoney(WarpPrice, ch->GetEmpire(), ch);

	//쿨타임 초기화
	ch->SetMC(CHARACTER::MI_WARP);
}

ACMD(do_monarch_transfer)
{
	if (true == LC_IsYMIR() || true == LC_IsKorea())
		return;

	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_STRING("사용법: transfer <name>"));
		return;
	}

	if (!CMonarch::instance().IsMonarch(ch->GetPlayerID(), ch->GetEmpire()))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_STRING("군주만이 사용 가능한 기능입니다"));
		return;
	}

	//군주 쿨타임 검사
	if (!ch->IsMCOK(CHARACTER::MI_TRANSFER))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_STRING("%d 초간 쿨타임이 적용중입니다.", ch->GetMCLTime(CHARACTER::MI_TRANSFER)));
		return;
	}

	//군주 워프 비용 
	const int WarpPrice = 10000;

	//군주 국고 검사 
	if (!CMonarch::instance().IsMoneyOk(WarpPrice, ch->GetEmpire()))
	{
		int NationMoney = CMonarch::instance().GetMoney(ch->GetEmpire());
		ch->ChatPacket(CHAT_TYPE_INFO, LC_STRING("국고에 돈이 부족합니다. 현재 : %u 필요금액 : %u", NationMoney, WarpPrice));
		return;
	}

	LPCHARACTER tch = CHARACTER_MANAGER::instance().FindPC(arg1);

	if (!tch)
	{
		CCI* pkCCI = P2P_MANAGER::instance().Find(arg1);

		if (pkCCI)
		{
			if (pkCCI->bEmpire != ch->GetEmpire())
			{
				ch->ChatPacket(CHAT_TYPE_INFO, LC_STRING("다른 제국 유저는 소환할 수 없습니다."));
				return;
			}
			if (pkCCI->bChannel != g_bChannel)
			{
				ch->ChatPacket(CHAT_TYPE_INFO, LC_STRING("%s 님은 %d 채널에 접속 중 입니다. (현재 채널: %d)", arg1, pkCCI->bChannel, g_bChannel));
				return;
			}
			if (!IsMonarchWarpZone(pkCCI->lMapIndex))
			{
				ch->ChatPacket(CHAT_TYPE_INFO, LC_STRING("해당 지역으로 이동할 수 없습니다."));
				return;
			}
			if (!IsMonarchWarpZone(ch->GetMapIndex()))
			{
				ch->ChatPacket(CHAT_TYPE_INFO, LC_STRING("해당 지역으로 소환할 수 없습니다."));
				return;
			}

			TPacketGGTransfer pgg;

			pgg.bHeader = HEADER_GG_TRANSFER;
			strlcpy(pgg.szName, arg1, sizeof(pgg.szName));
			pgg.lX = ch->GetX();
			pgg.lY = ch->GetY();

			P2P_MANAGER::instance().Send(&pgg, sizeof(TPacketGGTransfer));
			ch->ChatPacket(CHAT_TYPE_INFO, LC_STRING("%s 님을 소환하였습니다.", arg1));

			//군주 돈 삭감
			CMonarch::instance().SendtoDBDecMoney(WarpPrice, ch->GetEmpire(), ch);
			//쿨타임 초기화
			ch->SetMC(CHARACTER::MI_TRANSFER);
		}
		else
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_STRING("입력하신 이름을 가진 사용자가 없습니다."));
		}

		return;
	}

	if (ch == tch)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_STRING("자신을 소환할 수 없습니다."));
		return;
	}

	if (tch->GetEmpire() != ch->GetEmpire())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_STRING("다른 제국 유저는 소환할 수 없습니다."));
		return;
	}
	if (!IsMonarchWarpZone(tch->GetMapIndex()))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_STRING("해당 지역으로 이동할 수 없습니다."));
		return;
	}
	if (!IsMonarchWarpZone(ch->GetMapIndex()))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_STRING("해당 지역으로 소환할 수 없습니다."));
		return;
	}

	//tch->Show(ch->GetMapIndex(), ch->GetX(), ch->GetY(), ch->GetZ());
	tch->WarpSet(ch->GetX(), ch->GetY(), ch->GetMapIndex());

	//군주 돈 삭감
	CMonarch::instance().SendtoDBDecMoney(WarpPrice, ch->GetEmpire(), ch);
	//쿨타임 초기화
	ch->SetMC(CHARACTER::MI_TRANSFER);
}

ACMD(do_monarch_info)
{
	if (CMonarch::instance().IsMonarch(ch->GetPlayerID(), ch->GetEmpire()))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_STRING("나의 군주 정보"));
		TMonarchInfo* p = CMonarch::instance().GetMonarch();
		for (int n = 1; n < 4; ++n)
		{
			if (n == ch->GetEmpire())
				ch->ChatPacket(CHAT_TYPE_INFO, LC_STRING("[%s군주] : %s  보유금액 %lld ", EMPIRE_NAME(n), p->name[n], p->money[n]));
			else
				ch->ChatPacket(CHAT_TYPE_INFO, LC_STRING("[%s군주] : %s  ", EMPIRE_NAME(n), p->name[n]));

		}
	}
	else
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_STRING("군주 정보"));
		TMonarchInfo* p = CMonarch::instance().GetMonarch();
		for (int n = 1; n < 4; ++n)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_STRING("[%s군주] : %s  ", EMPIRE_NAME(n), p->name[n]));

		}
	}

}

ACMD(do_elect)
{
	db_clientdesc->DBPacketHeader(HEADER_GD_COME_TO_VOTE, ch->GetDesc()->GetHandle(), 0);
}

// LUA_ADD_GOTO_INFO
struct GotoInfo
{
	std::string st_name;

	BYTE empire;
	int mapIndex;
	DWORD x, y;

	GotoInfo()
	{
		st_name = "";
		empire = 0;
		mapIndex = 0;

		x = 0;
		y = 0;
	}

	GotoInfo(const GotoInfo& c_src)
	{
		__copy__(c_src);
	}

	void operator = (const GotoInfo& c_src)
	{
		__copy__(c_src);
	}

	void __copy__(const GotoInfo& c_src)
	{
		st_name = c_src.st_name;
		empire = c_src.empire;
		mapIndex = c_src.mapIndex;

		x = c_src.x;
		y = c_src.y;
	}
};

extern void BroadcastNotice(const char* c_pszBuf, const bool c_bBigFont);

ACMD(do_monarch_tax)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Usage: monarch_tax <1-50>");
		return;
	}

	// 군주 검사
	if (!ch->IsMonarch())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_STRING("군주만이 사용할수 있는 기능입니다"));
		return;
	}

	// 세금설정 
	int tax = 0;
	str_to_number(tax, arg1);

	if (tax < 1 || tax > 50)
		ch->ChatPacket(CHAT_TYPE_INFO, LC_STRING("1-50 사이의 수치를 선택해주세요"));

	quest::CQuestManager::instance().SetEventFlag("trade_tax", tax);

	// 군주에게 메세지 하나
	ch->ChatPacket(CHAT_TYPE_INFO, LC_STRING("세금이 %d %로 설정되었습니다"));

	// 공지 
	char szMsg[1024];

	snprintf(szMsg, sizeof(szMsg), "군주의 명으로 세금이 %d %% 로 변경되었습니다", tax);
	BroadcastNotice(szMsg);

	snprintf(szMsg, sizeof(szMsg), "앞으로는 거래 금액의 %d %% 가 국고로 들어가게됩니다.", tax);
	BroadcastNotice(szMsg);

	// 쿨타임 초기화 
	ch->SetMC(CHARACTER::MI_TAX);
}

static const DWORD cs_dwMonarchMobVnums[] =
{
	191, // 산견신
	192, // 저신
	193, // 웅신
	194, // 호신
	391, // 미정
	392, // 은정
	393, // 세랑
	394, // 진희
	491, // 맹환
	492, // 보우
	493, // 구패
	494, // 추흔
	591, // 비류단대장
	691, // 웅귀 족장
	791, // 밀교교주
	1304, // 누렁범귀
	1901, // 구미호
	2091, // 여왕거미
	2191, // 거대사막거북
	2206, // 화염왕i
	0,
};

ACMD(do_monarch_mob)
{
	char arg1[256];
	LPCHARACTER tch;

	one_argument(argument, arg1, sizeof(arg1));

	if (!ch->IsMonarch())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_STRING("군주만이 사용할수 있는 기능입니다"));
		return;
	}

	if (!*arg1)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Usage: mmob <mob name>");
		return;
	}

	BYTE pcEmpire = ch->GetEmpire();
	BYTE mapEmpire = SECTREE_MANAGER::instance().GetEmpireFromMapIndex(ch->GetMapIndex());

	if (LC_IsYMIR() == true || LC_IsKorea() == true)
	{
		if (mapEmpire != pcEmpire && mapEmpire != 0)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_STRING("자국 영토에서만 사용할 수 있는 기능입니다"));
			return;
		}
	}

	// 군주 몹 소환 비용 
	const int SummonPrice = 5000000;

	// 군주 쿨타임 검사
	if (!ch->IsMCOK(CHARACTER::MI_SUMMON))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_STRING("%d 초간 쿨타임이 적용중입니다.", ch->GetMCLTime(CHARACTER::MI_SUMMON)));
		return;
	}

	// 군주 국고 검사 
	if (!CMonarch::instance().IsMoneyOk(SummonPrice, ch->GetEmpire()))
	{
		int NationMoney = CMonarch::instance().GetMoney(ch->GetEmpire());
		ch->ChatPacket(CHAT_TYPE_INFO, LC_STRING("국고에 돈이 부족합니다. 현재 : %u 필요금액 : %u", NationMoney, SummonPrice));
		return;
	}

	const CMob* pkMob = nullptr;
	DWORD vnum = 0;

	if (isdigit(*arg1))
	{
		str_to_number(vnum, arg1);

		if ((pkMob = CMobManager::instance().Get(vnum)) == NULL)
			vnum = 0;
	}
	else
	{
		pkMob = CMobManager::Instance().Get(arg1, true);

		if (pkMob)
			vnum = pkMob->m_table.dwVnum;
	}

	if (pkMob == nullptr)
		return;

	DWORD count;

	// 소환 가능 몹 검사
	for (count = 0; cs_dwMonarchMobVnums[count] != 0; ++count)
		if (cs_dwMonarchMobVnums[count] == vnum)
			break;

	if (0 == cs_dwMonarchMobVnums[count])
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_STRING("소환할수 없는 몬스터 입니다. 소환가능한 몬스터는 홈페이지를 참조하세요"));
		return;
	}

	tch = CHARACTER_MANAGER::instance().SpawnMobRange(vnum,
		ch->GetMapIndex(),
		ch->GetX() - number(200, 750),
		ch->GetY() - number(200, 750),
		ch->GetX() + number(200, 750),
		ch->GetY() + number(200, 750),
		true,
		pkMob->m_table.bType == CHAR_TYPE_STONE,
		true);

	if (tch)
	{
		// 군주 돈 삭감
		CMonarch::instance().SendtoDBDecMoney(SummonPrice, ch->GetEmpire(), ch);

		// 쿨타임 초기화
		ch->SetMC(CHARACTER::MI_SUMMON);
	}
}

static const char* FN_point_string(DWORD dwApplyType)
{
	switch (dwApplyType)
	{
		case POINT_MAX_HP: return LC_STRING("최대 생명력 +%d");
		case POINT_MAX_SP: return LC_STRING("최대 정신력 +%d");
		case POINT_HT: return LC_STRING("체력 +%d");
		case POINT_IQ: return LC_STRING("지능 +%d");
		case POINT_ST: return LC_STRING("근력 +%d");
		case POINT_DX: return LC_STRING("민첩 +%d");
		case POINT_ATT_SPEED: return LC_STRING("공격속도 +%d");
		case POINT_MOV_SPEED: return LC_STRING("이동속도 %d");
		case POINT_CASTING_SPEED: return LC_STRING("쿨타임 -%d");
		case POINT_HP_REGEN: return LC_STRING("생명력 회복 +%d");
		case POINT_SP_REGEN: return LC_STRING("정신력 회복 +%d");
		case POINT_POISON_PCT: return LC_STRING("독공격 %d");
		case POINT_BLEEDING_PCT: return LC_STRING("독공격 %d");
		case POINT_STUN_PCT: return LC_STRING("스턴 +%d");
		case POINT_SLOW_PCT: return LC_STRING("슬로우 +%d");
		case POINT_CRITICAL_PCT: return LC_STRING("%d%% 확률로 치명타 공격");
		case POINT_RESIST_CRITICAL: return LC_STRING("상대의 치명타 확률 %d%% 감소");
		case POINT_PENETRATE_PCT: return LC_STRING("%d%% 확률로 관통 공격");
		case POINT_RESIST_PENETRATE: return LC_STRING("상대의 관통 공격 확률 %d%% 감소");
		case POINT_ATTBONUS_HUMAN: return LC_STRING("인간류 몬스터 타격치 +%d%%");
		case POINT_ATTBONUS_ANIMAL: return LC_STRING("동물류 몬스터 타격치 +%d%%");
		case POINT_ATTBONUS_ORC: return LC_STRING("웅귀족 타격치 +%d%%");
		case POINT_ATTBONUS_MILGYO: return LC_STRING("밀교류 타격치 +%d%%");
		case POINT_ATTBONUS_UNDEAD: return LC_STRING("시체류 타격치 +%d%%");
		case POINT_ATTBONUS_DEVIL: return LC_STRING("악마류 타격치 +%d%%");
		case POINT_STEAL_HP: return LC_STRING("타격치 %d%% 를 생명력으로 흡수");
		case POINT_STEAL_SP: return LC_STRING("타력치 %d%% 를 정신력으로 흡수");
		case POINT_MANA_BURN_PCT: return LC_STRING("%d%% 확률로 타격시 상대 전신력 소모");
		case POINT_DAMAGE_SP_RECOVER: return LC_STRING("%d%% 확률로 피해시 정신력 회복");
		case POINT_BLOCK: return LC_STRING("물리타격시 블럭 확률 %d%%");
		case POINT_DODGE: return LC_STRING("활 공격 회피 확률 %d%%");
		case POINT_RESIST_SWORD: return LC_STRING("한손검 방어 %d%%");
		case POINT_RESIST_TWOHAND: return LC_STRING("양손검 방어 %d%%");
		case POINT_RESIST_DAGGER: return LC_STRING("두손검 방어 %d%%");
		case POINT_RESIST_BELL: return LC_STRING("방울 방어 %d%%");
		case POINT_RESIST_FAN: return LC_STRING("부채 방어 %d%%");
		case POINT_RESIST_BOW: return LC_STRING("활공격 저항 %d%%");
		case POINT_RESIST_CLAW: return LC_STRING("두손검 방어 %d%%");
		case POINT_RESIST_FIRE: return LC_STRING("화염 저항 %d%%");
		case POINT_RESIST_ELEC: return LC_STRING("전기 저항 %d%%");
		case POINT_RESIST_MAGIC: return LC_STRING("마법 저항 %d%%");
#if defined(__MAGIC_REDUCTION__)
		case POINT_RESIST_MAGIC_REDUCTION:	return LC_STRING("마법 저항 %d%%");
#endif
		case POINT_RESIST_WIND: return LC_STRING("바람 저항 %d%%");
		case POINT_RESIST_ICE: return LC_STRING("냉기 저항 %d%%");
		case POINT_RESIST_EARTH: return LC_STRING("대지 저항 %d%%");
		case POINT_RESIST_DARK: return LC_STRING("어둠 저항 %d%%");
		case POINT_REFLECT_MELEE: return LC_STRING("직접 타격치 반사 확률 : %d%%");
		case POINT_REFLECT_CURSE: return LC_STRING("저주 되돌리기 확률 %d%%");
		case POINT_POISON_REDUCE: return LC_STRING("독 저항 %d%%");
		case POINT_BLEEDING_REDUCE:	return LC_STRING("독 저항 %d%%");
		case POINT_KILL_SP_RECOVER: return LC_STRING("%d%% 확률로 적퇴치시 정신력 회복");
		case POINT_EXP_DOUBLE_BONUS: return LC_STRING("%d%% 확률로 적퇴치시 경험치 추가 상승");
		case POINT_GOLD_DOUBLE_BONUS: return LC_STRING("%d%% 확률로 적퇴치시 돈 2배 드롭");
		case POINT_ITEM_DROP_BONUS: return LC_STRING("%d%% 확률로 적퇴치시 아이템 2배 드롭");
		case POINT_POTION_BONUS: return LC_STRING("물약 사용시 %d%% 성능 증가");
		case POINT_KILL_HP_RECOVERY: return LC_STRING("%d%% 확률로 적퇴치시 생명력 회복");
			//case POINT_IMMUNE_STUN: return LC_STRING("기절하지 않음 %d%%");
			//case POINT_IMMUNE_SLOW: return LC_STRING("느려지지 않음 %d%%");
			//case POINT_IMMUNE_FALL: return LC_STRING("넘어지지 않음 %d%%");
			//case POINT_SKILL: return LC_STRING("");
			//case POINT_BOW_DISTANCE: return LC_STRING("");
		case POINT_ATT_GRADE_BONUS: return LC_STRING("공격력 +%d");
		case POINT_DEF_GRADE_BONUS: return LC_STRING("방어력 +%d");
		case POINT_MAGIC_ATT_GRADE: return LC_STRING("마법 공격력 +%d");
		case POINT_MAGIC_DEF_GRADE: return LC_STRING("마법 방어력 +%d");
			//case POINT_CURSE_PCT: return LC_STRING("");
		case POINT_MAX_STAMINA: return LC_STRING("최대 지구력 +%d");
		case POINT_ATTBONUS_WARRIOR: return LC_STRING("무사에게 강함 +%d%%");
		case POINT_ATTBONUS_ASSASSIN: return LC_STRING("자객에게 강함 +%d%%");
		case POINT_ATTBONUS_SURA: return LC_STRING("수라에게 강함 +%d%%");
		case POINT_ATTBONUS_SHAMAN: return LC_STRING("무당에게 강함 +%d%%");
		case POINT_ATTBONUS_WOLFMAN: return LC_STRING("무당에게 강함 +%d%%");
		case POINT_ATTBONUS_MONSTER: return LC_STRING("몬스터에게 강함 +%d%%");
		case POINT_MALL_ATTBONUS: return LC_STRING("공격력 +%d%%");
		case POINT_MALL_DEFBONUS: return LC_STRING("방어력 +%d%%");
		case POINT_MALL_EXPBONUS: return LC_STRING("경험치 %d%%");
		case POINT_MALL_ITEMBONUS: return LC_STRING("아이템 드롭율 %.1f배");
		case POINT_MALL_GOLDBONUS: return LC_STRING("돈 드롭율 %.1f배");
		case POINT_MAX_HP_PCT: return LC_STRING("최대 생명력 +%d%%");
		case POINT_MAX_SP_PCT: return LC_STRING("최대 정신력 +%d%%");
		case POINT_SKILL_DAMAGE_BONUS: return LC_STRING("스킬 데미지 %d%%");
		case POINT_NORMAL_HIT_DAMAGE_BONUS: return LC_STRING("평타 데미지 %d%%");
		case POINT_SKILL_DEFEND_BONUS: return LC_STRING("스킬 데미지 저항 %d%%");
		case POINT_NORMAL_HIT_DEFEND_BONUS: return LC_STRING("평타 데미지 저항 %d%%");
			//case POINT_PC_BANG_EXP_BONUS: return LC_STRING("");
			//case POINT_PC_BANG_DROP_BONUS: return LC_STRING("");
			//case POINT_EXTRACT_HP_PCT: return LC_STRING("");
		case POINT_RESIST_WARRIOR: return LC_STRING("무사공격에 %d%% 저항");
		case POINT_RESIST_ASSASSIN: return LC_STRING("자객공격에 %d%% 저항");
		case POINT_RESIST_SURA: return LC_STRING("수라공격에 %d%% 저항");
		case POINT_RESIST_SHAMAN: return LC_STRING("무당공격에 %d%% 저항");
		case POINT_RESIST_WOLFMAN: return LC_STRING("무당공격에 %d%% 저항");
#if defined(__ELEMENT_SYSTEM__)
		case POINT_ENCHANT_ELECT: return LC_STRING("Lightning Power + %d%%");
		case POINT_ENCHANT_FIRE: return LC_STRING("Fire Power + %d%%");
		case POINT_ENCHANT_ICE: return LC_STRING("Ice Power + %d%%");
		case POINT_ENCHANT_WIND: return LC_STRING("Wind Power + %d%%");
		case POINT_ENCHANT_EARTH: return LC_STRING("Earth Power + %d%%");
		case POINT_ENCHANT_DARK: return LC_STRING("Dark Power + %d%%");
		case POINT_ATTBONUS_CZ: return LC_STRING("Strong against Zodiac Monsters + %d%%");
		case POINT_ATTBONUS_INSECT: return LC_STRING("Strong against Insects + %d%%");
		case POINT_ATTBONUS_DESERT: return LC_STRING("Strong against Desert Monsters + %d%%");
#endif
		case POINT_ATTBONUS_STONE: return LC_STRING("Strong against Metin Stones +%d%%");
		default:
			return LC_STRING("Unkown apply_number %d", dwApplyType);
	}
}

static bool FN_hair_affect_string(LPCHARACTER ch, char* buf, size_t bufsiz)
{
	if (NULL == ch || NULL == buf)
		return false;

	CAffect* aff = NULL;
	time_t expire = 0;
	struct tm ltm;
	int year, mon, day;
	int offset = 0;

	aff = ch->FindAffect(AFFECT_HAIR);

	if (NULL == aff)
		return false;

	expire = ch->GetQuestFlag("hair.limit_time");

	if (expire < get_global_time())
		return false;

	// set apply string
	offset = snprintf(buf, bufsiz, FN_point_string(aff->wApplyOn), aff->lApplyValue);

	if (offset < 0 || offset >= (int)bufsiz)
		offset = bufsiz - 1;

	localtime_r(&expire, &ltm);

	year = ltm.tm_year + 1900;
	mon = ltm.tm_mon + 1;
	day = ltm.tm_mday;

	snprintf(buf + offset, bufsiz - offset, LC_STRING(" (만료일 : %d년 %d월 %d일)", year, mon, day));

	return true;
}

ACMD(do_costume)
{
	char buf[1024];
	const size_t bufferSize = sizeof(buf);

	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	ch->ChatPacket(CHAT_TYPE_INFO, "COSTUME status:");

	CItem* pBody = ch->GetWear(WEAR_COSTUME_BODY);
	if (pBody)
	{
		const char* itemName = pBody->GetName();

		ch->ChatPacket(CHAT_TYPE_INFO, "   BODY : %s", itemName);

		if (pBody->IsEquipped() && arg1[0] == 'b')
			ch->UnequipItem(pBody);
	}

	CItem* pHair = ch->GetWear(WEAR_COSTUME_HAIR);
	if (pHair)
	{
		const char* itemName = pHair->GetName();

		ch->ChatPacket(CHAT_TYPE_INFO, "   HAIR : %s", itemName);

		for (int i = 0; i < pHair->GetAttributeCount(); ++i)
		{
			const TPlayerItemAttribute& attr = pHair->GetAttribute(i);
			if (0 < attr.wType)
			{
				snprintf(buf, bufferSize, FN_point_string(attr.wType), attr.lValue);
				ch->ChatPacket(CHAT_TYPE_INFO, "     %s", buf);
			}
		}

		if (pHair->IsEquipped() && arg1[0] == 'h')
			ch->UnequipItem(pHair);
	}

#if defined(__MOUNT_COSTUME_SYSTEM__)
	CItem* pMount = ch->GetWear(WEAR_COSTUME_MOUNT);
	if (pMount)
	{
		const char* itemName = pMount->GetName();

		ch->ChatPacket(CHAT_TYPE_INFO, "   MOUNT : %s", itemName);

		if (pMount->IsEquipped() && arg1[0] == 'm')
			ch->UnequipItem(pMount);
	}
#endif

#if defined(__ACCE_COSTUME_SYSTEM__)
	CItem* pAcce = ch->GetWear(WEAR_COSTUME_ACCE);
	if (pAcce)
	{
		const char* itemName = pAcce->GetName();

		ch->ChatPacket(CHAT_TYPE_INFO, "   ACCE : %s", itemName);

		if (pAcce->IsEquipped() && arg1[0] == 'a')
			ch->UnequipItem(pAcce);
	}
#endif

#if defined(__WEAPON_COSTUME_SYSTEM__)
	CItem* pWeapon = ch->GetWear(WEAR_COSTUME_WEAPON);
	if (pWeapon)
	{
		const char* itemName = pWeapon->GetName();
		ch->ChatPacket(CHAT_TYPE_INFO, "   WEAPON : %s", itemName);
		if (pWeapon->IsEquipped() && arg1[0] == 'w')
			ch->UnequipItem(pWeapon);
	}
#endif

#if defined(__AURA_COSTUME_SYSTEM__)
	CItem* pAura = ch->GetWear(WEAR_COSTUME_AURA);
	if (pAura)
	{
		const char* itemName = pAura->GetName();
		ch->ChatPacket(CHAT_TYPE_INFO, "  AURA : %s", itemName);
		if (pAura->IsEquipped() && arg1[0] == 'a')
			ch->UnequipItem(pAura);
	}
#endif
}

ACMD(do_hair)
{
	char buf[256];

	if (false == FN_hair_affect_string(ch, buf, sizeof(buf)))
		return;

	ch->ChatPacket(CHAT_TYPE_INFO, buf);
}

ACMD(do_inventory)
{
	int index = 0;
	int count = 1;

	char arg1[256];
	char arg2[256];

	LPITEM item;

	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));

	if (!*arg1)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Usage: inventory <start_index> <count>");
		return;
	}

	if (!*arg2)
	{
		index = 0;
		str_to_number(count, arg1);
	}
	else
	{
		str_to_number(index, arg1); index = MIN(index, INVENTORY_MAX_NUM);
		str_to_number(count, arg2); count = MIN(count, INVENTORY_MAX_NUM);
	}

	for (int i = 0; i < count; ++i)
	{
#if defined(__EXTEND_INVEN_SYSTEM__)
		if (index >= ch->GetExtendInvenMax())
#else
		if (index >= INVENTORY_MAX_NUM)
#endif
			break;

		item = ch->GetInventoryItem(index);

		ch->ChatPacket(CHAT_TYPE_INFO, "inventory [%d] = %s", index, item ? item->GetName() : "<NONE>");
		++index;
	}
}

// gift notify quest command
ACMD(do_gift)
{
	ch->ChatPacket(CHAT_TYPE_COMMAND, "gift");
}

#if !defined(__CUBE_RENEWAL__)
ACMD(do_cube)
{
	if (!ch->CanDoCube())
		return;

	dev_log(LOG_DEB0, "CUBE COMMAND <%s>: %s", ch->GetName(), argument);
	int cube_index = 0, inven_index = 0;
	const char* line;

	char arg1[256], arg2[256], arg3[256];

	line = two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));
	one_argument(line, arg3, sizeof(arg3));

	if (0 == arg1[0])
	{
		// print usage
		ch->ChatPacket(CHAT_TYPE_INFO, "Usage: cube open");
		ch->ChatPacket(CHAT_TYPE_INFO, "       cube close");
		ch->ChatPacket(CHAT_TYPE_INFO, "       cube add <inveltory_index>");
		ch->ChatPacket(CHAT_TYPE_INFO, "       cube delete <cube_index>");
		ch->ChatPacket(CHAT_TYPE_INFO, "       cube list");
		ch->ChatPacket(CHAT_TYPE_INFO, "       cube cancel");
		ch->ChatPacket(CHAT_TYPE_INFO, "       cube make [all]");
		return;
	}

	const std::string& strArg1 = std::string(arg1);

	// r_info (request information)
	// /cube r_info ==> (Client -> Server) 현재 NPC가 만들 수 있는 레시피 요청
	//					(Server -> Client) /cube r_list npcVNUM resultCOUNT 123,1/125,1/128,1/130,5
	//
	// /cube r_info 3 ==> (Client -> Server) 현재 NPC가 만들수 있는 레시피 중 3번째 아이템을 만드는 데 필요한 정보를 요청
	// /cube r_info 3 5 ==> (Client -> Server) 현재 NPC가 만들수 있는 레시피 중 3번째 아이템부터 이후 5개의 아이템을 만드는 데 필요한 재료 정보를 요청
	//					(Server -> Client) /cube m_info startIndex count 125,1|126,2|127,2|123,5&555,5&555,4/120000@125,1|126,2|127,2|123,5&555,5&555,4/120000
	//
	if (strArg1 == "r_info")
	{
		if (0 == arg2[0])
			Cube_request_result_list(ch);
		else
		{
			if (isdigit(*arg2))
			{
				int listIndex = 0, requestCount = 1;
				str_to_number(listIndex, arg2);

				if (0 != arg3[0] && isdigit(*arg3))
					str_to_number(requestCount, arg3);

				Cube_request_material_info(ch, listIndex, requestCount);
			}
		}

		return;
	}

	switch (LOWER(arg1[0]))
	{
		case 'o': // open
			Cube_open(ch);
			break;

		case 'c': // close
			Cube_close(ch);
			break;

		case 'l': // list
			Cube_show_list(ch);
			break;

		case 'a': // add cue_index inven_index
		{
			if (0 == arg2[0] || !isdigit(*arg2) ||
				0 == arg3[0] || !isdigit(*arg3))
				return;

			str_to_number(cube_index, arg2);
			str_to_number(inven_index, arg3);
			Cube_add_item(ch, cube_index, inven_index);
		}
		break;

		case 'd': // delete
		{
			if (0 == arg2[0] || !isdigit(*arg2))
				return;

			str_to_number(cube_index, arg2);
			Cube_delete_item(ch, cube_index);
		}
		break;

		case 'm': // make
			if (0 != arg2[0])
			{
				while (true == Cube_make(ch))
					dev_log(LOG_DEB0, "cube make success");
			}
			else
				Cube_make(ch);
			break;

		default:
			return;
	}
}
#endif

ACMD(do_in_game_mall)
{
	if (LC_IsYMIR() == true || LC_IsKorea() == true)
	{
		ch->ChatPacket(CHAT_TYPE_COMMAND, "mall http://metin2.co.kr/04_mall/mall/login.htm");
		return;
	}

	if (true == LC_IsTaiwan())
	{
		ch->ChatPacket(CHAT_TYPE_COMMAND, "mall http://203.69.141.203/mall/mall/item_main.htm");
		return;
	}

	if (LC_IsJapan() == true)
	{
		ch->ChatPacket(CHAT_TYPE_COMMAND, "mall http://mt2.oge.jp/itemmall/itemList.php");
		return;
	}

	if (LC_IsNewCIBN() == true && test_server)
	{
		ch->ChatPacket(CHAT_TYPE_COMMAND, "mall http://218.99.6.51/04_mall/mall/login.htm");
		return;
	}

	if (LC_IsSingapore() == true)
	{
		ch->ChatPacket(CHAT_TYPE_COMMAND, "mall http://www.metin2.sg/ishop.php");
		return;
	}

	/*
	if (LC_IsCanada() == true)
	{
		ch->ChatPacket(CHAT_TYPE_COMMAND, "mall http://mall.z8games.com/mall_entry.aspx?tb=m2");
		return;
	}
	*/

	if (LC_IsEurope() == true)
	{
		char country_code[3];

		switch (LC_GetLocalType())
		{
			case LC_GERMANY: country_code[0] = 'd'; country_code[1] = 'e'; country_code[2] = '\0'; break;
			case LC_FRANCE: country_code[0] = 'f'; country_code[1] = 'r'; country_code[2] = '\0'; break;
			case LC_ITALY: country_code[0] = 'i'; country_code[1] = 't'; country_code[2] = '\0'; break;
			case LC_SPAIN: country_code[0] = 'e'; country_code[1] = 's'; country_code[2] = '\0'; break;
			case LC_UK: country_code[0] = 'e'; country_code[1] = 'n'; country_code[2] = '\0'; break;
			case LC_TURKEY: country_code[0] = 't'; country_code[1] = 'r'; country_code[2] = '\0'; break;
			case LC_POLAND: country_code[0] = 'p'; country_code[1] = 'l'; country_code[2] = '\0'; break;
			case LC_PORTUGAL: country_code[0] = 'p'; country_code[1] = 't'; country_code[2] = '\0'; break;
			case LC_GREEK: country_code[0] = 'g'; country_code[1] = 'r'; country_code[2] = '\0'; break;
			case LC_RUSSIA: country_code[0] = 'r'; country_code[1] = 'u'; country_code[2] = '\0'; break;
			case LC_DENMARK: country_code[0] = 'd'; country_code[1] = 'k'; country_code[2] = '\0'; break;
			case LC_BULGARIA: country_code[0] = 'b'; country_code[1] = 'g'; country_code[2] = '\0'; break;
			case LC_CROATIA: country_code[0] = 'h'; country_code[1] = 'r'; country_code[2] = '\0'; break;
			case LC_MEXICO: country_code[0] = 'm'; country_code[1] = 'x'; country_code[2] = '\0'; break;
			case LC_ARABIA: country_code[0] = 'a'; country_code[1] = 'e'; country_code[2] = '\0'; break;
			case LC_CZECH: country_code[0] = 'c'; country_code[1] = 'z'; country_code[2] = '\0'; break;
			case LC_ROMANIA: country_code[0] = 'r'; country_code[1] = 'o'; country_code[2] = '\0'; break;
			case LC_HUNGARY: country_code[0] = 'h'; country_code[1] = 'u'; country_code[2] = '\0'; break;
			case LC_NETHERLANDS: country_code[0] = 'n'; country_code[1] = 'l'; country_code[2] = '\0'; break;
			case LC_USA: country_code[0] = 'u'; country_code[1] = 's'; country_code[2] = '\0'; break;
			case LC_CANADA: country_code[0] = 'c'; country_code[1] = 'a'; country_code[2] = '\0'; break;
			default:
			{
				if (test_server)
				{
					country_code[0] = 'u'; country_code[1] = 's'; country_code[2] = '\0';
				}
				break;
			}
		}

#if defined(__LOCALE_CLIENT__)
		snprintf(country_code, sizeof(country_code),
			"%s", LocaleService_GetCountry(ch->GetCountry()));
#endif

		char buf[512 + 1];
		char sas[33];
		MD5_CTX ctx;
		const char sas_key[] = "GF9001";

		snprintf(buf, sizeof(buf), "%u%u%s", ch->GetPlayerID(), ch->GetAID(), sas_key);

		MD5Init(&ctx);
		MD5Update(&ctx, (const unsigned char*)buf, strlen(buf));
#ifdef __FreeBSD__
		MD5End(&ctx, sas);
#else
		static const char hex[] = "0123456789abcdef";
		unsigned char digest[16];
		MD5Final(digest, &ctx);
		int i;
		for (i = 0; i < 16; ++i)
		{
			sas[i + i] = hex[digest[i] >> 4];
			sas[i + i + 1] = hex[digest[i] & 0x0f];
		}
		sas[i + i] = '\0';
#endif

		snprintf(buf, sizeof(buf), "mall http://%s/ishop?pid=%u&c=%s&sid=%d&sas=%s",
			g_strWebMallURL.c_str(), ch->GetPlayerID(), country_code, g_server_id, sas);

		ch->ChatPacket(CHAT_TYPE_COMMAND, buf);
	}
}

// 주사위
ACMD(do_dice)
{
	char arg1[256], arg2[256];
	int start = 1, end = 100;

	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));

	if (*arg1 && *arg2)
	{
		start = atoi(arg1);
		end = atoi(arg2);
	}
	else if (*arg1 && !*arg2)
	{
		start = 1;
		end = atoi(arg1);
	}

	end = MAX(start, end);
	start = MIN(start, end);

	int n = number(start, end);

#if defined(__DICE_SYSTEM__)
	if (ch->GetParty())
		ch->GetParty()->ChatPacketToAllMember(CHAT_TYPE_INFO, LC_STRING("%s님이 주사위를 굴려 %d가 나왔습니다. (%d-%d)", ch->GetName(), n, start, end));
	else
		ch->ChatPacket(CHAT_TYPE_INFO, LC_STRING("당신이 주사위를 굴려 %d가 나왔습니다. (%d-%d)", n, start, end));
#else
	if (ch->GetParty())
		ch->GetParty()->ChatPacketToAllMember(CHAT_TYPE_INFO, LC_STRING("%s님이 주사위를 굴려 %d가 나왔습니다. (%d-%d)", ch->GetName(), n, start, end));
	else
		ch->ChatPacket(CHAT_TYPE_INFO, LC_STRING("당신이 주사위를 굴려 %d가 나왔습니다. (%d-%d)", n, start, end));
#endif
}

ACMD(do_click_mall)
{
	ch->ChatPacket(CHAT_TYPE_COMMAND, "ShowMeMallPassword");
}

ACMD(do_ride)
{
	dev_log(LOG_DEB0, "[DO_RIDE] start");
	if (ch->IsDead() || ch->IsStun())
		return;

	if (ch->IsFishing())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_STRING("You cannot carry out this action while fishing."));
		return;
	}

#if defined(__MOUNT_COSTUME_SYSTEM__)
	if (SECTREE_MANAGER::Instance().IsBlockFilterMapIndex(SECTREE_MANAGER::MOUNT_BLOCK_MAP_INDEX, ch->GetMapIndex()))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_STRING("You cannot summon your mount/pet right now."));
		return;
	}
#endif

	if (ch->IsWearingDress())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_STRING("You cannot ride while you are wearing a Wedding Dress or a Tuxedo."));
		return;
	}

	// 내리기
	{
		if (ch->IsHorseRiding())
		{
			dev_log(LOG_DEB0, "[DO_RIDE] stop riding");
			ch->StopRiding();
			return;
		}

		if (ch->GetMountVnum())
		{
			dev_log(LOG_DEB0, "[DO_RIDE] unmount");
			do_unmount(ch, NULL, 0, 0);
			return;
		}
	}

	// 타기
	{
		if (ch->GetHorse() != NULL)
		{
			dev_log(LOG_DEB0, "[DO_RIDE] start riding");
			ch->StartRiding();
			return;
		}

		for (BYTE i = 0; i < INVENTORY_MAX_NUM; ++i)
		{
			LPITEM item = ch->GetInventoryItem(i);
			if (NULL == item)
				continue;

			// 유니크 탈것 아이템
			if (item->IsRideItem())
			{
				if (NULL == ch->GetWear(WEAR_UNIQUE1)
					|| NULL == ch->GetWear(WEAR_UNIQUE2)
#if defined(__MOUNT_COSTUME_SYSTEM__)
					|| NULL == ch->GetWear(WEAR_COSTUME_MOUNT)
#endif
					)
				{
					dev_log(LOG_DEB0, "[DO_RIDE] USE UNIQUE ITEM");
					//ch->EquipItem(item);
					ch->UseItem(TItemPos(INVENTORY, i));
					return;
				}
			}

			// 일반 탈것 아이템
			// TODO : 탈것용 SubType 추가
			switch (item->GetVnum())
			{
				case 71114: // 저신이용권
				case 71116: // 산견신이용권
				case 71118: // 투지범이용권
				case 71120: // 사자왕이용권
					dev_log(LOG_DEB0, "[DO_RIDE] USE QUEST ITEM");
					ch->UseItem(TItemPos(INVENTORY, i));
					return;
			}

			// GF mantis #113524, 52001~52090 번 탈것
			if ((item->GetVnum() > 52000) && (item->GetVnum() < 52091))
			{
				dev_log(LOG_DEB0, "[DO_RIDE] USE QUEST ITEM");
				ch->UseItem(TItemPos(INVENTORY, i));
				return;
			}
		}
	}

	// 타거나 내릴 수 없을때
	ch->ChatPacket(CHAT_TYPE_INFO, LC_STRING("말을 먼저 소환해주세요."));
}

#if defined(__MOVE_CHANNEL__)
ACMD(do_move_channel)
{
	if (!ch)
		return;

	if (ch->m_pkTimedEvent)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_STRING("취소 되었습니다."));
		event_cancel(&ch->m_pkTimedEvent);
		return;
	}

	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_STRING("Usage: channel <new channel>"));
		return;
	}

	short channel;
	str_to_number(channel, arg1);

	if (channel < 1 || channel > 4)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_STRING("Please enter a valid channel."));
		return;
	}

	if (channel == g_bChannel)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_STRING("You are already on channel %d.", g_bChannel));
		return;
	}

	if (g_bChannel == 99)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_STRING("You cannot change your channel."));
		return;
	}

	if (ch->GetDungeon() || ch->GetMapIndex() >= 10000)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_STRING("You cannot change your channel."));
		return;
	}

	TPacketChangeChannel p;
	p.iChannel = channel;
	p.lMapIndex = ch->GetMapIndex();

	db_clientdesc->DBPacket(HEADER_GD_FIND_CHANNEL, ch->GetDesc()->GetHandle(), &p, sizeof(p));
}
#endif

#if defined(__POPUP_NOTICE__)
ACMD(do_popup_notice_check)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
		return;

	int enable;
	str_to_number(enable, arg1);

	ch->SetQuestFlag("popup_notice.checkbox", enable);
}
#endif

#if defined(__GAME_OPTION_ESCAPE__)
ACMD(do_escape)
{
	if (ch->IsDead() || ch->IsPolymorphed())
		return;

	if (ch->GetEscapeCooltime() > thecore_pulse() && !ch->IsGM())
		return;

	const BYTE bEmpire = ch->GetEmpire();
	const long lMapIndex = ch->GetMapIndex();
	const PIXEL_POSITION& rkPos = ch->GetXYZ();

	const LPSECTREE_MAP pSectreeMap = SECTREE_MANAGER::instance().GetMap(lMapIndex);
	if (pSectreeMap == nullptr)
	{
		ch->WarpSet(g_start_position[bEmpire][0], g_start_position[bEmpire][1]);
		return;
	}

	const LPSECTREE pSectree = pSectreeMap->Find(rkPos.x, rkPos.y);
	const DWORD dwAttr = pSectree->GetAttribute(rkPos.x, rkPos.y);

	int iEscapeDistance = quest::CQuestManager::instance().GetEventFlag("escape_distance");
	iEscapeDistance = (iEscapeDistance > 0) ? iEscapeDistance : 300;

	int iEscapeCooltime = quest::CQuestManager::instance().GetEventFlag("escape_cooltime");
	iEscapeCooltime = (iEscapeCooltime > 0) ? iEscapeCooltime : 10;

	if (IS_SET(dwAttr, ATTR_BLOCK /*| ATTR_OBJECT*/))
	{
		/*
		* NOTE : If an object doesn't have a blocked area, players can still be blocked if they get inside it.
		* The problem is that bridges are treated as objects too, and we don't want players to use the escape feature through them.
		* The tricky part is figuring out whether a specific object is a bridge or not.
		* In the current state, we are only checking blocked areas.
		*
		* 20240117.Owsap
		*/

		PIXEL_POSITION kNewPos;
		if (SECTREE_MANAGER::instance().GetRandomLocation(lMapIndex, kNewPos, rkPos.x, rkPos.y, iEscapeDistance))
		{
			char szBuf[255 + 1];
			snprintf(szBuf, sizeof(szBuf), "%ld, (%d, %d) -> (%d, %d)",
				lMapIndex, rkPos.x, rkPos.y, kNewPos.x, kNewPos.y);
			LogManager::instance().CharLog(ch, lMapIndex, "ESCAPE", szBuf);

			ch->ChatPacket(CHAT_TYPE_INFO, LC_STRING("You have successfully freed yourself."));
			ch->Show(lMapIndex, kNewPos.x, kNewPos.y, rkPos.z);
		}
		else
		{
			char szBuf[255 + 1];
			snprintf(szBuf, sizeof(szBuf), "%ld, (%d, %d) -> EMPIRE START POSITION",
				lMapIndex, rkPos.x, rkPos.y);
			LogManager::instance().CharLog(ch, lMapIndex, "ESCAPE", szBuf);

			ch->WarpSet(g_start_position[bEmpire][0], g_start_position[bEmpire][1]);
		}
	}

	ch->SetEscapeCooltime(thecore_pulse() + PASSES_PER_SEC(10));
}
#endif

#if defined(__HIDE_COSTUME_SYSTEM__)
ACMD(do_hide_costume_part)
{
	char szArg1[256], szArg2[256];
	two_arguments(argument, szArg1, sizeof(szArg1), szArg2, sizeof(szArg2));

	if (!*szArg1 || !*szArg2)
		return;

	BYTE bCostumeSubType = 0, bHidden = 0;
	str_to_number(bCostumeSubType, szArg1);
	str_to_number(bHidden, szArg2);

	ch->SetHiddenCostumePart(bCostumeSubType, bHidden);
}
#endif
