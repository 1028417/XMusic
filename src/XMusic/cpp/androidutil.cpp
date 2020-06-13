
#include "util/util.h"

#include "androidutil.h"

//#include <jni.h>
//#include <QAndroidJniEnvironment>
//#include <QAndroidJniObject>
#include <QtAndroid>
//#include <QtAndroidExtras>

int g_jniVer = 0;
int g_androidSdkVer = 0;

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved)
{
    (void)reserved;

    g_androidSdkVer = QtAndroid::androidSdkVersion();

    JNIEnv* env = nullptr;
    if (vm->GetEnv((void**) &env, JNI_VERSION_1_6) == JNI_OK)
    {
        return g_jniVer = JNI_VERSION_1_6;
    }
    else if (vm->GetEnv((void**) &env, JNI_VERSION_1_4) == JNI_OK)
    {
        return g_jniVer = JNI_VERSION_1_4;
    }
    else if (vm->GetEnv((void**) &env, JNI_VERSION_1_2) == JNI_OK)
    {
        return g_jniVer = JNI_VERSION_1_2;
    }
    else if (vm->GetEnv((void**) &env, JNI_VERSION_1_1) == JNI_OK)
    {
        return g_jniVer = JNI_VERSION_1_1;
    }

    return JNI_ERR;
}

//？？QAndroidJniObject::getStaticField<jint>("android/os/Build/VERSION", "SDK_INT");

/*bool checkBuildSdkVerion(const char *pszVersion)
{
    return buildSdkVerion() == QAndroidJniObject::getStaticField(
            "android/os/Build/VERSION_CODES", pszVersion);
}*/

#if (QT_VERSION >= QT_VERSION_CHECK(5,10,0)) // Qt5.10以上
bool requestAndroidPermission(cqstr qsPermission) //API 23以上需要动态申请权限
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

static QAndroidJniObject _getService(const char *pszName)
{
    cauto jniName = QAndroidJniObject::getStaticObjectField(
                "android/content/Context",
                pszName,
                "Ljava/lang/String;");
    if (!jniName.isValid())
    {
        return QAndroidJniObject();
    }

    return QtAndroid::androidActivity().callObjectMethod(
                "getSystemService",
                "(Ljava/lang/String;)Ljava/lang/Object;",
                jniName.object<jstring>());
}

// Z -- jboolean -- bllean
// I -- jint -- int;
// J -- jlong -- long
bool checkMobileConnected()
{
    return QtAndroid::androidActivity().callMethod<jboolean>("checkMobileConnected");
}

void vibrate()
{
    cauto jniService = _getService("VIBRATOR_SERVICE");
    if (!jniService.isValid())
    {
        return;
    }
    jniService.callMethod<void>("vibrate", "(J)V", jlong(100));
}

void installApk(cqstr qsApkPath)
{
    // TODO 动态申请安装未知来源应用的权限(据说有些手机不提示未知来源应用)
    cauto jsApkPath = QAndroidJniObject::fromString(qsApkPath);
    QtAndroid::androidActivity().callMethod<void>("installApk",
                "(Ljava/lang/String;)V",
                jsApkPath.object<jstring>());
}
