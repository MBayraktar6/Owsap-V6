#include "stdafx.h"

#include <math.h>
#include "ProtoReader.h"

#include "CsvReader.h"

#include <sstream>

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

string trim(const string& str) { return trim_left(trim_right(str)); }

static string* StringSplit(string strOrigin, string strTok)
{
	int cutAt; // 자르는위치
	int index = 0; // 문자열인덱스
	string* strResult = new string[30]; //결과return 할변수

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
#if defined(__MOVE_COSTUME_ATTR__)		
		"ITEM_MEDIUM",
#endif									
#if defined(__GACHA_SYSTEM__)			
		"ITEM_GACHA",
#endif									
		"ITEM_PASSIVE",
		"ITEM_MERCENARY",
		"ITEM_ALCHEMY",
//#if defined(__SOUL_SYSTEM__)			
		"ITEM_SOUL",
//#endif
	};

	int retInt = -1;
	// cout << "Type : " << typeStr << " -> ";
	for (int j = 0; j < sizeof(arType) / sizeof(arType[0]); j++)
	{
		string tempString = arType[j];
		if (inputString.find(tempString) != string::npos && tempString.find(inputString) != string::npos)
		{
			// cout << j << " ";
			retInt = j;
			break;
		}
	}
	// cout << endl;

	return retInt;
}

int get_Item_SubType_Value(int type_value, string inputString)
{
	static string arSub1[] = {
		"WEAPON_SWORD",
		"WEAPON_DAGGER",
		"WEAPON_BOW",
		"WEAPON_TWO_HANDED",
		"WEAPON_BELL",
		"WEAPON_FAN",
		"WEAPON_ARROW",
		"WEAPON_MOUNT_SPEAR",
		"WEAPON_CLAW",
#if defined(__QUIVER_SYSTEM__)
		"WEAPON_QUIVER",
#endif
		"WEAPON_BOUQUE",
	};

	static string arSub2[] = {
		"ARMOR_BODY",
		"ARMOR_HEAD",
		"ARMOR_SHIELD",
		"ARMOR_WRIST",
		"ARMOR_FOOTS",
		"ARMOR_NECK",
		"ARMOR_EAR",
#if defined(__PENDANT_SYSTEM__)
		"ARMOR_PENDANT",
#endif
#if defined(__GLOVE_SYSTEM__)
		"ARMOR_GLOVE",
#endif
	};

	static string arSub3[] = {
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
#if defined(__MOVE_COSTUME_ATTR__)
		"USE_CHANGE_COSTUME_ATTR",
		"USE_RESET_COSTUME_ATTR",
#endif
		"USE_UNK_33",
#if defined(__CHANGED_ATTR__)
		"USE_SELECT_ATTRIBUTE",
#endif
		"USE_FLOWER",
#if defined(__EXPRESSING_EMOTIONS__)
		"USE_EMOTION_PACK",
#endif
#if defined(__REFINE_ELEMENT_SYSTEM__)
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

	static string arSub4[] = {
		"AUTOUSE_POTION",
		"AUTOUSE_ABILITY_UP",
		"AUTOUSE_BOMB",
		"AUTOUSE_GOLD",
		"AUTOUSE_MONEYBAG",
		"AUTOUSE_TREASURE_BOX",
	};

	static string arSub5[] = {
		"MATERIAL_LEATHER",
		"MATERIAL_BLOOD",
		"MATERIAL_ROOT",
		"MATERIAL_NEEDLE",
		"MATERIAL_JEWEL",
		"MATERIAL_DS_REFINE_NORMAL",
		"MATERIAL_DS_REFINE_BLESSED",
		"MATERIAL_DS_REFINE_HOLLY",
#if defined(__DS_CHANGE_ATTR__)
		"MATERIAL_DS_CHANGE_ATTR",
#endif
		"MATERIAL_PASSIVE_WEAPON",
		"MATERIAL_PASSIVE_ARMOR",
		"MATERIAL_PASSIVE_ACCE",
		"MATERIAL_PASSIVE_ELEMENT",
	};

	static string arSub6[] = {
		"SPECIAL_MAP",
		"SPECIAL_KEY",
		"SPECIAL_DOC",
		"SPECIAL_SPIRIT",
	};

	static string arSub7[] = {
		"TOOL_FISHING_ROD",
	};

	static string arSub8[] = {
		"LOTTERY_TICKET",
		"LOTTERY_INSTANT",
	};

	static string arSub10[] = {
		"METIN_NORMAL",
		"METIN_SUNGMA",
	};

	static string arSub12[] = {
		"FISH_ALIVE",
		"FISH_DEAD",
	};

	static string arSub14[] = {
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
#if defined(__AURA_COSTUME_SYSTEM__)
		"RESOURCE_AURA",
#endif
	};

	static string arSub16[] = {
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

	static string arSub18[] = {
		"QUEST_NONE",
		"QUEST_PET_PAY",
		"QUEST_WARP",
		"QUEST_GEM_BAG",
	};

	static std::string arSub23[]
	{
		"GIFTBOX_NONE",
		"GIFTBOX_NORMAL",
		"GIFTBOX_SPECIAL",
	};

	static string arSub28[] = {
		"COSTUME_BODY",
		"COSTUME_HAIR",
#if defined(__MOUNT_COSTUME_SYSTEM__)
		"COSTUME_MOUNT",
#endif
#if defined(__ACCE_COSTUME_SYSTEM__)
		"COSTUME_ACCE",
#endif
#if defined(__WEAPON_COSTUME_SYSTEM__)
		"COSTUME_WEAPON",
#endif
#if defined(__AURA_COSTUME_SYSTEM__)
		"COSTUME_AURA",
#endif
	};

	static string arSub29[] = {
		"DS_SLOT1",
		"DS_SLOT2",
		"DS_SLOT3",
		"DS_SLOT4",
		"DS_SLOT5",
		"DS_SLOT6",
#if defined(__DS_7_SLOT__)
		"DS_SLOT7",
#endif
	};

	static string arSub31[] = {
		"EXTRACT_DRAGON_SOUL",
		"EXTRACT_DRAGON_HEART",
	};

	static std::string arSub35[]
	{
//#if defined(__GROWTH_PET_SYSTEM__)
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
//#if defined(__PET_ATTR_DETERMINE__)
		"PET_ATTR_DETERMINE",
		"PET_ATTR_CHANGE",
//#endif
//#endif
		"PET_PAY",
//#if defined(__GROWTH_PET_SYSTEM__) && defined(__PET_PRIMIUM_FEEDSTUFF__)
		"PET_PRIMIUM_FEEDSTUFF",
//#endif
	};

#if defined(__MOVE_COSTUME_ATTR__)
	static string arSub36[] = {
		"MEDIUM_MOVE_COSTUME_ATTR",
#if defined(__ACCE_COSTUME_SYSTEM__)
		"MEDIUM_MOVE_ACCE_ATTR",
#endif
	};
#endif

#if defined(__GACHA_SYSTEM__)
	static string arSub37[] = {
		"USE_GACHA",
#if defined(__LUCKY_BOX__)
		"GEM_LUCKY_BOX_GACHA",
		"SPECIAL_LUCKY_BOX_GACHA",
#endif
	};
#endif

	static std::string arSub38[]
	{
		"PASSIVE_JOB",
	};

	static string arSub39[] = {
		"MERCENARY_0",
		"MERCENARY_1",
		"MERCENARY_2",
		"MERCENARY_3",
		"MERCENARY_4",
		"MERCENARY_5",
		"MERCENARY_6",
	};

	static std::string arSub40[]
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

//#if defined(__SOUL_SYSTEM__)
	static string arSub41[] = {
		"SOUL_RED",
		"SOUL_BLUE"
	};
//#endif

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
#if defined(__MOVE_COSTUME_ATTR__)
		arSub36,	// ITEM_MEDIUM
#endif
#if defined(__GACHA_SYSTEM__)
		arSub37,	// ITEM_GACHA
#endif
		arSub38,	// ITEM_PASSIVE
		arSub39,	// ITEM_MERCENARY
		arSub40,	// ITEM_ALCHEMY
//#if defined(__SOUL_SYSTEM__)
		arSub41,	// ITEM_SOUL
//#endif
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
#if defined(__MOVE_COSTUME_ATTR__)
		sizeof(arSub36) / sizeof(arSub36[0]),	// ITEM_MEDIUM
#endif
#if defined(__GACHA_SYSTEM__)
		sizeof(arSub37) / sizeof(arSub37[0]),	// ITEM_GACHA
#endif
		sizeof(arSub38) / sizeof(arSub38[0]),	// ITEM_PASSIVE
		sizeof(arSub39) / sizeof(arSub39[0]),	// ITEM_MERCENARY
		sizeof(arSub40) / sizeof(arSub40[0]),	// ITEM_ALCHEMY
//#if defined(__SOUL_SYSTEM__)
		sizeof(arSub41) / sizeof(arSub41[0]),	// ITEM_SOUL
//#endif
	};

	assert(_countof(arSubType) > type_value && "Subtype rule: Out of range!!");

	// assert 안 먹히는 듯..
	if (_countof(arSubType) <= type_value)
	{
		sys_err("SubType : Out of range!! (type_value: %d, count of registered subtype: %d", type_value, _countof(arSubType));
		return -1;
	}

	// 아이템 타입의 서브타입 어레이가 존재하는지 알아보고, 없으면 0 리턴
	if (arSubType[type_value] == 0)
		return 0;
	//

	int retInt = -1;
	// cout << "SubType : " << subTypeStr << " -> ";
	for (int j = 0; j < arNumberOfSubtype[type_value]; j++)
	{
		string tempString = arSubType[type_value][j];
		string tempInputString = trim(inputString);
		if (tempInputString.compare(tempString) == 0)
		{
			// cout << j << " ";
			retInt = j;
			break;
		}
	}
	// cout << endl;

	return retInt;
}

int get_Item_AntiFlag_Value(string inputString)
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
#if defined(__CHANGE_LOOK_SYSTEM__)
		"ANTI_CHANGELOOK",
#endif
		"ANTI_REINFORCE",
		"ANTI_ENCHANT",
		"ANTI_ENERGY",
//#if defined(__GROWTH_PET_SYSTEM__)
		"ANTI_PETFEED",
//#endif
		"ANTI_APPLY",
#if defined(__ACCE_COSTUME_SYSTEM__)
		"ANTI_ACCE",
#endif
#if defined(__MAILBOX__)
		"ANTI_MAIL",
#endif
		"ANTI_DESTROY" // Custom
	};

	int retValue = 0;
	string* arInputString = StringSplit(inputString, "|"); // 프로토 정보 내용을 단어별로 쪼갠 배열.
	for (int i = 0; i < sizeof(arAntiFlag) / sizeof(arAntiFlag[0]); i++)
	{
		string tempString = arAntiFlag[i];
		for (int j = 0; j < 30; j++) // 최대 30개 단어까지. (하드코딩)
		{
			string tempString2 = arInputString[j];
			if (tempString2.compare(tempString) == 0) // 일치하는지 확인.
			{
				retValue = retValue + static_cast<int>(pow((float)2, (float)i));
			}

			if (tempString2.compare("") == 0)
				break;
		}
	}
	delete[]arInputString;
	// cout << "AntiFlag : " << antiFlagStr << " -> " << retValue << endl;

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
				retValue = retValue + static_cast<int>(pow((float)2, (float)i));
			}

			if (tempString2.compare("") == 0)
				break;
		}
	}
	delete[]arInputString;
	// cout << "Flag : " << flagStr << " -> " << retValue << endl;

	return retValue;
}

int get_Item_WearFlag_Value(string inputString)
{
	// @ EItemWearableFlag
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
#if defined(__PENDANT_SYSTEM__)
		"WEAR_PENDANT",
#endif
#if defined(__GLOVE_SYSTEM__)
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
				retValue = retValue + static_cast<int>(pow((float)2, (float)i));
			}

			if (tempString2.compare("") == 0)
				break;
		}
	}
	delete[]arInputString;
	// cout << "WearFlag : " << wearFlagStr << " -> " << retValue << endl;

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
	string* arInputString = StringSplit(inputString, "|"); //프로토 정보 내용을 단어별로 쪼갠 배열.
	for (int i = 0; i < sizeof(arImmune) / sizeof(arImmune[0]); i++)
	{
		string tempString = arImmune[i];
		for (int j = 0; j < 30; j++) // 최대 30개 단어까지. (하드코딩)
		{
			string tempString2 = arInputString[j];
			if (tempString2.compare(tempString) == 0) // 일치하는지 확인.
			{
				retValue = retValue + static_cast<int>(pow((float)2, (float)i));
			}

			if (tempString2.compare("") == 0)
				break;
		}
	}
	delete[]arInputString;
	// cout << "Immune : " << immuneStr << " -> " << retValue << endl;

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
	// cout << "LimitType : " << limitTypeStr << " -> ";
	for (int j = 0; j < sizeof(arLimitType) / sizeof(arLimitType[0]); j++)
	{
		string tempString = arLimitType[j];
		string tempInputString = trim(inputString);
		if (tempInputString.compare(tempString) == 0)
		{
			// cout << j << " ";
			retInt = j;
			break;
		}
	}
	// cout << endl;

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
	// cout << "ApplyType : " << applyTypeStr << " -> ";
	for (int j = 0; j < sizeof(arApplyType) / sizeof(arApplyType[0]); j++)
	{
		string tempString = arApplyType[j];
		string tempInputString = trim(inputString);
		if (tempInputString.compare(tempString) == 0)
		{
			// cout << j << " ";
			retInt = j;
			break;
		}
	}
	// cout << endl;

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
	// cout << "Rank : " << rankStr << " -> ";
	for (int j = 0; j < sizeof(arRank) / sizeof(arRank[0]); j++)
	{
		string tempString = arRank[j];
		string tempInputString = trim(inputString);
		if (tempInputString.compare(tempString) == 0)
		{
			// cout << j << " ";
			retInt = j;
			break;
		}
	}
	// cout << endl;

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
	// cout << "Type : " << typeStr << " -> ";
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
	// cout << endl;

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
	// cout << "Battle Type : " << battleTypeStr << " -> ";
	for (int j = 0; j < sizeof(arBattleType) / sizeof(arBattleType[0]); j++)
	{
		string tempString = arBattleType[j];
		string tempInputString = trim(inputString);
		if (tempInputString.compare(tempString) == 0)
		{
			// cout << j << " ";
			retInt = j;
			break;
		}
	}
	// cout << endl;

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
	// cout << "Size : " << sizeStr << " -> ";
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
	// cout << endl;

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
				retValue = retValue + static_cast<int>(pow((float)2, (float)i));
			}

			if (tempString2.compare("") == 0)
				break;
		}
	}
	delete[]arInputString;
	// cout << "AIFlag : " << aiFlagStr << " -> " << retValue << endl;

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
				retValue = retValue + static_cast<int>(pow((float)2, (float)i));
			}

			if (tempString2.compare("") == 0)
				break;
		}
	}
	delete[]arInputString;
	// cout << "Race Flag : " << raceFlagStr << " -> " << retValue << endl;

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
				retValue = retValue + static_cast<int>(pow((float)2, (float)i));
			}

			if (tempString2.compare("") == 0)
				break;
		}
	}
	delete[]arInputString;
	// cout << "Immune Flag : " << immuneFlagStr << " -> " << retValue << endl;

	return retValue;
}

#ifndef __DUMP_PROTO__
// 몹 테이블을 셋팅해준다.
bool Set_Proto_Mob_Table(TMobTable* mobTable, cCsvTable& csvTable, std::map<int, const char*>& nameMap)
{
	int col = 0;

	str_to_number(mobTable->dwVnum, csvTable.AsStringByIndex(col++));
	strlcpy(mobTable->szName, csvTable.AsStringByIndex(col++), sizeof(mobTable->szName));

	// 3. 지역별 이름 넣어주기.
	map<int, const char*>::iterator it;
	it = nameMap.find(mobTable->dwVnum);
	if (it != nameMap.end())
	{
		const char* localeName = it->second;
		strlcpy(mobTable->szLocaleName, localeName, sizeof(mobTable->szLocaleName));
	}
	else
	{
		strlcpy(mobTable->szLocaleName, mobTable->szName, sizeof(mobTable->szLocaleName));
	}

	// RANK
	int rankValue = get_Mob_Rank_Value(csvTable.AsStringByIndex(col++));
	mobTable->bRank = rankValue;

	// TYPE
	int typeValue = get_Mob_Type_Value(csvTable.AsStringByIndex(col++));
	mobTable->bType = typeValue;

	// BATTLE_TYPE
	int battleTypeValue = get_Mob_BattleType_Value(csvTable.AsStringByIndex(col++));
	mobTable->bBattleType = battleTypeValue;

	str_to_number(mobTable->bLevel, csvTable.AsStringByIndex(col++));
	str_to_number(mobTable->bScale, csvTable.AsStringByIndex(col++));

	// SIZE
	int sizeValue = get_Mob_Size_Value(csvTable.AsStringByIndex(col++));
	mobTable->bSize = sizeValue;

	// AI_FLAG
	int aiFlagValue = get_Mob_AIFlag_Value(csvTable.AsStringByIndex(col++));
	mobTable->dwAIFlag = aiFlagValue;

	// MOUNT_CAPACITY;
	col++;

	// RACE_FLAG
	int raceFlagValue = get_Mob_RaceFlag_Value(csvTable.AsStringByIndex(col++));
	mobTable->dwRaceFlag = raceFlagValue;

	// IMMUNE_FLAG
	int immuneFlagValue = get_Mob_ImmuneFlag_Value(csvTable.AsStringByIndex(col++));
	mobTable->dwImmuneFlag = immuneFlagValue;

	str_to_number(mobTable->bEmpire, csvTable.AsStringByIndex(col++)); // col = 11

	strlcpy(mobTable->szFolder, csvTable.AsStringByIndex(col++), sizeof(mobTable->szFolder));

	str_to_number(mobTable->bOnClickType, csvTable.AsStringByIndex(col++));

	str_to_number(mobTable->bStr, csvTable.AsStringByIndex(col++));
	str_to_number(mobTable->bDex, csvTable.AsStringByIndex(col++));
	str_to_number(mobTable->bCon, csvTable.AsStringByIndex(col++));
	str_to_number(mobTable->bInt, csvTable.AsStringByIndex(col++));
	str_to_number(mobTable->bSungMaStr, csvTable.AsStringByIndex(col++));
	str_to_number(mobTable->bSungMaDex, csvTable.AsStringByIndex(col++));
	str_to_number(mobTable->bSungMaCon, csvTable.AsStringByIndex(col++));
	str_to_number(mobTable->bSungMaInt, csvTable.AsStringByIndex(col++));
	str_to_number(mobTable->dwDamageRange[0], csvTable.AsStringByIndex(col++));
	str_to_number(mobTable->dwDamageRange[1], csvTable.AsStringByIndex(col++));
	str_to_number(mobTable->dwMaxHP, csvTable.AsStringByIndex(col++));
	str_to_number(mobTable->bRegenCycle, csvTable.AsStringByIndex(col++));
	str_to_number(mobTable->bRegenPercent, csvTable.AsStringByIndex(col++));
	str_to_number(mobTable->dwGoldMin, csvTable.AsStringByIndex(col++));
	str_to_number(mobTable->dwGoldMax, csvTable.AsStringByIndex(col++));
	str_to_number(mobTable->dwExp, csvTable.AsStringByIndex(col++));
	str_to_number(mobTable->dwSungMaExp, csvTable.AsStringByIndex(col++));
	str_to_number(mobTable->wDef, csvTable.AsStringByIndex(col++));
	str_to_number(mobTable->sAttackSpeed, csvTable.AsStringByIndex(col++));
	str_to_number(mobTable->sMovingSpeed, csvTable.AsStringByIndex(col++));
	str_to_number(mobTable->bAggressiveHPPct, csvTable.AsStringByIndex(col++));
	str_to_number(mobTable->wAggressiveSight, csvTable.AsStringByIndex(col++));
	str_to_number(mobTable->wAttackRange, csvTable.AsStringByIndex(col++));

	str_to_number(mobTable->dwDropItemVnum, csvTable.AsStringByIndex(col++));
	str_to_number(mobTable->dwResurrectionVnum, csvTable.AsStringByIndex(col++));
	for (int i = 0; i < MOB_ENCHANTS_MAX_NUM; ++i)
		str_to_number(mobTable->cEnchants[i], csvTable.AsStringByIndex(col++));

	for (int i = 0; i < MOB_RESISTS_MAX_NUM; ++i)
		str_to_number(mobTable->cResists[i], csvTable.AsStringByIndex(col++));

#if defined(__ELEMENT_SYSTEM__)
	for (int i = 0; i < MOB_ELEMENT_MAX_NUM; ++i)
		str_to_number(mobTable->cElements[i], csvTable.AsStringByIndex(col++));

	str_to_number(mobTable->cResistDark, csvTable.AsStringByIndex(col++));
	str_to_number(mobTable->cResistIce, csvTable.AsStringByIndex(col++));
	str_to_number(mobTable->cResistEarth, csvTable.AsStringByIndex(col++));
#endif

	str_to_number(mobTable->fDamMultiply, csvTable.AsStringByIndex(col++));
	str_to_number(mobTable->dwSummonVnum, csvTable.AsStringByIndex(col++));
	str_to_number(mobTable->dwDrainSP, csvTable.AsStringByIndex(col++));

	// MOB_COLOR
	++col;

	str_to_number(mobTable->dwPolymorphItemVnum, csvTable.AsStringByIndex(col++));

	str_to_number(mobTable->Skills[0].bLevel, csvTable.AsStringByIndex(col++));
	str_to_number(mobTable->Skills[0].dwVnum, csvTable.AsStringByIndex(col++));
	str_to_number(mobTable->Skills[1].bLevel, csvTable.AsStringByIndex(col++));
	str_to_number(mobTable->Skills[1].dwVnum, csvTable.AsStringByIndex(col++));
	str_to_number(mobTable->Skills[2].bLevel, csvTable.AsStringByIndex(col++));
	str_to_number(mobTable->Skills[2].dwVnum, csvTable.AsStringByIndex(col++));
	str_to_number(mobTable->Skills[3].bLevel, csvTable.AsStringByIndex(col++));
	str_to_number(mobTable->Skills[3].dwVnum, csvTable.AsStringByIndex(col++));
	str_to_number(mobTable->Skills[4].bLevel, csvTable.AsStringByIndex(col++));
	str_to_number(mobTable->Skills[4].dwVnum, csvTable.AsStringByIndex(col++));

	str_to_number(mobTable->bBerserkPoint, csvTable.AsStringByIndex(col++));
	str_to_number(mobTable->bStoneSkinPoint, csvTable.AsStringByIndex(col++));
	str_to_number(mobTable->bGodSpeedPoint, csvTable.AsStringByIndex(col++));
	str_to_number(mobTable->bDeathBlowPoint, csvTable.AsStringByIndex(col++));
	str_to_number(mobTable->bRevivePoint, csvTable.AsStringByIndex(col++));

	str_to_number(mobTable->bHealPoint, csvTable.AsStringByIndex(col++));
	str_to_number(mobTable->bRAttSpeedPoint, csvTable.AsStringByIndex(col++));
	str_to_number(mobTable->bRCastSpeedPoint, csvTable.AsStringByIndex(col++));
	str_to_number(mobTable->bRHPRegenPoint, csvTable.AsStringByIndex(col++));
	str_to_number(mobTable->fHitRange, csvTable.AsStringByIndex(col++));

	sys_log(0, "MOB #%-5d %-24s level: %-3u rank: %u empire: %d", mobTable->dwVnum, mobTable->szLocaleName, mobTable->bLevel, mobTable->bRank, mobTable->bEmpire);

	return true;
}

bool Set_Proto_Item_Table(TItemTable* itemTable, cCsvTable& csvTable, std::map<int, const char*>& nameMap)
{
	// vnum 및 vnum range 읽기.
	{
		std::string s(csvTable.AsStringByIndex(0));
		int pos = s.find("~");
		// vnum 필드에 '~'가 없다면 패스
		if (std::string::npos == pos)
		{
			itemTable->dwVnum = atoi(s.c_str());
			if (0 == itemTable->dwVnum)
			{
				printf("INVALID VNUM %s\n", s.c_str());
				return false;
			}
			itemTable->dwVnumRange = 0;
		}
		else
		{
			std::string s_start_vnum(s.substr(0, pos));
			std::string s_end_vnum(s.substr(pos + 1));

			int start_vnum = atoi(s_start_vnum.c_str());
			int end_vnum = atoi(s_end_vnum.c_str());
			if (0 == start_vnum || (0 != end_vnum && end_vnum < start_vnum))
			{
				sys_err("INVALID VNUM %s", s.c_str());
				return false;
			}
			itemTable->dwVnum = start_vnum;
			itemTable->dwVnumRange = end_vnum - start_vnum;
		}
	}

	int col = 1;

	strlcpy(itemTable->szName, csvTable.AsStringByIndex(col++), sizeof(itemTable->szName));
	// 지역별 이름 넣어주기.
	std::map<int, const char*>::iterator it;
	it = nameMap.find(itemTable->dwVnum);
	if (it != nameMap.end())
	{
		const char* localeName = it->second;
		strlcpy(itemTable->szLocaleName, localeName, sizeof(itemTable->szLocaleName));
	}
	else
	{
		strlcpy(itemTable->szLocaleName, itemTable->szName, sizeof(itemTable->szLocaleName));
	}

	itemTable->bType = get_Item_Type_Value(csvTable.AsStringByIndex(col++));
	itemTable->bSubType = get_Item_SubType_Value(itemTable->bType, csvTable.AsStringByIndex(col++));
	itemTable->bSize = atoi(csvTable.AsStringByIndex(col++));
	itemTable->ullAntiFlags = get_Item_AntiFlag_Value(csvTable.AsStringByIndex(col++));
	itemTable->dwFlags = get_Item_Flag_Value(csvTable.AsStringByIndex(col++));
	itemTable->dwWearFlags = get_Item_WearFlag_Value(csvTable.AsStringByIndex(col++));
	itemTable->dwImmuneFlag = get_Item_Immune_Value(csvTable.AsStringByIndex(col++));
	itemTable->dwShopBuyPrice = atoi(csvTable.AsStringByIndex(col++));
	itemTable->dwShopSellPrice = atoi(csvTable.AsStringByIndex(col++));
	itemTable->dwRefinedVnum = atoi(csvTable.AsStringByIndex(col++));
	itemTable->wRefineSet = atoi(csvTable.AsStringByIndex(col++));
	itemTable->dw67AttrMaterial = atoi(csvTable.AsStringByIndex(col++));
	itemTable->bAlterToMagicItemPct = atoi(csvTable.AsStringByIndex(col++));
	itemTable->cLimitRealTimeFirstUseIndex = -1;
	itemTable->cLimitTimerBasedOnWearIndex = -1;

	for (int i = 0; i < ITEM_LIMIT_MAX_NUM; ++i)
	{
		itemTable->aLimits[i].bType = get_Item_LimitType_Value(csvTable.AsStringByIndex(col++));
		itemTable->aLimits[i].lValue = atoi(csvTable.AsStringByIndex(col++));

		if (LIMIT_REAL_TIME_START_FIRST_USE == itemTable->aLimits[i].bType)
			itemTable->cLimitRealTimeFirstUseIndex = (char)i;

		if (LIMIT_TIMER_BASED_ON_WEAR == itemTable->aLimits[i].bType)
			itemTable->cLimitTimerBasedOnWearIndex = (char)i;
	}

	for (int i = 0; i < ITEM_APPLY_MAX_NUM; ++i)
	{
		itemTable->aApplies[i].wType = get_Item_ApplyType_Value(csvTable.AsStringByIndex(col++));
		itemTable->aApplies[i].lValue = atoi(csvTable.AsStringByIndex(col++));
	}

	for (int i = 0; i < ITEM_VALUES_MAX_NUM; ++i)
		itemTable->alValues[i] = atoi(csvTable.AsStringByIndex(col++));

	itemTable->bSpecular = atoi(csvTable.AsStringByIndex(col++));
	itemTable->bGainSocketPct = atoi(csvTable.AsStringByIndex(col++));
	itemTable->sAddonType = atoi(csvTable.AsStringByIndex(col++));

	itemTable->bWeight = 0;

	sys_log(0, "ITEM #%-5d %-24s", itemTable->dwVnum, itemTable->szLocaleName);

	return true;
}
#endif
