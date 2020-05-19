#include "stdafx.h"

#include "dlg/FindDlg.h"

#include "dlg/ChooseDirDlg.h"

#include "dlg/VerifyResultDlg.h"

#include "dlg/SimilarFileDlg.h"

#include "dlg/ExportMediaSetDlg.h"
#include "dlg/option/ExportOptionDlg.h"

static const wstring __MediaFilter = L" ����֧����Ƶ|*.mp3;*.flac;*.wav;*.dsf;*.dff;*.m4a;*.ape;*.aiff;*.wma;*.ac3;*.aac"
	L"| Mp3�ļ�|*.mp3| Flac�ļ�|*.flac| Wav�ļ�|*.wav"
	L"| Dsf�ļ�|*.dsf|  Dff�ļ�|*.dff"
	L"| M4A�ļ�|*.m4a|  Ape�ļ�|*.ape| Aiff�ļ�|*.aiff"
	L"| Wma�ļ�|*.wma|  AC3�ļ�|*.ac3|  AAC�ļ�|*.aac|";

bool __view::show()
{
	bool bRet = mtutil::concurrence([&]() {
		if (!CPlaySpirit::inst().Create())
		{
			CMainApp::msgBox(L"����ִ�а�װ");
			return false;
		}

		m_globalSize.init();

		__AssertReturn(m_ImgMgr.init(m_globalSize.m_uBigIconSize, m_globalSize.m_uSmallIconSize, m_globalSize.m_uTabHeight), false);
		
		__EnsureReturn(_create(), false);

		m_PlayCtrl.showPlaySpirit();

		return true;
	}, [&]() {
		m_model.initMediaLib();
	});
	if (!bRet)
	{
		return false;
	}

	m_ImgMgr.initSingerImg();

	m_PlayingPage.RefreshList();
	m_MainWnd.show();

	__async([&]() {
		m_MediaResPage.ShowDir();
	});

	return true;
}

bool __view::_create()
{
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
	centerViewStyle.TabStyle.pImglst = &m_ImgMgr.smallImglst();
	centerViewStyle.TabStyle.uTabHeight = m_globalSize.m_uTabHeight;
	pDockView = m_MainWnd.CreateView(m_MediaResPage, centerViewStyle);
	__AssertReturn(pDockView, false);
	
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
	//m_PlayingPage.Active(false);

	//m_MediaResPage.Active();
}

void __view::clearView()
{
	CPlaySpirit::inst()->clear();

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
	//m_PlayingPage.Active(false);

	//m_MediaResPage.Active();
}

CMediaDir* __view::showChooseDirDlg(cwstr strTitle, bool bShowRoot)
{
	//CRootMediaDir RootDir(__medialib.path(), m_view.getOption().plAttachDir);

	CResGuard ResGuard(m_ResModule);
	CChooseDirDlg ChooseDirDlg(strTitle, __medialib, bShowRoot);
	return ChooseDirDlg.show();
}

void __view::showFindDlg(cwstr strFind, bool bQuickHittest)
{
	m_ResModule.ActivateResource();
	CFindDlg FindDlg(*this, bQuickHittest, E_FindMediaMode::FMM_MatchText, strFind);
	(void)FindDlg.DoModal();
}

void __view::findMedia(cwstr strFindPath, bool bDir, cwstr strSingerName)
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

			if (!ProgressDlg.checkStatus())
			{
				return false;
			}

			return true;
		});

		if (!ProgressDlg.checkStatus())
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
			ProgressDlg.SetStatusText(L"�����ɣ�δ�����쳣��Ŀ");
		}
	};

	CProgressDlg ProgressDlg(fnVerify, mapMedias.size());
	__Ensure(IDOK == ProgressDlg.DoModal(L"�����Ŀ", pWnd));
	
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
	cauto cbConfirm = [&](CMatchMediaInfo& MatchMediaInfo, CMediaResInfo& MediaResInfo)
	{
		WString strText;
		strText << fsutil::GetFileName(MediaResInfo->m_strPath)
			<< L"\n����:  " << MediaResInfo.fileTimeString()
			<< L"      ʱ��:  " << CMedia::genDurationString(CMediaOpaque::checkDuration(MediaResInfo->m_strPath))
			<< L"      ��С:  " << MediaResInfo.fileSizeString();

		cauto fnGenTag = [&](cwstr strPath) {
			auto eFileType = IMedia::GetMediaFileType(fsutil::GetFileExtName(strPath));
			if (E_MediaFileType::MFT_MP3 == eFileType || E_MediaFileType::MFT_FLAC == eFileType)
			{
				tagMediaTag MediaTag;
				if (E_MediaFileType::MFT_MP3 == eFileType)
				{
					CMediaRes::ReadMP3Tag(strPath, MediaTag);
				}
				else
				{
					CMediaRes::ReadFlacTag(strPath, MediaTag);
				}

				strutil::eraseChar(MediaTag.strTitle, '\0');
				strutil::eraseChar(MediaTag.strArtist, '\0');
				strutil::eraseChar(MediaTag.strAlbum, '\0');

				strText << L"\n����:  " << MediaTag.strTitle
					<< L"\n������:  " << MediaTag.strArtist
					<< L"\n��Ƭ��:  " << MediaTag.strAlbum;
			}
		};
		fnGenTag(MediaResInfo->m_strPath);

		cauto strMediaPath = __xmedialib.toAbsPath(MatchMediaInfo->m_strPath, false);
		
		strText << L"\n\n\n�Ƿ����������Ŀ��\n" << MatchMediaInfo->fileName()
			<< L"\n����:  " << MatchMediaInfo.fileTimeString()
			<< L"      ʱ��:  " << CMedia::genDurationString(CMediaOpaque::checkDuration(strMediaPath))
			<< L"      ��С:  " << MatchMediaInfo.fileSizeString();

		fnGenTag(strMediaPath);

		strText << L"\n\nĿ¼:  " << MatchMediaInfo->m_strPath
			<< L"\n������Ŀ:  ";

		MatchMediaInfo.medias()([&](CMedia& media) {
			strText.append(media.m_pParent->GetLogicPath() + L"\n           ");
		});

		int nRet = ProgressDlg.msgBox(strText, L"ƥ�䵽��Ŀ", MB_YESNOCANCEL);
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

	ProgressDlg.SetStatusText(L"������Ŀ...");

	TD_MediaList lstMedias;
	__xmedialib.GetAllMedias(lstMedias);
	__Ensure(lstMedias);

	CFileTitleGuard FileTitleGuard(m_model.getSingerMgr());
	map<wstring, CMatchMediaInfo> mapMatchMediaInfo;
	lstMedias([&](CMedia& media) {
		cauto strMediaPath = media.GetPath();
		//strutil::lowerCase_r(strMediaPath);

		cauto itr = mapMatchMediaInfo.find(strMediaPath);
		if (itr != mapMatchMediaInfo.end())
		{
			itr->second.medias().add(media);
		}
		else
		{
			wstring strSingerName;
			if (E_MediaSetType::MST_Album == media.GetMediaSetType())
			{
				strSingerName = CFileTitleGuard::genCollateSingerName(((CAlbumItem&)media).GetSingerName());
			}
			else
			{
				strSingerName = FileTitleGuard.matchSinger(media);
			}

			mapMatchMediaInfo[strMediaPath] = CMatchMediaInfo(strMediaPath, media, strSingerName);
		}
	});

	prlist<wstring, CMatchMediaInfo> lstMatchResult;
	for (auto& strFile : lstFiles)
	{
		ProgressDlg.SetStatusText((L"�ȶ��ļ�: " + strFile).c_str(), 1);
		if (!ProgressDlg.checkStatus())
		{
			return;
		}

		cauto strSingerName = FileTitleGuard.matchSinger(fsutil::getFileTitle(strFile));
		CMediaResInfo MediaResInfo(strFile, strSingerName);
		for (auto itr = mapMatchMediaInfo.begin(); itr != mapMatchMediaInfo.end(); )
		{
			CMatchMediaInfo& MatchMediaInfo = itr->second;

			if (MatchMediaInfo->match(MediaResInfo.fileTitle()))
			{
				E_MatchResult eRet = cbConfirm(MatchMediaInfo, MediaResInfo);
				if (E_MatchResult::MR_Ignore == eRet)
				{
					//itr = mapMatchMediaInfo.erase(itr);
					//continue;
					break;
				}

				if (E_MatchResult::MR_Yes == eRet)
				{
					lstMatchResult.emplace_back(MediaResInfo->m_strPath, MatchMediaInfo);

					(void)mapMatchMediaInfo.erase(itr);

					break;
				}
			}

			++itr;
		}
	}

	if (!lstMatchResult.empty())
	{
		ProgressDlg.SetProgress(0, lstMatchResult.size());
	}

	SMap<wstring, wstring> mapUpdateFiles;
	map<CMedia*, wstring> mapUpdatedMedias;
	for (auto& pr : lstMatchResult)
	{
		cauto strSrcPath = pr.first;
		cauto strSrcFileName = fsutil::GetFileName(strSrcPath);
		ProgressDlg.SetStatusText((L"�����ļ�: " + strSrcFileName).c_str(), 1);
		if (!ProgressDlg.checkStatus())
		{
			return;
		}

		CMatchMediaInfo& MatchMediaInfo = pr.second;
		cauto srcMediaDir = __xmedialib.toAbsPath(fsutil::GetParentDir(MatchMediaInfo->m_strPath), true);
		cauto strOldPath = srcMediaDir + __wcPathSeparator + fsutil::GetFileName(MatchMediaInfo->m_strPath);
		cauto strNewPath = srcMediaDir + __wcPathSeparator + strSrcFileName;

		m_model.getPlayMgr().moveFile(strOldPath, strNewPath, [&]() {
			(void)fsutil::removeFile(strOldPath);

			if (!fsutil::moveFile(strSrcPath, strNewPath))
			{
				CMainApp::msgBox(L"�����ļ�ʧ��: \n\n\t" + strNewPath);
				return false;
			}

			wstring strOppPath = __xmedialib.toOppPath(strNewPath);
			mapUpdateFiles.set(MatchMediaInfo->m_strPath, strOppPath);

			MatchMediaInfo.medias()([&](CMedia& media) {
				mapUpdatedMedias[&media] = strOppPath;
			});

			return true;
		});
	}

	if (!mapUpdatedMedias.empty())
	{
		__Ensure(m_model.updateMediaPath(mapUpdatedMedias));
		__Ensure(m_model.updateFile(mapUpdateFiles));

		m_model.refreshMediaLib();
	}

	ProgressDlg.SetStatusText((L"ƥ�����, ����" + to_wstring(mapUpdatedMedias.size()) + L"����Ŀ").c_str());
}

bool __view::_exportMedia(CWnd& wnd, cwstr strTitle
	, const function<UINT(CProgressDlg& ProgressDlg, tagExportOption& ExportOption)>& fnExport)
{
	m_ResModule.ActivateResource();

	tagExportOption ExportOption;
	CExportOptionDlg ExportOptionDlg(ExportOption);
	__EnsureReturn(IDOK == ExportOptionDlg.DoModal(), false);

	static CFolderDlgEx FolderDlg;
	WString strExportPath = FolderDlg.Show(strTitle, L"��ѡ�񵼳�λ��", wnd.m_hWnd);
	__EnsureReturn(!strExportPath->empty(), false);

	strExportPath << (ExportOption.bActualMode ? L"/XMusicMirror" : L"/XMusicExport");
	if (!__xmedialib.checkIndependentDir(strExportPath, true))
	{
		CMainApp::msgBox(L"��ѡ�����Ŀ¼����ص�Ŀ¼?", L"������Ŀ", &wnd);
		return false;
	}
	
	ExportOption.strExportPath = strExportPath;

	auto cb = [&](CProgressDlg& ProgressDlg) {
		UINT uCount = fnExport(ProgressDlg, ExportOption);
		if (0 == uCount || ExportOption.lstExportMedias.empty())
		{
			return;
		}

		ProgressDlg.SetProgress(0, uCount);

		UINT uRet = m_model.exportMedia(ExportOption, [&](UINT uProgressOffset, cwstr strDstFile) {
			if (!strDstFile.empty())
			{
				ProgressDlg.SetStatusText(strDstFile.c_str());
			}

			if (uProgressOffset != 0)
			{
				ProgressDlg.ForwardProgress(uProgressOffset);
			}

			if (!ProgressDlg.checkStatus())
			{
				return false;
			}

			return true;
		});

		ProgressDlg.SetStatusText((L"�ɹ������ļ�: " + to_wstring(uRet)).c_str());
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
		//CMainApp::msgBox(L"δ������Ŀ��", L"������Ŀ");
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

	(void)_exportMedia(*pWnd, L"������Ŀ", [&](CProgressDlg& ProgressDlg, tagExportOption& ExportOption) {
		if (ExportOption.bActualMode)
		{
			SMap<wstring, wstring> mapDirs;
			map<wstring, TD_IMediaList> mapMedias;
			lstMedias([&](CMedia& media) {
				auto strDstDir = ExportOption.strExportPath + fsutil::GetParentDir(media.GetPath());
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
				ExportMedia.strDstDir = ExportOption.strExportPath + __wcPathSeparator + pr.first->GetExportName();
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
	
	(void)_exportMedia(*pWnd, L"������Ŀ", [&](CProgressDlg& ProgressDlg, tagExportOption& ExportOption) {
		SMap<wstring, wstring> mapDirs;
		map<wstring, TD_IMediaList> mapMedias;
		lstMedias([&](IMedia& media) {
			auto strDstDir = ExportOption.strExportPath;
			if (ExportOption.bActualMode)
			{
				strDstDir.append(fsutil::GetParentDir(media.GetPath()));
			}
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

	_exportMedia(m_MainWnd, L"����Ŀ¼", [&](CProgressDlg& ProgressDlg, tagExportOption& ExportOption) {
		UINT uCount = 0;
		CPath::scanDir(ProgressDlg.runSignal(), dir, [&](CPath& dir, TD_XFileList& paSubFile) {
			if (!ProgressDlg.checkStatus())
			{
				return;
			}

			uCount += paSubFile.size();

			tagExportMedia ExportMedia;
			ExportMedia.strDstDir = ExportOption.strExportPath;
			if (ExportOption.bActualMode)
			{
				ExportMedia.strDstDir.append(((CMediaRes&)dir).GetPath());
			}
			ExportMedia.paMedias.add(TD_MediaResList(paSubFile));
			ExportOption.lstExportMedias.push_back(ExportMedia);

			for (cauto cueFile : ((CMediaDir&)dir).cuelist().cues())
			{
				ExportMedia.lstCueFiles.push_back(cueFile.filePath());
			}
		});

		return uCount;
	});
}

#define __SnapshotTimeFormat L"%Y.%m.%d_%H.%M.%S"

static const wstring __snapshotExt = L"snapshot";

void __view::snapshotDir(CMediaDir& dir)
{
	tagFileDlgOpt FileDlgOpt;
	FileDlgOpt.strTitle = L"ѡ�񱣴����·��";
	FileDlgOpt.strFilter = L"�����ļ�(*." + __snapshotExt + L")|*." + __snapshotExt + L"|";
	FileDlgOpt.strFileName = dir.GetName() + L'_' + tmutil::formatTime(__SnapshotTimeFormat, time(0));
	static CFileDlgEx fileDlg(FileDlgOpt);

	wstring strFile = fileDlg.ShowSave();
	if (!strFile.empty())
	{
		if (strutil::lowerCase_r(fsutil::GetFileExtName(strFile)) != __snapshotExt)
		{
			strFile.append(__wcDot + __snapshotExt);
		}
		
		_snapshotDir(dir, strFile);
	}
}

void __view::_snapshotDir(CMediaRes& dir, cwstr strOutputFile)
{
	CUTF8TxtWriter TxtWriter;
	if (!TxtWriter.open(strOutputFile, true))
	{
		return;
	}

	bool bGenDuration = &dir != &__medialib;

	auto cb = [&](CProgressDlg& ProgressDlg) {
		wstring strPrfix;

		function<bool(CPath&, JValue&)> fnSnapshot;
		fnSnapshot = [&](CPath& dir, JValue& jRoot) {
			if (!ProgressDlg.checkStatus())
			{
				return false;
			}

			jRoot["name"] = strutil::toUtf8(dir.fileName());

			auto& strDirInfo = wstring(strPrfix.size(), L'-') + dir.fileName();
			if (!TxtWriter.writeln(strDirInfo))
			{
				return false;
			}

			strPrfix.append(L"  ");

			cauto paSubDir = dir.dirs();

			ProgressDlg.SetProgress(0, paSubDir.size() + 1);
			ProgressDlg.SetStatusText((L"��������Ŀ¼����: " + dir.path()).c_str());

			cauto paSubFile = dir.files();
			if (paSubFile)
			{
				JValue& jFiles = jRoot["files"];

				paSubFile([&](XFile& subFile) {
					JValue jFile;
					jFile["name"] = strutil::toUtf8(fsutil::getFileTitle(__fileTitle_r(subFile.fileName())));
					jFile["size"] = ((CMediaRes&)subFile).fileSize();
					if (bGenDuration)
					{
						jFile["duration"] = CMediaOpaque::checkDuration((CMediaRes&)subFile);
					}
					jFiles.append(jFile);

					auto& strFileSize = ((CMediaRes&)subFile).fileSizeString(false);
					auto& strFileInfo = strPrfix + subFile.fileName() + L'\t' + strFileSize;
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
		(*pJRoot)["type"] = ".xmsc";
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

		ProgressDlg.SetStatusText(L"�����ɿ���");
	};

	CProgressDlg ProgressDlg(cb);
	(void)ProgressDlg.DoModal(L"���ɿ���", &m_MainWnd);
}

void __view::_checkSimilarFile(const function<void(CProgressDlg& ProgressDlg, TD_SimilarFile& arrResult)>& fnWork)
{
	TD_SimilarFile arrResult;

	CProgressDlg ProgressDlg([&](CProgressDlg& ProgressDlg) {
		fnWork(ProgressDlg, arrResult);

		if (!arrResult)
		{
			ProgressDlg.SetStatusText(L"�����ɣ�δ���������ļ�");
		}
		else
		{
			ProgressDlg.Close();
		}
	});
	if (IDOK == ProgressDlg.DoModal(L"��������ļ�", &m_MainWnd) && arrResult)
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
		CPath::scanDir(ProgressDlg.runSignal(), dir1, [&](CPath& dir, TD_XFileList& paSubFile) {
			if (!ProgressDlg.checkStatus())
			{
				return;
			}

			lstMediaRes1.add(paSubFile);

			ProgressDlg.SetProgress(0, lstMediaRes1.size());
		});

		if (!lstMediaRes1)
		{
			ProgressDlg.SetProgress(0, 0);

			ProgressDlg.SetStatusText(L"��һ��Ŀ¼δ�����ļ�");
			return;
		}

		UINT uProgress = lstMediaRes1.size();

		TD_MediaResList lstMediaRes2;
		CPath::scanDir(ProgressDlg.runSignal(), dir2, [&](CPath& dir, TD_XFileList& paSubFile) {
			if (!ProgressDlg.checkStatus())
			{
				return;
			}
			
			lstMediaRes2.add(paSubFile);
			
			ProgressDlg.SetProgress(0, uProgress + lstMediaRes2.size());
		});

		if (!lstMediaRes2)
		{
			ProgressDlg.SetProgress(0, 0);

			ProgressDlg.SetStatusText(L"�ڶ���Ŀ¼δ�����ļ�");
			return;
		}

		m_model.checkSimilarFile(lstMediaRes1, lstMediaRes2, [&](CMediaRes& MediaRes) {
			if (!ProgressDlg.checkStatus())
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
		CPath::scanDir(ProgressDlg.runSignal(), dir, [&](CPath& dir, TD_XFileList& paSubFile) {
			if (!ProgressDlg.checkStatus())
			{
				return;
			}
			
			lstMediaRes.add(paSubFile);

			ProgressDlg.SetProgress(0, lstMediaRes.size());
		});

		if (!lstMediaRes)
		{
			ProgressDlg.SetProgress(0, 0);

			ProgressDlg.SetStatusText(L"δ�����ļ�");
			return;
		}

		m_model.checkSimilarFile(lstMediaRes, [&](CMediaRes& MediaRes) {
			if (!ProgressDlg.checkStatus())
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
	E_RelatedMediaSet eRmsType;
	if (E_MediaSetType::MST_Playlist == MediaSetChanged.eMediaSetType)
	{
		eRmsType = E_RelatedMediaSet::RMS_Playlist;
	}
	else if (E_MediaSetType::MST_Album == MediaSetChanged.eMediaSetType)
	{
		eRmsType = E_RelatedMediaSet::RMS_Album;
	}
	else if (E_MediaSetType::MST_Singer == MediaSetChanged.eMediaSetType)
	{
		eRmsType = E_RelatedMediaSet::RMS_Singer;
	}
	else
	{
		return;
	}

	m_MediaResPage.UpdateRelated(eRmsType, MediaSetChanged);

	if (m_PlayItemPage)
	{
		m_PlayItemPage.UpdateRelated(eRmsType, MediaSetChanged);
	}

	if (m_AlbumPage)
	{
		m_AlbumPage.UpdateRelated(eRmsType, MediaSetChanged);
	}

	m_PlayingPage.RefreshList();
}

void __view::hittestMediaSet(CMediaSet& MediaSet, CMedia *pMedia, IMedia *pIMedia)
{
	if (E_MediaSetType::MST_Playlist == MediaSet.m_eType)
	{
		if (!m_PlaylistPage)
		{
			m_PlaylistPage.Active();
		}
		m_PlaylistPage.m_wndList.SelectObject(&MediaSet);

		if (pMedia)
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
		m_AlbumPage.Active();
	}
}

void __view::hittestMedia(CMedia& media)
{
	if (media.m_pParent)
	{
		this->hittestMediaSet(*media.m_pParent, &media);
	}
}

bool __view::hittestRelatedMediaSet(IMedia& media, E_RelatedMediaSet eRmsType)
{
	__waitCursor;

	int nRelatedMediaID = media.GetRelatedMediaID(eRmsType);
	if (nRelatedMediaID > 0)
	{
		CMedia *pRelatedMedia = __xmedialib.FindMedia((E_MediaSetType)eRmsType, (UINT)nRelatedMediaID);
		if (pRelatedMedia)
		{
			hittestMedia(*pRelatedMedia);
			return true;
		}
	}

	UINT uRelatedMediaSetID = media.GetRelatedMediaSetID(eRmsType);
	if (uRelatedMediaSetID > 0)
	{
		CMediaSet *pMediaSet = __xmedialib.FindSubSet((E_MediaSetType)eRmsType, uRelatedMediaSetID);
		__EnsureReturn(pMediaSet, false);

		this->hittestMediaSet(*pMediaSet, NULL, &media);

		return true;
	}

	return false;
}

bool __view::addSingerImage(CSinger& Singer, const list<wstring>& lstFiles)
{
	__EnsureReturn(m_ImgMgr.addSingerImg(Singer.m_uID, Singer.m_strName, lstFiles), false);

	tagMediaSetChanged MediaSetChanged(E_MediaSetType::MST_Singer, Singer.m_uID, E_MediaSetChanged::MSC_SingerImgChanged);
	//MediaSetChanged.uSingerImgPos = m_ImgMgr.getSingerImgPos(Singer.m_uID);
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

BYTE __view::genByteRateAlpha(CMedia& media)
{
	if (media.displayFileSize() > 0 && media.displayDuration() > 0)
	{
		UINT bitRate = UINT(media.displayFileSize() / media.displayDuration());
		if (bitRate < 60000) //ʵ��39kb
		{
			auto uAlpha = BYTE(255 - 255*pow((float)bitRate/60000, 2));
			return uAlpha;
		}
	}

	return 0;
}

void __view::exploreMedia(IMedia& media, bool bSel)
{
	cauto strPath = media.GetAbsPath();

	bool bDir = dynamic_cast<CMediaDir*>(&media) != NULL;
	if (bDir)
	{
		if (!fsutil::existDir(strPath))
		{
			AfxMessageBox(L"Ŀ¼�����ڣ�");
			return;
		}
	}
	else
	{
		if (!fsutil::existFile(strPath))
		{
			AfxMessageBox(L"�ļ������ڣ�");
			return;
		}
	}

	if (bSel || !bDir)
	{
		winfsutil::exploreFile(strPath);
	}
	else
	{
		winfsutil::exploreDir(strPath);
	}
}
