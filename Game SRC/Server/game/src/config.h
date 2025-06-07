#ifndef __INC_CONFIG_H__
#define __INC_CONFIG_H__

enum
{
	ADDRESS_MAX_LEN = 15
};

void config_init(const std::string& st_localeServiceName); // default "" is CONFIG

extern char sql_addr[256];

extern WORD mother_port;
extern WORD p2p_port;

extern char db_addr[ADDRESS_MAX_LEN + 1];
extern WORD db_port;

extern int passes_per_sec;
extern int save_event_second_cycle;
extern int ping_event_second_cycle;
extern int test_server;
extern int packet_analyze;
extern bool guild_mark_server;
extern BYTE guild_mark_min_level;
extern bool distribution_test_server;
extern bool china_event_server;

extern bool g_bNoMoreClient;
extern bool g_bNoRegen;

extern bool g_bTrafficProfileOn; ///< true 이면 TrafficProfiler 를 켠다.

extern BYTE g_bChannel;

extern bool map_allow_find(int index);
extern void map_allow_copy(long* pl, int size);
extern bool no_wander;

extern int g_iUserLimit;
extern time_t g_global_time;

const char* get_table_postfix();

extern std::string g_stHostname;
extern std::string g_stLocale;
extern std::string g_stLocaleFilename;

extern char g_szPublicIP[16];
extern char g_szInternalIP[16];
#if defined(__PROXY_IP__)
extern std::string g_stProxyIP;
#endif

extern int (*is_twobyte) (const char* str);
extern int (*check_name) (const char* str);

extern bool g_bSkillDisable;

extern int g_iFullUserCount;
extern int g_iBusyUserCount;
extern void LoadStateUserCount();

extern bool g_bEmpireWhisper;

extern BYTE g_bAuthServer;

extern BYTE PK_PROTECT_LEVEL;

extern void LoadValidCRCList();
extern bool IsValidProcessCRC(DWORD dwCRC);
extern bool IsValidFileCRC(DWORD dwCRC);

extern std::string g_stAuthMasterIP;
extern WORD g_wAuthMasterPort;

extern std::string g_stClientVersion;
extern bool g_bCheckClientVersion;
extern void CheckClientVersion();

extern std::string g_stQuestDir;
//extern std::string g_stQuestObjectDir;
extern std::set<std::string> g_setQuestObjectDir;

extern std::vector<std::string> g_stAdminPageIP;
extern std::string g_stAdminPagePassword;

extern int SPEEDHACK_LIMIT_COUNT;
extern int SPEEDHACK_LIMIT_BONUS;

extern int g_iSyncHackLimitCount;

extern int g_server_id;
extern std::string g_strWebMallURL;
#if defined(__POPUP_NOTICE__)
extern std::string g_strWebPopupNoticeURL;
#endif

extern int VIEW_RANGE;
extern int VIEW_BONUS_RANGE;

extern bool g_bCheckMultiHack;
extern bool g_protectNormalPlayer; // 범법자가 "평화모드" 인 일반유저를 공격하지 못함
extern bool g_noticeBattleZone; // 중립지대에 입장하면 안내메세지를 알려줌
extern bool g_bSafeboxChangePasswordBuff;

extern DWORD g_GoldDropTimeLimitValue;
#if defined(__CHEQUE_SYSTEM__)
extern DWORD g_ChequeDropTimeLimitValue;
#endif

extern int gPlayerMaxLevel;
#if defined(__CONQUEROR_LEVEL__)
extern int gPlayerMaxConquerorLevel;
#endif
extern int gPlayerMaxLevelStats;

extern bool gHackCheckEnable;

extern bool g_BlockCharCreation;

/// OSF Configs
extern bool g_bBootaryCheck;
extern bool g_bAttrLog;
extern bool g_bWhisperLog;
extern bool g_bGlobalShout;
extern bool g_bDisableShout;
extern bool g_bDisableEmotionMask;
extern bool g_bDisableGlassInsight;
extern bool g_bDisableDSQualification;
extern bool g_bChangeItemAttrCycle;
extern bool g_bCreateWithFullSet;
#if defined(__SOUL_BIND_SYSTEM__)
extern bool g_bSoulBind;
#endif
extern bool g_bWolfmanCreation;
extern bool g_bDisableBossKnockback;

extern bool g_bIgnoreDisallowedMap;
extern bool g_bTestMobLog;

extern bool g_bUnlimitedCapeOfCourage;
extern bool g_bUnlimitedAddAttributes;
extern bool g_bUnlimitedChangeAttributes;
extern bool g_bUnlimitedAddRareAttributes;
extern bool g_bUnlimitedChangeRareAttributes;
extern bool g_bUnlimitedChangeCostumeAttributes;
extern bool g_bUnlimitedResetCostumeAttributes;
extern bool g_bUnlimitedCleanSocket;
extern bool g_bUnlimitedAddAccessorySocket;
extern bool g_bUnlimitedAddAccessory;

extern bool g_bNeverFailAddAttributes;
extern bool g_bNeverFailAccessorySocket;
extern bool g_bNeverFailAccessory;
extern bool g_bNeverFailMetin;

extern int g_npcGroupRespawnRange[];
/// END_OF_OSP.CONFIGS

#endif /* __INC_CONFIG_H__ */
