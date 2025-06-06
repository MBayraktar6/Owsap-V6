#pragma once

#include "../EterBase/Stl.h"
#include "../EterLib/GrpObjectInstance.h"
#include "../EterLib/GrpShadowTexture.h"

#include "LODController.h"

constexpr int DONTUSEVALUE = -1;

class CGraphicThingInstance : public CGraphicObjectInstance
{
public:
	using TModelThingSet = struct SModelThingSet
	{
		void Clear() { m_pLODThingRefVector.clear(); }
		std::vector<std::unique_ptr<CGraphicThing::TRef>> m_pLODThingRefVector;
	};

public:
	enum EType
	{
		ID = THING_OBJECT
	};
	int GetType() const { return ID; }

	CGraphicThingInstance();
	virtual ~CGraphicThingInstance();

	void DeformNoSkin();

	void UpdateLODLevel();
	void UpdateTime();
	void DeformAll(); // ��� LOD ����

	bool LessRenderOrder(CGraphicThingInstance* pkThingInst);

	bool Picking(const D3DXVECTOR3& v, const D3DXVECTOR3& dir, float& out_x, float& out_y);

	void OnInitialize();

	bool CreateDeviceObjects();
	void DestroyDeviceObjects();

	void ReserveModelInstance(int iCount);
	void ReserveModelThing(int iCount);

	bool CheckModelInstanceIndex(int iModelInstance);
	bool CheckModelThingIndex(int iModelThing);
	bool CheckMotionThingIndex(DWORD dwMotionKey);
	bool GetMotionThingPointer(DWORD dwKey, CGraphicThing** ppMotion);
	bool IsMotionThing();

	void RegisterModelThing(int iModelThing, CGraphicThing* pModelThing);
	void RegisterLODThing(int iModelThing, CGraphicThing* pModelThing);
	void RegisterMotionThing(DWORD dwMotionKey, CGraphicThing* pMotionThing);

	bool SetModelInstance(int iDstModelInstance, int iSrcModelThing, int iSrcModel, int iSkelInstance = DONTUSEVALUE);
	void SetEndStopMotion();
	void SetMotionAtEnd();

	void AttachModelInstance(int iDstModelInstance, const char* c_szBoneName, int iSrcModelInstance);
	void AttachModelInstance(int iDstModelInstance, const char* c_szBoneName, CGraphicThingInstance& rsrcInstance, int iSrcModelInstance);
	void DetachModelInstance(int iDstModelInstance, CGraphicThingInstance& rSrcInstance, int SrcModelInstance);

	bool FindBoneIndex(int iModelInstance, const char* c_szBoneName, int* iRetBone);
	bool GetBonePosition(int iModelIndex, int iBoneIndex, float* pfx, float* pfy, float* pfz);

	void ResetLocalTime();
	void InsertDelay(float fDelay);

	void SetMaterialImagePointer(UINT ePart, const char* c_szImageName, CGraphicImage* pImage);
	void SetMaterialData(UINT ePart, const char* c_szImageName, SMaterialData kMaterialData);
	void SetSpecularInfo(UINT ePart, const char* c_szMtrlName, BOOL bEnable, float fPower);

	void __SetLocalTime(float fLocalTime); // Only Used by Tools
	float GetLastLocalTime();
	float GetLocalTime();
	float GetSecondElapsed();
	float GetAverageSecondElapsed();

	BYTE GetLODLevel(DWORD dwModelInstance);
	float GetHeight(bool bScaleX = false);

	void RenderWithOneTexture();
	void RenderWithTwoTexture();
	void BlendRenderWithOneTexture();
	void BlendRenderWithTwoTexture();

	DWORD GetLODControllerCount() const;
	CGrannyLODController* GetLODControllerPointer(DWORD dwModelIndex) const;
	CGrannyLODController* GetLODControllerPointer(DWORD dwModelIndex);

	void ReloadTexture();

#if defined(ENABLE_ACCE_COSTUME_SYSTEM)
	void RecalcAccePositionMatrixFromBoneMatrix();
#endif

public:
	CGraphicThing* GetBaseThingPtr();

	bool SetMotion(DWORD dwMotionKey, float blendTime = 0.0f, int loopCount = 0, float speedRatio = 1.0f);
	bool ChangeMotion(DWORD dwMotionKey, int loopCount = 0, float speedRatio = 1.0f);
	bool Intersect(float* pu, float* pv, float* pt);
	void GetBoundBox(D3DXVECTOR3* vtMin, D3DXVECTOR3* vtMax);
	BOOL GetBoundBox(DWORD dwModelInstanceIndex, D3DXVECTOR3* vtMin, D3DXVECTOR3* vtMax);
	BOOL GetBoneMatrix(DWORD dwModelInstanceIndex, DWORD dwBoneIndex, D3DXMATRIX** ppMatrix);
	BOOL GetCompositeBoneMatrix(DWORD dwModelInstanceIndex, DWORD dwBoneIndex, D3DXMATRIX** ppMatrix);
	void UpdateTransform(D3DXMATRIX* pMatrix, float fSecondsElapsed = 0.0f, int iModelInstanceIndex = 0);

public:
	void BuildBoundingSphere();
	void BuildBoundingAABB();

	virtual void CalculateBBox();
	virtual bool GetBoundingSphere(D3DXVECTOR3& v3Center, float& fRadius);
	virtual bool GetBoundingAABB(D3DXVECTOR3& v3Min, D3DXVECTOR3& v3Max);

protected:
	void OnClear();
	void OnDeform();
	void OnUpdate();
	void OnRender();
	void OnBlendRender();
	void OnRenderToShadowMap();
	void OnRenderShadow();
	void OnRenderPCBlocker();

protected:
	bool m_bUpdated;
	float m_fLastLocalTime;
	float m_fLocalTime;
	float m_fDelay;
	float m_fSecondElapsed;
	float m_fAverageSecondElapsed;
	float m_fRadius;

	D3DXVECTOR3 m_v3Center;
	D3DXVECTOR3 m_v3Min, m_v3Max;

	using LODControllerVector = std::vector<std::unique_ptr<CGrannyLODController>>;
	LODControllerVector m_LODControllerVector;

	using ModelSetThingVector = std::vector<TModelThingSet>;
	ModelSetThingVector m_modelThingSetVector;

	using MotionThingMap = std::map<DWORD, std::unique_ptr<CGraphicThing::TRef>>;
	MotionThingMap m_roMotionThingMap;

protected:
	virtual void OnUpdateCollisionData(const CStaticCollisionDataVector* pscdVector);
	virtual void OnUpdateHeighInstance(CAttributeInstance* pAttributeInstance);
	virtual bool OnGetObjectHeight(float fX, float fY, float* pfHeight);

public:
	static void CreateSystem(UINT uCapacity);
	static void DestroySystem();

	static CGraphicThingInstance* New();
	static void Delete(CGraphicThingInstance* pkInst);

	static CDynamicPool<CGraphicThingInstance> ms_kPool;

	bool HaveBlendThing();
};
