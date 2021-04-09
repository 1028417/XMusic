#pragma once

#define UA_MultiLogin 1

#if !__winvc
enum class E_LoginReult
{
	LR_Success,
	LR_NetworkError,
	LR_UserInvalid,
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

#if !__winvc
    wstring m_strUser;
    string m_strPwd;

    template <typename T>
    E_LoginReult _login(signal_t bRunSignal, cwstr strUser, const string& strPwd, int nRet, const T& bfProfile);
#endif

public:
    void init();

#if __winvc
	bool qurryUser(cwstr strUser, string& strPwd, uint64_t& tVip, UINT& uAuth);

    bool signupUser(cwstr strUser, const string& strPwd, uint64_t tVip, UINT uAuth);

    bool removeUser(cwstr strUser);

    UINT deployUser();

#else
    cwstr user() const
    {
        return m_strUser;
    }

    const string& pwd() const
    {
        return m_strPwd;
    }

    E_LoginReult syncLogin(signal_t bRunSignal, cwstr strUser, const string& strPwd);
    void asyncLogin(signal_t bRunSignal, cwstr strUser, const string& strPwd, cfn_void_t<E_LoginReult> cb);
#endif
};
