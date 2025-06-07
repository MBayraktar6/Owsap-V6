#ifndef __INC_BUFF_ON_ATTRIBUTES_H__
#define __INC_BUFF_ON_ATTRIBUTES_H__

class CHARACTER;

class CBuffOnAttributes
{
public:
	CBuffOnAttributes(LPCHARACTER pOwner, POINT_TYPE wPointType, std::vector<POINT_TYPE>* vec_pBuffWearTargets);
	~CBuffOnAttributes();

	// 장착 중 이면서, m_vec_pBuffWearTargets 에 해당하는 아이템인 경우, 해당 아이템으로 인해 붙은 효과를 제거.
	void RemoveBuffFromItem(LPITEM pItem);
	// m_vec_pBuffWearTargets 에 해당하는 아이템인 경우, 해당 아이템의 attribute 에 대한 효과 추가.
	void AddBuffFromItem(LPITEM pItem);
	// m_llBuffValue를 바꾸고, 버프의 값도 바꿈.
	void ChangeBuffValue(POINT_VALUE lNewValue);
	// CHRACTRE::ComputePoints에서 속성치를 초기화하고 다시 계산하므로, 
	// 버프 속성치들을 강제적으로 owner에게 줌.
	void GiveAllAttributes();

	// m_vec_pBuffWearTargets 에 해당하는 모든 아이템의 attribute를 type별로 합산하고,
	// 그 attribute들의 (m_llBuffValue)% 만큼을 버프로 줌.
	bool On(POINT_VALUE dwValue);
	// 버프 제거 후, 초기화
	void Off();

	void Initialize();

private:
	LPCHARACTER m_pBuffOwner;
	POINT_TYPE m_wPointType;
	POINT_VALUE m_lBuffValue;
	std::vector<POINT_TYPE>* m_vec_pBuffWearTargets;

	// apply_type, apply_value 페어의 맵
	typedef std::map<POINT_TYPE, POINT_VALUE> TMapAttr;
	// m_vec_pBuffWearTargets 에 해당하는 모든 아이템의 attribute 를 type별로 합산하여 가지고 있음.
	TMapAttr m_map_AdditionalAttrs;
};

#endif // __INC_BUFF_ON_ATTRIBUTES_H__
