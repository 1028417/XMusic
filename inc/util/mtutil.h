#pragma once

#include <thread>

//#include <future>

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
		std::thread thr([&]() {
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
                std::thread thr([&]() {
                    ret = cbThread();
		});

		fn();
		thr.join();

		return ret;
	}

	inline static void usleep(UINT uMS = 0)
	{
		std::this_thread::sleep_for(chrono::milliseconds(uMS));
	}

	inline static void yield()
    {
        std::this_thread::yield();
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
    const bool& runSignal() const
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


#include "mtlock.h"
using XT_RunSignal = const bool&;

class __UtilExt XThread
{
public:
    XThread() : m_sgnRuning(false)
    {
    }

    virtual ~XThread()
    {
        cancel();
    }

private:
    mutex m_mutex;
    CSignal m_sgnRuning;
    thread m_thread;

public:
    void start(cfn_void cb, UINT uMsLoop=0)
    {
        m_mutex.lock();

        m_thread = thread([=]() {
            m_sgnRuning.set();
            //mtutil::usleep(1);
            m_mutex.lock();
            m_mutex.unlock();

            while (true)
            {
                cb();
                if (0 == uMsLoop)
                {
                    break;
                }

                if (!this->usleepex(uMsLoop))
                {
                    return;
                }
            }

            if (m_sgnRuning)
            {
                m_sgnRuning.reset();
            }
        });

        m_sgnRuning.wait();

        m_mutex.unlock();
    }

    void start(cfn_void_t<XT_RunSignal> cb, UINT uMsLoop=0)
    {
        start([&, cb]() {
            cb(m_sgnRuning);
        }, uMsLoop);
    }

    void start(UINT uMsLoop, cfn_bool cb)
    {
        start([&, cb](){
            if (!cb())
            {
                if (m_sgnRuning)
                {
                    m_sgnRuning.reset();
                }
            }
        }, uMsLoop);
    }

    void start(UINT uMsLoop, cfn_bool_t<XT_RunSignal> cb)
    {
        start([&, cb](){
            if (!cb(m_sgnRuning))
            {
                if (m_sgnRuning)
                {
                    m_sgnRuning.reset();
                }
            }
        }, uMsLoop);
    }

    XT_RunSignal runSignal() const
    {
        return m_sgnRuning.value();
    }

    bool usleepex(UINT uMs)
    {
        if (!m_sgnRuning)
        {
            return false;
        }

        (void)((TSignal<bool>&)m_sgnRuning).wait(uMs, [](bool bValue) {
            return false == bValue;
        });

        return m_sgnRuning;
    }

    void detach()
    {
        if (m_thread.joinable())
        {
            m_mutex.lock();
            m_thread.detach();
            m_mutex.unlock();
        }
    }

    bool joinable() const
    {
        return m_thread.joinable();
    }

    void join()
    {
        if (!m_thread.joinable())
        {
            return;
        }

        m_mutex.lock();
        if (m_thread.joinable())
        {
            m_thread.join();
        }
        m_mutex.unlock();
    }

    void cancel(bool bJoin = true)
    {
        if (!m_thread.joinable())
        {
            return;
        }

        m_mutex.lock();
        if (m_thread.joinable())
        {
            if (m_sgnRuning)
            {
                m_sgnRuning.reset();
            }

            if (bJoin)
            {
                m_thread.join();
            }
        }
        m_mutex.unlock();
    }
};
