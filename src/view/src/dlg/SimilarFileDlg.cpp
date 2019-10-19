
#include "stdafx.h"

#include "SimilarFileDlg.h"

void CSimilarFileDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_LIST1, m_wndList);
}

BEGIN_MESSAGE_MAP(CSimilarFileDlg, CDialog)
	ON_WM_HSCROLL()

	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST1, &CSimilarFileDlg::OnLvnItemchangedList1)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST1, &CSimilarFileDlg::OnNMDblclkList1)

	ON_BN_CLICKED(IDC_BTN_REMOVE, &CSimilarFileDlg::OnBnClickedRemove)

	ON_BN_CLICKED(IDC_BTN_PLAY, &CSimilarFileDlg::OnBnClickedPlay)

	ON_BN_CLICKED(IDC_BTN_EXPLORE, &CSimilarFileDlg::OnBnClickedExplore)	
END_MESSAGE_MAP()

#define __Color_Flag RGB(240, 250, 255)

BOOL CSimilarFileDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	CRect rcClient;
	m_wndList.GetClientRect(rcClient);
	CListColumnGuard ColumnGuard(rcClient.Width() - m_view.m_globalSize.m_uScrollbarWidth * 2);

	auto& globalSize = m_view.m_globalSize;
	ColumnGuard.addDynamic(L"目录", 0.48)
		.addDynamic(L"文件", 0.52)
		.addFix(L"大小", 120)
		.addFix(L"时长", 70);

	CObjectList::tagListPara ListPara(ColumnGuard);

	__AssertReturn(m_wndList.InitCtrl(ListPara), FALSE);

	m_wndList.SetCustomDraw([&](tagLVCustomDraw& lvcd) {
		if (m_vecRowFlag[lvcd.uItem])
		{
			lvcd.crBkg = __Color_Flag;
		}
	});

	SSet<UINT> setPercent;
	m_arrSimilarFile([&](auto& arrSimilarFile) {
		arrSimilarFile(1, [&](auto& pr) {
			setPercent.add(pr.second);
		});
	});
	m_arrPercent = setPercent;

	CSliderCtrl* pSlider = (CSliderCtrl*)this->GetDlgItem(IDC_SLIDER1);
	pSlider->SetPageSize(1);

	auto uMax = m_arrPercent.size() - 1;
	pSlider->SetRange(0, uMax);
	pSlider->SetPos(uMax);

	Refresh(uMax);

	return true;
}

void CSimilarFileDlg::Refresh(UINT uPos)
{
	m_uPos = uPos;

	GetDlgItem(IDC_BTN_REMOVE)->EnableWindow(FALSE);
	GetDlgItem(IDC_BTN_PLAY)->EnableWindow(FALSE);
	GetDlgItem(IDC_BTN_EXPLORE)->EnableWindow(FALSE);

	CRedrawLockGuard RedrawLockGuard(m_wndList);

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

		m_wndList.InsertItemEx(uItem++, { strDir, mediaRes.GetName(), mediaRes.GetFileSizeString(false) }, L" ");
		m_arrSimilarFileInfo.add({ group, idx });

		m_vecRowFlag.push_back(uGroupCount % 2 != 0);
	};

	m_arrPercent.get(uPos, [&](UINT uPercent) {
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

		m_wndList.AsyncTask(50, [&](UINT uItem) {
			m_arrSimilarFileInfo.get(uItem, [&](auto& pr) {
				auto group = pr.first;
				auto index = pr.second;

				m_arrSimilarFile.get(group, [&](auto& arrSimilarFile) {
					arrSimilarFile.get(index, [&](auto& pr) {
						auto& strDuration = CMedia::GetDurationString(CMediaOpaque::checkDuration(*pr.first));
						m_wndList.SetItemText(uItem, 3, (L" " + strDuration).c_str());
					});
				});
			});
		});
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

void CSimilarFileDlg::OnNMDblclkList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;

	LPNMLISTVIEW lpNMList = (LPNMLISTVIEW)pNMHDR;
	__Ensure(0 <= lpNMList->iItem);
	
	m_wndList.DeselectAll();
	m_wndList.SelectItem(lpNMList->iItem);

	OnBnClickedPlay();
}

void CSimilarFileDlg::OnBnClickedRemove()
{
	set<CMediaRes*> setDelFile;
	SArray<pair<UINT, UINT>> arrDelFileInfo;

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

					arrDelFileInfo.add({group,index});
				});
			});
		});
	}

	__Ensure(!setDelFile.empty());
	
	__Ensure(CMainApp::showConfirmMsg(L"确认删除所选曲目?", this));

	CWaitCursor WaitCursor;

	TD_MediaResList arrDelFile(setDelFile);
	__Ensure(m_view.getController().removeMediaRes(arrDelFile));

	for (auto& pr : arrDelFileInfo)
	{
		m_arrSimilarFile.get(pr.first, [&](TD_SimilarFileGroup& arrSimilarFile) {
			(void)arrSimilarFile.del_pos(pr.second);
		});
	}

	int iTopItem = m_wndList.GetTopIndex();

	CSliderCtrl* pSlider = (CSliderCtrl*)this->GetDlgItem(IDC_SLIDER1);
	Refresh(pSlider->GetPos());

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
	int iItem = m_wndList.GetSelItem();
	__Ensure(iItem >= 0);

	m_arrSimilarFileInfo.get((UINT)iItem, [&](auto& pr) {
		m_arrSimilarFile.get(pr.first, [&](auto& arrSimilarFile) {
			arrSimilarFile.get(pr.second, [&](auto& pr) {
				pr.first->ShellExplore();
			});
		});
	});
}

void CSimilarFileDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if (SB_ENDSCROLL == nSBCode)
	{
		CSliderCtrl* pSlider = (CSliderCtrl*)this->GetDlgItem(IDC_SLIDER1);
		UINT uPos = pSlider->GetPos();
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
