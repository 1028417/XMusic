
#include "app.h"

#if __windows || __mac
#include <QLockFile>
#endif

int main(int argc, char *argv[])
{
#if __windows || __mac
    QLockFile lf(fsutil::getHomeDir() + "/xmusic.lock");
    lf.setStaleLockTime(1);
    if (!lf.tryLock(0))
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
