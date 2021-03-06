#pragma once

#include "view.h"

#include "ModelObserver.h"

class CPlayerView : public IPlayerView
{
public:
	CPlayerView(IXController& controller, IModel& model)
		: m_controller(controller)
		, m_model(model)
		, m_view(controller, model)
		, m_ModelObserver(m_view)
	{
	}

private:
	IXController& m_controller;
	IModel& m_model;

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

	void close() override;

	bool handleHotkey(const tagHotkeyInfo& HotkeyInfo) override;

	bool handleCommand(UINT uID) override;

private:
	bool msgBox(cwstr strMsg, bool bWarning = false) override;

	void updateMediaRelated(const tagMediaSetChanged& MediaSetChanged) override
	{
		m_view.updateMediaRelated(MediaSetChanged);
	}
};
