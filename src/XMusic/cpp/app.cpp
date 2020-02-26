
#include "app.h"

#include "mainwindow.h"

#include <QFontDatabase>

#include <QScreen>

#define __pkgName "com.musicrossoft.xmusic"

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
#endif

float g_fPixelRatio = 1;

static CUTF8TxtWriter m_logger;
ITxtWriter& g_logger(m_logger);

CAppInit::CAppInit(QApplication& app)
{
#if __android
    string strWorkDir = __sdcardDir __pkgName;
#else
    string strWorkDir = fsutil::getHomePath(__pkgName);
#endif
    (void)fsutil::createDir(strWorkDir);
    fsutil::setWorkDir(strWorkDir);

    m_logger.open(L"XMusic.log", true);

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

    PairList<int, QString> plFontFile;
    plFontFile.add(QFont::Weight::Light, "/font/msyhl-6.23.ttc");
    //plFontFile.add(QFont::Weight::Semilight, "/font/Microsoft-YaHei-Semilight-11.0.ttc");
#if __windows
    //g_mapFontFamily[QFont::Weight::Light] = "微软雅黑 Light";
    g_mapFontFamily[QFont::Weight::DemiBold] = "微软雅黑";
#else
    plFontFile.add(QFont::Weight::DemiBold, "/font/Microsoft-YaHei-Semibold-11.0.ttc");
#endif

    plFontFile([&](int nWeight, QString qsFontFile) {
#if __android
        qsFontFile = "assets:" +  qsFontFile;
#else
        qsFontFile = app.applicationDirPath() + qsFontFile;
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

                g_mapFontFamily[nWeight] = qsFamilyName;
            }
        }
    });

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
#if __android
    strRootDir = strutil::toWstr(__sdcardDir "XMusic");

#else
#if __window
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

    strRootDir = fsutil::getHomePath(L"/XMusic");

//#if __ios && TARGET_IPHONE_SIMULATOR
//    strRootDir = L"/Users/lhyuan/XMusic";
//#endif
#endif

    return fsutil::createDir(strRootDir + L"/" __medialibDir);
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

    //g_logger >> "stop controller";
    m_ctrl.stop();

    CPlayer::QuitSDK();

    //g_logger >> "quit";
    m_logger.close();

/*#if !__android // TODO 规避5.6.1退出的bug
    if (thrUpgrade.joinable())
    {
        thrUpgrade.join();
    }
#endif*/

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
    }*/

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
    IFBuffer ifbMedialibConf((cbyte_p)ba.data(), ba.size());
    tagMedialibConf orgMedialibConf;
    __EnsureReturn(_readMedialibConf(ifbMedialibConf, orgMedialibConf), false);
    g_logger << "orgMedialibConf AppVersion: " >> orgMedialibConf.strAppVersion
             << " CompatibleCode: " << orgMedialibConf.uCompatibleCode
             << " MedialibVersion: " >> orgMedialibConf.uMedialibVersion;

    tagMedialibConf userMedialibConf;
    IFStream ifsMedialibConf(m_model.medialibPath(L"medialib.conf"));
    if (ifsMedialibConf)
    {
         if (_readMedialibConf(ifsMedialibConf, userMedialibConf))
         {
             g_logger << "userMedialibConf AppVersion: " >> userMedialibConf.strAppVersion
                      << " CompatibleCode: " >> userMedialibConf.uCompatibleCode
                      << " MedialibVersion: " >> userMedialibConf.uMedialibVersion;
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

    return _upgradeMedialib(orgMedialibConf, userMedialibConf, eUpgradeErrMsg);
}

bool CApp::_upgradeMedialib(const tagMedialibConf& orgMedialibConf
                            , const tagMedialibConf& userMedialibConf, E_UpgradeErrMsg& eUpgradeErrMsg)
{
    const list<CUpgradeUrl>& lstUpgradeUrl = (userMedialibConf.strAppVersion >= orgMedialibConf.strAppVersion ||
            userMedialibConf.uCompatibleCode >= orgMedialibConf.uCompatibleCode)
            ? userMedialibConf.lstUpgradeUrl : orgMedialibConf.lstUpgradeUrl;
    for (cauto upgradeUrl : lstUpgradeUrl)
    {
        cauto strMedialibUrl = upgradeUrl.medialibUrl();
        g_logger << "dowloadMediaLib: " >> strMedialibUrl;

        CByteBuffer bbfZip;

        CDownloader downloader(3, 30, 1, 3);
        int nRet = downloader.syncDownload(strMedialibUrl, bbfZip, 1, [&](time_t beginTime, int64_t dltotal, int64_t dlnow){
                (void)beginTime;
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

        g_logger << "neeMedialibConf AppVersion: " >> userMedialibConf.strAppVersion
                 << " CompatibleCode: " >> userMedialibConf.uCompatibleCode
                 << " MedialibVersion: " >> userMedialibConf.uMedialibVersion;

        if (newMedialibConf.strAppVersion < MAX(orgMedialibConf.strAppVersion, userMedialibConf.strAppVersion))
        {
            g_logger << "AppVersion invalid: " >> newMedialibConf.strAppVersion;
            continue;
        }

        if (newMedialibConf.uCompatibleCode < MAX(orgMedialibConf.uCompatibleCode, userMedialibConf.uCompatibleCode))
        {
            g_logger << "CompatibleCode invalid: " >> newMedialibConf.uCompatibleCode;
            continue;
        }

        if (newMedialibConf.uMedialibVersion < MAX(orgMedialibConf.uMedialibVersion, userMedialibConf.uMedialibVersion))
        {
            g_logger << "MedialibVersion invalid: " >> newMedialibConf.uMedialibVersion;
            continue;
        }

        if (newMedialibConf.uCompatibleCode > orgMedialibConf.uCompatibleCode)
        {
            g_logger >> "medialib Uncompatible";
            eUpgradeErrMsg = E_UpgradeErrMsg::UEM_MedialibUncompatible;

#if !__ios
            if (newMedialibConf.strAppVersion > orgMedialibConf.strAppVersion)
            {
                if (_upgradeApp(newMedialibConf.lstUpgradeUrl))
                {
                    eUpgradeErrMsg = E_UpgradeErrMsg::UEM_AppUpgraded;
                }
                else
                {
                    eUpgradeErrMsg = E_UpgradeErrMsg::UEM_AppUpgradeFail;
                }
            }
#endif
            return false;
        }

        auto itrMedialib = mapUnzfile.find("medialib");
        if (itrMedialib == mapUnzfile.end())
        {
            g_logger >> "medialib not found";
            continue;
        }

        cauto strDBFile = m_model.medialibPath(__DBFile);
        bool bExistDB = fsutil::existFile(strDBFile);
        if (!bExistDB || newMedialibConf.uMedialibVersion > orgMedialibConf.uMedialibVersion)
        {
            CByteBuffer bbfMedialib;
            if (zipFile.read(itrMedialib->second, bbfMedialib) <= 0)
            {
                g_logger >> "readZip fail: medialib";
                continue;
            }

            /*// TODO 合入正在播放列表
            if (bExistDB)
            {
                (void)fsutil::moveFile(strDBFile, strDBFile + L".bak");
            }*/

            if (!OFStream::writefilex(strDBFile, true, bbfMedialib))
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
                if (!m_model.getMediaLib().loadXUrl(ifbData))
                {
                    g_logger << "loadXUrl fail: " >> unzfile.strPath;
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

int g_nAppUpgradeProgress = -1;

bool CApp::_upgradeApp(const list<CUpgradeUrl>& lstUpgradeUrl)
{
    for (cauto upgradeUrl : lstUpgradeUrl)
    {
        if (!m_bRunSignal)
        {
             return false;
        }

        cauto strAppUrl = upgradeUrl.appUrl();
        if (strAppUrl.empty())
        {
            continue;
        }
        g_logger << "dowloadApp: " >> strAppUrl;

        g_nAppUpgradeProgress = 0;

        CByteBuffer bbfData;
        CDownloader downloader;
        int nRet = downloader.syncDownload(strAppUrl, bbfData, 0, [&](time_t beginTime, int64_t dltotal, int64_t dlnow){
                (void)beginTime;

                if (dltotal > 10000)
                {
                    g_nAppUpgradeProgress = 100*dlnow/dltotal;
                }
                /*else if (dltotal > 0)
                {
                    g_logger << "invalid appBundleSize: " >> dltotal;
                    return false;
                }*/

                return m_bRunSignal;
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

        installApk(strutil::toQstr(strApkFile));

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

        cauto strParentDir = fsutil::GetParentDir(fsutil::getModuleDir()) + "\\";

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
#endif
        return true;
    }

    return false;
}
