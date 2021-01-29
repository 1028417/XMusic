
#include "stdafx.h"

#include "VerifyResultDlg.h"

#include "MediaSetDlg.h"

#include "FindDlg.h"

#include "ChooseDirDlg.h"

#define __Title _T("��Ŀ���")

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


// CVerifyResultDlg ��Ϣ�������

BOOL CVerifyResultDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	CObjectList::tagListPara ListPara({ { _T("���"), 80 },{ _T("��Ŀ"), 550 },{ _T("Ŀ¼"), 380 } });
	ListPara.uItemHeight = 40;

	__AssertReturn(m_wndList.InitCtrl(ListPara), FALSE);

	(void)m_wndList.ModifyStyle(0, LVS_SINGLESEL);


	__waitCursor;

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
	if (media.fileSize() == 0)
	{
		cstrStatus.Append(_T("������"));
	}
	else if (media.duration() == 0)
	{
		cstrStatus.Append(_T("���ɲ���"));
	}
	else
	{
		cstrStatus.Append(_T("����"));
	}
	(void)m_wndList.SetItemText(nItem, 0, cstrStatus);

	WString strMediaInfo;
	if (media.type() == E_MediaType::MT_AlbumItem)
	{
		CAlbumItem& AlbumItem = (CAlbumItem&)media;

		strMediaInfo << L"����" << __CNDot << AlbumItem.GetSingerName()	<< __CNDot << AlbumItem.GetAlbumName();
	}
	else
	{
		strMediaInfo << L"�赥" << __CNDot << ((CPlayItem&)media).GetPlaylistName();
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
	m_wndList.AsyncTask(__AsyncTaskElapse + m_wndList.GetItemCount()/10, [&](UINT uItem) {
		m_VerifyResult.paInvalidMedia.get(uItem, [&](CMedia& media) {
			__checkMedia(media);
			UpdateItem(uItem, media);
		});
	});
}

void CVerifyResultDlg::OnBnClickedAutoMatch()
{
	m_view.getModel().refreshMediaLib();
	CMediaDir *pDir = m_view.showChooseDirDlg(L"ѡ�����Ŀ¼");
	__Ensure(pDir);

	TD_MediaList paInvalidMedia = m_VerifyResult.paInvalidMedia.filter([](CMedia& media) {
		return media.duration() == 0;
	});

	map<CMedia*, wstring> mapUpdateMedias;
	auto cbAutoMatch = [&](CProgressDlg& ProgressDlg) {
		auto cbProgress = [&](cwstr strDir) {
			ProgressDlg.SetStatusText(strDir);			
			return ProgressDlg.checkStatus();
		};

		auto cbConfirm = [&](CMatchMediaInfo& MatchMediaInfo, CMediaResInfo& MediaResInfo)
		{
			WString strText;
			strText << fsutil::GetFileName(MediaResInfo->m_strPath)
				<< L"\nĿ¼:  " << fsutil::GetParentDir(MediaResInfo->m_strPath)
				<< L"\n��С:  " << MediaResInfo.fileSizeString() << L"      ʱ��:  " << CMedia::genDurationString(MediaResInfo.m_uDuration)
				<< L"\n\n�Ƿ����������Ŀ��\n" << MatchMediaInfo->fileName()
				<< L"\nԭĿ¼:  " << MatchMediaInfo->dir()
				+ L"\n\n������Ŀ:  ";

			for (auto pMedia : MatchMediaInfo.medias())
			{
				strText << '\n' << pMedia->m_pParent->GetLogicPath();
				strText << L"\n��С:  " << pMedia->displayFileSizeString(false) << L"      ʱ��:  " << pMedia->displayDurationString();
			}

			int nRet = ProgressDlg.msgBox(strText->c_str(), L"ƥ�䵽���ļ�", MB_YESNOCANCEL);
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
		
		cauto strTip = L"�Զ�ƥ����������� " + to_wstring(mapUpdateMedias.size()) + L" ��Ŀ";
		ProgressDlg.SetStatusText(strTip);
	};

	if (!m_view.showProgressDlg(L"�Զ�ƥ����Ŀ", cbAutoMatch, paInvalidMedia.size(), this))
	{
		return;
	}
	if (mapUpdateMedias.empty())
	{
		return;
	}
	CMedia* pMedia = NULL;
	m_VerifyResult.paInvalidMedia([&](CMedia& media, UINT uIdx) {
		if (mapUpdateMedias.find(&media) != mapUpdateMedias.end())
		{
			m_VerifyResult.paUpdateMedia.add(media);

			__checkMedia(media);
			UpdateItem(uIdx, media);

			(void)__app->DoEvents();
		}
	});
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
	cauto strBaseDir = media.GetBaseDir();

	wstring strInitialDir = __xmedialib.toAbsPath(media.GetDir(), true);
	if (!fsutil::existDir(strInitialDir))
	{
		if (!strBaseDir.empty())
		{
			cauto strBaseAbsDir = __xmedialib.toAbsPath(strBaseDir, true);
			if (!fsutil::existDir(strBaseAbsDir))
			{
				msgBox(L"����Ŀ¼�����ڣ�");
				return;
			}

			strInitialDir = strBaseAbsDir;
		}
		else
		{
			strInitialDir = __medialib.path();
		}
	}

	tagFileDlgOpt FileDlgOpt;
	FileDlgOpt.strTitle = L"�� " + media.GetTitle();
	FileDlgOpt.strFilter = __MediaFilter;
	FileDlgOpt.strInitialDir = strInitialDir;
	FileDlgOpt.strFileName = media.GetName();
	FileDlgOpt.hWndOwner = m_hWnd;
	CFileDlgEx fileDlg(FileDlgOpt);

	wstring strNewOppPath;
	while (true)
	{
		wstring strNewPath = fileDlg.ShowOpenSingle();
		__Ensure(!strNewPath.empty());

		strNewOppPath = __xmedialib.toOppPath(strNewPath);
		if (strNewOppPath.empty())
		{
			msgBox(L"��ѡ���Ŀ¼��Χ�ڵ��ļ�", __Title);
			continue;
		}

		if (!strBaseDir.empty())
		{
			if (!fsutil::CheckSubPath(strBaseDir, strNewOppPath))
			{
				msgBox(L"��ѡ������Ŀ¼�е��ļ�: \n\n\t" + strBaseDir, __Title);
				continue;
			}
		}

		if (!fsutil::existFile(strNewPath))
		{
			confirmBox(L"�ļ������ڣ�������ѡ��", __Title);
			continue;
		}

		if (strutil::matchIgnoreCase(strNewPath, media.GetAbsPath()))
		{
			if (__checkMedia(media) == 0)
			{
				msgBox(L"��ѡ���µ��ļ���", __Title);
				continue;
			}
		}

		break;
	}

	__Assert(m_view.getModel().updateMediaPath({ { &media, strNewOppPath } }));

	__checkMedia(media);
	UpdateItem(nItem, media);

	m_VerifyResult.paUpdateMedia.add(media);
}

void CVerifyResultDlg::OnBnClickedRemove()
{
	int nItem = m_wndList.GetSelItem();
	__Assert(nItem >= 0);

	m_VerifyResult.paInvalidMedia.get((UINT)nItem, [&](CMedia& media) {
		__Ensure(confirmBox(L"ȷ��ɾ����ѡ��Ŀ?", __Title));

		m_VerifyResult.paInvalidMedia.erase(m_VerifyResult.paInvalidMedia.begin() + nItem);
		m_VerifyResult.paRemoveMedia.add(media);

		(void)m_wndList.DeleteItem(nItem);
		(void)this->GetDlgItem(IDC_BTN_LINK)->EnableWindow(FALSE);
		(void)this->GetDlgItem(IDC_BTN_VERIFY)->EnableWindow(FALSE);
		(void)this->GetDlgItem(IDC_BTN_REMOVE)->EnableWindow(FALSE);

		__Assert(m_view.getModel().removeMedia(TD_MediaList(media)));
	});
}
