/*
* Filename: changelook.cpp
* Author: blackdragonx61 (aka. Mali)
*/

#include "stdafx.h"
#include "changelook.h"
#include "char.h"
#include "item.h"
#include "item_manager.h"
#include "unique_item.h"
#include "desc.h"
#include "packet.h"
#include "log.h"

#if defined(__CHANGE_LOOK_SYSTEM__)
CChangeLook::CChangeLook(const LPCHARACTER c_lpCh, const BYTE c_bType) :
	m_lpCh(c_lpCh),
	m_bType(static_cast<decltype(m_bType)>(c_bType)),
	m_aItem({}),
	m_lpPassItem(nullptr)
{
	m_lpCh->ChatPacket(CHAT_TYPE_COMMAND, "ShowChangeDialog %d", c_bType);
}

CChangeLook::~CChangeLook()
{
}

/* static */ void CChangeLook::Open(const LPCHARACTER c_lpCh, const BYTE c_bType)
{
	if (c_lpCh == nullptr)
		return;

	if (c_lpCh->PreventTradeWindow(WND_ALL))
	{
		c_lpCh->ChatPacket(CHAT_TYPE_INFO, LC_STRING("You have to close other windows."));
		return;
	}

	c_lpCh->SetChangeLook(new CChangeLook(c_lpCh, c_bType));
}

void CChangeLook::ItemCheckIn(const TItemPos& c_rItemPos, const BYTE c_bSlotIndex)
{
	if (c_bSlotIndex >= m_aItem.size())
	{
		sys_err("Unknown SlotIndex m_lpCh:%s, %d", m_lpCh->GetName(), c_bSlotIndex);
		return;
	}

	LPITEM& lp_rItem = m_aItem.at(c_bSlotIndex);
	if (lp_rItem != nullptr)
		return;

	if (c_rItemPos.IsDefaultInventoryPosition() == false)
		return;

	const LPITEM c_lpItem = m_lpCh->GetItem(c_rItemPos);
	if (c_lpItem == nullptr)
		return;

	if (c_lpItem->isLocked())
		return;

	switch (static_cast<EChangeLookSlots>(c_bSlotIndex))
	{
	case EChangeLookSlots::CHANGE_LOOK_SLOT_LEFT:
		if (CanAddItem(c_lpItem) == false)
			return;
		break;

	case EChangeLookSlots::CHANGE_LOOK_SLOT_RIGHT:
		if (CheckItem(c_lpItem) == false)
			return;
		break;

	default:
		return;
	}

	packet_set_changelook sPacket =
	{
		HEADER_GC_CHANGE_LOOK_SET,
		c_rItemPos.cell,
		static_cast<BYTE>(c_bSlotIndex),
	};

	m_lpCh->GetDesc()->Packet(&sPacket, sizeof(sPacket));

	lp_rItem = c_lpItem;
}

void CChangeLook::ItemCheckOut(const BYTE c_bSlotIndex)
{
	if (c_bSlotIndex >= m_aItem.size())
	{
		sys_err("Unknown SlotIndex (m_lpCh:%s), %d", m_lpCh->GetName(), c_bSlotIndex);
		return;
	}

	LPITEM& lp_rItem = m_aItem.at(c_bSlotIndex);
	if (lp_rItem == nullptr)
		return;

	packet_changelook_del sPacket =
	{
		HEADER_GC_CHANGE_LOOK_DEL,
		lp_rItem->GetCell(),
		static_cast<BYTE>(c_bSlotIndex)
	};

	m_lpCh->GetDesc()->Packet(&sPacket, sizeof(sPacket));

	lp_rItem = nullptr;
}

void CChangeLook::FreeItemCheckIn(const TItemPos& c_rItemPos)
{
	if (m_lpPassItem != nullptr)
		return;

	if (c_rItemPos.IsDefaultInventoryPosition() == false)
		return;

	const LPITEM c_lpItem = m_lpCh->GetItem(c_rItemPos);
	if (c_lpItem == nullptr)
		return;

	if (c_lpItem->isLocked())
		return;

	if (CanAddPassItem(c_lpItem) == false)
		return;

	packet_set_changelook sPacket =
	{
		HEADER_GC_CHANGE_LOOK_FREE_SET,
		c_rItemPos.cell,
		0,
	};

	m_lpCh->GetDesc()->Packet(&sPacket, sizeof(sPacket));

	m_lpPassItem = c_lpItem;
}

void CChangeLook::FreeItemCheckOut()
{
	if (m_lpPassItem == nullptr)
		return;

	packet_changelook_del sPacket =
	{
		HEADER_GC_CHANGE_LOOK_FREE_DEL,
		m_lpPassItem->GetCell(),
		0
	};

	m_lpCh->GetDesc()->Packet(&sPacket, sizeof(sPacket));

	m_lpPassItem = nullptr;
}

void CChangeLook::Accept()
{
	const LPITEM c_lpLeftItem = GetLeftItem();
	const LPITEM c_lpRightItem = GetRightItem();
	const LPITEM c_lpPassItem = GetPassItem();

	if (c_lpLeftItem == nullptr || c_lpRightItem == nullptr)
		return;

	DWORD dwPrice = GetPrice();
	if (m_lpCh->GetGold() < dwPrice)
	{
		m_lpCh->ChatPacket(CHAT_TYPE_INFO, LC_STRING("[Transmutation] You don't have enough yang."));
		return;
	}

	if (dwPrice)
		m_lpCh->PointChange(POINT_GOLD, -dwPrice);

	c_lpLeftItem->SetTransmutationVnum(c_lpRightItem->GetVnum());
	c_lpLeftItem->UpdatePacket();

	// Clear Slots
	for (BYTE bSlot = 0; bSlot < m_aItem.size(); bSlot++)
		ItemCheckOut(bSlot);

	FreeItemCheckOut();

	// Remove
	ITEM_MANAGER::instance().RemoveItem(c_lpRightItem, "REMOVE (ChangeLook - Item)");
	if (c_lpPassItem != nullptr)
	{
		//ITEM_MANAGER::instance().RemoveItem(c_lpPassItem, "REMOVE (ChangeLook - Pass Item)");
		LogManager::instance().ItemLog(m_lpCh, c_lpPassItem, "REMOVE (ChangeLook - Pass Item)", c_lpPassItem->GetName());
		c_lpPassItem->SetCount(c_lpPassItem->GetCount() - 1);
	}
}

void CChangeLook::Clear()
{
	// Clear Slots
	for (BYTE bSlot = 0; bSlot < m_aItem.size(); bSlot++)
		ItemCheckOut(bSlot);
}

bool CChangeLook::CanAddItem(const LPITEM c_lpItem) const
{
	const BYTE c_bItemType = c_lpItem->GetType();
	const BYTE c_bItemSubType = c_lpItem->GetSubType();

	switch (m_bType)
	{
	case EChangeLookType::CHANGE_LOOK_TYPE_ITEM:
		if (c_lpItem->IsMainWeapon()
#if defined(__WEAPON_COSTUME_SYSTEM__)
			|| c_lpItem->IsCostumeWeapon()
#endif
			) return true;

		if (c_lpItem->IsArmorBody()
#if defined(__COSTUME_SYSTEM__)
			|| c_lpItem->IsCostumeBody()
#endif
			) return true;

		break;

	case EChangeLookType::CHANGE_LOOK_TYPE_MOUNT:
#if defined(__MOUNT_COSTUME_SYSTEM__)
		if (c_lpItem->IsCostumeMount())
			return true;
#endif

		break;
	}

	return false;
}

bool CChangeLook::CheckItem(const LPITEM c_lpRightItem) const
{
	const LPITEM c_lpLeftItem = GetLeftItem();
	if (c_lpLeftItem == nullptr)
		return false;

	if (c_lpLeftItem == c_lpRightItem)
		return false;

	if (c_lpLeftItem->GetVnum() == c_lpRightItem->GetVnum())
		return false;

#if defined(__COSTUME_SYSTEM__)
	if (c_lpRightItem->GetType() != c_lpLeftItem->GetType())
	{
		bool bCanPass = false;
		if ((c_lpRightItem->IsCostumeBody() && c_lpLeftItem->IsArmorBody()) ||
			(c_lpRightItem->IsArmorBody() && c_lpLeftItem->IsCostumeBody()))
			bCanPass = true;

#	if defined(__WEAPON_COSTUME_SYSTEM__)
		if ((c_lpRightItem->IsCostumeWeapon() && c_lpLeftItem->IsMainWeapon() && c_lpRightItem->GetValue(3) == c_lpLeftItem->GetSubType()) ||
			(c_lpRightItem->IsMainWeapon() && c_lpLeftItem->IsCostumeWeapon() && c_lpRightItem->GetSubType() == c_lpLeftItem->GetValue(3)))
			bCanPass = true;
#	endif

		return bCanPass;
	}
	else
#endif
	{
		if (c_lpRightItem->GetType() != c_lpLeftItem->GetType())
			return false;

		if (c_lpRightItem->GetSubType() != c_lpLeftItem->GetSubType())
			return false;

		if (c_lpLeftItem->IsArmor() || c_lpLeftItem->IsMainWeapon())
			if (c_lpRightItem->GetAntiFlag() != c_lpLeftItem->GetAntiFlag())
				return false;

#if defined(__COSTUME_SYSTEM__)
		if (c_lpRightItem->IsCostumeBody() == c_lpLeftItem->IsCostumeBody())
		{
			if ((IS_SET(c_lpRightItem->GetAntiFlag(), ITEM_ANTIFLAG_FEMALE) && IS_SET(c_lpLeftItem->GetAntiFlag(), ITEM_ANTIFLAG_MALE)) ||
				(IS_SET(c_lpRightItem->GetAntiFlag(), ITEM_ANTIFLAG_MALE) && IS_SET(c_lpLeftItem->GetAntiFlag(), ITEM_ANTIFLAG_FEMALE)))
				return false;
		}
#endif

#if defined(__WEAPON_COSTUME_SYSTEM__)
		if (c_lpLeftItem->IsCostumeWeapon())
			if ((c_lpLeftItem->GetValue(3) != c_lpRightItem->GetValue(3)))
				return false;
#endif
	}

	return true;
}

bool CChangeLook::CanAddPassItem(const LPITEM c_lpItem) const
{
	const DWORD c_dwVnum = c_lpItem->GetVnum();
	if (IsTypeMount())
		return c_dwVnum == ITEM_CHANGE_LOOK_MOUNT_TICKET;
	else
		return c_dwVnum == ITEM_CHANGE_LOOK_TICKET_1 || c_dwVnum == ITEM_CHANGE_LOOK_TICKET_2;
}

bool CChangeLook::IsTypeItem() const
{
	return m_bType == EChangeLookType::CHANGE_LOOK_TYPE_ITEM;
}

bool CChangeLook::IsTypeMount() const
{
	return m_bType == EChangeLookType::CHANGE_LOOK_TYPE_MOUNT;
}

LPITEM CChangeLook::GetLeftItem() const
{
	return m_aItem.at(static_cast<size_t>(EChangeLookSlots::CHANGE_LOOK_SLOT_LEFT));
}

LPITEM CChangeLook::GetRightItem() const
{
	return m_aItem.at(static_cast<size_t>(EChangeLookSlots::CHANGE_LOOK_SLOT_RIGHT));
}

LPITEM CChangeLook::GetPassItem() const
{
	return m_lpPassItem;
}

DWORD CChangeLook::GetPrice() const
{
	if (GetPassItem() == nullptr)
	{
		if (IsTypeItem())
			return static_cast<DWORD>(EChangeLookPrice::CHANGE_LOOK_ITEM_PRICE);

		if (IsTypeMount())
			return static_cast<DWORD>(EChangeLookPrice::CHANGE_LOOK_MOUNT_PRICE);
	}
	return 0;
}
#endif
