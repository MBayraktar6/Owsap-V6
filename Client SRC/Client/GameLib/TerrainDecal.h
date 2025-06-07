// TerrainDecal.h: interface for the CTerrainDecal class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __INC_TERRAIN_DECAL_H__
#define __INC_TERRAIN_DECAL_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../EterLib/Decal.h"

class CMapOutdoor;

class CTerrainDecal : public CDecal
{
public:
	enum
	{
		MAX_SEARCH_VERTICES = 1024,
	};

	CTerrainDecal(CMapOutdoor* pMapOutdoor = NULL);
	virtual ~CTerrainDecal();

	virtual void Make(D3DXVECTOR3 v3Center, D3DXVECTOR3 v3Normal, D3DXVECTOR3 v3Tangent, float fWidth, float fHeight, float fDepth);
	//virtual void Update();
	virtual void Render();

	void SetMapOutdoor(CMapOutdoor* pMapOutdoor) { m_pMapOutdoor = pMapOutdoor; }

protected:

	void SearchAffectedTerrainMesh(float fMinX,
		float fMaxX,
		float fMinY,
		float fMaxY,
		DWORD* pdwAffectedPrimitiveCount,
		D3DXVECTOR3* pv3AffectedVertex,
		D3DXVECTOR3* pv3AffectedNormal);

	CMapOutdoor* m_pMapOutdoor;

};

#endif // __INC_TERRAIN_DECAL_H__
