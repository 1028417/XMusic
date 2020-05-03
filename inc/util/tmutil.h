
#pragma once

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

using time32_t = long;
using time64_t = int64_t;

class __UtilExt tmutil
{
public:
	static void getCurrentTime(int& nHour, int& nMinute);

	static bool timeToTM(time_t tTime, tagTM& tm);

    static wstring formatTime(cwstr strFormat, time32_t tTime);
    static string formatTime(const string& strFormat, time32_t tTime);

#if __windows
	static wstring formatTime(cwstr strFormat, time64_t tTime);
	static string formatTime(const string& strFormat, time64_t tTime);

	static time64_t transFileTime(unsigned long dwLowDateTime, unsigned long dwHighDateTime);
#endif
};
