#pragma once

#include <thread>

//#include <future>

#define __usleep(uMs) std::this_thread::sleep_for(chrono::milliseconds(uMs))
#define __yield() std::this_thread::yield()

class __UtilExt mtutil
{
#if __windows
public:
	static bool apcWakeup(HANDLE hThread, cfn_void fn = NULL)
	{
		return 0 != QueueUserAPC(APCFunc, hThread, fn ? (ULONG_PTR)&fn : 0);
	}

	static bool apcWakeup(DWORD dwThreadID, cfn_void fn = NULL)
	{
		HANDLE hThread = OpenThread(PROCESS_ALL_ACCESS, FALSE, dwThreadID);
		return apcWakeup(hThread, fn);
	}

	static bool poolStart(cfn_void fn)
	{
		return TRUE == QueueUserWorkItem(UserWorkItemProc, fn ? (PVOID)&fn : 0, WT_EXECUTEDEFAULT);
	}

private:
	static VOID WINAPI APCFunc(ULONG_PTR dwParam)
	{
		auto  pfn = (const fn_void *)dwParam;
		if (pfn && *pfn)
		{
			(*pfn)();
		}
	}

	static DWORD WINAPI UserWorkItemProc(LPVOID lpPara)
	{
		auto  pfn = (const fn_void *)lpPara;
		if (pfn && *pfn)
		{
			(*pfn)();
		}

		return 0;
	}
#endif

public:
	static void thread(cfn_void cb)
	{
		std::thread(cb).detach();
	}

    static bool concurrence(cfn_bool& fn, cfn_bool& cbThread)
	{
		bool bRet = true;
		std::thread thr([&]{
			if (!cbThread())
			{
				bRet = false;
			}
		});

		if (!fn())
		{
			bRet = false;
		}

		thr.join();

		return bRet;
	}

	template <typename FN, typename CB, typename = checkCBVoid_t<FN>, typename = checkCBVoid_t<CB>>
	static void concurrence(const FN& fn, const CB& cbThread)
	{
		std::thread thr(cbThread);

		fn();
		thr.join();
	}

        template <typename FN, typename CB, typename RET = decltype(declval<FN>()())
                  , typename = checkCBNotVoid_t<FN>, typename = checkCBVoid_t<CB>>
	static RET concurrence(const FN& fn, const CB& cbThread)
	{
		std::thread thr(cbThread);
		
		RET ret = fn();
		thr.join();

		return ret;
	}

        template <typename FN, typename CB, typename RET = decltype(declval<CB>()())
                  , typename = checkCBVoid_t<FN>, typename = checkCBNotVoid_t<CB>, typename=void>
	static RET concurrence(const FN& fn, const CB& cbThread)
	{
		RET ret;
                std::thread thr([&]{
                    ret = cbThread();
		});

		fn();
		thr.join();

		return ret;
    }
};


#include "mtlock.h"

class __UtilExt XThread
{
public:
    XThread()
        : m_bRunSignal(m_runSignal)
    {
    }

    virtual ~XThread()
    {
        cancel();
    }

private:
    thread m_thread;
    mutex m_mutex;

    CSignal<false> m_runSignal;
    signal_t m_bRunSignal;

private:
    inline void _reset()
    {
        if (!m_bRunSignal)
        {
            return;
        }
        m_runSignal.reset();
    }

public:
    CSignal<false>& runSignal()
    {
        return m_runSignal;
    }

    signal_t signal() const
    {
        return m_bRunSignal;
    }
    operator signal_t() const
    {
        return m_bRunSignal;
    }

    bool operator !() const
    {
        return !m_bRunSignal;
    }

    inline signal_t usleep(UINT uMs)
    {
        if (m_bRunSignal)
        {
            (void)m_runSignal.wait_false(uMs);
        }

        return m_bRunSignal;
    }

    void start(cfn_void cb)
    {
        m_mutex.lock();

        m_thread = thread([=]{
            m_runSignal.set();

            m_mutex.lock(); //__usleep(1); // 等待start函数返回，防止？？
            m_mutex.unlock();

            cb();

            _reset();
        });

        m_runSignal.wait();

        m_mutex.unlock();
    }    

    void start(cfn_void_t<signal_t> cb)
    {
        start([=]{
            cb(m_bRunSignal);
        });
    }

    void start(UINT uMsLoop, cfn_bool cb)
    {
        start([=]{
            do {
                if (!cb())
                {
                    _reset();
                    break;
                }
            } while (this->usleep(uMsLoop));
        });
    }

    void start(UINT uMsLoop, cfn_bool_t<signal_t> cb)
    {
        start(uMsLoop, [=]{
            return cb(m_bRunSignal);
        });
    }

    inline bool joinable() const
    {
        return m_thread.joinable();
    }

    void detach()
    {
        if (!joinable())
        {
            return;
        }

        m_mutex.lock();
        if (joinable())
        {
            m_thread.detach();
        }
        m_mutex.unlock();
    }

    void join()
    {
        if (!joinable())
        {
            return;
        }

        if (m_bRunSignal)
        {
            (void)m_runSignal.wait_false();
        }

        m_mutex.lock();
        if (joinable())
        {
            m_thread.join();
        }
        m_mutex.unlock();
    }

    void cancel(bool bJoin = true)
    {
        if (!joinable())
        {
            return;
        }

        m_mutex.lock();
        if (joinable())
        {
            _reset();

            if (bJoin)
            {
                m_thread.join();
            }
        }
        m_mutex.unlock();
    }
};


class __UtilExt cbthread
{
public:
    cbthread() = default;

private:
    mutex m_mtx;
    condition_variable m_cv;
    thread m_thr;

    bool m_bRunSignal = true;

    bool m_bReset = false;

    function<void(signal_t, const bool& bReset)> m_cb;

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

    bool try_emit(const function<void(signal_t, const bool& bReset)>& cb)
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

    bool try_emit(cfn_void_t<signal_t> cb)
    {
        return try_emit([=](signal_t bRunSignal, const bool& bReset){
            (void)bReset;
            cb(bRunSignal);
        });
    }

    bool try_emit(cfn_void cb)
    {
        return try_emit([=](signal_t bRunSignal, const bool& bReset){
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


class __UtilExt CThreadGroup
{
public:
    CThreadGroup()
        //: m_CancelEvent(TRUE)
    {
    }

private:
    vector<BOOL> m_vecThreadStatus;

    volatile bool m_bPause = false;

    bool m_bRunSignal = false;

public:
    signal_t runSignal() const
	{
		return m_bRunSignal;
	}

    using CB_WorkThread = function<void(UINT uThreadIndex)>;
    void start(UINT uThreadCount, const CB_WorkThread& cb, bool bBlock);

    bool checkStatus();

protected:
    void pause(bool bPause = true);

    void cancel();

    UINT getActiveCount();

public:
    template <typename T, typename R>
	using CB_SubTask = const function<bool(UINT uTaskIdx, T&, R&)>&;
	
	template <typename T, typename R>
	static void startMultiTask(ArrList<T>& alTask, vector<R>& vecResult, UINT uThreadCount, CB_SubTask<T, R> cb)
	{
		uThreadCount = MAX(1, uThreadCount);
		vecResult.resize(uThreadCount);

		CThreadGroup ThreadGroup;
		ThreadGroup.start(uThreadCount, [&](UINT uThreadIndex) {
			bool bCancelFlag = false;
			for (UINT uTaskIdx = uThreadIndex; uTaskIdx < alTask.size(); uTaskIdx += uThreadCount)
			{
				alTask.get(uTaskIdx, [&](T& task) {
					if (!cb(uTaskIdx, task, vecResult[uThreadIndex]))
					{
						bCancelFlag = true;
					}
				});
				if (bCancelFlag)
				{
					break;
				}
			}
		}, true);
	}

	template <typename T>
	static void startMultiTask(ArrList<T>& alTask, UINT uThreadCount, const function<bool(UINT uTaskIdx, T&)>& cb)
	{
		vector<BOOL> vecResult;
        startMultiTask(alTask, vecResult, uThreadCount, [&](UINT uTaskIdx, T& task, BOOL&) {
			return cb(uTaskIdx, task);
		});
	}
};


template <typename T, typename R=BOOL>
class CMultiTask
{
public:
    CMultiTask() = default;

private:
	vector<R> m_vecResult;

public:
	using CB_SubTask = CThreadGroup::CB_SubTask<T,R>;

	vector<R>& start(ArrList<T>& alTask, UINT uThreadCount, CB_SubTask cb)
	{
		CThreadGroup::startMultiTask(alTask, m_vecResult, uThreadCount, cb);
		return m_vecResult;
	}
};
