
#pragma once

#include "util/util.h"

#if __android
extern int g_jniVer;
extern int g_androidSdkVer;

extern function<void(int,int,int)> g_fnAccelerometerNotify;

#if (QT_VERSION >= QT_VERSION_CHECK(5,10,0)) // Qt5.10以上
    bool requestAndroidPermission(cqstr qsPermission); //API 23以上需要动态申请权限
#else
#define requestAndroidPermission(qs) true
#endif

bool checkMobileConnected();

void vibrate();

void installApk(cqstr qsApkPath);
#endif
