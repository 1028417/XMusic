
#include "app.h"

#include "mainwindow.h"

#include <QFontDatabase>

#include <QScreen>

static CUTF8TxtWriter m_logger;
ITxtWriter& g_logger(m_logger);

map<E_FontWeight, QFont> g_mapFont;

float g_fPixelRatio = 1;

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

CApp::CApp(int argc, char **argv) : QApplication(argc, argv)
{
#if __android
    fsutil::setWorkDir(__androidDataDir);
#else
    fsutil::setWorkDir(fsutil::getAppDir());
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
    fFontSize = this->font().pointSizeF();
    fFontSize = fFontSize*nScreenWidth/540;

#elif __mac
    fFontSize = 29;

#elif __windows
    float fDPIRate = getDPIRate();
    g_logger << "DPIRate: " >> fDPIRate;

    fFontSize = 22;
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
            this->setFont(font);
        }
    });
}

bool CXMusicApp::_resetRootDir(wstring& strRootDir)
{
#if __android
    strRootDir = L"/sdcard/XMusic";

#elif __mac
    strRootDir = fsutil::getHomeDir() + L"/XMusic";

#elif __ios
#if TARGET_IPHONE_SIMULATOR
    strRootDir = L"/Users/lhyuan/XMusic";
#else
    strRootDir = fsutil::getHomeDir() + L"/XMusic";
#endif

#else
    if (!XMediaLib::m_bOnlineMediaLib)
    {
        if (strRootDir.empty() || !fsutil::existDir(strRootDir))
        {
            CFolderDlg FolderDlg;
            strRootDir = FolderDlg.Show((HWND)m_mainWnd.winId(), NULL, L"设定媒体库路径", L"请选择媒体库目录");
            if (strRootDir.empty())
            {
                return false;
            }
        }

        return true;
    }

    strRootDir = fsutil::getHomeDir() + L"/XMusic";
#endif

    return fsutil::createDir(strRootDir + __medialibPath);
}

int CXMusicApp::run()
{
    auto& option = m_model.init();

    m_mainWnd.showLogo();

    std::thread thrUpgrade;
    bool bUpgradeFail = false;

    timerutil::async([&](){
        if (!_resetRootDir(option.strRootDir))
        {
            this->quit();
            return;
        }

        if (XMediaLib::m_bOnlineMediaLib)
        {
            thrUpgrade = std::thread([&](){
                g_logger >> "upgradeMediaLib";
                if (!_upgradeMediaLib())
                {
                    g_logger >> "upgradeMediaLib fail";
                    bUpgradeFail = true;
                }
            });
        }

        timerutil::async(6000, [&](){
            if (XMediaLib::m_bOnlineMediaLib)
            {
                thrUpgrade.join();
                if (bUpgradeFail)
                {
                    this->quit();
                    return;
                }
            }

            if(!_run())
            {
                this->quit();
                return;
            }

            m_mainWnd.show();
        });
    });

    int nRet = exec();

    g_logger >> "stop controller";
    m_ctrl.stop();

    CPlayer::QuitSDK();

    g_logger >> "quit";
    m_logger.close();

    return nRet;
}

struct tagUpgradeConf
{
    UINT uVersion = 0;

    list<string> lstUrl;
};

static bool _readUpgradeConf(Instream& ins, tagUpgradeConf& upgradeConf)
{
    JValue jRoot;
    if (!jsonutil::loadFile(ins, jRoot))
    {
        g_logger >> "readUpgradeConf fail: invalid jsonStream";
        return false;
    }
    const JValue& medialib = jRoot["medialib"];
    if (medialib.isNull())
    {
        g_logger >> "readUpgradeConf fail: medialib";
        return false;
    }

    if (!jsonutil::get(medialib["version"], upgradeConf.uVersion))
    {
        g_logger >> "readUpgradeConf fail: version";
        return false;
    }

    if (!jsonutil::getArray(medialib["url"], upgradeConf.lstUrl))
    {
        g_logger >> "readUpgradeConf fail: version";
        return false;
    }

    return true;
}

#define __upgradeConfFile (m_model.getOptionMgr().getOption().strRootDir + __medialibPath L"upgrade.conf")

bool CXMusicApp::_upgradeMediaLib()
{
    tagUpgradeConf upgradeConf;

    IFStream ifsUpgradeConf;
    if (ifsUpgradeConf.open(__upgradeConfFile))
    {
        __EnsureReturn(_readUpgradeConf(ifsUpgradeConf, upgradeConf), false);
        ifsUpgradeConf.close();
    }
    else
    {
#if __android
        QFile qf("assets:/upgrade.conf");
#else
        QFile qf(":/upgrade.conf");
#endif
        if (!qf.open(QFile::OpenModeFlag::ReadOnly))
        {
            g_logger >> "loadUpgradeConfResource fail";
            return false;
        }

        cauto ba = qf.readAll();
        IFBuffer ifbUpgradeConf((const byte_t*)ba.data(), ba.size());
        if (!ifbUpgradeConf)
        {
            g_logger >> "loadUpgradeConf fail";
            return false;
        }

        __EnsureReturn(_readUpgradeConf(ifbUpgradeConf, upgradeConf), false);
    }

    for (cauto strUrl : upgradeConf.lstUrl)
    {        
        CByteBuffer bbfZip;
        int nRet = CFileDownload::inst().download(strUrl, bbfZip);
        if (nRet != 0)
        {
            g_logger << "download fail: " << nRet << ", URL: " >> strUrl;
            continue;
        }

        IFBuffer ifbZip(bbfZip);
        /*string strPwd = "medialib";
        strPwd.append(".zip");*/
        CZipFile zipFile(ifbZip);
        if (!zipFile)
        {
            g_logger >> "invalid zipfile";
            continue;
        }

        return _upgradeMediaLib(upgradeConf.uVersion, zipFile);
    }

    return false;
}

bool CXMusicApp::_upgradeMediaLib(UINT uVersion, CZipFile& zipFile)
{
    CByteBuffer bbfUpgradeConf;
    if (zipFile.read("upgrade.conf", bbfUpgradeConf) <= 0)
    {
        g_logger >> "readZip fail: upgrade.conf";
        return false;
    }

    IFBuffer ifbUpgradeConf(bbfUpgradeConf);
    tagUpgradeConf newUpgradeConf;
    if (!_readUpgradeConf(ifbUpgradeConf, newUpgradeConf))
    {
        return false;
    }
    if (newUpgradeConf.uVersion != uVersion)
    {
        CByteBuffer bbfMedialib;
        if (zipFile.read("medialib", bbfMedialib) <= 0)
        {
            g_logger >> "readZip fail: medialib";
            return false;
        }

        cauto strDBFile = m_model.getOptionMgr().getOption().strRootDir + __medialibPath __medialibFile;
        OFStream ofsMedialib(strDBFile, true);
        if (!ofsMedialib)
        {
            return false;
        }
        (void)ofsMedialib.writex(bbfMedialib);

        OFStream ofbUpgradeConf(__upgradeConfFile, true);
        if (!ofbUpgradeConf)
        {
            return false;
        }
        (void)ofbUpgradeConf.writex(bbfUpgradeConf);
    }

    for (cauto pr : zipFile.fileMap())
    {
        const tagUnzFileInfo& fileInfo = pr.second;

        if (strutil::endWith(fileInfo.strPath, ".xurl"))
        {
            CByteBuffer bbfXurl;
            if (zipFile.read(fileInfo, bbfXurl) <= 0)
            {
                g_logger << "readXurl fail: " >> fileInfo.strPath;
                return false;
            }

            IFBuffer ifbXurl(bbfXurl);
            if (!m_model.getMediaLib().loadXurl(ifbXurl))
            {
                g_logger << "loadXurl fail: " >> fileInfo.strPath;
                return false;
            }
        }
        else if (strutil::endWith(fileInfo.strPath, ".snapshot.json"))
        {
            CByteBuffer bbfSnapshot;
            if (zipFile.read(fileInfo, bbfSnapshot) <= 0)
            {
                g_logger << "readSnapshot fail: " >> fileInfo.strPath;
                return false;
            }

            IFBuffer ifbSnapshot(bbfSnapshot);
            if (!m_model.getMediaLib().loadSnapshot(ifbSnapshot))
            {
                g_logger << "loadSnapshot fail: " >> fileInfo.strPath;
                return false;
            }
        }
    }

    return true;
}

bool CXMusicApp::_run()
{
    g_logger >> "initMediaLib";
    if (!m_model.initMediaLib())
    {
        g_logger >> "initMediaLib fail";
        return false;
    }

    g_logger >> "start controller";
    if (!m_ctrl.start())
    {
        g_logger >> "start controller fail";
        return false;
    }
    g_logger >> "start controller success, app running";

    return true;
}
