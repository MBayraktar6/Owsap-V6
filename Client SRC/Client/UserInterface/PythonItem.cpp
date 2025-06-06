#include "stdafx.h"
#include "../EterLib/GrpMath.h"
#include "../GameLib/ItemManager.h"
#include "../EffectLib/EffectManager.h"
#include "PythonBackground.h"

#include "PythonItem.h"
#include "PythonTextTail.h"

#if defined(ENABLE_ACCE_COSTUME_SYSTEM) || defined(ENABLE_CHANGE_LOOK_SYSTEM)
#	include "PythonPlayer.h"
#endif
#if defined(ENABLE_ITEM_DROP_RENEWAL)
#	include "PythonSkill.h"
#	include "PythonNonPlayer.h"
#endif

const float c_fDropStartHeight = 100.0f;
const float c_fDropTime = 0.5f;

std::string CPythonItem::TGroundItemInstance::ms_astDropSoundFileName[DROPSOUND_NUM];

void CPythonItem::GetInfo(std::string* pstInfo)
{
	char szInfo[256];
	sprintf(szInfo, "Item: Inst %d, Pool %d", m_GroundItemInstanceMap.size(), m_GroundItemInstancePool.GetCapacity());

	pstInfo->append(szInfo);
}

void CPythonItem::TGroundItemInstance::Clear()
{
	stOwnership = "";
	ThingInstance.Clear();
	CEffectManager::Instance().DestroyEffectInstance(dwEffectInstanceIndex);
}

void CPythonItem::TGroundItemInstance::__PlayDropSound(DWORD eItemType, const D3DXVECTOR3& c_rv3Pos)
{
	if (eItemType >= DROPSOUND_NUM)
		return;

	CSoundManager::Instance().PlaySound3D(c_rv3Pos.x, c_rv3Pos.y, c_rv3Pos.z, ms_astDropSoundFileName[eItemType].c_str());
}

bool CPythonItem::TGroundItemInstance::Update()
{
	if (bAnimEnded)
		return false;
	if (dwEndTime < CTimer::Instance().GetCurrentMillisecond())
	{
		ThingInstance.SetRotationQuaternion(qEnd);

		/*
		D3DXVECTOR3 v3Adjust = -v3Center;
		D3DXMATRIX mat;
		D3DXMatrixRotationYawPitchRoll(&mat,
			D3DXToRadian(rEnd.y),
			D3DXToRadian(rEnd.x),
			D3DXToRadian(rEnd.z));
		D3DXVec3TransformCoord(&v3Adjust, &v3Adjust, &mat);
		*/

		D3DXQUATERNION qAdjust(-v3Center.x, -v3Center.y, -v3Center.z, 0.0f);
		D3DXQUATERNION qc;
		D3DXQuaternionConjugate(&qc, &qEnd);
		D3DXQuaternionMultiply(&qAdjust, &qAdjust, &qEnd);
		D3DXQuaternionMultiply(&qAdjust, &qc, &qAdjust);

		ThingInstance.SetPosition(v3EndPosition.x + qAdjust.x,
			v3EndPosition.y + qAdjust.y,
			v3EndPosition.z + qAdjust.z);
		//ThingInstance.Update();
		bAnimEnded = true;

		__PlayDropSound(eDropSoundType, v3EndPosition);
	}
	else
	{
		DWORD time = CTimer::Instance().GetCurrentMillisecond() - dwStartTime;
		DWORD etime = dwEndTime - CTimer::Instance().GetCurrentMillisecond();
		float rate = time * 1.0f / (dwEndTime - dwStartTime);

		D3DXVECTOR3 v3NewPosition = v3EndPosition; //= rate * (v3EndPosition - v3StartPosition) + v3StartPosition;
		v3NewPosition.z += 100 - 100 * rate * (3 * rate - 2); //-100 * (rate - 1) * (3 * rate + 2);

		D3DXQUATERNION q;
		D3DXQuaternionRotationAxis(&q, &v3RotationAxis, etime * 0.03f * (-1 + rate * (3 * rate - 2)));
		//ThingInstance.SetRotation(rEnd.y + etime * rStart.y, rEnd.x + etime * rStart.x, rEnd.z + etime * rStart.z);
		D3DXQuaternionMultiply(&q, &qEnd, &q);

		ThingInstance.SetRotationQuaternion(q);
		D3DXQUATERNION qAdjust(-v3Center.x, -v3Center.y, -v3Center.z, 0.0f);
		D3DXQUATERNION qc;
		D3DXQuaternionConjugate(&qc, &q);
		D3DXQuaternionMultiply(&qAdjust, &qAdjust, &q);
		D3DXQuaternionMultiply(&qAdjust, &qc, &qAdjust);

		ThingInstance.SetPosition(v3NewPosition.x + qAdjust.x,
			v3NewPosition.y + qAdjust.y,
			v3NewPosition.z + qAdjust.z);

		/*
		D3DXVECTOR3 v3Adjust = -v3Center;
		D3DXMATRIX mat;
		D3DXMatrixRotationYawPitchRoll(&mat,
			D3DXToRadian(rEnd.y + etime * rStart.y),
			D3DXToRadian(rEnd.x + etime * rStart.x),
			D3DXToRadian(rEnd.z + etime * rStart.z));

		D3DXVec3TransformCoord(&v3Adjust, &v3Adjust, &mat);
		//Tracef("%f %f %f\n", v3Adjust.x, v3Adjust.y, v3Adjust.z);
		v3NewPosition += v3Adjust;
		ThingInstance.SetPosition(v3NewPosition.x, v3NewPosition.y, v3NewPosition.z);
		*/
	}
	ThingInstance.Transform();
	ThingInstance.Deform();
	return !bAnimEnded;
}

void CPythonItem::Update(const POINT& c_rkPtMouse)
{
	TGroundItemInstanceMap::iterator itor = m_GroundItemInstanceMap.begin();
	for (; itor != m_GroundItemInstanceMap.end(); ++itor)
	{
		itor->second->Update();
	}

	m_dwPickedItemID = __Pick(c_rkPtMouse);
}

void CPythonItem::Render()
{
#if defined(ENABLE_GRAPHIC_ON_OFF)
	if (!CPythonGraphicOnOff::Instance().CanRenderDropItem(3))
		return;
#endif

	CPythonGraphic::Instance().SetDiffuseOperation();
	TGroundItemInstanceMap::iterator itor = m_GroundItemInstanceMap.begin();
	for (; itor != m_GroundItemInstanceMap.end(); ++itor)
	{
		CGraphicThingInstance& rInstance = itor->second->ThingInstance;
		//rInstance.Update();
		rInstance.Render();
		rInstance.BlendRender();
	}
}

void CPythonItem::SetUseSoundFileName(DWORD eItemType, const std::string& c_rstFileName)
{
	if (eItemType >= USESOUND_NUM)
		return;

	//Tracenf("SetUseSoundFile %d : %s", eItemType, c_rstFileName.c_str());

	m_astUseSoundFileName[eItemType] = c_rstFileName;
}

void CPythonItem::SetDropSoundFileName(DWORD eItemType, const std::string& c_rstFileName)
{
	if (eItemType >= DROPSOUND_NUM)
		return;

	Tracenf("SetDropSoundFile %d : %s", eItemType, c_rstFileName.c_str());

	SGroundItemInstance::ms_astDropSoundFileName[eItemType] = c_rstFileName;
}

void CPythonItem::PlayUseSound(DWORD dwItemID)
{
	//CItemManager& rkItemMgr=CItemManager::Instance();

	CItemData* pkItemData;
	if (!CItemManager::Instance().GetItemDataPointer(dwItemID, &pkItemData))
		return;

	DWORD eItemType = __GetUseSoundType(*pkItemData);
	if (eItemType == USESOUND_NONE)
		return;
	if (eItemType >= USESOUND_NUM)
		return;

	CSoundManager::Instance().PlaySound2D(m_astUseSoundFileName[eItemType].c_str());
}

void CPythonItem::PlayDropSound(DWORD dwItemID)
{
	//CItemManager& rkItemMgr=CItemManager::Instance();

	CItemData* pkItemData;
	if (!CItemManager::Instance().GetItemDataPointer(dwItemID, &pkItemData))
		return;

	DWORD eItemType = __GetDropSoundType(*pkItemData);
	if (eItemType >= DROPSOUND_NUM)
		return;

	CSoundManager::Instance().PlaySound2D(SGroundItemInstance::ms_astDropSoundFileName[eItemType].c_str());
}

void CPythonItem::PlayUsePotionSound()
{
	CSoundManager::Instance().PlaySound2D(m_astUseSoundFileName[USESOUND_POTION].c_str());
}

DWORD CPythonItem::__GetDropSoundType(const CItemData& c_rkItemData)
{
	switch (c_rkItemData.GetType())
	{
	case CItemData::ITEM_TYPE_WEAPON:
		switch (c_rkItemData.GetWeaponType())
		{
		case CItemData::WEAPON_BOW:
			return DROPSOUND_BOW;
			break;
		case CItemData::WEAPON_ARROW:
#if defined(ENABLE_QUIVER_SYSTEM)
		case CItemData::WEAPON_QUIVER:
#endif
			return DROPSOUND_DEFAULT;
			break;
		default:
			return DROPSOUND_WEAPON;
			break;
		}
		break;
	case CItemData::ITEM_TYPE_ARMOR:
		switch (c_rkItemData.GetSubType())
		{
		case CItemData::ARMOR_NECK:
		case CItemData::ARMOR_EAR:
			return DROPSOUND_ACCESSORY;
			break;
		case CItemData::ARMOR_BODY:
			return DROPSOUND_ARMOR;
		default:
			return DROPSOUND_DEFAULT;
			break;
		}
		break;
	default:
		return DROPSOUND_DEFAULT;
		break;
	}

	return DROPSOUND_DEFAULT;
}

DWORD CPythonItem::__GetUseSoundType(const CItemData& c_rkItemData)
{
	switch (c_rkItemData.GetType())
	{
	case CItemData::ITEM_TYPE_WEAPON:
		switch (c_rkItemData.GetWeaponType())
		{
		case CItemData::WEAPON_BOW:
			return USESOUND_BOW;
			break;
		case CItemData::WEAPON_ARROW:
#if defined(ENABLE_QUIVER_SYSTEM)
		case CItemData::WEAPON_QUIVER:
#endif
			return USESOUND_DEFAULT;
			break;
		default:
			return USESOUND_WEAPON;
			break;
		}
		break;

	case CItemData::ITEM_TYPE_ARMOR:
		switch (c_rkItemData.GetSubType())
		{
		case CItemData::ARMOR_NECK:
		case CItemData::ARMOR_EAR:
			return USESOUND_ACCESSORY;
			break;
		case CItemData::ARMOR_BODY:
			return USESOUND_ARMOR;
		default:
			return USESOUND_DEFAULT;
			break;
		}
		break;

	case CItemData::ITEM_TYPE_USE:
		switch (c_rkItemData.GetSubType())
		{
		case CItemData::USE_ABILITY_UP:
			if ((c_rkItemData.GetIndex() == 27053 || c_rkItemData.GetIndex() == 27054) ||
				(c_rkItemData.GetIndex() >= 27100 && c_rkItemData.GetIndex() <= 27105) ||
				(c_rkItemData.GetIndex() >= 27110 && c_rkItemData.GetIndex() <= 27115))
				return USESOUND_NONE;
			else
				return USESOUND_POTION;
			break;
		case CItemData::USE_POTION:
			return USESOUND_NONE;
			break;
		case CItemData::USE_TALISMAN:
			return USESOUND_PORTAL;
			break;
		default:
			return USESOUND_DEFAULT;
			break;
		}
		break;

	default:
		return USESOUND_DEFAULT;
		break;
	}

	return USESOUND_DEFAULT;
}

void CPythonItem::CreateItem(DWORD dwVirtualID, DWORD dwVirtualNumber
#if defined(ENABLE_SET_ITEM)
	, BYTE bSetValue
#endif
	, float x, float y, float z, bool bDrop
#if defined(ENABLE_ITEM_DROP_RENEWAL)
	, long alSockets[ITEM_SOCKET_SLOT_MAX_NUM]
	, TPlayerItemAttribute aAttrs[ITEM_ATTRIBUTE_SLOT_MAX_NUM]
#endif
)
{
	//CItemManager& rkItemMgr = CItemManager::Instance();

	CItemData* pItemData;
	if (!CItemManager::Instance().GetItemDataPointer(dwVirtualNumber, &pItemData))
		return;

	CGraphicThing* pItemModel = pItemData->GetDropModelThing();

	TGroundItemInstance* pGroundItemInstance = m_GroundItemInstancePool.Alloc();
	pGroundItemInstance->dwVirtualNumber = dwVirtualNumber;

	bool bStabGround = false;

	if (bDrop)
	{
		z = CPythonBackground::Instance().GetHeight(x, y) + 10.0f;

		if (pItemData->GetType() == CItemData::ITEM_TYPE_WEAPON &&
			(pItemData->GetWeaponType() == CItemData::WEAPON_SWORD ||
				pItemData->GetWeaponType() == CItemData::WEAPON_ARROW))
			bStabGround = true;

#if defined(ENABLE_QUIVER_SYSTEM)
		if (pItemData->GetType() == CItemData::ITEM_TYPE_WEAPON &&
			pItemData->GetWeaponType() == CItemData::WEAPON_QUIVER)
			bStabGround = true;
#endif

		bStabGround = false;
		pGroundItemInstance->bAnimEnded = false;
	}
	else
	{
		pGroundItemInstance->bAnimEnded = true;
	}

	{
		// attaching effect
		CEffectManager& rem = CEffectManager::Instance();
		pGroundItemInstance->dwEffectInstanceIndex =
			rem.CreateEffect(m_dwDropItemEffectID, D3DXVECTOR3(x, -y, z), D3DXVECTOR3(0, 0, 0));

		pGroundItemInstance->eDropSoundType = __GetDropSoundType(*pItemData);
	}

	D3DXVECTOR3 normal;
	if (!CPythonBackground::Instance().GetNormal(int(x), int(y), &normal))
		normal = D3DXVECTOR3(0.0f, 0.0f, 1.0f);

	pGroundItemInstance->ThingInstance.Clear();
	pGroundItemInstance->ThingInstance.ReserveModelThing(1);
	pGroundItemInstance->ThingInstance.ReserveModelInstance(1);
	pGroundItemInstance->ThingInstance.RegisterModelThing(0, pItemModel);
	pGroundItemInstance->ThingInstance.SetModelInstance(0, 0, 0);
	if (bDrop)
	{
		pGroundItemInstance->v3EndPosition = D3DXVECTOR3(x, -y, z);
		pGroundItemInstance->ThingInstance.SetPosition(0, 0, 0);
	}
	else
		pGroundItemInstance->ThingInstance.SetPosition(x, -y, z);

	pGroundItemInstance->ThingInstance.Update();
	pGroundItemInstance->ThingInstance.Transform();
	pGroundItemInstance->ThingInstance.Deform();

	if (bDrop)
	{
		D3DXVECTOR3 vMin, vMax;
		pGroundItemInstance->ThingInstance.GetBoundBox(&vMin, &vMax);
		pGroundItemInstance->v3Center = (vMin + vMax) * 0.5f;

		std::pair<float, int> f[3] =
		{
			std::make_pair(vMax.x - vMin.x,0),
			std::make_pair(vMax.y - vMin.y,1),
			std::make_pair(vMax.z - vMin.z,2)
		};

		std::sort(f, f + 3);

		//int no_rotation_axis = -1;

		D3DXVECTOR3 rEnd;

		if ( /* f[1].first - f[0].first < (f[2].first - f[0].first) * 0.30f */ bStabGround)
		{
			// ����
			if (f[2].second == 0) // axis x
			{
				rEnd.y = 90.0f + frandom(-15.0f, 15.0f);
				rEnd.x = frandom(0.0f, 360.0f);
				rEnd.z = frandom(-15.0f, 15.0f);
			}
			else if (f[2].second == 1) // axis y
			{
				rEnd.y = frandom(0.0f, 360.0f);
				rEnd.x = frandom(-15.0f, 15.0f);
				rEnd.z = 180.0f + frandom(-15.0f, 15.0f);
			}
			else // axis z
			{
				rEnd.y = 180.0f + frandom(-15.0f, 15.0f);
				rEnd.x = 0.0f + frandom(-15.0f, 15.0f);
				rEnd.z = frandom(0.0f, 360.0f);
			}
		}
		else
		{
			// ����
			// ���� �븻�� ������ ���� ��
			if (f[0].second == 0)
			{
				// y, z = by normal
				pGroundItemInstance->qEnd =
					RotationArc(
						D3DXVECTOR3(
							((float)(random() % 2)) * 2 - 1 + frandom(-0.1f, 0.1f),
							0 + frandom(-0.1f, 0.1f),
							0 + frandom(-0.1f, 0.1f)),
						D3DXVECTOR3(0, 0, 1) /* normal */);
			}
			else if (f[0].second == 1)
			{
				pGroundItemInstance->qEnd =
					RotationArc(
						D3DXVECTOR3(
							0 + frandom(-0.1f, 0.1f),
							((float)(random() % 2)) * 2 - 1 + frandom(-0.1f, 0.1f),
							0 + frandom(-0.1f, 0.1f)),
						D3DXVECTOR3(0, 0, 1) /* normal */);
			}
			else
			{
				pGroundItemInstance->qEnd =
					RotationArc(
						D3DXVECTOR3(
							0 + frandom(-0.1f, 0.1f),
							0 + frandom(-0.1f, 0.1f),
							((float)(random() % 2)) * 2 - 1 + frandom(-0.1f, 0.1f)),
						D3DXVECTOR3(0, 0, 1) /* normal */);
			}
		}
		//D3DXQuaternionRotationYawPitchRoll(&pGroundItemInstance->qEnd, rEnd.y, rEnd.x, rEnd.z);
		float rot = frandom(0.0f, 2.0f * 3.14159265358979323846264338327950288f);
		D3DXQUATERNION q(0, 0, cosf(rot), sinf(rot));
		D3DXQuaternionMultiply(&pGroundItemInstance->qEnd, &pGroundItemInstance->qEnd, &q);
		q = RotationArc(D3DXVECTOR3(0, 0, 1), normal);
		D3DXQuaternionMultiply(&pGroundItemInstance->qEnd, &pGroundItemInstance->qEnd, &q);

		pGroundItemInstance->dwStartTime = CTimer::Instance().GetCurrentMillisecond();
		pGroundItemInstance->dwEndTime = pGroundItemInstance->dwStartTime + 300;
		pGroundItemInstance->v3RotationAxis.x = sinf(rot + 0); //frandom(0.4f, 0.7f)* (2 * (int)(random() % 2) - 1);
		pGroundItemInstance->v3RotationAxis.y = cosf(rot + 0); //frandom(0.4f, 0.7f)* (2 * (int)(random() % 2) - 1);
		pGroundItemInstance->v3RotationAxis.z = 0; //frandom(0.4f, 0.7f)* (2 * (int)(random() % 2) - 1);

		/*
		switch (no_rotation_axis)
		{
		case 0:
			pGroundItemInstance->rStart.x = 0;
			break;
		case 1:
			pGroundItemInstance->rStart.y = 0;
			break;
		case 2:
			pGroundItemInstance->rStart.z = 0;
			break;
		}
		*/

		D3DXVECTOR3 v3Adjust = -pGroundItemInstance->v3Center;
		D3DXMATRIX mat;
		D3DXMatrixRotationQuaternion(&mat, &pGroundItemInstance->qEnd);
		/*
		D3DXMatrixRotationYawPitchRoll(&mat,
			D3DXToRadian(pGroundItemInstance->rEnd.y),
			D3DXToRadian(pGroundItemInstance->rEnd.x),
			D3DXToRadian(pGroundItemInstance->rEnd.z)
		);
		*/

		D3DXVec3TransformCoord(&v3Adjust, &v3Adjust, &mat);
		//Tracef("%f %f %f\n", v3Adjust.x, v3Adjust.y, v3Adjust.z);
		//pGroundItemInstance->v3EndPosition += v3Adjust;
		//pGroundItemInstance->rEnd.z += pGroundItemInstance->v3Center.z;
	}

	pGroundItemInstance->ThingInstance.Show();

	m_GroundItemInstanceMap.insert(TGroundItemInstanceMap::value_type(dwVirtualID, pGroundItemInstance));

	CPythonTextTail& rkTextTail = CPythonTextTail::Instance();

#if defined(ENABLE_ITEM_DROP_RENEWAL)
	static char szItemName[128]{};
	int len = 0;

	switch (pItemData->GetType())
	{
	case CItemData::ITEM_TYPE_POLYMORPH:
	{
		CPythonNonPlayer& rkNonPlayer = CPythonNonPlayer::Instance();
		auto c_pMobTable = rkNonPlayer.GetTable(alSockets[0]);
		if (c_pMobTable)
			len += snprintf(szItemName, sizeof(szItemName), "%s", c_pMobTable->szLocaleName);
		break;
	}
	case CItemData::ITEM_TYPE_SKILLBOOK:
	case CItemData::ITEM_TYPE_SKILLFORGET:
	{
		const DWORD dwSkillVnum = (dwVirtualNumber == 50300 || dwVirtualNumber == 70037) ? alSockets[0] : 0;
		CPythonSkill::SSkillData* c_pSkillData;
		if ((dwSkillVnum != 0) && CPythonSkill::Instance().GetSkillData(dwSkillVnum, &c_pSkillData))
			len += snprintf(szItemName, sizeof(szItemName), "%s", c_pSkillData->GradeData[0].strName.c_str());

		break;
	}
	}

#if defined(ENABLE_SET_ITEM)
	len += snprintf(szItemName + len, sizeof(szItemName) - len, (len > 0) ? " %s" : "%s", GetPreName(bSetValue, pItemData->GetName()));
#else
	len += snprintf(szItemName + len, sizeof(szItemName) - len, (len > 0) ? " %s" : "%s", pItemData->GetName());
#endif

	bool bHasAttr = false;
	for (size_t i = 0; i < ITEM_ATTRIBUTE_SLOT_MAX_NUM; ++i)
	{
		if (aAttrs[i].wType != 0 && aAttrs[i].lValue != 0)
		{
			bHasAttr = true;
			break;
		}
	}
	rkTextTail.RegisterItemTextTail(
		dwVirtualID,
		szItemName,
		&pGroundItemInstance->ThingInstance,
		bHasAttr);
#else

#if defined(ENABLE_SET_ITEM)
	const char* szItemName = GetPreName(bSetValue, pItemData->GetName());
	rkTextTail.RegisterItemTextTail(
		dwVirtualID,
		szItemName,
		&pGroundItemInstance->ThingInstance);
#else
	rkTextTail.RegisterItemTextTail(
		dwVirtualID,
		pItemData->GetName(),
		&pGroundItemInstance->ThingInstance);
#endif
#endif
}

void CPythonItem::SetOwnership(DWORD dwVID, const char* c_pszName)
{
	TGroundItemInstanceMap::iterator itor = m_GroundItemInstanceMap.find(dwVID);

	if (m_GroundItemInstanceMap.end() == itor)
		return;

	TGroundItemInstance* pGroundItemInstance = itor->second;
	pGroundItemInstance->stOwnership.assign(c_pszName);

	CPythonTextTail& rkTextTail = CPythonTextTail::Instance();
	rkTextTail.SetItemTextTailOwner(dwVID, c_pszName);
}

bool CPythonItem::GetOwnership(DWORD dwVID, const char** c_pszName)
{
	TGroundItemInstanceMap::iterator itor = m_GroundItemInstanceMap.find(dwVID);

	if (m_GroundItemInstanceMap.end() == itor)
		return false;

	TGroundItemInstance* pGroundItemInstance = itor->second;
	*c_pszName = pGroundItemInstance->stOwnership.c_str();

	return true;
}

void CPythonItem::DeleteAllItems()
{
	CPythonTextTail& rkTextTail = CPythonTextTail::Instance();

	TGroundItemInstanceMap::iterator i;
	for (i = m_GroundItemInstanceMap.begin(); i != m_GroundItemInstanceMap.end(); ++i)
	{
		TGroundItemInstance* pGroundItemInst = i->second;
		rkTextTail.DeleteItemTextTail(i->first);
		pGroundItemInst->Clear();
		m_GroundItemInstancePool.Free(pGroundItemInst);
	}
	m_GroundItemInstanceMap.clear();
#if defined(ENABLE_LOOTING_SYSTEM)
	ClearLootFilteredItems();
#endif
}

void CPythonItem::DeleteItem(DWORD dwVirtualID)
{
	TGroundItemInstanceMap::iterator itor = m_GroundItemInstanceMap.find(dwVirtualID);
	if (m_GroundItemInstanceMap.end() == itor)
		return;

	TGroundItemInstance* pGroundItemInstance = itor->second;
	pGroundItemInstance->Clear();
	m_GroundItemInstancePool.Free(pGroundItemInstance);
	m_GroundItemInstanceMap.erase(itor);

	// Text Tail
	CPythonTextTail::Instance().DeleteItemTextTail(dwVirtualID);

#if defined(ENABLE_LOOTING_SYSTEM)
	EraseLootFilteredItem(dwVirtualID);
#endif
}

bool CPythonItem::GetCloseMoney(const TPixelPosition& c_rPixelPosition, DWORD* pdwItemID, DWORD dwDistance)
{
	DWORD dwCloseItemID = 0;
	DWORD dwCloseItemDistance = 1000 * 1000;

	TGroundItemInstanceMap::iterator i;
	for (i = m_GroundItemInstanceMap.begin(); i != m_GroundItemInstanceMap.end(); ++i)
	{
		TGroundItemInstance* pInstance = i->second;

		if (pInstance->dwVirtualNumber != VNUM_MONEY)
			continue;

		DWORD dwxDistance = DWORD(c_rPixelPosition.x - pInstance->v3EndPosition.x);
		DWORD dwyDistance = DWORD(c_rPixelPosition.y - (-pInstance->v3EndPosition.y));
		DWORD dwDistance = DWORD(dwxDistance * dwxDistance + dwyDistance * dwyDistance);

#if defined(ENABLE_LOOTING_SYSTEM)
		if (!IsLootFilteredItem(i->first) && dwxDistance * dwxDistance + dwyDistance * dwyDistance < dwCloseItemDistance)
#else
		if (dwxDistance * dwxDistance + dwyDistance * dwyDistance < dwCloseItemDistance)
#endif
		{
			dwCloseItemID = i->first;
			dwCloseItemDistance = dwDistance;
		}
	}

	if (dwCloseItemDistance > float(dwDistance) * float(dwDistance))
		return false;

	*pdwItemID = dwCloseItemID;

	return true;
}

bool CPythonItem::GetCloseItem(const TPixelPosition& c_rPixelPosition, DWORD* pdwItemID, DWORD dwDistance)
{
	DWORD dwCloseItemID = 0;
	DWORD dwCloseItemDistance = 1000 * 1000;

	TGroundItemInstanceMap::iterator i;
	for (i = m_GroundItemInstanceMap.begin(); i != m_GroundItemInstanceMap.end(); ++i)
	{
		TGroundItemInstance* pInstance = i->second;

		DWORD dwxDistance = DWORD(c_rPixelPosition.x - pInstance->v3EndPosition.x);
		DWORD dwyDistance = DWORD(c_rPixelPosition.y - (-pInstance->v3EndPosition.y));
		DWORD dwDistance = DWORD(dwxDistance * dwxDistance + dwyDistance * dwyDistance);

		if (dwxDistance * dwxDistance + dwyDistance * dwyDistance < dwCloseItemDistance)
		{
			dwCloseItemID = i->first;
			dwCloseItemDistance = dwDistance;
		}
	}

	if (dwCloseItemDistance > float(dwDistance) * float(dwDistance))
		return false;

	*pdwItemID = dwCloseItemID;

	return true;
}

BOOL CPythonItem::GetGroundItemPosition(DWORD dwVirtualID, TPixelPosition* pPosition)
{
	TGroundItemInstanceMap::iterator itor = m_GroundItemInstanceMap.find(dwVirtualID);
	if (m_GroundItemInstanceMap.end() == itor)
		return FALSE;

	TGroundItemInstance* pInstance = itor->second;

	const D3DXVECTOR3& rkD3DVct3 = pInstance->ThingInstance.GetPosition();

	pPosition->x = +rkD3DVct3.x;
	pPosition->y = -rkD3DVct3.y;
	pPosition->z = +rkD3DVct3.z;

	return TRUE;
}

DWORD CPythonItem::__Pick(const POINT& c_rkPtMouse)
{
	float fu, fv, ft;

	TGroundItemInstanceMap::iterator itor = m_GroundItemInstanceMap.begin();
	for (; itor != m_GroundItemInstanceMap.end(); ++itor)
	{
		TGroundItemInstance* pInstance = itor->second;

		if (pInstance->ThingInstance.Intersect(&fu, &fv, &ft))
		{
			return itor->first;
		}
	}

	CPythonTextTail& rkTextTailMgr = CPythonTextTail::Instance();
	return rkTextTailMgr.Pick(c_rkPtMouse.x, c_rkPtMouse.y);
}

bool CPythonItem::GetPickedItemID(DWORD* pdwPickedItemID)
{
	if (INVALID_ID == m_dwPickedItemID)
		return false;

	*pdwPickedItemID = m_dwPickedItemID;
	return true;
}

DWORD CPythonItem::GetVirtualNumberOfGroundItem(DWORD dwVID)
{
	TGroundItemInstanceMap::iterator itor = m_GroundItemInstanceMap.find(dwVID);

	if (itor == m_GroundItemInstanceMap.end())
		return 0;
	else
		return itor->second->dwVirtualNumber;
}

void CPythonItem::BuildNoGradeNameData(int iType)
{
	/*
	CMapIterator<std::string, CItemData*> itor = CItemManager::Instance().GetItemNameMapIterator();

	m_NoGradeNameItemData.clear();
	m_NoGradeNameItemData.reserve(1024);

	while (++itor)
	{
		CItemData* pItemData = *itor;
		if (iType == pItemData->GetType())
			m_NoGradeNameItemData.push_back(pItemData);
	}
	*/
}

DWORD CPythonItem::GetNoGradeNameDataCount()
{
	return m_NoGradeNameItemData.size();
}

CItemData* CPythonItem::GetNoGradeNameDataPtr(DWORD dwIndex)
{
	if (dwIndex >= m_NoGradeNameItemData.size())
		return NULL;

	return m_NoGradeNameItemData[dwIndex];
}

#if defined(ENABLE_ACCE_COSTUME_SYSTEM)
bool CPythonItem::IsAcceScroll(const DWORD c_dwVnum) const
{
	switch (c_dwVnum)
	{
		case EAcceScroll::ACCE_SCROLL_1:
		case EAcceScroll::ACCE_SCROLL_2:
			return true;
	}
	return false;
}
#endif

#if defined(ENABLE_CHANGE_LOOK_SYSTEM)
static bool __CanAddTransmutationItem(const CItemData* c_pItemData)
{
	const BYTE c_bType = CPythonPlayer::Instance().GetChangeLookWindowType();
	const BYTE c_bItemType = c_pItemData->GetType();
	const BYTE c_bItemSubType = c_pItemData->GetSubType();

	switch (static_cast<EChangeLookType>(c_bType))
	{
	case EChangeLookType::CHANGE_LOOK_TYPE_ITEM:
		if (c_pItemData->IsMainWeapon()
#if defined(ENABLE_WEAPON_COSTUME_SYSTEM)
			|| c_pItemData->IsCostumeWeapon()
#endif
			) return true;

		if (c_pItemData->IsArmorBody()
#if defined(ENABLE_COSTUME_SYSTEM)
			|| c_pItemData->IsCostumeBody()
#endif
			) return true;

		break;

	case EChangeLookType::CHANGE_LOOK_TYPE_MOUNT:
#if defined(ENABLE_MOUNT_COSTUME_SYSTEM)
		if (c_pItemData->IsCostumeMount())
			return true;
#endif
		break;
	}

	return false;
}

static bool __CheckTransmutationItem(const CItemData* c_pLeftItemData, const CItemData* c_pRightItemData)
{
	if (c_pLeftItemData == c_pRightItemData)
		return false;

	if (c_pRightItemData->GetIndex() == c_pLeftItemData->GetIndex())
		return false;

#if defined(ENABLE_COSTUME_SYSTEM)
	if (c_pRightItemData->GetType() != c_pLeftItemData->GetType())
	{
		bool bCanPass = false;
		if ((c_pRightItemData->IsCostumeBody() && c_pLeftItemData->IsArmorBody()) ||
			(c_pRightItemData->IsArmorBody() && c_pLeftItemData->IsCostumeBody()))

			bCanPass = true;

#	if defined(ENABLE_WEAPON_COSTUME_SYSTEM)
		if ((c_pRightItemData->IsCostumeWeapon() && c_pLeftItemData->IsMainWeapon() && c_pRightItemData->GetValue(3) == c_pLeftItemData->GetSubType()) ||
			(c_pRightItemData->IsMainWeapon() && c_pLeftItemData->IsCostumeWeapon() && c_pRightItemData->GetSubType() == c_pLeftItemData->GetValue(3)))
			bCanPass = true;
#	endif

		return bCanPass;
	}
	else
#endif
	{
		if (c_pRightItemData->GetType() != c_pLeftItemData->GetType())
			return false;

		if (c_pRightItemData->GetSubType() != c_pLeftItemData->GetSubType())
			return false;
		
		if (c_pLeftItemData->IsArmor() || c_pLeftItemData->IsMainWeapon())
			if (c_pRightItemData->GetAntiFlags() != c_pLeftItemData->GetAntiFlags())
				return false;

#if defined(ENABLE_COSTUME_SYSTEM)
		if (c_pRightItemData->IsCostumeBody() == c_pLeftItemData->IsCostumeBody())
		{
			if ((c_pRightItemData->IsAntiFlag(CItemData::ITEM_ANTIFLAG_FEMALE) && c_pLeftItemData->IsAntiFlag(CItemData::ITEM_ANTIFLAG_MALE)) ||
				(c_pRightItemData->IsAntiFlag(CItemData::ITEM_ANTIFLAG_MALE) && c_pLeftItemData->IsAntiFlag(CItemData::ITEM_ANTIFLAG_FEMALE)))
				return false;
		}
#endif

#if defined(ENABLE_WEAPON_COSTUME_SYSTEM)
		if (c_pLeftItemData->IsCostumeWeapon())
			if ((c_pLeftItemData->GetValue(3) != c_pRightItemData->GetValue(3)))
				return false;
#endif
	}

	return true;
}

bool CPythonItem::CanAddChangeLookItem(const CItemData* c_pLeftItemData, const CItemData* c_pRightItemData) const
{
	if (c_pLeftItemData == nullptr)
		return false;

	if (c_pRightItemData)
		if (__CheckTransmutationItem(c_pLeftItemData, c_pRightItemData) == false)
			return false;
		else if (__CanAddTransmutationItem(c_pLeftItemData) == false)
			return false;
		else
			return true;
	else
		if (__CanAddTransmutationItem(c_pLeftItemData) == false)
			return false;

	return true;
}

bool CPythonItem::CanAddChangeLookFreeItem(const DWORD c_dwVnum) const
{
	const BYTE c_bType = CPythonPlayer::Instance().GetChangeLookWindowType();
	switch (static_cast<EChangeLookType>(c_bType))
	{
		default:
		case EChangeLookType::CHANGE_LOOK_TYPE_ITEM:
			return c_dwVnum == static_cast<DWORD>(EChangeLookItems::CHANGE_LOOK_TICKET_1)
				|| c_dwVnum == static_cast<DWORD>(EChangeLookItems::CHANGE_LOOK_TICKET_2);

		case EChangeLookType::CHANGE_LOOK_TYPE_MOUNT:
			return c_dwVnum == static_cast<DWORD>(EChangeLookItems::CHANGE_LOOK_MOUNT_TICKET);
	}
}

bool CPythonItem::IsChangeLookClearScrollItem(const DWORD c_dwVnum) const
{
	return c_dwVnum == static_cast<DWORD>(EChangeLookItems::CHANGE_LOOK_CLEAR_SCROLL)
		|| c_dwVnum == static_cast<DWORD>(EChangeLookItems::CHANGE_LOOK_MOUNT_REVERSAL);
}
#endif

#if defined(ENABLE_LOOTING_SYSTEM)
bool CPythonItem::IsLootFilteredItem(DWORD dwVID) const
{
	return m_LootFilteredItemsSet.end() != m_LootFilteredItemsSet.find(dwVID);
}

void CPythonItem::InsertLootFilteredItem(DWORD dwVID)
{
	m_LootFilteredItemsSet.insert(dwVID);
}

void CPythonItem::EraseLootFilteredItem(DWORD dwVID)
{
	m_LootFilteredItemsSet.erase(dwVID);
}

void CPythonItem::ClearLootFilteredItems()
{
	m_LootFilteredItemsSet.clear();
}
#endif

#if defined(ENABLE_SET_ITEM)
void CPythonItem::SetPreName(const int iType, const std::string& c_rstPreName)
{
	m_PreNameMap.insert(std::make_pair(iType, c_rstPreName));
}

const char* CPythonItem::GetPreName(const int iType, const char* szItemName) const
{
	if (iType == 0 || iType >= SET_ITEM_SET_VALUE_MAX)
		return szItemName;

	static char szBuf[64 + 1];
	memset(szBuf, 0, sizeof(szBuf));

	const auto& it = m_PreNameMap.find(iType);
	if (it != m_PreNameMap.end() && !it->second.empty())
	{
		snprintf(szBuf, sizeof(szBuf), "%s%s", it->second.c_str(), szItemName);
		return szBuf;
	}

	return szItemName;
}
#endif

void CPythonItem::Destroy()
{
	DeleteAllItems();
	m_GroundItemInstancePool.Clear();
}

void CPythonItem::Create()
{
	CEffectManager::Instance().RegisterEffect2("d:/ymir work/effect/etc/dropitem/dropitem.mse", &m_dwDropItemEffectID);
#if defined(ENABLE_GRAPHIC_ON_OFF)
	CEffectManager::Instance().SetDropEffectID(m_dwDropItemEffectID);
#endif
}

CPythonItem::CPythonItem()
{
	m_dwPickedItemID = INVALID_ID;
}

CPythonItem::~CPythonItem()
{
	assert(m_GroundItemInstanceMap.empty());
#if defined(ENABLE_SET_ITEM)
	m_PreNameMap.clear();
#endif
}
