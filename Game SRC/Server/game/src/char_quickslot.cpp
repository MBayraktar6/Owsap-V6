#include "stdafx.h"
#include "constants.h"
#include "char.h"
#include "desc.h"
#include "desc_manager.h"
#include "packet.h"
#include "item.h"

/////////////////////////////////////////////////////////////////////////////
// QUICKSLOT HANDLING
/////////////////////////////////////////////////////////////////////////////
void CHARACTER::SyncQuickslot(BYTE bType, WORD wOldPos, WORD wNewPos) // wNewPos == WORD_MAX ¸é DELETE
{
	if (wOldPos == wNewPos)
		return;

	for (int i = 0; i < QUICKSLOT_MAX_NUM; ++i)
	{
		if (m_quickslot[i].type == bType && m_quickslot[i].pos == wOldPos)
		{
			if (wNewPos == WORD_MAX)
				DelQuickslot(i);
			else
			{
				TQuickslot slot;

				slot.type = bType;
				slot.pos = wNewPos;

				SetQuickslot(i, slot);
			}
		}
	}
}

bool CHARACTER::GetQuickslot(BYTE pos, TQuickslot** ppSlot)
{
	if (pos >= QUICKSLOT_MAX_NUM)
		return false;

	*ppSlot = &m_quickslot[pos];
	return true;
}

bool CHARACTER::SetQuickslot(BYTE pos, TQuickslot& rSlot)
{
	struct packet_quickslot_add pack_quickslot_add;

	if (pos >= QUICKSLOT_MAX_NUM)
		return false;

	if (rSlot.type >= SLOT_TYPE_MAX)
		return false;

	for (int i = 0; i < QUICKSLOT_MAX_NUM; ++i)
	{
		if (rSlot.type == 0)
			continue;

		if (m_quickslot[i].type == rSlot.type && m_quickslot[i].pos == rSlot.pos)
			DelQuickslot(i);
	}

	switch (rSlot.type)
	{
		case SLOT_TYPE_INVENTORY:
		{
			const TItemPos Cell(INVENTORY, rSlot.pos);
			if (false == Cell.IsInventoryPosition())
				return false;
		}
		break;

		case SLOT_TYPE_BELT_INVENTORY:
		{
			const TItemPos Cell(BELT_INVENTORY, rSlot.pos);
			if (false == Cell.IsBeltInventoryPosition())
				return false;
		}
		break;

		case SLOT_TYPE_SKILL:
		{
			if (rSlot.pos >= SKILL_MAX_NUM)
				return false;
		}
		break;

		case SLOT_TYPE_EMOTION:
			break;

		default:
			return false;
	}

	m_quickslot[pos] = rSlot;

	if (GetDesc())
	{
		pack_quickslot_add.header = HEADER_GC_QUICKSLOT_ADD;
		pack_quickslot_add.pos = pos;
		pack_quickslot_add.slot = m_quickslot[pos];

		GetDesc()->Packet(&pack_quickslot_add, sizeof(pack_quickslot_add));
	}

	return true;
}

bool CHARACTER::DelQuickslot(BYTE pos)
{
	struct packet_quickslot_del pack_quickslot_del;

	if (pos >= QUICKSLOT_MAX_NUM)
		return false;

	memset(&m_quickslot[pos], 0, sizeof(TQuickslot));

	pack_quickslot_del.header = HEADER_GC_QUICKSLOT_DEL;
	pack_quickslot_del.pos = pos;

	GetDesc()->Packet(&pack_quickslot_del, sizeof(pack_quickslot_del));
	return true;
}

bool CHARACTER::SwapQuickslot(BYTE a, BYTE b)
{
	if (a >= QUICKSLOT_MAX_NUM || b >= QUICKSLOT_MAX_NUM)
		return false;

	// Äü ½½·Ô ÀÚ¸®¸¦ ¼­·Î ¹Ù²Û´Ù.
	TQuickslot quickslot;
	quickslot = m_quickslot[a];

	m_quickslot[a] = m_quickslot[b];
	m_quickslot[b] = quickslot;

	struct packet_quickslot_swap pack_quickslot_swap;
	pack_quickslot_swap.header = HEADER_GC_QUICKSLOT_SWAP;
	pack_quickslot_swap.pos = a;
	pack_quickslot_swap.pos_to = b;

	GetDesc()->Packet(&pack_quickslot_swap, sizeof(pack_quickslot_swap));
	return true;
}

void CHARACTER::ChainQuickslotItem(LPITEM pItem, BYTE bType, WORD wOldPos)
{
#if defined(__DRAGON_SOUL_SYSTEM__)
	if (pItem->IsDragonSoul())
		return;
#endif

	for (int i = 0; i < QUICKSLOT_MAX_NUM; ++i)
	{
		if (m_quickslot[i].type == bType && m_quickslot[i].pos == wOldPos)
		{
			TQuickslot slot;
			slot.type = bType;
			slot.pos = pItem->GetCell();

			SetQuickslot(i, slot);
			break;
		}
	}
}

void CHARACTER::MoveQuickSlotItem(BYTE bOldType, WORD wOldPos, BYTE bNewType, WORD wNewPos)
{
	for (int i = 0; i < QUICKSLOT_MAX_NUM; ++i)
	{
		if (m_quickslot[i].type == bOldType && m_quickslot[i].pos == wOldPos)
		{
			TQuickslot slot{};
			slot.type = bNewType;
			slot.pos = wNewPos;

			SetQuickslot(i, slot);
			break;
		}
	}
}

void CHARACTER::CheckQuickSlotItems()
{
	for (int i = 0; i < QUICKSLOT_MAX_NUM; ++i)
	{
		const BYTE bSlotType = m_quickslot[i].type;
		const WORD wSlotPos = m_quickslot[i].pos;

		switch (bSlotType)
		{
			case SLOT_TYPE_INVENTORY:
			{
				const TItemPos Cell(INVENTORY, wSlotPos);
				if (false == CanAddToQuickSlot(GetItem(Cell)))
					DelQuickslot(i);
			}
			break;

			case SLOT_TYPE_BELT_INVENTORY:
			{
				const TItemPos Cell(BELT_INVENTORY, wSlotPos);
				if (false == CanAddToQuickSlot(GetItem(Cell)))
					DelQuickslot(i);
			}
			break;
		}
	}
}
bool CHARACTER::CanAddToQuickSlot(const LPITEM pItem)
{
	if (pItem == nullptr)
		return false;

	switch (pItem->GetType())
	{
		case ITEM_USE:
		case ITEM_QUEST:
			return true;
	}

	return false;
}
