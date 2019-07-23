#pragma once

#include "MediaSetDlg.h"

#define __Title L"选择导出项"

class CExportMediaSetDlg : public CMediaSetDlg
{
	DECLARE_MESSAGE_MAP()

public:
	CExportMediaSetDlg(__view& view, CMediaSet& MediaSet, TD_MediaSetList& lstRetMediaSets)
		: CMediaSetDlg(view, MediaSet, lstRetMediaSets, __Title)
	{
	}
	
private:
	thread m_thread;
	bool m_bClosing = false;

	map<CMediaSet*, ULONGLONG> m_mapSumSize;

private:
	BOOL OnInitDialog();

	ULONGLONG _sumSize(CMediaSet& MediaSet);

	void _sumCheckedSize();

	void OnDestroy();
};
