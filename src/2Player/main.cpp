
#ifdef __ANDROID__
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

    #define __FontSize 11
#else
    #define __FontSize 15
#endif

#define __FontFile "msyhl.ttc"

#include <QApplication>
#include <QFontDatabase>

#include "MainWindow.h"
#include <QTimer>

#include "view.h"

static CUTF8Writer m_logWriter;
const ITxtWriter& g_logWriter(m_logWriter);

class CApp : public QApplication
{
public:
    CApp(int argc, char *argv[]):
        QApplication(argc, argv),
        m_mainWnd(m_view),
        m_view(*this, m_mainWnd, m_model, m_ctrl),
        m_model(m_mainWnd),
        m_ctrl(m_view, m_model)
    {
    }

private:
    class MainWindow m_mainWnd;

    CPlayerView m_view;

    CModel m_model;

    CController m_ctrl;

private:
    bool _init()
    {
#ifdef __ANDROID__
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

        wstring strAppDataDir = L"/sdcard/Android/data/XMusic";
        if (fsutil::createDir(strAppDataDir))
        {
            fsutil::setWorkDir(strAppDataDir);
        }
#else
        fsutil::setWorkDir(fsutil::getModuleDir());
#endif

        m_logWriter.open(L"XMusic.log", true);

#ifdef __ANDROID__
        m_logWriter << "jniVer: " >> g_jniVer;

        cauto& strDBFile(fsutil::workDir() + L"/data.db");
        if (!fsutil::existFile(strDBFile))
        {
            g_logWriter << "copydb: " >> strDBFile;

            if (!fsutil::copyFile(L"assets:/data.db", strDBFile))
            {
                g_logWriter.writeln(L"copydb fail");
                return false;
            }
        }

        QString qsFontFile = "assets:/" __FontFile;
#else
        QString qsFontFile = __FontFile;
#endif

        /*QFontDatabase database;
        for (cauto& familyName : database.families(QFontDatabase::Korean))
        {
            g_logWriter << "familyName: " >> familyName;
            //this->setFont(QFont(familyName, 10));
            break;
        }*/

        int fontId = QFontDatabase::addApplicationFont(qsFontFile);
        g_logWriter << "newFontId: " >> fontId;
        if (-1 != fontId)
        {
            cauto& familyName = QFontDatabase::applicationFontFamilies(fontId).at(0);
            g_logWriter << "newfamilyName: " >> familyName;
            this->setFont(QFont(familyName, __FontSize));
        }
        else
        {
            this->setFont(QFont(QString::fromLocal8Bit(""), __FontSize));
        }

        return true;
    }

public:
    int run()
    {
        int nRet = -1;
        if (_init())
        {
            m_logWriter >> "init success";

            m_mainWnd.showLogo();

            QTimer::singleShot(6000, [&](){
                if (!m_model.init())
                {
                    m_logWriter >> "init model fail";
                    this->quit();
                    return;
                }
                m_logWriter >> "init model success";

                m_logWriter >> "start controller";
                if (!m_ctrl.start())
                {
                    m_logWriter >> "start controller fail";
                    this->quit();
                    return;
                }
                m_logWriter >> "start controller success, app running";

                m_mainWnd.show();
            });

            nRet = exec();

            m_logWriter >> "stop controller";
            m_ctrl.stop();

            m_logWriter >> "close model";
            m_model.close();

            CPlayer::QuitSDK();
        }
        else
        {
            m_logWriter >> "init fail";
        }

        m_logWriter >> "quit";
        m_logWriter.close();

        return nRet;
    }
};

int main(int argc, char *argv[])
{
    CApp app(argc, argv);
    return app.run();
}
