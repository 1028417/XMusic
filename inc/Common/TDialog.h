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

	void async(UINT uDelayTime, cfn_void cb)
	{
		auto hWnd = m_hWnd;
		__async(uDelayTime, [=]() {
			if (::IsWindow(hWnd))
			{
				cb();
			}
		});
	}
	
	void async(cfn_void cb)
	{
		async(0, cb);
	}


public:
	int msgBox(cwstr strMsg, cwstr strTitle, UINT nType)
	{
		return CMainApp::msgBox(strMsg, strTitle, nType, this);
	}
	void msgBox(cwstr strMsg, cwstr strTitle)
	{
		CMainApp::msgBox(strMsg, strTitle, this);
	}
	void msgBox(cwstr strMsg)
	{
		CMainApp::msgBox(strMsg, this);
	}

	bool confirmBox(cwstr strMsg, cwstr strTitle)
	{
		return CMainApp::confirmBox(strMsg, strTitle, this);
	}
	bool confirmBox(cwstr strMsg)
	{
		return CMainApp::confirmBox(strMsg, this);
	}
};
