#pragma once

#include <mutex>
#include <condition_variable>

#include <chrono>

using mutex_lock = std::unique_lock<mutex>;

template <typename T>
class TMutexData
{
public:
    TMutexData(T& data)
        : m_data(data)
    {
    }

private:
    T m_data;
    mutex m_mutex;

public:
    T& lock()
    {
        m_mutex.lock();

        return m_data;
    }

    void lock(const function<void(T& data)>& cb)
    {
        m_mutex.lock();

        cb(m_data);

        m_mutex.unlock();
    }

    void get(T& data)
    {
        m_mutex.lock();

        data = m_data;

        m_mutex.unlock();
    }

    void set(const T& data)
    {
        m_mutex.lock();

        m_data = data;

        m_mutex.unlock();
    }
};

template <typename T>
class CMutexData : public TMutexData<T>
{
public:
    CMutexData()
        : TMutexData<T>(m_data)
    {
    }

private:
    T m_data;
};

template <typename T>
class TSignal
{
public:
    TSignal() = default;

    TSignal(const T& value) :
        m_value(value)
    {
    }

private:
    mutex m_mutex;
    condition_variable m_condition;
    bool m_bStop = false;

protected:
    T m_value;

private:
    using CB_CheckSignal = const function<bool(T& value)>&;
    inline bool _wait(CB_CheckSignal cbCheck, int nMs = -1)
    {
        if (nMs >= 0)
        {
            mutex_lock lock(m_mutex);
            if (m_bStop)
            {
                return false;
            }

            if (cbCheck(m_value))
            {
                return true;
            }

            if (cv_status::timeout == m_condition.wait_for(lock, std::chrono::milliseconds(nMs)))
            {
                return false;
            }
            if (m_bStop)
            {
                return false;
            }

            return cbCheck(m_value);
        }
        else
        {
            mutex_lock lock(m_mutex);
            while (!m_bStop)
            {
                if (cbCheck(m_value))
                {
                    return true;
                }

                m_condition.wait(lock);
            }
            return false;
        }
    }

public:
    bool wait(CB_CheckSignal cbCheck)
    {
        return _wait(cbCheck);
    }

    bool wait(UINT uMs, CB_CheckSignal cbCheck)
    {
        return _wait(cbCheck, uMs);
    }

	void set(const T& value)
	{
		mutex_lock lock(m_mutex);
		m_bStop = false;
		m_value = value;
		m_condition.notify_one();
	}

    void set(const T& value, cfn_void fn)
    {
        mutex_lock lock(m_mutex);
		m_bStop = false;
        m_value = value;
		fn();
        m_condition.notify_one();
    }

    void stop()
    {
        mutex_lock lock(m_mutex);
		m_bStop = true;
        m_condition.notify_one();
    }
};

class CSignal : public TSignal<bool>
{
public:
    CSignal(bool bInitValue, bool bAutoReset)
        : TSignal(bInitValue)
		, m_bAutoReset(bAutoReset)
    {
    }

private:
	bool m_bAutoReset;

public:
	bool operator !() const
	{
		return !m_value;
	}

	operator const bool&() const
	{
		return m_value;
	}

    const bool& value() const
    {
        return m_value;
    }

    bool wait(cfn_void cb=NULL)
    {
		return TSignal::wait([=](bool bValue) {
            if (bValue)
            {
                if (m_bAutoReset)
                {
                    m_value = false;
                }

				if (cb)
				{
					cb();
				}

                return true;
            }

            return false;
        });
    }

    bool wait(UINT uMs, cfn_void cb = NULL)
    {
        return TSignal::wait(uMs, [=](bool bValue) {
           if (bValue)
           {
               if (m_bAutoReset)
               {
                   m_value = false;
               }

			   if (cb)
			   {
				   cb();
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

	void set(cfn_void fn)
	{
		TSignal::set(true, fn);
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
