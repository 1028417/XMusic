#pragma once

template <UINT _ResID=0, bool _CanOK = false, bool _CanCancel = true>
class TDialog : public CDialog
{
public:
	TDialog() : CDialog(_ResID)
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

private:
	CCompatableFont m_font;

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
};
