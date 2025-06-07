#include "StdAfx.h"
#include <string>
#include "../EterPack/EterPackManager.h"

#include "GroupTextParseTree.h"

CGroupTextParseTreeLoader::CGroupTextParseTreeLoader() : m_pRootGroupNode(NULL), m_dwcurLineIndex(0)
{
}

CGroupTextParseTreeLoader::~CGroupTextParseTreeLoader()
{
	if (NULL != m_pRootGroupNode)
		delete m_pRootGroupNode;

	m_dwcurLineIndex = 0;
}

const char* CGroupTextParseTreeLoader::GetFileName()
{
	return m_strFileName.c_str();
}

bool CGroupTextParseTreeLoader::Load(const char* c_szFileName)
{
	CMappedFile File;
	LPCVOID pData;
	if (!CEterPackManager::Instance().Get(File, c_szFileName, &pData))
		return false;

	m_strFileName = c_szFileName;
	m_dwcurLineIndex = 0;

	m_fileLoader.Bind(File.Size(), pData);
	if (NULL != m_pRootGroupNode)
		delete m_pRootGroupNode;

	m_pRootGroupNode = new CGroupNode;
	if (LoadGroup(m_pRootGroupNode))
		return true;

	if (NULL != m_pRootGroupNode)
		delete m_pRootGroupNode;

	return false;
}

bool CGroupTextParseTreeLoader::LoadGroup(CGroupNode* pGroupNode)
{
	CTokenVector stTokenVector;
	for (; m_dwcurLineIndex < m_fileLoader.GetLineCount(); ++m_dwcurLineIndex)
	{
		if (!m_fileLoader.SplitLine(m_dwcurLineIndex, &stTokenVector))
			continue;

		stl_lowers(stTokenVector[0]);

		if ('{' == stTokenVector[0][0])
			continue;

		if ('}' == stTokenVector[0][0])
			break;

		// Group
		if (0 == stTokenVector[0].compare("group"))
		{
			if (2 != stTokenVector.size())
			{
				TraceError("Invalid group syntax token size: %u != 2 (DO NOT SPACE IN NAME)", stTokenVector.size());
				for (unsigned int i = 0; i < stTokenVector.size(); ++i)
					TraceError("  %u %s", i, stTokenVector[i].c_str());
				exit(1);
				continue;
			}

			CGroupNode* pNewNode = new CGroupNode;
			pNewNode->strGroupName = stTokenVector[1];
			stl_lowers(pNewNode->strGroupName);

			pGroupNode->SetChildNode(pNewNode->strGroupName.c_str(), pNewNode);

			++m_dwcurLineIndex;

			LoadGroup(pNewNode);
		}
		// Column Names
		else if (0 == stTokenVector[0].compare("#--#"))
		{
			for (CTokenVector::size_type i = 1; i < stTokenVector.size(); i++)
			{
				stl_lowers(stTokenVector[i]);
				pGroupNode->m_map_columnNameToIndex.insert(CMapNameToIndex::value_type(stTokenVector[i], i - 1));
			}
		}
		else
		{
			std::string key = stTokenVector[0];

			if (1 == stTokenVector.size())
			{
				TraceError("CGroupTextParseTreeLoader::LoadGroup : must have a value (filename: %s line: %d key: %s)",
					m_strFileName.c_str(),
					m_dwcurLineIndex,
					key.c_str());
				break;
			}

			stTokenVector.erase(stTokenVector.begin());
			pGroupNode->m_map_rows.insert(std::make_pair(key, CGroupNode::CGroupNodeRow(pGroupNode, stTokenVector)));
		}
	}

	return true;
}

CGroupNode* CGroupTextParseTreeLoader::GetGroup(const char* c_szGroupName)
{
	if (NULL == m_pRootGroupNode)
		return NULL;

	return m_pRootGroupNode->GetChildNode(c_szGroupName);
}

CGroupNode::CGroupNode()
{
}

CGroupNode::~CGroupNode()
{
	stl_wipe_second(m_mapChildNodes);
}

DWORD CGroupNode::GetChildNodeCount()
{
	return m_mapChildNodes.size();
}

bool CGroupNode::SetChildNode(const char* c_szKey, CGroupNode* pChildGroup)
{
	if (NULL == pChildGroup)
	{
		m_mapChildNodes.erase(c_szKey);
		return true;
	}

	TMapGroup::iterator it = m_mapChildNodes.find(c_szKey);
	if (it != m_mapChildNodes.end())
		return false;

	m_mapChildNodes.insert(TMapGroup::value_type(c_szKey, pChildGroup));

	return true;
}

CGroupNode* CGroupNode::GetChildNode(const std::string& c_rstrKey) const
{
	TMapGroup::const_iterator it = m_mapChildNodes.find(c_rstrKey);
	if (it != m_mapChildNodes.end())
		return it->second;

	return NULL;
}

std::string CGroupNode::GetNodeName() const
{
	return strGroupName;
}

bool CGroupNode::IsToken(const std::string& c_rstrKey) const
{
	return m_map_rows.end() != m_map_rows.find(c_rstrKey);
}

int CGroupNode::GetRowCount()
{
	return m_map_rows.size();
}

bool CGroupNode::GetRow(const std::string& c_rstrRowKey, OUT const CGroupNode::CGroupNodeRow** ppRow) const
{
	TMapRow::const_iterator row_it = m_map_rows.find(c_rstrRowKey);
	if (m_map_rows.end() == row_it)
		return false;

	*ppRow = &row_it->second;

	return true;
}

// 참고로, idx랑 txt에 쓰여진 순서랑 관계 없음.
bool CGroupNode::GetRow(int idx, OUT const CGroupNode::CGroupNodeRow** ppRow) const
{
	if ((TMapRow::size_type)idx >= m_map_rows.size())
		return false;

	TMapRow::const_iterator row_it = m_map_rows.begin();

	std::advance(row_it, idx);

	*ppRow = &row_it->second;

	return true;
}

bool CGroupNode::GetGroupRow(const std::string& stGroupName, const std::string& stRow, OUT const CGroupNode::CGroupNodeRow** ppRow) const
{
	CGroupNode* pChildGroup = GetChildNode(stGroupName);
	if (NULL != pChildGroup)
	{
		if (pChildGroup->GetRow(stRow, ppRow))
			return true;
	}

	// default group을 살펴봄.
	pChildGroup = GetChildNode("default");
	if (NULL != pChildGroup)
	{
		if (pChildGroup->GetRow(stRow, ppRow))
			return true;
	}

	return false;
}

int	CGroupNode::GetColumnIndexFromName(const std::string& stName) const
{
	CMapNameToIndex::const_iterator it = m_map_columnNameToIndex.find(stName);
	if (m_map_columnNameToIndex.end() == it)
		return -1;

	return it->second;
}

CGroupNode::CGroupNodeRow::CGroupNodeRow(CGroupNode* pGroupNode, CTokenVector& vec_values)
{
	m_pOwnerGroupNode = pGroupNode;
	m_vec_values.clear();
	std::copy(vec_values.begin(), vec_values.end(), std::back_inserter(m_vec_values));
}

CGroupNode::CGroupNodeRow::~CGroupNodeRow()
{
}

int CGroupNode::CGroupNodeRow::GetSize() const
{
	return m_vec_values.size();
}
