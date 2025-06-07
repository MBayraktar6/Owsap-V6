#include "stdafx.h"

#if defined(__DRAGON_SOUL_SYSTEM__)
#include "char.h"
#include "item.h"
#include "desc.h"
#include "DragonSoul.h"
#include "log.h"
#include "config.h"

// 용혼석 초기화
// 용혼석 on/off는 Affect로 저장되기 때문에,
// 용혼석 Affect가 있다면 덱에 있는 용혼석을 activate해야한다.
// 또한 용혼석 사용 자격은 QuestFlag로 저장해 놓았기 때문에, 
// 퀘스트 Flag에서 용혼석 사용 자격을 읽어온다.

// 캐릭터의 affect, quest가 load 되기 전에 DragonSoul_Initialize를 호출하면 안된다.
// affect가 가장 마지막에 로드되어 LoadAffect에서 호출함.
void CHARACTER::DragonSoul_Initialize()
{
	for (BYTE bSlotIdx = WEAR_MAX_NUM; bSlotIdx < DRAGON_SOUL_EQUIP_SLOT_END; bSlotIdx++)
	{
		LPITEM pItem = GetItem(TItemPos(EQUIPMENT, bSlotIdx));
		if (NULL != pItem)
			pItem->SetSocket(ITEM_SOCKET_DRAGON_SOUL_ACTIVE_IDX, 0);
	}

	if (FindAffect(AFFECT_DRAGON_SOUL_DECK_0))
	{
		DragonSoul_ActivateDeck(DRAGON_SOUL_DECK_0);
	}
	else if (FindAffect(AFFECT_DRAGON_SOUL_DECK_1))
	{
		DragonSoul_ActivateDeck(DRAGON_SOUL_DECK_1);
	}
}

int CHARACTER::DragonSoul_GetActiveDeck() const
{
	return m_pointsInstant.iDragonSoulActiveDeck;
}

bool CHARACTER::DragonSoul_IsDeckActivated() const
{
	return m_pointsInstant.iDragonSoulActiveDeck >= 0;
}

bool CHARACTER::DragonSoul_IsQualified() const
{
	return FindAffect(AFFECT_DRAGON_SOUL_QUALIFIED) != NULL;
}

void CHARACTER::DragonSoul_GiveQualification()
{
	if (NULL == FindAffect(AFFECT_DRAGON_SOUL_QUALIFIED))
	{
		LogManager::instance().CharLog(this, 0, "DS_QUALIFIED", "");
	}
	AddAffect(AFFECT_DRAGON_SOUL_QUALIFIED, APPLY_NONE, 0, AFF_NONE, INFINITE_AFFECT_DURATION, 0, false, false);
	//SetQuestFlag("dragon_soul.is_qualified", 1);
	//// 자격있다면 POINT_DRAGON_SOUL_IS_QUALIFIED는 무조건 1
	//PointChange(POINT_DRAGON_SOUL_IS_QUALIFIED, 1 - GetPoint(POINT_DRAGON_SOUL_IS_QUALIFIED));
}

bool CHARACTER::DragonSoul_ActivateDeck(int iDeckIdx)
{
	if (iDeckIdx < DRAGON_SOUL_DECK_0 || iDeckIdx >= DRAGON_SOUL_DECK_MAX_NUM)
		return false;

	if (DragonSoul_GetActiveDeck() == iDeckIdx)
		return true;

	DragonSoul_DeactivateAll();

	if (!DragonSoul_IsQualified())
	{
		ChatPacket(CHAT_TYPE_INFO, LC_STRING("용혼석 상자가 활성화되지 않았습니다."));
		return false;
	}

	AddAffect(AFFECT_DRAGON_SOUL_DECK_0 + iDeckIdx, APPLY_NONE, 0, AFF_DS, INFINITE_AFFECT_DURATION, 0, false);
	/*
	if (iDeckIdx == DRAGON_SOUL_DECK_0)
		SpecificEffectPacket("d:\\ymir work\\effect\\etc\\dragonsoul\\dragonsoul_earth.mse");
	else
		SpecificEffectPacket("d:\\ymir work\\effect\\etc\\dragonsoul\\dragonsoul_sky.mse");
	*/

	m_pointsInstant.iDragonSoulActiveDeck = iDeckIdx;

	for (BYTE bSlotIdx = DRAGON_SOUL_EQUIP_SLOT_START + DS_SLOT_MAX * iDeckIdx;
		bSlotIdx < DRAGON_SOUL_EQUIP_SLOT_START + DS_SLOT_MAX * (iDeckIdx + 1); bSlotIdx++)
	{
		LPITEM pItem = GetEquipmentItem(bSlotIdx);
		if (NULL != pItem)
			DSManager::instance().ActivateDragonSoul(pItem);
	}

#if defined(__DS_SET__)
	DragonSoul_SetBonus();
#endif

	return true;
}

void CHARACTER::DragonSoul_DeactivateAll()
{
	for (BYTE bSlotIdx = DRAGON_SOUL_EQUIP_SLOT_START; bSlotIdx < DRAGON_SOUL_EQUIP_SLOT_END; bSlotIdx++)
		DSManager::instance().DeactivateDragonSoul(GetEquipmentItem(bSlotIdx), true);

	m_pointsInstant.iDragonSoulActiveDeck = -1;

	RemoveAffect(AFFECT_DRAGON_SOUL_DECK_0);
	RemoveAffect(AFFECT_DRAGON_SOUL_DECK_1);
#if defined(__DS_SET__)
	RemoveAffect(AFFECT_DS_SET);
#endif
}

#if defined(__DS_SET__)
void CHARACTER::DragonSoul_SetBonus()
{
	// Remove any existing Dragon Soul Set effects
	RemoveAffect(AFFECT_DS_SET);

	// Check if the Dragon Soul deck is activated
	if (!DragonSoul_IsDeckActivated())
		return;

	const BYTE bDeckIdx = DragonSoul_GetActiveDeck();
	const BYTE bStartSlotIdx = WEAR_MAX_NUM + (bDeckIdx * DS_SLOT_MAX); // Calculate the starting slot index for the active deck.
	const BYTE bEndSlotIdx = bStartSlotIdx + DS_SLOT_MAX; // Calculate the ending slot index for the active deck.

	// Validate the deck index
	if (bDeckIdx < DRAGON_SOUL_DECK_0 || bDeckIdx >= DRAGON_SOUL_DECK_MAX_NUM)
		return;

	std::vector<BYTE> vDSGrade(DS_SLOT_MAX, 0); // Initialize a vector to store grades of Dragon Souls.
	BYTE bMaxWear = DS_SLOT6 + 1; // Maximum number of slots that can be worn (e.g., 6)
	BYTE bWearCount = 0; // Count of active Dragon Soul items

	for (BYTE bSlotIdx = bStartSlotIdx; bSlotIdx < bEndSlotIdx; ++bSlotIdx)
	{
		const LPITEM pItem = GetWear(bSlotIdx); // Get the item in the current slot.
		if (NULL == pItem) // Skip if there is no item.
			continue;

		// Check if the item is an active Dragon Soul and if it has time left.
		if (!DSManager::instance().IsActiveDragonSoul(pItem) ||
			!DSManager::instance().IsTimeLeftDragonSoul(pItem))
			continue;

		// Store the grade of the Dragon Soul.
		vDSGrade[bWearCount] = (pItem->GetVnum() / 1000) % 10;
		bWearCount += 1; // Increment the wear count.
	}

	// Check if 6 or more Dragon Souls are activated and all grades are above ancient.
	if (bWearCount >= bMaxWear && vDSGrade[0] > DRAGON_SOUL_GRADE_ANCIENT)
	{
		// Clean up all Dragon Souls, removing old attributes and applying new ones.
		DragonSoul_CleanUp();

		// Check if all activated Dragon Souls have the same grade
		if (std::equal(vDSGrade.begin() + 1, vDSGrade.begin() + bMaxWear, vDSGrade.begin()))
			// Apply the Dragon Soul Set effect
			AddAffect(AFFECT_DS_SET, APPLY_NONE, vDSGrade[0], 0, INFINITE_AFFECT_DURATION, 0, true);

		// Activate all Dragon Souls
		DragonSoul_ActivateAll();
	}
}

void CHARACTER::DragonSoul_ActivateAll()
{
	for (BYTE bSlotIdx = DRAGON_SOUL_EQUIP_SLOT_START; bSlotIdx < DRAGON_SOUL_EQUIP_SLOT_END; bSlotIdx++)
		DSManager::instance().ActivateDragonSoul(GetEquipmentItem(bSlotIdx));
}
#endif

void CHARACTER::DragonSoul_CleanUp()
{
	for (BYTE bSlotIdx = DRAGON_SOUL_EQUIP_SLOT_START; bSlotIdx < DRAGON_SOUL_EQUIP_SLOT_END; bSlotIdx++)
		DSManager::instance().DeactivateDragonSoul(GetEquipmentItem(bSlotIdx), true);
}

bool CHARACTER::DragonSoul_RefineWindow_Open(LPENTITY pEntity)
{
	if (NULL == m_pointsInstant.m_pDragonSoulRefineWindowOpener)
	{
		m_pointsInstant.m_pDragonSoulRefineWindowOpener = pEntity;
	}

	TPacketGCDragonSoulRefine PDS;
	PDS.header = HEADER_GC_DRAGON_SOUL_REFINE;
	PDS.bSubType = DS_SUB_HEADER_OPEN;

	LPDESC d = GetDesc();

	if (NULL == d)
	{
		sys_err("User(%s)'s DESC is NULL POINT.", GetName());
		return false;
	}

	d->Packet(&PDS, sizeof(PDS));
	return true;
}

#if defined(__DS_CHANGE_ATTR__)
bool CHARACTER::DragonSoul_RefineWindow_ChangeAttr_Open(LPENTITY pEntity)
{
	if (m_pointsInstant.m_pDragonSoulRefineWindowOpener == nullptr)
		m_pointsInstant.m_pDragonSoulRefineWindowOpener = pEntity;

	TPacketGCDragonSoulRefine PDS;
	PDS.header = HEADER_GC_DRAGON_SOUL_REFINE;
	PDS.bSubType = DS_SUB_HEADER_OPEN_CHANGE_ATTR;

	LPDESC lpDesc = GetDesc();
	if (lpDesc == nullptr)
	{
		sys_err("User(%s)'s DESC is NULL POINT.", GetName());
		return false;
	}

	lpDesc->Packet(&PDS, sizeof(PDS));
	return true;
}
#endif

bool CHARACTER::DragonSoul_RefineWindow_Close()
{
	m_pointsInstant.m_pDragonSoulRefineWindowOpener = NULL;
	return true;
}

bool CHARACTER::DragonSoul_RefineWindow_CanRefine()
{
	return NULL != m_pointsInstant.m_pDragonSoulRefineWindowOpener;
}
#endif
