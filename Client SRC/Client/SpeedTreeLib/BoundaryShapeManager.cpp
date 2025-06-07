#include "StdAfx.h"

#include "../EterBase/Random.h"
#include "BoundaryShapeManager.h"

CBoundaryShapeManager::CBoundaryShapeManager() = default;
CBoundaryShapeManager::~CBoundaryShapeManager() = default;

bool CBoundaryShapeManager::LoadBsfFile(const char* pszFilename)
{
	bool bSuccess = true;
	try
	{
		FILE* pFile = fopen(pszFilename, "rb");
		if (pFile)
		{
			// number of boundary shapes
			UINT nNumBoundaries;
			if (fread(&nNumBoundaries, sizeof(UINT), 1, pFile) == 1)
			{
				for (UINT i = 0; i < nNumBoundaries && bSuccess; ++i)
				{
					SBoundaryShape sShape;

					// number of contours for this shape
					UINT nNumContours;
					if (fread(&nNumContours, sizeof(UINT), 1, pFile) == 1)
					{
						for (UINT j = 0; j < nNumContours && bSuccess; ++j)
						{
							// number of points in this contour
							std::vector<SPoint> vPoints;
							UINT nNumPoints;
							if (fread(&nNumPoints, sizeof(UINT), 1, pFile) == 1)
							{
								// read the points
								for (UINT k = 0; k < nNumPoints && bSuccess; ++k)
								{
									SPoint sPoint;

									if (fread(sPoint.m_afData, sizeof(float), 3, pFile) == 3)
									{
										vPoints.push_back(sPoint);

										// update extents
										if (j == 0 && k == 0)
										{
											// first point of this shape
											memcpy(sShape.m_afMin, sPoint.m_afData, 3 * sizeof(float));
											memcpy(sShape.m_afMax, sPoint.m_afData, 3 * sizeof(float));
										}
										else
										{
											// check extents
											for (int l = 0; l < 3; ++l)
											{
												if (sPoint.m_afData[l] < sShape.m_afMin[l])
													sShape.m_afMin[l] = sPoint.m_afData[l];
												else if (sPoint.m_afData[l] > sShape.m_afMax[l])
													sShape.m_afMax[l] = sPoint.m_afData[l];
											}
										}
									}
									else
									{
										m_strCurrentError = "Error in CBoundaryShapeManager::LoadBsfFile(): Failed to read point";
										bSuccess = false;
									}
								}
								sShape.m_vContours.push_back(vPoints);
							}
							else
							{
								m_strCurrentError = "Error in CBoundaryShapeManager::LoadBsfFile(): Failed to read number of points";
								bSuccess = false;
							}
						}
						m_vBoundaries.push_back(sShape);
					}
					else
					{
						m_strCurrentError = "Error in CBoundaryShapeManager::LoadBsfFile(): Failed to read number of contours";
						bSuccess = false;
					}
				}
			}
			else
			{
				m_strCurrentError = "Error in CBoundaryShapeManager::LoadBsfFile(): Failed to read number of boundaries";
				bSuccess = false;
			}
			fclose(pFile);
		}
		else
		{
			m_strCurrentError = std::string("Error in CBoundaryShapeManager::LoadBsfFile(): Could not open ") + std::string(pszFilename);
			bSuccess = false;
		}
	}

	catch (...)
	{
		m_strCurrentError = "Error in CBoundaryShapeManager::LoadBsfFile(): Unknown exception";
	}

	return bSuccess;
}

bool CBoundaryShapeManager::PointInside(float fX, float fY)
{
	bool bInside = false;

	for (DWORD i = 0; i < m_vBoundaries.size() && !bInside; ++i)
		bInside = PointInShape(m_vBoundaries[i], fX, fY);

	return bInside;
}

bool CBoundaryShapeManager::PointInShape(SBoundaryShape& sShape, float fX, float fY)
{
	bool bInside = false;

	for (DWORD k = 0; k < sShape.m_vContours.size(); ++k)
	{
		for (DWORD i = 0, j = sShape.m_vContours[k].size() - 1; i < sShape.m_vContours[k].size(); j = i++)
		{
			if ((((sShape.m_vContours[k][i][1] <= fY) && (fY < sShape.m_vContours[k][j][1])) ||
				((sShape.m_vContours[k][j][1] <= fY) && (fY < sShape.m_vContours[k][i][1]))) &&
				(fX < (sShape.m_vContours[k][i][0] - sShape.m_vContours[k][i][0]) * (fY - sShape.m_vContours[k][i][1]) / (sShape.m_vContours[k][j][1] - sShape.m_vContours[k][i][1]) + sShape.m_vContours[k][i][0]))
			{
				bInside = !bInside;
			}
		}
	}

	return bInside;
}

bool CBoundaryShapeManager::RandomPoint(float& fX, float& fY)
{
	bool bSuccess = false;

	if (m_vBoundaries.size() > 0)
	{
		// pick a random boundary shape
		int nIndex = random_range(0, m_vBoundaries.size() - 1);
		SBoundaryShape& sShape = m_vBoundaries[nIndex];

		// pick a point at random within its extents
		fX = frandom(sShape.m_afMin[0], sShape.m_afMax[0]);
		fY = frandom(sShape.m_afMin[1], sShape.m_afMax[1]);

		// try it
		bSuccess = PointInShape(sShape, fX, fY);
	}

	return bSuccess;
}
