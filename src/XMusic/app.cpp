
#include <QApplication>
#include <QFontDatabase>

#include "mainwindow.h"

#include "app.h"

#if __android
//#include <QAndroidJniObject>
//#include <QAndroidJniEnvironment>
//#include <QtAndroid>
//#include <QtAndroidExtras>

#include <jni.h>

int g_jniVer = 0;

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
#endif

static CUTF8Writer m_logger;
ITxtWriter& g_logger(m_logger);

map<E_FontWeight, QFont> g_mapFont;

class CApplication : public QApplication
{
public:
    CApplication(int argc, char **argv);
};

CApplication::CApplication(int argc, char **argv) : QApplication(argc, argv)
{
#if __winqt
    fsutil::setWorkDir(fsutil::getModuleDir());

#elif __android
    /*string strSdcardPath
    char *pszSdcardPath = getenv("SECONDARY_STORAGE");
    if (NULL == pszSdcardPath)
    {
        pszSdcardPath = getenv("EXTERNAL_STORAGE");
    }
    if (NULL != pszSdcardPath)
    {
        strSdcardPath = pszSdcardPath;
    }*/

    wstring strDataDir = L"/sdcard/XMusic/.xmusic";
    if (fsutil::createDir(strDataDir))
    {
        fsutil::setWorkDir(strDataDir);
    }

#elif __mac
    wstring strWorkDir = fsutil::workDir();
    strWorkDir = fsutil::GetParentDir(strWorkDir);
    strWorkDir = fsutil::GetParentDir(strWorkDir);
    strWorkDir = fsutil::GetParentDir(strWorkDir);
    strWorkDir = fsutil::GetParentDir(strWorkDir);
    fsutil::setWorkDir(strWorkDir);
#endif

    m_logger.open(L"XMusic.log", true);

    SMap<E_FontWeight, QString> mapFontFile {
        { E_FontWeight::FW_Light, "Microsoft-YaHei-Light.ttc" }
        //, { E_FontWeight::FW_Normal, "Microsoft-YaHei-Regular.ttc" }
        , { E_FontWeight::FW_SemiBold, "Microsoft-YaHei-Semibold.ttc" }
    };
    mapFontFile([&](E_FontWeight eWeight, QString qsFontFile) {
#if __android
        qsFontFile = "assets:/" +  qsFontFile;
#elif __mac
        qsFontFile = wsutil::toQStr(fsutil::workDir() + __wcFSSlant) + qsFontFile;
#endif

        QString qsFontfamilyName;
        int fontId = QFontDatabase::addApplicationFont(qsFontFile);
        g_logger << "newFontId: " >> fontId;
        if (-1 != fontId)
        {
            cauto& qslst = QFontDatabase::applicationFontFamilies(fontId);
            if (!qslst.empty())
            {
                qsFontfamilyName = qslst.at(0);
                g_logger << "newfamilyName: " >> qsFontfamilyName;
            }
        }

        UINT uFontSize = 12;
#if __android
        uFontSize = 12;
#elif __mac
        uFontSize = 26;
#elif __windows
        uFontSize = 21;
        uFontSize = UINT(uFontSize * getDPIRate());
#endif

        QFont font(qsFontfamilyName);
        font.setPointSize(uFontSize);
        font.setWeight((int)eWeight);
        g_mapFont[eWeight] = font;

        if (__defFontWeight == eWeight)
        {
            this->setFont(font);
        }
    });
}


class CApp : public CApplication
{
public:
    CApp(int argc, char **argv) :
        CApplication(argc, argv),
        m_model(m_mainWnd),
        m_view(*this, m_mainWnd, m_model, m_ctrl),
        m_ctrl(m_view, m_model),
        m_mainWnd(m_view)
    {
    }

private:
    CModel m_model;

    CPlayerView m_view;

    CController m_ctrl;

    class MainWindow m_mainWnd;

public:
    int run();
};

int CApp::run()
{
#if __android
    m_logger << "jniVer: " >> g_jniVer;
#endif

    m_model.init();

    m_mainWnd.showLogo();

    QTimer::singleShot(6000, [&](){
        if (!m_model.start())
        {
            m_logger >> "init model fail";
            this->quit();
            return;
        }
        m_logger >> "init model success";

        m_logger >> "start controller";
        if (!m_ctrl.start())
        {
            m_logger >> "start controller fail";
            this->quit();
            return;
        }
        m_logger >> "start controller success, app running";

        m_mainWnd.show();
    });

    int nRet = exec();

    m_logger >> "stop controller";
    m_ctrl.stop();

    m_logger >> "close model";
    m_model.close();

    CPlayer::QuitSDK();

    m_logger >> "quit";
    m_logger.close();

    return nRet;
}


QMainWindow& CPlayerView::getMainWnd() const
{
    return m_mainWnd;
}

IModelObserver& CPlayerView::getModelObserver()
{
    return m_mainWnd;
}


int main(int argc, char *argv[])
{
    CApp app(argc, argv);

    return app.run();
}
