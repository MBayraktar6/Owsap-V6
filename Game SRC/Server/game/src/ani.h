/**
* File : ani.h
* Date : 2007.11.16
* Author : mhh
* Description :
**/

#ifndef __INC_ANI_H__
#define __INC_ANI_H__

void ani_init();
DWORD ani_attack_speed(LPCHARACTER ch);
void ani_print_attack_speed();
DWORD ani_combo_speed(LPCHARACTER ch, BYTE combo);

#endif /* __INC_ANI_H__ */
