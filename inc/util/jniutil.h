
#pragma once

class __UtilExt jniutil
{
public:
    //安卓6以上需要动态申请权限
#if (QT_VERSION >= QT_VERSION_CHECK(5,10,0)) // Qt5.10以上
    static bool requestAndroidPermission(cqstr qsPermission);
#endif

	static bool checkMobileConnected();

	static void vibrate();

    static void installApk(cqstr qsApkPath);
};
