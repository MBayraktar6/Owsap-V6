/**
* Filename: PythonMyShopDecoManager.h
* Authors: Owsap, blackdragonx61 (aka. Mali)
**/

#pragma once

#if defined(ENABLE_MYSHOP_DECO)
#include "InstanceBase.h"

class CPythonMyShopDecoManager : public CSingleton<CPythonMyShopDecoManager>
{
public:
	CPythonMyShopDecoManager();
	virtual ~CPythonMyShopDecoManager();

	void Initialize();
	void Destroy();

	bool CreateBackground(DWORD dwWidth, DWORD dwHeight);
	void RenderBackground() const;

	bool CreateModelInstance(DWORD dwVnum);
	CInstanceBase* GetModelInstance(DWORD dwVnum) const;
	bool SelectModel(DWORD dwVnum);
	void UpdateModel();
	void DeformModel() const;
	void RenderModel() const;

	void SetShow(bool bShow);

private:
	std::unordered_map<DWORD, CInstanceBase*> m_ModelInstanceMap;
	CInstanceBase* m_pModelInstance;
	CGraphicImageInstance* m_pBackgroundImage;
	bool m_bShow;
	float m_fRot;
};
#endif
