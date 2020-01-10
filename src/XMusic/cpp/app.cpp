
#include "app.h"

#include "mainwindow.h"

#include <QFontDatabase>

#include <QScreen>

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

static void installApk(const QString &qsApkPath)
{
    QAndroidJniObject jFilePath = QAndroidJniObject::fromString(qsApkPath);
    QAndroidJniObject activity = QtAndroid::androidActivity();
    QAndroidJniObject::callStaticMethod<void>(
                "xmusic/XActivity",
                "installApk",
                "(Ljava/lang/String;Lorg/qtproject/qt5/android/bindings/QtActivity;)V",
                jFilePath.object<jstring>(),
                activity.object<jobject>()
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

#define __sdcardDir "/sdcard/"
// "/storage/emulated/0/"
#define __androidDataDir __sdcardDir "Android/data/com.musicrossoft.xmusic"
#endif

float g_fPixelRatio = 1;

static CUTF8TxtWriter m_logger;
ITxtWriter& g_logger(m_logger);

CAppInit::CAppInit(QApplication& app)
{
#if __windows
    fsutil::setWorkDir(fsutil::getModuleDir());
#elif __android
    (void)fsutil::createDir(__androidDataDir);
    fsutil::setWorkDir(__androidDataDir);
#else
    fsutil::setWorkDir(app.applicationDirPath().toStdWString());
#endif

    m_logger.open(L"XMusic.log", true);

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
#endif

#if __android
    g_uDefFontSize = 12;

#elif __ios
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
#endif

    SMap<int, QString> mapFontFile {
        { QFont::Weight::Light, "/font/Microsoft-YaHei-Light.ttc" }
        //, { QFont::Weight::Normal, "/font/Microsoft-YaHei-Regular.ttc" }
        , { QFont::Weight::DemiBold, "/font/Microsoft-YaHei-Semibold.ttc" }
    };
    mapFontFile([&](int nWeight, QString qsFontFile) {
#if __android
        qsFontFile = "assets:" +  qsFontFile;
#else
        qsFontFile = strutil::toQstr(fsutil::workDir()) + qsFontFile;
#endif

        int fontId = QFontDatabase::addApplicationFont(qsFontFile);
        g_logger << "newFontId: " >> fontId;
        if (-1 != fontId)
        {
            cauto qslst = QFontDatabase::applicationFontFamilies(fontId);
            if (!qslst.empty())
            {
                cauto qsFamilyName = qslst.at(0);
                g_logger << "newfamilyName: " >> qsFamilyName;

                g_lstFontFamily.emplace_back(nWeight, qsFamilyName);
            }
        }
    });

    QString qsFamilyName;
    if (g_lstFontFamily.empty())
    {
#if __windows
        qsFamilyName = "微软雅黑";
#else
        qsFamilyName = app.font().family();
#endif

        g_lstFontFamily.emplace_back(g_nDefFontWeight, qsFamilyName);
    }
    else
    {
        cauto pr = g_lstFontFamily.front();
        g_nDefFontWeight = pr.first;
        qsFamilyName = pr.second;
    }

    app.setFont(CFont());
}

bool CApp::m_bRunSignal = true;

void CApp::async(UINT uDelayTime, cfn_void cb)
{
    __async(uDelayTime, [&, cb](){
        if (!m_bRunSignal)
        {
            return;
        }

        cb();
    });
}


bool CApp::_resetRootDir(wstring& strRootDir)
{
    strRootDir = fsutil::getHomeDir() + L"/XMusic";

#if __android
    strRootDir = strutil::toWstr(__sdcardDir) + L"XMusic";

//#elif __ios && TARGET_IPHONE_SIMULATOR
//    strRootDir = L"/Users/lhyuan/XMusic";

#elif __window
    if (!XMediaLib::m_bOnlineMediaLib)
    {
        if (strRootDir.empty() || !fsutil::existDir(strRootDir))
        {
            CFolderDlg FolderDlg;
            strRootDir = FolderDlg.Show(m_mainWnd.hwnd(), NULL, L" 设定媒体库路径", L"请选择媒体库目录");
            if (strRootDir.empty())
            {
                return false;
            }
        }

        return true;
    }
#endif

    return fsutil::createDir(strRootDir + __medialibPath);
}

void CApp::slot_run(bool bUpgradeFail, int nUpgradeErrMsg)
{
    if (!m_bRunSignal)
    {
        return;
    }

    if (bUpgradeFail)
    {
        auto eUpgradeErrMsg = (E_UpgradeErrMsg)nUpgradeErrMsg;
        if (E_UpgradeErrMsg::UEM_AppUpgraded == eUpgradeErrMsg)
        {
            this->quit();
        }
        else
        {
            QString qsErrMsg;
            if (E_UpgradeErrMsg::UEM_AppUpgradeFail == eUpgradeErrMsg)
            {
                qsErrMsg = "更新App失败";
            }
            else
            {
                qsErrMsg = "更新媒体库失败";
            }
            m_msgbox.show(qsErrMsg, [&](){
                this->quit();
            });
        }

        return;
    }

    if (!m_model.initMediaLib())
    {
        g_logger >> "initMediaLib fail";
        this->quit();
        return;
    }

    if (!m_ctrl.start())
    {
        g_logger >> "start controller fail";
        this->quit();
        return;
    }
    g_logger >> "start controller success, app running";

    if (m_ctrl.getOption().crTheme >= 0)
    {
        g_crTheme.setRgb((int)m_ctrl.getOption().crTheme);
    }
    if (m_ctrl.getOption().crText >= 0)
    {
        g_crText.setRgb((int)m_ctrl.getOption().crText);
    }

    m_mainWnd.show();
}

int CApp::run()
{
    auto& option = m_ctrl.initOption();

    std::thread thrUpgrade;

    CApp::async(100, [&](){
        if (!_resetRootDir(option.strRootDir))
        {
            this->quit();
            return;
        }
        g_logger << "RootDir: " >> option.strRootDir;

        connect(this, &CApp::signal_run, this, &CApp::slot_run);

        thrUpgrade = std::thread([&]() {
            auto timeBegin = time(NULL);

            bool bUpgradeFail = false;
            E_UpgradeErrMsg eUpgradeErrMsg = E_UpgradeErrMsg::UEM_None;
            if (XMediaLib::m_bOnlineMediaLib)
            {
                if (!_upgradeMediaLib(eUpgradeErrMsg))
                {
                    bUpgradeFail = true;
                }
            }

            auto timeWait = 6 - (time(NULL) - timeBegin);
            if (timeWait > 0)
            {
                mtutil::usleep((UINT)timeWait*1000);
            }

            emit signal_run(bUpgradeFail, (int)eUpgradeErrMsg);
        });
    });

    m_mainWnd.showLogo();

    int nRet = exec();
    m_bRunSignal = false;

    if (thrUpgrade.joinable())
    {
        thrUpgrade.join();
    }

    g_logger >> "stop controller";
    m_ctrl.stop();

    CPlayer::QuitSDK();

    g_logger >> "quit";
    m_logger.close();

    return nRet;
}

bool CApp::_readMedialibConf(Instream& ins, tagMedialibConf& medialibConf)
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

    string strBkg;
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
    }

    const JValue& jVBkg = jRoot["vbkg"];
    if (jVBkg.isArray())
    {
        for (UINT uIdx = 0; uIdx < jVBkg.size(); uIdx++)
        {
            if (jsonutil::get(jVBkg[uIdx], strBkg))
            {
                medialibConf.lstOnlineVBkg.push_back(strBkg);
            }
        }
    }

    return true;
}

bool CApp::_upgradeMediaLib(E_UpgradeErrMsg& eUpgradeErrMsg)
{
    QFile qf(":/medialib.conf");
    if (!qf.open(QFile::OpenModeFlag::ReadOnly))
    {
        g_logger >> "loadMedialibConfResource fail";
        return false;
    }

    cauto ba = qf.readAll();
    IFBuffer ifbMedialibConf((c_byte_p)ba.data(), ba.size());
    tagMedialibConf orgMedialibConf;
    __EnsureReturn(_readMedialibConf(ifbMedialibConf, orgMedialibConf), false);
    g_logger << "appVersion: " >> orgMedialibConf.strAppVersion
             << "MediaLib orgVersion: " >> orgMedialibConf.uMedialibVersion;

    tagMedialibConf *pPrevMedialibConf = &orgMedialibConf;

    tagMedialibConf userMedialibConf;
    IFStream ifsMedialibConf(m_model.medialibPath(L"medialib.conf"));
    if (ifsMedialibConf)
    {
         if (_readMedialibConf(ifsMedialibConf, userMedialibConf))
         {
             if (userMedialibConf.uCompatibleCode == orgMedialibConf.uCompatibleCode)
             {
                 g_logger << "MediaLib userVersion: " >> userMedialibConf.uMedialibVersion;

                 if (userMedialibConf.uMedialibVersion >= orgMedialibConf.uMedialibVersion)
                 {
                     pPrevMedialibConf = &userMedialibConf;
                 }
             }
         }

         ifsMedialibConf.close();
    }

    string strVerInfo;
    int nRet = curlutil::initCurl(strVerInfo);
    if (nRet != 0)
    {
        g_logger << "initCurl fail: " >> nRet;
        return false;
    }
    g_logger << "CurlVerInfo: \n" >> strVerInfo;

    return _upgradeMedialib(*pPrevMedialibConf, eUpgradeErrMsg);
}

bool CApp::_upgradeMedialib(tagMedialibConf& prevMedialibConf, E_UpgradeErrMsg& eUpgradeErrMsg)
{
    for (cauto upgradeUrl : prevMedialibConf.lstUpgradeUrl)
    {
        cauto strMedialibUrl = upgradeUrl.medialibUrl();
        g_logger << "dowloadMediaLib: " >> strMedialibUrl;

        CByteBuffer bbfZip;

        CDownloader downloader(false, 10);
        int nRet = downloader.syncDownload(strMedialibUrl, bbfZip, 1, [&](int64_t dltotal, int64_t dlnow){
                (void)dltotal;
                (void)dlnow;
                return m_bRunSignal;
        });
        if (nRet != 0)
        {
            g_logger << "download fail: " >> nRet;
            continue;
        }

        IFBuffer ifbZip(bbfZip);
        string strPwd;
        //strPwd.append("medialib").append(".zip");
        CZipFile zipFile(ifbZip, strPwd);
        if (!zipFile)
        {
            g_logger >> "invalid zipfile";
            continue;
        }

        auto mapUnzfile = zipFile.unzfileMap();
        auto itrMedialibConf = mapUnzfile.find("medialib.conf");
        if (itrMedialibConf == mapUnzfile.end())
        {
            g_logger >> "medialibConf not found";
            continue;
        }

        CByteBuffer bbfMedialibConf;
        if (zipFile.read(itrMedialibConf->second, bbfMedialibConf) <= 0)
        {
            g_logger >> "readZip fail: medialibConf";
            continue;
        }

        mapUnzfile.erase(itrMedialibConf);

        IFBuffer ifbMedialibConf(bbfMedialibConf);
        auto& newMedialibConf = m_model.getMediaLib().medialibConf();
        newMedialibConf.clear();
        if (!_readMedialibConf(ifbMedialibConf, newMedialibConf))
        {
            g_logger >> "readMedialibConf fail";
            continue;
        }

        if (newMedialibConf.uMedialibVersion < prevMedialibConf.uMedialibVersion)
        {
            g_logger << "medialib version invalid: " >> newMedialibConf.uMedialibVersion;
            continue;
        }

        bool bUncompatible = false;
        if (newMedialibConf.uCompatibleCode > prevMedialibConf.uCompatibleCode)
        {
            bUncompatible = true;
            g_logger << "medialib not compatible: " >> newMedialibConf.uCompatibleCode;
        }

        if (newMedialibConf.strAppVersion != prevMedialibConf.strAppVersion)
        {
            g_logger << "upgradeApp: " >> newMedialibConf.strAppVersion;

            if (bUncompatible)
            {
                if (!_upgradeApp(prevMedialibConf.strAppVersion, newMedialibConf))
                {
                    eUpgradeErrMsg = E_UpgradeErrMsg::UEM_AppUpgradeFail;
                }
                else
                {
                    eUpgradeErrMsg = E_UpgradeErrMsg::UEM_AppUpgraded;
                }
            }
        }

        if (bUncompatible)
        {
            return false;
        }

        auto itrMedialib = mapUnzfile.find("medialib");
        if (itrMedialib == mapUnzfile.end())
        {
            g_logger >> "medialib not found";
            continue;
        }

        cauto strDBFile = m_model.medialibPath(__medialibFile);
        if (newMedialibConf.uMedialibVersion > prevMedialibConf.uMedialibVersion || !fsutil::existFile(strDBFile))
        {
            CByteBuffer bbfMedialib;
            if (zipFile.read(itrMedialib->second, bbfMedialib) <= 0)
            {
                g_logger >> "readZip fail: medialib";
                continue;
            }

            OFStream ofsMedialib(strDBFile, true);
            if (!ofsMedialib || ofsMedialib.writex(bbfMedialib) != bbfMedialib->size())
            {
                g_logger >> "write medialib fail";
                return false;
            }
        }

        mapUnzfile.erase(itrMedialib);

        for (cauto pr : mapUnzfile)
        {
            const tagUnzfile& unzfile = pr.second;
            CByteBuffer bbfData;
            if (zipFile.read(unzfile, bbfData) <= 0)
            {
                g_logger << "readSnapshot fail: " >> unzfile.strPath;
                return false;
            }
            IFBuffer ifbData(bbfData);

            if (strutil::endWith(unzfile.strPath, ".share"))
            {
                if (!m_model.getMediaLib().loadShareUrl(ifbData))
                {
                    g_logger << "loadShareUrl fail: " >> unzfile.strPath;
                    //continue;
                }
            }
            else if (strutil::endWith(unzfile.strPath, ".xurl"))
            {
                if (!m_model.getMediaLib().loadXurl(ifbData))
                {
                    g_logger << "loadXurl fail: " >> unzfile.strPath;
                    //continue;
                }
            }
            else if (strutil::endWith(unzfile.strPath, ".snapshot.json"))
            {
                if (!m_model.getMediaLib().loadSnapshot(ifbData))
                {
                    g_logger << "loadSnapshot fail: " >> unzfile.strPath;
                    //continue;
                }
            }
        }

        OFStream ofbMedialibConf(m_model.medialibPath(L"medialib.conf"), true);
        if (!ofbMedialibConf || ofbMedialibConf.writex(bbfMedialibConf) != bbfMedialibConf->size())
        {
            g_logger >> "write medialibConf fail";
            //return false;
        }

        return true;
    }

    return false;
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

bool CApp::_upgradeApp(const string& strPrevVersion, const tagMedialibConf& newMedialibConf)
{
#if __ios
    return true;
#endif

    for (cauto upgradeUrl : newMedialibConf.lstUpgradeUrl)
    {
        cauto strAppUrl = upgradeUrl.appUrl();
        if (strAppUrl.empty())
        {
            continue;
        }
        g_logger << "dowloadApp: " >> strAppUrl;

        CByteBuffer bbfData;
        CDownloader downloader(false);
        int nRet = downloader.syncDownload(strAppUrl, bbfData, 0, [&](int64_t dltotal, int64_t dlnow){
                (void)dltotal;
                (void)dlnow;

                if (!m_bRunSignal)
                {
                    return false;
                }

                //emit sgnal_appUpgradeProgress(dltotal, dlnow);

                return true;
        });
        if (nRet != 0)
        {
            g_logger << "download fail: " >> nRet;
            continue;
        }

#if __android
        cauto strApkFile = fsutil::workDir() + "/upgrade.apk";
        OFStream ofs(strApkFile, true);
        if (!ofs || ofs.writex(bbfData) != bbfData->size())
        {
            g_logger << "saveApk fail:" >> strApkFile;
            return false;
        }

        installApk(strutil::toQstr(strApkFile));

#else
        IFBuffer ifbData(bbfData);
        CZipFile zipFile(ifbData);
        if (!zipFile)
        {
            g_logger >> "invalid zipfile";
            continue;
        }

#if __windows
        cauto strParentDir = fsutil::GetParentDir(fsutil::getModuleDir()) + "\\";

        string strTempDir = strParentDir + "upgrade";
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

        strTempDir.append("\\");
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

#elif __mac

#endif
#endif
        return true;
    }

    return false;
}
