
#include "util/util.h"

#include <Dbghelp.h>
#if __winvc
#pragma comment(lib, "Dbghelp.lib")
#endif

static wstring g_strDumpFileName;

static inline void CreateMiniDump(PEXCEPTION_POINTERS pep, const wstring& strFileName)
{
	HANDLE hFile = CreateFileW(strFileName.c_str(), GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	if ((hFile != NULL) && (hFile != INVALID_HANDLE_VALUE))
	{
		MINIDUMP_EXCEPTION_INFORMATION mdei;
		mdei.ThreadId = GetCurrentThreadId();
		mdei.ExceptionPointers = pep;
		mdei.ClientPointers = FALSE;

		MINIDUMP_TYPE dumpType = (MINIDUMP_TYPE)(MiniDumpNormal  //
			| MiniDumpWithFullMemory     // full debug info
			| MiniDumpWithHandleData
			| MiniDumpWithUnloadedModules
			//| MiniDumpWithIndirectlyReferencedMemory
			//| MiniDumpScanMemory
			| MiniDumpWithProcessThreadData
			| MiniDumpWithThreadInfo);

		//MINIDUMP_CALLBACK_INFORMATION mci;
		//mci.CallbackRoutine     = (MINIDUMP_CALLBACK_ROUTINE)MiniDumpCallback;
		//mci.CallbackParam       = 0;

		//::MiniDumpWriteDump(::GetCurrentProcess(), ::GetCurrentProcessId(), hFile, MiniDumpNormal, (pep != 0) ? &mdei : 0, NULL, &mci);
		::MiniDumpWriteDump(::GetCurrentProcess(), ::GetCurrentProcessId(), hFile, dumpType, (pep != 0) ? &mdei : 0, NULL, NULL);

		CloseHandle(hFile);
	}
}

static LONG MyUnhandledExceptionFilter(PEXCEPTION_POINTERS pExceptionInfo)
{
	EXCEPTION_RECORD& exceptionRecord = *pExceptionInfo->ExceptionRecord;
	char pszExceptionInfo[128];
	memset(pszExceptionInfo, 0, sizeof(pszExceptionInfo));
	sprintf_s(pszExceptionInfo, "ExceptionCode=%u, ExceptionFlags=%u, ExceptionAddress=%d, NumberParameters=%u"
		, exceptionRecord.ExceptionCode, exceptionRecord.ExceptionFlags, (int)exceptionRecord.ExceptionAddress, exceptionRecord.NumberParameters);
	
    wstring strDumpFile = fsutil::workDir() + L'\\' + g_strDumpFileName + tmutil::formatTime64(L"%Y%m%d_%H%M%S") + L".dmp";
	CreateMiniDump(pExceptionInfo, strDumpFile);
	
    //exit(0);

	return EXCEPTION_EXECUTE_HANDLER;
}

// 此函数一旦成功调用，之后对 SetUnhandledExceptionFilter 的调用将无效
static void DisableSetUnhandledExceptionFilter()
{
	void* addr = (void*)GetProcAddress(LoadLibraryA("kernel32.dll"), "SetUnhandledExceptionFilter");

	if (addr)
	{
		unsigned char code[16];
		int size = 0;

		code[size++] = 0x33;
		code[size++] = 0xC0;
		code[size++] = 0xC2;
		code[size++] = 0x04;
		code[size++] = 0x00;

		DWORD dwOldFlag, dwTempFlag;
		VirtualProtect(addr, size, PAGE_EXECUTE_READWRITE, &dwOldFlag);

		WriteProcessMemory(GetCurrentProcess(), addr, code, size, NULL);

		VirtualProtect(addr, size, dwOldFlag, &dwTempFlag);
	}
}

void InitMinDump(const wstring& strDumpFileName)
{
    g_strDumpFileName = strDumpFileName;

	//注册异常处理函数
	SetUnhandledExceptionFilter((LPTOP_LEVEL_EXCEPTION_FILTER)MyUnhandledExceptionFilter);

	//使SetUnhandledExceptionFilter
	DisableSetUnhandledExceptionFilter();
}


/*void CreateDumpFile(LPCSTR lpstrDumpFilePathName, EXCEPTION_POINTERS *pException)
{
	// 创建Dump文件
	HANDLE hDumpFile = CreateFileA(lpstrDumpFilePathName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	// Dump信息
	MINIDUMP_EXCEPTION_INFORMATION dumpInfo;
	dumpInfo.ExceptionPointers = pException;
	dumpInfo.ThreadId = GetCurrentThreadId();
	dumpInfo.ClientPointers = TRUE;

	// 写入Dump文件内容
	MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hDumpFile, MiniDumpNormal, &dumpInfo, NULL, NULL);

	CloseHandle(hDumpFile);
}

LPTOP_LEVEL_EXCEPTION_FILTER WINAPI MyDummySetUnhandledExceptionFilter(LPTOP_LEVEL_EXCEPTION_FILTER)
{
	return NULL;
}

BOOL PreventSetUnhandledExceptionFilter()
{
	HMODULE hKernel32 = LoadLibraryA("kernel32.dll");
	if (hKernel32 == NULL)
		return FALSE;

    auto pOrgEntry = GetProcAddress(hKernel32, "SetUnhandledExceptionFilter");
	if (pOrgEntry == NULL)
		return FALSE;

	unsigned char newJump[100];
	DWORD dwOrgEntryAddr = (DWORD)pOrgEntry;
	dwOrgEntryAddr += 5; // add 5 for 5 op-codes for jmp far

    auto pNewFunc = &MyDummySetUnhandledExceptionFilter;
	DWORD dwNewEntryAddr = (DWORD)pNewFunc;
	DWORD dwRelativeAddr = dwNewEntryAddr - dwOrgEntryAddr;

	newJump[0] = 0xE9;  // JMP absolute
	memcpy(&newJump[1], &dwRelativeAddr, sizeof(pNewFunc));
    SIZE_T bytesWritten = 0;
    BOOL bRet = WriteProcessMemory(GetCurrentProcess(), (LPVOID)pOrgEntry, newJump, sizeof(pNewFunc) + 1, &bytesWritten);
	return bRet;
}

LONG WINAPI UnhandledExceptionFilterEx(struct _EXCEPTION_POINTERS *pException)
{
	char szMbsFile[MAX_PATH] = { 0 };
	::GetModuleFileNameA(NULL, szMbsFile, MAX_PATH);
	char* pFind = strrchr(szMbsFile, '\\');
	if (pFind)
	{
		*(pFind + 1) = 0;
		strcat_s(szMbsFile, "CrashDumpFile.dmp");
		CreateDumpFile(szMbsFile, pException);
	}

	FatalAppExitA(-1, "Fatal Error");
	return EXCEPTION_CONTINUE_SEARCH;
}

void RunCrashHandler()
{
	SetUnhandledExceptionFilter(UnhandledExceptionFilterEx);
	PreventSetUnhandledExceptionFilter();
}*/
