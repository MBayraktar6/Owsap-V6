#pragma once

#include "GrpImage.h"
#include "GrpIndexBuffer.h"
#include "GrpVertexBufferDynamic.h"
#include "Pool.h"
#include "../UserInterface/Locale_inc.h"

class CGraphicImageInstance
{
public:
	static DWORD Type();
	BOOL IsType(DWORD dwType);

public:
	CGraphicImageInstance();
	virtual ~CGraphicImageInstance();

	void Destroy();

	void Render(
#if defined(ENABLE_CLIP_MASK)
		RECT* pMaskRect = NULL
#endif
	);

	void RenderCoolTime(float fCoolTime);

	void SetDiffuseColor(float fr, float fg, float fb, float fa);
	void SetPosition(float fx, float fy);

	virtual void SetScale(float fx, float fy);
	void SetScale(D3DXVECTOR2 v2Scale);
	const D3DXVECTOR2& GetScale() const;

	void SetScalePercent(BYTE byPercent);
	void SetScalePivotCenter(bool bScalePivotCenter);

	void SetImagePointer(CGraphicImage* pImage);
	void ReloadImagePointer(CGraphicImage* pImage);

	bool IsEmpty() const;

	int GetWidth();
	int GetHeight();

	void LeftRightReverse();

	CGraphicTexture* GetTexturePointer();
	const CGraphicTexture& GetTextureReference() const;
	CGraphicImage* GetGraphicImagePointer();

	bool operator == (const CGraphicImageInstance& rhs) const;
	D3DXCOLOR& GetDiffuseColor() { return m_DiffuseColor; };

protected:
	void Initialize();

	virtual void OnRender(
#if defined(ENABLE_CLIP_MASK)
		RECT* pMaskRect
#endif
	);

	virtual void OnSetImagePointer();
	virtual void OnRenderCoolTime(float fCoolTime);

	virtual BOOL OnIsType(DWORD dwType);

protected:
	D3DXCOLOR m_DiffuseColor;
	D3DXVECTOR2 m_v2Position;
	D3DXVECTOR2 m_v2Scale;
	bool m_bScalePivotCenter;

	CGraphicImage::TRef m_roImage;

	bool m_bLeftRightReverse;

public:
	static void CreateSystem(UINT uCapacity);
	static void DestroySystem();

	static CGraphicImageInstance* New();
	static void Delete(CGraphicImageInstance* pkImgInst);

	static CDynamicPool<CGraphicImageInstance> ms_kPool;
};
