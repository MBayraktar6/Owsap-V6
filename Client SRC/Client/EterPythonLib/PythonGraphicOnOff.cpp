/**
* Filename: PythonGraphicOnOff.cpp
* Author: blackdragonx61 (aka. Mali)
**/

#include "StdAfx.h"

#if defined(ENABLE_GRAPHIC_ON_OFF)
#include "PythonGraphicOnOff.h"

#include "../GameLib/MapType.h"
#include "../GameLib/GameType.h"
#include "../GameLib/ActorInstance.h"

CPythonGraphicOnOff::CPythonGraphicOnOff() :
	m_bEffectLevel(0),
	m_bPrivateShopLevel(0),
	m_bDropItemLevel(0),
	m_bPetLevel(0),
	m_bNPCNameLevel(0)
{
}

CPythonGraphicOnOff::~CPythonGraphicOnOff()
{
	Tracef("Python GraphicOnOff Clear\n");
}

void CPythonGraphicOnOff::Destroy()
{
	Tracef("Python GraphicOnOff Destroy\n");
}

bool CPythonGraphicOnOff::SetEffectOnOffLevel(BYTE bVal)
{
	if (bVal >= EOPTIONLIMIT::EFFECT_LEVEL_COUNT)
		return false;

	m_bEffectLevel = bVal;

	return true;
}

bool CPythonGraphicOnOff::SetPrivateShopOnOffLevel(BYTE bVal)
{
	if (bVal >= EOPTIONLIMIT::PRIVATE_SHOP_LEVEL_COUNT)
		return false;

	m_bPrivateShopLevel = bVal;

	return true;
}

bool CPythonGraphicOnOff::SetDropItemOnOffLevel(BYTE bVal)
{
	if (bVal >= EOPTIONLIMIT::DROP_ITEM_LEVEL_COUNT)
		return false;

	m_bDropItemLevel = bVal;

	return true;
}

bool CPythonGraphicOnOff::SetPetOnOffStatus(BYTE bVal)
{
	if (bVal >= EOPTIONLIMIT::PET_LEVEL_COUNT)
		return false;

	m_bPetLevel = bVal;

	return true;
}

bool CPythonGraphicOnOff::SetNPCNameOnOffStatus(BYTE bVal)
{
	if (bVal >= EOPTIONLIMIT::NPC_NAME_LEVEL_COUNT)
		return false;

	m_bNPCNameLevel = bVal;

	return true;
}

BYTE CPythonGraphicOnOff::GetEffectOnOffLevel() const
{
	return m_bEffectLevel;
}

BYTE CPythonGraphicOnOff::GetPrivateShopOnOffLevel() const
{
	return m_bPrivateShopLevel;
}

BYTE CPythonGraphicOnOff::GetDropItemOnOffLevel() const
{
	return m_bDropItemLevel;
}

BYTE CPythonGraphicOnOff::GetPetOnOffStatus() const
{
	return m_bPetLevel;
}

BYTE CPythonGraphicOnOff::GetNPCNameOnOffStatus() const
{
	return m_bNPCNameLevel;
}

bool CPythonGraphicOnOff::CanRenderEffect(UINT iActorType, bool bIsMain, BYTE bRank) const
{
	switch (iActorType)
	{
		case CActorInstance::EType::TYPE_ENEMY:
			if (bRank >= NRaceData::EMobRank::MOB_RANK_BOSS)
				return true;
			break;
		case CActorInstance::EType::TYPE_NPC:
		case CActorInstance::EType::TYPE_WARP:
		case CActorInstance::EType::TYPE_GOTO:
			if (bRank >= NRaceData::EMobRank::MOB_RANK_KING)
				return true;
			break;
	}

	switch (m_bEffectLevel)
	{
		case 1:
			return (iActorType == CActorInstance::EType::TYPE_ENEMY || (iActorType == CActorInstance::EType::TYPE_PC && bIsMain));
		case 2:
			return (iActorType == CActorInstance::EType::TYPE_PC);
		case 3:
			return (iActorType == CActorInstance::EType::TYPE_PC && bIsMain);
		case 4:
			return false;
		default:
			return true;
	}
}

bool CPythonGraphicOnOff::CanRenderPrivateShop(DWORD dwDistance) const
{
	switch (m_bPrivateShopLevel)
	{
		case 0:
			return (dwDistance <= 0x5F5E100);
		case 1:
			return (dwDistance <= 0x17D7840);
		case 2:
			return (dwDistance <= 0x5F5E10);
		case 3:
			return (dwDistance <= 0xF4240);
		case 4:
			return (dwDistance <= 0x15F90);
		default:
			return true;
	}
}

bool CPythonGraphicOnOff::CanRenderDropItem(BYTE bLevel) const
{
	switch (m_bDropItemLevel)
	{
		case 0:
			return true;
		case 4:
			return false;
		default:
			return (bLevel > m_bDropItemLevel);
	}
}

bool CPythonGraphicOnOff::CanRenderPet() const
{
	return m_bPetLevel == 0;
}

bool CPythonGraphicOnOff::CanRenderNPCName() const
{
	return m_bNPCNameLevel == 0;
}
#endif
