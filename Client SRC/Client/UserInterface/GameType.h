#pragma once

#include "../GameLib/ItemData.h"
#include "../GameLib//ItemUtil.h"

struct SAffects
{
	enum
	{
		AFFECT_MAX_NUM = 32,
	};

	SAffects() : dwAffects(0) {}
	SAffects(const DWORD& c_rAffects)
	{
		__SetAffects(c_rAffects);
	}
	int operator = (const DWORD& c_rAffects)
	{
		__SetAffects(c_rAffects);
	}

	BOOL IsAffect(BYTE byIndex)
	{
		return dwAffects & (1 << byIndex);
	}

	void __SetAffects(const DWORD& c_rAffects)
	{
		dwAffects = c_rAffects;
	}

	DWORD dwAffects;

};

extern std::string g_strGuildSymbolPathName;

const DWORD c_Name_Max_Length = 64;
const DWORD c_FileName_Max_Length = 128;
const DWORD c_Short_Name_Max_Length = 32;

// Inventory
const DWORD c_Inventory_Page_Width = 5;
const DWORD c_Inventory_Page_Height = 9;
const DWORD c_Inventory_Page_Size = c_Inventory_Page_Width * c_Inventory_Page_Height;
#if defined(ENABLE_EXTEND_INVEN_SYSTEM)
const DWORD c_Inventory_Page_Count = 4;
#else
const DWORD c_Inventory_Page_Count = 2;
#endif
const DWORD c_Inventory_Slot_Count = c_Inventory_Page_Size * c_Inventory_Page_Count;
const DWORD c_ItemSlot_Count = c_Inventory_Slot_Count;

const DWORD c_Wear_Max = CItemData::WEAR_MAX_NUM;

// Equipment
const DWORD c_Equipment_Slot_Start = 0;
const DWORD c_Equipment_Slot_Body = c_Equipment_Slot_Start + CItemData::WEAR_BODY;
const DWORD c_Equipment_Slot_Head = c_Equipment_Slot_Start + CItemData::WEAR_HEAD;
const DWORD c_Equipment_Slot_Foots = c_Equipment_Slot_Start + CItemData::WEAR_FOOTS;
const DWORD c_Equipment_Slot_Wrist = c_Equipment_Slot_Start + CItemData::WEAR_WRIST;
const DWORD c_Equipment_Slot_Weapon = c_Equipment_Slot_Start + CItemData::WEAR_WEAPON;
const DWORD c_Equipment_Slot_Neck = c_Equipment_Slot_Start + CItemData::WEAR_NECK;
const DWORD c_Equipment_Slot_Ear = c_Equipment_Slot_Start + CItemData::WEAR_EAR;
const DWORD c_Equipment_Slot_Arrow = c_Equipment_Slot_Start + CItemData::WEAR_ARROW;
const DWORD c_Equipment_Slot_Shield = c_Equipment_Slot_Start + CItemData::WEAR_SHIELD;
const DWORD c_Equipment_Slot_Belt = c_Equipment_Slot_Start + CItemData::WEAR_BELT;
#if defined(ENABLE_PENDANT)
const DWORD c_Equipment_Slot_Pendant = c_Equipment_Slot_Start + CItemData::WEAR_PENDANT;
#endif
#if defined(ENABLE_GLOVE_SYSTEM)
const DWORD c_Equipment_Slot_Glove = c_Equipment_Slot_Start + CItemData::WEAR_GLOVE;
#endif
const DWORD c_Equipment_Slot_Unique1 = c_Equipment_Slot_Start + CItemData::WEAR_UNIQUE1;
const DWORD c_Equipment_Slot_Unique2 = c_Equipment_Slot_Start + CItemData::WEAR_UNIQUE2;

// Costume
const DWORD c_Costume_Slot_Start = c_Equipment_Slot_Start + CItemData::WEAR_COSTUME_BODY;
const DWORD c_Costume_Slot_Body = c_Costume_Slot_Start + CItemData::COSTUME_BODY;
const DWORD c_Costume_Slot_Hair = c_Costume_Slot_Start + CItemData::COSTUME_HAIR;
#if defined(ENABLE_MOUNT_COSTUME_SYSTEM)
const DWORD c_Costume_Slot_Mount = c_Costume_Slot_Start + CItemData::COSTUME_MOUNT;
#endif
#if defined(ENABLE_ACCE_COSTUME_SYSTEM)
const DWORD c_Costume_Slot_Acce = c_Costume_Slot_Start + CItemData::COSTUME_ACCE;
#endif
#if defined(ENABLE_WEAPON_COSTUME_SYSTEM)
const DWORD c_Costume_Slot_Weapon = c_Costume_Slot_Start + CItemData::COSTUME_WEAPON;
#endif
#if defined(ENABLE_AURA_COSTUME_SYSTEM)
const DWORD c_Costume_Slot_Aura = c_Costume_Slot_Start + CItemData::COSTUME_AURA;
#endif
const DWORD c_Costume_Slot_Count = CItemData::COSTUME_NUM_TYPES;
const DWORD c_Costume_Slot_End = c_Costume_Slot_Start + c_Costume_Slot_Count;

// Belt Inventory
const DWORD c_Belt_Inventory_Slot_Start = 0;
const DWORD c_Belt_Inventory_Width = 4;
const DWORD c_Belt_Inventory_Height = 4;
const DWORD c_Belt_Inventory_Slot_Count = c_Belt_Inventory_Width * c_Belt_Inventory_Height;
const DWORD c_Belt_Inventory_Slot_End = c_Belt_Inventory_Slot_Start + c_Belt_Inventory_Slot_Count;

#if defined(ENABLE_DRAGON_SOUL_SYSTEM)
// Dragon Soul Equipment
const DWORD c_DragonSoul_Equip_Start = c_Wear_Max;
const DWORD c_DragonSoul_Equip_End = c_DragonSoul_Equip_Start + CItemData::DS_SLOT_NUM_TYPES * DS_DECK_MAX_NUM;
#endif

#if defined(ENABLE_DRAGON_SOUL_SYSTEM)
// WEAR_MAX + Dragon Soul Equipment + Decks
const DWORD c_Equipment_Slot_Count = c_Wear_Max + CItemData::DS_SLOT_NUM_TYPES * DS_DECK_MAX_NUM;
#else
// WEAR_MAX
const DWORD c_Equipment_Slot_Count = c_Wear_Max;
#endif

#if defined(ENABLE_DRAGON_SOUL_SYSTEM)
// Dragon Soul Inventory
const DWORD c_DragonSoul_Inventory_Start = 0;
const DWORD c_DragonSoul_Inventory_Box_Size = 32;
const DWORD c_DragonSoul_Inventory_Count = CItemData::DS_SLOT_NUM_TYPES * DRAGON_SOUL_GRADE_MAX * c_DragonSoul_Inventory_Box_Size;
const DWORD c_DragonSoul_Inventory_End = c_DragonSoul_Inventory_Start + c_DragonSoul_Inventory_Count;
#endif

enum ECurrencyLimit
{
	GOLD_MAX = 2000000000,
#if defined(ENABLE_CHEQUE_SYSTEM)
	CHEQUE_MAX = 3000,
#endif
#if defined(ENABLE_GEM_SYSTEM)
	GEM_MAX = 1000000,
#endif
};

#if defined(ENABLE_CONQUEROR_LEVEL)
enum ELevelTypes
{
	LEVEL_TYPE_BASE,
	LEVEL_TYPE_CONQUEROR,
	LEVEL_TYPE_MAX
};
#endif

#if defined(WJ_ENABLE_TRADABLE_ICON)
enum ETopWindowTypes
{
	ON_TOP_WND_NONE,					// 0
	ON_TOP_WND_SHOP,					// 1
	ON_TOP_WND_EXCHANGE,				// 1
	ON_TOP_WND_SAFEBOX,					// 3
	ON_TOP_WND_PRIVATE_SHOP,			// 4
#if defined(ENABLE_MOVE_COSTUME_ATTR)
	ON_TOP_WND_ITEM_COMB,				// 5
#endif
//#if defined(ENABLE_GROWTH_PET_SYSTEM)
	ON_TOP_WND_PET_FEED,				// 6
//#endif
	ON_TOP_WND_FISH_EVENT,				// 7
	ON_TOP_WND_MAILBOX,					// 8
//#if defined(ENABLE_GROWTH_PET_SYSTEM) && defined(ENABLE_PET_ATTR_DETERMINE)
	ON_TOP_WND_PET_ATTR_CHANGE,			// 9
//#endif
#if defined(ENABLE_LUCKY_BOX)
	ON_TOP_WND_LUCKY_BOX,				// 10
#endif
	ON_TOP_WND_ATTR67,					// 11
//#if defined(ENABLE_GROWTH_PET_SYSTEM) && defined(ENABLE_PET_PRIMIUM_FEEDSTUFF)
	ON_TOP_WND_PET_PRIMIUM_FEEDSTUFF,	// 12
//#endif
	ON_TOP_WND_PASSIVE_ATTR,			// 13
	ON_TOP_WND_MAX,
};
#endif

#if defined(ENABLE_KEYCHANGE_SYSTEM)
enum EKeySettings
{
	KEY_NONE,
	KEY_MOVE_UP_1,
	KEY_MOVE_DOWN_1,
	KEY_MOVE_LEFT_1,
	KEY_MOVE_RIGHT_1,
	KEY_MOVE_UP_2,
	KEY_MOVE_DOWN_2,
	KEY_MOVE_LEFT_2,
	KEY_MOVE_RIGHT_2,
	KEY_CAMERA_ROTATE_POSITIVE_1,
	KEY_CAMERA_ROTATE_NEGATIVE_1,
	KEY_CAMERA_ZOOM_POSITIVE_1,
	KEY_CAMERA_ZOOM_NEGATIVE_1,
	KEY_CAMERA_PITCH_POSITIVE_1,
	KEY_CAMERA_PITCH_NEGATIVE_1,
	KEY_CAMERA_ROTATE_POSITIVE_2,
	KEY_CAMERA_ROTATE_NEGATIVE_2,
	KEY_CAMERA_ZOOM_POSITIVE_2,
	KEY_CAMERA_ZOOM_NEGATIVE_2,
	KEY_CAMERA_PITCH_POSITIVE_2,
	KEY_CAMERA_PITCH_NEGATIVE_2,
	KEY_ROOTING_1,
	KEY_ROOTING_2,
	KEY_ATTACK,
	KEY_RIDEMYHORS,
	KEY_FEEDMYHORS,
	KEY_BYEMYHORS,
	KEY_RIDEHORS,
	KEY_EMOTION1,
	KEY_EMOTION2,
	KEY_EMOTION3,
	KEY_EMOTION4,
	KEY_EMOTION5,
	KEY_EMOTION6,
	KEY_EMOTION7,
	KEY_EMOTION8,
	KEY_EMOTION9,
	KEY_SLOT_1,
	KEY_SLOT_2,
	KEY_SLOT_3,
	KEY_SLOT_4,
	KEY_SLOT_5,
	KEY_SLOT_6,
	KEY_SLOT_7,
	KEY_SLOT_8,
	KEY_SLOT_CHANGE_1,
	KEY_SLOT_CHANGE_2,
	KEY_SLOT_CHANGE_3,
	KEY_SLOT_CHANGE_4,
	KEY_OPEN_STATE,
	KEY_OPEN_SKILL,
	KEY_OPEN_QUEST,
	KEY_OPEN_INVENTORY,
	KEY_OPEN_DDS,
	KEY_OPEN_MINIMAP,
	KEY_OPEN_LOGCHAT,
	KEY_OPEN_PET,
	KEY_OPEN_GUILD,
	KEY_OPEN_MESSENGER,
	KEY_OPEN_HELP,
	KEY_OPEN_ACTION,
	KEY_SCROLL_ONOFF,
	KEY_PLUS_MINIMAP,
	KEY_MIN_MINIMAP,
	KEY_SCREENSHOT,
	KEY_SHOW_NAME,
	KEY_OPEN_AUTO,
	KEY_AUTO_RUN,
	KEY_NEXT_TARGET,
	KEY_MONSTER_CARD,
	KEY_PARTY_MATCH,
	KEY_SELECT_DSS_1,
	KEY_SELECT_DSS_2,
	KEY_OPEN_IN_GAME_EVENT,
	KEY_PASSIVE_ATTR1,
	KEY_PASSIVE_ATTR2,

	KEY_ADDKEYBUFFERCONTROL = 100,
	KEY_ADDKEYBUFFERALT = 300,
	KEY_ADDKEYBUFFERSHIFT = 500,
};
#endif

#if defined(ENABLE_EXPRESSING_EMOTION)
enum ESpecialActionMisc
{
	SPECIAL_ACTION_START_INDEX = 101,
	SPECIAL_ACTION_SLOT_COUNT = 12,
};
#endif

enum EEmotion
{
	EMOTION_NONE,
	EMOTION_CLAP,
	EMOTION_CONGRATULATION,
	EMOTION_FORGIVE,
	EMOTION_ANGRY,
	EMOTION_ATTRACTIVE,
	EMOTION_SAD,
	EMOTION_SHY,
	EMOTION_CHEERUP,
	EMOTION_BANTER,
	EMOTION_JOY,
	EMOTION_CHEERS_1,
	EMOTION_CHEERS_2,
	EMOTION_DANCE_1,
	EMOTION_DANCE_2,
	EMOTION_DANCE_3,
	EMOTION_DANCE_4,
	EMOTION_DANCE_5,
	EMOTION_DANCE_6, // 강남스타일

	EMOTION_KISS = 51,
	EMOTION_FRENCH_KISS = 52,
	EMOTION_SLAP = 53,

#if defined(ENABLE_EXPRESSING_EMOTION)
	EMOTION_PUSH_UP = SPECIAL_ACTION_START_INDEX,
	EMOTION_DANCE_7,
	EMOTION_EXERCISE,
	EMOTION_DOZE,
	EMOTION_SELFIE,
	EMOTION_CHARGING,
	EMOTION_NOSAY,
	EMOTION_WEATHER_1,
	EMOTION_WEATHER_2,
	EMOTION_WEATHER_3,
	EMOTION_HUNGRY,
	EMOTION_SIREN,
	EMOTION_LETTER,
	EMOTION_CALL,
	EMOTION_CELEBRATION,
	EMOTION_ALCOHOL,
	EMOTION_BUSY,
	EMOTION_WHIRL,
#endif

#if defined(ENABLE_FISHING_GAME)
	EMOTION_FISHING,
#endif

	EMOTION_NUM
};

#if defined(ENABLE_SKILLBOOK_COMB_SYSTEM)
const DWORD c_SkillBook_Comb_Size = 10;
enum ESkillBookComb
{
	SKILLBOOK_COMB_SLOT_MAX = c_SkillBook_Comb_Size
};
#endif

#if defined(ENABLE_LOOTING_SYSTEM)
enum ELootFilter
{
	WEAPON_SELECT_DATA_MAX = 5,
	ARMOR_SELECT_DATA_MAX = 5,
	HEAD_SELECT_DATA_MAX = 5,
	COMMON_SELECT_DATA_MAX = 10,
	COSTUME_SELECT_DATA_MAX = 6,
	DS_SELECT_DATA_MAX = 2,
	UNIQUE_SELECT_DATA_MAX = 2,
	REFINE_SELECT_DATA_MAX = 3,
	POTION_SELECT_DATA_MAX = 3,
	FISH_MINING_SELECT_DATA_MAX = 3,
	MOUNT_PET_SELECT_DATA_MAX = 4,
	SKILL_BOOK_SELECT_DATA_MAX = 6,
	ETC_SELECT_DATA_MAX = 8,
	EVENT_SELECT_DATA_MAX = 0, // NOT USING

	WEAPON_ON_OFF = 0,
	WEAPON_REFINE_MIN,
	WEAPON_REFINE_MAX,
	WEAPON_WEARING_LEVEL_MIN,
	WEAPON_WEARING_LEVEL_MAX,
	WEAPON_SELECT_DATA_JOB_WARRIOR,
	WEAPON_SELECT_DATA_JOB_SURA,
	WEAPON_SELECT_DATA_JOB_ASSASSIN,
	WEAPON_SELECT_DATA_JOB_SHAMAN,
	WEAPON_SELECT_DATA_JOB_LYCAN,

	ARMOR_ON_OFF,
	ARMOR_REFINE_MIN,
	ARMOR_REFINE_MAX,
	ARMOR_WEARING_LEVEL_MIN,
	ARMOR_WEARING_LEVEL_MAX,
	ARMOR_SELECT_DATA_JOB_WARRIOR,
	ARMOR_SELECT_DATA_JOB_SURA,
	ARMOR_SELECT_DATA_JOB_ASSASSIN,
	ARMOR_SELECT_DATA_JOB_SHAMAN,
	ARMOR_SELECT_DATA_JOB_LYCAN,

	HEAD_ON_OFF,
	HEAD_REFINE_MIN,
	HEAD_REFINE_MAX,
	HEAD_WEARING_LEVEL_MIN,
	HEAD_WEARING_LEVEL_MAX,
	HEAD_SELECT_DATA_JOB_WARRIOR,
	HEAD_SELECT_DATA_JOB_SURA,
	HEAD_SELECT_DATA_JOB_ASSASSIN,
	HEAD_SELECT_DATA_JOB_SHAMAN,
	HEAD_SELECT_DATA_JOB_LYCAN,

	COMMON_ON_OFF,
	COMMON_REFINE_MIN,
	COMMON_REFINE_MAX,
	COMMON_WEARING_LEVEL_MIN,
	COMMON_WEARING_LEVEL_MAX,
	COMMON_SELECT_DATA_SHOE,
	COMMON_SELECT_DATA_BELT,
	COMMON_SELECT_DATA_BRACELET,
	COMMON_SELECT_DATA_NECKLACE,
	COMMON_SELECT_DATA_EARRING,
	COMMON_SELECT_DATA_SHIELD,
	COMMON_SELECT_DATA_GLOVE,
	COMMON_SELECT_DATA_TALISMAN,
	COMMON_SELECT_DATA_FISHING_ROD,
	COMMON_SELECT_DATA_PICKAXE,

	COSTUME_ON_OFF,
	COSTUME_SELECT_DATA_WEAPON,
	COSTUME_SELECT_DATA_ARMOR,
	COSTUME_SELECT_DATA_HAIR,
	COSTUME_SELECT_DATA_ACCE,
	COSTUME_SELECT_DATA_AURA,
	COSTUME_SELECT_DATA_ETC,

	DS_ON_OFF,
	DS_SELECT_DATA_DS,
	DS_SELECT_DATA_ETC,

	UNIQUE_ON_OFF,
	UNIQUE_SELECT_DATA_ABILITY,
	UNIQUE_SELECT_DATA_ETC,

	REFINE_ON_OFF,
	REFINE_SELECT_DATA_MATERIAL,
	REFINE_SELECT_DATA_STONE,
	REFINE_SELECT_DATA_ETC,

	POTION_ON_OFF,
	POTION_SELECT_DATA_ABILITY,
	POTION_SELECT_DATA_HAIRDYE,
	POTION_SELECT_DATA_ETC,

	FISH_MINING_ON_OFF,
	FISH_MINING_SELECT_DATA_FOOD,
	FISH_MINING_SELECT_DATA_STONE,
	FISH_MINING_SELECT_DATA_ETC,

	MOUNT_PET_ON_OFF,
	MOUNT_PET_SELECT_DATA_CHARGED_PET,
	MOUNT_PET_SELECT_DATA_MOUNT,
	MOUNT_PET_SELECT_DATA_FREE_PET,
	MOUNT_PET_SELECT_DATA_EGG,

	SKILL_BOOK_ON_OFF,
	SKILL_BOOK_SELECT_DATA_JOB_WARRIOR,
	SKILL_BOOK_SELECT_DATA_JOB_SURA,
	SKILL_BOOK_SELECT_DATA_JOB_ASSASSIN,
	SKILL_BOOK_SELECT_DATA_JOB_SHAMAN,
	SKILL_BOOK_SELECT_DATA_JOB_LYCAN,
	SKILL_BOOK_SELECT_DATA_JOB_PUBLIC,

	ETC_ON_OFF,
	ETC_SELECT_DATA_GIFTBOX,
	ETC_SELECT_DATA_MATRIMONY,
	ETC_SELECT_DATA_SEAL,
	ETC_SELECT_DATA_PARTY,
	ETC_SELECT_DATA_POLYMORPH,
	ETC_SELECT_DATA_RECIPE,
	ETC_SELECT_DATA_WEAPON_ARROW,
	ETC_SELECT_DATA_ETC,

	EVENT_ON_OFF,

	LOOT_FILTER_SETTINGS_MAX
};
#endif

enum ESlotType
{
	SLOT_TYPE_NONE,
	SLOT_TYPE_INVENTORY,
	SLOT_TYPE_SKILL,
	SLOT_TYPE_EMOTION,
	SLOT_TYPE_SHOP,
	SLOT_TYPE_EXCHANGE_OWNER,
	SLOT_TYPE_EXCHANGE_TARGET,
	SLOT_TYPE_QUICK_SLOT,
	SLOT_TYPE_SAFEBOX,
	SLOT_TYPE_GUILDBANK,
	SLOT_TYPE_ACCE,
	SLOT_TYPE_PRIVATE_SHOP,
	SLOT_TYPE_MALL,
	SLOT_TYPE_DRAGON_SOUL_INVENTORY,
	SLOT_TYPE_PET_FEED_WINDOW,
	SLOT_TYPE_EQUIPMENT,
	SLOT_TYPE_BELT_INVENTORY,
	SLOT_TYPE_AUTO,
	SLOT_TYPE_CHANGE_LOOK,
	SLOT_TYPE_FISH_EVENT,
	SLOT_TYPE_AURA,
	SLOT_TYPE_PREMIUM_PRIVATE_SHOP,
	SLOT_TYPE_GOLDEN_LAND_FRUIT,
	SLOT_TYPE_MAX,
};

enum EWindows
{
	RESERVED_WINDOW,
	INVENTORY,
	EQUIPMENT,
	SAFEBOX,
	MALL,
	DRAGON_SOUL_INVENTORY,
	BELT_INVENTORY,
	GUILDBANK,
	MAIL,
	NPC_STORAGE,
	PREMIUM_PRIVATE_SHOP,
	ACCEREFINE,
	GROUND,
	PET_FEED,
	CHANGELOOK,
	AURA_REFINE,
	CUBE_WINDOW,

	WINDOW_TYPE_MAX,
};

#if defined(ENABLE_DRAGON_SOUL_SYSTEM)
enum EDSInventoryMaxNum
{
	DS_INVENTORY_MAX_NUM = c_DragonSoul_Inventory_Count,
	DS_REFINE_WINDOW_MAX_NUM = 15,
};
#endif

#if defined(ENABLE_MOVE_COSTUME_ATTR)
enum ECombSlotType
{
	COMB_WND_SLOT_MEDIUM,
	COMB_WND_SLOT_BASE,
	COMB_WND_SLOT_MATERIAL,
	COMB_WND_SLOT_RESULT,

	COMB_WND_SLOT_MAX
};
#endif

#if defined(ENABLE_DS_CHANGE_ATTR)
enum EDSRefineType
{
	DS_REFINE_TYPE_GRADE,
	DS_REFINE_TYPE_STEP,
	DS_REFINE_TYPE_STRENGTH,
	DS_REFINE_TYPE_CHANGE_ATTR,
	DS_REFINE_TYPE_MAX,
};
#endif

#if defined(ENABLE_ACCE_COSTUME_SYSTEM)
enum EAcce : DWORD
{
	ACCE_DRAIN_RATE_MAX = 25,
};

enum EAcceSlot
{
	ACCE_SLOT_LEFT,
	ACCE_SLOT_RIGHT,
	ACCE_SLOT_RESULT,
	ACCE_SLOT_MAX
};

enum EAcceSlotType
{
	ACCE_SLOT_TYPE_COMBINE,
	ACCE_SLOT_TYPE_ABSORB,
	ACCE_SLOT_TYPE_MAX
};

enum EAcceRefine : BYTE
{
	ITEM_SOCKET_ACCE_DRAIN_ITEM_VNUM,
	ITEM_SOCKET_ACCE_DRAIN_VALUE
};

enum EAcceScroll : DWORD
{
	ACCE_SCROLL_1 = 39046,
	ACCE_SCROLL_2 = 90000
};
#endif

#if defined(ENABLE_CHANGE_LOOK_SYSTEM)
enum class EChangeLookType : BYTE
{
	CHANGE_LOOK_TYPE_ITEM,
	CHANGE_LOOK_TYPE_MOUNT
};

enum class EChangeLookSlots : BYTE
{
	CHANGE_LOOK_SLOT_LEFT,
	CHANGE_LOOK_SLOT_RIGHT,
	CHANGE_LOOK_SLOT_MAX
};

enum class EChangeLookPrice : DWORD
{
	CHANGE_LOOK_ITEM_PRICE = 50000000,
	CHANGE_LOOK_MOUNT_PRICE = 30000000,
};

enum class EChangeLookItems : DWORD
{
	CHANGE_LOOK_TICKET_1 = 72326,
	CHANGE_LOOK_TICKET_2 = 72341,
	CHANGE_LOOK_CLEAR_SCROLL = 72325,
	CHANGE_LOOK_MOUNT_REVERSAL = 72343,
	CHANGE_LOOK_MOUNT_TICKET = 72344,
};
#endif

#if defined(ENABLE_ATTR_6TH_7TH)
enum EAttr67
{
	ATTR67_ADD_SLOT_MAX = 1,
};
#endif

#if defined(ENABLE_SET_ITEM)
enum ESetItemValue
{
	SET_ITEM_SET_VALUE_NONE,
	SET_ITEM_SET_VALUE_1,
	SET_ITEM_SET_VALUE_2,
	SET_ITEM_SET_VALUE_3,
	SET_ITEM_SET_VALUE_4,
	SET_ITEM_SET_VALUE_5,
	SET_ITEM_SET_VALUE_MAX
};
#endif

#if defined(ENABLE_LEFT_SEAT)
enum ELeftSeatTime
{
	LEFT_SEAT_TIME_10_MIN,
	LEFT_SEAT_TIME_30_MIN,
	LEFT_SEAT_TIME_90_MIN,
	LEFT_SEAT_TIME_MAX,
};

enum ELeftSeatLogoutTime
{
	LEFT_SEAT_LOGOUT_TIME_30_MIN,
	LEFT_SEAT_LOGOUT_TIME_60_MIN,
	LEFT_SEAT_LOGOUT_TIME_120_MIN,
	LEFT_SEAT_LOGOUT_TIME_180_MIN,
	LEFT_SEAT_LOGOUT_TIME_OFF,
	LEFT_SEAT_LOGOUT_TIME_MAX,
};
#endif

#if defined(ENABLE_SUMMER_EVENT_ROULETTE)
enum EMiniGameRoulette
{
	ROULETTE_ITEM_MAX = 20,
};
#endif

enum EDeadDialogType
{
	DEAD_DIALOG_NONE,
	DEAD_DIALOG_NORMAL,
	DEAD_DIALOG_GIVE_UP,
};

#if defined(ENABLE_FLOWER_EVENT)
enum EFlowerEventShootType
{
	SHOOT_ENVELOPE,
	SHOOT_CHRYSANTHEMUM,
	SHOOT_MAY_BELL,
	SHOOT_DAFFODIL,
	SHOOT_LILY,
	SHOOT_SUNFLOWER,
	SHOOT_TYPE_MAX,
};

enum EFlowerEventMisc
{
	FLOWER_EVENT_SHOOT_ENVELOPE_NEED_COUNT = 1,
	FLOWER_EVENT_SHOOT_NEED_COUNT = 10,
	FLOWER_EVENT_NEED_INVENTORY_SPACE = 1,
	FLOWER_EVENT_EXCHANGE_COOLTIME_SEC = 1,
	MAX_FLOWER_EVENT_ITEM_COUNT = 99999,
};

enum EFlowerEventChatType
{
	FLOWER_EVENT_CHAT_TYPE_NOT_ENOUGH_SHOOT_COUNT,
	FLOWER_EVENT_CHAT_TYPE_NOT_ENOUGH_EVENTORY_SPACE,
	FLOWER_EVENT_CHAT_TYPE_NOT_ENOUGH_SHOOT_ENVELOPE,
	FLOWER_EVENT_CHAT_TYPE_GET_SHOOT_ENVELOPE,
	FLOWER_EVENT_CHAT_TYPE_GET_SHOOT_CHRYSANTHEMUM,
	FLOWER_EVENT_CHAT_TYPE_GET_SHOOT_MAY_BELL,
	FLOWER_EVENT_CHAT_TYPE_GET_SHOOT_DAFFODIL,
	FLOWER_EVENT_CHAT_TYPE_GET_SHOOT_LILY,
	FLOWER_EVENT_CHAT_TYPE_GET_SHOOT_SUNFLOWER,
	FLOWER_EVENT_CHAT_TYPE_ITEM_FULL_AND_NOT_USE,
	FLOWER_EVENT_CHAT_TYPE_GET_SHOOT_CHRYSANTHEMUM_COUNT,
	FLOWER_EVENT_CHAT_TYPE_GET_SHOOT_MAY_BELL_COUNT,
	FLOWER_EVENT_CHAT_TYPE_GET_SHOOT_DAFFODIL_COUNT,
	FLOWER_EVENT_CHAT_TYPE_GET_SHOOT_LILY_COUNT,
	FLOWER_EVENT_CHAT_TYPE_GET_SHOOT_SUNFLOWER_COUNT,
	FLOWER_EVENT_CHAT_TYPE_ENVELOPE_MAX,
	FLOWER_EVENT_CHAT_TYPE_MAX,
};
#endif

#pragma pack (push, 1)
#define WORD_MAX 0xffff

typedef struct SItemPos
{
	BYTE window_type;
	WORD cell;
	SItemPos()
	{
		window_type = INVENTORY;
		cell = WORD_MAX;
	}

	SItemPos(BYTE _window_type, WORD _cell)
	{
		window_type = _window_type;
		cell = _cell;
	}

	// 기존에 cell의 형을 보면 BYTE가 대부분이지만, oi
	// 어떤 부분은 int, 어떤 부분은 WORD로 되어있어,
	// 가장 큰 자료형인 int로 받는다.
	/*
	int operator=(const int _cell)
	{
		window_type = INVENTORY;
		cell = _cell;
		return cell;
	}
	*/
	bool IsValidCell()
	{
		switch (window_type)
		{
			case INVENTORY:
				return cell < c_Inventory_Slot_Count;

			case EQUIPMENT:
				return cell < c_Equipment_Slot_Count;

#if defined(ENABLE_DRAGON_SOUL_SYSTEM)
			case DRAGON_SOUL_INVENTORY:
				return cell < c_DragonSoul_Inventory_Count;
#endif

			case BELT_INVENTORY:
				return cell < c_Belt_Inventory_Slot_Count;

#if defined(ENABLE_ATTR_6TH_7TH)
			case NPC_STORAGE:
				return cell < ATTR67_ADD_SLOT_MAX;
#endif

			default:
				return false;
		}
	}
	bool IsEquipCell()
	{
		switch (window_type)
		{
			case EQUIPMENT:
				return cell < c_Equipment_Slot_Count;

			default:
				return false;
		}
	}

#if defined(ENABLE_NEW_EQUIPMENT_SYSTEM)
	bool IsBeltInventoryCell()
	{
		bool bResult = c_Belt_Inventory_Slot_Start <= cell && c_Belt_Inventory_Slot_End > cell;
		return bResult;
	}
#endif

	bool IsNPOS()
	{
		return (window_type == RESERVED_WINDOW && cell == WORD_MAX);
	}
	bool operator==(const struct SItemPos& rhs) const
	{
		return (window_type == rhs.window_type) && (cell == rhs.cell);
	}
	bool operator!=(const struct SItemPos& rhs) const
	{
		return (window_type != rhs.window_type) || (cell != rhs.cell);
	}
	bool operator<(const struct SItemPos& rhs) const
	{
		return (window_type < rhs.window_type) || ((window_type == rhs.window_type) && (cell < rhs.cell));
	}
} TItemPos;
const TItemPos NPOS(RESERVED_WINDOW, WORD_MAX);
#pragma pack(pop)

const DWORD c_QuickBar_Line_Count = 3;
const DWORD c_QuickBar_Slot_Count = 12;

const float c_Idle_WaitTime = 5.0f;

const int c_Monster_Race_Start_Number = 6;
const int c_Monster_Model_Start_Number = 20001;

const float c_fAttack_Delay_Time = 0.2f;
const float c_fHit_Delay_Time = 0.1f;
const float c_fCrash_Wave_Time = 0.2f;
const float c_fCrash_Wave_Distance = 3.0f;

const float c_fHeight_Step_Distance = 50.0f;

enum
{
	DISTANCE_TYPE_FOUR_WAY,
	DISTANCE_TYPE_EIGHT_WAY,
	DISTANCE_TYPE_ONE_WAY,
	DISTANCE_TYPE_MAX_NUM,
};

const int c_iShow_UI_Window_Limit_Range = 1000;

const float c_fMagic_Script_Version = 1.0f;
const float c_fSkill_Script_Version = 1.0f;
const float c_fMagicSoundInformation_Version = 1.0f;
const float c_fBattleCommand_Script_Version = 1.0f;
const float c_fEmotionCommand_Script_Version = 1.0f;
const float c_fActive_Script_Version = 1.0f;
const float c_fPassive_Script_Version = 1.0f;

// Used by PushMove
const float c_fWalkDistance = 175.0f;
const float c_fRunDistance = 310.0f;

#define FILE_MAX_LEN 128

enum
{
	POINT_MAX_NUM = 1000,
	CHARACTER_NAME_MAX_LEN = 24,
#if defined(ENABLE_MULTI_LANGUAGE_SYSTEM)
	COUNTRY_NAME_MAX_LEN = 2,
#endif
#if defined(LOCALE_SERVICE_JAPAN)
	PLAYER_NAME_MAX_LEN = 16,
#else
	PLAYER_NAME_MAX_LEN = 12,
#endif
	CHARACTER_FOLDER_MAX_LEN = 64,
	ITEM_METIN_SOCKET_MAX = 3,
#if defined(ENABLE_ITEM_SOCKET6)
	ITEM_SOCKET_SLOT_MAX_NUM = 6,
#else
	ITEM_SOCKET_SLOT_MAX_NUM = 3,
#endif
	ITEM_ATTRIBUTE_SLOT_MAX_NUM = 7,
#if defined(ENABLE_APPLY_RANDOM)
	ITEM_APPLY_RANDOM_SLOT_MAX_NUM = 4,
#endif
#if defined(ENABLE_RANKING_SYSTEM)
	PARTY_MAX_MEMBER = 8,
#endif
#if defined(ENABLE_REFINE_ELEMENT_SYSTEM)
	REFINE_ELEMENT_MAX = 3,
#endif
};

#pragma pack(push)
#pragma pack(1)

typedef struct SQuickSlot
{
	BYTE Type;
	WORD Position;
} TQuickSlot;

typedef struct SPlayerItemAttribute
{
	POINT_TYPE wType;
	POINT_VALUE lValue;
#if defined(ENABLE_APPLY_RANDOM)
	BYTE bPath;
#endif
} TPlayerItemAttribute;

#if defined(ENABLE_REFINE_ELEMENT_SYSTEM)
enum ERefineElementType
{
	REFINE_ELEMENT_UPGRADE,
	REFINE_ELEMENT_DOWNGRADE,
	REFINE_ELEMENT_CHANGE
};

typedef struct SPlayerItemRefineElement
{
	WORD wApplyType;
	BYTE bGrade;
	BYTE abValue[REFINE_ELEMENT_MAX];
	BYTE abBonusValue[REFINE_ELEMENT_MAX];
} TPlayerItemRefineElement;
#endif

typedef struct packet_item
{
	DWORD dwVnum;
	DWORD dwCount;
	DWORD dwFlags;
	DWORD dwAntiFlags;
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
} TItemData;

typedef struct packet_shop_item
{
	DWORD dwVnum;
	DWORD dwCount;
	DWORD dwPrice;
#if defined(ENABLE_CHEQUE_SYSTEM)
	DWORD dwCheque;
#endif
	BYTE bDisplayPos;
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
#if defined(ENABLE_SHOPEX_RENEWAL)
	BYTE bPriceType;
	DWORD dwPriceVnum;
#endif
} TShopItemData;

#if defined(ENABLE_PRIVATESHOP_SEARCH_SYSTEM)
struct ShopSearchData
{
	TShopItemData item;
	std::string name;
	DWORD dwShopPID;
	DWORD dwShopVID;
};
#endif

#if defined(ENABLE_ATTR_6TH_7TH)
typedef struct SAttr67AddData
{
	SAttr67AddData() : wRegistItemPos(0), byMaterialCount(0), wSupportItemPos(0), bySupportItemCount(0) {}
	WORD wRegistItemPos;
	BYTE byMaterialCount;
	WORD wSupportItemPos;
	BYTE bySupportItemCount;
} TAttr67AddData;
#endif

#if defined(ENABLE_GEM_SYSTEM)
enum EGemShop
{
	GEM_SHOP_SLOT_COUNT = 9,
#	if defined(ENABLE_CONQUEROR_LEVEL)
	GEM_SHOP_SPECIAL_SLOT_COUNT = 3,
#	endif
};

typedef struct SGemShopItem
{
	bool bEnable;
	DWORD dwItemVnum;
	BYTE bCount;
	DWORD dwPrice;
} TGemShopItem;

typedef struct SGemShopTable
{
	DWORD dwPID;
	long lRefreshTime;
	BYTE bEnabledSlots;
	TGemShopItem GemShopItem[GEM_SHOP_SLOT_COUNT];
#	if defined(ENABLE_CONQUEROR_LEVEL)
	bool bSpecial;
#	endif
} TGemShopTable;
#endif

#if defined(ENABLE_EXTEND_INVEN_SYSTEM)
enum EEXInventory
{
	EX_INVENTORY_PAGE_COUNT = 2,
	EX_INVENTORY_PAGE_START = 3,
	EX_INVENTORY_STAGE_MAX = c_Inventory_Page_Height * EX_INVENTORY_PAGE_COUNT,
};
enum EEXInventoryRefine
{
	EX_INVEN_FAIL_FALL_SHORT,
	EX_INVEN_SUCCESS,
	EX_INVEN_FAIL_FOURTH_PAGE_STAGE_MAX,
};
#endif

#if defined(ENABLE_AURA_COSTUME_SYSTEM)
enum EAuraMisc
{
	AURA_MAX_LEVEL = 250,
};

enum EAuraWindowType
{
	AURA_WINDOW_TYPE_ABSORB,
	AURA_WINDOW_TYPE_GROWTH,
	AURA_WINDOW_TYPE_EVOLVE,
	AURA_WINDOW_TYPE_MAX,
};

enum EAuraSlotType
{
	AURA_SLOT_MAIN,
	AURA_SLOT_SUB,
	AURA_SLOT_RESULT,
	AURA_SLOT_MAX
};

enum EAuraGradeType
{
	AURA_GRADE_NONE,
	AURA_GRADE_ORDINARY,
	AURA_GRADE_SIMPLE,
	AURA_GRADE_NOBLE,
	AURA_GRADE_SPARKLING,
	AURA_GRADE_MAGNIFICENT,
	AURA_GRADE_RADIANT,
	AURA_GRADE_MAX_NUM,
};

enum EAuraRefineTokenType
{
	AURA_REFINE_INFO_STEP,
	AURA_REFINE_INFO_LEVEL_MIN,
	AURA_REFINE_INFO_LEVEL_MAX,
	AURA_REFINE_INFO_NEED_EXP,
	AURA_REFINE_INFO_MATERIAL_VNUM,
	AURA_REFINE_INFO_MATERIAL_COUNT,
	AURA_REFINE_INFO_NEED_GOLD,
	AURA_REFINE_INFO_EVOLVE_PCT,
	AURA_REFINE_INFO_MAX
};

enum EAuraRefineInfoType
{
	AURA_REFINE_INFO_SLOT_CURRENT,
	AURA_REFINE_INFO_SLOT_NEXT,
	AURA_REFINE_INFO_SLOT_EVOLVED,
	AURA_REFINE_INFO_SLOT_MAX
};

enum EItemAuraSocket
{
	ITEM_SOCKET_AURA_DRAIN_ITEM_VNUM,
	ITEM_SOCKET_AURA_LEVEL_VALUE,
};

enum EItemAuraMaterialValues
{
	ITEM_VALUE_AURA_MATERIAL_EXP,
};
#endif

#pragma pack(pop)

inline float GetSqrtDistance(int ix1, int iy1, int ix2, int iy2) // By sqrt
{
	float dx, dy;

	dx = float(ix1 - ix2);
	dy = float(iy1 - iy2);

	return sqrtf(dx * dx + dy * dy);
}

// DEFAULT_FONT
void DefaultFont_Startup();
void DefaultFont_Cleanup();
void DefaultFont_SetName(const char* c_szFontName);
CResource* DefaultFont_GetResource();
CResource* DefaultItalicFont_GetResource();
CResource* DefaultBoldFont_GetResource();
// END_OF_DEFAULT_FONT

void SetGuildSymbolPath(const char* c_szPathName);
const char* GetGuildSymbolFileName(DWORD dwGuildID);
BYTE SlotTypeToInvenType(BYTE bSlotType);
BYTE WindowTypeToSlotType(BYTE bWndType);
//#if defined(ENABLE_DETAILS_UI)
POINT_TYPE ApplyTypeToPointType(POINT_TYPE wApplyType);
POINT_TYPE PointTypeToApplyType(POINT_TYPE wPointType);
//#endif
#if defined(ENABLE_AURA_COSTUME_SYSTEM)
const int* GetAuraRefineInfo(BYTE bLevel);
#endif
