
#include "util/util.h"

#include "androidutil.h"

//#include <jni.h>
#include <QAndroidJniEnvironment>
//#include <QAndroidJniObject>
#include <QtAndroid>
//#include <QtAndroidExtras>

int g_jniVer = 0;
int g_androidSdkVer = 0;

function<void(int,int,int)> g_fnAccelerometerNotify;

static void accelerometerNotify(int x, int y, int z)
{
    if (g_fnAccelerometerNotify)
    {
        g_fnAccelerometerNotify(x,y,z);
    }
}

extern "C" {
JNIEXPORT void JNICALL
Java_xmusic_XActivity_accelerometerNotify(JNIEnv*, jclass
                                          , jint x, jint y, jint z)
{
    accelerometerNotify(x,y,z);
}
}

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved)
{
    (void)reserved;

    g_androidSdkVer = QtAndroid::androidSdkVersion();
    //？？QAndroidJniObject::getStaticField<jshort>("android/os/Build/VERSION", "SDK_INT");

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

/*jboolean    Z
jbyte       B
jchar       C
jshort      S
jint        I
jlong       J
jfloat      F
jdouble     D
jobject     L*/

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
