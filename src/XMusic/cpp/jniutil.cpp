
#include "jniutil.h"

//#include <QAndroidJniObject>
//#include <QAndroidJniEnvironment>
#include <QtAndroid>

//#include <QtAndroidExtras>
//#include <jni.h>

int jniutil::g_jniVer = 0;

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved)
{
    (void)reserved;

    JNIEnv* env = nullptr;
    if (vm->GetEnv((void**) &env, JNI_VERSION_1_6) == JNI_OK)
    {
        return jniutil::g_jniVer = JNI_VERSION_1_6;
    }
    else if (vm->GetEnv((void**) &env, JNI_VERSION_1_4) == JNI_OK)
    {
        return jniutil::g_jniVer = JNI_VERSION_1_4;
    }
    else if (vm->GetEnv((void**) &env, JNI_VERSION_1_2) == JNI_OK)
    {
        return jniutil::g_jniVer = JNI_VERSION_1_2;
    }
    else if (vm->GetEnv((void**) &env, JNI_VERSION_1_1) == JNI_OK)
    {
        return jniutil::g_jniVer = JNI_VERSION_1_1;
    }

    return JNI_ERR;
}

//安卓6以上需要动态申请权限
#if (QT_VERSION >= QT_VERSION_CHECK(5,10,0)) // Qt5.10以上
bool jniutil::requestAndroidPermission(cqstr qsPermission)
{
    auto ret = QtAndroid::checkPermission(qsPermission);
    if(QtAndroid::PermissionResult::Granted == ret)
    {
        return true;
    }

    QtAndroid::requestPermissionsSync( QStringList() << qsPermission );

    return QtAndroid::PermissionResult::Granted == QtAndroid::checkPermission(qsPermission);
}
#endif

// Z -- jboolean -- bllean
// I -- jint -- int;
// J -- jlong -- long
bool jniutil::checkMobileConnected()
{
    //QAndroidJniEnvironment env;
    return QtAndroid::androidActivity().callMethod<jboolean>("checkMobileConnected", "()Z");
}

void jniutil::vibrate()
{
    //QAndroidJniEnvironment env;

    cauto jsName = QAndroidJniObject::getStaticObjectField(
                "android/content/Context",
                "VIBRATOR_SERVICE",
                "Ljava/lang/String;");

    cauto jniService = QtAndroid::androidActivity().callObjectMethod(
                "getSystemService",
                "(Ljava/lang/String;)Ljava/lang/Object;",
                jsName.object<jstring>());

    jniService.callMethod<void>("vibrate", "(J)V", jlong(100));
}

void jniutil::installApk(cqstr qsApkPath)
{
    cauto jsApkPath = QAndroidJniObject::fromString(qsApkPath);
    QtAndroid::androidActivity().callMethod<void>("installApk",
                "(Ljava/lang/String;)V",
                jsApkPath.object<jstring>());
}

//QAndroidJniObject::callStaticMethod<void>("xmusic/XActivity", "installApk",
