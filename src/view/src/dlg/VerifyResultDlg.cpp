
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
	__AssertReturn(m_wndList.InitCtrl(ListPara), FALSE);

	(void)m_wndList.ModifyStyle(0, LVS_SINGLESEL);


	CWaitCursor WaitCursor;

	CString strRet;
	for (UINT uIndex = 0; uIndex < m_VerifyResult.vctVerifyResult.size(); uIndex++)
	{
		auto& prResult = m_VerifyResult.vctVerifyResult[uIndex];

		if (-1 == prResult.second)
		{
			strRet = _T("������");
		}
		else if (0 == prResult.second)
		{
			strRet = _T("���ɲ���");
		}
		else
		{
			strRet = _T("����");
		}

		(void)m_wndList.InsertItem(uIndex, strRet);
		(void)m_wndList.SetItemData(uIndex, (DWORD_PTR)(CListObject*)prResult.first);

		UpdateItem(uIndex, *prResult.first);
	}

	(void)m_wndList.SelectFirstItem();

	CString cstrTitle;
	(void)this->GetWindowText(cstrTitle);
	cstrTitle.Format(cstrTitle + "(%d)", m_VerifyResult.vctVerifyResult.size());

	(void)this->SetWindowText(cstrTitle);

	return TRUE;
}

void CVerifyResultDlg::UpdateItem(int nItem, CMedia& Media)
{
	wstringstream ssMediaInfo;
	if (Media.GetMediaSetType() == E_MediaSetType::MST_Album)
	{
		CAlbumItem& AlbumItem = (CAlbumItem&)Media;

		ssMediaInfo << L"����" << __CNDot << AlbumItem.GetSingerName()
			<< __CNDot << AlbumItem.GetAlbumName();
	}
	else
	{
		ssMediaInfo << L"�����б�" << __CNDot << ((CPlayItem&)Media).GetPlaylistName();
	}
	ssMediaInfo << __CNDot << Media.GetName();

	(void)m_wndList.SetItemText(nItem, 1, ssMediaInfo.str().c_str());
	(void)m_wndList.SetItemText(nItem, 2, fsutil::GetParentDir(Media.GetPath()).c_str());
}

void CVerifyResultDlg::OnNMDBLClickList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;
	LPNMLISTVIEW lpNMList = (LPNMLISTVIEW)pNMHDR;
	if (0 <= lpNMList->iItem)
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

int CVerifyResultDlg::VerifyMediaItem(int nItem)
{
	CMedia* pMedia = (CMedia*)m_wndList.GetItemObject(nItem);
	__AssertReturn(pMedia, -1);
	
	CAudioOpaque AudioOpaque(pMedia->GetAbsPath());
	int nDuration = AudioOpaque.checkDuration();
	if (AudioOpaque.fileSize() == -1)
	{
		nDuration = -1;
	}
	
	m_VerifyResult.vctVerifyResult[nItem].second = nDuration;

	wstring strVerifyResult;
	if (-1 == nDuration)
	{
		strVerifyResult = L"������";
	}
	else if (0 == nDuration)
	{
		strVerifyResult = L"���ɲ���";
	}
	else
	{
		strVerifyResult = L"����";
	}

	(void)m_wndList.SetItemText(nItem, 0, strVerifyResult.c_str());

	return nDuration;
}

void CVerifyResultDlg::OnBnClickedVerify()
{
	m_wndList.AsyncTask(50, [&](UINT uItem) {
		this->VerifyMediaItem(uItem);
	});
}

void CVerifyResultDlg::OnBnClickedAutoMatch()
{
	m_view.getModel().refreshMediaLib();

	CMediaDir* pDir = m_view.showChooseDirDlg(L"ѡ�����Ŀ¼", true);
	__Ensure(pDir);

	TD_MediaList lstMedias;
	for (auto& prResult : m_VerifyResult.vctVerifyResult)
	{
		if (prResult.second <= 0)
		{
			lstMedias.add(prResult.first);
		}
	}

	map<CMedia*, wstring> mapUpdateMedias;
	auto cbAutoMatch = [&](CProgressDlg& ProgressDlg) {
		auto cbProgress = [&](const wstring& strDir) {
			ProgressDlg.SetStatusText(strDir.c_str());
			
			return !ProgressDlg.checkCancel();
		};

		auto cbConfirm = [&](CSearchMediaInfo& SearchMediaInfo, tagMediaResInfo& MediaResInfo)
		{
			wstring strText = fsutil::GetFileName(MediaResInfo.strPath)
				+ L"\nĿ¼��" + fsutil::GetParentDir(MediaResInfo.strPath)
				+ L"\n��С��" + MediaResInfo.strFileSize + L"�ֽ�\nʱ����"
				+ CMedia::GetDurationString(MediaResInfo.nDuration)
				+ L"\n\n�Ƿ����������Ŀ��\n"
				+ fsutil::GetFileName(SearchMediaInfo.m_strAbsPath)
				+ L"\nԭĿ¼��" + fsutil::GetParentDir(m_view.getMediaLib().toOppPath(SearchMediaInfo.m_strAbsPath))
				+ L"\n\n������";

			SearchMediaInfo.m_lstMedias([&](CMedia& Media) {
				strText.append(L"\n" + Media.m_pParent->GetLogicPath());
			});

			int iRet = ProgressDlg.showMsgBox(strText.c_str(), L"ƥ�䵽���ļ�", MB_YESNOCANCEL);
			if (IDCANCEL == iRet)
			{
				return E_MatchResult::MR_Ignore;
			}

			__EnsureReturn(IDYES == iRet, E_MatchResult::MR_No);

			ProgressDlg.ForwardProgress();

			return E_MatchResult::MR_Yes;
		};

		__Ensure(m_view.getController().autoMatchMedia(*pDir, lstMedias, cbProgress, cbConfirm, mapUpdateMedias));
		
		ProgressDlg.SetStatusText((L"�Զ�ƥ�����������" + to_wstring(mapUpdateMedias.size()) + L"����Ŀ").c_str());
	};
	CProgressDlg ProgressDlg(cbAutoMatch, lstMedias.size());
	if (IDOK == ProgressDlg.DoModal(L"�Զ�ƥ����Ŀ", this) && !mapUpdateMedias.empty())
	{
		CMedia* pMedia = NULL;
		for (UINT uIndex = 0; uIndex < m_VerifyResult.vctVerifyResult.size(); uIndex++)
		{
			CMedia *pMedia = m_VerifyResult.vctVerifyResult[uIndex].first;

			if (mapUpdateMedias.find(pMedia) != mapUpdateMedias.end())
			{
				m_VerifyResult.lstUpdateMedia.add(pMedia);
			
				UpdateItem(uIndex, *pMedia);

				this->VerifyMediaItem(uIndex);

				(void)CMainApp::GetMainApp()->DoEvents();
			}
		}
	}
}

void CVerifyResultDlg::OnBnClickedLink()
{
	int nItem = m_wndList.GetSelItem();
	__Ensure(0 <= nItem);

	CMedia* pMedia = (CMedia*)m_wndList.GetItemObject(nItem);
	__Assert(pMedia);

	wstring strInitialDir = m_view.getMediaLib().toAbsPath(pMedia->GetBaseDir(), true);

	wstring strDir = pMedia->GetAbsPath();
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
	FileDlgOpt.strTitle = L"�� " + pMedia->GetTitle();
	FileDlgOpt.strFilter = __MediaFilter;
	FileDlgOpt.strInitialDir = strInitialDir;
	FileDlgOpt.strFileName = pMedia->GetName();
	FileDlgOpt.hWndOwner = m_hWnd;
	CFileDlgEx fileDlg(FileDlgOpt);

	wstring strBaseDir = pMedia->GetBaseDir();
	wstring strNewOppPath;

	while (true)
	{
		wstring strNewPath = fileDlg.ShowOpenSingle();
		__Ensure(!strNewPath.empty());

		if (wsutil::matchIgnoreCase(strNewPath, pMedia->GetAbsPath()))
		{
			if (VerifyMediaItem(nItem) <= 0)
			{
				CMainApp::showMsg(L"��ѡ���µ��ļ���", __Title, this);
				continue;
			}
		}

		if (!fsutil::existFile(strNewPath))
		{
			CMainApp::showConfirmMsg(L"�ļ������ڣ�������ѡ��", __Title, this);
			continue;
		}

		strNewOppPath = m_view.getMediaLib().toOppPath(strNewPath);
		if (strNewOppPath.empty())
		{
			CMainApp::showMsg(L"��ѡ���Ŀ¼��Χ�ڵ��ļ�", __Title, this);
			continue;
		}

		if (!strBaseDir.empty())
		{
			if (!fsutil::CheckSubPath(strBaseDir, strNewOppPath))
			{
				CMainApp::showMsg(L"��ѡ������Ŀ¼�е��ļ���\n\n\t" + strBaseDir, __Title, this);
				continue;
			}
		}

		break;
	}

	__Assert(m_view.getModel().updateMediaPath({ { pMedia, strNewOppPath } }));

	UpdateItem(nItem, *pMedia);

	this->VerifyMediaItem(nItem);

	m_VerifyResult.lstUpdateMedia.add(pMedia);
}

void CVerifyResultDlg::OnBnClickedRemove()
{
	int nItem = m_wndList.GetSelItem();
	__Assert(0 <= nItem);

	CMedia* pMedia = (CMedia*)m_wndList.GetItemObject(nItem);
	__Assert(pMedia);

	__Ensure(CMainApp::showConfirmMsg(L"ȷ��ɾ����ѡ��Ŀ?", __Title, this));
	
	m_VerifyResult.vctVerifyResult.erase(m_VerifyResult.vctVerifyResult.begin()+nItem);

	m_VerifyResult.lstRemoveMedia.add(pMedia);

	__Assert(m_view.getModel().removeMedia(TD_MediaList(pMedia)));
	
	(void)m_wndList.DeleteItem(nItem);

	(void)this->GetDlgItem(IDC_BTN_LINK)->EnableWindow(FALSE);
	(void)this->GetDlgItem(IDC_BTN_VERIFY)->EnableWindow(FALSE);
	(void)this->GetDlgItem(IDC_BTN_REMOVE)->EnableWindow(FALSE);
}
