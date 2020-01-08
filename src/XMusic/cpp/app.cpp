
#include "app.h"

#include "mainwindow.h"

#include <QFontDatabase>

#include <QScreen>

float g_fPixelRatio = 1;

static CUTF8TxtWriter m_logger;
ITxtWriter& g_logger(m_logger);

/*#if __android
static const wstring g_strSdcardPath;

static const wstring& sdcardPath()
{
    if (!g_strSdcardPath.empty())
    {
        return g_strSdcardPath;
    }

    char *pszSdcardPath = getenv("SECONDARY_STORAGE");
    if (NULL == pszSdcardPath)
    {
        pszSdcardPath = getenv("EXTERNAL_STORAGE");
    }
    if (pszSdcardPath)
    {
        return g_strSdcardPath = strutil::fromStr(pszSdcardPath);
    }

    g_strSdcardPath = L"/mnt/sdcard";
    if (!fsutil::existDir(g_strSdcardPath))
    {
        g_strSdcardPath = L"/storage/emulated/0";
        if (!fsutil::existDir(g_strSdcardPath))
        {
            g_strSdcardPath = L"/sdcard";
        }
    }

    return g_strSdcardPath;
}
#endif*/

CAppInit::CAppInit(QApplication& app)
{
#if __windows
    fsutil::setWorkDir(fsutil::getModuleDir());
#elif __android
    fsutil::setWorkDir(__androidDataDir);
#else
    fsutil::setWorkDir(app.applicationDirPath().toStdWString());
#endif

    m_logger.open(L"XMusic.log", true);

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

    float fFontSize = 0;
#if __android
    fFontSize = 12;

#elif __ios
    QSize szScreen = screen->size();
    int nScreenWidth = MIN(szScreen.width(), szScreen.height()) ;
    fFontSize = app.font().pointSizeF();
    fFontSize = fFontSize*nScreenWidth/540;

#elif __mac
    fFontSize = 29;

#elif __windows
    fFontSize = 22;

    float fDPIRate = getDPIRate();
    g_logger << "DPIRate: " >> fDPIRate;
    fFontSize *= fDPIRate;
#endif

    SMap<E_FontWeight, QString> mapFontFile {
        { E_FontWeight::FW_Light, "/font/Microsoft-YaHei-Light.ttc" }
        //, { E_FontWeight::FW_Normal, "/font/Microsoft-YaHei-Regular.ttc" }
        , { E_FontWeight::FW_SemiBold, "/font/Microsoft-YaHei-Semibold.ttc" }
    };
    mapFontFile([&](E_FontWeight eWeight, QString qsFontFile) {
#if __android
        qsFontFile = "assets:" +  qsFontFile;
#else
        qsFontFile = strutil::toQstr(fsutil::workDir()) + qsFontFile;
#endif

        QString qsFontfamilyName;
        int fontId = QFontDatabase::addApplicationFont(qsFontFile);
        g_logger << "newFontId: " >> fontId;
        if (-1 != fontId)
        {
            cauto qslst = QFontDatabase::applicationFontFamilies(fontId);
            if (!qslst.empty())
            {
                qsFontfamilyName = qslst.at(0);
                g_logger << "newfamilyName: " >> qsFontfamilyName;
            }
        }

        QFont font(qsFontfamilyName);
        font.setPointSizeF(fFontSize);
        font.setWeight((int)eWeight);
        g_mapFont[eWeight] = font;

        if (__defFontWeight == eWeight)
        {
            app.setFont(font);
        }
    });
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
    strRootDir = L"/sdcard/XMusic";

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

void CApp::slot_run(bool bUpgradeResult)
{
    if (!m_bRunSignal)
    {
        return;
    }

    if (!bUpgradeResult)
    {
        m_msgbox.show("更新媒体库失败", [&](){
            this->quit();
        });
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

            bool bUpgradeResult = true;
            if (XMediaLib::m_bOnlineMediaLib)
            {
                bUpgradeResult = _upgradeMediaLib();
            }

            auto timeWait = 6 - (time(NULL) - timeBegin);
            if (timeWait > 0)
            {
                mtutil::usleep((UINT)timeWait*1000);
            }

            emit signal_run(bUpgradeResult);
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

bool CApp::_upgradeMediaLib()
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


    IFStream ifsMedialibConf;
    if (!ifsMedialibConf.open(m_model.medialibPath(L"medialib.conf")))
    {
        return false;
    }

    tagMedialibConf userMedialibConf;
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
    tagMedialibConf& prevMedialibConf = *pPrevMedialibConf;

    string strVerInfo;
    int nRet = curlutil::initCurl(strVerInfo);
    if (nRet != 0)
    {
        g_logger << "initCurl fail: " >> nRet;
        return false;
    }
    g_logger << "CurlVerInfo: \n" >> strVerInfo;

    cauto cbProgress = [&](int64_t dltotal, int64_t dlnow){
        (void)dltotal;
        (void)dlnow;
        return m_bRunSignal;
    };

    for (cauto upgradeUrl : prevMedialibConf.lstUpgradeUrl)
    {
        cauto strMedialibUrl = upgradeUrl.medialibUrl();
        g_logger << "dowloadMediaLib: " >> strMedialibUrl;

        CByteBuffer bbfZip;
        CDownloader downloader(false, 10);
        nRet = downloader.syncDownload(strMedialibUrl, bbfZip, 1, cbProgress);
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


        CByteBuffer bbfMedialibConf;
        if (zipFile.read("medialib.conf", bbfMedialibConf) <= 0)
        {
            g_logger >> "readZip fail: medialibConf";
            return false;
        }

        IFBuffer ifbMedialibConf(bbfMedialibConf);
        auto& newMedialibConf = m_model.getMediaLib().medialibConf();
        newMedialibConf.clear();
        if (!_readMedialibConf(ifbMedialibConf, newMedialibConf))
        {
            return false;
        }

        if (newMedialibConf.uMedialibVersion < prevMedialibConf.uMedialibVersion)
        {
            g_logger << "medialib version invalid: " >> newMedialibConf.uMedialibVersion;
            return false;
        }

        /*if (m_newMedialibConf.uCompatibleCode > prevMedialibConf.uCompatibleCode)
            {
                g_logger << "medialib not compatible: " >> m_newMedialibConf.uCompatibleCode;

                if (m_newMedialibConf.strAppVersion != prevMedialibConf.strAppVersion)
                {
                    m_ModelObserver.
                    (void)_upgradeApp(m_newMedialibConf.lstUpgradeUrl);
                }

                return false;
            }*/

        if (!m_model.getMediaLib().loadShareLib(zipFile))
        {
            return false;
        }

        cauto strDBFile = m_model.medialibPath(__medialibFile);
        if (newMedialibConf.uMedialibVersion > prevMedialibConf.uMedialibVersion || !fsutil::existFile(strDBFile))
        {
            CByteBuffer bbfMedialib;
            if (zipFile.read("medialib", bbfMedialib) <= 0)
            {
                g_logger >> "readZip fail: medialib";
                return false;
            }

            OFStream ofsMedialib(strDBFile, true);
            if (!ofsMedialib)
            {
                return false;
            }
            (void)ofsMedialib.writex(bbfMedialib);
        }

        OFStream ofbMedialibConf(m_model.medialibPath(L"medialib.conf"), true);
        if (ofbMedialibConf)
        {
            (void)ofbMedialibConf.writex(bbfMedialibConf);
        }

        return true;
    }

    return false;
}
