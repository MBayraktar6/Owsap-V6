#include "stdafx.h"
#include "utils.h"
#include "config.h"
#include "char.h"
#include "desc.h"
#include "sectree_manager.h"
#include "packet.h"
#include "protocol.h"
#include "log.h"
#include "skill.h"
#include "unique_item.h"
#include "profiler.h"
#include "marriage.h"
#include "item_addon.h"
#include "dev_log.h"
#include "locale_service.h"
#include "item.h"
#include "item_manager.h"
#include "affect.h"
#include "DragonSoul.h"
#include "buff_on_attributes.h"
#include "belt_inventory_helper.h"
#include "../../common/VnumHelper.h"
#include "mob_manager.h"
#include "PetSystem.h"

CItem::CItem(DWORD dwVnum)
	: m_dwVnum(dwVnum), m_bWindow(0), m_dwID(0), m_bEquipped(false), m_dwVID(0), m_wCell(0), m_dwCount(0), m_lFlag(0), m_dwLastOwnerPID(0),
	m_bExchanging(false), m_pkDestroyEvent(NULL), m_pkUniqueExpireEvent(NULL), m_pkTimerBasedOnWearExpireEvent(NULL), m_pkRealTimeExpireEvent(NULL),
	m_pkExpireEvent(NULL),
	m_pkAccessorySocketExpireEvent(NULL), m_pkOwnershipEvent(NULL), m_dwOwnershipPID(0), m_bSkipSave(false), m_isLocked(false),
	m_dwMaskVnum(0), m_dwSIGVnum(0)
#if defined(__CHANGE_LOOK_SYSTEM__)
	, m_dwTransmutationVnum(0)
#endif
#if defined(__SET_ITEM__)
	, m_bSetValue(0)
#endif
#if defined(__SOUL_BIND_SYSTEM__)
	, m_lSealDate(E_SEAL_DATE_DEFAULT_TIMESTAMP)
	, m_pkSealDateExpireEvent(NULL)
#endif
#if defined(__SOUL_SYSTEM__)
	, m_pkSoulTimerUseEvent(nullptr)
#endif
{
	memset(&m_alSockets, 0, sizeof(m_alSockets));
	memset(&m_aAttr, 0, sizeof(m_aAttr));
#if defined(__REFINE_ELEMENT_SYSTEM__)
	memset(&m_RefineElement, 0, sizeof(m_RefineElement));
#endif
#if defined(__ITEM_APPLY_RANDOM__)
	memset(&m_aApplyRandom, 0, sizeof(m_aApplyRandom));
#endif
}

CItem::~CItem()
{
	Destroy();
}

void CItem::Initialize()
{
	CEntity::Initialize(ENTITY_ITEM);

	m_bWindow = RESERVED_WINDOW;
	m_pOwner = NULL;
	m_bEquipped = false;
	m_dwID = m_dwVID = m_dwCount = 0;
	m_wCell = 0;
	m_lFlag = 0;
	m_pProto = NULL;
	m_bExchanging = false;
	memset(&m_alSockets, 0, sizeof(m_alSockets));
	memset(&m_aAttr, 0, sizeof(m_aAttr));
#if defined(__REFINE_ELEMENT_SYSTEM__)
	memset(&m_RefineElement, 0, sizeof(m_RefineElement));
#endif
#if defined(__ITEM_APPLY_RANDOM__)
	memset(&m_aApplyRandom, 0, sizeof(m_aApplyRandom));
#endif

#if defined(__SOUL_BIND_SYSTEM__)
	m_lSealDate = 0;
	m_pkSealDateExpireEvent = NULL;
#endif

	m_pkDestroyEvent = NULL;
	m_pkOwnershipEvent = NULL;
	m_dwOwnershipPID = 0;
	m_pkUniqueExpireEvent = NULL;
	m_pkTimerBasedOnWearExpireEvent = NULL;
	m_pkRealTimeExpireEvent = NULL;

	m_pkAccessorySocketExpireEvent = NULL;
#if defined(__SOUL_SYSTEM__)
	m_pkSoulTimerUseEvent = NULL;
#endif

	m_bSkipSave = false;
	m_dwLastOwnerPID = 0;
}

void CItem::Destroy()
{
	event_cancel(&m_pkDestroyEvent);
	event_cancel(&m_pkOwnershipEvent);
	event_cancel(&m_pkUniqueExpireEvent);
	event_cancel(&m_pkTimerBasedOnWearExpireEvent);
	event_cancel(&m_pkRealTimeExpireEvent);
	event_cancel(&m_pkAccessorySocketExpireEvent);
#if defined(__SOUL_BIND_SYSTEM__)
	event_cancel(&m_pkSealDateExpireEvent);
#endif
#if defined(__SOUL_SYSTEM__)
	event_cancel(&m_pkSoulTimerUseEvent);
#endif

	CEntity::Destroy();

	if (GetSectree())
		GetSectree()->RemoveEntity(this);
}

EVENTFUNC(item_destroy_event)
{
	item_event_info* info = dynamic_cast<item_event_info*>(event->info);

	if (info == NULL)
	{
		sys_err("item_destroy_event> <Factor> Null pointer");
		return 0;
	}

	LPITEM pkItem = info->item;

	if (pkItem->GetOwner())
		sys_err("item_destroy_event: Owner exist. (item %s owner %s)", pkItem->GetName(), pkItem->GetOwner()->GetName());

	pkItem->SetDestroyEvent(NULL);
	M2_DESTROY_ITEM(pkItem);
	return 0;
}

void CItem::SetDestroyEvent(LPEVENT pkEvent)
{
	m_pkDestroyEvent = pkEvent;
}

void CItem::StartDestroyEvent(int iSec)
{
	if (m_pkDestroyEvent)
		return;

	item_event_info* info = AllocEventInfo<item_event_info>();
	info->item = this;

	SetDestroyEvent(event_create(item_destroy_event, info, PASSES_PER_SEC(iSec)));
}

void CItem::EncodeInsertPacket(LPENTITY ent)
{
	LPDESC d;

	if (!(d = ent->GetDesc()))
		return;

	const PIXEL_POSITION& c_pos = GetXYZ();

	struct packet_item_ground_add pack;

	pack.bHeader = HEADER_GC_ITEM_GROUND_ADD;
	pack.lX = c_pos.x;
	pack.lY = c_pos.y;
	pack.lZ = c_pos.z;
	pack.dwVID = m_dwVID;
	pack.dwVnum = GetVnum();
#if defined(__SET_ITEM__)
	pack.bSetValue = GetItemSetValue();
#endif

#if defined(__ITEM_DROP_RENEWAL__)
	for (size_t i = 0; i < ITEM_SOCKET_MAX_NUM; ++i)
		pack.alSockets[i] = GetSocket(i);

	thecore_memcpy(pack.aAttrs, GetAttributes(), sizeof(pack.aAttrs));
#endif

	d->Packet(&pack, sizeof(pack));

	if (m_pkOwnershipEvent != NULL)
	{
		item_event_info* info = dynamic_cast<item_event_info*>(m_pkOwnershipEvent->info);

		if (info == NULL)
		{
			sys_err("CItem::EncodeInsertPacket> <Factor> Null pointer");
			return;
		}

		TPacketGCItemOwnership p;

		p.bHeader = HEADER_GC_ITEM_OWNERSHIP;
		p.dwVID = m_dwVID;
		strlcpy(p.szName, info->szOwnerName, sizeof(p.szName));

		d->Packet(&p, sizeof(TPacketGCItemOwnership));
	}
}

void CItem::EncodeRemovePacket(LPENTITY ent)
{
	LPDESC d;

	if (!(d = ent->GetDesc()))
		return;

	struct packet_item_ground_del pack;

	pack.bHeader = HEADER_GC_ITEM_GROUND_DEL;
	pack.dwVID = m_dwVID;

	d->Packet(&pack, sizeof(pack));
	sys_log(2, "Item::EncodeRemovePacket %s to %s", GetName(), ((LPCHARACTER)ent)->GetName());
}

void CItem::SetProto(const TItemTable* table)
{
	assert(table != NULL);
	m_pProto = table;
	SetFlag(m_pProto->dwFlags);
}

void CItem::UsePacketEncode(LPCHARACTER ch, LPCHARACTER victim, struct packet_item_use* packet)
{
	if (!GetVnum())
		return;

	packet->header = HEADER_GC_ITEM_USE;
	packet->ch_vid = ch->GetVID();
	packet->victim_vid = victim->GetVID();
	packet->Cell = TItemPos(GetWindow(), m_wCell);
	packet->vnum = GetVnum();
}

void CItem::RemoveFlag(long bit)
{
	REMOVE_BIT(m_lFlag, bit);
}

void CItem::AddFlag(long bit)
{
	SET_BIT(m_lFlag, bit);
}

void CItem::UpdatePacket()
{
	if (!m_pOwner || !m_pOwner->GetDesc())
		return;

	TPacketGCItemUpdate pack;

	pack.bHeader = HEADER_GC_ITEM_UPDATE;
	pack.Cell = TItemPos(GetWindow(), m_wCell);
	pack.dwCount = m_dwCount;
#if defined(__SOUL_BIND_SYSTEM__)
	pack.lSealDate = m_lSealDate;
#endif
	for (int i = 0; i < ITEM_SOCKET_MAX_NUM; ++i)
		pack.alSockets[i] = m_alSockets[i];
	thecore_memcpy(pack.aAttr, GetAttributes(), sizeof(pack.aAttr));
#if defined(__CHANGE_LOOK_SYSTEM__)
	pack.dwTransmutationVnum = GetTransmutationVnum();
#endif
#if defined(__REFINE_ELEMENT_SYSTEM__)
	thecore_memcpy(&pack.RefineElement, GetRefineElement(), sizeof(pack.RefineElement));
#endif
#if defined(__ITEM_APPLY_RANDOM__)
	thecore_memcpy(pack.aApplyRandom, GetRandomApplies(), sizeof(pack.aApplyRandom));
#endif
#if defined(__SET_ITEM__)
	pack.bSetValue = GetItemSetValue();
#endif

	sys_log(2, "UpdatePacket %s -> %s", GetName(), m_pOwner->GetName());
	m_pOwner->GetDesc()->Packet(&pack, sizeof(pack));
}

DWORD CItem::GetCount()
{
	if (GetType() == ITEM_ELK)
		return MIN(m_dwCount, LONG_MAX);
	else
		return MIN(m_dwCount, ITEM_MAX_COUNT);
}

bool CItem::SetCount(DWORD count)
{
	if (GetType() == ITEM_ELK)
	{
		m_dwCount = MIN(count, LONG_MAX);
	}
	else
	{
		m_dwCount = MIN(count, ITEM_MAX_COUNT);
	}

	if (count == 0 && m_pOwner)
	{
		if (GetSubType() == USE_ABILITY_UP || GetSubType() == USE_POTION)
		{
			LPCHARACTER pOwner = GetOwner();
			BYTE bWindow = GetWindow();
			WORD wCell = GetCell();

			RemoveFromCharacter();

#if defined(__DRAGON_SOUL_SYSTEM__)
			if (!IsDragonSoul())
#endif
			{
				LPITEM pItem = pOwner->FindSpecifyItem(GetVnum());

				if (NULL != pItem)
				{
					if (bWindow == INVENTORY)
						pOwner->ChainQuickslotItem(pItem, SLOT_TYPE_INVENTORY, wCell);
					else if (bWindow == BELT_INVENTORY)
						pOwner->SyncQuickslot(SLOT_TYPE_BELT_INVENTORY, wCell, WORD_MAX);
				}
				else
				{
					if (bWindow == INVENTORY)
						pOwner->SyncQuickslot(SLOT_TYPE_INVENTORY, wCell, WORD_MAX);
					else if (bWindow == BELT_INVENTORY)
						pOwner->SyncQuickslot(SLOT_TYPE_BELT_INVENTORY, wCell, WORD_MAX);
				}
			}

			M2_DESTROY_ITEM(this);
		}
		else
		{
#if defined(__DRAGON_SOUL_SYSTEM__)
			if (!IsDragonSoul())
				m_pOwner->SyncQuickslot(SLOT_TYPE_INVENTORY, m_wCell, WORD_MAX);
#else
			m_pOwner->SyncQuickslot(SLOT_TYPE_INVENTORY, m_wCell, WORD_MAX);
#endif
			M2_DESTROY_ITEM(RemoveFromCharacter());
		}

		return false;
	}

	UpdatePacket();

	Save();
	return true;
}

#if defined(__SOUL_BIND_SYSTEM__)
bool CItem::CanSealItem() const
{
	switch (GetType())
	{
		case ITEM_WEAPON:
		{
			switch (GetSubType())
			{
				case WEAPON_ARROW:
				case WEAPON_MOUNT_SPEAR:
#if defined(__QUIVER_SYSTEM__)
				case WEAPON_QUIVER:
#endif
					//case WEAPON_BOUQUET:
					return false;
				default:
					return true;
			}
		}

		case ITEM_COSTUME:
		{
			switch (GetSubType())
			{
#if defined(__MOUNT_COSTUME_SYSTEM__)
				case COSTUME_MOUNT:
#endif
					return false;
				default:
					return true;
			}
		}

		case ITEM_ARMOR:
		case ITEM_BELT:
			return true;

#if defined(__DRAGON_SOUL_SYSTEM__) && defined(__DRAGON_SOUL_SEAL__)
		case ITEM_DS:
		{
			const BYTE bGradeIdx = (GetVnum() / 1000) % 10;
			if (bGradeIdx >= DRAGON_SOUL_GRADE_ANCIENT)
				return true;

			return false;
		}
#endif

		default:
			return false;
	}

	return false;
}

EVENTFUNC(SealDateExpireEvent)
{
	const item_vid_event_info* info = reinterpret_cast<const item_vid_event_info*>(event->info);
	if (!info)
		return 0;

	const LPITEM item = ITEM_MANAGER::instance().FindByVID(info->item_vid);
	if (!item)
		return 0;

	const time_t cur = get_global_time();
	if (cur > item->GetSealDate())
	{
		item->SealItem(E_SEAL_DATE_DEFAULT_TIMESTAMP);
		item->StopSealDateExpireTimerEvent();
		return 0;
	}

	return PASSES_PER_SEC(1);
}

void CItem::SealItem(long lSealDate)
{
	m_lSealDate = lSealDate;
	UpdatePacket();
	Save();
}

void CItem::StartSealDateExpireTimerEvent()
{
	if (m_pkSealDateExpireEvent)
		return;

	item_vid_event_info* info = AllocEventInfo<item_vid_event_info>();
	info->item_vid = GetVID();

	m_pkSealDateExpireEvent = event_create(SealDateExpireEvent, info, PASSES_PER_SEC(1));
	sys_log(0, "SEAL_DATE_TIME_EXPIRE: StartSealDateExpireTimerEvent");
}

void CItem::StopSealDateExpireTimerEvent()
{
	if (m_pkSealDateExpireEvent)
		event_cancel(&m_pkSealDateExpireEvent);

	sys_log(0, "SEAL_DATE_TIME_EXPIRE: StopSealDateExpireTimerEvent");
}
#endif

LPITEM CItem::RemoveFromCharacter()
{
	if (!m_pOwner)
	{
		sys_err("Item::RemoveFromCharacter owner null");
		return this;
	}

	LPCHARACTER pOwner = m_pOwner;
	if (!pOwner)
	{
		sys_err("Item::RemoveFromCharacter owner null");
		return this;
	}

	if (m_bEquipped) // 장착되었는가?
	{
		Unequip();
		//pOwner->UpdatePacket();

		SetWindow(RESERVED_WINDOW);
		Save();
		return this;
	}
	else
	{
		if (GetWindow() != SAFEBOX && GetWindow() != MALL
#if defined(__ATTR_6TH_7TH__)
			&& GetWindow() != NPC_STORAGE
#endif
			)
		{
#if defined(__DRAGON_SOUL_SYSTEM__)
			if (IsDragonSoul())
			{
				if (m_wCell >= DRAGON_SOUL_INVENTORY_MAX_NUM)
					sys_err("CItem::RemoveFromCharacter: pos >= DRAGON_SOUL_INVENTORY_MAX_NUM");
				else
					pOwner->SetItem(TItemPos(m_bWindow, m_wCell), NULL);
			}
			else if (GetWindow() == EQUIPMENT)
#else
			if (GetWindow() == EQUIPMENT)
#endif
			{
				if (m_wCell >= EQUIPMENT_MAX_NUM)
					sys_err("CItem::RemoveFromCharacter: pos >= EQUIPMENT_MAX_NUM");
				else
					pOwner->SetItem(TItemPos(m_bWindow, m_wCell), nullptr);
			}
			else if (GetWindow() == BELT_INVENTORY)
			{
				if (m_wCell >= BELT_INVENTORY_MAX_NUM)
					sys_err("CItem::RemoveFromCharacter: pos >= BELT_INVENTORY_MAX_NUM");
				else
					pOwner->SetItem(TItemPos(m_bWindow, m_wCell), nullptr);
			}
			else
			{
				const TItemPos cell(INVENTORY, m_wCell);

				if (cell.IsInventoryPosition() == false && cell.IsBeltInventoryPosition() == false) // 아니면 소지품에?
				{
					sys_err("CItem::RemoveFromCharacter: Invalid Item Position");
				}
				else
				{
					pOwner->SetItem(cell, nullptr);
				}
			}
		}

#if defined(__ATTR_6TH_7TH__)
		else if (GetWindow() == NPC_STORAGE)
			pOwner->SetItem(TItemPos(NPC_STORAGE, 0), nullptr);
#endif

		m_pOwner = nullptr;
		m_wCell = 0;

		SetWindow(RESERVED_WINDOW);
		Save();
		return this;
	}
}

bool CItem::AddToCharacter(LPCHARACTER ch, const TItemPos& Cell
#if defined(__WJ_PICKUP_ITEM_EFFECT__)
	, bool isHighLight
#endif
)
{
	assert(GetSectree() == NULL);
	assert(m_pOwner == NULL);

	BYTE bWindowType = Cell.window_type;
	WORD wCell = Cell.cell;

	switch (bWindowType)
	{
		case INVENTORY:
		{
#if defined(__EXTEND_INVEN_SYSTEM__)
			if (m_wCell >= ch->GetExtendInvenMax())
#else
			if (m_wCell >= INVENTORY_MAX_NUM)
#endif
			{
				sys_err("CItem::AddToCharacter: cell overflow: %s to %s cell %d", m_pProto->szName, ch->GetName(), m_wCell);
				return false;
			}
		}
		break;

		case EQUIPMENT:
		{
			if (m_wCell >= EQUIPMENT_MAX_NUM)
			{
				sys_err("CItem::AddToCharacter: cell overflow: %s to %s cell %d", m_pProto->szName, ch->GetName(), m_wCell);
				return false;
			}
		}
		break;

#if defined(__DRAGON_SOUL_SYSTEM__)
		case DRAGON_SOUL_INVENTORY:
		{
			if (m_wCell >= DRAGON_SOUL_INVENTORY_MAX_NUM)
			{
				sys_err("CItem::AddToCharacter: cell overflow: %s to %s cell %d", m_pProto->szName, ch->GetName(), m_wCell);
				return false;
			}
		}
		break;
#endif

		case BELT_INVENTORY:
		{
			if (m_wCell >= BELT_INVENTORY_SLOT_COUNT)
			{
				sys_err("CItem::AddToCharacter: cell overflow: %s to %s cell %d", m_pProto->szName, ch->GetName(), m_wCell);
				return false;
			}
		}
		break;
	}

	if (ch->GetDesc())
		m_dwLastOwnerPID = ch->GetPlayerID();

	event_cancel(&m_pkDestroyEvent);

#if defined(__WJ_PICKUP_ITEM_EFFECT__)
	ch->SetItem(TItemPos(bWindowType, wCell), this, isHighLight);
#else
	ch->SetItem(TItemPos(bWindowType, wCell), this);
#endif
	m_pOwner = ch;

	Save();
	return true;
}

LPITEM CItem::RemoveFromGround()
{
	if (GetSectree())
	{
		SetOwnership(NULL);

		GetSectree()->RemoveEntity(this);

		ViewCleanup();

		Save();
	}

	return (this);
}

bool CItem::AddToGround(long lMapIndex, const PIXEL_POSITION& pos, bool skipOwnerCheck)
{
	if (0 == lMapIndex)
	{
		sys_err("wrong map index argument: %d", lMapIndex);
		return false;
	}

	if (GetSectree())
	{
		sys_err("sectree already assigned");
		return false;
	}

	if (!skipOwnerCheck && m_pOwner)
	{
		sys_err("owner pointer not null");
		return false;
	}

	LPSECTREE tree = SECTREE_MANAGER::instance().Get(lMapIndex, pos.x, pos.y);

	if (!tree)
	{
		sys_err("cannot find sectree by %dx%d", pos.x, pos.y);
		return false;
	}

	//tree->Touch();

	SetWindow(GROUND);
	SetXYZ(pos.x, pos.y, pos.z);
	tree->InsertEntity(this);
	UpdateSectree();
	Save();
	return true;
}

bool CItem::DistanceValid(LPCHARACTER ch)
{
	if (!GetSectree())
		return false;

	int iDist = DISTANCE_APPROX(GetX() - ch->GetX(), GetY() - ch->GetY());

	if (iDist > 300)
		return false;

	return true;
}

bool CItem::CanUsedBy(LPCHARACTER ch)
{
	// Anti flag check
	switch (ch->GetJob())
	{
		case JOB_WARRIOR:
			if (GetAntiFlag() & ITEM_ANTIFLAG_WARRIOR)
				return false;
			break;

		case JOB_ASSASSIN:
			if (GetAntiFlag() & ITEM_ANTIFLAG_ASSASSIN)
				return false;
			break;

		case JOB_SHAMAN:
			if (GetAntiFlag() & ITEM_ANTIFLAG_SHAMAN)
				return false;
			break;

		case JOB_SURA:
			if (GetAntiFlag() & ITEM_ANTIFLAG_SURA)
				return false;
			break;

		case JOB_WOLFMAN:
			if (GetAntiFlag() & ITEM_ANTIFLAG_WOLFMAN)
				return false;
			break;

	}

	return true;
}

int CItem::FindEquipCell(LPCHARACTER ch, int iCandidateCell)
{
	// 코스츔 아이템(ITEM_COSTUME)은 WearFlag 없어도 됨. (sub type으로 착용위치 구분. 귀찮게 또 wear flag 줄 필요가 있나..)
	// 용혼석(ITEM_DS, ITEM_SPECIAL_DS)도 SUB_TYPE으로 구분. 신규 반지, 벨트는 ITEM_TYPE으로 구분 -_-
	if ((0 == GetWearFlag() || ITEM_TOTEM == GetType()) && ITEM_COSTUME != GetType()
#if defined(__DRAGON_SOUL_SYSTEM__)
		&& ITEM_DS != GetType() && ITEM_SPECIAL_DS != GetType()
#endif
		&& ITEM_RING != GetType() && ITEM_BELT != GetType()
		)
		return -1;

	// 용혼석 슬롯을 WEAR로 처리할 수가 없어서(WEAR는 최대 32개까지 가능한데 용혼석을 추가하면 32가 넘는다.)
	// 인벤토리의 특정 위치((INVENTORY_MAX_NUM + WEAR_MAX_NUM)부터 (INVENTORY_MAX_NUM + WEAR_MAX_NUM + DRAGON_SOUL_DECK_MAX_NUM * DS_SLOT_MAX - 1)까지)를
	// 용혼석 슬롯으로 정함.
	// return 할 때에, INVENTORY_MAX_NUM을 뺀 이유는,
	// 본래 WearCell이 INVENTORY_MAX_NUM를 빼고 return 하기 때문.
#if defined(__DRAGON_SOUL_SYSTEM__)
	if (GetType() == ITEM_DS || GetType() == ITEM_SPECIAL_DS)
	{
		if (iCandidateCell < 0)
		{
			return WEAR_MAX_NUM + GetSubType();
		}

		for (int i = 0; i < DRAGON_SOUL_DECK_MAX_NUM; i++)
		{
			if (WEAR_MAX_NUM + i * DS_SLOT_MAX + GetSubType() == iCandidateCell)
			{
				return iCandidateCell;
			}
		}
		return -1;
	}
#endif

	if (GetType() == ITEM_COSTUME)
	{
		switch (GetSubType())
		{
			case COSTUME_BODY:
				return WEAR_COSTUME_BODY;
			case COSTUME_HAIR:
				return WEAR_COSTUME_HAIR;
#if defined(__MOUNT_COSTUME_SYSTEM__)
			case COSTUME_MOUNT:
				return WEAR_COSTUME_MOUNT;
#endif
#if defined(__ACCE_COSTUME_SYSTEM__)
			case COSTUME_ACCE:
				return WEAR_COSTUME_ACCE;
#endif
#if defined(__WEAPON_COSTUME_SYSTEM__)
			case COSTUME_WEAPON:
				return WEAR_COSTUME_WEAPON;
#endif
#if defined(__AURA_COSTUME_SYSTEM__)
			case COSTUME_AURA:
				return WEAR_COSTUME_AURA;
#endif
		}
	}

	else if (GetWearFlag() & WEARABLE_BODY)
		return WEAR_BODY;
	else if (GetWearFlag() & WEARABLE_HEAD)
		return WEAR_HEAD;
	else if (GetWearFlag() & WEARABLE_FOOTS)
		return WEAR_FOOTS;
	else if (GetWearFlag() & WEARABLE_WRIST)
		return WEAR_WRIST;
	else if (GetWearFlag() & WEARABLE_WEAPON)
		return WEAR_WEAPON;
	else if (GetWearFlag() & WEARABLE_NECK)
		return WEAR_NECK;
	else if (GetWearFlag() & WEARABLE_EAR)
		return WEAR_EAR;
	else if (GetWearFlag() & WEARABLE_UNIQUE)
	{
		if (ch->GetWear(WEAR_UNIQUE1))
			return WEAR_UNIQUE2;
		else
			return WEAR_UNIQUE1;
	}
	else if (GetWearFlag() & WEARABLE_SHIELD)
		return WEAR_SHIELD;
	else if (GetWearFlag() & WEARABLE_ARROW)
		return WEAR_ARROW;
	else if (GetType() == ITEM_BELT)
		return WEAR_BELT;
#if defined(__PENDANT_SYSTEM__)
	else if (GetWearFlag() & WEARABLE_PENDANT)
		return WEAR_PENDANT;
#endif
#if defined(__GLOVE_SYSTEM__)
	else if (GetWearFlag() & WEARABLE_GLOVE)
		return WEAR_GLOVE;
#endif

	return -1;
}

//#define __CHECK_MODIFY_POINTS_PERF__

void CItem::ModifyPoints(bool bAdd)
{
#if defined(__CHECK_MODIFY_POINTS_PERF__)
	auto _modify_s1 = std::chrono::steady_clock::now();
#endif

	BYTE bAccessoryGrade = 0;

	// 무기와 갑옷만 소켓을 적용시킨다.
	if (!IsAccessoryForSocket())
	{
#if defined(__QUIVER_SYSTEM__)
		if ((m_pProto->bType == ITEM_WEAPON && m_pProto->bSubType != WEAPON_QUIVER) || m_pProto->bType == ITEM_ARMOR)
#else
		if (m_pProto->bType == ITEM_WEAPON || m_pProto->bType == ITEM_ARMOR)
#endif
		{
#if defined(__CHECK_MODIFY_POINTS_PERF__)
			auto _accessory_s1 = std::chrono::steady_clock::now();
#endif
			// 소켓이 속성강화에 사용되는 경우 적용하지 않는다 (ARMOR_WRIST ARMOR_NECK ARMOR_EAR ARMOR_GLOVE)
			for (BYTE bSocketIndex = 0; bSocketIndex < METIN_SOCKET_MAX_NUM; ++bSocketIndex)
			{
#if defined(__GLOVE_SYSTEM__)
				if (m_pProto->bType == ITEM_ARMOR && m_pProto->bSubType == ARMOR_GLOVE)
				{
					const DWORD dwData = GetSocket(bSocketIndex);
					if (dwData < 1000000)
						continue;

					const WORD wType = (dwData / 10000) % 1000;
					if (wType == APPLY_NONE)
						continue;

					const long lValue = dwData % 100;
					m_pOwner->ApplyPoint(wType, (bAdd) ? lValue : -lValue);
				}
				else
#endif
				{
					const DWORD dwVnum = GetSocket(bSocketIndex);
					if (dwVnum <= 2)
						continue;

					const TItemTable* pItemTable = ITEM_MANAGER::instance().GetTable(dwVnum);
					if (pItemTable == nullptr)
					{
						sys_err("cannot find table by vnum %lu", dwVnum);
						continue;
					}

					if (ITEM_METIN == pItemTable->bType
#if defined(__GLOVE_SYSTEM__)
						&& pItemTable->bSubType != METIN_SUNGMA
#endif
						)
					{
						for (BYTE bApplyIndex = 0; bApplyIndex < ITEM_APPLY_MAX_NUM; ++bApplyIndex)
						{
							if (pItemTable->aApplies[bApplyIndex].wType == APPLY_NONE)
								continue;

							if (pItemTable->aApplies[bApplyIndex].wType == APPLY_SKILL)
								m_pOwner->ApplyPoint(pItemTable->aApplies[bApplyIndex].wType,
									(bAdd) ? pItemTable->aApplies[bApplyIndex].lValue : pItemTable->aApplies[bApplyIndex].lValue ^ 0x00800000);
							else
								m_pOwner->ApplyPoint(pItemTable->aApplies[bApplyIndex].wType,
									(bAdd) ? pItemTable->aApplies[bApplyIndex].lValue : -pItemTable->aApplies[bApplyIndex].lValue);
						}
					}
				}
			}
#if defined(__CHECK_MODIFY_POINTS_PERF__)
			auto _accessory_e1 = std::chrono::steady_clock::now();
			auto _accessory_d1 = std::chrono::duration_cast<std::chrono::nanoseconds>(_accessory_e1 - _accessory_s1).count();
			m_pOwner->ChatPacket(CHAT_TYPE_INFO, "!IsAccessoryForSocket() took %lld nanoseconds.", static_cast<long long>(_accessory_d1));
#endif
		}

		bAccessoryGrade = 0;
	}
	else
	{
		bAccessoryGrade = static_cast<BYTE>(MIN(GetAccessorySocketGrade(), ITEM_ACCESSORY_SOCKET_MAX_NUM));
	}

#if defined(__CHECK_MODIFY_POINTS_PERF__)
	auto _apply_s1 = std::chrono::steady_clock::now();
#endif
	for (BYTE bApplyIndex = 0; bApplyIndex < ITEM_APPLY_MAX_NUM; ++bApplyIndex)
	{
		if (m_pProto->aApplies[bApplyIndex].wType == APPLY_NONE)
			continue;

		long lValue = m_pProto->aApplies[bApplyIndex].lValue;
		if (m_pProto->aApplies[bApplyIndex].wType == APPLY_SKILL)
		{
			m_pOwner->ApplyPoint(m_pProto->aApplies[bApplyIndex].wType, bAdd ? lValue : lValue ^ 0x00800000);
		}
#if defined(__ITEM_APPLY_RANDOM__)
		else if (m_pProto->aApplies[bApplyIndex].wType == APPLY_RANDOM && GetRandomApplyType(bApplyIndex))
		{
			const TPlayerItemAttribute& rItemApplyRandom = GetRandomApply(bApplyIndex);
			lValue = rItemApplyRandom.lValue;

			if (rItemApplyRandom.wType == APPLY_SKILL)
			{
				m_pOwner->ApplyPoint(rItemApplyRandom.wType, bAdd ? lValue : lValue ^ 0x00800000);
			}
			else
			{
				if (0 != bAccessoryGrade)
					lValue += MAX(bAccessoryGrade, lValue * aiAccessorySocketEffectivePct[bAccessoryGrade] / 100);

				m_pOwner->ApplyPoint(rItemApplyRandom.wType, bAdd ? lValue : -lValue);
			}
		}
#endif
		else
		{
			if (0 != bAccessoryGrade)
				lValue += MAX(bAccessoryGrade, lValue * aiAccessorySocketEffectivePct[bAccessoryGrade] / 100);

			m_pOwner->ApplyPoint(m_pProto->aApplies[bApplyIndex].wType, bAdd ? lValue : -lValue);
		}
	}

#if defined(__CHECK_MODIFY_POINTS_PERF__)
	auto _apply_e1 = std::chrono::steady_clock::now();
	auto _apply_d1 = std::chrono::duration_cast<std::chrono::nanoseconds>(_apply_e1 - _apply_s1).count();
	m_pOwner->ChatPacket(CHAT_TYPE_INFO, "Item applies took %lld nanoseconds.", static_cast<long long>(_apply_d1));
#endif

#if defined(__CHECK_MODIFY_POINTS_PERF__)
	auto _acce_s1 = std::chrono::steady_clock::now();
#endif
#if defined(__ACCE_COSTUME_SYSTEM__)
	if (m_pProto->bType == ITEM_COSTUME && m_pProto->bSubType == COSTUME_ACCE)
		m_pOwner->ModifyAccePoints(this, bAdd);
#endif
#if defined(__CHECK_MODIFY_POINTS_PERF__)
	auto _acce_e1 = std::chrono::steady_clock::now();
	auto _acce_d1 = std::chrono::duration_cast<std::chrono::nanoseconds>(_acce_e1 - _acce_s1).count();
	m_pOwner->ChatPacket(CHAT_TYPE_INFO, "ModifyAccePoints %lld nanoseconds.", static_cast<long long>(_acce_d1));
#endif

#if defined(__CHECK_MODIFY_POINTS_PERF__)
	auto _aura_s1 = std::chrono::steady_clock::now();
#endif
#if defined(__AURA_COSTUME_SYSTEM__)
	if (m_pProto->bType == ITEM_COSTUME && m_pProto->bSubType == COSTUME_AURA)
		m_pOwner->ModifyAuraPoints(this, bAdd);
#endif
#if defined(__CHECK_MODIFY_POINTS_PERF__)
	auto _aura_e1 = std::chrono::steady_clock::now();
	auto _aura_d1 = std::chrono::duration_cast<std::chrono::nanoseconds>(_aura_e1 - _aura_s1).count();
	m_pOwner->ChatPacket(CHAT_TYPE_INFO, "ModifyAuraPoints %lld nanoseconds.", static_cast<long long>(_aura_d1));
#endif

	// 초승달의 반지, 할로윈 사탕, 행복의 반지, 영원한 사랑의 펜던트의 경우
	// 기존의 하드 코딩으로 강제로 속성을 부여했지만,
	// 그 부분을 제거하고 special item group 테이블에서 속성을 부여하도록 변경하였다.
	// 하지만 하드 코딩되어있을 때 생성된 아이템이 남아있을 수도 있어서 특수처리 해놓는다.
	// 이 아이템들의 경우, 밑에 ITEM_UNIQUE일 때의 처리로 속성이 부여되기 때문에,
	// 아이템에 박혀있는 attribute는 적용하지 않고 넘어간다.
	//if (true == CItemVnumHelper::IsRamadanMoonRing(GetVnum()) || true == CItemVnumHelper::IsHalloweenCandy(GetVnum())
		//|| true == CItemVnumHelper::IsHappinessRing(GetVnum()) || true == CItemVnumHelper::IsLovePendant(GetVnum()))
	if (CItemVnumHelper::IsUniqueItem(GetVnum()))
	{
		// Do not anything.
	}
	else
	{
#if defined(__CHECK_MODIFY_POINTS_PERF__)
		auto _attr_s1 = std::chrono::steady_clock::now();
#endif
		for (BYTE bAttrIndex = 0; bAttrIndex < ITEM_ATTRIBUTE_MAX_NUM; ++bAttrIndex)
		{
#if defined(__ACCE_COSTUME_SYSTEM__)
			if (GetType() == ITEM_COSTUME && GetSubType() == COSTUME_ACCE)
				break; // Modifying at @ ModifyAccePoints
#endif

#if defined(__AURA_COSTUME_SYSTEM__)
			if (GetType() == ITEM_COSTUME && GetSubType() == COSTUME_AURA)
				break; // Modifying at @ ModifyAuraPoints
#endif

			if (GetAttributeType(bAttrIndex))
			{
				const TPlayerItemAttribute& rItemAttribute = GetAttribute(bAttrIndex);
#if defined(__DRAGON_SOUL_SYSTEM__) && defined(__DS_SET__)
				long lValue = rItemAttribute.lValue;
				if (IsDragonSoul() && m_pOwner->FindAffect(AFFECT_DS_SET))
				{
					if (bAttrIndex < DSManager::instance().GetApplyCount(GetVnum()))
					{
						lValue += DSManager::instance().GetBasicApplyValue(GetVnum(), rItemAttribute.wType, true);
					}
					else
					{
						lValue += DSManager::instance().GetAdditionalApplyValue(GetVnum(), rItemAttribute.wType, true);
					}
				}

				if (rItemAttribute.wType == APPLY_SKILL)
					m_pOwner->ApplyPoint(rItemAttribute.wType, bAdd ? lValue : lValue ^ 0x00800000);
				else
					m_pOwner->ApplyPoint(rItemAttribute.wType, bAdd ? lValue : -lValue);
#else
				if (rItemAttribute.wType == APPLY_SKILL)
					m_pOwner->ApplyPoint(rItemAttribute.wType, bAdd ? rItemAttribute.lValue : rItemAttribute.lValue ^ 0x00800000);
				else
					m_pOwner->ApplyPoint(rItemAttribute.wType, bAdd ? rItemAttribute.lValue : -rItemAttribute.lValue);
#endif
			}
		}
#if defined(__CHECK_MODIFY_POINTS_PERF__)
		auto _attr_e1 = std::chrono::steady_clock::now();
		auto _attr_d1 = std::chrono::duration_cast<std::chrono::nanoseconds>(_attr_e1 - _attr_s1).count();
		m_pOwner->ChatPacket(CHAT_TYPE_INFO, "Item attributes took % lld nanoseconds.", static_cast<long long>(_attr_d1));
#endif
	}

#if defined(__REFINE_ELEMENT_SYSTEM__)
	if (m_pProto->bType == ITEM_WEAPON && GetRefineElementGrade() != 0)
	{
		const long lValue = static_cast<long>(GetRefineElementValue());
		if (lValue > 0)
			m_pOwner->ApplyPoint(GetRefineElementApplyType(), bAdd ? lValue : -lValue);

		const long lBonusValue = static_cast<long>(GetRefineElementBonusValue());
		if (lBonusValue > 0)
			m_pOwner->ApplyPoint(APPLY_ATT_GRADE_BONUS, bAdd ? lBonusValue : -lBonusValue);
	}
#endif

	switch (m_pProto->bType)
	{
		case ITEM_PICK:
		case ITEM_ROD:
		{
			if (bAdd)
			{
				if (m_wCell == WEAR_WEAPON)
					m_pOwner->SetPart(PART_WEAPON, GetVnum());
			}
			else
			{
				if (m_wCell == WEAR_WEAPON)
					m_pOwner->SetPart(PART_WEAPON, m_pOwner->GetOriginalPart(PART_WEAPON));
			}
		}
		break;

		case ITEM_WEAPON:
		{
#if defined(__WEAPON_COSTUME_SYSTEM__)
			// 코스츔 body를 입고있다면 armor는 벗던 입던 상관 없이 비주얼에 영향을 주면 안 됨.
			if (m_pOwner->GetWear(WEAR_COSTUME_WEAPON) != 0)
			{
#if defined(__HIDE_WEAPON_COSTUME_WITH_NO_MAIN_WEAPON__)
				m_pOwner->SetPart(PART_WEAPON, m_pOwner->GetWear(WEAR_COSTUME_WEAPON)->GetVnum());
#endif
				return;
			}
#endif

			if (bAdd)
			{
#if defined(__CHANGE_LOOK_SYSTEM__)
				if (m_wCell == WEAR_WEAPON)
					m_pOwner->SetPart(PART_WEAPON, (GetTransmutationVnum() != 0) ? GetTransmutationVnum() : GetVnum());
#else
				if (m_wCell == WEAR_WEAPON)
					m_pOwner->SetPart(PART_WEAPON, GetVnum());
#endif
			}
			else
			{
				if (m_wCell == WEAR_WEAPON)
					m_pOwner->SetPart(PART_WEAPON, m_pOwner->GetOriginalPart(PART_WEAPON));
			}
		}
		break;

		case ITEM_ARMOR:
		{
#if defined(__COSTUME_SYSTEM__)
			// 코스츔 body를 입고있다면 armor는 벗던 입던 상관 없이 비주얼에 영향을 주면 안 됨.
			if (m_pOwner->GetWear(WEAR_COSTUME_BODY) != 0)
				break;
#endif

			if (GetSubType() == ARMOR_BODY || GetSubType() == ARMOR_HEAD || GetSubType() == ARMOR_FOOTS || GetSubType() == ARMOR_SHIELD)
			{
				if (bAdd)
				{
#if defined(__CHANGE_LOOK_SYSTEM__)
					if (GetProto()->bSubType == ARMOR_BODY)
						m_pOwner->SetPart(PART_MAIN, (GetTransmutationVnum() != 0) ? GetTransmutationVnum() : GetVnum());
#else
					if (GetProto()->bSubType == ARMOR_BODY)
						m_pOwner->SetPart(PART_MAIN, GetVnum());
#endif
				}
				else
				{
					if (GetProto()->bSubType == ARMOR_BODY)
						m_pOwner->SetPart(PART_MAIN, m_pOwner->GetOriginalPart(PART_MAIN));
				}
			}
		}
		break;

#if defined(__COSTUME_SYSTEM__)
		// 코스츔 아이템 입었을 때 캐릭터 parts 정보 세팅. 기존 스타일대로 추가함..
		case ITEM_COSTUME:
		{
			DWORD dwToSetValue = GetVnum();
			BYTE bSetPart = PART_MAX_NUM;

			switch (GetSubType())
			{
				// 갑옷 코스츔
				case COSTUME_BODY:
				{
					bSetPart = PART_MAIN;

					if (!bAdd)
					{
						// 코스츔 갑옷을 벗었을 때 원래 갑옷을 입고 있었다면 그 갑옷으로 look 세팅, 입지 않았다면 default look
						const CItem* pArmor = m_pOwner->GetWear(WEAR_BODY);
#if defined(__CHANGE_LOOK_SYSTEM__)
						if (pArmor)
							dwToSetValue = (pArmor->GetTransmutationVnum() != 0) ? pArmor->GetTransmutationVnum() : pArmor->GetVnum();
						else
							dwToSetValue = m_pOwner->GetOriginalPart(PART_MAIN);
#else
						dwToSetValue = (pArmor != nullptr) ? pArmor->GetVnum() : m_pOwner->GetOriginalPart(PART_MAIN);
#endif
					}
#if defined(__CHANGE_LOOK_SYSTEM__)
					else
						dwToSetValue = (GetTransmutationVnum() != 0) ? GetTransmutationVnum() : GetVnum();
#endif
				}
				break;

				// 헤어 코스츔
				case COSTUME_HAIR:
				{
					bSetPart = PART_HAIR;

					// 코스츔 헤어는 shape값을 item proto의 value3에 세팅하도록 함. 특별한 이유는 없고 기존 갑옷(ARMOR_BODY)의 shape값이 프로토의 value3에 있어서 헤어도 같이 value3으로 함.
					// [NOTE] 갑옷은 아이템 vnum을 보내고 헤어는 shape(value3)값을 보내는 이유는.. 기존 시스템이 그렇게 되어있음...
#if defined(__CHANGE_LOOK_SYSTEM__)
					const DWORD dwTransmutationVnum = GetTransmutationVnum();
					if (dwTransmutationVnum != 0)
					{
						const TItemTable* pItemTable = ITEM_MANAGER::instance().GetTable(dwTransmutationVnum);
						dwToSetValue = (pItemTable != nullptr) ? pItemTable->alValues[3] : GetValue(3);
					}
					else
						dwToSetValue = (bAdd) ? GetValue(3) : 0;
#else
					dwToSetValue = (bAdd) ? GetValue(3) : 0;
#endif
				}
				break;

#if defined(__ACCE_COSTUME_SYSTEM__)
				case COSTUME_ACCE:
				{
					bSetPart = PART_ACCE;
					dwToSetValue = (bAdd) ? dwToSetValue : 0;
				}
				break;
#endif

#if defined(__WEAPON_COSTUME_SYSTEM__)
				case COSTUME_WEAPON:
				{
					bSetPart = PART_WEAPON;

					if (!bAdd)
					{
						// 코스츔 갑옷을 벗었을 때 원래 갑옷을 입고 있었다면 그 갑옷으로 look 세팅, 입지 않았다면 default look
						const CItem* pWeapon = m_pOwner->GetWear(WEAR_WEAPON);
#if defined(__CHANGE_LOOK_SYSTEM__)
						if (pWeapon)
							dwToSetValue = (pWeapon->GetTransmutationVnum() != 0) ? pWeapon->GetTransmutationVnum() : pWeapon->GetVnum();
						else
							dwToSetValue = 0;
#else
						dwToSetValue = (pWeapon != nullptr) ? pWeapon->GetVnum() : m_pOwner->GetOriginalPart(PART_WEAPON);
#endif
					}
#if defined(__CHANGE_LOOK_SYSTEM__)
					else
						dwToSetValue = (GetTransmutationVnum() != 0) ? GetTransmutationVnum() : GetVnum();
#endif
				}
				break;
#endif

#if defined(__AURA_COSTUME_SYSTEM__)
				case COSTUME_AURA:
				{
					bSetPart = PART_AURA;
					dwToSetValue = (bAdd) ? dwToSetValue : 0;
				}
				break;
#endif
			}

			if (PART_MAX_NUM != bSetPart)
			{
				m_pOwner->SetPart(bSetPart, dwToSetValue);
				m_pOwner->UpdatePacket();
			}
		}
		break;
#endif

		case ITEM_UNIQUE:
		{
			sys_log(0, "ITEM_UNIQUE Process");
			if (GetSIGVnum() != 0)
			{
				sys_log(0, "ITEM_UNIQUE Contains SIGVnum %u", GetSIGVnum());

				const CSpecialItemGroup* pItemGroup = ITEM_MANAGER::instance().GetSpecialItemGroup(GetSIGVnum());
				if (pItemGroup == nullptr)
				{
					sys_log(0, "ITEM_UNIQUE No pointer for item group?");
					break;
				}

				const DWORD dwAttrVnum = pItemGroup->GetAttrVnum(GetVnum());
				const CSpecialAttrGroup* pAttrGroup = ITEM_MANAGER::instance().GetSpecialAttrGroup(dwAttrVnum);
				if (pAttrGroup == nullptr)
				{
					sys_log(0, "ITEM_UNIQUE No pointer for attr group?");
					break;
				}

				for (const CSpecialAttrInfo& it : pAttrGroup->m_vecAttrs)
					m_pOwner->ApplyPoint(it.apply_type, bAdd ? it.apply_value : -it.apply_value);
			}
		}
		break;
	}

#if defined(__WEAPON_COSTUME_SYSTEM__)
	if (m_pOwner->GetWear(WEAR_COSTUME_WEAPON) && !m_pOwner->GetWear(WEAR_WEAPON))
		m_pOwner->SetPart(PART_WEAPON, 0);
#endif
}

bool CItem::IsEquipable() const
{
	switch (this->GetType())
	{
		case ITEM_COSTUME:
		case ITEM_ARMOR:
		case ITEM_WEAPON:
		case ITEM_ROD:
		case ITEM_PICK:
		case ITEM_UNIQUE:
		case ITEM_DS:
		case ITEM_SPECIAL_DS:
		case ITEM_RING:
		case ITEM_BELT:
			return true;
	}

	return false;
}

// return false on error state
bool CItem::EquipTo(LPCHARACTER ch, BYTE bWearCell)
{
	if (!ch)
	{
		sys_err("EquipTo: null character");
		return false;
	}

	// 용혼석 슬롯 index는 WEAR_MAX_NUM 보다 큼.
#if defined(__DRAGON_SOUL_SYSTEM__)
	if (IsDragonSoul())
	{
		if (bWearCell < WEAR_MAX_NUM || bWearCell >= WEAR_MAX_NUM + DRAGON_SOUL_DECK_MAX_NUM * DS_SLOT_MAX)
		{
			sys_err("EquipTo: invalid dragon soul cell (this: #%d %s wearflag: %d cell: %d)", GetOriginalVnum(), GetName(), GetSubType(), bWearCell - WEAR_MAX_NUM);
			return false;
		}
	}
	else
#endif
	{
		if (bWearCell >= WEAR_MAX_NUM)
		{
			sys_err("EquipTo: invalid wear cell (this: #%d %s wearflag: %d cell: %d)", GetOriginalVnum(), GetName(), GetWearFlag(), bWearCell);
			return false;
		}
	}

	if (ch->GetWear(bWearCell))
	{
		sys_err("EquipTo: item already exist (this: #%d %s cell: %d %s)", GetOriginalVnum(), GetName(), bWearCell, ch->GetWear(bWearCell)->GetName());
		return false;
	}

	if (GetOwner())
		RemoveFromCharacter();

	ch->SetWear(bWearCell, this); // 여기서 패킷 나감

	m_pOwner = ch;
	m_bEquipped = true;
	m_wCell = bWearCell;

	DWORD dwImmuneFlag = 0;

	for (int i = 0; i < WEAR_MAX_NUM; ++i)
	{
		if (m_pOwner->GetWear(i))
		{
			SET_BIT(dwImmuneFlag, m_pOwner->GetWear(i)->GetRealImmuneFlag());
		}
	}

	m_pOwner->SetImmuneFlag(dwImmuneFlag);

#if defined(__DRAGON_SOUL_SYSTEM__)
	if (IsDragonSoul())
	{
		DSManager::instance().ActivateDragonSoul(this);
#if defined(__DS_SET__)
		m_pOwner->DragonSoul_SetBonus();
#endif
	}
	else
#endif
	{
		ModifyPoints(true);
		StartUniqueExpireEvent();
		if (-1 != GetProto()->cLimitTimerBasedOnWearIndex)
			StartTimerBasedOnWearExpireEvent();

		// ACCESSORY_REFINE
		StartAccessorySocketExpireEvent();
		// END_OF_ACCESSORY_REFINE
	}

	m_pOwner->BuffOnAttr_AddBuffsFromItem(this);

#if defined(__SET_ITEM__)
	if (IsCostume() || IsUnique())
		m_pOwner->RefreshItemSetBonus();

	if (GetItemSetValue())
		m_pOwner->RefreshItemSetBonusByValue();
#endif

	m_pOwner->ComputeBattlePoints();
	m_pOwner->UpdatePacket();

	Save();

	return (true);
}

bool CItem::Unequip()
{
	if (!m_pOwner || GetCell() > EQUIPMENT_MAX_NUM)
	{
		// ITEM_OWNER_INVALID_PTR_BUG
		sys_err("%s %u m_pOwner %p, GetCell %d",
			GetName(), GetID(), get_pointer(m_pOwner), GetCell());
		// END_OF_ITEM_OWNER_INVALID_PTR_BUG
		return false;
	}

	if (this != m_pOwner->GetWear(GetCell()))
	{
		sys_err("m_pOwner->GetWear() != this");
		return false;
	}

	// 신규 말 아이템 제거시 처리
	if (IsRideItem())
		ClearMountAttributeAndAffect();

#if defined(__DRAGON_SOUL_SYSTEM__)
	if (IsDragonSoul())
	{
		DSManager::instance().DeactivateDragonSoul(this);
#if defined(__DS_SET__)
		m_pOwner->DragonSoul_SetBonus();
#endif
	}
	else
#endif
	{
		ModifyPoints(false);
	}

	StopUniqueExpireEvent();

	if (-1 != GetProto()->cLimitTimerBasedOnWearIndex)
		StopTimerBasedOnWearExpireEvent();

	// ACCESSORY_REFINE
	StopAccessorySocketExpireEvent();
	// END_OF_ACCESSORY_REFINE

	m_pOwner->BuffOnAttr_RemoveBuffsFromItem(this);
	m_pOwner->SetWear(GetCell(), nullptr);

	DWORD dwImmuneFlag = 0;
	for (int i = 0; i < WEAR_MAX_NUM; ++i)
	{
		if (m_pOwner->GetWear(i))
		{
			SET_BIT(dwImmuneFlag, m_pOwner->GetWear(i)->GetRealImmuneFlag());
		}
	}
	m_pOwner->SetImmuneFlag(dwImmuneFlag);

#if defined(__SET_ITEM__)
	if (IsCostume() || IsUnique())
		m_pOwner->RefreshItemSetBonus();

	if (GetItemSetValue())
		m_pOwner->RefreshItemSetBonusByValue();
#endif

	m_pOwner->ComputeBattlePoints();
	m_pOwner->UpdatePacket();

	m_pOwner = nullptr;
	m_wCell = 0;
	m_bEquipped = false;

	return true;
}

long CItem::GetValue(DWORD idx)
{
	assert(idx < ITEM_VALUES_MAX_NUM);
	return GetProto()->alValues[idx];
}

void CItem::SetExchanging(bool bOn)
{
	m_bExchanging = bOn;
}

void CItem::Save()
{
	if (m_bSkipSave)
		return;

	ITEM_MANAGER::instance().DelayedSave(this);
}

#if defined(__MOVE_COSTUME_ATTR__)
bool CItem::CanChangeCostumeAttr() const
{
	if (GetType() != ITEM_COSTUME)
		return false;

	switch (GetSubType())
	{
		case COSTUME_BODY:
		case COSTUME_HAIR:
#if defined(__WEAPON_COSTUME_SYSTEM__)
		case COSTUME_WEAPON:
#endif
			return true;
	}

	return false;
}
#endif

bool CItem::CreateSocket(BYTE bSlot, BYTE bGold)
{
	assert(bSlot < ITEM_SOCKET_MAX_NUM);

	if (m_alSockets[bSlot] != 0)
	{
		sys_err("Item::CreateSocket : socket already exist %s %d", GetName(), bSlot);
		return false;
	}

	if (bGold)
		m_alSockets[bSlot] = 2;
	else
		m_alSockets[bSlot] = 1;

	UpdatePacket();

	Save();
	return true;
}

void CItem::SetSockets(const long* c_al)
{
	thecore_memcpy(m_alSockets, c_al, sizeof(m_alSockets));
	Save();
}

void CItem::SetSocket(int i, long v, bool bLog)
{
	assert(i < ITEM_SOCKET_MAX_NUM);
	m_alSockets[i] = v;
	UpdatePacket();
	Save();
	if (bLog)
		LogManager::instance().ItemLog(i, v, 0, GetID(), "SET_SOCKET", "", "", GetOriginalVnum());
}

int CItem::GetGold()
{
	if (IS_SET(GetFlag(), ITEM_FLAG_COUNT_PER_1GOLD))
	{
		if (GetProto()->dwShopSellPrice == 0)
			return GetCount();
		else
			return GetCount() / GetProto()->dwShopSellPrice;
	}
	else
		return GetProto()->dwShopSellPrice;
}

int CItem::GetShopBuyPrice()
{
	return GetProto()->dwShopBuyPrice;
}

int CItem::GetShopSellPrice()
{
	return GetProto()->dwShopSellPrice;
}

bool CItem::IsOwnership(LPCHARACTER ch)
{
	if (!m_pkOwnershipEvent)
		return true;

	return m_dwOwnershipPID == ch->GetPlayerID() ? true : false;
}

EVENTFUNC(ownership_event)
{
	item_event_info* info = dynamic_cast<item_event_info*>(event->info);

	if (info == NULL)
	{
		sys_err("ownership_event> <Factor> Null pointer");
		return 0;
	}

	LPITEM pkItem = info->item;

	pkItem->SetOwnershipEvent(NULL);

	TPacketGCItemOwnership p;

	p.bHeader = HEADER_GC_ITEM_OWNERSHIP;
	p.dwVID = pkItem->GetVID();
	p.szName[0] = '\0';

	pkItem->PacketAround(&p, sizeof(p));
	return 0;
}

void CItem::SetOwnershipEvent(LPEVENT pkEvent)
{
	m_pkOwnershipEvent = pkEvent;
}

void CItem::SetOwnership(LPCHARACTER ch, int iSec)
{
	if (!ch)
	{
		if (m_pkOwnershipEvent)
		{
			event_cancel(&m_pkOwnershipEvent);
			m_dwOwnershipPID = 0;

			TPacketGCItemOwnership p;

			p.bHeader = HEADER_GC_ITEM_OWNERSHIP;
			p.dwVID = m_dwVID;
			p.szName[0] = '\0';

			PacketAround(&p, sizeof(p));
		}
		return;
	}

	if (m_pkOwnershipEvent)
		return;

	if (true == LC_IsEurope())
	{
		if (iSec <= 10)
			iSec = 30;
	}

	m_dwOwnershipPID = ch->GetPlayerID();

	item_event_info* info = AllocEventInfo<item_event_info>();
	strlcpy(info->szOwnerName, ch->GetName(), sizeof(info->szOwnerName));
	info->item = this;

	SetOwnershipEvent(event_create(ownership_event, info, PASSES_PER_SEC(iSec)));

	TPacketGCItemOwnership p;

	p.bHeader = HEADER_GC_ITEM_OWNERSHIP;
	p.dwVID = m_dwVID;
	strlcpy(p.szName, ch->GetName(), sizeof(p.szName));

	PacketAround(&p, sizeof(p));
}

int CItem::GetSocketCount()
{
	for (int i = 0; i < ITEM_SOCKET_MAX_NUM; i++)
	{
		if (GetSocket(i) == 0)
			return i;
	}
	return ITEM_SOCKET_MAX_NUM;
}

bool CItem::AddSocket()
{
	int count = GetSocketCount();
	if (count == ITEM_SOCKET_MAX_NUM)
		return false;
	m_alSockets[count] = 1;
	return true;
}

void CItem::AlterToSocketItem(int iSocketCount)
{
	if (iSocketCount >= METIN_SOCKET_MAX_NUM)
	{
		sys_log(0, "Invalid Socket Count %d, set to maximum", METIN_SOCKET_MAX_NUM);
		iSocketCount = METIN_SOCKET_MAX_NUM;
	}

	for (int i = 0; i < iSocketCount; ++i)
		SetSocket(i, 1);
}

void CItem::AlterToMagicItem(int iSecondPct /*= 0*/, int iThirdPct /*= 0 */)
{
	int idx = GetAttributeSetIndex();

	if (idx < 0)
		return;

	//			Appeariance	Second	Third
	// Weapon	50			20		5
	// Armor	30			10		2
	// Acc		20			10		1

	//int iSecondPct;
	//int iThirdPct;

	switch (GetType())
	{
		case ITEM_WEAPON:
		{
			iSecondPct = 20;
			iThirdPct = 5;
		}
		break;

		case ITEM_ARMOR:
		{
			if (GetSubType() == ARMOR_BODY)
			{
				iSecondPct = 10;
				iThirdPct = 2;
			}
			else
			{
				iSecondPct = 10;
				iThirdPct = 1;
			}
		}
		break;

#if defined(__MOVE_COSTUME_ATTR__)
		case ITEM_COSTUME:
		{
			if (GetSubType() == COSTUME_BODY)
			{
				iSecondPct = 30;
				iThirdPct = 10;
			}
			else if (GetSubType() == COSTUME_HAIR)
			{
				iSecondPct = 30;
				iThirdPct = 10;
			}
#if defined(__WEAPON_COSTUME_SYSTEM__)
			else if (GetSubType() == COSTUME_WEAPON)
			{
				iSecondPct = 30;
				iThirdPct = 10;
			}
#endif
			else
			{
				iSecondPct = 0;
				iThirdPct = 0;
			}
		}
		break;
#endif

		default:
			return;
	}

	// 100% 확률로 좋은 속성 하나
	PutAttribute(aiItemMagicAttributePercentHigh);

	if (number(1, 100) <= iSecondPct)
		PutAttribute(aiItemMagicAttributePercentLow);

	if (number(1, 100) <= iThirdPct)
		PutAttribute(aiItemMagicAttributePercentLow);
}

DWORD CItem::GetRefineFromVnum()
{
	return ITEM_MANAGER::instance().GetRefineFromVnum(GetVnum());
}

int CItem::GetRefineLevel()
{
	const char* name = GetBaseName();
	char* p = const_cast<char*>(strrchr(name, '+'));

	if (!p)
		return 0;

	int	rtn = 0;
	str_to_number(rtn, p + 1);

	const char* locale_name = GetName();
	p = const_cast<char*>(strrchr(locale_name, '+'));

	if (p)
	{
		int	locale_rtn = 0;
		str_to_number(locale_rtn, p + 1);
		if (locale_rtn != rtn)
		{
			sys_err("refine_level_based_on_NAME(%d) is not equal to refine_level_based_on_LOCALE_NAME(%d).", rtn, locale_rtn);
		}
	}

	return rtn;
}

#if defined(__LOOT_FILTER_SYSTEM__)
long CItem::GetWearingLevelLimit() const
{
	if (m_pProto)
	{
		for (int i = 0; i < ITEM_LIMIT_MAX_NUM; ++i)
			if (m_pProto->aLimits[i].bType == LIMIT_LEVEL)
				return m_pProto->aLimits[i].lValue;
	}

	return 0;
}

bool CItem::IsHairDye() const
{
	switch (GetVnum())
	{
		case 70202:
		case 70203:
		case 70204:
		case 70205:
		case 70206:
			return true;
		default:
			return false;
	}
}
#endif

bool CItem::IsPolymorphItem()
{
	return GetType() == ITEM_POLYMORPH;
}

EVENTFUNC(unique_expire_event)
{
	item_event_info* info = dynamic_cast<item_event_info*>(event->info);

	if (info == NULL)
	{
		sys_err("unique_expire_event> <Factor> Null pointer");
		return 0;
	}

	LPITEM pkItem = info->item;

	if (pkItem->GetValue(2) == 0)
	{
		if (pkItem->GetSocket(ITEM_SOCKET_UNIQUE_REMAIN_TIME) <= 1)
		{
			sys_log(0, "UNIQUE_ITEM: expire %s %u", pkItem->GetName(), pkItem->GetID());
			pkItem->SetUniqueExpireEvent(NULL);
			ITEM_MANAGER::instance().RemoveItem(pkItem, "UNIQUE_EXPIRE");
			return 0;
		}
		else
		{
			pkItem->SetSocket(ITEM_SOCKET_UNIQUE_REMAIN_TIME, pkItem->GetSocket(ITEM_SOCKET_UNIQUE_REMAIN_TIME) - 1);
			return PASSES_PER_SEC(60);
		}
	}
	else
	{
		time_t cur = get_global_time();

		if (pkItem->GetSocket(ITEM_SOCKET_UNIQUE_REMAIN_TIME) <= cur)
		{
			pkItem->SetUniqueExpireEvent(NULL);
			ITEM_MANAGER::instance().RemoveItem(pkItem, "UNIQUE_EXPIRE");
			return 0;
		}
		else
		{
			// 게임 내에 시간제 아이템들이 빠릿빠릿하게 사라지지 않는 버그가 있어
			// 수정
			// by rtsummit
			if (pkItem->GetSocket(ITEM_SOCKET_UNIQUE_REMAIN_TIME) - cur < 600)
				return PASSES_PER_SEC(pkItem->GetSocket(ITEM_SOCKET_UNIQUE_REMAIN_TIME) - cur);
			else
				return PASSES_PER_SEC(600);
		}
	}
}

// 시간 후불제
// timer를 시작할 때에 시간 차감하는 것이 아니라, 
// timer가 발화할 때에 timer가 동작한 시간 만큼 시간 차감을 한다.
EVENTFUNC(timer_based_on_wear_expire_event)
{
	item_event_info* info = dynamic_cast<item_event_info*>(event->info);

	if (info == NULL)
	{
		sys_err("expire_event <Factor> Null pointer");
		return 0;
	}

	LPITEM pkItem = info->item;
	int remain_time = pkItem->GetSocket(ITEM_SOCKET_REMAIN_SEC) - processing_time / passes_per_sec;
	if (remain_time <= 0)
	{
		sys_log(0, "ITEM EXPIRED : expired %s %u", pkItem->GetName(), pkItem->GetID());
		pkItem->SetTimerBasedOnWearExpireEvent(NULL);
		pkItem->SetSocket(ITEM_SOCKET_REMAIN_SEC, 0);

		// 일단 timer based on wear 용혼석은 시간 다 되었다고 없애지 않는다.
#if defined(__DRAGON_SOUL_SYSTEM__)
		if (pkItem->IsDragonSoul())
		{
			DSManager::instance().DeactivateDragonSoul(pkItem);
#if defined(__DS_SET__)
			if (pkItem->GetOwner())
				pkItem->GetOwner()->DragonSoul_SetBonus();
#endif
		}
		else
#endif
		{
			ITEM_MANAGER::instance().RemoveItem(pkItem, "TIMER_BASED_ON_WEAR_EXPIRE");
		}
		return 0;
	}
	pkItem->SetSocket(ITEM_SOCKET_REMAIN_SEC, remain_time);
	return PASSES_PER_SEC(MIN(60, remain_time));
}

void CItem::SetUniqueExpireEvent(LPEVENT pkEvent)
{
	m_pkUniqueExpireEvent = pkEvent;
}

void CItem::SetTimerBasedOnWearExpireEvent(LPEVENT pkEvent)
{
	m_pkTimerBasedOnWearExpireEvent = pkEvent;
}

EVENTFUNC(real_time_expire_event)
{
	const item_vid_event_info* info = reinterpret_cast<const item_vid_event_info*>(event->info);

	if (NULL == info)
		return 0;

	const LPITEM item = ITEM_MANAGER::instance().FindByVID(info->item_vid);

	if (NULL == item)
		return 0;

	const time_t current = get_global_time();

	if (current > item->GetSocket(0))
	{
#if defined(__CHANGE_LOOK_SYSTEM__)
		LPCHARACTER owner = item->GetOwner();
		if (owner)
		{
			CChangeLook* pChangeLook = owner->GetChangeLook();
			if (pChangeLook)
				pChangeLook->Clear();
		}
#endif

		ITEM_MANAGER::instance().RemoveItem(item, "REAL_TIME_EXPIRE");
		return 0;
	}

	return PASSES_PER_SEC(1);
}

void CItem::StartRealTimeExpireEvent()
{
	if (m_pkRealTimeExpireEvent)
		return;

	for (int i = 0; i < ITEM_LIMIT_MAX_NUM; i++)
	{
		if (LIMIT_REAL_TIME == GetProto()->aLimits[i].bType || LIMIT_REAL_TIME_START_FIRST_USE == GetProto()->aLimits[i].bType)
		{
			item_vid_event_info* info = AllocEventInfo<item_vid_event_info>();
			info->item_vid = GetVID();

			m_pkRealTimeExpireEvent = event_create(real_time_expire_event, info, PASSES_PER_SEC(1));

			sys_log(0, "REAL_TIME_EXPIRE: StartRealTimeExpireEvent");

			return;
		}
	}
}

bool CItem::IsRealTimeItem()
{
	if (!GetProto())
		return false;

	for (int i = 0; i < ITEM_LIMIT_MAX_NUM; i++)
	{
		if (LIMIT_REAL_TIME == GetProto()->aLimits[i].bType || LIMIT_REAL_TIME_START_FIRST_USE == GetProto()->aLimits[i].bType)
			return true;
	}

	return false;
}

bool CItem::IsUsedTimeItem()
{
	return m_pkRealTimeExpireEvent != NULL;
}

void CItem::StartUniqueExpireEvent()
{
#if defined(__MOUNT_COSTUME_SYSTEM__)
	if (GetType() != ITEM_UNIQUE && !IsRideItem())
		return;
#else
	if (GetType() != ITEM_UNIQUE)
		return;
#endif

	if (m_pkUniqueExpireEvent)
		return;

	// 기간제 아이템일 경우 시간제 아이템은 동작하지 않는다
	if (IsRealTimeItem())
		return;

	// HARD CODING
	if (GetVnum() == UNIQUE_ITEM_HIDE_ALIGNMENT_TITLE)
		if (m_pOwner) m_pOwner->ShowAlignment(false);

	int iSec = GetSocket(ITEM_SOCKET_UNIQUE_SAVE_TIME);

	if (iSec == 0)
		iSec = 60;
	else
		iSec = MIN(iSec, 60);

	SetSocket(ITEM_SOCKET_UNIQUE_SAVE_TIME, 0);

	item_event_info* info = AllocEventInfo<item_event_info>();
	info->item = this;

	SetUniqueExpireEvent(event_create(unique_expire_event, info, PASSES_PER_SEC(iSec)));
}

// 시간 후불제
// timer_based_on_wear_expire_event 설명 참조
void CItem::StartTimerBasedOnWearExpireEvent()
{
	if (m_pkTimerBasedOnWearExpireEvent)
		return;

	// 기간제 아이템일 경우 시간제 아이템은 동작하지 않는다
	if (IsRealTimeItem())
		return;

	if (-1 == GetProto()->cLimitTimerBasedOnWearIndex)
		return;

	int iSec = GetSocket(0);

	if (0 != iSec)
	{
		iSec %= 60;
		if (0 == iSec)
			iSec = 60;
	}

	item_event_info* info = AllocEventInfo<item_event_info>();
	info->item = this;

	SetTimerBasedOnWearExpireEvent(event_create(timer_based_on_wear_expire_event, info, PASSES_PER_SEC(iSec)));
}

void CItem::StopUniqueExpireEvent()
{
	if (!m_pOwner)
		return;

	if (!m_pkUniqueExpireEvent)
		return;

	if (GetValue(2) != 0) // 게임시간제 이외의 아이템은 UniqueExpireEvent를 중단할 수 없다.
		return;

	// HARD CODING
	if (GetVnum() == UNIQUE_ITEM_HIDE_ALIGNMENT_TITLE)
		m_pOwner->ShowAlignment(true);

	SetSocket(ITEM_SOCKET_UNIQUE_SAVE_TIME, event_time(m_pkUniqueExpireEvent) / passes_per_sec);
	event_cancel(&m_pkUniqueExpireEvent);

	ITEM_MANAGER::instance().SaveSingleItem(this);
}

void CItem::StopTimerBasedOnWearExpireEvent()
{
	if (!m_pkTimerBasedOnWearExpireEvent)
		return;

	int remain_time = GetSocket(ITEM_SOCKET_REMAIN_SEC) - event_processing_time(m_pkTimerBasedOnWearExpireEvent) / passes_per_sec;

	SetSocket(ITEM_SOCKET_REMAIN_SEC, remain_time);
	event_cancel(&m_pkTimerBasedOnWearExpireEvent);

	ITEM_MANAGER::instance().SaveSingleItem(this);
}

void CItem::ApplyAddon(int iAddonType)
{
	CItemAddonManager::instance().ApplyAddonTo(iAddonType, this);
}

int CItem::GetSpecialGroup() const
{
	return ITEM_MANAGER::instance().GetSpecialGroupFromItem(GetVnum());
}

//
// 악세서리 소켓 처리.
//
bool CItem::IsAccessoryForSocket()
{
	return (m_pProto->bType == ITEM_ARMOR && (m_pProto->bSubType == ARMOR_WRIST || m_pProto->bSubType == ARMOR_NECK || m_pProto->bSubType == ARMOR_EAR)) ||
		(m_pProto->bType == ITEM_BELT); // 2013년 2월 새로 추가된 '벨트' 아이템의 경우 기획팀에서 악세서리 소켓 시스템을 그대로 이용하자고 함.
}

bool CItem::IsRemovableSocket()
{
	if (m_pProto->bType == ITEM_ARMOR)
		return m_pProto->bSubType == ARMOR_BODY
#if defined(__GLOVE_SYSTEM__)
		|| m_pProto->bSubType == ARMOR_GLOVE
#endif
		;

	if (m_pProto->bType == ITEM_WEAPON)
		return !(m_pProto->bSubType == WEAPON_ARROW
			|| m_pProto->bSubType == WEAPON_MOUNT_SPEAR
#if defined(__QUIVER_SYSTEM__)
			|| m_pProto->bSubType == WEAPON_QUIVER
#endif
			|| m_pProto->bSubType == WEAPON_BOUQUET);

	return false;
}

void CItem::SetAccessorySocketGrade(int iGrade)
{
	SetSocket(0, MINMAX(0, iGrade, GetAccessorySocketMaxGrade()));

	int iDownTime = aiAccessorySocketDegradeTime[GetAccessorySocketGrade()];

	//if (test_server)
	//	iDownTime /= 60;

	SetAccessorySocketDownGradeTime(iDownTime);
}

void CItem::SetAccessorySocketMaxGrade(int iMaxGrade)
{
	SetSocket(1, MINMAX(0, iMaxGrade, ITEM_ACCESSORY_SOCKET_MAX_NUM));
}

void CItem::SetAccessorySocketDownGradeTime(DWORD time)
{
	SetSocket(2, time);

	if (test_server && GetOwner())
		GetOwner()->ChatPacket(CHAT_TYPE_INFO, LC_STRING("%s에서 소켓 빠질때까지 남은 시간 %d", LC_ITEM(GetVnum()), time));
}

EVENTFUNC(accessory_socket_expire_event)
{
	item_vid_event_info* info = dynamic_cast<item_vid_event_info*>(event->info);

	if (info == NULL)
	{
		sys_err("accessory_socket_expire_event> <Factor> Null pointer");
		return 0;
	}

	LPITEM item = ITEM_MANAGER::instance().FindByVID(info->item_vid);

	if (item->GetAccessorySocketDownGradeTime() <= 1)
	{
	degrade:
		item->SetAccessorySocketExpireEvent(NULL);
		item->AccessorySocketDegrade();
		return 0;
	}
	else
	{
		int iTime = item->GetAccessorySocketDownGradeTime() - 60;

		if (iTime <= 1)
			goto degrade;

		item->SetAccessorySocketDownGradeTime(iTime);

		if (iTime > 60)
			return PASSES_PER_SEC(60);
		else
			return PASSES_PER_SEC(iTime);
	}
}

void CItem::StartAccessorySocketExpireEvent()
{
	if (!IsAccessoryForSocket())
		return;

	if (m_pkAccessorySocketExpireEvent)
		return;

	if (GetAccessorySocketMaxGrade() == 0)
		return;

	if (GetAccessorySocketGrade() == 0)
		return;

	int iSec = GetAccessorySocketDownGradeTime();
	SetAccessorySocketExpireEvent(NULL);

	if (iSec <= 1)
		iSec = 5;
	else
		iSec = MIN(iSec, 60);

	item_vid_event_info* info = AllocEventInfo<item_vid_event_info>();
	info->item_vid = GetVID();

	SetAccessorySocketExpireEvent(event_create(accessory_socket_expire_event, info, PASSES_PER_SEC(iSec)));
}

void CItem::StopAccessorySocketExpireEvent()
{
	if (!m_pkAccessorySocketExpireEvent)
		return;

	if (!IsAccessoryForSocket())
		return;

	int new_time = GetAccessorySocketDownGradeTime() - (60 - event_time(m_pkAccessorySocketExpireEvent) / passes_per_sec);

	event_cancel(&m_pkAccessorySocketExpireEvent);

	if (new_time <= 1)
	{
		AccessorySocketDegrade();
	}
	else
	{
		SetAccessorySocketDownGradeTime(new_time);
	}
}

bool CItem::IsRideItem()
{
	if (ITEM_UNIQUE == GetType() && UNIQUE_SPECIAL_RIDE == GetSubType())
		return true;

#if defined(__MOUNT_COSTUME_SYSTEM__)
	if (ITEM_COSTUME == GetType() && COSTUME_MOUNT == GetSubType())
		return true;
#endif

	return false;
}

void CItem::ClearMountAttributeAndAffect()
{
	LPCHARACTER ch = GetOwner();

	ch->RemoveAffect(AFFECT_MOUNT);
	ch->RemoveAffect(AFFECT_MOUNT_BONUS);

	ch->MountVnum(0);

	ch->PointChange(POINT_ST, 0);
	ch->PointChange(POINT_DX, 0);
	ch->PointChange(POINT_HT, 0);
	ch->PointChange(POINT_IQ, 0);
}

void CItem::SetAccessorySocketExpireEvent(LPEVENT pkEvent)
{
	m_pkAccessorySocketExpireEvent = pkEvent;
}

void CItem::AccessorySocketDegrade()
{
	if (GetAccessorySocketGrade() > 0)
	{
		LPCHARACTER ch = GetOwner();

		if (ch)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_STRING("%s에 박혀있던 보석이 사라집니다.", LC_ITEM(GetVnum())));
		}

		ModifyPoints(false);
		SetAccessorySocketGrade(GetAccessorySocketGrade() - 1);
		ModifyPoints(true);

		int iDownTime = aiAccessorySocketDegradeTime[GetAccessorySocketGrade()];

		if (test_server)
			iDownTime /= 60;

		SetAccessorySocketDownGradeTime(iDownTime);

		if (iDownTime)
			StartAccessorySocketExpireEvent();
	}
}

// ring에 item을 박을 수 있는지 여부를 체크해서 리턴
static const bool CanPutIntoRing(LPITEM ring, LPITEM item)
{
	const DWORD vnum = item->GetVnum();
	return false;
}

bool CItem::CanPutInto(LPITEM item)
{
	if (item->GetType() == ITEM_BELT)
		return this->GetSubType() == USE_PUT_INTO_BELT_SOCKET;

	else if (item->GetType() == ITEM_RING)
		return CanPutIntoRing(item, this);

	else if (item->GetType() != ITEM_ARMOR)
		return false;

	DWORD vnum = item->GetVnum();

	struct JewelAccessoryInfo
	{
		DWORD jewel;
		DWORD wrist;
		DWORD neck;
		DWORD ear;
	};
	const static JewelAccessoryInfo infos[] = {
		{ 50634, 14420, 16220, 17220 },
		{ 50635, 14500, 16500, 17500 },
		{ 50636, 14520, 16520, 17520 },
		{ 50637, 14540, 16540, 17540 },
		{ 50638, 14560, 16560, 17560 },
		{ 50639, 14570, 16570, 0 },
#if defined(__CONQUEROR_LEVEL__)
		{ 50661, 14590, 16590, 17580 },
		{ 50662, 14600, 16600, 17590 },
		{ 50663, 14610, 16610, 17600 },
		{ 50664, 14580, 16580, 17570 },
#endif
	};

	DWORD item_type = (item->GetVnum() / 10) * 10;
	for (int i = 0; i < sizeof(infos) / sizeof(infos[0]); i++)
	{
		const JewelAccessoryInfo& info = infos[i];
		switch (item->GetSubType())
		{
			case ARMOR_WRIST:
				if (info.wrist == item_type)
				{
					if (info.jewel == GetVnum())
					{
						return true;
					}
					else
					{
						return false;
					}
				}
				break;
			case ARMOR_NECK:
				if (info.neck == item_type)
				{
					if (info.jewel == GetVnum())
					{
						return true;
					}
					else
					{
						return false;
					}
				}
				break;
			case ARMOR_EAR:
				if (info.ear == item_type)
				{
					if (info.jewel == GetVnum())
					{
						return true;
					}
					else
					{
						return false;
					}
				}
				break;
		}
	}
	if (item->GetSubType() == ARMOR_WRIST)
		vnum -= 14000;
	else if (item->GetSubType() == ARMOR_NECK)
		vnum -= 16000;
	else if (item->GetSubType() == ARMOR_EAR)
		vnum -= 17000;
	else
		return false;

	DWORD type = vnum / 20;

	if (type < 0 || type > 11)
	{
		type = (vnum - 170) / 20;

		if (50623 + type != GetVnum())
			return false;
		else
			return true;
	}
	else if (item->GetVnum() >= 16210 && item->GetVnum() <= 16219)
	{
		if (50625 != GetVnum())
			return false;
		else
			return true;
	}
	else if (item->GetVnum() >= 16230 && item->GetVnum() <= 16239)
	{
		if (50626 != GetVnum())
			return false;
		else
			return true;
	}

	return 50623 + type == GetVnum();
}

bool CItem::IsStackable()
{
	return (GetFlag() & ITEM_FLAG_STACKABLE) ? true : false;
}

bool CItem::CheckItemUseLevel(int nLevel)
{
	for (int i = 0; i < ITEM_LIMIT_MAX_NUM; ++i)
	{
		if (this->m_pProto->aLimits[i].bType == LIMIT_LEVEL)
		{
			if (this->m_pProto->aLimits[i].lValue > nLevel) return false;
			else return true;
		}
	}
	return true;
}

POINT_VALUE CItem::FindApplyValue(POINT_TYPE wApplyType)
{
	if (m_pProto == NULL)
		return 0;

	for (int i = 0; i < ITEM_APPLY_MAX_NUM; ++i)
	{
		if (m_pProto->aApplies[i].wType == wApplyType)
			return m_pProto->aApplies[i].lValue;
	}

	return 0;
}

#if defined(__MOUNT_COSTUME_SYSTEM__)
DWORD CItem::GetMountVnum()
{
	if (!IsCostumeMount())
		return 0;

	const DWORD dwMountVnum = FindApplyValue(APPLY_MOUNT);
#if defined(__CHANGE_LOOK_SYSTEM__)
	const DWORD c_dwTransmutationVnum = GetTransmutationVnum();
	if (c_dwTransmutationVnum != 0)
	{
		TItemTable* pItemTable = ITEM_MANAGER::instance().GetTable(c_dwTransmutationVnum);
		if (pItemTable)
			return static_cast<DWORD>(pItemTable->FindApplyValue(APPLY_MOUNT));
	}
#endif
	return dwMountVnum;
}
#endif

void CItem::CopySocketTo(LPITEM pItem)
{
	for (int i = 0; i < ITEM_SOCKET_MAX_NUM; ++i)
	{
		pItem->m_alSockets[i] = m_alSockets[i];
	}
}

int CItem::GetAccessorySocketGrade()
{
	return MINMAX(0, GetSocket(0), GetAccessorySocketMaxGrade());
}

int CItem::GetAccessorySocketMaxGrade()
{
	return MINMAX(0, GetSocket(1), ITEM_ACCESSORY_SOCKET_MAX_NUM);
}

int CItem::GetAccessorySocketDownGradeTime()
{
	return MINMAX(0, GetSocket(2), aiAccessorySocketDegradeTime[GetAccessorySocketGrade()]);
}

void CItem::AttrLog()
{
	const char* pszIP = NULL;

	if (GetOwner() && GetOwner()->GetDesc())
		pszIP = GetOwner()->GetDesc()->GetHostName();

	for (int i = 0; i < ITEM_SOCKET_MAX_NUM; ++i)
	{
		if (m_alSockets[i])
		{
			LogManager::instance().ItemLog(i, m_alSockets[i], 0, GetID(), "INFO_SOCKET", "", pszIP ? pszIP : "", GetOriginalVnum());
		}
	}

	for (int i = 0; i < ITEM_ATTRIBUTE_MAX_NUM; ++i)
	{
		POINT_TYPE type = m_aAttr[i].wType;
		POINT_VALUE value = m_aAttr[i].lValue;

		if (type)
			LogManager::instance().ItemLog(i, type, value, GetID(), "INFO_ATTR", "", pszIP ? pszIP : "", GetOriginalVnum());
	}
}

int CItem::GetLevelLimit()
{
	for (int i = 0; i < ITEM_LIMIT_MAX_NUM; ++i)
	{
		if (this->m_pProto->aLimits[i].bType == LIMIT_LEVEL)
		{
			return this->m_pProto->aLimits[i].lValue;
		}
	}
	return 0;
}

int CItem::GetDurationLimit()
{
	for (int i = 0; i < ITEM_LIMIT_MAX_NUM; ++i)
	{
		if (this->m_pProto->aLimits[i].bType == LIMIT_DURATION)
		{
			return this->m_pProto->aLimits[i].lValue;
		}
	}
	return 0;
}

bool CItem::OnAfterCreatedItem()
{
#if defined(__SOUL_SYSTEM__)
	if (GetType() == ITEM_SOUL && GetLimitValue(1) != 0)
		StartSoulTimerUseEvent();
#endif

#if defined(__SOUL_BIND_SYSTEM__)
	if (GetSealDate() > E_SEAL_DATE_DEFAULT_TIMESTAMP)
		StartSealDateExpireTimerEvent();
#endif

	// 아이템을 한 번이라도 사용했다면, 그 이후엔 사용 중이지 않아도 시간이 차감되는 방식
	if (-1 != this->GetProto()->cLimitRealTimeFirstUseIndex)
	{
		// Socket1에 아이템의 사용 횟수가 기록되어 있으니, 한 번이라도 사용한 아이템은 타이머를 시작한다.
		if (0 != GetSocket(1))
		{
			StartRealTimeExpireEvent();
		}
	}

	return true;
}

#if defined(__DRAGON_SOUL_SYSTEM__)
bool CItem::IsDragonSoul()
{
	return GetType() == ITEM_DS;
}

int CItem::GiveMoreTime_Per(float fPercent)
{
	if (IsDragonSoul())
	{
		DWORD duration = DSManager::instance().GetDuration(this);
		DWORD remain_sec = GetSocket(ITEM_SOCKET_REMAIN_SEC);
		DWORD given_time = fPercent * duration / 100u;

		if (remain_sec == duration)
			return false;

		if ((given_time + remain_sec) >= duration)
		{
			SetSocket(ITEM_SOCKET_REMAIN_SEC, duration);
			return duration - remain_sec;
		}
		else
		{
			SetSocket(ITEM_SOCKET_REMAIN_SEC, given_time + remain_sec);
			return given_time;
		}
	}
	// 우선 용혼석에 관해서만 하도록 한다.
	else
		return 0;
}

int CItem::GiveMoreTime_Fix(DWORD dwTime)
{
	if (IsDragonSoul())
	{
		DWORD duration = DSManager::instance().GetDuration(this);
		DWORD remain_sec = GetSocket(ITEM_SOCKET_REMAIN_SEC);

		if (remain_sec == duration)
			return false;

		if ((dwTime + remain_sec) >= duration)
		{
			SetSocket(ITEM_SOCKET_REMAIN_SEC, duration);
			return duration - remain_sec;
		}
		else
		{
			SetSocket(ITEM_SOCKET_REMAIN_SEC, dwTime + remain_sec);
			return dwTime;
		}
	}
	// 우선 용혼석에 관해서만 하도록 한다.
	else
		return 0;
}
#endif

int CItem::GetDuration()
{
	if (!GetProto())
		return -1;

	for (int i = 0; i < ITEM_LIMIT_MAX_NUM; i++)
	{
		if (LIMIT_REAL_TIME == GetProto()->aLimits[i].bType)
			return GetProto()->aLimits[i].lValue;
	}

	if (-1 != GetProto()->cLimitTimerBasedOnWearIndex)
		return GetProto()->aLimits[GetProto()->cLimitTimerBasedOnWearIndex].lValue;

	return -1;
}

bool CItem::IsSameSpecialGroup(const LPITEM item) const
{
	// 서로 VNUM이 같다면 같은 그룹인 것으로 간주
	if (this->GetVnum() == item->GetVnum())
		return true;

	if (GetSpecialGroup() && (item->GetSpecialGroup() == GetSpecialGroup()))
		return true;

	return false;
}

DWORD CItem::GetRealImmuneFlag()
{
	DWORD dwImmuneFlag = m_pProto->dwImmuneFlag;
	for (int i = 0; i < ITEM_ATTRIBUTE_MAX_NUM; ++i)
	{
		if (GetAttributeType(i))
		{
			const TPlayerItemAttribute& ia = GetAttribute(i);

			if (ia.wType == APPLY_IMMUNE_STUN && !IS_SET(dwImmuneFlag, IMMUNE_STUN))
				SET_BIT(dwImmuneFlag, IMMUNE_STUN);
			else if (ia.wType == APPLY_IMMUNE_FALL && !IS_SET(dwImmuneFlag, IMMUNE_FALL))
				SET_BIT(dwImmuneFlag, IMMUNE_FALL);
			else if (ia.wType == APPLY_IMMUNE_SLOW && !IS_SET(dwImmuneFlag, IMMUNE_SLOW))
				SET_BIT(dwImmuneFlag, IMMUNE_SLOW);
		}
	}

	return dwImmuneFlag;
}

#if defined(__CHANGE_LOOK_SYSTEM__)
void CItem::SetTransmutationVnum(DWORD blVnum)
{
	m_dwTransmutationVnum = blVnum;
	Save();
}

DWORD CItem::GetTransmutationVnum() const
{
	return m_dwTransmutationVnum;
}
#endif

#if defined(__SET_ITEM__)
void CItem::SetItemSetValue(BYTE bSet)
{
	m_bSetValue = bSet;
	UpdatePacket();
}

BYTE CItem::GetItemSetValue() const
{
	return m_bSetValue;
}
#endif

#if defined(__REFINE_ELEMENT_SYSTEM__)
void CItem::SetRefineElement(const TPlayerItemRefineElement* pTable)
{
	thecore_memcpy(&m_RefineElement, pTable, sizeof(m_RefineElement));

	UpdatePacket();
	Save();
}

void CItem::CopyElementTo(const LPITEM pItem)
{
	pItem->SetRefineElement(&m_RefineElement);
}

void CItem::UpgradeRefineElement(WORD wApplyType, BYTE bValue, BYTE bBonusValue)
{
	BYTE& bGrade = m_RefineElement.bGrade;
	if (bGrade >= REFINE_ELEMENT_MAX)
		return;

	++bGrade;

	m_RefineElement.abValue[bGrade - 1] = bValue;
	m_RefineElement.abBonusValue[bGrade - 1] = bBonusValue;
	m_RefineElement.wApplyType = wApplyType;

	UpdatePacket();
	Save();
}

void CItem::DowngradeRefineElement()
{
	BYTE& bGrade = m_RefineElement.bGrade;
	if (bGrade == 0)
		return;

	--bGrade;

	m_RefineElement.abValue[bGrade] = 0;
	m_RefineElement.abBonusValue[bGrade] = 0;

	if (bGrade == 0)
		m_RefineElement.wApplyType = 0;

	UpdatePacket();
	Save();
}

void CItem::ChangeRefineElement(WORD wApplyType)
{
	m_RefineElement.wApplyType = wApplyType;

	UpdatePacket();
	Save();
}

BYTE CItem::GetRefineElementValue() const
{
	const BYTE& bGrade = m_RefineElement.bGrade;
	if (bGrade == 0)
		return 0;

	BYTE bValue = 0;
	for (BYTE bPos = 0; bPos < REFINE_ELEMENT_MAX && bPos < bGrade; ++bPos)
		bValue += m_RefineElement.abValue[bPos];

	return bValue;
}

BYTE CItem::GetRefineElementBonusValue() const
{
	const BYTE& bGrade = m_RefineElement.bGrade;
	if (bGrade == 0)
		return 0;

	BYTE bValue = 0;
	for (BYTE bPos = 0; bPos < REFINE_ELEMENT_MAX && bPos < bGrade; ++bPos)
		bValue += m_RefineElement.abBonusValue[bPos];

	return bValue;
}
#endif

#if defined(__SOUL_SYSTEM__)
EVENTFUNC(soul_timer_use_event)
{
	const item_vid_event_info* info = reinterpret_cast<const item_vid_event_info*>(event->info);
	if (info == nullptr)
		return 0;

	LPITEM item = ITEM_MANAGER::instance().FindByVID(info->item_vid);
	if (item == nullptr)
		return 0;

	long data = item->GetSocket(2);
	if (data == 0)
		data = item->GetValue(2);

	long keep_time = data / 10000;
	long max_time = item->GetLimitValue(1);
	int min_time = 60;
	long remain_count = data % 10000;

	if (keep_time % min_time == 0 && keep_time > 0 && item->GetSocket(1) != TRUE)
		if (const LPCHARACTER& ch = item->GetOwner())
			ch->ChatPacket(CHAT_TYPE_INFO, LC_STRING("[Soul System] Use %s to receive the effect.", LC_ITEM(item->GetVnum())));

	if (keep_time >= max_time)
		return 0;

	auto new_data = ((keep_time + 1) * 10000) + remain_count;
	item->SetSocket(2, new_data, false /* log */);

	return PASSES_PER_SEC(test_server ? 1 : 60);
}

void CItem::StartSoulTimerUseEvent()
{
	if (m_pkSoulTimerUseEvent)
		return;

	item_vid_event_info* info = AllocEventInfo<item_vid_event_info>();
	info->item_vid = GetVID();
	m_pkSoulTimerUseEvent = event_create(soul_timer_use_event, info, PASSES_PER_SEC(test_server ? 1 : 60));
}

void CItem::ResetSoulTimerUseEvent()
{
	if (m_pkSoulTimerUseEvent)
		event_cancel(&m_pkSoulTimerUseEvent);

	StartSoulTimerUseEvent();
}
#endif

#if defined(__GLOVE_SYSTEM__)
DWORD CItem::GetRandomSungMaSocketValue(BYTE bType, int iRefineLevel, bool bMultiplier)
{
	/*
	* NOTE : If you're planning to edit this function be very careful with its limitations.
	* The maximum data size cannot be more than a `LONG_MAX` value and each parameter has to
	* respect certain values.
	*
	* @param bType - Value5 of the Spirit Stone.
	* @param iRefineLeveL - Level of the Spirit Stone.
	* These parameters cannot be valued above 9!
	*
	* @param bMultiplier - Chance given to multiply the value of the bonus.
	*
	* Additional limitations to be respected:
	*	- ApplyType < 999 !
	*	- ApplyValue < 99 !
	*
	* Need Debugging?
	*	. 2394230 APPLY_SUNGMA_STR		RefineLevel 4	Type 2	ApplyValue 30
	*	. 2404230 APPLY_SUNGMA_HP		RefineLevel 4	Type 2	ApplyValue 30
	*	. 2414230 APPLY_SUNGMA_MOVE		RefineLevel 4	Type 2	ApplyValue 30
	*	. 2424230 APPLY_SUNGMA_IMMUNE	RefineLevel 4	Type 2	ApplyValue 30
	*
	* 20230511.Owsap
	*/

	if (bType >= 10 || iRefineLevel >= 10)
	{
		sys_err("CItem::GetRandomSungMaSocketValue:: bType %d (Value5) or iRefineLevel %d is too high to store in the socket data.", bType, iRefineLevel);
		return 0;
	}

	std::vector<WORD> vApplyTypes
	{
		APPLY_SUNGMA_STR,
		APPLY_SUNGMA_HP,
		APPLY_SUNGMA_MOVE,
		APPLY_SUNGMA_IMMUNE,
	};

	const BYTE bDefaultValues[] = { 1, 3, 6, 9, 15 };
	WORD wApplyType = vApplyTypes[number(0, (vApplyTypes.size() - 1))];

	// DUPLICATE_APPLY_TYPE_FIX
	std::unordered_set<WORD> wRepeatedSet;
	for (BYTE bSocketIndex = 0; bSocketIndex < ITEM_SOCKET_MAX_NUM; ++bSocketIndex)
	{
		const DWORD dwData = GetSocket(bSocketIndex);
		if (dwData < 1000000)
			continue;

		const WORD wSocketApplyType = (dwData / 10000) % 1000;
		if (std::find(vApplyTypes.begin(), vApplyTypes.end(), wSocketApplyType) != vApplyTypes.end())
			wRepeatedSet.emplace(wSocketApplyType);
	}

	bool bCheckRepeat = true;
	while (bCheckRepeat)
	{
		wApplyType = vApplyTypes[number(0, (vApplyTypes.size() - 1))];
		if (wRepeatedSet.count(wApplyType) == 0)
			bCheckRepeat = false;
	}
	// END_OF_DUPLICATE_APPLY_TYPE_FIX

	long lValue = bDefaultValues[iRefineLevel];
	if (lValue >= 100)
	{
		sys_err("CItem::GetRandomSungMaSocketValue:: ApplyValue %ld is too high to store in the socket data.", lValue);
		return 0;
	}

	if (bMultiplier)
		lValue *= 2;

	DWORD dwData = (wApplyType * 10000) + (iRefineLevel * 1000) + (bType * 100) + lValue;
	if (dwData >= LONG_MAX)
	{
		sys_err("CItem::GetRandomSungMaSocketValue:: Unsupported data!");
		return 0;
	}

	return dwData;
}
#endif
