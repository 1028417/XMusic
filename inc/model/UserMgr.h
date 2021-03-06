#pragma once

#define UA_MultiDev 1
#define UA_Xpk		2

#if !__winvc
enum class E_LoginReult
{
    LR_Success,
    LR_NetworkError,
    LR_ResponseError,
    LR_UserNotExist,
	LR_ProfileInvalid,
	LR_PwdWrong,
	LR_MultiLogin
};
#endif

class __ModelExt CUserMgr
{
public:
    CUserMgr(CModel& model)
        : m_model(model)
    {
    }

private:
    CModel& m_model;

private:
#if !__winvc
    template <typename T>
    E_LoginReult _checkProfile(const T& buffer, cwstr strUser, const string& strPwd);

    template <typename T>
    E_LoginReult _login(signal_t bRunSignal, cwstr strUser, const string& strPwd, int nRet, const T& bfProfile);
#endif

public:
#if __winvc
    void init();

	bool qurryUser(cwstr strUser, string& strPwd, uint64_t& tVip, UINT& uAuth);

    bool signupUser(cwstr strUser, const string& strPwd, uint64_t tVip, UINT uAuth);

    bool removeUser(cwstr strUser);

    UINT deployUser();

#else
    wstring loadProfile(string& strPwd);

    E_LoginReult syncLogin(signal_t bRunSignal, cwstr strUser, const string& strPwd);

    void asyncLogin(signal_t bRunSignal, cwstr strUser, const string& strPwd, cfn_void_t<E_LoginReult> cb);

    bool isVip() const;
#endif
};
