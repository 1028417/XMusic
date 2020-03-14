#pragma once

template <UINT _ResID=0, bool _CanOK = false, bool _CanCancel = true>
class TDialog : public CDialog
{
public:
	TDialog() : CDialog(_ResID)
	{
	}

private:
	CCompatableFont m_font;

private:
	void OnOK() override
	{
		if (_CanOK)
		{
			OnClose(false);
		}
	}

	void OnCancel() override
	{
		if (_CanCancel)
		{
			OnClose(true);
		}
	}
	
protected:
	virtual void PreSubclassWindow() override
	{
		__super::PreSubclassWindow();

		m_font.setFont(*this);
	}

	virtual void OnClose(bool bCancel)
	{
		if (bCancel)
		{
			__super::OnCancel();
		}
		else
		{
			__super::OnOK();
		}
	}

public:
	int msgBox(const wstring& strMsg, const wstring& strTitle, UINT nType)
	{
		return CMainApp::msgBox(strMsg, strTitle, nType, this);
	}
	void msgBox(const wstring& strMsg, const wstring& strTitle)
	{
		CMainApp::msgBox(strMsg, strTitle, this);
	}
	void msgBox(const wstring& strMsg)
	{
		CMainApp::msgBox(strMsg, this);
	}
	bool msgBox(const wstring& strMsg, bool bWarning)
	{
		return CMainApp::msgBox(strMsg, bWarning, this);
	}

	bool confirmBox(const wstring& strMsg, const wstring& strTitle)
	{
		return CMainApp::confirmBox(strMsg, strTitle, this);
	}
	bool confirmBox(const wstring& strMsg)
	{
		return CMainApp::confirmBox(strMsg, this);
	}
};
