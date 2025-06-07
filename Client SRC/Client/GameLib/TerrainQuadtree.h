// TerrainQuadtreeNode.h: interface for the CTerrainQuadtreeNode class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __INC_TERRAIN_QUADTREE_H__
#define __INC_TERRAIN_QUADTREE_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CTerrainQuadtreeNode
{
public:
	CTerrainQuadtreeNode();
	virtual ~CTerrainQuadtreeNode();

public:
	long x0, y0, x1, y1;
	CTerrainQuadtreeNode* NW_Node;
	CTerrainQuadtreeNode* NE_Node;
	CTerrainQuadtreeNode* SW_Node;
	CTerrainQuadtreeNode* SE_Node;
	long Size;
	long PatchNum;
	D3DXVECTOR3 center;
	float radius;
	BYTE m_byLODLevel;

};

#endif // __INC_TERRAIN_QUADTREE_H__
