
#include "stdafx.h"

#include "DuplicateMediaDlg.h"

void CDuplicateMediaDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_wndList);
}

BEGIN_MESSAGE_MAP(CDuplicateMediaDlg, CDialog)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST1, &CDuplicateMediaDlg::OnLvnItemchangedList1)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST1, &CDuplicateMediaDlg::OnNMDblclkList1)

	ON_BN_CLICKED(IDC_BTN_REMOVE, &CDuplicateMediaDlg::OnBnClickedRemove)

	ON_BN_CLICKED(IDC_BTN_PLAY, &CDuplicateMediaDlg::OnBnClickedPlay)
END_MESSAGE_MAP()

#define __Color_Flag RGB(240, 250, 255)

BOOL CDuplicateMediaDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CRect rcClient;
	m_wndList.GetClientRect(rcClient);
	CListColumnGuard ColumnGuard(rcClient.Width() - m_view.m_globalSize.m_uScrollbarWidth * 2);

	auto& globalSize = m_view.m_globalSize;
	ColumnGuard.addDynamic(L"曲目", 0.55)
		.addDynamic(L"目录", 0.45)
		.addFix(_T("所属列表"), globalSize.m_ColWidth_RelatedPlaylist)
		.addFix(_T("所属专辑"), globalSize.m_ColWidth_RelatedSingerAlbum);

	CObjectList::tagListPara ListPara(ColumnGuard);
	
	ListPara.cbCustomDraw = [&](tagLvCustomDraw& lvcd) {
		if (m_vecRowFlag[lvcd.uItem])
		{
			lvcd.clrTextBk = __Color_Flag;
		}
	};

	__AssertReturn(m_wndList.InitCtrl(ListPara), FALSE);

	UINT uItem = 0;
	bool bRowFlag = false;
	m_arrDuplicateMedia([&](TD_MediaList& arrDuplicateMedia, size_t group) {
		arrDuplicateMedia([&](CMedia& media, size_t pos) {
			SVector<wstring> vecText( media.GetName(), media.GetDir() );
			if (E_MediaSetType::MST_Playlist == media.GetMediaSetType())
			{
				vecText.add(((CPlayItem&)media).GetPlaylistName());
			}
			else
			{
				CAlbumItem& AlbumItem = (CAlbumItem&)media;
				vecText.add(L"", AlbumItem.GetSingerName() + __CNDot + AlbumItem.GetAlbumName());
			}

			m_wndList.InsertItemEx(uItem++, vecText, L" ");
			m_arrDuplicateMediaInfo.add({ group, pos });

			m_vecRowFlag.push_back(bRowFlag);
		});

		bRowFlag = !bRowFlag;
	});

	wstring strTitle(L"重复曲目");
	strTitle.append(L"(" + to_wstring(uItem) + L"项)");
	this->SetWindowText(strTitle.c_str());

	return true;
}

void CDuplicateMediaDlg::OnLvnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;

	(void)this->GetDlgItem(IDC_BTN_REMOVE)->EnableWindow(m_wndList.GetSelectedCount() > 0);
	(void)this->GetDlgItem(IDC_BTN_PLAY)->EnableWindow(m_wndList.GetSelectedCount() > 0);
}

void CDuplicateMediaDlg::OnNMDblclkList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;

	LPNMLISTVIEW lpNMList = (LPNMLISTVIEW)pNMHDR;
	__Ensure(0 <= lpNMList->iItem);

	m_wndList.DeselectAll();
	m_wndList.SelectItem(lpNMList->iItem);

	m_arrDuplicateMediaInfo.get(lpNMList->iItem, [&](auto& pr) {
		m_arrDuplicateMedia.get(pr.first, [&](TD_MediaList& arrDuplicateMedia) {
			arrDuplicateMedia.get(pr.second, [&](auto& media) {
				m_view.m_PlayCtrl.addPlayingItem(TD_IMediaList(media));
			});
		});
	});
}

void CDuplicateMediaDlg::OnBnClickedRemove()
{
	list<UINT> lstItems;
	m_wndList.GetSelItems(lstItems);
	
	TD_MediaList arrDelMedia;
	SArray<pair<UINT, UINT>> arrDelMediaInfo;
	for (auto uItem : lstItems)
	{
		m_arrDuplicateMediaInfo.get(uItem, [&](auto& pr) {
			m_arrDuplicateMedia.get(pr.first, [&](TD_MediaList& arrDuplicateMedia){
				arrDuplicateMedia.get(pr.second, [&](auto& media) {
					arrDelMedia.add(media);

					arrDelMediaInfo.add(pr);
				});
			});
		});
	}
	__Ensure(arrDelMedia);

	__Ensure(CMainApp::showConfirmMsg(L"确认删除所选曲目?", this));

	__Assert(m_view.getModel().removeMedia(arrDelMedia));

	set<UINT> setSingleGroup;
	arrDelMediaInfo([&](auto& pr) {
		m_arrDuplicateMedia.get(pr.first, [&](TD_MediaList& arrDuplicateMedia) {
			(void)arrDuplicateMedia.del_pos(pr.second);

			if (1 == arrDuplicateMedia.size())
			{
				setSingleGroup.insert(pr.first);
			}
		});
	});

	m_arrDuplicateMediaInfo.del_pos(lstItems);
	(void)m_wndList.DeleteItems(SSet<UINT>(lstItems));
	
	list<UINT> lstSingleItem;
	m_arrDuplicateMediaInfo([&](auto& pr, size_t pos) {
		if (setSingleGroup.find(pr.first) != setSingleGroup.end())
		{
			lstSingleItem.push_back(pos);
		}
	});
	if (!lstSingleItem.empty())
	{
		m_arrDuplicateMediaInfo.del_pos(lstSingleItem);
		(void)m_wndList.DeleteItems(SSet<UINT>(lstSingleItem));
	}

	(void)this->GetDlgItem(IDC_BTN_REMOVE)->EnableWindow(FALSE);
	(void)this->GetDlgItem(IDC_BTN_PLAY)->EnableWindow(FALSE);
}

void CDuplicateMediaDlg::OnBnClickedPlay()
{
	list<UINT> lstItems;
	m_wndList.GetSelItems(lstItems);

	TD_IMediaList lstMedias;
	for (auto uItem : lstItems)
	{
		m_arrDuplicateMediaInfo.get(uItem, [&](auto& pr) {
			m_arrDuplicateMedia.get(pr.first, [&](TD_MediaList& arrDuplicateMedia) {
				arrDuplicateMedia.get(pr.second, [&](auto& media) {
					lstMedias.add(media);
				});
			});
		});
	}

	m_view.m_PlayCtrl.addPlayingItem(lstMedias);
}
