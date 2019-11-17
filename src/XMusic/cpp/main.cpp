
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
#include <QLockFile>

int main(int argc, char *argv[])
{
#if __windows
    extern void InitMinDump();
    InitMinDump();
#endif

#if __windows || __mac
    QLockFile lockFile("temp");
    if (!lockFile.tryLock(100))
    {
        return -1;
    }
#endif

//#if __windows && (QT_VERSION >= QT_VERSION_CHECK(5,6,0))
//    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
//#endif

#if __android
    if (!fsutil::createDir(__androidDataDir))
    {
        return -1;
    }
#endif

    auto app = new CXMusicApp(argc, argv);

#if __android
    g_logger << "jniVer: " >> g_jniVer;
#endif

    int nRet = app->run();
    delete app;
    return nRet;
}
