/**
* Filename: GemShop.h
* Author: Owsap
**/

#pragma once

#if defined(__GEM_SHOP__)
#include "../../common/tables.h"

#define __GEM_SHOP_SAVE_EXPANSION__ // Saves the players expanded slots.

class CGemShop
{
public:
	CGemShop(const LPCHARACTER c_lpCh, const TGemShopTable* c_pTable);
	~CGemShop();

	void ServerProcess(const BYTE c_bSubHeader, const BYTE c_bSlotIndex = GEM_SHOP_SLOT_COUNT, const bool c_bEnable = false);
	void Update(const TGemShopTable* c_pTable);

	TGemShopTable& GetGemShopTable() { return m_GemShopTable; }

public:
	static void Open(const LPCHARACTER c_lpCh
#if defined(__CONQUEROR_LEVEL__)
		, const bool c_bSpecial
#endif
	);
	static void Create(const LPCHARACTER c_lpCh, const TGemShopTable* c_pTable);

	void Buy(const BYTE c_bSlotIndex);
	void AddSlot();

	// NOTE: @param c_bForce - Forces the refresh, reseting everything.
	void Refresh(const bool c_bForce = false);

private:
	LPCHARACTER m_lpCh;
	TGemShopTable m_GemShopTable;
#if defined(__CONQUEROR_LEVEL__)
	bool m_bIsSpecial;
#endif
};
#endif
