/**
* Filename: PythonGraphicOnOff.h
* Author: blackdragonx61 (aka. Mali)
**/

#pragma once

#if defined(ENABLE_GRAPHIC_ON_OFF)
class CPythonGraphicOnOff : public CSingleton<CPythonGraphicOnOff>
{
public:
	CPythonGraphicOnOff();
	~CPythonGraphicOnOff();

	void Destroy();

	bool SetEffectOnOffLevel(BYTE bVal);
	bool SetPrivateShopOnOffLevel(BYTE bVal);
	bool SetDropItemOnOffLevel(BYTE bVal);
	bool SetPetOnOffStatus(BYTE bVal);
	bool SetNPCNameOnOffStatus(BYTE bVal);

	BYTE GetEffectOnOffLevel() const;
	BYTE GetPrivateShopOnOffLevel() const;
	BYTE GetDropItemOnOffLevel() const;
	BYTE GetPetOnOffStatus() const;
	BYTE GetNPCNameOnOffStatus() const;

	bool CanRenderEffect(UINT iActorType, bool bIsMain, BYTE bRank) const;
	bool CanRenderPrivateShop(DWORD dwDistance) const;
	bool CanRenderDropItem(BYTE bLevel) const;
	bool CanRenderPet() const;
	bool CanRenderNPCName() const;

private:
	enum EOPTIONLIMIT : BYTE
	{
		EFFECT_LEVEL_COUNT = 5,
		PRIVATE_SHOP_LEVEL_COUNT = 5,
		DROP_ITEM_LEVEL_COUNT = 5,
		PET_LEVEL_COUNT = 2,
		NPC_NAME_LEVEL_COUNT = 2,
	};

	BYTE m_bEffectLevel;
	BYTE m_bPrivateShopLevel;
	BYTE m_bDropItemLevel;
	BYTE m_bPetLevel;
	BYTE m_bNPCNameLevel;
};
#endif
