
#pragma once

#include "TreeCtrl.h"
#include "ListCtrl.h"

template <typename T>
class CDirTreeT : public T
{
	struct tagDirSortor
	{
		bool operator () (CPath *pPath1, CPath *pPath2)
		{
			return 0 >= StrCmpA(pPath1->m_strName.c_str(), pPath2->m_strName.c_str());
		}
	};
	
public:
	CDirTreeT()
		: m_pRootDir(NULL)
	{
	}

	virtual	~CDirTreeT(void)
	{
	}

private:
	CDirObject *m_pRootDir;
	 
public:
	void SetRootDir(CDirObject *pRootDir, BOOL bShowRoot)
	{
		(void)DeleteAllItems();

		m_pRootDir = pRootDir;
	
		if (bShowRoot)
		{
			(void)InsertObject(*m_pRootDir);
			InsertChilds(m_pRootDir);

			(void)__super::SelectItem(getTreeItem(m_pRootDir));
			(void)ExpandObject(*m_pRootDir);
		}
		else
		{
			InsertChilds(m_pRootDir);
			InsertChildsEx(m_pRootDir);
		}
	}

	void InsertChilds(CDirObject *pDirObject)
	{
		pDirObject->GetSubPath()([&](CPath& SubDir){
			(void)InsertObject((CDirObject&)SubDir, pDirObject);
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

				CDirObject* pDirObject = (CDirObject*)__super::GetItemObject(pNMTreeView->itemNew.hItem);
				__AssertReturn(pDirObject, FALSE);
				InsertChildsEx(pDirObject);

				//return TRUE;
			}
		}

		return __super::handleNMNotify(NMHDR, pResult);
	}

private:
	void InsertChildsEx(CDirObject *pDirObject)
	{
		pDirObject->GetSubPath()([&](CPath& SubDir){
			InsertChilds((CDirObject*)&SubDir);
		});
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
