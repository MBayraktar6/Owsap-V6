#include "StdAfx.h"

#if defined(RENDER_TARGET)
#include "../EterBase/Stl.h"
#include "RenderTargetManager.h"

CRenderTargetManager::CRenderTargetManager()
{
	Initialize();
}

CRenderTargetManager::~CRenderTargetManager()
{
	Destroy();
}

void CRenderTargetManager::Initialize()
{
	m_dwWidth = 800;
	m_dwHeight = 600;
	m_pRenderTargetTexture = nullptr;
}

void CRenderTargetManager::Destroy()
{
	stl_wipe_second(m_RenderTargetMap);

	m_dwWidth = 800;
	m_dwHeight = 600;
	m_pRenderTargetTexture = nullptr;
}

void CRenderTargetManager::ClearRenderTarget(D3DCOLOR color)
{
	if (!m_pRenderTargetTexture)
		return;

	m_pRenderTargetTexture->Clear(color);
}

void CRenderTargetManager::ResetRenderTarget()
{
	if (!m_pRenderTargetTexture)
		return;

	m_pRenderTargetTexture->ResetRenderTarget();
	m_pRenderTargetTexture = nullptr;
}

void CRenderTargetManager::CreateRenderTargetTextures()
{
	for (const auto& elem : m_RenderTargetMap)
		elem.second->CreateTextures();
}

void CRenderTargetManager::ReleaseRenderTargetTextures()
{
	for (const auto& elem : m_RenderTargetMap)
		elem.second->ReleaseTextures();
}

bool CRenderTargetManager::CreateX8R8G8B8Texture(DWORD width, DWORD height)
{
	m_dwWidth = width;
	m_dwHeight = height;
	return __CreateRenderTargetTexture(RENDER_TARGET_INDEX_ILLUSTRATED, width, height, D3DFMT_X8R8G8B8, D3DFMT_D16);
}

bool CRenderTargetManager::CreateA8R8G8B8Texture(DWORD width, DWORD height, int index)
{
	m_dwWidth = width;
	m_dwHeight = height;
	return __CreateRenderTargetTexture(index, width, height, D3DFMT_A8R8G8B8, D3DFMT_D16);
}

bool CRenderTargetManager::__CreateRenderTargetTexture(int index, DWORD width, DWORD height, D3DFORMAT format, D3DFORMAT depthFormat)
{
	if (index >= RENDER_TARGET_INDEX_MAX || GetRenderTargetTexture(index))
		return false;

	CGraphicRenderTargetTexture* pRenderTargetTexture = new CGraphicRenderTargetTexture();
	if (!pRenderTargetTexture->Create(width, height, format, depthFormat))
	{
		delete pRenderTargetTexture;
		return false;
	}

	m_RenderTargetMap.emplace(index, pRenderTargetTexture);
	return true;
}

CGraphicRenderTargetTexture* CRenderTargetManager::GetRenderTargetTexture(int index) const
{
	const auto it = m_RenderTargetMap.find(index);
	if (it != m_RenderTargetMap.end())
		return it->second;

	return nullptr;
}

bool CRenderTargetManager::GetRenderTargetRect(int index, RECT* rect) const
{
	CGraphicRenderTargetTexture* pRenderTargetTexture = GetRenderTargetTexture(index);
	if (!pRenderTargetTexture)
		return false;

	*rect = pRenderTargetTexture->GetRenderingRect();
	return true;
}

bool CRenderTargetManager::ChangeRenderTarget(int index)
{
	m_pRenderTargetTexture = GetRenderTargetTexture(index);
	if (!m_pRenderTargetTexture)
		return false;

	m_pRenderTargetTexture->SetRenderTarget();
	return true;
}
#endif
