#pragma once

template <UINT _ResID=0, bool _CanOK = false, bool _CanCancel = true>
class CDialogT : public CDialog
{
public:
	CDialogT()
		: CDialog(_ResID)
	{
	}

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
};
