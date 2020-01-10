
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

#if __windows || __mac
#include <QLockFile>
#endif

int main(int argc, char *argv[])
{
#if __windows || __mac
    QLockFile lf(strutil::toQstr(fsutil::getHomeDir()) + "/xmusic.lock");
    lf.setStaleLockTime(1);
    if (!lf.tryLock(0))
    {
        return -1;
    }
#endif

#if __windows
    extern void InitMinDump(const string&);
    InitMinDump("xmusic_dump_");
#endif

//#if __windows && (QT_VERSION >= QT_VERSION_CHECK(5,6,0))
//    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
//#endif

    auto app = new CApp(argc, argv);

#if __android
    g_logger << "jniVer: " >> g_jniVer;
#endif

    int nRet = app->run();
    delete app;
    return nRet;
}
