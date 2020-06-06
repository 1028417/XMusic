
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

// Qt5.10以下
/*#include <QtCore/private/qjni_p.h>
#include <QtCore/private/qjnihelpers_p.h>

static QtAndroid::PermissionResultMap privateToPublicPermissionsHash(const QtAndroidPrivate::PermissionsHash &privateHash)
{
    QtAndroid::PermissionResultMap hash;
    for (auto it = privateHash.constBegin(); it != privateHash.constEnd(); ++it)
        hash[it.key()] = QtAndroid::PermissionResult(it.value());
    return hash;
}

static QtAndroid::PermissionResultMap requestPermissionsSync(const QStringList &permissions, int timeoutMs)
{
    return privateToPublicPermissionsHash(QtAndroidPrivate::requestPermissionsSync(QJNIEnvironmentPrivate(), permissions, timeoutMs));
}*/

//安卓6以上需要动态申请权限
bool jniutil::requestAndroidPermission(cqstr qsPermission)
{
    auto ret = QtAndroid::checkPermission(qsPermission);
    if(QtAndroid::PermissionResult::Granted == ret)
    {
        return true;
    }

    QtAndroid::requestPermissionsSync( QStringList() << qsPermission ); // Qt5.10以上版本才支持

    return QtAndroid::PermissionResult::Granted == QtAndroid::checkPermission(qsPermission);
}

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
