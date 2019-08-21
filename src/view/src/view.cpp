#include "stdafx.h"

#include "dlg/FindDlg.h"

#include "dlg/ChooseDirDlg.h"

#include "dlg/VerifyResultDlg.h"

#include "dlg/SimilarFileDlg.h"

#include "dlg/ExportMediaSetDlg.h"
#include "dlg/option/ExportOptionDlg.h"

const wstring __MediaFilter = L" 所有支持音频|*.mp3;*.flac;*.ape;*.wav;*.dsf;*.dff;*.wma;*.aac;*.ac3;*.aif;*.m4a"
	L"| Mp3文件|*.mp3| Flac文件|*.flac| Ape文件|*.ape"
	L"| Wav文件|*.wav| Dsf文件|*.dsf| Dff文件|*.dff"
	L"| Wma文件|*.wma| AAC文件|*.aac| AC3文件|*.ac3| AIF文件|*.aif| M4A文件|*.m4a|";

bool __view::init()
{
	if (!m_PlayCtrl.init())
	{
		CMainApp::showMsg(L"请先执行安装");
		return false;
	}

	m_globalSize.init();

	__AssertReturn(m_ImgMgr.init(m_globalSize.m_uBigIconSize, m_globalSize.m_uSmallIconSize, m_globalSize.m_uTabHeight), false);

	bool bRet = mtutil::thread([&]() {
		__EnsureReturn(m_MainWnd.Create(), false);

		m_MainWnd.ModifyStyle(WS_BORDER, 0);

		tagViewStyle leftViewStyle(E_DockViewType::DVT_DockLeft);
		leftViewStyle.uDockSize = m_globalSize.m_uLeftDockWidth;
		leftViewStyle.uMaxDockSize = UINT(m_globalSize.m_uLeftDockWidth*1.5);
		leftViewStyle.uStartPos = m_globalSize.m_uHeadHeight;
		leftViewStyle.TabStyle.eTabStyle = E_TabStyle::TS_Bottom;
		leftViewStyle.TabStyle.fTabFontSize = m_globalSize.m_fBigFontSize;
		leftViewStyle.TabStyle.uTabHeight = m_globalSize.m_uTabHeight;

		CDockView *pDockView = m_MainWnd.CreateView(m_PlayingPage, leftViewStyle);
		__AssertReturn(pDockView, false);
		__AssertReturn(m_MainWnd.AddPage(m_PlaylistPage, E_DockViewType::DVT_DockLeft), false);
		__AssertReturn(m_MainWnd.AddPage(m_SingerPage, E_DockViewType::DVT_DockLeft), false);

		tagViewStyle centerViewStyle(E_DockViewType::DVT_DockCenter);
		centerViewStyle.TabStyle.eTabStyle = E_TabStyle::TS_Bottom;
		centerViewStyle.TabStyle.fTabFontSize = m_globalSize.m_fBigFontSize;
		centerViewStyle.TabStyle.pImglst = &m_ImgMgr.getImglst(E_GlobalImglst::GIL_Tiny);
		centerViewStyle.TabStyle.uTabHeight = m_globalSize.m_uTabHeight;
		pDockView = m_MainWnd.CreateView(m_MediaResPage, centerViewStyle);
		__AssertReturn(pDockView, false);

		return true;
	}, [&]() {
		m_ImgMgr.initSingerImg();
	});

	if (!bRet)
	{
		return false;
	}

	m_MediaResPage.ShowPath();

	m_MainWnd.show();

	if (!m_model.status())
	{
		CMainApp::async([&]() {
			CFolderDlgEx FolderDlg;
			wstring strRootDir = FolderDlg.Show(L"设定根目录", L"根目录不存在，请重新设定");
			if (!strRootDir.empty())
			{
				(void)m_model.initRootMediaRes(strRootDir);
			}
		});
	}

	return true;
}

void __view::initView()
{
	m_ImgMgr.clearSingerImg();
	m_ImgMgr.initSingerImg();

	CMediaResPanel::RefreshMediaResPanel();

	if (m_SingerPage)
	{
		m_SingerPage.RefreshTree();
	}

	if (m_PlaylistPage)
	{
		m_PlaylistPage.RefreshList();
	}

	m_PlayingPage.RefreshList();
	//(void)m_PlayingPage.Active();

	//m_MediaResPage.Active();
}

void __view::clearView()
{
	m_PlayCtrl.getPlaySpirit()->clear();

	m_PlayItemPage.m_pPlaylist = NULL;

	if (m_PlaylistPage)
	{
		m_PlaylistPage.RefreshList();
	}

	if (m_PlayItemPage)
	{
		m_PlayItemPage.ShowPlaylist(NULL, false);
	}

	if (m_SingerPage)
	{
		m_SingerPage.RefreshTree();
	}
	if (m_AlbumPage)
	{
		m_AlbumPage.ShowSinger(NULL);
	}

	m_PlayingPage.RefreshList();
	//(void)m_PlayingPage.Active();

	//m_MediaResPage.Active();
}

void __view::quit()
{
	m_PlayCtrl.close();

	(void)CMainApp::GetMainApp()->Quit();
}

void __view::foregroundMainWnd()
{
	if (m_MainWnd.IsIconic())
	{
		(void)m_MainWnd.ShowWindow(SW_RESTORE);
	}
	else
	{
		::SetWindowPos(m_MainWnd.m_hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
		::SetWindowPos(m_MainWnd.m_hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
	}

	//m_MainWnd.OnFocus(TRUE);
}

CMediaRes* __view::showChooseDirDlg(const wstring& strTitle, bool bShowRoot, const wstring& t_strRootDir)
{
	CResGuard ResGuard(m_ResModule);

	wstring strRootDir = t_strRootDir;
	if (strRootDir.empty())
	{
		strRootDir = m_model.getRootMediaRes().GetAbsPath();
	}

	wstring strRetDir;
	CChooseDirDlg ChooseDirDlg(strTitle, strRootDir, bShowRoot, strRetDir);
	__EnsureReturn(IDOK == ChooseDirDlg.DoModal(), NULL);
	
	wstring strOppPath = m_model.getRootMediaRes().toOppPath(strRetDir);

	CMediaRes *pMediaRes = m_model.getRootMediaRes().FindSubPath(strOppPath, true);
	if (NULL == pMediaRes)
	{
		CMainApp::showMsg(L"目录不存在！");
	}

	return pMediaRes;
}

void __view::showFindDlg(const wstring& strFind, bool bQuickHittest)
{
	m_ResModule.ActivateResource();
	CFindDlg FindDlg(*this, bQuickHittest, E_FindMediaMode::FMM_MatchText, strFind);
	(void)FindDlg.DoModal();
}

void __view::findMedia(const wstring& strFindPath, bool bDir, const wstring& strSingerName)
{
	CResGuard ResGuard(m_ResModule);

	bool bQuickHittest = true;
	E_FindMediaMode eFindMediaMode = E_FindMediaMode::FMM_MatchPath;
	if (bDir)
	{
		bQuickHittest = false;
		eFindMediaMode = E_FindMediaMode::FMM_MatchDir;
	}

	CFindDlg FindDlg(*this, bQuickHittest, eFindMediaMode, strFindPath, strSingerName);
	(void)FindDlg.DoModal();
}

void __view::verifyMedia(const TD_MediaList& lstMedias, CWnd *pWnd, const function<void(const tagVerifyResult&)>&cb)
{
	if (NULL == pWnd)
	{
		pWnd = &m_MainWnd;
	}

	if (!lstMedias)
	{
		return;
	}

	map<wstring, TD_MediaList> mapMedias;
	lstMedias([&](CMedia& Media) {
		mapMedias[Media.GetAbsPath()].add(Media);
	});

	CCASLock lock;

	tagVerifyResult VerifyResult;
	auto fnVerify = [&](CProgressDlg& ProgressDlg) {
		PairList<wstring, TD_MediaList> plMedias(mapMedias);

		UINT uThreadCount = plMedias.size() / 500;
		uThreadCount = MIN(thread::hardware_concurrency(), uThreadCount);
		
		CMultiTask<pair<wstring, TD_MediaList>, BOOL>::start(plMedias, uThreadCount
			, [&](UINT taskIdx, auto& task) {

			ProgressDlg.SetStatusText(task.first.c_str(), 1);

			CFileOpaqueEx FileOpaque(task.first);
			int nDuration = FileOpaque.checkDuration(false);
			if (0 == nDuration)
			{
				if (!FileOpaque.exists())
				{
					nDuration = -1;
				}
			}
			if (nDuration <= 0)
			{
				lock.lock();
				task.second([&](CMedia& Media) {
					VerifyResult.vctVerifyResult.push_back(std::make_pair(&Media, nDuration));
				});
				lock.unlock();
			}

			if (ProgressDlg.checkCancel())
			{
				return false;
			}

			return true;
		});

		if (VerifyResult.vctVerifyResult.empty())
		{
			ProgressDlg.SetStatusText(L"检测完成，未发现异常曲目");
		}
		else
		{
			ProgressDlg.Close();
		}
	};

	CProgressDlg ProgressDlg(fnVerify, mapMedias.size());
	if (IDOK == ProgressDlg.DoModal(L"检测曲目", pWnd) && !VerifyResult.vctVerifyResult.empty())
	{
		CResGuard ResGuard(m_ResModule);
		CVerifyResultDlg dlg(*this, VerifyResult);
		(void)dlg.DoModal();
	}

	if (cb)
	{
		cb(VerifyResult);
	}
}

void __view::verifyMedia(CMediaSet& MediaSet, CWnd *pWnd)
{
	TD_MediaList lstMedias;
	MediaSet.GetAllMedias(lstMedias);
	verifyMedia(lstMedias, pWnd);
}

void __view::_exportMedia(CWnd& wnd, const wstring& strTitle, bool bActualMode
	, const function<UINT(CProgressDlg& ProgressDlg, tagExportOption& ExportOption)>& fnExport)
{
	static CFolderDlgEx FolderDlg;
	wstring strExportPath = FolderDlg.Show(strTitle, L"请选择导出位置", wnd.m_hWnd);
	__Ensure(!strExportPath.empty());
	if (!m_model.getRootMediaRes().checkIndependentDir(strExportPath, true))
	{
		CMainApp::showMsg(L"请选择与根目录不相关的目录?", L"导出曲目", &wnd);
		return;
	}

	tagExportOption ExportOption;
	ExportOption.strExportPath = strExportPath;
	ExportOption.bActualMode = bActualMode;

	m_ResModule.ActivateResource();

	CExportOptionDlg ExportOptionDlg(ExportOption);
	__Ensure(IDOK == ExportOptionDlg.DoModal());
	
	auto cb = [&](CProgressDlg& ProgressDlg) {
		UINT uCount = fnExport(ProgressDlg, ExportOption);
		if (0 == uCount || ExportOption.plMedias.empty())
		{
			return;
		}

		ProgressDlg.SetProgress(0, uCount);

		UINT uRet = m_model.exportMedia(ExportOption
			, [&](UINT uProgressOffset, const wstring& strDstFile) {

			if (!strDstFile.empty())
			{
				ProgressDlg.SetStatusText(strDstFile.c_str());
			}

			ProgressDlg.ForwardProgress(uProgressOffset);

			if (ProgressDlg.checkCancel())
			{
				return false;
			}

			return true;
		});

		ProgressDlg.SetStatusText((L"成功导出文件：" + to_wstring(uRet)).c_str());
	};

	CProgressDlg ProgressDlg(cb);
	(void)ProgressDlg.DoModal(strTitle, &wnd);
}

void __view::exportMedia(const TD_MediaList& lstMedias, CWnd& wnd)
{
	_exportMedia(wnd, L"导出曲目", false, [&](CProgressDlg& ProgressDlg, tagExportOption& ExportOption) {
		if (ExportOption.bActualMode)
		{
			SMap<wstring, wstring> mapDirs;
			map<wstring, TD_IMediaList> mapMedias;
			lstMedias([&](CMedia& media) {
				auto& strDstDir = ExportOption.strExportPath + fsutil::GetParentDir(media.GetPath());
				mapMedias[mapDirs.insert(wsutil::lowerCase_r(strDstDir), strDstDir)].add(media);
			});

			for (auto& pr : mapMedias)
			{
				ExportOption.plMedias.add(pr);
			}
		}
		else
		{
			map<CMediaSet*, TD_IMediaList> mapMediaList;
			lstMedias([&](auto& Media) {
				mapMediaList[Media.m_pParent].add(Media);
			});

			for (auto& pr : mapMediaList)
			{
				auto& strDstDir = ExportOption.strExportPath + __wcFSSlant + pr.first->GetExportName();
				ExportOption.plMedias.add(strDstDir, pr.second);
			}
		}

		return lstMedias.size();
	});
}

void __view::exportMediaSet(CMediaSet& MediaSet)
{
	TD_MediaList lstMedias;
	if (E_MediaSetType::MST_Playlist == MediaSet.m_eType || E_MediaSetType::MST_Album == MediaSet.m_eType)
	{
		MediaSet.GetAllMedias(lstMedias);
	}
	else
	{
		m_ResModule.ActivateResource();

		TD_MediaSetList lstMediaSets;
		CExportMediaSetDlg MediaSetDlg(*this, MediaSet, lstMediaSets);
		if (IDOK != MediaSetDlg.DoModal())
		{
			return;
		}

		lstMediaSets([&](CMediaSet& MediaSet) {
			MediaSet.GetMedias(lstMedias);
		});
	}

	if (!lstMedias)
	{
		//CMainApp::showMsg(L"未发现曲目！", L"导出曲目");
		return;
	}

	exportMedia(lstMedias, m_MainWnd);
}

void __view::exportDir(CMediaRes& dir)
{
	_exportMedia(m_MainWnd, L"导出目录", true, [&](CProgressDlg& ProgressDlg, tagExportOption& ExportOption) {
		UINT uCount = 0;

		dir.scan([&](CPath& dir, TD_PathList& paSubFile) {
			if (ProgressDlg.checkCancel())
			{
				return false;
			}

			if (paSubFile)
			{
				uCount += paSubFile.size();

				cauto& strExportPath = ExportOption.strExportPath + ((CMediaRes&)dir).GetPath();
				ExportOption.plMedias.add(strExportPath, TD_IMediaList(TD_MediaResList(paSubFile)));
			}

			return true;
		});

		return uCount;
	});
}

#define __SnapshotTimeFormat L"%Y.%m.%d_%H.%M.%S"

static const wstring __snapshotExt = L"snapshot";

void __view::snapshotDir(CMediaRes& dir)
{
	tagFileDlgOpt FileDlgOpt;
	FileDlgOpt.strTitle = L"选择保存快照路径";
	FileDlgOpt.strFilter = L"快照文件(*." + __snapshotExt + L")|*." + __snapshotExt + L"|";
	FileDlgOpt.strFileName = dir.GetName() + L'_' + tmutil::formatTime(__SnapshotTimeFormat);
	CFileDlgEx fileDlg(FileDlgOpt);

	wstring strFile = fileDlg.ShowSave();
	if (!strFile.empty())
	{
		if (wsutil::lowerCase_r(fsutil::GetFileExtName(strFile)) != __snapshotExt)
		{
			strFile.append(__wcDot + __snapshotExt);
		}
		
		_snapshotDir(dir, strFile);
	}
}

void __view::_snapshotDir(CMediaRes& dir, const wstring& strOutputFile)
{	
	CUTF8Writer TxtWriter;
	if (!TxtWriter.open(strOutputFile, true))
	{
		return;
	}

	auto cb = [&](CProgressDlg& ProgressDlg) {
		wstring strPrfix;

		function<bool(CPath&)> fnSnapshot;
		fnSnapshot = [&](CPath& dir) {
			if (ProgressDlg.checkCancel())
			{
				return false;
			}

			auto& strDirInfo = wstring(strPrfix.size(), L'-') + dir.GetName();
			TxtWriter.writeln(strDirInfo);

			strPrfix.append(L"  ");

			cauto& paSubDir = dir.dirs();

			ProgressDlg.SetProgress(0, paSubDir.size() + 1);
			ProgressDlg.SetStatusText((L"正在生成" + dir.GetPath() + L"目录快照").c_str());

			dir.files()([&](CPath& subFile) {
				auto& strFileSize = ((CMediaRes&)subFile).GetFileSizeString(false);
				auto& strFileInfo = strPrfix + subFile.GetName() + L'\t' + strFileSize;
				TxtWriter.writeln(strFileInfo);
			});

			ProgressDlg.ForwardProgress();

			paSubDir([&](CPath& subDir) {
				bool bRet = fnSnapshot(subDir);

				ProgressDlg.ForwardProgress();

				return bRet;
			});

			strPrfix = strPrfix.substr(0, strPrfix.size() - 2);

			return true;
		};

		(void)fnSnapshot(dir);
		
		ProgressDlg.SetStatusText(L"已生成快照");
	};

	CProgressDlg ProgressDlg(cb);
	(void)ProgressDlg.DoModal(L"生成快照", &m_MainWnd);
}

void __view::_checkSimilarFile(const function<void(CProgressDlg& ProgressDlg, TD_SimilarFile& arrResult)>& fnWork)
{
	TD_SimilarFile arrResult;

	CProgressDlg ProgressDlg([&](CProgressDlg& ProgressDlg) {
		fnWork(ProgressDlg, arrResult);

		if (!arrResult)
		{
			ProgressDlg.SetStatusText(L"检查完成，未发现相似文件");
		}
		else
		{
			ProgressDlg.Close();
		}
	});
	if (IDOK == ProgressDlg.DoModal(L"检查相似文件", &m_MainWnd) && arrResult)
	{
		CResGuard ResGuard(m_ResModule);
		CSimilarFileDlg SimilarFileDlg(*this, arrResult);
		SimilarFileDlg.DoModal();
	}
}

void __view::checkSimilarFile(CMediaRes& dir1, CMediaRes& dir2)
{
	_checkSimilarFile([&](CProgressDlg& ProgressDlg, TD_SimilarFile& arrResult) {
		TD_MediaResList lstMediaRes1;
		dir1.scan([&](CPath& dir, TD_PathList& paSubFile) {
			if (ProgressDlg.checkCancel())
			{
				return false;
			}

			if (paSubFile)
			{
				lstMediaRes1.add(paSubFile);

				ProgressDlg.SetProgress(0, lstMediaRes1.size());
			}

			return true;
		});

		if (!lstMediaRes1)
		{
			ProgressDlg.SetProgress(0, 0);

			ProgressDlg.SetStatusText(L"第一个目录未发现文件");
			return;
		}

		UINT uProgress = lstMediaRes1.size();

		TD_MediaResList lstMediaRes2;
		dir2.scan([&](CPath& dir, TD_PathList& paSubFile) {
			if (ProgressDlg.checkCancel())
			{
				return false;
			}

			if (paSubFile)
			{
				lstMediaRes2.add(paSubFile);

				ProgressDlg.SetProgress(0, uProgress + lstMediaRes2.size());
			}

			return true;
		});

		if (!lstMediaRes2)
		{
			ProgressDlg.SetProgress(0, 0);

			ProgressDlg.SetStatusText(L"第二个目录未发现文件");
			return;
		}

		m_model.checkSimilarFile(lstMediaRes1, lstMediaRes2, [&](CMediaRes& MediaRes) {
			if (ProgressDlg.checkCancel())
			{
				return false;
			}

			ProgressDlg.SetStatusText(MediaRes.GetPath().c_str(), 1);

			return true;
		}, arrResult);
	});
}

void __view::checkSimilarFile(CMediaRes& dir)
{
	_checkSimilarFile([&](CProgressDlg& ProgressDlg, TD_SimilarFile& arrResult) {
		TD_MediaResList lstMediaRes;
		dir.scan([&](CPath& dir, TD_PathList& paSubFile) {
			if (ProgressDlg.checkCancel())
			{
				return false;
			}

			if (paSubFile)
			{
				lstMediaRes.add(paSubFile);

				ProgressDlg.SetProgress(0, lstMediaRes.size());
			}

			return true;
		});

		if (!lstMediaRes)
		{
			ProgressDlg.SetProgress(0, 0);

			ProgressDlg.SetStatusText(L"未发现文件");
			return;
		}

		m_model.checkSimilarFile(lstMediaRes, [&](CMediaRes& MediaRes) {
			if (ProgressDlg.checkCancel())
			{
				return false;
			}

			ProgressDlg.SetStatusText(MediaRes.GetPath().c_str(), 1);

			return true;
		}, arrResult);
	});
}

void __view::updateMediaRelated(const tagMediaSetChanged& MediaSetChanged)
{
	m_MediaResPage.UpdateRelated(MediaSetChanged);

	if (m_PlayItemPage)
	{
		m_PlayItemPage.UpdateRelated(MediaSetChanged);
	}

	if (m_AlbumPage)
	{
		m_AlbumPage.UpdateRelated(MediaSetChanged);
	}
}

void __view::_hittestMediaSet(CMediaSet& MediaSet, CMedia *pMedia, IMedia *pIMedia)
{
	if (E_MediaSetType::MST_Playlist == MediaSet.m_eType)
	{
		if (!m_PlaylistPage)
		{
			(void)m_PlaylistPage.Active();
		}
		m_PlaylistPage.m_wndList.SelectObject(&MediaSet);

		if (NULL != pMedia)
		{
			m_PlayItemPage.HittestMedia(pMedia);
		}
	}
	else if (E_MediaSetType::MST_Singer == MediaSet.m_eType || E_MediaSetType::MST_Album == MediaSet.m_eType)
	{
		CSinger *pSinger = NULL;
		if (E_MediaSetType::MST_Singer == MediaSet.m_eType)
		{
			pSinger = (CSinger*)&MediaSet;
		}
		else
		{
			pSinger = (CSinger*)MediaSet.m_pParent;
		}

		(void)m_SingerPage.Active(*pSinger);
		m_AlbumPage.ShowSinger(pSinger, pMedia, pIMedia);
		(void)m_AlbumPage.Active();
	}
}

void __view::hittestMedia(CMedia& Media)
{
	if (NULL != Media.m_pParent)
	{
		this->_hittestMediaSet(*Media.m_pParent, &Media);
	}
}

bool __view::hittestRelatedMediaSet(IMedia& Media, E_MediaSetType eMediaSetType)
{
	CWaitCursor WaitCursor;

	int iRelatedMediaID = Media.GetRelatedMediaID(eMediaSetType);
	if (0 < iRelatedMediaID)
	{
		CMedia *pRelatedMedia = m_model.getRootMediaSet().HittestMedia(eMediaSetType, (UINT)iRelatedMediaID);
		if (NULL != pRelatedMedia && NULL != pRelatedMedia->m_pParent)
		{
			_hittestMediaSet(*pRelatedMedia->m_pParent, pRelatedMedia);

			return true;
		}
	}

	UINT uRelatedMediaSetID = Media.GetRelatedMediaSetID(eMediaSetType);
	if (uRelatedMediaSetID > 0)
	{
		CMediaSet *pMediaSet = m_model.getRootMediaSet().HittestMediaSet(eMediaSetType, uRelatedMediaSetID);
		__EnsureReturn(pMediaSet, false);

		_hittestMediaSet(*pMediaSet, NULL, &Media);

		return true;
	}

	return false;
}

bool __view::addSingerImage(CSinger& Singer, const list<wstring>& lstFiles)
{
	__EnsureReturn(m_ImgMgr.addSingerImg(Singer.m_uID, Singer.m_strName, lstFiles), false);

	tagMediaSetChanged MediaSetChanged(E_MediaSetType::MST_Singer, E_MediaSetChanged::MSC_SingerImgChanged, Singer.m_uID);
	MediaSetChanged.uSingerImgPos = m_ImgMgr.getSingerImgPos(Singer.m_uID);
	updateMediaRelated(MediaSetChanged);

	return true;
}

bool __view::copyMediaTitle(IMedia& media)
{
	if (!m_MainWnd.OpenClipboard())
	{
		return false;
	}
	
	auto& strTitle = media.GetTitle();
	size_t size = strTitle.size() * 2 + 2;
	HANDLE hClip = GlobalAlloc(GHND, size);
	wchar_t *lpBuff = (wchar_t*)GlobalLock(hClip);
	wcscpy_s(lpBuff, size, strTitle.c_str());
	GlobalUnlock(hClip);
	
	EmptyClipboard();
	SetClipboardData(CF_UNICODETEXT, hClip);
	CloseClipboard();

	return true;
}
