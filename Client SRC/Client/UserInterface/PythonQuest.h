#pragma once

class CPythonQuest : public CSingleton<CPythonQuest>
{
public:
	struct SQuestInstance
	{
		SQuestInstance()
		{
			dwIndex = 0;
#if defined(ENABLE_QUEST_RENEWAL)
			bType = 0;
			bIsConfirmed = false;
#endif
			iClockValue = 0;
			iCounterValue = 0;
			iStartTime = 0;
		}

		DWORD dwIndex;
#if defined(ENABLE_QUEST_RENEWAL)
		BYTE bType;
		bool bIsConfirmed;
#endif

		std::string	strIconFileName;
		std::string	strTitle;

		std::string	strClockName;
		std::string	strCounterName;

		int iClockValue;
		int iCounterValue;

		int iStartTime;
	};
	typedef std::vector<SQuestInstance> TQuestInstanceContainer;

#if defined(ENABLE_QUEST_RENEWAL)
	enum EQuestStringType
	{
		QUEST_STRING_TYPE_NORMAL,
		QUEST_STRING_TYPE_CLOCK,
		QUEST_STRING_TYPE_COUNT,
		QUEST_STRING_TYPE_MAX
	};

	enum EQuestType
	{
		QUEST_TYPE_MAIN,
		QUEST_TYPE_SUB,
		QUEST_TYPE_LEVELUP,
		QUEST_TYPE_EVENT,
		QUEST_TYPE_COLLECTION,
		QUEST_TYPE_SYSTEM,
		QUEST_TYPE_SCROLL,
		QUEST_TYPE_DAILY,
		QUEST_TYPE_UNEXPOSED,
		QUEST_TYPE_MAX
	};

	enum EQuestSkin
	{
		QUEST_SKIN_NOWINDOW,
		QUEST_SKIN_NORMAL,
		QUEST_SKIN_UNKOWN1,
		QUEST_SKIN_UNKOWN2,
		QUEST_SKIN_SCROLL,
		QUEST_SKIN_CINEMATIC,
		QUEST_SKIN_COUNT,
		QUEST_SKIN_MAX
	};
#endif

public:
	CPythonQuest();
	virtual ~CPythonQuest();

	void Clear();

	void RegisterQuestInstance(const SQuestInstance& c_rQuestInstance);
	void DeleteQuestInstance(DWORD dwIndex);

	bool IsQuest(DWORD dwIndex);
	void MakeQuest(DWORD dwIndex
#if defined(ENABLE_QUEST_RENEWAL)
		, BYTE bIndex
		, bool bIsConfirmed
#endif
	);

	void SetQuestTitle(DWORD dwIndex, const char* c_szTitle);
	void SetQuestClockName(DWORD dwIndex, const char* c_szClockName);
	void SetQuestCounterName(DWORD dwIndex, const char* c_szCounterName);
	void SetQuestClockValue(DWORD dwIndex, int iClockValue);
	void SetQuestCounterValue(DWORD dwIndex, int iCounterValue);
	void SetQuestIconFileName(DWORD dwIndex, const char* c_szIconFileName);
#if defined(ENABLE_QUEST_RENEWAL)
	void SetQuestIsConfirmed(DWORD dwIndex, bool bIsConfirmed);
#endif

	int GetQuestCount();
#if defined(ENABLE_QUEST_RENEWAL)
	int GetQuestButtonNoticeCount(BYTE bQuestType);
#endif

#if defined(ENABLE_QUEST_RENEWAL)
	bool GetQuestInstancePtr(DWORD dwQuestIndex, SQuestInstance** ppQuestInstance);
#else
	bool GetQuestInstancePtr(DWORD dwArrayIndex, SQuestInstance** ppQuestInstance);
#endif

protected:
	void __Initialize();
	bool __GetQuestInstancePtr(DWORD dwQuestIndex, SQuestInstance** ppQuestInstance);

protected:
	TQuestInstanceContainer m_QuestInstanceContainer;
};
