/***************************************************************************
*   Copyright (C) 2001-2008 by Józef Starosczyk                           *
*   ixen@copyhandler.com                                                  *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU Library General Public License          *
*   (version 2) as published by the Free Software Foundation;             *
*                                                                         *
*   This program is distributed in the hope that it will be useful,       *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
*   GNU General Public License for more details.                          *
*                                                                         *
*   You should have received a copy of the GNU Library General Public     *
*   License along with this program; if not, write to the                 *
*   Free Software Foundation, Inc.,                                       *
*   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
***************************************************************************/
/*************************************************************************
	CDirTreeCtrl tree control

	Files: DirTreeCtrl.h, DirTreeCtrl.cpp
	Author: Ixen Gerthannes
	Usage:
		Use it just as normal controls - place tree ctrl on your dialog and
		subclass it.
	Members:
		Functions:
			HTREEITEM InsertDesktopItem(); - initialisation - deletes
				all items in a list, and inserts desktop root icon
			bool ExpandItem(HTREEITEM hItem, UINT nCode); - replacement
				for function Expand - does more processing - better;
				UINT nCode just as in Expand(...)
			bool GetPath(HTREEITEM hItem, LPTSTR pszPath); - returns file
				system path (into pszPath) given HTREEITEM.
			bool SetPath(LPCTSTR lpszPath); - try to select given path in
				a directory tree. Returns true if whole path or a part of it
				was selected.
			bool GetItemInfoTip(HTREEITEM hItem, CString* pTip); - try to
				get an explorer's tool tip text for a given HTREEITEM.
				It uses IQueryInfo interface on IShellFolder. Cannot use it
				for top-level item (root). Note: function may return true
				with empty *pTip.
			bool GetItemShellData(HTREEITEM hItem, int nFormat, PVOID pBuffer, int iSize);
				functions gets data as in SHGetDataFromIDList
			bool GetItemStruct(HTREEITEM hItem, PSHELLITEMDATA *ppsid);
				function returns address of struct associated whith a given
				item (just as GetItemData, but type-casted to SHELLITEMDATA)
		Structs:
			SHELLITEMDATA - struct that is associated as lParam with every
				item inserted into this tree control (with new operator).
				Members:
					LPSHELLFOLDER lpsf;	- address of IShellFolder interface
						associated with this item
					LPITEMIDLIST lpiidl; - this's item id list relative to
						desktop folder
					LPSHELLFOLDER lpsfParent; - address of IShellFolder
						associated with a parent's item of this item
					LPITEMIDLIST lpiidlRelative; - this's item id list
						relative to lpsfParent

*************************************************************************/

#ifndef __DIRTREECTRL_H__
#define __DIRTREECTRL_H__

#include "shlobj.h"

// Functions that deal with PIDLs
LPITEMIDLIST	ConcatPidls(LPCITEMIDLIST pidl1, LPCITEMIDLIST pidl2);
LPITEMIDLIST	GetFullyQualPidl(LPSHELLFOLDER lpsf, LPITEMIDLIST lpi);
LPITEMIDLIST	CopyITEMID(LPMALLOC lpMalloc, LPITEMIDLIST lpi);
BOOL			GetName(LPSHELLFOLDER lpsf, LPITEMIDLIST  lpi, DWORD dwFlags, LPTSTR lpFriendlyName);
LPITEMIDLIST	CreatePidl(UINT cbSize);
void			FreePidl(LPITEMIDLIST lpiidl);
UINT			GetSize(LPCITEMIDLIST pidl);
LPITEMIDLIST	Next(LPCITEMIDLIST pidl);

/////////////////////////////////////////////////////////////////////////////
// CDirTreeCtrl window
#define WM_SETPATH				WM_USER+8
#define WM_CREATEFOLDERRESULT	WM_USER+9

#define SHELLITEMDATA		_SHELLITEMDATA
#define PSHELLITEMDATA		_SHELLITEMDATA*

struct _SHELLITEMDATA
{
	LPSHELLFOLDER lpsf;				// this shell folder (always exists)
	LPITEMIDLIST lpiidl;			// this item id list relative to desktop (always exists)
			
	LPSHELLFOLDER lpsfParent;		// parent shell folder (may be NULL)
	LPITEMIDLIST lpiidlRelative;	// this item id list relative to the parent's lpsf (may be NULL)
};

class CDirTreeCtrl : public CTreeCtrl
{
// Construction
public:
	CDirTreeCtrl();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDirTreeCtrl)
	protected:
	virtual void PreSubclassWindow();
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

	// Implementation
public:
	void SetIgnoreShellDialogs(bool bFlag);
	bool GetIgnoreShellDialogs();
	HTREEITEM InsertDesktopItem();
	HTREEITEM InsertNewFolder(HTREEITEM hParent, LPCTSTR lpszNewFolder);
	bool ExpandItem(HTREEITEM hItem, UINT nCode);
	bool GetPath(HTREEITEM hItem, LPTSTR pszPath);
	bool SetPath(LPCTSTR lpszPath);
	bool GetItemInfoTip(HTREEITEM hItem, CString* pTip);
	bool GetItemShellData(HTREEITEM hItem, int nFormat, PVOID pBuffer, int iSize);	
	bool GetItemStruct(HTREEITEM hItem, PSHELLITEMDATA *ppsid);
	bool IsEditing() const { return m_iEditType != 0; };
	virtual ~CDirTreeCtrl();

	// Generated message map functions
protected:
	bool RegisterWindowClass();
	friend int CALLBACK CompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM/* lParamSort*/);

	//{{AFX_MSG(CDirTreeCtrl)
	afx_msg void OnDestroy();
	afx_msg void OnItemexpanding(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeleteitem(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemexpanded(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBeginLabelEdit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

// members
protected:
	void InitControl();
	HTREEITEM RegularSelect(HTREEITEM hStart, LPCTSTR lpszPath);
//	HTREEITEM TraverseNetNode(HTREEITEM hItem, LPCTSTR lpszPath, LPTSTR lpszBuffer);
	bool ComparePaths(LPCTSTR lpszFull, LPCTSTR lpszPartial);
//	bool SetRemotePath(LPCTSTR lpszPath);
	bool SetLocalPath(LPCTSTR lpszPath);
	HRESULT FillNode(HTREEITEM hParent, LPSHELLFOLDER lpsf, LPITEMIDLIST lpidl, bool bSilent=false);

	bool m_bIgnoreShellDialogs;		// ignore dialogs of type 'insert floppy disk'
	HIMAGELIST m_hImageList;		// system img list
	HTREEITEM m_hDrives, m_hNetwork;	// my computer's and net neighbourhood's handles
	bool m_bIgnore;						// ignore the nearest adding of items in OnItemexpanding
	int m_iEditType;					// type of item editing (0-doesn't exist (nothing for edit), 1-new folder)
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif
