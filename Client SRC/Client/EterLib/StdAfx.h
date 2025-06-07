#pragma once

#define WIN32_LEAN_AND_MEAN
#ifndef _CRT_SECURE_NO_WARNINGS
#	define _CRT_SECURE_NO_WARNINGS
#endif

#include "../UserInterface/Locale_inc.h"

#define _WIN32_DCOM

#pragma warning(disable : 4710) // not inlined
#pragma warning(disable : 4786) // character 255 넘어가는거 끄기
#pragma warning(disable : 4244) // type conversion possible lose of data

#pragma warning(disable : 4018)
#pragma warning(disable : 4245)
#pragma warning(disable : 4512)
#pragma warning(disable : 4201)

#if _MSC_VER >= 1400
#	pragma warning(disable : 4201 4512 4238 4239)
#endif

#include <d3d8.h>
#include <d3dx8.h>

#define DIRECTINPUT_VERSION 0x0800

#include <dinput.h>

#pragma warning(disable : 4201)
#include <mmsystem.h>
#pragma warning(default : 4201)
#include <process.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <direct.h>
#include <malloc.h>

#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "d3d8.lib")
#pragma comment(lib, "d3dx8.lib")

#include "../EterBase/StdAfx.h"
#include "../EterBase/Debug.h"
#include "../EterLocale/CodePageId.h"

#ifndef VC_EXTRALEAN
#include <winsock.h>
#endif

#include <future>
#include <atomic>
