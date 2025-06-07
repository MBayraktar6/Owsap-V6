#include "StdAfx.h"
#include "ModelInstance.h"
#include "Model.h"

void CGrannyModelInstance::Clear()
{
	m_kMtrlPal.Clear();

	DestroyDeviceObjects();
	// WORK
	__DestroyMeshBindingVector();
	// END_OF_WORK
	__DestroyMeshMatrices();
	__DestroyModelInstance();
	__DestroyWorldPose();

	__Initialize();
}

void CGrannyModelInstance::SetMainModelPointer(CGrannyModel* pModel, CGraphicVertexBuffer* pkSharedDeformableVertexBuffer)
{
	SetLinkedModelPointer(pModel, pkSharedDeformableVertexBuffer, nullptr);
}

void CGrannyModelInstance::SetLinkedModelPointer(CGrannyModel* pkModel, CGraphicVertexBuffer* pkSharedDeformableVertexBuffer, CGrannyModelInstance** ppkSkeletonInst)
{
	Clear();

	if (m_pModel)
		m_pModel->Release();

	m_pModel = pkModel;
	m_pModel->AddReference();

	if (pkSharedDeformableVertexBuffer)
		__SetSharedDeformableVertexBuffer(pkSharedDeformableVertexBuffer);
	else
		__CreateDynamicVertexBuffer();

	__CreateModelInstance();

	// WORK
	if (ppkSkeletonInst && *ppkSkeletonInst)
	{
		m_ppkSkeletonInst = ppkSkeletonInst;
		__CreateWorldPose(*ppkSkeletonInst);
		__CreateMeshBindingVector(*ppkSkeletonInst);
	}
	else
	{
		__CreateWorldPose(nullptr);
		__CreateMeshBindingVector(nullptr);
	}
	// END_OF_WORK

	__CreateMeshMatrices();

	ResetLocalTime();

	m_kMtrlPal.Copy(pkModel->GetMaterialPalette());
}

// WORK
granny_world_pose* CGrannyModelInstance::__GetWorldPosePtr() const
{
	if (m_pgrnWorldPoseReal)
		return m_pgrnWorldPoseReal;

	if (m_ppkSkeletonInst && *m_ppkSkeletonInst)
		return (*m_ppkSkeletonInst)->m_pgrnWorldPoseReal;

	assert(m_ppkSkeletonInst != nullptr && "__GetWorldPosePtr - NO HAVE SKELETON");
	return nullptr;
}

const granny_int32x* CGrannyModelInstance::__GetMeshBoneIndices(unsigned int iMeshBinding) const
{
	assert(iMeshBinding < m_vct_pgrnMeshBinding.size());
	return GrannyGetMeshBindingToBoneIndices(m_vct_pgrnMeshBinding[iMeshBinding]);
}

bool CGrannyModelInstance::__CreateMeshBindingVector(CGrannyModelInstance* pkDstModelInst)
{
	assert(m_vct_pgrnMeshBinding.empty());

	if (!m_pModel)
		return false;

	granny_model* pgrnModel = m_pModel->GetGrannyModelPointer();
	if (!pgrnModel)
		return false;

	granny_skeleton* pgrnDstSkeleton = pgrnModel->Skeleton;
	if (pkDstModelInst && pkDstModelInst->m_pModel && pkDstModelInst->m_pModel->GetGrannyModelPointer())
		pgrnDstSkeleton = pkDstModelInst->m_pModel->GetGrannyModelPointer()->Skeleton;

	m_vct_pgrnMeshBinding.reserve(pgrnModel->MeshBindingCount);

	for (granny_int32 iMeshBinding = 0; iMeshBinding != pgrnModel->MeshBindingCount; ++iMeshBinding)
		m_vct_pgrnMeshBinding.emplace_back(GrannyNewMeshBinding(pgrnModel->MeshBindings[iMeshBinding].Mesh, pgrnModel->Skeleton, pgrnDstSkeleton));

	return true;
}

void CGrannyModelInstance::__DestroyMeshBindingVector()
{
	std::for_each(m_vct_pgrnMeshBinding.begin(), m_vct_pgrnMeshBinding.end(), GrannyFreeMeshBinding);
	m_vct_pgrnMeshBinding.clear();
}

// END_OF_WORK

void CGrannyModelInstance::__CreateWorldPose(CGrannyModelInstance* pkSkeletonInst)
{
	assert(m_pgrnModelInstance != nullptr);
	assert(m_pgrnWorldPoseReal == nullptr);

	// WORK
	if (pkSkeletonInst)
		return;
	// END_OF_WORK

	granny_skeleton* pgrnSkeleton = GrannyGetSourceSkeleton(m_pgrnModelInstance);

	// WORK
	m_pgrnWorldPoseReal = GrannyNewWorldPose(pgrnSkeleton->BoneCount);
	// END_OF_WORK
}

void CGrannyModelInstance::__DestroyWorldPose()
{
	if (!m_pgrnWorldPoseReal)
		return;

	GrannyFreeWorldPose(m_pgrnWorldPoseReal);
	m_pgrnWorldPoseReal = nullptr;
}

void CGrannyModelInstance::__CreateModelInstance()
{
	assert(m_pModel != nullptr);
	assert(m_pgrnModelInstance == nullptr);

	const granny_model* pgrnModel = m_pModel->GetGrannyModelPointer();
	m_pgrnModelInstance = GrannyInstantiateModel(pgrnModel);
}

void CGrannyModelInstance::__DestroyModelInstance()
{
	if (!m_pgrnModelInstance)
		return;

	GrannyFreeModelInstance(m_pgrnModelInstance);
	m_pgrnModelInstance = nullptr;
}

void CGrannyModelInstance::__CreateMeshMatrices()
{
	assert(m_pModel != nullptr);

	if (m_pModel->GetMeshCount() <= 0) // �޽��� ���� (ī�޶� ����) �𵨵� ��Ȥ �ִ�..
		return;

	int meshCount = m_pModel->GetMeshCount();
	m_meshMatrices.resize(meshCount);
}

void CGrannyModelInstance::__DestroyMeshMatrices()
{
	m_meshMatrices.clear();
}

DWORD CGrannyModelInstance::GetDeformableVertexCount()
{
	if (!m_pModel)
		return 0;

	return m_pModel->GetDeformVertexCount();
}

DWORD CGrannyModelInstance::GetVertexCount()
{
	if (!m_pModel)
		return 0;

	return m_pModel->GetVertexCount();
}

// WORK

void CGrannyModelInstance::__SetSharedDeformableVertexBuffer(CGraphicVertexBuffer* pkSharedDeformableVertexBuffer)
{
	m_pkSharedDeformableVertexBuffer = pkSharedDeformableVertexBuffer;
}

bool CGrannyModelInstance::__IsDeformableVertexBuffer()
{
	if (m_pkSharedDeformableVertexBuffer)
		return true;

	return m_kLocalDeformableVertexBuffer.IsEmpty();
}

IDirect3DVertexBuffer8* CGrannyModelInstance::__GetDeformableD3DVertexBufferPtr()
{
	return __GetDeformableVertexBufferRef().GetD3DVertexBuffer();
}

CGraphicVertexBuffer& CGrannyModelInstance::__GetDeformableVertexBufferRef()
{
	if (m_pkSharedDeformableVertexBuffer)
		return *m_pkSharedDeformableVertexBuffer;

	return m_kLocalDeformableVertexBuffer;
}

void CGrannyModelInstance::__CreateDynamicVertexBuffer()
{
	assert(m_pModel != nullptr);
	assert(m_kLocalDeformableVertexBuffer.IsEmpty());

	int vtxCount = m_pModel->GetDeformVertexCount();

	if (0 != vtxCount)
	{
		if (!m_kLocalDeformableVertexBuffer.Create(vtxCount,
			D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1,
			//D3DUSAGE_DYNAMIC, D3DPOOL_SYSTEMMEM

			D3DUSAGE_WRITEONLY, D3DPOOL_MANAGED
		))
			return;
	}
}

void CGrannyModelInstance::__DestroyDynamicVertexBuffer()
{
	m_kLocalDeformableVertexBuffer.Destroy();
	m_pkSharedDeformableVertexBuffer = nullptr;
}

// END_OF_WORK

bool CGrannyModelInstance::GetBoneIndexByName(const char* c_szBoneName, int* pBoneIndex) const
{
	assert(m_pgrnModelInstance != nullptr);

	granny_skeleton* pgrnSkeleton = GrannyGetSourceSkeleton(m_pgrnModelInstance);
	return GrannyFindBoneByName(pgrnSkeleton, c_szBoneName, pBoneIndex);
}

float* CGrannyModelInstance::GetBoneMatrixPointer(int iBone) const
{
	float* pgrnMatBone = GrannyGetWorldPose4x4(__GetWorldPosePtr(), iBone);
	if (!pgrnMatBone)
	{
		//TraceError("GrannyModelInstance(%s).GetBoneMatrixPointer(boneIndex(%d)).NOT_FOUND_BONE", m_pModel->GetGrannyModelPointer()->Name, iBone);
		return nullptr;
	}
	return pgrnMatBone;
}

float* CGrannyModelInstance::GetCompositeBoneMatrixPointer(int iBone) const
{
	// NOTE : GrannyGetWorldPose4x4�� ������ ������ �߸����� �� ����.. �׷��ϰ� �ӵ��� ����
	// GrannyGetWorldPose4x4�� ��� matrix ���Ҹ� �� ������ ���� ����
	return GrannyGetWorldPoseComposite4x4(__GetWorldPosePtr(), iBone);
}

void CGrannyModelInstance::ReloadTexture()
{
	assert("Not currently in use - CGrannyModelInstance::ReloadTexture()");
}
