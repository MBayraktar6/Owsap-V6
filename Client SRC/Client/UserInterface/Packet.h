#pragma once

#include "../GameLib/RaceData.h"
#include "PythonNonPlayer.h"
#include "Locale_inc.h"

typedef BYTE TPacketHeader;

enum CG_HEADERS
{
	HEADER_CG_LOGIN = 1,
	HEADER_CG_ATTACK = 2,
	HEADER_CG_CHAT = 3,
	HEADER_CG_PLAYER_CREATE = 4, // 새로운 플래이어를 생성
	HEADER_CG_PLAYER_DESTROY = 5, // 플래이어를 삭제.
	HEADER_CG_PLAYER_SELECT = 6,
	HEADER_CG_CHARACTER_MOVE = 7,
	HEADER_CG_SYNC_POSITION = 8,
	//HEADER_CG_DIRECT_ENTER = 9,
	HEADER_CG_ENTERGAME = 10,

	HEADER_CG_ITEM_USE = 11,
	HEADER_CG_ITEM_DROP = 12,
	HEADER_CG_ITEM_MOVE = 13,
	//HEADER_CG_UNUSED = 14,
	HEADER_CG_ITEM_PICKUP = 15,

	HEADER_CG_QUICKSLOT_ADD = 16,
	HEADER_CG_QUICKSLOT_DEL = 17,
	HEADER_CG_QUICKSLOT_SWAP = 18,
	HEADER_CG_WHISPER = 19,
	HEADER_CG_ITEM_DROP2 = 20,
#if defined(ENABLE_NEW_DROP_DIALOG)
	HEADER_CG_ITEM_DESTROY = 21,
#endif

	//HEADER_CG_UNUSED = 22,
	//HEADER_CG_UNUSED = 23,
	//HEADER_CG_UNUSED = 24,
	//HEADER_CG_UNUSED = 25,

	HEADER_CG_ON_CLICK = 26,
	HEADER_CG_EXCHANGE = 27,
	HEADER_CG_CHARACTER_POSITION = 28,
	HEADER_CG_SCRIPT_ANSWER = 29,
	HEADER_CG_QUEST_INPUT_STRING = 30,
	HEADER_CG_QUEST_CONFIRM = 31,
#if defined(ENABLE_OX_RENEWAL)
	HEADER_CG_QUEST_INPUT_LONG_STRING = 32,
#endif
	//HEADER_CG_UNUSED = 33,
	//HEADER_CG_UNUSED = 34,
	//HEADER_CG_UNUSED = 35,
	//HEADER_CG_UNUSED = 36,
	//HEADER_CG_UNUSED = 37,
	//HEADER_CG_UNUSED = 38,
	//HEADER_CG_UNUSED = 39,
	//HEADER_CG_UNUSED = 40,
	//HEADER_CG_UNUSED = 41,
	//HEADER_CG_UNUSED = 42,
	//HEADER_CG_UNUSED = 43,
	//HEADER_CG_UNUSED = 44,
	//HEADER_CG_UNUSED = 45,
	//HEADER_CG_UNUSED = 46,
#if defined(ENABLE_EXTEND_INVEN_SYSTEM)
	HEADER_CG_EXTEND_INVEN = 47,
#endif
	//HEADER_CG_UNUSED = 48,
	//HEADER_CG_UNUSED = 49,

	HEADER_CG_SHOP = 50,
	HEADER_CG_FLY_TARGETING = 51,
	HEADER_CG_USE_SKILL = 52,
	HEADER_CG_ADD_FLY_TARGETING = 53,
	HEADER_CG_SHOOT = 54,
	HEADER_CG_MYSHOP = 55,
#if defined(ENABLE_MYSHOP_DECO)
	HEADER_CG_MYSHOP_DECO = 56,
	HEADER_CG_MYSHOP_DECO_ADD = 57,
#endif
	//HEADER_CG_UNUSED = 58,

#if defined(ENABLE_SEND_TARGET_INFO)
	HEADER_CG_TARGET_INFO = 59,
#endif
	HEADER_CG_ITEM_USE_TO_ITEM = 60,
	HEADER_CG_TARGET = 61,

	//HEADER_CG_UNUSED = 62,
	//HEADER_CG_UNUSED = 63,
	HEADER_CG_TEXT = 64, // @ 로 시작되면 텍스트를 파싱한다.
	HEADER_CG_WARP = 65,
	HEADER_CG_SCRIPT_BUTTON = 66,
	HEADER_CG_MESSENGER = 67,
	//HEADER_CG_UNUSED = 68,

	HEADER_CG_MALL_CHECKOUT = 69,
	HEADER_CG_SAFEBOX_CHECKIN = 70, // 아이템을 창고에 넣는다.
	HEADER_CG_SAFEBOX_CHECKOUT = 71, // 아이템을 창고로 부터 빼온다.

	HEADER_CG_PARTY_INVITE = 72,
	HEADER_CG_PARTY_INVITE_ANSWER = 73,
	HEADER_CG_PARTY_REMOVE = 74,
	HEADER_CG_PARTY_SET_STATE = 75,
	HEADER_CG_PARTY_USE_SKILL = 76,
	HEADER_CG_SAFEBOX_ITEM_MOVE = 77,
	HEADER_CG_PARTY_PARAMETER = 78,
	//HEADER_CG_UNUSED = 79,

	HEADER_CG_GUILD = 80,
	HEADER_CG_ANSWER_MAKE_GUILD = 81,
	HEADER_CG_FISHING = 82,
	HEADER_CG_GIVE_ITEM = 83,
	//HEADER_CG_UNUSED = 84,
	//HEADER_CG_UNUSED = 85,
	//HEADER_CG_UNUSED = 86,
	//HEADER_CG_UNUSED = 87,
	//HEADER_CG_UNUSED = 88,
	//HEADER_CG_UNUSED = 89,

	HEADER_CG_EMPIRE = 90,
	//HEADER_CG_UNUSED = 91,
	//HEADER_CG_UNUSED = 92,
	//HEADER_CG_UNUSED = 93,
	//HEADER_CG_UNUSED = 94,
	//HEADER_CG_UNUSED = 95,

	HEADER_CG_REFINE = 96,
#if defined(ENABLE_CUBE_RENEWAL)
	HEADER_CG_CUBE = 97,
#endif
	//HEADER_CG_UNUSED = 98,
	//HEADER_CG_UNUSED = 99,

	HEADER_CG_MARK_LOGIN = 100,
	HEADER_CG_MARK_CRCLIST = 101,
	HEADER_CG_MARK_UPLOAD = 102,
	//HEADER_CG_CRC_REPORT = 103,
	HEADER_CG_MARK_IDXLIST = 104,

	HEADER_CG_HACK = 105,
	HEADER_CG_CHANGE_NAME = 106,
	//HEADER_CG_UNUSED = 107,
	//HEADER_CG_UNUSED = 108,
	HEADER_CG_LOGIN2 = 109,
	HEADER_CG_DUNGEON = 110,
	HEADER_CG_LOGIN3 = 111,

	HEADER_CG_GUILD_SYMBOL_UPLOAD = 112,
	HEADER_CG_GUILD_SYMBOL_CRC = 113,

	// SCRIPT_SELECT_ITEM
	HEADER_CG_SCRIPT_SELECT_ITEM = 114,
	// END_OF_SCRIPT_SELECT_ITEM

#if defined(ENABLE_GEM_SYSTEM)
	HEADER_CG_SELECT_ITEM_EX = 115,
#endif

	//HEADER_CG_UNUSED = 116,
	//HEADER_CG_UNUSED = 117,
	//HEADER_CG_UNUSED = 118,
	//HEADER_CG_UNUSED = 119,
#if defined(ENABLE_QUEST_REQUEST_EVENT)
	HEADER_CG_REQUEST_EVENT_QUEST = 120,
#endif
#if defined(ENABLE_LEFT_SEAT)
	HEADER_CG_LEFT_SEAT = 121,
#endif
	//HEADER_CG_UNUSED = 122,
	//HEADER_CG_UNUSED = 123,
	//HEADER_CG_UNUSED = 124,
	//HEADER_CG_UNUSED = 125,
	//HEADER_CG_UNUSED = 126,
	//HEADER_CG_UNUSED = 127,
	//HEADER_CG_UNUSED = 128,
	//HEADER_CG_UNUSED = 129,
	//HEADER_CG_UNUSED = 130,
	//HEADER_CG_UNUSED = 131,
	//HEADER_CG_UNUSED = 132,
	//HEADER_CG_UNUSED = 133,
	//HEADER_CG_UNUSED = 134,
	//HEADER_CG_UNUSED = 135,
	//HEADER_CG_UNUSED = 136,
	//HEADER_CG_UNUSED = 137,
	//HEADER_CG_UNUSED = 138,
	//HEADER_CG_UNUSED = 139,
	//HEADER_CG_UNUSED = 140,
	//HEADER_CG_UNUSED = 141,
	//HEADER_CG_UNUSED = 142,
	//HEADER_CG_UNUSED = 143,
	//HEADER_CG_UNUSED = 144,
#if defined(ENABLE_FISHING_GAME)
	HEADER_CG_FISHING_GAME = 145,
#endif
#if defined(ENABLE_REFINE_ELEMENT_SYSTEM)
	HEADER_CG_REFINE_ELEMENT = 146,
#endif
	//HEADER_CG_UNUSED = 147,
	//HEADER_CG_UNUSED = 148,
	//HEADER_CG_UNUSED = 149,
	//HEADER_CG_UNUSED = 150,
#if defined(ENABLE_SKILLBOOK_COMB_SYSTEM)
	HEADER_CG_SKILLBOOK_COMB = 151,
#endif
	//HEADER_CG_UNUSED = 152,
	//HEADER_CG_UNUSED = 153,
	//HEADER_CG_UNUSED = 154,
	//HEADER_CG_UNUSED = 155,
	//HEADER_CG_UNUSED = 156,
	//HEADER_CG_UNUSED = 157,
	//HEADER_CG_UNUSED = 158,
	//HEADER_CG_UNUSED = 159,
	//HEADER_CG_UNUSED = 160,
	//HEADER_CG_UNUSED = 161,
	//HEADER_CG_UNUSED = 162,
	//HEADER_CG_UNUSED = 163,
	//HEADER_CG_UNUSED = 164,
	//HEADER_CG_UNUSED = 165,
	//HEADER_CG_UNUSED = 166,
	//HEADER_CG_UNUSED = 167,
	//HEADER_CG_UNUSED = 168,
#if defined(ENABLE_ATTR_6TH_7TH)
	HEADER_CG_ATTR67_ADD = 169,
#endif
#if defined(ENABLE_SNOWFLAKE_STICK_EVENT)
	HEADER_CG_SNOWFLAKE_STICK_EVENT = 170,
#endif
	//HEADER_CG_UNUSED = 171,
	//HEADER_CG_UNUSED = 172,
	//HEADER_CG_UNUSED = 173,
	//HEADER_CG_UNUSED = 174,
	//HEADER_CG_UNUSED = 175,
	//HEADER_CG_UNUSED = 176,
	//HEADER_CG_UNUSED = 177,
	//HEADER_CG_UNUSED = 178,
	//HEADER_CG_UNUSED = 179,
	//HEADER_CG_UNUSED = 180,
#if defined(ENABLE_MINI_GAME_RUMI)
	HEADER_CG_MINI_GAME_RUMI = 181,
#endif
#if defined(ENABLE_MINI_GAME_YUTNORI)
	HEADER_CG_MINI_GAME_YUTNORI = 182,
#endif
#if defined(ENABLE_GEM_SYSTEM)
	HEADER_CG_GEM_SHOP = 183,
#endif
	//HEADER_CG_UNUSED = 184,
	//HEADER_CG_UNUSED = 185,
#if defined(ENABLE_SUMMER_EVENT_ROULETTE)
	HEADER_CG_MINI_GAME_ROULETTE = 186,
#endif
#if defined(ENABLE_FLOWER_EVENT)
	HEADER_CG_FLOWER_EVENT = 187,
#endif
	//HEADER_CG_UNUSED = 188,
	//HEADER_CG_UNUSED = 189,
	//HEADER_CG_UNUSED = 190,
	//HEADER_CG_UNUSED = 191,
	//HEADER_CG_UNUSED = 192,
	//HEADER_CG_UNUSED = 193,
	//HEADER_CG_UNUSED = 194,
	//HEADER_CG_UNUSED = 195,
	//HEADER_CG_UNUSED = 196,
	//HEADER_CG_UNUSED = 197,
	//HEADER_CG_UNUSED = 198,
	//HEADER_CG_UNUSED = 199,
	//HEADER_CG_UNUSED = 200,
	//HEADER_CG_UNUSED = 201,
	//HEADER_CG_UNUSED = 202,

	// NOTE : 이런 개XXX 정말 이거 Packet설계한 사람은 누구냐. 이렇게 코딩하고 밥이 넘어가나.
	// enum을 별도로 구별을 하던가. 아님 namepsace로 구별을 하던가..
	// 정말 packet generator까지는 바라지도 않는다. 이런 씨XX
	// 이러다가 숫자 겹치면 누가 책임지는데???
	//HEADER_CG_HS_ACK = 203,
	//HEADER_CG_XTRAP_ACK = 204,

	HEADER_CG_DRAGON_SOUL_REFINE = 205,
	HEADER_CG_STATE_CHECKER = 206,

	HEADER_CG_PHASE = 207,
#if defined(ENABLE_LUCKY_BOX)
	HEADER_CG_LUCKY_BOX = 208,
#endif
	//HEADER_CG_UNUSED = 209,
	//HEADER_CG_UNUSED = 210,
#if defined(ENABLE_ACCE_COSTUME_SYSTEM)
	HEADER_CG_ACCE_REFINE = 211,
#endif
#if defined(ENABLE_AURA_COSTUME_SYSTEM)
	HEADER_CG_AURA_REFINE = 212,
#endif
	//HEADER_CG_UNUSED = 213,
	//HEADER_CG_UNUSED = 214,
#if defined(ENABLE_MAILBOX)
	HEADER_CG_MAILBOX_WRITE = 215,
	HEADER_CG_MAILBOX_WRITE_CONFIRM = 216,
	HEADER_CG_MAILBOX_PROCESS = 217,
#endif
#if defined(ENABLE_MOVE_COSTUME_ATTR)
	HEADER_CG_ITEM_COMBINATION = 218,
	HEADER_CG_ITEM_COMBINATION_CANCEL = 219,
#endif
#if defined(ENABLE_PRIVATESHOP_SEARCH_SYSTEM)
	HEADER_CG_PRIVATESHOP_SEARCH = 220,
	HEADER_CG_PRIVATESHOP_SEARCH_CLOSE = 221,
	HEADER_CG_PRIVATESHOP_SEARCH_BUY_ITEM = 222,
#endif
	//HEADER_CG_UNUSED = 223,
	//HEADER_CG_UNUSED = 224,
	//HEADER_CG_UNUSED = 225,
#if defined(ENABLE_MINI_GAME_CATCH_KING)
	HEADER_CG_MINI_GAME_CATCH_KING = 226,
#endif
#if defined(ENABLE_CHANGED_ATTR)
	HEADER_CG_ITEM_SELECT_ATTR = 227,
#endif
#if defined(ENABLE_CHANGE_LOOK_SYSTEM)
	HEADER_CG_CHANGE_LOOK = 228,
#endif
	//HEADER_CG_UNUSED = 229,
#if defined(ENABLE_MULTI_LANGUAGE_SYSTEM)
	HEADER_CG_CHANGE_LANGUAGE = 230,
#endif
	//HEADER_CG_UNUSED = 231,
	//HEADER_CG_UNUSED = 232,
	//HEADER_CG_UNUSED = 233,
	//HEADER_CG_UNUSED = 234,
#if defined(ENABLE_LOOTING_SYSTEM)
	HEADER_CG_LOOT_FILTER = 235,
#endif
	//HEADER_CG_UNUSED = 236,
	//HEADER_CG_UNUSED = 237,
	//HEADER_CG_UNUSED = 238,
	//HEADER_CG_UNUSED = 239,
	//HEADER_CG_UNUSED = 240,
	HEADER_CG_CLIENT_VERSION2 = 0xf1, // 241
	//HEADER_CG_UNUSED = 242,
	//HEADER_CG_UNUSED = 243,
	//HEADER_CG_UNUSED = 244,
	//HEADER_CG_UNUSED = 245,
	//HEADER_CG_UNUSED = 246,
	//HEADER_CG_UNUSED = 247,
	//HEADER_CG_UNUSED = 248,
	//HEADER_CG_UNUSED = 249,
	//HEADER_CG_UNUSED = 250,
#if defined(__IMPROVED_PACKET_ENCRYPTION__)
	HEADER_CG_KEY_AGREEMENT = 0xfb, // __IMPROVED_PACKET_ENCRYPTION__ // 251
#endif
	HEADER_CG_TIME_SYNC = 0xfc, // 252
	HEADER_CG_CLIENT_VERSION = 0xfd, // 253
	HEADER_CG_PONG = 0xfe, // 254
	HEADER_CG_HANDSHAKE = 0xff, // 255
};

enum GC_HEADERS
{
	HEADER_GC_CHARACTER_ADD = 1,
	HEADER_GC_CHARACTER_DEL = 2,
	HEADER_GC_CHARACTER_MOVE = 3,
	HEADER_GC_CHAT = 4,
	HEADER_GC_SYNC_POSITION = 5,

	HEADER_GC_LOGIN_SUCCESS3 = 6,
	HEADER_GC_LOGIN_FAILURE = 7,

	HEADER_GC_PLAYER_CREATE_SUCCESS = 8,
	HEADER_GC_PLAYER_CREATE_FAILURE = 9,
	HEADER_GC_PLAYER_DELETE_SUCCESS = 10,
	HEADER_GC_PLAYER_DELETE_WRONG_SOCIAL_ID = 11,

	//HEADER_GC_UNUSED = 24,
	HEADER_GC_STUN = 13,
	HEADER_GC_DEAD = 14,

	HEADER_GC_MAIN_CHARACTER = 15,
	HEADER_GC_PLAYER_POINTS = 16,
	HEADER_GC_PLAYER_POINT_CHANGE = 17,
	HEADER_GC_CHANGE_SPEED = 18,
	HEADER_GC_CHARACTER_UPDATE = 19,

	HEADER_GC_ITEM_SET_EMPTY = 20, // 아이템 창에 추가
	HEADER_GC_ITEM_SET = 21, // 아이템 창에 추가
	HEADER_GC_ITEM_USE = 22, // 아이템 사용 (주위 사람들에게 보여주기 위해)
	HEADER_GC_ITEM_DROP = 23, // 아이템 버리기

	//HEADER_GC_UNUSED = 24,
	HEADER_GC_ITEM_UPDATE = 25, // 아이템 수치 업데이트
	HEADER_GC_ITEM_GROUND_ADD = 26, // 바닥에 아이템 추가
	HEADER_GC_ITEM_GROUND_DEL = 27, // 바닥에서 아이템 삭제

	HEADER_GC_QUICKSLOT_ADD = 28,
	HEADER_GC_QUICKSLOT_DEL = 29,
	HEADER_GC_QUICKSLOT_SWAP = 30,

	HEADER_GC_ITEM_OWNERSHIP = 31,

	HEADER_GC_LOGIN_SUCCESS4 = 32,

	//HEADER_GC_UNUSED = 33,
	HEADER_GC_WHISPER = 34,
	//HEADER_GC_UNUSED = 35,

	HEADER_GC_MOTION = 36,
	HEADER_GC_EMOTE = 37,

	HEADER_GC_SHOP = 38,
	HEADER_GC_SHOP_SIGN = 39,

	HEADER_GC_DUEL_START = 40,
	HEADER_GC_PVP = 41,
	HEADER_GC_EXCHANGE = 42,
	HEADER_GC_CHARACTER_POSITION = 43,

	HEADER_GC_PING = 44,
	HEADER_GC_SCRIPT = 45,
	HEADER_GC_QUEST_CONFIRM = 46,

#if defined(ENABLE_EXTEND_INVEN_SYSTEM)
	HEADER_GC_EXTEND_INVEN = 47,
	HEADER_GC_EXTEND_INVEN_ITEM_USE = 48,
#endif
#if defined(ENABLE_GUILD_DRAGONLAIR_SYSTEM)
	HEADER_GC_GUILD_DRAGONLAIR = 49,
#endif
#if defined(ENABLE_CLIENT_TIMER)
	HEADER_GC_CLIENT_TIMER = 50,
#endif
	//HEADER_GC_UNUSED = 51,
	//HEADER_GC_UNUSED = 52,
	//HEADER_GC_UNUSED = 53,
	//HEADER_GC_UNUSED = 54,
	//HEADER_GC_UNUSED = 55,
	//HEADER_GC_UNUSED = 56,
	//HEADER_GC_UNUSED = 57,
#if defined(ENABLE_SEND_TARGET_INFO)
	HEADER_GC_TARGET_INFO = 58,
#endif
	//HEADER_GC_UNUSED = 59,
	//HEADER_GC_UNUSED = 60,
	HEADER_GC_MOUNT = 61,
	HEADER_GC_OWNERSHIP = 62,
	HEADER_GC_TARGET = 63,
	//HEADER_GC_UNUSED = 64,
	HEADER_GC_WARP = 65,
	//HEADER_GC_UNUSED = 66,
	//HEADER_GC_UNUSED = 67,
	//HEADER_GC_UNUSED = 68,
	HEADER_GC_ADD_FLY_TARGETING = 69,

	HEADER_GC_CREATE_FLY = 70,
	HEADER_GC_FLY_TARGETING = 71,
	HEADER_GC_SKILL_LEVEL = 72,
	//HEADER_GC_UNUSED = 73,
	HEADER_GC_MESSENGER = 74,
	HEADER_GC_GUILD = 75,
	HEADER_GC_SKILL_LEVEL_NEW = 76,

	HEADER_GC_PARTY_INVITE = 77,
	HEADER_GC_PARTY_ADD = 78,
	HEADER_GC_PARTY_UPDATE = 79,
	HEADER_GC_PARTY_REMOVE = 80,

	HEADER_GC_QUEST_INFO = 81,
	HEADER_GC_REQUEST_MAKE_GUILD = 82,
	HEADER_GC_PARTY_PARAMETER = 83,
	//HEADER_GC_UNUSED = 84,

	HEADER_GC_SAFEBOX_MONEY_CHANGE = 84,
	HEADER_GC_SAFEBOX_SET = 85,
	HEADER_GC_SAFEBOX_DEL = 86,
	HEADER_GC_SAFEBOX_WRONG_PASSWORD = 87,
	HEADER_GC_SAFEBOX_SIZE = 88,

	HEADER_GC_FISHING = 89,

	HEADER_GC_EMPIRE = 90,

	HEADER_GC_PARTY_LINK = 91,
	HEADER_GC_PARTY_UNLINK = 92,
	//HEADER_GC_UNUSED = 93,
	//HEADER_GC_UNUSED = 94,
	HEADER_GC_REFINE_INFORMATION = 95,
	//HEADER_GC_UNUSED = 96,
#if defined(ENABLE_CUBE_RENEWAL)
	HEADER_GC_CUBE = 97,
#endif
	//HEADER_GC_UNUSED = 98,
	HEADER_GC_VIEW_EQUIP = 99,

	HEADER_GC_MARK_BLOCK = 100,
	//HEADER_GC_MARK_DIFF_DATA = 101,
	HEADER_GC_MARK_IDXLIST = 102,
	//HEADER_GC_UNUSED = 103,
	//HEADER_GC_UNUSED = 104,
	//HEADER_GC_SLOW_TIMER = 105,
	HEADER_GC_TIME = 106,
	HEADER_GC_CHANGE_NAME = 107,
	//HEADER_GC_UNUSED = 108,
	//HEADER_GC_UNUSED = 109,

	HEADER_GC_DUNGEON = 110,

	HEADER_GC_WALK_MODE = 111,
	HEADER_GC_CHANGE_SKILL_GROUP = 112,
	HEADER_GC_MAIN_CHARACTER2_EMPIRE = 113,

	HEADER_GC_SEPCIAL_EFFECT = 114,
	HEADER_GC_NPC_POSITION = 115,

	//HEADER_CG_UNUSED = 116,
	//HEADER_GC_UNUSED = 117,
	HEADER_GC_LOGIN_KEY = 118,

	HEADER_GC_REFINE_INFORMATION_NEW = 119,
	//HEADER_GC_UNUSED = 120,
	HEADER_GC_CHANNEL = 121,

	HEADER_GC_MALL_OPEN = 122,

	HEADER_GC_TARGET_UPDATE = 123,
	HEADER_GC_TARGET_DELETE = 124,
	HEADER_GC_TARGET_CREATE_NEW = 125,

	HEADER_GC_AFFECT_ADD = 126,
	HEADER_GC_AFFECT_REMOVE = 127,

	HEADER_GC_MALL_SET = 128,
	HEADER_GC_MALL_DEL = 129,

	HEADER_GC_LAND_LIST = 130,
	HEADER_GC_LOVER_INFO = 131,
	HEADER_GC_LOVE_POINT_UPDATE = 132,

	HEADER_GC_GUILD_SYMBOL_DATA = 133,

	// MINING
	HEADER_GC_DIG_MOTION = 134,
	// END_OF_MINING

	HEADER_GC_DAMAGE_INFO = 135,
	HEADER_GC_CHAR_ADDITIONAL_INFO = 136,

	// SUPPORT_BGM
	HEADER_GC_MAIN_CHARACTER3_BGM = 137,
	HEADER_GC_MAIN_CHARACTER4_BGM_VOL = 138,
	// END_OF_SUPPORT_BGM

	//HEADER_GC_UNUSED = 139,
	//HEADER_GC_UNUSED = 140,
	//HEADER_GC_UNUSED = 141,
	//HEADER_GC_UNUSED = 142,
	//HEADER_GC_UNUSED = 143,
	//HEADER_GC_UNUSED = 144,
#if defined(ENABLE_FISHING_GAME)
	HEADER_GC_FISHING_GAME = 145,
#endif
#if defined(ENABLE_REFINE_ELEMENT_SYSTEM)
	HEADER_GC_REFINE_ELEMENT = 146,
#endif
	//HEADER_GC_UNUSED = 147,
	//HEADER_GC_UNUSED = 148,
	//HEADER_GC_UNUSED = 149,

	HEADER_GC_AUTH_SUCCESS = 150,
	HEADER_GC_PANAMA_PACK = 151,

	// HYBRID CRYPT
	HEADER_GC_HYBRIDCRYPT_KEYS = 152,
	HEADER_GC_HYBRIDCRYPT_SDB = 153, // SDB means Supplmentary Data Blocks
	// HYBRID CRYPT

	//HEADER_GC_UNUSED = 154,
	//HEADER_GC_UNUSED = 155,
#if defined(ENABLE_PRIVATESHOP_SEARCH_SYSTEM)
	HEADER_GC_PRIVATESHOP_SEARCH = 156,
	HEADER_GC_PRIVATESHOP_SEARCH_OPEN = 157,
#endif
	//HEADER_GC_UNUSED = 158,
	//HEADER_GC_UNUSED = 159,
	//HEADER_GC_UNUSED = 160,
	//HEADER_GC_UNUSED = 161,
	//HEADER_GC_UNUSED = 162,
	//HEADER_GC_UNUSED = 163,
	//HEADER_GC_UNUSED = 164,
	//HEADER_GC_UNUSED = 165,
	//HEADER_GC_UNUSED = 166,
	//HEADER_GC_UNUSED = 167,
	//HEADER_GC_UNUSED = 168,
	//HEADER_GC_UNUSED = 169,
#if defined(ENABLE_SNOWFLAKE_STICK_EVENT)
	HEADER_GC_SNOWFLAKE_STICK_EVENT = 170,
#endif
	//HEADER_GC_UNUSED = 171,
	//HEADER_GC_UNUSED = 172,
	//HEADER_GC_UNUSED = 173,
	//HEADER_GC_UNUSED = 174,
	//HEADER_GC_UNUSED = 175,
	//HEADER_GC_UNUSED = 176,
	//HEADER_GC_UNUSED = 177,
	//HEADER_GC_UNUSED = 178,
	//HEADER_GC_UNUSED = 179,
	//HEADER_GC_UNUSED = 180,
#if defined(ENABLE_MINI_GAME_RUMI)
	HEADER_GC_MINI_GAME_RUMI = 181,
#endif
#if defined(ENABLE_MINI_GAME_YUTNORI)
	HEADER_GC_MINI_GAME_YUTNORI = 182,
#endif
	//HEADER_GC_UNUSED = 183,
	//HEADER_GC_UNUSED = 184,
	//HEADER_GC_UNUSED = 185,
#if defined(ENABLE_SUMMER_EVENT_ROULETTE)
	HEADER_GC_MINI_GAME_ROULETTE = 186,
#endif
#if defined(ENABLE_FLOWER_EVENT)
	HEADER_GC_FLOWER_EVENT = 187,
#endif
	//HEADER_GC_UNUSED = 188,
	//HEADER_GC_UNUSED = 189,
	//HEADER_GC_UNUSED = 190,
	//HEADER_GC_UNUSED = 191,
	//HEADER_GC_UNUSED = 192,
	//HEADER_GC_UNUSED = 193,
	//HEADER_GC_UNUSED = 194,
	//HEADER_GC_UNUSED = 195,
	//HEADER_GC_UNUSED = 196,
	//HEADER_GC_UNUSED = 197,
	//HEADER_GC_UNUSED = 198,
	//HEADER_GC_UNUSED = 199,
	//HEADER_GC_UNUSED = 200,

#if defined(ENABLE_GEM_SYSTEM)
	HEADER_GC_GEM_SHOP = 201,
	HEADER_GC_GEM_SHOP_PROCESS = 202,
#endif
	//HEADER_GC_UNUSED = 203,
	//HEADER_GC_UNUSED = 204,
	//HEADER_GC_UNUSED = 205,
	//HEADER_GC_UNUSED = 206,
	//HEADER_GC_UNUSED = 207,

	HEADER_GC_SPECIFIC_EFFECT = 208,

#if defined(ENABLE_DRAGON_SOUL_SYSTEM)
	HEADER_GC_DRAGON_SOUL_REFINE = 209,
#endif
	HEADER_GC_RESPOND_CHANNELSTATUS = 210,

	//HEADER_GC_UNUSED = 211,
	//HEADER_GC_UNUSED = 212,
	//HEADER_GC_UNUSED = 213,
#if defined(ENABLE_ACCE_COSTUME_SYSTEM)
	HEADER_GC_ACCE_REFINE = 214,
#endif
#if defined(ENABLE_AURA_COSTUME_SYSTEM)
	HEADER_GC_AURA_REFINE = 215,
#endif
	//HEADER_GC_UNUSED = 216,
#if defined(ENABLE_LUCKY_BOX)
	HEADER_GC_LUCKY_BOX = 217,
#endif
	//HEADER_GC_UNUSED = 218,
	//HEADER_GC_UNUSED = 219,
	//HEADER_GC_UNUSED = 220,
#if defined(ENABLE_MAILBOX)
	HEADER_GC_MAILBOX_PROCESS = 221,
	HEADER_GC_MAILBOX = 222,
	HEADER_GC_MAILBOX_ADD_DATA = 223,
	HEADER_GC_MAILBOX_ALL = 224,
	HEADER_GC_MAILBOX_UNREAD = 225,
#endif
	//HEADER_GC_UNUSED = 226,
#if defined(ENABLE_CHANGED_ATTR)
	HEADER_GC_ITEM_SELECT_ATTR = 227,
#endif
#if defined(ENABLE_CHANGE_LOOK_SYSTEM)
	HEADER_GC_CHANGE_LOOK_SET = 228,
	HEADER_GC_CHANGE_LOOK_DEL = 229,
	HEADER_GC_CHANGE_LOOK_FREE_SET = 230,
	HEADER_GC_CHANGE_LOOK_FREE_DEL = 231,
#endif
	//HEADER_GC_UNUSED = 232,
	//HEADER_GC_UNUSED = 233,
	//HEADER_GC_UNUSED = 234,
	//HEADER_GC_UNUSED = 235,
	//HEADER_GC_UNUSED = 236,
	//HEADER_GC_UNUSED = 237,
#if defined(ENABLE_MINI_GAME_CATCH_KING)
	HEADER_GC_MINI_GAME_CATCH_KING = 238,
#endif
	//HEADER_GC_UNUSED = 239,
#if defined(ENABLE_RANKING_SYSTEM)
	HEADER_GC_RANKING = 240,
#endif
	//HEADER_GC_UNUSED = 241,
	//HEADER_GC_UNUSED = 242,
	//HEADER_GC_UNUSED = 243,
	//HEADER_GC_UNUSED = 244,
#if defined(ENABLE_LOOTING_SYSTEM)
	HEADER_GC_LOOT_FILTER = 245,
#endif
	//HEADER_GC_UNUSED = 246,
	//HEADER_GC_UNUSED = 247,
	//HEADER_GC_UNUSED = 248,
	//HEADER_GC_UNUSED = 249,
#if defined(__IMPROVED_PACKET_ENCRYPTION__)
	HEADER_GC_KEY_AGREEMENT_COMPLETED = 0xfa, // __IMPROVED_PACKET_ENCRYPTION__ // 250
	HEADER_GC_KEY_AGREEMENT = 0xfb, // __IMPROVED_PACKET_ENCRYPTION__ // 251
#endif
	HEADER_GC_HANDSHAKE_OK = 0xfc, // 252
	HEADER_GC_PHASE = 0xfd, // 253
	HEADER_GC_BINDUDP = 0xfe, // 254
	HEADER_GC_HANDSHAKE = 0xff, // 255
};

enum
{
	ID_MAX_NUM = 30,
	PASS_MAX_NUM = 16,
	CHAT_MAX_NUM = 128,
	PATH_NODE_MAX_NUM = 64,
	SHOP_SIGN_MAX_LEN = 32,

	PLAYER_PER_ACCOUNT3 = 3,
#ifndef ENABLE_PLAYER_PER_ACCOUNT5
	PLAYER_PER_ACCOUNT4 = 4,
#else
	PLAYER_PER_ACCOUNT4 = 5,
	PLAYER_PER_ACCOUNT5 = 5,
#endif

	PLAYER_ITEM_SLOT_MAX_NUM = 20, // 플래이어의 슬롯당 들어가는 갯수.

	QUICKSLOT_MAX_LINE = 4,
	QUICKSLOT_MAX_COUNT_PER_LINE = 8, // 클라이언트 임의 결정값
	QUICKSLOT_MAX_COUNT = QUICKSLOT_MAX_LINE * QUICKSLOT_MAX_COUNT_PER_LINE,

	QUICKSLOT_MAX_NUM = 36, // 서버와 맞춰져 있는 값

#if defined(ENABLE_MYSHOP_EXPANSION)
	SHOP_HOST_ITEM_MAX = 80,
#endif
	SHOP_HOST_ITEM_MAX_NUM = 40,

	METIN_SOCKET_COUNT = 6,

	PARTY_AFFECT_SLOT_MAX_NUM = 7,

	GUILD_GRADE_NAME_MAX_LEN = 8,
	GUILD_NAME_MAX_LEN = 12,
	GUILD_GRADE_COUNT = 15,
	GULID_COMMENT_MAX_LEN = 50,

	MARK_CRC_NUM = 8 * 8,
	MARK_DATA_SIZE = 16 * 12,
	SYMBOL_DATA_SIZE = 128 * 256,
	QUEST_INPUT_STRING_MAX_NUM = 64,
#if defined(ENABLE_OX_RENEWAL)
	QUEST_INPUT_LONG_STRING_MAX_NUM = 128,
#endif

	PRIVATE_CODE_LENGTH = 8,

	REFINE_MATERIAL_MAX_NUM = 5,

	//CHINA_MATRIX_ANSWER_MAX_LEN = 8,

	//RUNUP_MATRIX_QUIZ_MAX_LEN = 8,
	//RUNUP_MATRIX_ANSWER_MAX_LEN = 4,
	//NEWCIBN_PASSPOD_ANSWER_MAX_LEN = 8,
	//NEWCIBN_PASSPOD_FAILURE_MAX_LEN = 128,

	WEAR_MAX_NUM = 32,

	SHOP_TAB_NAME_MAX = 32,
#if defined(ENABLE_SHOPEX_TAB4)
	SHOP_TAB_COUNT_MAX = 4,
#else
	SHOP_TAB_COUNT_MAX = 3,
#endif
};

#pragma pack(push)
#pragma pack(1)

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Mark
typedef struct command_mark_login
{
	BYTE header;
	DWORD handle;
	DWORD random_key;
} TPacketCGMarkLogin;

typedef struct command_mark_upload
{
	BYTE header;
	DWORD gid;
	BYTE image[16 * 12 * 4];
} TPacketCGMarkUpload;

typedef struct command_mark_idxlist
{
	BYTE header;
} TPacketCGMarkIDXList;

typedef struct command_mark_crclist
{
	BYTE header;
	BYTE imgIdx;
	DWORD crclist[80];
} TPacketCGMarkCRCList;

typedef struct packet_mark_idxlist
{
	BYTE header;
	DWORD bufSize;
	WORD count;
	//뒤에 size * (WORD + WORD)만큼 데이터 붙음
} TPacketGCMarkIDXList;

typedef struct packet_mark_block
{
	BYTE header;
	DWORD bufSize;
	BYTE imgIdx;
	DWORD count;
	// 뒤에 64 x 48 x 픽셀크기(4바이트) = 12288만큼 데이터 붙음
} TPacketGCMarkBlock;

typedef struct command_symbol_upload
{
	BYTE header;
	WORD size;
	DWORD handle;
} TPacketCGSymbolUpload;

typedef struct command_symbol_crc
{
	BYTE header;
	DWORD dwGuildID;
	DWORD dwCRC;
	DWORD dwSize;
} TPacketCGSymbolCRC;

typedef struct packet_symbol_data
{
	BYTE header;
	WORD size;
	DWORD guild_id;
} TPacketGCGuildSymbolData;

//
//
//

typedef struct packet_observer_add
{
	BYTE header;
	DWORD vid;
	WORD x;
	WORD y;
} TPacketGCObserverAdd;

typedef struct packet_observer_move
{
	BYTE header;
	DWORD vid;
	WORD x;
	WORD y;
} TPacketGCObserverMove;

typedef struct packet_observer_remove
{
	BYTE header;
	DWORD vid;
} TPacketGCObserverRemove;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// To Server

typedef struct command_checkin
{
	BYTE header;
	char name[ID_MAX_NUM + 1];
	char pwd[PASS_MAX_NUM + 1];
} TPacketCGCheckin;

typedef struct command_login
{
	BYTE header;
	char name[ID_MAX_NUM + 1];
	char pwd[PASS_MAX_NUM + 1];
} TPacketCGLogin;

// start - 권한 서버 접속을 위한 패킷들
typedef struct command_login2
{
	BYTE header;
	char name[ID_MAX_NUM + 1];
	DWORD login_key;
	DWORD adwClientKey[4];
} TPacketCGLogin2;

typedef struct command_login3
{
	BYTE header;
	char name[ID_MAX_NUM + 1];
	char pwd[PASS_MAX_NUM + 1];
	DWORD adwClientKey[4];
#if defined(ENABLE_MULTI_LANGUAGE_SYSTEM)
	char country[COUNTRY_NAME_MAX_LEN + 1];
#endif
} TPacketCGLogin3;
// end - 권한 서버 접속을 위한 패킷들

typedef struct command_direct_enter
{
	BYTE bHeader;
	char login[ID_MAX_NUM + 1];
	char passwd[PASS_MAX_NUM + 1];
	BYTE index;
} TPacketCGDirectEnter;

typedef struct command_player_select
{
	BYTE header;
	BYTE index;
#if defined(ENABLE_MULTI_LANGUAGE_SYSTEM)
	char country[COUNTRY_NAME_MAX_LEN + 1];
#endif
} TPacketCGSelectCharacter;

typedef struct command_attack
{
	BYTE header;
	BYTE bType; // 공격 유형
	DWORD dwVictimVID; // 적 VID
	BYTE bCRCMagicCubeProcPiece;
	BYTE bCRCMagicCubeFilePiece;
} TPacketCGAttack;

typedef struct command_chat
{
	BYTE header;
	WORD length;
	BYTE type;
} TPacketCGChat;

typedef struct command_whisper
{
	BYTE bHeader;
	WORD wSize;
	char szNameTo[CHARACTER_NAME_MAX_LEN + 1];
} TPacketCGWhisper;

enum EBattleMode
{
	BATTLEMODE_ATTACK = 0,
	BATTLEMODE_DEFENSE = 1,
};

typedef struct command_EnterFrontGame
{
	BYTE header;
} TPacketCGEnterFrontGame;

typedef struct command_item_use
{
	BYTE header;
	TItemPos pos;
} TPacketCGItemUse;

typedef struct command_item_use_to_item
{
	BYTE header;
	TItemPos source_pos;
	TItemPos target_pos;
} TPacketCGItemUseToItem;

typedef struct command_item_drop
{
	BYTE header;
	TItemPos pos;
	DWORD elk;
#if defined(ENABLE_CHEQUE_SYSTEM)
	DWORD cheque;
#endif
} TPacketCGItemDrop;

typedef struct command_item_drop2
{
	BYTE header;
	TItemPos pos;
	DWORD gold;
#if defined(ENABLE_CHEQUE_SYSTEM)
	DWORD cheque;
#endif
	WORD count;
} TPacketCGItemDrop2;

#if defined(ENABLE_NEW_DROP_DIALOG)
typedef struct command_item_destroy
{
	BYTE header;
	TItemPos pos;
} TPacketCGItemDestroy;
#endif

typedef struct command_item_move
{
	BYTE header;
	TItemPos pos;
	TItemPos change_pos;
	WORD num;
} TPacketCGItemMove;

typedef struct command_item_pickup
{
	BYTE header;
	DWORD vid;
} TPacketCGItemPickUp;

typedef struct command_quickslot_add
{
	BYTE header;
	BYTE pos;
	TQuickSlot slot;
}TPacketCGQuickSlotAdd;

typedef struct command_quickslot_del
{
	BYTE header;
	BYTE pos;
}TPacketCGQuickSlotDel;

typedef struct command_quickslot_swap
{
	BYTE header;
	BYTE pos;
	BYTE change_pos;
}TPacketCGQuickSlotSwap;

typedef struct command_on_click
{
	BYTE header;
	DWORD vid;
} TPacketCGOnClick;

enum
{
	SHOP_SUBHEADER_CG_END,
	SHOP_SUBHEADER_CG_BUY,
	SHOP_SUBHEADER_CG_SELL,
	SHOP_SUBHEADER_CG_SELL2,
};

typedef struct command_shop
{
	BYTE header;
	WORD subheader;
} TPacketCGShop;

typedef struct command_shop_sell
{
	WORD wPos;
	WORD wCount;
	BYTE bType;
} TPacketCGShopSell;

enum
{
	EXCHANGE_SUBHEADER_CG_START, // arg1 == vid of target character
	EXCHANGE_SUBHEADER_CG_ITEM_ADD, // arg1 == position of item
	EXCHANGE_SUBHEADER_CG_ITEM_DEL, // arg1 == position of item
	EXCHANGE_SUBHEADER_CG_ELK_ADD, // arg1 == amount of elk
#if defined(ENABLE_CHEQUE_SYSTEM)
	EXCHANGE_SUBHEADER_CG_CHEQUE_ADD, // arg1 == amount of cheque
#endif
	EXCHANGE_SUBHEADER_CG_ACCEPT, // arg1 == not used
	EXCHANGE_SUBHEADER_CG_CANCEL, // arg1 == not used
};

typedef struct command_exchange
{
	BYTE header;
	BYTE subheader;
	DWORD arg1;
	BYTE arg2;
	TItemPos Pos;
} TPacketCGExchange;

typedef struct command_position
{
	BYTE header;
	BYTE position;
} TPacketCGPosition;

typedef struct command_script_answer
{
	BYTE header;
	BYTE answer;
} TPacketCGScriptAnswer;

typedef struct command_script_button
{
	BYTE header;
	unsigned int idx;
} TPacketCGScriptButton;

typedef struct command_target
{
	BYTE header;
	DWORD dwVID;
} TPacketCGTarget;

typedef struct command_move
{
	BYTE bHeader;
	BYTE bFunc;
	BYTE bArg;
	BYTE bRot;
	LONG lX;
	LONG lY;
	DWORD dwTime;
} TPacketCGMove;

typedef struct command_sync_position_element
{
	DWORD dwVID;
	long lX;
	long lY;
} TPacketCGSyncPositionElement;

typedef struct command_sync_position
{
	BYTE bHeader;
	WORD wSize;
} TPacketCGSyncPosition;

typedef struct command_fly_targeting
{
	BYTE bHeader;
	DWORD dwTargetVID;
	long lX;
	long lY;
} TPacketCGFlyTargeting;

typedef struct packet_fly_targeting
{
	BYTE bHeader;
	DWORD dwShooterVID;
	DWORD dwTargetVID;
	long lX;
	long lY;
} TPacketGCFlyTargeting;

typedef struct packet_shoot
{
	BYTE bHeader;
	BYTE bType;
} TPacketCGShoot;

typedef struct command_warp
{
	BYTE bHeader;
} TPacketCGWarp;

enum
{
	SAFEBOX_MONEY_STATE_SAVE,
	SAFEBOX_MONEY_STATE_WITHDRAW,
};

typedef struct command_safebox_money
{
	BYTE bHeader;
	BYTE bState;
	DWORD dwMoney;
} TPacketCGSafeboxMoney;

typedef struct command_safebox_checkout
{
	BYTE bHeader;
	BYTE bSafePos;
	TItemPos ItemPos;
} TPacketCGSafeboxCheckout;

typedef struct command_safebox_checkin
{
	BYTE bHeader;
	BYTE bSafePos;
	TItemPos ItemPos;
} TPacketCGSafeboxCheckin;

typedef struct command_mall_checkout
{
	BYTE bHeader;
	BYTE bMallPos;
	TItemPos ItemPos;
} TPacketCGMallCheckout;

///////////////////////////////////////////////////////////////////////////////////
// Messenger

enum EMessengerConnectedState
{
	MESSENGER_CONNECTED_STATE_OFFLINE,
	MESSENGER_CONNECTED_STATE_ONLINE
};

enum EGCMessengerSubHeader
{
	MESSENGER_SUBHEADER_GC_LIST,
	MESSENGER_SUBHEADER_GC_LOGIN,
	MESSENGER_SUBHEADER_GC_LOGOUT,
	MESSENGER_SUBHEADER_GC_INVITE,
#if defined(ENABLE_MESSENGER_BLOCK)
	MESSENGER_SUBHEADER_GC_BLOCK_LIST,
	MESSENGER_SUBHEADER_GC_BLOCK_LOGIN,
	MESSENGER_SUBHEADER_GC_BLOCK_LOGOUT,
#endif
#if defined(ENABLE_MESSENGER_GM)
	MESSENGER_SUBHEADER_GC_GM_LIST,
	MESSENGER_SUBHEADER_GC_GM_LOGIN,
	MESSENGER_SUBHEADER_GC_GM_LOGOUT,
#endif
};

typedef struct packet_messenger
{
	BYTE bHeader;
	WORD wSize;
	BYTE bSubHeader;
} TPacketGCMessenger;

typedef struct packet_messenger_list
{
	BYTE bConnected;
	char szName[CHARACTER_NAME_MAX_LEN + 1];
#if defined(ENABLE_MESSENGER_DETAILS)
	DWORD dwLastPlayTime;
#if defined(ENABLE_MULTI_LANGUAGE_SYSTEM)
	char szCountry[COUNTRY_NAME_MAX_LEN + 1];
#endif
#endif
} TPacketGCMessengerList;

enum ECGMessengerSubHeader
{
	MESSENGER_SUBHEADER_CG_ADD_BY_VID,
	MESSENGER_SUBHEADER_CG_ADD_BY_NAME,
	MESSENGER_SUBHEADER_CG_REMOVE,
#if defined(ENABLE_MESSENGER_BLOCK)
	MESSENGER_SUBHEADER_CG_BLOCK_ADD_BY_VID,
	MESSENGER_SUBHEADER_CG_BLOCK_ADD_BY_NAME,
	MESSENGER_SUBHEADER_CG_BLOCK_REMOVE,
#endif
};

typedef struct command_messenger
{
	BYTE bHeader;
	BYTE bSubHeader;
} TPacketCGMessenger;

///////////////////////////////////////////////////////////////////////////////////
// Party

typedef struct command_use_skill
{
	BYTE bHeader;
	DWORD dwVnum;
	DWORD dwTargetVID;
} TPacketCGUseSkill;

typedef struct command_party_invite
{
	BYTE header;
	DWORD vid;
} TPacketCGPartyInvite;

typedef struct command_party_invite_answer
{
	BYTE header;
	DWORD leader_pid;
	BYTE accept;
} TPacketCGPartyInviteAnswer;

typedef struct command_party_remove
{
	BYTE header;
	DWORD pid;
} TPacketCGPartyRemove;

typedef struct command_party_set_state
{
	BYTE byHeader;
	DWORD dwVID;
	BYTE byState;
	BYTE byFlag;
} TPacketCGPartySetState;

typedef struct packet_party_link
{
	BYTE header;
	DWORD pid;
	DWORD vid;
#if defined(WJ_SHOW_PARTY_ON_MINIMAP)
	long mapIdx;
#endif
#if defined(ENABLE_PARTY_CHANNEL_FIX)
	BYTE channel;
#endif
} TPacketGCPartyLink;

typedef struct command_party_use_skill
{
	BYTE byHeader;
	BYTE bySkillIndex;
	DWORD dwTargetVID;
} TPacketCGPartyUseSkill;

enum
{
	GUILD_SUBHEADER_CG_ADD_MEMBER,
	GUILD_SUBHEADER_CG_REMOVE_MEMBER,
	GUILD_SUBHEADER_CG_CHANGE_GRADE_NAME,
	GUILD_SUBHEADER_CG_CHANGE_GRADE_AUTHORITY,
	GUILD_SUBHEADER_CG_OFFER,
	GUILD_SUBHEADER_CG_POST_COMMENT,
	GUILD_SUBHEADER_CG_DELETE_COMMENT,
	GUILD_SUBHEADER_CG_REFRESH_COMMENT,
	GUILD_SUBHEADER_CG_CHANGE_MEMBER_GRADE,
	GUILD_SUBHEADER_CG_USE_SKILL,
	GUILD_SUBHEADER_CG_CHANGE_MEMBER_GENERAL,
	GUILD_SUBHEADER_CG_GUILD_INVITE_ANSWER,
	GUILD_SUBHEADER_CG_CHARGE_GSP,
	GUILD_SUBHEADER_CG_DEPOSIT_MONEY,
	GUILD_SUBHEADER_CG_WITHDRAW_MONEY,
};

typedef struct command_guild
{
	BYTE byHeader;
	BYTE bySubHeader;
} TPacketCGGuild;

typedef struct command_guild_answer_make_guild
{
	BYTE header;
	char guild_name[GUILD_NAME_MAX_LEN + 1];
} TPacketCGAnswerMakeGuild;

typedef struct command_give_item
{
	BYTE byHeader;
	DWORD dwTargetVID;
	TItemPos ItemPos;
	WORD byItemCount;
} TPacketCGGiveItem;

typedef struct SPacketCGHack
{
	BYTE bHeader;
	char szBuf[255 + 1];
} TPacketCGHack;

typedef struct command_dungeon
{
	BYTE bHeader;
	WORD size;
} TPacketCGDungeon;

// Private Shop
typedef struct SShopItemTable
{
	DWORD vnum;
	DWORD count;

	TItemPos pos; // PC 상점에만 이용
	DWORD price; // PC 상점에만 이용
#if defined(ENABLE_CHEQUE_SYSTEM)
	DWORD cheque;
#endif
	BYTE display_pos; // PC 상점에만 이용, 보일 위치.
#if defined(ENABLE_SHOPEX_RENEWAL)
	BYTE bPriceType;
	DWORD dwPriceVnum;
	long alSockets[ITEM_SOCKET_SLOT_MAX_NUM];
#endif
} TShopItemTable;

typedef struct SPacketCGMyShop
{
	BYTE bHeader;
	char szSign[SHOP_SIGN_MAX_LEN + 1];
	BYTE bCount; // count of TShopItemTable, max 39
} TPacketCGMyShop;

#if defined(ENABLE_MYSHOP_DECO)
typedef struct SPacketGCMyPrivShopOpen
{
	BYTE bHeader;
	bool bCashItem;
	BYTE bTabCount;
} TPacketGCMyPrivShopOpen;

typedef struct STPacketCGMyShopDec
{
	BYTE bHeader;
	BYTE bState;
} TPacketCGMyShopDeco;

typedef struct SPacketCGMyShopDecoAdd
{
	BYTE bHeader;
	BYTE bType;
	DWORD dwPolyVnum;
} TPacketCGMyShopDecoAdd;
#endif

typedef struct SPacketCGRefine
{
	BYTE header;
	BYTE pos;
	BYTE type;
} TPacketCGRefine;

typedef struct SPacketCGChangeName
{
	BYTE header;
	BYTE index;
	char name[CHARACTER_NAME_MAX_LEN + 1];
} TPacketCGChangeName;

typedef struct command_client_version
{
	BYTE header;
	char filename[32 + 1];
	char timestamp[32 + 1];
} TPacketCGClientVersion;

typedef struct command_client_version2
{
	BYTE header;
	char filename[32 + 1];
	char timestamp[32 + 1];
} TPacketCGClientVersion2;

typedef struct command_crc_report
{
	BYTE header;
	BYTE byPackMode;
	DWORD dwBinaryCRC32;
	DWORD dwProcessCRC32;
	DWORD dwRootPackCRC32;
} TPacketCGCRCReport;

enum EPartyExpDistributionType
{
	PARTY_EXP_DISTRIBUTION_NON_PARITY,
	PARTY_EXP_DISTRIBUTION_PARITY,
};

typedef struct command_party_parameter
{
	BYTE bHeader;
	BYTE bDistributeMode;
} TPacketCGPartyParameter;

typedef struct command_quest_input_string
{
	BYTE bHeader;
	char szString[QUEST_INPUT_STRING_MAX_NUM + 1];
} TPacketCGQuestInputString;

#if defined(ENABLE_OX_RENEWAL)
typedef struct command_quest_input_long_string
{
	BYTE bHeader;
	char szString[QUEST_INPUT_LONG_STRING_MAX_NUM + 1];
} TPacketCGQuestInputLongString;
#endif

typedef struct command_quest_confirm
{
	BYTE header;
	BYTE answer;
	DWORD requestPID;
} TPacketCGQuestConfirm;

typedef struct command_script_select_item
{
	BYTE header;
	DWORD selection;
} TPacketCGScriptSelectItem;

#if defined(ENABLE_GEM_SYSTEM)
typedef struct command_select_item_ex
{
	BYTE bHeader;
	DWORD dwItemPos;
	BYTE bType;
	command_select_item_ex(const DWORD c_dwItemPos, const BYTE c_bType = 0) :
		bHeader(HEADER_CG_SELECT_ITEM_EX),
		dwItemPos(c_dwItemPos),
		bType(c_bType)
	{}
} TPacketCGSelectItemEx;
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// From Server
enum EPhase
{
	PHASE_CLOSE, // 끊기는 상태 (또는 끊기 전 상태)
	PHASE_HANDSHAKE, // 악수..;;
	PHASE_LOGIN, // 로그인 중
	PHASE_SELECT, // 캐릭터 선택 화면
	PHASE_LOADING, // 선택 후 로딩 화면
	PHASE_GAME, // 게임 화면
	PHASE_DEAD, // 죽었을 때.. (게임 안에 있는 것일 수도..)

	PHASE_DBCLIENT_CONNECTING, // 서버용
	PHASE_DBCLIENT, // 서버용
	PHASE_P2P, // 서버용
	PHASE_AUTH, // 로그인 인증 용
};

typedef struct packet_phase
{
	BYTE header;
	BYTE phase;
} TPacketGCPhase;

typedef struct packet_blank // 공백패킷.
{
	BYTE header;
} TPacketGCBlank;

typedef struct packet_blank_dynamic
{
	BYTE header;
	WORD size;
} TPacketGCBlankDynamic;

typedef struct packet_header_handshake
{
	BYTE header;
	DWORD dwHandshake;
	DWORD dwTime;
	LONG lDelta;
} TPacketGCHandshake;

typedef struct packet_header_bindudp
{
	BYTE header;
	DWORD addr;
	WORD port;
} TPacketGCBindUDP;

typedef struct packet_header_dynamic_size
{
	BYTE header;
	WORD size;
} TDynamicSizePacketHeader;

typedef struct SSimplePlayerInformation
{
	DWORD dwID;
	char szName[CHARACTER_NAME_MAX_LEN + 1];
	BYTE byJob;
	BYTE byLevel;
	DWORD dwPlayMinutes;
	BYTE byST, byHT, byDX, byIQ;
	//DWORD adwParts[CRaceData::PART_MAX_NUM];
	DWORD dwMainPart;
	BYTE bChangeName;
	DWORD dwHairPart;
#if defined(ENABLE_ACCE_COSTUME_SYSTEM)
	DWORD dwAccePart;
#endif
	BYTE bDummy[4];
	long x, y;
	LONG lAddr;
	WORD wPort;
	BYTE bySkillGroup;
	DWORD dwLastPlay;
#if defined(ENABLE_CONQUEROR_LEVEL)
	BYTE byConquerorLevel;
	BYTE bySungmaStr, bySungmaHp, bySungmaMove, bySungmaImmune;
#endif
} TSimplePlayerInformation;

typedef struct packet_login_success3
{
	BYTE header;
	TSimplePlayerInformation akSimplePlayerInformation[PLAYER_PER_ACCOUNT3];
	DWORD guild_id[PLAYER_PER_ACCOUNT3];
	char guild_name[PLAYER_PER_ACCOUNT3][GUILD_NAME_MAX_LEN + 1];
	DWORD handle;
	DWORD random_key;
} TPacketGCLoginSuccess3;

typedef struct packet_login_success4
{
	BYTE header;
	TSimplePlayerInformation akSimplePlayerInformation[PLAYER_PER_ACCOUNT4];
	DWORD guild_id[PLAYER_PER_ACCOUNT4];
	char guild_name[PLAYER_PER_ACCOUNT4][GUILD_NAME_MAX_LEN + 1];
	DWORD handle;
	DWORD random_key;
} TPacketGCLoginSuccess4;

#if defined(ENABLE_PLAYER_PER_ACCOUNT5)
typedef struct packet_login_success5
{
	BYTE header;
	TSimplePlayerInformation akSimplePlayerInformation[PLAYER_PER_ACCOUNT5];
	DWORD guild_id[PLAYER_PER_ACCOUNT5];
	char guild_name[PLAYER_PER_ACCOUNT5][GUILD_NAME_MAX_LEN + 1];
	DWORD handle;
	DWORD random_key;
} TPacketGCLoginSuccess5;
#endif

enum ELoginFailure
{
	LOGIN_FAILURE_NONE,
	LOGIN_FAILURE_ALREADY,
	LOGIN_FAILURE_NOID,
	LOGIN_FAILURE_WRONGPWD,
	LOGIN_FAILURE_FULL,
	LOGIN_FAILURE_SHUTDOWN,
	LOGIN_FAILURE_REPAIR,
	LOGIN_FAILURE_BLOCK,
	LOGIN_FAILURE_WRONGMAT,
	LOGIN_FAILURE_QUIT,
	LOGIN_FAILURE_BESAMEKEY,
	LOGIN_FAILURE_NOTAVAIL,
	LOGIN_FAILURE_BLKLOGIN,
	LOGIN_FAILURE_WEBBLK,
	LOGIN_FAILURE_BADSCLID,
	LOGIN_FAILURE_AGELIMIT,
	LOGIN_FAILURE_CONFIRM,
	LOGIN_FAILURE_INACTIVE,
	LOGIN_FAILURE_UNMIGRATION,
	LOGIN_FAILURE_MPROCESSING,
	LOGIN_FAILURE_LOCKED,
	LOGIN_FAILURE_BACKENDERR,
	LOGIN_FAILURE_INTEGRTING,
	LOGIN_FAILURE_COUNTRYERR,
	LOGIN_FAILURE_IOVATION,
	LOGIN_FAILURE_TNTERR,
	LOGIN_FAILURE_SERVER_CLOSED,
	LOGIN_FAILURE_SERVER_GRADE,
};

enum { LOGIN_STATUS_MAX_LEN = 8 };
typedef struct packet_login_failure
{
	BYTE header;
	char szStatus[LOGIN_STATUS_MAX_LEN + 1];
} TPacketGCLoginFailure;

typedef struct command_player_create
{
	BYTE header;
	BYTE index;
	char name[CHARACTER_NAME_MAX_LEN + 1];
	WORD job;
	BYTE shape;
	BYTE CON;
	BYTE INT;
	BYTE STR;
	BYTE DEX;
} TPacketCGCreateCharacter;

typedef struct command_player_create_success
{
	BYTE header;
	BYTE bAccountCharacterSlot;
	TSimplePlayerInformation kSimplePlayerInfomation;
} TPacketGCPlayerCreateSuccess;

typedef struct command_create_failure
{
	BYTE header;
	BYTE bType;
} TPacketGCCreateFailure;

typedef struct command_player_delete
{
	BYTE header;
	BYTE index;
	char szPrivateCode[PRIVATE_CODE_LENGTH];
} TPacketCGDestroyCharacter;

#if defined(ENABLE_DELETE_FAILURE_TYPE)
typedef struct SPacketGCDestroyCharacterFailure
{
	BYTE header;
	BYTE type;
	INT time;
} TPacketGCDestroyCharacterFailure;
#endif

typedef struct packet_player_delete_success
{
	BYTE header;
	BYTE account_index;
} TPacketGCDestroyCharacterSuccess;

enum
{
	ADD_CHARACTER_STATE_DEAD = (1 << 0),
	ADD_CHARACTER_STATE_SPAWN = (1 << 1),
	ADD_CHARACTER_STATE_GUNGON = (1 << 2),
	ADD_CHARACTER_STATE_KILLER = (1 << 3),
	ADD_CHARACTER_STATE_PARTY = (1 << 4),
};

enum EPKModes
{
	PK_MODE_PEACE,
	PK_MODE_REVENGE,
	PK_MODE_FREE,
	PK_MODE_PROTECT,
	PK_MODE_GUILD,
	PK_MODE_MAX_NUM,
};

// 2004.11.20.myevan.CRaceData::PART_MAX_NUM 사용안하게 수정 - 서버에서 사용하는것과 일치하지 않음
enum ECharacterEquipmentPart
{
	CHR_EQUIPPART_ARMOR,
	CHR_EQUIPPART_WEAPON,
	CHR_EQUIPPART_HEAD,
	CHR_EQUIPPART_HAIR,
#if defined(ENABLE_ACCE_COSTUME_SYSTEM)
	CHR_EQUIPPART_ACCE,
#endif
#if defined(ENABLE_AURA_COSTUME_SYSTEM)
	CHR_EQUIPPART_AURA,
#endif

	CHR_EQUIPPART_NUM,
};

typedef struct packet_add_char
{
	BYTE header;
	DWORD dwVID;

#ifdef WJ_SHOW_MOB_INFO
	DWORD dwLevel;
	DWORD dwAIFlag;
#endif

	float angle;
	long x;
	long y;
	long z;

	BYTE bType;
	WORD wRaceNum;
#if defined(ENABLE_RACE_SWAP)
	DWORD dwEventRaceNum;
#endif

	BYTE bMovingSpeed;
	BYTE bAttackSpeed;

	BYTE bStateFlag;
	DWORD dwAffectFlag[3];
} TPacketGCCharacterAdd;

typedef struct packet_char_additional_info
{
	BYTE header;
	DWORD dwVID;
	char name[CHARACTER_NAME_MAX_LEN + 1];
	DWORD adwPart[CHR_EQUIPPART_NUM];
	BYTE bEmpire;
	DWORD dwGuildID;
	DWORD dwLevel;
#if defined(ENABLE_CONQUEROR_LEVEL)
	DWORD dwConquerorLevel;
#endif
	short sAlignment; // 선악치
	BYTE bPKMode;
	DWORD dwMountVnum;
#if defined(ENABLE_QUIVER_SYSTEM)
	DWORD dwArrow;
#endif
#if defined(ENABLE_REFINE_ELEMENT_SYSTEM)
	WORD wRefineElementAffectType;
#endif
#if defined(ENABLE_GUILD_LEADER_GRADE_NAME)
	BYTE bGuildLeaderGrade;
#endif
#if defined(ENABLE_MULTI_LANGUAGE_SYSTEM)
	char szCountry[COUNTRY_NAME_MAX_LEN + 1];
#endif
} TPacketGCCharacterAdditionalInfo;

typedef struct packet_update_char
{
	BYTE header;
	DWORD dwVID;

	DWORD adwPart[CHR_EQUIPPART_NUM];
	BYTE bMovingSpeed;
	BYTE bAttackSpeed;

	BYTE bStateFlag;
	DWORD dwAffectFlag[3];

	DWORD dwGuildID;
	short sAlignment;
	DWORD dwLevel;
#if defined(ENABLE_CONQUEROR_LEVEL)
	DWORD dwConquerorLevel;
#endif
	BYTE bPKMode;
	DWORD dwMountVnum;
#if defined(ENABLE_QUIVER_SYSTEM)
	DWORD dwArrow;
#endif
#if defined(ENABLE_REFINE_ELEMENT_SYSTEM)
	WORD wRefineElementAffectType;
#endif
#if defined(ENABLE_GUILD_LEADER_GRADE_NAME)
	BYTE bGuildLeaderGrade;
#endif
#if defined(ENABLE_LEFT_SEAT)
	bool bLeftSeat;
#endif
} TPacketGCCharacterUpdate;

typedef struct packet_del_char
{
	BYTE header;
	DWORD dwVID;
} TPacketGCCharacterDelete;

typedef struct packet_GlobalTime
{
	BYTE header;
	float GlobalTime;
} TPacketGCGlobalTime;

enum EChatType
{
	CHAT_TYPE_TALKING, /* 그냥 채팅 */
	CHAT_TYPE_INFO, /* 정보 (아이템을 집었다, 경험치를 얻었다. 등) */
	CHAT_TYPE_NOTICE, /* 공지사항 */
	CHAT_TYPE_PARTY, /* 파티말 */
	CHAT_TYPE_GUILD, /* 길드말 */
	CHAT_TYPE_COMMAND, /* 명령 */
	CHAT_TYPE_SHOUT, /* 외치기 */
	CHAT_TYPE_WHISPER, // 서버와는 연동되지 않는 Only Client Enum
	CHAT_TYPE_BIG_NOTICE,
	CHAT_TYPE_MONARCH_NOTICE,
#if defined(ENABLE_OX_RENEWAL)
	CHAT_TYPE_BIG_CONTROL_NOTICE,
#endif
#if defined(ENABLE_DICE_SYSTEM)
	CHAT_TYPE_DICE_INFO,
#endif
	CHAT_TYPE_MISSION,
	CHAT_TYPE_SUB_MISSION,
#if defined(ENABLE_CHATTING_WINDOW_RENEWAL)
	CHAT_TYPE_EXP_INFO,
	CHAT_TYPE_ITEM_INFO,
	CHAT_TYPE_MONEY_INFO,
#endif
	CHAT_TYPE_MAX_NUM,
};

typedef struct packet_chatting
{
	BYTE header;
	WORD size;
	BYTE type;
	DWORD dwVID;
	BYTE bEmpire;
#if defined(ENABLE_LOCALE_CLIENT)
	bool bCanFormat;
#endif
#if defined(ENABLE_MULTI_LANGUAGE_SYSTEM)
	char szCountry[COUNTRY_NAME_MAX_LEN + 1];
#endif
} TPacketGCChat;

typedef struct packet_whisper // 가변 패킷
{
	BYTE bHeader;
	WORD wSize;
	BYTE bType;
	char szNameFrom[CHARACTER_NAME_MAX_LEN + 1];
#if defined(ENABLE_LOCALE_CLIENT)
	bool bCanFormat;
#endif
#if defined(ENABLE_MULTI_LANGUAGE_SYSTEM)
	char szCountry[COUNTRY_NAME_MAX_LEN + 1];
#endif
} TPacketGCWhisper;

typedef struct packet_stun
{
	BYTE header;
	DWORD vid;
} TPacketGCStun;

typedef struct packet_dead
{
	BYTE header;
	DWORD vid;
	BYTE dialog_type;
	long map_index;
} TPacketGCDead;

typedef struct packet_main_character
{
	BYTE header;
	DWORD dwVID;
	WORD wRaceNum;
	char szName[CHARACTER_NAME_MAX_LEN + 1];
	long lX, lY, lZ;
	BYTE bySkillGroup;
} TPacketGCMainCharacter;

// SUPPORT_BGM
typedef struct packet_main_character2_empire
{
	BYTE header;
	DWORD dwVID;
	WORD wRaceNum;
	char szName[CHARACTER_NAME_MAX_LEN + 1];
	long lX, lY, lZ;
	BYTE byEmpire;
	BYTE bySkillGroup;
} TPacketGCMainCharacter2_EMPIRE;

typedef struct packet_main_character3_bgm
{
	enum
	{
		MUSIC_NAME_MAX_LEN = 24,
	};
	BYTE header;
	DWORD dwVID;
	WORD wRaceNum;
	char szUserName[CHARACTER_NAME_MAX_LEN + 1];
	char szBGMName[MUSIC_NAME_MAX_LEN + 1];
	long lX, lY, lZ;
	BYTE byEmpire;
	BYTE bySkillGroup;
} TPacketGCMainCharacter3_BGM;

typedef struct packet_main_character4_bgm_vol
{
	enum
	{
		MUSIC_NAME_MAX_LEN = 24,
	};
	BYTE header;
	DWORD dwVID;
	WORD wRaceNum;
	char szUserName[CHARACTER_NAME_MAX_LEN + 1];
	char szBGMName[MUSIC_NAME_MAX_LEN + 1];
	float fBGMVol;
	long lX, lY, lZ;
	BYTE byEmpire;
	BYTE bySkillGroup;
} TPacketGCMainCharacter4_BGM_VOL;
// END_OF_SUPPORT_BGM

enum EPointTypes
{
	POINT_NONE,						// 0
	POINT_LEVEL,					// 1
	POINT_VOICE,					// 2
	POINT_EXP,						// 3
	POINT_NEXT_EXP,					// 4
	POINT_HP,						// 5
	POINT_MAX_HP,					// 6
	POINT_SP,						// 7
	POINT_MAX_SP,					// 8
	POINT_STAMINA,					// 9 스테미너
	POINT_MAX_STAMINA,				// 10 최대 스테미너

	POINT_GOLD,						// 11
	POINT_ST,						// 12 근력
	POINT_HT,						// 13 체력
	POINT_DX,						// 14 민첩성
	POINT_IQ,						// 15 정신력
	POINT_ATT_POWER,				// 16 공격력
	POINT_ATT_SPEED,				// 17 공격속도
	POINT_EVADE_RATE,				// 18 회피율
	POINT_MOV_SPEED,				// 19 이동속도
	POINT_DEF_GRADE,				// 20 방어등급
	POINT_CASTING_SPEED,			// 21 주문속도 (쿨다운타임*100) / (100 + 이값) = 최종 쿨다운 타임
	POINT_MAGIC_ATT_GRADE,			// 22 마법공격력
	POINT_MAGIC_DEF_GRADE,			// 23 마법방어력
	POINT_EMPIRE_POINT,				// 24 제국점수
	POINT_LEVEL_STEP,				// 25 한 레벨에서의 단계.. (1 2 3 될 때 보상, 4 되면 레벨 업)
	POINT_STAT,						// 26 능력치 올릴 수 있는 개수
	POINT_SUB_SKILL,				// 27 보조 스킬 포인트
	POINT_SKILL,					// 28 액티브 스킬 포인트
	//POINT_SKILL_PASV,				// 27 패시브 기술 올릴 수 있는 개수
	//POINT_SKILL_ACTIVE,			// 28 액티브 스킬 포인트
	POINT_MIN_ATK,					// 29 최소 파괴력
	POINT_MAX_ATK,					// 30 최대 파괴력
	POINT_PLAYTIME,					// 31 플레이시간
	POINT_HP_REGEN,					// 32 HP 회복률
	POINT_SP_REGEN,					// 33 SP 회복률

	POINT_BOW_DISTANCE,				// 34 활 사정거리 증가치 (meter)

	POINT_HP_RECOVERY,				// 35 체력 회복 증가량
	POINT_SP_RECOVERY,				// 36 정신력 회복 증가량

	POINT_POISON_PCT,				// 37 독 확률
	POINT_STUN_PCT,					// 38 기절 확률
	POINT_SLOW_PCT,					// 39 슬로우 확률
	POINT_CRITICAL_PCT,				// 40 크리티컬 확률
	POINT_PENETRATE_PCT,			// 41 관통타격 확률
	POINT_CURSE_PCT,				// 42 저주 확률

	POINT_ATTBONUS_HUMAN,			// 43 인간에게 강함
	POINT_ATTBONUS_ANIMAL,			// 44 동물에게 데미지 % 증가
	POINT_ATTBONUS_ORC,				// 45 웅귀에게 데미지 % 증가
	POINT_ATTBONUS_MILGYO,			// 46 밀교에게 데미지 % 증가
	POINT_ATTBONUS_UNDEAD,			// 47 시체에게 데미지 % 증가
	POINT_ATTBONUS_DEVIL,			// 48 마귀(악마)에게 데미지 % 증가
	POINT_ATTBONUS_INSECT,			// 49 벌레족
	POINT_ATTBONUS_FIRE,			// 50 화염족
	POINT_ATTBONUS_ICE,				// 51 빙설족
	POINT_ATTBONUS_DESERT,			// 52 사막족
	POINT_ATTBONUS_MONSTER,			// 53 모든 몬스터에게 강함
	POINT_ATTBONUS_WARRIOR,			// 54 무사에게 강함
	POINT_ATTBONUS_ASSASSIN,		// 55 자객에게 강함
	POINT_ATTBONUS_SURA,			// 56 수라에게 강함
	POINT_ATTBONUS_SHAMAN,			// 57 무당에게 강함
	POINT_ATTBONUS_TREE,			// 58 나무에게 강함 20050729.myevan UNUSED5
	POINT_RESIST_WARRIOR,			// 59 무사에게 저항
	POINT_RESIST_ASSASSIN,			// 60 자객에게 저항
	POINT_RESIST_SURA,				// 61 수라에게 저항
	POINT_RESIST_SHAMAN,			// 62 무당에게 저항

	POINT_STEAL_HP,					// 63 생명력 흡수
	POINT_STEAL_SP,					// 64 정신력 흡수

	POINT_MANA_BURN_PCT,			// 65 마나 번

	/// 피해시 보너스 ///

	POINT_DAMAGE_SP_RECOVER,		// 66 공격당할 시 정신력 회복 확률

	POINT_BLOCK,					// 67 블럭율
	POINT_DODGE,					// 68 회피율

	POINT_RESIST_SWORD,				// 69
	POINT_RESIST_TWOHAND,			// 70
	POINT_RESIST_DAGGER,			// 71
	POINT_RESIST_BELL,				// 72
	POINT_RESIST_FAN,				// 73
	POINT_RESIST_BOW,				// 74 화살 저항 : 대미지 감소
	POINT_RESIST_FIRE,				// 75 화염 저항 : 화염공격에 대한 대미지 감소
	POINT_RESIST_ELEC,				// 76 전기 저항 : 전기공격에 대한 대미지 감소
	POINT_RESIST_MAGIC,				// 77 술법 저항 : 모든술법에 대한 대미지 감소
	POINT_RESIST_WIND,				// 78 바람 저항 : 바람공격에 대한 대미지 감소

	POINT_REFLECT_MELEE,			// 79 공격 반사

	/// 특수 피해시 ///
	POINT_REFLECT_CURSE,			// 80 저주 반사
	POINT_POISON_REDUCE,			// 81 독데미지 감소

	/// 적 소멸시 ///
	POINT_KILL_SP_RECOVER,			// 82 적 소멸시 MP 회복
	POINT_EXP_DOUBLE_BONUS,			// 83
	POINT_GOLD_DOUBLE_BONUS,		// 84
	POINT_ITEM_DROP_BONUS,			// 85

	/// 회복 관련 ///
	POINT_POTION_BONUS,				// 86
	POINT_KILL_HP_RECOVERY,			// 87

	POINT_IMMUNE_STUN,				// 88
	POINT_IMMUNE_SLOW,				// 89
	POINT_IMMUNE_FALL,				// 90
	//////////////////

	POINT_PARTY_ATT_GRADE,			// 91
	POINT_PARTY_DEF_GRADE,			// 92

	POINT_ATT_BONUS,				// 93
	POINT_DEF_BONUS,				// 94

	POINT_ATT_GRADE_BONUS,			// 95
	POINT_DEF_GRADE_BONUS,			// 96
	POINT_MAGIC_ATT_GRADE_BONUS,	// 97
	POINT_MAGIC_DEF_GRADE_BONUS,	// 98

	POINT_RESIST_NORMAL_DAMAGE,		// 99

#if defined(ENABLE_MOUNT_COSTUME_SYSTEM)
	POINT_MOUNT = 109,				// 109
#endif

	POINT_STAT_RESET_COUNT = 112,	// 112
	POINT_HORSE_SKILL = 113,		// 113

	POINT_MALL_ATTBONUS,			// 114 공격력 +x%
	POINT_MALL_DEFBONUS,			// 115 방어력 +x%
	POINT_MALL_EXPBONUS,			// 116 경험치 +x%
	POINT_MALL_ITEMBONUS,			// 117 아이템 드롭율 x/10배
	POINT_MALL_GOLDBONUS,			// 118 돈 드롭율 x/10배
	POINT_MAX_HP_PCT,				// 119 최대생명력 +x%
	POINT_MAX_SP_PCT,				// 120 최대정신력 +x%

	POINT_SKILL_DAMAGE_BONUS,		// 121 스킬 데미지 *(100+x)%
	POINT_NORMAL_HIT_DAMAGE_BONUS,	// 122 평타 데미지 *(100+x)%

	POINT_SKILL_DEFEND_BONUS,		// 123 스킬 방어 데미지
	POINT_NORMAL_HIT_DEFEND_BONUS,	// 124 평타 방어 데미지
	POINT_PC_BANG_EXP_BONUS,		// 125
	POINT_PC_BANG_DROP_BONUS,		// 126 PC방 전용 드롭률 보너스

	POINT_ENERGY = 128,				// 128 기력

	// 기력 ui 용.
	// 이렇게 하고 싶지 않았지만, 
	// uiTaskBar에서는 affect에 접근할 수 없고,
	// 더구나 클라리언트에서는 blend_affect는 관리하지 않아,
	// 임시로 이렇게 둔다.
	POINT_ENERGY_END_TIME = 129,	// 129 기력 종료 시간

	POINT_COSTUME_ATTR_BONUS = 130,			// 130
	POINT_MAGIC_ATT_BONUS_PER = 131,		// 131
	POINT_MELEE_MAGIC_ATT_BONUS_PER = 132,	// 132

	POINT_RESIST_ICE = 133,				// 133 냉기 저항 : 얼음공격에 대한 대미지 감소
	POINT_RESIST_EARTH = 134,			// 134 대지 저항 : 얼음공격에 대한 대미지 감소
	POINT_RESIST_DARK = 135,			// 135 어둠 저항 : 얼음공격에 대한 대미지 감소

	POINT_RESIST_CRITICAL = 136,		// 136 크리티컬 저항 : 상대의 크리티컬 확률을 감소
	POINT_RESIST_PENETRATE = 137,		// 137 관통타격 저항 : 상대의 관통타격 확률을 감소

	POINT_BLEEDING_REDUCE = 138,		// 138
	POINT_BLEEDING_PCT = 139,			// 139
	POINT_ATTBONUS_WOLFMAN = 140,		// 140
	POINT_RESIST_WOLFMAN = 141,			// 141
	POINT_RESIST_CLAW = 142,			// 142

#if defined(ENABLE_ACCE_COSTUME_SYSTEM)
	POINT_ACCEDRAIN_RATE,
#endif
#if defined(ENABLE_MAGIC_REDUCTION)
	POINT_RESIST_MAGIC_REDUCTION,
#endif
#if defined(ENABLE_CHEQUE_SYSTEM)
	POINT_CHEQUE,
#endif
	POINT_BATTLE_POINT,
	POINT_RESIST_HUMAN,
	POINT_ENCHANT_ELECT,
	POINT_ENCHANT_FIRE,
	POINT_ENCHANT_ICE,
	POINT_ENCHANT_WIND,
	POINT_ENCHANT_EARTH,
	POINT_ENCHANT_DARK,
	POINT_ATTBONUS_CZ,
	POINT_BEAD,
#if defined(ENABLE_GEM_SYSTEM)
	POINT_GEM,
#endif
	POINT_ATTBONUS_SWORD,
	POINT_ATTBONUS_TWOHAND,
	POINT_ATTBONUS_DAGGER,
	POINT_ATTBONUS_BELL,
	POINT_ATTBONUS_FAN,
	POINT_ATTBONUS_BOW,
	POINT_ATTBONUS_CLAW,
	POINT_RESIST_MOUNT_FALL,
	POINT_RESIST_FIST,
	POINT_PREMIUM_EXPBONUS,
	POINT_PRIVILEGE_EXPBONUS,
	POINT_MARRIAGE_EXPBONUS,
	POINT_DEVILTOWER_EXPBONUS,
	POINT_PREMIUM_ITEMBONUS,
	POINT_PRIVILEGE_ITEMBONUS,
	POINT_PREMIUM_GOLDBONUS,
	POINT_PRIVILEGE_GOLDBONUS,
	POINT_SKILL_DAMAGE_SAMYEON,
	POINT_SKILL_DAMAGE_TANHWAN,
	POINT_SKILL_DAMAGE_PALBANG,
	POINT_SKILL_DAMAGE_GIGONGCHAM,
	POINT_SKILL_DAMAGE_GYOKSAN,
	POINT_SKILL_DAMAGE_GEOMPUNG,
	POINT_SKILL_DAMAGE_AMSEOP,
	POINT_SKILL_DAMAGE_GUNGSIN,
	POINT_SKILL_DAMAGE_CHARYUN,
	POINT_SKILL_DAMAGE_SANGONG,
	POINT_SKILL_DAMAGE_YEONSA,
	POINT_SKILL_DAMAGE_KWANKYEOK,
	POINT_SKILL_DAMAGE_GIGUNG,
	POINT_SKILL_DAMAGE_HWAJO,
	POINT_SKILL_DAMAGE_SWAERYUNG,
	POINT_SKILL_DAMAGE_YONGKWON,
	POINT_SKILL_DAMAGE_PABEOB,
	POINT_SKILL_DAMAGE_MARYUNG,
	POINT_SKILL_DAMAGE_HWAYEOMPOK,
	POINT_SKILL_DAMAGE_MAHWAN,
	POINT_SKILL_DAMAGE_BIPABU,
	POINT_SKILL_DAMAGE_YONGBI,
	POINT_SKILL_DAMAGE_PAERYONG,
	POINT_SKILL_DAMAGE_NOEJEON,
	POINT_SKILL_DAMAGE_BYEURAK,
	POINT_SKILL_DAMAGE_CHAIN,
	POINT_SKILL_DAMAGE_CHAYEOL,
	POINT_SKILL_DAMAGE_SALPOONG,
	POINT_SKILL_DAMAGE_GONGDAB,
	POINT_SKILL_DAMAGE_PASWAE,
	POINT_NORMAL_HIT_DEFEND_BONUS_BOSS_OR_MORE,
	POINT_SKILL_DEFEND_BONUS_BOSS_OR_MORE,
	POINT_NORMAL_HIT_DAMAGE_BONUS_BOSS_OR_MORE,
	POINT_SKILL_DAMAGE_BONUS_BOSS_OR_MORE,
	POINT_HIT_BUFF_ENCHANT_FIRE,
	POINT_HIT_BUFF_ENCHANT_ICE,
	POINT_HIT_BUFF_ENCHANT_ELEC,
	POINT_HIT_BUFF_ENCHANT_WIND,
	POINT_HIT_BUFF_ENCHANT_DARK,
	POINT_HIT_BUFF_ENCHANT_EARTH,
	POINT_HIT_BUFF_RESIST_FIRE,
	POINT_HIT_BUFF_RESIST_ICE,
	POINT_HIT_BUFF_RESIST_ELEC,
	POINT_HIT_BUFF_RESIST_WIND,
	POINT_HIT_BUFF_RESIST_DARK,
	POINT_HIT_BUFF_RESIST_EARTH,
	POINT_USE_SKILL_CHEONGRANG_MOV_SPEED,
	POINT_USE_SKILL_CHEONGRANG_CASTING_SPEED,
	POINT_USE_SKILL_CHAYEOL_CRITICAL_PCT,
	POINT_USE_SKILL_SANGONG_ATT_GRADE_BONUS,
	POINT_USE_SKILL_GIGUNG_ATT_GRADE_BONUS,
	POINT_USE_SKILL_JEOKRANG_DEF_BONUS,
	POINT_USE_SKILL_GWIGEOM_DEF_BONUS,
	POINT_USE_SKILL_TERROR_ATT_GRADE_BONUS,
	POINT_USE_SKILL_MUYEONG_ATT_GRADE_BONUS,
	POINT_USE_SKILL_MANASHILED_CASTING_SPEED,
	POINT_USE_SKILL_HOSIN_DEF_BONUS,
	POINT_USE_SKILL_GICHEON_ATT_GRADE_BONUS,
	POINT_USE_SKILL_JEONGEOP_ATT_GRADE_BONUS,
	POINT_USE_SKILL_JEUNGRYEOK_DEF_BONUS,
	POINT_USE_SKILL_GIHYEOL_ATT_GRADE_BONUS,
	POINT_USE_SKILL_CHUNKEON_CASTING_SPEED,
	POINT_USE_SKILL_NOEGEOM_ATT_GRADE_BONUS,
	POINT_SKILL_DURATION_INCREASE_EUNHYUNG,
	POINT_SKILL_DURATION_INCREASE_GYEONGGONG,
	POINT_SKILL_DURATION_INCREASE_GEOMKYUNG,
	POINT_SKILL_DURATION_INCREASE_JEOKRANG,
	POINT_USE_SKILL_PALBANG_HP_ABSORB,
	POINT_USE_SKILL_AMSEOP_HP_ABSORB,
	POINT_USE_SKILL_YEONSA_HP_ABSORB,
	POINT_USE_SKILL_YONGBI_HP_ABSORB,
	POINT_USE_SKILL_CHAIN_HP_ABSORB,
	POINT_USE_SKILL_PASWAE_SP_ABSORB,
	POINT_USE_SKILL_GIGONGCHAM_STUN,
	POINT_USE_SKILL_CHARYUN_STUN,
	POINT_USE_SKILL_PABEOB_STUN,
	POINT_USE_SKILL_MAHWAN_STUN,
	POINT_USE_SKILL_GONGDAB_STUN,
	POINT_USE_SKILL_SAMYEON_STUN,
	POINT_USE_SKILL_GYOKSAN_KNOCKBACK,
	POINT_USE_SKILL_SEOMJEON_KNOCKBACK,
	POINT_USE_SKILL_SWAERYUNG_KNOCKBACK,
	POINT_USE_SKILL_HWAYEOMPOK_KNOCKBACK,
	POINT_USE_SKILL_GONGDAB_KNOCKBACK,
	POINT_USE_SKILL_KWANKYEOK_KNOCKBACK,
	POINT_USE_SKILL_SAMYEON_NEXT_COOLTIME_DECREASE_10PER,
	POINT_USE_SKILL_GEOMPUNG_NEXT_COOLTIME_DECREASE_10PER,
	POINT_USE_SKILL_GUNGSIN_NEXT_COOLTIME_DECREASE_10PER,
	POINT_USE_SKILL_KWANKYEOK_NEXT_COOLTIME_DECREASE_10PER,
	POINT_USE_SKILL_YONGKWON_NEXT_COOLTIME_DECREASE_10PER,
	POINT_USE_SKILL_MARYUNG_NEXT_COOLTIME_DECREASE_10PER,
	POINT_USE_SKILL_BIPABU_NEXT_COOLTIME_DECREASE_10PER,
	POINT_USE_SKILL_NOEJEON_NEXT_COOLTIME_DECREASE_10PER,
	POINT_USE_SKILL_SALPOONG_NEXT_COOLTIME_DECREASE_10PER,
	POINT_USE_SKILL_PASWAE_NEXT_COOLTIME_DECREASE_10PER,
	POINT_ATTBONUS_STONE,
	POINT_DAMAGE_HP_RECOVERY,
	POINT_DAMAGE_SP_RECOVERY,
	POINT_ALIGNMENT_DAMAGE_BONUS,
	POINT_NORMAL_DAMAGE_GUARD,
	POINT_MORE_THEN_HP90_DAMAGE_REDUCE,
	POINT_USE_SKILL_TUSOK_HP_ABSORB,
	POINT_USE_SKILL_PAERYONG_HP_ABSORB,
	POINT_USE_SKILL_BYEURAK_HP_ABSORB,
	POINT_USE_SKILL_SAMYEON_NEXT_COOLTIME_DECREASE_20PER,
	POINT_USE_SKILL_GEOMPUNG_NEXT_COOLTIME_DECREASE_20PER,
	POINT_USE_SKILL_GUNGSIN_NEXT_COOLTIME_DECREASE_20PER,
	POINT_USE_SKILL_KWANKYEOK_NEXT_COOLTIME_DECREASE_20PER,
	POINT_USE_SKILL_YONGKWON_NEXT_COOLTIME_DECREASE_20PER,
	POINT_USE_SKILL_MARYUNG_NEXT_COOLTIME_DECREASE_20PER,
	POINT_USE_SKILL_BIPABU_NEXT_COOLTIME_DECREASE_20PER,
	POINT_USE_SKILL_NOEJEON_NEXT_COOLTIME_DECREASE_20PER,
	POINT_USE_SKILL_SALPOONG_NEXT_COOLTIME_DECREASE_20PER,
	POINT_USE_SKILL_PASWAE_NEXT_COOLTIME_DECREASE_20PER,
	POINT_USE_SKILL_CHAYEOL_HP_ABSORB,
	POINT_MEDAL_OF_HONOR,
	POINT_ALL_STAT_BONUS,
	POINT_SUNGMA_STR,
	POINT_SUNGMA_HP,
	POINT_SUNGMA_MOVE,
	POINT_SUNGMA_IMMUNE,
	POINT_CONQUEROR_LEVEL,
	POINT_CONQUEROR_LEVEL_STEP,
	POINT_CONQUEROR_EXP,
	POINT_CONQUEROR_NEXT_EXP,
	POINT_CONQUEROR_POINT,
	POINT_HIT_PCT,
	POINT_ATTBONUS_PER_HUMAN,
	POINT_ATTBONUS_PER_ANIMAL,
	POINT_ATTBONUS_PER_ORC,
	POINT_ATTBONUS_PER_MILGYO,
	POINT_ATTBONUS_PER_UNDEAD,
	POINT_ATTBONUS_PER_DEVIL,
	POINT_ENCHANT_PER_ELECT,
	POINT_ENCHANT_PER_FIRE,
	POINT_ENCHANT_PER_ICE,
	POINT_ENCHANT_PER_WIND,
	POINT_ENCHANT_PER_EARTH,
	POINT_ENCHANT_PER_DARK,
	POINT_ATTBONUS_PER_CZ,
	POINT_ATTBONUS_PER_INSECT,
	POINT_ATTBONUS_PER_DESERT,
	POINT_ATTBONUS_PER_STONE,
	POINT_ATTBONUS_PER_MONSTER,
	POINT_RESIST_PER_HUMAN,
	POINT_RESIST_PER_ICE,
	POINT_RESIST_PER_DARK,
	POINT_RESIST_PER_EARTH,
	POINT_RESIST_PER_FIRE,
	POINT_RESIST_PER_ELEC,
	POINT_RESIST_PER_MAGIC,
	POINT_RESIST_PER_WIND,
	POINT_HIT_BUFF_SUNGMA_STR,
	POINT_HIT_BUFF_SUNGMA_MOVE,
	POINT_HIT_BUFF_SUNGMA_HP,
	POINT_HIT_BUFF_SUNGMA_IMMUNE,
	POINT_MOUNT_MELEE_MAGIC_ATTBONUS_PER,
	POINT_DISMOUNT_MOVE_SPEED_BONUS_PER,
	POINT_HIT_AUTO_HP_RECOVERY,
	POINT_HIT_AUTO_SP_RECOVERY,
	POINT_USE_SKILL_COOLTIME_DECREASE_ALL,
	POINT_HIT_STONE_ATTBONUS_STONE,
	POINT_HIT_STONE_DEF_GRADE_BONUS,
	POINT_KILL_BOSS_ITEM_BONUS,
	POINT_MOB_HIT_MOB_AGGRESSIVE,
	POINT_NO_DEATH_AND_HP_RECOVERY30,
	POINT_AUTO_PICKUP,
	POINT_MOUNT_NO_KNOCKBACK,
	POINT_SUNGMA_PER_STR,
	POINT_SUNGMA_PER_HP,
	POINT_SUNGMA_PER_MOVE,
	POINT_SUNGMA_PER_IMMUNE,
	POINT_IMMUNE_POISON100,
	POINT_IMMUNE_BLEEDING100,
	POINT_MONSTER_DEFEND_BONUS,

	// 클라이언트 포인트
	POINT_MIN_WEP,
	POINT_MAX_WEP,
	POINT_MIN_MAGIC_WEP,
	POINT_MAX_MAGIC_WEP,
	POINT_HIT_RATE
};

typedef struct SPacketGCPoints
{
	BYTE bHeader;
	POINT_VALUE lPoints[POINT_MAX_NUM];
} TPacketGCPoints;

typedef struct SPacketGCPointChange
{
	BYTE bHeader;
	DWORD dwVID;
	POINT_TYPE wType;
	POINT_VALUE lAmount;
	POINT_VALUE lValue;
} TPacketGCPointChange;

typedef struct packet_motion
{
	BYTE header;
	DWORD vid;
	DWORD victim_vid;
	WORD motion;
} TPacketGCMotion;

typedef struct packet_set_empty_item
{
	BYTE bHeader;
	TItemPos Cell;
	DWORD dwVnum;
	DWORD dwCount;
	long alSockets[ITEM_SOCKET_SLOT_MAX_NUM];
	TPlayerItemAttribute aAttr[ITEM_ATTRIBUTE_SLOT_MAX_NUM];
#if defined(ENABLE_CHANGE_LOOK_SYSTEM)
	DWORD dwTransmutationVnum;
#endif
#if defined(ENABLE_REFINE_ELEMENT_SYSTEM)
	TPlayerItemRefineElement RefineElement;
#endif
#if defined(ENABLE_APPLY_RANDOM)
	TPlayerItemAttribute aApplyRandom[ITEM_APPLY_RANDOM_SLOT_MAX_NUM];
#endif
#if defined(ENABLE_SET_ITEM)
	BYTE bSetValue;
#endif
} TPacketGCItemSetEmpty;

typedef struct packet_set_item
{
	BYTE bHeader;
	TItemPos Cell;
	DWORD dwVnum;
	DWORD dwCount;
	DWORD dwFlags; // 플래그 추가
	DWORD dwAntiFlags; // 플래그 추가
	bool bHighLight;
#if defined(ENABLE_SOULBIND_SYSTEM)
	long lSealDate;
#endif
	long alSockets[ITEM_SOCKET_SLOT_MAX_NUM];
	TPlayerItemAttribute aAttr[ITEM_ATTRIBUTE_SLOT_MAX_NUM];
#if defined(ENABLE_CHANGE_LOOK_SYSTEM)
	DWORD dwTransmutationVnum;
#endif
#if defined(ENABLE_REFINE_ELEMENT_SYSTEM)
	TPlayerItemRefineElement RefineElement;
#endif
#if defined(ENABLE_APPLY_RANDOM)
	TPlayerItemAttribute aApplyRandom[ITEM_APPLY_RANDOM_SLOT_MAX_NUM];
#endif
#if defined(ENABLE_SET_ITEM)
	BYTE bSetValue;
#endif
} TPacketGCItemSet;

typedef struct packet_del_item
{
	BYTE header;
	BYTE pos;
} TPacketGCItemDel;

typedef struct packet_use_item
{
	BYTE header;
	TItemPos Cell;
	DWORD ch_vid;
	DWORD victim_vid;

	DWORD vnum;
} TPacketGCItemUse;

typedef struct packet_update_item
{
	BYTE bHeader;
	TItemPos Cell;
	DWORD dwCount;
#if defined(ENABLE_SOULBIND_SYSTEM)
	long lSealDate;
#endif
	long alSockets[ITEM_SOCKET_SLOT_MAX_NUM];
	TPlayerItemAttribute aAttr[ITEM_ATTRIBUTE_SLOT_MAX_NUM];
#if defined(ENABLE_CHANGE_LOOK_SYSTEM)
	DWORD dwTransmutationVnum;
#endif
#if defined(ENABLE_REFINE_ELEMENT_SYSTEM)
	TPlayerItemRefineElement RefineElement;
#endif
#if defined(ENABLE_APPLY_RANDOM)
	TPlayerItemAttribute aApplyRandom[ITEM_APPLY_RANDOM_SLOT_MAX_NUM];
#endif
#if defined(ENABLE_SET_ITEM)
	BYTE bSetValue;
#endif
} TPacketGCItemUpdate;

typedef struct packet_ground_add_item
{
#if defined(ENABLE_ITEM_DROP_RENEWAL)
	packet_ground_add_item()
	{
		memset(&alSockets, 0, sizeof(alSockets));
		memset(&aAttrs, 0, sizeof(aAttrs));
	}
#endif

	BYTE bHeader;
	long lX, lY, lZ;
	DWORD dwVID;
	DWORD dwVnum;
#if defined(ENABLE_SET_ITEM)
	BYTE bSetValue;
#endif

#if defined(ENABLE_ITEM_DROP_RENEWAL)
	long alSockets[ITEM_SOCKET_SLOT_MAX_NUM];
	TPlayerItemAttribute aAttrs[ITEM_ATTRIBUTE_SLOT_MAX_NUM];
#endif
} TPacketGCItemGroundAdd;

typedef struct packet_ground_del_item
{
	BYTE header;
	DWORD vid;
} TPacketGCItemGroundDel;

typedef struct packet_item_ownership
{
	BYTE bHeader;
	DWORD dwVID;
	char szName[CHARACTER_NAME_MAX_LEN + 1];
} TPacketGCItemOwnership;

typedef struct packet_quickslot_add
{
	BYTE header;
	BYTE pos;
	TQuickSlot slot;
} TPacketGCQuickSlotAdd;

typedef struct packet_quickslot_del
{
	BYTE header;
	BYTE pos; // quick slot pos
} TPacketGCQuickSlotDel;

typedef struct packet_quickslot_swap
{
	BYTE header;
	BYTE pos;
	BYTE change_pos;
} TPacketGCQuickSlotSwap;

typedef struct packet_shop_start
{
#if defined(ENABLE_MYSHOP_DECO)
	BYTE shop_tab_count;
#endif
#if defined(ENABLE_MYSHOP_EXPANSION)
	struct packet_shop_item items[SHOP_HOST_ITEM_MAX];
#else
	struct packet_shop_item items[SHOP_HOST_ITEM_MAX_NUM];
#endif
} TPacketGCShopStart;

typedef struct packet_shop_start_ex // 다음에 TSubPacketShopTab* shop_tabs 이 따라옴.
{
	typedef struct sub_packet_shop_tab
	{
		char name[SHOP_TAB_NAME_MAX];
		BYTE coin_type;
		packet_shop_item items[SHOP_HOST_ITEM_MAX_NUM];
	} TSubPacketShopTab;
	DWORD owner_vid;
	BYTE shop_tab_count;
} TPacketGCShopStartEx;

typedef struct packet_shop_update_item
{
	BYTE pos;
	struct packet_shop_item item;
} TPacketGCShopUpdateItem;

typedef struct packet_shop_update_price
{
	int iElkAmount;
#if defined(ENABLE_CHEQUE_SYSTEM)
	int iChequeAmount;
#endif
} TPacketGCShopUpdatePrice;

enum EPacketShopSubHeaders
{
	SHOP_SUBHEADER_GC_NONE,
	SHOP_SUBHEADER_GC_START,
	SHOP_SUBHEADER_GC_END,
	SHOP_SUBHEADER_GC_UPDATE_ITEM,
	SHOP_SUBHEADER_GC_UPDATE_PRICE,
	SHOP_SUBHEADER_GC_OK,
	SHOP_SUBHEADER_GC_NOT_ENOUGH_MONEY,
	SHOP_SUBHEADER_GC_SOLDOUT,
	SHOP_SUBHEADER_GC_INVENTORY_FULL,
	SHOP_SUBHEADER_GC_INVALID_POS,
	SHOP_SUBHEADER_GC_SOLD_OUT,
	SHOP_SUBHEADER_GC_START_EX,
	SHOP_SUBHEADER_GC_NOT_ENOUGH_MONEY_EX,
	SHOP_SUBHEADER_GC_NOT_ENOUGH_BP,
	SHOP_SUBHEADER_GC_EXCEED_LIMIT_TODAY,
#if defined(ENABLE_MYSHOP_DECO)
	SHOP_SUBHEADER_GC_MYPRIV_SHOP_OPEN,
#endif
	SHOP_SUBHEADER_GC_NOT_ENOUGH_10TH_COIN,
	SHOP_SUBHEADER_GC_LIMITED_PURCHASE_OVER,
	SHOP_SUBHEADER_GC_LIMITED_DATA_LOADING,
	SHOP_SUBHEADER_GC_UNK_19,
	SHOP_SUBHEADER_GC_UNK_20,
	SHOP_SUBHEADER_GC_UNK_21,
	SHOP_SUBHEADER_GC_NOT_ENOUGH_MEDAL_OF_HONOR,
	SHOP_SUBHEADER_GC_NOT_ENOUGH_GUILD_LEVEL,
	SHOP_SUBHEADER_GC_NOT_ENOUGH_BNW,
#if defined(ENABLE_SHOPEX_RENEWAL)
	SHOP_SUBHEADER_GC_NOT_ENOUGH_ITEM,
#endif
};

typedef struct packet_shop
{
	BYTE header;
	WORD size;
	BYTE subheader;
} TPacketGCShop;

typedef struct packet_exchange
{
	BYTE header;
	BYTE subheader;
	BYTE is_me;
	DWORD arg1;
	TItemPos arg2;
	DWORD arg3;
	TItemPos arg4;
	long alValues[ITEM_SOCKET_SLOT_MAX_NUM];
	TPlayerItemAttribute aAttr[ITEM_ATTRIBUTE_SLOT_MAX_NUM];
#if defined(ENABLE_CHANGE_LOOK_SYSTEM)
	DWORD dwTransmutationVnum;
#endif
#if defined(ENABLE_REFINE_ELEMENT_SYSTEM)
	TPlayerItemRefineElement RefineElement;
#endif
#if defined(ENABLE_APPLY_RANDOM)
	TPlayerItemAttribute aApplyRandom[ITEM_APPLY_RANDOM_SLOT_MAX_NUM];
#endif
#if defined(ENABLE_SET_ITEM)
	BYTE bSetValue;
#endif
} TPacketGCExchange;

enum
{
	EXCHANGE_SUBHEADER_GC_START, // arg1 == vid
	EXCHANGE_SUBHEADER_GC_ITEM_ADD, // arg1 == vnum  arg2 == pos  arg3 == count
	EXCHANGE_SUBHEADER_GC_ITEM_DEL, // arg1 == pos
	EXCHANGE_SUBHEADER_GC_ELK_ADD, // arg1 == elk
#if defined(ENABLE_CHEQUE_SYSTEM)
	EXCHANGE_SUBHEADER_GC_CHEQUE_ADD, // arg1 == cheque
#endif
	EXCHANGE_SUBHEADER_GC_ACCEPT, // arg1 == accept
	EXCHANGE_SUBHEADER_GC_END, // arg1 == not used
	EXCHANGE_SUBHEADER_GC_ALREADY, // arg1 == not used
	EXCHANGE_SUBHEADER_GC_LESS_ELK, // arg1 == not used
#if defined(ENABLE_CHEQUE_SYSTEM)
	EXCHANGE_SUBHEADER_GC_LESS_CHEQUE, // arg1 == not used
#endif
};

typedef struct packet_position
{
	BYTE header;
	DWORD vid;
	BYTE position;
} TPacketGCPosition;

typedef struct packet_ping
{
	BYTE header;
} TPacketGCPing;

typedef struct packet_pong
{
	BYTE bHeader;
} TPacketCGPong;

typedef struct packet_script
{
	BYTE header;
	WORD size;
	BYTE skin;
	WORD src_size;
} TPacketGCScript;

typedef struct packet_target
{
	BYTE header;
	DWORD dwVID;
	BYTE bHPPercent;
#if defined(ENABLE_VIEW_TARGET_DECIMAL_HP) || defined(ENABLE_DEFENSE_WAVE)
	int iMinHP;
	int iMaxHP;
#if defined(ENABLE_DEFENSE_WAVE)
	bool bAlliance;
#endif
#endif
#if defined(ENABLE_ELEMENT_ADD)
	BYTE bElement[CPythonNonPlayer::MOB_ELEMENT_MAX_NUM];
#endif
} TPacketGCTarget;

#if defined(ENABLE_SEND_TARGET_INFO)
typedef struct SPacketGCTargetDropInfo
{
	DWORD dwVnum;
	BYTE bCount;
} TPacketGCTargetDropInfo;

typedef struct SPacketGCTargetInfo
{
	BYTE bHeader;
	WORD wSize;
	DWORD dwRaceVnum;
	DWORD dwVID;
	bool bDropMetinStone;
} TPacketGCTargetInfo;

typedef struct SPacketCGTargetInfo
{
	BYTE bHeader;
	DWORD dwVID;
} TPacketCGTargetInfo;
#endif

typedef struct packet_damage_info
{
	BYTE header;
	DWORD dwVID;
	BYTE flag;
	int  damage;
} TPacketGCDamageInfo;

typedef struct packet_mount
{
	BYTE header;
	DWORD vid;
	DWORD mount_vid;
	BYTE pos;
	DWORD _x, _y;
} TPacketGCMount;

typedef struct packet_change_speed
{
	BYTE header;
	DWORD vid;
	WORD moving_speed;
} TPacketGCChangeSpeed;

typedef struct packet_move
{
	BYTE bHeader;
	BYTE bFunc;
	BYTE bArg;
	BYTE bRot;
	DWORD dwVID;
	LONG lX;
	LONG lY;
	DWORD dwTime;
	DWORD dwDuration;
} TPacketGCMove;

enum
{
	QUEST_SEND_IS_BEGIN = 1 << 0,
	QUEST_SEND_TITLE = 1 << 1, // 28자 까지
	QUEST_SEND_CLOCK_NAME = 1 << 2, // 16자 까지
	QUEST_SEND_CLOCK_VALUE = 1 << 3,
	QUEST_SEND_COUNTER_NAME = 1 << 4, // 16자 까지
	QUEST_SEND_COUNTER_VALUE = 1 << 5,
	QUEST_SEND_ICON_FILE = 1 << 6, // 24자 까지 
};

typedef struct packet_quest_info
{
	BYTE header;
	WORD size;
	WORD index;
#if defined(ENABLE_QUEST_RENEWAL)
	BYTE type;
	bool is_confirmed;
#endif
	BYTE flag;
} TPacketGCQuestInfo;

typedef struct packet_quest_confirm
{
	BYTE header;
	char msg[128 + 1];
	long timeout;
	DWORD requestPID;
} TPacketGCQuestConfirm;

typedef struct packet_attack
{
	BYTE header;
	DWORD dwVID;
	DWORD dwVictimVID; // 적 VID
	BYTE bType; // 공격 유형
} TPacketGCAttack;

typedef struct packet_c2c
{
	BYTE header;
	WORD wSize;
} TPacketGCC2C;

typedef struct packetd_sync_position_element
{
	DWORD dwVID;
	long lX;
	long lY;
} TPacketGCSyncPositionElement;

typedef struct packetd_sync_position
{
	BYTE bHeader;
	WORD wSize;
} TPacketGCSyncPosition;

typedef struct packet_ownership
{
	BYTE bHeader;
	DWORD dwOwnerVID;
	DWORD dwVictimVID;
} TPacketGCOwnership;

#define	SKILL_MAX_NUM 255

typedef struct packet_skill_level
{
	BYTE bHeader;
	BYTE abSkillLevels[SKILL_MAX_NUM];
} TPacketGCSkillLevel;

typedef struct SPlayerSkill
{
	BYTE bMasterType;
	BYTE bLevel;
	time_t tNextRead;
} TPlayerSkill;

typedef struct packet_skill_level_new
{
	BYTE bHeader;
	TPlayerSkill skills[SKILL_MAX_NUM];
} TPacketGCSkillLevelNew;

// fly
typedef struct packet_fly
{
	BYTE bHeader;
	BYTE bType;
	DWORD dwStartVID;
	DWORD dwEndVID;
} TPacketGCCreateFly;

enum EPVPModes
{
	PVP_MODE_NONE,
	PVP_MODE_AGREE,
	PVP_MODE_FIGHT,
	PVP_MODE_REVENGE,
};

typedef struct packet_duel_start
{
	BYTE header;
	WORD wSize; // DWORD가 몇개? 개수 = (wSize - sizeof(TPacketGCPVPList)) / 4
} TPacketGCDuelStart;

typedef struct packet_pvp
{
	BYTE header;
	DWORD dwVIDSrc;
	DWORD dwVIDDst;
	BYTE bMode;
} TPacketGCPVP;

typedef struct packet_warp
{
	BYTE bHeader;
	LONG lX;
	LONG lY;
	LONG lAddr;
	WORD wPort;
} TPacketGCWarp;

typedef struct packet_party_invite
{
	BYTE header;
	DWORD leader_pid;
} TPacketGCPartyInvite;

typedef struct packet_party_add
{
	BYTE header;
	DWORD pid;
	char name[CHARACTER_NAME_MAX_LEN + 1];
#if defined(WJ_SHOW_PARTY_ON_MINIMAP)
	long mapIdx;
#endif
#if defined(ENABLE_PARTY_CHANNEL_FIX)
	BYTE channel;
#endif
} TPacketGCPartyAdd;

typedef struct packet_party_update
{
	BYTE header;
	DWORD pid;
	BYTE state;
	BYTE percent_hp;
	short affects[PARTY_AFFECT_SLOT_MAX_NUM];
#if defined(WJ_SHOW_PARTY_ON_MINIMAP)
	long x;
	long y;
#endif
} TPacketGCPartyUpdate;

typedef struct packet_party_remove
{
	BYTE header;
	DWORD pid;
} TPacketGCPartyRemove;

typedef TPacketCGSafeboxCheckout TPacketGCSafeboxCheckout;
typedef TPacketCGSafeboxCheckin TPacketGCSafeboxCheckin;

typedef struct packet_safebox_wrong_password
{
	BYTE bHeader;
} TPacketGCSafeboxWrongPassword;

typedef struct packet_safebox_size
{
	BYTE bHeader;
	BYTE bSize;
} TPacketGCSafeboxSize;

typedef struct packet_safebox_money_change
{
	BYTE bHeader;
	DWORD dwMoney;
} TPacketGCSafeboxMoneyChange;

typedef struct command_empire
{
	BYTE bHeader;
	BYTE bEmpire;
} TPacketCGEmpire;

typedef struct packet_empire
{
	BYTE bHeader;
	BYTE bEmpire;
} TPacketGCEmpire;

enum
{
	FISHING_SUBHEADER_GC_START,
	FISHING_SUBHEADER_GC_STOP,
	FISHING_SUBHEADER_GC_REACT,
	FISHING_SUBHEADER_GC_SUCCESS,
	FISHING_SUBHEADER_GC_FAIL,
	FISHING_SUBHEADER_GC_FISH,
};

typedef struct packet_fishing
{
	BYTE header;
	BYTE subheader;
	DWORD info;
	BYTE dir;
} TPacketGCFishing;

typedef struct paryt_parameter
{
	BYTE bHeader;
	BYTE bDistributeMode;
} TPacketGCPartyParameter;

//////////////////////////////////////////////////////////////////////////
// Guild

enum
{
	GUILD_SUBHEADER_GC_LOGIN,
	GUILD_SUBHEADER_GC_LOGOUT,
	GUILD_SUBHEADER_GC_LIST,
	GUILD_SUBHEADER_GC_GRADE,
	GUILD_SUBHEADER_GC_ADD,
	GUILD_SUBHEADER_GC_REMOVE,
	GUILD_SUBHEADER_GC_GRADE_NAME,
	GUILD_SUBHEADER_GC_GRADE_AUTH,
	GUILD_SUBHEADER_GC_INFO,
	GUILD_SUBHEADER_GC_COMMENTS,
	GUILD_SUBHEADER_GC_CHANGE_EXP,
	GUILD_SUBHEADER_GC_CHANGE_MEMBER_GRADE,
	GUILD_SUBHEADER_GC_SKILL_INFO,
	GUILD_SUBHEADER_GC_CHANGE_MEMBER_GENERAL,
	GUILD_SUBHEADER_GC_GUILD_INVITE,
	GUILD_SUBHEADER_GC_WAR,
	GUILD_SUBHEADER_GC_GUILD_NAME,
	GUILD_SUBHEADER_GC_GUILD_WAR_LIST,
	GUILD_SUBHEADER_GC_GUILD_WAR_END_LIST,
	GUILD_SUBHEADER_GC_WAR_POINT,
	GUILD_SUBHEADER_GC_MONEY_CHANGE,
};

typedef struct packet_guild
{
	BYTE header;
	WORD size;
	BYTE subheader;
} TPacketGCGuild;

// SubHeader - Grade
enum
{
	GUILD_AUTH_ADD_MEMBER = (1 << 0),
	GUILD_AUTH_REMOVE_MEMBER = (1 << 1),
	GUILD_AUTH_NOTICE = (1 << 2),
	GUILD_AUTH_SKILL = (1 << 3),
};

typedef struct packet_guild_sub_grade
{
	char grade_name[GUILD_GRADE_NAME_MAX_LEN + 1]; // 8+1 길드장, 길드원 등의 이름
	BYTE auth_flag;
} TPacketGCGuildSubGrade;

typedef struct packet_guild_sub_member
{
	DWORD pid;
	BYTE byGrade;
	BYTE byIsGeneral;
	BYTE byJob;
	BYTE byLevel;
	DWORD dwOffer;
	BYTE byNameFlag;
	//if NameFlag is TRUE, name is sent from server.
	//	char szName[CHARACTER_ME_MAX_LEN + 1];
} TPacketGCGuildSubMember;

typedef struct packet_guild_sub_info
{
	WORD member_count;
	WORD max_member_count;
	DWORD guild_id;
	DWORD master_pid;
	DWORD exp;
	BYTE level;
	char name[GUILD_NAME_MAX_LEN + 1];
	DWORD gold;
	BYTE hasLand;
} TPacketGCGuildInfo;

enum EGuildWarState
{
	GUILD_WAR_NONE,
	GUILD_WAR_SEND_DECLARE,
	GUILD_WAR_REFUSE,
	GUILD_WAR_RECV_DECLARE,
	GUILD_WAR_WAIT_START,
	GUILD_WAR_CANCEL,
	GUILD_WAR_ON_WAR,
	GUILD_WAR_END,

	GUILD_WAR_DURATION = 2 * 60 * 60, // 2시간
};

typedef struct packet_guild_war
{
	DWORD dwGuildSelf;
	DWORD dwGuildOpp;
	BYTE bType;
	BYTE bWarState;
} TPacketGCGuildWar;

typedef struct SPacketGuildWarPoint
{
	DWORD dwGainGuildID;
	DWORD dwOpponentGuildID;
	long lPoint;
} TPacketGuildWarPoint;

// SubHeader - Dungeon
enum
{
	DUNGEON_SUBHEADER_GC_TIME_ATTACK_START = 0,
	DUNGEON_SUBHEADER_GC_DESTINATION_POSITION = 1,
};

typedef struct packet_dungeon
{
	BYTE bHeader;
	WORD size;
	BYTE subheader;
} TPacketGCDungeon;

// Private Shop
typedef struct SPacketGCShopSign
{
	BYTE bHeader;
	DWORD dwVID;
	char szSign[SHOP_SIGN_MAX_LEN + 1];
#if defined(ENABLE_MYSHOP_DECO)
	BYTE bType;
#endif
} TPacketGCShopSign;

typedef struct SPacketGCTime
{
	BYTE bHeader;
	time_t time;
} TPacketGCTime;

enum
{
	WALKMODE_RUN,
	WALKMODE_WALK,
};

typedef struct SPacketGCWalkMode
{
	BYTE header;
	DWORD vid;
	BYTE mode;
} TPacketGCWalkMode;

typedef struct SPacketGCChangeSkillGroup
{
	BYTE header;
	BYTE skill_group;
} TPacketGCChangeSkillGroup;

struct TMaterial
{
	DWORD vnum;
	DWORD count;
};

typedef struct SRefineTable
{
	DWORD src_vnum;
	DWORD result_vnum;
	WORD material_count;
	int cost; // 소요 비용
	int prob; // 확률
	TMaterial materials[REFINE_MATERIAL_MAX_NUM];
} TRefineTable;

typedef struct SPacketGCRefineInformation
{
	BYTE header;
	BYTE pos;
	TRefineTable refine_table;
} TPacketGCRefineInformation;

typedef struct SPacketGCRefineInformationNew
{
	BYTE header;
	BYTE type;
	BYTE pos;
	TRefineTable refine_table;
#if defined(ENABLE_REFINE_ELEMENT_SYSTEM)
	TPlayerItemRefineElement RefineElement;
#endif
#if defined(ENABLE_APPLY_RANDOM)
	TPlayerItemAttribute aApplyRandom[ITEM_APPLY_RANDOM_SLOT_MAX_NUM];
#endif
} TPacketGCRefineInformationNew;

enum SPECIAL_EFFECT
{
	SE_NONE,

	SE_HPUP_RED,
	SE_SPUP_BLUE,
	SE_SPEEDUP_GREEN,
	SE_DXUP_PURPLE,
	SE_CRITICAL,
	SE_PENETRATE,
	SE_BLOCK,
	SE_DODGE,
	SE_CHINA_FIREWORK,
	SE_SPIN_TOP,
	SE_SUCCESS,
	SE_FAIL,
	SE_FR_SUCCESS,
	SE_LEVELUP_ON_14_FOR_GERMANY, //레벨업 14일때 ( 독일전용 )
	SE_LEVELUP_UNDER_15_FOR_GERMANY, //레벨업 15일때 ( 독일전용 )
	SE_PERCENT_DAMAGE1,
	SE_PERCENT_DAMAGE2,
	SE_PERCENT_DAMAGE3,

	SE_AUTO_HPUP,
	SE_AUTO_SPUP,

	SE_EQUIP_RAMADAN_RING, // 초승달의 반지를 착용하는 순간에 발동하는 이펙트
	SE_EQUIP_HALLOWEEN_CANDY, // 할로윈 사탕을 착용(-_-;)한 순간에 발동하는 이펙트
	SE_EQUIP_HAPPINESS_RING, // 크리스마스 행복의 반지를 착용하는 순간에 발동하는 이펙트
	SE_EQUIP_LOVE_PENDANT, // 발렌타인 사랑의 팬던트(71145) 착용할 때 이펙트 (발동이펙트임, 지속이펙트 아님)

#if defined(ENABLE_ACCE_COSTUME_SYSTEM)
	SE_ACCE_SUCESS_ABSORB,
	SE_ACCE_EQUIP,
	SE_ACCE_BACK,
#endif

	SE_EQUIP_EASTER_CANDY,

	SE_THUNDER_AREA,
	SE_THUNDER,
	SE_HEAL,

	SE_CAPE_OF_COURAGE,
	SE_EQUIP_CHOCOLATE_PENDANT,
#if defined(ENABLE_BALANCE_IMPROVING)
	SE_FEATHER_WALK,
#endif
	SE_PEPSI_EVENT,

#if defined(ENABLE_GUILD_DRAGONLAIR_SYSTEM)
	SE_DRAGONLAIR_STONE_UNBEATABLE_1,
	SE_DRAGONLAIR_STONE_UNBEATABLE_2,
	SE_DRAGONLAIR_STONE_UNBEATABLE_3,
#endif

	SE_BATTLE_POTION, // Battle Field
	SE_REFLECT, // AI_FLAG_REFLECT

	SE_SKILL_DAMAGE_ZONE,
	SE_SKILL_SAFE_ZONE,
	SE_METEOR,
	SE_BEAD_RAIN,
	SE_ARROW_RAIN,
	SE_FALL_ROCK,
	SE_HORSE_DROP,
	SE_EGG_DROP,
	SE_DEAPO_BOOM,

#if defined(ENABLE_FLOWER_EVENT)
	SE_FLOWER_EVENT,
#endif
#if defined(ENABLE_GEM_SYSTEM)
	SE_GEM_PENDANT,
#endif
#if defined(ENABLE_DEFENSE_WAVE)
	SE_DEFENSE_WAVE_LASER,
#endif
	SE_PET_ATTR_CHANGE_NEW_TYPE,
	SE_PET_PAY_SUMMON1,
#if defined(ENABLE_SUMMER_EVENT_ROULETTE)
	SE_SPECIAL_ROULETTE,
#endif
	SE_MISTS_ISLAND_0,
	SE_MISTS_ISLAND_1,
	SE_MISTS_ISLAND_2,
	SE_MISTS_ISLAND_3,
	SE_PASSIVE_ATTR_SKILL,
	SE_SUICIDE_BOMB_SMALL,
	SE_SUICIDE_BOMB_LARGE,
	SE_GROUND_THORN,
	SE_GROUND_SNAKE,
	SE_WHITE_DRAGON_BERSERK,
	SE_WHITE_DRAGON_COOLING,
	SE_USE_METINSTONE_RAIN_SPAWN_TICKET,
	SE_FAKE_METINSTONE_SPLASH_DAMAGE,
	SE_MOUNT_UPGRADE_SKILL,
	SE_SOUTH_REAPER_SKILL_2,
	SE_YEOMWANG_SKILL_2,
	SE_BUFF_ITEM_5,
	SE_BUFF_ITEM_13,
	SE_BUFF_ITEM_14,
	SE_BUFF_SYMBOL1,
	SE_BUFF_ITEM_8_ELEMENT_1,
	SE_BUFF_ITEM_8_ELEMENT_2,
	SE_BUFF_FLOWER_OF_GALE,
	SE_BUFF_FLOWER_OF_DESTRUCTION,
	SE_SUNGMAHEE_GATE_FLOWER_BLOW_RANGE,
	SE_SUNGMAHEE_GATE_INVINCIBLE_GATE,
	SE_SUNGMAHEE_GATE_NORMAL_GATE,
	SE_MOUNT_UPGRADE_GYEONGGONG,
#if defined(ENABLE_SNOWFLAKE_STICK_EVENT)
	SE_USE_SNOWFLAKE_STICK,
#endif
	SE_SPORTS_MATCH_BUFF_EMBLEM,
	SE_POINT_AREA_ELECT_ATTACK,
#ifdef ENABLE_QUEEN_NETHIS
	SE_EFFECT_SNAKE_REGEN,
#endif
};

enum ESpecialEffectType
{
	SE_TYPE_NORMAL,
	SE_TYPE_POSITION,
};

typedef struct SPacketGCSpecialEffect
{
	BYTE bHeader;
	BYTE bEffectNum;
	DWORD dwVID;
	BYTE bEffectType;
	long xEffectPos, yEffectPos;
} TPacketGCSpecialEffect;

typedef struct SPacketGCNPCPosition
{
	BYTE header;
	WORD size;
	WORD count;
} TPacketGCNPCPosition;

struct TNPCPosition
{
	BYTE bType;
	char name[CHARACTER_NAME_MAX_LEN + 1];
	long x;
	long y;
};

typedef struct SPacketGCChangeName
{
	BYTE header;
	DWORD pid;
	char name[CHARACTER_NAME_MAX_LEN + 1];
} TPacketGCChangeName;

enum EBlockAction
{
	BLOCK_EXCHANGE = (1 << 0),
	BLOCK_PARTY_INVITE = (1 << 1),
	BLOCK_GUILD_INVITE = (1 << 2),
	BLOCK_WHISPER = (1 << 3),
	BLOCK_MESSENGER_INVITE = (1 << 4),
	BLOCK_PARTY_REQUEST = (1 << 5),
};

typedef struct packet_login_key
{
	BYTE bHeader;
	DWORD dwLoginKey;
} TPacketGCLoginKey;

typedef struct packet_auth_success
{
	BYTE bHeader;
	DWORD dwLoginKey;
	BYTE bResult;
} TPacketGCAuthSuccess;

typedef struct packet_channel
{
	BYTE header;
	BYTE channel;
} TPacketGCChannel;

typedef struct SEquipmentItemSet
{
	DWORD dwVnum;
	BYTE bCount;
	long alSockets[ITEM_SOCKET_SLOT_MAX_NUM];
	TPlayerItemAttribute aAttr[ITEM_ATTRIBUTE_SLOT_MAX_NUM];
#if defined(ENABLE_CHANGE_LOOK_SYSTEM)
	DWORD dwTransmutationVnum;
#endif
#if defined(ENABLE_REFINE_ELEMENT_SYSTEM)
	TPlayerItemRefineElement RefineElement;
#endif
#if defined(ENABLE_APPLY_RANDOM)
	TPlayerItemAttribute aApplyRandom[ITEM_APPLY_RANDOM_SLOT_MAX_NUM];
#endif
#if defined(ENABLE_SET_ITEM)
	BYTE bSetValue;
#endif
} TEquipmentItemSet;

typedef struct pakcet_view_equip
{
	BYTE bHeader;
	DWORD dwVID;
	TEquipmentItemSet Equips[WEAR_MAX_NUM];
} TPacketGCViewEquip;

typedef struct
{
	DWORD dwID;
	long x, y;
	long width, height;
	DWORD dwGuildID;
} TLandPacketElement;

typedef struct packet_land_list
{
	BYTE header;
	WORD size;
} TPacketGCLandList;

typedef struct
{
	BYTE bHeader;
	long lID;
	char szTargetName[32 + 1];
} TPacketGCTargetCreate;

enum
{
	CREATE_TARGET_TYPE_NONE,
	CREATE_TARGET_TYPE_LOCATION,
	CREATE_TARGET_TYPE_CHARACTER,
#if defined(ENABLE_PRIVATESHOP_SEARCH_SYSTEM)
	CREATE_TARGET_TYPE_SHOP_SEARCH = 1 << 2,
#endif
};

typedef struct
{
	BYTE bHeader;
	long lID;
	char szTargetName[32 + 1];
	DWORD dwVID;
	BYTE byType;
} TPacketGCTargetCreateNew;

typedef struct
{
	BYTE bHeader;
	long lID;
	long lX, lY;
#if defined(ENABLE_PRIVATESHOP_SEARCH_SYSTEM)
	bool bIsShopSearch;
#endif
} TPacketGCTargetUpdate;

typedef struct
{
	BYTE bHeader;
	long lID;
} TPacketGCTargetDelete;

typedef struct
{
	DWORD dwType;
	POINT_TYPE wApplyOn;
	POINT_VALUE lApplyValue;
	DWORD dwFlag;
	long lDuration;
	long lSPCost;
#if defined(ENABLE_AFFECT_RENEWAL)
	bool bRealTime;
	bool bUpdate;
#endif
} TPacketAffectElement;

typedef struct
{
	BYTE bHeader;
	TPacketAffectElement elem;
} TPacketGCAffectAdd;

typedef struct
{
	BYTE bHeader;
	DWORD dwType;
	POINT_TYPE wApplyOn;
} TPacketGCAffectRemove;

typedef struct packet_mall_open
{
	BYTE bHeader;
	BYTE bSize;
} TPacketGCMallOpen;

typedef struct packet_lover_info
{
	BYTE bHeader;
	char szName[CHARACTER_NAME_MAX_LEN + 1];
	BYTE byLovePoint;
} TPacketGCLoverInfo;

typedef struct packet_love_point_update
{
	BYTE bHeader;
	BYTE byLovePoint;
} TPacketGCLovePointUpdate;

typedef struct packet_dig_motion
{
	BYTE header;
	DWORD vid;
	DWORD target_vid;
	BYTE count;
} TPacketGCDigMotion;

typedef struct SPacketGCOnTime
{
	BYTE header;
	int ontime; // sec
} TPacketGCOnTime;

typedef struct SPacketGCResetOnTime
{
	BYTE header;
} TPacketGCResetOnTime;

typedef struct SPacketGCPanamaPack
{
	BYTE bHeader;
	char szPackName[256];
	BYTE abIV[32];
} TPacketGCPanamaPack;

typedef struct SPacketGCHybridCryptKeys
{
private:
	SPacketGCHybridCryptKeys() : m_pStream(NULL) {}

public:
	SPacketGCHybridCryptKeys(int iStreamSize) : iKeyStreamLen(iStreamSize)
	{
		m_pStream = new BYTE[iStreamSize];
	}
	~SPacketGCHybridCryptKeys()
	{
		if (m_pStream)
		{
			delete[] m_pStream;
			m_pStream = NULL;
		}
	}
	static int GetFixedHeaderSize()
	{
		return sizeof(BYTE) + sizeof(WORD) + sizeof(int);
	}

	BYTE bHeader;
	WORD wDynamicPacketSize;
	int iKeyStreamLen;
	BYTE* m_pStream;

} TPacketGCHybridCryptKeys;

typedef struct SPacketGCHybridSDB
{
private:
	SPacketGCHybridSDB() : m_pStream(NULL) {}

public:
	SPacketGCHybridSDB(int iStreamSize) : iSDBStreamLen(iStreamSize)
	{
		m_pStream = new BYTE[iStreamSize];
	}
	~SPacketGCHybridSDB()
	{
		delete[] m_pStream;
		m_pStream = NULL;
	}
	static int GetFixedHeaderSize()
	{
		return sizeof(BYTE) + sizeof(WORD) + sizeof(int);
	}

	BYTE bHeader;
	WORD wDynamicPacketSize;
	int iSDBStreamLen;
	BYTE* m_pStream;

} TPacketGCHybridSDB;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Client To Client

typedef struct packet_state
{
	BYTE bHeader;
	BYTE bFunc;
	BYTE bArg;
	BYTE bRot;
	DWORD dwVID;
	DWORD dwTime;
	TPixelPosition kPPos;
} TPacketCCState;

#if defined(__IMPROVED_PACKET_ENCRYPTION__)
struct TPacketKeyAgreement
{
	static const int MAX_DATA_LEN = 256;
	BYTE bHeader;
	WORD wAgreedLength;
	WORD wDataLength;
	BYTE data[MAX_DATA_LEN];
};

struct TPacketKeyAgreementCompleted
{
	BYTE bHeader;
	BYTE data[3]; // dummy (not used)
};
#endif

typedef struct SPacketGCSpecificEffect
{
	BYTE header;
	DWORD vid;
	char effect_file[128];
} TPacketGCSpecificEffect;

#if defined(ENABLE_DRAGON_SOUL_SYSTEM)
// 용혼석
enum EDragonSoulRefineWindowRefineType
{
	DragonSoulRefineWindow_UPGRADE,
	DragonSoulRefineWindow_IMPROVEMENT,
	DragonSoulRefineWindow_REFINE,
};

enum EPacketCGDragonSoulSubHeaderType
{
	DS_SUB_HEADER_OPEN,
	DS_SUB_HEADER_CLOSE,
	DS_SUB_HEADER_DO_UPGRADE,
	DS_SUB_HEADER_DO_IMPROVEMENT,
	DS_SUB_HEADER_DO_REFINE,
	DS_SUB_HEADER_REFINE_FAIL,
	DS_SUB_HEADER_REFINE_FAIL_MAX_REFINE,
	DS_SUB_HEADER_REFINE_FAIL_INVALID_MATERIAL,
	DS_SUB_HEADER_REFINE_FAIL_NOT_ENOUGH_MONEY,
	DS_SUB_HEADER_REFINE_FAIL_NOT_ENOUGH_MATERIAL,
	DS_SUB_HEADER_REFINE_FAIL_TOO_MUCH_MATERIAL,
	DS_SUB_HEADER_REFINE_SUCCEED,
#if defined(ENABLE_DS_CHANGE_ATTR)
	DS_SUB_HEADER_OPEN_CHANGE_ATTR,
	DS_SUB_HEADER_DO_CHANGE_ATTR,
#endif
};

typedef struct SPacketCGDragonSoulRefine
{
	SPacketCGDragonSoulRefine() : header(HEADER_CG_DRAGON_SOUL_REFINE) {}
	BYTE header;
	BYTE bSubType;
	TItemPos ItemGrid[DS_REFINE_WINDOW_MAX_NUM];
} TPacketCGDragonSoulRefine;

typedef struct SPacketGCDragonSoulRefine
{
	SPacketGCDragonSoulRefine() : header(HEADER_GC_DRAGON_SOUL_REFINE) {}
	BYTE header;
	BYTE bSubType;
	TItemPos Pos;
} TPacketGCDragonSoulRefine;
#endif

typedef struct SChannelStatus
{
	short nPort;
	BYTE bStatus;
} TChannelStatus;

#if defined(ENABLE_MINI_GAME_CATCH_KING)
enum EMiniGameCatchKingCGSubHeader
{
	CATCHKING_CG_START,
	CATCHKING_CG_CLICK_HAND,
	CATCHKING_CG_CLICK_CARD,
	CATCHKING_CG_REWARD,
#if defined(ENABLE_CATCH_KING_EVENT_FLAG_RENEWAL)
	CATCHKING_CG_REQUEST_QUEST_FLAG,
#endif
};

enum EMiniGameCatchKingGCSubHeader
{
	CATCHKING_GC_START,
	CATCHKING_GC_SET_CARD,
	CATCHKING_GC_RESULT_FIELD,
	CATCHKING_GC_SET_END_CARD,
	CATCHKING_GC_REWARD,
#if defined(ENABLE_CATCH_KING_EVENT_FLAG_RENEWAL)
	CATCHKING_GC_SET_CARD_PIECE_FLAG,
	CATCHKING_GC_SET_CARD_FLAG,
	CATCHKING_GC_SET_QUEST_FLAG,
	CATCHKING_GC_NO_MORE_GAIN,
#endif
};

typedef struct SPacketCGMiniGameCatchKing
{
	BYTE bHeader;
	BYTE bSubHeader;
	BYTE bSubArgument;
} TPacketCGMiniGameCatchKing;

typedef struct SPacketGCMiniGameCatchKing
{
	BYTE bHeader;
	WORD wSize;
	BYTE bSubHeader;
} TPacketGCMiniGameCatchKing;

typedef struct SPacketGCCatchKingEventInfo
{
	BYTE bHeader;
	bool bIsEnable;
} TPacketGCCatchKingEventInfo;

typedef struct SPacketGCMiniGameCatchKingResult
{
	DWORD dwPoints;
	BYTE bRowType;
	BYTE bCardPos;
	BYTE bCardValue;
	bool bKeepFieldCard;
	bool bDestroyHandCard;
	bool bGetReward;
	bool bIsFiveNearBy;
} TPacketGCMiniGameCatchKingResult;

typedef struct SPacketGCMiniGameCatchKingSetEndCard
{
	BYTE bCardPos;
	BYTE bCardValue;
} TPacketGCMiniGameCatchKingSetEndCard;

#if defined(ENABLE_CATCH_KING_EVENT_FLAG_RENEWAL)
typedef struct SPacketGCMiniGameCatchKingQuestFlag
{
	WORD wPieceCount, wPackCount;
} TPacketGCMiniGameCatchKingQuestFlag;
#endif
#endif

#if defined(ENABLE_PRIVATESHOP_SEARCH_SYSTEM)
typedef struct command_privateshop_searchcg
{
	BYTE header;
	BYTE bJob;
	BYTE bMaskType;
	int iMaskSub;
	int iMinRefine;
	int iMaxRefine;
	int iMinLevel;
	int iMaxLevel;
	int iMinGold;
	int iMaxGold;
	char szItemName[CItemData::ITEM_NAME_MAX_LEN + 1];
#if defined(ENABLE_CHEQUE_SYSTEM)
	int iMinCheque;
	int iMaxCheque;
#endif
} TPacketCGPrivateShopSearch;

typedef struct command_privateshopsearch_item
{
	packet_shop_item item;
	char szSellerName[CHARACTER_NAME_MAX_LEN + 1];
	DWORD dwShopPID;
	DWORD dwShopVID;
} TPacketGCPrivateShopSearchItem;

typedef struct command_privateshop_searchgc
{
	BYTE header;
	WORD size;
} TPacketGCPrivateShopSearch;

typedef struct command_privateshop_searchopengc
{
	BYTE header;
} TPacketGCPrivateShopSearchOpen;

typedef struct command_privateshop_closecg
{
	BYTE header;
} TPacketCGPrivateShopSearchClose;

typedef struct command_privateshop_buy_item
{
	BYTE header;
	BYTE pos;
	DWORD dwShopPID;
	DWORD dwShopVID;
} TPacketCGPrivateShopSearchBuyItem;
#endif

#if defined(ENABLE_SKILLBOOK_COMB_SYSTEM)
typedef struct SPacketCGSkillBookCombination
{
	BYTE bHeader;
	BYTE bAction;
	TItemPos CombItemGrid[SKILLBOOK_COMB_SLOT_MAX];
} TPacketCGSkillBookCombination;
#endif

#if defined(ENABLE_ACCE_COSTUME_SYSTEM)
enum ESubHeaderGCAcceRefine
{
	ACCE_REFINE_SUBHEADER_GC_CLOSE,
	ACCE_REFINE_SUBHEADER_GC_OPEN,
	ACCE_REFINE_SUBHEADER_GC_SET_ITEM,
	ACCE_REFINE_SUBHEADER_GC_CLEAR_SLOT,
	ACCE_REFINE_SUBHEADER_GC_CLEAR_ALL,
	ACCE_REFINE_SUBHEADER_GC_CLEAR_RIGHT,
};

typedef struct SSubPacketGCAcceRefineOpenClose
{
	BYTE bType;
	bool bServerClose;
} TSubPacketGCAcceRefineOpenClose;

typedef struct SSubPacketGCAcceRefineSetItem
{
	TItemPos AttachedPos;
	TItemPos SelectedPos;
	TItemData Item;
} TSubPacketGCAcceRefineSetItem;

typedef struct SSubPacketGCAcceRefineClearSlot
{
	BYTE bSlotIndex;
} TSubPacketGCAcceRefineClearSlot;

enum ESubHeaderCGAcceRefine
{
	ACCE_REFINE_SUBHEADER_CG_CHECKIN,
	ACCE_REFINE_SUBHEADER_CG_CHECKOUT,
	ACCE_REFINE_SUBHEADER_CG_ACCEPT,
	ACCE_REFINE_SUBHEADER_CG_CANCEL,
};

typedef struct SSubPacketCGAcceRefineCheckIn
{
	TItemPos AttachedPos;
	TItemPos SelectedPos;
	BYTE bType;
} TSubPacketCGAcceRefineCheckIn;

typedef struct SSubPacketCGAcceRefineCheckOut
{
	TItemPos SelectedPos;
	BYTE bType;
} TSubPacketCGAcceRefineCheckOut;

typedef struct SSubPacketCGAcceRefineAccept
{
	BYTE bType;
} TSubPacketCGAcceRefineAccept;

typedef struct SPacketGCAcceRefine
{
	BYTE bHeader;
	WORD wSize;
	BYTE bSubHeader;
} TPacketGCAcceRefine;

typedef struct SPacketCGAcceRefine
{
	SPacketCGAcceRefine() : bHeader(HEADER_CG_ACCE_REFINE) {}
	BYTE bHeader;
	WORD wSize;
	BYTE bSubHeader;
} TPacketCGAcceRefine;
#endif

#if defined(ENABLE_AURA_COSTUME_SYSTEM)
enum ESubHeaderGCAuraRefine
{
	AURA_REFINE_SUBHEADER_GC_CLOSE,
	AURA_REFINE_SUBHEADER_GC_OPEN,
	AURA_REFINE_SUBHEADER_GC_SET_ITEM,
	AURA_REFINE_SUBHEADER_GC_CLEAR_SLOT,
	AURA_REFINE_SUBHEADER_GC_CLEAR_ALL,
	AURA_REFINE_SUBHEADER_GC_INFO
};

typedef struct SSubPacketGCAuraRefineOpenClose
{
	BYTE bType;
	bool bServerClose;
} TSubPacketGCAuraRefineOpenClose;

typedef struct SSubPacketGCAuraRefineSetItem
{
	TItemPos AttachedPos;
	TItemPos SelectedPos;
	TItemData Item;
} TSubPacketGCAuraRefineSetItem;

typedef struct SSubPacketGCAuraRefineClearSlot
{
	BYTE bSlotIndex;
} TSubPacketGCAuraRefineClearSlot;

typedef struct SSubPacketGCAuraRefineInfo
{
	BYTE bInfoType;
	BYTE bInfoLevel;
	BYTE bInfoExpPercent;
} TSubPacketGCAuraRefineInfo;

enum ESubHeaderCGAuraRefine
{
	AURA_REFINE_SUBHEADER_CG_CHECKIN,
	AURA_REFINE_SUBHEADER_CG_CHECKOUT,
	AURA_REFINE_SUBHEADER_CG_ACCEPT,
	AURA_REFINE_SUBHEADER_CG_CANCEL
};

typedef struct SSubPacketCGAuraRefineCheckIn
{
	TItemPos AttachedPos;
	TItemPos SelectedPos;
	BYTE bType;
} TSubPacketCGAuraRefineCheckIn;

typedef struct SSubPacketCGAuraRefineCheckOut
{
	TItemPos SelectedPos;
	BYTE bType;
} TSubPacketCGAuraRefineCheckOut;

typedef struct SSubPacketCGAuraRefineAccept
{
	BYTE bType;
} TSubPacketCGAuraRefineAccept;

typedef struct SPacketGCAuraRefine
{
	BYTE bHeader;
	WORD wSize;
	BYTE bSubHeader;
} TPacketGCAuraRefine;

typedef struct SPacketCGAuraRefine
{
	SPacketCGAuraRefine() : bHeader(HEADER_CG_AURA_REFINE) {}
	BYTE bHeader;
	WORD wSize;
	BYTE bSubHeader;
} TPacketCGAuraRefine;
#endif

#if defined(ENABLE_CHANGE_LOOK_SYSTEM)
enum class EPacketCGChangeLookSubHeader : BYTE
{
	ITEM_CHECK_IN,
	ITEM_CHECK_OUT,
	FREE_ITEM_CHECK_IN,
	FREE_ITEM_CHECK_OUT,
	ACCEPT,
	CANCEL
};

typedef struct packet_changelook_set
{
	BYTE bHeader;
	WORD wCell;
	BYTE bSlotIndex;
} TPacketGCChangeLookSet;

typedef struct packet_changelook_del
{
	BYTE bHeader;
	WORD wCell;
	BYTE bSlotIndex;
} TPacketGCChangeLookDel;

typedef struct command_changelook
{
	command_changelook(const BYTE c_bSubHeader) :
		bHeader(HEADER_CG_CHANGE_LOOK),
		bSubHeader(c_bSubHeader),
		bSlotIndex(255)
	{}
	BYTE bHeader;
	BYTE bSubHeader;
	BYTE bSlotIndex;
	TItemPos ItemPos;
} TPacketCGChangeLook;
#endif

#if defined(ENABLE_MAILBOX)
typedef struct packet_mailbox_process
{
	packet_mailbox_process() : bArg1(0), bArg2(0) {}
	BYTE bHeader;
	BYTE bSubHeader;
	BYTE bArg1;
	BYTE bArg2;
} TPacketMailboxProcess;

typedef struct SMailBoxRespondUnreadData
{
	BYTE bHeader;
	BYTE bItemMessageCount;
	BYTE bCommonMessageCount;
	bool bGMVisible;
} TMailBoxRespondUnreadData;

typedef struct packet_mailbox_process_all
{
	BYTE Index;
} TPacketGCMailboxProcessAll;

typedef struct packet_mailbox_add_data
{
	BYTE bHeader;
	BYTE Index;
	char szFrom[CHARACTER_NAME_MAX_LEN + 1];
	char szMessage[100 + 1];
	int iYang;
	int iWon;
	DWORD dwItemVnum;
	DWORD dwItemCount;
	long alSockets[ITEM_SOCKET_SLOT_MAX_NUM];
	TPlayerItemAttribute aAttr[ITEM_ATTRIBUTE_SLOT_MAX_NUM];
#if defined(ENABLE_CHANGE_LOOK_SYSTEM)
	DWORD dwChangeLookVnum;
#endif
#if defined(ENABLE_REFINE_ELEMENT_SYSTEM)
	TPlayerItemRefineElement RefineElement;
#endif
#if defined(ENABLE_APPLY_RANDOM)
	TPlayerItemAttribute aApplyRandom[ITEM_APPLY_RANDOM_SLOT_MAX_NUM];
#endif
#if defined(ENABLE_SET_ITEM)
	BYTE bSetValue;
#endif
} TPacketGCMailBoxAddData;

typedef struct packet_mailbox_message
{
	time_t SendTime;
	time_t DeleteTime;
	char szTitle[25 + 1];
	bool bIsGMPost;
	bool bIsItemExist;
	bool bIsConfirm;
} TPacketGCMailBoxMessage;

typedef struct packet_mailbox
{
	BYTE bHeader;
	WORD wSize;
} TPacketGCMailBox;

typedef struct packet_mailbox_write
{
	BYTE bHeader;
	char szName[CHARACTER_NAME_MAX_LEN + 1];
	char szTitle[25 + 1];
	char szMessage[100 + 1];
	TItemPos pos;
	int iYang;
	int iWon;
} TPacketCGMailboxWrite;

typedef struct packet_mailbox_write_confirm
{
	BYTE bHeader;
	char szName[CHARACTER_NAME_MAX_LEN + 1];
} TPacketCGMailboxWriteConfirm;
#endif

#if defined(ENABLE_RANKING_SYSTEM)
typedef struct SPartyMemberName
{
	char szName[CHARACTER_NAME_MAX_LEN + 1];
} TPartyMember;

typedef struct SRankingData
{
	char szGuildName[GUILD_NAME_MAX_LEN + 1];
	SPartyMemberName Member[PARTY_MAX_MEMBER];
	DWORD dwRecord0, dwRecord1;
	DWORD dwStartTime;
	BYTE bEmpire;
} TRankingData;

typedef struct SPacketGCRanking
{
	BYTE bHeader;
	WORD wSize;
	BYTE bType;
	BYTE bCategory;
} TPacketGCRanking;
#endif

#if defined(ENABLE_MOVE_COSTUME_ATTR)
typedef struct packet_cg_item_combination
{
	BYTE Header;
	short MediumIndex;
	short BaseIndex;
	short MaterialIndex;
} TPacketCGItemCombination;

typedef struct packet_cg_item_combination_cancel
{
	BYTE Header;
} TPacketCGItemCombinationCancel;
#endif

#if defined(ENABLE_CHANGED_ATTR)
typedef struct packet_gc_item_select_attr
{
	BYTE bHeader;
	TItemPos pItemPos;
	TPlayerItemAttribute aAttr[ITEM_ATTRIBUTE_SLOT_MAX_NUM];
} TPacketGCItemSelectAttr;

typedef struct packet_cg_item_select_attr
{
	BYTE bHeader;
	bool bNew;
	TItemPos pItemPos;
} TPacketCGItemSelectAttr;
#endif

#if defined(ENABLE_LOOTING_SYSTEM)
typedef struct SPacketCGLootFilter
{
	BYTE header;
	BYTE settings[ELootFilter::LOOT_FILTER_SETTINGS_MAX];
} TPacketCGLootFilter;

typedef struct SPacketGCLootFilter
{
	BYTE header;
	bool enable;
	DWORD vid;
} TPacketGCLootFilter;
#endif

#if defined(ENABLE_MINI_GAME_RUMI)
enum EMiniGameRumiCGSubHeader
{
	RUMI_CG_SUBHEADER_END,
	RUMI_CG_SUBHEADER_START,
	RUMI_CG_SUBHEADER_DECK_CARD_CLICK,
	RUMI_CG_SUBHEADER_HAND_CARD_CLICK,
	RUMI_CG_SUBHEADER_FIELD_CARD_CLICK,
#if defined(ENABLE_OKEY_EVENT_FLAG_RENEWAL)
	RUMI_CG_SUBHEADER_REQUEST_QUEST_FLAG,
#endif
};

enum EMiniGameRumiGCSubHeader
{
	RUMI_GC_SUBHEADER_END,
	RUMI_GC_SUBHEADER_START,
	RUMI_GC_SUBHEADER_SET_DECK,
	RUMI_GC_SUBHEADER_SET_SCORE,
	RUMI_GC_SUBHEADER_MOVE_CARD,
#if defined(ENABLE_OKEY_EVENT_FLAG_RENEWAL)
	RUMI_GC_SUBHEADER_SET_CARD_PIECE_FLAG,
	RUMI_GC_SUBHEADER_SET_CARD_FLAG,
	RUMI_GC_SUBHEADER_SET_QUEST_FLAG,
	RUMI_GC_SUBHEADER_NO_MORE_GAIN,
#endif
};

typedef struct SPacketCGMiniGameRumi
{
	BYTE bHeader;
	BYTE bSubHeader;
	BOOL bUseCard;
	BYTE bIndex;
	SPacketCGMiniGameRumi() :
		bHeader(HEADER_CG_MINI_GAME_RUMI),
		bSubHeader(RUMI_CG_SUBHEADER_END),
		bUseCard(FALSE),
		bIndex(0)
	{}
} TPacketCGMiniGameRumi;

typedef struct SPacketGCMiniGameRumiSetDeck
{
	BYTE bDeckCount;
} TPacketGCMiniGameRumiSetDeck;

typedef struct SPacketGCMiniGameRumiMoveCard
{
	BYTE bSrcPos, bSrcIndex, bSrcColor, bSrcNumber;
	BYTE bDstPos, bDstIndex, bDstColor, bDstNumber;
} TPacketGCMiniGameRumiMoveCard;

typedef struct SPacketGCMiniGameRumiSetScore
{
	WORD wScore, wTotalScore;
} TPacketGCMiniGameRumiSetScore;

#if defined(ENABLE_OKEY_EVENT_FLAG_RENEWAL)
typedef struct SPacketGCMiniGameRumiQuestFlag
{
	WORD wCardPieceCount, wCardCount;
} TPacketGCMiniGameRumiQuestFlag;
#endif

typedef struct SPacketGCMiniGameRumi
{
	BYTE bHeader;
	WORD wSize;
	BYTE bSubHeader;
} TPacketGCMiniGameRumi;
#endif

#if defined(ENABLE_LUCKY_BOX)
enum ELUCKY_BOX_ACTION
{
	LUCKY_BOX_ACTION_RETRY,
	LUCKY_BOX_ACTION_RECEIVE,
};

typedef struct SPacketCGLuckyBox
{
	BYTE bHeader;
	BYTE bAction;
} TPacketCGLuckyBox;

typedef struct SPacketGCLuckyBox
{
	BYTE bHeader;
	DWORD dwVNum;
	BYTE bCount;
	int iNeedMoney;
	WORD wSlotIndex;
} TPacketGCLuckyBox;
#endif

#if defined(ENABLE_ATTR_6TH_7TH)
enum EAttr67AddSubHeader
{
	SUBHEADER_CG_ATTR67_ADD_CLOSE,
	SUBHEADER_CG_ATTR67_ADD_OPEN,
	SUBHEADER_CG_ATTR67_ADD_REGIST,
};

typedef struct SPacketCGAttr67Add
{
	BYTE byHeader;
	BYTE bySubHeader;
	TAttr67AddData Attr67AddData;
} TPacketCGAttr67Add;
#endif

#if defined(ENABLE_FISHING_GAME)
enum EFishingGameSubHeader
{
	FISHING_GAME_SUBHEADER_OPEN,
	FISHING_GAME_SUBHEADER_GOAL,
	FISHING_GAME_SUBHEADER_QUIT,
};

typedef struct SPacketGCFishingGame
{
	BYTE bHeader;
	BYTE bSubHeader;
	BYTE bLevel;
} TPacketGCFishingGame;

typedef struct SPacketCGFishingGame
{
	BYTE bHeader;
	BYTE bSubHeader;
	BYTE bGoals;
} TPacketCGFishingGame;
#endif

#if defined(ENABLE_GEM_SYSTEM)
enum EGemShopSubHeader : BYTE
{
	SUBHEADER_GEM_SHOP_CLOSE,
	SUBHEADER_GEM_SHOP_OPEN,
	SUBHEADER_GEM_SHOP_BUY,
	SUBHEADER_GEM_SHOP_SLOT_ADD,
	SUBHEADER_GEM_SHOP_REFRESH,
	SUBHEADER_GEM_SHOP_MAX,
};

typedef struct SPacketGCGemShop
{
	BYTE bHeader;
	WORD wSize;
} TPacketGCGemShop;

typedef struct SPacketGCGemShopProcess
{
	BYTE bHeader;
	BYTE bSubHeader;
	BYTE bSlotIndex;
	bool bEnable;
} TPacketGCGemShopProcess;

typedef struct command_gem_shop
{
	command_gem_shop(const BYTE c_bSubHeader, const BYTE c_bSlotIndex)
		: bHeader(HEADER_CG_GEM_SHOP), bSubHeader(c_bSubHeader), bSlotIndex(c_bSlotIndex)
	{}
	BYTE bHeader;
	BYTE bSubHeader;
	BYTE bSlotIndex;
} TPacketCGGemShop;
#endif

#if defined(ENABLE_EXTEND_INVEN_SYSTEM)
typedef struct SPacketCGExtendInven
{
	BYTE bHeader;
	bool bUpgrade;
	BYTE bIndex;
} TPacketCGExtendInven;

typedef struct SPacketGCExtendInven
{
	BYTE bHeader;
	BYTE bStage;
	WORD wMaxNum;
} TPacketGCExtendInven;

typedef struct SPacketGCExtendInvenItemUse
{
	BYTE bHeader;
	BYTE bMsgResult;
	BYTE bEnoughCount;
} TPacketGCExtendInvenItemUse;
#endif

#if defined(ENABLE_CLIENT_TIMER)
enum EClientTimerSubHeader
{
	CLIENT_TIMER_SUBHEADER_GC_SET,
	CLIENT_TIMER_SUBHEADER_GC_DELETE
};

enum EClientTimer
{
	ECLIENT_TIMER_END_TIME,
	ECLIENT_TIMER_ALARM_SECOND,
	ECLIENT_TIMER_MAX
};

typedef struct SPacketGCClientTimer
{
	BYTE bHeader;
	BYTE bSubHeader;
	DWORD dwData[ECLIENT_TIMER_MAX];
} TPacketGCClientTimer;
#endif

enum EEmoteSubHeader
{
	SUBHEADER_EMOTE_ADD,
	SUBHEADER_EMOTE_CLEAR,
	SUBHEADER_EMOTE_MOTION,
	SUBHEADER_EMOTE_ICON,
};

typedef struct SPacketGCEmote
{
	BYTE bHeader;
	BYTE bSubHeader;
	DWORD dwEmoteVnum;
	DWORD dwDuration;
	DWORD dwMainVID, dwTargetVID;
} TPacketGCEmote;

#if defined(ENABLE_CUBE_RENEWAL)
enum ECubeSubHeader
{
	SUBHEADER_GC_CUBE_OPEN = 0,
	SUBHEADER_GC_CUBE_CLOSE,
	SUBHEADER_GC_CUBE_RESULT,

	SUBHEADER_CG_CUBE_CLOSE = 0,
	SUBHEADER_CG_CUBE_MAKE,
};

typedef struct SPacketGCCube
{
	BYTE bHeader;
	BYTE bSubHeader;
	DWORD dwNPCVnum;
	BOOL bSuccess;
	DWORD dwFileCrc;
} TPacketGCCube;

typedef struct SPacketCGCube
{
	BYTE bHeader;
	BYTE bSubHeader;
	UINT iCubeIndex;
	UINT iQuantity;
	INT iImproveItemPos;
	DWORD dwFileCrc;
} TPacketCGCube;
#endif

#if defined(ENABLE_SNOWFLAKE_STICK_EVENT)
enum ESubPacketGCSnowflakeStickEvent
{
	SNOWFLAKE_STICK_EVENT_GC_SUBHEADER_EVENT_INFO,
	SNOWFLAKE_STICK_EVENT_GC_SUBHEADER_ADD_SNOW_BALL,
	SNOWFLAKE_STICK_EVENT_GC_SUBHEADER_ADD_TREE_BRANCH,
	SNOWFLAKE_STICK_EVENT_GC_SUBHEADER_MESSAGE_SNOW_BALL_MAX,
	SNOWFLAKE_STICK_EVENT_GC_SUBHEADER_MESSAGE_TREE_BRANCH_MAX,
	SNOWFLAKE_STICK_EVENT_GC_SUBHEADER_EXCHANGE_STICK_SUCCESS,
	SNOWFLAKE_STICK_EVENT_GC_SUBHEADER_EXCHANGE_PET_SUCCESS,
	SNOWFLAKE_STICK_EVENT_GC_SUBHEADER_EXCHANGE_MOUNT_SUCCESS,
	SNOWFLAKE_STICK_EVENT_GC_SUBHEADER_MESSAGE_USE_STICK_FAILED,
	SNOWFLAKE_STICK_EVENT_GC_SUBHEADER_MESSAGE_GET_RANK_BUFF,
	SNOWFLAKE_STICK_EVENT_GC_SUBHEADER_MESSAGE_GET_SNOWFLAKE_BUFF,
	SNOWFLAKE_STICK_EVENT_GC_SUBHEADER_ENABLE,
};

enum ESnowflakeStickEventQuestFlagType
{
	SNOWFLAKE_STICK_EVENT_QUEST_FLAG_SNOW_BALL,
	SNOWFLAKE_STICK_EVENT_QUEST_FLAG_TREE_BRANCH,
	SNOWFLAKE_STICK_EVENT_QUEST_FLAG_EXCHANGE_STICK,
	SNOWFLAKE_STICK_EVENT_QUEST_FLAG_EXCHANGE_STICK_COOLDOWN,
	SNOWFLAKE_STICK_EVENT_QUEST_FLAG_EXCHANGE_PET,
	SNOWFLAKE_STICK_EVENT_QUEST_FLAG_EXCHANGE_MOUNT,
	SNOWFLAKE_STICK_EVENT_QUEST_FLAG_USE_STICK_COOLDOWN,
	SNOWFLAKE_STICK_EVENT_QUEST_FLAG_MAX_NUM,
};

typedef struct SPacketGCSnowflakeStickEvent
{
	BYTE bHeader;
	BYTE bSubHeader;
	DWORD dwValue[SNOWFLAKE_STICK_EVENT_QUEST_FLAG_MAX_NUM];
} TPacketGCSnowflakeStickEvent;

enum ESubPacketCGSnowflakeStickEvent
{
	SNOWFLAKE_STICK_EVENT_CG_SUBHEADER_REQUEST_INFO,
	SNOWFLAKE_STICK_EVENT_CG_SUBHEADER_EXCHANGE_STICK,
	SNOWFLAKE_STICK_EVENT_CG_SUBHEADER_EXCHANGE_PET,
	SNOWFLAKE_STICK_EVENT_CG_SUBHEADER_EXCHANGE_MOUNT
};

typedef struct SPacketCGSnowflakeStickEvent
{
	BYTE bHeader;
	BYTE bSubHeader;
	SPacketCGSnowflakeStickEvent(const BYTE c_bSubHeader)
		: bHeader(HEADER_CG_SNOWFLAKE_STICK_EVENT), bSubHeader(c_bSubHeader) {}
} TPacketCGSnowflakeStickEvent;
#endif

#if defined(ENABLE_REFINE_ELEMENT_SYSTEM)
enum ERefineElementSubHeader
{
	REFINE_ELEMENT_GC_OPEN,
	REFINE_ELEMENT_GC_RESULT
};

enum EPacketCGRefineElement
{
	REFINE_ELEMENT_CG_CLOSE,
	REFINE_ELEMENT_CG_REFINE,
};

typedef struct SPacketGCRefineElement
{
	BYTE bHeader;
	BYTE bSubHeader;
	BYTE bRefineType;
	BYTE bResult;
	TItemPos SrcPos;
	TItemPos DestPos;
} TPacketGCRefineElement;

typedef struct SPacketCGRefineElement
{
	BYTE bHeader;
	BYTE bSubHeader;
	WORD wChangeElement;
	SPacketCGRefineElement(BYTE bSubHeader, WORD wChangeElement)
		: bHeader(HEADER_CG_REFINE_ELEMENT), bSubHeader(bSubHeader), wChangeElement(wChangeElement) {}
} TPacketCGRefineElement;
#endif

#if defined(ENABLE_MINI_GAME_YUTNORI)
enum EMiniGameYutnoriGCSubHeader
{
	YUTNORI_GC_SUBHEADER_START,
	YUTNORI_GC_SUBHEADER_STOP,
	YUTNORI_GC_SUBHEADER_SET_PROB,
	YUTNORI_GC_SUBHEADER_THROW,
	YUTNORI_GC_SUBHEADER_MOVE,
	YUTNORI_GC_SUBHEADER_AVAILABLE_AREA,
	YUTNORI_GC_SUBHEADER_PUSH_CATCH_YUT,
	YUTNORI_GC_SUBHEADER_SET_SCORE,
	YUTNORI_GC_SUBHEADER_SET_REMAIN_COUNT,
	YUTNORI_GC_SUBHEADER_PUSH_NEXT_TURN,
#if defined(ENABLE_YUTNORI_EVENT_FLAG_RENEWAL)
	YUTNORI_GC_SUBHEADER_SET_YUT_PIECE_FLAG,
	YUTNORI_GC_SUBHEADER_SET_YUT_BOARD_FLAG,
	YUTNORI_GC_SUBHEADER_SET_QUEST_FLAG,
	YUTNORI_GC_SUBHEADER_NO_MORE_GAIN,
#endif
};

enum EMiniGameYutnoriCGSubHeader
{
	YUTNORI_CG_SUBHEADER_START,
	YUTNORI_CG_SUBHEADER_GIVEUP,
	YUTNORI_CG_SUBHEADER_SET_PROB,
	YUTNORI_CG_SUBHEADER_CLICK_CHAR,
	YUTNORI_CG_SUBHEADER_THROW,
	YUTNORI_CG_SUBHEADER_MOVE,
	YUTNORI_CG_SUBHEADER_REQUEST_COM_ACTION,
	YUTNORI_CG_SUBHEADER_REWARD,
#if defined(ENABLE_YUTNORI_EVENT_FLAG_RENEWAL)
	YUTNORI_CG_SUBHEADER_REQUEST_QUEST_FLAG,
#endif
};

typedef struct SPacketCGMiniGameYutnori
{
	BYTE bHeader;
	BYTE bSubHeader;
	BYTE bArgument;
} TPacketCGMiniGameYutnori;

typedef struct SPacketGCMiniGameYutnori
{
	BYTE bHeader;
	WORD wSize;
	BYTE bSubHeader;
} TPacketGCMiniGameYutnori;

typedef struct SPacketGCMiniGameYutnoriSetProb
{
	BYTE bProbIndex;
} TPacketGCMiniGameYutnoriSetProb;

typedef struct SPacketGCMiniGameYutnoriThrowYut
{
	bool bPC;
	BYTE bYut;
} TPacketGCMiniGameYutnoriThrowYut;

typedef struct SPacketGCMiniGameYutnoriMoveYut
{
	bool bPC;
	BYTE bUnitIndex;
	bool bIsCatch;
	BYTE bStartIndex;
	BYTE bDestIndex;
} TPacketGCMiniGameYutnoriMoveYut;

typedef struct SPacketGCMiniGameYutnoriAvailableArea
{
	BYTE bPlayerIndex;
	BYTE bAvailableIndex;
} TPacketGCMiniGameYutnoriAvailableArea;

typedef struct SPacketGCMiniGameYutnoriPushCatchYut
{
	bool bPC;
	BYTE bUnitIndex;
} TPacketGCMiniGameYutnoriPushCatchYut;

typedef struct SPacketGCMiniGameYutnoriSetScore
{
	WORD wScore;
} TPacketGCMiniGameYutnoriSetScore;

typedef struct SPacketGCMiniGameYutnoriSetRemainCount
{
	BYTE bRemainCount;
} TPacketGCMiniGameYutnoriSetRemainCount;

typedef struct SPacketGCMiniGameYutnoriPushNextTurn
{
	bool bPC;
	BYTE bState;
} TPacketGCMiniGameYutnoriPushNextTurn;

#if defined(ENABLE_YUTNORI_EVENT_FLAG_RENEWAL)
typedef struct SPacketGCMiniGameYutnoriQuestFlag
{
	WORD wYutPieceCount;
	WORD wYutBoardCount;
} TPacketGCMiniGameYutnoriQuestFlag;
#endif
#endif

#if defined(ENABLE_QUEST_REQUEST_EVENT)
typedef struct SPacketCGRequestEventQuest
{
	BYTE bHeader;
	char szEventQuest[64 + 1];
} TPacketCGRequestEventQuest;
#endif

#if defined(ENABLE_LEFT_SEAT)
enum ELeftSeatCGSubHeader
{
	LEFT_SEAT_SET_WAIT_TIME_INDEX,
	LEFT_SEAT_SET_LOGOUT_TIME_INDEX,
	LEFT_SEAT_DISABLE_LOGOUT_STATE,
};

typedef struct SPacketCGLeftSeat
{
	BYTE bHeader;
	BYTE bSubHeader;
	BYTE bIndex;
} TPacketCGLeftSeat;
#endif

#if defined(ENABLE_GUILD_DRAGONLAIR_SYSTEM)
enum EGuildDragonLairType
{
	GUILD_DRAGONLAIR_TYPE_RED,
	GUILD_DRAGONLAIR_TYPE_BLUE,
	GUILD_DRAGONLAIR_TYPE_GREEN,
	GUILD_DRAGONLAIR_TYPE_MAX_NUM
};

enum EGuildDragonLairSubHeader
{
	GUILD_DRAGONLAIR_GC_SUBHEADER_RANKING,
#if defined(ENABLE_GUILD_DRAGONLAIR_PARTY_SYSTEM)
	GUILD_DRAGONLAIR_GC_SUBHEADER_1ST_GUILD_TEXT,
	GUILD_DRAGONLAIR_GC_SUBHEADER_START,
	GUILD_DRAGONLAIR_GC_SUBHEADER_SUCCESS,
#endif
};

typedef struct SPacketGCGuildDragonLair
{
	BYTE bHeader;
	WORD wSize;
	BYTE bSubHeader;
	BYTE bType;
#if defined(ENABLE_GUILD_DRAGONLAIR_PARTY_SYSTEM)
	DWORD dwSeconds;
#endif
} TPacketGCGuildDragonLair;

typedef struct SPacketGCGuildDragonLairRanking
{
	BYTE bType;
	DWORD dwGuildID;
	char szGuildName[GUILD_NAME_MAX_LEN + 1];
	BYTE bMemberCount;
	DWORD dwTime;
} TPacketGCGuildDragonLairRanking;
#endif

#if defined(ENABLE_SUMMER_EVENT_ROULETTE)
enum EPacketGCMiniGameRouletteSubHeader
{
	ROULETTE_GC_OPEN,
	ROULETTE_GC_START,
	ROULETTE_GC_REQUEST,
	ROULETTE_GC_END,
	ROULETTE_GC_CLOSE
};

enum EPacketCGMiniGameRouletteSubHeader
{
	ROULETTE_CG_START,
	ROULETTE_CG_REQUEST,
	ROULETTE_CG_END,
	ROULETTE_CG_CLOSE
};

typedef struct SPacketGCMiniGameRoulette
{
	BYTE bHeader;
	BYTE bSubHeader;
	BYTE bResult;
	DWORD dwExpireTime;
	struct
	{
		DWORD dwVnum;
		BYTE bCount;
	} ItemData[ROULETTE_ITEM_MAX];
} TPacketGCMiniGameRoulette;

typedef struct SPacketCGMiniGameRoulette
{
	BYTE bHeader;
	BYTE bSubHeader;
} TPacketCGMiniGameRoulette;
#endif

#if defined(ENABLE_FLOWER_EVENT)
enum EPacketGCFlowerEvent
{
	FLOWER_EVENT_SUBHEADER_GC_INFO_ALL,
	FLOWER_EVENT_SUBHEADER_GC_GET_INFO,
	FLOWER_EVENT_SUBHEADER_GC_UPDATE_INFO
};

typedef struct SPacketGCFlowerEvent
{
	BYTE bHeader;
	BYTE bSubHeader;
	BYTE bChatType;
	BYTE bShootType;
	int aiShootCount[SHOOT_TYPE_MAX + 1];
} TPacketGCFlowerEvent;

enum EPacketCGFlowerEvent
{
	FLOWER_EVENT_SUBHEADER_CG_INFO_ALL,
	FLOWER_EVENT_SUBHEADER_CG_EXCHANGE
};

typedef struct SPacketCGFlowerEvent
{
	BYTE bHeader;
	BYTE bSubHeader;
	BYTE bShootType;
	BYTE bExchangeKey;
	SPacketCGFlowerEvent()
		: bHeader(HEADER_CG_FLOWER_EVENT)
		, bSubHeader(FLOWER_EVENT_SUBHEADER_CG_INFO_ALL)
		, bShootType(SHOOT_TYPE_MAX)
		, bExchangeKey(0)
	{}
} TPacketCGFlowerEvent;
#endif

#pragma pack(pop)
