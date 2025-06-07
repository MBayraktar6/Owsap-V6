#include "StdAfx.h"

#if defined(RENDER_TARGET)
#include "../EterBase/Stl.h"

#include "GrpRenderTargetTexture.h"
#include "StateManager.h"

CGraphicRenderTargetTexture::CGraphicRenderTargetTexture()
{
	Initialize();
}

CGraphicRenderTargetTexture::~CGraphicRenderTargetTexture()
{
	Destroy();
}

void CGraphicRenderTargetTexture::Initialize()
{
	CGraphicTexture::Initialize();

	m_lpd3dRenderTexture = nullptr;
	m_lpd3dRenderTargetSurface = nullptr;
	m_lpd3dDepthSurface = nullptr;
	m_lpd3dOriginalRenderTarget = nullptr;
	m_lpd3dOldDepthBufferSurface = nullptr;

	m_d3dFormat = D3DFMT_UNKNOWN;
	m_depthStencilFormat = D3DFMT_UNKNOWN;

	m_renderRect.left = 0;
	m_renderRect.top = 0;
	m_renderRect.right = 0;
	m_renderRect.bottom = 0;
}

void CGraphicRenderTargetTexture::Destroy()
{
	CGraphicTexture::Destroy();
	ReleaseTextures();

	m_d3dFormat = D3DFMT_UNKNOWN;
	m_depthStencilFormat = D3DFMT_UNKNOWN;
}

void CGraphicRenderTargetTexture::ReleaseTextures()
{
	safe_release(m_lpd3dRenderTexture);
	safe_release(m_lpd3dRenderTargetSurface);
	safe_release(m_lpd3dDepthSurface);
	safe_release(m_lpd3dOriginalRenderTarget);
	safe_release(m_lpd3dOldDepthBufferSurface);

	m_renderRect.left = 0;
	m_renderRect.top = 0;
	m_renderRect.right = 0;
	m_renderRect.bottom = 0;
}

bool CGraphicRenderTargetTexture::Create(DWORD width, DWORD height, D3DFORMAT format, D3DFORMAT depthFormat)
{
	Destroy();

	m_height = height;
	m_width = width;

	if (!CreateRenderTexture(width, height, format))
		return false;

	if (!CreateRenderDepthStencil(width, height, depthFormat))
		return false;

	return true;
}

void CGraphicRenderTargetTexture::CreateTextures()
{
	if (CreateRenderTexture(m_width, m_height, m_d3dFormat))
		CreateRenderDepthStencil(m_width, m_height, m_depthStencilFormat);
}

bool CGraphicRenderTargetTexture::CreateRenderTexture(DWORD width, DWORD height, D3DFORMAT format)
{
	m_d3dFormat = format;

	if (FAILED(ms_lpd3dDevice->CreateTexture(width, height, 1, D3DUSAGE_RENDERTARGET, m_d3dFormat, D3DPOOL_DEFAULT, &m_lpd3dRenderTexture)))
		return false;

	if (FAILED(m_lpd3dRenderTexture->GetSurfaceLevel(0, &m_lpd3dRenderTargetSurface)))
		return false;

	return true;
}

bool CGraphicRenderTargetTexture::CreateRenderDepthStencil(DWORD width, DWORD height, D3DFORMAT depthFormat)
{
	m_depthStencilFormat = depthFormat;

	return SUCCEEDED(ms_lpd3dDevice->CreateDepthStencilSurface(width, height, m_depthStencilFormat, D3DMULTISAMPLE_NONE, &m_lpd3dDepthSurface));
}

void CGraphicRenderTargetTexture::SetRenderTarget()
{
	ms_lpd3dDevice->GetRenderTarget(&m_lpd3dOriginalRenderTarget);
	ms_lpd3dDevice->GetDepthStencilSurface(&m_lpd3dOldDepthBufferSurface);
	ms_lpd3dDevice->SetRenderTarget(m_lpd3dRenderTargetSurface, m_lpd3dDepthSurface);
}

void CGraphicRenderTargetTexture::ResetRenderTarget()
{
	ms_lpd3dDevice->SetRenderTarget(m_lpd3dOriginalRenderTarget, m_lpd3dOldDepthBufferSurface);

	safe_release(m_lpd3dOriginalRenderTarget);
	safe_release(m_lpd3dOldDepthBufferSurface);
}

void CGraphicRenderTargetTexture::Clear(D3DCOLOR color)
{
	ms_lpd3dDevice->Clear(0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, color, 1.0f, 0);
}

void CGraphicRenderTargetTexture::SetRenderingRect(RECT* rect)
{
	m_renderRect = *rect;
}

const RECT& CGraphicRenderTargetTexture::GetRenderingRect() const
{
	return m_renderRect;
}

LPDIRECT3DTEXTURE8 CGraphicRenderTargetTexture::GetRenderTargetTexture() const
{
	return m_lpd3dRenderTexture;
}

void CGraphicRenderTargetTexture::Render(
#if defined(ENABLE_CLIP_MASK)
	RECT* pClipRect
#endif
) const
{
#if defined(ENABLE_CLIP_MASK)
	float sx = m_renderRect.left - 0.5f;
	float sy = m_renderRect.top - 0.5f;
	float ex = m_renderRect.right - 0.5f;
	float ey = m_renderRect.bottom - 0.5f;

	float eu = 1.0f;
	float ev = 1.0f;
	float su = 0.0f;
	float sv = 0.0f;

	if (pClipRect)
	{
		const float width = ex - sx;
		const float height = ey - sy;
		const float uDiff = eu - su;
		const float vDiff = ev - sv;

		if (ex < pClipRect->left)
			return;

		if (ey < pClipRect->top)
			return;

		if (sx > pClipRect->right)
			return;

		if (sy > pClipRect->bottom)
			return;

		if (sx < pClipRect->left)
		{
			su += (pClipRect->left - sx) / width * uDiff;
			sx = pClipRect->left;
		}

		if (sy < pClipRect->top)
		{
			sv += (pClipRect->top - sy) / height * vDiff;
			sy = pClipRect->top;
		}

		if (ex > pClipRect->right)
		{
			eu -= (ex - pClipRect->right) / width * uDiff;
			ex = pClipRect->right;
		}

		if (ey > pClipRect->bottom)
		{
			ev -= (ey - pClipRect->bottom) / height * vDiff;
			ey = pClipRect->bottom;
		}
	}

	TPDTVertex vertices[4];

	vertices[0].position = TPosition(sx, sy, 0.0f);
	vertices[0].texCoord = TTextureCoordinate(su, sv);

	vertices[1].position = TPosition(ex, sy, 0.0f);
	vertices[1].texCoord = TTextureCoordinate(eu, sv);

	vertices[2].position = TPosition(sx, ey, 0.0f);
	vertices[2].texCoord = TTextureCoordinate(su, ev);

	vertices[3].position = TPosition(ex, ey, 0.0f);
	vertices[3].texCoord = TTextureCoordinate(eu, ev);
#else
	TPDTVertex vertices[4];

	vertices[0].position = TPosition(m_renderRect.left - 0.5f, m_renderRect.top - 0.5f, 0.0f);
	vertices[0].texCoord = TTextureCoordinate(0.0f, 0.0f);

	vertices[1].position = TPosition(m_renderRect.right - 0.5f, m_renderRect.top - 0.5f, 0.0f);
	vertices[1].texCoord = TTextureCoordinate(1.0f, 0.0f);

	vertices[2].position = TPosition(m_renderRect.left - 0.5f, m_renderRect.bottom - 0.5f, 0.0f);
	vertices[2].texCoord = TTextureCoordinate(0.0f, 1.0f);

	vertices[3].position = TPosition(m_renderRect.right - 0.5f, m_renderRect.bottom - 0.5f, 0.0f);
	vertices[3].texCoord = TTextureCoordinate(1.0f, 1.0f);
#endif

	for (int i = 0; i < 4; i++)
		vertices[i].diffuse = 0xFFFFFFFF;

	if (SetPDTStream(vertices, 4))
	{
		SetDefaultIndexBuffer(DEFAULT_IB_FILL_RECT);

		STATEMANAGER.SetTexture(0, GetRenderTargetTexture());
		STATEMANAGER.SetTexture(1, nullptr);
		STATEMANAGER.SetVertexShader(D3DFVF_XYZ | D3DFVF_TEX1 | D3DFVF_DIFFUSE);
		STATEMANAGER.DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 4, 0, 2);
	}
}
#endif
