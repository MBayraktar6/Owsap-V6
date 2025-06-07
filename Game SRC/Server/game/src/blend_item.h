/**
* File : blend_item.h
* Date : 2007.02.24
* Author : mhh
* Description :
**/

#ifndef __INC_BLEND_ITEM_H__
#define __INC_BLEND_ITEM_H__

#define MAX_BLEND_ITEM_VALUE 5

bool Blend_Item_init();
bool Blend_Item_load(char* file);
bool Blend_Item_set_value(LPITEM item);
bool Blend_Item_find(DWORD item_vnum);

#endif // __INC_BLEND_ITEM_H__
