#pragma once

#include "../EterLib/GrpObjectInstance.h"
#include "../EterLib/Pool.h"
#include "../MilesLib/Type.h"

#include "EffectElementBaseInstance.h"
#include "EffectData.h"
#include "EffectMeshInstance.h"
#include "ParticleSystemInstance.h"
#include "SimpleLightInstance.h"

class CEffectInstance : public CGraphicObjectInstance
{
public:
	typedef std::vector<CEffectElementBaseInstance*> TEffectElementInstanceVector;

	enum
	{
		ID = EFFECT_OBJECT
	};

	int GetType() const
	{
		return CEffectInstance::ID;
	}

	bool GetBoundingSphere(D3DXVECTOR3& v3Center, float& fRadius);

	static void DestroySystem();

	static CEffectInstance* New();
	static void Delete(CEffectInstance* pkEftInst);

	static void ResetRenderingEffectCount();
	static int GetRenderingEffectCount();

public:
	CEffectInstance();
	virtual ~CEffectInstance();

	bool LessRenderOrder(CEffectInstance* pkEftInst);

	void SetEffectDataPointer(CEffectData* pEffectData);

	void Clear();
	BOOL isAlive();
	void SetActive();
	void SetDeactive();
	void SetGlobalMatrix(const D3DXMATRIX& c_rmatGlobal);
	void UpdateSound();
	void OnUpdate();
	void OnRender();
	void OnBlendRender() {} // Not used
	void OnRenderToShadowMap() {} // Not used
	void OnRenderShadow() {} // Not used
	void OnRenderPCBlocker() {} // Not used

protected:
	void __Initialize();

	void __SetParticleData(CParticleSystemData* pData);
	void __SetMeshData(CEffectMeshScript* pMesh);
	void __SetLightData(CLightData* pData);

	virtual void OnUpdateCollisionData(const CStaticCollisionDataVector* pscdVector) {} // Not used
	virtual void OnUpdateHeighInstance(CAttributeInstance* pAttributeInstance) {}
	virtual bool OnGetObjectHeight(float fX, float fY, float* pfHeight) { return false; }

protected:
	BOOL m_isAlive;
	DWORD m_dwFrame;
	D3DXMATRIX m_matGlobal;

	CEffectData* m_pkEftData;

	std::vector<CParticleSystemInstance*> m_ParticleInstanceVector;
	std::vector<CEffectMeshInstance*> m_MeshInstanceVector;
	std::vector<CLightInstance*> m_LightInstanceVector;

	NSound::TSoundInstanceVector* m_pSoundInstanceVector;

	float m_fBoundingSphereRadius;
	D3DXVECTOR3 m_v3BoundingSpherePosition;

	float m_fLastTime;

public:
	static CDynamicPool<CEffectInstance> ms_kPool;
	static int ms_iRenderingEffectCount;

#if defined(ENABLE_GRAPHIC_ON_OFF)
public:
	void SetID(DWORD dwID);
	DWORD GetID() const;
	void SetRenderState(bool bCanRender);
	bool CanRender() const;

protected:
	DWORD m_dwID;
	bool m_bCanRender;
#endif

protected:
	float m_fParticleScale;
	D3DXVECTOR3 m_v3MeshScale;

public:
	void SetParticleScale(float fParticleScale) { m_fParticleScale = fParticleScale; }
	float GetParticleScale() { return m_fParticleScale; }

	void SetMeshScale(D3DXVECTOR3 rv3MeshScale) { m_v3MeshScale = rv3MeshScale; }
	D3DXVECTOR3 GetMeshScale() { return m_v3MeshScale; }

	const D3DXMATRIX& GetGlobalMatrix() const { return m_matGlobal; };
};
