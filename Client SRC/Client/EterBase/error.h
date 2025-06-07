#pragma once

extern void SetEterExceptionHandler();
#if defined(__ENABLE_CRASH_RPT__)
extern void CrashRptInit();
extern void CrashRptUnregister();
#endif
