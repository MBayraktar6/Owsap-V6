/**
* Filename: PythonCubeManager.cpp
* Author: Owsap
**/

#include "StdAfx.h"

#if defined(ENABLE_CUBE_RENEWAL)
#include "PythonApplication.h"
#include "PythonCubeManager.h"

#include "../EterPack/EterPackManager.h"

#include <cryptopp/crc.h>
#include <cryptopp/files.h>
#include <cryptopp/filters.h>

CPythonCubeManager::CPythonCubeManager()
{
	m_vCubeData.clear();
	m_dwFileCrc = 0;
}

CPythonCubeManager::~CPythonCubeManager()
{
	m_vCubeData.clear();
	m_dwFileCrc = 0;
}

bool CPythonCubeManager::LoadCubeTable(const char* szFileName)
{
	if (!m_vCubeData.empty())
		return true;

	LPCVOID pData;
	CMappedFile File;

	if (!CEterPackManager::Instance().Get(File, szFileName, &pData))
		return false;

	m_dwFileCrc = GetCRC32(reinterpret_cast<const char*>(pData), File.Size());

	CMemoryTextFileLoader TextFileLoader;
	TextFileLoader.Bind(File.Size(), pData);

	CubeDataPtr pCubeData = nullptr;
	SCubeValue CubeValue = { 0, 0 };
	UINT iCubeIndex = 0;

	CTokenVector TokenVector;
	for (DWORD dwLine = 0; dwLine < TextFileLoader.GetLineCount(); ++dwLine)
	{
		if (!TextFileLoader.SplitLine(dwLine, &TokenVector))
			continue;

		stl_lowers(TokenVector[0]);
		if (0 == TokenVector[0].compare("section"))
		{
			pCubeData = std::make_unique<SCubeData>();
		}
		else if (0 == TokenVector[0].compare("npc"))
		{
			pCubeData->dwNPCVnum = atol(TokenVector[1].c_str());
		}
		else if (0 == TokenVector[0].compare("item"))
		{
			CubeValue.dwVnum = atol(TokenVector[1].c_str());
			CubeValue.iCount = atol(TokenVector[2].c_str());
			pCubeData->vItem.emplace_back(CubeValue);
		}
		else if (0 == TokenVector[0].compare("reward"))
		{
			CubeValue.dwVnum = atol(TokenVector[1].c_str());
			CubeValue.iCount = atol(TokenVector[2].c_str());
			pCubeData->Reward = CubeValue;
		}
		else if (0 == TokenVector[0].compare("percent"))
		{
			pCubeData->iPercent = atoi(TokenVector[1].c_str());
		}
		else if (0 == TokenVector[0].compare("gold"))
		{
			pCubeData->iGold = atoi(TokenVector[1].c_str());
		}
		else if (0 == TokenVector[0].compare("gem"))
		{
			pCubeData->iGem = atoi(TokenVector[1].c_str());
		}
		else if (0 == TokenVector[0].compare("category"))
		{
			pCubeData->iCategory = GetCubeCategory(TokenVector[1]);
		}
		else if (0 == TokenVector[0].compare("subcategory"))
		{
			pCubeData->iSubCategory = atoi(TokenVector[1].c_str());
		}
#if defined(ENABLE_SET_ITEM)
		else if (0 == TokenVector[0].compare("set_value"))
		{
			pCubeData->iSetValue = atoi(TokenVector[1].c_str());
		}
		else if (0 == TokenVector[0].compare("not_remove"))
		{
			pCubeData->dwNotRemove = atoi(TokenVector[1].c_str());
		}
#endif
		else if (0 == TokenVector[0].compare("end"))
		{
			if (pCubeData && CheckCubeData(pCubeData))
			{
				pCubeData->iIndex = ++iCubeIndex;
				m_vCubeData.emplace_back(std::move(pCubeData));
			}
			else
			{
				pCubeData.reset();
				continue;
			}
		}
	}

	return true;
}

bool CPythonCubeManager::CheckCubeData(const CubeDataPtr& pkCubeData)
{
	std::size_t dwIndex = 0;
	std::size_t dwEndIndex = 0;

	if (pkCubeData->dwNPCVnum == 0)
		return false;

	dwEndIndex = pkCubeData->vItem.size();
	for (dwIndex = 0; dwIndex < dwEndIndex; ++dwIndex)
	{
		if (pkCubeData->vItem[dwIndex].dwVnum == 0)
			return false;

		if (pkCubeData->vItem[dwIndex].iCount == 0)
			return false;
	}

	if (pkCubeData->Reward.dwVnum == 0)
		return false;

	if (pkCubeData->Reward.iCount == 0)
		return false;

	return true;
}

INT CPythonCubeManager::GetCubeCategory(const std::string& rkCategory) const
{
	CategoryNameMap::const_iterator it = g_map_CubeCategoryName.find(rkCategory);
	return (it != g_map_CubeCategoryName.end() ? it->second : -1);
}

bool CPythonCubeManager::IsCubeSetAddCategory(const BYTE bCategory) const
{
	switch (bCategory)
	{
		case CUBE_SETADD_WEAPON:
		case CUBE_SETADD_ARMOR_BODY:
		case CUBE_SETADD_ARMOR_HELMET:
			return true;
	}
	return false;
}

CPythonCubeManager::CubeDataVector CPythonCubeManager::GetCubeData(const DWORD dwNPCVnum)
{
	CubeDataVector vCubeData;
	for (const CubeDataPtr& pCubeData : m_vCubeData)
	{
		if (pCubeData->dwNPCVnum == dwNPCVnum)
			vCubeData.emplace_back(std::move(pCubeData));
	}
	return vCubeData;
}
#endif
