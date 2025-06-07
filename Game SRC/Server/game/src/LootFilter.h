/*
* Filename: LootFilter.h
* Author: blackdragonx61 (aka. Mali)
*/

#pragma once
#include "../../common/tables.h"

class CLootFilter
{
public:
	CLootFilter();
	virtual ~CLootFilter();

	void SetLootFilterSettings(const BYTE(&settings)[ELootFilter::LOOT_FILTER_SETTINGS_MAX]);
	bool IsLootFilteredItem(DWORD dwVID) const;
	void InsertLootFilteredItem(DWORD dwVID);
	void ClearLootFilteredItems();
	bool CanPickUpItem(LPITEM item) const;

private:
	bool CheckTopicWeapon(LPITEM item) const;
	bool CheckTopicArmor(LPITEM item) const;
	bool CheckTopicHead(LPITEM item) const;
	bool CheckTopicCommon(LPITEM item) const;
	bool CheckTopicCostume(LPITEM item) const;
	bool CheckTopicDS(LPITEM item) const;
	bool CheckTopicUnique(LPITEM item) const;
	bool CheckTopicRefine(LPITEM item) const;
	bool CheckTopicPotion(LPITEM item) const;
	bool CheckTopicFishMining(LPITEM item) const;
	bool CheckTopicMountPet(LPITEM item) const;
	bool CheckTopicSkillBook(LPITEM item) const;
	bool CheckTopicEtc(LPITEM item) const;
	bool CheckTopicEvent(LPITEM item) const;

	BYTE m_LootFilterSettings[ELootFilter::LOOT_FILTER_SETTINGS_MAX];
	std::unordered_set<DWORD> m_LootFilteredItemsSet;
};
