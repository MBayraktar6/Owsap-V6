/**
* Filename: GemShop.cpp
* Author: Owsap
**/

#include "stdafx.h"

#if defined(__GEM_SHOP__)
#include "GemShop.h"
#include "char.h"
#include "buffer_manager.h"
#include "desc_client.h"
#include "questmanager.h"
#include "item.h"
#include "item_manager.h"

CGemShop::CGemShop(const LPCHARACTER c_lpCh, const TGemShopTable* c_pTable)
	: m_lpCh(c_lpCh)
{
	std::memset(&m_GemShopTable, 0, sizeof(m_GemShopTable));
#if defined(__CONQUEROR_LEVEL__)
	m_bIsSpecial = false;
#endif

	Update(c_pTable);
	ServerProcess(SUBHEADER_GEM_SHOP_OPEN);
}

CGemShop::~CGemShop()
{
	std::memset(&m_GemShopTable, 0, sizeof(m_GemShopTable));
#if defined(__CONQUEROR_LEVEL__)
	m_bIsSpecial = false;
#endif

	ServerProcess(SUBHEADER_GEM_SHOP_CLOSE);
	m_lpCh->SetGemShopLoading(false);
}

void CGemShop::Update(const TGemShopTable* c_pTable)
{
	if (c_pTable == nullptr)
	{
		Refresh(true);
		return;
	}

#if defined(__CONQUEROR_LEVEL__)
	m_bIsSpecial = c_pTable->bSpecial;
#endif

	if (c_pTable->dwPID == 0 || std::time(nullptr) > c_pTable->lRefreshTime)
	{
		Refresh(true);
		return;
	}

	TEMP_BUFFER TempBuf = {};

	TGemShopTable GemShopTable = {};
	GemShopTable.dwPID = c_pTable->dwPID;
#if defined(__CONQUEROR_LEVEL__)
	GemShopTable.bSpecial = c_pTable->bSpecial;
#endif
	GemShopTable.lRefreshTime = c_pTable->lRefreshTime;
	GemShopTable.bEnabledSlots = c_pTable->bEnabledSlots;
	for (BYTE bSlotIndex = 0; bSlotIndex < GEM_SHOP_SLOT_COUNT; bSlotIndex++)
	{
		GemShopTable.GemShopItem[bSlotIndex].bEnable = c_pTable->GemShopItem[bSlotIndex].bEnable;
		GemShopTable.GemShopItem[bSlotIndex].dwItemVnum = c_pTable->GemShopItem[bSlotIndex].dwItemVnum;
		GemShopTable.GemShopItem[bSlotIndex].bCount = c_pTable->GemShopItem[bSlotIndex].bCount;
		GemShopTable.GemShopItem[bSlotIndex].dwPrice = c_pTable->GemShopItem[bSlotIndex].dwPrice;
	}
	TempBuf.write(&GemShopTable, sizeof(GemShopTable));

	std::memcpy(&m_GemShopTable, c_pTable, sizeof(m_GemShopTable));

	TPacketGCGemShop Packet;
	Packet.bHeader = HEADER_GC_GEM_SHOP;
	Packet.wSize = sizeof(Packet) + TempBuf.size();
	if (TempBuf.size())
	{
		m_lpCh->GetDesc()->BufferedPacket(&Packet, sizeof(Packet));
		m_lpCh->GetDesc()->Packet(TempBuf.read_peek(), TempBuf.size());
	}
	else
		m_lpCh->GetDesc()->Packet(&Packet, sizeof(Packet));

	ServerProcess(SUBHEADER_GEM_SHOP_REFRESH);
}

void CGemShop::ServerProcess(const BYTE c_bSubHeader, const BYTE c_bSlotIndex, const bool c_bEnable)
{
	if (m_lpCh == nullptr)
		return;

	const LPDESC c_lpDesc = m_lpCh->GetDesc();
	if (c_lpDesc == nullptr)
		return;

	TPacketGCGemShopProcess SPacket(c_bSubHeader);
	SPacket.bSlotIndex = c_bSlotIndex;
	SPacket.bEnable = c_bEnable;
	c_lpDesc->Packet(&SPacket, sizeof(SPacket));
}

void CGemShop::Open(const LPCHARACTER c_lpCh
#if defined(__CONQUEROR_LEVEL__)
	, const bool c_bSpecial
#endif
)
{
	if (c_lpCh == nullptr)
		return;

	const LPDESC c_lpDesc = c_lpCh->GetDesc();
	if (c_lpDesc == nullptr)
		return;

	if (quest::CQuestManager::instance().GetEventFlag("gem_shop") <= 0)
	{
		c_lpCh->ChatPacket(CHAT_TYPE_INFO, LC_STRING("[Gaya] The Gaya Market is currently unavailable."));
		return;
	}

	if (c_lpCh->PreventTradeWindow(WND_ALL))
	{
		c_lpCh->ChatPacket(CHAT_TYPE_INFO, LC_STRING("[Gaya] You can only open the Gaya Market once you have closed the other windows."));
		return;
	}

	if (c_lpCh->IsGemShopLoading())
		return;

	c_lpCh->SetGemShopLoading(true);

	TGemShopLoad GemShopLoad;
	GemShopLoad.dwPID = c_lpCh->GetPlayerID();
#if defined(__CONQUEROR_LEVEL__)
	GemShopLoad.bSpecial = c_bSpecial;
#endif
	db_clientdesc->DBPacket(HEADER_GD_GEM_SHOP_LOAD, c_lpDesc->GetHandle(), &GemShopLoad, sizeof(GemShopLoad));
}

void CGemShop::Create(const LPCHARACTER c_lpCh, const TGemShopTable* c_pTable)
{
	if (c_lpCh == nullptr)
		return;

	if (c_lpCh->GetGemShop())
	{
		c_lpCh->ChatPacket(CHAT_TYPE_INFO, LC_STRING("[Gaya] The Gaya Market is already open."));
		return;
	}

	c_lpCh->SetGemShopLoading(false);
	c_lpCh->SetGemShop(new CGemShop(c_lpCh, c_pTable));
}

void CGemShop::Buy(const BYTE c_bSlotIndex)
{
	if (m_lpCh == nullptr)
		return;

	const LPDESC c_lpDesc = m_lpCh->GetDesc();
	if (c_lpDesc == nullptr)
		return;

	TGemShopTable GemShopTable = m_GemShopTable;
	if (std::time(nullptr) > GemShopTable.lRefreshTime)
	{
		sys_log(0, "CGemShop::Buy(c_bSlotIndex=%d): cannot buy item (index: %d pid: %d), time expired.",
			c_bSlotIndex, m_lpCh->GetPlayerID());

		m_lpCh->ChatPacket(CHAT_TYPE_INFO, LC_STRING("[Gaya] You cannot purchase this item."));
		return;
	}

	if (c_bSlotIndex > GEM_SHOP_SLOT_COUNT)
		return;

#if defined(__CONQUEROR_LEVEL__)
	if (m_bIsSpecial && c_bSlotIndex > GEM_SHOP_SPECIAL_SLOT_COUNT)
		return;
#endif

	TGemShopItem* pGemShopItem = &GemShopTable.GemShopItem[c_bSlotIndex];
	if (pGemShopItem == nullptr)
		return;

	if ((c_bSlotIndex + 1) > GemShopTable.bEnabledSlots)
	{
		m_lpCh->ChatPacket(CHAT_TYPE_INFO, LC_STRING("[Gaya] You cannot purchase this item."));
		return;
	}

	if (m_lpCh->GetGem() < pGemShopItem->dwPrice)
	{
		m_lpCh->ChatPacket(CHAT_TYPE_INFO, LC_STRING("[Gaya] You don't have enough Gaya."));
		return;
	}

	const LPITEM c_lpItem = ITEM_MANAGER::instance().CreateItem(pGemShopItem->dwItemVnum, pGemShopItem->bCount);
	if (c_lpItem == nullptr)
	{
		sys_err("CGemShop::Buy(c_bSlotIndex=%d): Failed to create item (vnum %d) (name: %s)",
			c_bSlotIndex, pGemShopItem->dwItemVnum, m_lpCh->GetName());
		return;
	}

#if defined(__DRAGON_SOUL_SYSTEM__)
	const int c_iSpace = c_lpItem->IsDragonSoul() ? m_lpCh->GetEmptyDragonSoulInventory(c_lpItem) : m_lpCh->GetEmptyInventory(c_lpItem->GetSize());
#else
	const int c_iSpace = m_lpCh->GetEmptyInventory(c_lpItem->GetSize());
#endif
	if (c_iSpace == -1)
	{
		m_lpCh->ChatPacket(CHAT_TYPE_INFO, "[Gaya] Your inventory is full.");
		return;
	}

	m_lpCh->PointChange(POINT_GEM, -pGemShopItem->dwPrice);

	m_lpCh->AutoGiveItem(c_lpItem);
	pGemShopItem->bEnable = false;

	db_clientdesc->DBPacket(HEADER_GD_GEM_SHOP_UPDATE, c_lpDesc->GetHandle(), &GemShopTable, sizeof(GemShopTable));
	ServerProcess(SUBHEADER_GEM_SHOP_BUY, c_bSlotIndex, false);
}

void CGemShop::AddSlot()
{
	if (m_lpCh == nullptr)
		return;

	const LPDESC c_lpDesc = m_lpCh->GetDesc();
	if (c_lpDesc == nullptr)
		return;

	TGemShopTable GemShopTable = m_GemShopTable;
	if (GemShopTable.bEnabledSlots >= GEM_SHOP_SLOT_COUNT)
	{
		m_lpCh->ChatPacket(CHAT_TYPE_INFO, LC_STRING("[Gaya] You cannot unlock any more slots on the Gaya Market."));
		return;
	}

	const SGemShopInfo* pGemShopInfo = &ITEM_MANAGER::instance().GetGemShopInfo();
	if (pGemShopInfo == nullptr)
		return;

	BYTE bOpenSlotItemCount = ITEM_MANAGER::Instance().GetGemShopAddSlotItemCount(GemShopTable.bEnabledSlots + 1);

	TItemTable* pItemTable = ITEM_MANAGER::instance().GetTable(pGemShopInfo->dwAddSlotItemVNum);
	if (!pItemTable)
	{
		m_lpCh->ChatPacket(CHAT_TYPE_INFO, LC_STRING("[Gaya] The Gaya Market is currently unavailable."));
		return;
	}

	if (m_lpCh->CountSpecifyItem(pGemShopInfo->dwAddSlotItemVNum) < bOpenSlotItemCount)
	{
		m_lpCh->ChatPacket(CHAT_TYPE_INFO, LC_STRING("[Gaya] You don't have enough %s. You need %d more to unlock an additional slot.",
			LC_ITEM(pItemTable->GetVNum()), bOpenSlotItemCount));
		return;
	}

	BYTE bRealSlotIndex = GemShopTable.bEnabledSlots - 1;
	BYTE bNextSlotIndex = bRealSlotIndex + 1;
	{
		TGemShopItem* pGemShopItem = &GemShopTable.GemShopItem[bNextSlotIndex];
		if (pGemShopItem == nullptr)
			return;

		pGemShopItem->bEnable = true;
		GemShopTable.bEnabledSlots += 1;
#if defined(__GEM_SHOP_SAVE_EXPANSION__)
		m_lpCh->SetQuestFlag("gem_system.expansion", m_lpCh->GetQuestFlag("gem_system.expansion") + 1);
#endif
	}

	m_lpCh->RemoveSpecifyItem(pGemShopInfo->dwAddSlotItemVNum, bOpenSlotItemCount);

	db_clientdesc->DBPacket(HEADER_GD_GEM_SHOP_UPDATE, c_lpDesc->GetHandle(), &GemShopTable, sizeof(GemShopTable));
	ServerProcess(SUBHEADER_GEM_SHOP_SLOT_ADD, bNextSlotIndex, true);
}

void CGemShop::Refresh(const bool c_bForce)
{
	if (m_lpCh == nullptr)
		return;

	const LPDESC c_lpDesc = m_lpCh->GetDesc();
	if (c_lpDesc == nullptr)
		return;

	const SGemShopInfo* pGemShopInfo = &ITEM_MANAGER::instance().GetGemShopInfo();
	if (pGemShopInfo == nullptr)
		return;

	if (c_bForce == false && m_lpCh->CountSpecifyItem(pGemShopInfo->dwRefreshItemVNum) <= 0)
	{
		m_lpCh->ChatPacket(CHAT_TYPE_INFO, LC_STRING("[Gaya] You don't have a Refresh Gaya Market item."));
		return;
	}

	TGemShopTable GemShopTable = m_GemShopTable;
	if (c_bForce == true)
	{
		GemShopTable.dwPID = m_lpCh->GetPlayerID();
#if defined(__CONQUEROR_LEVEL__)
		GemShopTable.bSpecial = m_bIsSpecial;
#endif
		GemShopTable.lRefreshTime = std::time(nullptr) + static_cast<long>(pGemShopInfo->dwRefreshTime);
#if defined(__GEM_SHOP_SAVE_EXPANSION__)
		BYTE bSlotExpansion = m_lpCh->GetQuestFlag("gem_system.expansion");
		if (bSlotExpansion > 0)
			GemShopTable.bEnabledSlots = MINMAX(pGemShopInfo->bDefaultOpenedSlots, pGemShopInfo->bDefaultOpenedSlots + bSlotExpansion, GEM_SHOP_SLOT_COUNT);
		else
			GemShopTable.bEnabledSlots = pGemShopInfo->bDefaultOpenedSlots;
#else
		GemShopTable.bEnabledSlots = pGemShopInfo->bDefaultOpenedSlots;
#endif
	}

	BYTE bSlotCount = GEM_SHOP_SLOT_COUNT;
#if defined(__CONQUEROR_LEVEL__)
	bSlotCount = (m_bIsSpecial ? GEM_SHOP_SPECIAL_SLOT_COUNT : GEM_SHOP_SLOT_COUNT);
#endif

	for (BYTE bSlotIndex = 0; bSlotIndex < bSlotCount; bSlotIndex++)
	{
		BYTE bRow = 0;
		if (bSlotIndex < 3)
		{
			bRow = 1;
#if defined(__CONQUEROR_LEVEL__)
			bRow = (m_bIsSpecial ? 3 : 1);
#endif
		}
		else if (bSlotIndex < 6)
			bRow = 2;
		else if (bSlotIndex < 9)
			bRow = 3;

		const CGemShopItemGroup* c_pGroup = ITEM_MANAGER::instance().GetGemShopItemGroup(bRow);
		if (!c_pGroup)
			break;

		if (c_pGroup->GetGroupSize() == 0)
			break;

		static std::random_device RandomDevice;
		static std::mt19937 Generate(RandomDevice());
		static std::uniform_real_distribution<> Distribute(0, c_pGroup->GetGroupSize());
		BYTE bRandomIndex = Distribute(Generate);

		GemShopTable.GemShopItem[bSlotIndex].bEnable = (((bSlotIndex + 1) > GemShopTable.bEnabledSlots) ? false : true);
		GemShopTable.GemShopItem[bSlotIndex].dwItemVnum = c_pGroup->GetVNum(bRandomIndex);
		GemShopTable.GemShopItem[bSlotIndex].bCount = c_pGroup->GetCount(bRandomIndex);
		GemShopTable.GemShopItem[bSlotIndex].dwPrice = c_pGroup->GetPrice(bRandomIndex);
	}
	if (c_bForce == false)
	{
		m_lpCh->ChatPacket(CHAT_TYPE_INFO, LC_STRING("[Gaya] The item list has been refreshed."));
		m_lpCh->RemoveSpecifyItem(pGemShopInfo->dwRefreshItemVNum, 1);
	}
	db_clientdesc->DBPacket(HEADER_GD_GEM_SHOP_UPDATE, c_lpDesc->GetHandle(), &GemShopTable, sizeof(GemShopTable));
}
#endif
