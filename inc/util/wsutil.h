
#pragma once

#include <string>

#if !__winvc
#include <QString>

#define to_string(x) QString::number(x).toStdString()
#define to_wstring(x) QString::number(x).toStdWString()
#endif

class __UtilExt wsutil
{
public:
	static const wchar_t wcSpace = L' ';

	static bool checkWChar(const wstring& str);

	static void trim(wstring& strText, wchar_t chr = ' ');
	static wstring trim_r(const wstring& strText, wchar_t chr = ' ');

	static void ltrim(wstring& strText, wchar_t chr = ' ');
	static wstring ltrim_r(const wstring& strText, wchar_t chr = ' ');

	static void rtrim(wstring& strText, wchar_t chr = ' ');
	static wstring rtrim_r(const wstring& strText, wchar_t chr = ' ');
	
	static void split(const wstring& strText, wchar_t wcSplitor, vector<wstring>& vecRet, bool bTrim=false);

	static int collate(const wstring& lhs, const wstring& rhs);
	static bool matchIgnoreCase(const wstring& str1, const wstring& str2);
	
	static void lowerCase(wstring& str);
	static wstring lowerCase_r(const wstring& str);
	
	static void upperCase(wstring& str);
	static wstring upperCase_r(const wstring& str);

	static void replaceChar(wstring& str, wchar_t chrFind, wchar_t chrReplace);
	static void replaceChars(wstring& str, const wstring& strFindChars, wchar_t chrReplace);

	static wstring replaceChar_r(const wstring& str, wchar_t chrFind, wchar_t chrReplace);
	static wstring replaceChars_r(const wstring& str, const wstring& strFindChars, wchar_t chrReplace);

	static wstring fromUTF8(const string& str);

	static string toUTF8(const wstring& str);
	static string toUTF8(const wchar_t *pStr);

    static wstring fromStr(const string& str, bool bCheckUTF8=false);
    static wstring fromStr(const char *pStr, bool bCheckUTF8=false);

	static string toStr(const wstring& str);
	static string toStr(const wchar_t *pStr);

#if !__winvc
    static QString toQStr(const wstring& str)
    {
        return QString::fromStdWString(str);
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
			return wsutil::collate(lhs, rhs)<0;
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
        return wsutil::toQStr(*this);
    }

    QString qstr() const
    {
        return wsutil::toQStr(*this);
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
