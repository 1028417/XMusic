#pragma once

#include <util/util.h>

class cbthread
{
public:
    cbthread() = default;

private:
    mutex m_mtx;
    condition_variable m_cv;
    thread m_thr;

    bool m_bRunSignal = true;

    bool m_bReset = false;

    function<void(XT_RunSignal, const bool& bReset)> m_cb;

public:
    void start_loop()
    {
        unique_lock<mutex> lock(m_mtx);

        m_thr = thread([&](){
            while (m_bRunSignal)
            {
                unique_lock<mutex> lock(m_mtx);
                if (!m_bRunSignal)
                {
                    break;
                }

                if (!m_bReset)
                {
                    m_cv.wait(lock);
                    if (!m_bRunSignal)
                    {
                        break;
                    }
                }

                m_bReset = false;
                auto cb = m_cb;
                lock.unlock();

                cb(m_bRunSignal, m_bReset);
            };
        });
    }

    bool try_emit(const function<void(XT_RunSignal, const bool& bReset)>& cb)
    {
        unique_lock<mutex> lock(m_mtx);
        if (!m_bRunSignal)
        {
            return false;
        }
        if (m_bReset)
        {
            return false;
        }

        m_bReset = true;
        m_cb = cb;
        m_cv.notify_one();

        return true;
    }

    bool try_emit(cfn_void_t<XT_RunSignal> cb)
    {
        return try_emit([=](XT_RunSignal bRunSignal, const bool& bReset){
            (void)bReset;
            cb(bRunSignal);
        });
    }

    bool try_emit(cfn_void cb)
    {
        return try_emit([=](XT_RunSignal bRunSignal, const bool& bReset){
            (void)bRunSignal;
            (void)bReset;
            cb();
        });
    }

    void quit(bool bJoin)
    {
        unique_lock<mutex> lock(m_mtx);
        if (!m_thr.joinable())
        {
            return;
        }

        m_bRunSignal = false;
        m_cv.notify_one();

        if (!bJoin)
        {
            m_thr.detach();
        }
        m_mtx.unlock();

        if (bJoin)
        {
            m_thr.join();
        }
    }
};
