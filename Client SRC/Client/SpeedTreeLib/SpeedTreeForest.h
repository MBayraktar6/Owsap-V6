#pragma once

#include <SpeedTreeRT.h>
#include "SpeedTreeWrapper.h"

#define Forest_RenderBranches (1 << 0)
#define Forest_RenderLeaves (1 << 1)
#define Forest_RenderFronds (1 << 2)
#define Forest_RenderBillboards (1 << 3)
#define Forest_RenderAll ((1 << 4) - 1)
#define Forest_RenderToShadow (1 << 5)
#define Forest_RenderToMiniMap (1 << 6)

class CSpeedTreeForest
{
public:
	using SpeedTreeWrapperPtr = std::shared_ptr<CSpeedTreeWrapper>;
	using SpeedTreeWrapperWeakPtr = std::weak_ptr<CSpeedTreeWrapper>;
	using TreeMap = std::map<DWORD, SpeedTreeWrapperPtr>;

public:
	CSpeedTreeForest();
	virtual ~CSpeedTreeForest();

	BOOL GetMainTree(DWORD dwCRC, SpeedTreeWrapperPtr& ppMainTree, const char* c_pszFileName);
	SpeedTreeWrapperPtr GetMainTree(DWORD dwCRC);

	SpeedTreeWrapperPtr CreateInstance(float x, float y, float z, DWORD dwTreeCRC, const char* c_pszTreeName);
	void DeleteInstance(SpeedTreeWrapperPtr pTree);

	void UpdateSystem(float fCurrentTime);

	void Clear();

	void SetLight(const float* afDirection, const float* afAmbient, const float* afDiffuse);
	void SetFog(float fFogNear, float fFogFar);
	//////////////////////////////////////////////////////////////////////////

	const float* GetExtents(void) const { return m_afForestExtents; }

	// wind management
	float GetWindStrength(void) const { return m_fWindStrength; }
	void SetWindStrength(float fStrength);
	void SetupWindMatrices(float fTimeInSecs);

	// overridden by specific graphics API
	virtual void UploadWindMatrix(UINT uiLocation, const float* pMatrix) const = 0;
	virtual	void UpdateCompundMatrix(const D3DXVECTOR3& c_rEyeVec, const D3DXMATRIX& c_rmatView, const D3DXMATRIX& c_rmatProj) = 0;
	virtual void Render(unsigned long ulRenderBitVector = Forest_RenderAll) = 0;
	virtual bool SetRenderingDevice(LPDIRECT3DDEVICE8 pDevice) = 0;

protected:
	TreeMap m_pMainTreeMap;

	float m_afLighting[12];
	float m_afFog[4];

private:
	void AdjustExtents(float x, float y, float z);

	float m_afForestExtents[6]; // [0] = min x, [1] = min y..., [3] = max x, [4] = max y...
	float m_fWindStrength; // 0.0 = no wind, 1.0 = full strength

	float m_fAccumTime;
};
