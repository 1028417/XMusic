#pragma once

#if !__winvc
enum class E_LoginReult
{
	LR_Success,
	LR_NetworkError,
	LR_UserInvalid,
	LR_ProfileInvalid,
	LR_PwdWrong,
	LR_MutiLogin
};
#endif

#define UA_MutiDev 1

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
	E_LoginReult _login(signal_t bRunSignal, const CByteBuffer& bbfProfile, cwstr strUser, const string& strPwd);

    void _relogin(signal_t bRunSignal, cwstr strUser, const string& strPwd, cfn_void_t<E_LoginReult> cb);
#endif

public:
    void init();

#if __winvc
    bool signupUser(cwstr strUser, const string& strPwd, uint64_t tVip, UINT uAuth);

    bool removeUser(cwstr strUser);

    UINT deployUser();
#else

    bool asyncLogin(signal_t bRunSignal, cwstr strUser, const string& strPwd, cfn_void_t<E_LoginReult> cb);
#endif
};
