#include "StdAfx.h"
#include "PythonNetworkStream.h"
#include "NetworkActorManager.h"
#include "PythonBackground.h"

#include "PythonApplication.h"
#include "AbstractPlayer.h"
#include "../GameLib/ActorInstance.h"

#include "InstanceBase.h"
#if defined(ENABLE_WEAPON_COSTUME_SYSTEM)
#	include "../GameLib/ItemData.h"
#endif

void CPythonNetworkStream::__GlobalPositionToLocalPosition(LONG& rGlobalX, LONG& rGlobalY)
{
	CPythonBackground& rkBgMgr = CPythonBackground::Instance();
	rkBgMgr.GlobalPositionToLocalPosition(rGlobalX, rGlobalY);
}

void CPythonNetworkStream::__LocalPositionToGlobalPosition(LONG& rLocalX, LONG& rLocalY)
{
	CPythonBackground& rkBgMgr = CPythonBackground::Instance();
	rkBgMgr.LocalPositionToGlobalPosition(rLocalX, rLocalY);
}

bool CPythonNetworkStream::__CanActMainInstance()
{
	CPythonCharacterManager& rkChrMgr = CPythonCharacterManager::Instance();
	CInstanceBase* pkInstMain = rkChrMgr.GetMainInstancePtr();
	if (!pkInstMain)
		return false;

	return pkInstMain->CanAct();
}

void CPythonNetworkStream::__ClearNetworkActorManager()
{
	m_rokNetActorMgr->Destroy();
}

void __SetWeaponPower(IAbstractPlayer& rkPlayer, DWORD dwWeaponID)
{
	DWORD dwMinPower = 0;
	DWORD dwMaxPower = 0;
	DWORD dwMinMagicPower = 0;
	DWORD dwMaxMagicPower = 0;
	DWORD dwAddPower = 0;

	/*
	* Note: Due to the implementation of the transmutation system, the dwWeaponID can vary when a weapon is transmuted.
	* To address this, the function now checks the equipped weapon by slot, ensuring that the correct weapon power is
	* determined regardless of transmutation.
	*/

	const TItemData* pItemData = CPythonPlayer::Instance().GetItemData(TItemPos(EQUIPMENT, c_Equipment_Slot_Weapon));
	if (pItemData && dwWeaponID)
	{
		CItemData* pkWeapon = nullptr;
		if (CItemManager::Instance().GetItemDataPointer(pItemData->dwVnum, &pkWeapon))
		{
			if (pkWeapon->GetType() == CItemData::ITEM_TYPE_WEAPON)
			{
#if defined(ENABLE_APPLY_RANDOM) && defined(ENABLE_ITEM_VALUE10)
				const DWORD dwRandomMagicPower = pItemData->alSockets[CItemData::ITEM_SOCKET_MTK_MINMAX_RANDOM];
				if (dwRandomMagicPower)
				{
					dwMinMagicPower = dwRandomMagicPower / CItemData::ITEM_VALUE_MINMAX_RANDOM_DIVISION_VALUE;
					dwMaxMagicPower = dwRandomMagicPower % (CItemData::ITEM_VALUE_MINMAX_RANDOM_DIVISION_VALUE / 100);
				}
				else
				{
					dwMinMagicPower = pkWeapon->GetValue(1);
					dwMaxMagicPower = pkWeapon->GetValue(2);
				}

				const DWORD dwRandomPower = pItemData->alSockets[CItemData::ITEM_SOCKET_ATK_MINMAX_RANDOM];
				if (dwRandomPower)
				{
					dwMinPower = dwRandomPower / CItemData::ITEM_VALUE_MINMAX_RANDOM_DIVISION_VALUE;
					dwMaxPower = dwRandomPower % (CItemData::ITEM_VALUE_MINMAX_RANDOM_DIVISION_VALUE / 100);
				}
				else
				{
					dwMinPower = pkWeapon->GetValue(3);
					dwMaxPower = pkWeapon->GetValue(4);
				}

				dwAddPower = pkWeapon->GetValue(5);
#else
				dwMinPower = pkWeapon->GetValue(3);
				dwMaxPower = pkWeapon->GetValue(4);
				dwMinMagicPower = pkWeapon->GetValue(1);
				dwMaxMagicPower = pkWeapon->GetValue(2);
				dwAddPower = pkWeapon->GetValue(5);
#endif
			}
		}
	}

	rkPlayer.SetWeaponPower(dwMinPower, dwMaxPower, dwMinMagicPower, dwMaxMagicPower, dwAddPower);
}

#if defined(ENABLE_ACCE_COSTUME_SYSTEM)
void __SetAccePower(IAbstractPlayer& rkPlayer, DWORD dwAcceID)
{
	if (!dwAcceID)
	{
		rkPlayer.SetAccePower(0, 0, 0, 0);
		return;
	}

	DWORD dwMinPower = 0;
	DWORD dwMaxPower = 0;
	DWORD dwMinMagicPower = 0;
	DWORD dwMaxMagicPower = 0;
	float fDrainPower = 0.0f;

	CItemData* pkAcce = nullptr;
	if (!CItemManager::Instance().GetItemDataPointer(dwAcceID, &pkAcce))
	{
		rkPlayer.SetAccePower(0, 0, 0, 0);
		return;
	}

	const TItemData* pItemData = CPythonPlayer::Instance().GetItemData(TItemPos(EQUIPMENT, c_Costume_Slot_Acce));
	if (!pItemData)
	{
		rkPlayer.SetAccePower(0, 0, 0, 0);
		return;
	}

	if (pkAcce->GetType() == CItemData::ITEM_TYPE_COSTUME && pkAcce->GetSubType() == CItemData::COSTUME_ACCE)
	{
		long lApplyValue = pkAcce->FindApplyValue(CItemData::APPLY_ACCEDRAIN_RATE);
		long lSocketInDrainValue = 0;

		if (pkAcce->GetRefineVNum() == 0)
		{
			lSocketInDrainValue = pItemData->alSockets[ITEM_SOCKET_ACCE_DRAIN_VALUE];
			fDrainPower = lSocketInDrainValue / 100.0;
		}
		else
		{
			lSocketInDrainValue = lApplyValue;
			fDrainPower = lApplyValue / 100.0;
		}

		CItemData* pDrainedItem;
		if (CItemManager::Instance().GetItemDataPointer(pItemData->alSockets[ITEM_SOCKET_ACCE_DRAIN_ITEM_VNUM], &pDrainedItem))
		{
			if (pDrainedItem->GetType() == CItemData::ITEM_TYPE_WEAPON)
			{
				const DWORD dwAddPower = pDrainedItem->GetValue(5);

#if defined(ENABLE_APPLY_RANDOM) && defined(ENABLE_ITEM_VALUE10)
				const DWORD dwRandomMagicPower = pItemData->alSockets[CItemData::ITEM_SOCKET_MTK_MINMAX_RANDOM];
				if (dwRandomMagicPower)
				{
					const DWORD dwRandomMinMagicPower = dwRandomMagicPower / CItemData::ITEM_VALUE_MINMAX_RANDOM_DIVISION_VALUE;
					const DWORD dwRandomMaxMagicPower = dwRandomMagicPower % (CItemData::ITEM_VALUE_MINMAX_RANDOM_DIVISION_VALUE / 100);

					if (dwMinMagicPower && dwMaxMagicPower)
					{
						dwMinMagicPower = MAX(2 * (dwRandomMinMagicPower + dwAddPower) * fDrainPower, 1);
						dwMaxMagicPower = MAX(2 * (dwRandomMaxMagicPower + dwAddPower) * fDrainPower, 1);
					}
				}
				else
				{
					if (pDrainedItem->GetValue(1) && pDrainedItem->GetValue(2))
					{
						dwMinMagicPower = MAX(2 * (pDrainedItem->GetValue(1) + dwAddPower) * fDrainPower, 1);
						dwMaxMagicPower = MAX(2 * (pDrainedItem->GetValue(2) + dwAddPower) * fDrainPower, 1);
					}
				}
#else
				if (pDrainedItem->GetValue(1) && pDrainedItem->GetValue(2))
				{
					dwMinMagicPower = MAX(2 * (pDrainedItem->GetValue(1) + dwAddPower) * fDrainPower, 1);
					dwMaxMagicPower = MAX(2 * (pDrainedItem->GetValue(2) + dwAddPower) * fDrainPower, 1);
				}
#endif

#if defined(ENABLE_APPLY_RANDOM) && defined(ENABLE_ITEM_VALUE10)
				const DWORD dwRandomPower = pItemData->alSockets[CItemData::ITEM_SOCKET_ATK_MINMAX_RANDOM];
				if (dwRandomPower)
				{
					const DWORD dwMinRandomPower = dwRandomPower / CItemData::ITEM_VALUE_MINMAX_RANDOM_DIVISION_VALUE;
					const DWORD dwMaxRandomPower = dwRandomPower % (CItemData::ITEM_VALUE_MINMAX_RANDOM_DIVISION_VALUE / 100);

					if (dwMinRandomPower && dwMaxRandomPower)
					{
						dwMinPower = MAX(2 * (dwMinRandomPower + dwAddPower) * fDrainPower, 1);
						dwMaxPower = MAX(2 * (dwMaxRandomPower + dwAddPower) * fDrainPower, 1);
					}
				}
				else
				{
					if (pDrainedItem->GetValue(3) && pDrainedItem->GetValue(4))
					{
						dwMinPower = MAX(2 * (pDrainedItem->GetValue(3) + dwAddPower) * fDrainPower, 1);
						dwMaxPower = MAX(2 * (pDrainedItem->GetValue(4) + dwAddPower) * fDrainPower, 1);
					}
				}
#else
				if (pDrainedItem->GetValue(3) && pDrainedItem->GetValue(4))
				{
					dwMinPower = MAX(2 * (pDrainedItem->GetValue(3) + dwAddPower) * fDrainPower, 1);
					dwMaxPower = MAX(2 * (pDrainedItem->GetValue(4) + dwAddPower) * fDrainPower, 1);
				}
#endif
			}
		}
	}

	rkPlayer.SetAccePower(dwMinPower, dwMaxPower, dwMinMagicPower, dwMaxMagicPower);
}
#endif

//테이블에서 이름이 "." 인 것들
//차후에 서버에서 보내주지 않게 되면 없어질 함수..(서버님께 꼭!!협박; )
bool IsInvisibleRace(WORD raceNum)
{
	switch (raceNum)
	{
	case 20025:
	case 20038:
	case 20039:
		return true;
	default:
		return false;
	}
}

// 20200629.Owsap : NPC Types
bool IsNPCType(BYTE bType)
{
	switch (bType)
	{
	case CActorInstance::TYPE_NPC:
	case CActorInstance::TYPE_HORSE:
	case CActorInstance::TYPE_PET:
	case CActorInstance::TYPE_PET_PAY:
		return true;
	default:
		return false;
	}
}

static SNetworkActorData s_kNetActorData;

bool CPythonNetworkStream::RecvCharacterAppendPacket()
{
	TPacketGCCharacterAdd chrAddPacket;
	if (!Recv(sizeof(chrAddPacket), &chrAddPacket))
		return false;

	__GlobalPositionToLocalPosition(chrAddPacket.x, chrAddPacket.y);

	SNetworkActorData kNetActorData;
	kNetActorData.m_bType = chrAddPacket.bType;
	kNetActorData.m_dwMovSpd = chrAddPacket.bMovingSpeed;
	kNetActorData.m_dwAtkSpd = chrAddPacket.bAttackSpeed;
	kNetActorData.m_dwRace = chrAddPacket.wRaceNum;
#if defined(ENABLE_RACE_SWAP)
	kNetActorData.m_dwEventRace = chrAddPacket.dwEventRaceNum;
#endif

	kNetActorData.m_dwStateFlags = chrAddPacket.bStateFlag;
	kNetActorData.m_dwVID = chrAddPacket.dwVID;
	kNetActorData.m_fRot = chrAddPacket.angle;

	kNetActorData.m_stName = "";

	kNetActorData.m_kAffectFlags.CopyData(0, sizeof(chrAddPacket.dwAffectFlag[0]), &chrAddPacket.dwAffectFlag[0]);
	kNetActorData.m_kAffectFlags.CopyData(32, sizeof(chrAddPacket.dwAffectFlag[1]), &chrAddPacket.dwAffectFlag[1]);
	kNetActorData.m_kAffectFlags.CopyData(64, sizeof(chrAddPacket.dwAffectFlag[2]), &chrAddPacket.dwAffectFlag[2]);

	kNetActorData.SetPosition(chrAddPacket.x, chrAddPacket.y);

	kNetActorData.m_sAlignment = 0; /*chrAddPacket.sAlignment*/;
	kNetActorData.m_byPKMode = 0; /*chrAddPacket.bPKMode*/;
	kNetActorData.m_dwGuildID = 0; /*chrAddPacket.dwGuild*/;
	kNetActorData.m_dwEmpireID = 0; /*chrAddPacket.bEmpire*/;
	kNetActorData.m_dwArmor = 0; /*chrAddPacket.awPart[CHR_EQUIPPART_ARMOR]*/;
	kNetActorData.m_dwWeapon = 0; /*chrAddPacket.awPart[CHR_EQUIPPART_WEAPON]*/;
	kNetActorData.m_dwHair = 0; /*chrAddPacket.awPart[CHR_EQUIPPART_HAIR]*/;
#if defined(ENABLE_ACCE_COSTUME_SYSTEM)
	kNetActorData.m_dwAcce = 0; /*chrAddPacket.awPart[CHR_EQUIPPART_ACCE]*/;
#endif
#if defined(ENABLE_AURA_COSTUME_SYSTEM)
	kNetActorData.m_dwAura = 0; /*chrAddPacket.awPart[CHR_EQUIPPART_AURA]*/
#endif
	kNetActorData.m_dwMountVnum = 0; /*chrAddPacket.dwMountVnum*/;
#if defined(ENABLE_QUIVER_SYSTEM)
	kNetActorData.m_dwArrow = 0; /*chrAddPacket.dwArrow*/;
#endif
#if defined(ENABLE_REFINE_ELEMENT_SYSTEM)
	kNetActorData.m_wRefineElementAffectType = 0;
#endif
#if defined(ENABLE_GUILD_LEADER_GRADE_NAME)
	kNetActorData.m_bGuildLeaderGrade = 0;
#endif

#ifdef WJ_SHOW_MOB_INFO
	kNetActorData.m_dwLevel = chrAddPacket.dwLevel;
	kNetActorData.m_dwAIFlag = chrAddPacket.dwAIFlag;
#else
	kNetActorData.m_dwLevel = 0; // 몬스터 레벨 표시 안함
#endif
#if defined(ENABLE_CONQUEROR_LEVEL)
	kNetActorData.m_dwConquerorLevel = 0;
#endif

	if (kNetActorData.m_bType != CActorInstance::TYPE_PC && !IsNPCType(kNetActorData.m_bType))
	{
		const char* c_szName;
		CPythonNonPlayer& rkNonPlayer = CPythonNonPlayer::Instance();
		if (rkNonPlayer.GetName(kNetActorData.m_dwRace, &c_szName))
			kNetActorData.m_stName = c_szName;
		//else
			//kNetActorData.m_stName=chrAddPacket.name;

		__RecvCharacterAppendPacket(&kNetActorData);
	}
	else
	{
		s_kNetActorData = kNetActorData;
	}

	return true;
}

bool CPythonNetworkStream::RecvCharacterAdditionalInfo()
{
	TPacketGCCharacterAdditionalInfo chrInfoPacket;
	if (!Recv(sizeof(chrInfoPacket), &chrInfoPacket))
		return false;

	SNetworkActorData kNetActorData = s_kNetActorData;
	if (IsInvisibleRace(kNetActorData.m_dwRace))
		return true;

	if (kNetActorData.m_dwVID == chrInfoPacket.dwVID)
	{
#if defined(ENABLE_LOCALE_CLIENT)
		const char* c_szName;
		if (CPythonNonPlayer::Instance().GetName(kNetActorData.m_dwRace, &c_szName))
		{
			switch (kNetActorData.m_bType)
			{
				case CActorInstance::TYPE_NPC:
					kNetActorData.m_stName = c_szName;
					break;

				case CActorInstance::TYPE_HORSE:
					CPythonLocale::Instance().FormatString(chrInfoPacket.name, sizeof(chrInfoPacket.name));
					kNetActorData.m_stName = chrInfoPacket.name;
					break;

				case CActorInstance::TYPE_PET_PAY:
				{
					char szPetName[1024];
					sprintf(szPetName, "%s - %s", chrInfoPacket.name, c_szName);
					kNetActorData.m_stName = szPetName;
				}
				break;

				default:
					kNetActorData.m_stName = chrInfoPacket.name;
					break;
			}
		}
		else
			kNetActorData.m_stName = chrInfoPacket.name;
#else
		kNetActorData.m_stName = chrInfoPacket.name;
#endif
		kNetActorData.m_dwGuildID = chrInfoPacket.dwGuildID;
		kNetActorData.m_dwLevel = chrInfoPacket.dwLevel;
#if defined(ENABLE_CONQUEROR_LEVEL)
		kNetActorData.m_dwConquerorLevel = chrInfoPacket.dwConquerorLevel;
#endif
		kNetActorData.m_sAlignment = chrInfoPacket.sAlignment;
		kNetActorData.m_byPKMode = chrInfoPacket.bPKMode;
		kNetActorData.m_dwGuildID = chrInfoPacket.dwGuildID;
		kNetActorData.m_dwEmpireID = chrInfoPacket.bEmpire;
		kNetActorData.m_dwArmor = chrInfoPacket.adwPart[CHR_EQUIPPART_ARMOR];
		kNetActorData.m_dwWeapon = chrInfoPacket.adwPart[CHR_EQUIPPART_WEAPON];
		kNetActorData.m_dwHair = chrInfoPacket.adwPart[CHR_EQUIPPART_HAIR];
#if defined(ENABLE_ACCE_COSTUME_SYSTEM)
		kNetActorData.m_dwAcce = chrInfoPacket.adwPart[CHR_EQUIPPART_ACCE];
#endif
#if defined(ENABLE_AURA_COSTUME_SYSTEM)
		kNetActorData.m_dwAura = chrInfoPacket.adwPart[CHR_EQUIPPART_AURA];
#endif
#if defined(ENABLE_QUIVER_SYSTEM)
		kNetActorData.m_dwArrow = chrInfoPacket.dwArrow;
#endif
#if defined(ENABLE_REFINE_ELEMENT_SYSTEM)
		kNetActorData.m_wRefineElementAffectType = chrInfoPacket.wRefineElementAffectType;
#endif
		kNetActorData.m_dwMountVnum = chrInfoPacket.dwMountVnum;
#if defined(ENABLE_GUILD_LEADER_GRADE_NAME)
		kNetActorData.m_bGuildLeaderGrade = chrInfoPacket.bGuildLeaderGrade;
#endif
#if defined(ENABLE_MULTI_LANGUAGE_SYSTEM)
		kNetActorData.m_stCountry = chrInfoPacket.szCountry;
#endif

		__RecvCharacterAppendPacket(&kNetActorData);
	}
	else
	{
		TraceError("TPacketGCCharacterAdditionalInfo name=%s vid=%d race=%d Error", chrInfoPacket.name, chrInfoPacket.dwVID, kNetActorData.m_dwRace);
	}
	return true;
}

bool CPythonNetworkStream::RecvCharacterUpdatePacket()
{
	TPacketGCCharacterUpdate chrUpdatePacket;
	if (!Recv(sizeof(chrUpdatePacket), &chrUpdatePacket))
		return false;

	SNetworkUpdateActorData kNetUpdateActorData;
	kNetUpdateActorData.m_dwGuildID = chrUpdatePacket.dwGuildID;
	kNetUpdateActorData.m_dwMovSpd = chrUpdatePacket.bMovingSpeed;
	kNetUpdateActorData.m_dwAtkSpd = chrUpdatePacket.bAttackSpeed;
	kNetUpdateActorData.m_dwArmor = chrUpdatePacket.adwPart[CHR_EQUIPPART_ARMOR];
	kNetUpdateActorData.m_dwWeapon = chrUpdatePacket.adwPart[CHR_EQUIPPART_WEAPON];
	kNetUpdateActorData.m_dwHair = chrUpdatePacket.adwPart[CHR_EQUIPPART_HAIR];
#if defined(ENABLE_ACCE_COSTUME_SYSTEM)
	kNetUpdateActorData.m_dwAcce = chrUpdatePacket.adwPart[CHR_EQUIPPART_ACCE];
#endif
#if defined(ENABLE_AURA_COSTUME_SYSTEM)
	kNetUpdateActorData.m_dwAura = chrUpdatePacket.adwPart[CHR_EQUIPPART_AURA];
#endif
#if defined(ENABLE_QUIVER_SYSTEM)
	kNetUpdateActorData.m_dwArrow = chrUpdatePacket.dwArrow;
#endif
#if defined(ENABLE_REFINE_ELEMENT_SYSTEM)
	kNetUpdateActorData.m_wRefineElementAffectType = chrUpdatePacket.wRefineElementAffectType;
#endif
	kNetUpdateActorData.m_dwVID = chrUpdatePacket.dwVID;
	kNetUpdateActorData.m_kAffectFlags.CopyData(0, sizeof(chrUpdatePacket.dwAffectFlag[0]), &chrUpdatePacket.dwAffectFlag[0]);
	kNetUpdateActorData.m_kAffectFlags.CopyData(32, sizeof(chrUpdatePacket.dwAffectFlag[1]), &chrUpdatePacket.dwAffectFlag[1]);
	kNetUpdateActorData.m_kAffectFlags.CopyData(64, sizeof(chrUpdatePacket.dwAffectFlag[2]), &chrUpdatePacket.dwAffectFlag[2]);
	kNetUpdateActorData.m_sAlignment = chrUpdatePacket.sAlignment;
	kNetUpdateActorData.m_byPKMode = chrUpdatePacket.bPKMode;
	kNetUpdateActorData.m_dwStateFlags = chrUpdatePacket.bStateFlag;
	kNetUpdateActorData.m_dwMountVnum = chrUpdatePacket.dwMountVnum;
#if defined(ENABLE_GUILD_LEADER_GRADE_NAME)
	kNetUpdateActorData.m_bGuildLeaderGrade = chrUpdatePacket.bGuildLeaderGrade;
#endif
	kNetUpdateActorData.m_dwLevel = chrUpdatePacket.dwLevel;
#if defined(ENABLE_CONQUEROR_LEVEL)
	kNetUpdateActorData.m_dwConquerorLevel = chrUpdatePacket.dwConquerorLevel;
#endif
#if defined(ENABLE_LEFT_SEAT)
	kNetUpdateActorData.m_bLeftSeat = chrUpdatePacket.bLeftSeat;
#endif

	__RecvCharacterUpdatePacket(&kNetUpdateActorData);

	return true;
}

void CPythonNetworkStream::__RecvCharacterAppendPacket(SNetworkActorData* pkNetActorData)
{
	// NOTE : 카메라가 땅에 묻히는 문제의 해결을 위해 메인 캐릭터가 지형에 올려지기
	// 전에 맵을 업데이트 해 높이를 구할 수 있도록 해놓아야 합니다.
	// 단, 게임이 들어갈때가 아닌 이미 캐릭터가 추가 된 이후에만 합니다.
	// 헌데 이동인데 왜 Move로 안하고 Append로 하는지..? - [levites]
	IAbstractPlayer& rkPlayer = IAbstractPlayer::GetSingleton();
	if (rkPlayer.IsMainCharacterIndex(pkNetActorData->m_dwVID))
	{
		rkPlayer.SetRace(pkNetActorData->m_dwRace);

		__SetWeaponPower(rkPlayer, pkNetActorData->m_dwWeapon);
#if defined(ENABLE_ACCE_COSTUME_SYSTEM)
		__SetAccePower(rkPlayer, pkNetActorData->m_dwAcce);
#endif

		if (rkPlayer.NEW_GetMainActorPtr())
		{
			CPythonBackground::Instance().Update(pkNetActorData->m_lCurX, pkNetActorData->m_lCurY, 0.0f);
			CPythonCharacterManager::Instance().Update();

			// NOTE : 사귀 타워일 경우 GOTO 로 이동시에도 맵 이름을 출력하도록 처리
			{
				std::string strMapName = CPythonBackground::Instance().GetWarpMapName();
				if (strMapName == "metin2_map_deviltower1")
					__ShowMapName(pkNetActorData->m_lCurX, pkNetActorData->m_lCurY);
			}
		}
		else
		{
			__ShowMapName(pkNetActorData->m_lCurX, pkNetActorData->m_lCurY);
#if defined(ENABLE_CONQUEROR_LEVEL)
			__SungMaAttrInfoProcess();
#endif
		}
	}

	m_rokNetActorMgr->AppendActor(*pkNetActorData);

	if (GetMainActorVID() == pkNetActorData->m_dwVID)
	{
		//rkPlayer.SetTarget(0);
		if (m_bComboSkillFlag)
			rkPlayer.SetComboSkillFlag(m_bComboSkillFlag);

		__SetGuildID(pkNetActorData->m_dwGuildID);
		//CPythonApplication::Instance().SkipRenderBuffering(10000);
	}
}

void CPythonNetworkStream::__RecvCharacterUpdatePacket(SNetworkUpdateActorData* pkNetUpdateActorData)
{
	m_rokNetActorMgr->UpdateActor(*pkNetUpdateActorData);

	IAbstractPlayer& rkPlayer = IAbstractPlayer::GetSingleton();
	if (rkPlayer.IsMainCharacterIndex(pkNetUpdateActorData->m_dwVID))
	{
		__SetGuildID(pkNetUpdateActorData->m_dwGuildID);
		__SetWeaponPower(rkPlayer, pkNetUpdateActorData->m_dwWeapon);
#if defined(ENABLE_ACCE_COSTUME_SYSTEM)
		__SetAccePower(rkPlayer, pkNetUpdateActorData->m_dwAcce);
#endif

		__RefreshStatus();
		__RefreshAlignmentWindow();
		__RefreshEquipmentWindow();
		__RefreshInventoryWindow();

#if defined(ENABLE_IMPROVED_LOGOUT_POINTS)
		m_akSimplePlayerInfo[m_dwSelectedCharacterIndex].dwHairPart = pkNetUpdateActorData->m_dwHair;
		m_akSimplePlayerInfo[m_dwSelectedCharacterIndex].dwMainPart = pkNetUpdateActorData->m_dwArmor;
#	if defined(ENABLE_ACCE_COSTUME_SYSTEM)
		m_akSimplePlayerInfo[m_dwSelectedCharacterIndex].dwAccePart = pkNetUpdateActorData->m_dwAcce;
#	endif
#endif
	}
	else
	{
		rkPlayer.NotifyCharacterUpdate(pkNetUpdateActorData->m_dwVID);
	}
}

bool CPythonNetworkStream::RecvCharacterDeletePacket()
{
	TPacketGCCharacterDelete chrDelPacket;

	if (!Recv(sizeof(chrDelPacket), &chrDelPacket))
	{
		TraceError("CPythonNetworkStream::RecvCharacterDeletePacket - Recv Error");
		return false;
	}

	m_rokNetActorMgr->RemoveActor(chrDelPacket.dwVID);

	// 캐릭터가 사라질때 개인 상점도 없애줍니다.
	// Key Check 를 하기때문에 없어도 상관은 없습니다.
	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME],
		"BINARY_PrivateShop_Disappear",
		Py_BuildValue("(i)", chrDelPacket.dwVID)
	);

	return true;
}

bool CPythonNetworkStream::RecvCharacterMovePacket()
{
	TPacketGCMove kMovePacket;
	if (!Recv(sizeof(TPacketGCMove), &kMovePacket))
	{
		Tracen("CPythonNetworkStream::RecvCharacterMovePacket - PACKET READ ERROR");
		return false;
	}

	__GlobalPositionToLocalPosition(kMovePacket.lX, kMovePacket.lY);

	SNetworkMoveActorData kNetMoveActorData;
	kNetMoveActorData.m_dwArg = kMovePacket.bArg;
	kNetMoveActorData.m_dwFunc = kMovePacket.bFunc;
	kNetMoveActorData.m_dwTime = kMovePacket.dwTime;
	kNetMoveActorData.m_dwVID = kMovePacket.dwVID;
	kNetMoveActorData.m_fRot = kMovePacket.bRot * 5.0f;
	kNetMoveActorData.m_lPosX = kMovePacket.lX;
	kNetMoveActorData.m_lPosY = kMovePacket.lY;
	kNetMoveActorData.m_dwDuration = kMovePacket.dwDuration;

	m_rokNetActorMgr->MoveActor(kNetMoveActorData);

	return true;
}

bool CPythonNetworkStream::RecvOwnerShipPacket()
{
	TPacketGCOwnership kPacketOwnership;

	if (!Recv(sizeof(kPacketOwnership), &kPacketOwnership))
		return false;

	m_rokNetActorMgr->SetActorOwner(kPacketOwnership.dwOwnerVID, kPacketOwnership.dwVictimVID);

	return true;
}

bool CPythonNetworkStream::RecvSyncPositionPacket()
{
	TPacketGCSyncPosition kPacketSyncPos;
	if (!Recv(sizeof(kPacketSyncPos), &kPacketSyncPos))
		return false;

	TPacketGCSyncPositionElement kSyncPos;

	UINT uSyncPosCount = (kPacketSyncPos.wSize - sizeof(kPacketSyncPos)) / sizeof(kSyncPos);
	for (UINT iSyncPos = 0; iSyncPos < uSyncPosCount; ++iSyncPos)
	{
		if (!Recv(sizeof(TPacketGCSyncPositionElement), &kSyncPos))
			return false;

#ifdef __MOVIE_MODE__
		return true;
#endif __MOVIE_MODE__

		//Tracenf("CPythonNetworkStream::RecvSyncPositionPacket %d (%d, %d)", kSyncPos.dwVID, kSyncPos.lX, kSyncPos.lY);

		__GlobalPositionToLocalPosition(kSyncPos.lX, kSyncPos.lY);
		m_rokNetActorMgr->SyncActor(kSyncPos.dwVID, kSyncPos.lX, kSyncPos.lY);

		/*
		CPythonCharacterManager & rkChrMgr = CPythonCharacterManager::Instance();
		CInstanceBase * pkChrInst = rkChrMgr.GetInstancePtr(kSyncPos.dwVID);

		if (pkChrInst)
		{
			pkChrInst->NEW_SyncPixelPosition(kSyncPos.lX, kSyncPos.lY);
		}
		*/
	}

	return true;
}

/*
bool CPythonNetworkStream::RecvCharacterAppendPacket()
{
	TPacketGCCharacterAdd chrAddPacket;

	if (!Recv(sizeof(chrAddPacket), &chrAddPacket))
		return false;

	__GlobalPositionToLocalPosition(chrAddPacket.lX, chrAddPacket.lY);

	SNetworkActorData kNetActorData;
	kNetActorData.m_dwGuildID=chrAddPacket.dwGuild;
	kNetActorData.m_dwEmpireID=chrAddPacket.bEmpire;
	kNetActorData.m_dwMovSpd=chrAddPacket.bMovingSpeed;
	kNetActorData.m_dwAtkSpd=chrAddPacket.bAttackSpeed;
	kNetActorData.m_dwRace=chrAddPacket.wRaceNum;
	kNetActorData.m_dwShape=chrAddPacket.parts[CRaceData::PART_MAIN];
	kNetActorData.m_dwStateFlags=chrAddPacket.bStateFlag;
	kNetActorData.m_dwVID=chrAddPacket.dwVID;
	kNetActorData.m_dwWeapon=chrAddPacket.parts[CRaceData::PART_WEAPON];
	kNetActorData.m_fRot=chrAddPacket.angle;
	kNetActorData.m_kAffectFlags.CopyData(0, sizeof(chrAddPacket.dwAffectFlag[0]), &chrAddPacket.dwAffectFlag[0]);
	kNetActorData.m_kAffectFlags.CopyData(32, sizeof(chrAddPacket.dwAffectFlag[1]), &chrAddPacket.dwAffectFlag[1]);
	kNetActorData.m_kAffectFlags.CopyData(64, sizeof(chrAddPacket.dwAffectFlag[2]), &chrAddPacket.dwAffectFlag[2]);
	kNetActorData.SetPosition(chrAddPacket.lX, chrAddPacket.lY);
	kNetActorData.m_stName=chrAddPacket.name;
	__RecvCharacterAppendPacket(&kNetActorData);
	return true;
}
*/
