#pragma once

class __view;

#define __Color_Text RGB(64, 64, 255)

#define __PlaySpiritSkinDir (fsutil::workDir() + L"/skin/")

#include "Common.h"

#include "model.h"

#include "viewdef.h"

#include "globalSize.h"

#include "ImgMgr.h"

#include "page/BasePage.h"

#include "PlayerMainWnd.h"

#include "page/PlaylistPage/PlayingPage.h"
#include "page/PlaylistPage/PlaylistPage.h"
#include "page/PlaylistPage/PlayItemPage.h"

#include "page/SingerPage/SingerPage.h"
#include "page/SingerPage/AlbumPage.h"

#include "PlayCtrl.h"

#include "FloatPanel.h"

extern const wstring __MediaFilter;

typedef vector<pair<CMedia*, int>> TD_VerifyResultVector;

struct tagVerifyResult
{
	TD_VerifyResultVector vctVerifyResult;

	TD_MediaList lstUpdateMedia;
	TD_MediaList lstRemoveMedia;
};

class __view
{
public:
	__view(IPlayerController& controller, IModel& model)
		: m_controller(controller)
		, m_model(model)

		, m_ResModule("view")

		, m_ImgMgr(model)

		, m_MainWnd(*this)

		, m_MediaResPage(*this, true)

		, m_PlayingPage(*this)
		, m_PlayItemPage(*this)
		, m_PlaylistPage(*this, m_PlayItemPage)

		, m_AlbumPage(*this)
		, m_SingerPage(*this, m_AlbumPage)

		, m_PlayCtrl(*this)
	{
	}

private:
	IPlayerController& m_controller;

	IModel& m_model;

public:
	IPlayerController& getController()
	{
		return m_controller;
	}

	IModel& getModel()
	{
		return m_model;
	}

	CRootMediaRes& getRootMediaRes()
	{
		return m_model.getRootMediaRes();
	}

	CMediaSet& getRootMediaSet()
	{
		return m_model.getRootMediaSet();
	}
	
	COptionMgr& getOptionMgr()
	{
		return m_model.getOptionMgr();
	}

	CDataMgr& getDataMgr()
	{
		return m_model.getDataMgr();
	}

	CPlaylistMgr& getPlaylistMgr()
	{
		return m_model.getPlaylistMgr();
	}

	CPlayMgr& getPlayMgr()
	{
		return m_model.getPlayMgr();
	}

	CSingerMgr& getSingerMgr()
	{
		return m_model.getSingerMgr();
	}

	CResModule m_ResModule;

	CGlobalSize m_globalSize;
	CImgMgr m_ImgMgr;

	CPlayerMainWnd m_MainWnd;

	CMediaResPanel m_MediaResPage;

	CPlayingPage m_PlayingPage;
	CPlayItemPage m_PlayItemPage;
	CPlaylistPage m_PlaylistPage;

	CAlbumPage m_AlbumPage;
	CSingerPage m_SingerPage;

	CPlayCtrl m_PlayCtrl;

private:
	void _hittestMediaSet(CMediaSet& MediaSet, CMedia *pMedia = NULL, IMedia *pIMedia = NULL);

	void _exportMedia(CWnd& wnd, const wstring& strTitle, bool bActualMode
		, const function<UINT(CProgressDlg& ProgressDlg, tagExportOption& ExportOption)>& fnExport);

	void _snapshotDir(CMediaRes& dir, const wstring& strOutputFile);

	void _checkSimilarFile(const function<void(CProgressDlg& ProgressDlg, TD_SimilarFile& arrResult)>& fnWork);

public:
	bool init();

	void quit();

	void foregroundMainWnd();
	
	CMediaRes* showChooseDirDlg(const wstring& strTitle, bool bShowRoot, const wstring& t_strRootDir = L"");

	void showFindDlg(const wstring& strFind, bool bQuickHittest=false);
	
	void findMedia(const wstring& strFindPath, bool bDir, const wstring& strSingerName = L"");

	void verifyMedia(const TD_MediaList& lstMedias, CWnd *pWnd = NULL
		, const function<void(const tagVerifyResult&)>&cb = NULL);
	void verifyMedia(CMediaSet& MediaSet, CWnd *pWnd = NULL);

	void exportMedia(const TD_MediaList& lstMedias, CWnd& wnd);
	void exportMediaSet(CMediaSet& MediaSet);
	void exportDir(CMediaRes& dir);
	
	void snapshotDir(CMediaRes& dir);

	void checkSimilarFile(CMediaRes& dir);
	void checkSimilarFile(CMediaRes& dir1, CMediaRes& dir2);

	void hittestMedia(CMedia& Media);

	bool hittestRelatedMediaSet(IMedia& Media, E_MediaSetType eMediaSetType);

	bool addSingerImage(CSinger& Singer, const list<wstring>& lstFiles);

	void updateMediaRelated(const tagMediaSetChanged& MediaSetChanged);

	bool copyMediaTitle(IMedia& media);

	void clearView();

	void restoreDB(const wstring& strFile);
};
