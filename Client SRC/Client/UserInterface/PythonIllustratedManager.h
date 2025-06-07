/**
* Filename: PythonIllustratedManager.h
* Author: dotpngr (aka. xP3NG3Rx)
**/

#pragma once

#if defined(ENABLE_MONSTER_CARD)
#include "InstanceBase.h"

class CPythonIllustratedManager : public CSingleton<CPythonIllustratedManager>
{
public:
	CPythonIllustratedManager();
	virtual ~CPythonIllustratedManager();

	void Initialize();
	void Destroy();

	bool CreateBackground(DWORD dwWidth, DWORD dwHeight);
	void RenderBackground() const;

	bool CreateModelInstance(DWORD dwVnum);
	CInstanceBase* GetModelInstance(DWORD dwVnum) const;
	bool SelectModel(DWORD dwVnum);
	void UpdateModel() const;
	void DeformModel() const;
	void RenderModel() const;
	void ModelViewReset();
	void ModelRotation(float fRot) const;
	void ModelZoom(bool bZoom);
	void ModelUpDown(bool bUp);

	void ChangeMotion(DWORD dwVnum) const;
	void SetShow(bool bShow);

private:
	std::unordered_map<DWORD, CInstanceBase*> m_ModelInstanceMap;
	CInstanceBase* m_pModelInstance;
	CGraphicImageInstance* m_pBackgroundImage;
	float m_f8;
	float m_f12;
	float m_f16;
	float m_f20;
	float m_f24;
	bool m_bShow;
};
#endif
