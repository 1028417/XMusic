
#include "stdafx.h"

#include "ExportMediaSetDlg.h"

BEGIN_MESSAGE_MAP(CExportMediaSetDlg, CMediaSetDlg)
	ON_WM_DESTROY()
END_MESSAGE_MAP()

BOOL CExportMediaSetDlg::OnInitDialog()
{
	__super::OnInitDialog();

	TD_MediaSetList arrMediaSets;
	m_MediaSet.GetAllMediaSets(arrMediaSets);
	arrMediaSets([&](CMediaSet& MediaSet) {
		if (MediaSet.property().isDisableExport())
		{
			m_wndTree.SetCheckState(MediaSet, false);
		}
	});

	m_wndTree.setCheckChangedCB([&](E_CheckState eCheckState) {
		_sumCheckedSize();
	});

	m_thread = thread([&]() {
		_sumSize(m_MediaSet);
	});

	return TRUE;
}

void CExportMediaSetDlg::_sumCheckedSize()
{
	TD_TreeObjectList lstChekedObjects;
	m_wndTree.GetAllObjects(lstChekedObjects, CS_Checked);

	uint64_t ulCheckedSize = 0;
	UINT uCount = 0;

	PtrArray<CPlaylist> lstPlaylist(lstChekedObjects);
	for (CPlaylist *pPlaylist : lstPlaylist)
	{
		uCount += pPlaylist->playItems().size();
		ulCheckedSize += m_mapSumSize[pPlaylist];
	}

	PtrArray<CAlbum> lstAlbum(lstChekedObjects);
	for (CAlbum *pAlbum : lstAlbum)
	{
		uCount += pAlbum->albumItems().size();
		ulCheckedSize += m_mapSumSize[pAlbum];
	}

	WString strTitle(__Title);
	if (uCount > 0)
	{
		strTitle << L"   " << uCount << L"个文件";

		uint64_t uMB = ulCheckedSize / __1e6;
		if (0 != uMB)
		{
			strTitle << L'/' << uMB << L"MB";
		}
	}

	this->SetWindowText(strTitle);
}

uint64_t CExportMediaSetDlg::_sumSize(CMediaSet& MediaSet)
{
	uint64_t uSumSize = 0;
	
	TD_MediaList lstMedias;
	MediaSet.GetMedias(lstMedias);

	for (CMedia *pMedia : lstMedias)
	{
		long long nFileSize = fsutil::GetFileSize64(pMedia->GetAbsPath());
		if (nFileSize > 0)
		{
			uSumSize += (uint64_t)nFileSize;
		}
		
		if (m_bClosing)
		{
			return 0;
		}

		mtutil::usleep(1);
	}

	TD_MediaSetList lstSubSets;
	MediaSet.GetSubSets(lstSubSets);
	for (auto pSubSets : lstSubSets)
	{
		if (m_bClosing)
		{
			return 0;
		}

		bool bChecked = false;
		__appSync([&]() {
			bChecked = CS_Unchecked != m_wndTree.GetCheckState(*pSubSets);
		});
		if (bChecked)
		{
			uSumSize += _sumSize(*pSubSets);
		}
	}

	mtutil::usleep(1);

	if (0 != uSumSize)
	{
		__appSync([&]() {
			uint64_t uMB = uSumSize / __1e6;

			m_mapSumSize[&MediaSet] = uSumSize;

			auto hTreeItem = m_wndTree.getTreeItem(MediaSet);
			WString strText((LPCWSTR)m_wndTree.GetItemText(hTreeItem));
			strText << L"   " << uMB << L"MB";

			if (!lstMedias.empty())
			{
				strText << L'/' << lstMedias.size() << L"个文件";
			}

			m_wndTree.SetItemText(hTreeItem, strText);

			_sumCheckedSize();
		});
	}

	return uSumSize;
}

void CExportMediaSetDlg::OnDestroy()
{
	m_bClosing = true;
	if (m_thread.joinable())
	{
		CMainApp::GetMainApp()->thread([&]() {
			m_thread.join();
		});
	}

	__super::OnDestroy();
}
