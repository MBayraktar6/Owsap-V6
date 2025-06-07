#include "stdafx.h"

#include "config.h"
#include "char.h"
#include "char_manager.h"
#include "affect.h"
#include "packet.h"
#include "buffer_manager.h"
#include "desc_client.h"
#include "battle.h"
#include "guild.h"
#include "utils.h"
#include "locale_service.h"
#include "lua_incl.h"
#include "arena.h"
#include "horsename_manager.h"
#include "item.h"
#include "DragonSoul.h"
#if defined(__SNOWFLAKE_STICK_EVENT__)
#	include "xmas_event.h"
#endif

#define IS_NO_SAVE_AFFECT(type) ((type) == AFFECT_WAR_FLAG || (type) == AFFECT_REVIVE_INVISIBLE || ((type) >= AFFECT_PREMIUM_START && (type) <= AFFECT_PREMIUM_END) || (type) == AFFECT_MOUNT_BONUS)
#define IS_NO_CLEAR_ON_DEATH_AFFECT(type) ((type) == AFFECT_BLOCK_CHAT || ((type) >= 500 && (type) < 600))
#if defined(__SET_ITEM__)
#	define IS_NO_CLEAR_SET_ITEM(type) ((type) == AFFECT_SET_ITEM || ((type) >= AFFECT_SET_ITEM_SET_VALUE_1 && (type) <= AFFECT_SET_ITEM_SET_VALUE_5))
#endif

void SendAffectRemovePacket(LPDESC pDesc, DWORD dwPID, DWORD dwType, POINT_TYPE wApplyOn)
{
	TPacketGCAffectRemove ptoc;
	ptoc.bHeader = HEADER_GC_AFFECT_REMOVE;
	ptoc.dwType = dwType;
	ptoc.wApplyOn = wApplyOn;
	pDesc->Packet(&ptoc, sizeof(TPacketGCAffectRemove));

	TPacketGDRemoveAffect ptod;
	ptod.dwPID = dwPID;
	ptod.dwType = dwType;
	ptod.wApplyOn = wApplyOn;
	db_clientdesc->DBPacket(HEADER_GD_REMOVE_AFFECT, 0, &ptod, sizeof(ptod));
}

void SendAffectAddPacket(LPDESC pDesc, CAffect* pkAff)
{
	TPacketGCAffectAdd ptoc;
	ptoc.bHeader = HEADER_GC_AFFECT_ADD;
	ptoc.elem.dwType = pkAff->dwType;
	ptoc.elem.wApplyOn = pkAff->wApplyOn;
	ptoc.elem.lApplyValue = pkAff->lApplyValue;
	ptoc.elem.dwFlag = pkAff->dwFlag;
	ptoc.elem.lDuration = pkAff->lDuration;
	ptoc.elem.lSPCost = pkAff->lSPCost;
#if defined(__AFFECT_RENEWAL__)
	ptoc.elem.bRealTime = pkAff->bRealTime;
	ptoc.elem.bUpdate = pkAff->bUpdate;
#endif
	pDesc->Packet(&ptoc, sizeof(TPacketGCAffectAdd));
}

////////////////////////////////////////////////////////////////////
// Affect
CAffect* CHARACTER::FindAffect(DWORD dwType, POINT_TYPE wApplyType) const
{
	for (const auto& it : m_list_pkAffect)
	{
		CAffect* pkAffect = it;
		if (pkAffect && (pkAffect->dwType == dwType && (wApplyType == APPLY_NONE || wApplyType == pkAffect->wApplyOn)))
			return pkAffect;
	}

	return NULL;
}

EVENTFUNC(affect_event)
{
	char_event_info* info = dynamic_cast<char_event_info*>(event->info);

	if (info == NULL)
	{
		sys_err("affect_event> <Factor> Null pointer");
		return 0;
	}

	LPCHARACTER ch = info->ch;

	if (ch == NULL) // <Factor>
		return 0;

	if (!ch->UpdateAffect())
		return 0;
	else
		return passes_per_sec; // 1초
}

bool CHARACTER::UpdateAffect()
{
	// affect_event 에서 처리할 일은 아니지만, 1초짜리 이벤트에서 처리하는 것이
	// 이것 뿐이라 여기서 물약 처리를 한다.
	if (GetPoint(POINT_HP_RECOVERY) > 0)
	{
		if (GetMaxHP() <= GetHP())
		{
			PointChange(POINT_HP_RECOVERY, -GetPoint(POINT_HP_RECOVERY));
		}
		else
		{
			int iVal = MIN(GetPoint(POINT_HP_RECOVERY), GetMaxHP() * 9 / 100);

			PointChange(POINT_HP, iVal);
			PointChange(POINT_HP_RECOVERY, -iVal);
		}
	}

	if (GetPoint(POINT_SP_RECOVERY) > 0)
	{
		if (GetMaxSP() <= GetSP())
		{
			PointChange(POINT_SP_RECOVERY, -GetPoint(POINT_SP_RECOVERY));
		}
		else
		{
			int iVal = MIN(GetPoint(POINT_SP_RECOVERY), GetMaxSP() * 7 / 100);

			PointChange(POINT_SP, iVal);
			PointChange(POINT_SP_RECOVERY, -iVal);
		}
	}

	if (GetPoint(POINT_HP_RECOVER_CONTINUE) > 0)
	{
		PointChange(POINT_HP, GetPoint(POINT_HP_RECOVER_CONTINUE));
	}

	if (GetPoint(POINT_SP_RECOVER_CONTINUE) > 0)
	{
		PointChange(POINT_SP, GetPoint(POINT_SP_RECOVER_CONTINUE));
	}

	AutoRecoveryItemProcess(AFFECT_AUTO_HP_RECOVERY);
	AutoRecoveryItemProcess(AFFECT_AUTO_SP_RECOVERY);

	// 스테미나 회복
	if (GetMaxStamina() > GetStamina())
	{
		int iSec = (get_dword_time() - GetStopTime()) / 3000;
		if (iSec)
			PointChange(POINT_STAMINA, GetMaxStamina() / 1);
	}

	// ProcessAffect는 affect가 없으면 true를 리턴한다.
	if (ProcessAffect())
	{
		if (GetPoint(POINT_HP_RECOVERY) == 0 && GetPoint(POINT_SP_RECOVERY) == 0 && GetStamina() == GetMaxStamina())
		{
			m_pkAffectEvent = NULL;
			return false;
		}
	}

	return true;
}

void CHARACTER::StartAffectEvent()
{
	if (m_pkAffectEvent)
		return;

	char_event_info* info = AllocEventInfo<char_event_info>();
	info->ch = this;
	m_pkAffectEvent = event_create(affect_event, info, passes_per_sec);
	sys_log(1, "StartAffectEvent %s %p %p", GetName(), this, get_pointer(m_pkAffectEvent));
}

void CHARACTER::ClearAffect(bool bSave)
{
	TAffectFlag afOld = m_afAffectFlag;
	long lMovSpd = GetPoint(POINT_MOV_SPEED);
	long lAttSpd = GetPoint(POINT_ATT_SPEED);

	AffectContainerList::iterator it = m_list_pkAffect.begin();
	while (it != m_list_pkAffect.end())
	{
		CAffect* pkAff = *it;
		if (pkAff == nullptr)
			continue;

		if (bSave)
		{
			if (IS_NO_CLEAR_ON_DEATH_AFFECT(pkAff->dwType) || IS_NO_SAVE_AFFECT(pkAff->dwType))
			{
				++it;
				continue;
			}

#if defined(__SOUL_SYSTEM__)
			if (pkAff->dwType == AFFECT_SOUL)
			{
				++it;
				continue;
			}
#endif

#if defined(__SET_ITEM__)
			if (IS_NO_CLEAR_SET_ITEM(pkAff->dwType))
			{
				++it;
				continue;
			}
#endif

#if defined(__SNOWFLAKE_STICK_EVENT__)
			if (pkAff->dwType == AFFECT_SNOWFLAKE_STICK_EVENT_RANK_BUFF
				|| pkAff->dwType == AFFECT_SNOWFLAKE_STICK_EVENT_SNOWFLAKE_BUFF)
			{
				++it;
				continue;
			}
#endif

			if (IsPC())
			{
				SendAffectRemovePacket(GetDesc(), GetPlayerID(), pkAff->dwType, pkAff->wApplyOn);
			}
		}

		ComputeAffect(pkAff, false);

		it = m_list_pkAffect.erase(it);
		CAffect::Release(pkAff);
	}

	if (afOld != m_afAffectFlag || lMovSpd != GetPoint(POINT_MOV_SPEED) || lAttSpd != GetPoint(POINT_ATT_SPEED))
		UpdatePacket();

	CheckMaximumPoints();

	if (m_list_pkAffect.empty())
		event_cancel(&m_pkAffectEvent);
}

int CHARACTER::ProcessAffect()
{
	bool bDiff = false;
	CAffect* pkAff = NULL;

	//
	// 프리미엄 처리
	//
	for (BYTE i = 0; i <= PREMIUM_MAX_NUM; ++i)
	{
		int aff_idx = i + AFFECT_PREMIUM_START;

		pkAff = FindAffect(aff_idx);
		if (!pkAff)
			continue;

		int remain = GetPremiumRemainSeconds(i);
		if (remain < 0)
		{
			RemoveAffect(aff_idx);
			bDiff = true;
		}
		else
			pkAff->lDuration = remain + 1;
	}

	////////// HAIR_AFFECT
	pkAff = FindAffect(AFFECT_HAIR);
	if (pkAff)
	{
		// IF HAIR_LIMIT_TIME() < CURRENT_TIME()
		if (this->GetQuestFlag("hair.limit_time") < get_global_time())
		{
			// SET HAIR NORMAL
			this->SetPart(PART_HAIR, 0);
			// REMOVE HAIR AFFECT
			RemoveAffect(AFFECT_HAIR);
		}
		else
		{
			// INCREASE AFFECT DURATION
			++(pkAff->lDuration);
		}
	}
	////////// HAIR_AFFECT

	CHorseNameManager::instance().Validate(this);

	TAffectFlag afOld = m_afAffectFlag;
	long lMovSpd = GetPoint(POINT_MOV_SPEED);
	long lAttSpd = GetPoint(POINT_ATT_SPEED);

	AffectContainerList::iterator it = m_list_pkAffect.begin();
	while (it != m_list_pkAffect.end())
	{
		pkAff = *it;
		if (pkAff == nullptr)
			continue;

		bool bEnd = false;

		if (pkAff->dwType >= GUILD_SKILL_START && pkAff->dwType <= GUILD_SKILL_END)
		{
			if (!GetGuild() || !GetGuild()->UnderAnyWar())
				bEnd = true;
		}

		if (pkAff->lSPCost > 0)
		{
			if (GetSP() < pkAff->lSPCost)
				bEnd = true;
			else
				PointChange(POINT_SP, -pkAff->lSPCost);
		}

		// AFFECT_DURATION_BUG_FIX
		// 무한 효과 아이템도 시간을 줄인다.
		// 시간을 매우 크게 잡기 때문에 상관 없을 것이라 생각됨.
#if defined(__AFFECT_RENEWAL__)
		if (pkAff->bRealTime)
		{
			if (pkAff->lDuration < get_global_time())
				bEnd = true;
		}
		else
		{
			if (--pkAff->lDuration <= 0)
				bEnd = true;
		}
#else
		if (--pkAff->lDuration <= 0)
		{
			bEnd = true;
		}
#endif
		// END_AFFECT_DURATION_BUG_FIX

		if (bEnd)
		{
			it = m_list_pkAffect.erase(it);
			ComputeAffect(pkAff, false);
			bDiff = true;

			if (IsPC())
				SendAffectRemovePacket(GetDesc(), GetPlayerID(), pkAff->dwType, pkAff->wApplyOn);

			CAffect::Release(pkAff);
			continue;
		}

		++it;
	}

	if (bDiff)
	{
		if (afOld != m_afAffectFlag || lMovSpd != GetPoint(POINT_MOV_SPEED) || lAttSpd != GetPoint(POINT_ATT_SPEED))
			UpdatePacket();

		CheckMaximumPoints();
	}

	if (m_list_pkAffect.empty())
		return true;

	return false;
}

void CHARACTER::SaveAffect()
{
	TPacketGDAddAffect p = {};

	AffectContainerList::iterator it = m_list_pkAffect.begin();
	while (it != m_list_pkAffect.end())
	{
		CAffect* pkAff = *it++;
		if (pkAff == nullptr)
			continue;

		if (IS_NO_SAVE_AFFECT(pkAff->dwType))
			continue;

		sys_log(1, "AFFECT_SAVE: %u %u %ld %d", pkAff->dwType, pkAff->wApplyOn, pkAff->lApplyValue, pkAff->lDuration);

		p.dwPID = GetPlayerID();
		p.elem.dwType = pkAff->dwType;
		p.elem.wApplyOn = pkAff->wApplyOn;
		p.elem.lApplyValue = pkAff->lApplyValue;
		p.elem.dwFlag = pkAff->dwFlag;
		p.elem.lDuration = pkAff->lDuration;
		p.elem.lSPCost = pkAff->lSPCost;
#if defined(__AFFECT_RENEWAL__)
		p.elem.bRealTime = pkAff->bRealTime;
		p.elem.bUpdate = pkAff->bUpdate;
#endif
		db_clientdesc->DBPacket(HEADER_GD_ADD_AFFECT, 0, &p, sizeof(p));
	}
}

EVENTINFO(load_affect_login_event_info)
{
	DWORD pid;
	DWORD count;
	char* data;

	load_affect_login_event_info()
		: pid(0)
		, count(0)
		, data(0)
	{
	}
};

EVENTFUNC(load_affect_login_event)
{
	load_affect_login_event_info* info = dynamic_cast<load_affect_login_event_info*>(event->info);

	if (info == NULL)
	{
		sys_err("load_affect_login_event_info> <Factor> Null pointer");
		return 0;
	}

	DWORD dwPID = info->pid;
	LPCHARACTER ch = CHARACTER_MANAGER::instance().FindByPID(dwPID);

	if (!ch)
	{
		M2_DELETE_ARRAY(info->data);
		return 0;
	}

	LPDESC d = ch->GetDesc();

	if (!d)
	{
		M2_DELETE_ARRAY(info->data);
		return 0;
	}

	if (d->IsPhase(PHASE_HANDSHAKE) ||
		d->IsPhase(PHASE_LOGIN) ||
		d->IsPhase(PHASE_SELECT) ||
		d->IsPhase(PHASE_DEAD) ||
		d->IsPhase(PHASE_LOADING))
	{
		return PASSES_PER_SEC(1);
	}
	else if (d->IsPhase(PHASE_CLOSE))
	{
		M2_DELETE_ARRAY(info->data);
		return 0;
	}
	else if (d->IsPhase(PHASE_GAME))
	{
		sys_log(1, "Affect Load by Event");
		ch->LoadAffect(info->count, (TPacketAffectElement*)info->data);
		M2_DELETE_ARRAY(info->data);
		return 0;
	}
	else
	{
		sys_err("input_db.cpp:quest_login_event INVALID PHASE pid %d", ch->GetPlayerID());
		M2_DELETE_ARRAY(info->data);
		return 0;
	}
}

void CHARACTER::LoadAffect(DWORD dwCount, TPacketAffectElement* pElements)
{
	m_bIsLoadedAffect = false;

	if (!GetDesc()->IsPhase(PHASE_GAME))
	{
		if (test_server)
			sys_log(0, "LOAD_AFFECT: Storing", GetName(), dwCount);

		load_affect_login_event_info* info = AllocEventInfo<load_affect_login_event_info>();
		info->pid = GetPlayerID();
		info->count = dwCount;
		info->data = M2_NEW char[sizeof(TPacketAffectElement) * dwCount];
		thecore_memcpy(info->data, pElements, sizeof(TPacketAffectElement) * dwCount);

		event_create(load_affect_login_event, info, PASSES_PER_SEC(1));

		return;
	}

	ClearAffect(true);

	if (test_server)
		sys_log(0, "LOAD_AFFECT: %s count %d", GetName(), dwCount);

	TAffectFlag afOld = m_afAffectFlag;

	long lMovSpd = GetPoint(POINT_MOV_SPEED);
	long lAttSpd = GetPoint(POINT_ATT_SPEED);

	for (DWORD i = 0; i < dwCount; ++i, ++pElements)
	{
		// 무영진은 로드하지않는다.
		if (pElements->dwType == SKILL_MUYEONG)
			continue;

		if (AFFECT_AUTO_HP_RECOVERY == pElements->dwType || AFFECT_AUTO_SP_RECOVERY == pElements->dwType)
		{
			LPITEM item = FindItemByID(pElements->dwFlag);

			if (NULL == item)
				continue;

			item->Lock(true);
		}

#if defined(__LOOT_FILTER_SYSTEM__) && defined(__PREMIUM_LOOT_FILTER__)
		if (AFFECT_LOOTING_SYSTEM == pElements->dwType)
			SetLootFilter();
#endif

#if defined(__SOUL_SYSTEM__)
		if (pElements->dwType == AFFECT_SOUL)
		{
			if (pElements->wApplyOn != AFF_SOUL_MIX)
			{
				LPITEM item = FindItemByID(pElements->lApplyValue);
				if (item == nullptr)
					continue;

				item->Lock(true);
			}
		}
#endif

#if defined(__SET_ITEM__)
		if (pElements->dwType == AFFECT_SET_ITEM)
			RefreshItemSetBonus();

		if (pElements->dwType >= AFFECT_SET_ITEM_SET_VALUE_1 && pElements->dwType <= AFFECT_SET_ITEM_SET_VALUE_5)
			RefreshItemSetBonusByValue();
#endif

		if (pElements->wApplyOn >= POINT_MAX_NUM)
		{
			sys_err("invalid affect data %s ApplyOn %u ApplyValue %ld",
				GetName(), pElements->wApplyOn, pElements->lApplyValue);
			continue;
		}

		if (test_server)
		{
			sys_log(0, "Load Affect : Affect %s %u %u", GetName(), pElements->dwType, pElements->wApplyOn);
		}

		CAffect* pkAff = CAffect::Acquire();
		m_list_pkAffect.push_back(pkAff);

		pkAff->dwType = pElements->dwType;
		pkAff->wApplyOn = pElements->wApplyOn;
		pkAff->lApplyValue = pElements->lApplyValue;
		pkAff->dwFlag = pElements->dwFlag;
		pkAff->lDuration = pElements->lDuration;
		pkAff->lSPCost = pElements->lSPCost;
#if defined(__AFFECT_RENEWAL__)
		pkAff->bRealTime = pElements->bRealTime;
		pkAff->bUpdate = pElements->bUpdate;
#endif
#if defined(__9TH_SKILL__)
		pkAff->lValue = 0;
#endif

		SendAffectAddPacket(GetDesc(), pkAff);

		ComputeAffect(pkAff, true);
	}

	if (CArenaManager::instance().IsArenaMap(GetMapIndex()) == true)
	{
		RemoveGoodAffect();
	}

	if (afOld != m_afAffectFlag || lMovSpd != GetPoint(POINT_MOV_SPEED) || lAttSpd != GetPoint(POINT_ATT_SPEED))
	{
		UpdatePacket();
	}

	StartAffectEvent();

	m_bIsLoadedAffect = true;

#if defined(__DRAGON_SOUL_SYSTEM__)
	// 용혼석 셋팅 로드 및 초기화
	DragonSoul_Initialize();
#endif

	if (!IsDead())
	{
		PointChange(POINT_HP, GetMaxHP() - GetHP());
		PointChange(POINT_SP, GetMaxSP() - GetSP());
	}
}

bool CHARACTER::AddAffect(DWORD dwType, POINT_TYPE wApplyOn, POINT_VALUE lApplyValue, DWORD dwFlag, long lDuration, long lSPCost, bool bOverride, bool IsCube
#if defined(__AFFECT_RENEWAL__)
	, bool bRealTime
#endif
#if defined(__9TH_SKILL__)
	, long lValue /* Skill iAmount2 */
#endif
)
{
	// CHAT_BLOCK
	if (dwType == AFFECT_BLOCK_CHAT && lDuration > 1)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_STRING("운영자 제제로 채팅이 금지 되었습니다."));
	}
	// END_OF_CHAT_BLOCK

	if (IsDead())
		return false;

#if defined(__LOOT_FILTER_SYSTEM__) && defined(__PREMIUM_LOOT_FILTER__)
	if (dwType == AFFECT_LOOTING_SYSTEM)
		SetLootFilter();
#endif

#if defined(__SNOWFLAKE_STICK_EVENT__)
	if (dwType == AFFECT_SNOWFLAKE_STICK_EVENT_SNOWFLAKE_BUFF)
		CSnowflakeStickEvent::Process(this, SNOWFLAKE_STICK_EVENT_GC_SUBHEADER_MESSAGE_GET_SNOWFLAKE_BUFF);
#endif

	if (lDuration == 0)
	{
		sys_err("Character::AddAffect lDuration == 0 type %d", lDuration, dwType);
		lDuration = 1;
	}

	CAffect* pkAff = NULL;

	if (IsCube)
		pkAff = FindAffect(dwType, wApplyOn);
	else
		pkAff = FindAffect(dwType);

	if (dwFlag == AFF_STUN)
	{
		if (m_posDest.x != GetX() || m_posDest.y != GetY())
		{
			m_posDest.x = m_posStart.x = GetX();
			m_posDest.y = m_posStart.y = GetY();
			battle_end(this);

			SyncPacket();
		}
	}

	// 이미 있는 효과를 덮어 쓰는 처리
	if (pkAff && bOverride)
	{
		ComputeAffect(pkAff, false); // 일단 효과를 삭제하고

		if (GetDesc())
			SendAffectRemovePacket(GetDesc(), GetPlayerID(), pkAff->dwType, pkAff->wApplyOn);
	}
	else
	{
		//
		// 새 에펙를 추가
		//
		// NOTE: 따라서 같은 type 으로도 여러 에펙트를 붙을 수 있다.
		//
		pkAff = CAffect::Acquire();
		m_list_pkAffect.push_back(pkAff);
	}

#if defined(__AFFECT_RENEWAL__)
	sys_log(0, "AddAffect %s type %u apply %u value %ld flag %u duration %ld real_time %d", GetName(), dwType, wApplyOn, lApplyValue, dwFlag, lDuration, (bRealTime ? 1 : 0));
	sys_log(1, "AddAffect %s type %u apply %u value %ld flag %u duration %ld real_time %d", GetName(), dwType, wApplyOn, lApplyValue, dwFlag, lDuration, (bRealTime ? 1 : 0));
#else
	sys_log(0, "AddAffect %s type %u apply %u value %ld flag %u duration %ld", GetName(), dwType, wApplyOn, lApplyValue, dwFlag, lDuration);
	sys_log(1, "AddAffect %s type %u apply %u value %ld flag %u duration %ld", GetName(), dwType, wApplyOn, lApplyValue, dwFlag, lDuration);
#endif

	pkAff->dwType = dwType;
	pkAff->wApplyOn = wApplyOn;
	pkAff->lApplyValue = lApplyValue;
	pkAff->dwFlag = dwFlag;
#if defined(__AFFECT_RENEWAL__)
	pkAff->lDuration = bRealTime ? (get_global_time() + lDuration) : lDuration;
	pkAff->bRealTime = bRealTime;
	pkAff->bUpdate = false;
#else
	pkAff->lDuration = lDuration;
#endif
	pkAff->lSPCost = lSPCost;
#if defined(__9TH_SKILL__)
	pkAff->lValue = lValue;
#endif

	long lMovSpd = GetPoint(POINT_MOV_SPEED);
	long lAttSpd = GetPoint(POINT_ATT_SPEED);

	ComputeAffect(pkAff, true);

	if (pkAff->dwFlag || lMovSpd != GetPoint(POINT_MOV_SPEED) || lAttSpd != GetPoint(POINT_ATT_SPEED))
		UpdatePacket();

	StartAffectEvent();

	if (IsPC())
	{
		SendAffectAddPacket(GetDesc(), pkAff);

		if (IS_NO_SAVE_AFFECT(pkAff->dwType))
			return true;

		TPacketGDAddAffect p;
		p.dwPID = GetPlayerID();
		p.elem.dwType = pkAff->dwType;
		p.elem.wApplyOn = pkAff->wApplyOn;
		p.elem.lApplyValue = pkAff->lApplyValue;
		p.elem.dwFlag = pkAff->dwFlag;
		p.elem.lDuration = pkAff->lDuration;
		p.elem.lSPCost = pkAff->lSPCost;
#if defined(__AFFECT_RENEWAL__)
		p.elem.bRealTime = pkAff->bRealTime;
		p.elem.bUpdate = pkAff->bUpdate;
#endif
		db_clientdesc->DBPacket(HEADER_GD_ADD_AFFECT, 0, &p, sizeof(p));
	}

	return true;
}

#if defined(__AFFECT_RENEWAL__)
bool CHARACTER::AddRealTimeAffect(DWORD dwType, POINT_TYPE wApplyOn, POINT_VALUE lApplyValue, DWORD dwFlag, long lDuration, long lSPCost, bool bOverride, bool IsCube, bool bRealTime)
{
	return AddAffect(dwType, wApplyOn, lApplyValue, dwFlag, lDuration, lSPCost, bOverride, IsCube, bRealTime);
}
#endif

void CHARACTER::RefreshAffect()
{
	for (CAffect* const& pAffect : m_list_pkAffect)
		ComputeAffect(pAffect, true);
}

void CHARACTER::ComputeAffect(const CAffect* pkAff, bool bAdd)
{
	if (bAdd && pkAff->dwType >= GUILD_SKILL_START && pkAff->dwType <= GUILD_SKILL_END)
	{
		if (!GetGuild())
			return;

		if (!GetGuild()->UnderAnyWar())
			return;
	}

	if (pkAff->dwFlag)
	{
		if (!bAdd)
			m_afAffectFlag.Reset(pkAff->dwFlag);
		else
			m_afAffectFlag.Set(pkAff->dwFlag);
	}

#if defined(__SOUL_SYSTEM__)
	if (pkAff->dwType == AFFECT_SOUL)
		return;
#endif

	if (bAdd)
		PointChange(pkAff->wApplyOn, pkAff->lApplyValue);
	else
		PointChange(pkAff->wApplyOn, -pkAff->lApplyValue);

	if (pkAff->dwType == SKILL_MUYEONG)
	{
		if (bAdd)
			StartMuyeongEvent();
		else
			StopMuyeongEvent();
	}

#if defined(__PVP_BALANCE_IMPROVING__)
	if (pkAff->dwType == SKILL_GYEONGGONG)
	{
		if (bAdd)
			StartGyeongGongEvent();
		else
			StopGyeongGongEvent();
	}
#endif

#if defined(__9TH_SKILL__)
	if (pkAff->dwType == SKILL_CHEONUN)
	{
		if (bAdd)
			StartCheonunEvent(pkAff->lApplyValue, pkAff->lValue);
		else
			StopCheonunEvent();
	}
#endif

	if (pkAff->dwType == AFFECT_MOUNT_FALL)
	{
		if (bAdd)
		{
			UnMount(false);
		}
		else
		{
			if (GetHorse())
				StartRiding();
			else if (GetWear(WEAR_COSTUME_MOUNT))
				MountVnum(GetPoint(POINT_MOUNT));
		}
	}
}

bool CHARACTER::RemoveAffect(CAffect* pkAff)
{
	if (!pkAff)
		return false;

	// AFFECT_BUF_FIX
	m_list_pkAffect.remove(pkAff);
	// END_OF_AFFECT_BUF_FIX

	ComputeAffect(pkAff, false);

	// 백기 버그 수정.
	// 백기 버그는 버프 스킬 시전->둔갑->백기 사용(AFFECT_REVIVE_INVISIBLE) 후 바로 공격 할 경우에 발생한다.
	// 원인은 둔갑을 시전하는 시점에, 버프 스킬 효과를 무시하고 둔갑 효과만 적용되게 되어있는데,
	// 백기 사용 후 바로 공격하면 RemoveAffect가 불리게 되고, ComputePoints하면서 둔갑 효과 + 버프 스킬 효과가 된다.
	// ComputePoints에서 둔갑 상태면 버프 스킬 효과 안 먹히도록 하면 되긴 하는데,
	// ComputePoints는 광범위하게 사용되고 있어서 큰 변화를 주는 것이 꺼려진다.(어떤 side effect가 발생할지 알기 힘들다.)
	// 따라서 AFFECT_REVIVE_INVISIBLE가 RemoveAffect로 삭제되는 경우만 수정한다.
	// 시간이 다 되어 백기 효과가 풀리는 경우는 버그가 발생하지 않으므로 그와 똑같이 함.
	// (ProcessAffect를 보면 시간이 다 되어서 Affect가 삭제되는 경우, ComputePoints를 부르지 않는다.)
	if (AFFECT_REVIVE_INVISIBLE != pkAff->dwType
#if defined(__SET_ITEM__)
		&& (!IS_NO_CLEAR_SET_ITEM(pkAff->dwType))
#endif
		&& IsPC())
		ComputePoints();
	else
		UpdatePacket();

#if defined(__LOOT_FILTER_SYSTEM__) && defined(__PREMIUM_LOOT_FILTER__)
	if (pkAff->dwType == AFFECT_LOOTING_SYSTEM)
		ClearLootFilter();
#endif

	CheckMaximumPoints();

	if (test_server)
		sys_log(0, "AFFECT_REMOVE: %s (flag %u apply: %u)", GetName(), pkAff->dwFlag, pkAff->wApplyOn);

	if (IsPC())
	{
		SendAffectRemovePacket(GetDesc(), GetPlayerID(), pkAff->dwType, pkAff->wApplyOn);
	}

	CAffect::Release(pkAff);
	return true;
}

bool CHARACTER::RemoveAffect(DWORD dwType)
{
	// CHAT_BLOCK
	if (dwType == AFFECT_BLOCK_CHAT)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_STRING("채팅 금지가 풀렸습니다."));
	}
	// END_OF_CHAT_BLOCK

	bool flag = false;

	CAffect* pkAff;
	while ((pkAff = FindAffect(dwType)))
	{
		RemoveAffect(pkAff);
		flag = true;
	}

	return flag;
}

#if defined(__SOUL_SYSTEM__)
void CHARACTER::RemoveAffect(DWORD dwType, POINT_TYPE wApplyType)
{
	const CAffect* pkAff = nullptr;
	for (const auto& it : m_list_pkAffect)
	{
		pkAff = it;

		if (pkAff->dwType == dwType)
		{
			if (pkAff->wApplyOn == wApplyType || pkAff->wApplyOn == APPLY_NONE)
			{
				break;
			}
		}

		pkAff = nullptr;
	}

	if (pkAff != nullptr)
		RemoveAffect(const_cast<CAffect*>(pkAff));
}
#endif

bool CHARACTER::IsAffectFlag(DWORD dwAff) const
{
	return m_afAffectFlag.IsSet(dwAff);
}

void CHARACTER::RemoveGoodAffect()
{
	RemoveAffect(AFFECT_MOV_SPEED);
	RemoveAffect(AFFECT_ATT_SPEED);
	RemoveAffect(AFFECT_STR);
	RemoveAffect(AFFECT_DEX);
	RemoveAffect(AFFECT_INT);
	RemoveAffect(AFFECT_CON);
	RemoveAffect(AFFECT_CHINA_FIREWORK);

	RemoveAffect(SKILL_JEONGWI);
	RemoveAffect(SKILL_GEOMKYUNG);
	RemoveAffect(SKILL_CHUNKEON);
	RemoveAffect(SKILL_EUNHYUNG);
	RemoveAffect(SKILL_GYEONGGONG);
	RemoveAffect(SKILL_GWIGEOM);
	RemoveAffect(SKILL_TERROR);
	RemoveAffect(SKILL_JUMAGAP);
	RemoveAffect(SKILL_MANASHILED);
	RemoveAffect(SKILL_HOSIN);
	RemoveAffect(SKILL_REFLECT);
	RemoveAffect(SKILL_KWAESOK);
	RemoveAffect(SKILL_JEUNGRYEOK);
	RemoveAffect(SKILL_GICHEON);

	RemoveAffect(SKILL_JEOKRANG);
	RemoveAffect(SKILL_CHEONGRANG);

#if defined(__9TH_SKILL__)
	RemoveAffect(SKILL_CHEONUN);
#endif
}

bool CHARACTER::IsGoodAffect(BYTE bAffectType) const
{
	switch (bAffectType)
	{
		case AFFECT_MOV_SPEED:
		case AFFECT_ATT_SPEED:
		case AFFECT_STR:
		case AFFECT_DEX:
		case AFFECT_INT:
		case AFFECT_CON:
		case AFFECT_CHINA_FIREWORK:

		case SKILL_JEONGWI:
		case SKILL_GEOMKYUNG:
		case SKILL_CHUNKEON:
		case SKILL_EUNHYUNG:
		case SKILL_GYEONGGONG:
		case SKILL_GWIGEOM:
		case SKILL_TERROR:
		case SKILL_JUMAGAP:
		case SKILL_MANASHILED:
		case SKILL_HOSIN:
		case SKILL_REFLECT:
		case SKILL_KWAESOK:
		case SKILL_JEUNGRYEOK:
		case SKILL_GICHEON:

		case SKILL_JEOKRANG:
		case SKILL_CHEONGRANG:

#if defined(__9TH_SKILL__)
		case SKILL_CHEONUN:
#endif
			return true;
	}

	return false;
}

void CHARACTER::RemoveBadAffect()
{
	sys_log(0, "RemoveBadAffect %s", GetName());
	// 독
	RemovePoison();
	RemoveFire();
	RemoveBleeding();

	// 스턴 : Value%로 상대방을 5초간 머리 위에 별이 돌아간다. (때리면 1/2 확률로 풀림) AFF_STUN
	RemoveAffect(AFFECT_STUN);

	// 슬로우 : Value%로 상대방의 공속/이속 모두 느려진다. 수련도에 따라 달라짐 기술로 사용 한 경우에 AFF_SLOW
	RemoveAffect(AFFECT_SLOW);

	// 투속마령
	RemoveAffect(SKILL_TUSOK);

	// 저주
	//RemoveAffect(SKILL_CURSE);

	// 파법술
	//RemoveAffect(SKILL_PABUP);

	// 기절 : Value%로 상대방을 기절시킨다. 2초 AFF_FAINT
	//RemoveAffect(AFFECT_FAINT);

	// 다리묶임 : Value%로 상대방의 이동속도를 떨어트린다. 5초간 -40 AFF_WEB
	//RemoveAffect(AFFECT_WEB);

	// 잠들기 : Value%로 상대방을 10초간 잠재운다. (때리면 풀림) AFF_SLEEP
	//RemoveAffect(AFFECT_SLEEP);

	// 저주 : Value%로 상대방의 공등/방등 모두 떨어트린다. 수련도에 따라 달라짐 기술로 사용 한 경우에 AFF_CURSE
	//RemoveAffect(AFFECT_CURSE);

	// 마비 : Value%로 상대방을 4초간 마비시킨다. AFF_PARA
	//RemoveAffect(AFFECT_PARALYZE);

	// 부동박부 : 무당 기술
	//RemoveAffect(SKILL_BUDONG);
}
