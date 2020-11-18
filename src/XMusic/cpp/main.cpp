
#include "app.h"

#if __windows || __mac
QLockFile g_lf(fsutil::getHomeDir() + "/xmusic.lock");
#endif

int main(int argc, char *argv[])
{
#if __windows || __mac
    g_lf.setStaleLockTime(1);
    if (!g_lf.tryLock(0))
    {
        return -1;
    }

#if __windows
    extern void InitMinDump(const string&);
    InitMinDump("xmusic_dump_");

    //#if (QT_VERSION >= QT_VERSION_CHECK(5,6,0))
    //    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    //#endif
#endif
#endif

#if __android
    //内置包路径不需要权限 data/data/xxx/files、/data/data/xxx/cache分别对应应用详情中的清除数据和清除缓存
    wstring strWorkDir = L"/data/data/" __pkgName;
    // = __sdcardDir L"Android/data/" __pkgName //居然也对应内置存储同一路径;

#else
    wstring strWorkDir = fsutil::getHomeDir().toStdWString() + L"/" __pkgName;
#endif
    if (!fsutil::createDir(strWorkDir))
    {
        return -1;
    }

#if __windows
    fsutil::setWorkDir(strutil::toGbk(strWorkDir));
#else
    fsutil::setWorkDir(strutil::toUtf8(strWorkDir));
#endif

    auto app = new CApp(argc, argv);

    int nRet = app->run(strWorkDir);
    //delete app;

    //fsutil::copyFile(__WS2Q(strWorkDir+L"/xmusic.log"), "/sdcard/xmusic.log");

    return nRet;
}
