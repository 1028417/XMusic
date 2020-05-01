
#include "app.h"

#include "mainwindow.h"

#include <QFontDatabase>

#include <QScreen>

#define __pkgName L"com.musicrossoft.xmusic"

static CUTF8TxtWriter m_logger;
ITxtWriter& g_logger(m_logger);

float g_fPixelRatio = 1;

static bool g_bRunSignal = true;

#if __android
#include <QAndroidJniObject>
#include <QAndroidJniEnvironment>
#include <QtAndroid>

//#include <QtAndroidExtras>
//#include <jni.h>

static int g_jniVer = 0;
JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved)
{
    (void)reserved;

    JNIEnv* env = nullptr;
    if (vm->GetEnv((void**) &env, JNI_VERSION_1_6) == JNI_OK)
    {
        return g_jniVer=JNI_VERSION_1_6;
    }
    else if (vm->GetEnv((void**) &env, JNI_VERSION_1_4) == JNI_OK)
    {
        return g_jniVer=JNI_VERSION_1_4;
    }
    else if (vm->GetEnv((void**) &env, JNI_VERSION_1_2) == JNI_OK)
    {
        return g_jniVer=JNI_VERSION_1_2;
    }
    else if (vm->GetEnv((void**) &env, JNI_VERSION_1_1) == JNI_OK)
    {
        return g_jniVer=JNI_VERSION_1_1;
    }

    return JNI_ERR;
}

void CApp::vibrate(UINT duration)
{
    QAndroidJniEnvironment env;

    QAndroidJniObject name = QAndroidJniObject::getStaticObjectField(
                "android/content/Context",
                "VIBRATOR_SERVICE",
                "Ljava/lang/String;"
                );

    QAndroidJniObject vibrateService = QtAndroid::androidActivity().callObjectMethod(
                "getSystemService",
                "(Ljava/lang/String;)Ljava/lang/Object;",
                name.object<jstring>());

    vibrateService.callMethod<void>("vibrate", "(J)V", jlong(duration));
}

static bool installApk(const QString &qsApkPath)
{
    QAndroidJniObject jFilePath = QAndroidJniObject::fromString(qsApkPath);
    QAndroidJniObject::callStaticMethod<void>(
                "xmusic/XActivity",
                "installApk",
                "(Ljava/lang/String;Lorg/qtproject/qt5/android/bindings/QtActivity;)V",
                jFilePath.object<jstring>(),
                QtAndroid::androidActivity().object<jobject>()
                );
}

static bool isMobileConnected()
{
    return QAndroidJniObject::callStaticMethod<bool>(
                "xmusic/XActivity",
                "isMobileConnected",
                "(Ljava/lang/String;)V",
                QtAndroid::androidActivity().object<jobject>()
                );
}

/*string g_strSDCardPath;
static const wstring& sdcardPath()
{
    if (!g_strSDCardPath.empty())
    {
        return g_strSDCardPath;
    }

    char *pszSdcardPath = getenv("SECONDARY_STORAGE");
    if (NULL == pszSdcardPath)
    {
        pszSdcardPath = getenv("EXTERNAL_STORAGE");
    }
    if (pszSdcardPath)
    {
        return g_strSDCardPath = strutil::fromStr(pszSdcardPath);
    }

    g_strSDCardPath = L"/mnt/sdcard";
    if (!fsutil::existDir(g_strSDCardPath))
    {
        g_strSDCardPath = L"/storage/emulated/0";
        if (!fsutil::existDir(g_strSDCardPath))
        {
            g_strSDCardPath = L"/sdcard";
        }
    }

    return g_strSDCardPath;
}*/

#define __sdcardDir L"/sdcard/" // "/storage/emulated/0/"

#elif __windows
void CApp::setForeground()
{
    auto hwnd = m_mainWnd.hwnd();
    if (IsIconic(hwnd))
    {
        ::ShowWindow(hwnd, SW_RESTORE);
    }
    else
    {
        //::SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE);
        //::SetWindowPos(hwnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE);
        ::SetForegroundWindow(hwnd);
    }
}
#endif

CAppInit::CAppInit(QApplication& app)
{
#if __android
    wstring strWorkDir = __sdcardDir __pkgName;
#else
    wstring strWorkDir = (fsutil::getHomeDir() + __WS2Q(L"/" __pkgName)).toStdWString();
#endif
    (void)fsutil::createDir(strWorkDir);

#if __windows
    fsutil::setWorkDir(strutil::toGbk(strWorkDir));
#else
    fsutil::setWorkDir(strutil::toUtf8(strWorkDir));
#endif

    m_logger.open("xmusic.log", true);

    g_logger << "applicationDirPath: " >> QApplication::applicationDirPath();
    g_logger << "applicationFilePath: " >> QApplication::applicationFilePath();

#if __android
    g_logger << "jniVer: " >> g_jniVer;
#endif

    QScreen *screen = QApplication::primaryScreen();
    float fPixelRatio = screen->devicePixelRatio();
    auto fDPI = screen->logicalDotsPerInch();
    g_logger << "DPR: " << fPixelRatio << " DPI: " >> fDPI;

#if __ios
    g_fPixelRatio = fPixelRatio;

    /*int nScreenSize = szScreen.width()*szScreen.height();
    switch (nScreenSize)
    {
    case 320*568: // iPhoneSE
        break;
    case 375*667: // iPhone6 iPhone6S iPhone7 iPhone8
        break;
    case 414*736: // iPhone6plus iPhone6Splus iPhone7plus iPhone8plus
        break;
    case 375*812: // iPhoneX iPhoneXS ??
        break;
    case 414*896: // iPhoneXR iPhoneXSmax ??
        break;
    case 1024*1366: // iPadPro1_12.9 iPadPro2_12.9 iPadPro3_12.9
        break;
    case 834*1194: // iPadPro1_11
        break;
    case 834*1112: // iPadPro1_10.5
        break;
    case 768*1024: // iPadPro1_9.7 iPadAir2 iPadAir iPad6 iPad5
        break;
    default:        // iPadMini
        break;
    };*/

    QSize szScreen = screen->size();
    int nScreenWidth = MIN(szScreen.width(), szScreen.height()) ;
    g_uDefFontSize = app.font().pointSize();
    g_uDefFontSize *= nScreenWidth/540.0f;

#elif __mac
    g_uDefFontSize = 29;

#elif __windows
    g_uDefFontSize = 22;

    float fDPIRate = getDPIRate();
    g_logger << "DPIRate: " >> fDPIRate;
    g_uDefFontSize *= fDPIRate;

#elif __android
    g_uDefFontSize = 12;
#endif

    g_mapFontFamily[g_nDefFontWeight] = app.font().family();

#if __windows
    //g_mapFontFamily[QFont::Weight::Light] = "微软雅黑 Light";
    g_mapFontFamily[QFont::Weight::DemiBold] = "微软雅黑";
#endif

    list<pair<int, QString>> plFontFile {
        {QFont::Weight::Light, "/font/msyhl-6.23.ttc"}
        //, {QFont::Weight::Semilight, "/font/Microsoft-YaHei-Semilight-11.0.ttc"}
        , {QFont::Weight::DemiBold, "/font/Microsoft-YaHei-Semibold-11.0.ttc"}
    };
    for (auto& pr : plFontFile)
    {
        auto& qsFontFile = pr.second;
#if __android
        qsFontFile = "assets:" +  qsFontFile;
#else
        qsFontFile = app.applicationDirPath() + qsFontFile;
#endif

        int fontId = QFontDatabase::addApplicationFont(qsFontFile);
        if (-1 == fontId)
        {
            g_logger << "addFont fail: " >> qsFontFile;
            continue;
        }

        cauto qslst = QFontDatabase::applicationFontFamilies(fontId);
        if (!qslst.empty())
        {
            g_logger << "newfamilyName: " >> qslst.front();
            g_mapFontFamily[pr.first] = qslst.front();
        }
    }

    app.setFont(CFont());
}

static bool _readMedialibConf(Instream& ins, tagMedialibConf& medialibConf)
{
    JValue jRoot;
    if (!jsonutil::loadFile(ins, jRoot))
    {
        g_logger >> "loadMedialibConf fail";
        return false;
    }

    if (!jsonutil::get(jRoot["appVersion"], medialibConf.strAppVersion))
    {
        g_logger >> "readMedialibConf fail: appVersion";
        return false;
    }

    if (!jsonutil::get(jRoot["medialibVersion"], medialibConf.uMedialibVersion))
    {
        g_logger >> "readMedialibConf fail: medialibVersion";
        return false;
    }

    if (!jsonutil::get(jRoot["compatibleCode"], medialibConf.uCompatibleCode))
    {
        g_logger >> "readMedialibConf fail: appVersion";
        return false;
    }

    const JValue& jMedialibUrl = jRoot["medialibUrl"];
    if (!jMedialibUrl.isArray())
    {
        g_logger >> "readMedialibConf fail: url";
        return false;
    }

    string strMedialib;
    for (UINT uIdx = 0; uIdx < jMedialibUrl.size(); uIdx++)
    {
        if (!jsonutil::get(jMedialibUrl[uIdx], strMedialib))
        {
            g_logger >> "readMedialibConf fail: url";
            return false;
        }

        medialibConf.lstUpgradeUrl.emplace_back(strMedialib);
    }

    /*string strBkg;
    const JValue& jHBkg = jRoot["hbkg"];
    if (jHBkg.isArray())
    {
        for (UINT uIdx = 0; uIdx < jHBkg.size(); uIdx++)
        {
            if (jsonutil::get(jHBkg[uIdx], strBkg))
            {
                medialibConf.lstOnlineHBkg.push_back(strBkg);
            }
        }
    }*/

    return true;
}

int CApp::run()
{
    std::thread thrUpgrade;

    CApp::async([&](){
        connect(this, &CApp::signal_run, this, &CApp::slot_run);

        thrUpgrade = std::thread([&]() {
            auto& option = m_ctrl.initOption();
            if (!_initRootDir(option.strRootDir))
            {
                this->quit();
                return;
            }
            g_logger << "RootDir: " >> option.strRootDir;

            QFile qf(":/medialib.conf");
            if (!qf.open(QFile::OpenModeFlag::ReadOnly))
            {
                g_logger >> "loadMedialibConfResource fail";
                this->quit();
                return;
            }

            cauto ba = qf.readAll();
            IFBuffer ifbMedialibConf((cbyte_p)ba.data(), ba.size());
            tagMedialibConf orgMedialibConf;
            if (!_readMedialibConf(ifbMedialibConf, orgMedialibConf))
            {
                g_logger >> "readMedialibConfResource fail";

                this->quit();
                return;
            }
            g_logger << "orgMedialibConf AppVersion: " << orgMedialibConf.strAppVersion
                     << " CompatibleCode: " << orgMedialibConf.uCompatibleCode
                     << " MedialibVersion: " >> orgMedialibConf.uMedialibVersion;
            m_strAppVersion = strutil::fromAsc(orgMedialibConf.strAppVersion);

            E_UpgradeResult eUpgradeResult = mtutil::concurrence([&](){
                return _initMediaLib(orgMedialibConf);
            }, [&](){
                mtutil::usleep(100);

                (void)m_pmHDDisk.load(__mediaPng(hddisk));
                (void)m_pmLLDisk.load(__mediaPng(lldisk));

                m_mainWnd.preinit();
            });

            emit signal_run((int)eUpgradeResult);
        });
    });

    m_mainWnd.showLogo();
    int nRet = exec();
    g_bRunSignal = false;

    g_logger >> "stop controller";
    m_ctrl.stop();

#if !__android // TODO 规避5.6.1退出的bug
    if (thrUpgrade.joinable())
    {
        thrUpgrade.join();
    }
#endif

    g_logger >> "app quit";
    m_logger.close();

    return nRet;
}

E_UpgradeResult CApp::_initMediaLib(const tagMedialibConf& orgMedialibConf)
{
    auto timeBegin = time(0);

#if __OnlineMediaLib
    E_UpgradeResult eUpgradeResult = this->_upgradeMedialib(orgMedialibConf);
    if (E_UpgradeResult::UR_Success != eUpgradeResult)
    {
        return eUpgradeResult;
    }
#endif

    //auto time0 = time(0);
    if (!m_model.initMediaLib())
    {
        g_logger >> "initMediaLib fail";
        return E_UpgradeResult::UR_Fail;
    }
    //g_logger << "initMediaLib success " >> (time(0)-time0);

    auto timeDiff = 6 - (time(0) - timeBegin);
    //g_logger << "timeDiff: " >> timeDiff;
    if (timeDiff > 0)
    {
        mtutil::usleep((UINT)timeDiff*1000);
    }

    return E_UpgradeResult::UR_Success;
}

bool CApp::_initRootDir(wstring& strRootDir)
{
#if __OnlineMediaLib
    strRootDir = QDir::currentPath().toStdWString();
    return fsutil::createDir(m_model.medialibPath());
#endif

#if __window
    if (strRootDir.empty() || !fsutil::existDir(strRootDir))
    {
        CFolderDlg FolderDlg;
        strRootDir = FolderDlg.Show(m_mainWnd.hwnd(), NULL, L" 设定媒体库路径", L"请选择媒体库目录");
        if (strRootDir.empty())
        {
            return false;
        }
    }

#elif __android
    strRootDir = __sdcardDir L"XMusic";

#else
    strRootDir = (fsutil::getHomeDir() + __WS2Q(L"/XMusic")).toStdWString();

//#if __ios && TARGET_IPHONE_SIMULATOR
//    strRootDir = L"/Users/lhyuan/XMusic";
//#endif
#endif

    return true;
}

static void _importPlayingList(const wstring& strDBFileBak, const wstring& strDBFile)
{
    CSQLiteDB db;
    if (!db.Connect(strDBFile))
    {
        return;
    }

    if (!db.Execute(L"attach \"" + strDBFileBak + L"\" as bak"))
    {
        return;
    }

    (void)db.Execute("INSERT INTO tbl_playitem SELECT * from bak.tbl_playitem where playlist_id = 0");
}

E_UpgradeResult CApp::_upgradeMedialib(const tagMedialibConf& orgMedialibConf)
{
    string strVerInfo;
    int nRet = curlutil::initCurl(strVerInfo);
    if (nRet != 0)
    {
        g_logger << "initCurl fail: " >> nRet;
        return E_UpgradeResult::UR_Fail;
    }
    g_logger << "CurlVerInfo: \n" >> strVerInfo;

    cauto strConffile = m_model.medialibPath(L"medialib.conf");
    tagMedialibConf userMedialibConf;
    IFStream ifsMedialibConf(strConffile);
    if (ifsMedialibConf)
    {
         if (_readMedialibConf(ifsMedialibConf, userMedialibConf))
         {
             g_logger << "userMedialibConf AppVersion: " << userMedialibConf.strAppVersion
                      << " CompatibleCode: " << userMedialibConf.uCompatibleCode
                      << " MedialibVersion: " >> userMedialibConf.uMedialibVersion;
         }

         ifsMedialibConf.close();
    }

    E_UpgradeResult eRet = E_UpgradeResult::UR_Fail;

    const list<CUpgradeUrl>& lstUpgradeUrl = (userMedialibConf.strAppVersion >= orgMedialibConf.strAppVersion ||
            userMedialibConf.uCompatibleCode >= orgMedialibConf.uCompatibleCode)
            ? userMedialibConf.lstUpgradeUrl : orgMedialibConf.lstUpgradeUrl;
    for (cauto upgradeUrl : lstUpgradeUrl)
    {
        cauto strMdlconfUrl = upgradeUrl.mdlconf();
        g_logger << "checkMediaLib: " >> strMdlconfUrl;

        CByteBuffer bbfConf;
        CDownloader downloader(3, 6);
        int nRet = downloader.syncDownload(g_bRunSignal, strMdlconfUrl, bbfConf, 1);
        if (nRet != 0)
        {
            g_logger << "checkMediaLib fail: " >> nRet;
            if (E_UpgradeResult::UR_Fail == eRet)
            {
                eRet = E_UpgradeResult::UR_DownloadFail;
            }
            continue;
        }

        IFBuffer ifbConf(bbfConf);
        auto& newMedialibConf = __xmedialib.medialibConf();
        newMedialibConf.clear();
        if (!_readMedialibConf(ifbConf, newMedialibConf))
        {
            g_logger >> "readMedialibConf fail";
            eRet = E_UpgradeResult::UR_MedialibInvalid;
            continue;
        }

        g_logger << "newMedialibConf AppVersion: " << userMedialibConf.strAppVersion
                 << " CompatibleCode: " << userMedialibConf.uCompatibleCode
                 << " MedialibVersion: " >> userMedialibConf.uMedialibVersion;

        if (newMedialibConf.strAppVersion < MAX(orgMedialibConf.strAppVersion, userMedialibConf.strAppVersion))
        {
            g_logger << "AppVersion invalid: " >> newMedialibConf.strAppVersion;
            eRet = E_UpgradeResult::UR_MedialibUncompatible;
            continue;
        }

        if (newMedialibConf.uCompatibleCode < MAX(orgMedialibConf.uCompatibleCode, userMedialibConf.uCompatibleCode))
        {
            g_logger << "CompatibleCode invalid: " >> newMedialibConf.uCompatibleCode;
            eRet = E_UpgradeResult::UR_MedialibUncompatible;
            continue;
        }

        if (newMedialibConf.uMedialibVersion < MAX(orgMedialibConf.uMedialibVersion, userMedialibConf.uMedialibVersion))
        {
            g_logger << "MedialibVersion invalid: " >> newMedialibConf.uMedialibVersion;
            eRet = E_UpgradeResult::UR_MedialibUncompatible;
            continue;
        }

        if (newMedialibConf.uCompatibleCode > orgMedialibConf.uCompatibleCode)
        {
            g_logger >> "medialib Uncompatible";

#if !__ios
            if (newMedialibConf.strAppVersion > orgMedialibConf.strAppVersion)
            {
                if (_upgradeApp(newMedialibConf.lstUpgradeUrl))
                {
                    return E_UpgradeResult::UR_AppUpgraded;
                }
                else
                {
                    return E_UpgradeResult::UR_AppUpgradeFail;
                }
            }
#endif
            return E_UpgradeResult::UR_MedialibUncompatible;
        }

        cauto strMdlUrl = upgradeUrl.mdl();
        g_logger << "dowloadMdl: " >> strMdlUrl;

        cauto strMdlFile = m_model.medialibPath(L"mdl");
        bool bExistMdl = fsutil::existFile(strMdlFile);
        if (!bExistMdl || newMedialibConf.uMedialibVersion > userMedialibConf.uMedialibVersion)
        {
            auto time0 = time(0);

            CByteBuffer bbfMdl;
            CDownloader downloader(3, 30, 1, 3);
            int nRet = downloader.syncDownload(g_bRunSignal, strMdlUrl, bbfMdl, 1);
            if (nRet != 0)
            {
                g_logger << "download fail: " >> nRet;
                return E_UpgradeResult::UR_DownloadFail;
            }

            IFBuffer ifbMdl(bbfMdl);
            CZipFile zipMdl(ifbMdl);
            auto eRet = _loadMdl(zipMdl, true);
            if (E_UpgradeResult::UR_Success != eRet)
            {
                return eRet;
            }

            if (!OFStream::writefilex(strMdlFile, true, bbfMdl))
            {
                g_logger >> "write mdl fail";
                //return E_UpgradeResult::UR_Fail;
            }

            g_logger << "upgradeMedialib success " >> (time(0)-time0);
        }
        else
        {
            IFStream ifsMdl(strMdlFile);
            CZipFile zipMdl(ifsMdl);
            eRet = _loadMdl(zipMdl, false);
            if (E_UpgradeResult::UR_Success != eRet)
            {
                return eRet;
            }
        }

        if (!OFStream::writefilex(strConffile, true, bbfConf))
        {
            g_logger >> "write medialibConf fail";
            //return E_UpgradeResult::UR_Fail;
        }

        return E_UpgradeResult::UR_Success;
    }

    return eRet;
}

E_UpgradeResult CApp::_loadMdl(CZipFile& zipMdl, bool bUpgradeDB)
{
    if (!zipMdl)
    {
        g_logger >> "invalid zipfile";
        return E_UpgradeResult::UR_MedialibInvalid;
    }

    auto mapUnzfile = zipMdl.unzfileMap();
    cauto itrUnzfile = mapUnzfile.find("mdl");
    if (itrUnzfile == mapUnzfile.end())
    {
        g_logger >> "medialib not found";
        return E_UpgradeResult::UR_MedialibInvalid;
    }

    cauto strDBFile = m_model.medialibPath(__DBFile);
    bool bExistDB = fsutil::existFile(strDBFile);
    if (!bExistDB || bUpgradeDB)
    {
        wstring strDBFileBak;
        if (bExistDB)
        {
            strDBFileBak = strDBFile+L".bak";
            (void)fsutil::moveFile(strDBFile, strDBFileBak);
        }

        CByteBuffer bbfMedialib;
        if (zipMdl.read(itrUnzfile->second, bbfMedialib) <= 0)
        {
            g_logger >> "readZip fail: medialib";
            return E_UpgradeResult::UR_ReadMedialibFail;
        }

        if (!OFStream::writefilex(strDBFile, true, bbfMedialib))
        {
            g_logger >> "write medialib fail";
            return E_UpgradeResult::UR_Fail;
        }

        if (bExistDB)
        {
            _importPlayingList(strDBFileBak, strDBFile);
            (void)fsutil::removeFile(strDBFileBak);
        }
    }

    mapUnzfile.erase(itrUnzfile);

    for (cauto pr : mapUnzfile)
    {
        const tagUnzfile& unzfile = pr.second;
        CByteBuffer bbfData;
        if (zipMdl.read(unzfile, bbfData) <= 0)
        {
            g_logger << "readZip fail: " >> unzfile.strPath;
            continue; // E_UpgradeResult::UR_ReadMedialibFail;
        }
        IFBuffer ifbData(bbfData);

        cauto strPath = strutil::fromGbk(unzfile.strPath);
        if (strutil::endWith(strPath, L".xurl"))
        {
            if (!__xmedialib.loadXUrl(ifbData))
            {
                g_logger << "loadXUrl fail: " >> strPath;
            }
        }
        else if (strutil::endWith(strPath, L".snapshot.json"))
        {
            if (!__xmedialib.loadXSnapshot(ifbData))
            {
                g_logger << "loadSnapshot fail: " >> strPath;
            }
        }
        else if (strutil::endWith(strPath, L".cue"))
        {
            if (!__xmedialib.loadXCue(ifbData, fsutil::getFileTitle(strPath)))
            {
                g_logger << "loadCue fail: " >> strPath;
            }
        }
    }

    return E_UpgradeResult::UR_Success;
}

#if __windows
static bool cmdShell(const string& strCmd, bool bBlock = true)
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

int g_nAppDownloadProgress = -1;

bool CApp::_upgradeApp(const list<CUpgradeUrl>& lstUpgradeUrl)
{
    for (cauto upgradeUrl : lstUpgradeUrl)
    {
        if (!g_bRunSignal)
        {
             return false;
        }

        cauto strAppUrl = upgradeUrl.app();
        if (strAppUrl.empty())
        {
            continue;
        }
        g_logger << "dowloadApp: " >> strAppUrl;

        g_nAppDownloadProgress = 0;

        CByteBuffer bbfData;
        CDownloader downloader;
        int nRet = downloader.syncDownload(strAppUrl, bbfData, 0, [&](time_t beginTime, int64_t dltotal, int64_t dlnow){
                (void)beginTime;

                if (dltotal > 10000)
                {
                    g_nAppDownloadProgress = 100*dlnow/dltotal;
                }
                /*else if (dltotal > 0)
                {
                    g_logger << "invalid appBundleSize: " >> dltotal;
                    return false;
                }*/

                return g_bRunSignal;
        });
        if (nRet != 0)
        {
            g_logger << "download fail: " >> nRet;
            continue;
        }

#if __android
        cauto strApkFile = fsutil::workDir() + "/upgrade.apk";
        if (!OFStream::writefilex(strApkFile, true, bbfData))
        {
            g_logger << "save appPackage fail: " >> strApkFile;
            return false;
        }

        installApk(__WS2Q(strutil::fromAsc(strApkFile)));

#elif __mac
        cauto strUpgradeFile = fsutil::workDir() + "/upgrade.zip";
        if (!OFStream::writefilex(strUpgradeFile, true, bbfData))
        {
            g_logger << "save appPackage fail: " >> strUpgradeFile;
            return false;
        }

        cauto strUpgradeDir = fsutil::workDir() + "/XMusic.app";

#define system(x) system((x).c_str())
        (void)system("rm -rf " + strUpgradeDir);

        nRet = system("tar -xf " + strUpgradeFile);
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
            g_logger >> "invalid zipfile";
            continue;
        }

        cauto strParentDir = fsutil::GetParentDir(fsutil::getModuleDir()) + __cPathSeparator;

        string strTempDir = strParentDir + "Upgrade";
        cauto strCmd = "cmd /C rd /S /Q \"" + strTempDir + "\"";
        if (!cmdShell(strCmd))
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
        for (cauto unzdir : zipFile.unzdirList())
        {
            cauto strSubDir = strTempDir + unzdir.strPath;
            if (!fsutil::createDir(strSubDir))
            {
                g_logger << "createDir fail: " >> strSubDir;
                return false;
            }
        }

#define __StartupFile "XMusicStartup.exe"
        string strStartupFile;
        for (cauto pr : zipFile.unzfileMap())
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

        if (!cmdShell("\"" + strParentDir + __StartupFile "\" -upg", false))
        {
            g_logger >> "shell StartupFile fail";
        }
#endif
        return true;
    }

    return false;
}

void CApp::slot_run(int nUpgradeResult)
{
    if (!g_bRunSignal)
    {
        return;
    }

    auto eUpgradeResult = (E_UpgradeResult)nUpgradeResult;
    if (E_UpgradeResult::UR_Success != eUpgradeResult)
    {
        if (E_UpgradeResult::UR_AppUpgraded == eUpgradeResult)
        {
            this->quit();
        }
        else
        {
            QString qsErrMsg;
            if (E_UpgradeResult::UR_DownloadFail == eUpgradeResult)
            {
                qsErrMsg = "更新媒体库失败:  网络异常";
            }
            else if (E_UpgradeResult::UR_MedialibInvalid == eUpgradeResult)
            {
                qsErrMsg = "更新媒体库失败:  媒体库异常";
            }
            else if (E_UpgradeResult::UR_MedialibUncompatible == eUpgradeResult)
            {
                qsErrMsg = "更新媒体库失败:  媒体库不兼容";
//#if __ios
//#endif
            }
            else if (E_UpgradeResult::UR_AppUpgradeFail == eUpgradeResult)
            {
                qsErrMsg = "更新App失败";
            }
            else
            {
                qsErrMsg = "更新媒体库失败";
            }

#if __windows
            setForeground();
#elif __android
            vibrate();
#endif
            m_msgbox.show(qsErrMsg, [&](){
                CApp::async([&](){
                    this->quit();
                });
            });
        }

        return;
    }

    if (m_ctrl.getOption().crBkg >= 0)
    {
        g_crBkg.setRgb((int)m_ctrl.getOption().crBkg);
    }
    if (m_ctrl.getOption().crFore >= 0)
    {
        g_crFore.setRgb((int)m_ctrl.getOption().crFore);
    }

    m_mainWnd.show();

    g_logger >> "start controller";
    m_ctrl.start();
    g_logger >> "app running";
}

void CApp::quit()
{
    g_bRunSignal = false;

/*#if __android
    m_mainWnd.setVisible(false);

    async(50, [](){
        QApplication::quit();
    });

    return;
#endif*/

    QApplication::quit();
}

void CApp::async(UINT uDelayTime, cfn_void cb)
{
    __async(uDelayTime, [&, cb](){
        if (!g_bRunSignal)
        {
            return;
        }

        cb();
    });
}

void CApp::asyncloop(UINT uDelayTime, UINT uTimes, cfn_void cb)
{
    async(uDelayTime, [=](){
        cb();

        if (uTimes > 1)
        {
            asyncloop(uDelayTime, uTimes-1, cb);
        }
    });
}

void CApp::asyncloop(UINT uDelayTime, cfn_bool cb)
{
    __async(uDelayTime, [=](){
        if (!g_bRunSignal)
        {
            return;
        }

        if (!cb())
        {
            return;
        }

        asyncloop(uDelayTime, cb);
    });
}

static const WString g_lpQuality[] {
    L"", L"LQ", L"HQ", L"SQ", L"CD", L"HiRes"
};

const WString& mediaQualityString(E_MediaQuality eQuality)
{
    return g_lpQuality[(UINT)eQuality];
}
