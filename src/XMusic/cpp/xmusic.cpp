
#include "xmusic.h"

#if __windows || __mac
#include <QLockFile>
static QLockFile g_lf(fsutil::getHomeDir() + "/xmusic.lock");
#endif

static wstring m_strWorkDir;
cwstr g_strWorkDir(m_strWorkDir);

int g_argc = 0;
char **g_argv = NULL;

int main(int argc, char *argv[])
{
    g_argc = argc;
    g_argv = argv;

#if __windows || __mac
    g_lf.setStaleLockTime(1);
    if (!g_lf.tryLock(0))
    {
        return -1;
    }
#endif

#if __windows
    extern void InitMinDump(const string&);
    InitMinDump("xmusic_dump_");

    //#if (QT_VERSION >= QT_VERSION_CHECK(5,6,0))
    //    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    //#endif
#endif

#if __android
    //内置包路径不需要权限 data/data/xxx/files、/data/data/xxx/cache分别对应应用详情中的清除数据和清除缓存
    m_strWorkDir = L"/data/data/" __pkgName;
    // = __sdcardDir L"Android/data/" __pkgName //居然也对应内置存储同一路径;
    //m_strWorkDir = __sdcardDir __pkgName;

#else
    m_strWorkDir = fsutil::getHomeDir().toStdWString() + L"/" __pkgName;
#endif
    if (!fsutil::createDir(m_strWorkDir))
    {
        return -1;
    }

#if __windows
    fsutil::setWorkDir(strutil::toGbk(m_strWorkDir));
#else
    fsutil::setWorkDir(strutil::toUtf8(m_strWorkDir));
#endif

    auto nRet = __app.run(m_strWorkDir);

    //fsutil::copyFile(m_strWorkDir+L"/xmusic.log", __sdcardDir L"xmusic.log");

    return nRet;
}

bool checkIPhoneXBangs(int cx, int cy)
{
    return __ios && ((375 == cx && 812 == cy) || (414 == cx && 896 == cy));
}

static const WString g_lpQuality[] {
    L"", L"LQ", L"HQ", L"SQ", L"CD", L"HiRes"
};
const WString& mediaQualityString(E_MediaQuality eQuality)
{
    return g_lpQuality[(UINT)eQuality];
}

inline void async(UINT uDelayTime, cfn_void cb)
{
    if (!g_bRunSignal)
    {
        return;
    }

    __async(uDelayTime, [&, cb]{
        if (!g_bRunSignal)
        {
            return;
        }

        cb();
    });
}

void async(cfn_void cb)
{
    async(0, cb);
}

#if __windows
static bool _cmdShell(const string& strCmd, bool bBlock = true)
{
    STARTUPINFOA si;
    memset(&si, 0, sizeof(si));
    si.cb = sizeof(si);
    si.hStdInput=GetStdHandle(STD_INPUT_HANDLE);
    si.wShowWindow = SW_HIDE;

    PROCESS_INFORMATION pi;
    memset(&pi, 0, sizeof(pi));
    if(!CreateProcessA(NULL, (char*)strCmd.c_str(), NULL, NULL, FALSE
                      , CREATE_NO_WINDOW, NULL, NULL, &si, &pi))
    {
        return false;
    }

    if (bBlock)
    {
        WaitForSingleObject(pi.hProcess,INFINITE);
    }
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    return true;
}
#endif

bool installApp(const CByteBuffer& bbfData)
{
#if __android
    cauto strApkFile = g_strWorkDir + L"/upgrade.apk";
    if (!OFStream::writefilex(strApkFile, true, bbfData))
    {
        g_logger << "save appPackage fail: " >> strApkFile;
        return false;
    }

    installApk(__WS2Q(strApkFile));

#elif __mac
    cauto strWorkDir = fsutil::workDir();
    cauto strUpgradeFile = strWorkDir + "/upgrade.zip";
    if (!OFStream::writefilex(strUpgradeFile, true, bbfData))
    {
        g_logger << "save appPackage fail: " >> strUpgradeFile;
        return false;
    }

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

#elif __windows
    IFBuffer ifbData(bbfData);
    CZipFile zipFile(ifbData);
    if (!zipFile)
    {
        g_logger >> "invalid appData";
        return false;
    }

    cauto strParentDir = fsutil::GetParentDir(fsutil::getModuleDir()) + __cPathSeparator;

    string strTempDir = strParentDir + "Upgrade";
    cauto strCmd = "cmd /C rd /S /Q \"" + strTempDir + "\"";
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
    for (cauto strSubDir : zipFile.subDirList())
    {
        cauto t_strSubDir = strTempDir + strSubDir;
        if (!fsutil::createDir(t_strSubDir))
        {
            g_logger << "createDir fail: " >> t_strSubDir;
            return false;
        }
    }

#define __StartupFile "XMusicStartup.exe"
    string strStartupFile;
    for (cauto pr : zipFile.subFileMap())
    {
        cauto strSubFile = strTempDir + pr.first;
        if (strutil::endWith(strSubFile, __StartupFile))
        {
            strStartupFile = strSubFile;
        }

        if (zipFile.unzip(pr.second, strSubFile) < 0)
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

    if (!_cmdShell("\"" + strParentDir + __StartupFile "\" -upg", false))
    {
        g_logger >> "shell StartupFile fail";
    }
#endif
    return true;
}
