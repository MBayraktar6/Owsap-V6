#include "stdafx.h"
#include "utils.h"
#include "config.h"
#include "char.h"
#include "char_manager.h"
#include "desc_client.h"
#include "db.h"
#include "log.h"
#include "skill.h"
#include "text_file_loader.h"
#include "priv_manager.h"
#include "questmanager.h"
#include "unique_item.h"
#include "unique_mob.h"
#include "safebox.h"
#include "blend_item.h"
#include "dev_log.h"
#include "locale_service.h"
#include "item.h"
#include "item_manager.h"

#include "../../common/VnumHelper.h"
#include "DragonSoul.h"
#include "cube.h"
#if defined(__SNOWFLAKE_STICK_EVENT__)
#	include "xmas_event.h"
#endif

ITEM_MANAGER::ITEM_MANAGER()
	: m_iTopOfTable(0), m_dwVIDCount(0), m_dwCurrentID(0)
#if defined(__ITEM_APPLY_RANDOM__)
	, m_pApplyRandomTable(nullptr)
#endif
{
	m_ItemIDRange.dwMin = m_ItemIDRange.dwMax = m_ItemIDRange.dwUsableItemIDMin = 0;
	m_ItemIDSpareRange.dwMin = m_ItemIDSpareRange.dwMax = m_ItemIDSpareRange.dwUsableItemIDMin = 0;
#if defined(__LUCKY_BOX__)
	m_pLuckyBox = nullptr;
#endif
#if defined(__ITEM_APPLY_RANDOM__)
	m_pApplyRandomTable = nullptr;
#endif
}

ITEM_MANAGER::~ITEM_MANAGER()
{
	Destroy();
}

void ITEM_MANAGER::Destroy()
{
	ITEM_VID_MAP::iterator it = m_VIDMap.begin();
	for (; it != m_VIDMap.end(); ++it)
	{
		M2_DELETE(it->second);
	}
	m_VIDMap.clear();

#if defined(__LUCKY_BOX__)
	if (m_pLuckyBox)
	{
		delete m_pLuckyBox;
		m_pLuckyBox = nullptr;
	}
#endif

#if defined(__ITEM_APPLY_RANDOM__)
	if (m_pApplyRandomTable)
	{
		delete m_pApplyRandomTable;
		m_pApplyRandomTable = nullptr;
	}
#endif

#if defined(__GEM_SYSTEM__) && defined(__GEM_SHOP__)
	if (!m_map_pGemShopItemGroup.empty())
	{
		GemShopItemGroupMap::iterator it = m_map_pGemShopItemGroup.begin();
		for (; it != m_map_pGemShopItemGroup.end(); ++it)
			M2_DELETE(it->second);
		m_map_pGemShopItemGroup.clear();
	}
	m_mapGemShopAddSlotItemGroup.clear();
#endif

#if defined(__SET_ITEM__)
	m_ItemSetValueMap.clear();
	m_ItemSetItemMap.clear();
#endif
}

void ITEM_MANAGER::GracefulShutdown()
{
	std::unordered_set<LPITEM>::iterator it = m_set_pkItemForDelayedSave.begin();
	while (it != m_set_pkItemForDelayedSave.end())
		SaveSingleItem(*(it++));

	m_set_pkItemForDelayedSave.clear();
}

bool ITEM_MANAGER::Initialize(TItemTable* table, int size)
{
	if (!m_vec_prototype.empty())
		m_vec_prototype.clear();

	int i;

	m_vec_prototype.resize(size);
	thecore_memcpy(&m_vec_prototype[0], table, sizeof(TItemTable) * size);
	for (int i = 0; i < size; i++)
	{
		if (0 != m_vec_prototype[i].dwVnumRange)
		{
			m_vec_item_vnum_range_info.push_back(&m_vec_prototype[i]);
		}
	}

	m_map_ItemRefineFrom.clear();
	for (i = 0; i < size; ++i)
	{
		if (m_vec_prototype[i].dwRefinedVnum)
			m_map_ItemRefineFrom.insert(std::make_pair(m_vec_prototype[i].dwRefinedVnum, m_vec_prototype[i].dwVnum));

		// NOTE : QUEST_GIVE ÇÃ·¡±×´Â npc ÀÌº¥Æ®·Î ¹ß»ý.
		if (m_vec_prototype[i].bType == ITEM_QUEST || IS_SET(m_vec_prototype[i].dwFlags, ITEM_FLAG_QUEST_USE | ITEM_FLAG_QUEST_USE_MULTIPLE))
			quest::CQuestManager::instance().RegisterNPCVnum(m_vec_prototype[i].dwVnum);

		// NOTE : Support for PET_PAY
		if (m_vec_prototype[i].bType == ITEM_PET && m_vec_prototype[i].bSubType == PET_PAY)
			quest::CQuestManager::instance().RegisterNPCVnum(m_vec_prototype[i].dwVnum);

#if defined(__FISHING_GAME__)
		// Quest item support for fish.
		if (m_vec_prototype[i].bType == ITEM_FISH && m_vec_prototype[i].bSubType == FISH_ALIVE)
			quest::CQuestManager::instance().RegisterNPCVnum(m_vec_prototype[i].dwVnum);
#endif

		m_map_vid.insert(std::map<DWORD, TItemTable>::value_type(m_vec_prototype[i].dwVnum, m_vec_prototype[i]));
		if (test_server)
			sys_log(0, "ITEM_INFO %d %s ", m_vec_prototype[i].dwVnum, m_vec_prototype[i].szName);
	}

	int len = 0, len2;
	char buf[512];

	for (i = 0; i < size; ++i)
	{
		len2 = snprintf(buf + len, sizeof(buf) - len, "%5u %-16s", m_vec_prototype[i].dwVnum, m_vec_prototype[i].szLocaleName);

		if (len2 < 0 || len2 >= (int)sizeof(buf) - len)
			len += (sizeof(buf) - len) - 1;
		else
			len += len2;

		if (!((i + 1) % 4))
		{
			if (!test_server)
				sys_log(0, "%s", buf);
			len = 0;
		}
		else
		{
			buf[len++] = '\t';
			buf[len] = '\0';
		}
	}

	if ((i + 1) % 4)
	{
		if (!test_server)
			sys_log(0, "%s", buf);
	}

	ITEM_VID_MAP::iterator it = m_VIDMap.begin();

	sys_log(1, "ITEM_VID_MAP %d", m_VIDMap.size());

	while (it != m_VIDMap.end())
	{
		LPITEM item = it->second;
		++it;

		const TItemTable* tableInfo = GetTable(item->GetOriginalVnum());

		if (NULL == tableInfo)
		{
			sys_err("cannot reset item table");
			item->SetProto(NULL);
		}

		item->SetProto(tableInfo);
	}

	return true;
}

LPITEM ITEM_MANAGER::CreateItem(DWORD vnum, DWORD count, DWORD id, bool bTryMagic, int iRarePct, bool bSkipSave, bool bSkilAddon)
{
	if (0 == vnum)
		return NULL;

	DWORD dwMaskVnum = 0;
	if (GetMaskVnum(vnum))
	{
		dwMaskVnum = GetMaskVnum(vnum);
	}

	if (LC_IsKorea() || LC_IsYMIR())
	{
		if (vnum == ITEM_SKILLBOOK_VNUM && bTryMagic)
		{
			// ¼ö·Ã¼­¸¦ ¾ÆÀÌÅÛ ¹øÈ£¸¦ ´Ù¸£°Ô ¸¸µé¾î ÁØ´Ù.
			DWORD dwSkillVnum;

			do
			{
#if defined(__WOLFMAN_CHARACTER__)
				dwSkillVnum = number(1, 175);
				if (dwSkillVnum > 111 && dwSkillVnum < 170)
					continue;
#else
				dwSkillVnum = number(1, 111);
#endif

				CSkillProto* pkSk = CSkillManager::instance().Get(dwSkillVnum);

				if (!pkSk)
					continue;

				break;
			} while (1);

			vnum = 50400 + dwSkillVnum;
		}
	}

	const TItemTable* table = GetTable(vnum);

	if (NULL == table)
		return NULL;

	LPITEM item = NULL;

	// id·Î °Ë»çÇØ¼­ Á¸ÀçÇÑ´Ù¸é -- ¸®ÅÏ!
	if (m_map_pkItemByID.find(id) != m_map_pkItemByID.end())
	{
		item = m_map_pkItemByID[id];
		if (item)
		{
			LPCHARACTER owner = item->GetOwner();
			if (!owner)
				sys_err("ITEM_ID_DUP: %u %s null owner", id, item->GetName());
			else
				sys_err("ITEM_ID_DUP: %u %s owner %p", id, item->GetName(), get_pointer(owner));
		}
		return NULL;
	}

	// ¾ÆÀÌÅÛ ÇÏ³ª ÇÒ´çÇÏ°í
	item = M2_NEW CItem(vnum);

	bool bIsNewItem = (0 == id);

	// ÃÊ±âÈ­ ÇÏ°í. Å×ÀÌºí ¼ÂÇÏ°í
	item->Initialize();
	item->SetProto(table);
	item->SetMaskVnum(dwMaskVnum);

#if defined(__SOUL_BIND_SYSTEM__)
	if (bIsNewItem)
		item->SealItem(E_SEAL_DATE_DEFAULT_TIMESTAMP);
#endif

	if (item->GetType() == ITEM_ELK) // µ·Àº ID°¡ ÇÊ¿ä¾ø°í ÀúÀåµµ ÇÊ¿ä¾ø´Ù.
		item->SetSkipSave(true);

	// Unique ID¸¦ ¼¼ÆÃÇÏÀÚ
	else if (!bIsNewItem)
	{
		item->SetID(id);
		item->SetSkipSave(true);
	}
	else
	{
		item->SetID(GetNewID());

#if defined(__MOUNT_COSTUME_SYSTEM__)
		if (item->GetType() == ITEM_UNIQUE || item->IsRideItem())
#else
		if (item->GetType() == ITEM_UNIQUE) // À¯´ÏÅ© ¾ÆÀÌÅÛÀº »ý¼º½Ã¿¡ ¼ÒÄÏ¿¡ ³²Àº½Ã°£À» ±â·ÏÇÑ´Ù.
#endif
		{
			if (item->GetValue(2) == 0)
				item->SetSocket(ITEM_SOCKET_UNIQUE_REMAIN_TIME, item->GetValue(0)); // °ÔÀÓ ½Ã°£ À¯´ÏÅ©
			else
			{
				// int globalTime = get_global_time();
				// int lastTime = item->GetValue(0);
				// int endTime = get_global_time() + item->GetValue(0);
				item->SetSocket(ITEM_SOCKET_UNIQUE_REMAIN_TIME, get_global_time() + item->GetValue(0)); // ½Ç½Ã°£ À¯´ÏÅ©
			}
		}
	}

	switch (item->GetVnum())
	{
		case ITEM_AUTO_HP_RECOVERY_S:
		case ITEM_AUTO_HP_RECOVERY_M:
		case ITEM_AUTO_HP_RECOVERY_L:
		case ITEM_AUTO_HP_RECOVERY_X:
		case ITEM_AUTO_SP_RECOVERY_S:
		case ITEM_AUTO_SP_RECOVERY_M:
		case ITEM_AUTO_SP_RECOVERY_L:
		case ITEM_AUTO_SP_RECOVERY_X:
		case REWARD_BOX_ITEM_AUTO_SP_RECOVERY_XS:
		case REWARD_BOX_ITEM_AUTO_SP_RECOVERY_S:
		case REWARD_BOX_ITEM_AUTO_HP_RECOVERY_XS:
		case REWARD_BOX_ITEM_AUTO_HP_RECOVERY_S:
			if (bIsNewItem)
				item->SetSocket(2, item->GetValue(0), true);
			else
				item->SetSocket(2, item->GetValue(0), false);
			break;
	}

	if (item->GetType() == ITEM_ELK) // µ·Àº ¾Æ¹« Ã³¸®°¡ ÇÊ¿äÇÏÁö ¾ÊÀ½
		;
	else if (item->IsStackable()) // ÇÕÄ¥ ¼ö ÀÖ´Â ¾ÆÀÌÅÛÀÇ °æ¿ì
	{
		count = MINMAX(1, count, ITEM_MAX_COUNT);

		if (bTryMagic && count <= 1 && IS_SET(item->GetFlag(), ITEM_FLAG_MAKECOUNT))
			count = item->GetValue(1);
	}
	else
		count = 1;

	item->SetVID(++m_dwVIDCount);

	if (bSkipSave == false)
		m_VIDMap.insert(ITEM_VID_MAP::value_type(item->GetVID(), item));

	if (item->GetID() != 0 && bSkipSave == false)
		m_map_pkItemByID.insert(ItemMap::value_type(item->GetID(), item));

	if (!item->SetCount(count))
		return NULL;

	item->SetSkipSave(false);

	if (item->GetType() == ITEM_UNIQUE && item->GetValue(2) != 0)
		item->StartUniqueExpireEvent();
#if defined(__MOUNT_COSTUME_SYSTEM__)
	else if (item->IsRideItem() && item->GetValue(2) != 0)
		item->StartUniqueExpireEvent();
#endif

	for (int i = 0; i < ITEM_LIMIT_MAX_NUM; i++)
	{
		// ¾ÆÀÌÅÛ »ý¼º ½ÃÁ¡ºÎÅÍ »ç¿ëÇÏÁö ¾Ê¾Æµµ ½Ã°£ÀÌ Â÷°¨µÇ´Â ¹æ½Ä
		if (LIMIT_REAL_TIME == item->GetLimitType(i))
		{
			if (item->GetLimitValue(i))
			{
				item->SetSocket(0, time(0) + item->GetLimitValue(i));
			}
			else
			{
				item->SetSocket(0, time(0) + 60 * 60 * 24 * 7);
			}

			item->StartRealTimeExpireEvent();
		}

		// ±âÁ¸ À¯´ÏÅ© ¾ÆÀÌÅÛÃ³·³ Âø¿ë½Ã¿¡¸¸ »ç¿ë°¡´É ½Ã°£ÀÌ Â÷°¨µÇ´Â ¹æ½Ä
		else if (LIMIT_TIMER_BASED_ON_WEAR == item->GetLimitType(i))
		{
			// ÀÌ¹Ì Âø¿ëÁßÀÎ ¾ÆÀÌÅÛÀÌ¸é Å¸ÀÌ¸Ó¸¦ ½ÃÀÛÇÏ°í, »õ·Î ¸¸µå´Â ¾ÆÀÌÅÛÀº »ç¿ë °¡´É ½Ã°£À» ¼¼ÆÃÇØÁØ´Ù. (
			// ¾ÆÀÌÅÛ¸ô·Î Áö±ÞÇÏ´Â °æ¿ì¿¡´Â ÀÌ ·ÎÁ÷¿¡ µé¾î¿À±â Àü¿¡ Socket0 °ªÀÌ ¼¼ÆÃÀÌ µÇ¾î ÀÖ¾î¾ß ÇÑ´Ù.
			if (true == item->IsEquipped())
			{
				item->StartTimerBasedOnWearExpireEvent();
			}
			else if (0 == id)
			{
				long duration = item->GetSocket(0);
				if (0 == duration)
					duration = item->GetLimitValue(i);

				if (0 == duration)
					duration = 60 * 60 * 10; // Á¤º¸°¡ ¾Æ¹«°Íµµ ¾øÀ¸¸é µðÆúÆ®·Î 10½Ã°£ ¼¼ÆÃ

				item->SetSocket(0, duration);
			}
		}
	}

#if defined(__MINI_GAME_CATCH_KING__)
	if (item->GetVnum() == ITEM_VNUM_CATCH_KING_PIECE || item->GetVnum() == ITEM_VNUM_CATCH_KING_PACK)
	{
		int iEndTime = quest::CQuestManager::instance().GetEventFlag("mini_game_catchking");
		if (iEndTime)
		{
			item->SetSocket(0, iEndTime);
		}
	}
#endif

#if defined(__GACHA_SYSTEM__)
	if (item->GetType() == ITEM_GACHA)
		item->SetSocket(0, item->GetLimitValue(1));
#endif

#if defined(__SOUL_SYSTEM__)
	if (item->GetType() == ITEM_SOUL)
		item->StartSoulTimerUseEvent();
#endif

	if (id == 0) // »õ·Î ¸¸µå´Â ¾ÆÀÌÅÛÀÏ ¶§¸¸ Ã³¸®
	{
		// »õ·ÎÃß°¡µÇ´Â ¾àÃÊµéÀÏ°æ¿ì ¼º´ÉÀ» ´Ù¸£°ÔÃ³¸®
		if (ITEM_BLEND == item->GetType())
		{
			if (Blend_Item_find(item->GetVnum()))
			{
				Blend_Item_set_value(item);
				return item;
			}
		}

#if defined(__ITEM_APPLY_RANDOM__)
		for (BYTE bApplyIndex = 0; bApplyIndex < ITEM_APPLY_MAX_NUM; ++bApplyIndex)
		{
			POINT_TYPE wApplyRandomType = table->aApplies[bApplyIndex].wType;
			POINT_VALUE lApplyRandomValue = table->aApplies[bApplyIndex].lValue;
			if (wApplyRandomType == APPLY_RANDOM)
			{
				POINT_TYPE wApplyType = APPLY_NONE; POINT_VALUE lApplyValue = 0; BYTE bPath = 0;
				if (GetApplyRandom(lApplyRandomValue, item->GetRefineLevel(), wApplyType, lApplyValue, bPath))
					item->SetForceRandomApply(bApplyIndex, wApplyType, lApplyValue, bPath);
			}
		}
#endif

#if defined(__ITEM_APPLY_RANDOM__) && defined(__ITEM_VALUE10__)
		if (item->GetType() == ITEM_WEAPON)
		{
			const DWORD dwMinMtkValRng = item->GetValue(ITEM_VALUE_MTK_MIN_INDEX);
			const DWORD dwMaxMtkValRng = item->GetValue(ITEM_VALUE_MTK_MAX_INDEX);

			if (dwMinMtkValRng && dwMaxMtkValRng)
			{
				DWORD dwMinValue, dwMaxValue;
				DWORD dwRndMinValue, dwRndMaxValue;

				dwMinValue = dwMinMtkValRng / ITEM_VALUE_MINMAX_RANDOM_DIVISION_VALUE;
				dwMaxValue = dwMinMtkValRng % (ITEM_VALUE_MINMAX_RANDOM_DIVISION_VALUE / 100);
				dwRndMinValue = number(dwMinValue, dwMaxValue);

				dwMinValue = dwMaxMtkValRng / ITEM_VALUE_MINMAX_RANDOM_DIVISION_VALUE;
				dwMaxValue = dwMaxMtkValRng % (ITEM_VALUE_MINMAX_RANDOM_DIVISION_VALUE / 100);
				dwRndMaxValue = number(dwMinValue, dwMaxValue);

				const DWORD dwValue = dwRndMinValue * ITEM_VALUE_MINMAX_RANDOM_DIVISION_VALUE + dwRndMaxValue;
				item->SetSocket(ITEM_SOCKET_MTK_MINMAX_RANDOM, dwValue);
			}

			const DWORD dwMinAtkValRng = item->GetValue(ITEM_VALUE_ATK_MIN_INDEX);
			const DWORD dwMaxAtkValRng = item->GetValue(ITEM_VALUE_ATK_MAX_INDEX);

			if (dwMinAtkValRng && dwMaxAtkValRng)
			{
				DWORD dwMinValue, dwMaxValue;
				DWORD dwRndMinValue, dwRndMaxValue;

				dwMinValue = dwMinAtkValRng / ITEM_VALUE_MINMAX_RANDOM_DIVISION_VALUE;
				dwMaxValue = dwMinAtkValRng % (ITEM_VALUE_MINMAX_RANDOM_DIVISION_VALUE / 100);
				dwRndMinValue = number(dwMinValue, dwMaxValue);

				dwMinValue = dwMaxAtkValRng / ITEM_VALUE_MINMAX_RANDOM_DIVISION_VALUE;
				dwMaxValue = dwMaxAtkValRng % (ITEM_VALUE_MINMAX_RANDOM_DIVISION_VALUE / 100);
				dwRndMaxValue = number(dwMinValue, dwMaxValue);

				const DWORD dwValue = dwRndMinValue * ITEM_VALUE_MINMAX_RANDOM_DIVISION_VALUE + dwRndMaxValue;
				item->SetSocket(ITEM_SOCKET_ATK_MINMAX_RANDOM, dwValue);
			}
		}
		else if (item->GetType() == ITEM_ARMOR || item->GetType() == ITEM_BELT)
		{
			const DWORD dwDefValRng = item->GetValue(ITEM_VALUE_DEF_MIN_INDEX);

			if (dwDefValRng)
			{
				DWORD dwMinValue, dwMaxValue;
				DWORD dwRndMinValue;

				dwMinValue = dwDefValRng / ITEM_VALUE_MINMAX_RANDOM_DIVISION_VALUE;
				dwMaxValue = dwDefValRng % (ITEM_VALUE_MINMAX_RANDOM_DIVISION_VALUE / 100);
				dwRndMinValue = number(dwMinValue, dwMaxValue);

				const DWORD dwValue = dwRndMinValue * ITEM_VALUE_MINMAX_RANDOM_DIVISION_VALUE + dwRndMinValue;
				item->SetSocket(ITEM_SOCKET_DEF_MINMAX_RANDOM, dwValue);
			}
		}
#endif

		if (table->sAddonType && !bSkilAddon)
		{
			item->ApplyAddon(table->sAddonType);
		}

		if (bTryMagic)
		{
			if (iRarePct == -1)
				iRarePct = table->bAlterToMagicItemPct;

			if (number(1, 100) <= iRarePct)
				item->AlterToMagicItem(number(40, 50), number(10, 15));
		}

		if (table->bGainSocketPct)
			item->AlterToSocketItem(table->bGainSocketPct);

		// 50300 == ±â¼ú ¼ö·Ã¼­
		if (vnum == ITEM_SKILLBOOK_VNUM || vnum == ITEM_SKILLFORGET_VNUM)
		{
			DWORD dwSkillVnum;

			do
			{
#if defined(__WOLFMAN_CHARACTER__)
				dwSkillVnum = number(1, 175);
				if (dwSkillVnum > 111 && dwSkillVnum < 170)
					continue;
#else
				dwSkillVnum = number(1, 111);
#endif

				if (NULL != CSkillManager::instance().Get(dwSkillVnum))
					break;
			} while (true);

			item->SetSocket(0, dwSkillVnum);
		}
		/*
		else if (ITEM_SKILLFORGET2_VNUM == vnum)
		{
			DWORD dwSkillVnum;

			do
			{
				dwSkillVnum = number(112, 119);

				if (NULL != CSkillManager::instance().Get(dwSkillVnum))
					break;
			} while (true);

			item->SetSocket(0, dwSkillVnum);
		}
		*/
	}

	if (item->GetType() == ITEM_QUEST)
	{
		for (auto it = m_map_pkQuestItemGroup.begin(); it != m_map_pkQuestItemGroup.end(); it++)
		{
			if (it->second->m_bType == CSpecialItemGroup::QUEST && it->second->Contains(vnum))
			{
				item->SetSIGVnum(it->first);
			}
		}
	}
	else if (item->GetType() == ITEM_UNIQUE)
	{
		for (auto it = m_map_pkSpecialItemGroup.begin(); it != m_map_pkSpecialItemGroup.end(); it++)
		{
			if (it->second->m_bType == CSpecialItemGroup::SPECIAL && it->second->Contains(vnum))
			{
				item->SetSIGVnum(it->first);
			}
		}
	}
#if defined(__ACCE_COSTUME_SYSTEM__)
	else if (item->IsCostumeAcce())
	{
		if (item->GetRefinedVnum() == 0)
			item->SetSocket(ITEM_SOCKET_ACCE_DRAIN_VALUE, number(11, ACCE_MAX_DRAINRATE));
	}
#endif
#if defined(__AURA_COSTUME_SYSTEM__)
	else if (item->GetType() == ITEM_COSTUME && item->GetSubType() == COSTUME_AURA)
	{
		const BYTE c_bGrade = item->GetOriginalVnum() % 10;
		const BYTE c_bBaseLevel = GetAuraRefineInfo(c_bGrade, AURA_REFINE_INFO_LEVEL_MIN);
		item->SetSocket(ITEM_SOCKET_AURA_LEVEL_VALUE, (1000 + c_bBaseLevel) * 100000);
	}
#endif

#if defined(__DRAGON_SOUL_SYSTEM__)
	// »õ·Î »ý¼ºµÇ´Â ¿ëÈ¥¼® Ã³¸®.
	if (item->IsDragonSoul() && 0 == id)
		DSManager::instance().DragonSoulItemInitialize(item);
#endif

	return item;
}

void ITEM_MANAGER::DelayedSave(LPITEM item)
{
	if (item && item->GetID() != 0)
		m_set_pkItemForDelayedSave.insert(item);
}

void ITEM_MANAGER::FlushDelayedSave(LPITEM item)
{
	std::unordered_set<LPITEM>::iterator it = m_set_pkItemForDelayedSave.find(item);
	if (it == m_set_pkItemForDelayedSave.end())
		return;

	m_set_pkItemForDelayedSave.erase(it);
	SaveSingleItem(item);
}

void ITEM_MANAGER::SaveSingleItem(LPITEM item)
{
	if (!item)
		return;

	const LPCHARACTER owner = item->GetOwner();
	if (NULL == owner)
	{
		DWORD dwID = item->GetID();
		DWORD dwOwnerID = item->GetLastOwnerPID();

		db_clientdesc->DBPacketHeader(HEADER_GD_ITEM_DESTROY, 0, sizeof(DWORD) + sizeof(DWORD));
		db_clientdesc->Packet(&dwID, sizeof(DWORD));
		db_clientdesc->Packet(&dwOwnerID, sizeof(DWORD));

		sys_log(1, "ITEM_DELETE %s:%u", item->GetName(), dwID);
		return;
	}

	sys_log(1, "ITEM_SAVE %s:%d in %s window %d", item->GetName(), item->GetID(), owner->GetName(), item->GetWindow());

	TPlayerItem t;

	t.dwID = item->GetID();
	t.bWindow = item->GetWindow();
	t.wPos = item->GetCell();
	t.dwVnum = item->GetOriginalVnum();
	t.dwCount = item->GetCount();

	switch (t.bWindow)
	{
		case SAFEBOX:
		case MALL:
			t.dwOwner = owner->GetDesc()->GetAccountTable().id;
			break;
		default:
			t.dwOwner = owner->GetPlayerID();
			break;
	}

#if defined(__SOUL_BIND_SYSTEM__)
	t.lSealDate = item->GetSealDate();
#endif
	thecore_memcpy(t.alSockets, item->GetSockets(), sizeof(t.alSockets));
	thecore_memcpy(t.aAttr, item->GetAttributes(), sizeof(t.aAttr));
#if defined(__CHANGE_LOOK_SYSTEM__)
	t.dwTransmutationVnum = item->GetTransmutationVnum();
#endif
#if defined(__REFINE_ELEMENT_SYSTEM__)
	thecore_memcpy(&t.RefineElement, item->GetRefineElement(), sizeof(t.RefineElement));
#endif
#if defined(__ITEM_APPLY_RANDOM__)
	thecore_memcpy(t.aApplyRandom, item->GetRandomApplies(), sizeof(t.aApplyRandom));
#endif
#if defined(__SET_ITEM__)
	t.bSetValue = item->GetItemSetValue();
#endif

	db_clientdesc->DBPacketHeader(HEADER_GD_ITEM_SAVE, 0, sizeof(TPlayerItem));
	db_clientdesc->Packet(&t, sizeof(TPlayerItem));
}

void ITEM_MANAGER::Update()
{
	std::unordered_set<LPITEM>::iterator it = m_set_pkItemForDelayedSave.begin();
	std::unordered_set<LPITEM>::iterator this_it;

	while (it != m_set_pkItemForDelayedSave.end())
	{
		this_it = it++;
		LPITEM item = *this_it;

		if (NULL == item)
		{
			sys_err("NULL item, erasing from delayed save.");
			m_set_pkItemForDelayedSave.erase(item);
			continue;
		}

		if (FindByVID(item->GetVID()) == NULL)
		{
			sys_err("Invalid item, erasing from delayed save.");
			m_set_pkItemForDelayedSave.erase(item);
			continue;
		}

		// SLOW_QUERY ÇÃ·¡±×°¡ ÀÖ´Â °ÍÀº Á¾·á½Ã¿¡¸¸ ÀúÀåÇÑ´Ù.
		if (item->GetOwner() && IS_SET(item->GetFlag(), ITEM_FLAG_SLOW_QUERY))
			continue;

		SaveSingleItem(item);
		m_set_pkItemForDelayedSave.erase(this_it);
	}
}

void ITEM_MANAGER::RemoveItem(LPITEM item, const char* c_pszReason)
{
	if (!item)
		return;

	LPCHARACTER o;

	if ((o = item->GetOwner()))
	{
		char szHint[64];
		snprintf(szHint, sizeof(szHint), "%s %u ", item->GetName(), item->GetCount());
		LogManager::instance().ItemLog(o, item, c_pszReason ? c_pszReason : "REMOVE", szHint);

		// SAFEBOX_TIME_LIMIT_ITEM_BUG_FIX
		if (item->GetWindow() == MALL || item->GetWindow() == SAFEBOX)
		{
			// 20050613.ipkn.½Ã°£Á¦ ¾ÆÀÌÅÛÀÌ »óÁ¡¿¡ ÀÖÀ» °æ¿ì ½Ã°£¸¸·á½Ã ¼­¹ö°¡ ´Ù¿îµÈ´Ù.
			CSafebox* pSafebox = item->GetWindow() == MALL ? o->GetMall() : o->GetSafebox();
			if (pSafebox)
			{
				pSafebox->Remove(item->GetCell());
			}
		}
		// END_OF_SAFEBOX_TIME_LIMIT_ITEM_BUG_FIX
		else
		{
			if (item->GetWindow() == INVENTORY)
				o->SyncQuickslot(SLOT_TYPE_INVENTORY, item->GetCell(), WORD_MAX);
			else if (item->GetWindow() == BELT_INVENTORY)
				o->SyncQuickslot(SLOT_TYPE_BELT_INVENTORY, item->GetCell(), WORD_MAX);

			item->RemoveFromCharacter();
		}
	}

	M2_DESTROY_ITEM(item);
}

#ifndef DEBUG_ALLOC
void ITEM_MANAGER::DestroyItem(LPITEM item)
#else
void ITEM_MANAGER::DestroyItem(LPITEM item, const char* file, size_t line)
#endif
{
	if (!item)
		return;

	if (item->GetSectree())
		item->RemoveFromGround();

	if (item->GetOwner())
	{
		if (CHARACTER_MANAGER::instance().Find(item->GetOwner()->GetPlayerID()) != NULL)
		{
			sys_err("DestroyItem (%lu): GetOwner %s %s!!", item->GetID(), item->GetName(), item->GetOwner()->GetName());
			item->RemoveFromCharacter();
		}
		else
		{
			sys_err("DestroyItem (%lu): WTH! Invalid item owner. owner pointer : %p", item->GetID(), item->GetOwner());
		}
	}

	std::unordered_set<LPITEM>::iterator it = m_set_pkItemForDelayedSave.find(item);
	if (it != m_set_pkItemForDelayedSave.end())
		m_set_pkItemForDelayedSave.erase(it);

	DWORD dwID = item->GetID();
	sys_log(2, "ITEM_DESTROY %s:%u", item->GetName(), dwID);

	if (!item->GetSkipSave() && dwID)
	{
		DWORD dwOwnerID = item->GetLastOwnerPID();

		db_clientdesc->DBPacketHeader(HEADER_GD_ITEM_DESTROY, 0, sizeof(DWORD) + sizeof(DWORD));
		db_clientdesc->Packet(&dwID, sizeof(DWORD));
		db_clientdesc->Packet(&dwOwnerID, sizeof(DWORD));
	}
	else
	{
		sys_log(2, "ITEM_DESTROY_SKIP %s:%u (skip=%d)", item->GetName(), dwID, item->GetSkipSave());
	}

	if (dwID)
		m_map_pkItemByID.erase(dwID);

	m_VIDMap.erase(item->GetVID());

#ifndef DEBUG_ALLOC
	M2_DELETE(item);
	item = NULL;
#else
	M2_DELETE_EX(item, file, line);
#endif
}

LPITEM ITEM_MANAGER::Find(DWORD id)
{
	ItemMap::iterator it = m_map_pkItemByID.find(id);
	if (it == m_map_pkItemByID.end())
		return NULL;
	return it->second;
}

LPITEM ITEM_MANAGER::FindByVID(DWORD vid)
{
	ITEM_VID_MAP::iterator it = m_VIDMap.find(vid);

	if (it == m_VIDMap.end())
		return NULL;

	return (it->second);
}

TItemTable* ITEM_MANAGER::GetTable(DWORD vnum)
{
	int rnum = RealNumber(vnum);

	if (rnum < 0)
	{
		for (int i = 0; i < m_vec_item_vnum_range_info.size(); i++)
		{
			TItemTable* p = m_vec_item_vnum_range_info[i];
			if ((p->dwVnum < vnum) &&
				vnum < (p->dwVnum + p->dwVnumRange))
			{
				return p;
			}
		}

		return NULL;
	}

	return &m_vec_prototype[rnum];
}

int ITEM_MANAGER::RealNumber(DWORD vnum)
{
	int bot, top, mid;

	bot = 0;
	top = m_vec_prototype.size();

	TItemTable* pTable = &m_vec_prototype[0];

	while (1)
	{
		mid = (bot + top) >> 1;

		if ((pTable + mid)->dwVnum == vnum)
			return (mid);

		if (bot >= top)
			return (-1);

		if ((pTable + mid)->dwVnum > vnum)
			top = mid - 1;
		else
			bot = mid + 1;
	}
}

bool ITEM_MANAGER::GetVnum(const char* c_pszName, DWORD& r_dwVnum)
{
	int len = strlen(c_pszName);

	TItemTable* pTable = &m_vec_prototype[0];

	for (DWORD i = 0; i < m_vec_prototype.size(); ++i, ++pTable)
	{
		if (!strncasecmp(c_pszName, pTable->szLocaleName, len))
		{
			r_dwVnum = pTable->dwVnum;
			return true;
		}
	}

	return false;
}

bool ITEM_MANAGER::GetVnumByOriginalName(const char* c_pszName, DWORD& r_dwVnum)
{
	int len = strlen(c_pszName);

	TItemTable* pTable = &m_vec_prototype[0];

	for (DWORD i = 0; i < m_vec_prototype.size(); ++i, ++pTable)
	{
		if (!strncasecmp(c_pszName, pTable->szName, len))
		{
			r_dwVnum = pTable->dwVnum;
			return true;
		}
	}

	return false;
}

std::set<DWORD> g_set_lotto;

void load_lotto()
{
	static int bLoaded = false;

	if (bLoaded)
		return;

	bLoaded = true;
	FILE* fp = fopen("lotto.txt", "r");

	if (!fp)
		return;

	char buf[256];

	while (fgets(buf, 256, fp))
	{
		char* psz = strchr(buf, '\n');

		if (NULL != psz)
			*psz = '\0';

		DWORD dw = 0;
		str_to_number(dw, buf);
		g_set_lotto.insert(dw);
	}

	fclose(fp);
}

DWORD lotto()
{
	load_lotto();

	char szBuf[6 + 1];

	do
	{
		for (int i = 0; i < 6; ++i)
			szBuf[i] = 48 + number(1, 9);

		szBuf[6] = '\0';

		DWORD dw = 0;
		str_to_number(dw, szBuf);

		if (g_set_lotto.end() == g_set_lotto.find(dw))
		{
			FILE* fp = fopen("lotto.txt", "a+");
			if (fp)
			{
				fprintf(fp, "%u\n", dw);
				fclose(fp);
			}
			return dw;
		}
	} while (1);
}

class CItemDropInfo
{
public:
	CItemDropInfo(int iLevelStart, int iLevelEnd, int iPercent, DWORD dwVnum) :
		m_iLevelStart(iLevelStart), m_iLevelEnd(iLevelEnd), m_iPercent(iPercent), m_dwVnum(dwVnum)
	{
	}

	int m_iLevelStart;
	int m_iLevelEnd;
	int m_iPercent; // 1 ~ 1000
	DWORD m_dwVnum;

	friend bool operator < (const CItemDropInfo& l, const CItemDropInfo& r)
	{
		return l.m_iLevelEnd < r.m_iLevelEnd;
	}
};

extern std::vector<CItemDropInfo> g_vec_pkCommonDropItem[MOB_RANK_MAX_NUM];

// 20050503.ipkn.
// iMinimum º¸´Ù ÀÛÀ¸¸é iDefault ¼¼ÆÃ (´Ü, iMinimumÀº 0º¸´Ù Ä¿¾ßÇÔ)
// 1, 0 ½ÄÀ¸·Î ON/OFF µÇ´Â ¹æ½ÄÀ» Áö¿øÇÏ±â À§ÇØ Á¸Àç
int GetDropPerKillPct(int iMinimum, int iDefault, int iDeltaPercent, const char* c_pszFlag)
{
	int iVal = 0;

	if ((iVal = quest::CQuestManager::instance().GetEventFlag(c_pszFlag)))
	{
		if (!test_server)
		{
			if (iVal < iMinimum)
				iVal = iDefault;

			if (iVal < 0)
				iVal = iDefault;
		}
	}

	if (iVal == 0)
		return 0;

	// ±âº» ¼¼ÆÃÀÏ¶§ (iDeltaPercent=100) 
	// 40000 iVal ¸¶¸®´ç ÇÏ³ª ´À³¦À» ÁÖ±â À§ÇÑ »ó¼öÀÓ
	return (40000 * iDeltaPercent / iVal);
}

bool ITEM_MANAGER::GetDropPct(LPCHARACTER pkChr, LPCHARACTER pkKiller, OUT int& iDeltaPercent, OUT int& iRandRange)
{
	if (NULL == pkChr || NULL == pkKiller)
		return false;

	BYTE bRank = pkChr->GetMobRank();
	int iLevel = pkKiller->GetLevel();

	iDeltaPercent = 100;

	if (!pkChr->IsStone() && pkChr->GetMobRank() >= MOB_RANK_BOSS)
		iDeltaPercent = PERCENT_LVDELTA_BOSS(pkKiller->GetLevel(), pkChr->GetLevel());
	else
		iDeltaPercent = PERCENT_LVDELTA(pkKiller->GetLevel(), pkChr->GetLevel());

	// LEVEL_105_DROP_RATE
	if (iLevel >= 105)
		iDeltaPercent = PERCENT_LVDELTA_BOSS(105, pkChr->GetLevel());
	// END_LEVEL_105_DROP_RATE

	if (1 == number(1, 50000))
		iDeltaPercent += 1000;
	else if (1 == number(1, 10000))
		iDeltaPercent += 500;

	sys_log(3, "CreateDropItem for level: %d rank: %u pct: %d", iLevel, bRank, iDeltaPercent);
	iDeltaPercent = iDeltaPercent * CHARACTER_MANAGER::instance().GetMobItemRate(pkKiller) / 100;

	int iDropBonus = CPrivManager::instance().GetPriv(pkKiller, PRIV_ITEM_DROP);

	// ADD_PREMIUM
	if (pkKiller->GetPremiumRemainSeconds(PREMIUM_ITEM) > 0)
	{
		iDropBonus += 50;
		iDeltaPercent += (iDeltaPercent * 50) / 100;
	}

	if (pkKiller->GetPoint(POINT_MALL_ITEMBONUS) > 0)
	{
		iDropBonus += MIN(100, pkKiller->GetPoint(POINT_MALL_ITEMBONUS));
		iDeltaPercent += iDeltaPercent * pkKiller->GetPoint(POINT_MALL_ITEMBONUS) / 100;
	}
	// END_OF_ADD_PREMIUM

	// NOTE: Thief's Gloves now work like special rings.
	/*
	if (pkKiller->IsEquipUniqueGroup(UNIQUE_GROUP_DOUBLE_ITEM))
	{
		iDropBonus += 50;
		iDeltaPercent += (iDeltaPercent * 50) / 100;
	}
	*/

	if (pkKiller->GetPoint(POINT_ITEM_DROP_BONUS) > 0)
	{
		iDropBonus += MIN(100, pkKiller->GetPoint(POINT_ITEM_DROP_BONUS));
		iDeltaPercent += iDeltaPercent * pkKiller->GetPoint(POINT_ITEM_DROP_BONUS) / 100;
	}

	iRandRange = 4000000;
	iRandRange = iRandRange * 100 / (100 + iDropBonus);

	return true;
}

#if defined(__SEND_TARGET_INFO__)
void ITEM_MANAGER::GetMonsterItemDropMap(LPCHARACTER pkChr, LPCHARACTER pkKiller, MonsterItemDropMap& rItemDropMap, bool& bDropMetinStone)
{
	if (pkChr->IsPolymorphed() || pkChr->IsPC())
		return;

	int iLevel = pkKiller->GetLevel();
	BYTE bRank = pkChr->GetMobRank();

	bool bShowCommonDrop = true;
	bool bShowDropItemGroup = true;
	bool bShowMobItemGroup = true;
	bool bShowLevelItemGroup = true;
	bool bShowGloveItemGroup = true;
	bool bShowEtcItemDrop = true;
	bool bShowDropMetinStone = true;

	//////////////////////////////////////////////////////////////////////////////////////////////
	// CommonDropItem (common_drop_item.txt)
	if (bShowCommonDrop)
	{
		std::vector<CItemDropInfo>::iterator it = g_vec_pkCommonDropItem[bRank].begin();
		while (it != g_vec_pkCommonDropItem[bRank].end())
		{
			const CItemDropInfo& c_rInfo = *(it++);
			if (iLevel < c_rInfo.m_iLevelStart || iLevel > c_rInfo.m_iLevelEnd)
				continue;

			DWORD dwVnum = c_rInfo.m_dwVnum;
			TItemTable* table = GetTable(dwVnum);
			if (!table)
				continue;

			MonsterItemDropMap::iterator it = rItemDropMap.find(dwVnum);
			if (it == rItemDropMap.end())
				rItemDropMap.insert(std::make_pair(dwVnum, 1));
		}
	}
	// CommonDropItem
	//////////////////////////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////////////////////////
	// DropItemGroup
	if (bShowDropItemGroup)
	{
		pkDropItemGroupMap::const_iterator it = m_map_pkDropItemGroup.find(pkChr->GetRaceNum());
		if (it != m_map_pkDropItemGroup.end())
		{
			DropItemGroupInfoVector v = it->second->GetVector();
			for (DWORD i = 0; i < v.size(); ++i)
			{
				DWORD dwVnum = v[i].dwVnum;
				MonsterItemDropMap::iterator it = rItemDropMap.find(dwVnum);
				if (it == rItemDropMap.end())
					rItemDropMap.insert(std::make_pair(dwVnum, v[i].iCount));
			}
		}
	}
	// DropItemGroup
	//////////////////////////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////////////////////////
	// MobItemGroup
	if (bShowMobItemGroup)
	{
		pkMobItemGroupMap::const_iterator it = m_map_pkMobItemGroup.find(pkChr->GetRaceNum());
		if (it != m_map_pkMobItemGroup.end())
		{
			CMobItemGroup* pGroup = it->second;
			if (pGroup && !pGroup->IsEmpty())
			{
				const SMobItemGroupInfo& info = pGroup->GetOne();
				DWORD dwVnum = info.dwItemVnum;
				MonsterItemDropMap::iterator it = rItemDropMap.find(dwVnum);
				if (it == rItemDropMap.end())
					rItemDropMap.insert(std::make_pair(dwVnum, info.iCount));
			}
		}
	}
	// MobItemGroup
	//////////////////////////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////////////////////////
	// LevelItemGroup (mob_drop_item.txt) type(limit)
	if (bShowLevelItemGroup)
	{
		pkLevelItemGroupMap::const_iterator it = m_map_pkLevelItemGroup.find(pkChr->GetRaceNum());
		if (it != m_map_pkLevelItemGroup.end())
		{
			if (it->second->GetLevelLimit() <= static_cast<DWORD>(iLevel))
			{
				LevelItemGroupInfoVector v = it->second->GetVector();
				for (DWORD i = 0; i < v.size(); i++)
				{
					DWORD dwVnum = v[i].dwVNum;
					MonsterItemDropMap::iterator it = rItemDropMap.find(dwVnum);
					if (it == rItemDropMap.end())
						rItemDropMap.insert(std::make_pair(dwVnum, v[i].iCount));
				}
			}
		}
	}
	// LevelItemGroup
	//////////////////////////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////////////////////////
	// GloveItemGroup (mob_drop_item.txt) type(thiefgloves) - Drops only if you are using gloves.
	if (bShowGloveItemGroup)
	{
		if (pkKiller->GetPremiumRemainSeconds(PREMIUM_ITEM) > 0 ||
			pkKiller->IsEquipUniqueGroup(UNIQUE_GROUP_DOUBLE_ITEM))
		{
			pkGloveItemGroupMap::const_iterator it = m_map_pkGloveItemGroup.find(pkChr->GetRaceNum());
			if (it != m_map_pkGloveItemGroup.end())
			{
				ItemThiefGroupInfoVector v = it->second->GetVector();
				for (DWORD i = 0; i < v.size(); ++i)
				{
					DWORD dwVnum = v[i].dwVnum;
					MonsterItemDropMap::iterator it = rItemDropMap.find(dwVnum);
					if (it == rItemDropMap.end())
						rItemDropMap.insert(std::make_pair(dwVnum, v[i].iCount));
				}
			}
		}
	}
	// GloveItemGroup
	//////////////////////////////////////////////////////////////////////////////////////////////

	if (bShowEtcItemDrop && pkChr->GetMobDropItemVnum())
	{
		EtcItemDropProbMap::const_iterator it = m_map_dwEtcItemDropProb.find(pkChr->GetMobDropItemVnum());
		if (it != m_map_dwEtcItemDropProb.end())
		{
			MonsterItemDropMap::iterator it = rItemDropMap.find(pkChr->GetMobDropItemVnum());
			if (it == rItemDropMap.end())
				rItemDropMap.insert(std::make_pair(pkChr->GetMobDropItemVnum(), 1));
		}
	}

	if (bShowDropMetinStone && pkChr->IsStone())
	{
		const DWORD dwMetinStoneVnum = pkChr->GetDropMetinStoneVnum();
		if (dwMetinStoneVnum)
		{
			MonsterItemDropMap::iterator it = rItemDropMap.find(dwMetinStoneVnum);
			if (it == rItemDropMap.end())
			{
				//rItemDropMap.insert(std::make_pair(pkChr->GetDropMetinStoneVnum(), 1));
				bDropMetinStone = true;
			}
		}
	}
}

bool ITEM_MANAGER::CreateDropItemVector(LPCHARACTER pkChr, LPCHARACTER pkKiller, std::vector<LPITEM>& vec_item)
{
	if (pkChr->IsPolymorphed() || pkChr->IsPC())
	{
		return false;
	}

	int iLevel = pkKiller->GetLevel();

	BYTE bRank = pkChr->GetMobRank();
	LPITEM item = NULL;

	std::vector<CItemDropInfo>::iterator it = g_vec_pkCommonDropItem[bRank].begin();

	while (it != g_vec_pkCommonDropItem[bRank].end())
	{
		const CItemDropInfo& c_rInfo = *(it++);

		if (iLevel < c_rInfo.m_iLevelStart || iLevel > c_rInfo.m_iLevelEnd)
			continue;

		TItemTable* table = GetTable(c_rInfo.m_dwVnum);

		if (!table)
			continue;

		item = NULL;

		if (table->bType == ITEM_POLYMORPH)
		{
			if (c_rInfo.m_dwVnum == pkChr->GetPolymorphItemVnum())
			{
				item = CreateItem(c_rInfo.m_dwVnum, 1, 0, true);

				if (item)
					item->SetSocket(0, pkChr->GetRaceNum());
			}
		}
		else
			item = CreateItem(c_rInfo.m_dwVnum, 1, 0, true);

		if (item) vec_item.push_back(item);
	}

	// Drop Item Group
	{
		pkDropItemGroupMap::const_iterator it = m_map_pkDropItemGroup.find(pkChr->GetRaceNum());

		if (it != m_map_pkDropItemGroup.end())
		{
			DropItemGroupInfoVector v = it->second->GetVector();

			for (DWORD i = 0; i < v.size(); ++i)
			{
				item = CreateItem(v[i].dwVnum, v[i].iCount, 0, true);

				if (item)
				{
					if (item->GetType() == ITEM_POLYMORPH)
					{
						if (item->GetVnum() == pkChr->GetPolymorphItemVnum())
						{
							item->SetSocket(0, pkChr->GetRaceNum());
						}
					}

					vec_item.push_back(item);
				}
			}
		}
	}

	// MobDropItem Group
	{
		pkMobItemGroupMap::const_iterator it = m_map_pkMobItemGroup.find(pkChr->GetRaceNum());

		if (it != m_map_pkMobItemGroup.end())
		{
			CMobItemGroup* pGroup = it->second;

			// MOB_DROP_ITEM_BUG_FIX
			if (pGroup && !pGroup->IsEmpty())
			{
				const SMobItemGroupInfo& info = pGroup->GetOne();
				item = CreateItem(info.dwItemVnum, info.iCount, 0, true, info.iRarePct);

				if (item) vec_item.push_back(item);
			}
			// END_OF_MOB_DROP_ITEM_BUG_FIX
		}
	}

	// Level Item Group
	{
		pkLevelItemGroupMap::const_iterator it = m_map_pkLevelItemGroup.find(pkChr->GetRaceNum());

		if (it != m_map_pkLevelItemGroup.end())
		{
			if (it->second->GetLevelLimit() <= (DWORD)iLevel)
			{
				LevelItemGroupInfoVector v = it->second->GetVector();

				for (DWORD i = 0; i < v.size(); i++)
				{
					DWORD dwVnum = v[i].dwVNum;
					item = CreateItem(dwVnum, v[i].iCount, 0, true);
					if (item) vec_item.push_back(item);
				}
			}
		}
	}

	// BuyerTheifGloves Item Group
	{
		if (pkKiller->GetPremiumRemainSeconds(PREMIUM_ITEM) > 0 ||
			pkKiller->IsEquipUniqueGroup(UNIQUE_GROUP_DOUBLE_ITEM))
		{
			pkGloveItemGroupMap::const_iterator it = m_map_pkGloveItemGroup.find(pkChr->GetRaceNum());

			if (it != m_map_pkGloveItemGroup.end())
			{
				ItemThiefGroupInfoVector v = it->second->GetVector();

				for (DWORD i = 0; i < v.size(); ++i)
				{
					DWORD dwVnum = v[i].dwVnum;
					item = CreateItem(dwVnum, v[i].iCount, 0, true);
					if (item) vec_item.push_back(item);
				}
			}
		}
	}

	if (pkChr->GetMobDropItemVnum())
	{
		EtcItemDropProbMap::const_iterator it = m_map_dwEtcItemDropProb.find(pkChr->GetMobDropItemVnum());

		if (it != m_map_dwEtcItemDropProb.end())
		{
			item = CreateItem(pkChr->GetMobDropItemVnum(), 1, 0, true);
			if (item) vec_item.push_back(item);
		}
	}

	if (pkChr->IsStone())
	{
		if (pkChr->GetDropMetinStoneVnum())
		{
			item = CreateItem(pkChr->GetDropMetinStoneVnum(), 1, 0, true);
			if (item) vec_item.push_back(item);
		}
	}

	return vec_item.size();
}
#endif

bool ITEM_MANAGER::CreateDropItem(LPCHARACTER pkChr, LPCHARACTER pkKiller, std::vector<LPITEM>& vec_item)
{
	const int iLevel = pkKiller->GetLevel();

	int iDeltaPercent, iRandRange;
	if (!GetDropPct(pkChr, pkKiller, iDeltaPercent, iRandRange))
		return false;

	BYTE bRank = pkChr->GetMobRank();
	LPITEM item = NULL;

	// Common Drop Items
	std::vector<CItemDropInfo>::iterator it = g_vec_pkCommonDropItem[bRank].begin();

	while (it != g_vec_pkCommonDropItem[bRank].end())
	{
		const CItemDropInfo& c_rInfo = *(it++);

		if (iLevel < c_rInfo.m_iLevelStart || iLevel > c_rInfo.m_iLevelEnd)
			continue;

		int iPercent = (c_rInfo.m_iPercent * iDeltaPercent) / 100;
		sys_log(3, "CreateDropItem %d ~ %d %d(%d)", c_rInfo.m_iLevelStart, c_rInfo.m_iLevelEnd, c_rInfo.m_dwVnum, iPercent, c_rInfo.m_iPercent);

		if (iPercent >= number(1, iRandRange))
		{
			TItemTable* table = GetTable(c_rInfo.m_dwVnum);

			if (!table)
				continue;

			item = NULL;

			if (table->bType == ITEM_POLYMORPH)
			{
				if (c_rInfo.m_dwVnum == pkChr->GetPolymorphItemVnum())
				{
					item = CreateItem(c_rInfo.m_dwVnum, 1, 0, true);
					if (item)
						item->SetSocket(0, pkChr->GetRaceNum());
				}
			}
			else
				item = CreateItem(c_rInfo.m_dwVnum, 1, 0, true);

			if (item)
				vec_item.emplace_back(item);
		}
	}

	// Drop Item Group
	{
		const auto it = m_map_pkDropItemGroup.find(pkChr->GetRaceNum());
		if (it != m_map_pkDropItemGroup.end())
		{
			const auto v = it->second->GetVector();
			for (DWORD i = 0; i < v.size(); ++i)
			{
				const int iPercent = (v[i].dwPct * iDeltaPercent) / 100;
				if (iPercent >= number(1, iRandRange))
				{
					item = CreateItem(v[i].dwVnum, v[i].iCount, 0, true);
					if (item)
					{
						if (item->GetType() == ITEM_POLYMORPH)
						{
							if (item->GetVnum() == pkChr->GetPolymorphItemVnum())
								item->SetSocket(0, pkChr->GetRaceNum());
						}
						vec_item.emplace_back(item);
					}
				}
			}
		}
	}

	// MobDropItem Group
	{
		const auto it = m_map_pkMobItemGroup.find(pkChr->GetRaceNum());
		if (it != m_map_pkMobItemGroup.end())
		{
			const CMobItemGroup* pGroup = it->second;

			// MOB_DROP_ITEM_BUG_FIX
			// 20050805.myevan.MobDropItem ¿¡ ¾ÆÀÌÅÛÀÌ ¾øÀ» °æ¿ì CMobItemGroup::GetOne() Á¢±Ù½Ã ¹®Á¦ ¹ß»ý ¼öÁ¤
			if (pGroup && !pGroup->IsEmpty())
			{
				const int iPercent = 40000 * iDeltaPercent / pGroup->GetKillPerDrop();
				if (iPercent >= number(1, iRandRange))
				{
					const SMobItemGroupInfo& info = pGroup->GetOne();
					item = CreateItem(info.dwItemVnum, info.iCount, 0, true, info.iRarePct);
					if (item)
						vec_item.emplace_back(item);
				}
			}
			// END_OF_MOB_DROP_ITEM_BUG_FIX
		}
	}

	// Level Item Group
	{
		const auto it = m_map_pkLevelItemGroup.find(pkChr->GetRaceNum());
		if (it != m_map_pkLevelItemGroup.end())
		{
			if (it->second->GetLevelLimit() <= (DWORD)iLevel)
			{
				const auto v = it->second->GetVector();
				for (DWORD i = 0; i < v.size(); i++)
				{
					if (v[i].dwPct >= (DWORD)number(1, 1000000/*iRandRange*/))
					{
						const DWORD dwVnum = v[i].dwVNum;
						item = CreateItem(dwVnum, v[i].iCount, 0, true);
						if (item)
							vec_item.emplace_back(item);
					}
				}
			}
		}
	}

	// BuyerTheitGloves Item Group
	{
		if (pkKiller->GetPremiumRemainSeconds(PREMIUM_ITEM) > 0 ||
			pkKiller->IsEquipUniqueGroup(UNIQUE_GROUP_DOUBLE_ITEM))
		{
			const auto it = m_map_pkGloveItemGroup.find(pkChr->GetRaceNum());
			if (it != m_map_pkGloveItemGroup.end())
			{
				const auto v = it->second->GetVector();
				for (DWORD i = 0; i < v.size(); ++i)
				{
					const int iPercent = (v[i].dwPct * iDeltaPercent) / 100;
					if (iPercent >= number(1, iRandRange))
					{
						const DWORD dwVnum = v[i].dwVnum;
						item = CreateItem(dwVnum, v[i].iCount, 0, true);
						if (item)
							vec_item.emplace_back(item);
					}
				}
			}
		}
	}

	// ÀâÅÛ
	if (pkChr->GetMobDropItemVnum())
	{
		const auto it = m_map_dwEtcItemDropProb.find(pkChr->GetMobDropItemVnum());
		if (it != m_map_dwEtcItemDropProb.end())
		{
			const int iPercent = (it->second * iDeltaPercent) / 100;
			if (iPercent >= number(1, iRandRange))
			{
				item = CreateItem(pkChr->GetMobDropItemVnum(), 1, 0, true);
				if (item)
					vec_item.emplace_back(item);
			}
		}
	}

	if (pkChr->IsStone())
	{
		if (pkChr->GetDropMetinStoneVnum())
		{
			const int iPercent = (pkChr->GetDropMetinStonePct() * iDeltaPercent) * 400;
			if (iPercent >= number(1, iRandRange))
			{
				item = CreateItem(pkChr->GetDropMetinStoneVnum(), 1, 0, true);
				if (item)
					vec_item.emplace_back(item);
			}
		}
	}

	if (pkKiller->IsHorseRiding() &&
		GetDropPerKillPct(1000, 1000000, iDeltaPercent, "horse_skill_book_drop") >= number(1, iRandRange))
	{
		sys_log(0, "EVENT HORSE_SKILL_BOOK_DROP");

		if ((item = CreateItem(ITEM_HORSE_SKILL_TRAIN_BOOK, 1, 0, true)))
			vec_item.emplace_back(item);
	}

	// NEW_SPIRIT_STONES
	if (8051 <= pkChr->GetRaceNum() && pkChr->GetRaceNum() <= 8056)
	{
		int ar_ListStones[10][2] =
		{
			{ 28044, 15 }, // + 0
			{ 28144, 12 }, // + 1
			{ 28244, 11 }, // + 2
			{ 28344, 7 }, // + 3
			{ 28444, 2 }, // + 4

			{ 28045, 15 }, // + 0
			{ 28145, 12 }, // + 1
			{ 28245, 11 }, // + 2
			{ 28345, 7 }, // + 3
			{ 28445, 2 } // + 4
		};

		int c_random = number(0, 9);
		int iPercent = (ar_ListStones[c_random][1] * iDeltaPercent) * 400;

		if (iPercent >= number(1, iRandRange))
		{
			item = CreateItem(ar_ListStones[c_random][0], 1, 0, true);
			if (item)
				vec_item.push_back(item);
		}
	}
	// END_OF_NEW_SPIRIT_STONES

	if (GetDropPerKillPct(100, 1000, iDeltaPercent, "lotto_drop") >= number(1, iRandRange))
	{
		DWORD* pdw = M2_NEW DWORD[3];

		pdw[0] = 50001;
		pdw[1] = 1;
		pdw[2] = quest::CQuestManager::Instance().GetEventFlag("lotto_round");

		// Çà¿îÀÇ ¼­´Â ¼ÒÄÏÀ» ¼³Á¤ÇÑ´Ù
		DBManager::instance().ReturnQuery(QID_LOTTO, pkKiller->GetPlayerID(), pdw,
			"INSERT INTO lotto_list VALUES(0, 'server%s', %u, NOW())",
			get_table_postfix(), pkKiller->GetPlayerID());
	}

	//
	// ½ºÆä¼È µå·Ó ¾ÆÀÌÅÛ
	//
	CreateQuestDropItem(pkChr, pkKiller, vec_item, iDeltaPercent, iRandRange);

	for (auto it = vec_item.begin(); it != vec_item.end(); ++it)
	{
		LPITEM item = *it;
		DBManager::instance().SendMoneyLog(MONEY_LOG_DROP, item->GetVnum(), item->GetCount());
	}

	return vec_item.size();
}

// ADD_GRANDMASTER_SKILL
int GetThreeSkillLevelAdjust(int level)
{
	if (level < 40)
		return 32;
	if (level < 45)
		return 16;
	if (level < 50)
		return 8;
	if (level < 55)
		return 4;
	if (level < 60)
		return 2;
	return 1;
}
// END_OF_ADD_GRANDMASTER_SKILL

// DROPEVENT_CHARSTONE
// drop_char_stone 1
// drop_char_stone.percent_lv01_10 5
// drop_char_stone.percent_lv11_30 10
// drop_char_stone.percent_lv31_MX 15
// drop_char_stone.level_range 10
static struct DropEvent_CharStone
{
	int percent_lv01_10;
	int percent_lv11_30;
	int percent_lv31_MX;
	int level_range;
	bool alive;

	DropEvent_CharStone()
	{
		percent_lv01_10 = 100;
		percent_lv11_30 = 200;
		percent_lv31_MX = 300;
		level_range = 10;
		alive = false;
	}
} gs_dropEvent_charStone;

static int __DropEvent_CharStone_GetDropPercent(int killer_level)
{
	int killer_levelStep = (killer_level - 1) / 10;

	switch (killer_levelStep)
	{
		case 0:
			return gs_dropEvent_charStone.percent_lv01_10;

		case 1:
		case 2:
			return gs_dropEvent_charStone.percent_lv11_30;
	}

	return gs_dropEvent_charStone.percent_lv31_MX;
}

static void __DropEvent_CharStone_DropItem(CHARACTER& killer, CHARACTER& victim, ITEM_MANAGER& itemMgr, std::vector<LPITEM>& vec_item)
{
	if (!gs_dropEvent_charStone.alive)
		return;

	int killer_level = killer.GetLevel();
	int dropPercent = __DropEvent_CharStone_GetDropPercent(killer_level);

	int MaxRange = 10000;

	if (LC_IsCanada() == true)
		MaxRange = 20000;

	if (number(1, MaxRange) <= dropPercent)
	{
		int log_level = (test_server || killer.GetGMLevel() >= GM_LOW_WIZARD) ? 0 : 1;
		int victim_level = victim.GetLevel();
		int level_diff = victim_level - killer_level;

		if (level_diff >= +gs_dropEvent_charStone.level_range || level_diff <= -gs_dropEvent_charStone.level_range)
		{
			sys_log(log_level,
				"dropevent.drop_char_stone.level_range_over: killer(%s: lv%d), victim(%s: lv:%d), level_diff(%d)",
				killer.GetName(), killer.GetLevel(), victim.GetName(), victim.GetLevel(), level_diff);
			return;
		}

		static const int Stones[] = { 30210, 30211, 30212, 30213, 30214, 30215, 30216, 30217, 30218, 30219, 30258, 30259, 30260, 30261, 30262, 30263 };
		int item_vnum = Stones[number(0, _countof(Stones))];

		LPITEM p_item = NULL;

		if ((p_item = itemMgr.CreateItem(item_vnum, 1, 0, true)))
		{
			vec_item.push_back(p_item);

			sys_log(log_level,
				"dropevent.drop_char_stone.item_drop: killer(%s: lv%d), victim(%s: lv:%d), item_name(%s)",
				killer.GetName(), killer.GetLevel(), victim.GetName(), victim.GetLevel(), p_item->GetName());
		}
	}
}

bool DropEvent_CharStone_SetValue(const std::string& name, int value)
{
	if (name == "drop_char_stone")
	{
		gs_dropEvent_charStone.alive = value;

		if (value)
			sys_log(0, "dropevent.drop_char_stone = on");
		else
			sys_log(0, "dropevent.drop_char_stone = off");

	}
	else if (name == "drop_char_stone.percent_lv01_10")
		gs_dropEvent_charStone.percent_lv01_10 = value;
	else if (name == "drop_char_stone.percent_lv11_30")
		gs_dropEvent_charStone.percent_lv11_30 = value;
	else if (name == "drop_char_stone.percent_lv31_MX")
		gs_dropEvent_charStone.percent_lv31_MX = value;
	else if (name == "drop_char_stone.level_range")
		gs_dropEvent_charStone.level_range = value;
	else
		return false;

	sys_log(0, "dropevent.drop_char_stone: %d", gs_dropEvent_charStone.alive ? true : false);
	sys_log(0, "dropevent.drop_char_stone.percent_lv01_10: %f", gs_dropEvent_charStone.percent_lv01_10 / 100.0f);
	sys_log(0, "dropevent.drop_char_stone.percent_lv11_30: %f", gs_dropEvent_charStone.percent_lv11_30 / 100.0f);
	sys_log(0, "dropevent.drop_char_stone.percent_lv31_MX: %f", gs_dropEvent_charStone.percent_lv31_MX / 100.0f);
	sys_log(0, "dropevent.drop_char_stone.level_range: %d", gs_dropEvent_charStone.level_range);

	return true;
}

// END_OF_DROPEVENT_CHARSTONE

// fixme
// À§ÀÇ °Í°ú ÇÔ²² quest·Î »¬°Í »©º¸ÀÚ. 
// ÀÌ°Å ³Ê¹« ´õ·´ÀÝ¾Æ...
// ”?. ÇÏµåÄÚµù ½È´Ù ¤Ì¤Ð
// °è·® ¾ÆÀÌÅÛ º¸»ó ½ÃÀÛ.
// by rtsummit °íÄ¡ÀÚ ÁøÂ¥
static struct DropEvent_RefineBox
{
	int percent_low;
	int low;
	int percent_mid;
	int mid;
	int percent_high;
	// int level_range;
	bool alive;

	DropEvent_RefineBox()
	{
		percent_low = 100;
		low = 20;
		percent_mid = 100;
		mid = 45;
		percent_high = 100;
		// level_range = 10;
		alive = false;
	}
} gs_dropEvent_refineBox;

static LPITEM __DropEvent_RefineBox_GetDropItem(CHARACTER& killer, CHARACTER& victim, ITEM_MANAGER& itemMgr)
{
	static const int lowerBox[] = { 50197, 50198, 50199 };
	static const int lowerBox_range = 3;
	static const int midderBox[] = { 50203, 50204, 50205, 50206 };
	static const int midderBox_range = 4;
	static const int higherBox[] = { 50207, 50208, 50209, 50210, 50211 };
	static const int higherBox_range = 5;

	if (victim.GetMobRank() < MOB_RANK_KNIGHT)
		return NULL;

	int killer_level = killer.GetLevel();
	/*
	int level_diff = victim.GetLevel() - killer_level;
	if (level_diff >= +gs_dropEvent_refineBox.level_range || level_diff <= -gs_dropEvent_refineBox.level_range)
	{
		sys_log(log_level, "dropevent.drop_refine_box.level_range_over: killer(%s: lv%d), victim(%s: lv:%d), level_diff(%d)",
			killer.GetName(), killer.GetLevel(), victim.GetName(), victim.GetLevel(), level_diff);
		return NULL;
	}
	*/

	if (killer_level <= gs_dropEvent_refineBox.low)
	{
		if (number(1, gs_dropEvent_refineBox.percent_low) == 1)
		{
			return itemMgr.CreateItem(lowerBox[number(1, lowerBox_range) - 1], 1, 0, true);
		}
	}
	else if (killer_level <= gs_dropEvent_refineBox.mid)
	{
		if (number(1, gs_dropEvent_refineBox.percent_mid) == 1)
		{
			return itemMgr.CreateItem(midderBox[number(1, midderBox_range) - 1], 1, 0, true);
		}
	}
	else
	{
		if (number(1, gs_dropEvent_refineBox.percent_high) == 1)
		{
			return itemMgr.CreateItem(higherBox[number(1, higherBox_range) - 1], 1, 0, true);
		}
	}
	return NULL;
}

static void __DropEvent_RefineBox_DropItem(CHARACTER& killer, CHARACTER& victim, ITEM_MANAGER& itemMgr, std::vector<LPITEM>& vec_item)
{
	if (!gs_dropEvent_refineBox.alive)
		return;

	int log_level = (test_server || killer.GetGMLevel() >= GM_LOW_WIZARD) ? 0 : 1;

	LPITEM p_item = __DropEvent_RefineBox_GetDropItem(killer, victim, itemMgr);

	if (p_item)
	{
		vec_item.push_back(p_item);

		sys_log(log_level,
			"dropevent.drop_refine_box.item_drop: killer(%s: lv%d), victim(%s: lv:%d), item_name(%s)",
			killer.GetName(), killer.GetLevel(), victim.GetName(), victim.GetLevel(), p_item->GetName());
	}
}

bool DropEvent_RefineBox_SetValue(const std::string& name, int value)
{
	if (name == "refine_box_drop")
	{
		gs_dropEvent_refineBox.alive = value;

		if (value)
			sys_log(0, "refine_box_drop = on");
		else
			sys_log(0, "refine_box_drop = off");

	}
	else if (name == "refine_box_low")
		gs_dropEvent_refineBox.percent_low = value < 100 ? 100 : value;
	else if (name == "refine_box_mid")
		gs_dropEvent_refineBox.percent_mid = value < 100 ? 100 : value;
	else if (name == "refine_box_high")
		gs_dropEvent_refineBox.percent_high = value < 100 ? 100 : value;
	//else if (name == "refine_box_level_range")
	//	gs_dropEvent_refineBox.level_range = value;
	else
		return false;

	sys_log(0, "refine_box_drop: %d", gs_dropEvent_refineBox.alive ? true : false);
	sys_log(0, "refine_box_low: %d", gs_dropEvent_refineBox.percent_low);
	sys_log(0, "refine_box_mid: %d", gs_dropEvent_refineBox.percent_mid);
	sys_log(0, "refine_box_high: %d", gs_dropEvent_refineBox.percent_high);
	//sys_log(0, "refine_box_low_level_range: %d", gs_dropEvent_refineBox.level_range);

	return true;
}
// °³·® ¾ÆÀÌÅÛ º¸»ó ³¡.

void ITEM_MANAGER::CreateQuestDropItem(LPCHARACTER pkChr, LPCHARACTER pkKiller, std::vector<LPITEM>& vec_item, int iDeltaPercent, int iRandRange)
{
	LPITEM item = NULL;

	if (!pkChr)
		return;

	if (!pkKiller)
		return;

	sys_log(1, "CreateQuestDropItem victim(%s), killer(%s)", pkChr->GetName(), pkKiller->GetName());

	// DROPEVENT_CHARSTONE
	__DropEvent_CharStone_DropItem(*pkKiller, *pkChr, *this, vec_item);
	// END_OF_DROPEVENT_CHARSTONE
	__DropEvent_RefineBox_DropItem(*pkKiller, *pkChr, *this, vec_item);

#if defined(__XMAS_EVENT_2008__)
	// Å©¸®½º¸¶½º ¾ç¸»
	if (quest::CQuestManager::instance().GetEventFlag("xmas_sock"))
	{
		int iDropPerKill[MOB_RANK_MAX_NUM] =
		{
			2000,
			1000,
			300,
			50,
			0,
			0,
		};

		if (iDropPerKill[pkChr->GetMobRank()] != 0)
		{
			int iPercent = 40000 * iDeltaPercent / iDropPerKill[pkChr->GetMobRank()];

			if (LC_IsHongKong())
			{
				iPercent *= 10;
			}

			sys_log(0, "SOCK DROP %d %d", iPercent, iRandRange);
			if (iPercent >= number(1, iRandRange))
			{
				if ((item = CreateItem(ITEM_XMAS_SOCK, 1, 0, true)))
					vec_item.push_back(item);
			}
		}
	}
#endif

	// ¿ù±¤ º¸ÇÕ
	if (quest::CQuestManager::instance().GetEventFlag("drop_moon"))
	{
		const DWORD ITEM_VNUM = 50011;

		int iDropPerKill[MOB_RANK_MAX_NUM] =
		{
			2000,
			1000,
			300,
			50,
			0,
			0,
		};

		if (iDropPerKill[pkChr->GetMobRank()])
		{
			int iPercent = 40000 * iDeltaPercent / iDropPerKill[pkChr->GetMobRank()];

			if (iPercent >= number(1, iRandRange))
			{
				if ((item = CreateItem(ITEM_VNUM, 1, 0, true)))
					vec_item.push_back(item);
			}
		}
	}

	if (LC_IsEurope())
	{
		if (pkKiller->GetLevel() >= 15 && abs(pkKiller->GetLevel() - pkChr->GetLevel()) <= 5)
		{
			int pct = quest::CQuestManager::instance().GetEventFlag("hc_drop");

			if (pct > 0)
			{
				const DWORD ITEM_VNUM = 30178;

				if (number(1, 100) <= pct)
				{
					if ((item = CreateItem(ITEM_VNUM, 1, 0, true)))
						vec_item.push_back(item);
				}
			}
		}
	}

	// À°°¢º¸ÇÕ
	if (GetDropPerKillPct(100, g_iUseLocale ? 2000 : 800, iDeltaPercent, "2006_drop") >= number(1, iRandRange))
	{
		sys_log(0, "À°°¢º¸ÇÕ DROP EVENT ");

		const static DWORD dwVnum = 50037;

		if ((item = CreateItem(dwVnum, 1, 0, true)))
			vec_item.push_back(item);

	}

	// À°°¢º¸ÇÕ+
	if (GetDropPerKillPct(100, g_iUseLocale ? 2000 : 800, iDeltaPercent, "2007_drop") >= number(1, iRandRange))
	{
		sys_log(0, "À°°¢º¸ÇÕ DROP EVENT ");

		const static DWORD dwVnum = 50043;

		if ((item = CreateItem(dwVnum, 1, 0, true)))
			vec_item.push_back(item);
	}

	// »õÇØ ÆøÁ× ÀÌº¥Æ®
	if (GetDropPerKillPct(/* minimum */ 100, /* default */ 1000, iDeltaPercent, "newyear_fire") >= number(1, iRandRange))
	{
		// Áß±¹Àº ÆøÁ×, ÇÑ±¹ ÆØÀÌ
		const DWORD ITEM_VNUM_FIRE = g_iUseLocale ? 50107 : 50108;

		if ((item = CreateItem(ITEM_VNUM_FIRE, 1, 0, true)))
			vec_item.push_back(item);
	}

	// »õÇØ ´ëº¸¸§ ¿ø¼Ò ÀÌº¥Æ®
	if (GetDropPerKillPct(100, 500, iDeltaPercent, "newyear_moon") >= number(1, iRandRange))
	{
		sys_log(0, "EVENT NEWYEAR_MOON DROP");

		const static DWORD wonso_items[6] = { 50016, 50017, 50018, 50019, 50019, 50019, };
		DWORD dwVnum = wonso_items[number(0, 5)];

		if ((item = CreateItem(dwVnum, 1, 0, true)))
			vec_item.push_back(item);
	}

#if defined(__2016_VALENTINE_EVENT__)
	// ¹ß·»Å¸ÀÎ µ¥ÀÌ ÀÌº¥Æ®. OGEÀÇ ¿ä±¸¿¡ µû¶ó event ÃÖ¼Ò°ªÀ» 1·Î º¯°æ.(´Ù¸¥ ÀÌº¥Æ®´Â ÀÏ´Ü ±×´ë·Î µÒ.)
	if (GetDropPerKillPct(1, g_iUseLocale ? 2000 : 800, iDeltaPercent, "valentine_drop") >= number(1, iRandRange))
	{
		sys_log(0, "EVENT VALENTINE_DROP");

		const static DWORD valentine_items[2] = { 50024, 50025 };
		DWORD dwVnum = valentine_items[number(0, 1)];

		if ((item = CreateItem(dwVnum, 1, 0, true)))
			vec_item.push_back(item);
	}
#endif

	// ¾ÆÀÌ½ºÅ©¸² ÀÌº¥Æ®
	if (GetDropPerKillPct(100, g_iUseLocale ? 2000 : 800, iDeltaPercent, "icecream_drop") >= number(1, iRandRange))
	{
		const static DWORD icecream = 50123;

		if ((item = CreateItem(icecream, 1, 0, true)))
			vec_item.push_back(item);
	}

#if defined(__XMAS_EVENT_2012__)
	// new Å©¸®½º¸¶½º ÀÌº¥Æ®
	// 53002 : ¾Æ±â ¼ø·Ï ¼ÒÈ¯±Ç
	if ((pkKiller->CountSpecifyItem(MOB_XMAS_REINDEER) > 0) || (pkKiller->CountSpecifyItem(MOB_XMAS_REINDEER_MALL) > 0))
	{
		if (GetDropPerKillPct(50, 100, iDeltaPercent, "new_xmas_event") >= number(1, iRandRange))
			pkKiller->AutoGiveItem(ITEM_XMAS_SOCK, 1);
	}
#endif

#if defined(__HALLOWEEN_EVENT_2014__)
	if (GetDropPerKillPct(100, g_iUseLocale ? 2000 : 800, iDeltaPercent, "halloween_drop") >= number(1, iRandRange))
	{
		if (pkChr->GetMobRank() >= MOB_RANK_BOSS || pkChr->IsStone())
		{
			if ((item = CreateItem(30322, 1, 0, true)))
				vec_item.push_back(item);
		}
		else
		{
			if ((item = CreateItem(30321, 1, 0, true)))
				vec_item.push_back(item);
		}
	}
#endif

	if (GetDropPerKillPct(100, g_iUseLocale ? 2000 : 800, iDeltaPercent, "ramadan_drop") >= number(1, iRandRange))
	{
		const static DWORD ramadan_item = 30315;

		if ((item = CreateItem(ramadan_item, 1, 0, true)))
			vec_item.push_back(item);
	}

#if defined(__EASTER_EVENT__)
	if (GetDropPerKillPct(100, g_iUseLocale ? 2000 : 800, iDeltaPercent, "easter_drop") >= number(1, iRandRange))
	{
		const static DWORD easter_item_base = 50160;
		if ((item = CreateItem(easter_item_base + number(0, 19), 1, 0, true)))
			vec_item.push_back(item);
	}
#endif

	// ¿ùµåÄÅ ÀÌº¥Æ®
	if (GetDropPerKillPct(100, g_iUseLocale ? 2000 : 800, iDeltaPercent, "football_drop") >= number(1, iRandRange))
	{
		const static DWORD football_item = 50096;

		if ((item = CreateItem(football_item, 1, 0, true)))
			vec_item.push_back(item);
	}

	// È­ÀÌÆ® µ¥ÀÌ ÀÌº¥Æ®
	if (GetDropPerKillPct(100, g_iUseLocale ? 2000 : 800, iDeltaPercent, "whiteday_drop") >= number(1, iRandRange))
	{
		sys_log(0, "EVENT WHITEDAY_DROP");
		const static DWORD whiteday_items[2] = { ITEM_WHITEDAY_ROSE, ITEM_WHITEDAY_CANDY };
		DWORD dwVnum = whiteday_items[number(0, 1)];

		if ((item = CreateItem(dwVnum, 1, 0, true)))
			vec_item.push_back(item);
	}

	// ¾î¸°ÀÌ³¯ ¼ö¼ö²²³¢ »óÀÚ ÀÌº¥Æ®
	if (pkKiller->GetLevel() >= 50)
	{
		if (GetDropPerKillPct(100, 1000, iDeltaPercent, "kids_day_drop_high") >= number(1, iRandRange))
		{
			DWORD ITEM_QUIZ_BOX = 50034;

			if ((item = CreateItem(ITEM_QUIZ_BOX, 1, 0, true)))
				vec_item.push_back(item);
		}
	}
	else
	{
		if (GetDropPerKillPct(100, 1000, iDeltaPercent, "kids_day_drop") >= number(1, iRandRange))
		{
			DWORD ITEM_QUIZ_BOX = 50034;

			if ((item = CreateItem(ITEM_QUIZ_BOX, 1, 0, true)))
				vec_item.push_back(item);
		}
	}

	// ¿Ã¸²ÇÈ µå·Ó ÀÌº¥Æ®
	if (pkChr->GetLevel() >= 30 && GetDropPerKillPct(50, 100, iDeltaPercent, "medal_part_drop") >= number(1, iRandRange))
	{
		const static DWORD drop_items[] = { 30265, 30266, 30267, 30268, 30269 };
		int i = number(0, 4);
		item = CreateItem(drop_items[i]);
		if (item != NULL)
			vec_item.push_back(item);
	}

	// ADD_GRANDMASTER_SKILL
	// È¥¼® ¾ÆÀÌÅÛ µå·Ó
	if (pkChr->GetLevel() >= 40 && pkChr->GetMobRank() >= MOB_RANK_BOSS && GetDropPerKillPct(/* minimum */ 1, /* default */ 1000, iDeltaPercent, "three_skill_item") / GetThreeSkillLevelAdjust(pkChr->GetLevel()) >= number(1, iRandRange))
	{
		const DWORD ITEM_VNUM = 50513;

		if ((item = CreateItem(ITEM_VNUM, 1, 0, true)))
			vec_item.push_back(item);
	}
	// END_OF_ADD_GRANDMASTER_SKILL

	//
	// Á¾ÀÚ ¾ÆÀÌÅÛ drop
	//
	if (GetDropPerKillPct(100, 1000, iDeltaPercent, "dragon_boat_festival_drop") >= number(1, iRandRange))
	{
		const DWORD ITEM_SEED = 50085;

		if ((item = CreateItem(ITEM_SEED, 1, 0, true)))
			vec_item.push_back(item);
	}

	// ¹«½ÅÀÇ Ãàº¹¼­¿ë ¸¸³âÇÑÃ¶ drop
	if (pkKiller->GetLevel() >= 15 && quest::CQuestManager::instance().GetEventFlag("mars_drop"))
	{
		const DWORD ITEM_HANIRON = 70035;
		int iDropMultiply[MOB_RANK_MAX_NUM] =
		{
			50,
			30,
			5,
			1,
			0,
			0,
		};

		if (iDropMultiply[pkChr->GetMobRank()] &&
			GetDropPerKillPct(1000, 1500, iDeltaPercent, "mars_drop") >= number(1, iRandRange) * iDropMultiply[pkChr->GetMobRank()])
		{
			if ((item = CreateItem(ITEM_HANIRON, 1, 0, true)))
				vec_item.push_back(item);
		}
	}

#if defined(__MINI_GAME_RUMI__) && defined(__OKEY_EVENT_FLAG_RENEWAL__)
	if (GetDropPerKillPct(50, 100, iDeltaPercent, "mini_game_okey_drop") >= number(1, iRandRange))
		CMiniGameRumi::UpdateQuestFlag(pkKiller);
#endif

#if defined(__MINI_GAME_CATCH_KING__) && defined(__CATCH_KING_EVENT_FLAG_RENEWAL__)
	if (GetDropPerKillPct(50, 100, iDeltaPercent, "mini_game_catchking_drop") >= number(1, iRandRange))
		CMiniGameCatchKing::UpdateQuestFlag(pkKiller);
#endif

#if defined(__MINI_GAME_YUTNORI__) && defined(__YUTNORI_EVENT_FLAG_RENEWAL__)
	if (GetDropPerKillPct(50, 100, iDeltaPercent, "mini_game_yutnori_drop") >= number(1, iRandRange))
		CMiniGameYutnori::UpdateQuestFlag(pkKiller);
#endif

#if defined(__SUMMER_EVENT_ROULETTE__)
	if (pkKiller->FindAffect(AFFECT_LATE_SUMMER_EVENT_BUFF) || pkKiller->FindAffect(AFFECT_LATE_SUMMER_EVENT_PRIMIUM_BUFF))
	{
		int iDropMultiply[MOB_RANK_MAX_NUM] =
		{
			0, // MOB_RANK_PAWN,
			0, // MOB_RANK_S_PAWN,
			0, // MOB_RANK_KNIGHT,
			5, // MOB_RANK_S_KNIGHT,
			30, // MOB_RANK_BOSS,
			50, // MOB_RANK_KING,
		};

		if (pkKiller->FindAffect(AFFECT_LATE_SUMMER_EVENT_PRIMIUM_BUFF))
			iDropMultiply[pkChr->GetMobRank()] += 30;

		if (GetDropPerKillPct(1000, 1500, iDeltaPercent, "e_late_summer_drop") >= number(1, iRandRange) * iDropMultiply[pkChr->GetMobRank()])
		{
			CMiniGameRoulette::UpdateQuestFlag(pkKiller);
		}
	}
#endif

#if defined(__SNOWFLAKE_STICK_EVENT__)
	if (quest::CQuestManager::instance().GetEventFlag("snowflake_stick_event"))
	{
		if (GetDropPerKillPct(50, 100, iDeltaPercent, "snowflake_stick_drop") >= number(1, iRandRange))
			CSnowflakeStickEvent::Reward(pkKiller);
	}
#endif

#if defined(__FLOWER_EVENT__)
	if (GetDropPerKillPct(50, 100, iDeltaPercent, "e_flower_drop") >= number(1, iRandRange))
		CFlowerEvent::Reward(pkKiller);
#endif
}

DWORD ITEM_MANAGER::GetRefineFromVnum(DWORD dwVnum)
{
	auto it = m_map_ItemRefineFrom.find(dwVnum);
	if (it != m_map_ItemRefineFrom.end())
		return it->second;
	return 0;
}

const CSpecialItemGroup* ITEM_MANAGER::GetSpecialItemGroup(DWORD dwVnum)
{
	auto it = m_map_pkSpecialItemGroup.find(dwVnum);
	if (it != m_map_pkSpecialItemGroup.end())
	{
		return it->second;
	}
	return NULL;
}

const CSpecialAttrGroup* ITEM_MANAGER::GetSpecialAttrGroup(DWORD dwVnum)
{
	auto it = m_map_pkSpecialAttrGroup.find(dwVnum);
	if (it != m_map_pkSpecialAttrGroup.end())
	{
		return it->second;
	}
	return NULL;
}

DWORD ITEM_MANAGER::GetMaskVnum(DWORD dwVnum)
{
	TMapDW2DW::iterator it = m_map_new_to_ori.find(dwVnum);
	if (it != m_map_new_to_ori.end())
	{
		return it->second;
	}
	else
		return 0;
}

// pkNewItemÀ¸·Î ¸ðµç ¼Ó¼º°ú ¼ÒÄÏ °ªµéÀ» ¸ñ»çÇÏ´Â ÇÔ¼ö.
// ±âÁ¸¿¡ char_item.cpp ÆÄÀÏ¿¡ ÀÖ´ø ·ÎÄÃÇÔ¼öÀÎ TransformRefineItem ±×´ë·Î º¹»çÇÔ
void ITEM_MANAGER::CopyAllAttrTo(LPITEM pkOldItem, LPITEM pkNewItem)
{
#if defined(__SOUL_SYSTEM__)
	if (pkOldItem == nullptr || pkNewItem == nullptr)
		return;

	if (pkOldItem->GetType() == ITEM_SOUL)
		return;
#endif

	// ACCESSORY_REFINE
	if (pkOldItem->IsAccessoryForSocket())
	{
		for (int i = 0; i < METIN_SOCKET_MAX_NUM; ++i)
		{
			pkNewItem->SetSocket(i, pkOldItem->GetSocket(i));
		}
		//pkNewItem->StartAccessorySocketExpireEvent();
	}
	// END_OF_ACCESSORY_REFINE
	else
	{
		// ¿©±â¼­ ±úÁø¼®ÀÌ ÀÚµ¿ÀûÀ¸·Î Ã»¼Ò µÊ
		for (int i = 0; i < METIN_SOCKET_MAX_NUM; ++i)
		{
			if (!pkOldItem->GetSocket(i))
				break;
			else
				pkNewItem->SetSocket(i, 1);
		}

		// ¼ÒÄÏ ¼³Á¤
		int slot = 0;

		for (int i = 0; i < METIN_SOCKET_MAX_NUM; ++i)
		{
			long socket = pkOldItem->GetSocket(i);
			const int ITEM_BROKEN_METIN_VNUM = 28960; // ÀÌ°Ç ¹¹ ¶È°°Àº »ó¼ö°¡ 3±ºµ¥³ª ÀÖ³Ä... ÇÏ³ª·Î ÇØ³õÁö¤Ð¤Ð¤Ð ³ª´Â ÆÐ½º È«ÀÌ ÇÒ²¨ÀÓ
			if (socket > 2 && socket != ITEM_BROKEN_METIN_VNUM)
				pkNewItem->SetSocket(slot++, socket);
		}
	}

#if defined(__ITEM_SOCKET6__)
	for (int i = METIN_SOCKET_MAX_NUM; i < ITEM_SOCKET_MAX_NUM; ++i)
		pkNewItem->SetSocket(i, pkOldItem->GetSocket(i));
#endif

	// ¸ÅÁ÷ ¾ÆÀÌÅÛ ¼³Á¤
	pkOldItem->CopyAttributeTo(pkNewItem);

#if defined(__CHANGE_LOOK_SYSTEM__)
	pkNewItem->SetTransmutationVnum(pkOldItem->GetTransmutationVnum());
#endif

#if defined(__REFINE_ELEMENT_SYSTEM__)
	pkNewItem->SetRefineElement(pkOldItem->GetRefineElement());
#endif

#if defined(__SET_ITEM__)
	pkNewItem->SetItemSetValue(pkOldItem->GetItemSetValue());
#endif
}

#if defined(__LUCKY_BOX__)
CLuckyBoxGroup::CLuckyBoxGroup(int iPrice, BYTE bMaxTryCount) :
	m_iPrice(iPrice),
	m_bMaxTryCount(bMaxTryCount)
{}

int CLuckyBoxGroup::GetPrice() const
{
	return m_iPrice;
}

BYTE CLuckyBoxGroup::GetMaxTryCount() const
{
	return m_bMaxTryCount;
}

void CLuckyBoxGroup::AddItem(DWORD dwBoxVNum, DWORD dwVNum, BYTE bCount)
{
	m_ItemsVec[dwBoxVNum].push_back({ dwVNum, bCount });
}

bool CLuckyBoxGroup::ContainsItems(DWORD dwBoxVNum) const
{
	auto it = m_ItemsVec.find(dwBoxVNum);
	if (it != m_ItemsVec.end() && !it->second.empty())
		return true;
	return false;
}

const CLuckyBoxGroup::SLuckyBoxItemInfo& CLuckyBoxGroup::GetRandomItem(DWORD dwBoxVNum) const
{
	const std::size_t nIndex = number(0, GetItemCount(dwBoxVNum) - 1);
	return m_ItemsVec.at(dwBoxVNum).at(nIndex);
}

std::size_t CLuckyBoxGroup::GetItemCount(DWORD dwBoxVNum) const
{
	return (ContainsItems(dwBoxVNum) ? m_ItemsVec.at(dwBoxVNum).size() : 0);
}
#endif

#if defined(__GEM_SHOP__)
const CGemShopItemGroup* ITEM_MANAGER::GetGemShopItemGroup(const BYTE c_bRow)
{
	GemShopItemGroupMap::iterator it = m_map_pGemShopItemGroup.find(c_bRow);
	if (it != m_map_pGemShopItemGroup.end())
		return it->second;
	return nullptr;
}

BYTE ITEM_MANAGER::GetGemShopAddSlotItemCount(const BYTE c_bSlotIndex) const
{
	GemShopAddSlotItemGroupMap::const_iterator it = m_mapGemShopAddSlotItemGroup.find(c_bSlotIndex);
	if (it != m_mapGemShopAddSlotItemGroup.end())
		return it->second;
	return 0;
}
#endif
