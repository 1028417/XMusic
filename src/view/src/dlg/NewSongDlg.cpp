#include "StdAfx.h"

#include "NewSongDlg.h"

CNewSongDlg::CNewSongDlg(__view& view)
	: m_view(view)
{
}

BEGIN_MESSAGE_MAP(CNewSongDlg, CDialog)
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_DATETIMEPICKER, &CNewSongDlg::OnDtnDatetimechange)
	
	ON_BN_CLICKED(IDC_CHECK1, &CNewSongDlg::OnBnClickedCheck1)
	
	ON_CBN_SELCHANGE(IDC_COMBO1, &CNewSongDlg::OnSelChangeCombo1)

	ON_NOTIFY(NM_DBLCLK, IDC_LIST1, &CNewSongDlg::OnNMDblclkList1)
	
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST1, &CNewSongDlg::OnLvnItemchangedList1)

	ON_BN_CLICKED(IDC_BTN_DEMAND, &CNewSongDlg::OnBnClickedBtnDemand)
	ON_BN_CLICKED(IDC_BTN_PLAY, &CNewSongDlg::OnBnClickedBtnPlay)
	ON_BN_CLICKED(IDC_BTN_EXPORT, &CNewSongDlg::OnBnClickedBtnExport)
END_MESSAGE_MAP()

void CNewSongDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_DATETIMEPICKER, m_wndDateTimeCtrl);

	DDX_Control(pDX, IDC_COMBO1, m_wndCombo);
	
	DDX_Control(pDX, IDC_LIST1, m_wndList);
}

// CNewSongDlg 消息处理程序

BOOL CNewSongDlg::OnInitDialog()
{
	(void)__super::OnInitDialog();

	UINT uIndex = 0;
	m_wndCombo.InsertString(uIndex++, L"      按时间");
	m_wndCombo.InsertString(uIndex++, L"      按标题");
	m_wndCombo.InsertString(uIndex++, L"      按类型");
	m_wndCombo.InsertString(uIndex++, L"      按路径");
	m_wndCombo.InsertString(uIndex++, L"      按歌单");
	m_wndCombo.InsertString(uIndex++, L"      按歌手");
	m_wndCombo.InsertString(uIndex++, L"      不排序");
	m_wndCombo.SetCurSel(0);

	CRect rcClient;
	m_wndList.GetClientRect(rcClient);
	CListColumnGuard ColumnGuard(rcClient.Width() - m_view.m_globalSize.m_uScrollbarWidth);

	auto& globalSize = m_view.m_globalSize;
	ColumnGuard.addDynamic(_T("曲目"), 0.35)
		.addFix(_T("类型"), globalSize.m_ColWidth_Type*2, true)
		.addDynamic(_T("目录"), 0.3)
		.addDynamic(_T("所属歌单"), 0.15)
		.addDynamic(_T("所属专辑"), 0.2)
		.addFix(_T("加入时间"), UINT(globalSize.m_ColWidth_AddTime*1.2), true);

	CObjectList::tagListPara ListPara(ColumnGuard);
	__AssertReturn(m_wndList.InitCtrl(ListPara), FALSE);

	__xmedialib.GetAllMedias(m_lstSrcMedias);
	
	mediatime_t maxTime = 0;
	m_lstSrcMedias([&](CMedia& media) {
		maxTime = MAX(maxTime, media.GetAddTime().m_time);
	});
	if (0 == maxTime)
	{
		maxTime = (mediatime_t)time(0);
	}

	tagTM tm;
	tmutil::timeToTM(maxTime, tm);
	CTime m_timeMax(tm.tm_year, tm.tm_mon, tm.tm_mday, 0, 0, 0);

	CTime time0;
	m_wndDateTimeCtrl.SetRange(&time0, &m_timeMax);

	Refresh();

	return TRUE;
}

void CNewSongDlg::Refresh()
{
	int iSortType = m_wndCombo.GetCurSel();

	CTime time;
	(void)m_wndDateTimeCtrl.GetTime(time);	
	CTime fiterTime(time.GetYear(), time.GetMonth(), time.GetDay(), 0,0,0);
	time_t tFilterTime = fiterTime.GetTime();

	if (tFilterTime != m_tFilterTime || iSortType != m_nSortType)
	{
		m_tFilterTime = tFilterTime;
		m_nSortType = iSortType;

		__waitCursor;

		m_MediaMixer.clear();

		m_lstSrcMedias([&](CMedia& media) {
			__Ensure(media.GetAddTime().m_time >= tFilterTime);

			m_MediaMixer.add(media);
		});

		CRedrawLock RedrawGuard(m_wndList);
		(void)m_wndList.DeleteAllItems();

		UINT uIndex = 0;
		_GetMediaMixture()([&](auto& MediaMixture){
			m_wndList.InsertItemEx(uIndex, { MediaMixture.GetTitle()
				, MediaMixture.GetExtName()
				, MediaMixture.GetDir().c_str()
				, MediaMixture.GetPlaylistName()
				, MediaMixture.GetSingerAlbumString()
				, MediaMixture.GetAddTime().GetText(false)
			}, L" ");
			
			uIndex++;
		});

		this->GetDlgItem(IDC_BTN_DEMAND)->EnableWindow(uIndex);
		this->GetDlgItem(IDC_BTN_EXPORT)->EnableWindow(uIndex);

		wstring strTitle(L"新曲目");
		if (0 != uIndex)
		{
			strTitle.append(L"(" + to_wstring(uIndex) + L"项)");
		}
		this->SetWindowText(strTitle.c_str());
	}
}

const TD_MediaMixtureVector& CNewSongDlg::_GetMediaMixture()
{
	switch (m_nSortType)
	{
	case 0:
		return m_MediaMixer.get(E_SortMediaType::SMT_ByAddTime);
	case 1:
		return m_MediaMixer.get(E_SortMediaType::SMT_ByTitle);
	case 2:
		return m_MediaMixer.get(E_SortMediaType::SMT_ByFileType);
	case 3:
		return m_MediaMixer.get(E_SortMediaType::SMT_ByPath);
	case 4:
		return m_MediaMixer.get(E_SortMediaType::SMT_ByPlaylist);
	case 5:
		return m_MediaMixer.get(E_SortMediaType::SMT_BySinger);
	default:
		return m_MediaMixer.get();
	}
}

void CNewSongDlg::OnSelChangeCombo1()
{
	Refresh();
}

void CNewSongDlg::OnBnClickedCheck1()
{
	Refresh();
}

void CNewSongDlg::OnNMDblclkList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;

	LPNMLISTVIEW lpNMList = (LPNMLISTVIEW)pNMHDR;
	__Ensure(lpNMList->iItem >= 0);

	m_wndList.DeselectAll();
	m_wndList.SelectItem(lpNMList->iItem);

	_GetMediaMixture().get(lpNMList->iItem, [&](CMediaMixture& MediaMixture) {
		m_view.m_PlayCtrl.addPlayingItem(TD_IMediaList(MediaMixture.GetMedia()));
	});
}

void CNewSongDlg::OnLvnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;

	int nSelCount = m_wndList.GetSelectedCount();

	(void)this->GetDlgItem(IDC_BTN_PLAY)->EnableWindow(nSelCount > 0);
}

void CNewSongDlg::OnDtnDatetimechange(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;

	this->Refresh();
}

void CNewSongDlg::OnBnClickedBtnDemand()
{
	TD_MediaList lstMedias;
	_GetMediaMixture()([&](auto& NewSong){
		lstMedias.add(NewSong.GetMedia());
	});

	if (lstMedias)
	{
		(void)m_view.getPlayMgr().demand(lstMedias);
	}
}

void CNewSongDlg::OnBnClickedBtnPlay()
{
	list<UINT> lstItems;
	m_wndList.GetSelItems(lstItems);

	auto& vecMediaMixture = _GetMediaMixture();

	TD_IMediaList lstMedias;
	for (auto uItem : lstItems)
	{
		vecMediaMixture.get(uItem, [&](CMediaMixture& MediaMixture) {
			lstMedias.add(MediaMixture.GetMedia());
		});
	}

	m_view.m_PlayCtrl.addPlayingItem(lstMedias);
}

void CNewSongDlg::OnBnClickedBtnExport()
{
	m_view.exportMedia(m_MediaMixer.getMedias(), this);
}
