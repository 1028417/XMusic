#pragma once

#include "view.h"

#include "ModelObserver.h"

class CPlayerView : public IPlayerView
{
public:
	CPlayerView(IXController& controller, IModel& model)
		: m_view(controller, model)
		, m_ModelObserver(m_view)
	{
	}

private:
	__view m_view;

	CModelObserver m_ModelObserver;

private:
	void _verifyMedia();

	void _checkDuplicateMedia(E_CheckDuplicateMode eMode);

	void _checkSimilarFile();

	void _addInMedia();
	
private:
	IModelObserver& getModelObserver() override
	{
		return m_ModelObserver;
	}

	CMainWnd* show() override;

	bool handleHotkey(const tagHotkeyInfo& HotkeyInfo) override;

	bool handleCommand(UINT uID) override;

private:
	bool showMsgBox(const wstring& strMsg, bool bWarning = false) override;

	void updateMediaRelated(const tagMediaSetChanged& MediaSetChanged) override
	{
		m_view.updateMediaRelated(MediaSetChanged);
	}
};
