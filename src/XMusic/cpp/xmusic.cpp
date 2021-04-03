
#include "xmusic.h"

#include <QScreen>

#if __windows || __mac
#include <QLockFile>
static QLockFile g_lf(fsutil::getHomeDir() + "/xmusic.lock");
#endif

static wstring m_strWorkDir;
cwstr g_strWorkDir(m_strWorkDir);

static CUTF8TxtWriter m_logger;
ITxtWriter& g_logger(m_logger);

static tagScreenInfo m_screen;
const tagScreenInfo& g_screen(m_screen);

bool g_bFullScreen = false;

#if __android
bool m_bAndroidSDPermission = false;
const bool& g_bAndroidSDPermission(m_bAndroidSDPermission);

bool requestAndroidSDPermission() // 安卓6(API 23)以上动态申请读写权限
{
    m_bAndroidSDPermission = requestAndroidPermission("android.permission.WRITE_EXTERNAL_STORAGE");
    return m_bAndroidSDPermission;
}
#endif

static CSignal<false> m_runSignal(true); //static bool m_bRunSignal = true;
signal_t g_bRunSignal(m_runSignal);

signal_t usleepex(UINT uMs)
{
    if (g_bRunSignal)
    {
        (void)m_runSignal.wait_false(uMs);
    }

    return g_bRunSignal;
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

static const WString g_lpQuality[] {
    L"", L"LQ", L"HQ", L"SQ", L"CD", L"HiRes"
};
const WString& mediaQualityString(E_MediaQuality eQuality)
{
    return g_lpQuality[(UINT)eQuality];
}

static int g_argc = 0;
static char **g_argv = NULL;

CAppBase::CAppBase() : QApplication(g_argc, g_argv)
{
    qRegisterMetaType<fn_void>("fn_void");

    connect(this, &CApp::signal_sync, this, [](fn_void cb){
        //if (!g_bRunSignal) return;
        cb();
    }, Qt::QueuedConnection);

    connect(this, &CApp::signal_syncex, this, [](fn_void cb){
        //if (!g_bRunSignal) return;
        cb();
    }, Qt::BlockingQueuedConnection);
}

void CAppBase::_init()
{
    m_logger.open("xmusic.log", true);

    QScreen *screen = QApplication::primaryScreen();
    cauto sz = screen->size();
    m_screen.nMaxSide = sz.width();
    m_screen.nMinSide = sz.height();
    if (m_screen.nMaxSide < m_screen.nMinSide)
    {
        std::swap(m_screen.nMaxSide, m_screen.nMinSide);
    }

    m_screen.fDPI = screen->logicalDotsPerInch();
    m_screen.pixelRatio = screen->devicePixelRatio();

    g_logger << "screen: " << m_screen.nMaxSide << '*' >> m_screen.nMinSide
             << "DPR: " << m_screen.pixelRatio << ", DPI: " >> m_screen.fDPI;

#if __ios
    m_screen.pixelRatio = MAX(m_screen.pixelRatio, 1);
#endif

    static union {char c[4]; uint32_t l;} endian_test{{'l', '?', '?', 'b'}};
    g_logger << "endian: " >> (char(endian_test.l));
    g_logger << "sizeof(wchar_t): " >> sizeof(wchar_t); //!!安卓/linux四字节

    g_logger << "appDirPath: " >> CApp::applicationDirPath() << "appFilePath: " >> CApp::applicationFilePath();
#if __android
    m_logger << "jniVer: " << g_jniVer << ", androidSdkVer: " >> g_androidSdkVer
             << "SecondaryStorage: " << g_strSecondaryStorage << ", ExternalStorage: " >> g_strExternalStorage
             << "version_sdk: " << g_androidInfo.version_sdk << " version_release: " >> g_androidInfo.version_release
             << "serialno: " << g_androidInfo.serialno << " board_platform: " >> g_androidInfo.board_platform
             << "host: " << g_androidInfo.host << " tags: " >> g_androidInfo.tags
             << "product_brand: " << g_androidInfo.product_brand << " product_model: " >> g_androidInfo.product_model
             << "product_device: " << g_androidInfo.product_device << " product_name: " >> g_androidInfo.product_name
             << "product_board: " << g_androidInfo.product_board << " product_manufacturer: " >> g_androidInfo.product_manufacturer;
#endif
}

int CAppBase::exec() // 派生将显示空白页
{
    //this->thread(
    //std::thread thrStartup(
    auto nRet = mtutil::concurrence([&]{
        int nRet = QApplication::exec();
        m_runSignal.reset(); //m_bRunSignal = false;
        //m_logger << "exec quit: " >> nRet;

        for (auto& thr : m_lstThread)
        {
            thr.cancel(false);
        }
        for (auto& thr : m_lstThread)
        {
            thr.join();
        }

        return nRet;
    }, [=]{
        _init(); //如果同步跑会影响空白页屏幕旋转
        sync([&]{
            CFont::init(this->font());
            this->setFont(CFont());
        });

        if (!_startup()) //派生将显示logo窗口
        {
            sync([&]{
                this->quit();
            });
        }
    });

    /*auto nRet = exec();

    if (thrStartup.joinable())
    {
//#if __android // TODO 规避5.6.1退出的bug
//    thrStartup.detach();
//#else
    thrStartup.join();
//#endif
    }*/

    return nRet;
}

void CAppBase::quit()
{
    for (auto& thr : m_lstThread)
    {
        thr.cancel(false);
    }

    sync([&]{
        m_runSignal.reset(); //m_bRunSignal = false;
        //m_logger >> "quit";
        QApplication::quit();
    });
}

static char g_lpApp[sizeof(CApp)];
CApp& __app(*(CApp*)g_lpApp);

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
    if (requestAndroidSDPermission())
    {
        m_strWorkDir = __sdcardDir __pkgName;
    }
    else
    {
        // 内置包路径不需要权限 data/data/xxx/files、/data/data/xxx/cache分别对应应用详情中的清除数据和清除缓存
        m_strWorkDir = __androidOrgPath; //= __sdcardDir L"Android/data/" __pkgName //居然也对应内置存储同一路径;
    }
#else
    m_strWorkDir = fsutil::getHomeDir().toStdWString() + __wcPathSeparator + __pkgName;
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

    memset(g_lpApp, 0, sizeof(g_lpApp));
    new (g_lpApp) CApp();
    auto nRet = __app.exec();

    m_logger << "exit: " >> nRet;
    m_logger.close();
    //fsutil::copyFile(m_strWorkDir+L"/xmusic.log", __sdcardDir L"xmusic.log");
    return nRet;
}

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
#endif

bool installApp(const CByteBuffer& bbfData)
{
#if __android
    cauto strApkFile = m_strWorkDir + L"/upgrade.apk";
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
    for (cauto pr : zipFile.subDirMap())
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
    for (cauto pr : zipFile.subFileMap())
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
#endif
    return true;
}
