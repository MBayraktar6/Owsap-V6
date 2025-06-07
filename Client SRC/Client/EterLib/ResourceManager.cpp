#include "StdAfx.h"
#include <io.h>

#include "../EterBase/CRC32.h"
#include "../EterBase/Timer.h"
#include "../EterBase/Stl.h"
#include "../EterPack/EterPackManager.h"

#include "ResourceManager.h"
#include "GrpImage.h"

int g_iLoadingDelayTime = 20;

const long c_Deleting_Wait_Time = 30000; // 삭제 대기 시간 (30초)
const long c_DeletingCountPerFrame = 30; // 프레임당 체크 리소스 갯수
const long c_Reference_Decrease_Wait_Time = 30000; // 선로딩 리소스의 해제 대기 시간 (30초)

CFileLoaderThread CResourceManager::ms_loadingThread;

void CResourceManager::LoadStaticCache(const char* c_szFileName)
{
	CResource* pkRes = GetResourcePointer(c_szFileName);
	if (pkRes == nullptr)
	{
		Lognf(1, "CResourceManager::LoadStaticCache %s - FAILED", c_szFileName);
		return;
	}

	const DWORD& c_dwCacheKey = GetCRC32(c_szFileName, strlen(c_szFileName));
	TResourcePointerMap::iterator it = m_pCacheMap.find(c_dwCacheKey);
	if (m_pCacheMap.end() != it)
		return;

	pkRes->AddReference();
	m_pCacheMap.emplace(TResourcePointerMap::value_type(c_dwCacheKey, pkRes));
}

void CResourceManager::ProcessBackgroundLoading()
{
	TResourceRequestMap::iterator it = m_RequestMap.begin();
	while (it != m_RequestMap.end())
	{
		const DWORD c_dwFileCRC = it->first;
		std::string& stFileName = it->second;

		if (isResourcePointerData(c_dwFileCRC) ||
			(m_WaitingMap.end() != m_WaitingMap.find(c_dwFileCRC)))
		{
			//printf("SKP %s\n", stFileName.c_str());
			it = m_RequestMap.erase(it);
			continue;
		}

		//printf("REQ %s\n", stFileName.c_str());
		ms_loadingThread.Request(stFileName);
		m_WaitingMap.emplace(TResourceRequestMap::value_type(c_dwFileCRC, stFileName));
		it = m_RequestMap.erase(it);
		//break; // NOTE: 여기서 break 하면 천천히 로딩 된다.
	}

	DWORD dwCurrentTime = ELTimer_GetMSec();
	CFileLoaderThread::TData* pData;
	while (ms_loadingThread.Fetch(&pData))
	{
		//printf("LOD %s\n", pData->stFileName.c_str());
		CResource* const& pResource = GetResourcePointer(pData->stFileName.c_str());

		if (pResource)
		{
			if (pResource->IsEmpty())
			{
				pResource->OnLoad(pData->dwSize, pData->pvBuf);
				pResource->AddReferenceOnly();

				// 여기서 올라간 레퍼런스 카운트를 일정 시간이 지난 뒤에 풀어주기 위하여
				m_pResRefDecreaseWaitingMap.insert(TResourceRefDecreaseWaitingMap::value_type(dwCurrentTime, pResource));
			}
		}

		m_WaitingMap.erase(GetCRC32(pData->stFileName.c_str(), pData->stFileName.size()));

		delete[]((char*)pData->pvBuf);
		delete pData;
	}

	// DO : 일정 시간이 지나고 난뒤 미리 로딩해 두었던 리소스의 레퍼런스 카운트를 감소 시킨다 - [levites]
	dwCurrentTime = ELTimer_GetMSec();

	TResourceRefDecreaseWaitingMap::iterator itRef = m_pResRefDecreaseWaitingMap.begin();
	while (itRef != m_pResRefDecreaseWaitingMap.end())
	{
		const long& lCreatingTime = itRef->first;

		if (dwCurrentTime - lCreatingTime > c_Reference_Decrease_Wait_Time)
		{
			CResource* const& pResource = itRef->second;

			// Decrease Reference Count
			pResource->Release();
			itRef = m_pResRefDecreaseWaitingMap.erase(itRef);
			//Tracef("Decrease Pre Loading Resource\n", lCreatingTime);
		}
		else
			++itRef;
	}
}

void CResourceManager::PushBackgroundLoadingSet(std::set<std::string>& LoadingSet)
{
	std::set<std::string>::iterator it = LoadingSet.begin();

	while (it != LoadingSet.end())
	{
		const DWORD c_dwFileCRC = __GetFileCRC(it->c_str());

		if (isResourcePointerData(c_dwFileCRC))
		{
			++it;
			continue;
		}

		m_RequestMap.emplace(TResourceRequestMap::value_type(c_dwFileCRC, it->c_str()));
		++it;
	}
}

void CResourceManager::__DestroyCacheMap()
{
	for (auto& it : m_pCacheMap)
		it.second->Release();

	m_pCacheMap.clear();
}

void CResourceManager::__DestroyDeletingResourceMap()
{
	Tracenf("CResourceManager::__DestroyDeletingResourceMap %d", m_ResourceDeletingMap.size());
	for (auto& it : m_ResourceDeletingMap)
		it.first->Clear();

	m_ResourceDeletingMap.clear();
}

void CResourceManager::__DestroyResourceMap()
{
	Tracenf("CResourceManager::__DestroyResourceMap %d", m_pResMap.size());

	for (auto& it : m_pResMap)
		it.second->Clear();

	stl_wipe_second(m_pResMap);
}

void CResourceManager::DestroyDeletingList()
{
	CResource::SetDeleteImmediately(true);

	__DestroyCacheMap();
	__DestroyDeletingResourceMap();
}

void CResourceManager::Destroy()
{
	assert(m_ResourceDeletingMap.empty() && "CResourceManager::Destroy - YOU MUST CALL DestroyDeletingList");
	__DestroyResourceMap();
}

void CResourceManager::RegisterResourceNewFunctionPointer(const char* c_szFileExt, CResource* (*pNewFunc)(const char* c_szFileName))
{
	m_pResNewFuncMap[c_szFileExt] = pNewFunc;
}

void CResourceManager::RegisterResourceNewFunctionByTypePointer(const int c_iType, CResource* (*pNewFunc) (const char* c_szFileName))
{
	assert(c_iType >= 0);
	m_pResNewFuncByTypeMap[c_iType] = pNewFunc;
}

CResource* CResourceManager::InsertResourcePointer(DWORD dwFileCRC, CResource* pResource)
{
	TResourcePointerMap::iterator it = m_pResMap.find(dwFileCRC);
	if (m_pResMap.end() != it)
	{
		TraceError("CResource::InsertResourcePointer: %s is already registered\n", pResource->GetFileName());
		assert(!"CResource::InsertResourcePointer: Resource already resistered");
		delete pResource;
		return it->second;
	}

	m_pResMap.emplace(TResourcePointerMap::value_type(dwFileCRC, pResource));
	return pResource;
}

int __ConvertPathName(const char* c_szPathName, char* pszRetPathName, int retLen)
{
	const char* pc;
	int len = 0;

	for (pc = c_szPathName; *pc && len < retLen; ++pc, ++len)
	{
		if (*pc == '/')
			*(pszRetPathName++) = '\\';
		else
			*(pszRetPathName++) = (char)korean_tolower(*pc);
	}

	*pszRetPathName = '\0';
	return len;
}

CResource* CResourceManager::GetTypeResourcePointer(const char* c_szFileName, const int c_iType)
{
	if (!c_szFileName || !*c_szFileName)
	{
		assert(c_szFileName != nullptr && *c_szFileName != '\0');
		return nullptr;
	}

	const char* c_pszFile = nullptr;
	const DWORD c_dwFileCRC = __GetFileCRC(c_szFileName, &c_pszFile);

	CResource* pResource = FindResourcePointer(c_dwFileCRC);
	if (pResource) // 이미 리소스가 있으면 리턴 한다.
		return pResource;

	CResource* (*pNewFunc) (const char*) = nullptr;
	if (c_iType != -1)
	{
		TResourceNewFunctionByTypePointerMap::iterator it = m_pResNewFuncByTypeMap.find(c_iType);
		if (m_pResNewFuncByTypeMap.end() != it)
			pNewFunc = it->second;
	}
	else
	{
		const char* c_pszFileExt = strrchr(c_pszFile, '.');
		if (c_pszFileExt)
		{
			static char s_szFileExt[8 + 1];
			strncpy(s_szFileExt, c_pszFileExt + 1, 8);

			TResourceNewFunctionPointerMap::iterator it = m_pResNewFuncMap.find(s_szFileExt);
			if (m_pResNewFuncMap.end() != it)
				pNewFunc = it->second;
		}
	}

	if (pNewFunc == nullptr)
	{
		TraceError("ResourceManager::GetResourcePointer: NOT SUPPORT FILE %s", c_pszFile);
		return nullptr;
	}

	pResource = InsertResourcePointer(c_dwFileCRC, pNewFunc(c_pszFile));
	return pResource;
}

CResource* CResourceManager::GetResourcePointer(const char* c_szFileName)
{
	if (!c_szFileName || !*c_szFileName)
	{
		TraceError("CResourceManager::GetResourcePointer: filename error!");
		return nullptr;
	}

	const char* c_pszFile = nullptr;
	const DWORD c_dwFileCRC = __GetFileCRC(c_szFileName, &c_pszFile);

	CResource* pResource = FindResourcePointer(c_dwFileCRC);
	if (pResource) // 이미 리소스가 있으면 리턴 한다.
		return pResource;

	const char* c_pszFileExt = strrchr(c_pszFile, '.');

#if defined(_DEBUG)
	if (!IsFileExist(c_szFileName))
	{
		if (c_pszFileExt == nullptr || (stricmp(c_pszFileExt, ".fnt") != 0))
		{
			TraceError("CResourceManager::GetResourcePointer: File not exist %s", c_szFileName);
		}
	}
#endif

	CResource* (*pNewFunc) (const char*) = nullptr;
	if (c_pszFileExt)
	{
		static char s_szFileExt[8 + 1];
		strncpy(s_szFileExt, c_pszFileExt + 1, 8);

		TResourceNewFunctionPointerMap::iterator f = m_pResNewFuncMap.find(s_szFileExt);
		if (m_pResNewFuncMap.end() != f)
			pNewFunc = f->second;
	}

	if (!pNewFunc)
	{
		TraceError("ResourceManager::GetResourcePointer: NOT SUPPORT FILE %s", c_pszFile);
		return nullptr;
	}

	pResource = InsertResourcePointer(c_dwFileCRC, pNewFunc(c_pszFile));
	return pResource;
}

CResource* CResourceManager::FindResourcePointer(const DWORD c_dwFileCRC)
{
	const TResourcePointerMap::iterator& it = m_pResMap.find(c_dwFileCRC);
	if (m_pResMap.end() == it)
		return nullptr;

	return it->second;
}

bool CResourceManager::isResourcePointerData(const DWORD c_dwFileCRC)
{
	const TResourcePointerMap::iterator& it = m_pResMap.find(c_dwFileCRC);
	if (m_pResMap.end() == it)
		return nullptr;

	return it->second->IsData();
}

DWORD CResourceManager::__GetFileCRC(const char* c_pszFileName, const char** c_ppszLowerFileName)
{
	static char s_szFullPathFileName[MAX_PATH];
	const char* c_pszSrc = c_pszFileName;
	char* pszDest = s_szFullPathFileName;
	int	iLen = 0;

	while (c_pszSrc[iLen])
	{
		if (c_pszSrc[iLen] == '/')
			pszDest[iLen] = '\\';
		else
			pszDest[iLen] = static_cast<char>(korean_tolower(c_pszSrc[iLen]));

		++iLen;
	}

	pszDest[iLen] = '\0';

	if (c_ppszLowerFileName)
		*c_ppszLowerFileName = &s_szFullPathFileName[0];

	return GetCRC32(s_szFullPathFileName, iLen);
}

typedef struct SDumpData
{
	const char* c_pszFileName;
	float fKB;
	DWORD dwCost;
} TDumpData;

bool DumpKBCompare(const TDumpData& lhs, const TDumpData& rhs)
{
	return (lhs.fKB > rhs.fKB);
}

bool DumpCostCompare(const TDumpData& lhs, const TDumpData& rhs)
{
	return (lhs.dwCost > rhs.dwCost);
}

struct FDumpPrint
{
	FILE* m_pFile;
	static float m_fTotalKB;

	void operator() (const TDumpData& data) const
	{
		m_fTotalKB += data.fKB;
		fprintf(m_pFile, "%6.1f %s\n", data.fKB, data.c_pszFileName);
	}
};

float FDumpPrint::m_fTotalKB;

struct FDumpCostPrint
{
	FILE* m_pFile;

	void operator() (const TDumpData& data) const
	{
		fprintf(m_pFile, "%-4d %s\n", data.dwCost, data.c_pszFileName);
	}
};

void CResourceManager::DumpFileListToTextFile(const char* c_pszFileName)
{
	std::vector<TDumpData> vDumpData;
	for (auto& it : m_pResMap)
	{
		CResource* const& pResource = it.second;
		if (pResource->IsEmpty())
			continue;

		TDumpData DumpData = {};
		DumpData.c_pszFileName = pResource->GetFileName();

		int iFileSize = 0;
		const char* c_pszFileExt = strrchr(DumpData.c_pszFileName, '.');
		if (pResource->IsType(CGraphicImage::Type()) && strnicmp(c_pszFileExt, ".sub", 4))
			iFileSize = dynamic_cast<CGraphicImage*>(pResource)->GetWidth() * dynamic_cast<CGraphicImage*>(pResource)->GetHeight() * 4;
		else
		{
			FILE* const pFile = fopen(DumpData.c_pszFileName, "rb");
			if (pFile)
			{
				fseek(pFile, 0L, SEEK_END);
				iFileSize = ftell(pFile);
				fclose(pFile);
			}
			else
				iFileSize = 0;
		}

		DumpData.fKB = static_cast<float>(iFileSize) / static_cast<float>(1024);
		DumpData.dwCost = pResource->GetLoadCostMilliSecond();

		vDumpData.push_back(DumpData);
	}

	FILE* const pFile = fopen(c_pszFileName, "w");
	if (pFile != nullptr)
	{
		std::sort(vDumpData.begin(), vDumpData.end(), DumpKBCompare);

		FDumpPrint DumpPrint = {};
		DumpPrint.m_pFile = pFile;
		DumpPrint.m_fTotalKB = 0;

		std::for_each(vDumpData.begin(), vDumpData.end(), DumpPrint);
		fprintf(pFile, "total: %.2fmb", DumpPrint.m_fTotalKB / 1024.0f);

		FDumpCostPrint DumpCostPrint = {};
		DumpCostPrint.m_pFile = pFile;

		std::sort(vDumpData.begin(), vDumpData.end(), DumpCostCompare);
		std::for_each(vDumpData.begin(), vDumpData.end(), DumpCostPrint);
		fprintf(pFile, "total: %.2fmb", DumpPrint.m_fTotalKB / 1024.0f);

		fclose(pFile);
	}
}

bool CResourceManager::IsFileExist(const char* c_pszFileName)
{
	return CEterPackManager::Instance().isExist(c_pszFileName);
}

void CResourceManager::Update()
{
	const DWORD& c_dwCurrentTime = ELTimer_GetMSec();
	int iCount = 0;

	TResourceDeletingMap::iterator it = m_ResourceDeletingMap.begin();
	while (it != m_ResourceDeletingMap.end())
	{
		CResource* const& pResource = it->first;
		if (c_dwCurrentTime >= it->second)
		{
			if (pResource->canDestroy())
			{
				//Tracef("Resource Clear %s\n", pResource->GetFileName());
				pResource->Clear();
			}

			it = m_ResourceDeletingMap.erase(it);

			if (++iCount >= c_DeletingCountPerFrame)
				break;
		}
		else
			++it;
	}

	ProcessBackgroundLoading();
}

void CResourceManager::ReserveDeletingResource(CResource* pResource)
{
	m_ResourceDeletingMap.emplace(TResourceDeletingMap::value_type(pResource, ELTimer_GetMSec() + c_Deleting_Wait_Time));
}

CResourceManager::CResourceManager()
{
	//ms_loadingThread.Create(0);
}

CResourceManager::~CResourceManager()
{
	Destroy();
	//ms_loadingThread.Shutdown();
}
