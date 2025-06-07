#ifndef __INC_SERVICE_DEFS_H__
#define __INC_SERVICE_DEFS_H__

//#define __IMPROVED_PACKET_ENCRYPTION__ // Improved Packet Encryption
//#define __SEND_SEQUENCE__ // Sequence Matching
#define __DOUBLE_RECV_BUFFER__ // Increase Recvieve Buffer Size
#define __VERTEX_BUFFER_PERFORMANCE__ // TextTail Vertex Buffer Performance
#define __ANI_IMAGE_PERFORMANCE__ // Image Animation Performance
#define __STATE_MANAGER_DEVICE_FIX__ // State Manager (Lost Device) Fix
#define __EXTENDED_ERROR_LOG__ // Extended Error Logs

// #if !defined(_DEBUG)
// #	define __ENABLE_CRASH_RPT__ // CrashRPT (Send Crash Reports)
// #endif

//#define __MOVIE_MODE__ // Movie Mode
#define __HIT_RANGE_RENEWAL__ // Hit Range Renewal
#define __RENDER_TARGET_VIEW_PORT_FIX__ // Render Target ViewPort Fix
#define __DISABLE_BOSS_PUSHBACK__ // Disable Boss Pushback
#define __FRAME_UPDATE_OPTIMIZATION__ // Frame Update Optimization
#define __PY_SECURE_EXEC_MEM_FILE__ // Securely execute a Python file.
#define __FONT_TEXTURE_FIX__ // Adds padding to prevent random dots in font textures.

#if !defined(_CYTHON)
//#	define __IMGUI__ // Immediate Mode GUI
#endif

enum EEterPackKeySet
{
	ETERPACK_NONE,
	ETERPACK_KEY,
	ETERPACK_SECURITY_KEY,
	ETERPACK_ITEM_PROTOCOL,
	ETERPACK_MOB_PROTOCOL,

	COMPRESS_BRICK_SIZE = 4,
	COMPRESS_BIRCK_LEN = 10,

	COMPRESS_KEY_SIZE = COMPRESS_BIRCK_LEN + COMPRESS_BRICK_SIZE + 2,
};

static const char* s_strEIX = ".eix"; // EterPack Index
static const char* s_strEPK = ".epk"; // EterPack

static std::string s_strEterPackSecurityKey[COMPRESS_BRICK_SIZE] = {
	std::string(AY_OBFUSCATE("599220564")),
	std::string(AY_OBFUSCATE("715591183")),
	std::string(AY_OBFUSCATE("1382647704")),
	std::string(AY_OBFUSCATE("1053692977"))
};

static std::string s_strEterPackKey[COMPRESS_BRICK_SIZE] = {
	std::string(AY_OBFUSCATE("843707904")),
	std::string(AY_OBFUSCATE("1176167288")),
	std::string(AY_OBFUSCATE("992328100")),
	std::string(AY_OBFUSCATE("1705708525"))
};

static std::string s_strItemProtoKey[COMPRESS_BRICK_SIZE] = {
	std::string(AY_OBFUSCATE("173217")),
	std::string(AY_OBFUSCATE("72619434")),
	std::string(AY_OBFUSCATE("408587239")),
	std::string(AY_OBFUSCATE("27973291"))
};

static std::string s_strMobProtoKey[COMPRESS_BRICK_SIZE] = {
	std::string(AY_OBFUSCATE("4813894")),
	std::string(AY_OBFUSCATE("18955")),
	std::string(AY_OBFUSCATE("552631")),
	std::string(AY_OBFUSCATE("6822045"))
};

#endif // __INC_SERVICE_DEFS_H__
