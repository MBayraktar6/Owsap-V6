#include "StdAfx.h"
#include "../EterBase/Debug.h"
#include "ModelInstance.h"
#include "Model.h"

void CGrannyModelInstance::Update(DWORD dwAniFPS)
{
	if (!dwAniFPS)
		return;

	const DWORD c_dwCurUpdateFrame = static_cast<DWORD>(GetLocalTime() * ANIFPS_MAX);
	const DWORD ANIFPS_STEP = ANIFPS_MAX / dwAniFPS;
	if (c_dwCurUpdateFrame > ANIFPS_STEP && c_dwCurUpdateFrame / ANIFPS_STEP == m_dwOldUpdateFrame / ANIFPS_STEP)
		return;

	m_dwOldUpdateFrame = c_dwCurUpdateFrame;

	GrannyFreeCompletedModelControls(m_pgrnModelInstance);
#ifdef __PERFORMANCE_CHECKER__
	DWORD dwTime1 = timeGetTime();
#endif
	GrannySetModelClock(m_pgrnModelInstance, GetLocalTime());
#ifdef __PERFORMANCE_CHECKER__
	DWORD dwTime99 = timeGetTime();
	{
		static FILE* s_pFile = fopen("perf_grn_setmodelclock.txt", "w");
		if (dwTime99 - dwTime1 > 3)
		{
			fprintf(s_pFile, "%f:%x:- GrannySetModelClock(time=%f) = %dms\n", timeGetTime() / 1000.0f, this, GetLocalTime(), dwTime99 - dwTime1);
			fflush(s_pFile);
		}
	}
#endif
}

void CGrannyModelInstance::UpdateLocalTime(float fElapsedTime)
{
	m_fSecondsElapsed = fElapsedTime;
	m_fLocalTime += fElapsedTime;
}

void CGrannyModelInstance::UpdateTransform(D3DXMATRIX* pMatrix, float fSecondsElapsed)
{
	if (!m_pgrnModelInstance)
	{
		TraceError("CGrannyModelIstance::UpdateTransform - m_pgrnModelInstance = nullptr");
		return;
	}
	GrannyUpdateModelMatrix(m_pgrnModelInstance, fSecondsElapsed, reinterpret_cast<const float*>(pMatrix), reinterpret_cast<float*>(pMatrix), false);
	//Tracef("%f %f %f", pMatrix->_41, pMatrix->_42, pMatrix->_43);
}

void CGrannyModelInstance::Deform(const D3DXMATRIX* c_pWorldMatrix)
{
	if (IsEmpty())
		return;

	// DELETED
	//m_pgrnWorldPose = m_pgrnWorldPoseReal;
	/////////////////////////////////////////////

	UpdateWorldPose();
	UpdateWorldMatrices(c_pWorldMatrix);

	if (m_pModel->CanDeformPNTVertices())
	{
		// WORK
		CGraphicVertexBuffer& rkDeformableVertexBuffer = __GetDeformableVertexBufferRef();
		TPNTVertex* pntVertices;
		if (rkDeformableVertexBuffer.LockRange(m_pModel->GetDeformVertexCount(), reinterpret_cast<void**>(&pntVertices)))
		{
			DeformPNTVertices(pntVertices);
			rkDeformableVertexBuffer.Unlock();
		}
		else
		{
			TraceError("GRANNY DEFORM DYNAMIC BUFFER LOCK ERROR");
		}
		// END_OF_WORK
	}
}

//////////////////////////////////////////////////////
class CGrannyLocalPose
{
public:
	CGrannyLocalPose()
	{
		m_pgrnLocalPose = nullptr;
		m_boneCount = 0;
	}

	virtual ~CGrannyLocalPose()
	{
		if (m_pgrnLocalPose)
			GrannyFreeLocalPose(m_pgrnLocalPose);
	}

	granny_local_pose* Get(int boneCount)
	{
		if (m_pgrnLocalPose)
		{
			if (m_boneCount >= boneCount)
				return m_pgrnLocalPose;

			GrannyFreeLocalPose(m_pgrnLocalPose);
		}

		m_boneCount = boneCount;
		m_pgrnLocalPose = GrannyNewLocalPose(m_boneCount);
		return m_pgrnLocalPose;
	}

private:
	granny_local_pose* m_pgrnLocalPose;
	int m_boneCount;

};
//////////////////////////////////////////////////////

void CGrannyModelInstance::UpdateSkeleton(const D3DXMATRIX* c_pWorldMatrix, float /*fLocalTime*/)
{
	// DELETED
	// m_pgrnWorldPose = m_pgrnWorldPoseReal;
	///////////////////////////////////////////
	UpdateWorldPose();
	UpdateWorldMatrices(c_pWorldMatrix);
}

void CGrannyModelInstance::UpdateWorldPose()
{
	// WEP = m_iParentBoneIndex != 0 -> UpdateWorldPose(O)
	// LOD = UpdateWorldPose(O)
	// Hair = UpdateWorldPose(X)

	if (m_ppkSkeletonInst)
		if (*m_ppkSkeletonInst != this)
			return;

	static CGrannyLocalPose s_SharedLocalPose;

	granny_skeleton* pgrnSkeleton = GrannyGetSourceSkeleton(m_pgrnModelInstance);
	granny_local_pose* pgrnLocalPose = s_SharedLocalPose.Get(pgrnSkeleton->BoneCount);

	const float* pAttachBoneMatrix = (mc_pParentInstance) ? mc_pParentInstance->GetBoneMatrixPointer(m_iParentBoneIndex) : nullptr;

	GrannySampleModelAnimationsAccelerated(m_pgrnModelInstance, pgrnSkeleton->BoneCount, pAttachBoneMatrix, pgrnLocalPose, __GetWorldPosePtr());
	/*
	GrannySampleModelAnimations(m_pgrnModelInstance, 0, pgrnSkeleton->BoneCount, pgrnLocalPose);
	GrannyBuildWorldPose(pgrnSkeleton, 0, pgrnSkeleton->BoneCount, pgrnLocalPose, pAttachBoneMatrix, m_pgrnWorldPose);
	*/
	GrannyFreeCompletedModelControls(m_pgrnModelInstance);
}

void CGrannyModelInstance::UpdateWorldMatrices(const D3DXMATRIX* c_pWorldMatrix)
{
	// NO_MESH_BUG_FIX
	if (m_meshMatrices.empty())
		return;
	// END_OF_NO_MESH_BUG_FIX

	assert(m_pModel != nullptr);
	assert(ms_lpd3dMatStack != nullptr);

	int meshCount = m_pModel->GetMeshCount();

	granny_matrix_4x4* pgrnMatCompositeBuffer = GrannyGetWorldPoseComposite4x4Array(__GetWorldPosePtr());
	D3DXMATRIX* boneMatrices = reinterpret_cast<D3DXMATRIX*>(pgrnMatCompositeBuffer);

	for (int i = 0; i < meshCount; ++i)
	{
		D3DXMATRIX& rWorldMatrix = m_meshMatrices[i];

		const CGrannyMesh* pMesh = m_pModel->GetMeshPointer(i);

		const granny_int32x* boneIndices = __GetMeshBoneIndices(i);

		if (pMesh->CanDeformPNTVertices())
		{
			rWorldMatrix = *c_pWorldMatrix;
		}
		else
		{
			const granny_int32x iBone = *boneIndices;
			D3DXMatrixMultiply(&rWorldMatrix, &boneMatrices[iBone], c_pWorldMatrix);
		}
	}
}

void CGrannyModelInstance::DeformPNTVertices(void* pvDest)
{
	assert(m_pModel != nullptr);
	assert(m_pModel->CanDeformPNTVertices());

	m_pModel->DeformPNTVertices(pvDest, reinterpret_cast<D3DXMATRIX*>(GrannyGetWorldPoseComposite4x4Array(__GetWorldPosePtr())),
		m_vct_pgrnMeshBinding);
}
