/**
* Filename: guild_dragonlair.h
* Author: Owsap
**/

#if !defined(__INC_GUILD_DRAGONLAIR_SYSTEM_H__) && defined(__GUILD_DRAGONLAIR_SYSTEM__)
#define __INC_GUILD_DRAGONLAIR_SYSTEM_H__

enum EGuildDragonLairLimit
{
	GUILD_DRAGONLAIR_STONE_COUNT = 4,
};

enum EGuildDragonLairUnique
{
	GUILD_DRAGONLAIR_UNIQUE_NPC,
	GUILD_DRAGONLAIR_UNIQUE_GATE,
	GUILD_DRAGONLAIR_UNIQUE_STONE,
	GUILD_DRAGONLAIR_UNIQUE_KING
};

enum EGuildDragonLairStage
{
	GUILD_DRAGONLAIR_STAGE_NONE,
	GUILD_DRAGONLAIR_STAGE1,
	GUILD_DRAGONLAIR_STAGE2,
	GUILD_DRAGONLAIR_STAGE3,
	GUILD_DRAGONLAIR_STAGE4,
	GUILD_DRAGONLAIR_STAGE_REWARD,
	GUILD_DRAGONLAIR_STAGE_MAX
};

class CGuildDragonLair
{
public:
	typedef std::unordered_set<LPCHARACTER> TCharacterSet;
	typedef std::unordered_set<DWORD> TPlayerIDSet;
	typedef std::unordered_map<DWORD, BYTE> TStoneAffectMap;
	typedef std::unordered_map<std::string, LPCHARACTER> TUniqueMobMap;

public:
	CGuildDragonLair(BYTE bType, long lOriginalMapIndex, long lPrivateMapIndex);
	~CGuildDragonLair();

	void Initialize();
	void Destroy();

public:
	bool Start();
	bool Cancel();
	bool End(bool bSuccess);

	void IncMember(LPCHARACTER pChar);
	void DecMember(LPCHARACTER pChar);

	bool Exit(LPCHARACTER pChar);
	void ExitAll();

	void ClearStage();
	void SetStage(BYTE bStage);

	void DeadCharacter(LPCHARACTER pChar);

	LPCHARACTER GetUnique(const std::string& strKey) const;
	bool IsUnique(LPCHARACTER pChar) const;

	void KillUnique(const std::string& strKey);
	void PurgeUnique(const std::string& strKey);

	void SetStoneAffect(LPCHARACTER pStone, BYTE bEffectNum, bool bSave = false);
	bool GetStoneAffect(LPCHARACTER pStone, BYTE bEffectNum) const;
	size_t GetAllStoneAffectCount(BYTE bEffectNum) const;

	void LockStones();
	void UnlockStones();

	bool Damage(LPCHARACTER pVictim);
	void Kill(LPCHARACTER pVictim);
	void TakeItem(LPCHARACTER pFrom, LPCHARACTER pTo, LPITEM pItem);

	bool GiveReward(LPCHARACTER pChar, DWORD dwVnum);
	bool GetReward(LPCHARACTER pChar);

	void ChatPacket(BYTE bChatType, const char* szText);

	void SetEndEvent(LPEVENT pEvent);
	void SetGateEvent(LPEVENT pEvent);
	void SetStoneEvent(LPEVENT pEvent);
	void SetSpawnEvent(LPEVENT pEvent);
	void SetEggEvent(LPEVENT pEvent);
	void SetExitEvent(LPEVENT pEvent);

	void CancelAllEvents();

	LPCHARACTER Spawn(DWORD dwVnum, int iX, int iY, int iDir = 0, bool bSpawnMotion = false);

	void RegisterGuildRanking(BYTE bType, DWORD dwGuildID, BYTE bMemberCount, DWORD dwTime);

#if defined(__GUILD_DRAGONLAIR_PARTY_SYSTEM__)
	void GetElapsedTime(int& iMinutes, int& iSeconds);

	bool CheckFirstRankingTimePassed();

	void SendFirstRankingTimeStart();
	void SendFirstRankingTimeFailed();
	void SendFirstRankingTimeSuccess();
	void SendFirstRankingTime(DWORD dwFirstRankingTime);
	void SendFirstRankingTime(LPCHARACTER pChar);
#endif

public:
	BYTE GetStage() const { return m_bStage; }

	void SetGuild(LPGUILD pGuild) { m_pGuild = pGuild; }
	LPGUILD GetGuild() const { return m_pGuild; }

	bool IsGeneral(LPCHARACTER pChar) { return m_set_pChar.find(pChar) != m_set_pChar.end(); }
	size_t GetMemberCount() { return m_set_pChar.size(); }

#if defined(__GUILD_DRAGONLAIR_PARTY_SYSTEM__)
	void SetParty(LPPARTY pParty) { m_pParty = pParty; }
	LPPARTY GetParty() const { return m_pParty; }
#endif

	long GetOriginalMapIndex() const { return m_lOriginalMapIndex; }
	long GetPrivateMapIndex() const { return m_lPrivateMapIndex; }
	long GetMapIndex() const { return m_lPrivateMapIndex; }
	LPSECTREE_MAP GetSectree() const { return m_pSectree; }

	void SetCenterPos(long x, long y) { m_lCenterX = x, m_lCenterY = y; }
	long GetCenterX() const { return m_lCenterX; }
	long GetCenterY() const { return m_lCenterY; }

	bool IsExit(DWORD dwPID) { return m_set_dwPID_Exit.find(dwPID) != m_set_dwPID_Exit.end(); }

	void SetEggKillCount(BYTE bCount) { m_bEggKillCount = bCount; }
	BYTE GetEggKillCount() const { return m_bEggKillCount; }

	void SetBossKillCount(BYTE bCount) { m_bBossKillCount = bCount; }
	BYTE GetBossKillCount() const { return m_bBossKillCount; }

	void SetStartTime(DWORD dwTimeStamp) { m_dwStartTime = dwTimeStamp; }
	DWORD GetStartTime() const { return m_dwStartTime; }

#if defined(__GUILD_DRAGONLAIR_PARTY_SYSTEM__)
	void SetFirstRankingTime(DWORD dwTime) { m_dwFirstRankingTime = dwTime; }
	DWORD GetFirstRankingTime() const { return m_dwFirstRankingTime; }
#endif

	void SetSpecialAttack(bool bSet) { m_bSpecialAttack = bSet; }
	bool GetSpecialAttack() const { return m_bSpecialAttack; }

	void SetDungeonType(const char* pszDungeonType) { m_szDungeonType = pszDungeonType; }
	const char* GetDungeonType() const { return m_szDungeonType; }

private:
	BYTE m_bType;
	BYTE m_bStage;

	long m_lOriginalMapIndex;
	long m_lPrivateMapIndex;
	LPSECTREE_MAP m_pSectree;

	long m_lCenterX;
	long m_lCenterY;

	TCharacterSet m_set_pChar;

	TPlayerIDSet m_set_dwPID_Exit;
	TPlayerIDSet m_set_dwPID_Stone;
	TPlayerIDSet m_set_dwPID_Reward;

	TStoneAffectMap m_set_dwStoneAffect;

	LPGUILD m_pGuild;
#if defined(__GUILD_DRAGONLAIR_PARTY_SYSTEM__)
	LPPARTY m_pParty;
#endif

	TUniqueMobMap m_map_UniqueMob;

	BYTE m_bEggKillCount;
	BYTE m_bBossKillCount;

	DWORD m_dwStartTime;
#if defined(__GUILD_DRAGONLAIR_PARTY_SYSTEM__)
	DWORD m_dwFirstRankingTime;
#endif

	bool m_bSpecialAttack;
	const char* m_szDungeonType;

	LPEVENT m_pEndEvent;
	LPEVENT m_pGateEvent;
	LPEVENT m_pStoneEvent;
	LPEVENT m_pSpawnEvent;
	LPEVENT m_pEggEvent;
	LPEVENT m_pExitEvent;
};

class CGuildDragonLairManager : public singleton<CGuildDragonLairManager>
{
	typedef std::unordered_map<long, CGuildDragonLair*> TMapDragonLair;
	typedef std::unordered_map<LPGUILD, CGuildDragonLair*> TGuildDragonLair;
#if defined(__GUILD_DRAGONLAIR_PARTY_SYSTEM__)
	typedef std::unordered_map<LPPARTY, CGuildDragonLair*> TPartyDragonLair;
#endif

public:
	CGuildDragonLairManager();
	virtual ~CGuildDragonLairManager();

	CGuildDragonLair* Create(BYTE bType, long lOriginalMapIndex);
	CGuildDragonLair* FindByMapIndex(long lMapIndex) const;

	void Destroy();
	void DestroyPrivateMap(long lMapIndex);

public:
	CGuildDragonLair* GetGuildDragonLair(LPGUILD pGuild) const;
#if defined(__GUILD_DRAGONLAIR_PARTY_SYSTEM__)
	CGuildDragonLair* GetGuildDragonLair(LPPARTY pParty) const;
#endif

	bool FindGuild(DWORD dwGuildID);
	bool CanRegister(LPCHARACTER pChar);
	bool RegisterGuild(LPCHARACTER pChar, BYTE bType, bool bUseTicket = false);
	bool EnterGuild(LPCHARACTER pChar, BYTE bType);
	BYTE GetGuildStage(DWORD dwGuildID) const;
	size_t GetGuildMemberCount(DWORD dwGuildID) const;

#if defined(__GUILD_DRAGONLAIR_PARTY_SYSTEM__)
	bool FindParty(DWORD dwLeaderPID);
	bool EnterParty(LPCHARACTER pChar, BYTE bType);
	BYTE GetPartyStage(DWORD dwLeaderPID) const;
	size_t GetPartyMemberCount(DWORD dwLeaderPID) const;

	DWORD GetFirstGuildRankingTime() const;
	DWORD GetFirstPartyRankingTime() const;
#endif

public:
	bool Exit(LPCHARACTER pChar);
	bool Start(LPCHARACTER pChar);
	bool Cancel(LPCHARACTER pChar);
	bool CancelGuild(const char* pszGuildName);

public:
	bool IsExit(LPCHARACTER pChar) const;
	BYTE GetStage(LPCHARACTER pChar) const;
	bool GiveReward(LPCHARACTER pChar, DWORD dwVnum);
	bool GetReward(LPCHARACTER pChar) const;

public:
	bool IsUnique(LPCHARACTER pChar) const;
	bool IsStone(DWORD dwRaceNum) const;
	bool IsEgg(DWORD dwRaceNum) const;
	bool IsBoss(DWORD dwRaceNum) const;
	bool IsKing(DWORD dwRaceNum) const;
	bool IsRedDragonLair(long lMapIndex) const;

private:
	TMapDragonLair m_map_pMapDragonLair;
	TGuildDragonLair m_map_pGuildDragonLair;
#if defined(__GUILD_DRAGONLAIR_PARTY_SYSTEM__)
	TPartyDragonLair m_map_pPartyDragonLair;
#endif
};

extern unsigned int GuildDragonLair_GetFactor(const size_t cnt, ...);
extern unsigned int GuildDragonLair_GetIndexFactor(const char* container, const size_t idx, const char* key);
extern unsigned int GuildDragonLair_GetIndexFactor2(const char* container, const char* sub_container, const size_t idx, const char* key);

#endif // __GUILD_DRAGONLAIR_SYSTEM__
