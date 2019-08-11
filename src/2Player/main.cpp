
#include <QApplication>
#include <QFontDatabase>

#include "MainWindow.h"
#include <QTimer>

#include "view.h"

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

#ifdef __ANDROID__
#define __FontFile "assets:/msyhl.ttc";
#else
#define __FontFile "msyhl.ttc"
#endif

static CUTF8Writer m_logWriter;
const ITxtWriter& g_logWriter(m_logWriter);

class CApplication : public QApplication
{
private:
    int argc;
    char **argv;

public:
    CApplication() : QApplication(argc=0, argv=0)
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

        wstring strDataDir = L"/sdcard/Android/data/XMusic";
        if (fsutil::createDir(strDataDir))
        {
            fsutil::setWorkDir(strDataDir);
        }
#else
        fsutil::setWorkDir(fsutil::getModuleDir());
#endif

        m_logWriter.open(L"XMusic.log", true);

        /*QFontDatabase database;
        for (cauto& familyName : database.families(QFontDatabase::Korean))
        {
            g_logWriter << "familyName: " >> familyName;
            //this->setFont(QFont(familyName, 10));
            break;
        }*/

        int fontId = QFontDatabase::addApplicationFont(__FontFile);
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
    }
};

class CApp : public CApplication
{
public:
    CApp() :
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

private:
    bool _init()
    {
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
#endif

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
    CApp app;
    return app.run();
}
