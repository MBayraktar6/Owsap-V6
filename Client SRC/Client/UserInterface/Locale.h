#pragma once

#include "Locale_inc.h"

bool LocaleService_IsYMIR();
bool LocaleService_IsJAPAN();
bool LocaleService_IsENGLISH();
bool LocaleService_IsHONGKONG();
bool LocaleService_IsTAIWAN();
bool LocaleService_IsNEWCIBN();
bool LocaleService_IsEUROPE();

unsigned LocaleService_GetCodePage();
const char* LocaleService_GetName();
const char* LocaleService_GetLocaleName();
const char* LocaleService_GetLocalePath();
const char* LocaleService_GetSecurityKey();
BOOL LocaleService_IsLeadByte(const char chByte);
int LocaleService_StringCompareCI(LPCSTR szStringLeft, LPCSTR szStringRight, size_t sizeLength);

void LocaleService_ForceSetLocale(const char* name, const char* localePath);
void LocaleService_LoadConfig(const char* fileName);
#if defined(ENABLE_LOCALE_CLIENT)
void LocaleService_LoadLocaConfig();
#endif
bool LocaleService_LoadGlobal(HINSTANCE hInstance);
unsigned LocaleService_GetLastExp(int level);
int LocaleService_GetSkillPower(unsigned level);
#if defined(ENABLE_PARTY_PROFICY)
int LocaleService_GetRoleProficiencyLeaderBonus(unsigned level);
#endif

// CHEONMA
void LocaleService_SetCHEONMA(bool isEnable);
bool LocaleService_IsCHEONMA();
// END_OF_CHEONMA

#if defined(ENABLE_MULTI_LANGUAGE_SYSTEM)
bool LocaleService_GetLocale(const char* locale);
const char* LocaleService_GetCountry(const char* country);
#endif
