#pragma once

#include "json/json.h"

using JValue = Json::Value;
using JReader = Json::Reader;

class __UtilExt jsonutil
{
public:
	static bool loadData(const string& strData, JValue& jRoot, bool bStrictRoot = true)
    {
        if (bStrictRoot)
        {
            return JReader(Json::Features::strictMode()).parse(strData, jRoot, false);
        }
        else
        {
            return JReader().parse(strData, jRoot, false);
        }
	}

	static bool loadStream(Instream& ins, JValue& jRoot, bool bStrictRoot = true)
	{
		string strText;
        CTxtReader::read(ins, strText);

		return loadData(strText, jRoot, bStrictRoot);
	}

	template <typename T>
	static bool loadFile(const T& file, JValue& jRoot, bool bStrictRoot = true)
	{
		IFStream ifs(file);
		__EnsureReturn(ifs, false);
		return loadStream(ifs, jRoot, bStrictRoot);
	}

    static string toStr(const JValue& jRoot, bool bStyled=false)
    {
        if (bStyled)
        {
            return Json::StyledWriter().write(jRoot);
        }
        else
        {
            return Json::FastWriter().write(jRoot);
        }
    }

	template <typename T>
	static bool writeFile(const JValue& jRoot, const T& file, bool bStyled=false)
	{
		CUTF8TxtWriter TxtWriter(false);
		if (!TxtWriter.open(file, true))
		{
			return false;
		}

        return TxtWriter.write(toStr(jRoot, bStyled));
	}

    static bool get(const JValue& jValue, string& strRet);

    static bool get(const JValue& jValue, wstring& strRet);
    static bool get(const JValue& jValue, bool bUtf8, wstring& strRet);

    static bool get(const JValue& jValue, bool& bRet);

    static bool get(const JValue& jValue, int& nRet);
    static bool get(const JValue& jValue, unsigned int& uRet);
    static bool getHex(const JValue& jValue, unsigned int& uRet);

#ifdef JSON_HAS_INT64
    static bool get(const JValue& jValue, JValue::Int64& nRet);
    static bool get(const JValue& jValue, JValue::UInt64& uRet);
#endif

    template <typename T>
    static bool getArray(const JValue& jValue, list<T>& lstRet)
    {
        if (!jValue.isArray())
        {
            return false;
        }

        T value;
        for (UINT uIdx = 0; uIdx < jValue.size(); uIdx++)
        {
            if (get(jValue[uIdx], value))
            {
                lstRet.push_back(value);
            }
        }

        return true;
    }
};
