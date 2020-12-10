
#pragma once

#include "util/util.h"

#if __android
extern int g_jniVer;
extern int g_androidSdkVer;

struct tagAndroidDevInfo
{
    int version_sdk = 0;
    int version_release = 0;

    char serialno[256] = {0};
    char board_platform[256] = {0};

    char host[256] = {0};
    char tags[256] = {0};

    char product_brand[256] = {0};
    char product_model[256] = {0};
    char product_device[256] = {0};
    char product_name[256] = {0};
    char product_board[256] = {0};
    char product_manufacturer[256] = {0};
};
extern tagAndroidDevInfo g_androidDevInfo;

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
