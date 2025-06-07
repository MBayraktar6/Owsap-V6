#pragma once

#include "Packet.h"

class CPythonGuild : public CSingleton<CPythonGuild>
{
public:
	enum
	{
		GUILD_SKILL_MAX_NUM = 12,
		ENEMY_GUILD_SLOT_MAX_COUNT = 6,
	};

	typedef struct SGulidInfo
	{
		DWORD dwGuildID;
		char szGuildName[GUILD_NAME_MAX_LEN + 1];
		DWORD dwMasterPID;
		DWORD dwGuildLevel;
		DWORD dwCurrentExperience;
		DWORD dwCurrentMemberCount;
		DWORD dwMaxMemberCount;
		DWORD dwGuildMoney;
		BOOL bHasLand;
	} TGuildInfo;

	typedef struct SGuildGradeData
	{
		SGuildGradeData() {}
		SGuildGradeData(BYTE byAuthorityFlag_, const char* c_szName_) : byAuthorityFlag(byAuthorityFlag_), strName(c_szName_) {}
		BYTE byAuthorityFlag;
		std::string strName;
	} TGuildGradeData;
	typedef std::map<BYTE, TGuildGradeData> TGradeDataMap;

	typedef struct SGuildMemberData
	{
		DWORD dwPID;

		std::string strName;
		BYTE byGrade;
		BYTE byJob;
		BYTE byLevel;
		BYTE byGeneralFlag;
		DWORD dwOffer;
	} TGuildMemberData;
	typedef std::vector<TGuildMemberData> TGuildMemberDataVector;

	typedef struct SGuildBoardCommentData
	{
		DWORD dwCommentID;
		std::string strName;
		std::string strComment;
	} TGuildBoardCommentData;
	typedef std::vector<TGuildBoardCommentData> TGuildBoardCommentDataVector;

	typedef struct SGuildSkillData
	{
		BYTE bySkillPoint;
		BYTE bySkillLevel[GUILD_SKILL_MAX_NUM];
		WORD wGuildPoint;
		WORD wMaxGuildPoint;
	} TGuildSkillData;

	typedef std::map<DWORD, std::string> TGuildNameMap;

#if defined(ENABLE_GUILD_DRAGONLAIR_SYSTEM)
	typedef struct SDragonLairRankingData
	{
		SDragonLairRankingData(DWORD dwGuildID, const char* pszGuildName, BYTE bMemberCount, DWORD dwTime) :
			dwGuildID(dwGuildID), strGuildName(pszGuildName), bMemberCount(bMemberCount), dwTime(dwTime) {}
		DWORD dwGuildID;
		std::string strGuildName;
		BYTE bMemberCount;
		DWORD dwTime;
	} TDragonLairRankingData;

	typedef std::vector<std::shared_ptr<SDragonLairRankingData>> TDragonLairRankingVector;
	typedef std::map<BYTE, TDragonLairRankingVector> TDragonLairRankingMap;
#endif

public:
	CPythonGuild();
	virtual ~CPythonGuild();

	void Destroy();

	void EnableGuild();
	void SetGuildMoney(DWORD dwMoney);
	void SetGuildEXP(BYTE byLevel, DWORD dwEXP);
	void SetGradeData(BYTE byGradeNumber, TGuildGradeData& rGuildGradeData);
	void SetGradeName(BYTE byGradeNumber, const char* c_szName);
	void SetGradeAuthority(BYTE byGradeNumber, BYTE byAuthority);
	void ClearComment();
	void RegisterComment(DWORD dwCommentID, const char* c_szName, const char* c_szComment);
	void RegisterMember(TGuildMemberData& rGuildMemberData);
	void ChangeGuildMemberGrade(DWORD dwPID, BYTE byGrade);
	void ChangeGuildMemberGeneralFlag(DWORD dwPID, BYTE byFlag);
	void RemoveMember(DWORD dwPID);
	void RegisterGuildName(DWORD dwID, const char* c_szName);

	BOOL IsMainPlayer(DWORD dwPID);
	BOOL IsGuildEnable();
	TGuildInfo& GetGuildInfoRef();
	BOOL GetGradeDataPtr(DWORD dwGradeNumber, TGuildGradeData** ppData);
	const TGuildBoardCommentDataVector& GetGuildBoardCommentVector();
	DWORD GetMemberCount();
	BOOL GetMemberDataPtr(DWORD dwIndex, TGuildMemberData** ppData);
	BOOL GetMemberDataPtrByPID(DWORD dwPID, TGuildMemberData** ppData);
	BOOL GetMemberDataPtrByName(const char* c_szName, TGuildMemberData** ppData);
	DWORD GetGuildMemberLevelSummary();
	DWORD GetGuildMemberLevelAverage();
	DWORD GetGuildExperienceSummary();
	TGuildSkillData& GetGuildSkillDataRef();
	bool GetGuildName(DWORD dwID, std::string* pstrGuildName);
	DWORD GetGuildID();
	BOOL HasGuildLand();

	void StartGuildWar(DWORD dwEnemyGuildID);
	void EndGuildWar(DWORD dwEnemyGuildID);
	DWORD GetEnemyGuildID(DWORD dwIndex);
	BOOL IsDoingGuildWar();

#if defined(ENABLE_GUILD_DRAGONLAIR_SYSTEM)
public:
	TDragonLairRankingData* GetDragonLairRanking(BYTE bType, DWORD dwIndex);
	TDragonLairRankingData* GetMyDragonLairRanking(BYTE bType, DWORD& dwIndex);
	size_t GetDragonLairRankingShowCount(BYTE bType);

	bool CheckDragonLairRanking(BYTE bType);
	bool IsDragonLairClearGuild(BYTE bType);
	void ClearDragonLairRanking(BYTE bType);
	void RegisterDragonLairRanking(BYTE bType, DWORD dwGuildID, const char* pszGuildName, BYTE bMemberCount, DWORD dwTime);

protected:
	TDragonLairRankingMap m_DragonLairRankingMap;
	bool m_bCheckDragonLairRanking[GUILD_DRAGONLAIR_TYPE_MAX_NUM];
#endif

protected:
	void __CalculateLevelAverage();
	void __SortMember();
	BOOL __IsGradeData(BYTE byGradeNumber);

	void __Initialize();

protected:
	TGuildInfo m_GuildInfo;
	TGradeDataMap m_GradeDataMap;
	TGuildMemberDataVector m_GuildMemberDataVector;
	TGuildBoardCommentDataVector m_GuildBoardCommentVector;
	TGuildSkillData m_GuildSkillData;
	TGuildNameMap m_GuildNameMap;
	DWORD m_adwEnemyGuildID[ENEMY_GUILD_SLOT_MAX_COUNT];

	DWORD m_dwMemberLevelSummary;
	DWORD m_dwMemberLevelAverage;
	DWORD m_dwMemberExperienceSummary;

	BOOL m_bGuildEnable;
};
