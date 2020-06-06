
#pragma once

#include <util/util.h>

class jniutil
{
public:
	static int g_jniVer;

    static bool requestAndroidPermission(cqstr qsPermission);

	static bool checkMobileConnected();

	static void vibrate();

    static void installApk(cqstr qsApkPath);
};
