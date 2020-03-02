
#include "stdafx.h"

#include "VerifyResultDlg.h"

#include "MediaSetDlg.h"

#include "FindDlg.h"

#include "ChooseDirDlg.h"

#define __Title _T("曲目检测")

// VerifyResultDlg

void CVerifyResultDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_wndList);
}


BEGIN_MESSAGE_MAP(CVerifyResultDlg, CDialog)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST1, &CVerifyResultDlg::OnLvnItemchangedList1)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST1, &CVerifyResultDlg::OnNMDBLClickList1)
	ON_BN_CLICKED(IDC_BTN_AutoMatch, &CVerifyResultDlg::OnBnClickedAutoMatch)
	ON_BN_CLICKED(IDC_BTN_LINK, &CVerifyResultDlg::OnBnClickedLink)
	ON_BN_CLICKED(IDC_BTN_VERIFY, &CVerifyResultDlg::OnBnClickedVerify)
	ON_BN_CLICKED(IDC_BTN_REMOVE, &CVerifyResultDlg::OnBnClickedRemove)
END_MESSAGE_MAP()


// CVerifyResultDlg 消息处理程序

BOOL CVerifyResultDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	CObjectList::tagListPara ListPara({ { _T("结果"), 80 },{ _T("曲目"), 550 },{ _T("目录"), 380 } });
	__AssertReturn(m_wndList.InitCtrl(ListPara), FALSE);

	(void)m_wndList.ModifyStyle(0, LVS_SINGLESEL);


	CWaitCursor WaitCursor;

	CString strRet;
	m_VerifyResult.paInvalidMedia([&](CMedia& media, UINT uIdx) {
		(void)m_wndList.InsertItem(uIdx, L"");

		UpdateItem(uIdx, media);
	});

	m_wndList.SelectFirstItem();

	CString cstrTitle;
	(void)this->GetWindowText(cstrTitle);
	cstrTitle.Format(cstrTitle + "(%d)", m_VerifyResult.paInvalidMedia.size());

	(void)this->SetWindowText(cstrTitle);

	return TRUE;
}

void CVerifyResultDlg::UpdateItem(int nItem, CMedia& media)
{
	CString cstrStatus;
	if (media.fileSize() < 0)
	{
		cstrStatus.Append(_T("不存在"));
	}
	else if (media.duration() == 0)
	{
		cstrStatus.Append(_T("不可播放"));
	}
	else
	{
		cstrStatus.Append(_T("正常"));
	}
	(void)m_wndList.SetItemText(nItem, 0, cstrStatus);

	WString strMediaInfo;
	if (media.GetMediaSetType() == E_MediaSetType::MST_Album)
	{
		CAlbumItem& AlbumItem = (CAlbumItem&)media;

		strMediaInfo << L"歌手" << __CNDot << AlbumItem.GetSingerName()
			<< __CNDot << AlbumItem.GetAlbumName();
	}
	else
	{
		strMediaInfo << L"歌单" << __CNDot << ((CPlayItem&)media).GetPlaylistName();
	}
	strMediaInfo << __CNDot << media.GetName();

	(void)m_wndList.SetItemText(nItem, 1, strMediaInfo->c_str());
	(void)m_wndList.SetItemText(nItem, 2, fsutil::GetParentDir(media.GetPath()).c_str());
}

void CVerifyResultDlg::OnNMDBLClickList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;
	LPNMLISTVIEW lpNMList = (LPNMLISTVIEW)pNMHDR;
	if (lpNMList->iItem >= 0)
	{
		OnBnClickedLink();
	}
}

void CVerifyResultDlg::OnLvnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;

	BOOL bEnable = (m_wndList.GetSelItem()>=0);
	
	(void)this->GetDlgItem(IDC_BTN_LINK)->EnableWindow(bEnable);
	(void)this->GetDlgItem(IDC_BTN_REMOVE)->EnableWindow(bEnable);

	(void)this->GetDlgItem(IDC_BTN_AutoMatch)->EnableWindow(m_wndList.GetItemCount()>0);
	(void)this->GetDlgItem(IDC_BTN_VERIFY)->EnableWindow(m_wndList.GetItemCount()>0);
}

void CVerifyResultDlg::OnBnClickedVerify()
{
	m_wndList.AsyncTask(__AsyncTaskElapse, [&](UINT uItem) {
		m_VerifyResult.paInvalidMedia.get(uItem, [&](CMedia& media) {
			(void)media.checkDuration();
			UpdateItem(uItem, media);
		});
	});
}

void CVerifyResultDlg::OnBnClickedAutoMatch()
{
	m_view.getModel().refreshMediaLib();
	CMediaDir* pDir = m_view.showChooseDirDlg(L"选择查找目录");
	__Ensure(pDir);

	TD_MediaList paInvalidMedia = m_VerifyResult.paInvalidMedia.filter([](CMedia& media) {
		return media.duration() == 0;
	});

	map<CMedia*, wstring> mapUpdateMedias;
	auto cbAutoMatch = [&](CProgressDlg& ProgressDlg) {
		auto cbProgress = [&](const wstring& strDir) {
			ProgressDlg.SetStatusText(strDir.c_str());
			
			return !ProgressDlg.checkCancel();
		};

		auto cbConfirm = [&](CSearchMediaInfo& SearchMediaInfo, CMediaResInfo& MediaResInfo)
		{
			WString strText;
			strText << fsutil::GetFileName(MediaResInfo->m_strPath)
				<< L"\n目录: " << fsutil::GetParentDir(MediaResInfo->m_strPath)
				<< L"\n大小: " << MediaResInfo.fileSizeString()
				<< L"\n时长: " << CMedia::genDurationString(MediaResInfo.m_uDuration)
				<< L"\n\n是否更新以下曲目？\n" << SearchMediaInfo->fileName()
				<< L"\n原目录: " << SearchMediaInfo->dir()
				+ L"\n\n关联: ";

			SearchMediaInfo.medias()([&](CMedia& media) {
				strText.append(L"\n" + media.m_pParent->GetLogicPath());
			});

			int nRet = ProgressDlg.msgBox(strText->c_str(), L"匹配到新文件", MB_YESNOCANCEL);
			if (IDCANCEL == nRet)
			{
				return E_MatchResult::MR_Ignore;
			}
			else if (IDNO == nRet)
			{
				return E_MatchResult::MR_No;
			}

			ProgressDlg.ForwardProgress();

			return E_MatchResult::MR_Yes;
		};

		__Ensure(m_view.getController().autoMatchMedia(*pDir, paInvalidMedia, cbProgress, cbConfirm, mapUpdateMedias));
		
		ProgressDlg.SetStatusText((L"自动匹配结束，更新" + to_wstring(mapUpdateMedias.size()) + L"个曲目").c_str());
	};
	CProgressDlg ProgressDlg(cbAutoMatch, paInvalidMedia.size());
	if (IDOK == ProgressDlg.DoModal(L"自动匹配曲目", this) && !mapUpdateMedias.empty())
	{
		CMedia* pMedia = NULL;
		m_VerifyResult.paInvalidMedia([&](CMedia& media, UINT uIdx) {
			if (mapUpdateMedias.find(&media) != mapUpdateMedias.end())
			{
				m_VerifyResult.paUpdateMedia.add(media);

				(void)media.checkDuration();
				UpdateItem(uIdx, media);

				(void)CMainApp::GetMainApp()->DoEvents();
			}
		});
	}
}

void CVerifyResultDlg::OnBnClickedLink()
{
	int nItem = m_wndList.GetSelItem();
	__Ensure(nItem >= 0);

	m_VerifyResult.paInvalidMedia.get((UINT)nItem, [&](CMedia& media) {
		LinkMedia(nItem, media);
	});
}

void CVerifyResultDlg::LinkMedia(int nItem, CMedia& media)
{
	wstring strInitialDir = m_view.getMediaLib().toAbsPath(media.GetBaseDir(), true);

	wstring strDir = media.GetAbsPath();
	while (!strDir.empty())
	{
		strDir = fsutil::GetParentDir(strDir);

		if (fsutil::existDir(strDir))
		{
			strInitialDir = strDir;
			break;
		}
	}

	tagFileDlgOpt FileDlgOpt;
	FileDlgOpt.strTitle = L"打开 " + media.GetTitle();
	FileDlgOpt.strFilter = __MediaFilter;
	FileDlgOpt.strInitialDir = strInitialDir;
	FileDlgOpt.strFileName = media.GetName();
	FileDlgOpt.hWndOwner = m_hWnd;
	CFileDlgEx fileDlg(FileDlgOpt);

	wstring strBaseDir = media.GetBaseDir();
	wstring strNewOppPath;

	while (true)
	{
		wstring strNewPath = fileDlg.ShowOpenSingle();
		__Ensure(!strNewPath.empty());

		strNewOppPath = m_view.getMediaLib().toOppPath(strNewPath);
		if (strNewOppPath.empty())
		{
			CMainApp::msgBox(L"请选择根目录范围内的文件", __Title, this);
			continue;
		}

		if (!strBaseDir.empty())
		{
			if (!fsutil::CheckSubPath(strBaseDir, strNewOppPath))
			{
				CMainApp::msgBox(L"请选择以下目录中的文件: \n\n\t" + strBaseDir, __Title, this);
				continue;
			}
		}

		if (!fsutil::existFile(strNewPath))
		{
			CMainApp::confirmBox(L"文件不存在，请重新选择！", __Title, this);
			continue;
		}

		if (strutil::matchIgnoreCase(strNewPath, media.GetAbsPath()))
		{
			if (media.checkDuration() == 0)
			{
				CMainApp::msgBox(L"请选择新的文件！", __Title, this);
				continue;
			}
		}

		break;
	}

	__Assert(m_view.getModel().updateMediaPath({ { &media, strNewOppPath } }));

	(void)media.checkDuration();
	UpdateItem(nItem, media);

	m_VerifyResult.paUpdateMedia.add(media);
}

void CVerifyResultDlg::OnBnClickedRemove()
{
	int nItem = m_wndList.GetSelItem();
	__Assert(nItem >= 0);

	m_VerifyResult.paInvalidMedia.get((UINT)nItem, [&](CMedia& media) {
		__Ensure(CMainApp::confirmBox(L"确认删除所选曲目?", __Title, this));

		m_VerifyResult.paInvalidMedia.erase(m_VerifyResult.paInvalidMedia.begin() + nItem);
		m_VerifyResult.paRemoveMedia.add(media);

		(void)m_wndList.DeleteItem(nItem);
		(void)this->GetDlgItem(IDC_BTN_LINK)->EnableWindow(FALSE);
		(void)this->GetDlgItem(IDC_BTN_VERIFY)->EnableWindow(FALSE);
		(void)this->GetDlgItem(IDC_BTN_REMOVE)->EnableWindow(FALSE);

		__Assert(m_view.getModel().removeMedia(TD_MediaList(media)));
	});
}
