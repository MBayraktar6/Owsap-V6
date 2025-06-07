#include "stdafx.h"
#include "start_position.h"

char g_nation_name[4][32] =
{
	"",
#if defined(__LOCALE_CLIENT__)
	"[LS;793]",
	"[LS;794]",
	"[LS;795]",
#else
	"신수국", // Shinsoo
	"천조국", // Chunjo
	"진노국", // Jinno
#endif
};

// LC_STRING("신수국")
// LC_STRING("천조국")
// LC_STRING("진노국")

long g_start_map[4] =
{
	0, // reserved
	1, // 신수국
	21, // 천조국
	41 // 진노국
};

DWORD g_start_position[4][2] =
{
	{ 0, 0 }, // reserved
	{ 475000, 966100 }, // 신수국
	{ 60000, 156000 }, // 천조국
	{ 963400, 278200 } // 진노국
};

DWORD arena_return_position[4][2] =
{
	{ 0, 0 },
	{ 347600, 882700 }, // 자양현
	{ 138600, 236600 }, // 복정현
	{ 857200, 251800 } // 박라현
};

DWORD g_create_position[4][2] =
{
	{ 0, 0 }, // reserved,
	{ 475000, 966100 }, // 신수국
	{ 60000, 156000 }, // 천조국
	{ 963400, 278200 } // 진노국
};

DWORD g_wolfman_create_position[4][2] =
{
	{ 0, 0 }, // reserved,
	{ 808500, 936000 }, // 신수국
	{ 859700, 936000 }, // 천조국
	{ 910900, 936000 }, // 진노국
};
