
#include "androidutil.h"

//#include <jni.h>
#include <QAndroidJniEnvironment>
//#include <QAndroidJniObject>
#include <QtAndroid>
//#include <QtAndroidExtras>

int g_jniVer = 0;
int g_androidSdkVer = 0;

string g_strExternalStorage;
string g_strSecondaryStorage;

tagAndroidInfo g_androidInfo;

/*TelephonyManager telecomManager = (TelephonyManager)context.getSystemService(Context.TELEPHONY_SERVICE);
String imei = telecomManager.getDeviceId();

WifiManager  wifiMan = (WifiManager)context.getSystemService(Context.WIFI_SERVICE) ;
WifiInfo wifiInf = wifiMan.getConnectionInfo();
return wifiInf.getMacAddress();*/

/*static wstring g_strSDCardPath;
static void checkSdcardPath()
{
    g_strSDCardPath = L"/storage/emulated/0/";
    if (fsutil::existDir(g_strSDCardPath))
    {
        return;
    }

    g_strSDCardPath = __sdcardDir;
}*/

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

#include <sys/system_properties.h>

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved)
{
    (void)reserved;

    char *lpPath = getenv("EXTERNAL_STORAGE"); // 目前发现返回"/sdcard"
    if (lpPath)
    {
        g_strExternalStorage.append(lpPath);
    }
    lpPath = getenv("SECONDARY_STORAGE");
    if (lpPath)
    {
        g_strSecondaryStorage.append(lpPath);
    }

    g_androidSdkVer = QtAndroid::androidSdkVersion(); //manifest中的api28？？
    //？？QAndroidJniObject::getStaticField<jshort>("android/os/Build/VERSION", "SDK_INT");

    __system_property_get("ro.serialno", g_androidInfo.serialno);

    __system_property_get("ro.board.platform", g_androidInfo.board_platform); //主板板卡型号 ro.board.platform=msm8916

    char buf[128] = {0};
    __system_property_get("ro.build.version.sdk", buf);
    g_androidInfo.version_sdk = atoi(buf);

    __system_property_get("ro.build.version.release", buf);
    g_androidInfo.version_release = atoi(buf);

    __system_property_get("ro.build.host", g_androidInfo.host); //系统主机名 ro.build.host=ubuntu-121-114
    __system_property_get("ro.build.tags", g_androidInfo.tags); //系统标记 ro.build.tags=release-keys

    //ro.build.display.full_id //显示标识，标识显示设备的完整版本号 ro.build.display.full_id=A31u_11_A.04_160613
    //ro.build.display.id //显示标识 ro.build.display.id=KTU84P release-keys

    //ro.build.date 系统编译时间 ro.build.date=Mon Jun 13 21:38:06 CST 2016
    //ro.build.date.utc 系统编译时间（UTC版本） ro.build.date.utc=1465825086

    __system_property_get("ro.product.brand", g_androidInfo.product_brand); //机器品牌 ro.product.brand=OPPO
    __system_property_get("ro.product.model", g_androidInfo.product_model); //机器型号 ro.product.model=msm8916_32
    __system_property_get("ro.product.device", g_androidInfo.product_device); //设备名 ro.product.device=A31u
    __system_property_get("ro.product.name", g_androidInfo.product_name); //机器名
    __system_property_get("ro.product.board", g_androidInfo.product_board); //主板名 ro.product.board=msm8916
    __system_property_get("ro.product.manufacturer", g_androidInfo.product_manufacturer); //制造商 ro.product.manufacturer=OPPO

    //ro.product.locale.language    zh
    //ro.product.locale.region      CN


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

bool requestAndroidPermission(cqstr qsPermission) //安卓6、API23以上需要动态申请权限
{
#if (QT_VERSION >= QT_VERSION_CHECK(5,10,0)) // Qt5.10以上
    if(QtAndroid::PermissionResult::Granted != QtAndroid::checkPermission(qsPermission))
    {
        QtAndroid::requestPermissionsSync( QStringList() << qsPermission );
        if (QtAndroid::PermissionResult::Granted != QtAndroid::checkPermission(qsPermission))
        {
            return false;
        }
    }

#else
    if (g_androidSdkVer >= 23 || g_androidInfo.version_sdk >= 23 || g_androidInfo.version_release >= 6)
    {
        return false;
    }
#endif

    //return true;
    return fsutil::existDir(__sdcardDir);
}

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

void hideSplashScreen()
{
#if (QT_VERSION >= QT_VERSION_CHECK(5,7,0)) // Qt5.7以上
    QtAndroid::hideSplashScreen();
#endif
}

void hideSplashScreen(UINT uMs) //渐变时间
{
#if (QT_VERSION >= QT_VERSION_CHECK(5,10,0)) // Qt5.10以上
    QtAndroid::hideSplashScreen(uMs);
#else
    hideSplashScreen();
#endif
}

void fullScreen(bool bSet) //普通全屏控制，qt WindowFullScreen就已经可以实现
{
    QtAndroid::runOnAndroidThread([=]() {
        QtAndroid::androidActivity().callMethod<void>("fullScreen", "(Z)V", jboolean(bSet));
    });
}

void showTranslucentStatusBar(bool bShow) //Qt WindowMaximized状态下，显示半透明状态栏，实际上只需调用一次true
{
    QtAndroid::runOnAndroidThread([=]() {
        QtAndroid::androidActivity().callMethod<void>("showStatusBar", "(Z)V", jboolean(bShow));
    });
}

void showTransparentStatusBar(bool bShow) //Qt WindowMaximized状态下，显示透明状态栏
{
    QtAndroid::runOnAndroidThread([=]() {
        QtAndroid::androidActivity().callMethod<void>("showTransparentStatusBar", "(Z)V", jboolean(bShow));
    });
}

void showLoginToast(bool bSuccess)
{
    QtAndroid::runOnAndroidThread([=]() {
        QtAndroid::androidActivity().callMethod<void>("showLoginToast", "(Z)V", jboolean(bSuccess));
    });
}

void showQuitToast()
{
    QtAndroid::runOnAndroidThread([=]() {
        QtAndroid::androidActivity().callMethod<void>("showQuitToast", "()V");
    });
}

/*这段只作为QAndroidJniObject语法参考
#include <QColor>
void setStatusBar(const QColor& cr, bool bLight)
{
    if (QtAndroid::androidSdkVersion() < 23)
    {
        return;
    }

// WindowManager.LayoutParams
#define FLAG_TRANSLUCENT_STATUS 0x04000000
#define FLAG_DRAWS_SYSTEM_BAR_BACKGROUNDS 0x80000000
// View
#define SYSTEM_UI_FLAG_LIGHT_STATUS_BAR 0x00002000

    QtAndroid::runOnAndroidThread([=]() {
        QAndroidJniObject window = QtAndroid::androidActivity().callObjectMethod("getWindow", "()Landroid/view/Window;");
        window.callMethod<void>("addFlags", "(I)V", FLAG_DRAWS_SYSTEM_BAR_BACKGROUNDS);
        window.callMethod<void>("clearFlags", "(I)V", FLAG_TRANSLUCENT_STATUS);

        //window.callMethod<void>("setStatusBarColor", "(I)V", cr.rgba());

        QAndroidJniObject view = window.callObjectMethod("getDecorView", "()Landroid/view/View;");
        int visibility = view.callMethod<int>("getSystemUiVisibility", "()I");
        if (bLight)
        {
            visibility |= SYSTEM_UI_FLAG_LIGHT_STATUS_BAR;
        }
        else
        {
            visibility &= ~SYSTEM_UI_FLAG_LIGHT_STATUS_BAR;
        }
        view.callMethod<void>("setSystemUiVisibility", "(I)V", visibility);
    });
}*/

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
