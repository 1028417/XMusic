
#include "stdafx.h"

#include "PlayRecordDlg.h"

CPlayRecordDlg::CPlayRecordDlg(__view& view)
	: m_view(view)
{
}

void CPlayRecordDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_DATETIMEPICKER, m_wndDateTimeCtrl);
	DDX_Control(pDX, IDC_LIST1, m_wndList);
}

BEGIN_MESSAGE_MAP(CPlayRecordDlg, CDialog)
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_DATETIMEPICKER, &CPlayRecordDlg::OnDtnDatetimechange)
	
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST1, &CPlayRecordDlg::OnLvnItemchangedList1)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST1, &CPlayRecordDlg::OnNMDblclkList1)

	ON_BN_CLICKED(IDC_BTN_PLAY, &CPlayRecordDlg::OnBnClickedPlay)
	ON_BN_CLICKED(IDC_BTN_CLEAR, &CPlayRecordDlg::OnBnClickedClear)
END_MESSAGE_MAP()

// CPlayRecordDlg 消息处理程序

BOOL CPlayRecordDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CRect rcClient;
	m_wndList.GetClientRect(rcClient);
	CListColumnGuard ColumnGuard(rcClient.Width() - m_view.m_globalSize.m_uScrollbarWidth);
	ColumnGuard.addFix(_T("时间"), 160, true)
		.addDynamic(_T("文件"), 1.0);

	CObjectList::tagListPara ListPara(ColumnGuard);

	ListPara.nHeaderHeight = 0;

	__AssertReturn(m_wndList.InitCtrl(ListPara), FALSE);

	dbtime_t maxTime = 0;
	__EnsureReturn(m_view.getDataMgr().queryPlayRecordMaxTime(maxTime), FALSE);
	if (0 == maxTime)
	{
		maxTime = __mediatime;
	}

	tagTM tm;
	tmutil::timeToTM(maxTime, tm);
	CTime timeMax(tm.tm_year, tm.tm_mon, tm.tm_mday, 0, 0, 0);

	CTime time0;
	m_wndDateTimeCtrl.SetRange(&time0, &timeMax);

	__EnsureReturn(this->Refresh(), FALSE);

	return TRUE;
}

BOOL CPlayRecordDlg::Refresh()
{
	CWaitCursor WaitCursor;

	CTime time;
	(void)m_wndDateTimeCtrl.GetTime(time);

	CTime fiterTime(time.GetYear(), time.GetMonth(), time.GetDay(), 0, 0, 0);

	vector<pair<wstring, int>> vecPlayRecord;
	__EnsureReturn(m_view.getDataMgr().queryPlayRecord((dbtime_t)fiterTime.GetTime(), vecPlayRecord), FALSE);

	GetDlgItem(IDC_BTN_PLAY)->EnableWindow(FALSE);

	CRedrawLock RedrawGuard(m_wndList);
	(void)m_wndList.DeleteAllItems();

	m_vctPlayRecord.clear();

	wstring strTitle(L"播放记录");

	if (!vecPlayRecord.empty())
	{
		for (auto& pr : vecPlayRecord)
		{
			m_vctPlayRecord.emplace_back(pr.first, pr.second);

			(void)m_wndList.InsertObject(m_vctPlayRecord.back());
		}

		(void)m_wndList.EnsureVisible(m_vctPlayRecord.size() - 1, FALSE);
		
		strTitle.append(L"(" + to_wstring(vecPlayRecord.size()) + L"项)");
	}
	
	this->SetWindowText(strTitle.c_str());

	return TRUE;
}

void CPlayRecordDlg::OnDtnDatetimechange(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;

	CTime time;
	m_wndDateTimeCtrl.GetTime(time);
	time_t tTime = time.GetTime();
	if (tTime == m_tFilterTime)
	{
		return;
	}

	m_tFilterTime= tTime;

	this->Refresh();
}

void CPlayRecordDlg::OnLvnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;

	(void)this->GetDlgItem(IDC_BTN_PLAY)->EnableWindow(m_wndList.GetSelectedCount() > 0);
}

void CPlayRecordDlg::OnNMDblclkList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;

	LPNMLISTVIEW lpNMList = (LPNMLISTVIEW)pNMHDR;
	__Ensure(lpNMList->iItem >= 0);

	m_wndList.DeselectAll();
	m_wndList.SelectItem(lpNMList->iItem);

	OnBnClickedPlay();
}

void CPlayRecordDlg::OnBnClickedPlay()
{
	TD_ListObjectList lstObjects;
	m_wndList.GetSelObjects(lstObjects);
	__Ensure(lstObjects);

	m_view.m_PlayCtrl.addPlayingItem(TD_IMediaList(lstObjects));
}

void CPlayRecordDlg::OnBnClickedClear()
{
	__Ensure(CMainApp::confirmBox(L"确认清空所有播放记录?", L"播放记录", this));

	__Ensure(m_view.getDataMgr().clearPlayRecord());

	m_vctPlayRecord.clear();
	(void)m_wndList.DeleteAllItems();
	GetDlgItem(IDC_BTN_PLAY)->EnableWindow(FALSE);
}
