#include <math.h>
#include "ItemCSVReader.h"

using namespace std;

inline string trim_left(const string& str)
{
	string::size_type n = str.find_first_not_of(" \t\v\n\r");
	return n == string::npos ? str : str.substr(n, str.length());
}

inline string trim_right(const string& str)
{
	string::size_type n = str.find_last_not_of(" \t\v\n\r");
	return n == string::npos ? str : str.substr(0, n + 1);
}

string trim(const string& str)
{
	return trim_left(trim_right(str));
}

static string* StringSplit(string strOrigin, string strTok)
{
	int cutAt; // 자르는위치
	int index = 0; // 문자열인덱스
	string* strResult = new string[30]; // 결과return 할변수

	// strTok을찾을때까지반복
	while ((cutAt = strOrigin.find_first_of(strTok)) != strOrigin.npos)
	{
		if (cutAt > 0) // 자르는위치가0보다크면(성공시)
		{
			strResult[index++] = strOrigin.substr(0, cutAt); // 결과배열에추가
		}
		strOrigin = strOrigin.substr(cutAt + 1); // 원본은자른부분제외한나머지
	}

	if (strOrigin.length() > 0) // 원본이아직남았으면
	{
		strResult[index++] = strOrigin.substr(0, cutAt); // 나머지를결과배열에추가
	}

	for (int i = 0; i < index; i++)
	{
		strResult[i] = trim(strResult[i]);
	}

	return strResult; // 결과return
}

int get_Item_Type_Value(string inputString)
{
	string arType[] = {
		"ITEM_NONE",
		"ITEM_WEAPON",
		"ITEM_ARMOR",
		"ITEM_USE",
		"ITEM_AUTOUSE",
		"ITEM_MATERIAL",
		"ITEM_SPECIAL",
		"ITEM_TOOL",
		"ITEM_LOTTERY",
		"ITEM_ELK",
		"ITEM_METIN",
		"ITEM_CONTAINER",
		"ITEM_FISH",
		"ITEM_ROD",
		"ITEM_RESOURCE",
		"ITEM_CAMPFIRE",
		"ITEM_UNIQUE",
		"ITEM_SKILLBOOK",
		"ITEM_QUEST",
		"ITEM_POLYMORPH",
		"ITEM_TREASURE_BOX",
		"ITEM_TREASURE_KEY",
		"ITEM_SKILLFORGET",
		"ITEM_GIFTBOX",
		"ITEM_PICK",
		"ITEM_HAIR",
		"ITEM_TOTEM",
		"ITEM_BLEND",
		"ITEM_COSTUME",
		"ITEM_DS",
		"ITEM_SPECIAL_DS",
		"ITEM_EXTRACT",
		"ITEM_SECONDARY_COIN",
		"ITEM_RING",
		"ITEM_BELT",
		"ITEM_PET",
#if defined(ENABLE_MOVE_COSTUME_ATTR)
		"ITEM_MEDIUM",
#endif
#if defined(ENABLE_GACHA)
		"ITEM_GACHA",
#endif
		"ITEM_PASSIVE",
		"ITEM_MERCENARY",
		"ITEM_ALCHEMY",
#if defined(ENABLE_SOUL_SYSTEM)
		"ITEM_SOUL",
#endif
	};

	int retInt = -1;
	//cout << "Type : " << typeStr << " -> ";
	for (int j = 0; j < sizeof(arType) / sizeof(arType[0]); j++)
	{
		string tempString = arType[j];
		if (inputString.find(tempString) != string::npos && tempString.find(inputString) != string::npos)
		{
			//cout << j << " ";
			retInt = j;
			break;
		}
	}
	//cout << endl;

	return retInt;
}

int get_Item_SubType_Value(int type_value, string inputString)
{
	string arSub1[] = {
		"WEAPON_SWORD",
		"WEAPON_DAGGER",
		"WEAPON_BOW",
		"WEAPON_TWO_HANDED",
		"WEAPON_BELL",
		"WEAPON_FAN",
		"WEAPON_ARROW",
		"WEAPON_MOUNT_SPEAR",
		"WEAPON_CLAW",
#if defined(ENABLE_QUIVER_SYSTEM)
		"WEAPON_QUIVER",
#endif
		"WEAPON_BOUQUET",
	};

	string arSub2[] = {
		"ARMOR_BODY",
		"ARMOR_HEAD",
		"ARMOR_SHIELD",
		"ARMOR_WRIST",
		"ARMOR_FOOTS",
		"ARMOR_NECK",
		"ARMOR_EAR",
#if defined(ENABLE_PENDANT)
		"ARMOR_PENDANT",
#endif
#if defined(ENABLE_GLOVE_SYSTEM)
		"ARMOR_GLOVE",
#endif
		"ARMOR_NUM_TYPES",
	};

	string arSub3[] = {
		"USE_POTION",
		"USE_TALISMAN",
		"USE_TUNING",
		"USE_MOVE",
		"USE_TREASURE_BOX",
		"USE_MONEYBAG",
		"USE_BAIT",
		"USE_ABILITY_UP",
		"USE_AFFECT",
		"USE_CREATE_STONE",
		"USE_SPECIAL",
		"USE_POTION_NODELAY",
		"USE_CLEAR",
		"USE_INVISIBILITY",
		"USE_DETACHMENT",
		"USE_BUCKET",
		"USE_POTION_CONTINUE",
		"USE_CLEAN_SOCKET",
		"USE_CHANGE_ATTRIBUTE",
		"USE_ADD_ATTRIBUTE",
		"USE_ADD_ACCESSORY_SOCKET",
		"USE_PUT_INTO_ACCESSORY_SOCKET",
		"USE_ADD_ATTRIBUTE2",
		"USE_RECIPE",
		"USE_CHANGE_ATTRIBUTE2",
		"USE_BIND",
		"USE_UNBIND",
		"USE_TIME_CHARGE_PER",
		"USE_TIME_CHARGE_FIX",
		"USE_PUT_INTO_BELT_SOCKET",
		"USE_PUT_INTO_RING_SOCKET",
#if defined(ENABLE_MOVE_COSTUME_ATTR)
		"USE_CHANGE_COSTUME_ATTR",
		"USE_RESET_COSTUME_ATTR",
#endif
		"USE_UNK_33",
#if defined(ENABLE_CHANGED_ATTR)
		"USE_SELECT_ATTRIBUTE",
#endif
		"USE_FLOWER",
#if defined(ENABLE_EXPRESSING_EMOTION)
		"USE_EMOTION_PACK",
#endif
#if defined(ENABLE_REFINE_ELEMENT_SYSTEM)
		"USE_ELEMENT_UPGRADE",
		"USE_ELEMENT_DOWNGRADE",
		"USE_ELEMENT_CHANGE",
#endif
		"USE_CALL",
		"USE_POTION_TOWER",
		"USE_POTION_NODELAY_TOWER",
		"USE_REMOVE_AFFECT",
		"USE_EMOTION_TOWER",
		"USE_SECRET_DUNGEON_SCROLL",
	};

	string arSub4[] = {
		"AUTOUSE_POTION",
		"AUTOUSE_ABILITY_UP",
		"AUTOUSE_BOMB",
		"AUTOUSE_GOLD",
		"AUTOUSE_MONEYBAG",
		"AUTOUSE_TREASURE_BOX",
	};

	string arSub5[] = {
		"MATERIAL_LEATHER",
		"MATERIAL_BLOOD",
		"MATERIAL_ROOT",
		"MATERIAL_NEEDLE",
		"MATERIAL_JEWEL",
		"MATERIAL_DS_REFINE_NORMAL",
		"MATERIAL_DS_REFINE_BLESSED",
		"MATERIAL_DS_REFINE_HOLLY",
#if defined(ENABLE_DS_CHANGE_ATTR)
		"MATERIAL_DS_CHANGE_ATTR",
#endif
		"MATERIAL_PASSIVE_WEAPON",
		"MATERIAL_PASSIVE_ARMOR",
		"MATERIAL_PASSIVE_ACCE",
		"MATERIAL_PASSIVE_ELEMENT",
	};

	string arSub6[] = {
		"SPECIAL_MAP",
		"SPECIAL_KEY",
		"SPECIAL_DOC",
		"SPECIAL_SPIRIT",
	};

	string arSub7[] = {
		"TOOL_FISHING_ROD"
	};

	string arSub8[] = {
		"LOTTERY_TICKET",
		"LOTTERY_INSTANT",
	};

	string arSub10[] = {
		"METIN_NORMAL",
		"METIN_SUNGMA",
	};

	string arSub12[] = {
		"FISH_ALIVE",
		"FISH_DEAD",
	};

	string arSub14[] = {
		"RESOURCE_FISHBONE",
		"RESOURCE_WATERSTONEPIECE",
		"RESOURCE_WATERSTONE",
		"RESOURCE_BLOOD_PEARL",
		"RESOURCE_BLUE_PEARL",
		"RESOURCE_WHITE_PEARL",
		"RESOURCE_BUCKET",
		"RESOURCE_CRYSTAL",
		"RESOURCE_GEM",
		"RESOURCE_STONE",
		"RESOURCE_METIN",
		"RESOURCE_ORE",
#if defined(ENABLE_AURA_COSTUME_SYSTEM)
		"RESOURCE_AURA",
#endif
	};

	string arSub16[] = {
		"UNIQUE_NONE",
		"UNIQUE_BOOK",
		"UNIQUE_SPECIAL_RIDE",
		"UNIQUE_3",
		"UNIQUE_4",
		"UNIQUE_5",
		"UNIQUE_6",
		"UNIQUE_7",
		"UNIQUE_8",
		"UNIQUE_9",
		"UNIQUE_BUNDLE",
	};

	string arSub18[] = {
		"QUEST_NONE",
		"QUEST_PET_PAY",
		"QUEST_WARP",
		"QUEST_GEM_BAG",
	};

	std::string arSub23[]
	{
		"GIFTBOX_NONE",
		"GIFTBOX_NORMAL",
		"GIFTBOX_SPECIAL",
	};

	string arSub28[] = {
		"COSTUME_BODY",
		"COSTUME_HAIR",
#if defined(ENABLE_MOUNT_COSTUME_SYSTEM)
		"COSTUME_MOUNT",
#endif
#if defined(ENABLE_ACCE_COSTUME_SYSTEM)
		"COSTUME_ACCE",
#endif
#if defined(ENABLE_WEAPON_COSTUME_SYSTEM)
		"COSTUME_WEAPON",
#endif
#if defined(ENABLE_AURA_COSTUME_SYSTEM)
		"COSTUME_AURA",
#endif
	};

	string arSub29[] = {
		"DS_SLOT1",
		"DS_SLOT2",
		"DS_SLOT3",
		"DS_SLOT4",
		"DS_SLOT5",
		"DS_SLOT6",
		"DS_SLOT7",
	};

	string arSub31[] = {
		"EXTRACT_DRAGON_SOUL",
		"EXTRACT_DRAGON_HEART",
	};

	string arSub35[] = {
		"PET_EGG",
		"PET_UPBRINGING",
		"PET_BAG",
		"PET_FEEDSTUFF",
		"PET_SKILL",
		"PET_SKILL_DEL_BOOK",
		"PET_NAME_CHANGE",
		"PET_EXPFOOD",
		"PET_SKILL_ALL_DEL_BOOK",
		"PET_EXPFOOD_PER",
		"PET_ATTR_DETERMINE",
		"PET_ATTR_CHANGE",
		"PET_PAY",
		"PET_PRIMIUM_FEEDSTUFF",
	};

#if defined(ENABLE_MOVE_COSTUME_ATTR)
	string arSub36[] = {
		"MEDIUM_MOVE_COSTUME_ATTR",
#if defined(ENABLE_ACCE_COSTUME_SYSTEM)
		"MEDIUM_MOVE_ACCE_ATTR",
#endif
	};
#endif

#if defined(ENABLE_GACHA)
	string arSub37[] = {
		"USE_GACHA",
#if defined(ENABLE_LUCKY_BOX)
		"GEM_LUCKY_BOX_GACHA",
		"SPECIAL_LUCKY_BOX_GACHA",
#endif
	};
#endif

	std::string arSub38[]
	{
		"PASSIVE_JOB",
	};

	string arSub39[] = {
		"MERCENARY_0",
		"MERCENARY_1",
		"MERCENARY_2",
		"MERCENARY_3",
		"MERCENARY_4",
		"MERCENARY_5",
		"MERCENARY_6",
	};

	std::string arSub40[]
	{
		"ARMOR",
		"WEAPON",
		"ACCESSORY",
		"BELT",
		"EVENT",
		"ETC",
		"JOB",
		"SETADD_WEAPON",
		"SETADD_ARMOR_BODY",
		"SETADD_ARMOR_HELMET",
		"PET",
		"SKILL_BOOK",
		"GLOVE",
	};

#if defined(ENABLE_SOUL_SYSTEM)
	string arSub41[] = {
		"SOUL_RED",
		"SOUL_BLUE"
	};
#endif

	static string* arSubType[] = {
		0,			// ITEM_NONE
		arSub1,		// ITEM_WEAPON
		arSub2,		// ITEM_ARMOR
		arSub3,		// ITEM_USE
		arSub4,		// ITEM_AUTOUSE
		arSub5,		// ITEM_MATERIAL
		arSub6,		// ITEM_SPECIAL
		arSub7,		// ITEM_TOOL
		arSub8,		// ITEM_LOTTERY
		0,			// ITEM_ELK
		arSub10,	// ITEM_METIN
		0,			// ITEM_CONTAINER
		arSub12,	// ITEM_FISH
		0,			// ITEM_ROD
		arSub14,	// ITEM_RESOURCE
		0,			// ITEM_CAMPFIRE
		arSub16,	// ITEM_UNIQUE
		0,			// ITEM_SKILLBOOK
		arSub18,	// ITEM_QUEST
		0,			// ITEM_POLYMORPH
		0,			// ITEM_TREASURE_BOX
		0,			// ITEM_TREASURE_KEY
		0,			// ITEM_SKILLFORGET
		arSub23,	// ITEM_GIFTBOX
		0,			// ITEM_PICK
		0,			// ITEM_HAIR
		0,			// ITEM_TOTEM
		0,			// ITEM_BLEND
		arSub28,	// ITEM_COSTUME
		arSub29,	// ITEM_DS
		arSub29,	// ITEM_SPECIAL_DS
		arSub31,	// ITEM_EXTRACT
		0,			// ITEM_SECONDARY_COIN
		0,			// ITEM_RING
		0,			// ITEM_BELT
		arSub35,	// ITEM_PET
#if defined(ENABLE_MOVE_COSTUME_ATTR)
		arSub36,	// ITEM_MEDIUM
#endif
#if defined(ENABLE_GACHA)
		arSub37,	// ITEM_GACHA
#endif
		arSub38,	// ITEM_PASSIVE
		arSub39,	// ITEM_MERCENARY
		arSub40,	// ITEM_ALCHEMY
#if defined(ENABLE_SOUL_SYSTEM)
		arSub41,	// ITEM_SOUL
#endif
	};

	static int arNumberOfSubtype[_countof(arSubType)] = {
		0,										// ITEM_NONE
		sizeof(arSub1) / sizeof(arSub1[0]),		// ITEM_WEAPON
		sizeof(arSub2) / sizeof(arSub2[0]),		// ITEM_ARMOR
		sizeof(arSub3) / sizeof(arSub3[0]),		// ITEM_USE
		sizeof(arSub4) / sizeof(arSub4[0]),		// ITEM_AUTOUSE
		sizeof(arSub5) / sizeof(arSub5[0]),		// ITEM_MATERIAL
		sizeof(arSub6) / sizeof(arSub6[0]),		// ITEM_SPECIAL
		sizeof(arSub7) / sizeof(arSub7[0]),		// ITEM_TOOL
		sizeof(arSub8) / sizeof(arSub8[0]),		// ITEM_LOTTERY
		0,										// ITEM_ELK
		sizeof(arSub10) / sizeof(arSub10[0]),	// ITEM_METIN
		0,										// ITEM_CONTAINER
		sizeof(arSub12) / sizeof(arSub12[0]),	// ITEM_FISH
		0,										// ITEM_ROD
		sizeof(arSub14) / sizeof(arSub14[0]),	// ITEM_RESOURCE
		0,										// ITEM_CAMPFIRE
		sizeof(arSub16) / sizeof(arSub16[0]),	// ITEM_UNIQUE
		0,										// ITEM_SKILLBOOK
		sizeof(arSub18) / sizeof(arSub18[0]),	// ITEM_QUEST
		0,										// ITEM_POLYMORPH
		0,										// ITEM_TREASURE_BOX
		0,										// ITEM_TREASURE_KEY
		0,										// ITEM_SKILLFORGET
		sizeof(arSub23) / sizeof(arSub23[0]),	// ITEM_GIFTBOX
		0,										// ITEM_PICK
		0,										// ITEM_HAIR
		0,										// ITEM_TOTEM
		0,										// ITEM_BLEND
		sizeof(arSub28) / sizeof(arSub28[0]),	// ITEM_COSTUME
		sizeof(arSub29) / sizeof(arSub29[0]),	// ITEM_DS
		sizeof(arSub29) / sizeof(arSub29[0]),	// ITEM_SPECIAL_DS
		sizeof(arSub31) / sizeof(arSub31[0]),	// ITEM_EXTRACT
		0,										// ITEM_SECONDARY_COIN
		0,										// ITEM_RING
		0,										// ITEM_BELT
		sizeof(arSub35) / sizeof(arSub35[0]),	// ITEM_PET
#if defined(ENABLE_MOVE_COSTUME_ATTR)
		sizeof(arSub36) / sizeof(arSub36[0]),	// ITEM_MEDIUM
#endif
#if defined(ENABLE_GACHA)
		sizeof(arSub37) / sizeof(arSub37[0]),	// ITEM_GACHA
#endif
		sizeof(arSub38) / sizeof(arSub38[0]),	// ITEM_PASSIVE
		sizeof(arSub39) / sizeof(arSub39[0]),	// ITEM_MERCENARY
		sizeof(arSub40) / sizeof(arSub40[0]),	// ITEM_ALCHEMY
#if defined(ENABLE_SOUL_SYSTEM)
		sizeof(arSub41) / sizeof(arSub41[0]),	// ITEM_SOUL
#endif
	};

	// 아이템 타입의 서브타입 어레이가 존재하는지 알아보고, 없으면 0 리턴
	if (arSubType[type_value] == 0)
	{
		return 0;
	}
	//

	int retInt = -1;
	//cout << "SubType : " << subTypeStr << " -> ";
	for (int j = 0; j < arNumberOfSubtype[type_value]; j++)
	{
		string tempString = arSubType[type_value][j];
		string tempInputString = trim(inputString);
		if (tempInputString.compare(tempString) == 0)
		{
			//cout << j << " ";
			retInt = j;
			break;
		}
	}
	//cout << endl;

	return retInt;
}

uint64_t get_Item_AntiFlag_Value(string inputString)
{
	string arAntiFlag[] = {
		"ANTI_FEMALE",
		"ANTI_MALE",
		"ANTI_MUSA",
		"ANTI_ASSASSIN",
		"ANTI_SURA",
		"ANTI_MUDANG",
		"ANTI_GET",
		"ANTI_DROP",
		"ANTI_SELL",
		"ANTI_EMPIRE_A",
		"ANTI_EMPIRE_B",
		"ANTI_EMPIRE_C",
		"ANTI_SAVE",
		"ANTI_GIVE",
		"ANTI_PKDROP",
		"ANTI_STACK",
		"ANTI_MYSHOP",
		"ANTI_SAFEBOX",
		"ANTI_WOLFMAN",
		"ANTI_RT_REMOVE",
		"ANTI_QUICKSLOT",
#if defined(ENABLE_CHANGE_LOOK_SYSTEM)
		"ANTI_CHANGELOOK",
#endif
		"ANTI_REINFORCE",
		"ANTI_ENCHANT",
		"ANTI_ENERGY",
		"ANTI_PETFEED",
		"ANTI_APPLY",
#if defined(ENABLE_ACCE_COSTUME_SYSTEM)
		"ANTI_ACCE",
#endif
#if defined(ENABLE_MAILBOX)
		"ANTI_MAIL",
#endif
		"ANTI_DESTROY" // Custom
	};

	uint64_t retValue = 0; //
	string* arInputString = StringSplit(inputString, "|"); // 프로토 정보 내용을 단어별로 쪼갠 배열.
	for (int i = 0; i < sizeof(arAntiFlag) / sizeof(arAntiFlag[0]); i++)
	{
		string tempString = arAntiFlag[i];
		for (int j = 0; j < 30; j++) // 최대 30개 단어까지. (하드코딩)
		{
			string tempString2 = arInputString[j];
			if (tempString2.compare(tempString) == 0) // 일치하는지 확인.
			{
				/* 2020.12.30.Owsap
				* NOTE : Changed DWORD -> uint64_t
				* With the addition of more anti-flags in the item_proto the sum of the "retValue" is bigger than
				* the limit the data type can contain, we expanded the limit to "unsigned long long".
				*/
				retValue = retValue + static_cast<uint64_t>(pow((float)2, (float)i));
			}

			if (tempString2.compare("") == 0)
				break;
		}
	}
	delete[]arInputString;
	//cout << "AntiFlag : " << antiFlagStr << " -> " << retValue << endl;

	return retValue;
}

int get_Item_Flag_Value(string inputString)
{
	string arFlag[] = {
		"ITEM_TUNABLE",
		"ITEM_SAVE",
		"ITEM_STACKABLE",
		"COUNT_PER_1GOLD",
		"ITEM_SLOW_QUERY",
		"ITEM_UNIQUE",
		"ITEM_MAKECOUNT",
		"ITEM_IRREMOVABLE",
		"CONFIRM_WHEN_USE",
		"QUEST_USE",
		"QUEST_USE_MULTIPLE",
		"QUEST_GIVE",
		"LOG",
		"ITEM_APPLICABLE",
	};

	int retValue = 0;
	string* arInputString = StringSplit(inputString, "|"); // 프로토 정보 내용을 단어별로 쪼갠 배열.
	for (int i = 0; i < sizeof(arFlag) / sizeof(arFlag[0]); i++)
	{
		string tempString = arFlag[i];
		for (int j = 0; j < 30; j++) // 최대 30개 단어까지. (하드코딩)
		{
			string tempString2 = arInputString[j];
			if (tempString2.compare(tempString) == 0) // 일치하는지 확인.
			{
				retValue = retValue + pow((float)2, (float)i);
			}

			if (tempString2.compare("") == 0)
				break;
		}
	}
	delete[]arInputString;
	//cout << "Flag : " << flagStr << " -> " << retValue << endl;

	return retValue;
}

int get_Item_WearFlag_Value(string inputString)
{
	string arWearrFlag[] = {
		"WEAR_BODY",
		"WEAR_HEAD",
		"WEAR_FOOTS",
		"WEAR_WRIST",
		"WEAR_WEAPON",
		"WEAR_NECK",
		"WEAR_EAR",
		"WEAR_UNIQUE",
		"WEAR_SHIELD",
		"WEAR_ARROW",
		"WEAR_HAIR",
#if defined(ENABLE_PENDANT)
		"WEAR_PENDANT",
#endif
#if defined(ENABLE_GLOVE_SYSTEM)
		"WEAR_GLOVE",
#endif
	};

	int retValue = 0;
	string* arInputString = StringSplit(inputString, "|"); // 프로토 정보 내용을 단어별로 쪼갠 배열.
	for (int i = 0; i < sizeof(arWearrFlag) / sizeof(arWearrFlag[0]); i++)
	{
		string tempString = arWearrFlag[i];
		for (int j = 0; j < 30; j++) // 최대 30개 단어까지. (하드코딩)
		{
			string tempString2 = arInputString[j];
			if (tempString2.compare(tempString) == 0) // 일치하는지 확인.
			{
				retValue = retValue + pow((float)2, (float)i);
			}

			if (tempString2.compare("") == 0)
				break;
		}
	}
	delete[]arInputString;
	//cout << "WearFlag : " << wearFlagStr << " -> " << retValue << endl;

	return retValue;
}

int get_Item_Immune_Value(string inputString)
{
	string arImmune[] = {
		"PARA",
		"CURSE",
		"STUN",
		"SLEEP",
		"SLOW",
		"POISON",
		"TERROR",
	};

	int retValue = 0;
	string* arInputString = StringSplit(inputString, "|"); // 프로토 정보 내용을 단어별로 쪼갠 배열.
	for (int i = 0; i < sizeof(arImmune) / sizeof(arImmune[0]); i++)
	{
		string tempString = arImmune[i];
		for (int j = 0; j < 30; j++) // 최대 30개 단어까지. (하드코딩)
		{
			string tempString2 = arInputString[j];
			if (tempString2.compare(tempString) == 0) // 일치하는지 확인.
			{
				retValue = retValue + pow((float)2, (float)i);
			}

			if (tempString2.compare("") == 0)
				break;
		}
	}
	delete[]arInputString;
	//cout << "Immune : " << immuneStr << " -> " << retValue << endl;

	return retValue;
}

int get_Item_LimitType_Value(string inputString)
{
	string arLimitType[] = {
		"LIMIT_NONE",
		"LEVEL",
		"STR",
		"DEX",
		"INT",
		"CON",
		"REAL_TIME",
		"REAL_TIME_FIRST_USE",
		"TIMER_BASED_ON_WEAR",
		"NEWWORLD_LEVEL",
		"DURATION",
	};

	int retInt = -1;
	//cout << "LimitType : " << limitTypeStr << " -> ";
	for (int j = 0; j < sizeof(arLimitType) / sizeof(arLimitType[0]); j++)
	{
		string tempString = arLimitType[j];
		string tempInputString = trim(inputString);
		if (tempInputString.compare(tempString) == 0)
		{
			//cout << j << " ";
			retInt = j;
			break;
		}
	}
	//cout << endl;

	return retInt;
}

int get_Item_ApplyType_Value(string inputString)
{
	string arApplyType[] = {
		"APPLY_NONE",
		"APPLY_MAX_HP",
		"APPLY_MAX_SP",
		"APPLY_CON",
		"APPLY_INT",
		"APPLY_STR",
		"APPLY_DEX",
		"APPLY_ATT_SPEED",
		"APPLY_MOV_SPEED",
		"APPLY_CAST_SPEED",
		"APPLY_HP_REGEN",
		"APPLY_SP_REGEN",
		"APPLY_POISON_PCT",
		"APPLY_STUN_PCT",
		"APPLY_SLOW_PCT",
		"APPLY_CRITICAL_PCT",
		"APPLY_PENETRATE_PCT",
		"APPLY_ATTBONUS_HUMAN",
		"APPLY_ATTBONUS_ANIMAL",
		"APPLY_ATTBONUS_ORC",
		"APPLY_ATTBONUS_MILGYO",
		"APPLY_ATTBONUS_UNDEAD",
		"APPLY_ATTBONUS_DEVIL",
		"APPLY_STEAL_HP",
		"APPLY_STEAL_SP",
		"APPLY_MANA_BURN_PCT",
		"APPLY_DAMAGE_SP_RECOVER",
		"APPLY_BLOCK",
		"APPLY_DODGE",
		"APPLY_RESIST_SWORD",
		"APPLY_RESIST_TWOHAND",
		"APPLY_RESIST_DAGGER",
		"APPLY_RESIST_BELL",
		"APPLY_RESIST_FAN",
		"APPLY_RESIST_BOW",
		"APPLY_RESIST_FIRE",
		"APPLY_RESIST_ELEC",
		"APPLY_RESIST_MAGIC",
		"APPLY_RESIST_WIND",
		"APPLY_REFLECT_MELEE",
		"APPLY_REFLECT_CURSE",
		"APPLY_POISON_REDUCE",
		"APPLY_KILL_SP_RECOVER",
		"APPLY_EXP_DOUBLE_BONUS",
		"APPLY_GOLD_DOUBLE_BONUS",
		"APPLY_ITEM_DROP_BONUS",
		"APPLY_POTION_BONUS",
		"APPLY_KILL_HP_RECOVER",
		"APPLY_IMMUNE_STUN",
		"APPLY_IMMUNE_SLOW",
		"APPLY_IMMUNE_FALL",
		"APPLY_SKILL",
		"APPLY_BOW_DISTANCE",
		"APPLY_ATT_GRADE_BONUS",
		"APPLY_DEF_GRADE_BONUS",
		"APPLY_MAGIC_ATT_GRADE",
		"APPLY_MAGIC_DEF_GRADE",
		"APPLY_CURSE_PCT",
		"APPLY_MAX_STAMINA",
		"APPLY_ATTBONUS_WARRIOR",
		"APPLY_ATTBONUS_ASSASSIN",
		"APPLY_ATTBONUS_SURA",
		"APPLY_ATTBONUS_SHAMAN",
		"APPLY_ATTBONUS_MONSTER",
		"APPLY_MALL_ATTBONUS",
		"APPLY_MALL_DEFBONUS",
		"APPLY_MALL_EXPBONUS",
		"APPLY_MALL_ITEMBONUS",
		"APPLY_MALL_GOLDBONUS",
		"APPLY_MAX_HP_PCT",
		"APPLY_MAX_SP_PCT",
		"APPLY_SKILL_DAMAGE_BONUS",
		"APPLY_NORMAL_HIT_DAMAGE_BONUS",
		"APPLY_SKILL_DEFEND_BONUS",
		"APPLY_NORMAL_HIT_DEFEND_BONUS",
		"APPLY_PC_BANG_EXP_BONUS",
		"APPLY_PC_BANG_DROP_BONUS",
		"APPLY_EXTRACT_HP_PCT",
		"APPLY_RESIST_WARRIOR",
		"APPLY_RESIST_ASSASSIN",
		"APPLY_RESIST_SURA",
		"APPLY_RESIST_SHAMAN",
		"APPLY_ENERGY",
		"APPLY_DEF_GRADE",
		"APPLY_COSTUME_ATTR_BONUS",
		"APPLY_MAGIC_ATTBONUS_PER",
		"APPLY_MELEE_MAGIC_ATTBONUS_PER",
		"APPLY_RESIST_ICE",
		"APPLY_RESIST_EARTH",
		"APPLY_RESIST_DARK",
		"APPLY_ANTI_CRITICAL_PCT",
		"APPLY_ANTI_PENETRATE_PCT",
		"APPLY_BLEEDING_REDUCE",
		"APPLY_BLEEDING_PCT",
		"APPLY_ATTBONUS_WOLFMAN",
		"APPLY_RESIST_WOLFMAN",
		"APPLY_RESIST_CLAW",
		"APPLY_ACCEDRAIN_RATE",
		"APPLY_RESIST_MAGIC_REDUCTION",
		"APPLY_ENCHANT_ELECT",
		"APPLY_ENCHANT_FIRE",
		"APPLY_ENCHANT_ICE",
		"APPLY_ENCHANT_WIND",
		"APPLY_ENCHANT_EARTH",
		"APPLY_ENCHANT_DARK",
		"APPLY_ATTBONUS_CZ",
		"APPLY_ATTBONUS_INSECT",
		"APPLY_ATTBONUS_DESERT",
		"APPLY_ATTBONUS_SWORD",
		"APPLY_ATTBONUS_TWOHAND",
		"APPLY_ATTBONUS_DAGGER",
		"APPLY_ATTBONUS_BELL",
		"APPLY_ATTBONUS_FAN",
		"APPLY_ATTBONUS_BOW",
		"APPLY_ATTBONUS_CLAW",
		"APPLY_RESIST_HUMAN",
		"APPLY_RESIST_MOUNT_FALL",
		"APPLY_RESIST_FIST",
		"APPLY_MOUNT",
		"APPLY_SKILL_DAMAGE_SAMYEON",
		"APPLY_SKILL_DAMAGE_TANHWAN",
		"APPLY_SKILL_DAMAGE_PALBANG",
		"APPLY_SKILL_DAMAGE_GIGONGCHAM",
		"APPLY_SKILL_DAMAGE_GYOKSAN",
		"APPLY_SKILL_DAMAGE_GEOMPUNG",
		"APPLY_SKILL_DAMAGE_AMSEOP",
		"APPLY_SKILL_DAMAGE_GUNGSIN",
		"APPLY_SKILL_DAMAGE_CHARYUN",
		"APPLY_SKILL_DAMAGE_SANGONG",
		"APPLY_SKILL_DAMAGE_YEONSA",
		"APPLY_SKILL_DAMAGE_KWANKYEOK",
		"APPLY_SKILL_DAMAGE_GIGUNG",
		"APPLY_SKILL_DAMAGE_HWAJO",
		"APPLY_SKILL_DAMAGE_SWAERYUNG",
		"APPLY_SKILL_DAMAGE_YONGKWON",
		"APPLY_SKILL_DAMAGE_PABEOB",
		"APPLY_SKILL_DAMAGE_MARYUNG",
		"APPLY_SKILL_DAMAGE_HWAYEOMPOK",
		"APPLY_SKILL_DAMAGE_MAHWAN",
		"APPLY_SKILL_DAMAGE_BIPABU",
		"APPLY_SKILL_DAMAGE_YONGBI",
		"APPLY_SKILL_DAMAGE_PAERYONG",
		"APPLY_SKILL_DAMAGE_NOEJEON",
		"APPLY_SKILL_DAMAGE_BYEURAK",
		"APPLY_SKILL_DAMAGE_CHAIN",
		"APPLY_SKILL_DAMAGE_CHAYEOL",
		"APPLY_SKILL_DAMAGE_SALPOONG",
		"APPLY_SKILL_DAMAGE_GONGDAB",
		"APPLY_SKILL_DAMAGE_PASWAE",
		"APPLY_NORMAL_HIT_DEFEND_BONUS_BOSS_OR_MORE",
		"APPLY_SKILL_DEFEND_BONUS_BOSS_OR_MORE",
		"APPLY_NORMAL_HIT_DAMAGE_BONUS_BOSS_OR_MORE",
		"APPLY_SKILL_DAMAGE_BONUS_BOSS_OR_MORE",
		"APPLY_HIT_BUFF_ENCHANT_FIRE",
		"APPLY_HIT_BUFF_ENCHANT_ICE",
		"APPLY_HIT_BUFF_ENCHANT_ELEC",
		"APPLY_HIT_BUFF_ENCHANT_WIND",
		"APPLY_HIT_BUFF_ENCHANT_DARK",
		"APPLY_HIT_BUFF_ENCHANT_EARTH",
		"APPLY_HIT_BUFF_RESIST_FIRE",
		"APPLY_HIT_BUFF_RESIST_ICE",
		"APPLY_HIT_BUFF_RESIST_ELEC",
		"APPLY_HIT_BUFF_RESIST_WIND",
		"APPLY_HIT_BUFF_RESIST_DARK",
		"APPLY_HIT_BUFF_RESIST_EARTH",
		"APPLY_USE_SKILL_CHEONGRANG_MOV_SPEED",
		"APPLY_USE_SKILL_CHEONGRANG_CASTING_SPEED",
		"APPLY_USE_SKILL_CHAYEOL_CRITICAL_PCT",
		"APPLY_USE_SKILL_SANGONG_ATT_GRADE_BONUS",
		"APPLY_USE_SKILL_GIGUNG_ATT_GRADE_BONUS",
		"APPLY_USE_SKILL_JEOKRANG_DEF_BONUS",
		"APPLY_USE_SKILL_GWIGEOM_DEF_BONUS",
		"APPLY_USE_SKILL_TERROR_ATT_GRADE_BONUS",
		"APPLY_USE_SKILL_MUYEONG_ATT_GRADE_BONUS",
		"APPLY_USE_SKILL_MANASHILED_CASTING_SPEED",
		"APPLY_USE_SKILL_HOSIN_DEF_BONUS",
		"APPLY_USE_SKILL_GICHEON_ATT_GRADE_BONUS",
		"APPLY_USE_SKILL_JEONGEOP_ATT_GRADE_BONUS",
		"APPLY_USE_SKILL_JEUNGRYEOK_DEF_BONUS",
		"APPLY_USE_SKILL_GIHYEOL_ATT_GRADE_BONUS",
		"APPLY_USE_SKILL_CHUNKEON_CASTING_SPEED",
		"APPLY_USE_SKILL_NOEGEOM_ATT_GRADE_BONUS",
		"APPLY_SKILL_DURATION_INCREASE_EUNHYUNG",
		"APPLY_SKILL_DURATION_INCREASE_GYEONGGONG",
		"APPLY_SKILL_DURATION_INCREASE_GEOMKYUNG",
		"APPLY_SKILL_DURATION_INCREASE_JEOKRANG",
		"APPLY_USE_SKILL_PALBANG_HP_ABSORB",
		"APPLY_USE_SKILL_AMSEOP_HP_ABSORB",
		"APPLY_USE_SKILL_YEONSA_HP_ABSORB",
		"APPLY_USE_SKILL_YONGBI_HP_ABSORB",
		"APPLY_USE_SKILL_CHAIN_HP_ABSORB",
		"APPLY_USE_SKILL_PASWAE_SP_ABSORB",
		"APPLY_USE_SKILL_GIGONGCHAM_STUN",
		"APPLY_USE_SKILL_CHARYUN_STUN",
		"APPLY_USE_SKILL_PABEOB_STUN",
		"APPLY_USE_SKILL_MAHWAN_STUN",
		"APPLY_USE_SKILL_GONGDAB_STUN",
		"APPLY_USE_SKILL_SAMYEON_STUN",
		"APPLY_USE_SKILL_GYOKSAN_KNOCKBACK",
		"APPLY_USE_SKILL_SEOMJEON_KNOCKBACK",
		"APPLY_USE_SKILL_SWAERYUNG_KNOCKBACK",
		"APPLY_USE_SKILL_HWAYEOMPOK_KNOCKBACK",
		"APPLY_USE_SKILL_GONGDAB_KNOCKBACK",
		"APPLY_USE_SKILL_KWANKYEOK_KNOCKBACK",
		"APPLY_USE_SKILL_SAMYEON_NEXT_COOLTIME_DECREASE_10PER",
		"APPLY_USE_SKILL_GEOMPUNG_NEXT_COOLTIME_DECREASE_10PER",
		"APPLY_USE_SKILL_GUNGSIN_NEXT_COOLTIME_DECREASE_10PER",
		"APPLY_USE_SKILL_KWANKYEOK_NEXT_COOLTIME_DECREASE_10PER",
		"APPLY_USE_SKILL_YONGKWON_NEXT_COOLTIME_DECREASE_10PER",
		"APPLY_USE_SKILL_MARYUNG_NEXT_COOLTIME_DECREASE_10PER",
		"APPLY_USE_SKILL_BIPABU_NEXT_COOLTIME_DECREASE_10PER",
		"APPLY_USE_SKILL_NOEJEON_NEXT_COOLTIME_DECREASE_10PER",
		"APPLY_USE_SKILL_SALPOONG_NEXT_COOLTIME_DECREASE_10PER",
		"APPLY_USE_SKILL_PASWAE_NEXT_COOLTIME_DECREASE_10PER",
		"APPLY_ATTBONUS_STONE",
		"APPLY_DAMAGE_HP_RECOVERY",
		"APPLY_DAMAGE_SP_RECOVERY",
		"APPLY_ALIGNMENT_DAMAGE_BONUS",
		"APPLY_NORMAL_DAMAGE_GUARD",
		"APPLY_MORE_THEN_HP90_DAMAGE_REDUCE",
		"APPLY_USE_SKILL_TUSOK_HP_ABSORB",
		"APPLY_USE_SKILL_PAERYONG_HP_ABSORB",
		"APPLY_USE_SKILL_BYEURAK_HP_ABSORB",
		"APPLY_FIRST_ATTRIBUTE_BONUS",
		"APPLY_SECOND_ATTRIBUTE_BONUS",
		"APPLY_THIRD_ATTRIBUTE_BONUS",
		"APPLY_FOURTH_ATTRIBUTE_BONUS",
		"APPLY_FIFTH_ATTRIBUTE_BONUS",
		"APPLY_USE_SKILL_SAMYEON_NEXT_COOLTIME_DECREASE_20PER",
		"APPLY_USE_SKILL_GEOMPUNG_NEXT_COOLTIME_DECREASE_20PER",
		"APPLY_USE_SKILL_GUNGSIN_NEXT_COOLTIME_DECREASE_20PER",
		"APPLY_USE_SKILL_KWANKYEOK_NEXT_COOLTIME_DECREASE_20PER",
		"APPLY_USE_SKILL_YONGKWON_NEXT_COOLTIME_DECREASE_20PER",
		"APPLY_USE_SKILL_MARYUNG_NEXT_COOLTIME_DECREASE_20PER",
		"APPLY_USE_SKILL_BIPABU_NEXT_COOLTIME_DECREASE_20PER",
		"APPLY_USE_SKILL_NOEJEON_NEXT_COOLTIME_DECREASE_20PER",
		"APPLY_USE_SKILL_SALPOONG_NEXT_COOLTIME_DECREASE_20PER",
		"APPLY_USE_SKILL_PASWAE_NEXT_COOLTIME_DECREASE_20PER",
		"APPLY_USE_SKILL_CHAYEOL_HP_ABSORB",
		"APPLY_SUNGMA_STR",
		"APPLY_SUNGMA_HP",
		"APPLY_SUNGMA_MOVE",
		"APPLY_SUNGMA_IMMUNE",
		"APPLY_HIT_PCT",
		"APPLY_RANDOM",
		"APPLY_ATTBONUS_PER_HUMAN",
		"APPLY_ATTBONUS_PER_ANIMAL",
		"APPLY_ATTBONUS_PER_ORC",
		"APPLY_ATTBONUS_PER_MILGYO",
		"APPLY_ATTBONUS_PER_UNDEAD",
		"APPLY_ATTBONUS_PER_DEVIL",
		"APPLY_ENCHANT_PER_ELECT",
		"APPLY_ENCHANT_PER_FIRE",
		"APPLY_ENCHANT_PER_ICE",
		"APPLY_ENCHANT_PER_WIND",
		"APPLY_ENCHANT_PER_EARTH",
		"APPLY_ENCHANT_PER_DARK",
		"APPLY_ATTBONUS_PER_CZ",
		"APPLY_ATTBONUS_PER_INSECT",
		"APPLY_ATTBONUS_PER_DESERT",
		"APPLY_ATTBONUS_PER_STONE",
		"APPLY_ATTBONUS_PER_MONSTER",
		"APPLY_RESIST_PER_HUMAN",
		"APPLY_RESIST_PER_ICE",
		"APPLY_RESIST_PER_DARK",
		"APPLY_RESIST_PER_EARTH",
		"APPLY_RESIST_PER_FIRE",
		"APPLY_RESIST_PER_ELEC",
		"APPLY_RESIST_PER_MAGIC",
		"APPLY_RESIST_PER_WIND",
		"APPLY_HIT_BUFF_SUNGMA_STR",
		"APPLY_HIT_BUFF_SUNGMA_MOVE",
		"APPLY_HIT_BUFF_SUNGMA_HP",
		"APPLY_HIT_BUFF_SUNGMA_IMMUNE",
		"APPLY_MOUNT_MELEE_MAGIC_ATTBONUS_PER",
		"APPLY_DISMOUNT_MOVE_SPEED_BONUS_PER",
		"APPLY_HIT_AUTO_HP_RECOVERY",
		"APPLY_HIT_AUTO_SP_RECOVERY",
		"APPLY_USE_SKILL_COOLTIME_DECREASE_ALL",
		"APPLY_HIT_STONE_ATTBONUS_STONE",
		"APPLY_HIT_STONE_DEF_GRADE_BONUS",
		"APPLY_KILL_BOSS_ITEM_BONUS",
		"APPLY_MOB_HIT_MOB_AGGRESSIVE",
		"APPLY_NO_DEATH_AND_HP_RECOVERY30",
		"APPLY_AUTO_PICKUP",
		"APPLY_MOUNT_NO_KNOCKBACK",
		"APPLY_SUNGMA_PER_STR",
		"APPLY_SUNGMA_PER_HP",
		"APPLY_SUNGMA_PER_MOVE",
		"APPLY_SUNGMA_PER_IMMUNE",
		"APPLY_IMMUNE_POISON100",
		"APPLY_IMMUNE_BLEEDING100",
		"APPLY_MONSTER_DEFEND_BONUS",
	};

	int retInt = -1;
	//cout << "ApplyType : " << applyTypeStr << " -> ";
	for (int j = 0; j < sizeof(arApplyType) / sizeof(arApplyType[0]); j++)
	{
		string tempString = arApplyType[j];
		string tempInputString = trim(inputString);
		if (tempInputString.compare(tempString) == 0)
		{
			//cout << j << " ";
			retInt = j;
			break;
		}
	}
	//cout << endl;

	return retInt;
}

//몬스터 프로토도 읽는다.
int get_Mob_Rank_Value(string inputString)
{
	string arRank[] = {
		"PAWN",
		"S_PAWN",
		"KNIGHT",
		"S_KNIGHT",
		"BOSS",
		"KING",
	};

	int retInt = -1;
	//cout << "Rank : " << rankStr << " -> ";
	for (int j = 0; j < sizeof(arRank) / sizeof(arRank[0]); j++)
	{
		string tempString = arRank[j];
		string tempInputString = trim(inputString);
		if (tempInputString.compare(tempString) == 0)
		{
			//cout << j << " ";
			retInt = j;
			break;
		}
	}
	//cout << endl;

	return retInt;
}

int get_Mob_Type_Value(string inputString)
{
	string arType[] = {
		"MONSTER",
		"NPC",
		"STONE",
		"WARP",
		"DOOR",
		"BUILDING",
		"PC",
		"POLYMORPH_PC",
		"HORSE",
		"GOTO",
		"PET",
		"PET_PAY",
		"SHOP",
		"OBJECT",
	};

	int retInt = -1;
	//cout << "Type : " << typeStr << " -> ";
	for (int j = 0; j < sizeof(arType) / sizeof(arType[0]); j++)
	{
		string tempString = arType[j];
		string tempInputString = trim(inputString);
		if (tempInputString.compare(tempString) == 0)
		{
			// cout << j << " ";
			retInt = j;
			break;
		}
	}
	//cout << endl;

	return retInt;
}

int get_Mob_BattleType_Value(string inputString)
{
	string arBattleType[] = {
		"MELEE",
		"RANGE",
		"MAGIC",
		"SPECIAL",
		"POWER",
		"TANKER",
		"SUPER_POWER",
		"SUPER_TANKER",
	};

	int retInt = -1;
	//cout << "Battle Type : " << battleTypeStr << " -> ";
	for (int j = 0; j < sizeof(arBattleType) / sizeof(arBattleType[0]); j++)
	{
		string tempString = arBattleType[j];
		string tempInputString = trim(inputString);
		if (tempInputString.compare(tempString) == 0)
		{
			//cout << j << " ";
			retInt = j;
			break;
		}
	}
	//cout << endl;

	return retInt;
}

int get_Mob_Size_Value(string inputString)
{
	string arSize[] = {
		"SMALL",
		"MEDIUM",
		"BIG",
	};

	int retInt = 0;
	//cout << "Size : " << sizeStr << " -> ";
	for (int j = 0; j < sizeof(arSize) / sizeof(arSize[0]); j++)
	{
		string tempString = arSize[j];
		string tempInputString = trim(inputString);
		if (tempInputString.compare(tempString) == 0)
		{
			// cout << j << " ";
			retInt = j + 1;
			break;
		}
	}
	//cout << endl;

	return retInt;
}

int get_Mob_AIFlag_Value(string inputString)
{
	string arAIFlag[] = {
		"AGGR",
		"NOMOVE",
		"COWARD",
		"NOATTSHINSU",
		"NOATTCHUNJO",
		"NOATTJINNO",
		"ATTMOB",
		"BERSERK",
		"STONESKIN",
		"GODSPEED",
		"DEATHBLOW",
		"REVIVE",
		"HEALER",
		"COUNT",
		"NORECOVERY",
		"REFLECT",
		"FALL",
		"VIT",
		"RATTSPEED",
		"RCASTSPEED",
		"RHP_REGEN",
		"TIMEVIT",
	};

	int retValue = 0;
	string* arInputString = StringSplit(inputString, ","); // 프로토 정보 내용을 단어별로 쪼갠 배열.
	for (int i = 0; i < sizeof(arAIFlag) / sizeof(arAIFlag[0]); i++)
	{
		string tempString = arAIFlag[i];
		for (int j = 0; j < 30; j++) // 최대 30개 단어까지. (하드코딩)
		{
			string tempString2 = arInputString[j];
			if (tempString2.compare(tempString) == 0) // 일치하는지 확인.
			{
				retValue = retValue + pow((float)2, (float)i);
			}

			if (tempString2.compare("") == 0)
				break;
		}
	}
	delete[]arInputString;
	//cout << "AIFlag : " << aiFlagStr << " -> " << retValue << endl;

	return retValue;
}

int get_Mob_RaceFlag_Value(string inputString)
{
	string arRaceFlag[] = {
		"ANIMAL",
		"UNDEAD",
		"DEVIL",
		"HUMAN",
		"ORC",
		"MILGYO",
		"INSECT",
		"DESERT",
		"TREE",
		"DECO",
		"HIDE",
		"ATT_CZ",
		"AWEAKEN",
		"SUNGMAHEE",
		"OUTPOST",
	};

	int retValue = 0;
	string* arInputString = StringSplit(inputString, ","); // 프로토 정보 내용을 단어별로 쪼갠 배열.
	for (int i = 0; i < sizeof(arRaceFlag) / sizeof(arRaceFlag[0]); i++)
	{
		string tempString = arRaceFlag[i];
		for (int j = 0; j < 30; j++) // 최대 30개 단어까지. (하드코딩)
		{
			string tempString2 = arInputString[j];
			if (tempString2.compare(tempString) == 0) // 일치하는지 확인.
			{
				retValue = retValue + pow((float)2, (float)i);
			}

			if (tempString2.compare("") == 0)
				break;
		}
	}
	delete[]arInputString;
	//cout << "Race Flag : " << raceFlagStr << " -> " << retValue << endl;

	return retValue;
}

int get_Mob_ImmuneFlag_Value(string inputString)
{
	string arImmuneFlag[] = {
		"STUN",
		"SLOW",
		"FALL",
		"CURSE",
		"POISON",
		"TERROR",
		"REFLECT",
	};

	int retValue = 0;
	string* arInputString = StringSplit(inputString, ","); // 프로토 정보 내용을 단어별로 쪼갠 배열.
	for (int i = 0; i < sizeof(arImmuneFlag) / sizeof(arImmuneFlag[0]); i++)
	{
		string tempString = arImmuneFlag[i];
		for (int j = 0; j < 30; j++) // 최대 30개 단어까지. (하드코딩)
		{
			string tempString2 = arInputString[j];
			if (tempString2.compare(tempString) == 0) // 일치하는지 확인.
			{
				retValue = retValue + pow((float)2, (float)i);
			}

			if (tempString2.compare("") == 0)
				break;
		}
	}
	delete[]arInputString;
	//cout << "Immune Flag : " << immuneFlagStr << " -> " << retValue << endl;

	return retValue;
}
