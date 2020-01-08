
#pragma once

#if !__windows
#define _time32 time
#endif

using time32_t = long;
using time64_t = int64_t;

struct tagTM : tm
{
	tagTM()
	{
	}

	tagTM& operator = (const tm& _tm)
	{
		memcpy(this, &_tm, sizeof(_tm));

		tm_year += 1900;     // tm_year is 1900 based
		tm_mon += 1;        // tm_mon is 0 based

		return *this;
	}
};

class __UtilExt tmutil
{
public:
	static void getCurrentTime(int& nHour, int& nMinute);

	static bool timeToTM(time32_t tTime, tagTM& tm);

	static wstring formatTime(const wstring& strFormat, time32_t tTime = -1);

#if __windows
	static bool time64ToTM(time64_t tTime, tagTM& tm);

	static wstring formatTime64(const wstring& strFormat, time64_t tTime = -1);
	static string formatTime64(const string& strFormat, time64_t tTime = -1);

	static time64_t transFileTime(unsigned long dwLowDateTime, unsigned long dwHighDateTime);
#endif
};
