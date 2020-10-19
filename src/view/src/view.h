#pragma once

class __view;

#define __Color_Text RGB(85, 85, 255)

#define __PlaySpiritSkinDir (fsutil::getModuleSubPath(L"skin\\"))

#define __AsyncTaskElapse (std::thread::hardware_concurrency()>4?30:200)

#include "viewdef.h"

#include "model/model.h"

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

struct tagVerifyResult
{
	TD_MediaList paInvalidMedia;

	TD_MediaList paUpdateMedia;
	TD_MediaList paRemoveMedia;
};

class __view
{
public:
	__view(IXController& controller, IModel& model)
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
	IXController& m_controller;

	IModel& m_model;

public:
	tagOption& getOption()
	{
		return m_controller.getOption();
	}

	IXController& getController()
	{
		return m_controller;
	}

	IModel& getModel()
	{
		return m_model;
	}
	
	CDataMgr& getDataMgr()
	{
		return m_model.getDataMgr();
	}

	CBackupMgr& getBackupMgr()
	{
		return m_model.getBackupMgr();
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

	CSingerImgMgr& getSingerImgMgr()
	{
		return m_model.getSingerImgMgr();
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
	bool _create();

	bool _exportMedia(CWnd& wnd, cwstr strTitle
		, const function<UINT(CProgressDlg& ProgressDlg, tagExportOption& ExportOption)>& fnExport);

	void _snapshotDir(CMediaRes& dir, cwstr strOutputFile);

	void _checkSimilarFile(const function<void(CProgressDlg& ProgressDlg, TD_SimilarFile& arrResult)>& fnWork);

public:
	bool show();

	void initView();
	void clearView();

	CMediaDir* showChooseDirDlg(cwstr strTitle, bool bShowRoot = true);

	void showFindDlg(cwstr strFind, bool bQuickHittest=false);
	
	void findMedia(cwstr strFindPath, bool bDir, cwstr strSingerName = L"");

	void verifyMedia(const TD_MediaList& lstMedias, CWnd *pWnd = NULL
		, cfn_void_t<const tagVerifyResult&> cb = NULL);
	void verifyMedia(CMediaSet& MediaSet, CWnd *pWnd = NULL);

	void addInMedia(const list<wstring>& lstFiles, CProgressDlg& ProgressDlg);

	void exportMediaSet(CMediaSet& MediaSet);
	void exportMedia(const TD_MediaList& lstMedias, CWnd *pWnd = NULL);
	void exportMedia(const TD_IMediaList& lstMedias, CWnd *pWnd = NULL);
	void exportDir(CMediaDir& dir);
	
	void snapshotDir(CMediaDir& dir);

	void checkSimilarFile(CMediaDir& dir);
	void checkSimilarFile(CMediaDir& dir1, CMediaDir& dir2);

	UINT formatFileTitle(CMediaDir& dir);

	void hittestMedia(CMedia& media);

	void hittestMediaSet(CMediaSet& MediaSet, CMedia *pMedia, IMedia *pIMedia = NULL);

	bool hittestRelatedMediaSet(IMedia& media, E_RelatedMediaSet eRmsType);

	bool addSingerImage(CSinger& Singer, const list<wstring>& lstFiles);

	void updateMediaRelated(const tagMediaSetChanged& MediaSetChanged);

	bool copyMediaTitle(IMedia& media);

	BYTE genByteRateAlpha(CMedia& media);

	void exploreMedia(IMedia& media, bool bSel = true);
};
