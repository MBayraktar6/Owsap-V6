#include "StdAfx.h"
#include "MapBase.h"

CMapBase::CMapBase()
{
	Clear();
}

CMapBase::~CMapBase()
{
	Clear();
}

void CMapBase::Clear()
{
	m_strName = "";
	m_eType = MAPTYPE_INVALID;
	mc_pEnvironmentData = NULL;
#if defined(ENABLE_CONQUEROR_LEVEL)
	m_bHasSungmaAttr = false;
	memset(&m_kSungmaAttr, 0, sizeof(m_kSungmaAttr));
#endif
	Leave();
}

bool CMapBase::Enter()
{
	m_bReady = true;
	return true;
}

bool CMapBase::Leave()
{
	m_bReady = false;
	return true;
}

void CMapBase::SetEnvironmentDataPtr(const TEnvironmentData* c_pEnvironmentData)
{
	mc_pEnvironmentData = c_pEnvironmentData;
	OnSetEnvironmentDataPtr();
}

void CMapBase::ResetEnvironmentDataPtr(const TEnvironmentData* c_pEnvironmentData)
{
	mc_pEnvironmentData = c_pEnvironmentData;
	OnResetEnvironmentDataPtr();
}

void CMapBase::Render()
{
	if (IsReady())
		OnRender();
}

bool CMapBase::LoadProperty()
{
	std::string strFileName = GetName() + "\\MapProperty.txt";

	CTokenVectorMap stTokenVectorMap;

	if (!LoadMultipleTextData(strFileName.c_str(), stTokenVectorMap))
	{
		TraceError("CMapBase::LoadProperty(FileName=%s) - LoadMultipleTextData ERROR 파일이 없을 가능성이 많습니다.", strFileName.c_str());
		return false;
	}

	if (stTokenVectorMap.end() == stTokenVectorMap.find("scripttype"))
	{
		TraceError("CMapBase::LoadProperty(FileName=%s) - FIND 'scripttype' - FAILED", strFileName.c_str());
		return false;
	}

	if (stTokenVectorMap.end() == stTokenVectorMap.find("maptype"))
	{
		TraceError("CMapBase::LoadProperty(FileName=%s) - FIND 'maptype' - FAILED", strFileName.c_str());
		return false;
	}

	// NOTE: 이미 존재하는 맵 데이터와 동일한 데이터를 사용하는 맵을 새로 추가할 때, 맵 배포 용량을 줄이기 위한 작업.
	// MapProperty.txt 파일에 ParentMapName 값이 설정되어 있다면, 실제 모든 데이터는 ParentMap에서 읽어온다.
	// 데이터의 부분공유(부분 오버라이트?) 기능은 필요 없대서, Parent Map에서 모든 데이터를 읽어옴.
	if (stTokenVectorMap.end() != stTokenVectorMap.find("parentmapname"))
	{
		m_strParentMapName = stTokenVectorMap["parentmapname"][0];
	}

	const std::string& c_rstrType = stTokenVectorMap["scripttype"][0];
	const std::string& c_rstrMapType = stTokenVectorMap["maptype"][0];

	if (0 != c_rstrType.compare("MapProperty"))
	{
		TraceError("CMapBase::LoadProperty(FileName=%s) - Resourse Type ERROR", strFileName.c_str());
		return false;
	}

	if (0 == c_rstrMapType.compare("Indoor"))
		SetType(MAPTYPE_INDOOR);
	else if (0 == c_rstrMapType.compare("Outdoor"))
		SetType(MAPTYPE_OUTDOOR);
	else if (0 == c_rstrMapType.compare("Invalid"))
		SetType(MAPTYPE_OUTDOOR);

	return true;
}

#if defined(ENABLE_CONQUEROR_LEVEL)
bool CMapBase::LoadSungMaAttr()
{
	const std::string strFileName = GetName() + "\\sungma_attr.txt";

	CTokenVectorMap stTokenVectorMap;
	if (!LoadMultipleTextData(strFileName.c_str(), stTokenVectorMap))
		return true;

	if (stTokenVectorMap.end() == stTokenVectorMap.find("sungma_str"))
	{
		TraceError("CMapBase::LoadSungMaAttr(FileName=%s) - sungmamap data ERROR", strFileName.c_str());
		return false;
	}

	if (stTokenVectorMap.end() == stTokenVectorMap.find("sungma_hp"))
	{
		TraceError("CMapBase::LoadSungMaAttr(FileName=%s) - sungmamap data ERROR", strFileName.c_str());
		return false;
	}

	if (stTokenVectorMap.end() == stTokenVectorMap.find("sungma_move"))
	{
		TraceError("CMapBase::LoadSungMaAttr(FileName=%s) - sungmamap data ERROR", strFileName.c_str());
		return false;
	}

	if (stTokenVectorMap.end() == stTokenVectorMap.find("sungma_immune"))
	{
		TraceError("CMapBase::LoadSungMaAttr(FileName=%s) - sungmamap data ERROR", strFileName.c_str());
		return false;
	}

	m_kSungmaAttr.str = atoi(stTokenVectorMap["sungma_str"][0].c_str());
	m_kSungmaAttr.hp = atoi(stTokenVectorMap["sungma_hp"][0].c_str());
	m_kSungmaAttr.move = atoi(stTokenVectorMap["sungma_move"][0].c_str());
	m_kSungmaAttr.immune = atoi(stTokenVectorMap["sungma_immune"][0].c_str());
	m_bHasSungmaAttr = true;

	return true;
}

bool CMapBase::HasSungMaAttr() const
{
	return m_bHasSungmaAttr;
}

const CMapBase::SSungmaAttr& CMapBase::GetSungMaAttrData() const
{
	return m_kSungmaAttr;
}
#endif
