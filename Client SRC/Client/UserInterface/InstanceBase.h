#pragma once

#include "../GameLib/RaceData.h"
#include "../GameLib/ActorInstance.h"
#include "../EterLib/GrpObjectInstance.h"

#include "AffectFlagContainer.h"

class CInstanceBase
{
public:
	struct SCreateData
	{
		BYTE m_bType;
		DWORD m_dwStateFlags;
		DWORD m_dwEmpireID;
		DWORD m_dwGuildID;
		DWORD m_dwLevel;
#if defined(ENABLE_CONQUEROR_LEVEL)
		DWORD m_dwConquerorLevel;
#endif
#ifdef WJ_SHOW_MOB_INFO
		DWORD m_dwAIFlag;
#endif
		DWORD m_dwVID;
		DWORD m_dwRace;
#if defined(ENABLE_RACE_SWAP)
		DWORD m_dwEventRace;
#endif
		DWORD m_dwMovSpd;
		DWORD m_dwAtkSpd;
		LONG m_lPosX;
		LONG m_lPosY;
		FLOAT m_fRot;
		DWORD m_dwArmor;
		DWORD m_dwWeapon;
		DWORD m_dwHair;
#if defined(ENABLE_ACCE_COSTUME_SYSTEM)
		DWORD m_dwAcce;
#endif
#if defined(ENABLE_AURA_COSTUME_SYSTEM)
		DWORD m_dwAura;
#endif
		DWORD m_dwMountVnum;
#if defined(ENABLE_QUIVER_SYSTEM)
		DWORD m_dwArrow;
#endif
#if defined(ENABLE_REFINE_ELEMENT_SYSTEM)
		WORD m_wRefineElementAffectType;
#endif
#if defined(ENABLE_GUILD_LEADER_GRADE_NAME)
		BYTE m_bGuildLeaderGrade;
#endif

#if defined(ENABLE_MULTI_LANGUAGE_SYSTEM)
		std::string m_stCountry;
#endif

		short m_sAlignment;
		BYTE m_byPKMode;

		CAffectFlagContainer m_kAffectFlags;

		std::string m_stName;

		bool m_isMain;
	};

public:
	typedef DWORD TType;

#ifdef WJ_SHOW_MOB_INFO
	enum EMobAIFlags
	{
		AIFLAG_AGGRESSIVE = (1 << 0),
		AIFLAG_NOMOVE = (1 << 1),
		AIFLAG_COWARD = (1 << 2),
		AIFLAG_NOATTACKSHINSU = (1 << 3),
		AIFLAG_NOATTACKCHUNJO = (1 << 4),
		AIFLAG_NOATTACKJINNO = (1 << 5),
		AIFLAG_ATTACKMOB = (1 << 6),
		AIFLAG_BERSERK = (1 << 7),
		AIFLAG_STONESKIN = (1 << 8),
		AIFLAG_GODSPEED = (1 << 9),
		AIFLAG_DEATHBLOW = (1 << 10),
		AIFLAG_REVIVE = (1 << 11),
		AIFLAG_HEALER = (1 << 12),
		AIFLAG_COUNT = (1 << 13),
		AIFLAG_NORECOVERY = (1 << 14),
		AIFLAG_REFLECT = (1 << 15),
		AIFLAG_FALL = (1 << 16),
		AIFLAG_VIT = (1 << 17),
		AIFLAG_RATTSPEED = (1 << 18),
		AIFLAG_RCASTSPEED = (1 << 19),
		AIFLAG_RHP_REGEN = (1 << 20),
		AIFLAG_TIMEVIT = (1 << 21),
	};
#endif

	enum EImmuneFlags
	{
		IMMUNE_STUN = (1 << 0),
		IMMUNE_SLOW = (1 << 1),
		IMMUNE_FALL = (1 << 2),
		IMMUNE_CURSE = (1 << 3),
		IMMUNE_POISON = (1 << 4),
		IMMUNE_TERROR = (1 << 5),
		IMMUNE_REFLECT = (1 << 6),
	};

	enum EDirection
	{
		DIR_NORTH,
		DIR_NORTHEAST,
		DIR_EAST,
		DIR_SOUTHEAST,
		DIR_SOUTH,
		DIR_SOUTHWEST,
		DIR_WEST,
		DIR_NORTHWEST,
		DIR_MAX_NUM,
	};

	enum
	{
		FUNC_WAIT,
		FUNC_MOVE,
		FUNC_ATTACK,
		FUNC_COMBO,
		FUNC_MOB_SKILL,
		FUNC_EMOTION,
		FUNC_SKILL = 0x80,
	};

	enum EAffectBits
	{
		AFFECT_YMIR,
		AFFECT_INVISIBILITY,
		AFFECT_SPAWN,

		AFFECT_POISON,
		AFFECT_SLOW,
		AFFECT_STUN,

		AFFECT_DUNGEON_READY, // 던전에서 준비 상태
		AFFECT_SHOW_ALWAYS, // AFFECT_DUNGEON_UNIQUE 에서 변경(클라이언트에서 컬링되지않음)

		AFFECT_BUILDING_CONSTRUCTION_SMALL, // AFF_SHACKLE
		AFFECT_BUILDING_CONSTRUCTION_LARGE,
		AFFECT_BUILDING_UPGRADE,

		AFFECT_MOV_SPEED_POTION,
		AFFECT_ATT_SPEED_POTION,

		AFFECT_FISH_MIND,

		AFFECT_JEONGWI, // 전귀혼
		AFFECT_GEOMGYEONG, // 검경
		AFFECT_CHEONGEUN, // 천근추
		AFFECT_GYEONGGONG, // 경공술
		AFFECT_EUNHYEONG, // 은형법
		AFFECT_GWIGEOM, // 귀검
		AFFECT_GONGPO, // 공포
		AFFECT_JUMAGAP, // 주마갑
		AFFECT_HOSIN, // 호신
		AFFECT_BOHO, // 보호
		AFFECT_KWAESOK, // 쾌속
		AFFECT_HEUKSIN, // 흑신수호
		AFFECT_MUYEONG, // 무영진
		AFFECT_REVIVE_INVISIBILITY, // 부활 무적
		AFFECT_FIRE, // 지속 불
		AFFECT_GICHEON, // 기천 대공
		AFFECT_JEUNGRYEOK, // 증력술 
		AFFECT_DASH, // 대쉬
		AFFECT_PABEOP, // 파법술
		AFFECT_FALLEN_CHEONGEUN, // 다운 그레이드 천근추

		AFFECT_POLYMORPH, // 폴리모프

		AFFECT_WAR_FLAG1,
		AFFECT_WAR_FLAG2,
		AFFECT_WAR_FLAG3,

		AFFECT_CHINA_FIREWORK,
		AFFECT_CANNOT_ATTACK,
		AFFECT_CANNOT_USE_SKILL,
		AFFECT_DS,

		AFFECT_BLEEDING,
		AFFECT_RED_POSSESSION,
		AFFECT_BLUE_POSSESSION,

		AFFECT_UNK46,
		AFFECT_UNK47,
#if defined(ENABLE_GUILD_DRAGONLAIR_SYSTEM)
		AFFECT_UNBEATABLE,
#endif

		AFFECT_BATTLE_FIELD_RANK1,
		AFFECT_BATTLE_FIELD_RANK2,
		AFFECT_BATTLE_FIELD_RANK3,
		AFFECT_TARGET_VICTIM,

		AFFECT_UNK53,

		AFFECT_ELECTRIC_SHOCK,
		AFFECT_CONFUSION,

#if defined(ENABLE_SOUL_SYSTEM)
		AFFECT_SOUL_RED,
		AFFECT_SOUL_BLUE,
		AFFECT_SOUL_MIX,
#endif

		AFFECT_UNK59,
		AFFECT_BATTLE_ROYALE_SLOW_1,
		AFFECT_BATTLE_ROYALE_SLOW_2,
		AFFECT_BATTLE_ROYALE_SLOW_3,

		AFFECT_UNK63,
		AFFECT_PASWAE,
		AFFECT_UNK65,

		AFFECT_ELEMENT_BUFF_CRACK_NONE,
		AFFECT_ELEMENT_BUFF_CRACK_FIRE,
		AFFECT_ELEMENT_BUFF_CRACK_ICE,
		AFFECT_ELEMENT_BUFF_CRACK_ELECT,
		AFFECT_ELEMENT_BUFF_CRACK_WIND,
		AFFECT_ELEMENT_BUFF_CRACK_EARTH,
		AFFECT_ELEMENT_BUFF_CRACK_DARK,

		AFFECT_FLAG_UNK73, // guild_battle (blue)
		AFFECT_FLAG_UNK74, // guild_battle (yellow)
		AFFECT_FLAG_UNK75, // guild_battle (green)
		AFFECT_FLAG_UNK76, // guild_battle (red)

		AFFECT_FLAG_UNK77,
		AFFECT_FLAG_UNK78,
		AFFECT_FLAG_UNK79,
		AFFECT_FLAG_UNK80,

#if defined(ENABLE_CONQUEROR_LEVEL)
		AFFECT_CHEONUN_INVINCIBILITY,
		AFFECT_CHEONUN_NORMAL,
		AFFECT_CHEONUN_MASTER,
		AFFECT_CHEONUN_GRAND_MASTER,
		AFFECT_CHEONUN_PERFECT_MASTER,
#endif

		AFFECT_NUM,

		AFFECT_HWAYEOM = AFFECT_GEOMGYEONG,
	};

	enum EAffectTypes
	{
		NEW_AFFECT_NONE = 0,

		//NEW_AFFECT_GUILD_SKILL_BLOOD = 152,
		//NEW_AFFECT_GUILD_SKILL_BLESS = 153,
		//NEW_AFFECT_GUILD_SKILL_SEONGHWI = 154,
		//NEW_AFFECT_GUILD_SKILL_ACCEL = 155,
		//NEW_AFFECT_GUILD_SKILL_BUNNO = 156,
		//NEW_AFFECT_GUILD_SKILL_JUMUN = 157,

		//NEW_AFFECT_SKILL_9_FINISH = 176,
		//NEW_AFFECT_SKILL_9_ILGWANGPYO = 177,
		//NEW_AFFECT_SKILL_9_PUNGLOEPO = 178,
		//NEW_AFFECT_SKILL_9_GEOMAGGWI = 179,
		//NEW_AFFECT_SKILL_9_MABEOBAGGWI = 180,
		//NEW_AFFECT_SKILL_9_METEO = 181,
#if defined(ENABLE_CONQUEROR_LEVEL)
		NEW_AFFECT_SKILL_9_CHEONUN = 182,
#endif
		//NEW_AFFECT_SKILL_9_ILIPUNGU = 183,

		NEW_AFFECT_MOV_SPEED = 200,
		NEW_AFFECT_ATT_SPEED,
		NEW_AFFECT_ATT_GRADE,
		NEW_AFFECT_INVISIBILITY,
		NEW_AFFECT_STR,
		NEW_AFFECT_DEX,
		NEW_AFFECT_CON,
		NEW_AFFECT_INT,
		NEW_AFFECT_FISH_MIND_PILL,

		NEW_AFFECT_POISON,
		NEW_AFFECT_STUN,
		NEW_AFFECT_SLOW,
		NEW_AFFECT_DUNGEON_READY,
		NEW_AFFECT_DUNGEON_UNIQUE,

		NEW_AFFECT_BUILDING,
		NEW_AFFECT_REVIVE_INVISIBLE,
		NEW_AFFECT_FIRE,
		NEW_AFFECT_CAST_SPEED,
		NEW_AFFECT_HP_RECOVER_CONTINUE,
		NEW_AFFECT_SP_RECOVER_CONTINUE,

		NEW_AFFECT_POLYMORPH,
		NEW_AFFECT_MOUNT,

		NEW_AFFECT_WAR_FLAG,

		NEW_AFFECT_BLOCK_CHAT,
		NEW_AFFECT_CHINA_FIREWORK,

		NEW_AFFECT_BOW_DISTANCE,
		NEW_AFFECT_DEF_GRADE,

		NEW_AFFECT_BLEEDING,

		//NEW_AFFECT_SKILL_MONSTER_WIDE_AREA_DAMAGE_SUNGMA_STR = 274,
		//NEW_AFFECT_SKILL_MONSTER_ATT_GRADE_DOWN = 279,
		//NEW_AFFECT_SKILL_POSION_DAMAGE_SUNGMA_MOVE = 280,
		//NEW_AFFECT_SKILL_CRUSH_DAMAGE_SUNGMA_STR = 281,

		NEW_AFFECT_RAMADAN_ABILITY = 300,
		NEW_AFFECT_RAMADAN_RING = 301, // 라마단 이벤트용 특수아이템 초승달의 반지 착용 유무

		NEW_AFFECT_NOG_POCKET_ABILITY = 302,
		NEW_AFFECT_HOLLY_STONE_POWER = 303,

		NEW_AFFECT_PREMIUM_START = 500,
		NEW_AFFECT_EXP_BONUS = 500, // 경험의 반지
		NEW_AFFECT_ITEM_BONUS = 501, // 도둑의 장갑
		NEW_AFFECT_SAFEBOX = 502, // PREMIUM_SAFEBOX,
		NEW_AFFECT_AUTOLOOT = 503, // PREMIUM_AUTOLOOT,
		NEW_AFFECT_FISH_MIND = 504, // PREMIUM_FISH_MIND,
		NEW_AFFECT_MARRIAGE_FAST = 505, // 원앙의 깃털 (금슬),
		NEW_AFFECT_GOLD_BONUS = 506,
		NEW_AFFECT_AUTO_USE = 507,
#if defined(ENABLE_CONQUEROR_LEVEL)
		NEW_AFFECT_SUNGMA_BONUS = 508,
#endif
		NEW_AFFECT_PREMIUM_END = 509,

		NEW_AFFECT_MALL = 510, // 몰 아이템 에펙트
		NEW_AFFECT_NO_DEATH_PENALTY = 511, // 용신의 가호 (경험치 패널티를 한번 막아준다)
		NEW_AFFECT_SKILL_BOOK_BONUS = 512, // 선인의 교훈 (책 수련 성공 확률이 50% 증가)
		NEW_AFFECT_SKILL_BOOK_NO_DELAY = 513, // 주안 술서 (책 수련 딜레이 없음)

		NEW_AFFECT_HAIR = 514, // 헤어 효과
		NEW_AFFECT_COLLECT = 515, // 수집퀘스트 

		NEW_AFFECT_EXP_BONUS_EURO_FREE = 516, // 경험의 반지 (유럽 버전 14 레벨 이하 기본 효과)
		NEW_AFFECT_EXP_BONUS_EURO_FREE_UNDER_15 = 517,
		NEW_AFFECT_UNIQUE_ABILITY = 518,

		NEW_AFFECT_BLEND,

		NEW_AFFECT_HORSE_NAME,
		NEW_AFFECT_MOUNT_BONUS,

		NEW_AFFECT_AUTO_HP_RECOVERY = 534, // 자동물약 HP
		NEW_AFFECT_AUTO_SP_RECOVERY = 535, // 자동물약 SP

		//NEW_AFFECT_PREMIUM_PRIVATE_SHOP = 536,
#if defined(ENABLE_LOOTING_SYSTEM) && defined(ENABLE_PREMIUM_LOOTING)
		NEW_AFFECT_LOOTING_SYSTEM = 537,
#endif

		//NEW_AFFECT_UNKNOWN_538,
		//NEW_AFFECT_UNKNOWN_539,

		NEW_AFFECT_DRAGON_SOUL_QUALIFIED = 540,
		NEW_AFFECT_DRAGON_SOUL_DECK1 = 541,
		NEW_AFFECT_DRAGON_SOUL_DECK2 = 542,

		//NEW_AFFECT_PVP_EXPBONUS = 543,
		//NEW_AFFECT_IMPOSSIBLE_ATTACK = 544,
		//NEW_AFFECT_PVP_ENTER = 545,

#if defined(ENABLE_SOUL_SYSTEM)
		NEW_AFFECT_SOUL = 546,
#endif
		//NEW_AFFECT_UNKNOWN_547,
		//NEW_AFFECT_UNKNOWN_548,
		//NEW_AFFECT_UNKNOWN_549,

#if defined(ENABLE_SET_ITEM)
		NEW_AFFECT_SET_ITEM = 550,
#endif
		//NEW_AFFECT_EXP_BONUS_EVENT = 551,
		//NEW_AFFECT_ATT_SPEED_SLOW = 552,
		//NEW_AFFECT_PEPSI_EVENT = 553,
		//NEW_AFFECT_SUMMER_EVENT = 554,

		//NEW_AFFECT_BATTLE_FIELD = 555,
		//NEW_AFFECT_BATTLE_POTION = 556,
		//NEW_AFFECT_BATTLE_RANK = 557,

#if defined(ENABLE_GUILD_DRAGONLAIR_SYSTEM)
		NEW_AFFECT_RED_DRAGONLAIR_STONE = 558,
#endif
		//NEW_AFFECT_UNKNOWN_559,

		//NEW_AFFECT_LUCKEY_EVENT_BUFF = 560,
		NEW_AFFECT_MOUNT_FALL = 561,
		//NEW_AFFECT_NO_RECOVERY = 562,
		//NEW_AFFECT_REDUCE_CAST_SPEED = 563,
		//NEW_AFFECT_UNKNOWN_564,

		//NEW_AFFECT_ATT_GRADE_DOWN = 565,
		//NEW_AFFECT_DEF_GRADE_DOWN = 566,

		//NEW_AFFECT_CRITICAL_PCT_DOWN = 567,
		//NEW_AFFECT_CZ_UNLIMIT_ENTER = 568,
		//NEW_AFFECT_UNKNOWN_569,
#if defined(ENABLE_FLOWER_EVENT)
		NEW_AFFECT_FLOWER_EVENT = 570,
#endif
#if defined(ENABLE_DS_SET)
		NEW_AFFECT_DS_SET = 571,
#endif
		NEW_AFFECT_RESEARCHER_ELIXIR = 572,
#if defined(ENABLE_DEFENSE_WAVE)
		NEW_AFFECT_DEFENSEWAVE_LASER = 573,
#endif

		//NEW_AFFECT_NO_DEATH_PENALTY_PLUS = 574,
#if defined(ENABLE_YUTNORI_EVENT_FLAG_RENEWAL)
		NEW_AFFECT_HALLOWEEN_EVENT = 575,
#endif
		//NEW_AFFECT_UNKNOWN_576,
#if defined(ENABLE_FISHING_GAME)
		AFFECT_FISHING_GOLD_TUNA = 577,
		AFFECT_FISHING_MOVE_SPEED_DOWN = 578,
#endif
		//NEW_AFFECT_UNKNOWN_579,
		NEW_AFFECT_SAFE_BOX_BUFF = 580,
		//NEW_AFFECT_UNKNOWN_581,
		//NEW_AFFECT_MALL_EXP_BONUS = 582,

#if defined(ENABLE_SUMMER_EVENT_ROULETTE)
		NEW_AFFECT_LATE_SUMMER_EVENT_BUFF = 583,
		NEW_AFFECT_LATE_SUMMER_EVENT_PRIMIUM_BUFF = 584,
#endif

#if defined(ENABLE_GUILD_DRAGONLAIR_SYSTEM)
		NEW_AFFECT_RED_DRAGONLAIR_BUFF = 585,
#endif
		//NEW_AFFECT_MISTS_ISLAND_BUFF = 586,

		//NEW_AFFECT_SEASON_RANKING_BUFF = 587,
		//NEW_AFFECT_SEASON_PVE_BUFF = 588,
		//AFFECT_UNKNOWN_589,

		//NEW_AFFECT_BATTLE_ROYALE_SLOW = 590,
		//NEW_AFFECT_BATTLE_ROYALE_MOVE_SPEED = 591,
		//NEW_AFFECT_UNKNOWN_592,

		//NEW_AFFECT_PASSIVE_JOB_DECK = 593,
#if defined(ENABLE_ELEMENTAL_DUNGEON)
		NEW_AFFECT_CURSE_OF_ELEMENTAL = 594,
		NEW_AFFECT_PROTECTION_OF_ELEMENTAL = 595,
		NEW_AFFECT_POTION_OF_ELEMENTAL = 596,
#endif
		//NEW_AFFECT_UNKNOWN_597,
		//NEW_AFFECT_UNKNOWN_598,
		//NEW_AFFECT_UNKNOWN_599,
		//NEW_AFFECT_BATTLE_ROYALE_INFINITE_STAMINA = 600,

#if defined(ENABLE_SET_ITEM)
		NEW_AFFECT_SET_ITEM_SET_VALUE_1 = 601,
		NEW_AFFECT_SET_ITEM_SET_VALUE_2 = 602,
		NEW_AFFECT_SET_ITEM_SET_VALUE_3 = 603,
		NEW_AFFECT_SET_ITEM_SET_VALUE_4 = 604,
		NEW_AFFECT_SET_ITEM_SET_VALUE_5 = 605,
#endif
		//NEW_AFFECT_UNKNOWN_606,
		//NEW_AFFECT_UNKNOWN_607,

#if defined(ENABLE_CONQUEROR_LEVEL)
		NEW_AFFECT_CHEONUN_INVINCIBILITY = 608,
#endif

		//NEW_AFFECT_WORLD_BOSS_REWARD = 609,
		//NEW_AFFECT_WORLD_BOSS_DAILY_REWARD = 610,

		//NEW_AFFECT_SUNGMAHEE_TOWER_BUFF = 611,
		//NEW_AFFECT_SUNGMAHEE_TOWER_DEBUFF = 612,
		//NEW_AFFECT_SUNGMAHEE_TOWER_CURSE = 613,
		//NEW_AFFECT_UNKNOWN_614,
		//NEW_AFFECT_GARRISON_DEBUFF = 615,
		//NEW_AFFECT_GARRISON_BUFF = 616,

		//NEW_AFFECT_UNKNOWN_617,
		//NEW_AFFECT_UNKNOWN_618,
		//NEW_AFFECT_UNKNOWN_619,
		//NEW_AFFECT_UNKNOWN_620,
		//NEW_AFFECT_UNKNOWN_621,
		//NEW_AFFECT_UNKNOWN_622,

#if defined(ENABLE_CONQUEROR_LEVEL)
		NEW_AFFECT_SUNGMA_STR = 623,
		NEW_AFFECT_SUNGMA_HP = 624,
		NEW_AFFECT_SUNGMA_MOVE = 625,
		NEW_AFFECT_SUNGMA_IMMUNE = 626,
#endif

		//NEW_AFFECT_UNKNOWN_627,
		//NEW_AFFECT_UNKNOWN_628,
		//NEW_AFFECT_SUBQUEST_NEWWORLD_5 = 629,
		//NEW_AFFECT_WHITE_DRAGON_CAVE_CURSE = 630,

		//NEW_AFFECT_SECRET_DUNGEON_BUFF_POINT_C = 631,
		//NEW_AFFECT_SECRET_DUNGEON_BUFF_C = 632,
		//NEW_AFFECT_SECRET_DUNGEON_BUFF_B = 633,
		//NEW_AFFECT_SECRET_DUNGEON_BUFF_A = 634,

		//NEW_AFFECT_OTHER_WORLD_DEAD_SOUL = 635,
		//NEW_AFFECT_OTHER_WORLD_GOBLIN_PROTECTION = 636,
		//NEW_AFFECT_OTHER_WORLD_YEOMWANG_CURSE = 637,
		//NEW_AFFECT_OTHER_WORLD_NORTH_REAPER_ROOM_DEBUFF = 638,

		//NEW_AFFECT_SKILL_MOUNT_UPGRADE_NIMBLE = 639,
		//NEW_AFFECT_SKILL_MOUNT_UPGRADE_EXP = 640,
		//NEW_AFFECT_SKILL_MOUNT_UPGRADE_SPEED = 641,
		//NEW_AFFECT_SKILL_MOUNT_UPGRADE_GYENONGGONG = 642,
		//NEW_AFFECT_SKILL_MOUNT_UPGRADE_INVINCIBLITIY = 643,
		//NEW_AFFECT_SKILL_MOUNT_UPGRADE_KNOCKBACK = 644,
		//NEW_AFFECT_SKILL_MOUNT_UPGRADE_TO_STRONG = 645,
		//NEW_AFFECT_SKILL_MOUNT_UPGRADE_EFFECT_UP = 646,
		//NEW_AFFECT_SKILL_MOUNT_UPGRADE_SUNGMA_STR = 647,
		//NEW_AFFECT_SKILL_MOUNT_UPGRADE_SUNGMA_HP = 648,
		//NEW_AFFECT_SKILL_MOUNT_UPGRADE_SUNGMA_MOVE = 649,
		//NEW_AFFECT_SKILL_MOUNT_UPGRADE_SUNGMA_IMMUNE = 650,
		//NEW_AFFECT_SKILL_MOUNT_UPGRADE_HIT_PCT = 651,

		//NEW_AFFECT_MERCENARY_MISSION_MAX_ACTIVE_INCREASE = 652,
		//NEW_AFFECT_SECRET_DUNGEON_BUFF_POINT_B = 653,
		//NEW_AFFECT_SECRET_DUNGEON_BUFF_POINT_A = 654,
		NEW_AFFECT_ELEMENT_BUFF_CRACK = 655,
		//NEW_AFFECT_FLOWER_OF_GALE = 656,
		//NEW_AFFECT_FLOWER_OF_DESTRUCTION = 657,
		//NEW_AFFECT_UNKNOWN_658,
		//NEW_AFFECT_UNKNOWN_659,
		//NEW_AFFECT_UNKNOWN_660,
		//NEW_AFFECT_UNKNOWN_661,
		//NEW_AFFECT_IMMUNE_MINE_TOWER_SKILL = 662,
#if defined(ENABLE_SNOWFLAKE_STICK_EVENT)
		NEW_AFFECT_SNOWFLAKE_STICK_EVENT_RANK_BUFF = 663,
		NEW_AFFECT_SNOWFLAKE_STICK_EVENT_SNOWFLAKE_BUFF = 664,
#endif

		NEW_AFFECT_QUEST_START_IDX = 1000,
	};

	enum
	{
		STONE_SMOKE1 = 0, // 99%
		STONE_SMOKE2 = 1, // 85%
		STONE_SMOKE3 = 2, // 80%
		STONE_SMOKE4 = 3, // 60%
		STONE_SMOKE5 = 4, // 45%
		STONE_SMOKE6 = 5, // 40%
		STONE_SMOKE7 = 6, // 20%
		STONE_SMOKE8 = 7, // 10%
		STONE_SMOKE_NUM = 4,
	};

	enum EBuildingAffect
	{
		BUILDING_CONSTRUCTION_SMALL = 0,
		BUILDING_CONSTRUCTION_LARGE = 1,
		BUILDING_UPGRADE = 2,
	};

	enum
	{
		WEAPON_DUALHAND,
		WEAPON_ONEHAND,
		WEAPON_TWOHAND,
		WEAPON_NUM,
	};

	enum
	{
		EMPIRE_NONE,
		EMPIRE_A,
		EMPIRE_B,
		EMPIRE_C,
		EMPIRE_NUM,
	};

	enum ENameColor
	{
		NAMECOLOR_MOB,
		NAMECOLOR_NPC,
		NAMECOLOR_PC,
		NAMECOLOR_PC_END = NAMECOLOR_PC + EMPIRE_NUM,
		NAMECOLOR_NORMAL_MOB,
		NAMECOLOR_NORMAL_NPC,
		NAMECOLOR_NORMAL_PC,
		NAMECOLOR_NORMAL_PC_END = NAMECOLOR_NORMAL_PC + EMPIRE_NUM,
		NAMECOLOR_EMPIRE_MOB,
		NAMECOLOR_EMPIRE_NPC,
		NAMECOLOR_EMPIRE_PC,
		NAMECOLOR_EMPIRE_PC_END = NAMECOLOR_EMPIRE_PC + EMPIRE_NUM,
		NAMECOLOR_FUNC,
		NAMECOLOR_PK,
		NAMECOLOR_PVP,
		NAMECOLOR_PARTY,
		NAMECOLOR_WARP,
		NAMECOLOR_WAYPOINT,
		NAMECOLOR_EXTRA = NAMECOLOR_FUNC + 10,
		NAMECOLOR_NUM = NAMECOLOR_EXTRA + 10,
	};

	enum ETitleColor
	{
		TITLE_RGB_GOOD_4,
		TITLE_RGB_GOOD_3,
		TITLE_RGB_GOOD_2,
		TITLE_RGB_GOOD_1,
		TITLE_RGB_NORMAL,
		TITLE_RGB_EVIL_1,
		TITLE_RGB_EVIL_2,
		TITLE_RGB_EVIL_3,
		TITLE_RGB_EVIL_4,
	};

	enum EAlignmentType
	{
		ALIGNMENT_TYPE_WHITE,
		ALIGNMENT_TYPE_NORMAL,
		ALIGNMENT_TYPE_DARK,
	};

	enum
	{
		EMOTICON_EXCLAMATION = 1,
		EMOTICON_FISH = 11,
#if defined(ENABLE_FISHING_GAME)
		EMOTICON_NO_FISH = 25,
#endif
		EMOTICON_NUM = 128,

		TITLE_NUM = 9,
		TITLE_NONE = 4,
	};

	enum //아래 번호가 바뀌면 registerEffect 쪽도 바꾸어 줘야 한다.
	{
		EFFECT_REFINED_NONE,

		EFFECT_SWORD_REFINED7,
		EFFECT_SWORD_REFINED8,
		EFFECT_SWORD_REFINED9,
		EFFECT_SWORD_REFINED10,

		EFFECT_BOW_REFINED7,
		EFFECT_BOW_REFINED8,
		EFFECT_BOW_REFINED9,
		EFFECT_BOW_REFINED10,

		EFFECT_FANBELL_REFINED7,
		EFFECT_FANBELL_REFINED8,
		EFFECT_FANBELL_REFINED9,
		EFFECT_FANBELL_REFINED10,

		EFFECT_SMALLSWORD_REFINED7,
		EFFECT_SMALLSWORD_REFINED8,
		EFFECT_SMALLSWORD_REFINED9,
		EFFECT_SMALLSWORD_REFINED10,

		EFFECT_SMALLSWORD_REFINED7_LEFT,
		EFFECT_SMALLSWORD_REFINED8_LEFT,
		EFFECT_SMALLSWORD_REFINED9_LEFT,
		EFFECT_SMALLSWORD_REFINED10_LEFT,

		EFFECT_CLAW_REFINED7,
		EFFECT_CLAW_REFINED8,
		EFFECT_CLAW_REFINED9,
		EFFECT_CLAW_REFINED10,

		EFFECT_CLAW_REFINED7_LEFT,
		EFFECT_CLAW_REFINED8_LEFT,
		EFFECT_CLAW_REFINED9_LEFT,
		EFFECT_CLAW_REFINED10_LEFT,

		EFFECT_BODYARMOR_REFINED7,
		EFFECT_BODYARMOR_REFINED8,
		EFFECT_BODYARMOR_REFINED9,
		EFFECT_BODYARMOR_REFINED10,

		EFFECT_BODYARMOR_SPECIAL, // 갑옷 4-2-1
		EFFECT_BODYARMOR_SPECIAL2, // 갑옷 4-2-2
		EFFECT_BODYARMOR_SPECIAL3, // 갑옷 5-1
		EFFECT_BODYARMOR_SPECIAL4, // 갑옷 5 jin

		EFFECT_REFINED_NUM,
	};

#if defined(ENABLE_REFINE_ELEMENT_SYSTEM)
	enum ERefineElementCategory
	{
		REFINE_ELEMENT_CATEGORY_ELECT,
		REFINE_ELEMENT_CATEGORY_FIRE,
		REFINE_ELEMENT_CATEGORY_ICE,
		REFINE_ELEMENT_CATEGORY_WIND,
		REFINE_ELEMENT_CATEGORY_EARTH,
		REFINE_ELEMENT_CATEGORY_DARK,
		REFINE_ELEMENT_CATEGORY_MAX,
	};

	enum ERefineElement
	{
		EFFECT_REFINE_ELEMENT_WEAPON_SWORD,
		EFFECT_REFINE_ELEMENT_WEAPON_BOW,
		EFFECT_REFINE_ELEMENT_WEAPON_FAN,
		EFFECT_REFINE_ELEMENT_WEAPON_DEGER,
		EFFECT_REFINE_ELEMENT_WEAPON_CLAW,
		EFFECT_REFINE_ELEMENT_WEAPON_MAX
	};
#endif

	enum DamageFlag
	{
		DAMAGE_NORMAL = (1 << 0),
		DAMAGE_POISON = (1 << 1),
		DAMAGE_DODGE = (1 << 2),
		DAMAGE_BLOCK = (1 << 3),
		DAMAGE_PENETRATE = (1 << 4),
		DAMAGE_CRITICAL = (1 << 5),
		DAMAGE_BLEEDING = (1 << 6),
		// 반-_-사
	};

	enum EEffect
	{
		EFFECT_DUST,
		EFFECT_STUN,
		EFFECT_HIT,
		EFFECT_FLAME_ATTACK,
		EFFECT_FLAME_HIT,
		EFFECT_FLAME_ATTACH,
		EFFECT_ELECTRIC_ATTACK,
		EFFECT_ELECTRIC_HIT,
		EFFECT_ELECTRIC_ATTACH,
		EFFECT_SPAWN_APPEAR,
		EFFECT_SPAWN_DISAPPEAR,
		EFFECT_LEVELUP,
		EFFECT_SKILLUP,
		EFFECT_HPUP_RED,
		EFFECT_SPUP_BLUE,
		EFFECT_SPEEDUP_GREEN,
		EFFECT_DXUP_PURPLE,
		EFFECT_CRITICAL,
		EFFECT_PENETRATE,
		EFFECT_BLOCK,
		EFFECT_DODGE,
		EFFECT_FIRECRACKER,
		EFFECT_SPIN_TOP,
		EFFECT_WEAPON,
		EFFECT_WEAPON_END = EFFECT_WEAPON + WEAPON_NUM,
		EFFECT_AFFECT,
		EFFECT_AFFECT_GYEONGGONG = EFFECT_AFFECT + AFFECT_GYEONGGONG,
		EFFECT_AFFECT_KWAESOK = EFFECT_AFFECT + AFFECT_KWAESOK,
		EFFECT_AFFECT_END = EFFECT_AFFECT + AFFECT_NUM,

#if defined(ENABLE_CONQUEROR_LEVEL)
		EFFECT_CHEONUN_INVINCIBILITY = EFFECT_AFFECT + AFFECT_CHEONUN_INVINCIBILITY,
		EFFECT_CHEONUN_NORMAL = EFFECT_AFFECT + AFFECT_CHEONUN_NORMAL,
		EFFECT_CHEONUN_MASTER = EFFECT_AFFECT + AFFECT_CHEONUN_MASTER,
		EFFECT_CHEONUN_GRAND_MASTER = EFFECT_AFFECT + AFFECT_CHEONUN_GRAND_MASTER,
		EFFECT_CHEONUN_PERFECT_MASTER = EFFECT_AFFECT + AFFECT_CHEONUN_PERFECT_MASTER,
#endif

		//EFFECT_UNK_113,
		//EFFECT_UNK_114,
		//EFFECT_UNK_115,
		//EFFECT_UNK_116,
		//EFFECT_UNK_117,
		//EFFECT_UNK_118,
		//EFFECT_UNK_119,
		//EFFECT_UNK_120,
		//EFFECT_UNK_121,
		//EFFECT_UNK_122,

		EFFECT_EMOTICON,
		EFFECT_EMOTICON_END = EFFECT_EMOTICON + EMOTICON_NUM,
		EFFECT_SELECT,
		EFFECT_TARGET,
		EFFECT_EMPIRE,
		EFFECT_EMPIRE_END = EFFECT_EMPIRE + EMPIRE_NUM,
		EFFECT_HORSE_DUST,
		EFFECT_REFINED,
		EFFECT_REFINED_END = EFFECT_REFINED + EFFECT_REFINED_NUM,
		EFFECT_DAMAGE_TARGET,
		EFFECT_DAMAGE_NOT_TARGET,
		EFFECT_DAMAGE_SELFDAMAGE,
		EFFECT_DAMAGE_SELFDAMAGE2,
		EFFECT_DAMAGE_POISON,
		EFFECT_DAMAGE_MISS,
		EFFECT_DAMAGE_TARGETMISS,
		EFFECT_DAMAGE_CRITICAL,
		EFFECT_SUCCESS,
		EFFECT_FAIL,
		EFFECT_FR_SUCCESS,
		EFFECT_LEVELUP_ON_14_FOR_GERMANY, //레벨업 14일때 ( 독일전용 )
		EFFECT_LEVELUP_UNDER_15_FOR_GERMANY, //레벨업 15일때 ( 독일전용 )
		EFFECT_PERCENT_DAMAGE1,
		EFFECT_PERCENT_DAMAGE2,
		EFFECT_PERCENT_DAMAGE3,
		EFFECT_AUTO_HPUP,
		EFFECT_AUTO_SPUP,
		EFFECT_RAMADAN_RING_EQUIP, // 초승달 반지 착용 순간에 발동하는 이펙트
		EFFECT_HALLOWEEN_CANDY_EQUIP, // 할로윈 사탕 착용 순간에 발동하는 이펙트
		EFFECT_HAPPINESS_RING_EQUIP, // 행복의 반지 착용 순간에 발동하는 이펙트
		EFFECT_LOVE_PENDANT_EQUIP, // 행복의 반지 착용 순간에 발동하는 이펙트

#if defined(ENABLE_ACCE_COSTUME_SYSTEM)
		EFFECT_ACCE_SUCESS_ABSORB,
		EFFECT_ACCE_EQUIP,
		EFFECT_ACCE_BACK,
#endif

		EFFECT_EASTER_CANDY_EQIP,
		EFFECT_THUNDER_AREA,
		EFFECT_THUNDER,
		EFFECT_HEAL,
		EFFECT_CAPE_OF_COURAGE,
		EFFECT_CHOCOLATE_PENDANT,
		EFFECT_FEATHER_WALK,
		EFFECT_PEPSI_EVENT,

#if defined(ENABLE_GUILD_DRAGONLAIR_SYSTEM)
		EFFECT_DRAGONLAIR_STONE_UNBEATABLE_1,
		EFFECT_DRAGONLAIR_STONE_UNBEATABLE_2,
		EFFECT_DRAGONLAIR_STONE_UNBEATABLE_3,
#endif

		EFFECT_BATTLE_POTION,
		EFFECT_REFLECT,
		EFFECT_SKILL_DAMAGE_ZONE,
		EFFECT_SKILL_SAFE_ZONE,
		EFFECT_METEOR,
		EFFECT_BEAD_RAIN,
		EFFECT_ARROW_RAIN,
		EFFECT_FALL_ROCK,
		EFFECT_HORSE_DROP,
		EFFECT_EGG_DROP,
		EFFECT_DEAPO_BOOM,
#if defined(ENABLE_FLOWER_EVENT)
		EFFECT_FLOWER_EVENT,
#endif
#if defined(ENABLE_GEM_SYSTEM)
		EFFECT_GEM_PENDANT,
#endif
#if defined(ENABLE_DEFENSE_WAVE)
		EFFECT_DEFENSE_WAVE_LASER,
#endif
		EFFECT_PET_ATTR_CHANGE_NEW_TYPE,
		EFFECT_PET_PAY_SUMMON1,
#if defined(ENABLE_SUMMER_EVENT_ROULETTE)
		EFFECT_SPECIAL_ROULETTE,
#endif

#if defined(ENABLE_REFINE_ELEMENT_SYSTEM)
		EFFECT_REFINED_ELEMENT,
		EFFECT_REFINED_ELEMENT_END = EFFECT_REFINED_ELEMENT + REFINE_ELEMENT_CATEGORY_MAX * EFFECT_REFINE_ELEMENT_WEAPON_MAX,
#endif

		EFFECT_MISTS_ISLAND_0,
		EFFECT_MISTS_ISLAND_1,
		EFFECT_MISTS_ISLAND_2,
		EFFECT_MISTS_ISLAND_3,
		EFFECT_PASSIVE_ATTR_SKILL,
		EFFECT_SUICIDE_BOMB_SMALL,
		EFFECT_SUICIDE_BOMB_LARGE,
		EFFECT_GROUND_THORN,
		EFFECT_GROUND_SNAKE,

		EFFECT_WHITE_DRAGON_BERSERK,
		EFFECT_WHITE_DRAGON_COOLING,

		EFFECT_USE_METINSTONE_RAIN_SPAWN_TICKET,
		EFFECT_FAKE_METINSTONE_SPLASH_DAMAGE,
		EFFECT_MOUNT_UPGRADE_SKILL,
		EFFECT_SOUTH_REAPER_SKILL_2,
		EFFECT_YEOMWANG_SKILL_2,
		EFFECT_BUFF_ITEM_5,
		EFFECT_BUFF_ITEM_13,
		EFFECT_BUFF_ITEM_14,
		EFFECT_BUFF_SYMBOL1,
		EFFECT_BUFF_ITEM_8_ELEMENT_1,
		EFFECT_BUFF_ITEM_8_ELEMENT_2,
		EFFECT_BUFF_FLOWER_OF_GALE,
		EFFECT_BUFF_FLOWER_OF_DESTRUCTION,
		EFFECT_SUNGMAHEE_GATE_FLOWER_BLOW_RANGE,
		EFFECT_SUNGMAHEE_GATE_INVINCIBLE_GATE,
		EFFECT_SUNGMAHEE_GATE_NORMAL_GATE,
		EFFECT_MOUNT_UPGRADE_GYEONGGONG,
		//EFFECT_GOLDEN_LAND_STONE_INVINCIBLE,
		//EFFECT_GOLDEN_T_REX_STONE_DROP,
		EFFECT_WHITE_DRAGON_BLESS,
		EFFECT_IRON_DRAGON_BLESS,
#if defined(ENABLE_SNOWFLAKE_STICK_EVENT)
		EFFECT_USE_SNOWFLAKE_STICK,
#endif
		//EFFECT_MINE_TOWER_DEAD_SKILL_SELF,
		//EFFECT_MINE_TOWER_DEAD_SKILL_BLOW,
		//EFFECT_MINE_TOWER_DEAD_SKILL_BLOW_GUARD,
		//EFFECT_USE_MINE_TOWER_DEAD_SKILL_IMMUNE,
		EFFECT_POLYMORPH_SNOWMAN,
		EFFECT_SPORTS_MATCH_BUFF_EMBLEM,
		EFFECT_POINT_AREA_ELECT_ATTACK,
		EFFECT_NORMAL_ATTACK_FORCE_BUFF,
		EFFECT_SKILL_ATTACK_FORCE_BUFF,
		EFFECT_FORCE_MIX_BUFF,

		EFFECT_TEMP,

#ifdef ENABLE_QUEEN_NETHIS
		EFFECT_SNAKE_REGEN,
#endif

		EFFECT_NUM,
	};

	enum EFlyEffects
	{
		FLY_NONE,
		FLY_EXP,
		FLY_HP_MEDIUM,
		FLY_HP_BIG,
		FLY_SP_SMALL,
		FLY_SP_MEDIUM,
		FLY_SP_BIG,
		FLY_FIREWORK1,
		FLY_FIREWORK2,
		FLY_FIREWORK3,
		FLY_FIREWORK4,
		FLY_FIREWORK5,
		FLY_FIREWORK6,
		FLY_FIREWORK_XMAS,
		FLY_CHAIN_LIGHTNING,
		FLY_HP_SMALL,
		FLY_SKILL_MUYEONG,
#if defined(ENABLE_QUIVER_SYSTEM)
		FLY_QUIVER_ATTACK_NORMAL,
#endif
#if defined(ENABLE_CONQUEROR_LEVEL)
		FLY_ILGWANGPYO_NORMAL,
		FLY_ILGWANGPYO_MASTER,
		FLY_ILGWANGPYO_GRAND_MASTER,
		FLY_ILGWANGPYO_PERFECT_MASTER,
		FLY_CONQUEROR_EXP,
#endif
	};

	enum
	{
		DUEL_NONE,
		DUEL_CANNOTATTACK,
		DUEL_START,
	};

public:
	static void DestroySystem();
	static void CreateSystem(UINT uCapacity);

	static bool RegisterEffect(UINT eEftType, const char* c_szEftAttachBone, const char* c_szEftName, bool isCache);

	static void RegisterTitleName(int iIndex, const char* c_szTitleName);
#if defined(ENABLE_SET_ITEM)
	static void RegisterItemSetName(int iIndex, const char* c_szItemSetName);
#endif
#if defined(ENABLE_GUILD_LEADER_GRADE_NAME)
	static void RegisterGuildLeaderGradeName(int iIndex, const char* c_szGradeName);
#endif
#if defined(ENABLE_LEFT_SEAT)
	static void RegisterLeftSeatText(const char* c_szLeftSeatText);
#endif
	static bool RegisterNameColor(UINT uIndex, UINT r, UINT g, UINT b);
	static bool RegisterTitleColor(UINT uIndex, UINT r, UINT g, UINT b);
	static bool ChangeEffectTexture(UINT eEftType, const char* c_szSrcFileName, const char* c_szDstFileName);

	static void SetDustGap(float fDustGap);
	static void SetHorseDustGap(float fDustGap);

	static void SetEmpireNameMode(bool isEnable);
	static const D3DXCOLOR& GetIndexedNameColor(UINT eNameColor);

public:
	void SetMainInstance();

	void OnSelected();
	void OnUnselected();
	void OnTargeted();
	void OnUntargeted();

protected:
	bool __IsExistMainInstance();
	bool __IsMainInstance();
	bool __MainCanSeeHiddenThing();
	float __GetBowRange();

protected:
	DWORD __AttachEffect(UINT eEftType, float fScale = 1.0f);
#if defined(ENABLE_REFINE_ELEMENT_SYSTEM)
	DWORD __AttachBoneEffect(DWORD dwPartIndex, DWORD dwEID);
#endif
	//DWORD __AttachEffect(char filename[128]);
	void __DetachEffect(DWORD dwEID);

public:
	void CreateSpecialPositionEffect(DWORD iEffectIndex, D3DXVECTOR3& rPosition, float fScale = 1.0f);
	void CreateSpecialEffect(DWORD iEffectIndex, float fScale = 1.0f);
	void AttachSpecialEffect(DWORD effect, float fScale = 1.0f);

protected:
	static std::string ms_astAffectEffectAttachBone[EFFECT_NUM];
	static DWORD ms_adwCRCAffectEffect[EFFECT_NUM];
	static float ms_fDustGap;
	static float ms_fHorseDustGap;

public:
	CInstanceBase();
	virtual ~CInstanceBase();

	bool LessRenderOrder(CInstanceBase* pkInst);

	void MountHorse(UINT eRace);
	void DismountHorse();

	// 스크립트용 테스트 함수. 나중에 없에자
	void SCRIPT_SetAffect(UINT eAffect, bool isVisible);

	float CalculateDistanceSq3d(const TPixelPosition& c_rkPPosDst);

	// Instance Data
	bool IsFlyTargetObject();
	void ClearFlyTargetInstance();
	void SetFlyTargetInstance(CInstanceBase& rkInstDst);
	void AddFlyTargetInstance(CInstanceBase& rkInstDst);
	void AddFlyTargetPosition(const TPixelPosition& c_rkPPosDst);

	float GetFlyTargetDistance();

	void SetAlpha(float fAlpha);

	void DeleteBlendOut();

	void AttachTextTail();
	void DetachTextTail();
	void UpdateTextTailLevel(DWORD level);
#if defined(ENABLE_CONQUEROR_LEVEL)
	void UpdateTextTailConquerorLevel(DWORD level);
#endif
#if defined(ENABLE_MULTI_LANGUAGE_SYSTEM)
	void UpdateTextTailCountryFlag(const std::string& c_stCountry);
#endif
#if defined(ENABLE_LEFT_SEAT)
	void UpdateTextTailLeftSeat(const std::string& c_rstrText);
#endif

	void RefreshTextTail();
	void RefreshTextTailTitle();

	bool Create(const SCreateData& c_rkCreateData);

	bool CreateDeviceObjects();
	void DestroyDeviceObjects();

	void Destroy();

	void Update();
	bool UpdateDeleting();

	void Transform();
	void Deform();
	void Render();
	void RenderTrace();
	void RenderToShadowMap();
	void RenderCollision();
	void RegisterBoundingSphere();

	// Temporary
	void GetBoundBox(D3DXVECTOR3* vtMin, D3DXVECTOR3* vtMax);

	void SetNameString(const char* c_szName, int len);
	bool SetRace(DWORD dwRaceIndex
#if defined(ENABLE_RACE_SWAP)
		, DWORD eEventRace = 0
#endif
	);
	void SetVirtualID(DWORD wVirtualNumber);
	void SetVirtualNumber(DWORD dwVirtualNumber);
	void SetInstanceType(int iInstanceType);
	void SetAlignment(short sAlignment);
	void SetPKMode(BYTE byPKMode);
	void SetKiller(bool bFlag);
	void SetPartyMemberFlag(bool bFlag);
	void SetStateFlags(DWORD dwStateFlags);

#if defined(ENABLE_LEFT_SEAT)
	void SetLeftSeat(bool bLeftSeat);
	bool GetLeftSeat() const { return m_bLeftSeat; }
#endif

	void SetArmor(DWORD dwArmor);
	void SetShape(DWORD eShape, float fSpecular = 0.0f);
	void SetHair(DWORD eHair);
#if defined(ENABLE_ACCE_COSTUME_SYSTEM)
	bool SetAcce(DWORD eAcce);
	void ChangeAcce(DWORD eAcce);
#endif
#if defined(ENABLE_AURA_COSTUME_SYSTEM)
	bool SetAura(DWORD eAura);
	void ChangeAura(DWORD eAura);
#endif
#if defined(ENABLE_QUIVER_SYSTEM)
	bool SetArrow(DWORD eArrow);
#endif
#if defined(ENABLE_REFINE_ELEMENT_SYSTEM)
	void SetRefineElement(WORD wAffectType);
#endif
	bool SetWeapon(DWORD eWeapon);
	bool ChangeArmor(DWORD dwArmor);
	void ChangeWeapon(DWORD eWeapon);
	void ChangeHair(DWORD eHair);
	void ChangeGuild(DWORD dwGuildID
#if defined(ENABLE_GUILD_LEADER_GRADE_NAME)
		, BYTE bGuildLeaderGrade
#endif
	);

	DWORD GetWeaponType();
	bool IsUsingFishingRod();

	void SetComboType(UINT uComboType);
	void SetAttackSpeed(UINT uAtkSpd);
	void SetMoveSpeed(UINT uMovSpd);
	void SetRotationSpeed(float fRotSpd);

	const char* GetNameString();

	int GetLevel();
	int GetInstanceType();

	DWORD GetPart(CRaceData::EParts part);
	DWORD GetShape();
	DWORD GetRace();
	DWORD GetVirtualID();
	DWORD GetVirtualNumber();
	DWORD GetEmpireID();
#ifdef WJ_SHOW_MOB_INFO
	DWORD GetAIFlag();
#endif
	DWORD GetGuildID();
#if defined(ENABLE_GUILD_LEADER_GRADE_NAME)
	BYTE GetGuildLeaderGrade();
	BYTE GetGuildLeaderGradeType();
#endif

#if defined(ENABLE_METINSTONE_SWAP)
	bool ChangeStoneShape(DWORD dwRace, int iShape);
	void SetColorOperation(DWORD dwColor);
	void ResetRaceDataStoneShapeEvent();
	bool SetRaceDataStoneShapeEvent(DWORD dwRace);
#endif

	int GetAlignment();
	UINT GetAlignmentGrade();
	int GetAlignmentType();
	BYTE GetPKMode();
	bool IsKiller();
	bool IsPartyMember();

	void ActDualEmotion(CInstanceBase& rkDstInst, WORD dwMotionNumber1, WORD dwMotionNumber2);
	void ActEmotion(DWORD dwMotionNumber);
	void LevelUp();
	void SkillUp();
	void UseSpinTop();
	void Revive();
	void Stun();
	void Die();
	void Hide();
	void Show();

	bool CanAct();
	bool CanMove();
	bool CanAttack();
	bool CanUseSkill(DWORD dwSkillIndex);
	bool CanFishing();
	bool IsConflictAlignmentInstance(CInstanceBase& rkInstVictim);
	bool IsAttackableInstance(CInstanceBase& rkInstVictim);
	bool IsTargetableInstance(CInstanceBase& rkInstVictim);
	bool IsPVPInstance(CInstanceBase& rkInstVictim);
	bool CanChangeTarget();
	bool CanPickInstance();
	bool CanViewTargetHP(CInstanceBase& rkInstVictim);

	// Movement
	BOOL IsGoing();
	bool NEW_Goto(const TPixelPosition& c_rkPPosDst, float fDstRot);
	void EndGoing();

	void SetRunMode();
	void SetWalkMode();

	bool IsAffect(UINT uAffect);
	BOOL IsInvisibility();
	BOOL IsStealth();
	BOOL IsParalysis();
	BOOL IsGameMaster();
	BOOL IsSameEmpire(CInstanceBase& rkInstDst);
	BOOL IsBowMode();
	BOOL IsHandMode();
	BOOL IsFishingMode();
	BOOL IsFishing();

	BOOL IsWearingDress();
	BOOL IsHoldingPickAxe();
	BOOL IsHoldingFishingRod();
	BOOL IsMountingHorse();
	BOOL IsNewMount();
	BOOL IsForceVisible();
	BOOL IsInSafe();
	BOOL IsEnemy();
	BOOL IsStone();
	BOOL IsResource();
	BOOL IsNPC();
	BOOL IsPC();
	BOOL IsPoly();
	BOOL IsWarp();
	BOOL IsGoto();
	BOOL IsObject();
	BOOL IsDoor();
	BOOL IsBuilding();
	BOOL IsWoodenDoor();
	BOOL IsStoneDoor();
	BOOL IsFlag();
#if defined(ENABLE_EVENT_BANNER_FLAG)
	BOOL IsBannerFlag();
#endif
	BOOL IsGuildWall();
	BOOL IsHorse();
	BOOL IsPet();
	BOOL IsPetPay();
	BOOL IsShop();

	BOOL IsDead();
	BOOL IsStun();
	BOOL IsSleep();
	BOOL __IsSyncing();
	BOOL IsWaiting();
	BOOL IsWalking();
	BOOL IsPushing();
	BOOL IsAttacking();
	BOOL IsActingTargetEmotion();
	BOOL IsActingEmotion();
	BOOL IsAttacked();
	BOOL IsKnockDown();
	BOOL IsUsingSkill();
	BOOL IsUsingMovingSkill();
	BOOL CanCancelSkill();
	BOOL CanAttackHorseLevel();

#ifdef __MOVIE_MODE__
	BOOL IsMovieMode(); // 운영자용 완전히 안보이는거
#endif
	bool NEW_CanMoveToDestPixelPosition(const TPixelPosition& c_rkPPosDst);

	void NEW_SetAdvancingRotationFromPixelPosition(const TPixelPosition& c_rkPPosSrc, const TPixelPosition& c_rkPPosDst);
	void NEW_SetAdvancingRotationFromDirPixelPosition(const TPixelPosition& c_rkPPosDir);
	bool NEW_SetAdvancingRotationFromDestPixelPosition(const TPixelPosition& c_rkPPosDst);
	void SetAdvancingRotation(float fRotation);

	void EndWalking(float fBlendingTime = 0.15f);
	void EndWalkingWithoutBlending();

	// Battle
	void SetEventHandler(CActorInstance::IEventHandler* pkEventHandler);

	void PushUDPState(DWORD dwCmdTime, const TPixelPosition& c_rkPPosDst, float fDstRot, UINT eFunc, UINT uArg);
	void PushTCPState(DWORD dwCmdTime, const TPixelPosition& c_rkPPosDst, float fDstRot, UINT eFunc, UINT uArg);
	void PushTCPStateExpanded(DWORD dwCmdTime, const TPixelPosition& c_rkPPosDst, float fDstRot, UINT eFunc, UINT uArg, UINT uTargetVID);

	void NEW_Stop();

	bool NEW_UseSkill(UINT uSkill, UINT uMot, UINT uMotLoopCount, bool isMovingSkill);
	void NEW_Attack();
	void NEW_Attack(float fDirRot);
	void NEW_AttackToDestPixelPositionDirection(const TPixelPosition& c_rkPPosDst);
	bool NEW_AttackToDestInstanceDirection(CInstanceBase& rkInstDst, IFlyEventHandler* pkFlyHandler);
	bool NEW_AttackToDestInstanceDirection(CInstanceBase& rkInstDst);

	bool NEW_MoveToDestPixelPositionDirection(const TPixelPosition& c_rkPPosDst);
	void NEW_MoveToDestInstanceDirection(CInstanceBase& rkInstDst);
	void NEW_MoveToDirection(float fDirRot);

	float NEW_GetDistanceFromDirPixelPosition(const TPixelPosition& c_rkPPosDir);
	float NEW_GetDistanceFromDestPixelPosition(const TPixelPosition& c_rkPPosDst);
	float NEW_GetDistanceFromDestInstance(CInstanceBase& rkInstDst);

	float NEW_GetRotation();
	float NEW_GetRotationFromDestPixelPosition(const TPixelPosition& c_rkPPosDst);
	float NEW_GetRotationFromDirPixelPosition(const TPixelPosition& c_rkPPosDir);
	float NEW_GetRotationFromDestInstance(CInstanceBase& rkInstDst);

	float NEW_GetAdvancingRotationFromDirPixelPosition(const TPixelPosition& c_rkPPosDir);
	float NEW_GetAdvancingRotationFromDestPixelPosition(const TPixelPosition& c_rkPPosDst);
	float NEW_GetAdvancingRotationFromPixelPosition(const TPixelPosition& c_rkPPosSrc, const TPixelPosition& c_rkPPosDst);

	BOOL NEW_IsClickableDistanceDestPixelPosition(const TPixelPosition& c_rkPPosDst);
	BOOL NEW_IsClickableDistanceDestInstance(CInstanceBase& rkInstDst);

	bool NEW_GetFrontInstance(CInstanceBase** ppoutTargetInstance, float fDistance);
	void NEW_GetRandomPositionInFanRange(CInstanceBase& rkInstTarget, TPixelPosition* pkPPosDst);
	bool NEW_GetInstanceVectorInFanRange(float fSkillDistance, CInstanceBase& rkInstTarget, std::vector<CInstanceBase*>* pkVct_pkInst);
	bool NEW_GetInstanceVectorInCircleRange(float fSkillDistance, std::vector<CInstanceBase*>* pkVct_pkInst);

	void NEW_SetOwner(DWORD dwOwnerVID);
	void NEW_SyncPixelPosition(long& nPPosX, long& nPPosY);
	void NEW_SyncCurrentPixelPosition();

	void NEW_SetPixelPosition(const TPixelPosition& c_rkPPosDst);

	bool NEW_IsLastPixelPosition();
	const TPixelPosition& NEW_GetLastPixelPositionRef();

	// Battle
	BOOL isNormalAttacking();
	BOOL isComboAttacking();
	MOTION_KEY GetNormalAttackIndex();
	DWORD GetComboIndex();
	float GetAttackingElapsedTime();
	void InputNormalAttack(float fAtkDirRot);
	void InputComboAttack(float fAtkDirRot);

	void RunNormalAttack(float fAtkDirRot);
	void RunComboAttack(float fAtkDirRot, DWORD wMotionIndex);

	CInstanceBase* FindNearestVictim();
	BOOL CheckAdvancing();

	bool AvoidObject(const CGraphicObjectInstance& c_rkBGObj);
	bool IsBlockObject(const CGraphicObjectInstance& c_rkBGObj);
	void BlockMovement();

public:
	BOOL CheckAttacking(CInstanceBase& rkInstVictim);
	void ProcessHitting(DWORD dwMotionKey, CInstanceBase* pVictimInstance);
	void ProcessHitting(DWORD dwMotionKey, BYTE byEventIndex, CInstanceBase* pVictimInstance);
	void GetBlendingPosition(TPixelPosition* pPixelPosition);
	void SetBlendingPosition(const TPixelPosition& c_rPixelPosition);

	// Fishing
	void StartFishing(float frot);
	void StopFishing();
	void ReactFishing();
	void CatchSuccess();
	void CatchFail();
	BOOL GetFishingRot(int* pirot);
	BOOL GetFishingRod();

	// Render Mode
	void RestoreRenderMode();
	void SetAddRenderMode();
	void SetModulateRenderMode();
	void SetRenderMode(int iRenderMode);
	void SetAddColor(const D3DXCOLOR& c_rColor);

	// Position
	void SCRIPT_SetPixelPosition(float fx, float fy);
	void NEW_GetPixelPosition(TPixelPosition* pPixelPosition);

	// Rotation
	void NEW_LookAtFlyTarget();
	void NEW_LookAtDestInstance(CInstanceBase& rkInstDst);
	void NEW_LookAtDestPixelPosition(const TPixelPosition& c_rkPPosDst);

	float GetRotation();
	float GetAdvancingRotation();
	void SetRotation(float fRotation);
	void BlendRotation(float fRotation, float fBlendTime = 0.1f);

	void SetDirection(int dir);
	void BlendDirection(int dir, float blendTime);
	float GetDegreeFromDirection(int dir);

	// Motion
	// Motion Deque
	BOOL isLock();

	void SetMotionMode(int iMotionMode);
	int GetMotionMode(DWORD dwMotionIndex);

	// Motion
	// Pushing Motion
	void ResetLocalTime();
	void SetLoopMotion(WORD wMotion, float fBlendTime = 0.1f, float fSpeedRatio = 1.0f);
	void PushOnceMotion(WORD wMotion, float fBlendTime = 0.1f, float fSpeedRatio = 1.0f);
	void PushLoopMotion(WORD wMotion, float fBlendTime = 0.1f, float fSpeedRatio = 1.0f);
	void SetEndStopMotion();

	// Intersect
	bool IntersectDefendingSphere();
	bool IntersectBoundingBox();

	// Part
	//void SetParts(const WORD* c_pParts);
	void Refresh(DWORD dwMotIndex, bool isLoop);

	//void AttachEffectByID(DWORD dwParentPartIndex, const char* c_pszBoneName, DWORD dwEffectID, int dwLife = CActorInstance::EFFECT_LIFE_INFINITE); // 수명은 ms단위입니다.
	//void AttachEffectByName(DWORD dwParentPartIndex, const char* c_pszBoneName, const char* c_pszEffectName, int dwLife = CActorInstance::EFFECT_LIFE_INFINITE); // 수명은 ms단위입니다.

	float GetDistance(CInstanceBase* pkTargetInst);
	float GetDistance(const TPixelPosition& c_rPixelPosition);

	float GetBaseHeight();

	// ETC
	CActorInstance& GetGraphicThingInstanceRef();
	CActorInstance* GetGraphicThingInstancePtr();

	bool __Background_IsWaterPixelPosition(const TPixelPosition& c_rkPPos);
	bool __Background_GetWaterHeight(const TPixelPosition& c_rkPPos, float* pfHeight);

	// 2004.07.25.myevan.이펙트 안나오는 문제
	/////////////////////////////////////////////////////////////
	void __ClearAffectFlagContainer();
	void __ClearAffects();
	/////////////////////////////////////////////////////////////

	void __SetAffect(UINT eAffect, bool isVisible);

	void SetAffectFlagContainer(const CAffectFlagContainer& c_rkAffectFlagContainer);

	void __SetNormalAffectFlagContainer(const CAffectFlagContainer& c_rkAffectFlagContainer);
	void __SetStoneSmokeFlagContainer(const CAffectFlagContainer& c_rkAffectFlagContainer);

	void SetEmoticon(UINT eEmoticon);
	void SetFishEmoticon();
	bool IsPossibleEmoticon();

	void EnableSkipHorseCollision();

protected:
	UINT __LessRenderOrder_GetLODLevel();
	void __Initialize();
	void __InitializeRotationSpeed();

	void __Create_SetName(const SCreateData& c_rkCreateData);
	void __Create_SetWarpName(const SCreateData& c_rkCreateData);

	CInstanceBase* __GetMainInstancePtr();
	CInstanceBase* __FindInstancePtr(DWORD dwVID);

	bool __FindRaceType(DWORD dwRace, BYTE* pbType);
	DWORD __GetRaceType();

	bool __IsShapeAnimalWear();
	BOOL __IsChangableWeapon(int iWeaponID);

	void __EnableSkipCollision();
	void __DisableSkipCollision();

	void __ClearMainInstance();

	void __Shaman_SetParalysis(bool isParalysis);
	void __Warrior_SetGeomgyeongAffect(bool isVisible);
	void __Assassin_SetEunhyeongAffect(bool isVisible);
	void __SetReviveInvisibilityAffect(bool isVisible);

	BOOL __CanProcessNetworkStatePacket();

	bool __IsInDustRange();

	// Emotion
	void __ProcessFunctionEmotion(DWORD dwMotionNumber, DWORD dwTargetVID, const TPixelPosition& c_rkPosDst);
	void __EnableChangingTCPState();
	void __DisableChangingTCPState();
	BOOL __IsEnableTCPProcess(UINT eCurFunc);

	// 2004.07.17.levites.isShow를 ViewFrustumCheck로 변경
	bool __CanRender();
	bool __IsInViewFrustum();

	// HORSE
	void __AttachHorseSaddle();
	void __DetachHorseSaddle();

	struct SHORSE
	{
		bool m_isMounting;

		std::unique_ptr<CActorInstance> m_pkActor;

		SHORSE();
		~SHORSE();

		void Destroy();
		void Create(const TPixelPosition& c_rkPPos, UINT eRace, UINT eHitEffect);

		void SetAttackSpeed(UINT uAtkSpd);
		void SetMoveSpeed(UINT uMovSpd);
		void Deform();
		void Render();
		CActorInstance& GetActorRef();
		CActorInstance* GetActorPtr();

		bool IsMounting();
		bool CanAttack();
		bool CanUseSkill();

		UINT GetLevel();
		bool IsNewMount();

		void __Initialize();
	} m_kHorse;

#if defined(ENABLE_SOUL_SYSTEM)
	void __SetSoulAffect(UINT eAffect, bool isVisible);
#endif

protected:
	// Blend Mode
	void __SetBlendRenderingMode();
	void __SetAlphaValue(float fAlpha);
	float __GetAlphaValue();

	void __ComboProcess();
	void MovementProcess();
	void TodoProcess();
	void StateProcess();
	void AttackProcess();

	void StartWalking();
	float GetLocalTime();

	void RefreshState(DWORD dwMotIndex, bool isLoop);
	void RefreshActorInstance();

protected:
	void OnSyncing();
	void OnWaiting();
	void OnMoving();

	void NEW_SetCurPixelPosition(const TPixelPosition& c_rkPPosDst);
	void NEW_SetSrcPixelPosition(const TPixelPosition& c_rkPPosDst);
	void NEW_SetDstPixelPosition(const TPixelPosition& c_rkPPosDst);
	void NEW_SetDstPixelPositionZ(FLOAT z);

	const TPixelPosition& NEW_GetCurPixelPositionRef();
	const TPixelPosition& NEW_GetSrcPixelPositionRef();

public:
	const TPixelPosition& NEW_GetDstPixelPositionRef();

protected:
	BOOL m_isTextTail;

	// Instance Data
	std::string m_stName;

	DWORD m_adwPart[CRaceData::PART_MAX_NUM];

	DWORD m_dwLevel;
#if defined(ENABLE_CONQUEROR_LEVEL)
	DWORD m_dwConquerorLevel;
#endif
#ifdef WJ_SHOW_MOB_INFO
	DWORD m_dwAIFlag;
#endif
	DWORD m_dwEmpireID;
	DWORD m_dwGuildID;
#if defined(ENABLE_GUILD_LEADER_GRADE_NAME)
	BYTE m_bGuildLeaderGrade;
#endif
#if defined(ENABLE_MULTI_LANGUAGE_SYSTEM)
	std::string m_stCountry;
#endif
#if defined(ENABLE_LEFT_SEAT)
	bool m_bLeftSeat;
#endif

protected:
	CAffectFlagContainer m_kAffectFlagContainer;
	DWORD m_adwCRCAffectEffect[AFFECT_NUM];

	UINT __GetRefinedEffect(CItemData* pItem);
	void __ClearWeaponRefineEffect();
	void __ClearArmorRefineEffect();
#if defined(ENABLE_ACCE_COSTUME_SYSTEM)
	void __ClearAcceRefineEffect();
#endif

protected:
	void __AttachSelectEffect();
	void __DetachSelectEffect();

	void __AttachTargetEffect();
	void __DetachTargetEffect();

	void __AttachEmpireEffect(DWORD eEmpire);

protected:
	struct SEffectContainer
	{
		typedef std::map<DWORD, DWORD> Dict;
		Dict m_kDct_dwEftID;
	} m_kEffectContainer;

	void __EffectContainer_Initialize();
	void __EffectContainer_Destroy();

	DWORD __EffectContainer_AttachEffect(DWORD eEffect);
	void __EffectContainer_DetachEffect(DWORD eEffect);

	SEffectContainer::Dict& __EffectContainer_GetDict();

protected:
	struct SStoneSmoke
	{
		DWORD m_dwEftID;
	} m_kStoneSmoke;

	void __StoneSmoke_Inialize();
	void __StoneSmoke_Destroy();
	void __StoneSmoke_Create(DWORD eSmoke);

#if defined(ENABLE_GRAPHIC_ON_OFF)
public:
	bool IsMainInstance() const;
	bool CanRenderActor() const;
	bool CanRenderEffect() const;
	bool HasPrivateShopSign() const;
	DWORD GetDistanceBetweenShop();
	void SetRank(BYTE bRank);
#endif

#if defined(ENABLE_OPTIMIZATION)
	void AttackProcessOnlySelf();
#endif

protected:
	// Emoticon
	//DWORD m_adwCRCEmoticonEffect[EMOTICON_NUM];

	BYTE m_eType;
	BYTE m_eRaceType;
	DWORD m_eShape;
	DWORD m_dwRace;
	DWORD m_dwVirtualNumber;
	short m_sAlignment;
	BYTE m_byPKMode;
	bool m_isKiller;
	bool m_isPartyMember;

	// Movement
	int m_iRotatingDirection;

	DWORD m_dwAdvActorVID;
	DWORD m_dwLastDmgActorVID;

	LONG m_nAverageNetworkGap;
	DWORD m_dwNextUpdateHeightTime;

	bool m_isGoing;

	TPixelPosition m_kPPosDust;

	DWORD m_dwLastComboIndex;

	DWORD m_swordRefineEffectRight;
	DWORD m_swordRefineEffectLeft;
	DWORD m_armorRefineEffect;
#if defined(ENABLE_REFINE_ELEMENT_SYSTEM)
	WORD m_wRefineElementAffectType;
	DWORD m_dwSwordRefineElementEffectRight;
	DWORD m_dwSwordRefineElementEffectLeft;
#endif
#if defined(ENABLE_ACCE_COSTUME_SYSTEM)
	DWORD m_acceRefineEffect;
#endif
#if defined(ENABLE_AURA_COSTUME_SYSTEM)
	DWORD m_auraRefineEffect;
#endif
#if defined(ENABLE_GUILD_DRAGONLAIR_SYSTEM)
	DWORD m_dragonLairStoneEffect;
#endif
#if defined(ENABLE_SUMMER_EVENT_ROULETTE)
	DWORD m_specialRouletteEffect;
#endif

	struct SMoveAfterFunc
	{
		UINT eFunc;
		UINT uArg;

		// For Emotion Function
		UINT uArgExpanded;
		TPixelPosition kPosDst;
	};

	SMoveAfterFunc m_kMovAfterFunc;

	float m_fDstRot;
	float m_fAtkPosTime;
	float m_fRotSpd;
	float m_fMaxRotSpd;

	BOOL m_bEnableTCPState;

	// Graphic Instance
	CActorInstance m_GraphicThingInstance;

protected:
	struct SCommand
	{
		DWORD m_dwChkTime;
		DWORD m_dwCmdTime;
		float m_fDstRot;
		UINT m_eFunc;
		UINT m_uArg;
		UINT m_uTargetVID;
		TPixelPosition m_kPPosDst;
	};

	typedef std::list<SCommand> CommandQueue;

	DWORD m_dwBaseChkTime;
	DWORD m_dwBaseCmdTime;

	DWORD m_dwSkipTime;

	CommandQueue m_kQue_kCmdNew;

	BOOL m_bDamageEffectType;

	struct SEffectDamage
	{
		DWORD damage;
		BYTE flag;
		BOOL bSelf;
		BOOL bTarget;
	};

	typedef std::list<SEffectDamage> CommandDamageQueue;
	CommandDamageQueue m_DamageQueue;

	bool ProcessDamage();

public:
	void AddDamageEffect(DWORD damage, BYTE flag, BOOL bSelf, BOOL bTarget);

protected:
	struct SWarrior
	{
		DWORD m_dwGeomgyeongEffect;
	};

	SWarrior m_kWarrior;

	void __Warrior_Initialize();

public:
	static void ClearPVPKeySystem();

	static void InsertPVPKey(DWORD dwSrcVID, DWORD dwDstVID);
	static void InsertPVPReadyKey(DWORD dwSrcVID, DWORD dwDstVID);
	static void RemovePVPKey(DWORD dwSrcVID, DWORD dwDstVID);

	static void InsertGVGKey(DWORD dwSrcGuildVID, DWORD dwDstGuildVID);
	static void RemoveGVGKey(DWORD dwSrcGuildVID, DWORD dwDstGuildVID);

	static void InsertDUELKey(DWORD dwSrcVID, DWORD dwDstVID);

	UINT GetNameColorIndex();

	const D3DXCOLOR& GetNameColor();
	const D3DXCOLOR& GetTitleColor();

protected:
	static DWORD __GetPVPKey(DWORD dwSrcVID, DWORD dwDstVID);
	static bool __FindPVPKey(DWORD dwSrcVID, DWORD dwDstVID);
	static bool __FindPVPReadyKey(DWORD dwSrcVID, DWORD dwDstVID);
	static bool __FindGVGKey(DWORD dwSrcGuildID, DWORD dwDstGuildID);
	static bool __FindDUELKey(DWORD dwSrcGuildID, DWORD dwDstGuildID);

protected:
	CActorInstance::IEventHandler* GetEventHandlerPtr();
	CActorInstance::IEventHandler& GetEventHandlerRef();

protected:
	static float __GetBackgroundHeight(float x, float y);
	static DWORD __GetShadowMapColor(float x, float y);

public:
	static void ResetPerformanceCounter();
	static void GetInfo(std::string* pstInfo);

public:
	static CInstanceBase* New();
	static void Delete(CInstanceBase* pkInst);

	static CDynamicPool<CInstanceBase>	ms_kPool;

protected:
	static DWORD ms_dwUpdateCounter;
	static DWORD ms_dwRenderCounter;
	static DWORD ms_dwDeformCounter;

public:
	DWORD GetDuelMode();
	void SetDuelMode(DWORD type);

protected:
	DWORD m_dwDuelMode;
	DWORD m_dwEmoticonTime;

#if defined(RENDER_TARGET)
public:
	void SetLODLimits(DWORD dwModelIndex, float fFarLOD);
	void SetAlwaysRender(bool val);
protected:
	bool m_bIsAlwaysRender;
#endif
};

inline int RaceToJob(int race)
{
	if (race == 8)
		return 4;

	const int JOB_NUM = 4;
	return race % JOB_NUM;
}

inline int RaceToSex(int race)
{
	switch (race)
	{
		case 0:
		case 2:
		case 5:
		case 7:
		case 8:
			return 1;
		case 1:
		case 3:
		case 4:
		case 6:
			return 0;
	}
	return 0;
}
