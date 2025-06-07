#include "StdAfx.h"

#include "SpeedTreeForestDirectX8.h"
#include "VertexShaders.h"

#include "../EterBase/Timer.h"
#include "../EterLib/StateManager.h"
#include "../EterLib/Camera.h"

CSpeedTreeForestDirectX8::CSpeedTreeForestDirectX8() : m_pDx(NULL), m_dwBranchVertexShader(0), m_dwLeafVertexShader(0) {}

CSpeedTreeForestDirectX8::~CSpeedTreeForestDirectX8() = default;

void CSpeedTreeForestDirectX8::UploadWindMatrix(UINT uiLocation, const float* pMatrix) const
{
	STATEMANAGER.SetVertexShaderConstant(uiLocation, pMatrix, 4);
}

void CSpeedTreeForestDirectX8::UpdateCompundMatrix(const D3DXVECTOR3& c_rEyeVec, const D3DXMATRIX& c_rmatView, const D3DXMATRIX& c_rmatProj)
{
	D3DXMATRIX matBlend;
	D3DXMatrixIdentity(&matBlend);

	D3DXMATRIX matBlendShader;
	D3DXMatrixMultiply(&matBlendShader, &c_rmatView, &c_rmatProj);

	float afDirection[3];
	afDirection[0] = matBlendShader.m[0][2];
	afDirection[1] = matBlendShader.m[1][2];
	afDirection[2] = matBlendShader.m[2][2];
	CSpeedTreeRT::SetCamera(c_rEyeVec, afDirection);

	D3DXMatrixTranspose(&matBlendShader, &matBlendShader);
	STATEMANAGER.SetVertexShaderConstant(c_nVertexShader_CompoundMatrix, (const float*)&matBlendShader, 4);
}

void CSpeedTreeForestDirectX8::Render(unsigned long ulRenderBitVector)
{
	UpdateSystem(CTimer::Instance().GetCurrentSecond());

	if (m_pMainTreeMap.empty())
		return;

	if (!(ulRenderBitVector & Forest_RenderToShadow) && !(ulRenderBitVector & Forest_RenderToMiniMap))
		UpdateCompundMatrix(CCameraManager::Instance().GetCurrentCamera()->GetEye(), ms_matView, ms_matProj);

	DWORD dwLightState = STATEMANAGER.GetRenderState(D3DRS_LIGHTING);
	DWORD dwColorVertexState = STATEMANAGER.GetRenderState(D3DRS_COLORVERTEX);
	DWORD dwFogVertexMode = STATEMANAGER.GetRenderState(D3DRS_FOGVERTEXMODE);

#ifdef WRAPPER_USE_DYNAMIC_LIGHTING
	STATEMANAGER.SetRenderState(D3DRS_LIGHTING, TRUE);
#else
	STATEMANAGER.SetRenderState(D3DRS_LIGHTING, FALSE);
	STATEMANAGER.SetRenderState(D3DRS_COLORVERTEX, TRUE);
#endif

	UINT uiCount;
	TreeMap::iterator itor = m_pMainTreeMap.begin();
	while (itor != m_pMainTreeMap.end())
	{
		const SpeedTreeWrapperPtr& pMainTree = (itor++)->second;
		for (const SpeedTreeWrapperWeakPtr& pWeakPtr : pMainTree->GetInstances(uiCount))
		{
			if (pWeakPtr.expired())
				continue;

			const SpeedTreeWrapperPtr& pInstance = pWeakPtr.lock();
			if (pInstance)
				pInstance->Advance();
		}
	}

	STATEMANAGER.SetVertexShaderConstant(c_nVertexShader_Light, m_afLighting, 3);
	STATEMANAGER.SetVertexShaderConstant(c_nVertexShader_Fog, m_afFog, 1);

	if (ulRenderBitVector & Forest_RenderToShadow)
	{
		STATEMANAGER.SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
		STATEMANAGER.SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
		STATEMANAGER.SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	}
	else
	{
		STATEMANAGER.SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		STATEMANAGER.SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
		STATEMANAGER.SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
		STATEMANAGER.SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
		STATEMANAGER.SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
		STATEMANAGER.SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
		STATEMANAGER.SetTextureStageState(0, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
		STATEMANAGER.SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
		STATEMANAGER.SetTextureStageState(0, D3DTSS_MIPFILTER, D3DTEXF_LINEAR);

		STATEMANAGER.SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		STATEMANAGER.SetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_CURRENT);
		STATEMANAGER.SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_MODULATE);
		STATEMANAGER.SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
		STATEMANAGER.SetTextureStageState(1, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP);
		STATEMANAGER.SetTextureStageState(1, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP);
	}

	STATEMANAGER.SaveRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	STATEMANAGER.SaveRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	STATEMANAGER.SaveRenderState(D3DRS_CULLMODE, D3DCULL_CW);

	// set up fog if it is enabled
	if (STATEMANAGER.GetRenderState(D3DRS_FOGENABLE))
	{
#if defined WRAPPER_USE_GPU_WIND || defined WRAPPER_USE_GPU_LEAF_PLACEMENT
		STATEMANAGER.SetRenderState(D3DRS_FOGVERTEXMODE, D3DFOG_LINEAR);
#endif
	}

	// choose fixed function pipeline or custom shader for fronds and branches
	STATEMANAGER.SetVertexShader(m_dwBranchVertexShader);

	// render branches
	if (ulRenderBitVector & Forest_RenderBranches)
	{
		itor = m_pMainTreeMap.begin();
		while (itor != m_pMainTreeMap.end())
		{
			const SpeedTreeWrapperPtr& pMainTree = (itor++)->second;
			pMainTree->SetupBranchForTreeType();
			for (const SpeedTreeWrapperWeakPtr& pWeakPtr : pMainTree->GetInstances(uiCount))
			{
				if (pWeakPtr.expired())
					continue;

				const SpeedTreeWrapperPtr& pInstance = pWeakPtr.lock();
				if (pInstance && pInstance->isShow())
					pInstance->RenderBranches();
			}
		}
	}

	// set render states
	STATEMANAGER.SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	if (ulRenderBitVector & Forest_RenderFronds)
	{
		itor = m_pMainTreeMap.begin();
		while (itor != m_pMainTreeMap.end())
		{
			const SpeedTreeWrapperPtr& pMainTree = (itor++)->second;
			pMainTree->SetupFrondForTreeType();
			for (const SpeedTreeWrapperWeakPtr& pWeakPtr : pMainTree->GetInstances(uiCount))
			{
				if (pWeakPtr.expired())
					continue;

				const SpeedTreeWrapperPtr& pInstance = pWeakPtr.lock();
				if (pInstance && pInstance->isShow())
					pInstance->RenderFronds();
			}
		}
	}

	// render leaves
	if (ulRenderBitVector & Forest_RenderLeaves)
	{
		STATEMANAGER.SetVertexShader(m_dwLeafVertexShader);
		if (STATEMANAGER.GetRenderState(D3DRS_FOGENABLE))
		{
#if defined WRAPPER_USE_GPU_WIND || defined WRAPPER_USE_GPU_LEAF_PLACEMENT
			STATEMANAGER.SetRenderState(D3DRS_FOGVERTEXMODE, D3DFOG_LINEAR);
#endif
		}

		if (ulRenderBitVector & Forest_RenderToShadow || ulRenderBitVector & Forest_RenderToMiniMap)
		{
			STATEMANAGER.SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_NOTEQUAL);
			STATEMANAGER.SaveRenderState(D3DRS_ALPHAREF, 0x00000000);
		}

		itor = m_pMainTreeMap.begin();
		while (itor != m_pMainTreeMap.end())
		{
			const SpeedTreeWrapperPtr& pMainTree = (itor++)->second;
			pMainTree->SetupLeafForTreeType();
			for (const SpeedTreeWrapperWeakPtr& pWeakPtr : pMainTree->GetInstances(uiCount))
			{
				if (pWeakPtr.expired())
					continue;

				const SpeedTreeWrapperPtr& pInstance = pWeakPtr.lock();
				if (pInstance && pInstance->isShow())
					pInstance->RenderLeaves();
			}
		}

		while (itor != m_pMainTreeMap.end())
			(itor++)->second->EndLeafForTreeType();

		if (ulRenderBitVector & Forest_RenderToShadow || ulRenderBitVector & Forest_RenderToMiniMap)
		{
			STATEMANAGER.SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
			STATEMANAGER.RestoreRenderState(D3DRS_ALPHAREF);
		}
	}

	// render billboards
#if !defined WRAPPER_NO_BILLBOARD_MODE
	if (ulRenderBitVector & Forest_RenderBillboards)
	{
		STATEMANAGER.SetRenderState(D3DRS_LIGHTING, FALSE);
		STATEMANAGER.SetRenderState(D3DRS_COLORVERTEX, FALSE);

		itor = m_pMainTreeMap.begin();
		while (itor != m_pMainTreeMap.end())
		{
			const SpeedTreeWrapperPtr& pMainTree = (itor++)->second;
			pMainTree->SetupBranchForTreeType();
			for (const SpeedTreeWrapperWeakPtr& pWeakPtr : pMainTree->GetInstances(uiCount))
			{
				if (pWeakPtr.expired())
					continue;

				const SpeedTreeWrapperPtr& pInstance = pWeakPtr.lock();
				if (pInstance && pInstance->isShow())
					pInstance->RenderBillboards();
			}
		}
	}
#endif

	STATEMANAGER.SetRenderState(D3DRS_LIGHTING, dwLightState);
	STATEMANAGER.SetRenderState(D3DRS_COLORVERTEX, dwColorVertexState);
	STATEMANAGER.SetRenderState(D3DRS_FOGVERTEXMODE, dwFogVertexMode);

	// 셀프섀도우로 쓰는 TextureStage 1의 COLOROP와 ALPHAOP를 꺼줘야 다음 렌더링 할 놈들이
	// 제대로 나온다. (안그러면 검게 나올 가능성이..)
	if (!(ulRenderBitVector & Forest_RenderToShadow))
	{
		STATEMANAGER.SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
		STATEMANAGER.SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
	}

	STATEMANAGER.RestoreRenderState(D3DRS_ALPHATESTENABLE);
	STATEMANAGER.RestoreRenderState(D3DRS_ALPHAFUNC);
	STATEMANAGER.RestoreRenderState(D3DRS_CULLMODE);
}

bool CSpeedTreeForestDirectX8::SetRenderingDevice(LPDIRECT3DDEVICE8 lpDevice)
{
	m_pDx = lpDevice;

	if (!__InitVertexShaders())
		return false;

	const float c_afLightPosition[4] = { -0.707f, -0.300f, 0.707f, 0.0f };
	const float	c_afLightAmbient[4] = { 0.5f, 0.5f, 0.5f, 1.0f };
	const float	c_afLightDiffuse[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	const float	c_afLightSpecular[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	float afLight1[] =
	{
		c_afLightPosition[0], c_afLightPosition[1], c_afLightPosition[2],
		c_afLightDiffuse[0], c_afLightDiffuse[1], c_afLightDiffuse[2],
		c_afLightAmbient[0], c_afLightAmbient[1], c_afLightAmbient[2],
		c_afLightSpecular[0], c_afLightSpecular[1], c_afLightSpecular[2],
		c_afLightPosition[3],
		1.0f, 0.0f, 0.0f
	};

	CSpeedTreeRT::SetNumWindMatrices(c_nNumWindMatrices);

	CSpeedTreeRT::SetLightAttributes(0, afLight1);
	CSpeedTreeRT::SetLightState(0, true);
	return true;
}

bool CSpeedTreeForestDirectX8::__InitVertexShaders()
{
	NANOBEGIN;

	// load the vertex shaders
	if (!m_dwBranchVertexShader)
		m_dwBranchVertexShader = LoadBranchShader(m_pDx);

	if (!m_dwLeafVertexShader)
		m_dwLeafVertexShader = LoadLeafShader(m_pDx);

	if (m_dwBranchVertexShader && m_dwLeafVertexShader)
	{
		CSpeedTreeWrapper::SetVertexShaders(m_dwBranchVertexShader, m_dwLeafVertexShader);
		return true;
	}

	NANOEND;
	return true;
}
