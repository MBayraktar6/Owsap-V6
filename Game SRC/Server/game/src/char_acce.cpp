/**
* Filename: char_acce.cpp
* Author: Owsap
**/

#include "stdafx.h"

#if defined(__ACCE_COSTUME_SYSTEM__)
#include "char.h"
#include "item.h"
#include "desc.h"
#include "utils.h"
#include "refine.h"

enum EAcceWeaponAttackType : BYTE
{
	WEAPON_MELEE_ATTACK,
	WEAPON_MAGIC_ATTACK,
};

void CHARACTER::AcceRefineWindowOpen(const LPENTITY pEntity, BYTE bType)
{
	if (pEntity == nullptr)
	{
		sys_err("AcceRefineWindowOpen(pEntity=%p, bType=%d): NULL ENTITY CHAR %s",
			get_pointer(pEntity), GetName());
		return;
	}

	if (PreventTradeWindow(WND_ALL))
		return;

	int iDist = DISTANCE_APPROX(GetX() - pEntity->GetX(), GetY() - pEntity->GetY());
	if (iDist >= WINDOW_OPENER_MAX_DISTANCE)
		return;

	TPacketGCAcceRefine Packet;
	Packet.wSize = sizeof(TPacketGCAcceRefine) + sizeof(TSubPacketGCAcceRefineOpenClose);
	Packet.bSubHeader = ACCE_REFINE_SUBHEADER_GC_OPEN;

	TSubPacketGCAcceRefineOpenClose SubPacket;
	switch (bType)
	{
		case ACCE_SLOT_TYPE_COMBINE:
		case ACCE_SLOT_TYPE_ABSORB:
			SubPacket.bType = bType;
			break;

		case ACCE_SLOT_TYPE_MAX:
			return;
	}

	const LPDESC pDesc = GetDesc();
	if (pDesc == nullptr)
	{
		sys_err("AcceRefineWindowOpen(pEntity=%p, bType=%d): NULL DESC CHAR %s",
			get_pointer(pEntity), GetName());
		return;
	}

	TEMP_BUFFER TempBuffer;
	TempBuffer.write(&Packet, sizeof(TPacketGCAcceRefine));
	TempBuffer.write(&SubPacket, sizeof(TSubPacketGCAcceRefineOpenClose));

	if (m_pointsInstant.m_pAcceRefineWindowOpener == nullptr)
		m_pointsInstant.m_pAcceRefineWindowOpener = pEntity;

	m_bAcceRefineWindowType = bType;
	m_bAcceRefineWindowOpen = ACCE_SLOT_TYPE_MAX != m_bAcceRefineWindowType;

	pDesc->Packet(TempBuffer.read_peek(), TempBuffer.size());
}

void CHARACTER::AcceRefineWindowClose(bool bServerClose)
{
	const LPDESC pDesc = GetDesc();
	if (pDesc == nullptr)
	{
		sys_err("AcceRefineWindowClose(): NULL DESC CHAR %s", GetName());
		return;
	}

	m_pointsInstant.m_pAcceRefineWindowOpener = nullptr;
	m_bAcceRefineWindowType = ACCE_SLOT_TYPE_MAX;
	m_bAcceRefineWindowOpen = false;

	for (BYTE bSlotIndex = 0; bSlotIndex < ACCE_SLOT_MAX; ++bSlotIndex)
	{
		if (m_pAcceRefineWindowItemSlot[bSlotIndex] != NPOS)
		{
			const LPITEM pItem = GetItem(m_pAcceRefineWindowItemSlot[bSlotIndex]);
			if (pItem != nullptr)
				pItem->Lock(false);

			m_pAcceRefineWindowItemSlot[bSlotIndex] = NPOS;
		}
	}

	TPacketGCAcceRefine Packet;
	Packet.bSubHeader = ACCE_REFINE_SUBHEADER_GC_CLOSE;
	Packet.wSize = sizeof(TPacketGCAcceRefine) + sizeof(TSubPacketGCAcceRefineOpenClose);

	TSubPacketGCAcceRefineOpenClose SubPacket;
	SubPacket.bType = ACCE_SLOT_TYPE_MAX;
	SubPacket.bServerClose = bServerClose;

	TEMP_BUFFER TempBuffer;
	TempBuffer.write(&Packet, sizeof(TPacketGCAcceRefine));
	TempBuffer.write(&SubPacket, sizeof(TSubPacketGCAcceRefineOpenClose));
	pDesc->Packet(TempBuffer.read_peek(), TempBuffer.size());
}

bool CHARACTER::IsAcceRefineWindowCanRefine()
{
	if (!IsAcceRefineWindowOpen())
		return false;

	if (!CanHandleItem())
		return false;

	const LPENTITY pEntity = GetAcceRefineWindowOpener();
	if (pEntity == nullptr)
		return false;

	int iDist = DISTANCE_APPROX(GetX() - pEntity->GetX(), GetY() - pEntity->GetY());
	if (iDist >= WINDOW_OPENER_MAX_DISTANCE)
		return false;

	return true;
}

void CHARACTER::AcceRefineWindowCheckIn(BYTE bType, TItemPos SelectedPos, TItemPos AttachedPos)
{
	if (SelectedPos.window_type != ACCEREFINE || SelectedPos.cell >= ACCE_SLOT_RESULT)
		return;

	if (!AttachedPos.IsValidItemPosition() || AttachedPos.IsEquipPosition())
		return;

	if (m_bAcceRefineWindowType != bType)
		return;

	const LPDESC pDesc = GetDesc();
	if (pDesc == nullptr)
	{
		sys_err("AcceRefineWindowCheckIn(bType=%d): NULL DESC CHAR %s", bType, GetName());
		return;
	}

	if (!IsAcceRefineWindowOpen())
		return;

	if (!IsAcceRefineWindowCanRefine())
		if (!IsAcceRefineWindowOpen() || !GetAcceRefineWindowOpener())
			return;

	const LPITEM pAttachedItem = GetItem(AttachedPos);
	if (pAttachedItem == nullptr)
		return;

	if (pAttachedItem->isLocked() || pAttachedItem->IsExchanging() || pAttachedItem->IsEquipped())
		return;

#if defined(__SOUL_BIND_SYSTEM__)
	if (pAttachedItem->IsSealed())
	{
		ChatPacket(CHAT_TYPE_INFO, LC_STRING("You cannot use any soulbound items."));
		return;
	}
#endif

	if (IS_SET(pAttachedItem->GetAntiFlag(), ITEM_ANTIFLAG_ACCE))
	{
		ChatPacket(CHAT_TYPE_INFO, LC_STRING("This item cannot be absorbed."));
		return;
	}

	switch (bType)
	{
		case ACCE_SLOT_TYPE_COMBINE:
		{
			if (m_pAcceRefineWindowItemSlot[SelectedPos.cell] != NPOS)
				return;

			if (SelectedPos.cell == ACCE_SLOT_LEFT && !(pAttachedItem->IsCostumeAcce()))
				return;

			if (SelectedPos.cell == ACCE_SLOT_RIGHT && !(pAttachedItem->IsCostumeAcce()))
				return;

			if (pAttachedItem->GetSocket(ITEM_SOCKET_ACCE_DRAIN_VALUE) >= ACCE_MAX_DRAINRATE)
				return;

			pAttachedItem->Lock(true);
			m_pAcceRefineWindowItemSlot[SelectedPos.cell] = AttachedPos;

			TPacketGCAcceRefine Packet;
			Packet.wSize = sizeof(TPacketGCAcceRefine) + sizeof(TSubPacketGCAcceRefineSetItem);
			Packet.bSubHeader = ACCE_REFINE_SUBHEADER_GC_SET_ITEM;

			TSubPacketGCAcceRefineSetItem SubPacket;
			SubPacket.AttachedPos = AttachedPos;
			SubPacket.SelectedPos = SelectedPos;

			SubPacket.Item.dwVnum = pAttachedItem->GetVnum();
			SubPacket.Item.dwCount = pAttachedItem->GetCount();
			SubPacket.Item.dwFlags = pAttachedItem->GetFlag();
			SubPacket.Item.dwAntiFlags = pAttachedItem->GetAntiFlag();
			thecore_memcpy(SubPacket.Item.alSockets, pAttachedItem->GetSockets(), sizeof(SubPacket.Item.alSockets));
			thecore_memcpy(SubPacket.Item.aAttr, pAttachedItem->GetAttributes(), sizeof(SubPacket.Item.aAttr));
#if defined(__CHANGE_LOOK_SYSTEM__)
			SubPacket.Item.dwTransmutationVnum = pAttachedItem->GetTransmutationVnum();
#endif
#if defined(__REFINE_ELEMENT_SYSTEM__)
			thecore_memcpy(&SubPacket.Item.RefineElement, pAttachedItem->GetRefineElement(), sizeof(SubPacket.Item.RefineElement));
#endif
#if defined(__ITEM_APPLY_RANDOM__)
			thecore_memcpy(SubPacket.Item.aApplyRandom, pAttachedItem->GetRandomApplies(), sizeof(SubPacket.Item.aApplyRandom));
#endif
#if defined(__SET_ITEM__)
			SubPacket.Item.bSetValue = pAttachedItem->GetItemSetValue();
#endif

			TEMP_BUFFER TempBuffer;
			TempBuffer.write(&Packet, sizeof(TPacketGCAcceRefine));
			TempBuffer.write(&SubPacket, sizeof(TSubPacketGCAcceRefineSetItem));
			pDesc->Packet(TempBuffer.read_peek(), TempBuffer.size());

			if (NPOS != m_pAcceRefineWindowItemSlot[ACCE_SLOT_LEFT])
			{
				const LPITEM pLeftItem = GetItem(m_pAcceRefineWindowItemSlot[ACCE_SLOT_LEFT]);
				if (!pLeftItem || !IsValidItemPosition(m_pAcceRefineWindowItemSlot[ACCE_SLOT_LEFT]))
					return;

				TPacketGCAcceRefine Packet2;
				Packet2.wSize = sizeof(TPacketGCAcceRefine) + sizeof(TSubPacketGCAcceRefineSetItem);
				Packet2.bSubHeader = ACCE_REFINE_SUBHEADER_GC_SET_ITEM;

				TSubPacketGCAcceRefineSetItem SubPacket2;
				SubPacket2.AttachedPos = TItemPos(RESERVED_WINDOW, 0);
				SubPacket2.SelectedPos = TItemPos(ACCEREFINE, ACCE_SLOT_RESULT);

				SubPacket2.Item.dwVnum = pLeftItem->GetRefineSet() == 409 ? pLeftItem->GetRefinedVnum() : pLeftItem->GetOriginalVnum();
				SubPacket2.Item.dwCount = pLeftItem->GetCount();
				SubPacket2.Item.dwFlags = pLeftItem->GetFlag();
				SubPacket2.Item.dwAntiFlags = pLeftItem->GetAntiFlag();

				TEMP_BUFFER TempBuffer;
				TempBuffer.write(&Packet2, sizeof(TPacketGCAcceRefine));
				TempBuffer.write(&SubPacket2, sizeof(TSubPacketGCAcceRefineSetItem));
				pDesc->Packet(TempBuffer.read_peek(), TempBuffer.size());
			}
		}
		break;

		case ACCE_SLOT_TYPE_ABSORB:
		{
			if (m_pAcceRefineWindowItemSlot[SelectedPos.cell] != NPOS)
				return;

			if (SelectedPos.cell == ACCE_SLOT_LEFT && !(pAttachedItem->IsCostumeAcce()))
				return;

			if (SelectedPos.cell == ACCE_SLOT_LEFT && pAttachedItem->GetSocket(ITEM_SOCKET_ACCE_DRAIN_ITEM_VNUM))
				return;

			if (SelectedPos.cell == ACCE_SLOT_RIGHT && !(pAttachedItem->IsWeapon() || pAttachedItem->IsArmorBody()))
				return;

			pAttachedItem->Lock(true);
			m_pAcceRefineWindowItemSlot[SelectedPos.cell] = AttachedPos;

			TPacketGCAcceRefine Packet;
			Packet.wSize = sizeof(TPacketGCAcceRefine) + sizeof(TSubPacketGCAcceRefineSetItem);
			Packet.bSubHeader = ACCE_REFINE_SUBHEADER_GC_SET_ITEM;

			TSubPacketGCAcceRefineSetItem SubPacket;
			SubPacket.AttachedPos = AttachedPos;
			SubPacket.SelectedPos = SelectedPos;
			SubPacket.Item.dwVnum = pAttachedItem->GetVnum();
			SubPacket.Item.dwCount = pAttachedItem->GetCount();
			SubPacket.Item.dwFlags = pAttachedItem->GetFlag();
			SubPacket.Item.dwAntiFlags = pAttachedItem->GetAntiFlag();
			thecore_memcpy(SubPacket.Item.alSockets, pAttachedItem->GetSockets(), sizeof(SubPacket.Item.alSockets));
			thecore_memcpy(SubPacket.Item.aAttr, pAttachedItem->GetAttributes(), sizeof(SubPacket.Item.aAttr));
#if defined(__CHANGE_LOOK_SYSTEM__)
			SubPacket.Item.dwTransmutationVnum = pAttachedItem->GetTransmutationVnum();
#endif
#if defined(__REFINE_ELEMENT_SYSTEM__)
			thecore_memcpy(&SubPacket.Item.RefineElement, pAttachedItem->GetRefineElement(), sizeof(SubPacket.Item.RefineElement));
#endif
#if defined(__ITEM_APPLY_RANDOM__)
			thecore_memcpy(SubPacket.Item.aApplyRandom, pAttachedItem->GetRandomApplies(), sizeof(SubPacket.Item.aApplyRandom));
#endif
#if defined(__SET_ITEM__)
			SubPacket.Item.bSetValue = pAttachedItem->GetItemSetValue();
#endif

			TEMP_BUFFER TempBuffer;
			TempBuffer.write(&Packet, sizeof(TPacketGCAcceRefine));
			TempBuffer.write(&SubPacket, sizeof(TSubPacketGCAcceRefineSetItem));
			pDesc->Packet(TempBuffer.read_peek(), TempBuffer.size());

			if ((NPOS != m_pAcceRefineWindowItemSlot[ACCE_SLOT_LEFT]) && (NPOS != m_pAcceRefineWindowItemSlot[ACCE_SLOT_RIGHT]))
			{
				const LPITEM pLeftItem = GetItem(m_pAcceRefineWindowItemSlot[ACCE_SLOT_LEFT]), pRightItem = GetItem(m_pAcceRefineWindowItemSlot[ACCE_SLOT_RIGHT]);
				if (!pLeftItem || !pRightItem)
					return;

				TPacketGCAcceRefine Packet2;
				Packet2.wSize = sizeof(TPacketGCAcceRefine) + sizeof(TSubPacketGCAcceRefineSetItem);
				Packet2.bSubHeader = ACCE_REFINE_SUBHEADER_GC_SET_ITEM;

				TSubPacketGCAcceRefineSetItem SubPacket2;
				SubPacket2.AttachedPos = TItemPos(RESERVED_WINDOW, 0);
				SubPacket2.SelectedPos = TItemPos(ACCEREFINE, ACCE_SLOT_RESULT);
				SubPacket2.Item.dwVnum = pLeftItem->GetOriginalVnum();
				SubPacket2.Item.dwCount = pLeftItem->GetCount();
				SubPacket2.Item.dwFlags = pLeftItem->GetFlag();
				SubPacket2.Item.dwAntiFlags = pLeftItem->GetAntiFlag();
				thecore_memcpy(SubPacket2.Item.alSockets, pLeftItem->GetSockets(), sizeof(SubPacket2.Item.alSockets));
				SubPacket2.Item.alSockets[ITEM_SOCKET_ACCE_DRAIN_ITEM_VNUM] = pRightItem->GetOriginalVnum();
				thecore_memcpy(SubPacket2.Item.aAttr, pRightItem->GetAttributes(), sizeof(SubPacket2.Item.aAttr));
#if defined(__CHANGE_LOOK_SYSTEM__)
				SubPacket2.Item.dwTransmutationVnum = pLeftItem->GetTransmutationVnum();
#endif
#if defined(__REFINE_ELEMENT_SYSTEM__)
				thecore_memcpy(&SubPacket2.Item.RefineElement, pRightItem->GetRefineElement(), sizeof(SubPacket.Item.RefineElement));
#endif
#if defined(__ITEM_APPLY_RANDOM__)
				thecore_memcpy(SubPacket2.Item.aApplyRandom, pRightItem->GetRandomApplies(), sizeof(SubPacket2.Item.aApplyRandom));
#endif
#if defined(__SET_ITEM__)
				SubPacket2.Item.bSetValue = pLeftItem->GetItemSetValue();
#endif

				TEMP_BUFFER TempBuffer;
				TempBuffer.write(&Packet2, sizeof(TPacketGCAcceRefine));
				TempBuffer.write(&SubPacket2, sizeof(TSubPacketGCAcceRefineSetItem));
				pDesc->Packet(TempBuffer.read_peek(), TempBuffer.size());
			}
		}
		break;
	}
}

void CHARACTER::AcceRefineWindowCheckOut(BYTE bType, TItemPos SelectedPos)
{
	if (SelectedPos.window_type != ACCEREFINE || SelectedPos.cell >= ACCE_SLOT_RESULT)
		return;

	if (m_bAcceRefineWindowType != bType)
		return;

	if (m_pAcceRefineWindowItemSlot[SelectedPos.cell] == NPOS)
		return;

	const LPDESC pDesc = GetDesc();
	if (pDesc == nullptr)
	{
		sys_err("AcceRefineWindowCheckOut(bType=%d, SelectedPos=TItemPos(%d, %d)): NULL DESC CHAR %s",
			bType, SelectedPos.window_type, SelectedPos.cell, GetName());
		return;
	}

	if (!IsAcceRefineWindowOpen())
		return;

	if (!IsAcceRefineWindowCanRefine())
		if (!IsAcceRefineWindowOpen() || !GetAcceRefineWindowOpener())
			return;

	if (SelectedPos.cell == ACCE_SLOT_LEFT && m_pAcceRefineWindowItemSlot[ACCE_SLOT_RIGHT] != NPOS)
	{
		const LPITEM pLeftItem = GetItem(m_pAcceRefineWindowItemSlot[ACCE_SLOT_LEFT]), pRightItem = GetItem(m_pAcceRefineWindowItemSlot[ACCE_SLOT_RIGHT]);
		if (!pLeftItem || !pRightItem)
			return;

		pLeftItem->Lock(false);
		pRightItem->Lock(false);

		m_pAcceRefineWindowItemSlot[SelectedPos.cell] = NPOS;
		m_pAcceRefineWindowItemSlot[ACCE_SLOT_RIGHT] = NPOS;

		TPacketGCAcceRefine Packet;
		Packet.wSize = sizeof(TPacketGCAcceRefine);
		Packet.bSubHeader = ACCE_REFINE_SUBHEADER_GC_CLEAR_ALL;
		pDesc->Packet(&Packet, sizeof(TPacketGCAcceRefine));
	}
	else
	{
		const LPITEM pItem = GetItem(m_pAcceRefineWindowItemSlot[SelectedPos.cell]);
		if (pItem == nullptr)
			return;

		pItem->Lock(false);
		m_pAcceRefineWindowItemSlot[SelectedPos.cell] = NPOS;

		TPacketGCAcceRefine Packet;
		Packet.wSize = sizeof(TPacketGCAcceRefine) + sizeof(TSubPacketGCAcceRefineClearSlot);
		Packet.bSubHeader = ACCE_REFINE_SUBHEADER_GC_CLEAR_SLOT;

		TSubPacketGCAcceRefineClearSlot SubPacket;
		SubPacket.bSlotIndex = SelectedPos.cell;

		TEMP_BUFFER TempBuffer;
		TempBuffer.write(&Packet, sizeof(TPacketGCAcceRefine));
		TempBuffer.write(&SubPacket, sizeof(TSubPacketGCAcceRefineClearSlot));
		pDesc->Packet(TempBuffer.read_peek(), TempBuffer.size());
	}
}

void CHARACTER::AcceRefineWindowAccept(BYTE bType)
{
	if (m_bAcceRefineWindowType != bType)
		return;

	if (m_pAcceRefineWindowItemSlot[ACCE_SLOT_LEFT] == NPOS || m_pAcceRefineWindowItemSlot[ACCE_SLOT_RIGHT] == NPOS)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_STRING("Drag the items into the window."));
		return;
	}

	const LPDESC pDesc = GetDesc();
	if (pDesc == nullptr)
	{
		sys_err("AcceRefineWindowAccept(bType=%d): NULL DESC char %s", bType, GetName());
		return;
	}

	if (!IsAcceRefineWindowOpen())
		return;

	if (!IsAcceRefineWindowCanRefine())
		if (!IsAcceRefineWindowOpen() || !GetAcceRefineWindowOpener())
			return;

	const LPITEM pLeftItem = GetItem(m_pAcceRefineWindowItemSlot[ACCE_SLOT_LEFT]);
	if (!pLeftItem || !m_pAcceRefineWindowItemSlot[ACCE_SLOT_LEFT].IsValidItemPosition())
		return;

	const LPITEM pRightItem = GetItem(m_pAcceRefineWindowItemSlot[ACCE_SLOT_RIGHT]);
	if (!pRightItem || !m_pAcceRefineWindowItemSlot[ACCE_SLOT_RIGHT].IsValidItemPosition())
		return;

	if (pLeftItem->IsExchanging() || pRightItem->IsExchanging())
		return;

	if (pLeftItem->IsEquipped() || pRightItem->IsEquipped())
		return;

#if defined(__SOUL_BIND_SYSTEM__)
	if (pLeftItem->IsSealed() || pRightItem->IsSealed())
		return;
#endif

	if (IS_SET(pLeftItem->GetAntiFlag(), ITEM_ANTIFLAG_ACCE) || IS_SET(pRightItem->GetAntiFlag(), ITEM_ANTIFLAG_ACCE))
		return;

	switch (__CheckAcceRefineItem(pLeftItem, pRightItem))
	{
		case ACCE_SLOT_TYPE_COMBINE:
			ChatPacket(CHAT_TYPE_INFO, LC_STRING("These sashes cannot be combined."));
			return;

		case ACCE_SLOT_TYPE_ABSORB:
			ChatPacket(CHAT_TYPE_INFO, LC_STRING("You can only use Shoulder Sashes as well as weapons and armour pieces."));
			return;

		case ACCE_SLOT_TYPE_MAX:
			sys_err("AcceRefineWindowAccept(bType=%d): Failed to check acce refine items for char %s", m_bAcceRefineWindowType, GetName());
			return;
	}

	switch (m_bAcceRefineWindowType)
	{
		case ACCE_SLOT_TYPE_COMBINE:
		{
			const int iPrice = pLeftItem->GetShopBuyPrice();
			if (GetGold() < iPrice)
			{
				ChatPacket(CHAT_TYPE_INFO, LC_STRING("You do not have enough money for the combination."));
				return;
			}

			const DWORD dwRefineSet = pLeftItem->GetRefineSet();
			const DWORD dwRefineVNum = dwRefineSet == 409 ? pLeftItem->GetRefinedVnum() : pLeftItem->GetOriginalVnum();
			const TRefineTable* pRefineTable = CRefineManager::Instance().GetRefineRecipe(dwRefineSet);
			BYTE bSuccessPct = 60;
			if (pRefineTable != nullptr)
				bSuccessPct = pRefineTable->prob;

			LPITEM pNewItem = ITEM_MANAGER::Instance().CreateItem(dwRefineVNum, 1, 0, false);
			if (pNewItem == nullptr)
			{
				sys_err("AcceRefineWindowAccept(bType=%d): Cannot create item %u for char %s",
					m_bAcceRefineWindowType, dwRefineVNum, GetName());
				return;
			}

			const int iEmptyPos = GetEmptyInventory(pNewItem->GetSize());
			if (iEmptyPos == -1)
			{
				ChatPacket(CHAT_TYPE_INFO, LC_STRING("There isn't enough space in your inventory."));
				M2_DESTROY_ITEM(pNewItem);
				return;
			}

			if (iPrice)
				PointChange(POINT_GOLD, -iPrice, true);

			const DWORD dwAbsorbItemVNum = static_cast<DWORD>(pLeftItem->GetSocket(ITEM_SOCKET_ACCE_DRAIN_ITEM_VNUM));
			const BYTE bDrainRate = static_cast<BYTE>(pLeftItem->GetSocket(ITEM_SOCKET_ACCE_DRAIN_VALUE));
			const BYTE bNewDrainRate = __GetNextDrainRate(pNewItem, bDrainRate);

			if (number(1, 100) <= bSuccessPct && bNewDrainRate > bDrainRate)
			{
				const WORD wCell = pLeftItem->GetCell();

				pLeftItem->CopySocketTo(pNewItem);
				pLeftItem->CopyAttributeTo(pNewItem);
#if defined(__ITEM_APPLY_RANDOM__)
				pLeftItem->CopyRandomAppliesTo(pNewItem);
#endif
#if defined(__REFINE_ELEMENT_SYSTEM__)
				pLeftItem->CopyElementTo(pNewItem);
#endif

				pNewItem->SetSocket(ITEM_SOCKET_ACCE_DRAIN_ITEM_VNUM, pLeftItem->GetSocket(ITEM_SOCKET_ACCE_DRAIN_ITEM_VNUM));
				if (pNewItem->GetRefinedVnum() == 0)
					pNewItem->SetSocket(ITEM_SOCKET_ACCE_DRAIN_VALUE, bNewDrainRate);
#if defined(__CHANGE_LOOK_SYSTEM__)
				pNewItem->SetTransmutationVnum(pLeftItem->GetTransmutationVnum());
#endif
#if defined(__SET_ITEM__)
				pNewItem->SetItemSetValue(pLeftItem->GetItemSetValue());
#endif

				ITEM_MANAGER::Instance().RemoveItem(pLeftItem, "ACCE LEFT ITEM REMOVE (COMBINATION SUCCESS)");
				ITEM_MANAGER::Instance().RemoveItem(pRightItem, "ACCE RIGHT ITEM REMOVE (COMBINATION SUCCESS)");

				pNewItem->AddToCharacter(this, TItemPos(INVENTORY, wCell));
				ITEM_MANAGER::Instance().FlushDelayedSave(pNewItem);

				m_pAcceRefineWindowItemSlot[ACCE_SLOT_LEFT] = NPOS;
				m_pAcceRefineWindowItemSlot[ACCE_SLOT_RIGHT] = NPOS;

				TPacketGCAcceRefine Packet;
				Packet.wSize = sizeof(TPacketGCAcceRefine);
				Packet.bSubHeader = ACCE_REFINE_SUBHEADER_GC_CLEAR_ALL;
				pDesc->Packet(&Packet, sizeof(TPacketGCAcceRefine));

				EffectPacket(SE_ACCE_SUCESS_ABSORB);

				if (dwRefineSet == 0)
				{
					ChatPacket(CHAT_TYPE_INFO, LC_STRING("New absorption rate: %d", bNewDrainRate));
				}
				else
				{
					if (bNewDrainRate == 20)
					{
						char szBuf[256];
						snprintf(szBuf, sizeof(szBuf), LC_STRING("%s was successful! A %d%% absorption rate on their new %s. Congratulations!",
							GetName(), bNewDrainRate, LC_ITEM(pNewItem->GetVnum())));

						SendNotice(szBuf);
					}
					else
						ChatPacket(CHAT_TYPE_INFO, LC_STRING("Success!"));
				}
			}
			else
			{
				M2_DESTROY_ITEM(pNewItem);

				ITEM_MANAGER::Instance().RemoveItem(pRightItem, "ACCE RIGHT ITEM REMOVE (COMBINATION FAILED)");

				m_pAcceRefineWindowItemSlot[ACCE_SLOT_RIGHT] = NPOS;

				TPacketGCAcceRefine Packet;
				Packet.wSize = sizeof(TPacketGCAcceRefine) + sizeof(TSubPacketGCAcceRefineClearSlot);
				Packet.bSubHeader = ACCE_REFINE_SUBHEADER_GC_CLEAR_SLOT;

				TSubPacketGCAcceRefineClearSlot SubPacket;
				SubPacket.bSlotIndex = ACCE_SLOT_RIGHT;

				TEMP_BUFFER TempBuffer;
				TempBuffer.write(&Packet, sizeof(TPacketGCAcceRefine));
				TempBuffer.write(&SubPacket, sizeof(TSubPacketGCAcceRefineClearSlot));
				pDesc->Packet(TempBuffer.read_peek(), TempBuffer.size());

				if (dwRefineSet == 0)
					ChatPacket(CHAT_TYPE_INFO, LC_STRING("New absorption rate: %d", bDrainRate));
				else
					ChatPacket(CHAT_TYPE_INFO, LC_STRING("Failed!"));
			}
		}
		break;

		case ACCE_SLOT_TYPE_ABSORB:
		{
			LPITEM pNewItem = ITEM_MANAGER::Instance().CreateItem(pLeftItem->GetVnum(), pLeftItem->GetCount(), 0, false);
			if (pNewItem == nullptr)
			{
				sys_err("AcceRefineWindowAccept(bType=%d): Cannot create item %u for char %s",
					m_bAcceRefineWindowType, pLeftItem->GetVnum(), GetName());
				return;
			}

			int iEmptyPos = GetEmptyInventory(pNewItem->GetSize());
			if (iEmptyPos == -1)
			{
				ChatPacket(CHAT_TYPE_INFO, LC_STRING("There isn't enough space in your inventory."));
				M2_DESTROY_ITEM(pNewItem);
				return;
			}

			const WORD wLeftItemPos = pLeftItem->GetCell();

			pRightItem->CopySocketTo(pNewItem);
			pRightItem->CopyAttributeTo(pNewItem);
#if defined(__ITEM_APPLY_RANDOM__)
			pRightItem->CopyRandomAppliesTo(pNewItem);
#endif
#if defined(__REFINE_ELEMENT_SYSTEM__)
			pRightItem->CopyElementTo(pNewItem);
#endif

			pNewItem->SetSocket(ITEM_SOCKET_ACCE_DRAIN_ITEM_VNUM, pRightItem->GetOriginalVnum());
			pNewItem->SetSocket(ITEM_SOCKET_ACCE_DRAIN_VALUE, pLeftItem->GetSocket(ITEM_SOCKET_ACCE_DRAIN_VALUE));
#if defined(__CHANGE_LOOK_SYSTEM__)
			pNewItem->SetTransmutationVnum(pLeftItem->GetTransmutationVnum());
#endif
#if defined(__SET_ITEM__)
			pNewItem->SetItemSetValue(pRightItem->GetItemSetValue());
#endif

			ITEM_MANAGER::Instance().RemoveItem(pLeftItem, "ACCE LEFT ITEM REMOVE (ABSORPTION SUCCESS)");
			ITEM_MANAGER::Instance().RemoveItem(pRightItem, "ACCE RIGHT ITEM REMOVE (ABSORPTION SUCCESS)");

			pNewItem->AddToCharacter(this, TItemPos(INVENTORY, wLeftItemPos));
			ITEM_MANAGER::Instance().FlushDelayedSave(pNewItem);

			m_pAcceRefineWindowItemSlot[ACCE_SLOT_LEFT] = NPOS;
			m_pAcceRefineWindowItemSlot[ACCE_SLOT_RIGHT] = NPOS;

			TPacketGCAcceRefine Packet;
			Packet.wSize = sizeof(TPacketGCAcceRefine);
			Packet.bSubHeader = ACCE_REFINE_SUBHEADER_GC_CLEAR_ALL;
			pDesc->Packet(&Packet, sizeof(TPacketGCAcceRefine));

			ChatPacket(CHAT_TYPE_INFO, LC_STRING("Success!"));
		}
		break;
	}
}

int CHARACTER::GetAcceWeaponAttack() const
{
	return __CalculateAcceDrainValues(WEAPON_MELEE_ATTACK);
}

int CHARACTER::GetAcceWeaponMagicAttack() const
{
	return __CalculateAcceDrainValues(WEAPON_MAGIC_ATTACK);
}

int CHARACTER::__CheckAcceRefineItem(const LPITEM& rLeftItem, const LPITEM& rRightItem) const
{
	if (rLeftItem == rRightItem)
		return ACCE_SLOT_TYPE_MAX;

	switch (m_bAcceRefineWindowType)
	{
		case ACCE_SLOT_TYPE_COMBINE:
		{
			if (rRightItem->GetType() != rLeftItem->GetType())
				return ACCE_SLOT_TYPE_COMBINE;

			if (rRightItem->GetSubType() != rLeftItem->GetSubType())
				return ACCE_SLOT_TYPE_COMBINE;

			if (rRightItem->FindApplyValue(APPLY_ACCEDRAIN_RATE) != rLeftItem->FindApplyValue(APPLY_ACCEDRAIN_RATE))
				return ACCE_SLOT_TYPE_COMBINE;

			return -1;
		}
		break;

		case ACCE_SLOT_TYPE_ABSORB:
		{
			if (rLeftItem->GetSocket(ITEM_SOCKET_ACCE_DRAIN_ITEM_VNUM) != 0)
				return ACCE_SLOT_TYPE_ABSORB;

			if (!rLeftItem->IsCostumeAcce())
				return ACCE_SLOT_TYPE_ABSORB;

			if (!rRightItem->IsWeapon() && !rRightItem->IsArmorBody())
				return ACCE_SLOT_TYPE_ABSORB;

			return -1;
		}
		break;
	}

	return ACCE_SLOT_TYPE_MAX;
}

BYTE CHARACTER::__GetNextDrainRate(const LPITEM& rAcceItem, BYTE bMinDrainRate) const
{
	if (rAcceItem->GetRefinedVnum() == 0)
	{
		BYTE bMinDrainValue = bMinDrainRate;
		BYTE bMaxDrainValue = 0;

		if (bMinDrainValue < 11)
		{
			bMinDrainValue = 11;
			bMaxDrainValue = 20;
		}
		else
		{
			bMaxDrainValue = bMinDrainValue + 5;
		}

		if (bMaxDrainValue >= ACCE_MAX_DRAINRATE)
			bMaxDrainValue = ACCE_MAX_DRAINRATE;

		static std::mt19937 rng(std::random_device{}());
		static std::discrete_distribution<int> dist({ 7, 5, 3, 1, 0.5f }); // +5

		bool bAdvance = false;
		if (bMinDrainValue > 18)
		{
			if (number(1, 100) <= aiAcceDrainRateAdvancePct[bMinDrainValue])
				bAdvance = true;
		}
		else
			bAdvance = true;

		BYTE bStep = bAdvance ? MINMAX(1, dist(rng) + 1, 5) : 0;
		if (bAdvance && test_server)
			sys_log(0, " LUCKY DRAINRATE ADVANCE %d STEP", bStep);

		return MINMAX(bMinDrainValue, bMinDrainValue + bStep, bMaxDrainValue);
	}
	else
	{
		TItemTable* pItemTable = ITEM_MANAGER::Instance().GetTable(rAcceItem->GetVnum());
		return (pItemTable ? pItemTable->FindApplyValue(APPLY_ACCEDRAIN_RATE) : 0);
	}
}

int CHARACTER::__CalculateAcceDrainValues(BYTE bWeaponAttackType) const
{
	const LPITEM& rAcceItem = GetWear(WEAR_COSTUME_ACCE);
	if (rAcceItem == nullptr)
		return 0;

	long lSocketInDrainItemVnum = rAcceItem->GetSocket(ITEM_SOCKET_ACCE_DRAIN_ITEM_VNUM);
	const TItemTable* pItemTable = ITEM_MANAGER::Instance().GetTable(lSocketInDrainItemVnum);
	if (pItemTable == nullptr)
		return 0;

	long lApplyValue = rAcceItem->FindApplyValue(APPLY_ACCEDRAIN_RATE);
	long lSocketInDrainValue = 0;
	float fDrain = 0.0f;

	if (rAcceItem->GetRefinedVnum() == 0)
	{
		lSocketInDrainValue = rAcceItem->GetSocket(ITEM_SOCKET_ACCE_DRAIN_VALUE);
		fDrain = lSocketInDrainValue / 100.0;
	}
	else
	{
		lSocketInDrainValue = lApplyValue;
		fDrain = lApplyValue / 100.0;
	}

	if (lSocketInDrainValue == 0)
		return 0;

	if (pItemTable->bType == ITEM_WEAPON)
	{
		long lAddPower = pItemTable->alValues[5];

		switch (bWeaponAttackType)
		{
			case WEAPON_MELEE_ATTACK:
			{
				if (pItemTable->alValues[3] && pItemTable->alValues[4])
				{
#if defined(__ITEM_APPLY_RANDOM__) && defined(__ITEM_VALUE10__)
					int iMinAtk = pItemTable->alValues[3];
					int iMaxAtk = pItemTable->alValues[4];

					int iMinMax = pItemTable->alSockets[ITEM_SOCKET_ATK_MINMAX_RANDOM];
					if (iMinMax)
					{
						iMinAtk = iMinMax / ITEM_VALUE_MINMAX_RANDOM_DIVISION_VALUE;
						iMaxAtk = iMinMax % (ITEM_VALUE_MINMAX_RANDOM_DIVISION_VALUE / 100);
					}

					int iMinPower = MAX(2 * (iMinAtk + lAddPower) * fDrain, 1);
					int iMaxPower = MAX(2 * (iMaxAtk + lAddPower) * fDrain, 1);
#else
					int iMinPower = MAX(2 * (pItemTable->alValues[3] + lAddPower) * fDrain, 1);
					int iMaxPower = MAX(2 * (pItemTable->alValues[4] + lAddPower) * fDrain, 1);
#endif

					if (iMinPower < iMaxPower)
						return number(iMinPower, iMaxPower);
					else
						return iMinPower;
				}
			}
			break;

			case WEAPON_MAGIC_ATTACK:
			{
				if (pItemTable->alValues[1] && pItemTable->alValues[2])
				{
#if defined(__ITEM_APPLY_RANDOM__) && defined(__ITEM_VALUE10__)
					int iMinMtk = pItemTable->alValues[1];
					int iMaxMtk = pItemTable->alValues[2];

					int iMinMax = pItemTable->alSockets[ITEM_SOCKET_MTK_MINMAX_RANDOM];
					if (iMinMax)
					{
						iMinMtk = iMinMax / ITEM_VALUE_MINMAX_RANDOM_DIVISION_VALUE;
						iMaxMtk = iMinMax % (ITEM_VALUE_MINMAX_RANDOM_DIVISION_VALUE / 100);
					}

					int iMinMagicAttackPower = MAX(2 * (iMinMtk + lAddPower) * fDrain, 1);
					int iMaxMagicAttackPower = MAX(2 * (iMaxMtk + lAddPower) * fDrain, 1);
#else
					int iMinMagicAttackPower = MAX(2 * (pItemTable->alValues[1] + lAddPower) * fDrain, 1);
					int iMaxMagicAttackPower = MAX(2 * (pItemTable->alValues[2] + lAddPower) * fDrain, 1);
#endif

					if (iMinMagicAttackPower < iMaxMagicAttackPower)
						return number(iMinMagicAttackPower, iMaxMagicAttackPower);
					else
						return iMinMagicAttackPower;
				}
			}
			break;
		}
	}

	return 0;
}

void CHARACTER::ModifyAccePoints(const LPITEM& rAcceItem, bool bAdd)
{
	const long lDrainedItemVNum = rAcceItem->GetSocket(ITEM_SOCKET_ACCE_DRAIN_ITEM_VNUM);
	const TItemTable* pDrainedItem = ITEM_MANAGER::Instance().GetTable(lDrainedItemVNum);
	if (pDrainedItem == nullptr)
		return;

	const long lApplyValue = rAcceItem->FindApplyValue(APPLY_ACCEDRAIN_RATE);
	long lDrainValue = 0;
	float fDrain = 0.0f;

	if (rAcceItem->GetRefinedVnum() == 0)
	{
		lDrainValue = rAcceItem->GetSocket(ITEM_SOCKET_ACCE_DRAIN_VALUE);
		fDrain = lDrainValue / 100.0;
	}
	else
	{
		lDrainValue = lApplyValue;
		fDrain = lApplyValue / 100.0;
	}

	if (lDrainValue == 0)
		return;

	if (pDrainedItem->bType == ITEM_WEAPON)
	{
		// battle.cpp, calculated @ __CalculateAcceDrainValues

#if defined(__REFINE_ELEMENT_SYSTEM__)
		// Attack Grade Bonus
		if (rAcceItem->GetRefineElementGrade() != 0)
		{
			const long lBonusValue = static_cast<long>(rAcceItem->GetRefineElementBonusValue());
			int iDrainValue = MAX(lBonusValue * fDrain, 1);
			if (iDrainValue)
				ApplyPoint(APPLY_ATT_GRADE_BONUS, bAdd ? iDrainValue : -iDrainValue);
		}
#endif
	}
	if (pDrainedItem->bType == ITEM_ARMOR)
	{
		// Defense
		long lDefGrade = pDrainedItem->alValues[1];
#if defined(__ITEM_APPLY_RANDOM__) && defined(__ITEM_VALUE10__)
		int iMinMax = rAcceItem->GetSocket(ITEM_SOCKET_DEF_MINMAX_RANDOM);
		if (iMinMax != 0)
			lDefGrade = iMinMax / ITEM_VALUE_MINMAX_RANDOM_DIVISION_VALUE;
#endif
		long lDefBonus = pDrainedItem->alValues[5] * 2;
		if (lDefGrade > 0)
		{
			int iDrainValue = MAX(((lDefGrade + lDefBonus) * fDrain), 1);
			ApplyPoint(APPLY_DEF_GRADE_BONUS, bAdd ? iDrainValue : -iDrainValue);
		}
	}

	for (BYTE bApplyIndex = 0; bApplyIndex < ITEM_APPLY_MAX_NUM; ++bApplyIndex)
	{
		if (pDrainedItem->aApplies[bApplyIndex].wType == APPLY_NONE)
			continue;

		int iDrainValue = MAX(pDrainedItem->aApplies[bApplyIndex].lValue * fDrain, 1);
		if (pDrainedItem->aApplies[bApplyIndex].wType == APPLY_SKILL)
			ApplyPoint(pDrainedItem->aApplies[bApplyIndex].wType, bAdd ? iDrainValue : iDrainValue ^ 0x00800000);
#if defined(__ITEM_APPLY_RANDOM__)
		else if (pDrainedItem->aApplies[bApplyIndex].wType == APPLY_RANDOM)
		{
			const TPlayerItemAttribute& rRandomApply = rAcceItem->GetRandomApply(bApplyIndex);
			if (rRandomApply.lValue <= 0)
				continue;

			iDrainValue = MAX(rRandomApply.lValue * fDrain, 1);
			if (rRandomApply.wType == APPLY_SKILL)
				ApplyPoint(rRandomApply.wType, bAdd ? iDrainValue : iDrainValue ^ 0x00800000);
			else
				ApplyPoint(rRandomApply.wType, bAdd ? iDrainValue : -iDrainValue);
		}
#endif
		else
			ApplyPoint(pDrainedItem->aApplies[bApplyIndex].wType, bAdd ? iDrainValue : -iDrainValue);
	}

	for (BYTE bAttrIndex = 0; bAttrIndex < ITEM_ATTRIBUTE_MAX_NUM; ++bAttrIndex)
	{
		if (rAcceItem->GetAttributeType(bAttrIndex))
		{
			const TPlayerItemAttribute& rItemAttribute = rAcceItem->GetAttribute(bAttrIndex);
			if (rItemAttribute.lValue <= 0)
				continue;

			int iDrainValue = MAX(rItemAttribute.lValue * fDrain, 1);
			if (rItemAttribute.wType == APPLY_SKILL)
				ApplyPoint(rItemAttribute.wType, bAdd ? iDrainValue : iDrainValue ^ 0x00800000);
			else
				ApplyPoint(rItemAttribute.wType, bAdd ? iDrainValue : -iDrainValue);
		}
	}

#if defined(__REFINE_ELEMENT_SYSTEM__)
	if (rAcceItem->GetRefineElementGrade() != 0)
	{
		const long lValue = static_cast<long>(rAcceItem->GetRefineElementValue());
		int iDrainValue = MAX(lValue * fDrain, 1);
		if (iDrainValue)
			ApplyPoint(rAcceItem->GetRefineElementApplyType(), bAdd ? iDrainValue : -iDrainValue);
	}
#endif
}
#endif // __ACCE_COSTUME_SYSTEM__
