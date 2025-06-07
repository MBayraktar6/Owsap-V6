/*
* Filename: MailBox.cpp
* Author: blackdragonx61 (aka. Mali)
*/

#include "stdafx.h"

#if defined(__MAILBOX__)
#include "MailBox.h"
#include "char.h"
#include "char_manager.h"
#include "desc.h"
#include "packet.h"
#include "item.h"
#include "item_manager.h"
#include "banword.h"
#include "buffer_manager.h"
#include "db.h"
#include "config.h"
#include "desc_client.h"
#include "log.h"
#if defined(__MESSENGER_BLOCK_SYSTEM__)
#include "messenger_manager.h"
#endif

CMailBox::CMailBox(const LPCHARACTER m_ch, const TMailBoxTable* pTable, const WORD Size)
	: Owner(m_ch)
{
	for (WORD i = 0; i < Size; ++i, ++pTable)
		vecMailBox.emplace_back(*pTable);

	/* Load */
	TEMP_BUFFER buf;
	for (const TMailBoxTable& mail : vecMailBox)
	{
		TPacketGCMailBoxMessage p;
		p.SendTime = mail.Message.SendTime;
		p.DeleteTime = mail.Message.DeleteTime;
		std::memcpy(&p.szTitle, mail.Message.szTitle, sizeof(p.szTitle));
		p.bIsGMPost = mail.Message.bIsGMPost;
		p.bIsItemExist = mail.Message.bIsItemExist;
		p.bIsConfirm = mail.Message.bIsConfirm;
		buf.write(&p, sizeof(p));
	}

	TPacketGCMailBox pack;
	pack.bHeader = HEADER_GC_MAILBOX;
	pack.wSize = sizeof(pack) + buf.size();
	if (buf.size())
	{
		Owner->GetDesc()->BufferedPacket(&pack, sizeof(pack));
		Owner->GetDesc()->Packet(buf.read_peek(), buf.size());
	}
	else
		Owner->GetDesc()->Packet(&pack, sizeof(pack));

	ServerProcess(EMAILBOX_GC::MAILBOX_GC_OPEN, true);
}

CMailBox::~CMailBox()
{
	ServerProcess(EMAILBOX_GC::MAILBOX_GC_CLOSE);
	Owner->SetMailBoxLoading(false);
	Owner->SetMyMailBoxTime();
}

void CMailBox::ServerProcess(const BYTE SubHeader, const BYTE arg1, const BYTE arg2) const
{
	if (Owner == nullptr)
		return;

	const LPDESC d = Owner->GetDesc();
	if (d == nullptr)
		return;

	TPacketMailboxProcess p;
	p.bHeader = HEADER_GC_MAILBOX_PROCESS;
	p.bSubHeader = SubHeader;
	p.bArg1 = arg1;
	p.bArg2 = arg2;
	d->Packet(&p, sizeof(p));
}

void CMailBox::Write(const char* const szName, const char* const szTitle, const char* const szMessage,
	const TItemPos& pos, const int iYang, const int iWon)
{
	const LPDESC d = Owner->GetDesc();
	if (d == nullptr)
		return;

	const int TotalYang = iYang + EMAILBOX::MAILBOX_PRICE_YANG;
	if (TotalYang > Owner->GetGold())
	{
		ServerProcess(EMAILBOX_GC::MAILBOX_GC_POST_WRITE, EMAILBOX_POST_WRITE::POST_WRITE_YANG_NOT_ENOUGHT);
		return;
	}

#if defined(__CHEQUE_SYSTEM__)
	if (iWon > Owner->GetCheque())
	{
		ServerProcess(EMAILBOX_GC::MAILBOX_GC_POST_WRITE, EMAILBOX_POST_WRITE::POST_WRITE_WON_NOT_ENOUGHT);
		return;
	}
#endif

	if (CBanwordManager::instance().CheckString(szName, strlen(szName)))
	{
		ServerProcess(EMAILBOX_GC::MAILBOX_GC_POST_WRITE, EMAILBOX_POST_WRITE::POST_WRITE_INVALID_NAME);
		return;
	}

	if (CBanwordManager::instance().CheckString(szTitle, strlen(szTitle)))
	{
		ServerProcess(EMAILBOX_GC::MAILBOX_GC_POST_WRITE, EMAILBOX_POST_WRITE::POST_WRITE_WRONG_TITLE);
		return;
	}

	if (CBanwordManager::instance().CheckString(szMessage, strlen(szMessage)))
	{
		ServerProcess(EMAILBOX_GC::MAILBOX_GC_POST_WRITE, EMAILBOX_POST_WRITE::POST_WRITE_WRONG_MESSAGE);
		return;
	}

	TMailBoxTable p;
	std::memcpy(p.szName, szName, sizeof(p.szName));
	p.bIsDeleted = false;

	p.AddData.bHeader = 0;
	p.AddData.Index = 0;
	std::memcpy(p.AddData.szFrom, Owner->GetName(), sizeof(p.AddData.szFrom));
	std::memcpy(p.AddData.szMessage, szMessage, sizeof(p.AddData.szMessage));
	p.AddData.iYang = iYang;
	p.AddData.iWon = iWon;
	p.AddData.dwItemVnum = 0;
	p.AddData.dwItemCount = 0;
	memset(p.AddData.alSockets, 0, sizeof(p.AddData.alSockets));
	memset(p.AddData.aAttr, 0, sizeof(p.AddData.aAttr));
#if defined(__CHANGE_LOOK_SYSTEM__)
	p.AddData.dwChangeLookVnum = 0;
#endif
#if defined(__REFINE_ELEMENT_SYSTEM__)
	memset(&p.AddData.RefineElement, 0, sizeof(p.AddData.RefineElement));
#endif
#if defined(__ITEM_APPLY_RANDOM__)
	memset(p.AddData.aApplyRandom, 0, sizeof(p.AddData.aApplyRandom));
#endif
#if defined(__SET_ITEM__)
	p.AddData.bSetValue = 0;
#endif

	p.Message.SendTime = time(nullptr);
	p.Message.DeleteTime = p.Message.SendTime + (p.Message.bIsGMPost ? EMAILBOX::MAILBOX_REMAIN_DAY_GM : EMAILBOX::MAILBOX_REMAIN_DAY) * 60 * 60 * 24;
	std::memcpy(p.Message.szTitle, szTitle, sizeof(p.Message.szTitle));
	p.Message.bIsGMPost = Owner->IsGM();
	p.Message.bIsConfirm = false;

	if (pos.IsValidItemPosition())
	{
		if (pos.IsEquipPosition())
		{
			ServerProcess(EMAILBOX_GC::MAILBOX_GC_POST_WRITE, EMAILBOX_POST_WRITE::POST_WRITE_WRONG_ITEM);
			return;
		}

		const LPITEM pItem = Owner->GetItem(pos);
		if (!pItem || pItem->IsExchanging() || pItem->isLocked()
#if defined(__SOUL_BIND_SYSTEM__)
			|| pItem->IsSealed()
#endif
			)
		{
			ServerProcess(EMAILBOX_GC::MAILBOX_GC_POST_WRITE, EMAILBOX_POST_WRITE::POST_WRITE_WRONG_ITEM);
			return;
		}

		p.AddData.dwItemVnum = pItem->GetVnum();
		p.AddData.dwItemCount = pItem->GetCount();
		std::memcpy(p.AddData.alSockets, pItem->GetSockets(), sizeof(p.AddData.alSockets));
		std::memcpy(p.AddData.aAttr, pItem->GetAttributes(), sizeof(p.AddData.aAttr));
#if defined(__CHANGE_LOOK_SYSTEM__)
		p.AddData.dwChangeLookVnum = pItem->GetTransmutationVnum();
#endif
#if defined(__REFINE_ELEMENT_SYSTEM__)
		std::memcpy(&p.AddData.RefineElement, pItem->GetRefineElement(), sizeof(p.AddData.RefineElement));
#endif
#if defined(__ITEM_APPLY_RANDOM__)
		std::memcpy(p.AddData.aApplyRandom, pItem->GetRandomApplies(), sizeof(p.AddData.aApplyRandom));
#endif
#if defined(__SET_ITEM__)
		p.AddData.bSetValue = pItem->GetItemSetValue();
#endif

		pItem->RemoveFromCharacter();
		ITEM_MANAGER::instance().DestroyItem(pItem);
	}

	p.Message.bIsItemExist = p.AddData.dwItemVnum > 0 || p.AddData.iYang > 0 || p.AddData.iWon > 0;

	Owner->PointChange(POINT_GOLD, -TotalYang);
#if defined(__CHEQUE_SYSTEM__)
	Owner->PointChange(POINT_CHEQUE, -iWon);
#endif

	db_clientdesc->DBPacket(HEADER_GD_MAILBOX_WRITE, d->GetHandle(), &p, sizeof(p));
	ServerProcess(EMAILBOX_GC::MAILBOX_GC_POST_WRITE, EMAILBOX_POST_WRITE::POST_WRITE_OK);

	LogManager::Instance().MailLog(szName, Owner->GetName(), szTitle, szMessage, p.Message.bIsGMPost, p.AddData.dwItemVnum, p.AddData.dwItemCount, p.AddData.iYang, p.AddData.iWon);
}

/* static */ void CMailBox::Open(const LPCHARACTER ch)
{
	if (ch == nullptr)
		return;

	const LPDESC d = ch->GetDesc();
	if (d == nullptr)
		return;

	if (ch->GetLevel() < EMAILBOX::MAILBOX_LEVEL_LIMIT)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_STRING("문자 메세지 전송은 레벨 20부터 할 수 있습니다."));
		return;
	}

	if (ch->PreventTradeWindow(WND_ALL))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_STRING("Please close all other windows to open your mailbox."));
		return;
	}

	if (ch->IsMailBoxLoading())
		return;

	const int iPulse = thecore_pulse();
	if (iPulse - ch->GetMyMailBoxTime() < PASSES_PER_SEC(10))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_STRING("You must wait a while before opening your mailbox again."));
		return;
	}

	ch->SetMailBoxLoading(true);

	/* Request */
	TMailBox p;
	std::memcpy(p.szName, ch->GetName(), sizeof(p.szName));
	p.Index = 0;
	db_clientdesc->DBPacket(HEADER_GD_MAILBOX_LOAD, d->GetHandle(), &p, sizeof(p));
}

/* static */ void CMailBox::Create(const LPCHARACTER ch, const TMailBoxTable* pTable, const WORD Size)
{
	if (ch == nullptr)
		return;

	if (ch->GetMailBox())
		return;

	if (ch->IsMailBoxLoading() == false)
		return;

	ch->SetMailBoxLoading(false);
	ch->SetMailBox(new CMailBox(ch, pTable, Size));
	ch->SetMyMailBoxTime();
}

/* static */ void CMailBox::UnreadData(const LPCHARACTER ch)
{
	if (ch == nullptr)
		return;

	const LPDESC d = ch->GetDesc();
	if (d == nullptr)
		return;

	/* Request */
	TMailBox p;
	std::memcpy(p.szName, ch->GetName(), sizeof(p.szName));
	p.Index = 0;
	db_clientdesc->DBPacket(HEADER_GD_MAILBOX_UNREAD, d->GetHandle(), &p, sizeof(p));
}

/* static */ void CMailBox::ResultUnreadData(const LPCHARACTER ch, TMailBoxRespondUnreadData* data)
{
	if (data == nullptr)
		return;

	if (ch == nullptr)
		return;

	const LPDESC d = ch->GetDesc();
	if (d == nullptr)
		return;

	data->bHeader = HEADER_GC_MAILBOX_UNREAD;
	d->Packet(&(*data), sizeof(TMailBoxRespondUnreadData));
}

/* static */ void CMailBox::SendGMMail(const char* const cPlayerName, const char* const cTitle, const char* const cMessage, const DWORD dwItemVnum, const DWORD dwItemCount, const int iYang, const int iWon)
{
	TMailBoxTable p;
	std::memcpy(p.szName, cPlayerName, sizeof(p.szName));
	p.bIsDeleted = false;

	p.AddData.bHeader = 0;
	p.AddData.Index = 0;
	static /*constexpr*/ const char* GM_NAME = "[GM]";
	std::memcpy(p.AddData.szFrom, GM_NAME, sizeof(p.AddData.szFrom));
	std::memcpy(p.AddData.szMessage, cMessage, sizeof(p.AddData.szMessage));
	p.AddData.iYang = iYang;
	p.AddData.iWon = iWon;
	p.AddData.dwItemVnum = dwItemVnum;
	p.AddData.dwItemCount = dwItemCount;
	memset(p.AddData.alSockets, 0, sizeof(p.AddData.alSockets));
	memset(p.AddData.aAttr, 0, sizeof(p.AddData.aAttr));
#if defined(__CHANGE_LOOK_SYSTEM__)
	p.AddData.dwChangeLookVnum = 0;
#endif
#if defined(__REFINE_ELEMENT_SYSTEM__)
	memset(&p.AddData.RefineElement, 0, sizeof(p.AddData.RefineElement));
#endif
#if defined(__ITEM_APPLY_RANDOM__)
	memset(p.AddData.aApplyRandom, 0, sizeof(p.AddData.aApplyRandom));
#endif
#if defined(__SET_ITEM__)
	p.AddData.bSetValue = 0;
#endif

	p.Message.SendTime = time(nullptr);
	p.Message.DeleteTime = p.Message.SendTime + (p.Message.bIsGMPost ? EMAILBOX::MAILBOX_REMAIN_DAY_GM : EMAILBOX::MAILBOX_REMAIN_DAY) * 60 * 60 * 24;
	std::memcpy(p.Message.szTitle, cTitle, sizeof(p.Message.szTitle));
	p.Message.bIsGMPost = true;
	p.Message.bIsItemExist = p.AddData.dwItemVnum > 0 || p.AddData.iYang > 0 || p.AddData.iWon > 0;
	p.Message.bIsConfirm = false;

	db_clientdesc->DBPacket(HEADER_GD_MAILBOX_WRITE, 0, &p, sizeof(p));

	CMailBox::UnreadData(CHARACTER_MANAGER::Instance().FindPC(cPlayerName));

	LogManager::Instance().MailLog(cPlayerName, GM_NAME, cTitle, cMessage, dwItemVnum, p.Message.bIsGMPost, dwItemCount, p.AddData.iYang, p.AddData.iWon);
}

void CMailBox::CheckPlayer(const char* const szName) const
{
	const LPDESC d = Owner->GetDesc();
	if (d == nullptr)
		return;

	if (Owner->IsMailBoxLoading())
		return;

	// NOTE: You can send mail to yourself at official game.
	/*if (!strcasecmp(Owner->GetName(), szName))
	{
		ServerProcess(EMAILBOX_GC::MAILBOX_GC_POST_WRITE_CONFIRM, POST_WRITE_FAIL);
		return;
	}*/

	Owner->SetMailBoxLoading(true);

	/* Request */
	TMailBox p;
	std::memcpy(p.szName, szName, sizeof(p.szName));
	p.Index = 0;
	db_clientdesc->DBPacket(HEADER_GD_MAILBOX_CHECK_NAME, d->GetHandle(), &p, sizeof(p));
}

void CMailBox::CheckPlayerResult(const TMailBox* t)
{
	if (Owner->IsMailBoxLoading() == false)
		return;

	Owner->SetMailBoxLoading(false);

	if (t == nullptr)
		return;

	BYTE arg = EMAILBOX_POST_WRITE::POST_WRITE_INVALID_NAME;
	if (strlen(t->szName) != 0)
	{
		arg = EMAILBOX_POST_WRITE::POST_WRITE_OK;
		if (t->Index >= MAILBOX_MAX_MAIL)
			arg = EMAILBOX_POST_WRITE::POST_WRITE_FULL_MAILBOX;

#if defined(__MESSENGER_BLOCK_SYSTEM__)
		if (CMessengerManager::instance().IsBlocked(Owner->GetName(), t->szName))
			arg = EMAILBOX_POST_WRITE::POST_WRITE_TARGET_BLOCKED;
		else if (CMessengerManager::instance().IsBlocked(t->szName, Owner->GetName()))
			arg = EMAILBOX_POST_WRITE::POST_WRITE_BLOCKED_ME;
#endif
	}

	ServerProcess(EMAILBOX_GC::MAILBOX_GC_POST_WRITE_CONFIRM, arg);
}

void CMailBox::AddData(const BYTE button_idx, const BYTE data_idx)
{
	const LPDESC d = Owner->GetDesc();
	if (d == nullptr)
		return;

	if (data_idx >= vecMailBox.size())
		return;

	TMailBoxTable& mail = vecMailBox.at(data_idx);
	if (mail.bIsDeleted)
		return;

	/* Notify DB */
	TMailBox p;
	std::memcpy(p.szName, Owner->GetName(), sizeof(p.szName));
	p.Index = data_idx;
	db_clientdesc->DBPacket(HEADER_GD_MAILBOX_CONFIRM, Owner->GetDesc()->GetHandle(), &p, sizeof(p));

	mail.AddData.bHeader = HEADER_GC_MAILBOX_ADD_DATA;
	mail.AddData.Index = data_idx;
	mail.Message.bIsConfirm = true;
	d->Packet(&mail.AddData, sizeof(mail.AddData));
	ServerProcess(EMAILBOX_GC::MAILBOX_GC_ADD_DATA, button_idx, data_idx);
}

void CMailBox::GetAllItems()
{
	const LPDESC d = Owner->GetDesc();
	if (d == nullptr)
		return;

	if (vecMailBox.empty())
	{
		ServerProcess(EMAILBOX_GC::MAILBOX_GC_POST_ALL_GET_ITEMS, EMAILBOX_POST_ALL_GET_ITEMS::POST_ALL_GET_ITEMS_EMPTY);
		return;
	}

	TEMP_BUFFER buf;

	for (size_t i = 0; i < vecMailBox.size(); i++)
	{
		const bool bReceive = GetItem(static_cast<BYTE>(i), true);
		if (bReceive == false)
			continue;

		TPacketGCMailboxProcessAll p;
		p.Index = static_cast<BYTE>(i);
		buf.write(&p, sizeof(p));
	}

	TPacketGCMailBox pack;
	pack.bHeader = HEADER_GC_MAILBOX_ALL;
	pack.wSize = sizeof(pack) + buf.size();
	if (buf.size())
	{
		d->BufferedPacket(&pack, sizeof(pack));
		d->Packet(buf.read_peek(), buf.size());
	}
	else
		d->Packet(&pack, sizeof(pack));
}

bool CMailBox::GetItem(const BYTE data_idx, const bool bAll)
{
	if (data_idx >= vecMailBox.size())
	{
		if (bAll == false)
			ServerProcess(EMAILBOX_GC::MAILBOX_GC_POST_GET_ITEMS, data_idx, EMAILBOX_POST_GET_ITEMS::POST_GET_ITEMS_FAIL);
		return false;
	}

	TMailBoxTable& mail = vecMailBox.at(data_idx);
	if (mail.bIsDeleted)
	{
		if (bAll == false)
			ServerProcess(EMAILBOX_GC::MAILBOX_GC_POST_GET_ITEMS, data_idx, EMAILBOX_POST_GET_ITEMS::POST_GET_ITEMS_NONE);
		return false;
	}

	if (mail.Message.bIsItemExist == false)
	{
		if (bAll == false)
			ServerProcess(EMAILBOX_GC::MAILBOX_GC_POST_GET_ITEMS, data_idx, EMAILBOX_POST_GET_ITEMS::POST_GET_ITEMS_NONE);
		return false;
	}

	static_assert(EMAILBOX::MAILBOX_TAX >= 0 && EMAILBOX::MAILBOX_TAX <= 100, "EMAILBOX::MAILBOX_TAX must be: [0-100]");
	const int TotalYang = mail.AddData.iYang - mail.AddData.iYang * EMAILBOX::MAILBOX_TAX / 100;

	if (TotalYang + Owner->GetGold() >= GOLD_MAX)
	{
		if (bAll == false)
			ServerProcess(EMAILBOX_GC::MAILBOX_GC_POST_GET_ITEMS, data_idx, EMAILBOX_POST_GET_ITEMS::POST_GET_ITEMS_YANG_OVERFLOW);
		return false;
	}

#if defined(__CHEQUE_SYSTEM__)
	if (mail.AddData.iWon + Owner->GetCheque() > CHEQUE_MAX)
	{
		ServerProcess(EMAILBOX_GC::MAILBOX_GC_POST_GET_ITEMS, data_idx, EMAILBOX_POST_GET_ITEMS::POST_GET_ITEMS_WON_OVERFLOW);
		return false;
	}
#endif

	if (mail.AddData.dwItemVnum > 0)
	{
		const LPITEM item = ITEM_MANAGER::Instance().CreateItem(mail.AddData.dwItemVnum, mail.AddData.dwItemCount);
		if (item == nullptr)
			return false;

#if defined(__DRAGON_SOUL_SYSTEM__)
		const int pos = item->IsDragonSoul() ? Owner->GetEmptyDragonSoulInventory(item) : Owner->GetEmptyInventory(item->GetSize());
#else
		const int pos = Owner->GetEmptyInventory(item->GetSize());
#endif
		if (-1 == pos)
		{
			if (bAll == false)
				ServerProcess(EMAILBOX_GC::MAILBOX_GC_POST_GET_ITEMS, data_idx, EMAILBOX_POST_GET_ITEMS::POST_GET_ITEMS_NOT_ENOUGHT_INVENTORY);
			ITEM_MANAGER::instance().DestroyItem(item);
			return false;
		}

		item->SetSockets(mail.AddData.alSockets);

		if (item->IsRealTimeItem() && item->GetSocket(0) == 0)
		{
			for (int i = 0; i < ITEM_LIMIT_MAX_NUM; i++)
			{
				// 아이템 생성 시점부터 사용하지 않아도 시간이 차감되는 방식
				if (LIMIT_REAL_TIME == item->GetLimitType(i))
				{
					if (item->GetLimitValue(i))
						item->SetSocket(0, time(0) + item->GetLimitValue(i));
					else
						item->SetSocket(0, time(0) + 60 * 60 * 24 * 7);

					item->StartRealTimeExpireEvent();
				}
				// 기존 유니크 아이템처럼 착용시에만 사용가능 시간이 차감되는 방식
				else if (LIMIT_TIMER_BASED_ON_WEAR == item->GetLimitType(i))
				{
					// 이미 착용중인 아이템이면 타이머를 시작하고, 새로 만드는 아이템은 사용 가능 시간을 세팅해준다. (
					// 아이템몰로 지급하는 경우에는 이 로직에 들어오기 전에 Socket0 값이 세팅이 되어 있어야 한다.
					if (item->GetID() == 0)
					{
						long duration = item->GetSocket(0);
						if (0 == duration)
							duration = item->GetLimitValue(i);

						if (0 == duration)
							duration = 60 * 60 * 10; // 정보가 아무것도 없으면 디폴트로 10시간 세팅

						item->SetSocket(0, duration);
					}
				}
			}
		}

		item->SetAttributes(mail.AddData.aAttr);
#if defined(__CHANGE_LOOK_SYSTEM__)
		item->SetTransmutationVnum(mail.AddData.dwChangeLookVnum);
#endif
#if defined(__REFINE_ELEMENT_SYSTEM__)
		item->SetRefineElement(&mail.AddData.RefineElement);
#endif
#if defined(__ITEM_APPLY_RANDOM__)
		item->SetRandomApplies(mail.AddData.aApplyRandom);
#endif
#if defined(__SET_ITEM__)
		item->SetItemSetValue(mail.AddData.bSetValue);
#endif
		Owner->AutoGiveItem(item);
	}

	Owner->GiveGold(TotalYang);
#if defined(__CHEQUE_SYSTEM__)
	Owner->GiveCheque(mail.AddData.iWon);
#endif

	mail.AddData.iYang = 0;
	mail.AddData.iWon = 0;
	mail.AddData.dwItemVnum = 0;
	mail.AddData.dwItemCount = 0;
	memset(mail.AddData.alSockets, 0, sizeof(mail.AddData.alSockets));
	memset(mail.AddData.aAttr, 0, sizeof(mail.AddData.aAttr));
#if defined(__CHANGE_LOOK_SYSTEM__)
	mail.AddData.dwChangeLookVnum = 0;
#endif
#if defined(__REFINE_ELEMENT_SYSTEM__)
	memset(&mail.AddData.RefineElement, 0, sizeof(mail.AddData.RefineElement));
#endif
#if defined(__ITEM_APPLY_RANDOM__)
	memset(mail.AddData.aApplyRandom, 0, sizeof(mail.AddData.aApplyRandom));
#endif
#if defined(__SET_ITEM__)
	mail.AddData.bSetValue = 0;
#endif
	mail.Message.bIsItemExist = false;

	/* Notify DB */
	TMailBox p;
	std::memcpy(p.szName, Owner->GetName(), sizeof(p.szName));
	p.Index = data_idx;
	db_clientdesc->DBPacket(HEADER_GD_MAILBOX_GET, Owner->GetDesc()->GetHandle(), &p, sizeof(p));

	if (bAll == false)
		ServerProcess(EMAILBOX_GC::MAILBOX_GC_POST_GET_ITEMS, data_idx, EMAILBOX_POST_GET_ITEMS::POST_GET_ITEMS_OK);

	return true;
}

void CMailBox::DeleteAllMails()
{
	const LPDESC d = Owner->GetDesc();
	if (d == nullptr)
		return;

	if (vecMailBox.empty())
	{
		ServerProcess(EMAILBOX_GC::MAILBOX_GC_POST_ALL_DELETE, EMAILBOX_POST_ALL_DELETE::POST_ALL_DELETE_FAIL_EMPTY);
		return;
	}

	TEMP_BUFFER buf;

	bool bIsFail = false;
	for (size_t i = 0; i < vecMailBox.size(); i++)
		if (DeleteMail(static_cast<BYTE>(i), true) == false)
			bIsFail = true;

	if (bIsFail == false)
		ServerProcess(EMAILBOX_GC::MAILBOX_GC_POST_ALL_DELETE, EMAILBOX_POST_ALL_DELETE::POST_ALL_DELETE_OK);
}

bool CMailBox::DeleteMail(const BYTE data_idx, const bool bAll)
{
	if (data_idx >= vecMailBox.size())
	{
		if (bAll == false)
			ServerProcess(EMAILBOX_GC::MAILBOX_GC_POST_DELETE, data_idx, EMAILBOX_POST_DELETE_FAIL::POST_DELETE_FAIL);
		return false;
	}

	TMailBoxTable& mail = vecMailBox.at(data_idx);
	if (mail.bIsDeleted)
	{
		if (bAll == false)
			ServerProcess(EMAILBOX_GC::MAILBOX_GC_POST_DELETE, data_idx, EMAILBOX_POST_DELETE_FAIL::POST_DELETE_FAIL);
		return true;
	}

	if (mail.Message.bIsItemExist)
	{
		if (bAll == false)
			ServerProcess(EMAILBOX_GC::MAILBOX_GC_POST_DELETE, data_idx, EMAILBOX_POST_DELETE_FAIL::POST_DELETE_FAIL_EXIST_ITEMS);
		return false;
	}

	/* Notify DB */
	TMailBox p;
	std::memcpy(p.szName, Owner->GetName(), sizeof(p.szName));
	p.Index = data_idx;
	db_clientdesc->DBPacket(HEADER_GD_MAILBOX_DELETE, Owner->GetDesc()->GetHandle(), &p, sizeof(p));

	mail.bIsDeleted = true;

	ServerProcess(EMAILBOX_GC::MAILBOX_GC_POST_DELETE, data_idx, EMAILBOX_POST_DELETE_FAIL::POST_DELETE_OK);
	return true;
}
#endif