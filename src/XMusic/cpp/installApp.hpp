
#if __windows
static bool _cmdShell(cwstr strCmd, bool bBlock = true)
{
    STARTUPINFOW si;
    memset(&si, 0, sizeof(si));
    si.cb = sizeof(si);
    si.hStdInput=GetStdHandle(STD_INPUT_HANDLE);

    /*if (nCmdSow)
    {
        si.wShowWindow = nCmdSow;
        si.dwFlags = STARTF_USESHOWWINDOW;
    }*/

    PROCESS_INFORMATION pi;
    memset(&pi, 0, sizeof(pi));
    if(!CreateProcessW(NULL, (wchar_t*)strCmd.c_str(), NULL, NULL, FALSE
                      , CREATE_NO_WINDOW, NULL, NULL, &si, &pi))
    {
        return false;
    }

    if (bBlock)
    {
        WaitForSingleObject(pi.hProcess,INFINITE);
        //GetExitCodeProcess
    }
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    return true;
}

static bool _installWinZip(CZipFile& zipFile)
{
    if (!zipFile)
    {
        g_logger >> "invalid appData";
        return false;
    }

    cauto strParentDir = fsutil::GetParentDir(fsutil::getModuleDir((const wchar_t *)NULL)) + __wcPathSeparator;

    auto strTempDir = strParentDir + L"Upgrade";
    cauto strCmd = L"cmd /C rd /S /Q \"" + strTempDir + L"\"";
    if (!_cmdShell(strCmd))
    {
        g_logger << "cmdShell fail: " >> strCmd;
        return false;
    }

    if (!fsutil::createDir(strTempDir))
    {
        g_logger << "createDir fail: " >> strTempDir;
        return false;
    }

    strTempDir.push_back(__cPathSeparator);
    for (cauto pr : zipFile.subDirList())
    {
        cauto t_strSubDir = strTempDir + pr.first;
        if (!fsutil::createDir(t_strSubDir))
        {
            g_logger << "createDir fail: " >> t_strSubDir;
            return false;
        }
    }

#define __StartupFile L"XMusicStartup.exe"
    wstring strStartupFile;
    for (cauto pr : zipFile.subFileList())
    {
        cauto strSubFile = strTempDir + pr.first;
        if (strutil::endWith(strSubFile, __StartupFile))
        {
            strStartupFile = strSubFile;
        }

        if (zipFile.unzip(*pr.second, strSubFile) < 0)
        {
            g_logger << "unzip fail: " >> strSubFile;
            return false;
        }
    }

    if (strStartupFile.empty())
    {
        g_logger >> "StartupFile not found";
        return false;
    }

    if (!fsutil::copyFile(strStartupFile, strParentDir + __StartupFile))
    {
        g_logger >> "copy StartupFile fail";
        return false;
    }

    if (!_cmdShell(L"\"" + strParentDir + __StartupFile L"\" -upg", false))
    {
        g_logger >> "shell StartupFile fail";
    }

    return true;
}

#elif __mac
static bool _installMacApp(const string& strUpgradeFile)
{
    cauto strUpgradeDir = strWorkDir + "/XMusic.app";
#define system(x) system((x).c_str())
    (void)system("rm -rf " + strUpgradeDir);

    auto nRet = system("tar -xf " + strUpgradeFile);
    if (nRet)
    {
        g_logger << "unpack app fail: " >> nRet;
        return false;
    }
    (void)system("rm -f " + strUpgradeFile);

    auto strAppDir = QApplication::applicationDirPath().toStdString();
    strAppDir = fsutil::GetParentDir(strAppDir);
    strAppDir = fsutil::GetParentDir(strAppDir);
    strutil::replace(strAppDir, " ", "\\ ");

    cauto strBakDir = strAppDir + ".bak";
    (void)system("rm -rf " + strBakDir);

    nRet = system("mv " + strAppDir + " " + strBakDir);
    if (nRet)
    {
        g_logger << "backupApp fail: " >> nRet;
        return false;
    }

    nRet = system("mv " + strUpgradeDir + " " + strAppDir);
    if (nRet)
    {
        g_logger << "upgradeApp fail: " >> nRet;
        return false;
    }

    (void)system("rm -rf " + strBakDir);

    g_logger << "appUpgrade success, restart: " >> strAppDir;
    g_lf.unlock();

    nRet = system("open -n " + strAppDir);
    if (nRet)
    {
        g_logger << "restartApp fail: " >> nRet;
    }

    return true;
}
#endif

bool installApp(const string& strUpgradeFile)
{
#if __android
    installApk(__WS2Q(strutil::fromAsc(strUpgradeFile)));
    return true;

#elif __mac
    return _installMacApp(strUpgradeFile);

#elif __windows
    CZipFile zipFile(strUpgradeFile);
    return _installWinZip(zipFile);
#endif

    return false;
}

bool installApp(const CByteBuffer& bbfUpgradeFile)
{
#if __android
    cauto strApkFile = g_app.workDir() + L"/upgrade.apk";
    if (!OFStream::writefilex(strApkFile, true, bbfUpgradeFile))
    {
        g_logger << "save appPackage fail: " >> strApkFile;
        return false;
    }

    installApk(__WS2Q(strApkFile));
    return true;

#elif __mac
    cauto strWorkDir = fsutil::workDir();
    cauto strUpgradeFile = strWorkDir + "/upgrade.zip";
    if (!OFStream::writefilex(strUpgradeFile, true, bbfData))
    {
        g_logger << "save appPackage fail: " >> strUpgradeFile;
        return false;
    }

    return _installMacApp(strUpgradeFile);

#elif __windows
    IFBuffer ifUpgradeFile(bbfUpgradeFile);
    CZipFile zipFile(ifUpgradeFile);
    return _installWinZip(zipFile);
#endif

    return false;
}
