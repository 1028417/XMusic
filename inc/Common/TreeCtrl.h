
#pragma once

#include "img.h"

#include "Guard.h"

class __CommonExt CBaseTree: public CTouchWnd<CTreeCtrl>
{
public:
	~CBaseTree()
	{
		(void)m_Imglst.DeleteImageList();
	}

	DECLARE_MESSAGE_MAP()

private:
	CCompatableFont m_font;

	CImglst m_Imglst;

public:
	BOOL InitImglst(const CSize& size, const TD_IconVec& vecIcons = {});

	BOOL InitImglst(CBitmap& Bitmap);

	void SetImageList(CImglst& imglst)
	{
		(void)__super::SetImageList(&imglst, TVSIL_NORMAL);
	}

	void SetFontSize(float fFontSizeOffset);
	
	void GetAllItems(list<HTREEITEM>& lstItems);
	
protected:
	virtual HTREEITEM InsertItem(HTREEITEM hParentItem, LPCTSTR lpszItem, DWORD_PTR dwData, int nImage=0);

	void GetChildItems(HTREEITEM hItem, list<HTREEITEM>& lstItems);

	virtual void PreSubclassWindow() override;
	
	virtual BOOL PreTranslateMessage(MSG* pMsg) override;

	virtual BOOL OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult) override;

	virtual BOOL handleNMNotify(NMHDR& NMHDR) { return FALSE; }
};


//CObjectTree

class __CommonExt CObjectTree : public CBaseTree
{
public:
	CObjectTree() {}

private:
	map<const CTreeObject*, HTREEITEM> m_mapTreeObject;
	
public:
	CTreeObject *GetItemObject(HTREEITEM hItem);
	
	inline HTREEITEM getTreeItem(const CTreeObject& Object)
	{
		auto itr = m_mapTreeObject.find(&Object);
		if (itr == m_mapTreeObject.end())
		{
			return NULL;
		}

		return itr->second;
	}

	inline HTREEITEM getTreeItem(const CTreeObject *pObject)
	{
		if (NULL == pObject)
		{
			return NULL;
		}
		return getTreeItem(*pObject);
	}

	void SetRootObject(CTreeObject& Object);

	virtual HTREEITEM InsertObject(CTreeObject& Object, CTreeObject *pParentObject=NULL);
	HTREEITEM InsertObjectEx(CTreeObject& Object, CTreeObject *pParentObject=NULL);

	void UpdateImage(CTreeObject& Object);

	CTreeObject *GetSelectedObject();
	
	CTreeObject *GetParentObject(CTreeObject& Object);
	
	void GetAllObjects(TD_TreeObjectList& lstObjects);

	BOOL EnsureVisible(const CTreeObject& Object)
	{
		return __super::EnsureVisible(getTreeItem(Object));
	}

	BOOL SelectObject(const CTreeObject& Object)
	{
		return __super::SelectItem(getTreeItem(Object));
	}

	BOOL ExpandObject(const CTreeObject& Object, bool bExpand = true)
	{
		return __super::Expand(getTreeItem(Object), bExpand ? TVE_EXPAND : TVE_COLLAPSE);
	}

	CEdit* EditObject(const CTreeObject& Object)
	{
		return __super::EditLabel(getTreeItem(Object));
	}
	
private:
	virtual BOOL handleNMNotify(NMHDR& NMHDR) override;
};


enum E_CheckState
{
	CS_Nocheck = 0
	, CS_Unchecked
	, CS_Checked
	, CS_Grayed
};

// CObjectCheckTree
class __CommonExt CObjectCheckTree : public CObjectTree
{
	DECLARE_MESSAGE_MAP()

public:
	~CObjectCheckTree()
	{
		(void)m_StateImageList.DeleteImageList();
	}

private:
	bool m_bShowNocheck = true;

	function<void(E_CheckState)> m_cbCheckChanged;

//protected:
	CImageList m_StateImageList;

public:
	BOOL InitCtrl();

	void setCheckChangedCB(const function<void(E_CheckState)>& cbCheckChanged)
	{
		m_cbCheckChanged = cbCheckChanged;
	}

public:
	void SetRootObject(CTreeObject& Object, bool bShowNocheck=false);

	HTREEITEM InsertObject(CTreeObject& Object, CTreeObject *pParentObject=NULL);

	void SetCheckState(CTreeObject& Object, bool bCheck);
	E_CheckState GetCheckState(CTreeObject& Object);
	
	void GetAllObjects(TD_TreeObjectList& lstObjects);
	void GetAllObjects(TD_TreeObjectList& lstObjects, E_CheckState eCheckState);
	void GetCheckedObjects(TD_TreeObjectList& lstObjects);

private:
	inline void _setImgMask(HTREEITEM hItem, E_CheckState eCheckState);

	void _setCheckState(HTREEITEM hItem, E_CheckState eCheckState);
	E_CheckState _getCheckState(HTREEITEM hItem);

	void _setChildsState(HTREEITEM hItem, E_CheckState eCheckState);
	void _updateParentState(HTREEITEM hItem);

	void _onItemClick(HTREEITEM hItem);

protected:
	virtual BOOL PreTranslateMessage(MSG* pMsg) override;
};
