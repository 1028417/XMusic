
#include <QApplication>
#include <QFontDatabase>

#include "MainWindow.h"
#include <QTimer>

#include "view.h"

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

#define __FontSize 12
#else
#define __FontSize 14
#endif

static CUTF8Writer m_logWriter;
ITxtWriter& g_logWriter(m_logWriter);

map<E_FontWeight, QFont> g_mapFont;

class CApplication : public QApplication
{
private:
    int argc;
    char **argv;

public:
    CApplication() : QApplication(argc=0, argv=0)
    {
#if __android
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

        SMap<E_FontWeight, QString> mapFontFile {
            { E_FontWeight::FW_Light, "Microsoft-YaHei-Light.ttc" }
            //, { E_FontWeight::FW_Normal, "Microsoft-YaHei-Regular.ttc" }
            , { E_FontWeight::FW_SemiBold, "Microsoft-YaHei-Semibold.ttc" }
        };
        mapFontFile([&](E_FontWeight eWeight, QString qsFontFile) {
#if __android
            qsFontFile = "assets:/" +  qsFontFile;
#endif

            QString qsFontfamilyName;
            int fontId = QFontDatabase::addApplicationFont(qsFontFile);
            g_logWriter << "newFontId: " >> fontId;
            if (-1 != fontId)
            {
                cauto& qslst = QFontDatabase::applicationFontFamilies(fontId);
                if (!qslst.empty())
                {
                    qsFontfamilyName = qslst.at(0);
                    g_logWriter << "newfamilyName: " >> qsFontfamilyName;
                }
            }

            QFont font(qsFontfamilyName);
            font.setPointSizeF(__FontSize);
            font.setWeight((int)eWeight);
            g_mapFont[eWeight] = font;

            if (__defFontWeight == eWeight)
            {
                this->setFont(font);
            }
        });
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

public:
    int run()
    {
#if __android
        m_logWriter << "jniVer: " >> g_jniVer;
#endif

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

        int nRet = exec();

        m_logWriter >> "stop controller";
        m_ctrl.stop();

        m_logWriter >> "close model";
        m_model.close();

        CPlayer::QuitSDK();

        m_logWriter >> "quit";
        m_logWriter.close();

        return nRet;
    }
};

int main(int, char *[])
{
    CApp app;
    return app.run();
}
