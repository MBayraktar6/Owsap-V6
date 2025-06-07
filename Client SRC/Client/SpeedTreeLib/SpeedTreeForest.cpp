#include "StdAfx.h"

#include "../EterBase/Filename.h"
#include "../EterBase/MappedFile.h"
#include "../EterPack/EterPackManager.h"

#include "SpeedTreeForest.h"
#include "SpeedTreeConfig.h"

CSpeedTreeForest::CSpeedTreeForest() : m_fWindStrength(0.0f)
{
	CSpeedTreeRT::SetNumWindMatrices(c_nNumWindMatrices);

	m_afForestExtents[0] = m_afForestExtents[1] = m_afForestExtents[2] = FLT_MAX;
	m_afForestExtents[3] = m_afForestExtents[4] = m_afForestExtents[5] = -FLT_MAX;
}

CSpeedTreeForest::~CSpeedTreeForest()
{
	Clear();
}

void CSpeedTreeForest::Clear()
{
	if (m_pMainTreeMap.begin() != m_pMainTreeMap.end())
		m_pMainTreeMap.clear();
}

CSpeedTreeForest::SpeedTreeWrapperPtr CSpeedTreeForest::GetMainTree(DWORD dwCRC)
{
	TreeMap::iterator itor = m_pMainTreeMap.find(dwCRC);
	if (itor == m_pMainTreeMap.end())
		return nullptr;

	return itor->second;
}

BOOL CSpeedTreeForest::GetMainTree(DWORD dwCRC, SpeedTreeWrapperPtr& ppMainTree, const char* c_pszFileName)
{
	TreeMap::iterator itor = m_pMainTreeMap.find(dwCRC);
	SpeedTreeWrapperPtr pTree = nullptr;
	if (itor != m_pMainTreeMap.end())
		pTree = itor->second;
	else
	{
		CMappedFile file;
		LPCVOID c_pvData;

		if (!CEterPackManager::Instance().Get(file, c_pszFileName, &c_pvData))
			return FALSE;

		pTree = std::make_shared<CSpeedTreeWrapper>();
		if (!pTree->LoadTree(c_pszFileName, (const BYTE*)c_pvData, file.Size()))
		{
			pTree.reset();
			return FALSE;
		}

		m_pMainTreeMap.emplace(dwCRC, pTree);

		file.Destroy();
	}

	ppMainTree = pTree;
	return TRUE;
}

CSpeedTreeForest::SpeedTreeWrapperPtr CSpeedTreeForest::CreateInstance(float x, float y, float z, DWORD dwTreeCRC, const char* c_szTreeName)
{
	SpeedTreeWrapperPtr pMainTree;
	if (!GetMainTree(dwTreeCRC, pMainTree, c_szTreeName))
		return nullptr;

	SpeedTreeWrapperPtr pTreeInst = pMainTree->MakeInstance();
	pTreeInst->SetPosition(x, y, z);
	pTreeInst->RegisterBoundingSphere();
	return pTreeInst;
}

void CSpeedTreeForest::DeleteInstance(SpeedTreeWrapperPtr pInstance)
{
	if (!pInstance)
		return;

	SpeedTreeWrapperPtr pParentTree = pInstance->InstanceOf();
	if (!pParentTree)
		return;

	pParentTree->DeleteInstance(pInstance);
}

void CSpeedTreeForest::UpdateSystem(float fCurrentTime)
{
	// 업데이트 할 때 한번
	static float fLastTime = fCurrentTime;
	float fElapsedTime = fCurrentTime - fLastTime;
	CSpeedTreeRT::SetTime(fElapsedTime);

	m_fAccumTime += fElapsedTime;
	SetupWindMatrices(m_fAccumTime);
}

void CSpeedTreeForest::AdjustExtents(float x, float y, float z)
{
	// min
	m_afForestExtents[0] = __min(m_afForestExtents[0], x);
	m_afForestExtents[1] = __min(m_afForestExtents[1], y);
	m_afForestExtents[2] = __min(m_afForestExtents[2], z);

	// max
	m_afForestExtents[3] = __max(m_afForestExtents[3], x);
	m_afForestExtents[4] = __max(m_afForestExtents[4], y);
	m_afForestExtents[5] = __max(m_afForestExtents[5], z);
}

void CSpeedTreeForest::SetWindStrength(float fStrength)
{
	if (m_fWindStrength == fStrength)
		return;

	m_fWindStrength = fStrength;

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
			if (pInstance && pInstance->isShow())
				pInstance->SetWindStrength(m_fWindStrength);
		}
	}
}

void CSpeedTreeForest::SetupWindMatrices(float fTimeInSecs)
{
	// matrix computational data
	static float afMatrixTimes[c_nNumWindMatrices] = { 0.0f };
	static float afFrequencies[c_nNumWindMatrices][2] =
	{
		{ 0.15f, 0.17f },
		{ 0.25f, 0.15f },
		{ 0.19f, 0.05f },
		{ 0.15f, 0.22f }
	};

	// compute time since last call
	static float fTimeOfLastCall = 0.0f;
	float fTimeSinceLastCall = fTimeInSecs - fTimeOfLastCall;
	fTimeOfLastCall = fTimeInSecs;

	// wind strength
	static float fOldStrength = m_fWindStrength;

	// increment matrix times
	for (int i = 0; i < c_nNumWindMatrices; ++i)
		afMatrixTimes[i] += fTimeSinceLastCall;

	// compute maximum branch throw
	float fBaseAngle = m_fWindStrength * 35.0f;

	// build rotation matrices
	for (int j = 0; j < c_nNumWindMatrices; ++j)
	{
		// adjust time to prevent "jumping"
		if (m_fWindStrength != 0.0f)
			afMatrixTimes[j] = (afMatrixTimes[j] * fOldStrength) / m_fWindStrength;

		// compute percentages for each axis
		float fBaseFreq = m_fWindStrength * 20.0f;
		float fXPercent = sinf(fBaseFreq * afFrequencies[j % c_nNumWindMatrices][0] * afMatrixTimes[j]);
		float fYPercent = cosf(fBaseFreq * afFrequencies[j % c_nNumWindMatrices][1] * afMatrixTimes[j]);

		// build compound rotation matrix (rotate on 'x' then on 'y')
		const float c_fDeg2Rad = 57.2957795f;
		float fSinX = sinf(fBaseAngle * fXPercent / c_fDeg2Rad);
		float fSinY = sinf(fBaseAngle * fYPercent / c_fDeg2Rad);
		float fCosX = cosf(fBaseAngle * fXPercent / c_fDeg2Rad);
		float fCosY = cosf(fBaseAngle * fYPercent / c_fDeg2Rad);

		float afMatrix[16] = { 0.0f };
		afMatrix[0] = fCosY;
		afMatrix[2] = -fSinY;
		afMatrix[4] = fSinX * fSinY;
		afMatrix[5] = fCosX;
		afMatrix[6] = fSinX * fCosY;
		afMatrix[8] = fSinY * fCosX;
		afMatrix[9] = -fSinX;
		afMatrix[10] = fCosX * fCosY;
		afMatrix[15] = 1.0f;

#ifdef WRAPPER_USE_CPU_WIND
		CSpeedTreeRT::SetWindMatrix(j, afMatrix);
#endif

#ifdef WRAPPER_USE_GPU_WIND
		// graphics API specific
		UploadWindMatrix(c_nVertexShader_WindMatrices + j * 4, afMatrix);
#endif
	}

	// track wind strength
	fOldStrength = m_fWindStrength;
}

void CSpeedTreeForest::SetLight(const float* afDirection, const float* afAmbient, const float* afDiffuse)
{
	m_afLighting[0] = afDirection[0];
	m_afLighting[1] = afDirection[1];
	m_afLighting[2] = afDirection[2];
	m_afLighting[3] = 1.0f;

	m_afLighting[4] = afAmbient[0];
	m_afLighting[5] = afAmbient[1];
	m_afLighting[6] = afAmbient[2];
	m_afLighting[7] = afAmbient[3];

	m_afLighting[8] = afDiffuse[0];
	m_afLighting[9] = afDiffuse[1];
	m_afLighting[10] = afDiffuse[2];
	m_afLighting[11] = afDiffuse[3];
}

void CSpeedTreeForest::SetFog(float fFogNear, float fFogFar)
{
	const float c_fFogLinearScale = (1.0f / (fFogFar - fFogNear));

	m_afFog[0] = fFogNear;
	m_afFog[1] = fFogFar;
	m_afFog[2] = c_fFogLinearScale;
	m_afFog[3] = 0.0f;
}
