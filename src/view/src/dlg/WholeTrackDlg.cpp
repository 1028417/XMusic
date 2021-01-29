
#include "stdafx.h"

#include "view.h"

#include "WholeTrackDlg.h"

#include "TrackDetailDlg.h"

BEGIN_MESSAGE_MAP(CWholeTrackDlg, CDialog)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST1, &CWholeTrackDlg::OnLvnItemchangedList1)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST1, &CWholeTrackDlg::OnNMDbclickList1)

	ON_BN_CLICKED(IDC_BTN_DETAIL, &CWholeTrackDlg::OnBnClickedViewDetail)
	ON_BN_CLICKED(IDC_BTN_EXPLORE, &CWholeTrackDlg::OnBnClickedBtnExplore)
	ON_BN_CLICKED(IDC_BTN_PLAY, &CWholeTrackDlg::OnBnClickedPlay)
END_MESSAGE_MAP()

void CWholeTrackDlg::DoDataExchange(CDataExchange* pDX)
{
	DDX_Control(pDX, IDC_LIST1, m_wndList);

	CDialog::DoDataExchange(pDX);
}

// CWholeTrackDlg 消息处理程序
BOOL CWholeTrackDlg::OnInitDialog()
{
	__super::OnInitDialog();
	
	CRect rcClient;
	m_wndList.GetClientRect(rcClient);

	CListColumnGuard ColumnGuard(rcClient.Width() - m_view.m_globalSize.m_uScrollbarWidth);
	ColumnGuard.addDynamic(L"Cue", 0.3)
		.addDynamic(L"标题", 0.2)
		.addFix(L"艺术家", 90)
		.addFix(L"音轨数", 70, true)
		.addDynamic(L"目录", 0.2)
		.addDynamic(L"文件", 0.3)
		.addFix(L"大小", 80, true);

	CObjectList::tagListPara ListPara(ColumnGuard);
	ListPara.uItemHeight = 50;
	__AssertReturn(m_wndList.InitCtrl(ListPara), FALSE);

	(void)m_wndList.ModifyStyle(0, LVS_SINGLESEL);

	m_thread.start([&](signal_t bRunSignal) {
		PairList<CMediaRes*, wstring> plUnmatchFile;
		CPath::scanDir(bRunSignal, __medialib, [&](CPath& dir, TD_XFileList& paSubFile) {
			CMediaDir& subDir = (CMediaDir&)dir;

			map<LPCCueFile, CMediaRes*> mapCueFile;
			for (auto pSubFile : paSubFile)
			{
				__yield();

				auto pMediaRes = (CMediaRes*)pSubFile;
				auto& cueFile = pMediaRes->cueFile();
				if (cueFile)
				{
					mapCueFile[cueFile] = pMediaRes;
				}
				else
				{
					if (pMediaRes->fileSize() > 100e6 && __checkDuration(pMediaRes->GetAbsPath()) > 600)
					{
						plUnmatchFile.add(pMediaRes, subDir.GetPath());
					}
				}
			}
			
			for (CRCueFile cueFile : subDir.cuelist().cues())
			{
				if (!bRunSignal)
				{
					return;
				}

				CMediaRes *pMediaRes = NULL;
				auto itr = mapCueFile.find(&cueFile);
				if (itr != mapCueFile.end())
				{
					pMediaRes = itr->second;
				}

				__appSync([&]{
					showCueInfo(subDir.GetPath(), cueFile, pMediaRes);
				});
			}
		});

		if (!bRunSignal)
		{
			return;
		}

		if (plUnmatchFile)
		{
			__appSync([&]{
				plUnmatchFile([&](CMediaRes *pMediaRes, cwstr strDir) {
					showCueInfo(strDir, CCueFile::NoCue, pMediaRes);
				});
			});
		}
	});
	
	return true;
}

void CWholeTrackDlg::showCueInfo(cwstr strDir, CRCueFile cueFile, CMediaRes *pMediaRes)
{
	m_plCueFile.add(&cueFile, pMediaRes);
	
	SVector<wstring> vecText;
	if (cueFile)
	{
		vecText.add(cueFile.m_strCueName, cueFile.m_strTitle, cueFile.m_strPerformer);
	}
	else
	{
		vecText.add(L"", L"", L"");
	}

	if (!cueFile.m_alTrackInfo.empty())
	{
		vecText.add(to_wstring(cueFile.m_alTrackInfo.size()));
	}
	else
	{
		vecText.add(L"");
	}
	
	vecText.add(strDir);

	if (NULL != pMediaRes)
	{
		vecText.add(pMediaRes->GetName(), pMediaRes->fileSizeString(true));
	}

	m_wndList.InsertItemEx(m_plCueFile.size()-1, vecText, L" ");
}

void CWholeTrackDlg::OnLvnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	int nItem = m_wndList.GetSelItem();
	
	(void)this->GetDlgItem(IDC_BTN_EXPLORE)->EnableWindow(nItem >= 0);

	BOOL bPlayable = FALSE;
	UINT uTrackCount = 0;
	if (nItem >= 0)
	{
		m_plCueFile.getFirst((UINT)nItem, [&](LPCCueFile pCueFile) {
			uTrackCount = pCueFile->m_alTrackInfo.size();
		});

		m_plCueFile.getSecond((UINT)nItem, [&](CMediaRes *pMediaRes) {
			if (NULL != pMediaRes)
			{
				m_view.m_MediaResPage.HittestMediaRes(*pMediaRes);

				bPlayable = TRUE;
			}
		});
	}
	
	(void)this->GetDlgItem(IDC_BTN_PLAY)->EnableWindow(bPlayable);
	(void)this->GetDlgItem(IDC_BTN_DETAIL)->EnableWindow(0 != uTrackCount);
}

void CWholeTrackDlg::OnNMDbclickList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;

	int nItem = m_wndList.GetSelItem();
	__Ensure(nItem >= 0);
	
	OnBnClickedPlay();

	OnBnClickedViewDetail();
}

void CWholeTrackDlg::OnBnClickedViewDetail()
{
	int nItem = m_wndList.GetSelItem();
	__Ensure(nItem >= 0);

	m_plCueFile.getFirst((UINT)nItem, [&](LPCCueFile pCueFile) {
		CRCueFile CueFile = *pCueFile;
		if (CueFile)
		{
			m_plCueFile.getSecond((UINT)nItem, [&](CMediaRes *pMediaRes) {
				(void)CTrackDetailDlg(m_view, CueFile, pMediaRes).DoModal();
			});
		}
	});
}

void CWholeTrackDlg::OnBnClickedBtnExplore()
{
	int nItem = m_wndList.GetSelItem();
	__Ensure(nItem >= 0);

	m_plCueFile.get((UINT)nItem, [&](cauto pr) {
		if (NULL != pr.second)
		{
			m_view.exploreMedia(*pr.second);
		}
		else
		{
			winfsutil::exploreFile(pr.first->filePath());
		}
	});
}

void CWholeTrackDlg::OnBnClickedPlay()
{
	int nItem = m_wndList.GetSelItem();
	m_plCueFile.getSecond(nItem, [&](auto pMediaRes) {
		if (pMediaRes)
		{
			m_view.m_PlayCtrl.addPlayingItem(pMediaRes->GetPath());
		}
	});
}

void CWholeTrackDlg::OnCancel()
{
	if (m_thread.joinable())
	{
		CMainApp::GetMainApp()->concurrence([&]{
			m_thread.cancel();
		});
	}

	CDialog::OnCancel();
}
