#include "StdAfx.h"
#include "GrpMarkInstance.h"
#include "StateManager.h"
#include "ResourceManager.h"

#include "../EterBase/CRC32.h"

CDynamicPool<CGraphicMarkInstance> CGraphicMarkInstance::ms_kPool;

void CGraphicMarkInstance::SetImageFileName(const char* c_szFileName)
{
	m_stImageFileName = c_szFileName;
}

const std::string& CGraphicMarkInstance::GetImageFileName()
{
	return m_stImageFileName;
}

void CGraphicMarkInstance::CreateSystem(UINT uCapacity)
{
	ms_kPool.Create(uCapacity);
}

void CGraphicMarkInstance::DestroySystem()
{
	ms_kPool.Destroy();
}

CGraphicMarkInstance* CGraphicMarkInstance::New()
{
	return ms_kPool.Alloc();
}

void CGraphicMarkInstance::Delete(CGraphicMarkInstance* pkImgInst)
{
	pkImgInst->Destroy();
	ms_kPool.Free(pkImgInst);
}

void CGraphicMarkInstance::Render(
#if defined(ENABLE_CLIP_MASK)
	RECT* pMaskRect
#endif
)
{
	if (IsEmpty())
		return;

	assert(!IsEmpty());

#if defined(ENABLE_CLIP_MASK)
	OnRender(pMaskRect);
#else
	OnRender();
#endif
}

void CGraphicMarkInstance::OnRender(
#if defined(ENABLE_CLIP_MASK)
	RECT* pMaskRect
#endif
)
{
	CGraphicImage* pImage = m_roImage.GetPointer();
	CGraphicTexture* pTexture = pImage->GetTexturePointer();

	UINT uColCount = pImage->GetWidth() / MARK_WIDTH;

	if (uColCount == 0)
		return;

	UINT uCol = m_uIndex % uColCount;
	UINT uRow = m_uIndex / uColCount;

	RECT kRect;
	kRect.left = uCol * MARK_WIDTH;
	kRect.top = uRow * MARK_HEIGHT;
	kRect.right = kRect.left + MARK_WIDTH;
	kRect.bottom = kRect.top + MARK_HEIGHT;

	float texReverseWidth = 1.0f / float(pTexture->GetWidth());
	float texReverseHeight = 1.0f / float(pTexture->GetHeight());
	float su = kRect.left * texReverseWidth;
	float sv = kRect.top * texReverseHeight;
	float eu = kRect.right * texReverseWidth;
	float ev = kRect.bottom * texReverseHeight;

	float fRenderWidth = static_cast<float>(MARK_WIDTH) * m_fScale;
	float fRenderHeight = static_cast<float>(MARK_HEIGHT) * m_fScale;

#if defined(ENABLE_CLIP_MASK)
	float v1 = m_v2Position.x - 0.5f;
	float v2 = m_v2Position.y - 0.5f;
	float v3 = m_v2Position.x + fRenderWidth - 0.5f;
	float v4 = m_v2Position.y + fRenderHeight - 0.5f;

	if (pMaskRect)
	{
		const float v5 = v3 - v1;
		const float v6 = v4 - v2;
		const float v7 = eu - su;
		const float v8 = ev - sv;

		if (v3 < pMaskRect->left)
			return;

		if (v1 < pMaskRect->left)
		{
			const float fCal = pMaskRect->left - v1;
			su += fCal / v5 * v7;
			v1 += fCal;
		}

		if (v4 < pMaskRect->top)
			return;

		if (v2 < pMaskRect->top)
		{
			const float fCal = pMaskRect->top - v2;
			sv += fCal / v6 * v8;
			v2 += fCal;
		}

		if (v1 > pMaskRect->right)
			return;

		if (v1 < pMaskRect->right && v3 > pMaskRect->right)
		{
			const float fCal = v3 - pMaskRect->right;
			v3 -= fCal;
			eu -= fCal / v5 * v7;
		}

		if (v2 > pMaskRect->bottom)
			return;

		if (v2 < pMaskRect->bottom && v4 > pMaskRect->bottom)
		{
			const float fCal = v4 - pMaskRect->bottom;
			ev -= v8 * fCal / v6;
			v4 -= fCal;
		}
	}

	TPDTVertex vertices[4];
	vertices[0].position.x = v1;
	vertices[0].position.y = v2;
	vertices[0].position.z = 0.0f;
	vertices[0].texCoord = TTextureCoordinate(su, sv);
	vertices[0].diffuse = m_DiffuseColor;

	vertices[1].position.x = v3;
	vertices[1].position.y = v2;
	vertices[1].position.z = 0.0f;
	vertices[1].texCoord = TTextureCoordinate(eu, sv);
	vertices[1].diffuse = m_DiffuseColor;

	vertices[2].position.x = v1;
	vertices[2].position.y = v4;
	vertices[2].position.z = 0.0f;
	vertices[2].texCoord = TTextureCoordinate(su, ev);
	vertices[2].diffuse = m_DiffuseColor;

	vertices[3].position.x = v3;
	vertices[3].position.y = v4;
	vertices[3].position.z = 0.0f;
	vertices[3].texCoord = TTextureCoordinate(eu, ev);
	vertices[3].diffuse = m_DiffuseColor;
#else
	TPDTVertex vertices[4];
	vertices[0].position.x = m_v2Position.x - 0.5f;
	vertices[0].position.y = m_v2Position.y - 0.5f;
	vertices[0].position.z = 0.0f;
	vertices[0].texCoord = TTextureCoordinate(su, sv);
	vertices[0].diffuse = m_DiffuseColor;

	vertices[1].position.x = m_v2Position.x + fRenderWidth - 0.5f;
	vertices[1].position.y = m_v2Position.y - 0.5f;
	vertices[1].position.z = 0.0f;
	vertices[1].texCoord = TTextureCoordinate(eu, sv);
	vertices[1].diffuse = m_DiffuseColor;

	vertices[2].position.x = m_v2Position.x - 0.5f;
	vertices[2].position.y = m_v2Position.y + fRenderHeight - 0.5f;
	vertices[2].position.z = 0.0f;
	vertices[2].texCoord = TTextureCoordinate(su, ev);
	vertices[2].diffuse = m_DiffuseColor;

	vertices[3].position.x = m_v2Position.x + fRenderWidth - 0.5f;
	vertices[3].position.y = m_v2Position.y + fRenderHeight - 0.5f;
	vertices[3].position.z = 0.0f;
	vertices[3].texCoord = TTextureCoordinate(eu, ev);
	vertices[3].diffuse = m_DiffuseColor;
#endif

	if (CGraphicBase::SetPDTStream(vertices, 4))
	{
		CGraphicBase::SetDefaultIndexBuffer(CGraphicBase::DEFAULT_IB_FILL_RECT);

		STATEMANAGER.SetTexture(0, pTexture->GetD3DTexture());
		STATEMANAGER.SetTexture(1, NULL);
		STATEMANAGER.SetVertexShader(D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1);
		STATEMANAGER.DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 4, 0, 2);
		// OLD: STATEMANAGER.DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, 4, 2, c_FillRectIndices, D3DFMT_INDEX16, vertices, sizeof(TPDTVertex));
	}
}

const CGraphicTexture& CGraphicMarkInstance::GetTextureReference() const
{
	return m_roImage->GetTextureReference();
}

CGraphicTexture* CGraphicMarkInstance::GetTexturePointer()
{
	return m_roImage->GetTexturePointer();
}

CGraphicImage* CGraphicMarkInstance::GetGraphicImagePointer()
{
	return m_roImage.GetPointer();
}

void CGraphicMarkInstance::SetScale(float fScale)
{
	m_fScale = fScale;
}

void CGraphicMarkInstance::SetIndex(UINT uIndex)
{
	m_uIndex = uIndex;
}

int CGraphicMarkInstance::GetWidth()
{
	if (IsEmpty())
		return 0;

	//return m_roImage->GetWidth();
	return 16;
}

int CGraphicMarkInstance::GetHeight()
{
	if (IsEmpty())
		return 0;

	//return m_roImage->GetHeight();
	return 12;
}

void CGraphicMarkInstance::SetDiffuseColor(float fr, float fg, float fb, float fa)
{
	m_DiffuseColor.r = fr;
	m_DiffuseColor.g = fg;
	m_DiffuseColor.b = fb;
	m_DiffuseColor.a = fa;
}

void CGraphicMarkInstance::SetPosition(float fx, float fy)
{
	m_v2Position.x = fx;
	m_v2Position.y = fy;
}

void CGraphicMarkInstance::Load()
{
	if (GetImageFileName().empty())
		return;

	CResource* pResource = CResourceManager::Instance().GetResourcePointer(GetImageFileName().c_str());

	if (!pResource)
	{
		TraceError("CGraphicMarkinstance::Load - [%s] NOT EXIST", GetImageFileName().c_str());
		return;
	}

	if (pResource->IsType(CGraphicImage::Type()))
		SetImagePointer(static_cast<CGraphicImage*>(pResource));
}

void CGraphicMarkInstance::SetImagePointer(CGraphicImage* pImage)
{
	m_roImage.SetPointer(pImage);

	OnSetImagePointer();
}

bool CGraphicMarkInstance::IsEmpty() const
{
	if (!m_roImage.IsNull() && !m_roImage->IsEmpty())
		return false;

	return true;
}

bool CGraphicMarkInstance::operator == (const CGraphicMarkInstance& rhs) const
{
	return (m_roImage.GetPointer() == rhs.m_roImage.GetPointer());
}

DWORD CGraphicMarkInstance::Type()
{
	static DWORD s_dwType = GetCRC32("CGraphicMarkInstance", strlen("CGraphicMarkInstance"));
	return (s_dwType);
}

BOOL CGraphicMarkInstance::IsType(DWORD dwType)
{
	return OnIsType(dwType);
}

BOOL CGraphicMarkInstance::OnIsType(DWORD dwType)
{
	if (CGraphicMarkInstance::Type() == dwType)
		return true;

	return false;
}

void CGraphicMarkInstance::OnSetImagePointer()
{
}

void CGraphicMarkInstance::Initialize()
{
	m_DiffuseColor.r = m_DiffuseColor.g = m_DiffuseColor.b = m_DiffuseColor.a = 1.0f;
	m_v2Position.x = m_v2Position.y = 0.0f;
	m_uIndex = 0;
	m_fScale = 1.0f;
}

void CGraphicMarkInstance::Destroy()
{
	m_roImage.SetPointer(NULL); // CRef 에서 레퍼런스 카운트가 떨어져야 함.
	Initialize();
}

CGraphicMarkInstance::CGraphicMarkInstance()
{
	Initialize();
}

CGraphicMarkInstance::~CGraphicMarkInstance()
{
	Destroy();
}
