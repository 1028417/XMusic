#pragma once

#include "json/json.h"

using JValue = Json::Value;

class __UtilExt jsonutil
{
public:
    static bool load(const wstring& strFile, JValue& jRoot, bool bStrictRoot=true);

    static bool get(const JValue& jValue, wstring& strRet);

    static bool get(const JValue& jValue, string& strRet);

    static bool get(const JValue& jValue, bool& bRet);

    static bool get(const JValue& jValue, int& nRet);

    static bool get(const JValue& jValue, UINT& uRet);
};
