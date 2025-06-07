#pragma once

#pragma warning(disable : 4702)
#pragma warning(disable : 4100)
#pragma warning(disable : 4201)
#pragma warning(disable : 4511)
#pragma warning(disable : 4663)
#pragma warning(disable : 4018)
#pragma warning(disable : 4245)
#pragma warning(disable : 4838)
#pragma warning(disable : 26495)

#if _MSC_VER >= 1400
// if don't use below, time_t is 64bit
#	define _USE_32BIT_TIME_T
#endif

#include "../EterLib/StdAfx.h"
#include "../EterPythonLib/StdAfx.h"
#include "../GameLib/StdAfx.h"
#include "../ScriptLib/StdAfx.h"
#include "../MilesLib/StdAfx.h"
#include "../EffectLib/StdAfx.h"
#include "../PRTerrainLib/StdAfx.h"
#include "../SpeedTreeLib/StdAfx.h"

#ifndef __D3DRM_H__
#	define __D3DRM_H__
#endif

#include <dshow.h>
#include <qedit.h>

#include "Locale.h"
#include "GameType.h"

extern DWORD __DEFAULT_CODE_PAGE__;

#define APP_NAME "Metin 2"

void initudp();
void initapp();
void initime();
#if defined(_CYTHON)
void initsystemSetting();
#else
void initsystem();
#endif
void initchr();
void initchrmgr();
void initChat();
void initTextTail();
void initime();
void initItem();
void initNonPlayer();
void initnet();
void initPlayer();
void initSectionDisplayer();
void initServerStateChecker();
void initTrade();
void initMiniMap();
void initProfiler();
void initEvent();
void initeffect();
void initsnd();
void initeventmgr();
void initBackground();
void initwndMgr();
void initshop();
void initpack();
void initskill();
void initfly();
void initquest();
#if defined(ENABLE_RANKING_SYSTEM)
void initranking();
#endif
#if defined(ENABLE_MAILBOX)
void initmail();
#endif
void initsafebox();
void initguild();
void initMessenger();
#if defined(_CYTHON)
void initrootlibManager();
#endif
#if defined(ENABLE_EVENT_BANNER)
void initInGameEventSystem();
#endif

extern const std::string& ApplicationStringTable_GetString(DWORD dwID);
extern const std::string& ApplicationStringTable_GetString(DWORD dwID, LPCSTR szKey);

extern const char* ApplicationStringTable_GetStringz(DWORD dwID);
extern const char* ApplicationStringTable_GetStringz(DWORD dwID, LPCSTR szKey);

extern void ApplicationSetErrorString(const char* szErrorString);
