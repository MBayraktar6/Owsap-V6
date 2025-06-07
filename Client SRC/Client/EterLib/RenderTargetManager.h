#pragma once

#if defined(RENDER_TARGET)
#include "../EterBase/Singleton.h"
#include "GrpRenderTargetTexture.h"

#include <unordered_map>

class CRenderTargetManager : public CSingleton<CRenderTargetManager>
{
public:
	CRenderTargetManager();
	virtual ~CRenderTargetManager();

	enum ERENDERTARGETINDEX
	{
//#if defined(ENABLE_MONSTER_CARD)
		RENDER_TARGET_INDEX_ILLUSTRATED,
//#endif
#if defined(ENABLE_MYSHOP_DECO)
		RENDER_TARGET_INDEX_MYSHOPDECO,
#endif
#if defined(ENABLE_MINI_GAME_YUTNORI)
		RENDER_TARGET_INDEX_YUTNORI,
#endif

		RENDER_TARGET_INDEX_MAX
	};

	void Initialize();
	void Destroy();

	void ClearRenderTarget(D3DCOLOR color = 0);
	void ResetRenderTarget();

	void CreateRenderTargetTextures();
	void ReleaseRenderTargetTextures();

	bool CreateX8R8G8B8Texture(DWORD width, DWORD height);
	bool CreateA8R8G8B8Texture(DWORD width, DWORD height, int index);

	CGraphicRenderTargetTexture* GetRenderTargetTexture(int index) const;
	bool GetRenderTargetRect(int index, RECT* rect) const;

	bool ChangeRenderTarget(int index);

protected:
	bool __CreateRenderTargetTexture(int index, DWORD width, DWORD height, D3DFORMAT format, D3DFORMAT depthFormat);

private:
	std::unordered_map<int, CGraphicRenderTargetTexture*> m_RenderTargetMap;
	DWORD m_dwWidth;
	DWORD m_dwHeight;
	CGraphicRenderTargetTexture* m_pRenderTargetTexture;
};
#endif
