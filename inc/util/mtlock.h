#pragma once

#include <mutex>
#include <condition_variable>

#include <chrono>

using mutex_lock = std::unique_lock<mutex>;

template <typename T>
class CMtxLock : public mutex
{
public:
    CMtxLock() {}

    T m_data;

public:
    T& lock()
    {
        mutex::lock();

        return m_data;
    }

    void lock(const function<void(T& data)>& cb)
    {
        mutex::lock();

        cb(m_data);

        mutex::unlock();
    }

    void get(T& data)
    {
        mutex::lock();

        data = m_data;

        mutex::unlock();
    }
};

template <typename T>
class TSignal
{
public:
    TSignal() {}

    TSignal(const T& value) :
        m_value(value)
    {
    }

private:
    mutex m_mutex;
    condition_variable m_condition;

protected:
    T m_value;

private:
    using CB_CheckSignal = const function<bool(const T& value)>&;
    inline bool _wait(CB_CheckSignal cbCheck, int nMs = -1)
    {
        mutex_lock lock(m_mutex);

        if (nMs >= 0)
        {
            if (!cbCheck(m_value))
            {
                if (cv_status::timeout == m_condition.wait_for(lock, std::chrono::milliseconds(nMs)))
                {
                    return false;
                }

                if (!cbCheck(m_value))
                {
                    return false;
                }
            }
        }
        else
        {
            while (!cbCheck(m_value))
            {
                m_condition.wait(lock);
            }
        }

        return true;
    }

public:
    const T& value() const
    {
        return m_value;
    }

    T wait(CB_CheckSignal cbCheck)
    {
        T ret;
		memzero(ret);

		(void)_wait([&](const T& value) {
			ret = value;
			return cbCheck(value);
		});

        return ret;
    }

    bool wait_for(UINT uMs, CB_CheckSignal cbCheck)
    {
        return _wait(cbCheck, uMs);
    }

    void set(const T& value)
    {
        mutex_lock lock(m_mutex);
        
        m_value = value;
        
        m_condition.notify_one();
    }
};

class CSignal : public TSignal<bool>
{
public:
    CSignal(bool bInitValue)
        : TSignal(bInitValue)
    {
    }

public:
    const bool& value() const
    {
        return m_value;
    }

    void wait(bool bReset = false)
    {
        TSignal::wait([=](bool bValue) {
            if (bValue)
            {
                if (bReset)
                {
                    m_value = false;
                }

                return true;
            }

            return false;
        });
    }

    bool wait_for(UINT uMs, bool bReset = false)
    {
        return TSignal::wait_for(uMs, [=](bool bValue) {
           if (bValue)
           {
               if (bReset)
               {
                   m_value = false;
               }

               return true;
           }

           return false;
        });
    }

    void set()
    {
        TSignal::set(true);
    }

    void reset()
    {
        TSignal::set(false);
    }
};

#if __windows
class __UtilExt CWinEvent
{
public:
	CWinEvent(BOOL bManualReset=FALSE, BOOL bInitialState=FALSE)
	{
		m_hEvent = ::CreateEvent(NULL, bManualReset, bInitialState, NULL);
	}

	~CWinEvent()
	{
		(void)::CloseHandle(m_hEvent);
	}

private:
	HANDLE m_hEvent = INVALID_HANDLE_VALUE;

public:
	bool wait(DWORD dwTimeout = INFINITE)
	{
		return WAIT_OBJECT_0 == ::WaitForSingleObject(m_hEvent, dwTimeout);
	}

	bool check()
	{
		return wait(0);
	}

	bool notify()
	{
		return TRUE == ::SetEvent(m_hEvent);
	}

	bool reset()
	{
		return TRUE == ::ResetEvent(m_hEvent);
	}
};

class __UtilExt CCSLock
{
public:
	CCSLock()
	{
		InitializeCriticalSection(&m_cs);
	}

	~CCSLock()
	{
		DeleteCriticalSection(&m_cs);
	}

private:
	CRITICAL_SECTION m_cs;

public:
	void lock()
	{
		EnterCriticalSection(&m_cs);
	}

	void unlock()
	{
		LeaveCriticalSection(&m_cs);
	}
};

class __UtilExt CCASLock
{
public:
	CCASLock()
	{
	}

private:
#if __winvc
	volatile char m_lockFlag = 0;

#pragma intrinsic(_InterlockedCompareExchange8, _InterlockedExchange8)
#define __CompareExchange _InterlockedCompareExchange8
#else
	volatile long m_lockFlag = 0;
#define	__CompareExchange InterlockedCompareExchange
#endif

	bool _lock(UINT uRetryTimes = 0, UINT uSleepTime = 0)
	{
		while (__CompareExchange(&m_lockFlag, 1, 0))
		{
			if (0 != uRetryTimes && 0 == --uRetryTimes)
			{
				return false;
			}

			if (0 == uSleepTime)
			{
				mtutil::yield();
			}
			else
			{
				mtutil::usleep(uSleepTime);
			}
		}

		return true;
	}

public:
	bool try_lock(UINT uRetryTimes = 1)
	{
		return _lock(uRetryTimes, 0);
	}

	void lock(UINT uSleepTime = 0)
	{
		_lock(0, uSleepTime);
	}

	void unlock()
	{
		//(void)__CompareExchange(&m_lockFlag, 0);
		m_lockFlag = 0;
	}
};
#endif
