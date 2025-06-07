#pragma once

#include <deque>
#include "Thing.h"
#include "ModelInstance.h"

class CGrannyLODController : public CGraphicBase
{
public:
	static void SetMinLODMode(bool isEnable);

public:
	CGrannyLODController();
	virtual ~CGrannyLODController();

	void Clear();

	void CreateDeviceObjects();
	void DestroyDeviceObjects();

	void AddModel(CGraphicThing* pThing, int iSrcModel, CGrannyLODController* pSkelLODController = nullptr);
	void AttachModelInstance(CGrannyLODController* pSrcLODController, const char* c_szBoneName);
	void DetachModelInstance(CGrannyLODController* pSrcLODController);
	void SetLODLimits(float fNearLOD, float fFarLOD);
	void SetLODLevel(BYTE bLODLevel);
	BYTE GetLODLevel() { return m_bLODLevel; }
	void SetMaterialImagePointer(const char* c_szImageName, CGraphicImage* pImage);
	void SetMaterialData(const char* c_szImageName, const SMaterialData& c_rkMaterialData);
	void SetSpecularInfo(const char* c_szMtrlName, BOOL bEnable, float fPower);

	void RenderWithOneTexture();
	void RenderWithTwoTexture();
	void BlendRenderWithOneTexture();
	void BlendRenderWithTwoTexture();

	void Update(float fElapsedTime, float fDistanceFromCenter, float fDistanceFromCamera);
	void UpdateLODLevel(float fDistanceFromCenter, float fDistanceFromCamera);
	void UpdateTime(float fElapsedTime);

	void UpdateSkeleton(const D3DXMATRIX* c_pWorldMatrix, float fElapsedTime);
	void Deform(const D3DXMATRIX* c_pWorldMatrix);
	void DeformNoSkin(const D3DXMATRIX* c_pWorldMatrix);
	void DeformAll(const D3DXMATRIX* c_pWorldMatrix);

	void RenderToShadowMap();
	void RenderShadow();
	void ReloadTexture();

	void GetBoundBox(D3DXVECTOR3* vtMin, D3DXVECTOR3* vtMax);
	bool Intersect(const D3DXMATRIX* c_pMatrix, float* u, float* v, float* t);

	void SetLocalTime(float fLocalTime);
	void ResetLocalTime();

	void SetMotionPointer(const CGrannyMotion* c_pMotion, float fBlendTime, int iLoopCount, float speedRatio);
	void ChangeMotionPointer(const CGrannyMotion* c_pMotion, int iLoopCount, float speedRatio);
	void SetMotionAtEnd();

	BOOL isModelInstance();
	CGrannyModelInstance* GetModelInstance();
	bool HaveBlendThing() { return nullptr != GetModelInstance() ? GetModelInstance()->HaveBlendThing() : false; } // NOTE: GetModelInstance() == 0일 때 클라 크래쉬나는 문제 수정(2012. 05. 07)

protected:
	void SetCurrentModelInstance(CGrannyModelInstance* pgrnModelInstance);
	void RefreshAttachedModelInstance();

	void __ReserveSharedDeformableVertexBuffer(DWORD deformableVertexCount);

protected:
	float m_fLODDistance;
	DWORD m_dwLODAniFPS;

	//// Attaching Link Data
	// Data of Parent Side
	using TAttachingModelData = struct SAttachingModelData
	{
		CGrannyLODController* pkLODController;
		std::string strBoneName;
	};

	using AttachedModelDataVector = std::vector<TAttachingModelData>;
	AttachedModelDataVector m_AttachedModelDataVector;
	// Data of Child Side
	CGrannyLODController* m_pAttachedParentModel;

	BYTE m_bLODLevel;
	CGrannyModelInstance* m_pCurrentModelInstance;

	// WORK
	std::deque<CGrannyModelInstance*> m_que_pkModelInst;

	CGraphicVertexBuffer* m_pkSharedDeformableVertexBuffer;
	// END_OF_WORK
};
