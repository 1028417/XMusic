
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
		.addFix(L"音轨数", 70)
		.addDynamic(L"目录", 0.2)
		.addDynamic(L"文件", 0.3)
		.addFix(L"大小", 80);

	CObjectList::tagListPara ListPara(ColumnGuard);
	ListPara.uItemHeight = 50;
	__AssertReturn(m_wndList.InitCtrl(ListPara), FALSE);

	(void)m_wndList.ModifyStyle(0, LVS_SINGLESEL);

	m_bCancel = false;
	m_thread = thread([&]() {
		PairList<CMediaRes*, wstring> plUnmatchFile;

		m_view.getRootMediaRes().enumSubFile([&](CPath& dir, TD_PathList& lstSubFile) {
			mtutil::usleep(1);
			if (m_bCancel)
			{
				return false;
			}

			CMediaRes& subDir = (CMediaRes&)dir;

			map<LPCCueFile, CMediaRes*> mapCueFile;
			for (auto pSubFile : lstSubFile)
			{
				mtutil::yield();

				auto pMediaRes = (CMediaRes*)pSubFile;
				auto& cueFile = pMediaRes->getCueFile();
				if (cueFile)
				{
					mapCueFile[cueFile] = pMediaRes;
				}
				else
				{
					if (pMediaRes->GetFileSize() > 100e6 && CFileOpaqueEx::checkDuration(*pMediaRes) > 600)
					{
						plUnmatchFile.add(pMediaRes, subDir.GetPath());
					}
				}
			}
			
			for (CRCueFile cueFile : subDir.SubCueList())
			{
				if (m_bCancel)
				{
					return false;
				}

				CMediaRes *pMediaRes = NULL;
				auto itr = mapCueFile.find(&cueFile);
				if (itr != mapCueFile.end())
				{
					pMediaRes = itr->second;
				}

				CMainApp::sync([&]() {
					showCueInfo(subDir.GetPath(), cueFile, pMediaRes);
				});
			}

			return true;
		});

		if (plUnmatchFile)
		{
			CMainApp::sync([&]() {
				plUnmatchFile([&](CMediaRes *pMediaRes, wstring& strDir) {
					showCueInfo(strDir, CCueFile::NoCue, pMediaRes);
				});
			});
		}
	});
	
	return true;
}

void CWholeTrackDlg::showCueInfo(const wstring& strDir, CRCueFile cueFile, CMediaRes *pMediaRes)
{
	m_plCueFile.add(&cueFile, pMediaRes);
	
	SVector<wstring> vecText;
	if (cueFile)
	{
		vecText.add(cueFile.m_strFileName, cueFile.m_strTitle, cueFile.m_strPerformer);
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
		vecText.add(pMediaRes->GetName());

		(void)pMediaRes->AsyncTask();
		vecText.add(pMediaRes->GetFileSizeString());
	}

	m_wndList.InsertItemEx(m_plCueFile.size()-1, vecText, L" ");
}

void CWholeTrackDlg::OnLvnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	int iItem = m_wndList.GetSelItem();
	
	(void)this->GetDlgItem(IDC_BTN_EXPLORE)->EnableWindow(iItem >= 0);

	BOOL bPlayable = FALSE;
	UINT uTrackCount = 0;
	if (iItem >= 0)
	{
		m_plCueFile.getFirst((UINT)iItem, [&](LPCCueFile pCueFile) {
			uTrackCount = pCueFile->m_alTrackInfo.size();
		});

		m_plCueFile.getSecond((UINT)iItem, [&](CMediaRes *pMediaRes) {
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

	OnBnClickedPlay();

	OnBnClickedViewDetail();
}

void CWholeTrackDlg::OnBnClickedViewDetail()
{
	int iItem = m_wndList.GetSelItem();
	__Ensure(iItem >= 0);

	m_plCueFile.getFirst((UINT)iItem, [&](LPCCueFile pCueFile) {
		CRCueFile CueFile = *pCueFile;
		if (CueFile)
		{
			m_plCueFile.getSecond((UINT)iItem, [&](CMediaRes *pMediaRes) {
				(void)CTrackDetailDlg(m_view, CueFile, pMediaRes).DoModal();
			});
		}
	});
}

void CWholeTrackDlg::OnBnClickedBtnExplore()
{
	int iItem = m_wndList.GetSelItem();
	__Ensure(iItem >= 0);

	m_plCueFile.getSecond((UINT)iItem, [&](auto pMediaRes) {
		if (NULL != pMediaRes)
		{
			pMediaRes->ShellExplore();
		}
		else
		{
			wstring strDir = m_wndList.GetItemText(iItem, 4);
			wsutil::trim(strDir);

			wstring strCueFile =  m_wndList.GetItemText(iItem, 0);
			wsutil::trim(strCueFile);

			wstring strCueFilePath = m_view.getRootMediaRes().toAbsPath(strDir, true) + __wcBackSlant + strCueFile;
			winfsutil::exploreFile(strCueFilePath);
		}
	});
}

void CWholeTrackDlg::OnBnClickedPlay()
{
	int iItem = m_wndList.GetSelItem();
	m_plCueFile.getSecond(iItem, [&](auto pMediaRes) {
		if (NULL != pMediaRes)
		{
			TD_IMediaList lstMedias(pMediaRes);
			m_view.m_PlayCtrl.addPlayingItem(lstMedias);
		}
	});
}

void CWholeTrackDlg::OnCancel()
{
	m_bCancel = true;

	if (m_thread.joinable())
	{
		CMainApp::GetMainApp()->thread([&]() {
			m_thread.join();
		});
	}

	CDialog::OnCancel();
}
