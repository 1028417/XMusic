
#pragma once

#include "img.h"

#include "Guard.h"

class CListHeader : public CHeaderCtrl
{
	DECLARE_MESSAGE_MAP()

public:
	CListHeader(){}
	
private:
	UINT m_uHeight = 0;

	CCompatableFont m_font;

private:
	LRESULT OnLayout(WPARAM wParam, LPARAM lParam);

public:
	BOOL Init(UINT uHeight, float fFontSizeOffset = 0);
};

enum class E_ListViewType
{
	LVT_Icon = LV_VIEW_ICON,
	LVT_Report = LV_VIEW_DETAILS,
	LVT_SmallIcon = LV_VIEW_SMALLICON,
	LVT_List = LV_VIEW_LIST,
	LVT_Tile = LV_VIEW_TILE,
};

// CObjectList

struct __CommonExt tagListColumn
{
	CString cstrText;
	UINT uWidth = 0;
	bool bCenter = false;
};
typedef list<tagListColumn> TD_ListColumn;

class __CommonExt CListColumnGuard
{
public:
	CListColumnGuard()
	{
	}

	CListColumnGuard(const TD_ListColumn& lstColumn, UINT uTotalWidth=0)
		: m_lstColumn(lstColumn)
	{
		m_uTotalWidth = uTotalWidth;

		for (auto& column : m_lstColumn)
		{
			m_uFixWidth += column.uWidth;
		}
	}

	CListColumnGuard(UINT uTotalWidth)
	{
		m_uTotalWidth = uTotalWidth;
	}

private:
	UINT m_uTotalWidth = 0;

	TD_ListColumn m_lstColumn;

	list<pair<tagListColumn*, double>> m_lstDblWidth;

	UINT m_uFixWidth = 0;
	
public:
	CListColumnGuard& addFix(const CString& cstrText, UINT uWidth, bool bCenter=false)
	{
		m_lstColumn.push_back({ cstrText, uWidth, bCenter });

		m_uFixWidth += uWidth;

		return *this;
	}

	CListColumnGuard& addDynamic(const CString& cstrText, double dbWidth, bool bCenter = false)
	{
		addFix(cstrText, 0, bCenter);

		m_lstDblWidth.push_back({ &m_lstColumn.back(), dbWidth });

		return *this;
	}

	const TD_ListColumn& getListColumn()
	{
		return getListColumn(m_uTotalWidth);
	}

	const TD_ListColumn& getListColumn(UINT uTotalWidth)
	{
		if (!m_lstDblWidth.empty())
		{
			if (m_uFixWidth < uTotalWidth)
			{
				UINT uRamainWidth = uTotalWidth - m_uFixWidth;
				for (auto& pr : m_lstDblWidth)
				{
					pr.first->uWidth = UINT(uRamainWidth*pr.second);
				}
			}
		}
		else
		{
			if (0 != uTotalWidth)
			{
				if (m_uFixWidth != uTotalWidth)
				{
					for (auto& column : m_lstColumn)
					{
						column.uWidth = uTotalWidth * column.uWidth / m_uFixWidth;
					}

					m_uFixWidth = uTotalWidth;
				}
			}
		}

		return m_lstColumn;
	}

	operator const TD_ListColumn&()
	{
		return getListColumn(m_uTotalWidth);
	}
};

struct tagLvCustomDraw
{
	tagLvCustomDraw(NMLVCUSTOMDRAW& lvcd, COLORREF t_crText, COLORREF t_crBkg=RGB(255, 255, 255))
		: rcPos(lvcd.nmcd.rc)
		, hDC(lvcd.nmcd.hdc)
		, pObject((CListObject*)lvcd.nmcd.lItemlParam)
		, uItem(lvcd.nmcd.dwItemSpec)
		, nSubItem(lvcd.iSubItem)
		, crBkg(lvcd.clrTextBk)
		, crText(lvcd.clrText)
	{
		crBkg = t_crBkg;
		crText = t_crText;
	}

	const RECT& rcPos;

	const HDC hDC;
	
	CListObject *pObject = NULL;

	const UINT uItem;
	const int nSubItem;
	
	COLORREF& crBkg;
	COLORREF& crText;
	BYTE uTextAlpha = 0;

	float fFontSizeOffset = 0;

	bool bSetUnderline = false;

	bool bSkipDefault = false;
};

class __CommonExt CObjectList : public CTouchWnd<CListCtrl>
{
public:
	using CB_LVCustomDraw = function<void(tagLvCustomDraw& lvcd)>;
	using CB_ListViewChanged = function<void(E_ListViewType)>;

	using CB_LButtonHover = function<void(const CPoint&)>;

	struct tagListPara
	{
		tagListPara()
		{
		}

		tagListPara(const TD_ListColumn& t_lstColumns)
			: eViewType(E_ListViewType::LVT_Report)
			, lstColumns(t_lstColumns)
		{
		}

		E_ListViewType eViewType = (E_ListViewType)-1;

		TD_ListColumn lstColumns;

		COLORREF crText = 0;
		float fFontSize = 0;

		UINT uHeaderHeight = 0;
		float fHeaderFontSize = 0;

		UINT uItemHeight = 0;

		UINT uTileWidth = 0;
		UINT uTileHeight = 0;
	};

	CObjectList(){}

	virtual ~CObjectList()
	{
		(void)m_Imglst.DeleteImageList();
		(void)m_ImglstSmall.DeleteImageList();		
	}

private:
	tagListPara m_para;

	CB_LVCustomDraw m_cbCustomDraw;
	CB_ListViewChanged m_cbViewChanged;
	CB_TrackMouseEvent m_cbTrackMouseEvent;

	fn_voidvoid	m_cbLButtondown;

	CImglst m_Imglst;
	CImglst m_ImglstSmall;

	CCompatableFont m_font;
	CCompatableFont m_fontUnderline;
	CCompatableFont m_fontCustom;

	CListHeader m_wndHeader;

	UINT m_uColumnCount = 1;
	
	bool m_bTrackingMouse = false;
	
	CString m_cstrRenameText;

	vector<BOOL> m_vecAsyncTaskFlag;
	CWinTimer m_AsyncTaskTimer;

protected:
	void InitColumn(const TD_ListColumn& lstColumns);

	BOOL InitHeader(UINT uHeaderHeight, float fHeaderFontSize = 0);

	BOOL InitFont(COLORREF crText, float fFontSizeOffset = 0);

	BOOL SetItemHeight(UINT uItemHeight);

	virtual bool GetRenameText(UINT uItem, wstring& strRenameText);

public:
	BOOL InitCtrl(const tagListPara& para);

	BOOL InitImglst(const CSize& size, const CSize *pszSmall = NULL, const TD_IconVec& vecIcons = {});
	BOOL InitImglst(CBitmap& Bitmap, CBitmap *pBitmapSmall=NULL);
	void SetImageList(CImglst *pImglst, CImglst *pImglstSmall = NULL);

	void SetView(E_ListViewType eViewType, bool bArrange = false);
	E_ListViewType GetView();

	void SetTileSize(ULONG cx, ULONG cy);

	void SetCustomDraw(const CB_LVCustomDraw& cb)
	{
		m_cbCustomDraw = cb;
	}

	void SetViewAutoChange(const CB_ListViewChanged& cb)
	{
		m_cbViewChanged = cb;
	}

	void SetTrackMouse(const CB_TrackMouseEvent& cb)
	{
		m_cbTrackMouseEvent = cb;
	}

public:
	int InsertItemEx(UINT uItem, const vector<wstring>& vecText, const wstring& strPrefix = L"");
	int InsertItemEx(UINT uItem, const list<pair<UINT, wstring>>& lstText, const wstring& strPrefix = L"");

	void SetItemTexts(UINT uItem, const vector<wstring>& vecText, const wstring& strPrefix = L"");
	void SetItemTexts(UINT uItem, const list<pair<UINT, wstring>>& lstText, const wstring& strPrefix = L"");
	void SetItemTexts(UINT uItem, UINT uSubItem, const vector<wstring>& vecText, const wstring& strPrefix);

	void SetTexts(const vector<vector<wstring>>& vecTexts);// , int nPos = 0, const wstring& strPrefix = L"");

	void SetObjects(const TD_ListObjectList& lstObjects);// , int nPos = 0, const wstring& strPrefix = L"");

	int InsertObject(CListObject& Object, int nItem=-1, const wstring& strPrefix=L"");

	void UpdateItem(UINT uItem, CListObject *pObject=NULL);
	void UpdateItems();

	void UpdateColumns(const list<UINT>& lstColumn);
	void UpdateColumn(UINT uColumn)
	{
		UpdateColumns({ uColumn });
	}

	void SetColumnText(UINT uColumn, const wstring& strText);

	BOOL DeleteObject(const CListObject *pObject);
	void DeleteObjects(const set<CListObject*>& setDeleteObjects);
	void DeleteItems(const set<UINT>& setItems);

	void SetItemImage(UINT uItem, UINT uImage);

public:
	CListObject *GetItemObject(int nItem);
	int GetObjectItem(const CListObject *pObject);
	void GetAllObjects(TD_ListObjectList& lstListObjects);

	int GetSelItem();
	CListObject *GetSelObject();

	void GetSelItems(list<UINT>& lstItems);
	void GetSelObjects(map<UINT, CListObject*>& mapObjects);
	void GetSelObjects(TD_ListObjectList& lstObjects);

	BOOL SelectFirstItem();
	void SelectItem(UINT uItem, BOOL bSetFocus=TRUE);
	void SelectObject(const CListObject *pObject, BOOL bSetFocus=TRUE);
	void SelectItems(UINT uItem, UINT uSelectCount);
	
	void SelectAll();
	void DeselectAll();

	UINT GetHeaderHeight();

	const LVHITTESTINFO& hittest(const POINT& ptPos) const;

	using CB_AsyncTask = function<void(UINT uItem)>;
	void AsyncTask(UINT uElapse, const CB_AsyncTask& cb=NULL);

	void CObjectList::AsyncLButtondown(const fn_voidvoid& cb);

protected:
	virtual void GenListItem(CListObject& Object, bool bReportView, vector<wstring>& vecText, int& iImage);

	virtual void PreSubclassWindow() override;

	virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult) override;

	virtual BOOL OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult) override;

	virtual BOOL handleNMNotify(NMHDR& NMHDR, LRESULT* pResult);

	void ChangeListCtrlView(short zDelta);

private:
	void _SetItemObject(UINT uItem, CListObject& Object, const wstring& strPrefix=L"");

	template <bool _clear_other>
	void _SetItemTexts(UINT uItem, const vector<wstring>& vecText, const wstring& strPrefix = L"");

	virtual void OnListItemRename(UINT uItem, const CString& cstrNewText) {}

	virtual void onAsyncTask(UINT uItem) {}
};
