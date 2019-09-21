#pragma once

#include <thread>

//#include <future>

class __UtilExt mtutil
{
#if __windows
public:
	static bool apcWakeup(HANDLE hThread, const fn_void& fn = NULL)
	{
		return 0 != QueueUserAPC(APCFunc, hThread, fn ? (ULONG_PTR)&fn : 0);
	}

	static bool apcWakeup(DWORD dwThreadID, const fn_void& fn = NULL)
	{
		HANDLE hThread = OpenThread(PROCESS_ALL_ACCESS, FALSE, dwThreadID);
		return apcWakeup(hThread, fn);
	}

	static bool poolStart(const fn_void& fn)
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
	static void thread(const fn_void& cb)
	{
		std::thread(cb).detach();
	}

	template <typename FN, typename RET=decltype(declval<FN>()())>
	static RET thread(const FN& fn, const fn_void& cbThread)
	{
		std::thread thr(cbThread);
		
		RET ret = fn();

		thr.join();

		return ret;
	}

	template <typename FN, typename = checkCBVoid_t<FN>>
	static void thread(const FN& fn, const fn_void& cbThread)
	{
		std::thread thr(cbThread);

		fn();

		thr.join();
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

	bool m_bCancelEvent = false;

public:
	using CB_WorkThread = function<void(UINT uThreadIndex)>;
	void start(UINT uThreadCount, const CB_WorkThread& cb, bool bBlock);

	bool checkCancel();

protected:
	void pause(bool bPause = true);

	void cancel();

	UINT getActiveCount();
};

template <typename T, typename R>
class CMultiTask
{
public:
	CMultiTask()
	{
	}

private:
	vector<R> m_vecResult;

public:
	using CB_SubTask = const function<bool(UINT uTaskIdx, T&, R&)>&;
	static void start(ArrList<T>& alTask, vector<R>& vecResult, UINT uThreadCount, CB_SubTask cb)
	{
		if (0 == uThreadCount)
		{
			uThreadCount = 1;
		}

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

	static void start(ArrList<T>& alTask, UINT uThreadCount, const function<bool(UINT uTaskIdx, T&)>& cb)
	{
		vector<R> vecResult;
		start(alTask, vecResult, uThreadCount, [&](UINT uTaskIdx, T& task, R&) {
			return cb(uTaskIdx, task);
		});
	}

	vector<R>& start(ArrList<T>& alTask, UINT uThreadCount, CB_SubTask cb)
	{
		start(alTask, m_vecResult, uThreadCount, [&](UINT uTaskIdx, T& task, R& result) {
			return cb(uTaskIdx, task, result);
		});

		return m_vecResult;
	}
};

#include "mtlock.h"
