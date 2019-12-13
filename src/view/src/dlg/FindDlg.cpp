// FindDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "FindDlg.h"
#include "VerifyResultDlg.h"

// CFindDlg 对话框

void CFindDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_wndList);
	DDX_Control(pDX, IDC_EDIT1, m_wndEdit);
}

BEGIN_MESSAGE_MAP(CFindDlg, CDialog)
	ON_EN_CHANGE(IDC_EDIT1, &CFindDlg::OnEnChangeEdit1)

	ON_BN_CLICKED(IDC_BTN_PLAY, &CFindDlg::OnBnClickedPlay)

	ON_BN_CLICKED(IDC_BTN_VERIFY, &CFindDlg::OnBnClickedBtnVerify)

	ON_BN_CLICKED(IDC_BTN_EXPLORE, &CFindDlg::OnBnClickedBtnExplore)

	ON_BN_CLICKED(IDC_BTN_EXPORT, &CFindDlg::OnBnClickedBtnExport)

	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST1, &CFindDlg::OnLvnItemchangedList1)
	ON_NOTIFY(NM_CLICK, IDC_LIST1, &CFindDlg::OnNMClickList1)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST1, &CFindDlg::OnNMDblclkList1)
END_MESSAGE_MAP()


// CFindDlg 消息处理程序

BOOL CFindDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CRect rcClient;
	m_wndList.GetClientRect(rcClient);
	CListColumnGuard ColumnGuard(rcClient.Width() - m_view.m_globalSize.m_uScrollbarWidth);

	auto& globalSize = m_view.m_globalSize;
	ColumnGuard.addDynamic(_T("曲目"), 0.35)
		.addFix(_T("类型"), globalSize.m_ColWidth_Type*2)
		.addDynamic(_T("目录"), 0.3)
		.addDynamic(_T("所属歌单"), 0.15)
		.addDynamic(_T("所属专辑"), 0.2)
		.addFix(_T("加入时间"), UINT(globalSize.m_ColWidth_AddTime*1.2));

	CObjectList::tagListPara ListPara(ColumnGuard);
	__AssertReturn(m_wndList.InitCtrl(ListPara), FALSE);

	if (!m_strFindText.empty())
	{
		auto& vecMediaMixtures = FindMedia(m_eFindMediaMode, m_strFindText, m_strFindSingerName);
		if (!vecMediaMixtures)
		{
			CDialog::OnCancel();
			//CMainApp::showMsg(L"未查找到曲目！", L"查找");
			return FALSE;
		}
		if (vecMediaMixtures.size() == 1)
		{
			if (m_bQuickHittest)
			{
				CDialog::OnCancel();				
				return FALSE;
			}
		}

		if (E_FindMediaMode::FMM_MatchText == m_eFindMediaMode)
		{
			(void)m_wndEdit.SetWindowText(m_strFindText.c_str());
		}
	}

	__async([&]() {
		GetDlgItem(IDC_EDIT1)->SetFocus();
	});
	
	return TRUE;
}

void CFindDlg::OnEnChangeEdit1()
{
	__async(300, [&]() {
		CString cstrFind;
		(void)m_wndEdit.GetWindowText(cstrFind);
		(void)cstrFind.Trim();

		if (cstrFind == m_cstrPrevFind)
		{
			return;
		}
		m_cstrPrevFind = cstrFind;

		auto& vecMediaMixtures = FindMedia(E_FindMediaMode::FMM_MatchText, (wstring)cstrFind);
		if (!vecMediaMixtures)
		{
			m_wndEdit.SetFocus();
		}
	});
}

const TD_MediaMixtureVector& CFindDlg::FindMedia(E_FindMediaMode eFindMediaMode, const wstring& strFindText, const wstring& strFindSingerName)
{
	tagFindMediaResult FindResult;

	if (!strFindText.empty())
	{
		CWaitCursor WaitCursor;

		wstring t_strFindText(strFindText);
		if (E_FindMediaMode::FMM_MatchText == eFindMediaMode)
		{
			if (IMedia::GetMediaFileType(fsutil::GetFileExtName(t_strFindText)) != E_MediaFileType::MFT_Null)
			{
				t_strFindText = fsutil::getFileTitle(t_strFindText);
			}
		}

		tagFindMediaPara FindPara(eFindMediaMode, t_strFindText);
		FindPara.strFindSingerName = strutil::lowerCase_r(strFindSingerName);

		if (E_FindMediaMode::FMM_MatchDir == eFindMediaMode)
		{
			FindPara.strDir = strFindText;
		}
		else if (E_FindMediaMode::FMM_MatchPath == eFindMediaMode)
		{
			FindPara.strDir = fsutil::GetParentDir(strFindText);
		}

		m_view.getMediaLib().FindMedia(FindPara, FindResult);
	}

	m_MediaMixer.clear();
	auto& vecMediaMixtures = m_MediaMixer.add(FindResult.lstRetMedias);
	if (vecMediaMixtures.size() == 1)
	{
		vecMediaMixtures.front([&](CMediaMixture& MediaMixture) {
			m_view.hittestMedia(*MediaMixture.GetMedia());
		});
	}

	Refresh();

	return vecMediaMixtures;
}

void CFindDlg::Refresh()
{
	CRedrawLockGuard RedrawGuard(m_wndList);
	(void)m_wndList.DeleteAllItems();

	UINT uItem = 0;
	const TD_MediaMixtureVector& arrMediaMixture = m_MediaMixer.get();
	arrMediaMixture([&](CMediaMixture& MediaMixture) {
		m_wndList.InsertItemEx(uItem++, { MediaMixture.GetTitle()
			, MediaMixture.GetFileTypeString()
			, MediaMixture.GetDir().c_str()
			, MediaMixture.GetPlaylistName()
			, MediaMixture.GetSingerAlbumString()
			, MediaMixture.GetMediaTime().GetText()
		}, L" ");
	});

	CString strTitle;
	strTitle.Format(L"查找结果(%d项)", uItem);
	this->SetWindowText(strTitle);

	(void)this->GetDlgItem(IDC_BTN_PLAY)->EnableWindow(FALSE);

	(void)this->GetDlgItem(IDC_BTN_VERIFY)->EnableWindow(arrMediaMixture);
	(void)this->GetDlgItem(IDC_BTN_EXPORT)->EnableWindow(arrMediaMixture);

	(void)this->GetDlgItem(IDC_BTN_EXPLORE)->EnableWindow(FALSE);
}

void CFindDlg::OnLvnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;

	int nSelCount = m_wndList.GetSelectedCount();

	(void)this->GetDlgItem(IDC_BTN_PLAY)->EnableWindow(nSelCount > 0); 
	(void)this->GetDlgItem(IDC_BTN_EXPLORE)->EnableWindow(1 == nSelCount);
}

void CFindDlg::OnNMClickList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;

	__Ensure(1 == m_wndList.GetSelectedCount());
	__Ensure(!CMainApp::getKeyState(VK_SHIFT) && !CMainApp::getKeyState(VK_CONTROL));

	LPNMLISTVIEW lpNMList = (LPNMLISTVIEW)pNMHDR;

	int iItem = lpNMList->iItem;
	int iSubItem = lpNMList->iSubItem;

	HWND hWndList = m_wndList.m_hWnd;
	m_wndList.AsyncLButtondown([=]() {
		if (!::IsWindow(hWndList))
		{
			return;
		}

		m_MediaMixer.getMediaMixture([&](const CMediaMixture& MediaMixture) {
			auto pPlayItem = MediaMixture.GetPlayItem();
			auto pAlbumItem = MediaMixture.GetAlbumItem();
			if (2 == iSubItem)
			{
				if (NULL != pPlayItem)
				{
					m_view.m_MediaResPage.HittestMediaRes(*pPlayItem, *this);
				}
				else if (NULL != pAlbumItem)
				{
					m_view.m_MediaResPage.HittestMediaRes(*pAlbumItem, *this);
				}
				
				return;
			}

			if (4 == iSubItem || NULL == pPlayItem)
			{
				if (pAlbumItem)
				{
					m_view.hittestMedia(*pAlbumItem);
				
					return;
				}
			}

			if (NULL != pPlayItem)
			{
				m_view.hittestMedia(*pPlayItem);
			}
		}, iItem);
	});
}

void CFindDlg::OnNMDblclkList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;

	LPNMLISTVIEW lpNMList = (LPNMLISTVIEW)pNMHDR;
	__Ensure(lpNMList->iItem >= 0);

	m_wndList.DeselectAll();
	m_wndList.SelectItem(lpNMList->iItem);

	OnBnClickedPlay();
}

void CFindDlg::OnBnClickedPlay()
{
	list<UINT> lstItems;
	m_wndList.GetSelItems(lstItems);

	TD_IMediaList lstMedias;
	for (auto uItem : lstItems)
	{
		m_MediaMixer.getMedia([&](CMedia& media) {
			lstMedias.add(media);
		}, uItem);
	}
	
	m_view.m_PlayCtrl.addPlayingItem(lstMedias);
}

void CFindDlg::OnBnClickedBtnVerify()
{
	m_view.verifyMedia(m_MediaMixer.getMedias(), this, [&](const tagVerifyResult& VerifyResult) {
		bool bFlag = false;

		// 先删策略
		if (VerifyResult.paRemoveMedia)
		{
			bFlag = true;
			m_MediaMixer.remove(VerifyResult.paRemoveMedia);
		}
		
		if (VerifyResult.paUpdateMedia)
		{
			bFlag = true;
			m_MediaMixer.update(VerifyResult.paUpdateMedia);
		}
		
		if (bFlag)
		{
			this->Refresh();
		}
	});
}

void CFindDlg::OnBnClickedBtnExport()
{
	m_view.exportMedia(m_MediaMixer.getMedias(), *this);
}

void CFindDlg::OnBnClickedBtnExplore()
{
	int iItem = m_wndList.GetSelItem();
	if (iItem >= 0)
	{
		m_MediaMixer.getMedia([&](CMedia& media) {
			media.ShellExplore();
		}, (UINT)iItem);
	}
}
