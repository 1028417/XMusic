#pragma once

#include "json/json.h"

using JValue = Json::Value;
using JReader = Json::Reader;
using JWriter = Json::StyledWriter;

class __UtilExt jsonutil
{
public:
	static bool loadData(const string& strData, JValue& jRoot, bool bStrictRoot = true)
	{
		Json::Features features;
		features.strictRoot_ = bStrictRoot;
		return JReader(features).parse(strData, jRoot, false);
	}

	static bool loadFile(Instream& ins, JValue& jRoot, bool bStrictRoot = true)
	{
		string strText;
        CTxtReader().read(ins, strText);

		return loadData(strText, jRoot, bStrictRoot);
	}
	static bool loadFile(const wstring& strFile, JValue& jRoot, bool bStrictRoot = true)
	{
		IFStream ifs(strFile);
		__EnsureReturn(ifs, false);
		return loadFile(ifs, jRoot, bStrictRoot);
	}
	
    static bool get(const JValue& jValue, wstring& strRet);

    static bool get(const JValue& jValue, string& strRet);

    static bool get(const JValue& jValue, bool& bRet);

    static bool get(const JValue& jValue, int& nRet);

    static bool get(const JValue& jValue, UINT& uRet);

    template <typename T>
    static bool getArray(const JValue& jValue, vector<T>& vecRet)
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
                vecRet.push_back(value);
            }
        }

        return true;
    }
};
