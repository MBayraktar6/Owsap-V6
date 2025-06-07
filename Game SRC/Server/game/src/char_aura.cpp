/**
* Filename: char_aura.cpp
* Author: Owsap
**/

#include "stdafx.h"

#if defined(__AURA_COSTUME_SYSTEM__)
#include "char.h"
#include "item.h"
#include "desc.h"
#include "utils.h"

void CHARACTER::OpenAuraRefineWindow(const LPENTITY pEntity, BYTE bType)
{
	if (pEntity == nullptr)
	{
		sys_err("OpenAuraRefineWindow(pEntity=%p, bType=%d): NULL ENTITY CHAR %s",
			get_pointer(pEntity), GetName());
		return;
	}

	if (PreventTradeWindow(WND_ALL))
		return;

	int iDist = DISTANCE_APPROX(GetX() - pEntity->GetX(), GetY() - pEntity->GetY());
	if (iDist >= WINDOW_OPENER_MAX_DISTANCE)
		return;

	TPacketGCAuraRefine Packet;
	Packet.wSize = sizeof(TPacketGCAuraRefine) + sizeof(TSubPacketGCAuraRefineOpenClose);
	Packet.bSubHeader = AURA_REFINE_SUBHEADER_GC_OPEN;

	TSubPacketGCAuraRefineOpenClose SubPacket;
	switch (bType)
	{
		case AURA_WINDOW_TYPE_ABSORB:
		case AURA_WINDOW_TYPE_GROWTH:
		case AURA_WINDOW_TYPE_EVOLVE:
			SubPacket.bType = bType;
			break;

		case AURA_WINDOW_TYPE_MAX:
			return;
	}

	const LPDESC pDesc = GetDesc();
	if (pDesc == nullptr)
	{
		sys_err("OpenAuraRefineWindow:: NULL DESC CHAR %s", GetName());
		return;
	}

	TEMP_BUFFER TempBuffer;
	TempBuffer.write(&Packet, sizeof(TPacketGCAuraRefine));
	TempBuffer.write(&SubPacket, sizeof(TSubPacketGCAuraRefineOpenClose));

	if (m_pointsInstant.m_pAuraRefineWindowOpener == nullptr)
		m_pointsInstant.m_pAuraRefineWindowOpener = pEntity;

	m_bAuraRefineWindowType = bType;
	m_bAuraRefineWindowOpen = AURA_WINDOW_TYPE_MAX != m_bAuraRefineWindowType;

	pDesc->Packet(TempBuffer.read_peek(), TempBuffer.size());
}

void CHARACTER::AuraRefineWindowClose(bool bServerClose)
{
	const LPDESC pDesc = GetDesc();
	if (pDesc == nullptr)
	{
		sys_err("AuraRefineWindowClose:: NULL DESC CHAR %s", GetName());
		return;
	}

	m_pointsInstant.m_pAuraRefineWindowOpener = nullptr;
	m_bAuraRefineWindowType = AURA_WINDOW_TYPE_MAX;
	m_bAuraRefineWindowOpen = false;

	for (BYTE bSlotIndex = 0; bSlotIndex < AURA_SLOT_MAX; ++bSlotIndex)
	{
		if (m_pAuraRefineWindowItemSlot[bSlotIndex] != NPOS)
		{
			const LPITEM pItem = GetItem(m_pAuraRefineWindowItemSlot[bSlotIndex]);
			if (pItem != nullptr)
				pItem->Lock(false);

			m_pAuraRefineWindowItemSlot[bSlotIndex] = NPOS;
		}
	}

	TPacketGCAuraRefine Packet;
	Packet.bSubHeader = AURA_REFINE_SUBHEADER_GC_CLOSE;
	Packet.wSize = sizeof(TPacketGCAuraRefine) + sizeof(TSubPacketGCAuraRefineOpenClose);

	TSubPacketGCAuraRefineOpenClose SubPacket;
	SubPacket.bType = AURA_WINDOW_TYPE_MAX;
	SubPacket.bServerClose = bServerClose;

	TEMP_BUFFER TempBuffer;
	TempBuffer.write(&Packet, sizeof(TPacketGCAuraRefine));
	TempBuffer.write(&SubPacket, sizeof(TSubPacketGCAuraRefineOpenClose));
	pDesc->Packet(TempBuffer.read_peek(), TempBuffer.size());
}

bool CHARACTER::IsAuraRefineWindowCanRefine()
{
	if (!IsAuraRefineWindowOpen())
		return false;

	if (!CanHandleItem())
		return false;

	const LPENTITY pEntity = GetAuraRefineWindowOpener();
	if (pEntity == nullptr)
		return false;

	int iDist = DISTANCE_APPROX(GetX() - pEntity->GetX(), GetY() - pEntity->GetY());
	if (iDist >= WINDOW_OPENER_MAX_DISTANCE)
		return false;

	return true;
}

void CHARACTER::AuraRefineWindowCheckIn(BYTE bType, TItemPos SelectedPos, TItemPos AttachedPos)
{
	if (SelectedPos.window_type != AURA_REFINE || SelectedPos.cell >= AURA_SLOT_RESULT)
		return;

	if (!AttachedPos.IsValidItemPosition() || AttachedPos.IsEquipPosition())
		return;

	if (m_bAuraRefineWindowType != bType)
		return;

	const LPDESC pDesc = GetDesc();
	if (pDesc == nullptr)
	{
		sys_err("AuraRefineWindowCheckIn(bType=%d):: NULL DESC CHAR %s", bType, GetName());
		return;
	}

	if (!IsAuraRefineWindowOpen())
		return;

	if (!IsAuraRefineWindowCanRefine())
		if (!IsAuraRefineWindowOpen() || !GetAuraRefineWindowOpener())
			return;
	
	const LPITEM pAttachedItem = GetItem(AttachedPos);
	if (pAttachedItem == nullptr)
		return;

	if (pAttachedItem->isLocked() || pAttachedItem->IsExchanging() || pAttachedItem->IsEquipped())
	{
		ChatPacket(CHAT_TYPE_INFO, LC_STRING("[Aura] Cannot be improved."));
		return;
	}

#if defined(__SOUL_BIND_SYSTEM__)
	if (pAttachedItem->IsSealed())
	{
		ChatPacket(CHAT_TYPE_INFO, LC_STRING("[Aura] Does not work with soulbound items."));
		return;
	}
#endif

	switch (bType)
	{
		case AURA_WINDOW_TYPE_ABSORB:
		{
			if (m_pAuraRefineWindowItemSlot[SelectedPos.cell] != NPOS)
				return;

			if (SelectedPos.cell == AURA_SLOT_MAIN && !(pAttachedItem->IsCostumeAura()))
			{
				ChatPacket(CHAT_TYPE_INFO, LC_STRING("[Aura] Only Aura Outfit can be inserted."));
				return;
			}

			if (SelectedPos.cell == AURA_SLOT_MAIN && pAttachedItem->GetSocket(ITEM_SOCKET_AURA_DRAIN_ITEM_VNUM))
			{
				ChatPacket(CHAT_TYPE_INFO, LC_STRING("[Aura] Does not work with an Aura Outfit onto which an item has already been transmitted."));
				return;
			}

			if (SelectedPos.cell == AURA_SLOT_SUB && !(pAttachedItem->IsArmorType(ARMOR_SHIELD) || pAttachedItem->IsArmorType(ARMOR_WRIST) || pAttachedItem->IsArmorType(ARMOR_NECK) || pAttachedItem->IsArmorType(ARMOR_EAR)))
			{
				ChatPacket(CHAT_TYPE_INFO, LC_STRING("[Aura] Only Aura Outfit, Shield, Necklace, Earring or Armband can be inserted."));
				return;
			}

			pAttachedItem->Lock(true);
			m_pAuraRefineWindowItemSlot[SelectedPos.cell] = AttachedPos;

			TPacketGCAuraRefine Packet;
			Packet.wSize = sizeof(TPacketGCAuraRefine) + sizeof(TSubPacketGCAuraRefineSetItem);
			Packet.bSubHeader = AURA_REFINE_SUBHEADER_GC_SET_ITEM;

			TSubPacketGCAuraRefineSetItem SubPacket;
			SubPacket.AttachedPos = AttachedPos;
			SubPacket.SelectedPos = SelectedPos;
			SubPacket.Item.dwVnum = pAttachedItem->GetVnum();
			SubPacket.Item.dwCount = pAttachedItem->GetCount();
			SubPacket.Item.dwFlags = pAttachedItem->GetFlag();
			SubPacket.Item.dwAntiFlags = pAttachedItem->GetAntiFlag();
			thecore_memcpy(SubPacket.Item.alSockets, pAttachedItem->GetSockets(), sizeof(SubPacket.Item.alSockets));
			thecore_memcpy(SubPacket.Item.aAttr, pAttachedItem->GetAttributes(), sizeof(SubPacket.Item.aAttr));
#if defined(__ITEM_APPLY_RANDOM__)
			thecore_memcpy(SubPacket.Item.aApplyRandom, pAttachedItem->GetRandomApplies(), sizeof(SubPacket.Item.aApplyRandom));
#endif

			TEMP_BUFFER TempBuffer;
			TempBuffer.write(&Packet, sizeof(TPacketGCAuraRefine));
			TempBuffer.write(&SubPacket, sizeof(TSubPacketGCAuraRefineSetItem));
			pDesc->Packet(TempBuffer.read_peek(), TempBuffer.size());

			if ((NPOS != m_pAuraRefineWindowItemSlot[AURA_SLOT_MAIN]) && (NPOS != m_pAuraRefineWindowItemSlot[AURA_SLOT_SUB]))
			{
				const LPITEM pMainItem = GetItem(m_pAuraRefineWindowItemSlot[AURA_SLOT_MAIN]), pSubItem = GetItem(m_pAuraRefineWindowItemSlot[AURA_SLOT_SUB]);
				if (!pMainItem || !pSubItem)
					return;

				TPacketGCAuraRefine Packet2;
				Packet2.wSize = sizeof(TPacketGCAuraRefine) + sizeof(TSubPacketGCAuraRefineSetItem);
				Packet2.bSubHeader = AURA_REFINE_SUBHEADER_GC_SET_ITEM;

				TSubPacketGCAuraRefineSetItem SubPacket2;
				SubPacket2.AttachedPos = TItemPos(RESERVED_WINDOW, 0);
				SubPacket2.SelectedPos = TItemPos(AURA_REFINE, AURA_SLOT_RESULT);
				SubPacket2.Item.dwVnum = pMainItem->GetOriginalVnum();
				SubPacket2.Item.dwCount = pMainItem->GetCount();
				SubPacket2.Item.dwFlags = pMainItem->GetFlag();
				SubPacket2.Item.dwAntiFlags = pMainItem->GetAntiFlag();
				thecore_memcpy(SubPacket2.Item.alSockets, pMainItem->GetSockets(), sizeof(SubPacket2.Item.alSockets));
				SubPacket2.Item.alSockets[ITEM_SOCKET_AURA_DRAIN_ITEM_VNUM] = pSubItem->GetOriginalVnum();
				thecore_memcpy(SubPacket2.Item.aAttr, pSubItem->GetAttributes(), sizeof(SubPacket2.Item.aAttr));
#if defined(__ITEM_APPLY_RANDOM__)
				thecore_memcpy(SubPacket2.Item.aApplyRandom, pSubItem->GetRandomApplies(), sizeof(SubPacket2.Item.aApplyRandom));
#endif

				TEMP_BUFFER TempBuffer;
				TempBuffer.write(&Packet2, sizeof(TPacketGCAuraRefine));
				TempBuffer.write(&SubPacket2, sizeof(TSubPacketGCAuraRefineSetItem));
				pDesc->Packet(TempBuffer.read_peek(), TempBuffer.size());
			}
		}
		break;

		case AURA_WINDOW_TYPE_GROWTH:
		{
			if (SelectedPos.cell == AURA_SLOT_MAIN)
			{
				if (!pAttachedItem->IsCostumeAura())
				{
					ChatPacket(CHAT_TYPE_INFO, LC_STRING("[Aura] Only Aura Outfit can be inserted."));
					return;
				}
				else
				{
					const long lLevelSocket = pAttachedItem->GetSocket(ITEM_SOCKET_AURA_LEVEL_VALUE);
					const BYTE bCurrentLevel = (lLevelSocket / 100000) - 1000;
					const WORD wCurrentExp = lLevelSocket % 100000;
					const int* aiAuraRefineTable = GetAuraRefineInfo(bCurrentLevel);
					if (bCurrentLevel == aiAuraRefineTable[AURA_REFINE_INFO_LEVEL_MAX] && wCurrentExp == aiAuraRefineTable[AURA_REFINE_INFO_NEED_EXP])
					{
						ChatPacket(CHAT_TYPE_INFO, LC_STRING("[Aura] Cannot be improved."));
						if (aiAuraRefineTable[AURA_REFINE_INFO_STEP] == AURA_GRADE_RADIANT)
							ChatPacket(CHAT_TYPE_INFO, LC_STRING("[Aura] Your Aura Outfit has reached the maximum level."));

						return;
					}
				}
			}

			if (SelectedPos.cell == AURA_SLOT_SUB)
			{
				if (NPOS == m_pAuraRefineWindowItemSlot[AURA_SLOT_MAIN])
				{
					ChatPacket(CHAT_TYPE_INFO, LC_STRING("[Aura] Can only be done once an Aura Outfit has been inserted."));
					return;
				}
				else if (!(pAttachedItem->GetType() == ITEM_RESOURCE && pAttachedItem->GetSubType() == RESOURCE_AURA))
				{
					if (test_server)
						ChatPacket(CHAT_TYPE_INFO, "[Aura Test] Required aura resource.");
					return;
				}
			}

			if (m_pAuraRefineWindowItemSlot[SelectedPos.cell] != NPOS)
				return;

			pAttachedItem->Lock(true);
			m_pAuraRefineWindowItemSlot[SelectedPos.cell] = AttachedPos;

			TPacketGCAuraRefine Packet;
			Packet.wSize = sizeof(TPacketGCAuraRefine) + sizeof(TSubPacketGCAuraRefineSetItem);
			Packet.bSubHeader = AURA_REFINE_SUBHEADER_GC_SET_ITEM;

			TSubPacketGCAuraRefineSetItem SubPacket;
			SubPacket.AttachedPos = AttachedPos;
			SubPacket.SelectedPos = SelectedPos;
			SubPacket.Item.dwVnum = pAttachedItem->GetVnum();
			SubPacket.Item.dwCount = pAttachedItem->GetCount();
			SubPacket.Item.dwFlags = pAttachedItem->GetFlag();
			SubPacket.Item.dwAntiFlags = pAttachedItem->GetAntiFlag();
			thecore_memcpy(SubPacket.Item.alSockets, pAttachedItem->GetSockets(), sizeof(SubPacket.Item.alSockets));
			thecore_memcpy(SubPacket.Item.aAttr, pAttachedItem->GetAttributes(), sizeof(SubPacket.Item.aAttr));
#if defined(__ITEM_APPLY_RANDOM__)
			thecore_memcpy(SubPacket.Item.aApplyRandom, pAttachedItem->GetRandomApplies(), sizeof(SubPacket.Item.aApplyRandom));
#endif

			TEMP_BUFFER TempBuffer;
			TempBuffer.write(&Packet, sizeof(TPacketGCAuraRefine));
			TempBuffer.write(&SubPacket, sizeof(TSubPacketGCAuraRefineSetItem));
			pDesc->Packet(TempBuffer.read_peek(), TempBuffer.size());

			{
				const LPITEM pAttachedItem = GetItem(AttachedPos);
				if (!pAttachedItem)
					return;

				TPacketGCAuraRefine Packet;
				Packet.wSize = sizeof(TPacketGCAuraRefine) + sizeof(TSubPacketGCAuraRefineInfo);
				Packet.bSubHeader = AURA_REFINE_SUBHEADER_GC_INFO;

				TSubPacketGCAuraRefineInfo SubPacket;
				TAuraRefineInfo RefineInfo = {};
				if (SelectedPos.cell == AURA_SLOT_MAIN)
				{
					SubPacket.bInfoType = AURA_REFINE_INFO_SLOT_CURRENT;
					RefineInfo = __GetAuraRefineInfo(AttachedPos);
				}
				if (SelectedPos.cell == AURA_SLOT_SUB)
				{
					SubPacket.bInfoType = AURA_REFINE_INFO_SLOT_NEXT;
					RefineInfo = __CalcAuraRefineInfo(m_pAuraRefineWindowItemSlot[AURA_SLOT_MAIN], AttachedPos);
				}
				SubPacket.bInfoLevel = RefineInfo.bLevel;
				SubPacket.bInfoExpPercent = RefineInfo.bExpPercent;

				TEMP_BUFFER TempBuffer;
				TempBuffer.write(&Packet, sizeof(TPacketGCAuraRefine));
				TempBuffer.write(&SubPacket, sizeof(TSubPacketGCAuraRefineInfo));
				pDesc->Packet(TempBuffer.read_peek(), TempBuffer.size());
			}
		}
		break;

		case AURA_WINDOW_TYPE_EVOLVE:
		{
			if (SelectedPos.cell == AURA_SLOT_MAIN)
			{
				if (!(pAttachedItem->IsCostumeAura()))
				{
					ChatPacket(CHAT_TYPE_INFO, LC_STRING("[Aura] Only Aura Outfit can be inserted."));
					return;
				}
				else
				{
					const long lLevelSocket = pAttachedItem->GetSocket(ITEM_SOCKET_AURA_LEVEL_VALUE);
					const BYTE bCurrentLevel = (lLevelSocket / 100000) - 1000;
					const WORD wCurrentExp = lLevelSocket % 100000;
					const int* aiAuraRefineTable = GetAuraRefineInfo(bCurrentLevel);
					if (!(bCurrentLevel == aiAuraRefineTable[AURA_REFINE_INFO_LEVEL_MAX] && wCurrentExp == aiAuraRefineTable[AURA_REFINE_INFO_NEED_EXP]))
					{
						ChatPacket(CHAT_TYPE_INFO, LC_STRING("[Aura] You cannot evolve your Aura Outfit until you have improved it."));
						if (test_server)
							ChatPacket(CHAT_TYPE_INFO, "[Aura Test] Required level %d with %d experience.", aiAuraRefineTable[AURA_REFINE_INFO_LEVEL_MAX], aiAuraRefineTable[AURA_REFINE_INFO_NEED_EXP]);
						return;
					}
				}
			}

			if (SelectedPos.cell == AURA_SLOT_SUB)
			{
				if (NPOS == m_pAuraRefineWindowItemSlot[AURA_SLOT_MAIN])
				{
					ChatPacket(CHAT_TYPE_INFO, LC_STRING("[Aura] Can only be done once an Aura Outfit has been inserted."));
					return;
				}
				else
				{
					const LPITEM pMainItem = GetItem(m_pAuraRefineWindowItemSlot[AURA_SLOT_MAIN]);
					const long lLevelSocket = pMainItem->GetSocket(ITEM_SOCKET_AURA_LEVEL_VALUE);
					const BYTE bCurrentLevel = (lLevelSocket / 100000) - 1000;
					const int* aiAuraRefineTable = GetAuraRefineInfo(bCurrentLevel);
					if (pAttachedItem->GetOriginalVnum() != aiAuraRefineTable[AURA_REFINE_INFO_MATERIAL_VNUM])
					{
						if (test_server)
						{
							ChatPacket(CHAT_TYPE_INFO, "[Aura Test] Required aura resource.");
							ChatPacket(CHAT_TYPE_INFO, "[Aura Test] Need, %s - %u", LC_ITEM(aiAuraRefineTable[AURA_REFINE_INFO_MATERIAL_VNUM]), aiAuraRefineTable[AURA_REFINE_INFO_MATERIAL_COUNT]);
						}
						return;
					}
					else
					{
						if (CountSpecifyItem(pAttachedItem->GetOriginalVnum()) > aiAuraRefineTable[AURA_REFINE_INFO_MATERIAL_COUNT])
							ChatPacket(CHAT_TYPE_INFO, LC_STRING("[Aura] The amount of inserted materials has been adjusted to match the required amount."));
					}
				}
			}

			if (m_pAuraRefineWindowItemSlot[SelectedPos.cell] != NPOS)
				return;

			pAttachedItem->Lock(true);
			m_pAuraRefineWindowItemSlot[SelectedPos.cell] = AttachedPos;

			TPacketGCAuraRefine Packet;
			Packet.wSize = sizeof(TPacketGCAuraRefine) + sizeof(TSubPacketGCAuraRefineSetItem);
			Packet.bSubHeader = AURA_REFINE_SUBHEADER_GC_SET_ITEM;

			TSubPacketGCAuraRefineSetItem SubPacket;
			SubPacket.AttachedPos = AttachedPos;
			SubPacket.SelectedPos = SelectedPos;
			SubPacket.Item.dwVnum = pAttachedItem->GetVnum();
			SubPacket.Item.dwCount = pAttachedItem->GetCount();
			SubPacket.Item.dwFlags = pAttachedItem->GetFlag();
			SubPacket.Item.dwAntiFlags = pAttachedItem->GetAntiFlag();
			thecore_memcpy(SubPacket.Item.alSockets, pAttachedItem->GetSockets(), sizeof(SubPacket.Item.alSockets));
			thecore_memcpy(SubPacket.Item.aAttr, pAttachedItem->GetAttributes(), sizeof(SubPacket.Item.aAttr));
#if defined(__ITEM_APPLY_RANDOM__)
			thecore_memcpy(SubPacket.Item.aApplyRandom, pAttachedItem->GetRandomApplies(), sizeof(SubPacket.Item.aApplyRandom));
#endif

			TEMP_BUFFER TempBuffer;
			TempBuffer.write(&Packet, sizeof(TPacketGCAuraRefine));
			TempBuffer.write(&SubPacket, sizeof(TSubPacketGCAuraRefineSetItem));
			pDesc->Packet(TempBuffer.read_peek(), TempBuffer.size());

			if (SelectedPos.cell == AURA_SLOT_MAIN)
			{
				LPITEM pMainItem = GetItem(AttachedPos);
				if (pMainItem == nullptr)
					return;

				TPacketGCAuraRefine Packet;
				TSubPacketGCAuraRefineInfo SubPacketA;
				TSubPacketGCAuraRefineInfo SubPacketB;
				Packet.wSize = sizeof(TPacketGCAuraRefine) + 2 * sizeof(TSubPacketGCAuraRefineInfo);
				Packet.bSubHeader = AURA_REFINE_SUBHEADER_GC_INFO;

				TAuraRefineInfo RefineInfo = __GetAuraRefineInfo(AttachedPos);
				SubPacketA.bInfoType = AURA_REFINE_INFO_SLOT_CURRENT;
				SubPacketA.bInfoLevel = RefineInfo.bLevel;
				SubPacketA.bInfoExpPercent = RefineInfo.bExpPercent;

				RefineInfo = __GetAuraEvolvedRefineInfo(AttachedPos);
				SubPacketB.bInfoType = AURA_REFINE_INFO_SLOT_EVOLVED;
				SubPacketB.bInfoLevel = RefineInfo.bLevel;
				SubPacketB.bInfoExpPercent = RefineInfo.bExpPercent;

				TEMP_BUFFER TempBuffer;
				TempBuffer.write(&Packet, sizeof(TPacketGCAuraRefine));
				TempBuffer.write(&SubPacketA, sizeof(TSubPacketGCAuraRefineInfo));
				TempBuffer.write(&SubPacketB, sizeof(TSubPacketGCAuraRefineInfo));
				pDesc->Packet(TempBuffer.read_peek(), TempBuffer.size());
			}
		}
		break;
	}
}

void CHARACTER::AuraRefineWindowCheckOut(BYTE bType, TItemPos SelectedPos)
{
	if (SelectedPos.window_type != AURA_REFINE || SelectedPos.cell >= AURA_SLOT_RESULT)
		return;

	if (m_bAuraRefineWindowType != bType)
		return;

	if (m_pAuraRefineWindowItemSlot[SelectedPos.cell] == NPOS)
		return;

	const LPDESC pDesc = GetDesc();
	if (pDesc == nullptr)
	{
		sys_err("AuraRefineWindowCheckOut(bType=%d, SelectedPos=TItemPos(%d, %d)): NULL DESC CHAR %s",
			bType, SelectedPos.window_type, SelectedPos.cell, GetName());
		return;
	}

	if (!IsAuraRefineWindowOpen())
		return;

	if (!IsAuraRefineWindowCanRefine())
		if (!IsAuraRefineWindowOpen() || !GetAuraRefineWindowOpener())
			return;

	if (SelectedPos.cell == AURA_SLOT_MAIN && m_pAuraRefineWindowItemSlot[AURA_SLOT_SUB] != NPOS)
	{
		const LPITEM pMainItem = GetItem(m_pAuraRefineWindowItemSlot[SelectedPos.cell]), pSubItem = GetItem(m_pAuraRefineWindowItemSlot[AURA_SLOT_SUB]);
		if (!pMainItem || !pSubItem)
			return;

		pMainItem->Lock(false);
		pSubItem->Lock(false);

		m_pAuraRefineWindowItemSlot[SelectedPos.cell] = NPOS;
		m_pAuraRefineWindowItemSlot[AURA_SLOT_SUB] = NPOS;

		TPacketGCAuraRefine Packet;
		Packet.wSize = sizeof(TPacketGCAuraRefine);
		Packet.bSubHeader = AURA_REFINE_SUBHEADER_GC_CLEAR_ALL;
		pDesc->Packet(&Packet, sizeof(TPacketGCAuraRefine));
	}
	else
	{
		const LPITEM pItem = GetItem(m_pAuraRefineWindowItemSlot[SelectedPos.cell]);
		if (pItem == nullptr)
			return;

		pItem->Lock(false);
		m_pAuraRefineWindowItemSlot[SelectedPos.cell] = NPOS;

		TPacketGCAuraRefine Packet;
		Packet.wSize = sizeof(TPacketGCAuraRefine) + sizeof(TSubPacketGCAuraRefineClearSlot);
		Packet.bSubHeader = AURA_REFINE_SUBHEADER_GC_CLEAR_SLOT;

		TSubPacketGCAuraRefineClearSlot SubPacket;
		SubPacket.bSlotIndex = SelectedPos.cell;

		TEMP_BUFFER TempBuffer;
		TempBuffer.write(&Packet, sizeof(TPacketGCAuraRefine));
		TempBuffer.write(&SubPacket, sizeof(TSubPacketGCAuraRefineClearSlot));
		pDesc->Packet(TempBuffer.read_peek(), TempBuffer.size());
	}
}

void CHARACTER::AuraRefineWindowAccept(BYTE bType)
{
	if (m_bAuraRefineWindowType != bType)
		return;

	if (m_pAuraRefineWindowItemSlot[AURA_SLOT_MAIN] == NPOS || m_pAuraRefineWindowItemSlot[AURA_SLOT_SUB] == NPOS)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_STRING("[Aura] You have to insert items."));
		return;
	}

	const LPDESC pDesc = GetDesc();
	if (pDesc == nullptr)
	{
		sys_err("AcceRefineWindowAccept(bType=%d): NULL DESC char %s", bType, GetName());
		return;
	}

	if (!IsAuraRefineWindowOpen())
		return;

	if (!IsAuraRefineWindowCanRefine())
		if (!IsAuraRefineWindowOpen() || !GetAuraRefineWindowOpener())
			return;

	const LPITEM pMainItem = GetItem(m_pAuraRefineWindowItemSlot[AURA_SLOT_MAIN]);
	if (!pMainItem || !m_pAuraRefineWindowItemSlot[AURA_SLOT_MAIN].IsValidItemPosition())
		return;

	const LPITEM pSubItem = GetItem(m_pAuraRefineWindowItemSlot[AURA_SLOT_SUB]);
	if (!pSubItem || !m_pAuraRefineWindowItemSlot[AURA_SLOT_MAIN].IsValidItemPosition())
		return;

	if (pMainItem->IsExchanging() || pSubItem->IsExchanging())
		return;

	if (pMainItem->IsEquipped() || pSubItem->IsEquipped())
		return;

#if defined(__SOUL_BIND_SYSTEM__)
	if (pMainItem->IsSealed() || pSubItem->IsSealed())
	{
		ChatPacket(CHAT_TYPE_INFO, LC_STRING("[Aura] Does not work with soulbound items."));
		return;
	}
#endif

	switch (m_bAuraRefineWindowType)
	{
		case AURA_WINDOW_TYPE_ABSORB:
		{
			pMainItem->Lock(false);

			const long lLevelSocket = pMainItem->GetSocket(ITEM_SOCKET_AURA_LEVEL_VALUE);

			pSubItem->CopySocketTo(pMainItem);
			pSubItem->CopyAttributeTo(pMainItem);
#if defined(__ITEM_APPLY_RANDOM__)
			pSubItem->CopyRandomAppliesTo(pMainItem);
#endif
#if defined(__REFINE_ELEMENT_SYSTEM__)
			pSubItem->CopyElementTo(pMainItem);
#endif

			pMainItem->SetSocket(ITEM_SOCKET_AURA_DRAIN_ITEM_VNUM, pSubItem->GetOriginalVnum());
			pMainItem->SetSocket(ITEM_SOCKET_AURA_LEVEL_VALUE, lLevelSocket);
#if defined(__SET_ITEM__)
			pMainItem->SetItemSetValue(pSubItem->GetItemSetValue());
#endif
			pMainItem->UpdatePacket();

			ITEM_MANAGER::Instance().RemoveItem(pSubItem, "AURA SUB ITEM REMOVE (ABSORPTION SUCCESS)");

			m_pAuraRefineWindowItemSlot[AURA_SLOT_MAIN] = NPOS;
			m_pAuraRefineWindowItemSlot[AURA_SLOT_SUB] = NPOS;

			TPacketGCAuraRefine Packet;
			Packet.wSize = sizeof(TPacketGCAuraRefine);
			Packet.bSubHeader = AURA_REFINE_SUBHEADER_GC_CLEAR_ALL;
			pDesc->Packet(&Packet, sizeof(TPacketGCAuraRefine));
		}
		break;

		case AURA_WINDOW_TYPE_GROWTH:
		{
			const long lLevelSocket = pMainItem->GetSocket(ITEM_SOCKET_AURA_LEVEL_VALUE);
			BYTE bNextLevel = (lLevelSocket / 100000) - 1000;
			const int* aiAuraRefineTable = GetAuraRefineInfo(bNextLevel);
			WORD wCurExp = lLevelSocket % 100000;

			if (bNextLevel == aiAuraRefineTable[AURA_REFINE_INFO_LEVEL_MAX] && wCurExp == aiAuraRefineTable[AURA_REFINE_INFO_NEED_EXP])
			{
				ChatPacket(CHAT_TYPE_INFO, LC_STRING("[Aura] Cannot be improved."));
				if (aiAuraRefineTable[AURA_REFINE_INFO_STEP] == AURA_GRADE_RADIANT)
					ChatPacket(CHAT_TYPE_INFO, LC_STRING("[Aura] Your Aura Outfit has reached the maximum level."));

				return;
			}

			int iAdditionalExp = pSubItem->GetCount() * pSubItem->GetValue(ITEM_AURA_MATERIAL_EXP_VALUE);
			WORD wUsedItemCount = 0;
			while (true)
			{
				wUsedItemCount += 1;
				int iNeedExp = aiAuraRefineTable[AURA_REFINE_INFO_NEED_EXP] - wCurExp;

				if (wCurExp + iAdditionalExp < aiAuraRefineTable[AURA_REFINE_INFO_NEED_EXP])
				{
					wCurExp += iAdditionalExp;
					iAdditionalExp = 0;
					wUsedItemCount = pSubItem->GetCount();
					break;
				}

				if (bNextLevel >= aiAuraRefineTable[AURA_REFINE_INFO_LEVEL_MAX])
				{
					if (wCurExp + pSubItem->GetValue(ITEM_AURA_MATERIAL_EXP_VALUE) >= aiAuraRefineTable[AURA_REFINE_INFO_NEED_EXP])
					{
						wCurExp += iNeedExp;
						iAdditionalExp -= iNeedExp;
						break;
					}
				}

				if (wCurExp + pSubItem->GetValue(ITEM_AURA_MATERIAL_EXP_VALUE) >= aiAuraRefineTable[AURA_REFINE_INFO_NEED_EXP])
				{
					wCurExp = 0;
					iAdditionalExp -= iNeedExp;
					bNextLevel += 1;
					continue;
				}

				wCurExp += pSubItem->GetValue(ITEM_AURA_MATERIAL_EXP_VALUE);
				iAdditionalExp -= pSubItem->GetValue(ITEM_AURA_MATERIAL_EXP_VALUE);
			}

			pMainItem->Lock(false);
			pMainItem->SetSocket(ITEM_SOCKET_AURA_LEVEL_VALUE, (1000 + bNextLevel) * 100000 + wCurExp);
			pMainItem->UpdatePacket();

			if (bNextLevel == aiAuraRefineTable[AURA_REFINE_INFO_LEVEL_MAX] && wCurExp == aiAuraRefineTable[AURA_REFINE_INFO_NEED_EXP])
				ChatPacket(CHAT_TYPE_INFO, LC_STRING("[Aura] Improvement successful. Evolution is now possible."));
			else
				ChatPacket(CHAT_TYPE_INFO, LC_STRING("[Aura] Improvement successful!"));

			m_pAuraRefineWindowItemSlot[AURA_SLOT_MAIN] = NPOS;
			m_pAuraRefineWindowItemSlot[AURA_SLOT_SUB] = NPOS;

			TPacketGCAuraRefine Packet;
			Packet.wSize = sizeof(TPacketGCAuraRefine);
			Packet.bSubHeader = AURA_REFINE_SUBHEADER_GC_CLEAR_ALL;
			pDesc->Packet(&Packet, sizeof(TPacketGCAuraRefine));

			if (IS_SET(pSubItem->GetFlag(), ITEM_FLAG_STACKABLE) && !IS_SET(pSubItem->GetAntiFlag(), ITEM_ANTIFLAG_STACK) && pSubItem->GetCount() > wUsedItemCount)
			{
				pSubItem->Lock(false);
				pSubItem->SetCount(pSubItem->GetCount() - wUsedItemCount);
			}
			else
				ITEM_MANAGER::Instance().RemoveItem(pSubItem, "AURA SUB ITEM REMOVE (GROWTH SUCCESS)");
		}
		break;

		case AURA_WINDOW_TYPE_EVOLVE:
		{
			const long lLevelSocket = pMainItem->GetSocket(ITEM_SOCKET_AURA_LEVEL_VALUE);
			BYTE bNextLevel = (lLevelSocket / 100000) - 1000;
			const int* aiAuraRefineTable = GetAuraRefineInfo(bNextLevel);
			WORD wCurExp = lLevelSocket % 100000;

			if (bNextLevel < aiAuraRefineTable[AURA_REFINE_INFO_LEVEL_MAX] || wCurExp != aiAuraRefineTable[AURA_REFINE_INFO_NEED_EXP] || aiAuraRefineTable[AURA_REFINE_INFO_STEP] == AURA_GRADE_RADIANT)
			{
				ChatPacket(CHAT_TYPE_INFO, LC_STRING("[Aura] Cannot be evolved."));
				if (aiAuraRefineTable[AURA_REFINE_INFO_STEP] == AURA_GRADE_RADIANT)
					ChatPacket(CHAT_TYPE_INFO, LC_STRING("[Aura] Your Aura Outfit has reached the maximum level."));

				return;
			}

			const int iPrice = static_cast<int>(aiAuraRefineTable[AURA_REFINE_INFO_NEED_GOLD]);
			if (GetGold() < iPrice)
			{
				ChatPacket(CHAT_TYPE_INFO, LC_STRING("[Aura] You don’t have enough money to evolve your Aura Outfit."));
				return;
			}

			if (CountSpecifyItem(aiAuraRefineTable[AURA_REFINE_INFO_MATERIAL_VNUM]) < aiAuraRefineTable[AURA_REFINE_INFO_MATERIAL_COUNT])
				return;

			const BYTE bRefinePct = aiAuraRefineTable[AURA_REFINE_INFO_EVOLVE_PCT];
			DWORD dwRefinedAuraVnum = pMainItem->GetRefineSet() == 409 ? pMainItem->GetRefinedVnum() : pMainItem->GetOriginalVnum();
			if (number(1, 100) <= bRefinePct)
			{
				const LPITEM pNewItem = ITEM_MANAGER::Instance().CreateItem(dwRefinedAuraVnum, 1, 0, false);
				if (pNewItem)
				{
					pMainItem->Lock(false);
					pMainItem->CopySocketTo(pNewItem);
					pMainItem->CopyAttributeTo(pNewItem);
#if defined(__ITEM_APPLY_RANDOM__)
					pMainItem->CopyRandomAppliesTo(pNewItem);
#endif
#if defined(__REFINE_ELEMENT_SYSTEM__)
					pMainItem->CopyElementTo(pNewItem);
#endif

					pNewItem->SetSocket(ITEM_SOCKET_AURA_LEVEL_VALUE, (1000 + bNextLevel + 1) * 100000);
#if defined(__SET_ITEM__)
					pNewItem->SetItemSetValue(pMainItem->GetItemSetValue());
#endif

					ITEM_MANAGER::Instance().RemoveItem(pMainItem, "AURA MAIN ITEM REMOVE (EVOLVE SUCCESS)");

					pNewItem->AddToCharacter(this, m_pAuraRefineWindowItemSlot[AURA_SLOT_MAIN]);
					ITEM_MANAGER::Instance().FlushDelayedSave(pNewItem);

					if (IS_SET(pSubItem->GetFlag(), ITEM_FLAG_STACKABLE) && !IS_SET(pSubItem->GetAntiFlag(), ITEM_ANTIFLAG_STACK) && pSubItem->GetCount() > aiAuraRefineTable[AURA_REFINE_INFO_MATERIAL_COUNT])
					{
						pSubItem->Lock(false);
						pSubItem->SetCount(pSubItem->GetCount() - aiAuraRefineTable[AURA_REFINE_INFO_MATERIAL_COUNT]);
					}
					else
						ITEM_MANAGER::Instance().RemoveItem(pSubItem, "AURA SUB ITEM REMOVE (EVOLVE SUCCESS)");

					PointChange(POINT_GOLD, -iPrice, true);
					ChatPacket(CHAT_TYPE_INFO, LC_STRING("[Aura] Evolution successful!"));

					m_pAuraRefineWindowItemSlot[AURA_SLOT_MAIN] = NPOS;
					m_pAuraRefineWindowItemSlot[AURA_SLOT_SUB] = NPOS;

					TPacketGCAuraRefine Packet;
					Packet.wSize = sizeof(TPacketGCAuraRefine);
					Packet.bSubHeader = AURA_REFINE_SUBHEADER_GC_CLEAR_ALL;
					pDesc->Packet(&Packet, sizeof(TPacketGCAuraRefine));
				}
				else
				{
					sys_err("AuraRefineWindowAccept:: ITEM VNUM %d cannot be created for CHAR %s, Window Type %d",
						dwRefinedAuraVnum, GetName(), bType);
				}
			}
			else
			{
				if (IS_SET(pSubItem->GetFlag(), ITEM_FLAG_STACKABLE) && !IS_SET(pSubItem->GetAntiFlag(), ITEM_ANTIFLAG_STACK) && pSubItem->GetCount() > aiAuraRefineTable[AURA_REFINE_INFO_MATERIAL_COUNT])
				{
					pSubItem->SetCount(pSubItem->GetCount() - aiAuraRefineTable[AURA_REFINE_INFO_MATERIAL_COUNT]);
				}
				else
					ITEM_MANAGER::Instance().RemoveItem(pSubItem, "AURA SUB ITEM REMOVE (EVOLVE FAIL)");

				pMainItem->Lock(false);
				m_pAuraRefineWindowItemSlot[AURA_SLOT_SUB] = NPOS;

				TPacketGCAuraRefine Packet;
				Packet.wSize = sizeof(TPacketGCAuraRefine) + sizeof(TSubPacketGCAuraRefineClearSlot);
				Packet.bSubHeader = AURA_REFINE_SUBHEADER_GC_CLEAR_SLOT;

				TSubPacketGCAuraRefineClearSlot SubPacket;
				SubPacket.bSlotIndex = AURA_SLOT_SUB;

				TEMP_BUFFER TempBuffer;
				TempBuffer.write(&Packet, sizeof(TPacketGCAuraRefine));
				TempBuffer.write(&SubPacket, sizeof(TSubPacketGCAuraRefineClearSlot));
				pDesc->Packet(TempBuffer.read_peek(), TempBuffer.size());

				ChatPacket(CHAT_TYPE_INFO, LC_STRING("Upgrade change failed."));
			}
		}
		break;
	}
}

void CHARACTER::ModifyAuraPoints(const LPITEM& rAuraItem, bool bAdd)
{
	float fDrainPer = 0.0f;

	const long lLevelSocket = rAuraItem->GetSocket(ITEM_SOCKET_AURA_LEVEL_VALUE);
	const long lDrainSocket = rAuraItem->GetSocket(ITEM_SOCKET_AURA_DRAIN_ITEM_VNUM);

	BYTE bCurLevel = (lLevelSocket / 100000) - 1000;
	fDrainPer = (1.0f * bCurLevel / 10.0f) / 100.0f;

	TItemTable* pDrainedItem = nullptr;
	if (lDrainSocket != 0)
		pDrainedItem = ITEM_MANAGER::Instance().GetTable(lDrainSocket);

	if (pDrainedItem != nullptr && (pDrainedItem->bType == ITEM_ARMOR))
	{
		switch (pDrainedItem->bSubType)
		{
			case ARMOR_SHIELD:
			case ARMOR_WRIST:
			case ARMOR_NECK:
			case ARMOR_EAR:
			{
				for (BYTE bApplyIndex = 0; bApplyIndex < ITEM_APPLY_MAX_NUM; ++bApplyIndex)
				{
					if (pDrainedItem->aApplies[bApplyIndex].wType == APPLY_NONE || pDrainedItem->aApplies[bApplyIndex].wType == APPLY_MOUNT || pDrainedItem->aApplies[bApplyIndex].wType == APPLY_MOV_SPEED || pDrainedItem->aApplies[bApplyIndex].lValue <= 0)
						continue;

					float fValue = pDrainedItem->aApplies[bApplyIndex].lValue * fDrainPer;
					int iValue = static_cast<int>((fValue < 1.0f) ? ceilf(fValue) : truncf(fValue));
					if (pDrainedItem->aApplies[bApplyIndex].wType == APPLY_SKILL)
						ApplyPoint(pDrainedItem->aApplies[bApplyIndex].wType, bAdd ? iValue : iValue ^ 0x00800000);
#if defined(__ITEM_APPLY_RANDOM__)
					else if (pDrainedItem->aApplies[bApplyIndex].wType == APPLY_RANDOM)
					{
						const TPlayerItemAttribute& rRandomApply = rAuraItem->GetRandomApply(bApplyIndex);
						if (rRandomApply.lValue <= 0)
							continue;

						fValue = rRandomApply.lValue * fDrainPer;
						iValue = static_cast<int>((fValue < 1.0f) ? ceilf(fValue) : truncf(fValue));
						if (rRandomApply.wType == APPLY_SKILL)
							ApplyPoint(rRandomApply.wType, bAdd ? iValue : iValue ^ 0x00800000);
						else
							ApplyPoint(rRandomApply.wType, bAdd ? iValue : -iValue);
					}
#endif
					else
						ApplyPoint(pDrainedItem->aApplies[bApplyIndex].wType, bAdd ? iValue : -iValue);
				}
			}
			break;
		}
	}

	for (BYTE bAttrIndex = 0; bAttrIndex < ITEM_ATTRIBUTE_MAX_NUM; ++bAttrIndex)
	{
		if (rAuraItem->GetAttributeType(bAttrIndex))
		{
			const TPlayerItemAttribute& rItemAttribute = rAuraItem->GetAttribute(bAttrIndex);
			if (rItemAttribute.lValue <= 0)
				continue;

			float fValue = rItemAttribute.lValue * fDrainPer;
			int iDrainValue = static_cast<int>((fValue < 1.0f) ? ceilf(fValue) : truncf(fValue));
			if (rItemAttribute.wType == APPLY_SKILL)
				ApplyPoint(rItemAttribute.wType, bAdd ? iDrainValue : iDrainValue ^ 0x00800000);
			else
				ApplyPoint(rItemAttribute.wType, bAdd ? iDrainValue : -iDrainValue);
		}
	}
}

TAuraRefineInfo CHARACTER::__GetAuraRefineInfo(TItemPos ItemPos)
{
	TAuraRefineInfo RefineInfo = {};
	const LPITEM pItem = GetItem(ItemPos);
	if (pItem == nullptr)
	{
		RefineInfo.bLevel = 0;
		RefineInfo.bExpPercent = 0;
		return RefineInfo;
	}

	const long lLevelSocket = pItem->GetSocket(ITEM_SOCKET_AURA_LEVEL_VALUE);
	RefineInfo.bLevel = (lLevelSocket / 100000) - 1000;
	const int* aiAuraRefineTable = GetAuraRefineInfo(RefineInfo.bLevel);
	RefineInfo.bExpPercent = static_cast<BYTE>((lLevelSocket % 100000) * 1.0f / aiAuraRefineTable[AURA_REFINE_INFO_NEED_EXP] * 100);
	return RefineInfo;
}

TAuraRefineInfo CHARACTER::__CalcAuraRefineInfo(TItemPos MainPos, TItemPos SubPos)
{
	TAuraRefineInfo RefineInfo = {};
	const LPITEM pMainItem = GetItem(MainPos);
	const LPITEM pSubItem = GetItem(SubPos);
	if (pMainItem == nullptr || pSubItem == nullptr)
	{
		RefineInfo.bLevel = 0;
		RefineInfo.bExpPercent = 0;
		return RefineInfo;
	}

	const long lLevelSocket = pMainItem->GetSocket(ITEM_SOCKET_AURA_LEVEL_VALUE);
	BYTE bNextLevel = (lLevelSocket / 100000) - 1000;
	const int* aiAuraRefineTable = GetAuraRefineInfo(bNextLevel);
	WORD wCurExp = lLevelSocket % 100000;
	int iAdditionalExp = pSubItem->GetCount() * pSubItem->GetValue(ITEM_AURA_MATERIAL_EXP_VALUE);
	WORD wUsedItemCount = 0;
	while (true)
	{
		wUsedItemCount += 1;
		int iNeedExp = aiAuraRefineTable[AURA_REFINE_INFO_NEED_EXP] - wCurExp;

		if (wCurExp + iAdditionalExp < aiAuraRefineTable[AURA_REFINE_INFO_NEED_EXP])
		{
			wCurExp += iAdditionalExp;
			iAdditionalExp = 0;
			wUsedItemCount = pSubItem->GetCount();
			break;
		}

		if (bNextLevel >= aiAuraRefineTable[AURA_REFINE_INFO_LEVEL_MAX])
		{
			if (wCurExp + pSubItem->GetValue(ITEM_AURA_MATERIAL_EXP_VALUE) >= aiAuraRefineTable[AURA_REFINE_INFO_NEED_EXP])
			{
				wCurExp += iNeedExp;
				iAdditionalExp -= iNeedExp;
				break;
			}
		}

		if (wCurExp + pSubItem->GetValue(ITEM_AURA_MATERIAL_EXP_VALUE) >= aiAuraRefineTable[AURA_REFINE_INFO_NEED_EXP])
		{
			wCurExp = 0;
			iAdditionalExp -= iNeedExp;
			bNextLevel += 1;
			continue;
		}

		wCurExp += pSubItem->GetValue(ITEM_AURA_MATERIAL_EXP_VALUE);
		iAdditionalExp -= pSubItem->GetValue(ITEM_AURA_MATERIAL_EXP_VALUE);
	}

	RefineInfo.bLevel = bNextLevel;
	RefineInfo.bExpPercent = static_cast<BYTE>(wCurExp * 1.0f / aiAuraRefineTable[AURA_REFINE_INFO_NEED_EXP] * 100);
	return RefineInfo;
}

TAuraRefineInfo CHARACTER::__GetAuraEvolvedRefineInfo(TItemPos ItemPos)
{
	TAuraRefineInfo RefineInfo = {};
	RefineInfo.bLevel = 0;
	RefineInfo.bExpPercent = 0;

	const LPITEM pItem = GetItem(ItemPos);
	if (pItem == nullptr)
		return RefineInfo;

	const long lLevelSocket = pItem->GetSocket(ITEM_SOCKET_AURA_LEVEL_VALUE);
	BYTE bCurLevel = (lLevelSocket / 100000) - 1000;
	const int* aiAuraRefineTable = GetAuraRefineInfo(bCurLevel);
	WORD wCurExp = lLevelSocket % 100000;

	if (bCurLevel < aiAuraRefineTable[AURA_REFINE_INFO_LEVEL_MAX] || wCurExp < aiAuraRefineTable[AURA_REFINE_INFO_NEED_EXP])
		return RefineInfo;

	if (aiAuraRefineTable[AURA_REFINE_INFO_STEP] == AURA_GRADE_RADIANT)
		return RefineInfo;

	RefineInfo.bLevel = bCurLevel + 1;
	return RefineInfo;
}
#endif
