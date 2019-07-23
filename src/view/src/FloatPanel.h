#pragma once

#define MAX_ALPHA 255

class CFloatPanel : public CWnd
{
public:
	CFloatPanel()
	{
	}
	
	DECLARE_MESSAGE_MAP()

private:
	void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);

public:
	bool Create(CWnd& wndParent, bool bLayerd = false, uint8_t uAlpha = MAX_ALPHA);
};
