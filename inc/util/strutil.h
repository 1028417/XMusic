
#pragma once

#include <string>

#if !__winvc
#include <QString>

#define to_string(x) QString::number(x).toStdString()
#define to_wstring(x) QString::number(x).toStdWString()
#endif

class __UtilExt strutil
{
public:
	static const wchar_t wcSpace = L' ';

	static void trim(wstring& strText, wchar_t chr = ' ');
	static void trim(string& strText, char chr = ' ');
	static wstring trim_r(const wstring& strText, wchar_t chr = ' ');
	static string trim_r(const string& strText, char chr = ' ');

	static void ltrim(wstring& strText, wchar_t chr = ' ');
	static void ltrim(string& strText, char chr = ' ');
	static wstring ltrim_r(const wstring& strText, wchar_t chr = ' ');
	static string ltrim_r(const string& strText, char chr = ' ');

	static void rtrim(wstring& strText, wchar_t chr = ' ');
	static void rtrim(string& strText, char chr = ' ');
	static wstring rtrim_r(const wstring& strText, wchar_t chr = ' ');
	static string rtrim_r(const string& strText, char chr = ' ');

	static void split(const wstring& strText, wchar_t wcSplitor, vector<wstring>& vecRet, bool bTrim = false);
	static void split(const string& strText, char wcSplitor, vector<string>& vecRet, bool bTrim = false);

	static int collate(const wstring& lhs, const wstring& rhs);
	static bool matchIgnoreCase(const wstring& str1, const wstring& str2);

	static void lowerCase(wstring& str);
	static void lowerCase(string& str);
	static wstring lowerCase_r(const wstring& str);
	static string lowerCase_r(const string& str);

	static void upperCase(wstring& str);
	static void upperCase(string& str);
	static wstring upperCase_r(const wstring& str);
	static string upperCase_r(const string& str);

	static void replaceChar(wstring& str, wchar_t chrFind, wchar_t chrReplace);
	static void replaceChars(wstring& str, const wstring& strFindChars, wchar_t chrReplace);

	static wstring replaceChar_r(const wstring& str, wchar_t chrFind, wchar_t chrReplace);
	static wstring replaceChars_r(const wstring& str, const wstring& strFindChars, wchar_t chrReplace);

	static wstring utf8ToWstr(const string& str);

	static string wstrToUTF8(const wstring& str);
	static string wstrToUTF8(const wchar_t *pStr);

    static wstring strToWstr(const string& str, bool bCheckUTF8=false);
    static wstring strToWstr(const char *pStr, bool bCheckUTF8=false);

	static string wstrToStr(const wstring& str);
	static string wstrToStr(const wchar_t *pStr);

#if !__winvc
	static QString wstrToQStr(const wstring& str)
	{
		return QString::fromStdWString(str);
	}

	static QString strToQStr(const string& str)
	{
		return QString::fromStdString(str);
	}
#endif

	template <typename T>
	static wstring ContainerToStr(const T& container, const wstring& strSplitor)
	{
		wstringstream ssResult;
        for (typename T::const_iterator it = container.begin(); ; )
		{
			ssResult << *it;
			
			it++;
			__EnsureBreak(it != container.end());

			ssResult << strSplitor;
		}

		return ssResult.str();
	}
	
	struct tagCNSortor
	{
		bool operator()(const wstring& lhs, const wstring& rhs) const
		{
			return strutil::collate(lhs, rhs)<0;
		}
	};
};

class WString : public wstring
{
public:
    WString() {}

    template <typename T>
    WString(const T& t)
    {
        *this << t;
    }

    WString(const WString& other)
        : wstring(other)
    {
    }

    WString(WString&& other)
    {
		wstring::swap(other);
    }

    WString(wstring&& str)
    {
		wstring::swap(str);
    }

    WString& operator =(WString&& other)
    {
		wstring::swap(other);
        return *this;
    }

    WString& operator =(wstring&& str)
    {
		wstring::swap(str);
        return *this;
    }

    template <typename T>
    WString& operator =(const T& t)
    {
		wstring::clear();
        *this << t;
        return *this;
    }
	
public:
#if !__winvc
    operator QString() const
    {
        return strutil::wstrToQStr(*this);
    }

    QString qstr() const
    {
        return strutil::wstrToQStr(*this);
    }
#endif
	
    WString& operator<<(const WString& other)
    {
        wstring::append(other);
        return *this;
    }

    WString& operator<<(const wchar_t *pStr)
    {
        if (pStr)
        {
            wstring::append(pStr);
        }
        return *this;
    }

	WString& operator<<(wchar_t wc)
	{
		wstring::append(1, wc);
		return *this;
	}

	WString& operator<<(char c)
	{
		return *this << (wchar_t)c;
	}

    WString& operator<<(const wstring& str)
    {
		wstring::append(str);
        return *this;
    }

    template <typename T>
    WString& operator<<(const T& t)
    {
		wstring::append(to_wstring(t));
        return *this;
    }

    template <typename T>
    WString& append(const T& t)
    {
        *this << t;
        return *this;
    }

    template <typename T>
    WString operator +(const T& t) const
    {
        WString ret(*this);
        ret << t;
        return ret;
    }
};
