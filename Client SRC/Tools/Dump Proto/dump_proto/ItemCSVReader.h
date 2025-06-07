#ifndef __INC_ITEM_CSV_READER_H__
#define __INC_ITEM_CSV_READER_H__

#include "../../../Client/UserInterface/Locale_inc.h"

#define ENABLE_GOLD_VALUES

#include <iostream>

// csv 파일을 읽어와서 아이템 테이블에 넣어준다.
void putItemIntoTable(); // (테이블, 테스트여부)

int get_Item_Type_Value(std::string inputString);
int get_Item_SubType_Value(int type_value, std::string inputString);
uint64_t get_Item_AntiFlag_Value(std::string inputString);
int get_Item_Flag_Value(std::string inputString);
int get_Item_WearFlag_Value(std::string inputString);
int get_Item_Immune_Value(std::string inputString);
int get_Item_LimitType_Value(std::string inputString);
int get_Item_ApplyType_Value(std::string inputString);

// 몬스터 프로토도 읽을 수 있다.
int get_Mob_Rank_Value(std::string inputString);
int get_Mob_Type_Value(std::string inputString);
int get_Mob_BattleType_Value(std::string inputString);

int get_Mob_Size_Value(std::string inputString);
int get_Mob_AIFlag_Value(std::string inputString);
int get_Mob_RaceFlag_Value(std::string inputString);
int get_Mob_ImmuneFlag_Value(std::string inputString);

#endif // __INC_ITEM_CSV_READER_H__
