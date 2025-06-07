/**
* Filename: PythonIllustratedManager.cpp
* Author: dotpngr (aka. xP3NG3Rx)
**/

#include "StdAfx.h"

#if defined(ENABLE_MONSTER_CARD)
#include "../EterLib/Camera.h"
#include "../EterBase/Stl.h"

#include "PythonApplication.h"
#include "PythonIllustratedManager.h"

CPythonIllustratedManager::CPythonIllustratedManager()
{
	Initialize();
}

CPythonIllustratedManager::~CPythonIllustratedManager()
{
	Destroy();
}

void CPythonIllustratedManager::Initialize()
{
	m_pModelInstance = nullptr;
	m_f8 = 0.0f;
	m_f12 = 0.0f;
	m_f16 = 0.0f;
	m_f20 = 0.0f;
	m_f24 = 0.0f;
	m_pBackgroundImage = nullptr;
	m_bShow = false;
}

void CPythonIllustratedManager::Destroy()
{
	m_pModelInstance = nullptr;
	m_f8 = 0.0f;
	m_f12 = 0.0f;
	m_f16 = 0.0f;
	m_f20 = 0.0f;
	m_f24 = 0.0f;

	if (m_pBackgroundImage)
	{
		delete m_pBackgroundImage;
		m_pBackgroundImage = nullptr;
	}

	m_bShow = false;

	stl_wipe_second(m_ModelInstanceMap);
}

bool CPythonIllustratedManager::CreateBackground(DWORD dwWidth, DWORD dwHeight)
{
	if (!m_pBackgroundImage)
	{
		CResource* pResource = CResourceManager::Instance().GetResourcePointer("d:/ymir work/ui/game/monster_card/model_view_bg.sub");
		m_pBackgroundImage = new CGraphicImageInstance;
		m_pBackgroundImage->SetImagePointer(static_cast<CGraphicImage*>(pResource));
		m_pBackgroundImage->SetScale(static_cast<float>(dwWidth) / 240.0f, static_cast<float>(dwHeight) / 306.0f);
	}

	return true;
}

void CPythonIllustratedManager::RenderBackground() const
{
	if (!m_bShow)
		return;

	if (!m_pBackgroundImage)
		return;

	RECT rectRender;
	if (!CRenderTargetManager::Instance().GetRenderTargetRect(CRenderTargetManager::RENDER_TARGET_INDEX_ILLUSTRATED, &rectRender))
		return;

	if (!CRenderTargetManager::Instance().ChangeRenderTarget(CRenderTargetManager::RENDER_TARGET_INDEX_ILLUSTRATED))
		return;

	CRenderTargetManager::Instance().ClearRenderTarget();
	CPythonGraphic::Instance().SetInterfaceRenderState();

	m_pBackgroundImage->Render();

	CRenderTargetManager::Instance().ResetRenderTarget();
}

bool CPythonIllustratedManager::CreateModelInstance(DWORD dwVnum)
{
	if (GetModelInstance(dwVnum))
		return true;

	CInstanceBase::SCreateData kCreateData;
	memset(&kCreateData, 0, sizeof(kCreateData));
	kCreateData.m_bType = CActorInstance::TYPE_OBJECT;
	kCreateData.m_dwRace = dwVnum;

	CInstanceBase* pModel = new CInstanceBase();
	if (!pModel->Create(kCreateData))
	{
		delete pModel;
		return false;
	}

	m_ModelInstanceMap.emplace(dwVnum, pModel);
	return true;
}

CInstanceBase* CPythonIllustratedManager::GetModelInstance(DWORD dwVnum) const
{
	const auto it = m_ModelInstanceMap.find(dwVnum);
	if (m_ModelInstanceMap.end() == it)
		return nullptr;

	return it->second;
}

bool CPythonIllustratedManager::SelectModel(DWORD dwVnum)
{
	if (!dwVnum)
	{
		m_pModelInstance = nullptr;
		return false;
	}

	if (!CreateModelInstance(dwVnum))
	{
		Tracef("MC SelectModel failed 0 : %u\n", dwVnum);
		m_pModelInstance = nullptr;
		return false;
	}

	m_pModelInstance = GetModelInstance(dwVnum);
	if (!m_pModelInstance)
	{
		Tracef("MC SelectModel failed 1 : %u\n", dwVnum);
		return false;
	}

	m_pModelInstance->Refresh(CRaceMotionData::NAME_WAIT, true);
	m_pModelInstance->SetLODLimits(0, 100.0f);
	m_pModelInstance->SetAlwaysRender(true);
	m_pModelInstance->SetRotation(0.0f);
	m_pModelInstance->NEW_SetPixelPosition(TPixelPosition(0.0f, 0.0f, 0.0f));

	CActorInstance& rkActor = m_pModelInstance->GetGraphicThingInstanceRef();
	float fHeight = rkActor.GetHeight();
	m_f8 = fHeight;
	m_f12 = 0.0f;
	m_f20 = 0.0f;
	m_f16 = m_f8 * 8.9f / 140.0f;
	m_f24 = m_f8 / 35.0f;

	CCameraManager::Instance().SetCurrentCamera(CCameraManager::DEFAULT_ILLUSTRATED_CAMERA);
	CCamera* pCam = CCameraManager::Instance().GetCurrentCamera();
	pCam->SetTargetHeight(m_f8 / 2.0f);
	CCameraManager::Instance().ResetToPreviousCamera();
	return true;
}

void CPythonIllustratedManager::UpdateModel() const
{
	if (!m_bShow)
		return;

	if (!m_pModelInstance)
		return;

	m_pModelInstance->Transform();
	CActorInstance& rkModelActor = m_pModelInstance->GetGraphicThingInstanceRef();
	rkModelActor.RotationProcess();
}

void CPythonIllustratedManager::DeformModel() const
{
	if (!m_bShow)
		return;

	if (!m_pModelInstance)
		return;

	m_pModelInstance->Deform();
}

void CPythonIllustratedManager::RenderModel() const
{
	if (!m_bShow)
		return;

	RECT rectRender;
	if (!CRenderTargetManager::Instance().GetRenderTargetRect(CRenderTargetManager::RENDER_TARGET_INDEX_ILLUSTRATED, &rectRender))
	{
		Tracef("MC Render failed : 0\n");
		return;
	}

	if (!CRenderTargetManager::Instance().ChangeRenderTarget(CRenderTargetManager::RENDER_TARGET_INDEX_ILLUSTRATED))
		return;

	if (!m_pModelInstance)
	{
		CRenderTargetManager::Instance().ResetRenderTarget();
		return;
	}

	CPythonGraphic::Instance().ClearDepthBuffer();

	const float fFov = CPythonGraphic::Instance().GetFOV();
	const float fAspect = CPythonGraphic::Instance().GetAspect();
	const float fNearY = CPythonGraphic::Instance().GetNearY();
	const float fFarY = CPythonGraphic::Instance().GetFarY();

#if defined(ENABLE_FOG_FIX)
	const BOOL bIsFog = CPythonBackground::Instance().GetFogMode();
#else
	const BOOL bIsFog = STATEMANAGER.GetRenderState(D3DRS_FOGENABLE);
#endif
	STATEMANAGER.SetRenderState(D3DRS_FOGENABLE, FALSE);

	const float fWidth = static_cast<float>(rectRender.right - rectRender.left);
	const float fHeight = static_cast<float>(rectRender.bottom - rectRender.top);

	CCameraManager::Instance().SetCurrentCamera(CCameraManager::DEFAULT_ILLUSTRATED_CAMERA);
	CCamera* pCam = CCameraManager::Instance().GetCurrentCamera();

	CPythonGraphic::Instance().PushState();

	pCam->SetViewParams(
		D3DXVECTOR3(0.0f, -(m_f8 * 8.9f + m_f12), 0.0f),
		D3DXVECTOR3(0.0f, m_f20, 0.0f),
		D3DXVECTOR3(0.0f, 0.0f, 1.0f)
	);

#if defined(__RENDER_TARGET_VIEW_PORT_FIX__)
	const D3DXVECTOR3& v3Target = pCam->GetTarget();
	pCam->Move(D3DXVECTOR3(v3Target.x, v3Target.y, v3Target.y + pCam->GetTargetHeight()));
	CPythonGraphic::Instance().UpdateViewMatrix();
#else
	CPythonApplication::Instance().SetIllustratedCameraPosition();
#endif

	CPythonGraphic::Instance().SetPerspective(10.0f, fWidth / fHeight, 100.0f, 15000.0f);

	m_pModelInstance->Render();

	CCameraManager::Instance().ResetToPreviousCamera();
#if defined(__RENDER_TARGET_VIEW_PORT_FIX__)
	CPythonGraphic::Instance().RestoreViewport();
#endif
	CPythonGraphic::Instance().PopState();
	CPythonGraphic::Instance().SetPerspective(fFov, fAspect, fNearY, fFarY);
	CRenderTargetManager::Instance().ResetRenderTarget();
	STATEMANAGER.SetRenderState(D3DRS_FOGENABLE, bIsFog);
}

void CPythonIllustratedManager::ModelViewReset()
{
	if (!m_pModelInstance)
		return;

	m_f12 = 0.0f;
	m_f20 = 0.0f;
	m_pModelInstance->SetRotation(0.0f);
	m_pModelInstance->NEW_SetPixelPosition(TPixelPosition(0.0f, 0.0f, 0.0f));
}

void CPythonIllustratedManager::ModelRotation(float fRot) const
{
	if (!m_pModelInstance)
		return;

	m_pModelInstance->SetRotation(fmod(fRot, 360.0f));
}

void CPythonIllustratedManager::ModelZoom(bool bZoom)
{
	if (!m_pModelInstance)
		return;

	if (bZoom)
	{
		m_f12 = m_f12 - m_f16;
		float v3 = -(m_f8 * 8.9f - m_f8 * 3.0f);
		m_f12 = fmax(v3, m_f12);
	}
	else
	{
		m_f12 = m_f12 + m_f16;
		float v6 = 14000.0f - m_f8 * 8.9f;
		float v7 = m_f8 * 8.9f + m_f8 * 5.0f;
		m_f12 = fmin(m_f12, v6);
		m_f12 = fmin(m_f12, v7);
	}
}

void CPythonIllustratedManager::ModelUpDown(bool bUp)
{
	if (!m_pModelInstance)
		return;

	if (bUp)
	{
		m_f20 = m_f20 + m_f24;
		m_f20 = fmin(m_f8, m_f20);
	}
	else
	{
		m_f20 = m_f20 - m_f24;
		m_f20 = fmax(-m_f8 / 2.0f, m_f20);
	}
}

void CPythonIllustratedManager::ChangeMotion(DWORD dwVnum) const
{
	if (!m_pModelInstance)
		return;

	const WORD wNextWalidMotion = m_pModelInstance->GetGraphicThingInstanceRef().NextValidMotion();

	if (m_pModelInstance->IsWalking())
		m_pModelInstance->EndWalking();

	m_pModelInstance->Refresh(wNextWalidMotion, true);
}

void CPythonIllustratedManager::SetShow(bool bShow)
{
	m_bShow = bShow;
}
#endif
