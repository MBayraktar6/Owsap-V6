#ifndef __INC_PACKET_H__
#define __INC_PACKET_H__

enum CG_HEADERS
{
	HEADER_CG_LOGIN = 1,
	HEADER_CG_ATTACK = 2,
	HEADER_CG_CHAT = 3,
	HEADER_CG_CHARACTER_CREATE = 4,
	HEADER_CG_CHARACTER_DELETE = 5,
	HEADER_CG_CHARACTER_SELECT = 6,
	HEADER_CG_MOVE = 7,
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
#if defined(__NEW_DROP_DIALOG__)
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
#if defined(__OX_RENEWAL__)
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
#if defined(__EXTEND_INVEN_SYSTEM__)
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
#if defined(__MYSHOP_DECO__)
	HEADER_CG_MYSHOP_DECO_STATE = 56,
	HEADER_CG_MYSHOP_DECO_ADD = 57,
#endif
	//HEADER_CG_UNUSED = 58,

#if defined(__SEND_TARGET_INFO__)
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
	HEADER_CG_SAFEBOX_CHECKIN = 70, // 아이템을 창고에 넣넎는다.
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
	HEADER_CG_ITEM_GIVE = 83,
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
#if defined(__CUBE_RENEWAL__)
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
	HEADER_CG_SYMBOL_CRC = 113,

	// SCRIPT_SELECT_ITEM
	HEADER_CG_SCRIPT_SELECT_ITEM = 114,
	// END_OF_SCRIPT_SELECT_ITEM

#if defined(__GEM_SYSTEM__)
	HEADER_CG_SELECT_ITEM_EX = 115,
#endif

	//HEADER_CG_UNUSED = 116,
	//HEADER_CG_UNUSED = 117,
	//HEADER_CG_UNUSED = 118,
	//HEADER_CG_UNUSED = 119,
#if defined(__QUEST_REQUEST_EVENT__)
	HEADER_CG_REQUEST_EVENT_QUEST = 120,
#endif
#if defined(__LEFT_SEAT__)
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
#if defined(__FISHING_GAME__)
	HEADER_CG_FISHING_GAME = 145,
#endif
#if defined(__REFINE_ELEMENT_SYSTEM__)
	HEADER_CG_REFINE_ELEMENT = 146,
#endif
	//HEADER_CG_UNUSED = 147,
	//HEADER_CG_UNUSED = 148,
	//HEADER_CG_UNUSED = 149,
	//HEADER_CG_UNUSED = 150,
#if defined(__SKILLBOOK_COMB_SYSTEM__)
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
#if defined(__ATTR_6TH_7TH__)
	HEADER_CG_ATTR67_ADD = 169,
#endif
#if defined(__SNOWFLAKE_STICK_EVENT__)
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
#if defined(__MINI_GAME_RUMI__)
	HEADER_CG_MINI_GAME_RUMI = 181,
#endif
#if defined(__MINI_GAME_YUTNORI__)
	HEADER_CG_MINI_GAME_YUTNORI = 182,
#endif
#if defined(__GEM_SHOP__)
	HEADER_CG_GEM_SHOP = 183,
#endif
	//HEADER_CG_UNUSED = 184,
	//HEADER_CG_UNUSED = 185,
#if defined(__SUMMER_EVENT_ROULETTE__)
	HEADER_CG_MINI_GAME_ROULETTE = 186,
#endif
#if defined(__FLOWER_EVENT__)
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

#if defined(__DRAGON_SOUL_SYSTEM__)
	HEADER_CG_DRAGON_SOUL_REFINE = 205,
#endif
	HEADER_CG_STATE_CHECKER = 206,

	//HEADER_CG_UNUSED = 207,
#if defined(__LUCKY_BOX__)
	HEADER_CG_LUCKY_BOX = 208,
#endif
	//HEADER_CG_UNUSED = 209,
	//HEADER_CG_UNUSED = 210,
#if defined(__ACCE_COSTUME_SYSTEM__)
	HEADER_CG_ACCE_REFINE = 211,
#endif
#if defined(__AURA_COSTUME_SYSTEM__)
	HEADER_CG_AURA_REFINE = 212,
#endif
	//HEADER_CG_UNUSED = 213,
	//HEADER_CG_UNUSED = 214,
#if defined(__MAILBOX__)
	HEADER_CG_MAILBOX_WRITE = 215,
	HEADER_CG_MAILBOX_WRITE_CONFIRM = 216,
	HEADER_CG_MAILBOX_PROCESS = 217,
#endif
#if defined(__MOVE_COSTUME_ATTR__)
	HEADER_CG_ITEM_COMBINATION = 218,
	HEADER_CG_ITEM_COMBINATION_CANCEL = 219,
#endif
#if defined(__PRIVATESHOP_SEARCH_SYSTEM__)
	HEADER_CG_PRIVATESHOP_SEARCH = 220,
	HEADER_CG_PRIVATESHOP_SEARCH_CLOSE = 221,
	HEADER_CG_PRIVATESHOP_SEARCH_BUY_ITEM = 222,
#endif
	//HEADER_CG_UNUSED = 223,
	//HEADER_CG_UNUSED = 224,
	//HEADER_CG_UNUSED = 225,
#if defined(__MINI_GAME_CATCH_KING__)
	HEADER_CG_MINI_GAME_CATCH_KING = 226,
#endif
#if defined(__CHANGED_ATTR__)
	HEADER_CG_ITEM_SELECT_ATTR = 227,
#endif
#if defined(__CHANGE_LOOK_SYSTEM__)
	HEADER_CG_CHANGE_LOOK = 228,
#endif
	//HEADER_CG_UNUSED = 229,
	//HEADER_CG_UNUSED = 230,
	//HEADER_CG_UNUSED = 231,
	//HEADER_CG_UNUSED = 232,
	//HEADER_CG_UNUSED = 233,
	//HEADER_CG_UNUSED = 234,
#if defined(__LOOT_FILTER_SYSTEM__)
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
	HEADER_CG_KEY_AGREEMENT = 0xfb, // 251
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
	HEADER_GC_MOVE = 3,
	HEADER_GC_CHAT = 4,
	HEADER_GC_SYNC_POSITION = 5,

	HEADER_GC_LOGIN_SUCCESS = 6,
	HEADER_GC_LOGIN_FAILURE = 7,

	HEADER_GC_CHARACTER_CREATE_SUCCESS = 8,
	HEADER_GC_CHARACTER_CREATE_FAILURE = 9,
	HEADER_GC_CHARACTER_DELETE_SUCCESS = 10,
	HEADER_GC_CHARACTER_DELETE_WRONG_SOCIAL_ID = 11,

	//HEADER_GC_UNUSED = 12,
	HEADER_GC_STUN = 13,
	HEADER_GC_DEAD = 14,

	HEADER_GC_MAIN_CHARACTER_OLD = 15,
	HEADER_GC_CHARACTER_POINTS = 16,
	HEADER_GC_CHARACTER_POINT_CHANGE = 17,
	HEADER_GC_CHANGE_SPEED = 18,
	HEADER_GC_CHARACTER_UPDATE = 19,

	HEADER_GC_ITEM_DEL = 20,
	HEADER_GC_ITEM_SET = 21,
	HEADER_GC_ITEM_USE = 22,
	HEADER_GC_ITEM_DROP = 23,

	//HEADER_GC_UNUSED = 24,
	HEADER_GC_ITEM_UPDATE = 25,
	HEADER_GC_ITEM_GROUND_ADD = 26,
	HEADER_GC_ITEM_GROUND_DEL = 27,

	HEADER_GC_QUICKSLOT_ADD = 28,
	HEADER_GC_QUICKSLOT_DEL = 29,
	HEADER_GC_QUICKSLOT_SWAP = 30,

	HEADER_GC_ITEM_OWNERSHIP = 31,

	HEADER_GC_LOGIN_SUCCESS_NEWSLOT = 32,

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

#if defined(__EXTEND_INVEN_SYSTEM__)
	HEADER_GC_EXTEND_INVEN = 47,
	HEADER_GC_EXTEND_INVEN_ITEM_USE = 48,
#endif
#if defined(__GUILD_DRAGONLAIR_SYSTEM__)
	HEADER_GC_GUILD_DRAGONLAIR = 49,
#endif
#if defined(__CLIENT_TIMER__)
	HEADER_GC_CLIENT_TIMER = 50,
#endif
	//HEADER_GC_UNUSED = 51,
	//HEADER_GC_UNUSED = 52,
	//HEADER_GC_UNUSED = 53,
	//HEADER_GC_UNUSED = 54,
	//HEADER_GC_UNUSED = 55,
	//HEADER_GC_UNUSED = 56,
	//HEADER_GC_UNUSED = 57,
#if defined(__SEND_TARGET_INFO__)
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
	HEADER_GC_SKILL_LEVEL_OLD = 72,
	//HEADER_GC_UNUSED = 73,
	HEADER_GC_MESSENGER = 74,
	HEADER_GC_GUILD = 75,
	HEADER_GC_SKILL_LEVEL = 76,

	HEADER_GC_PARTY_INVITE = 77,
	HEADER_GC_PARTY_ADD = 78,
	HEADER_GC_PARTY_UPDATE = 79,
	HEADER_GC_PARTY_REMOVE = 80,

	HEADER_GC_QUEST_INFO = 81,
	HEADER_GC_REQUEST_MAKE_GUILD = 82,
	HEADER_GC_PARTY_PARAMETER = 83,
	//HEADER_GC_UNUSED = 84,

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
	//HEADER_GC_UNUSED = 95,
	//HEADER_GC_UNUSED = 96,
#if defined(__CUBE_RENEWAL__)
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
	HEADER_GC_SKILL_GROUP = 112,
	HEADER_GC_MAIN_CHARACTER = 113,

	HEADER_GC_SEPCIAL_EFFECT = 114,
	HEADER_GC_NPC_POSITION = 115,

	//HEADER_CG_UNUSED = 116,
	//HEADER_GC_UNUSED = 117,
	HEADER_GC_LOGIN_KEY = 118,

	HEADER_GC_REFINE_INFORMATION = 119,
	//HEADER_GC_UNUSED = 120,
	HEADER_GC_CHANNEL = 121,

	HEADER_GC_MALL_OPEN = 122,

	HEADER_GC_TARGET_UPDATE = 123,
	HEADER_GC_TARGET_DELETE = 124,
	HEADER_GC_TARGET_CREATE = 125,

	HEADER_GC_AFFECT_ADD = 126,
	HEADER_GC_AFFECT_REMOVE = 127,

	HEADER_GC_MALL_SET = 128,
	HEADER_GC_MALL_DEL = 129,

	HEADER_GC_LAND_LIST = 130,
	HEADER_GC_LOVER_INFO = 131,
	HEADER_GC_LOVE_POINT_UPDATE = 132,

	HEADER_GC_SYMBOL_DATA = 133,

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
#if defined(__FISHING_GAME__)
	HEADER_GC_FISHING_GAME = 145,
#endif
#if defined(__REFINE_ELEMENT_SYSTEM__)
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
#if defined(__PRIVATESHOP_SEARCH_SYSTEM__)
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
#if defined(__SNOWFLAKE_STICK_EVENT__)
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
#if defined(__MINI_GAME_RUMI__)
	HEADER_GC_MINI_GAME_RUMI = 181,
#endif
#if defined(__MINI_GAME_YUTNORI__)
	HEADER_GC_MINI_GAME_YUTNORI = 182,
#endif
	//HEADER_GC_UNUSED = 183,
	//HEADER_GC_UNUSED = 184,
	//HEADER_GC_UNUSED = 185,
#if defined(__SUMMER_EVENT_ROULETTE__)
	HEADER_GC_MINI_GAME_ROULETTE = 186,
#endif
#if defined(__FLOWER_EVENT__)
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

#if defined(__GEM_SHOP__)
	HEADER_GC_GEM_SHOP = 201,
	HEADER_GC_GEM_SHOP_PROCESS = 202,
#endif
	//HEADER_GC_UNUSED = 203,
	//HEADER_GC_UNUSED = 204,
	//HEADER_GC_UNUSED = 205,
	//HEADER_GC_UNUSED = 206,
	//HEADER_GC_UNUSED = 207,

	HEADER_GC_SPECIFIC_EFFECT = 208,

	HEADER_GC_DRAGON_SOUL_REFINE = 209,
	HEADER_GC_RESPOND_CHANNELSTATUS = 210,

	//HEADER_GC_UNUSED = 211,
	//HEADER_GC_UNUSED = 212,
	//HEADER_GC_UNUSED = 213,
#if defined(__ACCE_COSTUME_SYSTEM__)
	HEADER_GC_ACCE_REFINE = 214,
#endif
#if defined(__AURA_COSTUME_SYSTEM__)
	HEADER_GC_AURA_REFINE = 215,
#endif
	//HEADER_GC_UNUSED = 216,
#if defined(__LUCKY_BOX__)
	HEADER_GC_LUCKY_BOX = 217,
#endif
	//HEADER_GC_UNUSED = 218,
	//HEADER_GC_UNUSED = 219,
	//HEADER_GC_UNUSED = 220,
#if defined(__MAILBOX__)
	HEADER_GC_MAILBOX_PROCESS = 221,
	HEADER_GC_MAILBOX = 222,
	HEADER_GC_MAILBOX_ADD_DATA = 223,
	HEADER_GC_MAILBOX_ALL = 224,
	HEADER_GC_MAILBOX_UNREAD = 225,
#endif
	//HEADER_GC_UNUSED = 226,
#if defined(__CHANGED_ATTR__)
	HEADER_GC_ITEM_SELECT_ATTR = 227,
#endif
#if defined(__CHANGE_LOOK_SYSTEM__)
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
#if defined(__MINI_GAME_CATCH_KING__)
	HEADER_GC_MINI_GAME_CATCH_KING = 238,
#endif
	//HEADER_GC_UNUSED = 239,
#if defined(__RANKING_SYSTEM__)
	HEADER_GC_RANKING = 240,
#endif
	//HEADER_GC_UNUSED = 241,
	//HEADER_GC_UNUSED = 242,
	//HEADER_GC_UNUSED = 243,
	//HEADER_GC_UNUSED = 244,
#if defined(__LOOT_FILTER_SYSTEM__)
	HEADER_GC_LOOT_FILTER = 245,
#endif
	//HEADER_GC_UNUSED = 246,
	//HEADER_GC_UNUSED = 247,
	//HEADER_GC_UNUSED = 248,
	//HEADER_GC_UNUSED = 249,
#if defined(__IMPROVED_PACKET_ENCRYPTION__)
	HEADER_GC_KEY_AGREEMENT_COMPLETED = 250, // 250
	HEADER_GC_KEY_AGREEMENT = 251, // 251
#endif
	HEADER_GC_TIME_SYNC = 252, // 252
	HEADER_GC_PHASE = 253, // 253
	HEADER_GC_BINDUDP = 254, // 254
	HEADER_GC_HANDSHAKE = 255, // 255
};

enum GG_HEADERS
{
	HEADER_GG_LOGIN = 1,
	HEADER_GG_LOGOUT = 2,
	HEADER_GG_RELAY = 3,
	HEADER_GG_NOTICE = 4,
	HEADER_GG_SHUTDOWN = 5,
	HEADER_GG_GUILD = 6,
	HEADER_GG_DISCONNECT = 7, // 누군가의 접속을 강제로 끊을 때
	HEADER_GG_SHOUT = 8,
	HEADER_GG_SETUP = 9,
	HEADER_GG_MESSENGER_ADD = 10,
	HEADER_GG_MESSENGER_REMOVE = 11,
	HEADER_GG_FIND_POSITION = 12,
	HEADER_GG_WARP_CHARACTER = 13,
	//HEADER_GG_UNUSED = 14,
	HEADER_GG_GUILD_WAR_ZONE_MAP_INDEX = 15,
	HEADER_GG_TRANSFER = 16,
#if defined(__XMAS_EVENT_2008__)
	HEADER_GG_XMAS_WARP_SANTA = 17,
	HEADER_GG_XMAS_WARP_SANTA_REPLY = 18,
#endif
	HEADER_GG_RELOAD_CRC_LIST = 19,
	HEADER_GG_LOGIN_PING = 20,
	HEADER_GG_CHECK_CLIENT_VERSION = 21,
	HEADER_GG_BLOCK_CHAT = 22,
	HEADER_GG_BIG_NOTICE = 23,
	HEADER_GG_BLOCK_EXCEPTION = 24,
	HEADER_GG_SIEGE = 25,
	HEADER_GG_MONARCH_NOTICE = 26,
	HEADER_GG_MONARCH_TRANSFER = 27,
	HEADER_GG_PCBANG_UPDATE = 28,
	HEADER_GG_CHECK_AWAKENESS = 29,
#if defined(__MESSENGER_BLOCK_SYSTEM__)
	HEADER_GG_MESSENGER_BLOCK_ADD = 30,
	HEADER_GG_MESSENGER_BLOCK_REMOVE = 31,
#endif
};

#pragma pack(1)
typedef struct SPacketGGSetup
{
	BYTE bHeader;
	WORD wPort;
	BYTE bChannel;
} TPacketGGSetup;

typedef struct SPacketGGLogin
{
	BYTE bHeader;
	char szName[CHARACTER_NAME_MAX_LEN + 1];
	DWORD dwPID;
	BYTE bEmpire;
	long lMapIndex;
	BYTE bChannel;
} TPacketGGLogin;

typedef struct SPacketGGLogout
{
	BYTE bHeader;
	char szName[CHARACTER_NAME_MAX_LEN + 1];
} TPacketGGLogout;

typedef struct SPacketGGRelay
{
	BYTE bHeader;
	char szName[CHARACTER_NAME_MAX_LEN + 1];
	long lSize;
} TPacketGGRelay;

typedef struct SPacketGGNotice
{
	BYTE bHeader;
	long lSize;
	char szArg[CHAT_MAX_LEN + 1];
	bool bBigFont;
} TPacketGGNotice;

typedef struct SPacketGGMonarchNotice
{
	BYTE bHeader;
	BYTE bEmpire;
	long lSize;
} TPacketGGMonarchNotice;

// FORKED_ROAD
typedef struct SPacketGGForkedMapInfo
{
	BYTE bHeader;
	BYTE bPass;
	BYTE bSungzi;
} TPacketGGForkedMapInfo;
// END_FORKED_ROAD

typedef struct SPacketGGShutdown
{
	BYTE bHeader;
} TPacketGGShutdown;

typedef struct SPacketGGGuild
{
	BYTE bHeader;
	BYTE bSubHeader;
	DWORD dwGuild;
} TPacketGGGuild;

enum
{
	GUILD_SUBHEADER_GG_CHAT,
	GUILD_SUBHEADER_GG_SET_MEMBER_COUNT_BONUS,
};

typedef struct SPacketGGGuildChat
{
	BYTE bHeader;
	BYTE bSubHeader;
	DWORD dwGuild;
	char szText[CHAT_MAX_LEN + 1];
} TPacketGGGuildChat;

typedef struct SPacketGGParty
{
	BYTE header;
	BYTE subheader;
	DWORD pid;
	DWORD leaderpid;
} TPacketGGParty;

enum
{
	PARTY_SUBHEADER_GG_CREATE,
	PARTY_SUBHEADER_GG_DESTROY,
	PARTY_SUBHEADER_GG_JOIN,
	PARTY_SUBHEADER_GG_QUIT,
};

typedef struct SPacketGGDisconnect
{
	BYTE bHeader;
	char szLogin[LOGIN_MAX_LEN + 1];
} TPacketGGDisconnect;

typedef struct SPacketGGShout
{
	BYTE bHeader;
	BYTE bEmpire;
	char szText[CHAT_MAX_LEN + 1];
#if defined(__MESSENGER_BLOCK_SYSTEM__)
	char szName[CHARACTER_NAME_MAX_LEN + 1];
#endif
#if defined(__MULTI_LANGUAGE_SYSTEM__)
	char szCountry[COUNTRY_NAME_MAX_LEN + 1];
#endif
} TPacketGGShout;

#if defined(__XMAS_EVENT_2008__)
typedef struct SPacketGGXmasWarpSanta
{
	BYTE bHeader;
	BYTE bChannel;
	long lMapIndex;
} TPacketGGXmasWarpSanta;

typedef struct SPacketGGXmasWarpSantaReply
{
	BYTE bHeader;
	BYTE bChannel;
} TPacketGGXmasWarpSantaReply;
#endif

typedef struct SPacketGGMessenger
{
	BYTE bHeader;
	char szAccount[CHARACTER_NAME_MAX_LEN + 1];
	char szCompanion[CHARACTER_NAME_MAX_LEN + 1];
} TPacketGGMessenger;

typedef struct SPacketGGFindPosition
{
	BYTE header;
	DWORD dwFromPID; // 저 위치로 워프하려는 사람
	DWORD dwTargetPID; // 찾는 사람
} TPacketGGFindPosition;

typedef struct SPacketGGWarpCharacter
{
	BYTE header;
	DWORD pid;
	long x;
	long y;
} TPacketGGWarpCharacter;

//HEADER_GG_GUILD_WAR_ZONE_MAP_INDEX = 15,

typedef struct SPacketGGGuildWarMapIndex
{
	BYTE bHeader;
	DWORD dwGuildID1;
	DWORD dwGuildID2;
	long lMapIndex;
} TPacketGGGuildWarMapIndex;

typedef struct SPacketGGTransfer
{
	BYTE bHeader;
	char szName[CHARACTER_NAME_MAX_LEN + 1];
	long lX, lY;
} TPacketGGTransfer;

typedef struct SPacketGGLoginPing
{
	BYTE bHeader;
	char szLogin[LOGIN_MAX_LEN + 1];
} TPacketGGLoginPing;

typedef struct SPacketGGBlockChat
{
	BYTE bHeader;
	char szName[CHARACTER_NAME_MAX_LEN + 1];
	long lBlockDuration;
} TPacketGGBlockChat;

/* 클라이언트 측에서 보내는 패킷 */
typedef struct command_text
{
	BYTE bHeader;
} TPacketCGText;

typedef struct command_phase
{
	BYTE bHeader;
} TPacketCGPhase;

/* 로그인 (1) */
typedef struct command_handshake
{
	BYTE bHeader;
	DWORD dwHandshake;
	DWORD dwTime;
	long lDelta;
} TPacketCGHandshake;

typedef struct command_login
{
	BYTE header;
	char login[LOGIN_MAX_LEN + 1];
	char passwd[PASSWD_MAX_LEN + 1];
} TPacketCGLogin;

typedef struct command_login2
{
	BYTE header;
	char login[LOGIN_MAX_LEN + 1];
	DWORD dwLoginKey;
	DWORD adwClientKey[4];
} TPacketCGLogin2;

typedef struct command_login3
{
	BYTE header;
	char login[LOGIN_MAX_LEN + 1];
	char passwd[PASSWD_MAX_LEN + 1];
	DWORD adwClientKey[4];
#if defined(__MULTI_LANGUAGE_SYSTEM__)
	char country[COUNTRY_NAME_MAX_LEN + 1];
#endif
} TPacketCGLogin3;

typedef struct packet_login_key
{
	BYTE bHeader;
	DWORD dwLoginKey;
} TPacketGCLoginKey;

typedef struct command_player_select
{
	BYTE header;
	BYTE index;
#if defined(__MULTI_LANGUAGE_SYSTEM__)
	char country[COUNTRY_NAME_MAX_LEN + 1];
#endif
} TPacketCGPlayerSelect;

typedef struct command_player_delete
{
	BYTE header;
	BYTE index;
	char private_code[8];
} TPacketCGPlayerDelete;

typedef struct command_player_create
{
	BYTE header;
	BYTE index;
	char name[CHARACTER_NAME_MAX_LEN + 1];
	WORD job;
	BYTE shape;
	BYTE Con;
	BYTE Int;
	BYTE Str;
	BYTE Dex;
} TPacketCGPlayerCreate;

typedef struct command_player_create_success
{
	BYTE header;
	BYTE bAccountCharacterIndex;
	TSimplePlayer player;
} TPacketGCPlayerCreateSuccess;

// 공격
typedef struct command_attack
{
	BYTE bHeader;
	BYTE bType;
	DWORD dwVID;
	BYTE bCRCMagicCubeProcPiece;
	BYTE bCRCMagicCubeFilePiece;
} TPacketCGAttack;

enum EMoveFuncType
{
	FUNC_WAIT,
	FUNC_MOVE,
	FUNC_ATTACK,
	FUNC_COMBO,
	FUNC_MOB_SKILL,
	_FUNC_SKILL,
	FUNC_MAX_NUM,
	FUNC_SKILL = 0x80,
};

// 이동
typedef struct command_move
{
	BYTE bHeader;
	BYTE bFunc;
	BYTE bArg;
	BYTE bRot;
	long lX;
	long lY;
	DWORD dwTime;
} TPacketCGMove;

typedef struct command_sync_position_element
{
	DWORD dwVID;
	long lX;
	long lY;
} TPacketCGSyncPositionElement;

// 위치 동기화
typedef struct command_sync_position // 가변 패킷
{
	BYTE bHeader;
	WORD wSize;
} TPacketCGSyncPosition;

/* 채팅 (3) */
typedef struct command_chat // 가변 패킷
{
	BYTE header;
	WORD size;
	BYTE type;
} TPacketCGChat;

/* 귓속말 */
typedef struct command_whisper
{
	BYTE bHeader;
	WORD wSize;
	char szNameTo[CHARACTER_NAME_MAX_LEN + 1];
} TPacketCGWhisper;

typedef struct command_entergame
{
	BYTE header;
} TPacketCGEnterGame;

typedef struct command_item_use
{
	BYTE header;
	TItemPos Cell;
} TPacketCGItemUse;

typedef struct command_item_use_to_item
{
	BYTE header;
	TItemPos Cell;
	TItemPos TargetCell;
} TPacketCGItemUseToItem;

typedef struct command_item_drop
{
	BYTE header;
	TItemPos Cell;
	DWORD gold;
#if defined(__CHEQUE_SYSTEM__)
	DWORD cheque;
#endif
} TPacketCGItemDrop;

typedef struct command_item_drop2
{
	BYTE header;
	TItemPos Cell;
	DWORD gold;
#if defined(__CHEQUE_SYSTEM__)
	DWORD cheque;
#endif
	WORD count;
} TPacketCGItemDrop2;

#if defined(__NEW_DROP_DIALOG__)
typedef struct command_item_destroy
{
	BYTE header;
	TItemPos Cell;
} TPacketCGItemDestroy;
#endif

typedef struct command_item_move
{
	BYTE header;
	TItemPos Cell;
	TItemPos CellTo;
	WORD count;
} TPacketCGItemMove;

typedef struct command_item_pickup
{
	BYTE header;
	DWORD vid;
} TPacketCGItemPickup;

typedef struct command_quickslot_add
{
	BYTE header;
	BYTE pos;
	TQuickslot slot;
} TPacketCGQuickslotAdd;

typedef struct command_quickslot_del
{
	BYTE header;
	BYTE pos;
} TPacketCGQuickslotDel;

typedef struct command_quickslot_swap
{
	BYTE header;
	BYTE pos;
	BYTE change_pos;
} TPacketCGQuickslotSwap;

enum
{
	SHOP_SUBHEADER_CG_END,
	SHOP_SUBHEADER_CG_BUY,
	SHOP_SUBHEADER_CG_SELL,
	SHOP_SUBHEADER_CG_SELL2,
};

typedef struct command_shop_buy
{
	BYTE count;
} TPacketCGShopBuy;

typedef struct command_shop_sell
{
	WORD wPos;
	WORD wCount;
	BYTE bType;
} TPacketCGShopSell;

typedef struct command_shop
{
	BYTE header;
	WORD subheader;
} TPacketCGShop;

typedef struct command_on_click
{
	BYTE header;
	DWORD vid;
} TPacketCGOnClick;

enum
{
	EXCHANGE_SUBHEADER_CG_START, /* arg1 == vid of target character */
	EXCHANGE_SUBHEADER_CG_ITEM_ADD, /* arg1 == position of item */
	EXCHANGE_SUBHEADER_CG_ITEM_DEL, /* arg1 == position of item */
	EXCHANGE_SUBHEADER_CG_ELK_ADD, /* arg1 == amount of gold */
#if defined(__CHEQUE_SYSTEM__)
	EXCHANGE_SUBHEADER_CG_CHEQUE_ADD, /* arg1 == amount of cheque */
#endif
	EXCHANGE_SUBHEADER_CG_ACCEPT, /* arg1 == not used */
	EXCHANGE_SUBHEADER_CG_CANCEL, /* arg1 == not used */
};

typedef struct command_exchange
{
	BYTE header;
	BYTE sub_header;
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
	//char file[32 + 1];
	//BYTE answer[16 + 1];
} TPacketCGScriptAnswer;

typedef struct command_script_button
{
	BYTE header;
	unsigned int idx;
} TPacketCGScriptButton;

typedef struct command_quest_input_string
{
	BYTE header;
	char msg[64 + 1];
} TPacketCGQuestInputString;

#if defined(__OX_RENEWAL__)
typedef struct command_quest_input_long_string
{
	BYTE header;
	char msg[128 + 1];
} TPacketCGQuestInputLongString;
#endif

typedef struct command_quest_confirm
{
	BYTE header;
	BYTE answer;
	DWORD requestPID;
} TPacketCGQuestConfirm;

/*
* 서버 측에서 보내는 패킷
*/
typedef struct packet_quest_confirm
{
	BYTE header;
	char msg[128 + 1];
	long timeout;
	DWORD requestPID;
} TPacketGCQuestConfirm;

typedef struct packet_handshake
{
	BYTE bHeader;
	DWORD dwHandshake;
	DWORD dwTime;
	long lDelta;
} TPacketGCHandshake;

enum EPhase
{
	PHASE_CLOSE,
	PHASE_HANDSHAKE,
	PHASE_LOGIN,
	PHASE_SELECT,
	PHASE_LOADING,
	PHASE_GAME,
	PHASE_DEAD,

	PHASE_CLIENT_CONNECTING,
	PHASE_DBCLIENT,
	PHASE_P2P,
	PHASE_AUTH,
};

typedef struct packet_phase
{
	BYTE header;
	BYTE phase;
} TPacketGCPhase;

typedef struct packet_bindudp
{
	BYTE header;
	DWORD addr;
	WORD port;
} TPacketGCBindUDP;

typedef struct packet_login_success
{
	BYTE bHeader;
	TSimplePlayer players[PLAYER_PER_ACCOUNT];
	DWORD guild_id[PLAYER_PER_ACCOUNT];
	char guild_name[PLAYER_PER_ACCOUNT][GUILD_NAME_MAX_LEN + 1];

	DWORD handle;
	DWORD random_key;
} TPacketGCLoginSuccess;

typedef struct packet_auth_success
{
	BYTE bHeader;
	DWORD dwLoginKey;
	BYTE bResult;
	BOOL bState;
} TPacketGCAuthSuccess;

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

typedef struct packet_login_failure
{
	BYTE header;
	char szStatus[ACCOUNT_STATUS_MAX_LEN + 1];
} TPacketGCLoginFailure;

typedef struct packet_create_failure
{
	BYTE header;
	BYTE bType;
} TPacketGCCreateFailure;

enum
{
	ADD_CHARACTER_STATE_DEAD = (1 << 0),
	ADD_CHARACTER_STATE_SPAWN = (1 << 1),
	ADD_CHARACTER_STATE_GUNGON = (1 << 2),
	ADD_CHARACTER_STATE_KILLER = (1 << 3),
	ADD_CHARACTER_STATE_PARTY = (1 << 4),
};

enum ECharacterEquipmentPart
{
	CHR_EQUIPPART_ARMOR,
	CHR_EQUIPPART_WEAPON,
	CHR_EQUIPPART_HEAD,
	CHR_EQUIPPART_HAIR,
#if defined(__ACCE_COSTUME_SYSTEM__)
	CHR_EQUIPPART_ACCE,
#endif
	CHR_EQUIPPART_AURA,

	CHR_EQUIPPART_NUM,
};

typedef struct packet_add_char
{
	BYTE header;
	DWORD dwVID;

#if defined(__WJ_SHOW_MOB_INFO__)
	DWORD dwLevel;
	DWORD dwAIFlag;
#endif

	float angle;
	long x;
	long y;
	long z;

	BYTE bType;
	WORD wRaceNum;
#if defined(__RACE_SWAP__)
	DWORD dwEventRaceNum;
#endif
	BYTE bMovingSpeed;
	BYTE bAttackSpeed;

	BYTE bStateFlag;
	DWORD dwAffectFlag[3]; // 효과
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
#if defined(__CONQUEROR_LEVEL__)
	DWORD dwConquerorLevel;
#endif
	short sAlignment; // 선악치
	BYTE bPKMode;
	DWORD dwMountVnum;
#if defined(__QUIVER_SYSTEM__)
	DWORD dwArrow;
#endif
#if defined(__REFINE_ELEMENT_SYSTEM__)
	WORD wRefineElementAffectType;
#endif
#if defined(__GUILD_LEADER_GRADE_NAME__)
	BYTE bGuildLeaderGrade;
#endif
#if defined(__MULTI_LANGUAGE_SYSTEM__)
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
#if defined(__CONQUEROR_LEVEL__)
	DWORD dwConquerorLevel;
#endif
	BYTE bPKMode;
	DWORD dwMountVnum;
#if defined(__QUIVER_SYSTEM__)
	DWORD dwArrow;
#endif
#if defined(__REFINE_ELEMENT_SYSTEM__)
	WORD wRefineElementAffectType;
#endif
#if defined(__GUILD_LEADER_GRADE_NAME__)
	BYTE bGuildLeaderGrade;
#endif
#if defined(__LEFT_SEAT__)
	bool bLeftSeat;
#endif
} TPacketGCCharacterUpdate;

typedef struct packet_del_char
{
	BYTE header;
	DWORD id;
} TPacketGCCharacterDelete;

typedef struct packet_chat // 가변 패킷
{
	BYTE header;
	WORD size;
	BYTE type;
	DWORD id;
	BYTE bEmpire;
#if defined(__LOCALE_CLIENT__)
	bool bCanFormat;
#endif
#if defined(__MULTI_LANGUAGE_SYSTEM__)
	char szCountry[COUNTRY_NAME_MAX_LEN + 1];
#endif
#if defined(__LOCALE_CLIENT__)
	packet_chat() : bCanFormat(true) {}
#endif
} TPacketGCChat;

typedef struct packet_whisper // 가변 패킷
{
	BYTE bHeader;
	WORD wSize;
	BYTE bType;
	char szNameFrom[CHARACTER_NAME_MAX_LEN + 1];
#if defined(__LOCALE_CLIENT__)
	bool bCanFormat;
#endif
#if defined(__MULTI_LANGUAGE_SYSTEM__)
	char szCountry[COUNTRY_NAME_MAX_LEN + 1];
#endif
#if defined(__LOCALE_CLIENT__)
	packet_whisper() : bCanFormat(true) {}
#endif
} TPacketGCWhisper;

typedef struct packet_main_character
{
	BYTE header;
	DWORD dwVID;
	WORD wRaceNum;
	char szName[CHARACTER_NAME_MAX_LEN + 1];
	long lx, ly, lz;
	BYTE empire;
	BYTE skill_group;
} TPacketGCMainCharacter;

// SUPPORT_BGM
typedef struct packet_main_character3_bgm
{
	enum
	{
		MUSIC_NAME_LEN = 24,
	};

	BYTE header;
	DWORD dwVID;
	WORD wRaceNum;
	char szChrName[CHARACTER_NAME_MAX_LEN + 1];
	char szBGMName[MUSIC_NAME_LEN + 1];
	long lx, ly, lz;
	BYTE empire;
	BYTE skill_group;
} TPacketGCMainCharacter3_BGM;

typedef struct packet_main_character4_bgm_vol
{
	enum
	{
		MUSIC_NAME_LEN = 24,
	};

	BYTE header;
	DWORD dwVID;
	WORD wRaceNum;
	char szChrName[CHARACTER_NAME_MAX_LEN + 1];
	char szBGMName[MUSIC_NAME_LEN + 1];
	float fBGMVol;
	long lx, ly, lz;
	BYTE empire;
	BYTE skill_group;
} TPacketGCMainCharacter4_BGM_VOL;
// END_OF_SUPPORT_BGM

typedef struct packet_points
{
	BYTE bHeader;
	POINT_VALUE lPoints[POINT_MAX_NUM];
} TPacketGCPoints;

typedef struct packet_skill_level
{
	BYTE bHeader;
	TPlayerSkill skills[SKILL_MAX_NUM];
} TPacketGCSkillLevel;

typedef struct packet_point_change
{
	BYTE bHeader;
	DWORD dwVID;
	POINT_TYPE wType;
	POINT_VALUE lAmount;
	POINT_VALUE lValue;
} TPacketGCPointChange;

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
	packet_dead() : dialog_type(DEAD_DIALOG_NORMAL), map_index(0) {}
} TPacketGCDead;

typedef struct packet_item_set_empty
{
	BYTE bHeader;
	TItemPos Cell;
	DWORD dwVnum;
	DWORD dwCount;
	long alSockets[ITEM_SOCKET_MAX_NUM];
	TPlayerItemAttribute aAttr[ITEM_ATTRIBUTE_MAX_NUM];
#if defined(__CHANGE_LOOK_SYSTEM__)
	DWORD dwTransmutationVnum;
#endif
#if defined(__REFINE_ELEMENT_SYSTEM__)
	TPlayerItemRefineElement RefineElement;
#endif
#if defined(__ITEM_APPLY_RANDOM__)
	TPlayerItemAttribute aApplyRandom[ITEM_APPLY_MAX_NUM];
#endif
#if defined(__SET_ITEM__)
	BYTE bSetValue;
#endif
} TPacketGCItemSetEmpty;

typedef struct packet_item_set
{
	BYTE bHeader;
	TItemPos Cell;
	DWORD dwVnum;
	DWORD dwCount;
	DWORD dwFlags;
	DWORD dwAntiFlags;
	bool bHighLight;
#if defined(__SOUL_BIND_SYSTEM__)
	long lSealDate;
#endif
	long alSockets[ITEM_SOCKET_MAX_NUM];
	TPlayerItemAttribute aAttr[ITEM_ATTRIBUTE_MAX_NUM];
#if defined(__CHANGE_LOOK_SYSTEM__)
	DWORD dwTransmutationVnum;
#endif
#if defined(__REFINE_ELEMENT_SYSTEM__)
	TPlayerItemRefineElement RefineElement;
#endif
#if defined(__ITEM_APPLY_RANDOM__)
	TPlayerItemAttribute aApplyRandom[ITEM_APPLY_MAX_NUM];
#endif
#if defined(__SET_ITEM__)
	BYTE bSetValue;
#endif
} TPacketGCItemSet;

typedef struct packet_item_del
{
	BYTE header;
	BYTE pos;
} TPacketGCItemDel;

struct packet_item_use
{
	BYTE header;
	TItemPos Cell;
	DWORD ch_vid;
	DWORD victim_vid;
	DWORD vnum;
};

struct packet_item_move
{
	BYTE header;
	TItemPos Cell;
	TItemPos CellTo;
};

typedef struct packet_item_update
{
	BYTE bHeader;
	TItemPos Cell;
	DWORD dwCount;
#if defined(__SOUL_BIND_SYSTEM__)
	long lSealDate;
#endif
	long alSockets[ITEM_SOCKET_MAX_NUM];
	TPlayerItemAttribute aAttr[ITEM_ATTRIBUTE_MAX_NUM];
#if defined(__CHANGE_LOOK_SYSTEM__)
	DWORD dwTransmutationVnum;
#endif
#if defined(__REFINE_ELEMENT_SYSTEM__)
	TPlayerItemRefineElement RefineElement;
#endif
#if defined(__ITEM_APPLY_RANDOM__)
	TPlayerItemAttribute aApplyRandom[ITEM_APPLY_MAX_NUM];
#endif
#if defined(__SET_ITEM__)
	BYTE bSetValue;
#endif
} TPacketGCItemUpdate;

typedef struct packet_item_ground_add
{
#if defined(__ITEM_DROP_RENEWAL__)
	packet_item_ground_add()
	{
		memset(&alSockets, 0, sizeof(alSockets));
		memset(&aAttrs, 0, sizeof(aAttrs));
	}
#endif

	BYTE bHeader;
	long lX, lY, lZ;
	DWORD dwVID;
	DWORD dwVnum;
#if defined(__SET_ITEM__)
	BYTE bSetValue;
#endif

#if defined(__ITEM_DROP_RENEWAL__)
	long alSockets[ITEM_SOCKET_MAX_NUM];
	TPlayerItemAttribute aAttrs[ITEM_ATTRIBUTE_MAX_NUM];
#endif
} TPacketGCItemGroundAdd;

typedef struct packet_item_ownership
{
	BYTE bHeader;
	DWORD dwVID;
	char szName[CHARACTER_NAME_MAX_LEN + 1];
} TPacketGCItemOwnership;

typedef struct packet_item_ground_del
{
	BYTE bHeader;
	DWORD dwVID;
} TPacketGCItemGroundDel;

struct packet_quickslot_add
{
	BYTE header;
	BYTE pos;
	TQuickslot slot;
};

struct packet_quickslot_del
{
	BYTE header;
	BYTE pos;
};

struct packet_quickslot_swap
{
	BYTE header;
	BYTE pos;
	BYTE pos_to;
};

struct packet_motion
{
	BYTE header;
	DWORD vid;
	DWORD victim_vid;
	WORD motion;
};

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
#if defined(__MYSHOP_DECO__)
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
#if defined(__SHOPEX_RENEWAL__)
	SHOP_SUBHEADER_GC_NOT_ENOUGH_ITEM,
#endif
};

struct packet_shop_item
{
	DWORD dwVnum;
	DWORD dwCount;
	DWORD dwPrice;
#if defined(__CHEQUE_SYSTEM__)
	DWORD dwCheque;
#endif
	BYTE bDisplayPos;
	long alSockets[ITEM_SOCKET_MAX_NUM];
	TPlayerItemAttribute aAttr[ITEM_ATTRIBUTE_MAX_NUM];
#if defined(__CHANGE_LOOK_SYSTEM__)
	DWORD dwTransmutationVnum;
#endif
#if defined(__REFINE_ELEMENT_SYSTEM__)
	TPlayerItemRefineElement RefineElement;
#endif
#if defined(__ITEM_APPLY_RANDOM__)
	TPlayerItemAttribute aApplyRandom[ITEM_APPLY_MAX_NUM];
#endif
#if defined(__SET_ITEM__)
	BYTE bSetValue;
#endif
#if defined(__SHOPEX_RENEWAL__)
	BYTE bPriceType;
	DWORD dwPriceVnum;
#endif
	packet_shop_item()
	{
#if defined(__CHEQUE_SYSTEM__)
		dwCheque = 0;
#endif
		memset(&alSockets, 0, sizeof(alSockets));
		memset(&aAttr, 0, sizeof(aAttr));
#if defined(__CHANGE_LOOK_SYSTEM__)
		dwTransmutationVnum = 0;
#endif
#if defined(__REFINE_ELEMENT_SYSTEM__)
		memset(&RefineElement, 0, sizeof(RefineElement));
#endif
#if defined(__ITEM_APPLY_RANDOM__)
		memset(&aApplyRandom, 0, sizeof(aApplyRandom));
#endif
#if defined(__SET_ITEM__)
		bSetValue = 0;
#endif
#if defined(__SHOPEX_RENEWAL__)
		bPriceType = SHOP_COIN_TYPE_GOLD;
		dwPriceVnum = 0;
#endif
	}
};

typedef struct packet_shop_start
{
	DWORD owner_vid;
#if defined(__MYSHOP_DECO__)
	BYTE shop_tab_count;
#endif
#if defined(__MYSHOP_EXPANSION__)
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
	int iPrice;
#if defined(__CHEQUE_SYSTEM__)
	int iCheque;
#endif
} TPacketGCShopUpdatePrice;

typedef struct packet_shop // 가변 패킷
{
	BYTE header;
	WORD size;
	BYTE subheader;
} TPacketGCShop;

typedef struct packet_exchange
{
	BYTE header;
	BYTE sub_header;
	BYTE is_me;
	DWORD arg1; // vnum
	TItemPos arg2; // cell
	DWORD arg3; // count
	TItemPos arg4;
	long alSockets[ITEM_SOCKET_MAX_NUM];
	TPlayerItemAttribute aAttr[ITEM_ATTRIBUTE_MAX_NUM];
#if defined(__CHANGE_LOOK_SYSTEM__)
	DWORD dwTransmutationVnum;
#endif
#if defined(__REFINE_ELEMENT_SYSTEM__)
	TPlayerItemRefineElement RefineElement;
#endif
#if defined(__ITEM_APPLY_RANDOM__)
	TPlayerItemAttribute aApplyRandom[ITEM_APPLY_MAX_NUM];
#endif
#if defined(__SET_ITEM__)
	BYTE bSetItem;
#endif
} TPacketGCExchange;

enum EPacketTradeSubHeaders
{
	EXCHANGE_SUBHEADER_GC_START, /* arg1 == vid */
	EXCHANGE_SUBHEADER_GC_ITEM_ADD, /* arg1 == vnum arg2 == pos arg3 == count */
	EXCHANGE_SUBHEADER_GC_ITEM_DEL,
	EXCHANGE_SUBHEADER_GC_GOLD_ADD, /* arg1 == gold */
#if defined(__CHEQUE_SYSTEM__)
	EXCHANGE_SUBHEADER_GC_CHEQUE_ADD, /* arg1 == cheque */
#endif
	EXCHANGE_SUBHEADER_GC_ACCEPT, /* arg1 == accept */
	EXCHANGE_SUBHEADER_GC_END, /* arg1 == not used */
	EXCHANGE_SUBHEADER_GC_ALREADY, /* arg1 == not used */
	EXCHANGE_SUBHEADER_GC_LESS_GOLD, /* arg1 == not used */
#if defined(__CHEQUE_SYSTEM__)
	EXCHANGE_SUBHEADER_GC_LESS_CHEQUE, /* arg1 == not used */
#endif
};

struct packet_position
{
	BYTE header;
	DWORD vid;
	BYTE position;
};

typedef struct packet_ping
{
	BYTE header;
} TPacketGCPing;

struct packet_script
{
	BYTE header;
	WORD size;
	BYTE skin;
	WORD src_size;
};

typedef struct packet_change_speed
{
	BYTE header;
	DWORD vid;
	WORD moving_speed;
} TPacketGCChangeSpeed;

struct packet_mount
{
	BYTE header;
	DWORD vid;
	DWORD mount_vid;
	BYTE pos;
	DWORD x, y;
};

typedef struct packet_move
{
	BYTE bHeader;
	BYTE bFunc;
	BYTE bArg;
	BYTE bRot;
	DWORD dwVID;
	long lX;
	long lY;
	DWORD dwTime;
	DWORD dwDuration;
} TPacketGCMove;

// 소유권
typedef struct packet_ownership
{
	BYTE bHeader;
	DWORD dwOwnerVID;
	DWORD dwVictimVID;
} TPacketGCOwnership;

// 위치 동기화 패킷의 bCount 만큼 붙는 단위
typedef struct packet_sync_position_element
{
	DWORD dwVID;
	long lX;
	long lY;
} TPacketGCSyncPositionElement;

// 위치 동기화
typedef struct packet_sync_position // 가변 패킷
{
	BYTE bHeader;
	WORD wSize; // 개수 = (wSize - sizeof(TPacketGCSyncPosition)) / sizeof(TPacketGCSyncPositionElement) 
} TPacketGCSyncPosition;

typedef struct packet_fly
{
	BYTE bHeader;
	BYTE bType;
	DWORD dwStartVID;
	DWORD dwEndVID;
} TPacketGCCreateFly;

typedef struct command_fly_targeting
{
	BYTE bHeader;
	DWORD dwTargetVID;
	long x, y;
} TPacketCGFlyTargeting;

typedef struct packet_fly_targeting
{
	BYTE bHeader;
	DWORD dwShooterVID;
	DWORD dwTargetVID;
	long x, y;
} TPacketGCFlyTargeting;

typedef struct packet_shoot
{
	BYTE bHeader;
	BYTE bType;
} TPacketCGShoot;

typedef struct packet_duel_start
{
	BYTE header;
	WORD wSize; // DWORD가 몇개? 개수 = (wSize - sizeof(TPacketGCPVPList)) / 4
} TPacketGCDuelStart;

enum EPVPModes
{
	PVP_MODE_NONE,
	PVP_MODE_AGREE,
	PVP_MODE_FIGHT,
	PVP_MODE_REVENGE
};

typedef struct packet_pvp
{
	BYTE bHeader;
	DWORD dwVIDSrc;
	DWORD dwVIDDst;
	BYTE bMode; // 0 이면 끔, 1이면 켬
} TPacketGCPVP;

typedef struct command_use_skill
{
	BYTE bHeader;
	DWORD dwVnum;
	DWORD dwVID;
} TPacketCGUseSkill;

typedef struct command_target
{
	BYTE header;
	DWORD dwVID;
} TPacketCGTarget;

typedef struct packet_target
{
	BYTE header;
	DWORD dwVID;
	BYTE bHPPercent;
#if defined(__VIEW_TARGET_HP__) || defined(__DEFENSE_WAVE__)
	int iMinHP;
	int iMaxHP;
	bool bAlliance;
#endif
#if defined(__ELEMENT_SYSTEM__)
	BYTE bElement[MOB_ELEMENT_MAX_NUM];
#endif
	packet_target() : dwVID(0), bHPPercent(0)
	{
#if defined(__VIEW_TARGET_HP__) || defined(__DEFENSE_WAVE__)
		iMinHP = 0;
		iMaxHP = 0;
		bAlliance = false;
#endif
#if defined(__ELEMENT_SYSTEM__)
		memset(&bElement, 0, sizeof(bElement));
#endif
	}
} TPacketGCTarget;

#if defined(__SEND_TARGET_INFO__)
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

typedef struct packet_warp
{
	BYTE bHeader;
	long lX;
	long lY;
	long lAddr;
	WORD wPort;
} TPacketGCWarp;

typedef struct command_warp
{
	BYTE bHeader;
} TPacketCGWarp;

struct packet_quest_info
{
	BYTE header;
	WORD size;
	WORD index;
#if defined(__QUEST_RENEWAL__)
	BYTE type;
	bool is_confirmed;
#endif
	BYTE flag;
};

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
#if defined(__MESSENGER_BLOCK_SYSTEM__)
	MESSENGER_SUBHEADER_GC_BLOCK_LIST,
	MESSENGER_SUBHEADER_GC_BLOCK_LOGIN,
	MESSENGER_SUBHEADER_GC_BLOCK_LOGOUT,
#endif
#if defined(__MESSENGER_GM__)
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
	packet_messenger_list() :
		bConnected(MESSENGER_CONNECTED_STATE_OFFLINE)
#if defined(__MESSENGER_DETAILS__)
		, dwLastPlayTime(0)
#endif
	{
	}
	BYTE bConnected;
	char szName[CHARACTER_NAME_MAX_LEN + 1];
#if defined(__MESSENGER_DETAILS__)
	DWORD dwLastPlayTime;
#if defined(__MULTI_LANGUAGE_SYSTEM__)
	char szCountry[COUNTRY_NAME_MAX_LEN + 1]{};
#endif
#endif
} TPacketGCMessengerList;

enum ECGMessengerSubHeader
{
	MESSENGER_SUBHEADER_CG_ADD_BY_VID,
	MESSENGER_SUBHEADER_CG_ADD_BY_NAME,
	MESSENGER_SUBHEADER_CG_REMOVE,
#if defined(__MESSENGER_BLOCK_SYSTEM__)
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

typedef struct command_messenger_add_by_vid
{
	DWORD dwVID;
} TPacketCGMessengerAddByVID;

#if defined(__MESSENGER_BLOCK_SYSTEM__)
typedef struct command_messenger_add_block_by_vid
{
	DWORD dwVID;
} TPacketCGMessengerAddBlockByVID;
#endif

///////////////////////////////////////////////////////////////////////////////////
// Party
typedef struct command_party_parameter
{
	BYTE bHeader;
	BYTE bDistributeMode;
} TPacketCGPartyParameter;

typedef struct paryt_parameter
{
	BYTE bHeader;
	BYTE bDistributeMode;
} TPacketGCPartyParameter;

typedef struct packet_party_add
{
	BYTE header;
	DWORD pid;
	char name[CHARACTER_NAME_MAX_LEN + 1];
#if defined(__WJ_SHOW_PARTY_ON_MINIMAP__)
	long mapIdx;
#endif
#if defined(__PARTY_CHANNEL_FIX__)
	BYTE channel;
#endif
} TPacketGCPartyAdd;

typedef struct command_party_invite
{
	BYTE header;
	DWORD vid;
} TPacketCGPartyInvite;

typedef struct packet_party_invite
{
	BYTE header;
	DWORD leader_vid;
} TPacketGCPartyInvite;

typedef struct command_party_invite_answer
{
	BYTE header;
	DWORD leader_vid;
	BYTE accept;
} TPacketCGPartyInviteAnswer;

typedef struct packet_party_update
{
	BYTE header;
	DWORD pid;
	BYTE role;
	BYTE percent_hp;
	short affects[7];
#if defined(__WJ_SHOW_PARTY_ON_MINIMAP__)
	long x;
	long y;
#endif
} TPacketGCPartyUpdate;

typedef struct packet_party_remove
{
	BYTE header;
	DWORD pid;
} TPacketGCPartyRemove;

typedef struct packet_party_link
{
	BYTE header;
	DWORD pid;
	DWORD vid;
#if defined(__WJ_SHOW_PARTY_ON_MINIMAP__)
	long mapIdx;
#endif
#if defined(__PARTY_CHANNEL_FIX__)
	BYTE channel;
#endif
} TPacketGCPartyLink;

typedef struct command_party_remove
{
	BYTE header;
	DWORD pid;
} TPacketCGPartyRemove;

typedef struct command_party_set_state
{
	BYTE header;
	DWORD pid;
	BYTE byRole;
	BYTE flag;
} TPacketCGPartySetState;

enum
{
	PARTY_SKILL_HEAL = 1,
	PARTY_SKILL_WARP = 2
};

typedef struct command_party_use_skill
{
	BYTE header;
	BYTE bySkillIndex;
	DWORD vid;
} TPacketCGPartyUseSkill;

typedef struct packet_safebox_size
{
	BYTE bHeader;
	BYTE bSize;
} TPacketCGSafeboxSize;

typedef struct packet_safebox_wrong_password
{
	BYTE bHeader;
} TPacketCGSafeboxWrongPassword;

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
	SAFEBOX_MONEY_STATE_SAVE,
	SAFEBOX_MONEY_STATE_WITHDRAW,
};

typedef struct command_safebox_money
{
	BYTE bHeader;
	BYTE bState;
	long lMoney;
} TPacketCGSafeboxMoney;

typedef struct packet_safebox_money_change
{
	BYTE bHeader;
	long lMoney;
} TPacketGCSafeboxMoneyChange;

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
	GUILD_SUBHEADER_GC_WAR_SCORE,
	GUILD_SUBHEADER_GC_MONEY_CHANGE,
};

enum GUILD_SUBHEADER_CG
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

typedef struct packet_guild
{
	BYTE header;
	WORD size;
	BYTE subheader;
} TPacketGCGuild;

typedef struct packet_guild_name_t
{
	BYTE header;
	WORD size;
	BYTE subheader;
	DWORD guildID;
	char guildName[GUILD_NAME_MAX_LEN];
} TPacketGCGuildName;

typedef struct packet_guild_war
{
	DWORD dwGuildSelf;
	DWORD dwGuildOpp;
	BYTE bType;
	BYTE bWarState;
} TPacketGCGuildWar;

typedef struct command_guild
{
	BYTE header;
	BYTE subheader;
} TPacketCGGuild;

typedef struct command_guild_answer_make_guild
{
	BYTE header;
	char guild_name[GUILD_NAME_MAX_LEN + 1];
} TPacketCGAnswerMakeGuild;

typedef struct command_guild_use_skill
{
	DWORD dwVnum;
	DWORD dwPID;
} TPacketCGGuildUseSkill;

// Guild Mark
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
	// 뒤에 size * (WORD + WORD)만큼 데이터 붙음
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
	DWORD guild_id;
} TPacketCGGuildSymbolUpload;

typedef struct command_symbol_crc
{
	BYTE header;
	DWORD guild_id;
	DWORD crc;
	DWORD size;
} TPacketCGSymbolCRC;

typedef struct packet_symbol_data
{
	BYTE header;
	WORD size;
	DWORD guild_id;
} TPacketGCGuildSymbolData;

// Fishing
typedef struct command_fishing
{
	BYTE header;
	BYTE dir;
} TPacketCGFishing;

typedef struct packet_fishing
{
	BYTE header;
	BYTE subheader;
	DWORD info;
	BYTE dir;
} TPacketGCFishing;

enum
{
	FISHING_SUBHEADER_GC_START,
	FISHING_SUBHEADER_GC_STOP,
	FISHING_SUBHEADER_GC_REACT,
	FISHING_SUBHEADER_GC_SUCCESS,
	FISHING_SUBHEADER_GC_FAIL,
	FISHING_SUBHEADER_GC_FISH,
};

#if defined(__FISHING_GAME__)
enum EFishingGameSubHeader
{
	FISHING_GAME_SUBHEADER_OPEN,
	FISHING_GAME_SUBHEADER_GOAL,
	FISHING_GAME_SUBHEADER_QUIT,
};

enum EFishingGameMisc
{
	FISHING_GAME_DURATION = 15,
};

typedef struct SPacketGCFishingGame
{
	SPacketGCFishingGame(const BYTE bSubHeader, const BYTE bLevel = 0) :
		bHeader(HEADER_GC_FISHING_GAME),
		bSubHeader(bSubHeader),
		bLevel(bLevel)
	{}
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

typedef struct command_give_item
{
	BYTE byHeader;
	DWORD dwTargetVID;
	TItemPos ItemPos;
	WORD wItemCount;
} TPacketCGGiveItem;

typedef struct SPacketCGHack
{
	BYTE bHeader;
	char szBuf[255 + 1];
} TPacketCGHack;

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

typedef struct packet_dungeon_dest_position
{
	long x;
	long y;
} TPacketGCDungeonDestPosition;

typedef struct SPacketGCShopSign
{
	BYTE bHeader;
	DWORD dwVID;
	char szSign[SHOP_SIGN_MAX_LEN + 1];
#if defined(__MYSHOP_DECO__)
	BYTE bType;
#endif
} TPacketGCShopSign;

typedef struct SPacketCGMyShop
{
	BYTE bHeader;
	char szSign[SHOP_SIGN_MAX_LEN + 1];
	BYTE bCount; // Count of TShopItemTable
} TPacketCGMyShop;

#if defined(__MYSHOP_DECO__)
typedef struct SPacketGCMyPrivShopOpen
{
	BYTE bHeader;
	bool bCashItem;
	BYTE bTabCount;
} TPacketGCMyPrivShopOpen;

typedef struct SPacketCGMyShopDecoState
{
	BYTE bHeader;
	BYTE bState;
} TPacketCGMyShopDecoState;

typedef struct SPacketCGMyShopDecoAdd
{
	BYTE bHeader;
	BYTE bType;
	DWORD dwPolyVnum;
} TPacketCGMyShopDecoAdd;
#endif

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

typedef struct SPacketCGRefine
{
	BYTE header;
	BYTE pos;
	BYTE type;
} TPacketCGRefine;

typedef struct SPacketCGRequestRefineInfo
{
	BYTE header;
	BYTE pos;
} TPacketCGRequestRefineInfo;

typedef struct SPacketGCRefineInformaion
{
	BYTE header;
	BYTE type;
	BYTE pos;
	DWORD src_vnum;
	DWORD result_vnum;
	WORD material_count;
	int cost; // 소요 비용
	int prob; // 확률
	TRefineMaterial materials[REFINE_MATERIAL_MAX_NUM];
#if defined(__REFINE_ELEMENT_SYSTEM__)
	TPlayerItemRefineElement RefineElement;
#endif
#if defined(__ITEM_APPLY_RANDOM__)
	TPlayerItemAttribute aApplyRandom[ITEM_APPLY_MAX_NUM];
#endif
} TPacketGCRefineInformation;

struct TNPCPosition
{
	BYTE bType;
	char name[CHARACTER_NAME_MAX_LEN + 1];
	long x;
	long y;
};

typedef struct SPacketGCNPCPosition
{
	BYTE header;
	WORD size;
	WORD count;
	// array of TNPCPosition
} TPacketGCNPCPosition;

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
	SPacketGCSpecialEffect()
		: bEffectType(SE_TYPE_NORMAL)
		, xEffectPos(0)
		, yEffectPos(0)
	{}
} TPacketGCSpecialEffect;

typedef struct SPacketCGChangeName
{
	BYTE header;
	BYTE index;
	char name[CHARACTER_NAME_MAX_LEN + 1];
} TPacketCGChangeName;

typedef struct SPacketGCChangeName
{
	BYTE header;
	DWORD pid;
	char name[CHARACTER_NAME_MAX_LEN + 1];
} TPacketGCChangeName;

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

typedef struct packet_channel
{
	BYTE header;
	BYTE channel;
} TPacketGCChannel;

typedef struct SEquipmentItemSet
{
	DWORD dwVnum;
	BYTE bCount;
	long alSockets[ITEM_SOCKET_MAX_NUM];
	TPlayerItemAttribute aAttr[ITEM_ATTRIBUTE_MAX_NUM];
#if defined(__CHANGE_LOOK_SYSTEM__)
	DWORD dwTransmutationVnum;
#endif
#if defined(__REFINE_ELEMENT_SYSTEM__)
	TPlayerItemRefineElement RefineElement;
#endif
#if defined(__ITEM_APPLY_RANDOM__)
	TPlayerItemAttribute aApplyRandom[ITEM_APPLY_MAX_NUM];
#endif
#if defined(__SET_ITEM__)
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
	char szName[32 + 1];
	DWORD dwVID;
	BYTE bType;
} TPacketGCTargetCreate;

typedef struct
{
	BYTE bHeader;
	long lID;
	long lX, lY;
#if defined(__PRIVATESHOP_SEARCH_SYSTEM__)
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
	BYTE bHeader;
	TPacketAffectElement elem;
} TPacketGCAffectAdd;

typedef struct
{
	BYTE bHeader;
	DWORD dwType;
	POINT_TYPE wApplyOn;
} TPacketGCAffectRemove;

typedef struct packet_lover_info
{
	BYTE header;
	char name[CHARACTER_NAME_MAX_LEN + 1];
	BYTE love_point;
} TPacketGCLoverInfo;

typedef struct packet_love_point_update
{
	BYTE header;
	BYTE love_point;
} TPacketGCLovePointUpdate;

// MINING
typedef struct packet_dig_motion
{
	BYTE header;
	DWORD vid;
	DWORD target_vid;
	BYTE count;
} TPacketGCDigMotion;
// END_OF_MINING

// SCRIPT_SELECT_ITEM
typedef struct command_script_select_item
{
	BYTE header;
	DWORD selection;
} TPacketCGScriptSelectItem;
// END_OF_SCRIPT_SELECT_ITEM

#if defined(__GEM_SHOP__)
typedef struct command_select_item_ex
{
	BYTE bHeader;
	DWORD dwItemPos;
	BYTE bType;
} TPacketCGSelectItemEx;
#endif

typedef struct packet_damage_info
{
	BYTE header;
	DWORD dwVID;
	BYTE flag;
	int damage;
} TPacketGCDamageInfo;

typedef struct tag_GGSiege
{
	BYTE bHeader;
	BYTE bEmpire;
	BYTE bTowerCount;
} TPacketGGSiege;

typedef struct SPacketGGMonarchTransfer
{
	BYTE bHeader;
	DWORD dwTargetPID;
	long x;
	long y;
} TPacketMonarchGGTransfer;

typedef struct SPacketGGPCBangUpdate
{
	BYTE bHeader;
	unsigned long ulPCBangID;
} TPacketPCBangUpdate;

typedef struct SPacketGGCheckAwakeness
{
	BYTE bHeader;
} TPacketGGCheckAwakeness;

typedef struct SPacketGCPanamaPack
{
	BYTE bHeader;
	char szPackName[256];
	BYTE abIV[32];
} TPacketGCPanamaPack;

// TODO : 아우 짱나..가변패킷 사이즈 받아들일수 있게 패킷 핸들러 Refactoring 하자. 
typedef struct SPacketGCHybridCryptKeys
{
	SPacketGCHybridCryptKeys() : m_pStream(NULL) {}
	~SPacketGCHybridCryptKeys()
	{
		// GCC 에선 NULL delete 해도 괜찮나? 일단 안전하게 NULL 체크 하자. ( 근데 이거 C++ 표준아니었나 --a )
		if (m_pStream)
		{
			delete[] m_pStream;
			m_pStream = NULL;
		}
	}

	DWORD GetStreamSize()
	{
		return sizeof(bHeader) + sizeof(WORD) + sizeof(int) + KeyStreamLen;
	}

	BYTE* GetStreamData()
	{
		if (m_pStream)
			delete[] m_pStream;

		uDynamicPacketSize = (WORD)GetStreamSize();

		m_pStream = new BYTE[uDynamicPacketSize];

		memcpy(m_pStream, &bHeader, 1);
		memcpy(m_pStream + 1, &uDynamicPacketSize, 2);
		memcpy(m_pStream + 3, &KeyStreamLen, 4);

		if (KeyStreamLen > 0)
			memcpy(m_pStream + 7, pDataKeyStream, KeyStreamLen);

		return m_pStream;
	}

	BYTE bHeader;
	WORD uDynamicPacketSize; // 빌어먹을 클라 DynamicPacketHeader 구조때문에 맞춰줘야한다 -_-;
	int KeyStreamLen;
	BYTE* pDataKeyStream;

private:
	BYTE* m_pStream;

} TPacketGCHybridCryptKeys;

typedef struct SPacketGCPackageSDB
{
	SPacketGCPackageSDB() : m_pDataSDBStream(NULL), m_pStream(NULL) {}
	~SPacketGCPackageSDB()
	{
		if (m_pStream)
		{
			delete[] m_pStream;
			m_pStream = NULL;
		}
	}

	DWORD GetStreamSize()
	{
		return sizeof(bHeader) + sizeof(WORD) + sizeof(int) + iStreamLen;
	}

	BYTE* GetStreamData()
	{
		if (m_pStream)
			delete[] m_pStream;

		uDynamicPacketSize = static_cast<WORD>(GetStreamSize());

		m_pStream = new BYTE[uDynamicPacketSize];

		memcpy(m_pStream, &bHeader, 1);
		memcpy(m_pStream + 1, &uDynamicPacketSize, 2);
		memcpy(m_pStream + 3, &iStreamLen, 4);

		if (iStreamLen > 0)
			memcpy(m_pStream + 7, m_pDataSDBStream, iStreamLen);

		return m_pStream;
	}

	BYTE bHeader;
	WORD uDynamicPacketSize; // 빌어먹을 클라 DynamicPacketHeader 구조때문에 맞춰줘야한다 -_-;
	int iStreamLen;
	BYTE* m_pDataSDBStream;

private:
	BYTE* m_pStream;

} TPacketGCPackageSDB;

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

#define MAX_EFFECT_FILE_NAME 128
typedef struct SPacketGCSpecificEffect
{
	BYTE header;
	DWORD vid;
	char effect_file[MAX_EFFECT_FILE_NAME];
} TPacketGCSpecificEffect;

#if defined(__DRAGON_SOUL_SYSTEM__)
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
	DS_SUB_HEADER_DO_REFINE_GRADE,
	DS_SUB_HEADER_DO_REFINE_STEP,
	DS_SUB_HEADER_DO_REFINE_STRENGTH,
	DS_SUB_HEADER_REFINE_FAIL,
	DS_SUB_HEADER_REFINE_FAIL_MAX_REFINE,
	DS_SUB_HEADER_REFINE_FAIL_INVALID_MATERIAL,
	DS_SUB_HEADER_REFINE_FAIL_NOT_ENOUGH_MONEY,
	DS_SUB_HEADER_REFINE_FAIL_NOT_ENOUGH_MATERIAL,
	DS_SUB_HEADER_REFINE_FAIL_TOO_MUCH_MATERIAL,
	DS_SUB_HEADER_REFINE_SUCCEED,
#if defined(__DS_CHANGE_ATTR__)
	DS_SUB_HEADER_OPEN_CHANGE_ATTR,
	DS_SUB_HEADER_DO_CHANGE_ATTR,
#endif
};

typedef struct SPacketCGDragonSoulRefine
{
	SPacketCGDragonSoulRefine() : header(HEADER_CG_DRAGON_SOUL_REFINE)
	{}
	BYTE header;
	BYTE bSubType;
	TItemPos ItemGrid[DRAGON_SOUL_REFINE_GRID_SIZE];
} TPacketCGDragonSoulRefine;

typedef struct SPacketGCDragonSoulRefine
{
	SPacketGCDragonSoulRefine() : header(HEADER_GC_DRAGON_SOUL_REFINE)
	{}
	BYTE header;
	BYTE bSubType;
	TItemPos Pos;
} TPacketGCDragonSoulRefine;
#endif

typedef struct SPacketCGStateCheck
{
	BYTE header;
	unsigned long key;
	unsigned long index;
} TPacketCGStateCheck;

typedef struct SPacketGCStateCheck
{
	BYTE header;
	unsigned long key;
	unsigned long index;
	unsigned char state;
} TPacketGCStateCheck;

#if defined(__ACCE_COSTUME_SYSTEM__)
enum ESubHeaderGCAcceRefine
{
	ACCE_REFINE_SUBHEADER_GC_CLOSE,
	ACCE_REFINE_SUBHEADER_GC_OPEN,
	ACCE_REFINE_SUBHEADER_GC_SET_ITEM,
	ACCE_REFINE_SUBHEADER_GC_CLEAR_SLOT,
	ACCE_REFINE_SUBHEADER_GC_CLEAR_ALL,
	ACCE_REFINE_SUBHEADER_GC_CLEAR_RIGHT
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
	ACCE_REFINE_SUBHEADER_CG_CANCEL
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
	SPacketGCAcceRefine() : bHeader(HEADER_GC_ACCE_REFINE) {}
	BYTE bHeader;
	WORD wSize;
	BYTE bSubHeader;
} TPacketGCAcceRefine;

typedef struct SPacketCGAcceRefine
{
	BYTE bHeader;
	WORD wSize;
	BYTE bSubHeader;
} TPacketCGAcceRefine;
#endif

#if defined(__AURA_COSTUME_SYSTEM__)
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
	SPacketGCAuraRefine() : bHeader(HEADER_GC_AURA_REFINE) {}
	BYTE bHeader;
	WORD wSize;
	BYTE bSubHeader;
} TPacketGCAuraRefine;

typedef struct SPacketCGAuraRefine
{
	BYTE bHeader;
	WORD wSize;
	BYTE bSubHeader;
} TPacketCGAuraRefine;
#endif

#if defined(__MINI_GAME_CATCH_KING__)
enum EMiniGameCatchKingCGSubHeader
{
	CATCHKING_CG_START,
	CATCHKING_CG_CLICK_HAND,
	CATCHKING_CG_CLICK_CARD,
	CATCHKING_CG_REWARD,
#if defined(__CATCH_KING_EVENT_FLAG_RENEWAL__)
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
#if defined(__CATCH_KING_EVENT_FLAG_RENEWAL__)
	CATCHKING_GC_SET_CARD_PIECE_FLAG,
	CATCHKING_GC_SET_CARD_FLAG,
	CATCHKING_GC_SET_QUEST_FLAG,
	CATCHKING_GC_NO_MORE_GAIN,
#endif
};

typedef struct SCatchKingCard
{
	SCatchKingCard() { bIndex = 0; bIsExposed = false; }
	SCatchKingCard(BYTE index, bool isExposed)
	{
		bIndex = index;
		bIsExposed = isExposed;
	}

	BYTE bIndex;
	bool bIsExposed;
} TCatchKingCard;

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

#if defined(__CATCH_KING_EVENT_FLAG_RENEWAL__)
typedef struct SPacketGCMiniGameCatchKingQuestFlag
{
	WORD wPieceCount, wPackCount;
	SPacketGCMiniGameCatchKingQuestFlag(WORD wPieceCount, WORD wPackCount) :
		wPieceCount(wPieceCount), wPackCount(wPackCount) {}
} TPacketGCMiniGameCatchKingQuestFlag;
#endif
#endif

#if defined(__PRIVATESHOP_SEARCH_SYSTEM__)
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
	char szItemName[ITEM_NAME_MAX_LEN + 1];
#if defined(__CHEQUE_SYSTEM__)
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
	BYTE bPos;
	DWORD dwShopPID;
	DWORD dwShopVID;
} TPacketCGPrivateShopSearchBuyItem;
#endif

#if defined(__SKILLBOOK_COMB_SYSTEM__)
typedef struct SPacketCGSkillBookCombination
{
	BYTE bHeader;
	BYTE bAction;
	TItemPos CombItemGrid[SKILLBOOK_COMB_SLOT_MAX];
} TPacketCGSkillBookCombination;
#endif

#if defined(__CHANGE_LOOK_SYSTEM__)
enum class EPacketCGChangeLookSubHeader : BYTE
{
	ITEM_CHECK_IN,
	ITEM_CHECK_OUT,
	FREE_ITEM_CHECK_IN,
	FREE_ITEM_CHECK_OUT,
	ACCEPT,
	CANCEL
};

typedef struct packet_set_changelook
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
	BYTE bHeader;
	BYTE bSubHeader;
	BYTE bSlotIndex;
	TItemPos ItemPos;
} TPacketCGChangeLook;
#endif

#if defined(__MAILBOX__)
typedef struct packet_mailbox_process
{
	BYTE bHeader;
	BYTE bSubHeader;
	BYTE bArg1;
	BYTE bArg2;
} TPacketMailboxProcess;

typedef struct packet_mailbox_process_all
{
	BYTE Index;
} TPacketGCMailboxProcessAll;

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

#if defined(__RANKING_SYSTEM__)
typedef struct SPacketGCRanking
{
	BYTE bHeader;
	WORD wSize;
	BYTE bType;
	BYTE bCategory;
} TPacketGCRanking;
#endif

#if defined(__MOVE_COSTUME_ATTR__)
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

#if defined(__CHANGED_ATTR__)
typedef struct packet_gc_item_select_attr
{
	BYTE bHeader;
	TItemPos pItemPos;
	TPlayerItemAttribute aAttr[ITEM_ATTRIBUTE_MAX_NUM];
} TPacketGCItemSelectAttr;

typedef struct packet_cg_item_select_attr
{
	BYTE bHeader;
	bool bNew;
	TItemPos pItemPos;
} TPacketCGItemSelectAttr;
#endif

#if defined(__LOOT_FILTER_SYSTEM__)
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

#if defined(__MINI_GAME_RUMI__)
enum EMiniGameRumiCGSubHeader
{
	RUMI_CG_SUBHEADER_END,
	RUMI_CG_SUBHEADER_START,
	RUMI_CG_SUBHEADER_DECK_CARD_CLICK,
	RUMI_CG_SUBHEADER_HAND_CARD_CLICK,
	RUMI_CG_SUBHEADER_FIELD_CARD_CLICK,
#if defined(__OKEY_EVENT_FLAG_RENEWAL__)
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
#if defined(__OKEY_EVENT_FLAG_RENEWAL__)
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
} TPacketCGMiniGameRumi;

typedef struct SPacketGCMiniGameRumiSetDeck
{
	BYTE bDeckCount;
	SPacketGCMiniGameRumiSetDeck(BYTE bDeckCount) : bDeckCount(bDeckCount) {}
} TPacketGCMiniGameRumiSetDeck;

typedef struct SPacketGCMiniGameRumiMoveCard
{
	BYTE bSrcPos, bSrcIndex, bSrcColor, bSrcNumber;
	BYTE bDstPos, bDstIndex, bDstColor, bDstNumber;
	SPacketGCMiniGameRumiMoveCard() :
		bSrcPos(0), bSrcIndex(0), bSrcColor(0), bSrcNumber(0),
		bDstPos(0), bDstIndex(0), bDstColor(0), bDstNumber(0) {}
} TPacketGCMiniGameRumiMoveCard;

typedef struct SPacketGCMiniGameRumiSetScore
{
	WORD wScore, wTotalScore;
	SPacketGCMiniGameRumiSetScore(WORD wScore, WORD wTotalScore) :
		wScore(wScore), wTotalScore(wTotalScore) {}
} TPacketGCMiniGameRumiSetScore;

#if defined(__OKEY_EVENT_FLAG_RENEWAL__)
typedef struct SPacketGCMiniGameRumiQuestFlag
{
	WORD wCardPieceCount, wCardCount;
	SPacketGCMiniGameRumiQuestFlag(WORD wCardPieceCount, WORD wCardCount) :
		wCardPieceCount(wCardPieceCount), wCardCount(wCardCount) {}
} TPacketGCMiniGameRumiQuestFlag;
#endif

typedef struct SPacketGCMiniGameRumi
{
	BYTE bHeader;
	WORD wSize;
	BYTE bSubHeader;
} TPacketGCMiniGameRumi;
#endif

#if defined(__LUCKY_BOX__)
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

#if defined(__ATTR_6TH_7TH__)
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

#if defined(__GEM_SHOP__)
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
	SPacketGCGemShopProcess(const BYTE c_bSubHeader) :
		bHeader(HEADER_GC_GEM_SHOP_PROCESS),
		bSubHeader(c_bSubHeader),
		bSlotIndex(GEM_SHOP_SLOT_COUNT),
		bEnable(false)
	{}
	BYTE bHeader;
	BYTE bSubHeader;
	BYTE bSlotIndex;
	bool bEnable;
} TPacketGCGemShopProcess;

typedef struct command_gem_shop
{
	BYTE bHeader;
	BYTE bSubHeader;
	BYTE bSlotIndex;
} TPacketCGGemShop;
#endif

#if defined(__EXTEND_INVEN_SYSTEM__)
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

#if defined(__CLIENT_TIMER__)
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
	SPacketGCClientTimer(const BYTE _bSubHeader)
		: bHeader(HEADER_GC_CLIENT_TIMER), bSubHeader(_bSubHeader)
	{
		memset(&dwData, 0, sizeof(dwData));
	};
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
	SPacketGCEmote() : bHeader(HEADER_GC_EMOTE) {}
	BYTE bHeader;
	BYTE bSubHeader;
	DWORD dwEmoteVnum;
	DWORD dwDuration;
	DWORD dwMainVID, dwTargetVID;
} TPacketGCEmote;

#if defined(__CUBE_RENEWAL__)
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

#if defined(__SNOWFLAKE_STICK_EVENT__)
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
	SPacketGCSnowflakeStickEvent(const BYTE c_bSubHeader)
		: bHeader(HEADER_GC_SNOWFLAKE_STICK_EVENT), bSubHeader(c_bSubHeader)
	{;
		memset(&dwValue, 0, sizeof(dwValue));
	}
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
} TPacketCGSnowflakeStickEvent;
#endif

#if defined(__REFINE_ELEMENT_SYSTEM__)
enum EPacketGCRefineElement
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
} TPacketCGRefineElement;
#endif

#if defined(__MINI_GAME_YUTNORI__)
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
#if defined(__YUTNORI_EVENT_FLAG_RENEWAL__)
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
#if defined(__YUTNORI_EVENT_FLAG_RENEWAL__)
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
	SPacketGCMiniGameYutnoriSetProb(BYTE bProbIndex)
		: bProbIndex(bProbIndex) {}
} TPacketGCMiniGameYutnoriSetProb;

typedef struct SPacketGCMiniGameYutnoriThrow
{
	bool bPC;
	BYTE bYut;
	SPacketGCMiniGameYutnoriThrow(bool bPC, BYTE bYut)
		: bPC(bPC), bYut(bYut) {}
} TPacketGCMiniGameYutnoriThrow;

typedef struct SPacketGCMiniGameYutnoriMove
{
	bool bPC;
	BYTE bUnitIndex;
	bool bIsCatch;
	BYTE bStartIndex;
	BYTE bDestIndex;
	SPacketGCMiniGameYutnoriMove(bool bPC, BYTE bUnitIndex, bool bIsCatch, BYTE bStartIndex, BYTE bDestIndex)
		: bPC(bPC), bUnitIndex(bUnitIndex), bIsCatch(bIsCatch), bStartIndex(bStartIndex), bDestIndex(bDestIndex) {}
} TPacketGCMiniGameYutnoriMove;

typedef struct SPacketGCMiniGameYutnoriAvailableArea
{
	BYTE bPlayerIndex;
	BYTE bAvailableIndex;
	SPacketGCMiniGameYutnoriAvailableArea(BYTE bPlayerIndex, BYTE bAvailableIndex)
		: bPlayerIndex(bPlayerIndex), bAvailableIndex(bAvailableIndex) {}
} TPacketGCMiniGameYutnoriAvailableArea;

typedef struct SPacketGCMiniGameYutnoriPushCatchYut
{
	bool bPC;
	BYTE bUnitIndex;
	SPacketGCMiniGameYutnoriPushCatchYut(bool bPC, BYTE bUnitIndex)
		: bPC(bPC), bUnitIndex(bUnitIndex) {}
} TPacketGCMiniGameYutnoriPushCatchYut;

typedef struct SPacketGCMiniGameYutnoriSetScore
{
	WORD wScore;
	SPacketGCMiniGameYutnoriSetScore(WORD wScore)
		: wScore(wScore) {}
} TPacketGCMiniGameYutnoriSetScore;

typedef struct SPacketGCMiniGameYutnoriSetRemainCount
{
	BYTE bRemainCount;
	SPacketGCMiniGameYutnoriSetRemainCount(BYTE bRemainCount)
		: bRemainCount(bRemainCount) {}
} TPacketGCMiniGameYutnoriSetRemainCount;

typedef struct SPacketGCMiniGameYutnoriPushNextTurn
{
	bool bPC;
	BYTE bState;
	SPacketGCMiniGameYutnoriPushNextTurn(bool bPC, BYTE bState)
		: bPC(bPC), bState(bState) {}
} TPacketGCMiniGameYutnoriPushNextTurn;

#if defined(__YUTNORI_EVENT_FLAG_RENEWAL__)
typedef struct SPacketGCMiniGameYutnoriQuestFlag
{
	WORD wYutPieceCount;
	WORD wYutBoardCount;
	SPacketGCMiniGameYutnoriQuestFlag(WORD wYutPieceCount, WORD wYutBoardCount)
		: wYutPieceCount(wYutPieceCount), wYutBoardCount(wYutBoardCount) {}
} TPacketGCMiniGameYutnoriQuestFlag;
#endif
#endif

#if defined(__QUEST_REQUEST_EVENT__)
typedef struct SPacketCGRequestEventQuest
{
	BYTE bHeader;
	char szEventQuest[64 + 1];
} TPacketCGRequestEventQuest;
#endif

#if defined(__LEFT_SEAT__)
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

#if defined(__GUILD_DRAGONLAIR_SYSTEM__)
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
#if defined(__GUILD_DRAGONLAIR_PARTY_SYSTEM__)
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
#if defined(__GUILD_DRAGONLAIR_PARTY_SYSTEM__)
	DWORD dwFirstRankingTime;
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

#if defined(__SUMMER_EVENT_ROULETTE__)
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

#if defined(__FLOWER_EVENT__)
enum EPacketGCFlowerEvent
{
	FLOWER_EVENT_SUBHEADER_GC_INFO_ALL,
	FLOWER_EVENT_SUBHEADER_GC_GET_INFO,
	FLOWER_EVENT_SUBHEADER_GC_UPDATE_INFO,
};

typedef struct SPacketGCFlowerEvent
{
	BYTE bHeader;
	BYTE bSubHeader;
	BYTE bChatType;
	BYTE bShootType;
	int aiShootCount[SHOOT_TYPE_MAX + 1];
	SPacketGCFlowerEvent()
		: bHeader(HEADER_GC_FLOWER_EVENT)
		, bSubHeader(FLOWER_EVENT_SUBHEADER_GC_INFO_ALL)
		, bChatType(FLOWER_EVENT_CHAT_TYPE_MAX)
		, bShootType(SHOOT_TYPE_MAX)
	{
		memset(&aiShootCount, 0, sizeof(aiShootCount));
	}
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
}TPacketCGFlowerEvent;
#endif

#pragma pack()
#endif // __INC_PACKET_H__
