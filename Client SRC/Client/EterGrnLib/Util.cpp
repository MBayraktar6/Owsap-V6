#include "StdAfx.h"
#include "Util.h"

bool GrannyMeshIsDeform(granny_mesh* pgrnMesh)
{
	return !GrannyMeshIsRigid(pgrnMesh);
}
