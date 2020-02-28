#include "stdafx.h"

#include "dlg/FindDlg.h"

#include "dlg/ChooseDirDlg.h"

#include "dlg/VerifyResultDlg.h"

#include "dlg/SimilarFileDlg.h"

#include "dlg/ExportMediaSetDlg.h"
#include "dlg/option/ExportOptionDlg.h"

static const wstring __MediaFilter = L" 所有支持音频|*.mp3;*.flac;*.ape;*.wav;*.dsf;*.dff;*.wma;*.m4a;*.aac;*.ac3"
	L"| Mp3文件|*.mp3| Flac文件|*.flac| Ape文件|*.ape"
	L"| Wav文件|*.wav| Dsf文件|*.dsf| Dff文件|*.dff"
	L"| Wma文件|*.wma| M4A文件|*.m4a| AAC文件|*.aac| AC3文件|*.ac3|";

bool __view::init()
{
	if (!m_PlayCtrl.init())
	{
		CMainApp::msgBox(L"请先执行安装");
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
	//(void)m_view.m_MainWnd.ActivePage(m_PlayingPage, false);

	//(void)m_view.m_MainWnd.ActivePage(m_MediaResPage);
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
	//(void)m_view.m_MainWnd.ActivePage(m_PlayingPage, false);

	//(void)m_view.m_MainWnd.ActivePage(m_MediaResPage);
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

CMediaDir* __view::showChooseDirDlg(const wstring& strTitle, bool bShowRoot)
{
	//CRootMediaDir RootDir(getMediaLib().GetAbsPath(), m_view.getOption().plAttachDir);

	CResGuard ResGuard(m_ResModule);
	CChooseDirDlg ChooseDirDlg(strTitle, getMediaLib(), bShowRoot);
	return ChooseDirDlg.show();
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

void __view::verifyMedia(const TD_MediaList& lstMedias, CWnd *pWnd, cfn_void_t<const tagVerifyResult&> cb)
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
	lstMedias([&](CMedia& media) {
		mapMedias[media.GetAbsPath()].add(media);
	});

	tagVerifyResult VerifyResult;

	auto fnVerify = [&](CProgressDlg& ProgressDlg) {
		PairList<wstring, TD_MediaList> plMedias(mapMedias);

		UINT uThreadCount = plMedias.size() / 300;
		uThreadCount = MIN(thread::hardware_concurrency(), uThreadCount);
		
		CMultiTask<pair<wstring, TD_MediaList>, pair<CMediaOpaque, TD_MediaList>> multiTask;
		cauto vecVerifyResult = multiTask.start(plMedias, uThreadCount
			, [&](UINT taskIdx, auto& prTask, auto& prGroup) {
			ProgressDlg.SetStatusText(prTask.first.c_str(), 1);

			long long nFileSize = 0;
			UINT uDuration = prGroup.first.checkFileDuration(prTask.first, nFileSize);
			prTask.second([&](CMedia& media) {
				media.SetDuration(uDuration, nFileSize);
				if (0 == uDuration)
				{
					prGroup.second.add(media);
				}
			});

			if (ProgressDlg.checkCancel())
			{
				return false;
			}

			return true;
		});

		if (ProgressDlg.checkCancel())
		{
			return;
		}

		(void)m_model.getDataMgr().updateMediaSizeDuration(lstMedias);

		for (cauto prInvalidMedia : vecVerifyResult)
		{
			VerifyResult.paInvalidMedia.add(prInvalidMedia.second);
		}
		if (VerifyResult.paInvalidMedia)
		{
			ProgressDlg.Close();
		}
		else
		{
			ProgressDlg.SetStatusText(L"检测完成，未发现异常曲目");
		}
	};

	CProgressDlg ProgressDlg(fnVerify, mapMedias.size());
	__Ensure(IDOK == ProgressDlg.DoModal(L"检测曲目", pWnd));
	
	if (VerifyResult.paInvalidMedia)
	{
		CResGuard ResGuard(m_ResModule);
		CVerifyResultDlg dlg(*this, VerifyResult);
		(void)dlg.DoModal();

		if (VerifyResult.paUpdateMedia)
		{
			(void)m_model.getDataMgr().updateMediaSizeDuration(VerifyResult.paUpdateMedia);
		}
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

void __view::addInMedia(const list<wstring>& lstFiles, CProgressDlg& ProgressDlg)
{
	auto cbConfirm = [&](CSearchMediaInfo& SearchMediaInfo, tagMediaResInfo& MediaResInfo)
	{
		WString strText;
		strText << fsutil::GetFileName(MediaResInfo.strPath)
			<< L"\n时长: " << CMedia::genDurationString(CMediaOpaque::checkDuration(MediaResInfo.strPath))
			<< L"    大小: " << MediaResInfo.strFileSize << L"字节"
			<< L"\n日期: " << MediaResInfo.strFileTime
			<< L"\n\n\n是否更新以下曲目？\n"
			<< fsutil::GetFileName(SearchMediaInfo.m_strAbsPath)
			<< L"\n时长: " << CMedia::genDurationString(CMediaOpaque::checkDuration(SearchMediaInfo.m_strAbsPath))
			<< L"    大小: " << SearchMediaInfo.GetFileSize() << L"字节"
			<< L"\n日期: " << SearchMediaInfo.GetFileTime()
			<< L"\n目录: " << m_model.getMediaLib().toOppPath(fsutil::GetParentDir(SearchMediaInfo.m_strAbsPath))
			<< L"\n\n\n关联: ";

		SearchMediaInfo.m_lstMedias([&](CMedia& media) {
			strText.append(L"\n" + media.m_pParent->GetLogicPath());
		});

		int nRet = ProgressDlg.msgBox(strText, L"匹配到文件", MB_YESNOCANCEL);
		if (IDCANCEL == nRet)
		{
			return E_MatchResult::MR_Ignore;
		}
		else if (IDNO == nRet)
		{
			return E_MatchResult::MR_No;
		}

		return E_MatchResult::MR_Yes;
	};

	ProgressDlg.SetStatusText(L"正在分析曲目...");

	TD_MediaList lstMedias;
	m_model.getMediaLib().GetAllMedias(lstMedias);
	__Ensure(lstMedias);

	CSearchMediaInfoGuard SearchMediaInfoGuard(m_model.getSingerMgr());
	TD_SearchMediaInfoMap mapSearchMedias;
	lstMedias([&](CMedia& media) {
		SearchMediaInfoGuard.genSearchMediaInfo(media, mapSearchMedias);
	});

	ProgressDlg.SetStatusText(L"正在比对文件...");

	list<pair<wstring, CSearchMediaInfo>> lstMatchResult;
	for (auto& strFile : lstFiles)
	{
		ProgressDlg.SetStatusText(strFile.c_str(), 1);
		if (ProgressDlg.checkCancel())
		{
			return;
		}

		tagMediaResInfo MediaResInfo(strFile);
		for (auto itr = mapSearchMedias.begin(); itr != mapSearchMedias.end(); )
		{
			CSearchMediaInfo& SearchMediaInfo = itr->second;

			if (SearchMediaInfo.matchMediaRes(MediaResInfo))
			{
				E_MatchResult eRet = cbConfirm(SearchMediaInfo, MediaResInfo);
				if (E_MatchResult::MR_Ignore == eRet)
				{
					itr = mapSearchMedias.erase(itr);
					continue;
				}

				if (E_MatchResult::MR_Yes == eRet)
				{
					lstMatchResult.emplace_back(MediaResInfo.strPath, SearchMediaInfo);

					(void)mapSearchMedias.erase(itr);

					break;
				}
			}

			++itr;
		}
	}

	ProgressDlg.SetProgress(0, lstMatchResult.size());

	SMap<wstring, wstring> mapUpdateFiles;
	map<CMedia*, wstring> mapUpdatedMedias;
	for (auto& pr : lstMatchResult)
	{
		wstring& strSrcPath = pr.first;
		ProgressDlg.SetStatusText((L"正在合入: " + fsutil::GetFileName(strSrcPath)).c_str(), 1);
		if (ProgressDlg.checkCancel())
		{
			return;
		}

		CSearchMediaInfo& SearchMediaInfo = pr.second;
		wstring strDstPath = fsutil::GetParentDir(SearchMediaInfo.m_strAbsPath)
			+ __wchDirSeparator + fsutil::GetFileName(strSrcPath);

		m_model.getPlayMgr().moveFile(SearchMediaInfo.m_strAbsPath, strDstPath, [&]() {
			(void)fsutil::removeFile(SearchMediaInfo.m_strAbsPath);

			if (!fsutil::moveFile(strSrcPath, strDstPath))
			{
				CMainApp::msgBox(L"复制文件失败: \n\n\t" + strDstPath);
				return false;
			}

			wstring strOppPath = m_model.getMediaLib().toOppPath(strDstPath);
			mapUpdateFiles.set(m_model.getMediaLib().toOppPath(SearchMediaInfo.m_strAbsPath), strOppPath);

			SearchMediaInfo.m_lstMedias([&](CMedia& media) {
				mapUpdatedMedias[&media] = strOppPath;
			});

			return true;
		});
	}

	__Ensure(!mapUpdatedMedias.empty());

	__Ensure(m_model.updateMediaPath(mapUpdatedMedias));

	__Ensure(m_model.updateFile(mapUpdateFiles));

	m_model.refreshMediaLib();

	ProgressDlg.SetStatusText((L"匹配结束, 更新" + to_wstring(mapUpdatedMedias.size()) + L"个曲目").c_str());
}

bool __view::_exportMedia(CWnd& wnd, const wstring& strTitle, bool bActualMode
	, const function<UINT(CProgressDlg& ProgressDlg, tagExportOption& ExportOption)>& fnExport)
{
	static CFolderDlgEx FolderDlg;
	wstring strExportPath = FolderDlg.Show(strTitle, L"请选择导出位置", wnd.m_hWnd);
	__EnsureReturn(!strExportPath.empty(), false);
	if (!getMediaLib().checkIndependentDir(strExportPath, true))
	{
		CMainApp::msgBox(L"请选择与根目录不相关的目录?", L"导出曲目", &wnd);
		return false;
	}

	tagExportOption ExportOption;
	ExportOption.strExportPath = strExportPath + L"/XMusic";
	ExportOption.bActualMode = bActualMode;

	m_ResModule.ActivateResource();

	CExportOptionDlg ExportOptionDlg(ExportOption);
	__EnsureReturn(IDOK == ExportOptionDlg.DoModal(), false);
	
	auto cb = [&](CProgressDlg& ProgressDlg) {
		UINT uCount = fnExport(ProgressDlg, ExportOption);
		if (0 == uCount || ExportOption.lstExportMedias.empty())
		{
			return;
		}

		ProgressDlg.SetProgress(0, uCount);

		UINT uRet = m_model.exportMedia(ExportOption, [&](UINT uProgressOffset, const wstring& strDstFile) {
			if (!strDstFile.empty())
			{
				ProgressDlg.SetStatusText(strDstFile.c_str());
			}

			if (uProgressOffset != 0)
			{
				ProgressDlg.ForwardProgress(uProgressOffset);
			}

			if (ProgressDlg.checkCancel())
			{
				return false;
			}

			return true;
		});

		ProgressDlg.SetStatusText((L"成功导出文件: " + to_wstring(uRet)).c_str());
	};

	CProgressDlg ProgressDlg(cb);
	return IDOK == ProgressDlg.DoModal(strTitle, &wnd);
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
		//CMainApp::msgBox(L"未发现曲目！", L"导出曲目");
		return;
	}

	exportMedia(lstMedias);
}

void __view::exportMedia(const TD_MediaList& lstMedias, CWnd *pWnd)
{
	if (NULL == pWnd)
	{
		pWnd = &m_MainWnd;
	}

	(void)_exportMedia(*pWnd, L"导出曲目", false, [&](CProgressDlg& ProgressDlg, tagExportOption& ExportOption) {
		if (ExportOption.bActualMode)
		{
			SMap<wstring, wstring> mapDirs;
			map<wstring, TD_IMediaList> mapMedias;
			lstMedias([&](CMedia& media) {
				auto& strDstDir = ExportOption.strExportPath + fsutil::GetParentDir(media.GetPath());
				mapMedias[mapDirs.insert(strutil::lowerCase_r(strDstDir), strDstDir)].add(media);
			});

			for (auto& pr : mapMedias)
			{
				tagExportMedia ExportMedia;
				ExportMedia.strDstDir = pr.first;
				ExportMedia.paMedias.swap(pr.second);
				ExportOption.lstExportMedias.push_back(ExportMedia);
			}
		}
		else
		{
			map<CMediaSet*, TD_IMediaList> mapMediaList;
			lstMedias([&](auto& media) {
				mapMediaList[media.m_pParent].add(media);
			});

			for (auto& pr : mapMediaList)
			{
				tagExportMedia ExportMedia;
				ExportMedia.strDstDir = ExportOption.strExportPath + __wchDirSeparator + pr.first->GetExportName();
				ExportMedia.paMedias.swap(pr.second);
				ExportOption.lstExportMedias.push_back(ExportMedia);
			}
		}

		return lstMedias.size();
	});
}

void __view::exportMedia(const TD_IMediaList& lstMedias, CWnd *pWnd)
{
	if (NULL == pWnd)
	{
		pWnd = &m_MainWnd;
	}
	
	(void)_exportMedia(*pWnd, L"导出曲目", true, [&](CProgressDlg& ProgressDlg, tagExportOption& ExportOption) {
		SMap<wstring, wstring> mapDirs;
		map<wstring, TD_IMediaList> mapMedias;
		lstMedias([&](IMedia& media) {
			auto& strDstDir = ExportOption.strExportPath + fsutil::GetParentDir(media.GetPath());
			mapMedias[mapDirs.insert(strutil::lowerCase_r(strDstDir), strDstDir)].add(media);
		});

		for (auto& pr : mapMedias)
		{
			tagExportMedia ExportMedia;
			ExportMedia.strDstDir = pr.first;
			ExportMedia.paMedias.swap(pr.second);
			ExportOption.lstExportMedias.push_back(ExportMedia);
		}
		
		return lstMedias.size();
	});
}

void __view::exportDir(CMediaDir& dir)
{
	dir.clear();
	CMediaResPanel::RefreshMediaResPanel();

	_exportMedia(m_MainWnd, L"导出目录", true, [&](CProgressDlg& ProgressDlg, tagExportOption& ExportOption) {
		UINT uCount = 0;

		dir.scan([&](CPath& dir, TD_XFileList& paSubFile) {
			if (ProgressDlg.checkCancel())
			{
				return false;
			}

			if (paSubFile)
			{
				uCount += paSubFile.size();

				tagExportMedia ExportMedia;
				ExportMedia.strDstDir = ExportOption.strExportPath + ((CMediaRes&)dir).GetPath();
				ExportMedia.paMedias.add(TD_MediaResList(paSubFile));
				ExportMedia.alCueFiles.add(((CMediaDir&)dir).SubCueList());
				ExportOption.lstExportMedias.push_back(ExportMedia);
			}

			return true;
		});

		return uCount;
	});
}

#define __SnapshotTimeFormat L"%Y.%m.%d_%H.%M.%S"

static const wstring __snapshotExt = L"snapshot";

void __view::snapshotDir(CMediaDir& dir)
{
	tagFileDlgOpt FileDlgOpt;
	FileDlgOpt.strTitle = L"选择保存快照路径";
	FileDlgOpt.strFilter = L"快照文件(*." + __snapshotExt + L")|*." + __snapshotExt + L"|";
	FileDlgOpt.strFileName = dir.GetName() + L'_' + tmutil::formatTime(__SnapshotTimeFormat, time(0));
	static CFileDlgEx fileDlg(FileDlgOpt);

	wstring strFile = fileDlg.ShowSave();
	if (!strFile.empty())
	{
		if (strutil::lowerCase_r(fsutil::GetFileExtName(strFile)) != __snapshotExt)
		{
			strFile.append(__wchDot + __snapshotExt);
		}
		
		_snapshotDir(dir, strFile);
	}
}

void __view::_snapshotDir(CMediaRes& dir, const wstring& strOutputFile)
{
	CUTF8TxtWriter TxtWriter;
	if (!TxtWriter.open(strOutputFile, true))
	{
		return;
	}

	bool bGenDuration = &dir != &m_model.getMediaLib();

	auto cb = [&](CProgressDlg& ProgressDlg) {
		wstring strPrfix;

		function<bool(CPath&, JValue&)> fnSnapshot;
		fnSnapshot = [&](CPath& dir, JValue& jRoot) {
			if (ProgressDlg.checkCancel())
			{
				return false;
			}

			jRoot["name"] = strutil::toUtf8(dir.name());

			auto& strDirInfo = wstring(strPrfix.size(), L'-') + dir.name();
			if (!TxtWriter.writeln(strDirInfo))
			{
				return false;
			}

			strPrfix.append(L"  ");

			cauto paSubDir = dir.dirs();

			ProgressDlg.SetProgress(0, paSubDir.size() + 1);
			ProgressDlg.SetStatusText((L"正在生成目录快照: " + dir.absPath()).c_str());

			cauto paSubFile = dir.files();
			if (paSubFile)
			{
				JValue& jFiles = jRoot["files"];

				paSubFile([&](XFile& subFile) {
					auto& strFileSize = ((CMediaRes&)subFile).fileSizeString(false);

					JValue jFile;
					jFile["name"] = strutil::toUtf8(subFile.name());
					jFile["size"] = ((CMediaRes&)subFile).fileSize();
					if (bGenDuration)
					{
						jFile["duration"] = CMediaOpaque::checkDuration((CMediaRes&)subFile);
					}
					jFiles.append(jFile);

					auto& strFileInfo = strPrfix + subFile.name() + L'\t' + strFileSize;
					return TxtWriter.writeln(strFileInfo);
				});
			}
			ProgressDlg.ForwardProgress();

			if (paSubDir)
			{
				auto& jDirs = jRoot["dirs"];

				paSubDir([&](CPath& subDir, size_t uIdx) {
					jDirs.append(JValue());

					bool bRet = fnSnapshot(subDir, jDirs[uIdx]);

					ProgressDlg.ForwardProgress();

					return bRet;
				});
			}

			strPrfix = strPrfix.substr(0, strPrfix.size() - 2);

			return true;
		};

		JValue *pJRoot = new JValue;

		if (fnSnapshot(dir, *pJRoot))
		{
			CUTF8TxtWriter TxtWriter(false);
			if (TxtWriter.open(strOutputFile + L".json", true))
			{
				string str = Json::FastWriter().write(*pJRoot);
				TxtWriter.writeln(str);
			}
		}

		delete pJRoot;

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

void __view::checkSimilarFile(CMediaDir& dir1, CMediaDir& dir2)
{
	_checkSimilarFile([&](CProgressDlg& ProgressDlg, TD_SimilarFile& arrResult) {
		TD_MediaResList lstMediaRes1;
		dir1.scan([&](CPath& dir, TD_XFileList& paSubFile) {
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
		dir2.scan([&](CPath& dir, TD_XFileList& paSubFile) {
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

void __view::checkSimilarFile(CMediaDir& dir)
{
	_checkSimilarFile([&](CProgressDlg& ProgressDlg, TD_SimilarFile& arrResult) {
		TD_MediaResList lstMediaRes;
		dir.scan([&](CPath& dir, TD_XFileList& paSubFile) {
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

	m_PlayingPage.RefreshList();
}

void __view::_hittestMediaSet(CMediaSet& MediaSet, CMedia *pMedia, IMedia *pIMedia)
{
	if (E_MediaSetType::MST_Playlist == MediaSet.m_eType)
	{
		if (!m_PlaylistPage)
		{
			(void)m_MainWnd.ActivePage(m_PlaylistPage);
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
		(void)m_MainWnd.ActivePage(m_AlbumPage);
	}
}

void __view::hittestMedia(CMedia& media)
{
	if (NULL != media.m_pParent)
	{
		this->_hittestMediaSet(*media.m_pParent, &media);
	}
}

bool __view::hittestRelatedMediaSet(IMedia& media, E_MediaSetType eMediaSetType)
{
	CWaitCursor WaitCursor;

	int nRelatedMediaID = media.GetRelatedMediaID(eMediaSetType);
	if (nRelatedMediaID > 0)
	{
		CMedia *pRelatedMedia = getMediaLib().FindMedia(eMediaSetType, (UINT)nRelatedMediaID);
		if (NULL != pRelatedMedia && NULL != pRelatedMedia->m_pParent)
		{
			_hittestMediaSet(*pRelatedMedia->m_pParent, pRelatedMedia);

			return true;
		}
	}

	UINT uRelatedMediaSetID = media.GetRelatedMediaSetID(eMediaSetType);
	if (uRelatedMediaSetID > 0)
	{
		CMediaSet *pMediaSet = getMediaLib().FindSubSet(eMediaSetType, uRelatedMediaSetID);
		__EnsureReturn(pMediaSet, false);

		_hittestMediaSet(*pMediaSet, NULL, &media);

		return true;
	}

	return false;
}

bool __view::addSingerImage(CSinger& Singer, const list<wstring>& lstFiles)
{
	__EnsureReturn(m_ImgMgr.addSingerImg(Singer.m_uID, Singer.m_strName, lstFiles), false);

	tagMediaSetChanged MediaSetChanged(E_MediaSetType::MST_Singer, Singer.m_uID, E_MediaSetChanged::MSC_SingerImgChanged);
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

	cauto strTitle = media.GetTitle();
	auto len = (strTitle.size() + 1) * 2;

	HANDLE hClip = GlobalAlloc(GMEM_MOVEABLE, len);
	wchar_t *lpBuff = (wchar_t*)GlobalLock(hClip);
	memcpy(lpBuff, strTitle.c_str(), len);
	GlobalUnlock(hClip);

	EmptyClipboard();
	SetClipboardData(CF_UNICODETEXT, hClip);
	CloseClipboard();

	return true;
}
