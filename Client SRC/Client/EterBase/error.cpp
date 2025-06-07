#include "StdAfx.h"
#if defined(__EXTENDED_ERROR_LOG__)
#	include "Debug.h"
#endif

FILE* fException;

#if defined(__EXTENDED_ERROR_LOG__)
static constexpr bool s_bShowSymbolName = true;
static constexpr bool s_bShowLineNumber = true;
#endif

BOOL CALLBACK EnumerateLoadedModulesProc(PCSTR ModuleName, ULONG ModuleBase, ULONG ModuleSize, PVOID UserContext)
{
	DWORD offset = *((DWORD*)UserContext);

	if (offset >= ModuleBase && offset <= ModuleBase + ModuleSize)
	{
		fprintf(fException, "%s", ModuleName);
		return FALSE;
	}
	else
		return TRUE;
}

LONG __stdcall EterExceptionFilter(_EXCEPTION_POINTERS* pExceptionInfo)
{
	HANDLE hProcess = GetCurrentProcess();
	HANDLE hThread = GetCurrentThread();

#if defined(__EXTENDED_ERROR_LOG__)
	std::filesystem::path cur_path = std::filesystem::current_path();

	// Copy crashed syserr to crash folder.
	{
		auto logfile = std::to_string(GetLogFileTimeStamp());

		auto src_path = cur_path.string() + "\\syserr\\" + logfile + ".txt";
		auto copy_path = cur_path.string() + "\\syserr\\crash\\" + logfile + ".txt";

		std::filesystem::copy_file(src_path, copy_path);
	}

	auto error_log_file = cur_path.string() + "\\syserr\\crash\\error_log.txt";
	fException = fopen(error_log_file.c_str(), "wt");
#else
	fException = fopen("ErrorLog.txt", "wt");
#endif
	if (fException)
	{
#if defined(__EXTENDED_ERROR_LOG__)
		SymInitialize(hProcess, NULL, TRUE);
#endif

		HMODULE hModule = GetModuleHandle(NULL);
		TCHAR szModuleName[256];
		GetModuleFileName(hModule, szModuleName, sizeof(szModuleName));
		std::time_t timestamp = static_cast<std::time_t>(GetTimestampForLoadedLibrary(hModule));

		fprintf(fException, "Module Name: %s\n", szModuleName);
		fprintf(fException, "Time Stamp: %s - %s\n", std::to_string(timestamp).c_str(), std::ctime(&timestamp));
		fprintf(fException, "Exception Type: 0x%08x\n", pExceptionInfo->ExceptionRecord->ExceptionCode);
		fprintf(fException, "\n");

		CONTEXT& rContext = *pExceptionInfo->ContextRecord;

		fprintf(fException, "eax: 0x%08x\tebx: 0x%08x\n", rContext.Eax, rContext.Ebx);
		fprintf(fException, "ecx: 0x%08x\tedx: 0x%08x\n", rContext.Ecx, rContext.Edx);
		fprintf(fException, "esi: 0x%08x\tedi: 0x%08x\n", rContext.Esi, rContext.Edi);
		fprintf(fException, "ebp: 0x%08x\tesp: 0x%08x\n", rContext.Ebp, rContext.Esp);
		fprintf(fException, "\n");

		// Initialize stack walking.
		STACKFRAME rStackFrame = { 0 };
		rStackFrame.AddrPC.Offset = rContext.Eip;
		rStackFrame.AddrStack.Offset = rContext.Esp;
		rStackFrame.AddrFrame.Offset = rContext.Ebp;
		rStackFrame.AddrPC.Mode = AddrModeFlat; // EIP
		rStackFrame.AddrStack.Mode = AddrModeFlat; // EBP
		rStackFrame.AddrFrame.Mode = AddrModeFlat; // ESP

#if defined(__EXTENDED_ERROR_LOG__)
		DWORD dwDisplament = 0;
		while (StackWalk(IMAGE_FILE_MACHINE_I386, hProcess, hThread, &rStackFrame, &rContext, NULL, &SymFunctionTableAccess, &SymGetModuleBase, NULL))
		{
			DWORD dwAddress = rStackFrame.AddrPC.Offset;
			if (dwAddress == 0)
				break;

			// Get symbol name
			TCHAR szBuffer[MAX_PATH + sizeof(IMAGEHLP_SYMBOL)] = { 0 };
			PIMAGEHLP_SYMBOL pSymbol = (PIMAGEHLP_SYMBOL)szBuffer;
			pSymbol->SizeOfStruct = sizeof(szBuffer);
			pSymbol->MaxNameLength = sizeof(szBuffer) - sizeof(IMAGEHLP_SYMBOL);

			// Get module name
			IMAGEHLP_MODULE moduleInfo = { 0 };
			moduleInfo.SizeOfStruct = sizeof(IMAGEHLP_MODULE);
			TCHAR* szModule = "";
			if (SymGetModuleInfo(hProcess, dwAddress, &moduleInfo))
			{
				szModule = moduleInfo.ModuleName;
			}

			if (SymGetSymFromAddr(hProcess, rStackFrame.AddrPC.Offset, &dwDisplament, pSymbol))
			{
				// Get file name and line count.
				IMAGEHLP_LINE ImageLine = { 0 };
				ImageLine.SizeOfStruct = sizeof(IMAGEHLP_LINE);

				if (SymGetLineFromAddr(hProcess, rStackFrame.AddrPC.Offset, &dwDisplament, &ImageLine))
				{
					//fprintf(fException, "%08x %s!%s [%s @ %lu]\n", pSymbol->Address, szModule, pSymbol->Name, ImageLine.FileName, ImageLine.LineNumber);
					fprintf(fException, "%08x\t%s!%s @ %lu\n",
						pSymbol->Address,
						szModule,
						s_bShowSymbolName ? pSymbol->Name : "",
						s_bShowLineNumber ? ImageLine.LineNumber : 0
					);
				}
				else
					fprintf(fException, "%08x\t%s!%s\n", pSymbol->Address, szModule, s_bShowSymbolName ? pSymbol->Name : "");
			}
		}
		SymCleanup(hProcess);
#else
		for (int i = 0; i < 512 && rStackFrame.AddrPC.Offset; ++i)
		{
			if (StackWalk(IMAGE_FILE_MACHINE_I386, hProcess, hThread, &rStackFrame, &rContext, NULL, NULL, NULL, NULL) != FALSE)
			{
				fprintf(fException, "0x%08x\t", rStackFrame.AddrPC.Offset);
				EnumerateLoadedModules(hProcess, (PENUMLOADED_MODULES_CALLBACK)EnumerateLoadedModulesProc, &rStackFrame.AddrPC.Offset);
				fprintf(fException, "\n");
			}
			else
			{
				break;
			}
		}
#endif

		fprintf(fException, "\n");
		fflush(fException);

		fclose(fException);
		fException = NULL;
	}

#if defined(__EXTENDED_ERROR_LOG__)
	// Dump DxDiag Information
	STARTUPINFO si = { sizeof(STARTUPINFO) };
	si.cb = sizeof(si);
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_HIDE;
	PROCESS_INFORMATION pi;

	auto crash_path = cur_path.string() + "\\syserr\\crash";
	if (CreateProcess("C:\\Windows\\System32\\dxdiag.exe", "/dontskip /whql:off /t dxdiag_log.txt", NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, crash_path.c_str(), &si, &pi))
	{
		WaitForSingleObject(pi.hProcess, INFINITE);
		CloseHandle(pi.hThread);
		CloseHandle(pi.hProcess);
	}
#endif

	return EXCEPTION_EXECUTE_HANDLER;
}

void SetEterExceptionHandler()
{
	SetUnhandledExceptionFilter(EterExceptionFilter);
}

#if defined(__ENABLE_CRASH_RPT__)
#	include <crashrpt/CrashRpt.h>
#	if defined(_DEBUG)
#		pragma comment(lib, "Debug/CrashRpt.lib")
#		pragma comment(lib, "Debug/CrashRptProbe.lib")
#	else
#		pragma comment(lib, "Release/CrashRpt.lib")
#		pragma comment(lib, "Release/CrashRptProbe.lib")
#	endif
#
// Define the crash callback
int CALLBACK CrashCallback(CR_CRASH_CALLBACK_INFO* pInfo)
{
	PEXCEPTION_POINTERS pExceptionInfo = pInfo->pExceptionInfo->pexcptrs;

	HANDLE hProcess = GetCurrentProcess();
	HANDLE hThread = GetCurrentThread();

#if defined(__EXTENDED_ERROR_LOG__)
	std::filesystem::path cur_path = std::filesystem::current_path();

	// Copy crashed syserr to crash folder.
	{
		auto logfile = std::to_string(GetLogFileTimeStamp());

		auto src_path = cur_path.string() + "\\syserr\\" + logfile + ".txt";
		auto copy_path = cur_path.string() + "\\syserr\\crash\\" + logfile + ".txt";

		std::filesystem::copy_file(src_path, copy_path);
	}

	auto error_log_file = cur_path.string() + "\\syserr\\crash\\error_log.txt";
	fException = fopen(error_log_file.c_str(), "wt");
#else
	fException = fopen("ErrorLog.txt", "wt");
#endif
	if (fException)
	{
#if defined(__EXTENDED_ERROR_LOG__)
		SymInitialize(hProcess, NULL, TRUE);
#endif

		HMODULE hModule = GetModuleHandle(NULL);
		TCHAR szModuleName[256];
		GetModuleFileName(hModule, szModuleName, sizeof(szModuleName));
		std::time_t timestamp = static_cast<std::time_t>(GetTimestampForLoadedLibrary(hModule));

		fprintf(fException, "Module Name: %s\n", szModuleName);
		fprintf(fException, "Time Stamp: %s - %s\n", std::to_string(timestamp).c_str(), std::ctime(&timestamp));
		fprintf(fException, "Exception Type: 0x%08x\n", pExceptionInfo->ExceptionRecord->ExceptionCode);
		fprintf(fException, "\n");

		CONTEXT& rContext = *pExceptionInfo->ContextRecord;

		fprintf(fException, "eax: 0x%08x\tebx: 0x%08x\n", rContext.Eax, rContext.Ebx);
		fprintf(fException, "ecx: 0x%08x\tedx: 0x%08x\n", rContext.Ecx, rContext.Edx);
		fprintf(fException, "esi: 0x%08x\tedi: 0x%08x\n", rContext.Esi, rContext.Edi);
		fprintf(fException, "ebp: 0x%08x\tesp: 0x%08x\n", rContext.Ebp, rContext.Esp);
		fprintf(fException, "\n");

		// Initialize stack walking.
		STACKFRAME rStackFrame = { 0 };
		rStackFrame.AddrPC.Offset = rContext.Eip;
		rStackFrame.AddrStack.Offset = rContext.Esp;
		rStackFrame.AddrFrame.Offset = rContext.Ebp;
		rStackFrame.AddrPC.Mode = AddrModeFlat; // EIP
		rStackFrame.AddrStack.Mode = AddrModeFlat; // EBP
		rStackFrame.AddrFrame.Mode = AddrModeFlat; // ESP

#if defined(__EXTENDED_ERROR_LOG__)
		DWORD dwDisplament = 0;
		while (StackWalk(IMAGE_FILE_MACHINE_I386, hProcess, hThread, &rStackFrame, &rContext, NULL, &SymFunctionTableAccess, &SymGetModuleBase, NULL))
		{
			DWORD dwAddress = rStackFrame.AddrPC.Offset;
			if (dwAddress == 0)
				break;

			// Get symbol name
			TCHAR szBuffer[MAX_PATH + sizeof(IMAGEHLP_SYMBOL)] = { 0 };
			PIMAGEHLP_SYMBOL pSymbol = (PIMAGEHLP_SYMBOL)szBuffer;
			pSymbol->SizeOfStruct = sizeof(szBuffer);
			pSymbol->MaxNameLength = sizeof(szBuffer) - sizeof(IMAGEHLP_SYMBOL);

			// Get module name
			IMAGEHLP_MODULE moduleInfo = { 0 };
			moduleInfo.SizeOfStruct = sizeof(IMAGEHLP_MODULE);
			TCHAR* szModule = "";
			if (SymGetModuleInfo(hProcess, dwAddress, &moduleInfo))
				szModule = moduleInfo.ModuleName;

			if (SymGetSymFromAddr(hProcess, rStackFrame.AddrPC.Offset, &dwDisplament, pSymbol))
			{
				// Get file name and line count.
				IMAGEHLP_LINE ImageLine = { 0 };
				ImageLine.SizeOfStruct = sizeof(IMAGEHLP_LINE);

				if (SymGetLineFromAddr(hProcess, rStackFrame.AddrPC.Offset, &dwDisplament, &ImageLine))
				{
					//fprintf(fException, "%08x %s!%s [%s @ %lu]\n", pSymbol->Address, szModule, pSymbol->Name, ImageLine.FileName, ImageLine.LineNumber);
					fprintf(fException, "%08x\t%s!%s @ %lu\n",
						pSymbol->Address,
						szModule,
						s_bShowSymbolName ? pSymbol->Name : "",
						s_bShowLineNumber ? ImageLine.LineNumber : 0
					);
				}
				else
				{
					fprintf(fException, "%08x\t%s!%s\n", pSymbol->Address, szModule, s_bShowSymbolName ? pSymbol->Name : "");
				}
			}
		}
		SymCleanup(hProcess);
#else
		for (int i = 0; i < 512 && rStackFrame.AddrPC.Offset; ++i)
		{
			if (StackWalk(IMAGE_FILE_MACHINE_I386, hProcess, hThread, &rStackFrame, &rContext, NULL, NULL, NULL, NULL) != FALSE)
			{
				fprintf(fException, "0x%08x\t", rStackFrame.AddrPC.Offset);
				EnumerateLoadedModules(hProcess, (PENUMLOADED_MODULES_CALLBACK)EnumerateLoadedModulesProc, &rStackFrame.AddrPC.Offset);
				fprintf(fException, "\n");
			}
			else
			{
				break;
			}
		}
#endif

		fprintf(fException, "\n");
		fflush(fException);

		fclose(fException);
		fException = NULL;
	}

	// Dump DxDiag Information
	STARTUPINFO si = { sizeof(STARTUPINFO) };
	si.cb = sizeof(si);
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_HIDE;
	PROCESS_INFORMATION pi;

	auto crash_path = cur_path.string() + "\\syserr\\crash";
	if (CreateProcess("C:\\Windows\\System32\\dxdiag.exe", "/dontskip /whql:off /t dxdiag_log.txt", NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, crash_path.c_str(), &si, &pi))
	{
		WaitForSingleObject(pi.hProcess, INFINITE);
		CloseHandle(pi.hThread);
		CloseHandle(pi.hProcess);
	}

	// Proceed with crash report generation. 
	// This return code also makes CrashRpt to not call this callback function for 
	// the next crash report generation stage.
	return CR_CB_DODEFAULT;
}

void CrashRptUnregister()
{
	// Uninitialize CrashRpt before exiting the main function
	crUninstall();
}

void CrashRptInit()
{
	// Define CrashRpt configuration parameters
	CR_INSTALL_INFO info;
	memset(&info, 0, sizeof(CR_INSTALL_INFO));
	info.cb = sizeof(CR_INSTALL_INFO);

	info.pszAppName = "METIN2";

	char szVersion[20]{};
	sprintf(szVersion, "%s", VER_FILE_VERSION_STR);
	info.pszAppVersion = szVersion;

	info.pszEmailSubject = "Game Client Crash"; //!< Subject of crash report e-mail.
	info.pszEmailTo = "support@owsap.dev"; //!< E-mail address of crash reports recipient.
	info.pszUrl = "http://62.171.146.112/crashrpt/crashrpt.php"; //!< URL of server-side script (used in HTTP connection).

	info.uPriorities[CR_HTTP] = 3; // First try send report over HTTP
	info.uPriorities[CR_SMTP] = 2; // Second try send report over SMTP
	info.uPriorities[CR_SMAPI] = 1; // Third try send report over Simple MAPI

	// Install all available exception handlers
	info.dwFlags |= CR_INST_ALL_POSSIBLE_HANDLERS;
	// Restart the app on crash
	info.dwFlags |= CR_INST_APP_RESTART;
	info.dwFlags |= CR_INST_SEND_QUEUED_REPORTS;
	info.pszRestartCmdLine = "/restart";

	// Define the Privacy Policy URL
	//info.pszPrivacyPolicyURL = CRASH_RPT_POLICY_URL;

	// Install crash reporting
	crInstall(&info);

	// Set crash callback function
	crSetCrashCallback(CrashCallback, NULL);

	// Add our log file to the error report
	std::filesystem::path cur_path = std::filesystem::current_path();

#if defined(__EXTENDED_ERROR_LOG__)
	// syserr.txt
	auto syserr_file = cur_path.string() + "\\syserr\\crash\\" + std::to_string(GetLogFileTimeStamp()) + ".txt";
	crAddFile2(syserr_file.c_str(), NULL, "System Error", CR_AF_MISSING_FILE_OK);

	// error_log.txt
	auto error_log_file = cur_path.string() + "\\syserr\\crash\\error_log.txt";
	crAddFile2(error_log_file.c_str(), NULL, "Error Log", CR_AF_MISSING_FILE_OK);

	// dxdiag_log.txt
	auto dxdiag_log_file = cur_path.string() + "\\syserr\\crash\\dxdiag_log.txt";
	crAddFile2(dxdiag_log_file.c_str(), NULL, "DirectX Dialog", CR_AF_MISSING_FILE_OK);
#else
	auto syserr_file = cur_path.string() + "\\syserr.txt";
	crAddFile2(syserr_file.c_str(), NULL, "System Error", CR_AF_MISSING_FILE_OK);
#endif

	// We want the screenshot of the entire desktop to be added on crash.
	crAddScreenshot2(CR_AS_VIRTUAL_SCREEN, CR_AV_QUALITY_LOW);
}
#endif
