/**
* Filename: flower_event.cpp
* Author: Owsap
**/

#include "stdafx.h"

#if defined(__FLOWER_EVENT__)
#include "flower_event.h"
#include "char.h"
#include "desc.h"
#include "item.h"
#include "questmanager.h"
#include "config.h"

static const CFlowerEvent::TShootEventFlag g_aShootEventFlag[SHOOT_TYPE_MAX]
{
	{ SHOOT_ENVELOPE, "flower_event.envelope", 83022 },
	{ SHOOT_CHRYSANTHEMUM, "flower_event.chrysanthemum", 25121 },
	{ SHOOT_MAY_BELL, "flower_event.may_bell", 25122 },
	{ SHOOT_DAFFODIL, "flower_event.daffodil", 25123 },
	{ SHOOT_LILY, "flower_event.lily", 25123 },
	{ SHOOT_SUNFLOWER, "flower_event.sunflower", 25125 },
};

static const BYTE g_bExchangeCount[] = { 1, 10, 50, 100 };
static const DWORD g_dwShootGiftBox[SHOOT_TYPE_MAX] = { 0, 83023, 83024, 83025, 83026,83027 };

/* static */ void CFlowerEvent::Reward(LPCHARACTER pChar)
{
	int iShootCount = pChar->GetQuestFlag(g_aShootEventFlag[SHOOT_ENVELOPE].szQuestFlag);
	if (iShootCount >= MAX_FLOWER_EVENT_ITEM_COUNT)
	{
		Process(pChar, FLOWER_EVENT_SUBHEADER_GC_GET_INFO, FLOWER_EVENT_CHAT_TYPE_ENVELOPE_MAX);
		return;
	}

	int iNewShootCount = iShootCount + 1;
	pChar->SetQuestFlag(g_aShootEventFlag[SHOOT_ENVELOPE].szQuestFlag, iNewShootCount);

	Process(pChar, FLOWER_EVENT_SUBHEADER_GC_UPDATE_INFO, FLOWER_EVENT_CHAT_TYPE_MAX, SHOOT_ENVELOPE, iNewShootCount);
	Process(pChar, FLOWER_EVENT_SUBHEADER_GC_GET_INFO, FLOWER_EVENT_CHAT_TYPE_GET_SHOOT_ENVELOPE, SHOOT_ENVELOPE);
}

/* static */ void CFlowerEvent::RequestAllInfo(LPCHARACTER pChar)
{
	Process(pChar, FLOWER_EVENT_SUBHEADER_GC_INFO_ALL);
}

/* static */ bool CFlowerEvent::Exchange(LPCHARACTER pChar, BYTE bShootType, BYTE bExchangeKey)
{
	int iPulse = thecore_pulse();
	if (iPulse - pChar->GetLastFlowerEventExchangePulse() < passes_per_sec * FLOWER_EVENT_EXCHANGE_COOLTIME_SEC)
		return false;

	pChar->SetLastFlowerEventExchangePulse(thecore_pulse());

	int iShootCount = pChar->GetQuestFlag(g_aShootEventFlag[bShootType].szQuestFlag);
	int iExchangeCount = (bShootType == SHOOT_ENVELOPE) ?
		FLOWER_EVENT_SHOOT_ENVELOPE_NEED_COUNT * g_bExchangeCount[bExchangeKey] :
		FLOWER_EVENT_SHOOT_NEED_COUNT * g_bExchangeCount[bExchangeKey];

	if (iShootCount < iExchangeCount)
	{
		BYTE bChatType = (bShootType == SHOOT_ENVELOPE) ? FLOWER_EVENT_CHAT_TYPE_NOT_ENOUGH_SHOOT_ENVELOPE : FLOWER_EVENT_CHAT_TYPE_NOT_ENOUGH_SHOOT_COUNT;
		Process(pChar, FLOWER_EVENT_SUBHEADER_GC_GET_INFO, bChatType);
		return false;
	}

	int iNewShootCount = iShootCount - iExchangeCount;
	if (bShootType == SHOOT_ENVELOPE)
	{
		bool bFullAndNotUse = true;
		for (const CFlowerEvent::SShootEventFlag& it : g_aShootEventFlag)
		{
			if (pChar->GetQuestFlag(it.szQuestFlag) <= MAX_FLOWER_EVENT_ITEM_COUNT)
			{
				bFullAndNotUse = false;
				break;
			}
		}

		if (bFullAndNotUse)
		{
			Process(pChar, FLOWER_EVENT_SUBHEADER_GC_GET_INFO, FLOWER_EVENT_CHAT_TYPE_ITEM_FULL_AND_NOT_USE);
			return false;
		}

		if (GiveRandomShoot(pChar, iExchangeCount))
		{
			pChar->SetQuestFlag(g_aShootEventFlag[bShootType].szQuestFlag, iNewShootCount);
		}
	}
	else
	{
		LPITEM pItem = ITEM_MANAGER::Instance().CreateItem(g_dwShootGiftBox[bShootType], MIN(ITEM_MAX_COUNT, g_bExchangeCount[bExchangeKey]));
		if (NULL == pItem)
		{
			sys_err("Cannot create item vnum %u (char : %s)", g_dwShootGiftBox[bShootType], pChar->GetName());
			return false;
		}

		int iEmptyPos = pChar->GetEmptyInventory(pItem->GetSize());
		if (iEmptyPos == -1)
		{
			Process(pChar, FLOWER_EVENT_SUBHEADER_GC_GET_INFO, FLOWER_EVENT_CHAT_TYPE_NOT_ENOUGH_EVENTORY_SPACE);
			M2_DESTROY_ITEM(pItem);
			return false;
		}

#if defined(__WJ_PICKUP_ITEM_EFFECT__)
		pChar->AutoGiveItem(pItem, false, true, true);
#else
		pChar->AutoGiveItem(pItem, false, true);
#endif
		ITEM_MANAGER::Instance().FlushDelayedSave(pItem);

		pChar->SetQuestFlag(g_aShootEventFlag[bShootType].szQuestFlag, iNewShootCount);
		Process(pChar, FLOWER_EVENT_SUBHEADER_GC_UPDATE_INFO, FLOWER_EVENT_CHAT_TYPE_MAX, bShootType, iNewShootCount);
	}

	Process(pChar, FLOWER_EVENT_SUBHEADER_GC_UPDATE_INFO, FLOWER_EVENT_CHAT_TYPE_MAX, bShootType, iNewShootCount);
	return true;
}

/* static */ bool CFlowerEvent::GiveRandomShoot(LPCHARACTER pChar, int iExchangeCount)
{
	int aiShootCount[SHOOT_TYPE_MAX];
	memset(&aiShootCount, 0, sizeof(aiShootCount));

	for (int i = 0; i < iExchangeCount; ++i)
	{
		BYTE bRandomShootType = static_cast<BYTE>(number(SHOOT_CHRYSANTHEMUM, SHOOT_SUNFLOWER));
		aiShootCount[bRandomShootType]++;
	}

	for (BYTE bShootType = SHOOT_CHRYSANTHEMUM; bShootType <= SHOOT_SUNFLOWER; ++bShootType)
	{
		if (aiShootCount[bShootType] == 0)
			continue;

		int iShootCount = pChar->GetQuestFlag(g_aShootEventFlag[bShootType].szQuestFlag);
		if (iShootCount >= MAX_FLOWER_EVENT_ITEM_COUNT)
			continue;

		int iNewShootCount = iShootCount + aiShootCount[bShootType];
		if (iNewShootCount > MAX_FLOWER_EVENT_ITEM_COUNT)
		{
			aiShootCount[bShootType] = MAX_FLOWER_EVENT_ITEM_COUNT - iShootCount;
			iNewShootCount = MAX_FLOWER_EVENT_ITEM_COUNT;
		}

		pChar->SetQuestFlag(g_aShootEventFlag[bShootType].szQuestFlag, iNewShootCount);

		BYTE bChatType = FLOWER_EVENT_CHAT_TYPE_MAX;
		switch (bShootType)
		{
			case SHOOT_CHRYSANTHEMUM:
				bChatType = (aiShootCount[bShootType] == 1) ? FLOWER_EVENT_CHAT_TYPE_GET_SHOOT_CHRYSANTHEMUM : FLOWER_EVENT_CHAT_TYPE_GET_SHOOT_CHRYSANTHEMUM_COUNT;
				break;
			case SHOOT_MAY_BELL:
				bChatType = (aiShootCount[bShootType] == 1) ? FLOWER_EVENT_CHAT_TYPE_GET_SHOOT_MAY_BELL : FLOWER_EVENT_CHAT_TYPE_GET_SHOOT_MAY_BELL_COUNT;
				break;
			case SHOOT_DAFFODIL:
				bChatType = (aiShootCount[bShootType] == 1) ? FLOWER_EVENT_CHAT_TYPE_GET_SHOOT_DAFFODIL : FLOWER_EVENT_CHAT_TYPE_GET_SHOOT_DAFFODIL_COUNT;
				break;
			case SHOOT_LILY:
				bChatType = (aiShootCount[bShootType] == 1) ? FLOWER_EVENT_CHAT_TYPE_GET_SHOOT_LILY : FLOWER_EVENT_CHAT_TYPE_GET_SHOOT_LILY_COUNT;
				break;
			case SHOOT_SUNFLOWER:
				bChatType = (aiShootCount[bShootType] == 1) ? FLOWER_EVENT_CHAT_TYPE_GET_SHOOT_SUNFLOWER : FLOWER_EVENT_CHAT_TYPE_GET_SHOOT_SUNFLOWER_COUNT;
				break;
			default:
				break;
		}

		Process(pChar, FLOWER_EVENT_SUBHEADER_GC_UPDATE_INFO, FLOWER_EVENT_CHAT_TYPE_MAX, bShootType, iNewShootCount);
		Process(pChar, FLOWER_EVENT_SUBHEADER_GC_GET_INFO, bChatType, bShootType, aiShootCount[bShootType]);
	}

	return true;
}

/* static */ void CFlowerEvent::Process(LPCHARACTER pChar, BYTE bSubHeader, BYTE bChatType, BYTE bShootType, int iShootCount)
{
	LPDESC pDesc = pChar->GetDesc();
	if (pDesc == NULL)
		return;

	TPacketGCFlowerEvent FlowerEventPacket;
	FlowerEventPacket.bHeader = HEADER_GC_FLOWER_EVENT;
	FlowerEventPacket.bSubHeader = bSubHeader;
	FlowerEventPacket.bChatType = bChatType;

	if (bSubHeader == FLOWER_EVENT_SUBHEADER_GC_INFO_ALL)
	{
		FlowerEventPacket.bShootType = SHOOT_TYPE_MAX;
		for (const CFlowerEvent::SShootEventFlag& it : g_aShootEventFlag)
			FlowerEventPacket.aiShootCount[it.bShootType] = pChar->GetQuestFlag(it.szQuestFlag);
	}
	else
	{
		FlowerEventPacket.bShootType = bShootType;
		FlowerEventPacket.aiShootCount[bShootType] = iShootCount;
	}

	pDesc->Packet(&FlowerEventPacket, sizeof(FlowerEventPacket));
}

static BYTE GetFlowerAffectLevel(BYTE bCurLevel, BYTE bMinValue, BYTE bMaxValue, BYTE bMaxLevel)
{
	if (bCurLevel < bMinValue || bCurLevel == 0)
		return 1;

	if (bCurLevel >= bMaxValue)
		return bMaxLevel;

	return 1 + ((bCurLevel - bMinValue) * bMaxLevel) / (bMaxValue - bMinValue);
}

/* static */ bool CFlowerEvent::UseFlower(LPCHARACTER pChar, LPITEM pItem)
{
	if (!quest::CQuestManager::instance().GetEventFlag("e_flower_drop"))
	{
		pChar->ChatPacket(CHAT_TYPE_INFO, LC_STRING("The event is over. You cannot use this item anymore."));
		return false;
	}

	DWORD dwItemVNum = pItem->GetVnum();
	DWORD dwAffectType = pItem->GetValue(0);
	POINT_TYPE wApplyType = aApplyInfo[pItem->GetValue(1)].wPointType;
	long lAffectValue = pItem->GetValue(2);
	long lAffectDuration = pItem->GetValue(3);
	long lAffectValueMultiplier = pItem->GetValue(4);
	long lAffectMaxValue = lAffectValueMultiplier * AFFECT_MAX_LEVEL;

	LPAFFECT pAffect = pChar->FindAffect(dwAffectType, wApplyType);
	if (pAffect)
	{
		BYTE bAffectLevel = GetFlowerAffectLevel(pAffect->lApplyValue, lAffectValue, lAffectMaxValue, AFFECT_MAX_LEVEL);

		if (pAffect->lApplyValue >= lAffectMaxValue || bAffectLevel == AFFECT_MAX_LEVEL)
		{
			pChar->ChatPacket(CHAT_TYPE_INFO, LC_STRING("The maximum level has already been reached."));
			return false;
		}

		bAffectLevel += 1;

		if (number(1, 100) <= AFFECT_UPGRADE_SUCCESS_RATE)
		{
			pChar->AddAffect(dwAffectType, wApplyType, (bAffectLevel * lAffectValueMultiplier), 0, lAffectDuration, 0, true);
			pChar->EffectPacket(SE_FLOWER_EVENT);

			pChar->ChatPacket(CHAT_TYPE_INFO, LC_STRING("Success! The %s effect has been increased to level %d.", LC_ITEM(dwItemVNum), bAffectLevel));
		}
		else
		{
			pChar->ChatPacket(CHAT_TYPE_INFO, LC_STRING("Failure! The attempt to upgrade the %s effect to level %d failed.", LC_ITEM(dwItemVNum), bAffectLevel));
		}
	}
	else
	{
		bool bChange = pChar->FindAffect(dwAffectType);
		BYTE bSuccessRate = bChange ? AFFECT_CHANGE_SUCCESS_RATE : AFFECT_ADD_SUCCESS_RATE;

		if (number(1, 100) <= bSuccessRate)
		{
			pChar->AddAffect(dwAffectType, wApplyType, lAffectValue, 0, lAffectDuration, 0, true);
			pChar->EffectPacket(SE_FLOWER_EVENT);

			if (bChange)
				pChar->ChatPacket(CHAT_TYPE_INFO, LC_STRING("Success! The %s effect is now active.", LC_ITEM(dwItemVNum)));
			else
				pChar->ChatPacket(CHAT_TYPE_INFO, LC_STRING("Success! The %s effect is now active.", LC_ITEM(dwItemVNum)));
		}
		else
		{
			if (bChange)
				pChar->ChatPacket(CHAT_TYPE_INFO, LC_STRING("Failure! The attempt to change to the %s effect failed.", LC_ITEM(dwItemVNum)));
			else
				pChar->ChatPacket(CHAT_TYPE_INFO, LC_STRING("Failure! The %s effect was not activated.", LC_ITEM(dwItemVNum)));
		}
	}

	pItem->SetCount(pItem->GetCount() - 1);
	return true;
}
#endif // __FLOWER_EVENT__
