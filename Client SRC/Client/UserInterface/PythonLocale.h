/**
* Filename: PythonLocale.h
* Author: Owsap, blackdragonx61 (aka. Mali)
**/

#pragma once

#if defined(ENABLE_LOCALE_CLIENT)
class CPythonLocale : public CSingleton<CPythonLocale>
{
public:
	CPythonLocale();
	virtual ~CPythonLocale();

	enum ELOCALE_STRING_TYPE
	{
		LOCALE_STRING,
		LOCALE_QUEST_STRING,
		LOCALE_OXQUIZ_STRING,

		LOCALE_STRING_MAX
	};

	bool LoadLocaleString(const char* c_szFileName);
	bool LoadQuestLocaleString(const char* c_szFileName, bool bOverride = false);
	bool LoadOXQuizLocaleString(const char* c_szFileName);

#if defined(ENABLE_LOADING_TIP)
	bool LoadLoadingTipVnum(const char* c_szFileName);
	bool LoadLoadingTipList(const char* c_szFileName);
	const char* GetLoadingTipVnum(int iMapIndex);
#endif

	void FormatString(std::string& sMessage) const;
	void FormatString(char* buf, size_t size) const;

	void FilterHyperlinkName(std::string& sMessage) const;

	void MultiLineSplit(const std::string& sMessage, TTokenVector& vec) const;

private:
	void ReplaceSkillName(std::string& sMessage) const;
	void ReplacePetSkillName(std::string& sMessage) const;
	void ReplaceMobName(std::string& sMessage) const;
	void ReplaceItemName(std::string& sMessage) const;
#if defined(ENABLE_SET_ITEM)
	void ReplacePreItemName(std::string& sMessage) const;
#endif
	void ReplaceLocaleString(std::string& sMessage) const;
	void ReplaceQuestLocaleString(std::string& sMessage) const;
	void ReplaceOXQuizLocaleString(std::string& sMessage) const;

	std::unordered_map<std::string, std::string> m_LocaleStringMap[LOCALE_STRING_MAX];

#if defined(ENABLE_LOADING_TIP)
private:
	std::map<DWORD, std::vector<DWORD>> m_kMap_dwMapIndex_vecTipList;
	std::map<DWORD, std::string> m_kMap_dwID_strTipString;
#endif
};
#endif
