
#include "stdafx.h"

#include "SimilarFileDlg.h"

void CSimilarFileDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_LIST1, m_wndList);

	DDX_Control(pDX, IDC_SLIDER1, m_wndSlider);
}

BEGIN_MESSAGE_MAP(CSimilarFileDlg, CDialog)
	ON_WM_HSCROLL()

	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST1, &CSimilarFileDlg::OnLvnItemchangedList1)
	ON_NOTIFY(NM_CLICK, IDC_LIST1, &CSimilarFileDlg::OnNMClickList1)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST1, &CSimilarFileDlg::OnNMDblclkList1)

	ON_BN_CLICKED(IDC_BTN_REMOVE, &CSimilarFileDlg::OnBnClickedRemove)

	ON_BN_CLICKED(IDC_BTN_PLAY, &CSimilarFileDlg::OnBnClickedPlay)

	ON_BN_CLICKED(IDC_BTN_EXPLORE, &CSimilarFileDlg::OnBnClickedExplore)	
END_MESSAGE_MAP()

#define __crFlag RGB(245, 252, 255)

BOOL CSimilarFileDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	CRect rcClient;
	m_wndList.GetClientRect(rcClient);
	CListColumnGuard ColumnGuard(rcClient.Width() - m_view.m_globalSize.m_uScrollbarWidth);

	auto& globalSize = m_view.m_globalSize;
	ColumnGuard.addDynamic(L"目录", 0.48)
		.addDynamic(L"文件", 0.52)
		.addFix(L"大小", 120)
		.addFix(L"时长", 80, true)
		.addFix(L"日期", 100, true);

	CObjectList::tagListPara ListPara(ColumnGuard);
	ListPara.uItemHeight = 50;

	__AssertReturn(m_wndList.InitCtrl(ListPara), FALSE);

	m_wndList.SetCustomDraw([&](tagLVDrawSubItem& lvcd) {
		if (m_vecRowFlag[lvcd.uItem])
		{
			lvcd.crBkg = __crFlag;
		}

		if (0 == lvcd.nSubItem)
		{
			lvcd.bSetUnderline = true;
		}
	});

	_genPercent();

	Refresh(100);

	return true;
}

void CSimilarFileDlg::_genPercent()
{
	SSet<UINT> setPercent;
	m_arrSimilarFile([&](auto& arrSimilarFile) {
		arrSimilarFile(1, [&](auto& pr) {
			setPercent.add(pr.second);
		});
	});
	m_arrPercent.assign(setPercent);

	m_wndSlider.SetPageSize(1);

	auto uMax = m_arrPercent.size() - 1;
	m_wndSlider.SetRange(0, uMax);
	m_wndSlider.SetPos(uMax);
}

void CSimilarFileDlg::Refresh(UINT uPos)
{
	CRedrawLock RedrawLock(m_wndList);

	GetDlgItem(IDC_BTN_REMOVE)->EnableWindow(FALSE);
	GetDlgItem(IDC_BTN_PLAY)->EnableWindow(FALSE);
	GetDlgItem(IDC_BTN_EXPLORE)->EnableWindow(FALSE);

	m_wndList.DeleteAllItems();

	m_arrSimilarFileInfo.clear();

	UINT uItem = 0;
	UINT uGroupCount = 0;
	m_vecRowFlag.clear();

	auto fnInsertItem = [&](CMediaRes& mediaRes, auto group, auto idx) {
		wstring strDir;
		auto pParent = mediaRes.parent();
		if (pParent)
		{
			strDir = pParent->GetPath();
		}

		m_wndList.InsertItemEx(uItem++, { strDir, mediaRes.GetName(), mediaRes.fileSizeString()
			, L" ", mediaRes.fileTimeString(true) });
		m_arrSimilarFileInfo.add({ group, idx });

		m_vecRowFlag.push_back(uGroupCount % 2 != 0);
	};

	m_uPos = MIN(uPos, m_arrPercent.size()-1);

	m_wndSlider.SetPos(m_uPos);

	m_arrPercent.get(m_uPos, [&](UINT uPercent) {
		m_arrSimilarFile([&](auto& arrSimilarFile, size_t group) {
			SMap<size_t, CMediaRes*> mapMediaRes;
			arrSimilarFile(1, [&](auto& pr, size_t pos) {
				if (pr.second == uPercent)
				{
					mapMediaRes.set(pos, pr.first);
				}
			});

			if (!mapMediaRes)
			{
				return;
			}

			arrSimilarFile.front([&](auto& pr) {
				fnInsertItem(*pr.first, group, 0);
			});
				
			for (auto& pr : mapMediaRes)
			{
				fnInsertItem(*pr.second, group, pr.first);
			}

			uGroupCount++;
		});

		m_wndList.AsyncTask(__AsyncTaskElapse + m_wndList.GetItemCount()/10, [&](UINT uItem) {
			m_arrSimilarFileInfo.get(uItem, [&](auto& pr) {
				auto group = pr.first;
				auto index = pr.second;

				m_arrSimilarFile.get(group, [&](auto& arrSimilarFile) {
					arrSimilarFile.get(index, [&](auto& pr) {
						auto& strDuration = CMedia::genDurationString(CMediaOpaque::checkDuration(*pr.first));
						m_wndList.SetItemText(uItem, 3, (L" " + strDuration).c_str());
					});
				});
			});
		});

		return true;
	});

	wstring strTitle(L"相似文件");
	strTitle.append(L"(" + to_wstring(uItem) + L"项/" + to_wstring(uGroupCount) + L"组)");
	this->SetWindowText(strTitle.c_str());
}

void CSimilarFileDlg::OnLvnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;

	(void)this->GetDlgItem(IDC_BTN_REMOVE)->EnableWindow(m_wndList.GetSelectedCount() > 0);
	(void)this->GetDlgItem(IDC_BTN_PLAY)->EnableWindow(m_wndList.GetSelectedCount() > 0);
	(void)this->GetDlgItem(IDC_BTN_EXPLORE)->EnableWindow(m_wndList.GetSelectedCount() == 1);
}

void CSimilarFileDlg::OnNMClickList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;

	__Ensure(m_wndList.GetSelectedCount()<=1 && !CMainApp::getKeyState(VK_SHIFT) && !CMainApp::getKeyState(VK_CONTROL));

	LPNMLISTVIEW lpNMList = (LPNMLISTVIEW)pNMHDR;
	if (0 == lpNMList->iSubItem)
	{
		m_wndList.AsyncLButtondown([=]{
			OnBnClickedExplore();
		});
	}
}

void CSimilarFileDlg::OnNMDblclkList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;

	LPNMLISTVIEW lpNMList = (LPNMLISTVIEW)pNMHDR;
	__Ensure(lpNMList->iItem >= 0);
	
	m_wndList.DeselectAll();
	m_wndList.SelectItem(lpNMList->iItem);

	OnBnClickedPlay();
}

void CSimilarFileDlg::OnBnClickedRemove()
{
	set<CMediaRes*> setDelFile;

	list<UINT> lstItems;
	m_wndList.GetSelItems(lstItems);
	for (auto uItem : lstItems)
	{
		m_arrSimilarFileInfo.get(uItem, [&](auto& pr) {
			auto group = pr.first;
			auto index = pr.second;

			m_arrSimilarFile.get(group, [&](auto& arrSimilarFile){
				arrSimilarFile.get(index, [&](auto& pr) {
					setDelFile.insert(pr.first);
				});
			});
		});
	}

	__Ensure(!setDelFile.empty());
	
	__Ensure(confirmBox(L"确认删除所选曲目?"));

	__waitCursor;

	TD_MediaResList arrDelFile(setDelFile);
	__Ensure(m_view.getController().removeMediaRes(arrDelFile));
	
	for (CMediaRes *pMediaRes : setDelFile)
	{
		m_arrSimilarFile.del([&](auto& arrSimilarFile) {
			arrSimilarFile.del([&](auto& pr) {
				return pr.first == pMediaRes;
			});

			return arrSimilarFile.size() <= 1;
		});
	}
	
	auto prevPos = m_wndSlider.GetPos();
	_genPercent();

	int iTopItem = m_wndList.GetTopIndex();

	Refresh(prevPos);

	m_wndList.EnsureVisible(iTopItem, TRUE);
	m_wndList.EnsureVisible(iTopItem + m_wndList.GetCountPerPage() - 1, TRUE);
}

void CSimilarFileDlg::OnBnClickedPlay()
{
	list<UINT> lstItems;
	m_wndList.GetSelItems(lstItems);

	TD_IMediaList lstMedias;
	for (auto uItem : lstItems)
	{
		m_arrSimilarFileInfo.get(uItem, [&](auto& pr) {
			m_arrSimilarFile.get(pr.first, [&](auto& arrSimilarFile) {
				arrSimilarFile.get(pr.second, [&](auto& pr) {
					lstMedias.add(pr.first);
				});
			});
		});
	}

	m_view.m_PlayCtrl.addPlayingItem(lstMedias);
}

void CSimilarFileDlg::OnBnClickedExplore()
{
	int nItem = m_wndList.GetSelItem();
	__Ensure(nItem >= 0);

	m_arrSimilarFileInfo.get((UINT)nItem, [&](auto& pr) {
		m_arrSimilarFile.get(pr.first, [&](auto& arrSimilarFile) {
			arrSimilarFile.get(pr.second, [&](auto& pr) {
				m_view.exploreMedia(*pr.first);
			});
		});
	});
}

void CSimilarFileDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if (SB_ENDSCROLL == nSBCode)
	{
		UINT uPos = m_wndSlider.GetPos();
		if (uPos != m_uPos)
		{
			Refresh(uPos);
		}
	}
}

void CSimilarFileDlg::OnClose(bool bCancel)
{
	__super::OnClose(bCancel);

	m_view.getModel().refreshMediaLib();
}
