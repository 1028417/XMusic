// XMusicStartup.cpp : ����Ӧ�ó������ڵ㡣
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

	WinExec("./bin/XMusic.exe", SW_SHOW);

    return 0;
}