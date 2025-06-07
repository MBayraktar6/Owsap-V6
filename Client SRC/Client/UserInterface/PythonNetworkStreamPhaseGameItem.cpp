#include "StdAfx.h"
#include "PythonNetworkStream.h"
#include "PythonItem.h"
#include "PythonShop.h"
#include "PythonExchange.h"
#include "PythonSafeBox.h"
#include "PythonCharacterManager.h"

#include "AbstractPlayer.h"
#include "PythonBackground.h"

//////////////////////////////////////////////////////////////////////////
// SafeBox

bool CPythonNetworkStream::SendSafeBoxMoneyPacket(BYTE byState, DWORD dwMoney)
{
	assert(!"CPythonNetworkStream::SendSafeBoxMoneyPacket - 사용하지 않는 함수");
	return false;

	/*
	TPacketCGSafeboxMoney kSafeboxMoney;
	kSafeboxMoney.bHeader = HEADER_CG_SAFEBOX_MONEY;
	kSafeboxMoney.bState = byState;
	kSafeboxMoney.dwMoney = dwMoney;
	if (!Send(sizeof(kSafeboxMoney), &kSafeboxMoney))
		return false;

	return SendSequence();
	*/
}

bool CPythonNetworkStream::SendSafeBoxCheckinPacket(TItemPos InventoryPos, BYTE bySafeBoxPos)
{
	__PlayInventoryItemDropSound(InventoryPos);

	TPacketCGSafeboxCheckin kSafeboxCheckin;
	kSafeboxCheckin.bHeader = HEADER_CG_SAFEBOX_CHECKIN;
	kSafeboxCheckin.ItemPos = InventoryPos;
	kSafeboxCheckin.bSafePos = bySafeBoxPos;
	if (!Send(sizeof(kSafeboxCheckin), &kSafeboxCheckin))
		return false;

	return SendSequence();
}

bool CPythonNetworkStream::SendSafeBoxCheckoutPacket(BYTE bySafeBoxPos, TItemPos InventoryPos)
{
	__PlaySafeBoxItemDropSound(bySafeBoxPos);

	TPacketCGSafeboxCheckout kSafeboxCheckout;
	kSafeboxCheckout.bHeader = HEADER_CG_SAFEBOX_CHECKOUT;
	kSafeboxCheckout.bSafePos = bySafeBoxPos;
	kSafeboxCheckout.ItemPos = InventoryPos;
	if (!Send(sizeof(kSafeboxCheckout), &kSafeboxCheckout))
		return false;

	return SendSequence();
}

bool CPythonNetworkStream::SendSafeBoxItemMovePacket(BYTE bySourcePos, BYTE byTargetPos, WORD wCount)
{
	__PlaySafeBoxItemDropSound(bySourcePos);

	TPacketCGItemMove kItemMove;
	kItemMove.header = HEADER_CG_SAFEBOX_ITEM_MOVE;
	kItemMove.pos = TItemPos(INVENTORY, bySourcePos);
	kItemMove.num = wCount;
	kItemMove.change_pos = TItemPos(INVENTORY, byTargetPos);
	if (!Send(sizeof(kItemMove), &kItemMove))
		return false;

	return SendSequence();
}

bool CPythonNetworkStream::RecvSafeBoxSetPacket()
{
	TPacketGCItemSet kItemSet;
	if (!Recv(sizeof(kItemSet), &kItemSet))
		return false;

	TItemData kItemData;
	kItemData.dwVnum = kItemSet.dwVnum;
	kItemData.dwCount = kItemSet.dwCount;
	kItemData.dwFlags = kItemSet.dwFlags;
	kItemData.dwAntiFlags = kItemSet.dwAntiFlags;
#if defined(ENABLE_SOULBIND_SYSTEM)
	kItemData.lSealDate = kItemSet.lSealDate;
#endif
	for (BYTE bSocketPos = 0; bSocketPos < ITEM_SOCKET_SLOT_MAX_NUM; ++bSocketPos)
		kItemData.alSockets[bSocketPos] = kItemSet.alSockets[bSocketPos];
	for (BYTE bAttrPos = 0; bAttrPos < ITEM_ATTRIBUTE_SLOT_MAX_NUM; ++bAttrPos)
		kItemData.aAttr[bAttrPos] = kItemSet.aAttr[bAttrPos];
#if defined(ENABLE_CHANGE_LOOK_SYSTEM)
	kItemData.dwTransmutationVnum = kItemSet.dwTransmutationVnum;
#endif
#if defined(ENABLE_REFINE_ELEMENT_SYSTEM)
	kItemData.RefineElement = kItemSet.RefineElement;
#endif
#if defined(ENABLE_APPLY_RANDOM)
	for (BYTE bApplyPos = 0; bApplyPos < ITEM_APPLY_RANDOM_SLOT_MAX_NUM; ++bApplyPos)
		kItemData.aApplyRandom[bApplyPos] = kItemSet.aApplyRandom[bApplyPos];
#endif
#if defined(ENABLE_SET_ITEM)
	kItemData.bSetValue = kItemSet.bSetValue;
#endif

	CPythonSafeBox::Instance().SetItemData(kItemSet.Cell.cell, kItemData);

	__RefreshSafeboxWindow();

	return true;
}

bool CPythonNetworkStream::RecvSafeBoxDelPacket()
{
	TPacketGCItemDel kItemDel;
	if (!Recv(sizeof(kItemDel), &kItemDel))
		return false;

	CPythonSafeBox::Instance().DelItemData(kItemDel.pos);

	__RefreshSafeboxWindow();

	return true;
}

bool CPythonNetworkStream::RecvSafeBoxWrongPasswordPacket()
{
	TPacketGCSafeboxWrongPassword kSafeboxWrongPassword;

	if (!Recv(sizeof(kSafeboxWrongPassword), &kSafeboxWrongPassword))
		return false;

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OnSafeBoxError", Py_BuildValue("()"));

	return true;
}

bool CPythonNetworkStream::RecvSafeBoxSizePacket()
{
	TPacketGCSafeboxSize kSafeBoxSize;
	if (!Recv(sizeof(kSafeBoxSize), &kSafeBoxSize))
		return false;

	CPythonSafeBox::Instance().OpenSafeBox(kSafeBoxSize.bSize);
	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OpenSafeboxWindow", Py_BuildValue("(i)", kSafeBoxSize.bSize));

	return true;
}

// SafeBox
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Mall
bool CPythonNetworkStream::SendMallCheckoutPacket(BYTE byMallPos, TItemPos InventoryPos)
{
	__PlayMallItemDropSound(byMallPos);

	TPacketCGMallCheckout kMallCheckoutPacket;
	kMallCheckoutPacket.bHeader = HEADER_CG_MALL_CHECKOUT;
	kMallCheckoutPacket.bMallPos = byMallPos;
	kMallCheckoutPacket.ItemPos = InventoryPos;
	if (!Send(sizeof(kMallCheckoutPacket), &kMallCheckoutPacket))
		return false;

	return SendSequence();
}

bool CPythonNetworkStream::RecvMallOpenPacket()
{
	TPacketGCMallOpen kMallOpen;
	if (!Recv(sizeof(kMallOpen), &kMallOpen))
		return false;

	CPythonSafeBox::Instance().OpenMall(kMallOpen.bSize);
	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OpenMallWindow", Py_BuildValue("(i)", kMallOpen.bSize));

	return true;
}
bool CPythonNetworkStream::RecvMallItemSetPacket()
{
	TPacketGCItemSet kItemSet;
	if (!Recv(sizeof(kItemSet), &kItemSet))
		return false;

	TItemData kItemData;
	kItemData.dwVnum = kItemSet.dwVnum;
	kItemData.dwCount = kItemSet.dwCount;
	kItemData.dwFlags = kItemSet.dwFlags;
	kItemData.dwAntiFlags = kItemSet.dwAntiFlags;
#if defined(ENABLE_SOULBIND_SYSTEM)
	kItemData.lSealDate = kItemSet.lSealDate;
#endif
	for (BYTE bSocketPos = 0; bSocketPos < ITEM_SOCKET_SLOT_MAX_NUM; ++bSocketPos)
		kItemData.alSockets[bSocketPos] = kItemSet.alSockets[bSocketPos];
	for (BYTE bAttrPos = 0; bAttrPos < ITEM_ATTRIBUTE_SLOT_MAX_NUM; ++bAttrPos)
		kItemData.aAttr[bAttrPos] = kItemSet.aAttr[bAttrPos];
#if defined(ENABLE_CHANGE_LOOK_SYSTEM)
	kItemData.dwTransmutationVnum = kItemSet.dwTransmutationVnum;
#endif
#if defined(ENABLE_REFINE_ELEMENT_SYSTEM)
	kItemData.RefineElement = kItemSet.RefineElement;
#endif
#if defined(ENABLE_APPLY_RANDOM)
	for (BYTE bApplyPos = 0; bApplyPos < ITEM_APPLY_RANDOM_SLOT_MAX_NUM; ++bApplyPos)
		kItemData.aApplyRandom[bApplyPos] = kItemSet.aApplyRandom[bApplyPos];
#endif
#if defined(ENABLE_SET_ITEM)
	kItemData.bSetValue = kItemSet.bSetValue;
#endif

	CPythonSafeBox::Instance().SetMallItemData(kItemSet.Cell.cell, kItemData);

	__RefreshMallWindow();

	return true;
}
bool CPythonNetworkStream::RecvMallItemDelPacket()
{
	TPacketGCItemDel kItemDel;
	if (!Recv(sizeof(kItemDel), &kItemDel))
		return false;

	CPythonSafeBox::Instance().DelMallItemData(kItemDel.pos);

	__RefreshMallWindow();
	Tracef(" >> CPythonNetworkStream::RecvMallItemDelPacket\n");

	return true;
}
// Mall
//////////////////////////////////////////////////////////////////////////

// Item
// Recieve
bool CPythonNetworkStream::RecvItemSetEmptyPacket()
{
	TPacketGCItemSetEmpty packet_item_set;
	if (!Recv(sizeof(TPacketGCItemSetEmpty), &packet_item_set))
		return false;

	TItemData kItemData;
	kItemData.dwVnum = packet_item_set.dwVnum;
	kItemData.dwCount = packet_item_set.dwCount;
	kItemData.dwFlags = 0;
	for (BYTE bSocketPos = 0; bSocketPos < ITEM_SOCKET_SLOT_MAX_NUM; ++bSocketPos)
		kItemData.alSockets[bSocketPos] = packet_item_set.alSockets[bSocketPos];
	for (BYTE bAttrPos = 0; bAttrPos < ITEM_ATTRIBUTE_SLOT_MAX_NUM; ++bAttrPos)
		kItemData.aAttr[bAttrPos] = packet_item_set.aAttr[bAttrPos];
#if defined(ENABLE_CHANGE_LOOK_SYSTEM)
	kItemData.dwTransmutationVnum = packet_item_set.dwTransmutationVnum;
#endif
#if defined(ENABLE_REFINE_ELEMENT_SYSTEM)
	kItemData.RefineElement = packet_item_set.RefineElement;
#endif
#if defined(ENABLE_APPLY_RANDOM)
	for (BYTE bApplyPos = 0; bApplyPos < ITEM_APPLY_RANDOM_SLOT_MAX_NUM; ++bApplyPos)
		kItemData.aApplyRandom[bApplyPos] = packet_item_set.aApplyRandom[bApplyPos];
#endif
#if defined(ENABLE_SET_ITEM)
	kItemData.bSetValue = packet_item_set.bSetValue;
#endif

	IAbstractPlayer& rkPlayer = IAbstractPlayer::GetSingleton();

	rkPlayer.SetItemData(packet_item_set.Cell, kItemData);

	__RefreshInventoryWindow();
	return true;
}

bool CPythonNetworkStream::RecvItemSetPacket()
{
	TPacketGCItemSet packet_item_set;
	if (!Recv(sizeof(TPacketGCItemSet), &packet_item_set))
		return false;

	TItemData kItemData;
	kItemData.dwVnum = packet_item_set.dwVnum;
	kItemData.dwCount = packet_item_set.dwCount;
	kItemData.dwFlags = packet_item_set.dwFlags;
	kItemData.dwAntiFlags = packet_item_set.dwAntiFlags;
#if defined(ENABLE_SOULBIND_SYSTEM)
	kItemData.lSealDate = packet_item_set.lSealDate;
#endif
	for (BYTE bSocketPos = 0; bSocketPos < ITEM_SOCKET_SLOT_MAX_NUM; ++bSocketPos)
		kItemData.alSockets[bSocketPos] = packet_item_set.alSockets[bSocketPos];
	for (BYTE bAttrPos = 0; bAttrPos < ITEM_ATTRIBUTE_SLOT_MAX_NUM; ++bAttrPos)
		kItemData.aAttr[bAttrPos] = packet_item_set.aAttr[bAttrPos];
#if defined(ENABLE_CHANGE_LOOK_SYSTEM)
	kItemData.dwTransmutationVnum = packet_item_set.dwTransmutationVnum;
#endif
#if defined(ENABLE_REFINE_ELEMENT_SYSTEM)
	kItemData.RefineElement = packet_item_set.RefineElement;
#endif
#if defined(ENABLE_APPLY_RANDOM)
	for (BYTE bApplyPos = 0; bApplyPos < ITEM_APPLY_RANDOM_SLOT_MAX_NUM; ++bApplyPos)
		kItemData.aApplyRandom[bApplyPos] = packet_item_set.aApplyRandom[bApplyPos];
#endif
#if defined(ENABLE_SET_ITEM)
	kItemData.bSetValue = packet_item_set.bSetValue;
#endif

	IAbstractPlayer& rkPlayer = IAbstractPlayer::GetSingleton();
	rkPlayer.SetItemData(packet_item_set.Cell, kItemData);

	if (packet_item_set.bHighLight)
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_Highlight_Item", Py_BuildValue("(ii)", packet_item_set.Cell.window_type, packet_item_set.Cell.cell));

	__RefreshInventoryWindow();
	return true;
}

bool CPythonNetworkStream::RecvItemUsePacket()
{
	TPacketGCItemUse packet_item_use;

	if (!Recv(sizeof(TPacketGCItemUse), &packet_item_use))
		return false;

	__RefreshInventoryWindow();
	return true;
}

bool CPythonNetworkStream::RecvItemUpdatePacket()
{
	TPacketGCItemUpdate packet_item_update;
	if (!Recv(sizeof(TPacketGCItemUpdate), &packet_item_update))
		return false;

	IAbstractPlayer& rkPlayer = IAbstractPlayer::GetSingleton();
	rkPlayer.SetItemCount(packet_item_update.Cell, packet_item_update.dwCount);
#if defined(ENABLE_SOULBIND_SYSTEM)
	rkPlayer.SealItem(packet_item_update.Cell, packet_item_update.lSealDate);
#endif

	for (BYTE bSocketPos = 0; bSocketPos < ITEM_SOCKET_SLOT_MAX_NUM; ++bSocketPos)
		rkPlayer.SetItemMetinSocket(packet_item_update.Cell, bSocketPos, packet_item_update.alSockets[bSocketPos]);
	for (BYTE bAttrPos = 0; bAttrPos < ITEM_ATTRIBUTE_SLOT_MAX_NUM; ++bAttrPos)
		rkPlayer.SetItemAttribute(packet_item_update.Cell, bAttrPos, packet_item_update.aAttr[bAttrPos].wType, packet_item_update.aAttr[bAttrPos].lValue);
#if defined(ENABLE_CHANGE_LOOK_SYSTEM)
	rkPlayer.SetItemTransmutationVnum(packet_item_update.Cell, packet_item_update.dwTransmutationVnum);
#endif
#if defined(ENABLE_REFINE_ELEMENT_SYSTEM)
	rkPlayer.SetItemRefineElement(packet_item_update.Cell, packet_item_update.RefineElement);
#endif
#if defined(ENABLE_APPLY_RANDOM)
	for (BYTE bApplyPos = 0; bApplyPos < ITEM_APPLY_RANDOM_SLOT_MAX_NUM; ++bApplyPos)
		rkPlayer.SetItemApplyRandom(packet_item_update.Cell, bApplyPos, packet_item_update.aApplyRandom[bApplyPos].wType, packet_item_update.aApplyRandom[bApplyPos].lValue);
#endif
#if defined(ENABLE_SET_ITEM)
	rkPlayer.SetItemSetValue(packet_item_update.Cell, packet_item_update.bSetValue);
#endif

	__RefreshInventoryWindow();
	return true;
}

bool CPythonNetworkStream::RecvItemGroundAddPacket()
{
	TPacketGCItemGroundAdd recv;
	if (!Recv(sizeof(TPacketGCItemGroundAdd), &recv))
		return false;

	__GlobalPositionToLocalPosition(recv.lX, recv.lY);

	CPythonItem::Instance().CreateItem(recv.dwVID, recv.dwVnum
#if defined(ENABLE_SET_ITEM)
		, recv.bSetValue
#endif		
		, recv.lX, recv.lY, recv.lZ, true
#if defined(ENABLE_ITEM_DROP_RENEWAL)
		, recv.alSockets
		, recv.aAttrs
#endif
	);

	return true;
}

bool CPythonNetworkStream::RecvItemOwnership()
{
	TPacketGCItemOwnership p;

	if (!Recv(sizeof(TPacketGCItemOwnership), &p))
		return false;

	CPythonItem::Instance().SetOwnership(p.dwVID, p.szName);
	return true;
}

bool CPythonNetworkStream::RecvItemGroundDelPacket()
{
	TPacketGCItemGroundDel	packet_item_ground_del;

	if (!Recv(sizeof(TPacketGCItemGroundDel), &packet_item_ground_del))
		return false;

	CPythonItem::Instance().DeleteItem(packet_item_ground_del.vid);
	return true;
}

bool CPythonNetworkStream::RecvQuickSlotAddPacket()
{
	TPacketGCQuickSlotAdd packet_quick_slot_add;

	if (!Recv(sizeof(TPacketGCQuickSlotAdd), &packet_quick_slot_add))
		return false;

	IAbstractPlayer& rkPlayer = IAbstractPlayer::GetSingleton();
	rkPlayer.AddQuickSlot(packet_quick_slot_add.pos, packet_quick_slot_add.slot.Type, packet_quick_slot_add.slot.Position);

	__RefreshInventoryWindow();

	return true;
}

bool CPythonNetworkStream::RecvQuickSlotDelPacket()
{
	TPacketGCQuickSlotDel packet_quick_slot_del;

	if (!Recv(sizeof(TPacketGCQuickSlotDel), &packet_quick_slot_del))
		return false;

	IAbstractPlayer& rkPlayer = IAbstractPlayer::GetSingleton();
	rkPlayer.DeleteQuickSlot(packet_quick_slot_del.pos);

	__RefreshInventoryWindow();

	return true;
}

bool CPythonNetworkStream::RecvQuickSlotMovePacket()
{
	TPacketGCQuickSlotSwap packet_quick_slot_swap;

	if (!Recv(sizeof(TPacketGCQuickSlotSwap), &packet_quick_slot_swap))
		return false;

	IAbstractPlayer& rkPlayer = IAbstractPlayer::GetSingleton();
	rkPlayer.MoveQuickSlot(packet_quick_slot_swap.pos, packet_quick_slot_swap.change_pos);

	__RefreshInventoryWindow();

	return true;
}

bool CPythonNetworkStream::SendShopEndPacket()
{
	if (!__CanActMainInstance())
		return true;

	TPacketCGShop packet_shop;
	packet_shop.header = HEADER_CG_SHOP;
	packet_shop.subheader = SHOP_SUBHEADER_CG_END;

	if (!Send(sizeof(packet_shop), &packet_shop))
	{
		Tracef("SendShopEndPacket Error\n");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendShopBuyPacket(BYTE bPos)
{
	if (!__CanActMainInstance())
		return true;

	TPacketCGShop PacketShop;
	PacketShop.header = HEADER_CG_SHOP;
	PacketShop.subheader = SHOP_SUBHEADER_CG_BUY;

	if (!Send(sizeof(TPacketCGShop), &PacketShop))
	{
		Tracef("SendShopBuyPacket Error\n");
		return false;
	}

	BYTE bCount = 1;
	if (!Send(sizeof(BYTE), &bCount))
	{
		Tracef("SendShopBuyPacket Error\n");
		return false;
	}

	if (!Send(sizeof(BYTE), &bPos))
	{
		Tracef("SendShopBuyPacket Error\n");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendShopSellPacket(BYTE bySlot)
{
	if (!__CanActMainInstance())
		return true;

	TPacketCGShop PacketShop;
	PacketShop.header = HEADER_CG_SHOP;
	PacketShop.subheader = SHOP_SUBHEADER_CG_SELL;

	if (!Send(sizeof(TPacketCGShop), &PacketShop))
	{
		Tracef("SendShopSellPacket Error\n");
		return false;
	}
	if (!Send(sizeof(BYTE), &bySlot))
	{
		Tracef("SendShopAddSellPacket Error\n");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendShopSellPacketNew(WORD wSlot, WORD wCount, BYTE byType)
{
	if (!__CanActMainInstance())
		return true;

	TPacketCGShop PacketShop;
	PacketShop.header = HEADER_CG_SHOP;
	PacketShop.subheader = SHOP_SUBHEADER_CG_SELL2;

	if (!Send(sizeof(TPacketCGShop), &PacketShop))
	{
		Tracef("SendShopSellPacket Error\n");
		return false;
	}
	if (!Send(sizeof(WORD), &wSlot))
	{
		Tracef("SendShopAddSellPacket Error\n");
		return false;
	}

	if (!Send(sizeof(WORD), &wCount))
	{
		Tracef("SendShopAddSellPacket Error\n");
		return false;
	}
	if (!Send(sizeof(BYTE), &byType))
	{
		Tracef("SendShopAddSellPacket Error\n");
		return false;
	}

	Tracef(" SendShopSellPacketNew(wSlot=%d, byCount=%d, byType=%d)\n", wSlot, wCount, byType);

	return SendSequence();
}

// Send
bool CPythonNetworkStream::SendItemUsePacket(TItemPos pos)
{
	if (!__CanActMainInstance())
		return true;

	if (__IsEquipItemInSlot(pos))
	{
		if (CPythonExchange::Instance().IsTrading())
		{
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_AppendNotifyMessage", Py_BuildValue("(s)", "CANNOT_EQUIP_EXCHANGE"));
			return true;
		}

		if (CPythonShop::Instance().IsOpen())
		{
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_AppendNotifyMessage", Py_BuildValue("(s)", "CANNOT_EQUIP_SHOP"));
			return true;
		}

		if (__IsPlayerAttacking())
			return true;
	}

	__PlayInventoryItemUseSound(pos);

	TPacketCGItemUse itemUsePacket;
	itemUsePacket.header = HEADER_CG_ITEM_USE;
	itemUsePacket.pos = pos;

	if (!Send(sizeof(TPacketCGItemUse), &itemUsePacket))
	{
		Tracen("SendItemUsePacket Error");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendItemUseToItemPacket(TItemPos source_pos, TItemPos target_pos)
{
	if (!__CanActMainInstance())
		return true;

	TPacketCGItemUseToItem itemUseToItemPacket;
	itemUseToItemPacket.header = HEADER_CG_ITEM_USE_TO_ITEM;
	itemUseToItemPacket.source_pos = source_pos;
	itemUseToItemPacket.target_pos = target_pos;

	if (!Send(sizeof(TPacketCGItemUseToItem), &itemUseToItemPacket))
	{
		Tracen("SendItemUseToItemPacket Error");
		return false;
	}

#ifdef _DEBUG
	Tracef(" << SendItemUseToItemPacket(src=%d, dst=%d)\n", source_pos.cell, target_pos.cell);
#endif

	return SendSequence();
}

#if defined(ENABLE_CHEQUE_SYSTEM)
bool CPythonNetworkStream::SendItemDropPacket(TItemPos pos, DWORD elk, DWORD cheque)
#else
bool CPythonNetworkStream::SendItemDropPacket(TItemPos pos, DWORD elk)
#endif
{
	if (!__CanActMainInstance())
		return true;

	TPacketCGItemDrop itemDropPacket;
	itemDropPacket.header = HEADER_CG_ITEM_DROP;
	itemDropPacket.pos = pos;
	itemDropPacket.elk = elk;
#if defined(ENABLE_CHEQUE_SYSTEM)
	itemDropPacket.cheque = cheque;
#endif

	if (!Send(sizeof(TPacketCGItemDrop), &itemDropPacket))
	{
		Tracen("SendItemDropPacket Error");
		return false;
	}

	return SendSequence();
}

#if defined(ENABLE_CHEQUE_SYSTEM)
bool CPythonNetworkStream::SendItemDropPacketNew(TItemPos pos, DWORD elk, DWORD cheque, DWORD count)
#else
bool CPythonNetworkStream::SendItemDropPacketNew(TItemPos pos, DWORD elk, DWORD count)
#endif
{
	if (!__CanActMainInstance())
		return true;

	TPacketCGItemDrop2 itemDropPacket;
	itemDropPacket.header = HEADER_CG_ITEM_DROP2;
	itemDropPacket.pos = pos;
	itemDropPacket.gold = elk;
#if defined(ENABLE_CHEQUE_SYSTEM)
	itemDropPacket.cheque = cheque;
#endif
	itemDropPacket.count = count;

	if (!Send(sizeof(itemDropPacket), &itemDropPacket))
	{
		Tracen("SendItemDropPacket Error");
		return false;
	}

	return SendSequence();
}

#if defined(ENABLE_NEW_DROP_DIALOG)
bool CPythonNetworkStream::SendItemDestroyPacket(TItemPos pos)
{
	if (!__CanActMainInstance())
		return true;

	TPacketCGItemDestroy itemDestroyPacket;
	itemDestroyPacket.header = HEADER_CG_ITEM_DESTROY;
	itemDestroyPacket.pos = pos;

	if (!Send(sizeof(itemDestroyPacket), &itemDestroyPacket))
	{
		Tracen("SendItemDestroyPacket Error");
		return false;
	}

	return SendSequence();
}
#endif

bool CPythonNetworkStream::__IsEquipItemInSlot(TItemPos uSlotPos)
{
	IAbstractPlayer& rkPlayer = IAbstractPlayer::GetSingleton();
	return rkPlayer.IsEquipItemInSlot(uSlotPos);
}

void CPythonNetworkStream::__PlayInventoryItemUseSound(TItemPos uSlotPos)
{
	IAbstractPlayer& rkPlayer = IAbstractPlayer::GetSingleton();
	DWORD dwItemID = rkPlayer.GetItemIndex(uSlotPos);

	CPythonItem& rkItem = CPythonItem::Instance();
	rkItem.PlayUseSound(dwItemID);
}

void CPythonNetworkStream::__PlayInventoryItemDropSound(TItemPos uSlotPos)
{
	IAbstractPlayer& rkPlayer = IAbstractPlayer::GetSingleton();
	DWORD dwItemID = rkPlayer.GetItemIndex(uSlotPos);

	CPythonItem& rkItem = CPythonItem::Instance();
	rkItem.PlayDropSound(dwItemID);
}

//void CPythonNetworkStream::__PlayShopItemDropSound(UINT uSlotPos)
//{
//	DWORD dwItemID;
//	CPythonShop& rkShop=CPythonShop::Instance();
//	if (!rkShop.GetSlotItemID(uSlotPos, &dwItemID))
//		return;
//	
//	CPythonItem& rkItem=CPythonItem::Instance();
//	rkItem.PlayDropSound(dwItemID);
//}

void CPythonNetworkStream::__PlaySafeBoxItemDropSound(UINT uSlotPos)
{
	DWORD dwItemID;
	CPythonSafeBox& rkSafeBox = CPythonSafeBox::Instance();
	if (!rkSafeBox.GetSlotItemID(uSlotPos, &dwItemID))
		return;

	CPythonItem& rkItem = CPythonItem::Instance();
	rkItem.PlayDropSound(dwItemID);
}

void CPythonNetworkStream::__PlayMallItemDropSound(UINT uSlotPos)
{
	DWORD dwItemID;
	CPythonSafeBox& rkSafeBox = CPythonSafeBox::Instance();
	if (!rkSafeBox.GetSlotMallItemID(uSlotPos, &dwItemID))
		return;

	CPythonItem& rkItem = CPythonItem::Instance();
	rkItem.PlayDropSound(dwItemID);
}

bool CPythonNetworkStream::SendItemMovePacket(TItemPos pos, TItemPos change_pos, WORD num)
{
	if (!__CanActMainInstance())
		return true;

	if (__IsEquipItemInSlot(pos))
	{
		if (CPythonExchange::Instance().IsTrading())
		{
			if (pos.IsEquipCell() || change_pos.IsEquipCell())
			{
				PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_AppendNotifyMessage", Py_BuildValue("(s)", "CANNOT_EQUIP_EXCHANGE"));
				return true;
			}
		}

		if (CPythonShop::Instance().IsOpen())
		{
			if (pos.IsEquipCell() || change_pos.IsEquipCell())
			{
				PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_AppendNotifyMessage", Py_BuildValue("(s)", "CANNOT_EQUIP_SHOP"));
				return true;
			}
		}

		if (__IsPlayerAttacking())
			return true;
	}

	__PlayInventoryItemDropSound(pos);

	TPacketCGItemMove itemMovePacket;
	itemMovePacket.header = HEADER_CG_ITEM_MOVE;
	itemMovePacket.pos = pos;
	itemMovePacket.change_pos = change_pos;
	itemMovePacket.num = num;

	if (!Send(sizeof(TPacketCGItemMove), &itemMovePacket))
	{
		Tracen("SendItemMovePacket Error");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendItemPickUpPacket(DWORD vid)
{
	if (!__CanActMainInstance())
		return true;

	TPacketCGItemPickUp itemPickUpPacket;
	itemPickUpPacket.header = HEADER_CG_ITEM_PICKUP;
	itemPickUpPacket.vid = vid;

	if (!Send(sizeof(TPacketCGItemPickUp), &itemPickUpPacket))
	{
		Tracen("SendItemPickUpPacket Error");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendQuickSlotAddPacket(BYTE wpos, BYTE type, int pos)
{
	if (!__CanActMainInstance())
		return true;

	TPacketCGQuickSlotAdd quickSlotAddPacket;

	quickSlotAddPacket.header = HEADER_CG_QUICKSLOT_ADD;
	quickSlotAddPacket.pos = wpos;
	quickSlotAddPacket.slot.Type = type;
	quickSlotAddPacket.slot.Position = pos;

	if (!Send(sizeof(TPacketCGQuickSlotAdd), &quickSlotAddPacket))
	{
		Tracen("SendQuickSlotAddPacket Error");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendQuickSlotDelPacket(BYTE pos)
{
	if (!__CanActMainInstance())
		return true;

	TPacketCGQuickSlotDel quickSlotDelPacket;

	quickSlotDelPacket.header = HEADER_CG_QUICKSLOT_DEL;
	quickSlotDelPacket.pos = pos;

	if (!Send(sizeof(TPacketCGQuickSlotDel), &quickSlotDelPacket))
	{
		Tracen("SendQuickSlotDelPacket Error");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendQuickSlotMovePacket(BYTE pos, BYTE change_pos)
{
	if (!__CanActMainInstance())
		return true;

	TPacketCGQuickSlotSwap quickSlotSwapPacket;

	quickSlotSwapPacket.header = HEADER_CG_QUICKSLOT_SWAP;
	quickSlotSwapPacket.pos = pos;
	quickSlotSwapPacket.change_pos = change_pos;

	if (!Send(sizeof(TPacketCGQuickSlotSwap), &quickSlotSwapPacket))
	{
		Tracen("SendQuickSlotSwapPacket Error");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::RecvSpecialEffect()
{
	TPacketGCSpecialEffect kSpecialEffect;
	if (!Recv(sizeof(kSpecialEffect), &kSpecialEffect))
		return false;

	int effect = -1;
	bool bPlayPotionSound = false; // 포션을 먹을 경우는 포션 사운드를 출력하자.!!
	bool bAttachEffect = true; // 캐리터에 붙는 어태치 이펙트와 일반 이펙트 구분.!!
	float fScale = 1.0f;

	switch (kSpecialEffect.bEffectNum)
	{
		case SE_HPUP_RED:
			effect = CInstanceBase::EFFECT_HPUP_RED;
			bPlayPotionSound = true;
			break;
		case SE_SPUP_BLUE:
			effect = CInstanceBase::EFFECT_SPUP_BLUE;
			bPlayPotionSound = true;
			break;
		case SE_SPEEDUP_GREEN:
			effect = CInstanceBase::EFFECT_SPEEDUP_GREEN;
			bPlayPotionSound = true;
			break;
		case SE_DXUP_PURPLE:
			effect = CInstanceBase::EFFECT_DXUP_PURPLE;
			bPlayPotionSound = true;
			break;
		case SE_CRITICAL:
			effect = CInstanceBase::EFFECT_CRITICAL;
			break;
		case SE_PENETRATE:
			effect = CInstanceBase::EFFECT_PENETRATE;
			break;
		case SE_BLOCK:
			effect = CInstanceBase::EFFECT_BLOCK;
			break;
		case SE_DODGE:
			effect = CInstanceBase::EFFECT_DODGE;
			break;
		case SE_CHINA_FIREWORK:
			effect = CInstanceBase::EFFECT_FIRECRACKER;
			bAttachEffect = false;
			break;
		case SE_SPIN_TOP:
			effect = CInstanceBase::EFFECT_SPIN_TOP;
			bAttachEffect = false;
			break;
		case SE_SUCCESS:
			effect = CInstanceBase::EFFECT_SUCCESS;
			bAttachEffect = false;
			break;
		case SE_FAIL:
			effect = CInstanceBase::EFFECT_FAIL;
			break;
		case SE_FR_SUCCESS:
			effect = CInstanceBase::EFFECT_FR_SUCCESS;
			bAttachEffect = false;
			break;
		case SE_LEVELUP_ON_14_FOR_GERMANY:	//레벨업 14일때 ( 독일전용 )
			effect = CInstanceBase::EFFECT_LEVELUP_ON_14_FOR_GERMANY;
			bAttachEffect = false;
			break;
		case SE_LEVELUP_UNDER_15_FOR_GERMANY: //레벨업 15일때 ( 독일전용 )
			effect = CInstanceBase::EFFECT_LEVELUP_UNDER_15_FOR_GERMANY;
			bAttachEffect = false;
			break;
		case SE_PERCENT_DAMAGE1:
			effect = CInstanceBase::EFFECT_PERCENT_DAMAGE1;
			break;
		case SE_PERCENT_DAMAGE2:
			effect = CInstanceBase::EFFECT_PERCENT_DAMAGE2;
			break;
		case SE_PERCENT_DAMAGE3:
			effect = CInstanceBase::EFFECT_PERCENT_DAMAGE3;
			break;
		case SE_AUTO_HPUP:
			effect = CInstanceBase::EFFECT_AUTO_HPUP;
			break;
		case SE_AUTO_SPUP:
			effect = CInstanceBase::EFFECT_AUTO_SPUP;
			break;
		case SE_EQUIP_RAMADAN_RING:
			effect = CInstanceBase::EFFECT_RAMADAN_RING_EQUIP;
			break;
		case SE_EQUIP_HALLOWEEN_CANDY:
			effect = CInstanceBase::EFFECT_HALLOWEEN_CANDY_EQUIP;
			break;
		case SE_EQUIP_HAPPINESS_RING:
			effect = CInstanceBase::EFFECT_HAPPINESS_RING_EQUIP;
			break;
		case SE_EQUIP_LOVE_PENDANT:
			effect = CInstanceBase::EFFECT_LOVE_PENDANT_EQUIP;
			break;
#ifdef ENABLE_QUEEN_NETHIS
		case SE_EFFECT_SNAKE_REGEN:
			effect = CInstanceBase::EFFECT_SNAKE_REGEN;
			bAttachEffect = false;
			break;
#endif
#if defined(ENABLE_ACCE_COSTUME_SYSTEM)
		case SE_ACCE_SUCESS_ABSORB:
			effect = CInstanceBase::EFFECT_ACCE_SUCESS_ABSORB;
			break;
		case SE_ACCE_EQUIP:
			effect = CInstanceBase::EFFECT_ACCE_EQUIP;
			break;
		case SE_ACCE_BACK:
			effect = CInstanceBase::EFFECT_ACCE_BACK;
			break;
#endif
		case SE_EQUIP_EASTER_CANDY:
			effect = CInstanceBase::EFFECT_EASTER_CANDY_EQIP;
			break;
		case SE_THUNDER_AREA:
			effect = CInstanceBase::EFFECT_THUNDER_AREA;
			break;
		case SE_THUNDER:
			effect = CInstanceBase::EFFECT_THUNDER;
			break;
		case SE_HEAL:
			effect = CInstanceBase::EFFECT_HEAL;
			break;
		case SE_CAPE_OF_COURAGE:
			effect = CInstanceBase::EFFECT_CAPE_OF_COURAGE;
			break;
		case SE_EQUIP_CHOCOLATE_PENDANT:
			effect = CInstanceBase::EFFECT_CHOCOLATE_PENDANT;
			break;
#if defined(ENABLE_BALANCE_IMPROVING)
		case SE_FEATHER_WALK:
			effect = CInstanceBase::EFFECT_FEATHER_WALK;
			bAttachEffect = false;
			break;
#endif
		case SE_PEPSI_EVENT:
			effect = CInstanceBase::EFFECT_PEPSI_EVENT;
			break;
#if defined(ENABLE_GUILD_DRAGONLAIR_SYSTEM)
		case SE_DRAGONLAIR_STONE_UNBEATABLE_1:
			effect = CInstanceBase::EFFECT_DRAGONLAIR_STONE_UNBEATABLE_1;
			break;
		case SE_DRAGONLAIR_STONE_UNBEATABLE_2:
			effect = CInstanceBase::EFFECT_DRAGONLAIR_STONE_UNBEATABLE_2;
			break;
		case SE_DRAGONLAIR_STONE_UNBEATABLE_3:
			effect = CInstanceBase::EFFECT_DRAGONLAIR_STONE_UNBEATABLE_3;
			break;
#endif
		case SE_BATTLE_POTION:
			effect = CInstanceBase::EFFECT_BATTLE_POTION;
			break;
		case SE_REFLECT:
			effect = CInstanceBase::EFFECT_REFLECT;
			break;
		case SE_SKILL_DAMAGE_ZONE:
			effect = CInstanceBase::EFFECT_SKILL_DAMAGE_ZONE;
			break;
		case SE_SKILL_SAFE_ZONE:
			effect = CInstanceBase::EFFECT_SKILL_SAFE_ZONE;
			break;
		case SE_METEOR:
			effect = CInstanceBase::EFFECT_METEOR;
			break;
		case SE_BEAD_RAIN:
			effect = CInstanceBase::EFFECT_BEAD_RAIN;
			break;
		case SE_ARROW_RAIN:
			effect = CInstanceBase::EFFECT_ARROW_RAIN;
			break;
		case SE_FALL_ROCK:
			effect = CInstanceBase::EFFECT_FALL_ROCK;
			break;
		case SE_HORSE_DROP:
			effect = CInstanceBase::EFFECT_HORSE_DROP;
			break;
		case SE_EGG_DROP:
			effect = CInstanceBase::EFFECT_EGG_DROP;
			break;
		case SE_DEAPO_BOOM:
			effect = CInstanceBase::EFFECT_DEAPO_BOOM;
			break;
#if defined(ENABLE_FLOWER_EVENT)
		case SE_FLOWER_EVENT:
			effect = CInstanceBase::EFFECT_FLOWER_EVENT;
			break;
#endif
#if defined(ENABLE_GEM_SYSTEM)
		case SE_GEM_PENDANT:
			effect = CInstanceBase::EFFECT_GEM_PENDANT;
			break;
#endif
#if defined(ENABLE_DEFENSE_WAVE)
		case SE_DEFENSE_WAVE_LASER:
			effect = CInstanceBase::EFFECT_DEFENSE_WAVE_LASER;
			break;
#endif
		case SE_PET_ATTR_CHANGE_NEW_TYPE:
			effect = CInstanceBase::EFFECT_PET_ATTR_CHANGE_NEW_TYPE;
			break;
		case SE_PET_PAY_SUMMON1:
			effect = CInstanceBase::EFFECT_PET_PAY_SUMMON1;
			break;
#if defined(ENABLE_SUMMER_EVENT_ROULETTE)
		case SE_SPECIAL_ROULETTE:
			effect = CInstanceBase::EFFECT_SPECIAL_ROULETTE;
			break;
#endif
		case SE_MISTS_ISLAND_0:
			effect = CInstanceBase::EFFECT_MISTS_ISLAND_0;
			break;
		case SE_MISTS_ISLAND_1:
			effect = CInstanceBase::EFFECT_MISTS_ISLAND_1;
			break;
		case SE_MISTS_ISLAND_2:
			effect = CInstanceBase::EFFECT_MISTS_ISLAND_2;
			break;
		case SE_MISTS_ISLAND_3:
			effect = CInstanceBase::EFFECT_MISTS_ISLAND_3;
			break;
		case SE_PASSIVE_ATTR_SKILL:
			effect = CInstanceBase::EFFECT_PASSIVE_ATTR_SKILL;
			break;
		case SE_SUICIDE_BOMB_SMALL:
			effect = CInstanceBase::EFFECT_SUICIDE_BOMB_SMALL;
			break;
		case SE_SUICIDE_BOMB_LARGE:
			effect = CInstanceBase::EFFECT_SUICIDE_BOMB_LARGE;
			break;
		case SE_GROUND_THORN:
			effect = CInstanceBase::EFFECT_GROUND_THORN;
			break;
		case SE_GROUND_SNAKE:
			effect = CInstanceBase::EFFECT_GROUND_SNAKE;
			break;
		case SE_WHITE_DRAGON_BERSERK:
			effect = CInstanceBase::EFFECT_WHITE_DRAGON_BERSERK;
			break;
		case SE_WHITE_DRAGON_COOLING:
			effect = CInstanceBase::EFFECT_WHITE_DRAGON_COOLING;
			break;
		case SE_USE_METINSTONE_RAIN_SPAWN_TICKET:
			effect = CInstanceBase::EFFECT_USE_METINSTONE_RAIN_SPAWN_TICKET;
			break;
		case SE_FAKE_METINSTONE_SPLASH_DAMAGE:
			effect = CInstanceBase::EFFECT_FAKE_METINSTONE_SPLASH_DAMAGE;
			break;
		case SE_MOUNT_UPGRADE_SKILL:
			effect = CInstanceBase::EFFECT_MOUNT_UPGRADE_SKILL;
			break;
		case SE_SOUTH_REAPER_SKILL_2:
			effect = CInstanceBase::EFFECT_SOUTH_REAPER_SKILL_2;
			break;
		case SE_YEOMWANG_SKILL_2:
			effect = CInstanceBase::EFFECT_YEOMWANG_SKILL_2;
			break;
		case SE_BUFF_ITEM_5:
			effect = CInstanceBase::EFFECT_BUFF_ITEM_5;
			break;
		case SE_BUFF_ITEM_13:
			effect = CInstanceBase::EFFECT_BUFF_ITEM_13;
			break;
		case SE_BUFF_ITEM_14:
			effect = CInstanceBase::EFFECT_BUFF_ITEM_14;
			break;
		case SE_BUFF_SYMBOL1:
			effect = CInstanceBase::EFFECT_BUFF_SYMBOL1;
			break;
		case SE_BUFF_ITEM_8_ELEMENT_1:
			effect = CInstanceBase::EFFECT_BUFF_ITEM_8_ELEMENT_1;
			break;
		case SE_BUFF_ITEM_8_ELEMENT_2:
			effect = CInstanceBase::EFFECT_BUFF_ITEM_8_ELEMENT_2;
			break;
		case SE_BUFF_FLOWER_OF_GALE:
			effect = CInstanceBase::EFFECT_BUFF_FLOWER_OF_GALE;
			break;
		case SE_BUFF_FLOWER_OF_DESTRUCTION:
			effect = CInstanceBase::EFFECT_BUFF_FLOWER_OF_DESTRUCTION;
			break;
		case SE_SUNGMAHEE_GATE_FLOWER_BLOW_RANGE:
			effect = CInstanceBase::EFFECT_SUNGMAHEE_GATE_FLOWER_BLOW_RANGE;
			break;
		case SE_SUNGMAHEE_GATE_INVINCIBLE_GATE:
			effect = CInstanceBase::EFFECT_SUNGMAHEE_GATE_INVINCIBLE_GATE;
			break;
		case SE_SUNGMAHEE_GATE_NORMAL_GATE:
			effect = CInstanceBase::EFFECT_SUNGMAHEE_GATE_NORMAL_GATE;
			break;
		case SE_MOUNT_UPGRADE_GYEONGGONG:
			effect = CInstanceBase::EFFECT_MOUNT_UPGRADE_GYEONGGONG;
			break;
#if defined(ENABLE_SNOWFLAKE_STICK_EVENT)
		case SE_USE_SNOWFLAKE_STICK:
			effect = CInstanceBase::EFFECT_USE_SNOWFLAKE_STICK;
			break;
#endif
		case SE_SPORTS_MATCH_BUFF_EMBLEM:
			effect = CInstanceBase::EFFECT_SPORTS_MATCH_BUFF_EMBLEM;
			break;
		case SE_POINT_AREA_ELECT_ATTACK:
			effect = CInstanceBase::EFFECT_POINT_AREA_ELECT_ATTACK;
			break;
		default:
			TraceError("%d 는 없는 스페셜 이펙트 번호입니다.TPacketGCSpecialEffect", kSpecialEffect.bEffectNum);
			break;
	}

	switch (kSpecialEffect.bEffectType)
	{
		case SE_TYPE_NORMAL:
		{
			if (bPlayPotionSound)
			{
				IAbstractPlayer& rkPlayer = IAbstractPlayer::GetSingleton();
				if (rkPlayer.IsMainCharacterIndex(kSpecialEffect.dwVID))
				{
					CPythonItem& rkItem = CPythonItem::Instance();
					rkItem.PlayUsePotionSound();
				}
			}

			if (-1 != effect)
			{
				CInstanceBase* pInstance = CPythonCharacterManager::Instance().GetInstancePtr(kSpecialEffect.dwVID);
				if (pInstance)
				{
					if (bAttachEffect)
						pInstance->AttachSpecialEffect(effect, fScale);
					else
						pInstance->CreateSpecialEffect(effect, fScale);
				}
			}
		}
		break;

		case SE_TYPE_POSITION:
		{
			if (-1 != effect)
			{
				CInstanceBase* pInstance = CPythonCharacterManager::Instance().GetInstancePtr(kSpecialEffect.dwVID);
				if (pInstance)
				{
					LONG xEffectPos = kSpecialEffect.xEffectPos;
					LONG yEffectPos = kSpecialEffect.yEffectPos;
					__GlobalPositionToLocalPosition(xEffectPos, yEffectPos);

					float fHeight = CPythonBackground::Instance().GetHeight(
						static_cast<float>(xEffectPos),
						static_cast<float>(yEffectPos)
					) + 60.0f;

					D3DXVECTOR3 v3EffectPos;
					v3EffectPos.x = xEffectPos;
					v3EffectPos.y = -yEffectPos;
					v3EffectPos.z = fHeight;

					pInstance->CreateSpecialPositionEffect(effect, v3EffectPos, fScale);
				}
			}
		}
		break;
	}

	return true;
}

bool CPythonNetworkStream::RecvSpecificEffect()
{
	TPacketGCSpecificEffect kSpecificEffect;
	if (!Recv(sizeof(kSpecificEffect), &kSpecificEffect))
		return false;

	CInstanceBase* pInstance = CPythonCharacterManager::Instance().GetInstancePtr(kSpecificEffect.vid);
	// EFFECT_TEMP
	if (pInstance)
	{
#if defined(ENABLE_GRAPHIC_ON_OFF)
		if (!pInstance->CanRenderActor() || !pInstance->CanRenderEffect())
			return true;
#endif

		CInstanceBase::RegisterEffect(CInstanceBase::EFFECT_TEMP, "", kSpecificEffect.effect_file, false);
		pInstance->AttachSpecialEffect(CInstanceBase::EFFECT_TEMP);
	}

	return true;
}

#if defined(ENABLE_DRAGON_SOUL_SYSTEM)
bool CPythonNetworkStream::RecvDragonSoulRefine()
{
	TPacketGCDragonSoulRefine kDragonSoul;

	if (!Recv(sizeof(kDragonSoul), &kDragonSoul))
		return false;

	switch (kDragonSoul.bSubType)
	{
#if defined(ENABLE_DS_CHANGE_ATTR)
		case DS_SUB_HEADER_OPEN:
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_DragonSoulRefineWindow_Open", Py_BuildValue("(i)", DS_REFINE_TYPE_GRADE));
			break;

		case DS_SUB_HEADER_OPEN_CHANGE_ATTR:
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_DragonSoulRefineWindow_Open", Py_BuildValue("(i)", DS_REFINE_TYPE_CHANGE_ATTR));
			break;
#else
		case DS_SUB_HEADER_OPEN:
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_DragonSoulRefineWindow_Open", Py_BuildValue("()"));
			break;
#endif

		case DS_SUB_HEADER_REFINE_FAIL:
		case DS_SUB_HEADER_REFINE_FAIL_MAX_REFINE:
		case DS_SUB_HEADER_REFINE_FAIL_INVALID_MATERIAL:
		case DS_SUB_HEADER_REFINE_FAIL_NOT_ENOUGH_MONEY:
		case DS_SUB_HEADER_REFINE_FAIL_NOT_ENOUGH_MATERIAL:
		case DS_SUB_HEADER_REFINE_FAIL_TOO_MUCH_MATERIAL:
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_DragonSoulRefineWindow_RefineFail", Py_BuildValue("(iii)",
				kDragonSoul.bSubType, kDragonSoul.Pos.window_type, kDragonSoul.Pos.cell));
			break;

		case DS_SUB_HEADER_REFINE_SUCCEED:
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_DragonSoulRefineWindow_RefineSucceed",
				Py_BuildValue("(ii)", kDragonSoul.Pos.window_type, kDragonSoul.Pos.cell));
			break;
	}

	return true;
}
#endif

#if defined(ENABLE_LUCKY_BOX)
bool CPythonNetworkStream::SendLuckyBoxActionPacket(BYTE bAction)
{
	TPacketCGLuckyBox Packet;
	Packet.bHeader = HEADER_CG_LUCKY_BOX;
	Packet.bAction = bAction;

	if (!Send(sizeof(Packet), &Packet))
		return false;

	return SendSequence();
}

bool CPythonNetworkStream::RecvLuckyBoxPacket()
{
	TPacketGCLuckyBox Packet;
	if (!Recv(sizeof(Packet), &Packet))
	{
		Tracen("RecvLuckyBox Packet Error");
		return false;
	}

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "ShowLuckyBoxWindow", Py_BuildValue("(iiii)", Packet.dwVNum, Packet.bCount, Packet.iNeedMoney, Packet.wSlotIndex));
	return true;
}
#endif

#if defined(ENABLE_ATTR_6TH_7TH)
bool CPythonNetworkStream::SendAttr67AddPacket(const TAttr67AddData* pkAttr67AddData, bool bAddPacket)
{
	TPacketCGAttr67Add packet;
	packet.byHeader = HEADER_CG_ATTR67_ADD;
	if (bAddPacket)
	{
		packet.bySubHeader = SUBHEADER_CG_ATTR67_ADD_REGIST;
		memcpy(&packet.Attr67AddData, pkAttr67AddData, sizeof(TAttr67AddData));
	}
	else
	{
		packet.bySubHeader = SUBHEADER_CG_ATTR67_ADD_OPEN;
		memset(&packet.Attr67AddData, 0, sizeof(TAttr67AddData));
	}

	if (!Send(sizeof(packet), &packet))
		return false;

	return SendSequence();
}

bool CPythonNetworkStream::SendAttr67ClosePacket()
{
	TPacketCGAttr67Add packet;
	packet.byHeader = HEADER_CG_ATTR67_ADD;
	packet.bySubHeader = SUBHEADER_CG_ATTR67_ADD_CLOSE;
	memset(&packet.Attr67AddData, 0, sizeof(TAttr67AddData));

	if (!Send(sizeof(packet), &packet))
		return false;

	return SendSequence();
}
#endif
