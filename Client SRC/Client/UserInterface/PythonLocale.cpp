/**
* Filename: PythonLocale.cpp
* Author: Owsap, blackdragonx61 (aka. Mali)
**/

#include "StdAfx.h"

#if defined(ENABLE_LOCALE_CLIENT)
#include "PythonApplication.h"

#include "../EterPack/EterPackManager.h"
#include "../GameLib/ItemManager.h"

#undef snprintf
#include <boost\format.hpp>
#define snprintf _snprintf

#if defined(ENABLE_LOADING_TIP)
#	include <random>
#endif

CPythonLocale::CPythonLocale() = default;
CPythonLocale::~CPythonLocale() = default;

bool CPythonLocale::LoadLocaleString(const char* c_szFileName)
{
	if (CPythonApplication::Instance().GetReloadLocale())
		m_LocaleStringMap[LOCALE_STRING].clear();

	if (m_LocaleStringMap[LOCALE_STRING].empty() == false)
		return true;

	const VOID* pvData;
	CMappedFile kFile;
	if (!CEterPackManager::Instance().Get(kFile, c_szFileName, &pvData))
	{
		TraceError("CPythonLocale::LoadLocaleString(c_szFileName: %s) - Load Error", c_szFileName);
		return false;
	}

	CMemoryTextFileLoader kTextFileLoader;
	kTextFileLoader.Bind(kFile.Size(), pvData);

	CTokenVector kTokenVector;
	for (DWORD i = 0; i < kTextFileLoader.GetLineCount(); ++i)
	{
		const std::string& c_rstLine = kTextFileLoader.GetLineString(i);
		if (c_rstLine.empty())
			continue;

		if (c_rstLine[0] == '#')
			continue;

		if (!kTextFileLoader.SplitLineByTab(i, &kTokenVector))
			continue;

		if (kTokenVector.size() != 2)
			continue;

		m_LocaleStringMap[LOCALE_STRING][kTokenVector[0]] = kTokenVector[1];
	}

	return true;
}

bool CPythonLocale::LoadQuestLocaleString(const char* c_szFileName, bool bOverride)
{
	if (!bOverride)
	{
		if (CPythonApplication::Instance().GetReloadLocale())
			m_LocaleStringMap[LOCALE_QUEST_STRING].clear();

		if (m_LocaleStringMap[LOCALE_QUEST_STRING].empty() == false)
			return true;
	}

	const VOID* pvData;
	CMappedFile kFile;
	if (!CEterPackManager::Instance().Get(kFile, c_szFileName, &pvData))
	{
		TraceError("CPythonLocale::LoadQuestLocaleString(c_szFileName: %s) - Load Error", c_szFileName);
		return false;
	}

	CMemoryTextFileLoader kTextFileLoader;
	kTextFileLoader.Bind(kFile.Size(), pvData);

	CTokenVector kTokenVector;
	for (DWORD i = 0; i < kTextFileLoader.GetLineCount(); ++i)
	{
		const std::string& c_rstLine = kTextFileLoader.GetLineString(i);
		if (c_rstLine.empty())
			continue;

		if (c_rstLine[0] == '#')
			continue;

		if (!kTextFileLoader.SplitLineByTab(i, &kTokenVector))
			continue;

		if (kTokenVector.size() != 2)
			continue;

		m_LocaleStringMap[LOCALE_QUEST_STRING][kTokenVector[0]] = kTokenVector[1];
	}

	return true;
}

bool CPythonLocale::LoadOXQuizLocaleString(const char* c_szFileName)
{
	if (CPythonApplication::Instance().GetReloadLocale())
		m_LocaleStringMap[LOCALE_OXQUIZ_STRING].clear();

	if (m_LocaleStringMap[LOCALE_OXQUIZ_STRING].empty() == false)
		return true;

	const VOID* pvData;
	CMappedFile kFile;
	if (!CEterPackManager::Instance().Get(kFile, c_szFileName, &pvData))
	{
		TraceError("CPythonLocale::LoadOXQuizLocaleString(c_szFileName: %s) - Load Error", c_szFileName);
		return false;
	}

	CMemoryTextFileLoader kTextFileLoader;
	kTextFileLoader.Bind(kFile.Size(), pvData);

	CTokenVector kTokenVector;
	for (DWORD i = 0; i < kTextFileLoader.GetLineCount(); ++i)
	{
		const std::string& c_rstLine = kTextFileLoader.GetLineString(i);
		if (c_rstLine.empty())
			continue;

		if (c_rstLine[0] == '#')
			continue;

		if (!kTextFileLoader.SplitLineByTab(i, &kTokenVector))
			continue;

		if (kTokenVector.size() != 3)
			continue;

		m_LocaleStringMap[LOCALE_OXQUIZ_STRING][kTokenVector[1]] = kTokenVector[2];
	}

	return true;
}

#if defined(ENABLE_LOADING_TIP)
bool CPythonLocale::LoadLoadingTipVnum(const char* c_szFileName)
{
	m_kMap_dwID_strTipString.clear();

	const VOID* pvData;
	CMappedFile kFile;
	if (!CEterPackManager::Instance().Get(kFile, c_szFileName, &pvData))
	{
		Tracef("CPythonLocale::LoadLoadingTipVnum(c_szFileName=%s) - Load Error", c_szFileName);
		return false;
	}

	CMemoryTextFileLoader kTextFileLoader;
	kTextFileLoader.Bind(kFile.Size(), pvData);

	CTokenVector kTokenVector;
	for (DWORD i = 0; i < kTextFileLoader.GetLineCount(); ++i)
	{
		if (!kTextFileLoader.SplitLineByTab(i, &kTokenVector))
			continue;

		const char* c_szComment = "#";
		if (kTokenVector[0].compare(0, 1, c_szComment) == 0 || kTokenVector.size() != 2)
			continue;

		m_kMap_dwID_strTipString.insert(std::make_pair(atoi(kTokenVector[0].c_str()), kTokenVector[1]));
	}

	return true;
}

bool CPythonLocale::LoadLoadingTipList(const char* c_szFileName)
{
	m_kMap_dwMapIndex_vecTipList.clear();

	CTextFileLoader kTextFileLoader;
	if (!kTextFileLoader.Load(c_szFileName))
	{
		Tracef("CPythonLocale::LoadLoadingTipList(c_szFileName=%s) - Load Error", c_szFileName);
		return false;
	}

	for (DWORD i = 0; i < kTextFileLoader.GetChildNodeCount(); ++i)
	{
		CTextFileLoader::CGotoChild GotoChild(&kTextFileLoader, i);

		CTokenVector* pTipTokens;
		std::vector<DWORD> vecTipList;
		vecTipList.clear();

		if (kTextFileLoader.GetTokenVector("tip_vnum", &pTipTokens))
		{
			for (size_t j = 0; j < pTipTokens->size(); ++j)
			{
				vecTipList.push_back(atoi(pTipTokens->at(j).c_str()));
			}
		}

		CTokenVector* pMapTokens;
		std::vector<DWORD> vecMapList;
		vecMapList.clear();

		if (kTextFileLoader.GetTokenVector("map_index", &pMapTokens))
		{
			for (size_t k = 0; k < pMapTokens->size(); ++k)
			{
				vecMapList.push_back(atoi(pMapTokens->at(k).c_str()));
			}
		}

		if (vecTipList.size() > 0)
		{
			if (vecMapList.size() == 0)
			{
				m_kMap_dwMapIndex_vecTipList.insert(std::make_pair(0, vecTipList));
			}
			else
			{
				for (size_t l = 0; l < vecMapList.size(); ++l)
				{
					m_kMap_dwMapIndex_vecTipList.insert(std::make_pair(vecMapList.at(l), vecTipList));
				}
			}
		}
	}

	return true;
}

const char* CPythonLocale::GetLoadingTipVnum(int iMapIndex)
{
	if (iMapIndex > 10000)
		iMapIndex /= 10000;

	std::random_device rd;
	std::mt19937 mt(rd());
	std::map<DWORD, std::vector<DWORD>>::iterator it = m_kMap_dwMapIndex_vecTipList.find(iMapIndex);

	static char s_szTip[512] = {};
	if (it != m_kMap_dwMapIndex_vecTipList.end())
	{
		std::uniform_int_distribution<std::mt19937::result_type> dist(0, it->second.size() - 1);
		auto rit = it->second.at(dist(mt));
		sprintf(s_szTip, "%s", m_kMap_dwID_strTipString.find(rit)->second.c_str());
	}
	else
	{
		std::uniform_int_distribution<std::mt19937::result_type> dist(0, m_kMap_dwID_strTipString.size() - 1);
		auto rit = std::next(std::begin(m_kMap_dwID_strTipString), dist(mt));
		sprintf(s_szTip, "%s", rit->second.c_str());
	}

	return s_szTip;
}
#endif

void CPythonLocale::FormatString(std::string& sMessage) const
{
	ReplaceSkillName(sMessage);
	ReplacePetSkillName(sMessage);
	ReplaceMobName(sMessage);
	ReplaceItemName(sMessage);
#if defined(ENABLE_SET_ITEM)
	ReplacePreItemName(sMessage);
#endif
	ReplaceLocaleString(sMessage);
	ReplaceQuestLocaleString(sMessage);
	ReplaceOXQuizLocaleString(sMessage);
}

void CPythonLocale::FormatString(char* buf, size_t size) const
{
	std::string sFormat = { buf };
	FormatString(sFormat);
	
	strncpy(buf, sFormat.c_str(), size);
	buf[size - 1] = '\0';
}

void CPythonLocale::MultiLineSplit(const std::string& sMessage, TTokenVector& vec) const
{
	if (sMessage.find("[ENTER]") == std::string::npos)
		return;

	size_t size = 0;
	while (true)
	{
		const size_t pos = sMessage.find("[ENTER]", size);
		if (pos == std::string::npos)
			break;

		vec.push_back(sMessage.substr(size, pos - size));
		size = pos + 7;
	}

	if (size < sMessage.size())
		vec.push_back(sMessage.substr(size));
}

void CPythonLocale::ReplaceSkillName(std::string& sMessage) const
{
	while (true)
	{
		size_t pos_begin = sMessage.find("[SN;");
		if (pos_begin == std::string::npos)
			break;

		size_t pos_mid = sMessage.find(';', pos_begin);
		if (pos_mid == std::string::npos)
			break;

		size_t pos_end = sMessage.find(']', ++pos_mid);
		if (pos_end == std::string::npos)
			break;

		DWORD dwVnum{};
		try
		{
			dwVnum = std::stoul(sMessage.substr(pos_mid, pos_end - pos_mid));
		}
		catch (const std::exception& ex)
		{
			TraceError("CPythonLocale::ReplaceSkillName: Error: %s", ex.what());
			break;
		}

		CPythonSkill::TSkillData* pSkillData;
		if (!CPythonSkill::Instance().GetSkillData(dwVnum, &pSkillData))
		{
			TraceError("CPythonLocale::ReplaceSkillName: can't find skill vnum: %lu", dwVnum);
			break;
		}

		sMessage.replace(pos_begin, (pos_end + 1) - pos_begin, pSkillData->strName);
	}
}

void CPythonLocale::ReplacePetSkillName(std::string& sMessage) const
{
}

void CPythonLocale::ReplaceMobName(std::string& sMessage) const
{
	while (true)
	{
		size_t pos_begin = sMessage.find("[MN;");
		if (pos_begin == std::string::npos)
			break;

		size_t pos_mid = sMessage.find(';', pos_begin);
		if (pos_mid == std::string::npos)
			break;

		size_t pos_end = sMessage.find(']', ++pos_mid);
		if (pos_end == std::string::npos)
			break;

		DWORD dwVnum{};
		try
		{
			dwVnum = std::stoul(sMessage.substr(pos_mid, pos_end - pos_mid));
		}
		catch (const std::exception& ex)
		{
			TraceError("CPythonLocale::ReplaceMobName: Error: %s", ex.what());
			break;
		}

		const char* c_szName;
		if (!CPythonNonPlayer::Instance().GetName(dwVnum, &c_szName))
		{
			TraceError("CPythonLocale::ReplaceMobName: can't find mob vnum: %lu", dwVnum);
			break;
		}

		sMessage.replace(pos_begin, (pos_end + 1) - pos_begin, c_szName);
	}
}

void CPythonLocale::ReplaceItemName(std::string& sMessage) const
{
	while (true)
	{
		size_t pos_begin = sMessage.find("[IN;");
		if (pos_begin == std::string::npos)
			break;

		size_t pos_mid = sMessage.find(';', pos_begin);
		if (pos_mid == std::string::npos)
			break;

		size_t pos_end = sMessage.find(']', ++pos_mid);
		if (pos_end == std::string::npos)
			break;

		DWORD dwVnum{};
		try
		{
			dwVnum = std::stoul(sMessage.substr(pos_mid, pos_end - pos_mid));
		}
		catch (const std::exception& ex)
		{
			TraceError("CPythonLocale::ReplaceItemName: Error: %s", ex.what());
			break;
		}

		CItemData* pItemData;
		if (!CItemManager::Instance().GetItemDataPointer(dwVnum, &pItemData))
		{
			TraceError("CPythonLocale::ReplaceItemName: can't find item vnum: %lu", dwVnum);
			break;
		}

		sMessage.replace(pos_begin, (pos_end + 1) - pos_begin, pItemData->GetName());
	}
}

#if defined(ENABLE_SET_ITEM)
void CPythonLocale::ReplacePreItemName(std::string& sMessage) const
{
	while (true)
	{
		size_t pos_1 = sMessage.rfind("[PRE_IN;");
		if (pos_1 == std::string::npos)
			break;

		size_t pos_2 = sMessage.find(';', pos_1);
		if (pos_2 == std::string::npos)
			break;

		++pos_2;

		size_t pos_3 = sMessage.find('[', pos_2);
		size_t pos_4 = sMessage.find(']', pos_2);
		if (pos_4 == std::string::npos)
			break;

		if (pos_3 != std::string::npos && pos_3 < pos_4)
			pos_4 = sMessage.find(']', pos_4 + 1);

		std::string sArgs = sMessage.substr(pos_2, pos_4 - pos_2);

		CTokenVector kTokenVector;
		SplitLine(sArgs.c_str(), ";", &kTokenVector);

		if (kTokenVector.empty())
			break;

		BYTE bSetValue = 0;
		DWORD dwVnum = 0;
		try
		{
			bSetValue = std::stoul(kTokenVector[0]);
			dwVnum = std::stoul(kTokenVector[1]);
		}
		catch (const std::exception& ex)
		{
			TraceError("CPythonLocale::ReplacePreItemName: Error: %s", ex.what());
			break;
		}

		CItemData* pItemData;
		if (!CItemManager::Instance().GetItemDataPointer(dwVnum, &pItemData))
		{
			TraceError("CPythonLocale::ReplacePreItemName: can't find item vnum: %lu", dwVnum);
			break;
		}

		sMessage.replace(pos_1, (pos_4 + 1) - pos_1, CPythonItem::Instance().GetPreName(bSetValue, pItemData->GetName()));
	}
}
#endif

void CPythonLocale::ReplaceLocaleString(std::string& sMessage) const
{
	while (true)
	{
		size_t pos_1 = sMessage.rfind("[LS;");
		if (pos_1 == std::string::npos)
			break;

		size_t pos_2 = sMessage.find(';', pos_1);
		if (pos_2 == std::string::npos)
			break;

		++pos_2;

		size_t pos_3 = sMessage.find('[', pos_2);
		size_t pos_4 = sMessage.find(']', pos_2);
		if (pos_4 == std::string::npos)
			break;

		if (pos_3 != std::string::npos && pos_3 < pos_4)
			pos_4 = sMessage.find(']', pos_4 + 1);

		std::string sArgs = sMessage.substr(pos_2, pos_4 - pos_2);

		CTokenVector kTokenVector;
		SplitLine(sArgs.c_str(), ";", &kTokenVector);

		if (kTokenVector.empty())
			break;

		auto it = m_LocaleStringMap[LOCALE_STRING].find(kTokenVector[0]);
		if (it == m_LocaleStringMap[LOCALE_STRING].end())
		{
			TraceError("CPythonLocale::ReplaceLocaleString wrong LS vnum : %s", kTokenVector[0].c_str());
			break;
		}

		if (kTokenVector.size() > 1)
		{
			try
			{
				boost::format f(it->second);

				for (size_t i = 1; i < kTokenVector.size(); i++)
					f% kTokenVector[i];

				sMessage.replace(pos_1, (pos_4 + 1) - pos_1, f.str());
			}
			catch (const std::exception& ex)
			{
				TraceError("CPythonLocale::ReplaceLocaleString: Error: %s", ex.what());
				break;
			}
		}
		else
			sMessage.replace(pos_1, (pos_4 + 1) - pos_1, it->second);
	}
}

void CPythonLocale::ReplaceQuestLocaleString(std::string& sMessage) const
{
	while (true)
	{
		size_t pos_1 = sMessage.rfind("[LC;");
		if (pos_1 == std::string::npos)
			break;

		size_t pos_2 = sMessage.find(';', pos_1);
		if (pos_2 == std::string::npos)
			break;

		++pos_2;

		size_t pos_3 = sMessage.find('[', pos_2);
		size_t pos_4 = sMessage.find(']', pos_2);
		if (pos_4 == std::string::npos)
			break;

		if (pos_3 != std::string::npos && pos_3 < pos_4)
			pos_4 = sMessage.find(']', pos_4 + 1);

		std::string sArgs = sMessage.substr(pos_2, pos_4 - pos_2);

		CTokenVector kTokenVector;
		SplitLine(sArgs.c_str(), ";", &kTokenVector);

		if (kTokenVector.empty())
			break;

		auto it = m_LocaleStringMap[LOCALE_QUEST_STRING].find(kTokenVector[0]);
		if (it == m_LocaleStringMap[LOCALE_QUEST_STRING].end())
		{
			TraceError("CPythonLocale::ReplaceQuestLocaleString wrong LC vnum : %s", kTokenVector[0].c_str());
			break;
		}

		if (kTokenVector.size() > 1)
		{
			try
			{
				boost::format f(it->second);

				for (size_t i = 1; i < kTokenVector.size(); i++)
					f% kTokenVector[i];

				sMessage.replace(pos_1, (pos_4 + 1) - pos_1, f.str());
			}
			catch (const std::exception& ex)
			{
				TraceError("CPythonLocale::ReplaceQuestLocaleString: Error: %s", ex.what());
				break;
			}
		}
		else
			sMessage.replace(pos_1, (pos_4 + 1) - pos_1, it->second);
	}
}

void CPythonLocale::ReplaceOXQuizLocaleString(std::string& sMessage) const
{
	while (true)
	{
		size_t pos_1 = sMessage.rfind("[LOX;");
		if (pos_1 == std::string::npos)
			break;

		size_t pos_2 = sMessage.find(';', pos_1);
		if (pos_2 == std::string::npos)
			break;

		++pos_2;

		size_t pos_3 = sMessage.find('[', pos_2);
		size_t pos_4 = sMessage.find(']', pos_2);
		if (pos_4 == std::string::npos)
			break;

		if (pos_3 != std::string::npos && pos_3 < pos_4)
			pos_4 = sMessage.find(']', pos_4 + 1);

		std::string sArgs = sMessage.substr(pos_2, pos_4 - pos_2);

		CTokenVector kTokenVector;
		SplitLine(sArgs.c_str(), ";", &kTokenVector);

		if (kTokenVector.empty())
			break;

		auto it = m_LocaleStringMap[LOCALE_OXQUIZ_STRING].find(kTokenVector[0]);
		if (it == m_LocaleStringMap[LOCALE_OXQUIZ_STRING].end())
		{
			TraceError("CPythonLocale::ReplaceOXQuizLocaleString wrong LOX level-key : %s", kTokenVector[0].c_str());
			break;
		}

		if (kTokenVector.size() > 1)
		{
			try
			{
				boost::format f(it->second);

				for (size_t i = 1; i < kTokenVector.size(); i++)
					f% kTokenVector[i];

				sMessage.replace(pos_1, (pos_4 + 1) - pos_1, f.str());
			}
			catch (const std::exception& ex)
			{
				TraceError("CPythonLocale::ReplaceOXQuizLocaleString: Error: %s", ex.what());
				break;
			}
		}
		else
			sMessage.replace(pos_1, (pos_4 + 1) - pos_1, it->second);
	}
}

// 20221117.Owsap : Replace Hyperlink with correct locale item name.
#include "../EterBase/Utils.h"
void CPythonLocale::FilterHyperlinkName(std::string& sMessage) const
{
	std::string sHyperItemLinkBegin = "|Hitem";

	std::string::size_type pos = 0;
	while ((pos = sMessage.find(sHyperItemLinkBegin, pos)) != std::string::npos)
	{
		pos += sHyperItemLinkBegin.length();

		std::string element = sMessage.substr(pos, sMessage.length());

		if (element.find("[") != std::string::npos && element.find("]") != std::string::npos)
		{
			size_t pos_begin = element.find(':');
			if (pos_begin == std::string::npos)
				break;

			size_t pos_vnum = element.find_first_of(':', ++pos_begin);
			if (pos_vnum == std::string::npos)
				break;

#if defined(ENABLE_SET_ITEM)
			size_t pos_pre_set_value = element.find_first_of(':', pos_vnum + 1);
			if (pos_pre_set_value == std::string::npos)
				break;
#endif

			uint32_t dwItemVNum{};
#if defined(ENABLE_SET_ITEM)
			int iSetValue{};
#endif
			try
			{
				dwItemVNum = htoi(element.substr(pos_begin, pos_vnum - pos_begin).c_str());
#if defined(ENABLE_SET_ITEM)
				iSetValue = atoi(element.substr(pos_vnum + 1, pos_pre_set_value - pos_vnum).c_str());
#endif
			}
			catch (const std::exception& ex)
			{
				printf("CPythonLocale::FilterHyperlinkName: Error: %s", ex.what());
				break;
			}

			CItemData* pItemData;
			if (!CItemManager::Instance().GetItemDataPointer(dwItemVNum, &pItemData))
			{
				TraceError("CPythonLocale::FilterHyperlinkName: can't find item vnum: %lu", dwItemVNum);
				break;
			}

			size_t pos_name_begin = element.find('[');
			if (pos_name_begin == std::string::npos)
				break;

			size_t pos_name_end = element.find(']', ++pos_name_begin);
			if (pos_name_end == std::string::npos)
				break;

#if defined(ENABLE_SET_ITEM)
			sMessage.replace(pos + pos_name_begin, pos_name_end - pos_name_begin, CPythonItem::Instance().GetPreName(iSetValue, pItemData->GetName()));
#else
			sMessage.replace(pos + pos_name_begin, pos_name_end - pos_name_begin, pItemData->GetName());
#endif
		}
	}
}
#endif
