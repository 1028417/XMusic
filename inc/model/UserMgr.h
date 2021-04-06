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

class __ModelExt CUserMgr
{
public:
    CUserMgr() = default;

public:
#if !__winvc
    void init();

	bool asyncLogin(signal_t bRunSignal, cwstr strUser, const string& strPwd, cfn_void_t<E_LoginReult> cb);
#endif

private:
#if !__winvc
	E_LoginReult _login(signal_t bRunSignal, const CByteBuffer& bbfProfile, cwstr strUser, const string& strPwd);
#endif
};
