#ifndef __INC_ETER_BASE_CRC32_H__
#define __INC_ETER_BASE_CRC32_H__

#include <windows.h>
#include "../UserInterface/Locale_inc.h"

DWORD GetCRC32(const char* buffer, size_t count);
DWORD GetCaseCRC32(const char* buf, size_t len);
DWORD GetHFILECRC32(HANDLE hFile);
DWORD GetFileCRC32(const char* c_szFileName);
DWORD GetFileSize(const char* c_szFileName);

#endif // __INC_ETER_BASE_CRC32_H__
