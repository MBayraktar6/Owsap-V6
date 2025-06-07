#include "stdafx.h"

#include "../../common/tables.h"

#include "buff_on_attributes.h"
#include "item.h"
#include "char.h"

CBuffOnAttributes::CBuffOnAttributes(LPCHARACTER pOwner, POINT_TYPE wPointType, std::vector<POINT_TYPE>* vec_pBuffWearTargets)
	: m_pBuffOwner(pOwner), m_wPointType(wPointType), m_vec_pBuffWearTargets(vec_pBuffWearTargets)
{
	Initialize();
}

CBuffOnAttributes::~CBuffOnAttributes()
{
	Off();
}

void CBuffOnAttributes::Initialize()
{
	m_lBuffValue = 0;
	m_map_AdditionalAttrs.clear();
}

void CBuffOnAttributes::RemoveBuffFromItem(LPITEM pItem)
{
	if (0 == m_lBuffValue)
		return;

	if (pItem == nullptr)
		return;

	if (pItem->GetCell() < INVENTORY_MAX_NUM)
		return;

	const auto& it = std::find(m_vec_pBuffWearTargets->begin(), m_vec_pBuffWearTargets->end(), pItem->GetCell());
	if (m_vec_pBuffWearTargets->end() == it)
		return;

	BYTE bAttrCount = pItem->GetAttributeCount();
	for (BYTE bIndex = 0; bIndex < bAttrCount; bIndex++)
	{
		TPlayerItemAttribute PlayerItemAttribute = pItem->GetAttribute(bIndex);
		const TMapAttr::iterator& it = m_map_AdditionalAttrs.find(PlayerItemAttribute.wType);
		// m_map_AdditionalAttrs에서 해당 attribute type에 대한 값을 제거하고,
		// 변경된 값의 (m_llBuffValue)%만큼의 버프 효과 감소
		if (it != m_map_AdditionalAttrs.end())
		{
			POINT_VALUE& llSumOfAttrValue = it->second;
			POINT_VALUE llOldValue = llSumOfAttrValue * m_lBuffValue / 100;
			POINT_VALUE llNewValue = (llSumOfAttrValue - PlayerItemAttribute.lValue) * m_lBuffValue / 100;
			m_pBuffOwner->ApplyPoint(PlayerItemAttribute.wType, llNewValue - llOldValue);
			llSumOfAttrValue -= PlayerItemAttribute.lValue;
		}
		else
		{
			sys_err("Buff ERROR(type %u). This item(%u) attr_type(%u) was not in buff pool", m_wPointType, pItem->GetVnum(), PlayerItemAttribute.wType);
			return;
		}
	}
}

void CBuffOnAttributes::AddBuffFromItem(LPITEM pItem)
{
	if (0 == m_lBuffValue)
		return;

	if (pItem == nullptr)
		return;

	if (pItem->GetCell() < INVENTORY_MAX_NUM)
		return;

	const auto& it = std::find(m_vec_pBuffWearTargets->begin(), m_vec_pBuffWearTargets->end(), pItem->GetCell());
	if (m_vec_pBuffWearTargets->end() == it)
		return;

	BYTE bAttrCount = pItem->GetAttributeCount();
	for (BYTE bIndex = 0; bIndex < bAttrCount; bIndex++)
	{
		TPlayerItemAttribute PlayerItemAttribute = pItem->GetAttribute(bIndex);
		const TMapAttr::iterator& it = m_map_AdditionalAttrs.find(PlayerItemAttribute.wType);

		// m_map_AdditionalAttrs에서 해당 attribute type에 대한 값이 없다면 추가.
		// 추가된 값의 (m_llBuffValue)%만큼의 버프 효과 추가
		if (it == m_map_AdditionalAttrs.end())
		{
			m_pBuffOwner->ApplyPoint(PlayerItemAttribute.wType, PlayerItemAttribute.lValue * m_lBuffValue / 100);
			m_map_AdditionalAttrs.insert(TMapAttr::value_type(PlayerItemAttribute.wType, PlayerItemAttribute.lValue));
		}
		// m_map_AdditionalAttrs 에서 해당 attribute type에 대한 값이 있다면, 그 값을 증가시키고,
		// 변경된 값의 (m_llBuffValue)%만큼의 버프 효과 추가
		else
		{
			POINT_VALUE& llSumOfAttrValue = it->second;
			POINT_VALUE llOldValue = llSumOfAttrValue * m_lBuffValue / 100;
			POINT_VALUE llNewValue = (llSumOfAttrValue + PlayerItemAttribute.lValue) * m_lBuffValue / 100;
			m_pBuffOwner->ApplyPoint(PlayerItemAttribute.wType, llNewValue - llOldValue);
			llSumOfAttrValue += PlayerItemAttribute.lValue;
		}
	}
}

void CBuffOnAttributes::ChangeBuffValue(POINT_VALUE lNewValue)
{
	if (0 == m_lBuffValue)
		On(lNewValue);
	else if (0 == lNewValue)
		Off();
	else
	{
		// 기존에, m_map_AdditionalAttrs 의 값의 (m_bBuffValue)%만큼이 버프로 들어가 있었으므로,
		// (llNewValue)%만큼으로 값을 변경함.
		for (const TMapAttr::iterator::value_type& it : m_map_AdditionalAttrs)
		{
			const POINT_VALUE& llSumOfAttrValue = it.second;
			POINT_VALUE old_value = llSumOfAttrValue * m_lBuffValue / 100;
			POINT_VALUE new_value = llSumOfAttrValue * lNewValue / 100;

			m_pBuffOwner->ApplyPoint(it.first, -llSumOfAttrValue * m_lBuffValue / 100);
		}
		m_lBuffValue = lNewValue;
	}
}

void CBuffOnAttributes::GiveAllAttributes()
{
	if (0 == m_lBuffValue)
		return;

	for (const TMapAttr::iterator::value_type& it : m_map_AdditionalAttrs)
	{
		POINT_TYPE dwApplyType = it.first;
		POINT_VALUE llApplyValue = it.second * m_lBuffValue / 100;

		m_pBuffOwner->ApplyPoint(dwApplyType, llApplyValue);
	}
}

bool CBuffOnAttributes::On(POINT_VALUE lValue)
{
	if (0 != m_lBuffValue || 0 == lValue)
		return false;

	m_map_AdditionalAttrs.clear();

	for (std::size_t nPos = 0; nPos < m_vec_pBuffWearTargets->size(); nPos++)
	{
		const LPITEM& pItem = m_pBuffOwner->GetWear(m_vec_pBuffWearTargets->at(nPos));
		if (pItem == nullptr)
			continue;

		BYTE bAttrCount = pItem->GetAttributeCount();
		for (BYTE bIndex = 0; bIndex < bAttrCount; bIndex++)
		{
			TPlayerItemAttribute PlayerItemAttribute = pItem->GetAttribute(bIndex);
			const TMapAttr::iterator& it = m_map_AdditionalAttrs.find(PlayerItemAttribute.wType);
			if (it != m_map_AdditionalAttrs.end())
			{
				it->second += PlayerItemAttribute.lValue;
			}
			else
			{
				m_map_AdditionalAttrs.emplace(TMapAttr::value_type(PlayerItemAttribute.wType, PlayerItemAttribute.lValue));
			}
		}
	}

	for (auto const& it : m_map_AdditionalAttrs)
		m_pBuffOwner->ApplyPoint(it.first, it.second * lValue / 100);

	m_lBuffValue = lValue;

	return true;
}

void CBuffOnAttributes::Off()
{
	if (0 == m_lBuffValue)
		return;

	for (const auto& it : m_map_AdditionalAttrs)
		m_pBuffOwner->ApplyPoint(it.first, it.second * m_lBuffValue / 100);

	Initialize();
}
