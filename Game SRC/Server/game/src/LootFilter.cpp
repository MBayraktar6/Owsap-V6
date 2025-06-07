/*
* Filename: LootFilter.cpp
* Author: blackdragonx61 (aka. Mali)
*/

#include "stdafx.h"

#if defined(__LOOT_FILTER_SYSTEM__)
#include "LootFilter.h"
#include "affect.h"
#include "affect_flag.h"
#include "item.h"
#include "skill.h"
#include "unique_item.h"

CLootFilter::CLootFilter()
{
	memset(m_LootFilterSettings, 0, sizeof(m_LootFilterSettings));
}

CLootFilter::~CLootFilter()
{
}

void CLootFilter::SetLootFilterSettings(const BYTE(&settings)[ELootFilter::LOOT_FILTER_SETTINGS_MAX])
{
	ClearLootFilteredItems();
	memcpy(m_LootFilterSettings, settings, sizeof(m_LootFilterSettings));
}

bool CLootFilter::IsLootFilteredItem(DWORD dwVID) const
{
	return m_LootFilteredItemsSet.end() != m_LootFilteredItemsSet.find(dwVID);
}

void CLootFilter::InsertLootFilteredItem(DWORD dwVID)
{
	m_LootFilteredItemsSet.insert(dwVID);
}

void CLootFilter::ClearLootFilteredItems()
{
	m_LootFilteredItemsSet.clear();
}

bool CLootFilter::CanPickUpItem(LPITEM item) const
{
	const DWORD dwVnum = item->GetVnum();
	const BYTE bType = item->GetType();
	const BYTE bSubType = item->GetSubType();

	switch (bType)
	{
		case EItemTypes::ITEM_WEAPON:
			return CheckTopicWeapon(item);

		case EItemTypes::ITEM_ARMOR:
		{
			switch (bSubType)
			{
				case EArmorSubTypes::ARMOR_BODY:
					return CheckTopicArmor(item);

				case EArmorSubTypes::ARMOR_HEAD:
					return CheckTopicHead(item);

				default:
					return CheckTopicCommon(item);
			}
		}
		case EItemTypes::ITEM_BELT:
		case EItemTypes::ITEM_ROD:
		case EItemTypes::ITEM_PICK:
			return CheckTopicCommon(item);

		case EItemTypes::ITEM_COSTUME:
			return CheckTopicCostume(item);

		case EItemTypes::ITEM_DS:
			return CheckTopicDS(item);

		case EItemTypes::ITEM_UNIQUE:
			return CheckTopicUnique(item);

		case EItemTypes::ITEM_SKILLBOOK:
		case EItemTypes::ITEM_SKILLFORGET:
			return CheckTopicSkillBook(item);

		case EItemTypes::ITEM_MATERIAL:
		case EItemTypes::ITEM_METIN:
			return CheckTopicRefine(item);

		case EItemTypes::ITEM_USE:
		{
			switch (bSubType)
			{
				case EUseSubTypes::USE_POTION:
				case EUseSubTypes::USE_ABILITY_UP:
				case EUseSubTypes::USE_POTION_NODELAY:
				case EUseSubTypes::USE_POTION_CONTINUE:
					return CheckTopicPotion(item);

				case EUseSubTypes::USE_BAIT:
				case EUseSubTypes::USE_PUT_INTO_ACCESSORY_SOCKET:
					return CheckTopicFishMining(item);
			}
			break;
		}

		case EItemTypes::ITEM_GIFTBOX:
#if defined(__GACHA_SYSTEM__)
		case EItemTypes::ITEM_GACHA:
#endif
		case EItemTypes::ITEM_POLYMORPH:
			return CheckTopicEtc(item);

		case EItemTypes::ITEM_FISH:
			return CheckTopicFishMining(item);

		case EItemTypes::ITEM_RESOURCE:
		{
			switch (bSubType)
			{
				case EResourceSubTypes::RESOURCE_BLOOD_PEARL:
				case EResourceSubTypes::RESOURCE_BLUE_PEARL:
				case EResourceSubTypes::RESOURCE_WHITE_PEARL:
					return CheckTopicFishMining(item);
			}
			break;
		}
	}

	if (item->IsHairDye())
		return CheckTopicPotion(item);

	switch (dwVnum)
	{
		case 50304:
		case 50305:
		case 50306:
		case 50311:
		case 50312:
		case 50313:
		case 50314:
		case 50315:
		case 50316:
		case 50600:
		case 71100:
			return CheckTopicSkillBook(item);

		case 27987:
			return CheckTopicFishMining(item);

		default:
			return true;
	}
}

bool CLootFilter::CheckTopicWeapon(LPITEM item) const
{
	const BYTE bRefineLevel = static_cast<BYTE>(item->GetRefineLevel());
	const BYTE bWearingLevelLimit = static_cast<BYTE>(item->GetWearingLevelLimit());

	if (!m_LootFilterSettings[ELootFilter::WEAPON_ON_OFF])
		return false;

	if (bRefineLevel < m_LootFilterSettings[ELootFilter::WEAPON_REFINE_MIN])
		return false;

	if (bRefineLevel > m_LootFilterSettings[ELootFilter::WEAPON_REFINE_MAX])
		return false;

	if (bWearingLevelLimit < m_LootFilterSettings[ELootFilter::WEAPON_WEARING_LEVEL_MIN])
		return false;

	if (bWearingLevelLimit > m_LootFilterSettings[ELootFilter::WEAPON_WEARING_LEVEL_MAX])
		return false;

	if (m_LootFilterSettings[ELootFilter::WEAPON_SELECT_DATA_JOB_WARRIOR] && !IS_SET(item->GetAntiFlag(), ITEM_ANTIFLAG_WARRIOR))
		return true;

	if (m_LootFilterSettings[ELootFilter::WEAPON_SELECT_DATA_JOB_ASSASSIN] && !IS_SET(item->GetAntiFlag(), ITEM_ANTIFLAG_ASSASSIN))
		return true;

	if (m_LootFilterSettings[ELootFilter::WEAPON_SELECT_DATA_JOB_SURA] && !IS_SET(item->GetAntiFlag(), ITEM_ANTIFLAG_SURA))
		return true;

	if (m_LootFilterSettings[ELootFilter::WEAPON_SELECT_DATA_JOB_SHAMAN] && !IS_SET(item->GetAntiFlag(), ITEM_ANTIFLAG_SHAMAN))
		return true;

#if defined(__WOLFMAN_CHARACTER__)
	if (m_LootFilterSettings[ELootFilter::WEAPON_SELECT_DATA_JOB_LYCAN] && !IS_SET(item->GetAntiFlag(), ITEM_ANTIFLAG_WOLFMAN))
		return true;
#endif

	return false;
}

bool CLootFilter::CheckTopicArmor(LPITEM item) const
{
	const BYTE bRefineLevel = static_cast<BYTE>(item->GetRefineLevel());
	const BYTE bWearingLevelLimit = static_cast<BYTE>(item->GetWearingLevelLimit());

	if (!m_LootFilterSettings[ELootFilter::ARMOR_ON_OFF])
		return false;

	if (bRefineLevel < m_LootFilterSettings[ELootFilter::ARMOR_REFINE_MIN])
		return false;

	if (bRefineLevel > m_LootFilterSettings[ELootFilter::ARMOR_REFINE_MAX])
		return false;

	if (bWearingLevelLimit < m_LootFilterSettings[ELootFilter::ARMOR_WEARING_LEVEL_MIN])
		return false;

	if (bWearingLevelLimit > m_LootFilterSettings[ELootFilter::ARMOR_WEARING_LEVEL_MAX])
		return false;

	if (m_LootFilterSettings[ELootFilter::ARMOR_SELECT_DATA_JOB_WARRIOR] && !IS_SET(item->GetAntiFlag(), ITEM_ANTIFLAG_WARRIOR))
		return true;

	if (m_LootFilterSettings[ELootFilter::ARMOR_SELECT_DATA_JOB_ASSASSIN] && !IS_SET(item->GetAntiFlag(), ITEM_ANTIFLAG_ASSASSIN))
		return true;

	if (m_LootFilterSettings[ELootFilter::ARMOR_SELECT_DATA_JOB_SURA] && !IS_SET(item->GetAntiFlag(), ITEM_ANTIFLAG_SURA))
		return true;

	if (m_LootFilterSettings[ELootFilter::ARMOR_SELECT_DATA_JOB_SHAMAN] && !IS_SET(item->GetAntiFlag(), ITEM_ANTIFLAG_SHAMAN))
		return true;

#if defined(__WOLFMAN_CHARACTER__)
	if (m_LootFilterSettings[ELootFilter::ARMOR_SELECT_DATA_JOB_LYCAN] && !IS_SET(item->GetAntiFlag(), ITEM_ANTIFLAG_WOLFMAN))
		return true;
#endif

	return false;
}

bool CLootFilter::CheckTopicHead(LPITEM item) const
{
	const BYTE bRefineLevel = static_cast<BYTE>(item->GetRefineLevel());
	const BYTE bWearingLevelLimit = static_cast<BYTE>(item->GetWearingLevelLimit());

	if (!m_LootFilterSettings[ELootFilter::HEAD_ON_OFF])
		return false;

	if (bRefineLevel < m_LootFilterSettings[ELootFilter::HEAD_REFINE_MIN])
		return false;

	if (bRefineLevel > m_LootFilterSettings[ELootFilter::HEAD_REFINE_MAX])
		return false;

	if (bWearingLevelLimit < m_LootFilterSettings[ELootFilter::HEAD_WEARING_LEVEL_MIN])
		return false;

	if (bWearingLevelLimit > m_LootFilterSettings[ELootFilter::HEAD_WEARING_LEVEL_MAX])
		return false;

	if (m_LootFilterSettings[ELootFilter::HEAD_SELECT_DATA_JOB_WARRIOR] && !IS_SET(item->GetAntiFlag(), ITEM_ANTIFLAG_WARRIOR))
		return true;

	if (m_LootFilterSettings[ELootFilter::HEAD_SELECT_DATA_JOB_ASSASSIN] && !IS_SET(item->GetAntiFlag(), ITEM_ANTIFLAG_ASSASSIN))
		return true;

	if (m_LootFilterSettings[ELootFilter::HEAD_SELECT_DATA_JOB_SURA] && !IS_SET(item->GetAntiFlag(), ITEM_ANTIFLAG_SURA))
		return true;

	if (m_LootFilterSettings[ELootFilter::HEAD_SELECT_DATA_JOB_SHAMAN] && !IS_SET(item->GetAntiFlag(), ITEM_ANTIFLAG_SHAMAN))
		return true;

#if defined(__WOLFMAN_CHARACTER__)
	if (m_LootFilterSettings[ELootFilter::HEAD_SELECT_DATA_JOB_LYCAN] && !IS_SET(item->GetAntiFlag(), ITEM_ANTIFLAG_WOLFMAN))
		return true;
#endif

	return false;
}

bool CLootFilter::CheckTopicCommon(LPITEM item) const
{
	const BYTE bType = item->GetType();
	const BYTE bSubType = item->GetSubType();
	const BYTE bRefineLevel = static_cast<BYTE>(item->GetRefineLevel());
	const BYTE bWearingLevelLimit = static_cast<BYTE>(item->GetWearingLevelLimit());

	if (!m_LootFilterSettings[ELootFilter::COMMON_ON_OFF])
		return false;

	if (bRefineLevel < m_LootFilterSettings[ELootFilter::COMMON_REFINE_MIN])
		return false;

	if (bRefineLevel > m_LootFilterSettings[ELootFilter::COMMON_REFINE_MAX])
		return false;

	if (bWearingLevelLimit < m_LootFilterSettings[ELootFilter::COMMON_WEARING_LEVEL_MIN])
		return false;

	if (bWearingLevelLimit > m_LootFilterSettings[ELootFilter::COMMON_WEARING_LEVEL_MAX])
		return false;

	switch (bType)
	{
		case EItemTypes::ITEM_BELT:
			return m_LootFilterSettings[ELootFilter::COMMON_SELECT_DATA_BELT];
		case EItemTypes::ITEM_ROD:
			return m_LootFilterSettings[ELootFilter::COMMON_SELECT_DATA_FISHING_ROD];
		case EItemTypes::ITEM_PICK:
			return m_LootFilterSettings[ELootFilter::COMMON_SELECT_DATA_PICKAXE];
		case EItemTypes::ITEM_ARMOR:
		{
			switch (bSubType)
			{
				case EArmorSubTypes::ARMOR_FOOTS:
					return m_LootFilterSettings[ELootFilter::COMMON_SELECT_DATA_SHOE];
				case EArmorSubTypes::ARMOR_WRIST:
					return m_LootFilterSettings[ELootFilter::COMMON_SELECT_DATA_BRACELET];
				case EArmorSubTypes::ARMOR_NECK:
					return m_LootFilterSettings[ELootFilter::COMMON_SELECT_DATA_NECKLACE];
				case EArmorSubTypes::ARMOR_EAR:
					return m_LootFilterSettings[ELootFilter::COMMON_SELECT_DATA_EARRING];
				case EArmorSubTypes::ARMOR_SHIELD:
					return m_LootFilterSettings[ELootFilter::COMMON_SELECT_DATA_SHIELD];
#if defined(__PENDANT_SYSTEM__)
				case EArmorSubTypes::ARMOR_PENDANT:
					return m_LootFilterSettings[ELootFilter::COMMON_SELECT_DATA_TALISMAN];
#endif
#if defined(__GLOVE_SYSTEM__)
				case EArmorSubTypes::ARMOR_GLOVE:
					return m_LootFilterSettings[ELootFilter::COMMON_SELECT_DATA_GLOVE];
#endif
				default:
					return true;
			}
		}
		default:
			return true;
	}
}

bool CLootFilter::CheckTopicCostume(LPITEM item) const
{
	const BYTE bType = item->GetType();
	const BYTE bSubType = item->GetSubType();

	if (!m_LootFilterSettings[ELootFilter::COSTUME_ON_OFF])
		return false;

	switch (bType)
	{
		case EItemTypes::ITEM_COSTUME:
		{
			switch (bSubType)
			{
				case ECostumeSubTypes::COSTUME_BODY:
					return m_LootFilterSettings[ELootFilter::COSTUME_SELECT_DATA_ARMOR];
				case ECostumeSubTypes::COSTUME_HAIR:
					return m_LootFilterSettings[ELootFilter::COSTUME_SELECT_DATA_HAIR];
#if defined(__ACCE_COSTUME_SYSTEM__)
				case ECostumeSubTypes::COSTUME_ACCE:
					return m_LootFilterSettings[ELootFilter::COSTUME_SELECT_DATA_ACCE];
#endif
#if defined(__AURA_COSTUME_SYSTEM__)
				case ECostumeSubTypes::COSTUME_AURA:
					return m_LootFilterSettings[ELootFilter::COSTUME_SELECT_DATA_AURA];
#endif

				default:
					return true;
			}
		}
		default:
			return true;
	}
}

bool CLootFilter::CheckTopicDS(LPITEM item) const
{
	if (!m_LootFilterSettings[ELootFilter::DS_ON_OFF])
		return false;

	return true;
}

bool CLootFilter::CheckTopicUnique(LPITEM item) const
{
	if (!m_LootFilterSettings[ELootFilter::UNIQUE_ON_OFF])
		return false;

	return true;
}

bool CLootFilter::CheckTopicRefine(LPITEM item) const
{
	const BYTE bType = item->GetType();
	const BYTE bSubType = item->GetSubType();

	if (!m_LootFilterSettings[ELootFilter::REFINE_ON_OFF])
		return false;

	switch (bType)
	{
		case EItemTypes::ITEM_MATERIAL:
		{
			switch (bSubType)
			{
				case EMaterialSubTypes::MATERIAL_LEATHER:
					return m_LootFilterSettings[ELootFilter::REFINE_SELECT_DATA_MATERIAL];
				default:
					return m_LootFilterSettings[ELootFilter::REFINE_SELECT_DATA_ETC];
			}
		}
		case EItemTypes::ITEM_METIN:
			return m_LootFilterSettings[ELootFilter::REFINE_SELECT_DATA_STONE];
		default:
			return true;
	}
}

bool CLootFilter::CheckTopicPotion(LPITEM item) const
{
	if (!m_LootFilterSettings[ELootFilter::POTION_ON_OFF])
		return false;

	if (item->IsHairDye())
		return m_LootFilterSettings[ELootFilter::POTION_SELECT_DATA_HAIRDYE];

	return m_LootFilterSettings[ELootFilter::POTION_SELECT_DATA_ABILITY];
}

bool CLootFilter::CheckTopicFishMining(LPITEM item) const
{
	const BYTE bType = item->GetType();
	const BYTE bSubType = item->GetSubType();

	if (!m_LootFilterSettings[ELootFilter::FISH_MINING_ON_OFF])
		return false;

	switch (bType)
	{
		case EItemTypes::ITEM_USE:
			switch (bSubType)
			{
				case EUseSubTypes::USE_BAIT:
					return m_LootFilterSettings[ELootFilter::FISH_MINING_SELECT_DATA_FOOD];
				case EUseSubTypes::USE_PUT_INTO_ACCESSORY_SOCKET:
					return m_LootFilterSettings[ELootFilter::FISH_MINING_SELECT_DATA_STONE];
			}
			break;
	}

	return m_LootFilterSettings[ELootFilter::FISH_MINING_SELECT_DATA_ETC];
}

bool CLootFilter::CheckTopicMountPet(LPITEM item) const
{
	if (!m_LootFilterSettings[ELootFilter::MOUNT_PET_ON_OFF])
		return false;

	return true;
}

bool CLootFilter::CheckTopicSkillBook(LPITEM item) const
{
	//const DWORD dwVnum = item->GetVnum();
	const BYTE bType = item->GetType();

	if (!m_LootFilterSettings[ELootFilter::SKILL_BOOK_ON_OFF])
		return false;

	switch (bType)
	{
		case EItemTypes::ITEM_SKILLBOOK:
		case EItemTypes::ITEM_SKILLFORGET:
		{
			DWORD dwSkillVnum = 0;

			if (item->GetVnum() == ITEM_SKILLBOOK_VNUM || bType == EItemTypes::ITEM_SKILLFORGET)
				dwSkillVnum = item->GetSocket(0);
			else
				dwSkillVnum = item->GetValue(0);

			const CSkillProto* pkSk = CSkillManager::instance().Get(dwSkillVnum);

			if (!pkSk)
				return true;

			switch (pkSk->dwType)
			{
				case SKILL_BOOK_TYPE_WARRIOR:
					return m_LootFilterSettings[ELootFilter::SKILL_BOOK_SELECT_DATA_JOB_WARRIOR];
				case SKILL_BOOK_TYPE_ASSASSIN:
					return m_LootFilterSettings[ELootFilter::SKILL_BOOK_SELECT_DATA_JOB_ASSASSIN];
				case SKILL_BOOK_TYPE_SURA:
					return m_LootFilterSettings[ELootFilter::SKILL_BOOK_SELECT_DATA_JOB_SURA];
				case SKILL_BOOK_TYPE_SHAMAN:
					return m_LootFilterSettings[ELootFilter::SKILL_BOOK_SELECT_DATA_JOB_SHAMAN];
#if defined(__WOLFMAN_CHARACTER__)
				case SKILL_BOOK_TYPE_WOLFMAN:
					return m_LootFilterSettings[ELootFilter::SKILL_BOOK_SELECT_DATA_JOB_LYCAN];
#endif
				default:
					return true;
			}
		}
		default:
			return m_LootFilterSettings[ELootFilter::SKILL_BOOK_SELECT_DATA_JOB_PUBLIC];
	}
}

bool CLootFilter::CheckTopicEtc(LPITEM item) const
{
	const BYTE bType = item->GetType();

	if (!m_LootFilterSettings[ELootFilter::ETC_ON_OFF])
		return false;

	switch (bType)
	{
		case EItemTypes::ITEM_GIFTBOX:
#if defined(__GACHA_SYSTEM__)
		case EItemTypes::ITEM_GACHA:
#endif
			return m_LootFilterSettings[ELootFilter::ETC_SELECT_DATA_GIFTBOX];
		default:
			return m_LootFilterSettings[ELootFilter::ETC_SELECT_DATA_ETC];
	}

	return true;
}

bool CLootFilter::CheckTopicEvent(LPITEM item) const
{
	if (!m_LootFilterSettings[ELootFilter::EVENT_ON_OFF])
		return false;

	return true;
}
#endif
