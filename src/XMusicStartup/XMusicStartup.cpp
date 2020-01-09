// XMusicStartup.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "XMusicStartup.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    char pszPath[MAX_PATH+64];
    memset(pszPath, 0, sizeof(pszPath));
    ::GetModuleFileNameA(::GetModuleHandleA(pszModuleName), pszPath, sizeof(pszPath));
	for (int nIdx = sizeof(pszPath)-1; nIdx >= 0; nIdx--)
	{
		auto& chr = pszPath[nIdx];
		if ('\\' == chr)
		{
			chr = '\0';
			break;
		}			
	}
	
	for (auto p = lpCmdLine; *p; p++)
	{
		if (strcmp(p-4, "-upg"))
		{
			return 0;
		}
	}
	
	WinExec("./bin/XMusic.exe", SW_SHOW);

    return 0;
}
