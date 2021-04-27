
#include "xmusic.h"

#include <QScreen>

#include "installApp.hpp"

#include "getcpuid.hpp"

#if __windows || __mac
#include <QLockFile>
static QLockFile g_lf(__WS2Q(fsutil::getHomeDir()) + "/xmusic.lock");
#endif

static CUTF8TxtWriter m_logger;
ITxtWriter& g_logger(m_logger);

static tagScreenInfo m_screen;
const tagScreenInfo& g_screen(m_screen);

#if __android
bool m_bAndroidSDPermission = false;
const bool& g_bAndroidSDPermission(m_bAndroidSDPermission);

bool requestAndroidSDPermission() // 安卓6(API 23)以上动态申请读写权限
{
    m_bAndroidSDPermission = requestAndroidPermission("android.permission.WRITE_EXTERNAL_STORAGE");
    return m_bAndroidSDPermission;
}
#endif

static CSignal<false> m_runSignal(true);
signal_t g_bRunSignal(m_runSignal);

signal_t usleepex(UINT uMs)
{
    if (g_bRunSignal)
    {
        (void)m_runSignal.wait_false(uMs); //注意，此处独占，会阻塞其他线程
    }

    return g_bRunSignal;
}

void async(UINT uDelayTime, cfn_void cb)
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

static const QString g_lpQuality[] {
    "", "LQ", "HQ", "SQ", "CD", "HiRes"
};
cqstr mediaQualityString(IMedia& media)
{
    auto eQuality = media.quality();
    return g_lpQuality[(UINT)eQuality];
}

static int g_argc = 0;
static char **g_argv = NULL;

CAppBase::CAppBase() : QApplication(g_argc, g_argv)
{
    qRegisterMetaType<fn_void>("fn_void");

    onSignal(Qt::QueuedConnection, &CApp::signal_sync, [](fn_void cb){
        //if (!g_bRunSignal) return;
        cb();
    });

    onSignal(Qt::BlockingQueuedConnection, &CApp::signal_syncex, [](fn_void cb){
        //if (!g_bRunSignal) return;
        cb();
    });
}

void CAppBase::init()
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
    int nRet = QApplication::exec();

    for (auto& thr : m_lstThread)
    {
        thr.cancel(false);
    }

    m_runSignal.reset();

    //m_logger << "exec quit: " >> nRet;

    for (auto& thr : m_lstThread)
    {
        thr.join();
    }

    return nRet;
}

void CAppBase::_quit()
{
    m_runSignal.reset();
    QApplication::quit();
}

//static char m_lpApp[sizeof(CApp)];
static void *m_lpApp = malloc(sizeof(CApp));
CApp& g_app = *(CApp*)m_lpApp;

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

#if __windows
    void InitMinDump(const string&);
    InitMinDump("xmusic_dump_");

    //#if (QT_VERSION >= QT_VERSION_CHECK(5,6,0))
    //    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    //#endif
#endif
#endif

    wstring strWorkDir;
#if __android
    if (requestAndroidSDPermission())
    {
        strWorkDir = __sdcardDir __pkgName;
    }
    else
    {
        // 内置包路径不需要权限 data/data/xxx/files、/data/data/xxx/cache分别对应应用详情中的清除数据和清除缓存
        strWorkDir = __androidOrgPath; //= __sdcardDir L"Android/data/" __pkgName //居然也对应内置存储同一路径;
    }
#else
    strWorkDir = fsutil::getHomeDir() + __wcPathSeparator + __pkgName;
#endif
    if (!fsutil::createDir(strWorkDir))
    {
        return -1;
    }
#if __windows
    fsutil::setWorkDir(strutil::toGbk(strWorkDir));
#else
    fsutil::setWorkDir(strutil::toUtf8(strWorkDir));
#endif

    memset(m_lpApp, 0, sizeof(CApp));
    new (m_lpApp) CApp(strWorkDir);
    auto nRet =  g_app.exec();
    if (nRet != 0)
    {
        m_logger << "exit: " >> nRet;
    }

    m_logger.close();

    // 会引起静态对话框析构异常delete (CApp*)m_lpApp;//g_app.~CApp();

#if __android
    exit(0);
#endif

    return nRet;
}
