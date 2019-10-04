#pragma once

#include "json/json.h"

using JValue = Json::Value;
using JReader = Json::Reader;
using JWriter = Json::StyledWriter;

class __UtilExt jsonutil
{
public:
    static bool load(const wstring& strFile, JValue& jRoot, bool bStrictRoot=true);

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
