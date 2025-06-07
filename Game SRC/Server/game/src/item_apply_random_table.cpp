/**
* Filename: item_apply_random_table.cpp
* Author: Owsap
* Description: Generate a random bonus path.
**/

#include "stdafx.h"

#if defined(__ITEM_APPLY_RANDOM__)
#include "group_text_parse_tree.h"
#include "item_apply_random_table.h"
#include "constants.h"

const std::string c_stApplyRandomTypeGroupName = "applyrandomtype";

bool CApplyRandomTable::ReadApplyRandomTableFile(const char* c_pszFileName)
{
	m_pLoader = new CGroupTextParseTreeLoader;
	CGroupTextParseTreeLoader& rkLoader = *m_pLoader;

	if (rkLoader.Load(c_pszFileName) == false)
		return false;

	if (!ReadApplyRandomMapper())
		return false;

	if (!ReadApplyRandomTypes())
		return false;

	m_pApplyRandomValueTableNode = rkLoader.GetGroup("applyrandomvalues");
	if (m_pApplyRandomValueTableNode)
		return true;

	return false;
}

bool CApplyRandomTable::ReadApplyRandomMapper()
{
	CGroupNode* pGroupNode = m_pLoader->GetGroup("applyrandommapper");
	if (pGroupNode == nullptr)
	{
		sys_err(0, "Group ApplyRandomMapper not found.");
		return false;
	}

	{
		std::size_t nSize = pGroupNode->GetRowCount();
		if (nSize == 0)
		{
			sys_err(0, "Group ApplyRandomMapper is Empty.");
			return false;
		}

		for (std::size_t n = 0; n < nSize; n++)
		{
			const CGroupNode::CGroupNodeRow* c_pRow;
			pGroupNode->GetRow(n, &c_pRow);

			std::string stGroupName;
			if (!c_pRow->GetValue("group_name", stGroupName))
			{
				sys_err(0, "In Group ApplyRandomMapper, Column Group_Name not found.");
				return false;
			}

			std::transform(stGroupName.begin(), stGroupName.end(), stGroupName.begin(), ::tolower);
			m_vecApplyRandomGroups.emplace_back(stGroupName);
		}
	}
	return true;
}

bool CApplyRandomTable::ReadApplyRandomTypes()
{
	CGroupNode* pGroupNode = m_pLoader->GetGroup("applyrandomtypes");
	if (pGroupNode == nullptr)
	{
		sys_err(0, "Table needs ApplyRandomTypes.");
		return false;
	}

	for (std::size_t n = 0; n < m_vecApplyRandomGroups.size(); n++)
	{
		CGroupNode* pChild;
		if (nullptr == (pChild = pGroupNode->GetChildNode(m_vecApplyRandomGroups[n])))
		{
			sys_err(0, "In Group ApplyRandomTypes, %s group is not defined.", m_vecApplyRandomGroups[n].c_str());
			return false;
		}

		ApplyRandomVector vecApplyRandom;
		std::size_t nSize = pChild->GetRowCount();
		for (int j = 1; j <= nSize; j++)
		{
			std::stringstream stStream;
			stStream << j;
			const CGroupNode::CGroupNodeRow* c_pRow = nullptr;

			pChild->GetRow(stStream.str(), &c_pRow);
			if (c_pRow == nullptr)
			{
				sys_err(0, "In Group ApplyRandomTypes, No %d row.", j);
				return false;
			}

			std::string stTypeName;
			if (!c_pRow->GetValue("apply_type", stTypeName))
			{
				sys_err(0, "In Group ApplyRandomTypes, %s group's Apply_Type is empty.", m_vecApplyRandomGroups[n].c_str());
				return false;
			}

			EApplyTypes ApplyType;
			if (!(ApplyType = (EApplyTypes)FN_get_apply_type(stTypeName.c_str())))
			{
				sys_err(0, "In Group ApplyRandomTypes, %s group's Apply_Type %s is invalid.", m_vecApplyRandomGroups[n].c_str(), stTypeName.c_str());
				return false;
			}

			std::string stApplyValueGroupName;
			if (!c_pRow->GetValue("apply_value_group_name", stApplyValueGroupName))
			{
				sys_err(0, "In Group ApplyRandomTypes, %s group's Apply_Value_Group_Name is empty.", m_vecApplyRandomGroups[n].c_str());
				return false;
			}

			vecApplyRandom.push_back(SApplyRandom(ApplyType, stApplyValueGroupName));
		}
		m_mapApplyRandomGroup.insert(ApplyRandomGroupMap::value_type(m_vecApplyRandomGroups[n].c_str(), vecApplyRandom));
	}
	return true;
}

bool CApplyRandomTable::GetApplyRandom(BYTE bIndex, BYTE bLevel, POINT_TYPE& wApplyType, POINT_VALUE& lApplyValue, BYTE& bPath)
{
	std::string stApplyListName(c_stApplyRandomTypeGroupName + std::to_string(bIndex));
	std::transform(stApplyListName.begin(), stApplyListName.end(), stApplyListName.begin(), ::tolower);

	ApplyRandomGroupMap::const_iterator it = m_mapApplyRandomGroup.find(stApplyListName);
	if (m_mapApplyRandomGroup.end() != it)
	{
		if (bLevel >= ITEM_REFINE_MAX_LEVEL)
			bLevel = ITEM_REFINE_MAX_LEVEL;

		ApplyRandomVector vecApplyRandom = it->second;
		int iRandom = number(0, vecApplyRandom.size() - 1);
		wApplyType = vecApplyRandom[iRandom].ApplyType;

		const BYTE bRandomPath = GetApplyRandomPath(vecApplyRandom[iRandom].ApplyValueGroupName);
		lApplyValue = GetApplyRandomRowValue(vecApplyRandom[iRandom].ApplyValueGroupName, bLevel, bRandomPath);
		bPath = bRandomPath;

		return true;
	}
	else
	{
		wApplyType = APPLY_NONE;
		lApplyValue = 0;
		bPath = 0;
	}

	return false;
}

POINT_VALUE CApplyRandomTable::GetApplyRandomValue(BYTE bIndex, BYTE bLevel, BYTE bPath, POINT_TYPE wApplyType)
{
	std::string stApplyListName(c_stApplyRandomTypeGroupName + std::to_string(bIndex));
	std::transform(stApplyListName.begin(), stApplyListName.end(), stApplyListName.begin(), ::tolower);

	ApplyRandomGroupMap::const_iterator it = m_mapApplyRandomGroup.find(stApplyListName.c_str());
	if (m_mapApplyRandomGroup.end() != it)
	{
		for (const SApplyRandom& vecApplyRandom : it->second)
		{
			if (vecApplyRandom.ApplyType == wApplyType)
			{
				return GetApplyRandomRowValue(vecApplyRandom.ApplyValueGroupName, bLevel, bPath);
			}
		}
	}
	return 0;
}

BYTE CApplyRandomTable::GetApplyRandomPath(std::string stApplyValueGroupName) const
{
	std::transform(stApplyValueGroupName.begin(), stApplyValueGroupName.end(), stApplyValueGroupName.begin(), ::tolower);

	CGroupNode* pChildNode = m_pApplyRandomValueTableNode->GetChildNode(stApplyValueGroupName);
	if (pChildNode == nullptr)
		return 0;

	const BYTE bPath = number(0, pChildNode->GetRowCount() - 1);
	printf("path choosen : %d (max:%d)", bPath, pChildNode->GetRowCount() - 1);

	return bPath;
}

POINT_VALUE CApplyRandomTable::GetApplyRandomRowValue(std::string stApplyValueGroupName, BYTE bLevel, BYTE bPath) const
{
	std::transform(stApplyValueGroupName.begin(), stApplyValueGroupName.end(), stApplyValueGroupName.begin(), ::tolower);

	CGroupNode* pChildNode = m_pApplyRandomValueTableNode->GetChildNode(stApplyValueGroupName);
	if (pChildNode == nullptr)
		return 0;

	long lValue = 0;
	if (bPath < pChildNode->GetRowCount())
	{
		const CGroupNode::CGroupNodeRow* pRow;
		pChildNode->GetRow(bPath, &pRow);

		if (!pRow->GetValue(bLevel, lValue))
		{
			sys_err(0, "In Group %s, Level %d not found.", pChildNode->GetNodeName().c_str(), bLevel);
			return 0;
		}
	}

	return lValue;
}

CApplyRandomTable::CApplyRandomTable() : m_pLoader(nullptr), m_pApplyRandomValueTableNode(nullptr) {}
CApplyRandomTable::~CApplyRandomTable()
{
	if (m_pLoader)
		delete m_pLoader;
}
#endif
