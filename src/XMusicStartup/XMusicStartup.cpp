// XMusicStartup.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "XMusicStartup.h"

#include <string>
using std::string;
using std::wstring;

static bool cmdShell(const string& strCmd, bool bBlock = true)
{
	STARTUPINFOA si;
	memset(&si, 0, sizeof(si));
	si.cb = sizeof(si);
	si.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
	si.wShowWindow = SW_HIDE;

	PROCESS_INFORMATION pi;
	memset(&pi, 0, sizeof(pi));
	if (!CreateProcessA(NULL, (char*)strCmd.c_str(), NULL, NULL, FALSE
		, CREATE_NO_WINDOW, NULL, NULL, &si, &pi))
	{
		return false;
	}

	if (bBlock)
	{
		WaitForSingleObject(pi.hProcess, INFINITE);
	}
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);

	return true;
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    char pszPath[MAX_PATH+64];
    memset(pszPath, 0, sizeof(pszPath));
    ::GetModuleFileNameA(::GetModuleHandleA(pszPath), pszPath, sizeof(pszPath));
	for (int nIdx = sizeof(pszPath)-1; nIdx >= 0; nIdx--)
	{
		auto& chr = pszPath[nIdx];
		if ('\\' == chr)
		{
			chr = '\0';
			break;
		}
	}
	string strDir(pszPath);
	string strBinDir = strDir + "\\bin\\";

	wstring strUpgradeFlag = L"-upg";
	wstring strCmdLine(lpCmdLine);
	if (strCmdLine.size() >= strUpgradeFlag.size())
	{
		if (strCmdLine.substr(strCmdLine.size() - strUpgradeFlag.size()) == strUpgradeFlag)
		{
			Sleep(1000);

			string strCmd = "cmd /C rd /S /Q \"" + strBinDir + "\"";
			if (!cmdShell(strCmd))
			{
				MessageBoxA(NULL, "清除原目录失败！", NULL, 0);
				return -1;
			}
			//cmdShell("cmd /C mkdir \"" + strBinDir + "\"");

			string strUpgradeDir = strDir + "\\upgrade";
			string strUpgradeBinDir = strUpgradeDir + "\\XMusic-win32\\bin";

			strCmd = "cmd /C move /Y \"" + strUpgradeBinDir + "\" \"" + strDir + "\"";
			//strCmd = "cmd /C xcopy /E /R /y \"" + strUpgradeBinDir + "\" \"" + strBinDir + "\\\"";
			if (!cmdShell(strCmd))
			{
				MessageBoxA(NULL, "拷贝新目录失败！", NULL, 0);
				return -1;
			}

			Sleep(3000);

			strCmd = "cmd /C rd /S /Q \"" + strUpgradeDir + "\"";
			(void)cmdShell(strCmd);
		}
	}

	if (!cmdShell(strBinDir + "XMusic.exe", false))
	{
		MessageBoxA(NULL, "启动主程序失败！", NULL, 0);
		return -1;
	}

    return 0;
}
