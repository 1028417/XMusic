
#pragma once

#include "TreeCtrl.h"
#include "ListCtrl.h"

template <typename T>
class CDirTreeT : public T
{
	/*struct tagDirSortor
	{
		bool operator () (CPath *pPath1, CPath *pPath2)
		{
			return 0 >= StrCmpA(pPath1->m_strName.c_str(), pPath2->m_strName.c_str());
		}
	};*/
	
public:
	CDirTreeT() {}

	virtual	~CDirTreeT() {}

private:
	void InsertChildsEx(CDirObject& DirObject)
	{
		DirObject.dirs()([&](CPath& SubDir) {
			InsertChilds((CDirObject&)SubDir);
		});
	}

	void InsertChildsEx(CTreeObject& DirObject)
	{
		TD_TreeObjectList paChildObject;
		DirObject.GetTreeChilds(paChildObject);
		paChildObject([&](CTreeObject& ChildObject) {
			InsertChilds(ChildObject);
		});
	}

public:
	void SetRootDir(CDirObject& RootDir, BOOL bShowRoot)
	{
		(void)DeleteAllItems();

		if (bShowRoot)
		{
			(void)InsertObject(RootDir);
			InsertChilds(RootDir);

			(void)__super::SelectItem(getTreeItem(&RootDir));
			(void)ExpandObject(RootDir);
		}
		else
		{
			InsertChilds(RootDir);
			InsertChildsEx(RootDir);
		}
	}

	void SetRootDir(CTreeObject& RootDir, BOOL bShowRoot)
	{
		(void)DeleteAllItems();

		if (bShowRoot)
		{
			(void)InsertObject(RootDir);
			InsertChilds(RootDir);

			(void)__super::SelectItem(getTreeItem(&RootDir));
			(void)ExpandObject(RootDir);
		}
		else
		{
			InsertChilds(RootDir);
			InsertChildsEx(RootDir);
		}
	}

	void InsertChilds(CTreeObject& DirObject)
	{
		TD_TreeObjectList paChildObject;
		DirObject.GetTreeChilds(paChildObject);
		paChildObject([&](CTreeObject& ChildObject) {
			(void)InsertObject(ChildObject, &DirObject);
		});
	}

	void InsertChilds(CDirObject& DirObject)
	{
		DirObject.dirs()([&](CPath& SubDir){
			(void)InsertObject((CDirObject&)SubDir, &DirObject);
		});
	}
	
	virtual BOOL handleNMNotify(NMHDR& NMHDR, LRESULT* pResult) override
	{
		if (TVN_ITEMEXPANDING == NMHDR.code)
		{
			LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(&NMHDR);
	
			if (TVE_EXPAND == pNMTreeView->action)
			{
				CWaitCursor WaitCursor;

				CTreeObject* pTreeObject = __super::GetItemObject(pNMTreeView->itemNew.hItem);
				__AssertReturn(pTreeObject, FALSE);
				CDirObject *pDirObject = dynamic_cast<CDirObject*>(pTreeObject);
				if (pDirObject)
				{
					InsertChildsEx(*pDirObject);
				}
				else
				{
					InsertChildsEx(*pTreeObject);
				}

				//return TRUE;
			}
		}

		return __super::handleNMNotify(NMHDR, pResult);
	}
};

using CDirTree = CDirTreeT<CObjectTree>;
using CDirCheckTree = CDirTreeT<CObjectCheckTree>;


class __CommonExt CPathList: public CObjectList
{
public:
	CPathList(){}
		
private:
	virtual void PreSubclassWindow() override;

public:
	BOOL InitCtrl(const tagListPara& para)
	{
		return __super::InitCtrl(para);
	}

	BOOL InitCtrl(COLORREF crText, float fFontSize, const TD_ListColumn& lstColumns=TD_ListColumn());

	BOOL InitCtrl(COLORREF crText, float fFontSize, const CSize& szImglst, const CSize *pszSmallImglst = NULL, const TD_IconVec& vecIcons = {});

	void SetPath(CPathObject& dir);

	BOOL IsFileItem(int nItem);
	
	void GetAllPathObjects(TD_PathObjectList& lstPathObjects);
	void GetAllPathObjects(TD_PathObjectList& lstPathObjects, bool bDir);
};
