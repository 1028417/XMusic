
#pragma once

#include "util/util.h"

#if __android
extern int g_jniVer;
extern int g_androidSdkVer;

extern string g_strExternalStorage;
extern string g_strSecondaryStorage;

struct tagAndroidInfo
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
extern tagAndroidInfo g_androidInfo;

extern function<void(int,int,int)> g_fnAccelerometerNotify;

bool requestAndroidPermission(cqstr qsPermission);

void hideSplashScreen();
void hideSplashScreen(UINT uMs);

void fullScreenex(bool bSet); //合并成一次调用
void fullScreen(bool bSet);

void showTranslucentStatusBar(bool bShow);
void showTransparentStatusBar(bool bShow);

void showToast(cqstr qsTip, bool bLongTime);

bool checkMobileConnected();
bool checkUnMobileConnected();

void vibrate();

void installApk(cqstr qsApkPath);
#endif
