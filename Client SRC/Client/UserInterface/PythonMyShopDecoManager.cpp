/**
* Filename: PythonMyShopDecoManager.cpp
* Authors: Owsap, blackdragonx61 (aka. Mali)
**/

#include "StdAfx.h"

#if defined(ENABLE_MYSHOP_DECO)
#include "../EterLib/Camera.h"
#include "../EterBase/Stl.h"

#include "PythonApplication.h"
#include "PythonMyShopDecoManager.h"

CPythonMyShopDecoManager::CPythonMyShopDecoManager()
{
	Initialize();
}

CPythonMyShopDecoManager::~CPythonMyShopDecoManager()
{
	Destroy();
}

void CPythonMyShopDecoManager::Initialize()
{
	m_pBackgroundImage = nullptr;
	m_pModelInstance = nullptr;
	m_bShow = false;
	m_fRot = 0.0f;
}

void CPythonMyShopDecoManager::Destroy()
{
	m_bShow = false;
	m_fRot = 0.0f;
	m_pModelInstance = nullptr;

	if (m_pBackgroundImage)
	{
		delete m_pBackgroundImage;
		m_pBackgroundImage = nullptr;
	}

	stl_wipe_second(m_ModelInstanceMap);
}

bool CPythonMyShopDecoManager::CreateBackground(DWORD dwWidth, DWORD dwHeight)
{
#if defined(RENDER_TARGET)
	m_bShow = false;

	if (!m_pBackgroundImage)
	{
		CResource* pResource = CResourceManager::Instance().GetResourcePointer("d:/ymir work/ui/game/myshop_deco/model_view_bg.sub");
		m_pBackgroundImage = new CGraphicImageInstance;
		m_pBackgroundImage->SetImagePointer(static_cast<CGraphicImage*>(pResource));
		m_pBackgroundImage->SetScale(static_cast<float>(dwWidth) / 190.0f, static_cast<float>(dwHeight) / 210.0f);
	}

	return true;
#else
	return false;
#endif
}

void CPythonMyShopDecoManager::RenderBackground() const
{
#if defined(RENDER_TARGET)
	if (!m_bShow)
		return;

	if (!m_pBackgroundImage)
		return;

	RECT rectRender;
	if (!CRenderTargetManager::Instance().GetRenderTargetRect(CRenderTargetManager::RENDER_TARGET_INDEX_MYSHOPDECO, &rectRender))
		return;

	if (!CRenderTargetManager::Instance().ChangeRenderTarget(CRenderTargetManager::RENDER_TARGET_INDEX_MYSHOPDECO))
		return;

	CRenderTargetManager::Instance().ClearRenderTarget();
	CPythonGraphic::Instance().SetInterfaceRenderState();

	m_pBackgroundImage->Render();

	CRenderTargetManager::Instance().ResetRenderTarget();
#endif
}

bool CPythonMyShopDecoManager::CreateModelInstance(DWORD dwVnum)
{
#if defined(RENDER_TARGET)
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
#else
	return false;
#endif
}

CInstanceBase* CPythonMyShopDecoManager::GetModelInstance(DWORD dwVnum) const
{
#if defined(RENDER_TARGET)
	const auto it = m_ModelInstanceMap.find(dwVnum);
	if (m_ModelInstanceMap.end() == it)
		return nullptr;

	return it->second;
#else
	return nullptr;
#endif
}

bool CPythonMyShopDecoManager::SelectModel(DWORD dwVnum)
{
#if defined(RENDER_TARGET)
	if (!dwVnum)
	{
		m_pModelInstance = nullptr;
		return false;
	}

	if (!CreateModelInstance(dwVnum))
	{
		m_pModelInstance = nullptr;
		return false;
	}

	m_pModelInstance = GetModelInstance(dwVnum);
	if (!m_pModelInstance)
		return false;

	m_pModelInstance->GetGraphicThingInstancePtr()->ClearAttachingEffect();
	m_fRot = 0.0f;

	m_pModelInstance->Refresh(CRaceMotionData::NAME_WAIT, true);
	m_pModelInstance->SetLoopMotion(CRaceMotionData::NAME_WAIT, 0.0f, 0.0f);
	m_pModelInstance->SetLODLimits(0, 100.0f);
	m_pModelInstance->SetAlwaysRender(true);
	m_pModelInstance->SetRotation(0.0f);
	m_pModelInstance->NEW_SetPixelPosition(TPixelPosition(0.0f, 0.0f, 0.0f));

	CCameraManager::Instance().SetCurrentCamera(CCameraManager::DEFAULT_MYSHOPDECO_CAMERA);
	CCamera* pCam = CCameraManager::Instance().GetCurrentCamera();
	pCam->SetTargetHeight(110.0f);
	CCameraManager::Instance().ResetToPreviousCamera();

	return true;
#else
	return false;
#endif
}

void CPythonMyShopDecoManager::UpdateModel()
{
#if defined(RENDER_TARGET)
	if (!m_bShow)
		return;

	if (!m_pModelInstance)
		return;

	if (m_fRot < 360.0f)
		m_fRot += 1.0f;
	else
		m_fRot = 0.0f;

	m_pModelInstance->SetRotation(m_fRot);
	m_pModelInstance->Transform();

	CActorInstance& rkModelActor = m_pModelInstance->GetGraphicThingInstanceRef();
	rkModelActor.RotationProcess();
#endif
}

void CPythonMyShopDecoManager::DeformModel() const
{
#if defined(RENDER_TARGET)
	if (!m_bShow)
		return;

	if (!m_pModelInstance)
		return;

	m_pModelInstance->Deform();
#endif
}

void CPythonMyShopDecoManager::RenderModel() const
{
#if defined(RENDER_TARGET)
	if (!m_bShow)
		return;

	RECT rectRender;
	if (!CRenderTargetManager::Instance().GetRenderTargetRect(CRenderTargetManager::RENDER_TARGET_INDEX_MYSHOPDECO, &rectRender))
		return;

	if (!CRenderTargetManager::Instance().ChangeRenderTarget(CRenderTargetManager::RENDER_TARGET_INDEX_MYSHOPDECO))
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

	CCameraManager::Instance().SetCurrentCamera(CCameraManager::DEFAULT_MYSHOPDECO_CAMERA);
	CCamera* pCam = CCameraManager::Instance().GetCurrentCamera();

	CPythonGraphic::Instance().PushState();

	pCam->SetViewParams(
		D3DXVECTOR3(0.0f, -1500.0f, 600.0f),
		D3DXVECTOR3(0.0f, 0.0f, 0.0f),
		D3DXVECTOR3(0.0f, 0.0f, 1.0f)
	);

#if defined(__RENDER_TARGET_VIEW_PORT_FIX__)
	const D3DXVECTOR3& v3Target = pCam->GetTarget();

	float fPitchDegree = fMIN(80.0f, fMAX(-80.0f, pCam->GetPitch()));
	pCam->RotateEyeAroundTarget(fPitchDegree, pCam->GetRoll());

	pCam->Move(D3DXVECTOR3(v3Target.x, v3Target.y, v3Target.y + pCam->GetTargetHeight()));
	CPythonGraphic::Instance().UpdateViewMatrix();
#else
	CPythonApplication::Instance().SetMyShopDecoCameraPosition();
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
#endif
}

void CPythonMyShopDecoManager::SetShow(bool bShow)
{
	m_bShow = bShow;
}
#endif
