#include "stdafx.h"
#include "constants.h"
#include "log.h"
#include "item.h"
#include "char.h"
#include "desc.h"
#include "item_manager.h"
#include "config.h"

const int MAX_NORM_ATTR_NUM = ITEM_MANAGER::MAX_NORM_ATTR_NUM;
const int MAX_RARE_ATTR_NUM = ITEM_MANAGER::MAX_RARE_ATTR_NUM;

int CItem::GetAttributeSetIndex()
{
	if (GetType() == ITEM_WEAPON)
	{
		if (GetSubType() == WEAPON_ARROW)
			return -1;

#if defined(__QUIVER_SYSTEM__)
		if (GetSubType() == WEAPON_QUIVER)
			return -1;
#endif

		return ATTRIBUTE_SET_WEAPON;
	}

	if (GetType() == ITEM_ARMOR)
	{
		switch (GetSubType())
		{
			case ARMOR_BODY:
				//case COSTUME_BODY: // 코스츔 갑옷은 일반 갑옷과 동일한 Attribute Set을 이용하여 랜덤속성 붙음 (ARMOR_BODY == COSTUME_BODY)
				return ATTRIBUTE_SET_BODY;

			case ARMOR_WRIST:
				return ATTRIBUTE_SET_WRIST;

			case ARMOR_FOOTS:
				return ATTRIBUTE_SET_FOOTS;

			case ARMOR_NECK:
				return ATTRIBUTE_SET_NECK;

			case ARMOR_HEAD:
				// case COSTUME_HAIR: // 코스츔 헤어는 일반 투구 아이템과 동일한 Attribute Set을 이용하여 랜덤속성 붙음 (ARMOR_HEAD == COSTUME_HAIR)
				return ATTRIBUTE_SET_HEAD;

			case ARMOR_SHIELD:
				return ATTRIBUTE_SET_SHIELD;

			case ARMOR_EAR:
				return ATTRIBUTE_SET_EAR;

#if defined(__PENDANT_SYSTEM__)
			case ARMOR_PENDANT:
				return ATTRIBUTE_SET_PENDANT;
#endif

#if defined(__GLOVE_SYSTEM__)
			case ARMOR_GLOVE:
				return ATTRIBUTE_SET_GLOVE;
#endif
		}
	}
#if defined(__COSTUME_SYSTEM__)
	else if (GetType() == ITEM_COSTUME)
	{
		switch (GetSubType())
		{
			case COSTUME_BODY:
				return ATTRIBUTE_SET_COSTUME_BODY;

			case COSTUME_HAIR:
				return ATTRIBUTE_SET_COSTUME_HAIR;

#if defined(__MOUNT_COSTUME_SYSTEM__)
			case COSTUME_MOUNT:
				break;
#endif

#if defined(__ACCE_COSTUME_SYSTEM__)
			case COSTUME_ACCE:
				break;
#endif

#if defined(__WEAPON_COSTUME_SYSTEM__)
			case COSTUME_WEAPON:
				return ATTRIBUTE_SET_COSTUME_WEAPON;
#endif

#if defined(__AURA_COSTUME_SYSTEM__)
			case COSTUME_AURA:
				break;
#endif
		}
	}
#endif

	return -1;
}

bool CItem::HasAttr(POINT_TYPE wApply)
{
	for (int i = 0; i < ITEM_APPLY_MAX_NUM; ++i)
		if (m_pProto->aApplies[i].wType == wApply)
			return true;

	for (int i = 0; i < MAX_NORM_ATTR_NUM; ++i)
		if (GetAttributeType(i) == wApply)
			return true;

	return false;
}

bool CItem::HasRareAttr(POINT_TYPE wType)
{
	for (int i = 0; i < MAX_RARE_ATTR_NUM; ++i)
		if (GetAttributeType(i + MAX_NORM_ATTR_NUM) == wType)
			return true;

	return false;
}

void CItem::AddAttribute(POINT_TYPE dwType, POINT_VALUE llValue)
{
	if (HasAttr(dwType))
		return;

	BYTE bIndex = GetAttributeCount();
	if (bIndex >= MAX_NORM_ATTR_NUM)
		sys_err("item attribute overflow!");
	else
	{
		if (llValue)
			SetAttribute(bIndex, dwType, llValue);
	}
}

void CItem::AddAttr(POINT_TYPE wApply, BYTE bLevel)
{
	if (HasAttr(wApply))
		return;

	if (bLevel <= 0)
		return;

	int i = GetAttributeCount();

	if (i == MAX_NORM_ATTR_NUM)
		sys_err("item attribute overflow!");
	else
	{
		const TItemAttrTable& r = g_map_itemAttr[wApply];
		long lVal = r.lValues[MIN(4, bLevel - 1)];

		if (lVal)
			SetAttribute(i, wApply, lVal);
	}
}

void CItem::PutAttributeWithLevel(BYTE bLevel)
{
	const int iAttributeSet = GetAttributeSetIndex();
	if (iAttributeSet < 0)
		return;

	if (bLevel > ITEM_ATTRIBUTE_MAX_LEVEL)
		return;

	std::vector<int> avail;

	int total = 0;

	// 붙일 수 있는 속성 배열을 구축
	for (int i = 0; i < MAX_APPLY_NUM; ++i)
	{
		const TItemAttrTable& r = g_map_itemAttr[i];

		if (r.bMaxLevelBySet[iAttributeSet] && !HasAttr(i))
		{
			avail.push_back(i);
			total += r.dwProb;
		}
	}

	// 구축된 배열로 확률 계산을 통해 붙일 속성 선정
	unsigned int prob = number(1, total);
	int attr_idx = APPLY_NONE;

	for (DWORD i = 0; i < avail.size(); ++i)
	{
		const TItemAttrTable& r = g_map_itemAttr[avail[i]];

		if (prob <= r.dwProb)
		{
			attr_idx = avail[i];
			break;
		}

		prob -= r.dwProb;
	}

	if (!attr_idx)
	{
		sys_err("Cannot put item attribute %d %d", iAttributeSet, bLevel);
		return;
	}

	const TItemAttrTable& r = g_map_itemAttr[attr_idx];

	// 종류별 속성 레벨 최대값 제한
	if (bLevel > r.bMaxLevelBySet[iAttributeSet])
		bLevel = r.bMaxLevelBySet[iAttributeSet];

	AddAttr(attr_idx, bLevel);
}

void CItem::PutAttribute(const int* aiAttrPercentTable)
{
	int iAttrLevelPercent = number(1, 100);
	int i;

	for (i = 0; i < ITEM_ATTRIBUTE_MAX_LEVEL; ++i)
	{
		if (iAttrLevelPercent <= aiAttrPercentTable[i])
			break;

		iAttrLevelPercent -= aiAttrPercentTable[i];
	}

	PutAttributeWithLevel(i + 1);
}

void CItem::ChangeAttribute(const int* aiChangeProb)
{
	const int iAttributeCount = GetAttributeCount();

	ClearAttribute();

	if (iAttributeCount == 0)
		return;

	TItemTable const* pProto = GetProto();

	if (pProto && pProto->sAddonType)
	{
		ApplyAddon(pProto->sAddonType);
	}

	static const int tmpChangeProb[ITEM_ATTRIBUTE_MAX_LEVEL] =
	{
		0, 10, 40, 35, 15,
	};

	for (int i = GetAttributeCount(); i < iAttributeCount; ++i)
	{
		if (aiChangeProb == NULL)
		{
			PutAttribute(tmpChangeProb);
		}
		else
		{
			PutAttribute(aiChangeProb);
		}
	}
}

void CItem::AddAttribute()
{
	static const int aiItemAddAttributePercent[ITEM_ATTRIBUTE_MAX_LEVEL] =
	{
		40, 50, 10, 0, 0
	};

	if (GetAttributeCount() < MAX_NORM_ATTR_NUM)
		PutAttribute(aiItemAddAttributePercent);
}

void CItem::ClearAttribute()
{
	for (int i = 0; i < MAX_NORM_ATTR_NUM; ++i)
	{
		m_aAttr[i].wType = 0;
		m_aAttr[i].lValue = 0;
	}
}

void CItem::ClearAllAttribute()
{
	for (int i = 0; i < ITEM_ATTRIBUTE_MAX_NUM; ++i)
	{
		m_aAttr[i].wType = 0;
		m_aAttr[i].lValue = 0;
	}
}

BYTE CItem::GetAttributeCount()
{
	BYTE bIndex = 0;
	for (; bIndex < MAX_NORM_ATTR_NUM; ++bIndex)
	{
		if (GetAttributeType(bIndex) == 0)
			break;
	}
	return bIndex;
}

int CItem::FindAttribute(POINT_TYPE wType)
{
	for (int i = 0; i < MAX_NORM_ATTR_NUM; ++i)
	{
		if (GetAttributeType(i) == wType)
			return i;
	}

	return -1;
}

bool CItem::RemoveAttributeAt(int index)
{
	if (GetAttributeCount() <= index)
		return false;

	for (int i = index; i < MAX_NORM_ATTR_NUM - 1; ++i)
	{
		SetAttribute(i, GetAttributeType(i + 1), GetAttributeValue(i + 1));
	}

	SetAttribute(MAX_NORM_ATTR_NUM - 1, APPLY_NONE, 0);
	return true;
}

bool CItem::RemoveAttributeType(POINT_TYPE wType)
{
	int index = FindAttribute(wType);
	return index != -1 && RemoveAttributeType(index);
}

void CItem::SetAttributes(const TPlayerItemAttribute* c_pAttribute)
{
	thecore_memcpy(m_aAttr, c_pAttribute, sizeof(m_aAttr));
	Save();
}

void CItem::SetAttribute(BYTE bIndex, POINT_TYPE wType, POINT_VALUE lValue)
{
	assert(bIndex < MAX_NORM_ATTR_NUM);

	m_aAttr[bIndex].wType = wType;
	m_aAttr[bIndex].lValue = lValue;

	UpdatePacket();
	Save();
}

void CItem::SetForceAttribute(BYTE bIndex, POINT_TYPE wType, POINT_VALUE lValue)
{
	assert(bIndex < ITEM_ATTRIBUTE_MAX_NUM);

	m_aAttr[bIndex].wType = wType;
	m_aAttr[bIndex].lValue = lValue;

	UpdatePacket();
	Save();
}

void CItem::CopyAttributeTo(LPITEM pItem)
{
	pItem->SetAttributes(m_aAttr);
}

#if defined(__ITEM_APPLY_RANDOM__)
void CItem::CopyRandomAppliesTo(LPITEM pItem)
{
	pItem->SetRandomApplies(m_aApplyRandom);
}
#endif

BYTE CItem::GetRareAttrCount()
{
	BYTE bCount = 0;
	for (BYTE bIndex = ITEM_ATTRIBUTE_RARE_START; bIndex < ITEM_ATTRIBUTE_RARE_END; bIndex++)
		if (m_aAttr[bIndex].wType != 0)
			bCount++;
	return bCount;
}

bool CItem::ChangeRareAttribute()
{
	if (GetRareAttrCount() == 0)
		return false;

	const int cnt = GetRareAttrCount();

	for (int i = 0; i < cnt; ++i)
	{
		m_aAttr[i + ITEM_ATTRIBUTE_RARE_START].wType = 0;
		m_aAttr[i + ITEM_ATTRIBUTE_RARE_START].lValue = 0;
	}

	if (g_bAttrLog)
	{
		if (GetOwner() && GetOwner()->GetDesc())
			LogManager::instance().ItemLog(GetOwner(), this, "SET_RARE_CHANGE", "");
		else
			LogManager::instance().ItemLog(0, 0, 0, GetID(), "SET_RARE_CHANGE", "", "", GetOriginalVnum());
	}

	for (int i = 0; i < cnt; ++i)
		AddRareAttribute();

	return true;
}

bool CItem::AddRareAttribute()
{
	int count = GetRareAttrCount();
	if (count >= ITEM_ATTRIBUTE_RARE_NUM)
		return false;

	int pos = count + ITEM_ATTRIBUTE_RARE_START;
	TPlayerItemAttribute& attr = m_aAttr[pos];

	const int nAttrSet = GetAttributeSetIndex();
	std::vector<int> avail;

	for (int i = 0; i < MAX_APPLY_NUM; ++i)
	{
		const TItemAttrTable& r = g_map_itemRare[i];

		if (r.wApplyIndex != 0 && r.bMaxLevelBySet[nAttrSet] > 0 && HasRareAttr(i) != true)
		{
			avail.emplace_back(i);
		}
	}

	if (avail.empty())
	{
		sys_err("Couldn't add a rare bonus - item_attr_rare has incorrect values!");
		return false;
	}

	const TItemAttrTable& r = g_map_itemRare[avail[number(0, avail.size() - 1)]];
	int nAttrLevel = number(1, ITEM_ATTRIBUTE_MAX_LEVEL);

	if (nAttrLevel > r.bMaxLevelBySet[nAttrSet])
		nAttrLevel = r.bMaxLevelBySet[nAttrSet];

	attr.wType = r.wApplyIndex;
	attr.lValue = r.lValues[nAttrLevel - 1];

	UpdatePacket();

	Save();

	const char* pszIP = nullptr;

	if (GetOwner() && GetOwner()->GetDesc())
		pszIP = GetOwner()->GetDesc()->GetHostName();

	LogManager::instance().ItemLog(pos, attr.wType, attr.lValue, GetID(), "SET_RARE", "", pszIP ? pszIP : "", GetOriginalVnum());
	return true;
}

#if defined(__CHANGED_ATTR__)
#include "utils.h"
void CItem::GetSelectAttr(TPlayerItemAttribute(&arr)[ITEM_ATTRIBUTE_MAX_NUM])
{
	auto __GetAttributeCount = [&arr]() -> BYTE
	{
		auto c = std::count_if(std::begin(arr), std::end(arr),
			[](const TPlayerItemAttribute& _attr) { return _attr.wType != 0 && _attr.lValue != 0; });

		return static_cast<BYTE>(c);
	};

	auto __HasAttr = [this, &arr](const DWORD dwApply) -> bool
	{
		if (m_pProto)
		{
			for (BYTE bIndex = 0; bIndex < ITEM_APPLY_MAX_NUM; ++bIndex)
				if (m_pProto->aApplies[bIndex].wType == dwApply)
					return true;
		}

		for (BYTE bIndex = 0; bIndex < MAX_NORM_ATTR_NUM; ++bIndex)
			if (arr[bIndex].wType == dwApply)
				return true;

		return false;
	};

	auto __PutAttributeWithLevel = [this, &__HasAttr, &__GetAttributeCount, &arr](BYTE bLevel) -> void
	{
		const int iAttributeSet = GetAttributeSetIndex();

		if (iAttributeSet < 0)
			return;

		if (bLevel > ITEM_ATTRIBUTE_MAX_LEVEL)
			return;

		std::vector<WORD> avail;

		int total = 0;

		for (WORD i = 0; i < MAX_APPLY_NUM; ++i)
		{
			const TItemAttrTable& r = g_map_itemAttr[i];

			if (r.bMaxLevelBySet[iAttributeSet] && !__HasAttr(i))
			{
				avail.emplace_back(i);
				total += r.dwProb;
			}
		}

		unsigned int prob = number(1, total);
		WORD attr_idx = APPLY_NONE;

		for (size_t i = 0; i < avail.size(); ++i)
		{
			const TItemAttrTable& r = g_map_itemAttr[avail[i]];

			if (prob <= r.dwProb)
			{
				attr_idx = avail[i];
				break;
			}

			prob -= r.dwProb;
		}

		if (!attr_idx)
		{
			sys_err("Cannot put item attribute %d %d", iAttributeSet, bLevel);
			return;
		}

		const TItemAttrTable& r = g_map_itemAttr[attr_idx];
		if (bLevel > r.bMaxLevelBySet[iAttributeSet])
			bLevel = r.bMaxLevelBySet[iAttributeSet];

		const long lVal = r.lValues[MIN(4, bLevel - 1)];
		arr[__GetAttributeCount()] = { attr_idx, lVal, 0 };
	};

	if (m_pProto && m_pProto->sAddonType)
	{
		long iSkillBonus = MINMAX(-30, static_cast<int>(gauss_random(0, 5) + 0.5f), 30);
		long iNormalHitBonus = 0;
		if (abs(iSkillBonus) <= 20)
			iNormalHitBonus = -2 * iSkillBonus + abs(number(-8, 8) + number(-8, 8)) + number(1, 4);
		else
			iNormalHitBonus = -2 * iSkillBonus + number(1, 5);

		arr[__GetAttributeCount()] = { APPLY_NORMAL_HIT_DAMAGE_BONUS, iNormalHitBonus, 0 };
		arr[__GetAttributeCount()] = { APPLY_SKILL_DAMAGE_BONUS, iSkillBonus, 0 };
	}

	static constexpr BYTE aiAttrPercentTable[MAX_NORM_ATTR_NUM] = { 0, 10, 40, 35, 15 };
	for (BYTE c = __GetAttributeCount(); c < GetAttributeCount(); c++)
	{
		int iAttrLevelPercent = number(1, 100);
		BYTE i;

		for (i = 0; i < MAX_NORM_ATTR_NUM; ++i)
		{
			if (iAttrLevelPercent <= aiAttrPercentTable[i])
				break;

			iAttrLevelPercent -= aiAttrPercentTable[i];
		}

		__PutAttributeWithLevel(i + 1);
	}

	std::copy(std::begin(m_aAttr) + MAX_NORM_ATTR_NUM, std::end(m_aAttr), std::begin(arr) + MAX_NORM_ATTR_NUM);
}
#endif

#if defined(__ITEM_APPLY_RANDOM__)
void CItem::SetRandomApply(BYTE bIndex, POINT_TYPE wType, POINT_VALUE lValue, BYTE bPath)
{
	assert(bIndex < ITEM_APPLY_MAX_NUM);

	m_aApplyRandom[bIndex].wType = wType;
	m_aApplyRandom[bIndex].lValue = lValue;
	m_aApplyRandom[bIndex].bPath = bPath;

	UpdatePacket();
	Save();
}

void CItem::SetForceRandomApply(BYTE bIndex, POINT_TYPE wType, POINT_VALUE lValue, BYTE bPath)
{
	assert(bIndex < ITEM_APPLY_MAX_NUM);

	m_aApplyRandom[bIndex].wType = wType;
	m_aApplyRandom[bIndex].lValue = lValue;
	m_aApplyRandom[bIndex].bPath = bPath;

	UpdatePacket();
	Save();
}

void CItem::SetRandomApplies(const TPlayerItemAttribute* c_pApplyRandom)
{
	thecore_memcpy(m_aApplyRandom, c_pApplyRandom, sizeof(m_aApplyRandom));
	Save();
}

void CItem::GetRandomApplyTable(TPlayerItemAttribute* pApplyRandomTable, BYTE bGetType)
{
	int iRefineLevel = GetRefineLevel();
	const TItemTable* c_pItemProto = GetProto();
	if (c_pItemProto == nullptr)
		return;

	switch (bGetType)
	{
		case CApplyRandomTable::GET_PREVIOUS:
			--iRefineLevel;
			break;

		case CApplyRandomTable::GET_NEXT:
			++iRefineLevel;
			break;

		case CApplyRandomTable::GET_CURRENT:
		default:
			break;
	}

	if (iRefineLevel > ITEM_REFINE_MAX_LEVEL)
		iRefineLevel = ITEM_REFINE_MAX_LEVEL;

	if (iRefineLevel < 0)
		iRefineLevel = 0;

	memset(pApplyRandomTable, 0, sizeof(TPlayerItemAttribute) * ITEM_APPLY_MAX_NUM);

	for (BYTE bApplyIndex = 0; bApplyIndex < ITEM_APPLY_MAX_NUM; ++bApplyIndex)
	{
		POINT_TYPE wApplyRandomType = c_pItemProto->aApplies[bApplyIndex].wType;
		POINT_VALUE lApplyRandomValue = c_pItemProto->aApplies[bApplyIndex].lValue;

		if (wApplyRandomType == APPLY_RANDOM)
		{
			pApplyRandomTable[bApplyIndex].wType = m_aApplyRandom[bApplyIndex].wType;
			pApplyRandomTable[bApplyIndex].lValue = ITEM_MANAGER::instance().GetApplyRandomValue(lApplyRandomValue, iRefineLevel, m_aApplyRandom[bApplyIndex].bPath, m_aApplyRandom[bApplyIndex].wType);
			pApplyRandomTable[bApplyIndex].bPath = m_aApplyRandom[bApplyIndex].bPath;
		}
	}
}
#endif
