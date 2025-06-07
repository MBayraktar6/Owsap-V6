#pragma once

// NOTE : Item�� ���� ���� Ŭ������.
// Icon, Model (droped on ground), Game Data

#include "../EterLib/GrpSubImage.h"
#include "../EterGrnLib/Thing.h"
#include "../UserInterface/Locale_inc.h"

class CItemData
{
public:
	enum
	{
		ITEM_NAME_MAX_LEN = 30,
		ITEM_SMALL_DESCR_MAX_LEN = 256,
		ITEM_LIMIT_MAX_NUM = 2,
#if defined(ENABLE_ITEM_APPLY4)
		ITEM_APPLY_MAX_NUM = 4,
#else
		ITEM_APPLY_MAX_NUM = 3,
#endif
#if defined(ENABLE_ITEM_VALUE10)
		ITEM_VALUES_MAX_NUM = 10,
#else
		ITEM_VALUES_MAX_NUM = 6,
#endif
#if defined(ENABLE_ITEM_SOCKET6)
		ITEM_SOCKET_MAX_NUM = 6,
#else
		ITEM_SOCKET_MAX_NUM = 3,
#endif
	};

	enum EItemTypes
	{
		ITEM_TYPE_NONE,
		ITEM_TYPE_WEAPON,
		ITEM_TYPE_ARMOR,
		ITEM_TYPE_USE,
		ITEM_TYPE_AUTOUSE,
		ITEM_TYPE_MATERIAL,
		ITEM_TYPE_SPECIAL,
		ITEM_TYPE_TOOL,
		ITEM_TYPE_LOTTERY,
		ITEM_TYPE_ELK,
		ITEM_TYPE_METIN,
		ITEM_TYPE_CONTAINER,
		ITEM_TYPE_FISH,
		ITEM_TYPE_ROD,
		ITEM_TYPE_RESOURCE,
		ITEM_TYPE_CAMPFIRE,
		ITEM_TYPE_UNIQUE,
		ITEM_TYPE_SKILLBOOK,
		ITEM_TYPE_QUEST,
		ITEM_TYPE_POLYMORPH,
		ITEM_TYPE_TREASURE_BOX,
		ITEM_TYPE_TREASURE_KEY,
		ITEM_TYPE_SKILLFORGET,
		ITEM_TYPE_GIFTBOX,
		ITEM_TYPE_PICK,
		ITEM_TYPE_HAIR,
		ITEM_TYPE_TOTEM,
		ITEM_TYPE_BLEND,
		ITEM_TYPE_COSTUME,
		ITEM_TYPE_DS,
		ITEM_TYPE_SPECIAL_DS,
		ITEM_TYPE_EXTRACT,
		ITEM_TYPE_SECONDARY_COIN,
		ITEM_TYPE_RING,
		ITEM_TYPE_BELT,
		ITEM_TYPE_PET,
#if defined(ENABLE_MOVE_COSTUME_ATTR)
		ITEM_TYPE_MEDIUM,
#endif
#if defined(ENABLE_GACHA)
		ITEM_TYPE_GACHA,
#endif
		ITEM_TYPE_PASSIVE,
		ITEM_TYPE_MERCENARY,
		ITEM_TYPE_ALCHEMY,
		//#if defined(ENABLE_SOUL_SYSTEM)
		ITEM_TYPE_SOUL,
		//#endif
		ITEM_TYPE_MAX_NUM,
	};

	enum EWeaponSubTypes
	{
		WEAPON_SWORD,
		WEAPON_DAGGER, // �̵���
		WEAPON_BOW,
		WEAPON_TWO_HANDED,
		WEAPON_BELL,
		WEAPON_FAN,
		WEAPON_ARROW,
		WEAPON_MOUNT_SPEAR,
		WEAPON_CLAW,
#if defined(ENABLE_QUIVER_SYSTEM)
		WEAPON_QUIVER,
#endif
		WEAPON_BOUQUET,

		WEAPON_NUM_TYPES,
		WEAPON_NONE = WEAPON_NUM_TYPES + 1
	};

	enum EArmorSubTypes
	{
		ARMOR_BODY,
		ARMOR_HEAD,
		ARMOR_SHIELD,
		ARMOR_WRIST,
		ARMOR_FOOTS,
		ARMOR_NECK,
		ARMOR_EAR,
#if defined(ENABLE_PENDANT)
		ARMOR_PENDANT,
#endif
#if defined(ENABLE_GLOVE_SYSTEM)
		ARMOR_GLOVE,
#endif
		ARMOR_NUM_TYPES,
	};

	enum EUseSubTypes
	{
		USE_POTION,
		USE_TALISMAN,
		USE_TUNING,
		USE_MOVE,
		USE_TREASURE_BOX,
		USE_MONEYBAG,
		USE_BAIT,
		USE_ABILITY_UP,
		USE_AFFECT,
		USE_CREATE_STONE,
		USE_SPECIAL,
		USE_POTION_NODELAY,
		USE_CLEAR,
		USE_INVISIBILITY,
		USE_DETACHMENT,
		USE_BUCKET,
		USE_POTION_CONTINUE,
		USE_CLEAN_SOCKET,
		USE_CHANGE_ATTRIBUTE,
		USE_ADD_ATTRIBUTE,
		USE_ADD_ACCESSORY_SOCKET,
		USE_PUT_INTO_ACCESSORY_SOCKET,
		USE_ADD_ATTRIBUTE2,
		USE_RECIPE,
		USE_CHANGE_ATTRIBUTE2,
		USE_BIND,
		USE_UNBIND,
		USE_TIME_CHARGE_PER,
		USE_TIME_CHARGE_FIX,
		USE_PUT_INTO_BELT_SOCKET,
		USE_PUT_INTO_RING_SOCKET,
#if defined(ENABLE_MOVE_COSTUME_ATTR)
		USE_CHANGE_COSTUME_ATTR,
		USE_RESET_COSTUME_ATTR,
#endif
		USE_UNK_33,
#if defined(ENABLE_CHANGED_ATTR)
		USE_SELECT_ATTRIBUTE,
#endif
		USE_FLOWER,
#if defined(ENABLE_EXPRESSING_EMOTION)
		USE_EMOTION_PACK,
#endif
#if defined(ENABLE_REFINE_ELEMENT_SYSTEM)
		USE_ELEMENT_UPGRADE,
		USE_ELEMENT_DOWNGRADE,
		USE_ELEMENT_CHANGE,
#endif
		USE_CALL,
		USE_POTION_TOWER,
		USE_POTION_NODELAY_TOWER,
		USE_REMOVE_AFFECT,
		USE_EMOTION_TOWER,
		USE_SECRET_DUNGEON_SCROLL,
	};

	enum EAutoUseSubTypes
	{
		AUTOUSE_POTION,
		AUTOUSE_ABILITY_UP,
		AUTOUSE_BOMB,
		AUTOUSE_GOLD,
		AUTOUSE_MONEYBAG,
		AUTOUSE_TREASURE_BOX,
	};

	enum EMaterialSubTypes
	{
		MATERIAL_LEATHER,
		MATERIAL_BLOOD,
		MATERIAL_ROOT,
		MATERIAL_NEEDLE,
		MATERIAL_JEWEL,
		MATERIAL_DS_REFINE_NORMAL,
		MATERIAL_DS_REFINE_BLESSED,
		MATERIAL_DS_REFINE_HOLLY,
#if defined(ENABLE_DS_CHANGE_ATTR)
		MATERIAL_DS_CHANGE_ATTR,
#endif
		MATERIAL_PASSIVE_WEAPON,
		MATERIAL_PASSIVE_ARMOR,
		MATERIAL_PASSIVE_ACCE,
		MATERIAL_PASSIVE_ELEMENT,
	};

	enum ESpecialSubTypes
	{
		SPECIAL_MAP,
		SPECIAL_KEY,
		SPECIAL_DOC,
		SPECIAL_SPIRIT,
	};

	enum EToolSubTypes
	{
		TOOL_FISHING_ROD,
	};

	enum ELotterySubTypes
	{
		LOTTERY_TICKET,
		LOTTERY_INSTANT,
	};

	enum EMetinSubTypes
	{
		METIN_NORMAL,
		METIN_SUNGMA,
	};

	enum EFishSubTypes
	{
		FISH_ALIVE,
		FISH_DEAD,
	};

	enum EResourceSubTypes
	{
		RESOURCE_FISHBONE,
		RESOURCE_WATERSTONEPIECE,
		RESOURCE_WATERSTONE,
		RESOURCE_BLOOD_PEARL,
		RESOURCE_BLUE_PEARL,
		RESOURCE_WHITE_PEARL,
		RESOURCE_BUCKET,
		RESOURCE_CRYSTAL,
		RESOURCE_GEM,
		RESOURCE_STONE,
		RESOURCE_METIN,
		RESOURCE_ORE,
#if defined(ENABLE_AURA_COSTUME_SYSTEM)
		RESOURCE_AURA,
#endif
	};

	enum EUniqueSubTypes
	{
		UNIQUE_NONE,
		UNIQUE_BOOK,
		UNIQUE_SPECIAL_RIDE,
		UNIQUE_3,
		UNIQUE_4,
		UNIQUE_5,
		UNIQUE_6,
		UNIQUE_7,
		UNIQUE_8,
		UNIQUE_9,
		UNIQUE_BUNDLE,
	};

	enum EQuestTypes
	{
		QUEST_NONE,
		QUEST_PET_PAY,
		QUEST_WARP,
		QUEST_GEM_BAG,
	};

	enum EGiftBoxSubTypes
	{
		GIFTBOX_NONE,
		GIFTBOX_NORMAL,
		GIFTBOX_SPECIAL,
	};

	enum ECostumeSubTypes
	{
		COSTUME_BODY, // 0 ����(main look)
		COSTUME_HAIR, // 1 ���(Ż������)
#if defined(ENABLE_MOUNT_COSTUME_SYSTEM)
		COSTUME_MOUNT,
#endif
#if defined(ENABLE_ACCE_COSTUME_SYSTEM)
		COSTUME_ACCE,
#endif
#if defined(ENABLE_WEAPON_COSTUME_SYSTEM)
		COSTUME_WEAPON,
#endif
#if defined(ENABLE_AURA_COSTUME_SYSTEM)
		COSTUME_AURA,
#endif

		COSTUME_NUM_TYPES,
	};

#if defined(ENABLE_DRAGON_SOUL_SYSTEM)
	enum EDragonSoulSubType
	{
		DS_SLOT1,
		DS_SLOT2,
		DS_SLOT3,
		DS_SLOT4,
		DS_SLOT5,
		DS_SLOT6,
		DS_SLOT7,
		DS_SLOT_NUM_TYPES = 7,
	};
#endif

	enum EExtractSubTypes
	{
		EXTRACT_DRAGON_SOUL,
		EXTRACT_DRAGON_HEART,
	};

	enum EPetSubTypes
	{
		//#if defined(ENABLE_GROWTH_PET_SYSTEM)
		PET_EGG,
		PET_UPBRINGING,
		PET_BAG,
		PET_FEEDSTUFF,
		PET_SKILL,
		PET_SKILL_DEL_BOOK,
		PET_NAME_CHANGE,
		PET_EXPFOOD,
		PET_SKILL_ALL_DEL_BOOK,
		PET_EXPFOOD_PER,
		//#if defined(ENABLE_PET_ATTR_DETERMINE)
		PET_ATTR_DETERMINE,
		PET_ATTR_CHANGE,
		//#endif
		//#endif
		PET_PAY,
		//#if defined(ENABLE_GROWTH_PET_SYSTEM) && defined(ENABLE_PET_PREMIUM_FEEDSTUFF)
		PET_PRIMIUM_FEEDSTUFF,
		//#endif
	};

#if defined(ENABLE_MOVE_COSTUME_ATTR)
	enum EMediumSubTypes
	{
		MEDIUM_MOVE_COSTUME_ATTR,
#if defined(ENABLE_ACCE_COSTUME_SYSTEM)
		MEDIUM_MOVE_ACCE_ATTR,
#endif
	};
#endif

#if defined(ENABLE_GACHA)
	enum EGachaSubType
	{
		USE_GACHA,
#if defined(ENABLE_LUCKY_BOX)
		GEM_LUCKY_BOX_GACHA,
		SPECIAL_LUCKY_BOX_GACHA,
#endif
	};
#endif

	enum EPassiveSubTypes
	{
		PASSIVE_JOB,
	};

	enum EMercenarySubType
	{
		MERCENARY_0,
		MERCENARY_1,
		MERCENARY_2,
		MERCENARY_3,
		MERCENARY_4,
		MERCENARY_5,
		MERCENARY_6
	};

	enum EAlchemySubTypes
	{
		ALCHEMY_ARMOR,
		ALCHEMY_WEAPON,
		ALCHEMY_ACCESSORY,
		ALCHEMY_BELT,
		ALCHEMY_EVENT,
		ALCHEMY_ETC,
		ALCHEMY_JOB,
		ALCHEMY_SETADD_WEAPON,
		ALCHEMY_SETADD_ARMOR_BODY,
		ALCHEMY_SETADD_ARMOR_HELMET,
		ALCHEMY_PET,
		ALCHEMY_SKILL_BOOK,
		ALCHEMY_GLOVE,
	};

#if defined(ENABLE_SOUL_SYSTEM)
	enum ESoulSubTypes
	{
		RED_SOUL,
		BLUE_SOUL,
	};
#endif

	enum EItemFlags
	{
		ITEM_FLAG_REFINEABLE = (1 << 0),
		ITEM_FLAG_SAVE = (1 << 1),
		ITEM_FLAG_STACKABLE = (1 << 2), // ������ ��ĥ �� ����
		ITEM_FLAG_COUNT_PER_1GOLD = (1 << 3),
		ITEM_FLAG_SLOW_QUERY = (1 << 4),
		ITEM_FLAG_UNIQUE = (1 << 5),
		ITEM_FLAG_MAKECOUNT = (1 << 6),
		ITEM_FLAG_IRREMOVABLE = (1 << 7),
		ITEM_FLAG_CONFIRM_WHEN_USE = (1 << 8),
		ITEM_FLAG_QUEST_USE = (1 << 9),
		ITEM_FLAG_QUEST_USE_MULTIPLE = (1 << 10),
		ITEM_FLAG_QUEST_GIVE = (1 << 11),
		ITEM_FLAG_LOG = (1 << 12),
		ITEM_FLAG_APPLICABLE = (1 << 13),
	};

	enum EItemAntiFlags
	{
		ITEM_ANTIFLAG_FEMALE = (1 << 0), // ���� ��� �Ұ�
		ITEM_ANTIFLAG_MALE = (1 << 1), // ���� ��� �Ұ�
		ITEM_ANTIFLAG_WARRIOR = (1 << 2), // ���� ��� �Ұ�
		ITEM_ANTIFLAG_ASSASSIN = (1 << 3), // �ڰ� ��� �Ұ�
		ITEM_ANTIFLAG_SURA = (1 << 4), // ���� ��� �Ұ� 
		ITEM_ANTIFLAG_SHAMAN = (1 << 5), // ���� ��� �Ұ�
		ITEM_ANTIFLAG_GET = (1 << 6), // ���� �� ����
		ITEM_ANTIFLAG_DROP = (1 << 7), // ���� �� ����
		ITEM_ANTIFLAG_SELL = (1 << 8), // �� �� ����
		ITEM_ANTIFLAG_EMPIRE_A = (1 << 9), // A ���� ��� �Ұ�
		ITEM_ANTIFLAG_EMPIRE_B = (1 << 10), // B ���� ��� �Ұ�
		ITEM_ANTIFLAG_EMPIRE_C = (1 << 11), // C ���� ��� �Ұ�
		ITEM_ANTIFLAG_SAVE = (1 << 12), // ������� ����
		ITEM_ANTIFLAG_GIVE = (1 << 13), // �ŷ� �Ұ�
		ITEM_ANTIFLAG_PKDROP = (1 << 14), // PK�� �������� ����
		ITEM_ANTIFLAG_STACK = (1 << 15), // ��ĥ �� ����
		ITEM_ANTIFLAG_MYSHOP = (1 << 16), // ���� ������ �ø� �� ����
		ITEM_ANTIFLAG_SAFEBOX = (1 << 17), // â�� ���� �� ����
		ITEM_ANTIFLAG_WOLFMAN = (1 << 18),
		ITEM_ANTIFLAG_RT_REMOVE = (1 << 19),
		ITEM_ANTIFLAG_QUICKSLOT = (1 << 20),
#if defined(ENABLE_CHANGE_LOOK_SYSTEM)
		ITEM_ANTIFLAG_CHANGELOOK = (1 << 21),
#endif
		ITEM_ANTIFLAG_REINFORCE = (1 << 22),
		ITEM_ANTIFLAG_ENCHANT = (1 << 23),
		ITEM_ANTIFLAG_ENERGY = (1 << 24),
		ITEM_ANTIFLAG_PETFEED = (1 << 25),
		ITEM_ANTIFLAG_APPLY = (1 << 26),
#if defined(ENABLE_ACCE_COSTUME_SYSTEM)
		ITEM_ANTIFLAG_ACCE = (1 << 27),
#endif
#if defined(ENABLE_MAILBOX)
		ITEM_ANTIFLAG_MAIL = (1 << 28),
#endif
		ITEM_ANTIFLAG_DESTROY = (1 << 29), // Custom
	};

	enum EItemWearableFlag
	{
		WEARABLE_BODY = (1 << 0),
		WEARABLE_HEAD = (1 << 1),
		WEARABLE_FOOTS = (1 << 2),
		WEARABLE_WRIST = (1 << 3),
		WEARABLE_WEAPON = (1 << 4),
		WEARABLE_NECK = (1 << 5),
		WEARABLE_EAR = (1 << 6),
		WEARABLE_UNIQUE = (1 << 7),
		WEARABLE_SHIELD = (1 << 8),
		WEARABLE_ARROW = (1 << 9),
		WEARABLE_HAIR = (1 << 10),
#if defined(ENABLE_PENDANT)
		WEARABLE_PENDANT = (1 << 11),
#endif
#if defined(ENABLE_GLOVE_SYSTEM)
		WEARABLE_GLOVE = (1 << 12),
#endif
	};

	enum EWearPositions
	{
		WEAR_BODY,
		WEAR_HEAD,
		WEAR_FOOTS,
		WEAR_WRIST,
		WEAR_WEAPON,
		WEAR_NECK,
		WEAR_EAR,
		WEAR_UNIQUE1,
		WEAR_UNIQUE2,
		WEAR_ARROW,
		WEAR_SHIELD,
		WEAR_BELT,
#if defined(ENABLE_PENDANT)
		WEAR_PENDANT,
#endif
#if defined(ENABLE_GLOVE_SYSTEM)
		WEAR_GLOVE,
#endif
		WEAR_COSTUME_BODY,
		WEAR_COSTUME_HAIR,
#if defined(ENABLE_MOUNT_COSTUME_SYSTEM)
		WEAR_COSTUME_MOUNT,
#endif
#if defined(ENABLE_ACCE_COSTUME_SYSTEM)
		WEAR_COSTUME_ACCE,
#endif
#if defined(ENABLE_WEAPON_COSTUME_SYSTEM)
		WEAR_COSTUME_WEAPON,
#endif
#if defined(ENABLE_AURA_COSTUME_SYSTEM)
		WEAR_COSTUME_AURA,
#endif
		WEAR_MAX_NUM = 32
	};

	enum ELimitTypes
	{
		LIMIT_NONE,

		LIMIT_LEVEL,
		LIMIT_STR,
		LIMIT_DEX,
		LIMIT_INT,
		LIMIT_CON,

		/// ���� ���ο� ��� ���� �ǽð����� �ð� ���� (socket0�� �Ҹ� �ð��� ����: unix_timestamp Ÿ��)
		LIMIT_REAL_TIME,

		/// �������� �� ó�� ���(Ȥ�� ����) �� �������� ����Ÿ�� Ÿ�̸� ���� 
		/// ���� ��� ������ socket0�� ��밡�ɽð�(�ʴ���, 0�̸� �������� limit value�� ���) ���� �����ִٰ� 
		/// ������ ���� socket1�� ��� Ƚ���� ������ socket0�� unix_timestamp Ÿ���� �Ҹ�ð��� ����.
		LIMIT_REAL_TIME_START_FIRST_USE,

		/// �������� ���� ���� ���� ��� �ð��� �����Ǵ� ������
		/// socket0�� ���� �ð��� �ʴ����� ����. (������ ���� ���� �ش� ���� 0�̸� �������� limit value���� socket0�� ����)
		LIMIT_TIMER_BASED_ON_WEAR,

		LIMIT_NEWWORLD_LEVEL,
		LIMIT_DURATION,

		LIMIT_MAX_NUM
	};

	enum ERefineTypes
	{
		REFINE_TYPE_CHUKBOK,
		REFINE_TYPE_HYUNIRON,
		REFINE_TYPE_YONGSIN,
		REFINE_TYPE_MUSIN,
		REFINE_TYPE_YAGONG,
		REFINE_TYPE_MEMO,
		REFINE_TYPE_BDRAGON,
#if defined(ENABLE_STONE_OF_BLESS)
		REFINE_TYPE_STONE_OF_BLESS,
#endif
#if defined(ENABLE_SOUL_SYSTEM)
		REFINE_TYPE_SOUL_AWAKE,
		REFINE_TYPE_SOUL_EVOLVE,
#endif
	};

	enum EApplyTypes
	{
		APPLY_NONE,							// 0
		APPLY_MAX_HP,						// 1
		APPLY_MAX_SP,						// 2
		APPLY_CON,							// 3
		APPLY_INT,							// 4
		APPLY_STR,							// 5
		APPLY_DEX,							// 6
		APPLY_ATT_SPEED,					// 7
		APPLY_MOV_SPEED,					// 8
		APPLY_CAST_SPEED,					// 9
		APPLY_HP_REGEN,						// 10
		APPLY_SP_REGEN,						// 11
		APPLY_POISON_PCT,					// 12
		APPLY_STUN_PCT,						// 13
		APPLY_SLOW_PCT,						// 14
		APPLY_CRITICAL_PCT,					// 15
		APPLY_PENETRATE_PCT,				// 16
		APPLY_ATTBONUS_HUMAN,				// 17
		APPLY_ATTBONUS_ANIMAL,				// 18
		APPLY_ATTBONUS_ORC,					// 19
		APPLY_ATTBONUS_MILGYO,				// 20
		APPLY_ATTBONUS_UNDEAD,				// 21
		APPLY_ATTBONUS_DEVIL,				// 22
		APPLY_STEAL_HP,						// 23
		APPLY_STEAL_SP,						// 24
		APPLY_MANA_BURN_PCT,				// 25
		APPLY_DAMAGE_SP_RECOVER,			// 26
		APPLY_BLOCK,						// 27
		APPLY_DODGE,						// 28
		APPLY_RESIST_SWORD,					// 29
		APPLY_RESIST_TWOHAND,				// 30
		APPLY_RESIST_DAGGER,				// 31
		APPLY_RESIST_BELL,					// 32
		APPLY_RESIST_FAN,					// 33
		APPLY_RESIST_BOW,					// 34
		APPLY_RESIST_FIRE,					// 35
		APPLY_RESIST_ELEC,					// 36
		APPLY_RESIST_MAGIC,					// 37
		APPLY_RESIST_WIND,					// 38
		APPLY_REFLECT_MELEE,				// 39
		APPLY_REFLECT_CURSE,				// 40
		APPLY_POISON_REDUCE,				// 41
		APPLY_KILL_SP_RECOVER,				// 42
		APPLY_EXP_DOUBLE_BONUS,				// 43
		APPLY_GOLD_DOUBLE_BONUS,			// 44
		APPLY_ITEM_DROP_BONUS,				// 45
		APPLY_POTION_BONUS,					// 46
		APPLY_KILL_HP_RECOVER,				// 47
		APPLY_IMMUNE_STUN,					// 48
		APPLY_IMMUNE_SLOW,					// 49
		APPLY_IMMUNE_FALL,					// 50
		APPLY_SKILL,						// 51
		APPLY_BOW_DISTANCE,					// 52
		APPLY_ATT_GRADE_BONUS,				// 53
		APPLY_DEF_GRADE_BONUS,				// 54
		APPLY_MAGIC_ATT_GRADE,				// 55
		APPLY_MAGIC_DEF_GRADE,				// 56
		APPLY_CURSE_PCT,					// 57
		APPLY_MAX_STAMINA,					// 58
		APPLY_ATTBONUS_WARRIOR,				// 59
		APPLY_ATTBONUS_ASSASSIN,			// 60
		APPLY_ATTBONUS_SURA,				// 61
		APPLY_ATTBONUS_SHAMAN,				// 62
		APPLY_ATTBONUS_MONSTER,				// 63
		APPLY_MALL_ATTBONUS,				// 64 ���ݷ� +x%
		APPLY_MALL_DEFBONUS,				// 65 ���� +x%
		APPLY_MALL_EXPBONUS,				// 66 ����ġ +x%
		APPLY_MALL_ITEMBONUS,				// 67 ������ ����� x/10��
		APPLY_MALL_GOLDBONUS,				// 68 �� ����� x/10��
		APPLY_MAX_HP_PCT,					// 69 �ִ� ����� +x%
		APPLY_MAX_SP_PCT,					// 70 �ִ� ���ŷ� +x%
		APPLY_SKILL_DAMAGE_BONUS,			// 71 ��ų ������ * (100+x)%
		APPLY_NORMAL_HIT_DAMAGE_BONUS,		// 72 ��Ÿ ������ * (100+x)%
		APPLY_SKILL_DEFEND_BONUS,			// 73 ��ų ������ ��� * (100-x)%
		APPLY_NORMAL_HIT_DEFEND_BONUS,		// 74 ��Ÿ ������ ��� * (100-x)%
		APPLY_EXTRACT_HP_PCT,				// 75
		APPLY_PC_BANG_EXP_BONUS,			// 76
		APPLY_PC_BANG_DROP_BONUS,			// 77
		APPLY_RESIST_WARRIOR,				// 78
		APPLY_RESIST_ASSASSIN,				// 79
		APPLY_RESIST_SURA,					// 80
		APPLY_RESIST_SHAMAN,				// 81
		APPLY_ENERGY,						// 82
		APPLY_DEF_GRADE,					// 83 ����. DEF_GRADE_BONUS�� Ŭ�󿡼� �ι�� �������� �ǵ��� ����(...)�� �ִ�.
		APPLY_COSTUME_ATTR_BONUS,			// 84 �ڽ�Ƭ �����ۿ� ���� �Ӽ�ġ ���ʽ�
		APPLY_MAGIC_ATTBONUS_PER,			// 85 ���� ���ݷ� +x%
		APPLY_MELEE_MAGIC_ATTBONUS_PER,		// 86 ���� + �и� ���ݷ� +x%

		APPLY_RESIST_ICE,					// 87 �ñ� ����
		APPLY_RESIST_EARTH,					// 88 ���� ����
		APPLY_RESIST_DARK,					// 89 ��� ����

		APPLY_ANTI_CRITICAL_PCT,			// 90 ũ��Ƽ�� ����
		APPLY_ANTI_PENETRATE_PCT,			// 91 ����Ÿ�� ����

		APPLY_BLEEDING_REDUCE = 92,			// 92
		APPLY_BLEEDING_PCT = 93,			// 93
		APPLY_ATTBONUS_WOLFMAN = 94,		// 94
		APPLY_RESIST_WOLFMAN = 95,			// 95
		APPLY_RESIST_CLAW = 96,				// 96

		APPLY_ACCEDRAIN_RATE,
		APPLY_RESIST_MAGIC_REDUCTION,

		APPLY_ENCHANT_ELECT,
		APPLY_ENCHANT_FIRE,
		APPLY_ENCHANT_ICE,
		APPLY_ENCHANT_WIND,
		APPLY_ENCHANT_EARTH,
		APPLY_ENCHANT_DARK,

		APPLY_ATTBONUS_CZ,
		APPLY_ATTBONUS_INSECT,
		APPLY_ATTBONUS_DESERT,
		APPLY_ATTBONUS_SWORD,
		APPLY_ATTBONUS_TWOHAND,
		APPLY_ATTBONUS_DAGGER,
		APPLY_ATTBONUS_BELL,
		APPLY_ATTBONUS_FAN,
		APPLY_ATTBONUS_BOW,
		APPLY_ATTBONUS_CLAW,

		APPLY_RESIST_HUMAN,
		APPLY_RESIST_MOUNT_FALL,
		APPLY_RESIST_FIST,

		APPLY_MOUNT,

		APPLY_SKILL_DAMAGE_SAMYEON,
		APPLY_SKILL_DAMAGE_TANHWAN,
		APPLY_SKILL_DAMAGE_PALBANG,
		APPLY_SKILL_DAMAGE_GIGONGCHAM,
		APPLY_SKILL_DAMAGE_GYOKSAN,
		APPLY_SKILL_DAMAGE_GEOMPUNG,
		APPLY_SKILL_DAMAGE_AMSEOP,
		APPLY_SKILL_DAMAGE_GUNGSIN,
		APPLY_SKILL_DAMAGE_CHARYUN,
		APPLY_SKILL_DAMAGE_SANGONG,
		APPLY_SKILL_DAMAGE_YEONSA,
		APPLY_SKILL_DAMAGE_KWANKYEOK,
		APPLY_SKILL_DAMAGE_GIGUNG,
		APPLY_SKILL_DAMAGE_HWAJO,
		APPLY_SKILL_DAMAGE_SWAERYUNG,
		APPLY_SKILL_DAMAGE_YONGKWON,
		APPLY_SKILL_DAMAGE_PABEOB,
		APPLY_SKILL_DAMAGE_MARYUNG,
		APPLY_SKILL_DAMAGE_HWAYEOMPOK,
		APPLY_SKILL_DAMAGE_MAHWAN,
		APPLY_SKILL_DAMAGE_BIPABU,
		APPLY_SKILL_DAMAGE_YONGBI,
		APPLY_SKILL_DAMAGE_PAERYONG,
		APPLY_SKILL_DAMAGE_NOEJEON,
		APPLY_SKILL_DAMAGE_BYEURAK,
		APPLY_SKILL_DAMAGE_CHAIN,
		APPLY_SKILL_DAMAGE_CHAYEOL,
		APPLY_SKILL_DAMAGE_SALPOONG,
		APPLY_SKILL_DAMAGE_GONGDAB,
		APPLY_SKILL_DAMAGE_PASWAE,

		APPLY_NORMAL_HIT_DEFEND_BONUS_BOSS_OR_MORE,
		APPLY_SKILL_DEFEND_BONUS_BOSS_OR_MORE,
		APPLY_NORMAL_HIT_DAMAGE_BONUS_BOSS_OR_MORE,
		APPLY_SKILL_DAMAGE_BONUS_BOSS_OR_MORE,

		APPLY_HIT_BUFF_ENCHANT_FIRE,
		APPLY_HIT_BUFF_ENCHANT_ICE,
		APPLY_HIT_BUFF_ENCHANT_ELEC,
		APPLY_HIT_BUFF_ENCHANT_WIND,
		APPLY_HIT_BUFF_ENCHANT_DARK,
		APPLY_HIT_BUFF_ENCHANT_EARTH,
		APPLY_HIT_BUFF_RESIST_FIRE,
		APPLY_HIT_BUFF_RESIST_ICE,
		APPLY_HIT_BUFF_RESIST_ELEC,
		APPLY_HIT_BUFF_RESIST_WIND,
		APPLY_HIT_BUFF_RESIST_DARK,
		APPLY_HIT_BUFF_RESIST_EARTH,

		APPLY_USE_SKILL_CHEONGRANG_MOV_SPEED,
		APPLY_USE_SKILL_CHEONGRANG_CASTING_SPEED,
		APPLY_USE_SKILL_CHAYEOL_CRITICAL_PCT,
		APPLY_USE_SKILL_SANGONG_ATT_GRADE_BONUS,
		APPLY_USE_SKILL_GIGUNG_ATT_GRADE_BONUS,
		APPLY_USE_SKILL_JEOKRANG_DEF_BONUS,
		APPLY_USE_SKILL_GWIGEOM_DEF_BONUS,
		APPLY_USE_SKILL_TERROR_ATT_GRADE_BONUS,
		APPLY_USE_SKILL_MUYEONG_ATT_GRADE_BONUS,
		APPLY_USE_SKILL_MANASHILED_CASTING_SPEED,
		APPLY_USE_SKILL_HOSIN_DEF_BONUS,
		APPLY_USE_SKILL_GICHEON_ATT_GRADE_BONUS,
		APPLY_USE_SKILL_JEONGEOP_ATT_GRADE_BONUS,
		APPLY_USE_SKILL_JEUNGRYEOK_DEF_BONUS,
		APPLY_USE_SKILL_GIHYEOL_ATT_GRADE_BONUS,
		APPLY_USE_SKILL_CHUNKEON_CASTING_SPEED,
		APPLY_USE_SKILL_NOEGEOM_ATT_GRADE_BONUS,

		APPLY_SKILL_DURATION_INCREASE_EUNHYUNG,
		APPLY_SKILL_DURATION_INCREASE_GYEONGGONG,
		APPLY_SKILL_DURATION_INCREASE_GEOMKYUNG,
		APPLY_SKILL_DURATION_INCREASE_JEOKRANG,

		APPLY_USE_SKILL_PALBANG_HP_ABSORB,
		APPLY_USE_SKILL_AMSEOP_HP_ABSORB,
		APPLY_USE_SKILL_YEONSA_HP_ABSORB,
		APPLY_USE_SKILL_YONGBI_HP_ABSORB,
		APPLY_USE_SKILL_CHAIN_HP_ABSORB,
		APPLY_USE_SKILL_PASWAE_SP_ABSORB,
		APPLY_USE_SKILL_GIGONGCHAM_STUN,
		APPLY_USE_SKILL_CHARYUN_STUN,
		APPLY_USE_SKILL_PABEOB_STUN,
		APPLY_USE_SKILL_MAHWAN_STUN,
		APPLY_USE_SKILL_GONGDAB_STUN,
		APPLY_USE_SKILL_SAMYEON_STUN,
		APPLY_USE_SKILL_GYOKSAN_KNOCKBACK,
		APPLY_USE_SKILL_SEOMJEON_KNOCKBACK,
		APPLY_USE_SKILL_SWAERYUNG_KNOCKBACK,
		APPLY_USE_SKILL_HWAYEOMPOK_KNOCKBACK,
		APPLY_USE_SKILL_GONGDAB_KNOCKBACK,
		APPLY_USE_SKILL_KWANKYEOK_KNOCKBACK,
		APPLY_USE_SKILL_SAMYEON_NEXT_COOLTIME_DECREASE_10PER,
		APPLY_USE_SKILL_GEOMPUNG_NEXT_COOLTIME_DECREASE_10PER,
		APPLY_USE_SKILL_GUNGSIN_NEXT_COOLTIME_DECREASE_10PER,
		APPLY_USE_SKILL_KWANKYEOK_NEXT_COOLTIME_DECREASE_10PER,
		APPLY_USE_SKILL_YONGKWON_NEXT_COOLTIME_DECREASE_10PER,
		APPLY_USE_SKILL_MARYUNG_NEXT_COOLTIME_DECREASE_10PER,
		APPLY_USE_SKILL_BIPABU_NEXT_COOLTIME_DECREASE_10PER,
		APPLY_USE_SKILL_NOEJEON_NEXT_COOLTIME_DECREASE_10PER,
		APPLY_USE_SKILL_SALPOONG_NEXT_COOLTIME_DECREASE_10PER,
		APPLY_USE_SKILL_PASWAE_NEXT_COOLTIME_DECREASE_10PER,

		APPLY_ATTBONUS_STONE,

		APPLY_DAMAGE_HP_RECOVERY,
		APPLY_DAMAGE_SP_RECOVERY,

		APPLY_ALIGNMENT_DAMAGE_BONUS,

		APPLY_NORMAL_DAMAGE_GUARD,
		APPLY_MORE_THEN_HP90_DAMAGE_REDUCE,

		APPLY_USE_SKILL_TUSOK_HP_ABSORB,
		APPLY_USE_SKILL_PAERYONG_HP_ABSORB,
		APPLY_USE_SKILL_BYEURAK_HP_ABSORB,

		APPLY_FIRST_ATTRIBUTE_BONUS,
		APPLY_SECOND_ATTRIBUTE_BONUS,
		APPLY_THIRD_ATTRIBUTE_BONUS,
		APPLY_FOURTH_ATTRIBUTE_BONUS,
		APPLY_FIFTH_ATTRIBUTE_BONUS,

		APPLY_USE_SKILL_SAMYEON_NEXT_COOLTIME_DECREASE_20PER,
		APPLY_USE_SKILL_GEOMPUNG_NEXT_COOLTIME_DECREASE_20PER,
		APPLY_USE_SKILL_GUNGSIN_NEXT_COOLTIME_DECREASE_20PER,
		APPLY_USE_SKILL_KWANKYEOK_NEXT_COOLTIME_DECREASE_20PER,
		APPLY_USE_SKILL_YONGKWON_NEXT_COOLTIME_DECREASE_20PER,
		APPLY_USE_SKILL_MARYUNG_NEXT_COOLTIME_DECREASE_20PER,
		APPLY_USE_SKILL_BIPABU_NEXT_COOLTIME_DECREASE_20PER,
		APPLY_USE_SKILL_NOEJEON_NEXT_COOLTIME_DECREASE_20PER,
		APPLY_USE_SKILL_SALPOONG_NEXT_COOLTIME_DECREASE_20PER,
		APPLY_USE_SKILL_PASWAE_NEXT_COOLTIME_DECREASE_20PER,
		APPLY_USE_SKILL_CHAYEOL_HP_ABSORB,

		APPLY_SUNGMA_STR,
		APPLY_SUNGMA_HP,
		APPLY_SUNGMA_MOVE,
		APPLY_SUNGMA_IMMUNE,

		APPLY_HIT_PCT,
		APPLY_RANDOM,

		APPLY_ATTBONUS_PER_HUMAN,
		APPLY_ATTBONUS_PER_ANIMAL,
		APPLY_ATTBONUS_PER_ORC,
		APPLY_ATTBONUS_PER_MILGYO,
		APPLY_ATTBONUS_PER_UNDEAD,
		APPLY_ATTBONUS_PER_DEVIL,

		APPLY_ENCHANT_PER_ELECT,
		APPLY_ENCHANT_PER_FIRE,
		APPLY_ENCHANT_PER_ICE,
		APPLY_ENCHANT_PER_WIND,
		APPLY_ENCHANT_PER_EARTH,
		APPLY_ENCHANT_PER_DARK,

		APPLY_ATTBONUS_PER_CZ,
		APPLY_ATTBONUS_PER_INSECT,
		APPLY_ATTBONUS_PER_DESERT,
		APPLY_ATTBONUS_PER_STONE,
		APPLY_ATTBONUS_PER_MONSTER,

		APPLY_RESIST_PER_HUMAN,
		APPLY_RESIST_PER_ICE,
		APPLY_RESIST_PER_DARK,
		APPLY_RESIST_PER_EARTH,
		APPLY_RESIST_PER_FIRE,
		APPLY_RESIST_PER_ELEC,
		APPLY_RESIST_PER_MAGIC,
		APPLY_RESIST_PER_WIND,

		APPLY_HIT_BUFF_SUNGMA_STR,
		APPLY_HIT_BUFF_SUNGMA_MOVE,
		APPLY_HIT_BUFF_SUNGMA_HP,
		APPLY_HIT_BUFF_SUNGMA_IMMUNE,

		APPLY_MOUNT_MELEE_MAGIC_ATTBONUS_PER,
		APPLY_DISMOUNT_MOVE_SPEED_BONUS_PER,

		APPLY_HIT_AUTO_HP_RECOVERY,
		APPLY_HIT_AUTO_SP_RECOVERY,

		APPLY_USE_SKILL_COOLTIME_DECREASE_ALL,

		APPLY_HIT_STONE_ATTBONUS_STONE,
		APPLY_HIT_STONE_DEF_GRADE_BONUS,

		APPLY_KILL_BOSS_ITEM_BONUS,
		APPLY_MOB_HIT_MOB_AGGRESSIVE,
		APPLY_NO_DEATH_AND_HP_RECOVERY30,

		APPLY_AUTO_PICKUP,
		APPLY_MOUNT_NO_KNOCKBACK,

		APPLY_SUNGMA_PER_STR,
		APPLY_SUNGMA_PER_HP,
		APPLY_SUNGMA_PER_MOVE,
		APPLY_SUNGMA_PER_IMMUNE,

		APPLY_IMMUNE_POISON100,
		APPLY_IMMUNE_BLEEDING100,

		APPLY_MONSTER_DEFEND_BONUS,

		MAX_APPLY_NUM,
	};

	enum EWeddingItem
	{
		WEDDING_TUXEDO1 = 11901,
		WEDDING_TUXEDO2 = 11902,
		WEDDING_BRIDE_DRESS1 = 11903,
		WEDDING_BRIDE_DRESS2 = 11904,
		WEDDING_TUXEDO3 = 11911,
		WEDDING_TUXEDO4 = 11912,
		WEDDING_BRIDE_DRESS3 = 11913,
		WEDDING_BRIDE_DRESS4 = 11914,
		WEDDING_BOUQUET1 = 50201,
		WEDDING_BOUQUET2 = 50202,
	};

#if defined(ENABLE_SOULBIND_SYSTEM)
	enum ESealDate
	{
		E_SEAL_DATE_DEFAULT_TIMESTAMP = 0, // Empty Seal Date
		U_SEAL_DATE_DEFAULT_TIMESTAMP = -1, // Unlimited Seal Date
	};

	enum ESealItemVnum
	{
		SEAL_ITEM_BINDING_VNUM = 50263,
		SEAL_ITEM_UNBINDING_VNUM = 50264,
#if defined(ENABLE_UN_SEAL_SCROLL_PLUS)
		SEAL_ITEM_UNBINDING_VNUM_MALL = 50262,
#endif
	};
#endif

#if defined(ENABLE_APPLY_RANDOM)
	enum EItemMinMaxSocket
	{
		ITEM_VALUE_DEF_MIN_INDEX = 6,
		ITEM_VALUE_MTK_MIN_INDEX = 6,
		ITEM_VALUE_MTK_MAX_INDEX = 7,
		ITEM_VALUE_ATK_MIN_INDEX = 8,
		ITEM_VALUE_ATK_MAX_INDEX = 9,

		ITEM_SOCKET_DEF_MINMAX_RANDOM = 3,
		ITEM_SOCKET_BELT_DEF_MINMAX_RANDOM = 3,
		ITEM_SOCKET_MTK_MINMAX_RANDOM = 3,
		ITEM_SOCKET_ATK_MINMAX_RANDOM = 4,
		ITEM_VALUE_MINMAX_RANDOM_DIVISION_VALUE = 100000,
	};
#endif

#pragma pack(push)
#pragma pack(1)
	typedef struct SItemLimit
	{
		BYTE bType;
		long lValue;
	} TItemLimit;

	typedef struct SItemApply
	{
		POINT_TYPE wType;
		POINT_VALUE lValue;
	} TItemApply;

	typedef struct SItemTable
	{
		DWORD dwVnum;
		DWORD dwVnumRange;
		char szName[ITEM_NAME_MAX_LEN + 1];
		char szLocaleName[ITEM_NAME_MAX_LEN + 1];
		BYTE bType;
		BYTE bSubType;

		BYTE bWeight;
		BYTE bSize;

		uint64_t ullAntiFlags;
		DWORD dwFlags;
		DWORD dwWearFlags;
		DWORD dwImmuneFlag;

		DWORD dwIBuyItemPrice;
		DWORD dwISellItemPrice;

		TItemLimit aLimits[ITEM_LIMIT_MAX_NUM];
		TItemApply aApplies[ITEM_APPLY_MAX_NUM];
		long alValues[ITEM_VALUES_MAX_NUM];
		long alSockets[ITEM_SOCKET_MAX_NUM];
		DWORD dwRefinedVnum;
		WORD wRefineSet;
		DWORD dw67AttrMaterial;
		BYTE bAlterToMagicItemPct;
		BYTE bSpecular;
		BYTE bGainSocketPct;
	} TItemTable;

#if defined(ENABLE_ACCE_COSTUME_SYSTEM)
	struct SScaleInfo
	{
		float fScaleX, fScaleY, fScaleZ;
		float fPositionX, fPositionY, fPositionZ;
	};

	typedef struct SScaleTable
	{
		SScaleInfo tInfo[10];
	} TScaleTable;
#endif

	/*
	typedef struct SItemTable
	{
		DWORD dwVnum;
		char szItemName[ITEM_NAME_MAX_LEN + 1];
		BYTE bType;
		BYTE bSubType;
		BYTE bSize;
		DWORD dwAntiFlags;
		DWORD dwFlags;
		DWORD dwWearFlags;
		DWORD dwIBuyItemPrice;
		DWORD dwISellItemPrice;
		TItemLimit aLimits[ITEM_LIMIT_MAX_NUM];
		TItemApply aApplies[ITEM_APPLY_MAX_NUM];
		long alValues[ITEM_VALUES_MAX_NUM];
		long alSockets[ITEM_SOCKET_MAX_NUM];
		DWORD dwRefinedVnum;
		BYTE bSpecular;
		DWORD dwIconNumber;
	} TItemTable;
	*/
#pragma pack(pop)

public:
	CItemData();
	virtual ~CItemData();

	void Clear();
	void SetSummary(const std::string& c_rstSumm);
	void SetDescription(const std::string& c_rstDesc);

	CGraphicThing* GetModelThing();
	CGraphicThing* GetSubModelThing();
	CGraphicThing* GetDropModelThing();
	CGraphicSubImage* GetIconImage();

	DWORD GetLODModelThingCount();
	BOOL GetLODModelThingPointer(DWORD dwIndex, CGraphicThing** ppModelThing);

	DWORD GetAttachingDataCount();
	BOOL GetCollisionDataPointer(DWORD dwIndex, const NRaceData::TAttachingData** c_ppAttachingData);
	BOOL GetAttachingDataPointer(DWORD dwIndex, const NRaceData::TAttachingData** c_ppAttachingData);

	/////
	const TItemTable* GetTable() const;
	DWORD GetIndex() const;
	const char* GetName() const;
	const char* GetDescription() const;
	const char* GetSummary() const;
	BYTE GetType() const;
	BYTE GetSubType() const;
	uint64_t GetAntiFlags() const;
	UINT GetRefine() const;
	const char* GetUseTypeString() const;
	DWORD GetWeaponType() const;
	BYTE GetSize() const;
	BOOL IsAntiFlag(uint64_t ullFlag) const;
	BOOL IsFlag(DWORD dwFlag) const;
	BOOL IsWearableFlag(DWORD dwFlag) const;
	BOOL IsWearableByJob(BYTE bJob) const;
	BOOL HasNextGrade() const;
	DWORD GetRefineVNum() const;
	DWORD Get67AttrMaterial() const;
	DWORD GetWearFlags() const;
	DWORD GetIBuyItemPrice() const;
	DWORD GetISellItemPrice() const;
	long GetLevelLimit() const;
	BOOL GetLimit(BYTE byIndex, TItemLimit* pItemLimit) const;
	BOOL GetApply(BYTE byIndex, TItemApply* pItemApply) const;
	POINT_VALUE FindApplyValue(POINT_TYPE wApplyType);
	long GetValue(BYTE byIndex) const;
	long GetSocket(BYTE byIndex) const;
	long SetSocket(BYTE byIndex, DWORD value);
	int GetSocketCount() const;
	DWORD GetIconNumber() const;

	UINT GetSpecularPoweru() const;
	float GetSpecularPowerf() const;

	/////

	BOOL IsEquipment() const;
#if defined(ENABLE_SOULBIND_SYSTEM)
	bool CanSealItem() const;
#endif

	/////

	//BOOL LoadItemData(const char* c_szFileName);
	void SetDefaultItemData(const char* c_szIconFileName, const char* c_szModelFileName = NULL);
	void SetItemTableData(TItemTable* pItemTable);

	/////
	// Mappers
public:

	//
	// �Ǽ����� ���� ó��.
	//
	bool IsAccessoryForSocket() const
	{
		return (GetType() == ITEM_TYPE_ARMOR && (GetSubType() == ARMOR_WRIST || GetSubType() == ARMOR_NECK || GetSubType() == ARMOR_EAR)) ||
			(GetType() == ITEM_TYPE_BELT); // 2013�� 2�� ���� �߰��� '��Ʈ' �������� ��� ��ȹ������ �Ǽ����� ���� �ý����� �״�� �̿����ڰ� ��.
	}

	bool IsRemovableSocket() const
	{
		if (GetType() == ITEM_TYPE_ARMOR)
			return GetSubType() == ARMOR_BODY
#if defined(ENABLE_GLOVE_SYSTEM)
			|| GetSubType() == ARMOR_GLOVE
#endif
			;

		if (GetType() == ITEM_TYPE_WEAPON)
			return !(GetSubType() == WEAPON_ARROW
				|| GetSubType() == WEAPON_MOUNT_SPEAR
#if defined(ENABLE_QUIVER_SYSTEM)
				|| GetSubType() == WEAPON_QUIVER
#endif
				|| GetSubType() == WEAPON_BOUQUET);

		return false;
	}

	// Costume
	bool IsCostume() const { return GetType() == ITEM_TYPE_COSTUME; }
	bool IsCostumeBody() const { return GetType() == ITEM_TYPE_COSTUME && GetSubType() == COSTUME_BODY; }
	bool IsCostumeHair() const { return GetType() == ITEM_TYPE_COSTUME && GetSubType() == COSTUME_HAIR; }
#if defined(ENABLE_MOUNT_COSTUME_SYSTEM)
	bool IsCostumeMount() const { return GetType() == ITEM_TYPE_COSTUME && GetSubType() == COSTUME_MOUNT; }
#endif
#if defined(ENABLE_ACCE_COSTUME_SYSTEM)
	bool IsCostumeAcce() const { return GetType() == ITEM_TYPE_COSTUME && GetSubType() == COSTUME_ACCE; }
#endif
#if defined(ENABLE_WEAPON_COSTUME_SYSTEM)
	bool IsCostumeWeapon() const { return GetType() == ITEM_TYPE_COSTUME && GetSubType() == COSTUME_WEAPON; }
#endif
#if defined(ENABLE_AURA_COSTUME_SYSTEM)
	bool IsCostumeAura() const { return GetType() == ITEM_TYPE_COSTUME && GetSubType() == COSTUME_AURA; }
#endif

	// Weapon
	bool IsWeapon() const { return GetType() == ITEM_TYPE_WEAPON; }
	bool IsMainWeapon() const
	{
		return GetType() == ITEM_TYPE_WEAPON && (
			GetSubType() == WEAPON_SWORD
			|| GetSubType() == WEAPON_DAGGER
			|| GetSubType() == WEAPON_BOW
			|| GetSubType() == WEAPON_TWO_HANDED
			|| GetSubType() == WEAPON_BELL
			|| GetSubType() == WEAPON_FAN
			|| GetSubType() == WEAPON_CLAW
			);
	}
	bool IsWeaponSword() const { return GetType() == ITEM_TYPE_WEAPON && GetSubType() == WEAPON_SWORD; }
	bool IsWeaponDagger() const { return GetType() == ITEM_TYPE_WEAPON && GetSubType() == WEAPON_DAGGER; }
	bool IsWeaponBow() const { return GetType() == ITEM_TYPE_WEAPON && GetSubType() == WEAPON_BOW; }
	bool IsWeaponTwoHandSword() const { return GetType() == ITEM_TYPE_WEAPON && GetSubType() == WEAPON_TWO_HANDED; }
	bool IsWeaponBell() const { return GetType() == ITEM_TYPE_WEAPON && GetSubType() == WEAPON_BELL; }
	bool IsWeaponFan() const { return GetType() == ITEM_TYPE_WEAPON && GetSubType() == WEAPON_FAN; }
	bool IsWeaponArrow() const { return GetType() == ITEM_TYPE_WEAPON && GetSubType() == WEAPON_ARROW; }
	bool IsWeaponMountSpear() const { return GetType() == ITEM_TYPE_WEAPON && GetSubType() == WEAPON_MOUNT_SPEAR; }
#if defined(ENABLE_QUIVER_SYSTEM)
	bool IsWeaponQuiver() const { return GetType() == ITEM_TYPE_WEAPON && GetSubType() == WEAPON_QUIVER; }
#endif
	bool IsWeaponClaw() const { return GetType() == ITEM_TYPE_WEAPON && GetSubType() == WEAPON_CLAW; }
	bool IsWeaponBouquet() const { return GetType() == ITEM_TYPE_WEAPON && GetSubType() == WEAPON_BOUQUET; }

	// Armor
	bool IsArmor() const { return GetType() == ITEM_TYPE_ARMOR; }
	bool IsArmorBody() const { return GetType() == ITEM_TYPE_ARMOR && GetSubType() == ARMOR_BODY; }
	bool IsArmorHead() const { return GetType() == ITEM_TYPE_ARMOR && GetSubType() == ARMOR_HEAD; }
	bool IsArmorShield() const { return GetType() == ITEM_TYPE_ARMOR && GetSubType() == ARMOR_SHIELD; }
	bool IsArmorWrist() const { return GetType() == ITEM_TYPE_ARMOR && GetSubType() == ARMOR_WRIST; }
	bool IsArmorFoots() const { return GetType() == ITEM_TYPE_ARMOR && GetSubType() == ARMOR_FOOTS; }
	bool IsArmorNecklace() const { return GetType() == ITEM_TYPE_ARMOR && GetSubType() == ARMOR_NECK; }
	bool IsArmorEarRing() const { return GetType() == ITEM_TYPE_ARMOR && GetSubType() == ARMOR_EAR; }
#if defined(ENABLE_PENDANT)
	bool IsArmorPendant() const { return GetType() == ITEM_TYPE_ARMOR && GetSubType() == ARMOR_PENDANT; }
#endif
#if defined(ENABLE_GLOVE_SYSTEM)
	bool IsArmorGlove() const { return GetType() == ITEM_TYPE_ARMOR && GetSubType() == ARMOR_GLOVE; }
#endif

	// Unique
	bool IsBelt() const { return GetType() == ITEM_TYPE_BELT; }
	bool IsRing() const { return GetType() == ITEM_TYPE_RING; }
	/////

protected:
	void __LoadFiles();
	void __SetIconImage(const char* c_szFileName);

protected:
	std::string m_strModelFileName;
	std::string m_strSubModelFileName;
	std::string m_strDropModelFileName;
	std::string m_strIconFileName;
	std::string m_strDescription;
	std::string m_strSummary;
	std::vector<std::string> m_strLODModelFileNameVector;

	CGraphicThing* m_pModelThing;
	CGraphicThing* m_pSubModelThing;
	CGraphicThing* m_pDropModelThing;
	CGraphicSubImage* m_pIconImage;
	std::vector<CGraphicThing*> m_pLODModelThingVector;

	NRaceData::TAttachingDataVector m_AttachingDataVector;
	DWORD m_dwVnum;
	TItemTable m_ItemTable;
#if defined(ENABLE_ACCE_COSTUME_SYSTEM) || defined(ENABLE_AURA_COSTUME_SYSTEM)
	TScaleTable m_ScaleTable;
#endif

public:
	static void DestroySystem();

	static CItemData* New();
	static void Delete(CItemData* pkItemData);

	static CDynamicPool<CItemData> ms_kPool;

#if defined(ENABLE_ACCE_COSTUME_SYSTEM) || defined(ENABLE_AURA_COSTUME_SYSTEM)
public:
	enum EAcceGradeType
	{
		ACCE_GRADE_NONE,
		ACCE_GRADE_BASIC,
		ACCE_GRADE_FINE,
		ACCE_GRADE_NOBLE,
		ACCE_GRADE_CUSTOM,
		ACCE_GRADE_MAX_NUM,
	};

	void SetItemTableScaleData(BYTE bJob, BYTE bSex, float fScaleX, float fScaleY, float fScaleZ, float fScaleParticle);
	D3DXVECTOR3& GetItemScaleVector(BYTE bJob, BYTE bSex);
	float GetItemParticleScale(BYTE bJob, BYTE bSex);

protected:
	typedef struct SItemScaleTable
	{
		D3DXVECTOR3 v3Scale[NRaceData::SEX_MAX_NUM][NRaceData::JOB_MAX_NUM];
		float fScaleParticle[NRaceData::SEX_MAX_NUM][NRaceData::JOB_MAX_NUM];
	} TItemScaleTable;
	TItemScaleTable m_ItemScaleTable;
#endif

#if defined(ENABLE_AURA_COSTUME_SYSTEM)
public:
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

	void SetAuraEffectID(const char* szAuraEffectPath);
	DWORD GetAuraEffectID() const { return m_dwAuraEffectID; }

protected:
	DWORD m_dwAuraEffectID;
#endif
};
