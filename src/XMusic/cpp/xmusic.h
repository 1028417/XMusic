#pragma once

#include "viewdef.h"
#include "model/model.h"
#include "../controller.h"

#include "androidutil.h"

#include <QApplication>

using Double_T = float; //= double; // double_t与math.h冲突

extern cwstr g_strWorkDir;

extern ITxtWriter& g_logger;

struct tagScreenInfo
{
    int nMaxSide = 0;
    int nMinSide = 0;

    float fDPI = 0;

    UINT pixelRatio = 1; //float fPixelRatio = 1;
};
extern const tagScreenInfo& g_screen;

#if __android
extern const bool& g_bAndroidSDPermission;
bool requestAndroidSDPermission();
#endif

#if __ios
#define __cyIPhoneXBangs __size(128)
inline static UINT checkIPhoneXBangs(int cx, int cy)
{
    if ((375 == cx && 812 == cy) || (414 == cx && 896 == cy))
    {
        return __cyIPhoneXBangs;
    }
    return 0;
}
#else
#define checkIPhoneXBangs(cx, cy) 0u
#endif

#if __android
#define __cyAndroidStatusBar 30
inline static UINT checkAndroidStatusBar()
{
    extern bool g_bFullScreen;
    if (!g_bFullScreen)
    {
        return __cyAndroidStatusBar;
    }
    return 0;
}
#else
#define checkAndroidStatusBar() 0u
#endif

extern signal_t g_bRunSignal;

const bool& usleepex(UINT uMs);

void async(cfn_void cb);
void async(UINT uDelayTime, cfn_void cb);

const WString& mediaQualityString(E_MediaQuality eQuality);

bool installApp(const CByteBuffer& bbfData);

Q_DECLARE_METATYPE(fn_void);

class CAppBase : public QApplication
{
    Q_OBJECT
public:
    CAppBase();

private:
    list<XThread> m_lstThread;

signals:
    void signal_sync(fn_void cb);
    void signal_syncex(fn_void cb);

private:
    void _init();

    bool _run();

    virtual bool _startup(cwstr strWorkDir) = 0;

protected:
    virtual int _exec();

public:
    int exec();
    void quit();

    inline void sync(cfn_void cb)
    {
        if (!g_bRunSignal)
        {
            return;
        }

        emit signal_sync(cb);
    }    
    void sync(cfn_void cb, const UINT *pSeq)
    {
        auto seq = *pSeq;
        sync([=]{
            if (seq != *pSeq)
            {
                return;
            }
            cb();
        });
    }

    inline void sync(UINT uDelayTime, cfn_void cb)
    {
        sync([=]{
            async(uDelayTime, cb);
        });
    }
    void sync(UINT uDelayTime, cfn_void cb, const UINT *pSeq)
    {
        auto seq = *pSeq;
        sync(uDelayTime, [=]{
            if (seq != *pSeq)
            {
                return;
            }
            cb();
        });
    }

    void syncex(cfn_void cb) //阻塞式的，使用时要避免死锁
    {
        if (!g_bRunSignal)
        {
            return;
        }

        emit signal_syncex(cb);
    }

    XThread& thread()
    {
        m_lstThread.emplace_back();
        return m_lstThread.back();
    }

    template<typename... T>
    XThread& thread(const T&... args)
    {
        m_lstThread.emplace_back();
        auto& thr = m_lstThread.back();
        thr.start(args...);
        return thr;
    }
};

#include "app.h"

extern CApp& __app;
