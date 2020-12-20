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

#define __cyIPhoneXBangs __size(128)
bool checkIPhoneXBangs(int cx, int cy);

extern signal_t g_bRunSignal;

const bool& usleepex(UINT uMs);

void async(cfn_void cb);
void async(UINT uDelayTime, cfn_void cb);

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

private:
    void _init();

    bool _run();

    virtual bool _startup(cwstr strWorkDir) = 0;

protected:
    virtual int _exec();

public:
    int exec();
    void quit();

    void sync(cfn_void cb);
    void sync(UINT uDelayTime, cfn_void cb);

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
