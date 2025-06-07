/**
* Filename: PythonYutnoriManager.h
* Author: blackdragonx61 (aka. Mali)
**/

#pragma once

#if defined(ENABLE_MINI_GAME_YUTNORI)
#include "InstanceBase.h"

class CPythonYutnoriManager : public CSingleton<CPythonYutnoriManager>
{
public:
	CPythonYutnoriManager();
	virtual ~CPythonYutnoriManager();

	void Initialize();
	void Destroy();

	bool Create(DWORD dwWidth, DWORD dwHeight);
	void Reset() const;

	bool CreateModelInstance();
	void SelectModel() const;
	void UpdateModel();
	void DeformModel() const;
	void RenderModel() const;

	bool ChangeMotion(DWORD dwMotionIndex);
	void NotifyMotionDone() const;
	void SetShow(bool bShow);

private:
	std::vector<DWORD> m_MotionVec;
	CInstanceBase* m_pModelInstance;
	bool m_bShow;
	bool m_bMotionProcess;
	bool m_bMotionDoneProcess;
	float m_fAlpha;
	float m_fColorAlpha;
	D3DXCOLOR m_dColor;
};
#endif
