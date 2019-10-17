
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

int main(int argc, char *argv[])
{
    tagXmlElementInfo rootElementInfo;
    if (!xmlutil::loadHtml("C:/Users/Administrator/Desktop/0.txt", rootElementInfo))
    {
        return -1;
    }

    list<string> lstChildName{"li", "input"};
    rootElementInfo.getChildAttr(lstChildName, "value", [](tagXmlElementInfo& childInfo, const string& strAttrValue){
        return true;
    });

    /*if (!fsutil::zUncompressZip("d:/zlib-1.2.11.zip", "d:/zlib-1.2.11-----"))
    {
        return -1;
    }*/

//#if __windows && (QT_VERSION >= QT_VERSION_CHECK(5,6,0))
//    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
//#endif

#if __android
    wstring strDataDir = L"/sdcard/Android/data/com.musicrosoft.xmusic";
    if (!fsutil::createDir(strDataDir))
    {
        return -1;
    }
    fsutil::setWorkDir(strDataDir);

#else
    fsutil::setWorkDir(fsutil::getAppDir());
#endif

    m_logger.open(L"XMusic.log", true);

#if __android
    g_logger << "jniVer: " >> g_jniVer;
#endif

    CXMusicApp app(argc, argv);
    int nRet = app.run();

    m_logger.close();

    return nRet;
}
