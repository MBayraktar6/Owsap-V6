#include "StdAfx.h"
#include "PythonApplication.h"
#include "ProcessScanner.h"
#include "PythonExceptionSender.h"
#include "resource.h"

#ifdef _DEBUG
//#include <crtdbg.h>
#endif

#include "../EterPack/EterPackManager.h"
#include "../EterLib/Util.h"
#include "../CWebBrowser/CWebBrowser.h"
#include "CefWebBrowser.h"
#include "../EterBase/CPostIt.h"

#include "CheckLatestFiles.h"

#if defined(ENABLE_CHECK_DLL_MD5)
#	include "md5.h"
#endif

// NOTE : Trick AMD and Nvidia drivers to use the most powerful GPU instead
// of a lower-performance (such as integrated) GPU.
extern "C"
{
	extern int _fltused;
	volatile int _AVOID_FLOATING_POINT_LIBRARY_BUG = _fltused;

	__declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
	__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;

};
extern "C" { FILE __iob_func[3] = { *stdin, *stdout, *stderr }; }

#pragma comment(linker, "/NODEFAULTLIB:libci.lib")

#pragma comment(lib, "version.lib")
#pragma comment(lib, "python27.lib")
#pragma comment(lib, "imagehlp.lib")
#pragma comment(lib, "devil.lib")
#pragma comment(lib, "granny2.lib")
#pragma comment(lib, "mss32.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "imm32.lib")
#pragma comment(lib, "oldnames.lib")
#pragma comment(lib, "SpeedTreeRT.lib")
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "strmiids.lib")
#pragma comment(lib, "ddraw.lib")
#pragma comment(lib, "dmoguids.lib")
//#pragma comment(lib, "wsock32.lib")

#if defined(ENABLE_DISCORD_RPC)
#	if defined(_DEBUG)
#		pragma comment(lib, "Debug/DiscordRPC.lib")
#	else
#		pragma comment(lib, "Release/DiscordRPC.lib")
#	endif
#endif

#if defined(_DEBUG)
#	pragma comment(lib, "Debug/libcef.lib")
#	pragma comment(lib, "Debug/libcef_dll_wrapper.lib")
#else
#	pragma comment(lib, "Release/libcef.lib")
#	pragma comment(lib, "Release/libcef_dll_wrapper.lib")
#endif

#include <stdlib.h>
#include <cryptopp/cryptoppLibLink.h>
bool __IS_TEST_SERVER_MODE__ = false;

#if defined(_CYTHON)
#include "PythonrootlibManager.h"
#endif

extern bool SetDefaultCodePage(DWORD codePage);

static const char* sc_apszPythonLibraryFilenames[] =
{
	"UserDict.pyc",
	"__future__.pyc",
	"copy_reg.pyc",
	"linecache.pyc",
	"ntpath.pyc",
	"os.pyc",
	"site.pyc",
	"stat.pyc",
	"string.pyc",
	"traceback.pyc",
	"types.pyc",
	"\n",
};

#if defined(ENABLE_CHECK_DLL_MD5)
static const char* ar_szMD5FileNames[][2] =
{
	// Miles
	{ "miles/mss32.dll", "6400e224b8b44ece59a992e6d8233719" },
	{ "miles/mssa3d.m3d", "e089ce52b0617a6530069f22e0bdba2a" },
	{ "miles/mssds3d.m3d", "85267776d45dbf5475c7d9882f08117c" },
	{ "miles/mssdsp.flt", "cb71b1791009eca618e9b1ad4baa4fa9" },
	{ "miles/mssdx7.m3d", "2727e2671482a55b2f1f16aa88d2780f" },
	{ "miles/msseax.m3d", "788bd950efe89fa5166292bd6729fa62" },
	{ "miles/mssmp3.asi", "189576dfe55af3b70db7e3e2312cd0fd" },
	{ "miles/mssrsx.m3d", "7fae15b559eb91f491a5f75cfa103cd4" },
	{ "miles/msssoft.m3d", "bdc9ad58ade17dbd939522eee447416f" },
	{ "miles/mssvoice.asi", "3d5342edebe722748ace78c930f4d8a5" },

#if defined(ENABLE_CEF_BROWSER)
	{ "cef.pak", "290f0060612f305b075f92b7fd1ad036" },
	{ "cef_100_percent.pak", "0a20cb807b4c606db89fe14cae041885" },
	{ "cef_200_percent.pak", "a3d62f9485070ca84ca074a12b3fb4e8" },
	{ "cef_extensions.pak", "a60a12468870b84fffee8d9dea64ff74" },
	{ "d3dcompiler_43.dll", "1c9b45e87528b8bb8cfa884ea0099a85" },
	{ "d3dcompiler_47.dll", "8d5695f0b0a0330fe07802e4f3576b15" },
	{ "icudtl.dat", "3ed56e55ff45ab973ffc483e5d483a5a" },
	{ "libcef.dll", "e5f94b992adddb5a08b36bfab4420e0f" },
	{ "libEGL.dll", "da896a0034a9f4f18f510e4a37a2dff0" },
	{ "libGLESv2.dll", "9da02ce63ef4433c12eb854cc98dc75f" },
	{ "natives_blob.bin", "03dc57f8a76d2a781ba2d8b3777b58d1" },
	{ "snapshot_blob.bin", "4fde446a9303d60f7eb76438cab8ba30" },
#endif

	{ "config.exe", "df9f6413dbffe48212c61ad93e228a7d" },
	{ "config.ini", "216cfa8cad9ae8b52141c8cbc335df96" },
	{ "locale_list.txt", "d48c75f94a2bacd5542183ad149f6b5f" },

#if defined(__ENABLE_CRASH_RPT__)
	{ "CrashSender.exe", "bf9539c750ead70371ef26cb55b6d61d" },
	{ "CrashRpt.dll", "ab80f2f623b2c6f194ab92cd5fe534eb" },
	{ "crashrpt_lang.ini", "771da39b527e886a247a0c0a33ffb715" },
#endif

	{ "devil.dll", "8df4d4324e5755f1a0567db3c5be4c58" },
	{ "granny2.dll", "6f403e1359bbc6b5f23b2def240e4c3c" },
	{ "MSS32.DLL", "6400e224b8b44ece59a992e6d8233719" },
	{ "python27.dll", "a35fef665eec18e0bf4195359a106261" },
	{ "SpeedTreeRT.dll", "1ac3d612389fa679f5ca3c6bab855145" },
};
#endif

char gs_szErrorString[512] = "";

void ApplicationSetErrorString(const char* szErrorString)
{
	strcpy(gs_szErrorString, szErrorString);
}

bool CheckPythonLibraryFilenames()
{
	for (int i = 0; *sc_apszPythonLibraryFilenames[i] != '\n'; ++i)
	{
		std::string stFilename = "lib\\";
		stFilename += sc_apszPythonLibraryFilenames[i];

		if (_access(stFilename.c_str(), 0) != 0)
		{
			return false;
		}

		MoveFile(stFilename.c_str(), stFilename.c_str());
	}

	return true;
}

#if defined(ENABLE_CEF_BROWSER)
static CefRefPtr<ClientApp> gs_pkClientApp;
#endif

struct ApplicationStringTable
{
	HINSTANCE m_hInstance;
	std::map<DWORD, std::string> m_kMap_dwID_stLocale;
} gs_kAppStrTable;

void ApplicationStringTable_Initialize(HINSTANCE hInstance)
{
	gs_kAppStrTable.m_hInstance = hInstance;
}

const std::string& ApplicationStringTable_GetString(DWORD dwID, LPCSTR szKey)
{
	char szBuffer[512];
	char szIniFileName[256];
	char szLocale[256];

	::GetCurrentDirectory(sizeof(szIniFileName), szIniFileName);
	if (szIniFileName[lstrlen(szIniFileName) - 1] != '\\')
		strcat(szIniFileName, "\\");
	strcat(szIniFileName, "metin2client.dat");

	strcpy(szLocale, LocaleService_GetLocalePath());
	if (strnicmp(szLocale, "locale/", strlen("locale/")) == 0)
		strcpy(szLocale, LocaleService_GetLocalePath() + strlen("locale/"));
	::GetPrivateProfileString(szLocale, szKey, NULL, szBuffer, sizeof(szBuffer) - 1, szIniFileName);
	if (szBuffer[0] == '\0')
		LoadString(gs_kAppStrTable.m_hInstance, dwID, szBuffer, sizeof(szBuffer) - 1);
	if (szBuffer[0] == '\0')
		::GetPrivateProfileString("en", szKey, NULL, szBuffer, sizeof(szBuffer) - 1, szIniFileName);
	if (szBuffer[0] == '\0')
		strcpy(szBuffer, szKey);

	std::string& rstLocale = gs_kAppStrTable.m_kMap_dwID_stLocale[dwID];
	rstLocale = szBuffer;

	return rstLocale;
}

const std::string& ApplicationStringTable_GetString(DWORD dwID)
{
	char szBuffer[512];

	LoadString(gs_kAppStrTable.m_hInstance, dwID, szBuffer, sizeof(szBuffer) - 1);
	std::string& rstLocale = gs_kAppStrTable.m_kMap_dwID_stLocale[dwID];
	rstLocale = szBuffer;

	return rstLocale;
}

const char* ApplicationStringTable_GetStringz(DWORD dwID, LPCSTR szKey)
{
	return ApplicationStringTable_GetString(dwID, szKey).c_str();
}

const char* ApplicationStringTable_GetStringz(DWORD dwID)
{
	return ApplicationStringTable_GetString(dwID).c_str();
}

////////////////////////////////////////////

int Setup(LPSTR lpCmdLine); // Internal function forward

#if defined(DISABLE_INDEX_FILE)
bool PackInitialize(const char* c_pszFolder)
{
	std::vector<std::vector<std::string>> indexVec
	{
		{ "pack/", "patch_osf" },
		{ "d:/ymir work/pc/", "pc" },
		{ "d:/ymir work/pc2/", "pc2" },
		{ "d:/ymir work/pc3/", "pc3" },
		{ "d:/ymir work/monster/", "monster" },
		{ "d:/ymir work/monster2/", "monster2" },
		{ "d:/ymir work/effect/", "effect" },
		{ "d:/ymir work/zone/", "zone" },
		{ "d:/ymir work/terrainmaps/", "terrain" },
		{ "d:/ymir work/npc/", "npc" },
		{ "d:/ymir work/npc2/", "npc2" },
		{ "d:/ymir work/npc_mount/", "npc_mount" },
		{ "d:/ymir work/npc_pet/", "npc_pet" },
		{ "d:/ymir work/tree/", "tree" },
		{ "d:/ymir work/guild/", "guild" },
		{ "d:/ymir work/item/", "item" },
		{ "textureset/", "textureset" },
		{ "property/", "property" },
		{ "icon/", "icon" },
		// Season
		{ "season1/", "season1" },
		{ "season2/", "season2" },
		// Maps
		{ "gm_guild_build/", "map" },
		{ "map_a2/", "map" },
		{ "map_n_snowm_01/", "map" },
		{ "metin2_12zi_stage/", "map" },
		{ "metin2_guild_pve/", "map" },
		{ "metin2_guild_village/", "map" },
		{ "metin2_guild_village_01/", "map" },
		{ "metin2_guild_village_02/", "map" },
		{ "metin2_guild_village_03/", "map" },
		{ "metin2_guild_war1/", "map" },
		{ "metin2_guild_war2/", "map" },
		{ "metin2_guild_war3/", "map" },
		{ "metin2_guild_war4/", "map" },
		{ "metin2_map_a1/", "map" },
		{ "metin2_map_a3/", "map" },
		{ "metin2_map_b1/", "map" },
		{ "metin2_map_b3/", "map" },
		{ "metin2_map_battlefied/", "map" },
		{ "metin2_map_bayblacksand/", "map" },
		{ "metin2_map_boss_awaken_dawnmist/", "map" },
		{ "metin2_map_boss_awaken_flame/", "map" },
		{ "metin2_map_boss_awaken_skipia/", "map" },
		{ "metin2_map_boss_awaken_snow/", "map" },
		{ "metin2_map_boss_crack_dawnmist/", "map" },
		{ "metin2_map_boss_crack_flame/", "map" },
		{ "metin2_map_boss_crack_skipia/", "map" },
		{ "metin2_map_boss_crack_snow/", "map" },
		{ "metin2_map_c1/", "map" },
		{ "metin2_map_c3/", "map" },
		{ "metin2_map_capedragonhead/", "map" },
		{ "metin2_map_dawnmistwood/", "map" },
		{ "metin2_map_dawnmist_dungeon_01/", "map" },
		{ "metin2_map_defensewave/", "map" },
		{ "metin2_map_defensewave_port/", "map" },
		{ "metin2_map_devilscatacomb/", "map" },
		{ "metin2_map_deviltower1/", "map" },
		{ "metin2_map_duel/", "map" },
		{ "metin2_map_e1/", "map" },
		{ "metin2_map_elemental_01/", "map" },
		{ "metin2_map_elemental_02/", "map" },
		{ "metin2_map_elemental_03/", "map" },
		{ "metin2_map_elemental_04/", "map" },
		{ "metin2_map_guild_01/", "map" },
		{ "metin2_map_guild_02/", "map" },
		{ "metin2_map_guild_03/", "map" },
		{ "metin2_map_labyrinth/", "map" },
		{ "metin2_map_milgyo/", "map" },
		{ "metin2_map_miniboss_01/", "map" },
		{ "metin2_map_miniboss_02/", "map" },
		{ "metin2_map_mists_of_island/", "map" },
		{ "metin2_map_monkeydungeon/", "map" },
		{ "metin2_map_monkeydungeon_02/", "map" },
		{ "metin2_map_monkeydungeon_03/", "map" },
		{ "metin2_map_mt_thunder/", "map" },
		{ "metin2_map_mt_th_dungeon_01/", "map" },
		{ "metin2_map_n_desert_01/", "map" },
		{ "metin2_map_n_flame_01/", "map" },
		{ "metin2_map_n_flame_dragon/", "map" },
		{ "metin2_map_n_flame_dungeon_01/", "map" },
		{ "metin2_map_n_snow_dungeon_01/", "map" },
		{ "metin2_map_pvp_arena/", "map" },
		{ "metin2_map_skipia_dungeon_02/", "map" },
		{ "metin2_map_skipia_dungeon_boss/", "map" },
		{ "metin2_map_spiderdungeon/", "map" },
		{ "metin2_map_spiderdungeon_02/", "map" },
		{ "metin2_map_spiderdungeon_03/", "map" },
		{ "metin2_map_t1/", "map" },
		{ "metin2_map_t2/", "map" },
		{ "metin2_map_t3/", "map" },
		{ "metin2_map_t4/", "map" },
		{ "metin2_map_trent/", "map" },
		{ "metin2_map_trent02/", "map" },
		{ "metin2_map_wedding_01/", "map" },
		// Sound
		{ "sound/ambience/", "sound" },
		{ "sound/common/", "sound" },
		{ "sound/effect/", "sound" },
		{ "sound/monster/", "sound" },
		{ "sound/npc/", "sound" },
		{ "sound/pc/", "sound" },
		{ "sound/ui/", "sound" },
		{ "sound/ambience/", "sound" },
		{ "sound/common/", "sound" },
		{ "sound/effect/", "sound" },
		{ "sound/monster/", "sound" },
		{ "sound/npc/", "sound" },
		{ "sound/npc_mount/", "sound" },
		{ "sound/pc/", "sound" },
		{ "sound/ui/", "sound" },
		{ "sound/monster2/", "sound" },
		{ "sound/pc2/", "sound" },
		{ "sound/pc3/", "sound" },
		// BGM
		{ "bgm/", "bgm" },
		// ETC
		{ "d:/ymir work/special/", "special" },
		{ "d:/ymir work/environment/", "environment" },
		// LOCALE
		{ "locale/common/", "locale" },
		{ "locale/ca/", "locale" },
		{ "locale/ae/", "locale" },
		{ "locale/de/", "locale" },
		{ "locale/es/", "locale" },
		{ "locale/fr/", "locale" },
		{ "locale/gr/", "locale" },
		{ "locale/it/", "locale" },
		{ "locale/nl/", "locale" },
		{ "locale/pl/", "locale" },
		{ "locale/pt/", "locale" },
		{ "locale/tr/", "locale" },
		{ "locale/uk/", "locale" },
		{ "locale/bg/", "locale" },
		{ "locale/en/", "locale" },
		{ "locale/mx/", "locale" },
		{ "locale/ro/", "locale" },
		{ "locale/ru/", "locale" },
		{ "locale/dk/", "locale" },
		{ "locale/cz/", "locale" },
		{ "locale/hu/", "locale" },
		{ "locale/us/", "locale" },
		{ "locale/pa/", "locale" },
		{ "uiscript/", "uiscript" },
		{ "d:/ymir work/ui/", "ui" },
		{ "d:/ymir work/uiloading/", "uiloading" },
	};

	if (_access(c_pszFolder, 0) != 0)
		return true;

	std::string stFolder(c_pszFolder);
	stFolder += "/";

	const bool bPackFirst = false;

#if defined(USE_RELATIVE_PATH)
	CEterPackManager::Instance().SetRelativePathMode();
#endif
	CEterPackManager::Instance().SetCacheMode();
	CEterPackManager::Instance().SetSearchMode(bPackFirst);

	CSoundData::SetPackMode();

	std::string strPackName, strTexCachePackName;

	for (auto& elem : indexVec)
	{
		const std::string& c_rstFolder = elem[0];
		const std::string& c_rstName = elem[1];

		strPackName = stFolder + c_rstName;
		strTexCachePackName = strPackName + "_texcache";

		CEterPackManager::Instance().RegisterPack(strPackName.c_str(), c_rstFolder.c_str());
		CEterPackManager::Instance().RegisterPack(strTexCachePackName.c_str(), c_rstFolder.c_str());
	}

	CEterPackManager::Instance().RegisterRootPack((stFolder + std::string("root")).c_str());

	NANOEND

		return true;
}
#else
bool PackInitialize(const char* c_pszFolder)
{
	NANOBEGIN

		if (_access(c_pszFolder, 0) != 0)
			return true;

	std::string stFolder(c_pszFolder);
	stFolder += "/";

	std::string stFileName(stFolder);
	stFileName += "Index";

	CMappedFile file;
	LPCVOID pvData;

	if (!file.Create(stFileName.c_str(), &pvData, 0, 0))
	{
		LogBoxf("FATAL ERROR! File not exist: %s", stFileName.c_str());
		TraceError("FATAL ERROR! File not exist: %s", stFileName.c_str());
		return true;
	}

	CMemoryTextFileLoader TextLoader;
	TextLoader.Bind(file.Size(), pvData);

	bool bPackFirst = true;

	const std::string& strPackType = TextLoader.GetLineString(0);

	if (strPackType.compare("FILE") && strPackType.compare("PACK"))
	{
		TraceError("Pack/Index has invalid syntax. First line must be 'PACK' or 'FILE'");
		return false;
	}

#ifdef _DISTRIBUTE
	Tracef("알림: 팩 모드입니다.\n");

	/*
	if (0 == strPackType.compare("FILE"))
	{
		bPackFirst = FALSE;
		Tracef("알림: 파일 모드입니다.\n");
	}
	else
	{
		Tracef("알림: 팩 모드입니다.\n");
	}
	*/
#else
	bPackFirst = false;
	Tracef("알림: 파일 모드입니다.\n");
#endif

	CTextFileLoader::SetCacheMode();
#if defined(USE_RELATIVE_PATH)
	CEterPackManager::Instance().SetRelativePathMode();
#endif
	CEterPackManager::Instance().SetCacheMode();
	CEterPackManager::Instance().SetSearchMode(bPackFirst);

	CSoundData::SetPackMode(); // Miles 파일 콜백을 셋팅

	std::string strPackName, strTexCachePackName;
	for (DWORD i = 1; i < TextLoader.GetLineCount() - 1; i += 2)
	{
		const std::string& c_rstFolder = TextLoader.GetLineString(i);
		const std::string& c_rstName = TextLoader.GetLineString(i + 1);

		strPackName = stFolder + c_rstName;
		strTexCachePackName = strPackName + "_texcache";

		CEterPackManager::Instance().RegisterPack(strPackName.c_str(), c_rstFolder.c_str());
		CEterPackManager::Instance().RegisterPack(strTexCachePackName.c_str(), c_rstFolder.c_str());
	}

	CEterPackManager::Instance().RegisterRootPack((stFolder + std::string("root")).c_str());

	NANOEND

		return true;
}
#endif

bool RunMainScript(CPythonLauncher& pyLauncher, const char* lpCmdLine)
{
	initpack();
	initdbg();
	initime();
	initgrp();
	initgrpImage();
	initgrpText();
	initwndMgr();
	/////////////////////////////////////////////
	initudp();
	initapp();
#if defined(_CYTHON)
	initsystemSetting();
#else
	initsystem();
#endif
	initchr();
	initchrmgr();
	initPlayer();
	initItem();
	initNonPlayer();
	initTrade();
	initChat();
	initTextTail();
	initnet();
	initMiniMap();
	initProfiler();
	initEvent();
	initeffect();
	initfly();
	initsnd();
	initeventmgr();
	initshop();
	initskill();
	initquest();
	initBackground();
	initMessenger();
#if defined(ENABLE_MAILBOX)
	initmail();
#endif
	initsafebox();
	initguild();
	initServerStateChecker();
#if defined(_CYTHON)
	initrootlibManager();
#endif
#if defined(ENABLE_RANKING_SYSTEM)
	initranking();
#endif
#if defined(ENABLE_EVENT_BANNER)
	initInGameEventSystem();
#endif

	NANOBEGIN

#define __BUILT_IN__
#if defined(__BUILT_IN__)
		PyObject* builtins = PyImport_ImportModule("__builtin__");

	// RegisterDebugFlag
#ifdef _DEBUG
	PyModule_AddIntConstant(builtins, "__DEBUG__", 1);
#else
	PyModule_AddIntConstant(builtins, "__DEBUG__", 0);
#endif

	// RegisterCythonFlag
#if defined(_CYTHON)
	PyModule_AddIntConstant(builtins, "__USE_CYTHON__", 1);
#else
	PyModule_AddIntConstant(builtins, "__USE_CYTHON__", 0);
#endif
#else
		// RegisterDebugFlag
	{
		std::string stRegisterDebugFlag;

#ifdef _DEBUG
		stRegisterDebugFlag = "__DEBUG__ = 1";
#else
		stRegisterDebugFlag = "__DEBUG__ = 0";
#endif

		if (!pyLauncher.RunLine(stRegisterDebugFlag.c_str()))
		{
			TraceError("RegisterDebugFlag Error");
			return false;
		}
	}
#endif

	// RegisterCommandLine
	{
		std::string stRegisterCmdLine;

		const char* loginMark = "-cs";
		const char* loginMark_NonEncode = "-ncs";
		const char* seperator = " ";

		std::string stCmdLine;
		const int CmdSize = 3;
		std::vector<std::string> stVec;
		SplitLine(lpCmdLine, seperator, &stVec);
		if (CmdSize == stVec.size() && stVec[0] == loginMark)
		{
			char buf[MAX_PATH]; // TODO 아래 함수 string 형태로 수정
			base64_decode(stVec[2].c_str(), buf);
			stVec[2] = buf;
			string_join(seperator, stVec, &stCmdLine);
		}
		else if (CmdSize <= stVec.size() && stVec[0] == loginMark_NonEncode)
		{
			stVec[0] = loginMark;
			string_join(" ", stVec, &stCmdLine);
		}
		else
			stCmdLine = lpCmdLine;

#if defined(_CYTHON)
		PyModule_AddStringConstant(builtins, "__COMMAND_LINE__", stCmdLine.c_str());
#else
		stRegisterCmdLine = "__COMMAND_LINE__ = ";
		stRegisterCmdLine += '"';
		stRegisterCmdLine += stCmdLine;
		stRegisterCmdLine += '"';

		const CHAR* c_szRegisterCmdLine = stRegisterCmdLine.c_str();
		if (!pyLauncher.RunLine(c_szRegisterCmdLine))
		{
			TraceError("RegisterCommandLine Error");
			return false;
		}
#endif
	}
	{
		std::vector<std::string> stVec;
		SplitLine(lpCmdLine, " ", &stVec);

		if (stVec.size() != 0 && "--pause-before-create-window" == stVec[0])
		{
			system("pause");
		}

#if defined(_CYTHON)
		if (!pyLauncher.RunLine("import rootlib\nrootlib.moduleImport('system')"))
#else
		if (!pyLauncher.RunFile("system.py"))
#endif
		{
			TraceError("RunMain Error");
			return false;
		}
	}

	NANOEND
		return true;
}

bool Main(HINSTANCE hInstance, LPSTR lpCmdLine)
{
#ifdef LOCALE_SERVICE_YMIR
	extern bool g_isScreenShotKey;
	g_isScreenShotKey = true;
#endif

	DWORD dwRandSeed = time(NULL) + DWORD(GetCurrentProcess());
	srandom(dwRandSeed);
	srand(random());

	SetLogLevel(1);

#ifdef LOCALE_SERVICE_VIETNAM_MILD
	extern BOOL USE_VIETNAM_CONVERT_WEAPON_VNUM;
	USE_VIETNAM_CONVERT_WEAPON_VNUM = true;
#endif

	if (_access("perf_game_update.txt", 0) == 0)
	{
		DeleteFile("perf_game_update.txt");
	}

	if (_access("newpatch.exe", 0) == 0)
	{
		system("patchupdater.exe");
		return false;
	}

	// DevIL
	ilInit();

	if (!Setup(lpCmdLine))
		return false;

#if defined(_DEBUG)
#	if defined(ENABLE_CONSOLE_WINDOW)
	OpenConsoleWindow();
#	endif
	OpenLogFile(true); // true == uses syserr.txt and log.txt
#else
	OpenLogFile(false); // false == uses syserr.txt only
#endif

	static CLZO lzo;
	static CEterPackManager EterPackManager;

	if (!PackInitialize("pack"))
	{
		LogBox("Pack Initialization failed. Check log.txt file..");
		return false;
	}

	if (LocaleService_LoadGlobal(hInstance))
		SetDefaultCodePage(LocaleService_GetCodePage());

	CPythonApplication* app = new CPythonApplication;
	app->Initialize(hInstance);

	bool ret = false;
	{
		CPythonLauncher pyLauncher;
		CPythonExceptionSender pyExceptionSender;
		SetExceptionSender(&pyExceptionSender);

		if (pyLauncher.Create())
		{
			ret = RunMainScript(pyLauncher, lpCmdLine); // 게임 실행중엔 함수가 끝나지 않는다.
		}

		//ProcessScanner_ReleaseQuitEvent();

		// 게임 종료시.
		app->Clear();

		timeEndPeriod(1);
		pyLauncher.Clear();
	}

	app->Destroy();
	delete app;

	return ret;
}

HANDLE CreateMetin2GameMutex()
{
	SECURITY_ATTRIBUTES sa;
	ZeroMemory(&sa, sizeof(SECURITY_ATTRIBUTES));
	sa.nLength = sizeof(sa);
	sa.lpSecurityDescriptor = NULL;
	sa.bInheritHandle = FALSE;

	return CreateMutex(&sa, FALSE, "Metin2GameMutex");
}

void DestroyMetin2GameMutex(HANDLE hMutex)
{
	if (hMutex)
	{
		ReleaseMutex(hMutex);
		hMutex = NULL;
	}
}

void __ErrorPythonLibraryIsNotExist()
{
	LogBoxf("FATAL ERROR!! Python Library file not exist!");
}

bool __IsTimeStampOption(LPSTR lpCmdLine)
{
	const char* TIMESTAMP = "/timestamp";
	return (strncmp(lpCmdLine, TIMESTAMP, strlen(TIMESTAMP)) == 0);
}

void __PrintTimeStamp()
{
#ifdef _DEBUG
	if (__IS_TEST_SERVER_MODE__)
		LogBoxf("METIN2 BINARY TEST DEBUG VERSION %s ( MS C++ %d Compiled )", __TIMESTAMP__, _MSC_VER);
	else
		LogBoxf("METIN2 BINARY DEBUG VERSION %s ( MS C++ %d Compiled )", __TIMESTAMP__, _MSC_VER);

#else
	if (__IS_TEST_SERVER_MODE__)
		LogBoxf("METIN2 BINARY TEST VERSION %s ( MS C++ %d Compiled )", __TIMESTAMP__, _MSC_VER);
	else
		LogBoxf("METIN2 BINARY DISTRIBUTE VERSION %s ( MS C++ %d Compiled )", __TIMESTAMP__, _MSC_VER);
#endif
}

bool __IsLocaleOption(LPSTR lpCmdLine)
{
	return (strcmp(lpCmdLine, "--locale") == 0);
}

bool __IsLocaleVersion(LPSTR lpCmdLine)
{
	return (strcmp(lpCmdLine, "--perforce-revision") == 0);
}

#if defined(ENABLE_CHECK_DLL_MD5)
bool CheckDLLMD5()
{
	MD5 md5;
	for (int i = 0; i < std::size(ar_szMD5FileNames); i++)
	{
		if (strcmp(md5.digestFile((char*)ar_szMD5FileNames[i][0]), ar_szMD5FileNames[i][1]))
		{
			char szMessage[256];
			_snprintf(szMessage, sizeof(szMessage), ApplicationStringTable_GetStringz(IDS_ERR_NOT_LATEST_FILE, "ERR_NOT_LATEST_FILE"), ar_szMD5FileNames[i][0]);
			MessageBox(NULL, szMessage, ApplicationStringTable_GetStringz(IDS_APP_NAME, "APP_NAME"), MB_ICONSTOP);
			return false;
		}
	}
	return true;
}
#endif

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
#ifdef _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_CHECK_CRT_DF | _CRTDBG_LEAK_CHECK_DF);
	//_CrtSetBreakAlloc(110247); 
#endif

	ApplicationStringTable_Initialize(hInstance);

	LocaleService_LoadConfig("locale.cfg");
#if defined(ENABLE_LOCALE_CLIENT)
	LocaleService_LoadLocaConfig();
#endif

	SetDefaultCodePage(LocaleService_GetCodePage());

#if defined(ENABLE_CHECK_DLL_MD5)
	if (!CheckDLLMD5())
		return 0;
#endif

#if defined(CHECK_LATEST_DATA_FILES)
	if (!CheckLatestFiles())
		return 0;
#endif

	bool bQuit = false;
	int nArgc = 0;
	PCHAR* szArgv = CommandLineToArgv(lpCmdLine, &nArgc);

	for (int i = 0; i < nArgc; i++)
	{
		if (szArgv[i] == 0)
			continue;

		if (__IsLocaleVersion(szArgv[i])) // #0000829: [M2EU] 버전 파일이 항상 생기지 않도록 수정 
		{
			char szModuleName[MAX_PATH];
			char szVersionPath[MAX_PATH];
			GetModuleFileName(NULL, szModuleName, sizeof(szModuleName));
			sprintf(szVersionPath, "%s.version", szModuleName);
			FILE* fp = fopen(szVersionPath, "wt");
			if (fp)
			{
				fprintf(fp, "r%d\n", VERSION);
				fclose(fp);
			}
			bQuit = true;
		}
		else if (__IsLocaleOption(szArgv[i]))
		{
			FILE* fp = fopen("locale.txt", "wt");
			fprintf(fp, "service[%s] code_page[%d]",
				LocaleService_GetName(), LocaleService_GetCodePage());
			fclose(fp);
			bQuit = true;
		}
		else if (__IsTimeStampOption(szArgv[i]))
		{
			__PrintTimeStamp();
			bQuit = true;
		}
		else if ((strcmp(szArgv[i], "--force-set-locale") == 0))
		{
			// locale 설정엔 인자가 두 개 더 필요함 (로케일 명칭, 데이터 경로)
			if (nArgc <= i + 2)
			{
				MessageBox(NULL, "Invalid arguments", ApplicationStringTable_GetStringz(IDS_APP_NAME, "APP_NAME"), MB_ICONSTOP);
				goto Clean;
			}

			const char* localeName = szArgv[++i];
			const char* localePath = szArgv[++i];

			LocaleService_ForceSetLocale(localeName, localePath);
		}
	}

	//SymInitialize(GetCurrentProcess(), 0, true);
	//SymSetOptions(SYMOPT_LOAD_LINES);

	if (bQuit)
		goto Clean;

#if defined(NEEDED_COMMAND_ARGUMENT)
	// 옵션이 없으면 비정상 실행으로 간주, 프로그램 종료
	if (strstr(lpCmdLine, NEEDED_COMMAND_ARGUMENT) == 0) {
		MessageBox(NULL, ApplicationStringTable_GetStringz(IDS_ERR_MUST_LAUNCH_FROM_PATCHER, "ERR_MUST_LAUNCH_FROM_PATCHER"), ApplicationStringTable_GetStringz(IDS_APP_NAME, "APP_NAME"), MB_ICONSTOP);
		goto Clean;
	}
#endif

#if defined(NEEDED_COMMAND_CLIPBOARD)
	{
		CHAR szSecKey[256];
		CPostIt cPostIt("VOLUME1");

		if (cPostIt.Get("SEC_KEY", szSecKey, sizeof(szSecKey)) == FALSE) {
			MessageBox(NULL, ApplicationStringTable_GetStringz(IDS_ERR_MUST_LAUNCH_FROM_PATCHER, "ERR_MUST_LAUNCH_FROM_PATCHER"), ApplicationStringTable_GetStringz(IDS_APP_NAME, "APP_NAME"), MB_ICONSTOP);
			goto Clean;
		}
		if (strstr(szSecKey, NEEDED_COMMAND_CLIPBOARD) == 0) {
			MessageBox(NULL, ApplicationStringTable_GetStringz(IDS_ERR_MUST_LAUNCH_FROM_PATCHER, "ERR_MUST_LAUNCH_FROM_PATCHER"), ApplicationStringTable_GetStringz(IDS_APP_NAME, "APP_NAME"), MB_ICONSTOP);
			goto Clean;
		}
		cPostIt.Empty();
	}
#endif

	// Start web browser "engine"
#if defined(ENABLE_CEF_BROWSER)
	CefWebBrowser::Startup(hInstance);
#else
	WebBrowser_Startup(hInstance);
#endif

	if (!CheckPythonLibraryFilenames())
	{
		__ErrorPythonLibraryIsNotExist();
		goto Clean;
	}

	// Main routine
	Main(hInstance, lpCmdLine);

	// Clean Up Web Browser
#if defined(ENABLE_CEF_BROWSER)
	CefWebBrowser::Cleanup();
#else
	WebBrowser_Cleanup();
#endif

	// Cleanup COM
	::CoUninitialize();

	//FILE* o = fopen("stack.log", "w");
	//if (o)
	//{
	//	StackTraceLog(o, 1);
	//	fflush(o);
	//	fclose(o);
	//}

	if (gs_szErrorString[0])
		MessageBox(NULL, gs_szErrorString, ApplicationStringTable_GetStringz(IDS_APP_NAME, "APP_NAME"), MB_ICONSTOP);

Clean:
	SAFE_FREE_GLOBAL(szArgv);

	return 0;
}

static void GrannyError(granny_log_message_type Type,
	granny_log_message_origin Origin,
	char const* File,
	granny_int32x Line,
	char const* Error,
	void* UserData)
{
	if (Origin == GrannyFileReadingLogMessage || Origin == GrannyControlLogMessage || Origin == GrannyMeshBindingLogMessage)
		return;

#ifdef _DEBUG
	TraceError("GRANNY: %s(%d): ERROR: %s --- [%d] %s --- [%d] %s", File, Line, Error, Type, GrannyGetLogMessageTypeString(Type), Origin, GrannyGetLogMessageOriginString(Origin));
#endif
}

int Setup(LPSTR lpCmdLine)
{
	/*
	* 타이머 정밀도를 올린다.
	*/
	TIMECAPS tc;
	UINT wTimerRes;

	if (timeGetDevCaps(&tc, sizeof(TIMECAPS)) != TIMERR_NOERROR)
		return 0;

	wTimerRes = MINMAX(tc.wPeriodMin, 1, tc.wPeriodMax);
	timeBeginPeriod(wTimerRes);

	/*
	* 그래니 에러 핸들링
	*/

	granny_log_callback Callback;
	Callback.Function = GrannyError;
	Callback.UserData = 0;
	GrannySetLogCallback(&Callback);
	return 1;
}
