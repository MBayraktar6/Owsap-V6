#pragma once

#include "SpeedTreeMaterial.h"
#include "SpeedTreeConfig.h"
#include <SpeedTreeRT.h>

#include "../EterLib/GrpObjectInstance.h"
#include "../EterLib/GrpImageInstance.h"
#include "../EterLib/GrpCollisionObject.h"

#ifndef SAFE_DELETE
#	define SAFE_DELETE(p) { if (p) { delete (p); (p) = nullptr; } }
#endif

#ifndef SAFE_DELETE_ARRAY
#	define SAFE_DELETE_ARRAY(p) { if (p) { delete[] (p); (p) = nullptr; } }
#endif

#ifndef SAFE_RELEASE
#	define SAFE_RELEASE(p) { if (p) { (p)->Release(); (p) = nullptr; } }
#endif

class CSpeedTreeWrapper : public CGraphicObjectInstance, public std::enable_shared_from_this<CSpeedTreeWrapper>
{
public:
	using SpeedTreeWrapperPtr = std::shared_ptr<CSpeedTreeWrapper>;
	using SpeedTreeWrapperWeakPtr = std::weak_ptr<CSpeedTreeWrapper>;

	enum
	{
		ID = TREE_OBJECT
	};

	int GetType() const { return ID; }

	// Collision Data
public:
	UINT GetCollisionObjectCount();
	void GetCollisionObject(UINT nIndex, CSpeedTreeRT::ECollisionObjectType& eType, float* pPosition, float* pDimensions);

protected:
	virtual void OnUpdateCollisionData(const CStaticCollisionDataVector* pscdVector);
	virtual void OnUpdateHeighInstance(CAttributeInstance* pAttributeInstance) {}
	virtual bool OnGetObjectHeight(float fX, float fY, float* pfHeight) { return false; }

	// Bounding Sphere
public:
	virtual bool GetBoundingSphere(D3DXVECTOR3& v3Center, float& fRadius);

public:
	static bool ms_bSelfShadowOn;

public:
	// methods from CGraphicObjectInstance
	virtual void SetPosition(float x, float y, float z);
	virtual void CalculateBBox();

	/*
	* Render 시에 메소드, 그러나 프리뷰나 특수한 경우에만 직접 Render 콜을 부르며
	* 그 이외에는 RenderBranches, RenderFronds 등의 메소드를 CSpeedTreeForest에서 호출한다.
	*/
	virtual void OnRender();

	virtual void OnBlendRender() {}
	virtual void OnRenderToShadowMap() {}
	virtual void OnRenderShadow() {}
	virtual void OnRenderPCBlocker();

public:
	CSpeedTreeWrapper();
	virtual ~CSpeedTreeWrapper();

	const float* GetPosition();
	static void SetVertexShaders(DWORD dwBranchVertexShader, DWORD dwLeafVertexShader);

	// geometry
	bool LoadTree(const char* pszSptFile, const BYTE* c_pbBlock = nullptr, UINT uiBlockSize = 0, UINT nSeed = 1, float fSize = -1.0f, float fSizeVariance = -1.0f);
	void GetTreeSize(float& r_fSize, float& r_fVariance);
	const float* GetBoundingBox() const { return m_afBoundingBox; }

	// rendering
	void SetupBranchForTreeType() const;
	void SetupFrondForTreeType() const;
	void SetupLeafForTreeType() const;
	void EndLeafForTreeType();

#ifdef WRAPPER_USE_GPU_LEAF_PLACEMENT
	void UploadLeafTables(UINT uiLocation) const;
#endif

	void RenderBranches() const;
	void RenderFronds() const;
	void RenderLeaves() const;
	void RenderBillboards() const;

	// instancing
	std::list<SpeedTreeWrapperWeakPtr> GetInstances(UINT& nCount);
	SpeedTreeWrapperPtr InstanceOf() const { return m_pInstanceOf; }
	SpeedTreeWrapperPtr MakeInstance();
	void DeleteInstance(SpeedTreeWrapperPtr pInstance);
	CSpeedTreeRT* GetSpeedTree() const { return m_pSpeedTree; }

	// lighting
	const CSpeedTreeMaterial& GetBranchMaterial() const { return m_cBranchMaterial; }
	const CSpeedTreeMaterial& GetFrondMaterial() const { return m_cFrondMaterial; }
	const CSpeedTreeMaterial& GetLeafMaterial() const { return m_cLeafMaterial; }
	float GetLeafLightingAdjustment() const { return m_pSpeedTree->GetLeafLightingAdjustment(); }

	// wind
	void SetWindStrength(float fStrength) { m_pSpeedTree->SetWindStrength(fStrength); }
	void Advance();

	// utility
	LPDIRECT3DTEXTURE8 GetBranchTexture(void) const;
	void CleanUpMemory(void);

private:
	void SetupBuffers();
	void SetupBranchBuffers();
	void SetupFrondBuffers();
	void SetupLeafBuffers();
	void PositionTree() const;
	static bool LoadTexture(const char* pFilename, CGraphicImageInstance& rImage);
	void SetShaderConstants(const float* pMaterial) const;

private:
	// SpeedTreeRT data
	CSpeedTreeRT* m_pSpeedTree; // the SpeedTree object
	CSpeedTreeRT::STextures* m_pTextureInfo; // texture info cache
	bool m_bIsInstance; // is this an instance?
	std::vector<SpeedTreeWrapperPtr> m_vInstances; // what is an instance of us
	SpeedTreeWrapperPtr m_pInstanceOf; // which tree is this an instance of

	// geometry cache
	CSpeedTreeRT::SGeometry* m_pGeometryCache; // cache for pulling geometry from SpeedTree avoids lots of reallocation

	// branch buffers
	LPDIRECT3DVERTEXBUFFER8 m_pBranchVertexBuffer; // branch vertex buffer
	UINT m_unBranchVertexCount; // number of vertices in branches
	LPDIRECT3DINDEXBUFFER8 m_pBranchIndexBuffer; // branch index buffer
	unsigned short* m_pBranchIndexCounts; // number of indexes per branch LOD level

	// frond buffers
	LPDIRECT3DVERTEXBUFFER8 m_pFrondVertexBuffer; // frond vertex buffer
	unsigned int m_unFrondVertexCount; // number of vertices in frond
	LPDIRECT3DINDEXBUFFER8 m_pFrondIndexBuffer; // frond index buffer
	unsigned short* m_pFrondIndexCounts; // number of indexes per frond LOD level

	// leaf buffers
	unsigned short m_usNumLeafLods; // the number of leaf LODs
	LPDIRECT3DVERTEXBUFFER8* m_pLeafVertexBuffer; // leaf vertex buffer
	bool* m_pLeavesUpdatedByCpu; // stores which LOD's have been updated already per frame

	// tree properties
	float m_afPos[3]; // tree position
	float m_afBoundingBox[6]; // tree bounding box

	// materials
	CSpeedTreeMaterial m_cBranchMaterial; // branch material
	CSpeedTreeMaterial m_cLeafMaterial; // leaf material
	CSpeedTreeMaterial m_cFrondMaterial; // frond material

	// branch texture
	CGraphicImageInstance m_BranchImageInstance;
	CGraphicImageInstance m_ShadowImageInstance; // shadow texture object (used if shadows are enabled)
	CGraphicImageInstance m_CompositeImageInstance;

	static DWORD ms_dwBranchVertexShader;
	static DWORD ms_dwLeafVertexShader;
};
