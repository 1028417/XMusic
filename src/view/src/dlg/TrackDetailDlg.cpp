#include "stdafx.h"

#include "view.h"

#include "TrackDetailDlg.h"

CTrackDetailDlg::CTrackDetailDlg(__view& view, CRCueFile cueFile, CMediaRes *pMediaRes)
	: m_view(view)
	, m_cueFile(cueFile)
	, m_pMediaRes(pMediaRes)
{
}

BEGIN_MESSAGE_MAP(CTrackDetailDlg, CDialog)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST1, &CTrackDetailDlg::OnNMDbclickList1)

	ON_BN_CLICKED(ID_BTN_PLAY, &CTrackDetailDlg::OnBnClickedPlay)
END_MESSAGE_MAP()

void CTrackDetailDlg::DoDataExchange(CDataExchange* pDX)
{
	DDX_Control(pDX, IDC_LIST1, m_wndList);

	CDialog::DoDataExchange(pDX);
}

BOOL CTrackDetailDlg::OnInitDialog()
{
	__super::OnInitDialog();

	wstring strTitle = L"整轨 - " + m_cueFile.m_strFileName + L" - ";
	if (NULL != m_pMediaRes)
	{
		strTitle.append(m_pMediaRes->GetName());
	}
	else
	{
		strTitle.append(m_cueFile.m_strTitle);

		this->GetDlgItem(ID_BTN_PLAY)->EnableWindow(FALSE);
	}
	this->SetWindowText(strTitle.c_str());

	CRect rcClient;
	m_wndList.GetClientRect(rcClient);

	CListColumnGuard ColumnGuard(rcClient.Width() - m_view.m_globalSize.m_uScrollbarWidth);
	ColumnGuard.addDynamic(L"标题", 1.0)
		.addFix(L"艺术家", 120)
		.addFix(L"时长", 80);

	CObjectList::tagListPara ListPara(ColumnGuard);
	ListPara.uItemHeight = 36;
	__AssertReturn(m_wndList.InitCtrl(ListPara), FALSE);

	m_wndList.ModifyStyle(0, LVS_SINGLESEL);

	CMainApp::async([&]() {
		m_cueFile.m_alTrackInfo([&](CRTrackInfo trackInfo, UINT uIdx) {
			SVector<wstring> vecText{ trackInfo.strTitle, trackInfo.strPerformer };

			UINT uTrackDuration = 0;
			if (!m_cueFile.m_alTrackInfo.get(uIdx + 1, [&](CRTrackInfo trackInfoNext) {
				uTrackDuration = (trackInfoNext.uMsBegin - trackInfo.uMsBegin) / 1000;
			}))
			{
				if (NULL != m_pMediaRes)
				{
					UINT uDuration = CMediaOpaque::checkDuration(*m_pMediaRes);
					if (uDuration > 0)
					{
						uTrackDuration = uDuration - trackInfo.uMsBegin / 1000;
					}
				}
			}

			if (0 != uTrackDuration)
			{
				wchar_t pszDuration[8] {0};
				swprintf(pszDuration, sizeof(pszDuration), L"%02u:%02u", uTrackDuration / 60, uTrackDuration % 60);
				vecText.add(pszDuration);
			}

			m_wndList.InsertItemEx(uIdx++, vecText, L" ");
		});
	});

	return TRUE;
}

void CTrackDetailDlg::OnNMDbclickList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;

	OnBnClickedPlay();
}

void CTrackDetailDlg::OnBnClickedPlay()
{
	if (NULL != m_pMediaRes)
	{
		m_view.getPlayMgr().clear();
		m_view.m_PlayCtrl.addPlayingItem(TD_IMediaList(m_pMediaRes));

		int iItem = m_wndList.GetSelItem();
		if (iItem >= 1)
		{
			m_cueFile.m_alTrackInfo.get((UINT)iItem, [&](CRTrackInfo trackInfo) {
				m_view.m_PlayCtrl.seek(trackInfo.uMsBegin / 1000);
			});
		}
	}
}
