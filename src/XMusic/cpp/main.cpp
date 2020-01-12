
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
#endif

#if __windows
    extern void InitMinDump(const string&);
    InitMinDump("xmusic_dump_");
#endif

//#if __windows && (QT_VERSION >= QT_VERSION_CHECK(5,6,0))
//    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
//#endif

    auto app = new CApp(argc, argv);
    int nRet = app->run();
    delete app;
    return nRet;
}
