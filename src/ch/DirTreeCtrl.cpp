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
#include "stdafx.h"
#include "DirTreeCtrl.h"
#include "afxtempl.h"
#include "memdc.h"
#include "shlobj.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define WM_INITCONTROL	WM_USER+7

LPITEMIDLIST Next(LPCITEMIDLIST pidl)
{
   LPSTR lpMem=(LPSTR)pidl;

   lpMem+=pidl->mkid.cb;

   return (LPITEMIDLIST)lpMem;
}

UINT GetSize(LPCITEMIDLIST pidl)
{
    UINT cbTotal = 0;
    if (pidl)
    {
        cbTotal += sizeof(pidl->mkid.cb);       // Null terminator
        while (pidl->mkid.cb)
        {
            cbTotal += pidl->mkid.cb;
            pidl = Next(pidl);
        }
    }

    return cbTotal;
}

LPITEMIDLIST CreatePidl(UINT cbSize)
{
    LPMALLOC lpMalloc;
    HRESULT  hr;
    LPITEMIDLIST pidl=NULL;

    hr=SHGetMalloc(&lpMalloc);

    if (FAILED(hr))
       return 0;

    pidl=(LPITEMIDLIST)lpMalloc->Alloc(cbSize);

    if (pidl)
        memset(pidl, 0, cbSize);      // zero-init for external task   alloc

    if (lpMalloc) lpMalloc->Release();

    return pidl;
}

void FreePidl(LPITEMIDLIST lpiidl)
{
    LPMALLOC lpMalloc;
    HRESULT  hr;

    hr=SHGetMalloc(&lpMalloc);

    if (FAILED(hr))
       return;

    lpMalloc->Free(lpiidl);

    if (lpMalloc) lpMalloc->Release();
}

LPITEMIDLIST ConcatPidls(LPCITEMIDLIST pidl1, LPCITEMIDLIST pidl2)
{
    LPITEMIDLIST pidlNew;
    UINT cb1;
    UINT cb2;

    if (pidl1)  //May be NULL
       cb1 = GetSize(pidl1) - sizeof(pidl1->mkid.cb);
    else
       cb1 = 0;

    cb2 = GetSize(pidl2);

    pidlNew = CreatePidl(cb1 + cb2);
    if (pidlNew)
    {
        if (pidl1)
           memcpy(pidlNew, pidl1, cb1);
        memcpy(((LPSTR)pidlNew) + cb1, pidl2, cb2);
    }
    return pidlNew;
}

LPITEMIDLIST CopyITEMID(LPMALLOC lpMalloc, LPITEMIDLIST lpi)
{
   LPITEMIDLIST lpiTemp;

   lpiTemp=(LPITEMIDLIST)lpMalloc->Alloc(lpi->mkid.cb+sizeof(lpi->mkid.cb));
   CopyMemory((PVOID)lpiTemp, (CONST VOID *)lpi, lpi->mkid.cb+sizeof(lpi->mkid.cb));

   return lpiTemp;
}

BOOL GetName(LPSHELLFOLDER lpsf,
             LPITEMIDLIST  lpi,
			 DWORD         dwFlags,
             LPTSTR         lpFriendlyName)
{
   BOOL   bSuccess=TRUE;
   STRRET str;

   if (NOERROR==lpsf->GetDisplayNameOf(lpi,dwFlags, &str))
   {
     switch (str.uType)
     {
        case STRRET_WSTR:
			{
				CW2T cw2t(str.pOleStr);
				lstrcpy(lpFriendlyName, cw2t);
				break;
			}
         case STRRET_OFFSET:
			 {
				 lstrcpy(lpFriendlyName, (LPTSTR)lpi+str.uOffset);
				 break;
			 }

         case STRRET_CSTR:
			 {
				 CA2T ca2t(str.cStr);
				 lstrcpy(lpFriendlyName, ca2t);
				 break;
			 }

         default:
             bSuccess = FALSE;
             break;
      }
   }
   else
      bSuccess = FALSE;

   return bSuccess;
}

LPITEMIDLIST GetFullyQualPidl(LPSHELLFOLDER lpsf, LPITEMIDLIST lpi)
{
   TCHAR szBuff[MAX_PATH];
   LPSHELLFOLDER lpsfDeskTop;
   LPITEMIDLIST  lpifq;
   ULONG ulEaten, ulAttribs;
   HRESULT hr;

   if (!GetName(lpsf, lpi, SHGDN_FORPARSING, szBuff))
      return NULL;

   hr=SHGetDesktopFolder(&lpsfDeskTop);

   if (FAILED(hr))
      return NULL;

   CT2W ct2w(szBuff);
   hr=lpsfDeskTop->ParseDisplayName(NULL,
									NULL,
									ct2w,
									&ulEaten,
									&lpifq,
									&ulAttribs);

   lpsfDeskTop->Release();

   if (FAILED(hr))
      return NULL;

   return lpifq;
}

/////////////////////////////////////////////////////////////////////////////
// CDirTreeCtrl

CDirTreeCtrl::CDirTreeCtrl()
{
	m_hDrives=NULL;
	m_hNetwork=NULL;
	m_hImageList=NULL;
	m_bIgnore=false;
	m_iEditType=0;
//	RegisterWindowClass();
}
/*
bool CDirTreeCtrl::RegisterWindowClass()
{
	WNDCLASS wndcls;
    HINSTANCE hInst = AfxGetInstanceHandle();

    if (!(::GetClassInfo(hInst, DIRTREECTRL_CLASSNAME, &wndcls)))
    {
        // otherwise we need to register a new class
        wndcls.style            = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW;
        wndcls.lpfnWndProc      = ::DefWindowProc;
        wndcls.cbClsExtra       = wndcls.cbWndExtra = 0;
        wndcls.hInstance        = hInst;
        wndcls.hIcon            = NULL;
        wndcls.hCursor          = AfxGetApp()->LoadStandardCursor(IDC_ARROW);
        wndcls.hbrBackground    = (HBRUSH) (COLOR_3DFACE + 1);
        wndcls.lpszMenuName     = NULL;
        wndcls.lpszClassName    = DIRTREECTRL_CLASSNAME;

        if (!AfxRegisterClass(&wndcls))
        {
            AfxThrowResourceException();
            return false;
        }
    }

    return true;
}
*/
CDirTreeCtrl::~CDirTreeCtrl()
{
}


BEGIN_MESSAGE_MAP(CDirTreeCtrl, CTreeCtrl)
	//{{AFX_MSG_MAP(CDirTreeCtrl)
	ON_WM_DESTROY()
	ON_NOTIFY_REFLECT(TVN_ITEMEXPANDING, OnItemexpanding)
	ON_NOTIFY_REFLECT(TVN_DELETEITEM, OnDeleteitem)
	ON_NOTIFY_REFLECT(TVN_ITEMEXPANDED, OnItemexpanded)
	ON_NOTIFY_REFLECT(TVN_ENDLABELEDIT, OnEndlabeledit)
	ON_NOTIFY_REFLECT(TVN_BEGINLABELEDIT, OnBeginLabelEdit)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDirTreeCtrl message handlers

/////////////////////////////////////////////////////////////////////////////
// inicjalizacja
void CDirTreeCtrl::PreSubclassWindow() 
{
//	InitControl();		// here's not needed (strange ??)
	CTreeCtrl::PreSubclassWindow();
}

int CDirTreeCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CTreeCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	InitControl();
	
	return 0;
}

void CDirTreeCtrl::InitControl()
{
	// prepare image list
    SHFILEINFO sfi;
    m_hImageList = (HIMAGELIST)SHGetFileInfo(_T("C:\\"), FILE_ATTRIBUTE_NORMAL, &sfi, sizeof(SHFILEINFO), 
		SHGFI_SYSICONINDEX | SHGFI_SMALLICON);

	TreeView_SetImageList(this->m_hWnd, m_hImageList, TVSIL_NORMAL);

	// insert desktop icon
	InsertDesktopItem();

	// expanding, ...
	PostMessage(WM_INITCONTROL);
}

/////////////////////////////////////////////////////////////////////////////
// wstawia ikonê pulpitu
HTREEITEM CDirTreeCtrl::InsertDesktopItem()
{
	// clear treectrl - it shouldn't be more than 1 desktop
	if (!DeleteAllItems())
		return NULL;

	// clear vars
	m_hDrives=NULL;
	m_hNetwork=NULL;

	// fill with items
	SHFILEINFO sfi;
	LPSHELLFOLDER lpsfDesktop;
	LPITEMIDLIST lpiidlDesktop;
	if (FAILED(SHGetDesktopFolder(&lpsfDesktop)))
		return NULL;
	if (SHGetSpecialFolderLocation(this->GetSafeHwnd(), CSIDL_DESKTOP, &lpiidlDesktop) != NOERROR)
		return NULL;

	// add desktop
	TVITEM tvi;
	TVINSERTSTRUCT tvis;
	TCHAR szText[_MAX_PATH];
	_SHELLITEMDATA *psid=new _SHELLITEMDATA;
	psid->lpiidl=lpiidlDesktop;
	psid->lpsf=lpsfDesktop;
	psid->lpiidlRelative=NULL;
	psid->lpsfParent=NULL;

	tvi.mask=TVIF_CHILDREN | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_TEXT | TVIF_PARAM;
	if (!GetName(lpsfDesktop, lpiidlDesktop, 0/*SHGDN_INCLUDE_NONFILESYS*/, szText))
		lstrcpy(szText, _T("???"));
	tvi.pszText=szText;
	sfi.iIcon=-1;
	SHGetFileInfo((LPCTSTR)lpiidlDesktop, 0, &sfi, sizeof(SHFILEINFO), SHGFI_PIDL | SHGFI_SYSICONINDEX | SHGFI_SMALLICON);
	tvi.iImage=sfi.iIcon;
	sfi.iIcon=-1;
	SHGetFileInfo((LPCTSTR)lpiidlDesktop, 0, &sfi, sizeof(SHFILEINFO), SHGFI_PIDL | SHGFI_SYSICONINDEX | SHGFI_SMALLICON | SHGFI_OPENICON);
	tvi.iSelectedImage=sfi.iIcon;
	tvi.cChildren=1;
	tvi.lParam=reinterpret_cast<LPARAM>(psid);

	tvis.hParent=TVI_ROOT;
	tvis.item=tvi;
	return InsertItem(&tvis);
}

//////////////////////////////////////////////////////////////////////////////
// processes WM_INITCONTROL
LRESULT CDirTreeCtrl::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	switch(message)
	{
	case WM_INITCONTROL:
		ExpandItem(GetRootItem(), TVE_EXPAND);
		break;
	case WM_SETPATH:
		SetPath((LPCTSTR)lParam);
		break;
	}

	return CTreeCtrl::WindowProc(message, wParam, lParam);
}

////////////////////////////////////////////////////////////////////////////
// enables image list, ...
void CDirTreeCtrl::OnDestroy() 
{
	SetImageList(NULL, LVSIL_SMALL);	
	CTreeCtrl::OnDestroy();
}

////////////////////////////////////////////////////////////////////////////
// compares two items
int CALLBACK CompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM/* lParamSort*/)
{
	// jeœli siê nie da
	if (lParam1 == NULL || lParam2 == NULL)
		return 0;

	// normalne przetwarzanie
	SHELLITEMDATA* psidl1=(SHELLITEMDATA*)lParam1, *psidl2=(SHELLITEMDATA*)lParam2;
	
	LPSHELLFOLDER lpsf;
	if (SHGetDesktopFolder(&lpsf) != NOERROR)
		return 0;

	HRESULT hRes=lpsf->CompareIDs(0, psidl1->lpiidl, psidl2->lpiidl);
	if (!SUCCEEDED(hRes))
		return 0;

	lpsf->Release();

    return (short)SCODE_CODE(GetScode(hRes));
}

/////////////////////////////////////////////////////////////////////////////
// fills a hParent node with items starting with lpsf and lpdil for this item
HRESULT CDirTreeCtrl::FillNode(HTREEITEM hParent, LPSHELLFOLDER lpsf, LPITEMIDLIST lpidl, bool bSilent)
{
	// get the global flag under consideration
	if (m_bIgnoreShellDialogs)
		bSilent=m_bIgnoreShellDialogs;

	// get the desktop interface and id's of list for net neigh. and my computer
	LPSHELLFOLDER lpsfDesktop=NULL;
	LPITEMIDLIST lpiidlDrives=NULL;
	LPITEMIDLIST lpiidlNetwork=NULL;
	HRESULT hResult = S_OK;
	if (hParent == GetRootItem())
	{
		hResult = SHGetDesktopFolder(&lpsfDesktop);
		if(SUCCEEDED(hResult))
			hResult = SHGetSpecialFolderLocation(this->GetSafeHwnd(), CSIDL_DRIVES, &lpiidlDrives);
		if(SUCCEEDED(hResult))
			hResult = SHGetSpecialFolderLocation(this->GetSafeHwnd(), CSIDL_NETWORK, &lpiidlNetwork);
	}

	// shell allocator
	LPMALLOC lpm = NULL;
	if(SUCCEEDED(hResult))
		hResult = SHGetMalloc(&lpm);

	// enumerate child items for lpsf
	LPENUMIDLIST lpeid = NULL;
	if(SUCCEEDED(hResult))
		hResult = lpsf->EnumObjects(bSilent ? NULL : GetParent()->GetSafeHwnd(), SHCONTF_FOLDERS | SHCONTF_INCLUDEHIDDEN, &lpeid);

	bool bFound=false;
	if(SUCCEEDED(hResult))
	{
		LPITEMIDLIST lpiidl;
		SHFILEINFO sfi;
		ULONG ulAttrib;
		TVITEM tvi;
		TVINSERTSTRUCT tvis;
		_SHELLITEMDATA *psid;
		TCHAR szText[_MAX_PATH];
		HTREEITEM hCurrent=NULL;
		while (lpeid->Next(1, &lpiidl, NULL) == NOERROR)
		{
			// filer what has been found
			ulAttrib=SFGAO_FOLDER | SFGAO_HASSUBFOLDER | SFGAO_FILESYSANCESTOR | SFGAO_FILESYSTEM;
			lpsf->GetAttributesOf(1, (const struct _ITEMIDLIST **)&lpiidl, &ulAttrib);
			if (ulAttrib & SFGAO_FOLDER && (ulAttrib & SFGAO_FILESYSANCESTOR || ulAttrib & SFGAO_FILESYSTEM) )
			{
				// there's something to add so set bFound
				bFound=true;

				// it's time to add everything
				psid=new _SHELLITEMDATA;
				lpsf->BindToObject(lpiidl, NULL, IID_IShellFolder, (void**)&psid->lpsf);
				psid->lpiidl=ConcatPidls(lpidl, lpiidl);
				psid->lpiidlRelative=CopyITEMID(lpm, lpiidl);
				psid->lpsfParent=lpsf;

				tvi.mask=TVIF_CHILDREN | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_TEXT | TVIF_PARAM;
				if (!GetName(lpsf, lpiidl, SHGDN_INFOLDER/* | SHGDN_INCLUDE_NONFILESYS*/, szText))
					lstrcpy(szText, _T("???"));
				tvi.pszText=szText;
				sfi.iIcon=-1;
				SHGetFileInfo((LPCTSTR)psid->lpiidl, 0, &sfi, sizeof(SHFILEINFO), SHGFI_PIDL | SHGFI_SYSICONINDEX | SHGFI_SMALLICON);
				tvi.iImage=sfi.iIcon;
				sfi.iIcon=-1;
				SHGetFileInfo((LPCTSTR)psid->lpiidl, 0, &sfi, sizeof(SHFILEINFO), SHGFI_PIDL | SHGFI_SYSICONINDEX | SHGFI_SMALLICON | SHGFI_OPENICON);
				tvi.iSelectedImage=sfi.iIcon;
				tvi.cChildren=(ulAttrib & SFGAO_HASSUBFOLDER);
				tvi.lParam=reinterpret_cast<LPARAM>(psid);

				tvis.hParent=hParent;
				tvis.item=tvi;
				hCurrent=InsertItem(&tvis);

				if (hParent == GetRootItem())
				{
					// if this is My computer or net. neigh. - it's better to remember the handles
					// compare psid->lpiidl and (lpiidlDrives & lpiidlNetwork)
					if (SCODE_CODE(lpsfDesktop->CompareIDs(0, psid->lpiidl, lpiidlDrives)) == 0)
						m_hDrives=hCurrent;
					else if (SCODE_CODE(lpsfDesktop->CompareIDs(0, psid->lpiidl, lpiidlNetwork)) == 0)
						m_hNetwork=hCurrent;
				}

				FreePidl(lpiidl);	// free found pidl - it was copied already
			}
		}
	}

	if(lpeid)
		lpeid->Release();
	if(lpsfDesktop)
		lpsfDesktop->Release();
	if(lpm)
		lpm->Release();

	// sortuj
	if(SUCCEEDED(hResult))
	{
		if(bFound)
		{
			TVSORTCB tvscb;
			tvscb.hParent=hParent;
			tvscb.lpfnCompare=&CompareFunc;
			tvscb.lParam=NULL;
			if (!SortChildrenCB(&tvscb))
				TRACE("SortChildren failed\n");
		}
		else
		{
			// some items has + and some not - correction
			TVITEM tvi;
			tvi.mask=TVIF_HANDLE | TVIF_CHILDREN;
			tvi.hItem=hParent;
			if (GetItem(&tvi) && tvi.cChildren == 1)
			{
				tvi.cChildren=0;
				SetItem(&tvi);
			}
		}
	}
	
	return hResult;
}

////////////////////////////////////////////////////////////////////////////
// alternate function for Expand(), makes additional processing
bool CDirTreeCtrl::ExpandItem(HTREEITEM hItem, UINT nCode)
{
	switch(nCode)
	{
	case TVE_EXPAND:
		{
			// get the item's data
			TVITEM tvi;
			tvi.mask=TVIF_PARAM | TVIF_STATE;
			tvi.hItem=hItem;
			tvi.stateMask=TVIS_EXPANDEDONCE | TVIS_SELECTED | TVIS_EXPANDED;
			if (!GetItem(&tvi))
				return false;
			
			if (tvi.state & TVIS_EXPANDED)
				return true;

			// Fill node before normal expanding
			SHELLITEMDATA* psid;
			if (GetItemStruct(hItem, &psid))
			{
				if(FAILED(FillNode(hItem, psid->lpsf, psid->lpiidl, true)))
					TRACE("FillNode in ExpandItem failed...\n");

				// ignore fillnode in onitemexpanding
				m_bIgnore=true;
			}

			// normal expand
			EnsureVisible(hItem);
			SelectItem(hItem);
			return Expand(hItem, TVE_EXPAND) != 0;

			break;
		}
	default:
		return Expand(hItem, nCode) != 0;
	}
}

/////////////////////////////////////////////////////////////////////////////
// node expand handling - calls FillNode
void CDirTreeCtrl::OnItemexpanding(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	_SHELLITEMDATA* psid=reinterpret_cast<_SHELLITEMDATA*>(pNMTreeView->itemNew.lParam);

	switch (pNMTreeView->action)
	{
	case TVE_EXPAND:
		if (!m_bIgnore)
		{
			// fill
			if (FAILED(FillNode(pNMTreeView->itemNew.hItem, psid->lpsf, psid->lpiidl)))
				TRACE("FillNode failed...\n");
		}
		else
		{
			// now refresh normally
			m_bIgnore=false;
		}
		break;
	}

	*pResult = 0;
}

////////////////////////////////////////////////////////////////////////////
// deleting items after node collapses
void CDirTreeCtrl::OnItemexpanded(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	
	switch (pNMTreeView->action)
	{
	case TVE_COLLAPSE:
		if (ItemHasChildren(pNMTreeView->itemNew.hItem))
		{
			HTREEITEM hItem;
			while ((hItem=GetChildItem(pNMTreeView->itemNew.hItem)) != NULL)
				DeleteItem(hItem);
		}

		break;
	}

	*pResult = 0;
}

//////////////////////////////////////////////////////////////////////////////
// deletes everything what has been allocated for an item
void CDirTreeCtrl::OnDeleteitem(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	_SHELLITEMDATA *psid=reinterpret_cast<_SHELLITEMDATA*>(pNMTreeView->itemOld.lParam);
	
	if(psid)
	{
		if(psid->lpsf)
			psid->lpsf->Release();
		FreePidl(psid->lpiidl);
		FreePidl(psid->lpiidlRelative);
		delete psid;
	}

	*pResult = 0;
}

///////////////////////////////////////////////////////////////////////////
// returns path associated with an item
bool CDirTreeCtrl::GetPath(HTREEITEM hItem, LPTSTR pszPath)
{
	TVITEM tvi;
	tvi.mask=TVIF_HANDLE | TVIF_PARAM;
	tvi.hItem=hItem;

	if (GetItem(&tvi))
	{
		// item data
		_SHELLITEMDATA* psid=reinterpret_cast<_SHELLITEMDATA*>(tvi.lParam);
		if (psid == NULL)
			return false;

		// desktop interface
		LPSHELLFOLDER lpsf;
		if (SHGetDesktopFolder(&lpsf) != NOERROR)
			return false;

		if (!SHGetPathFromIDList(psid->lpiidl, pszPath))
		{
			lpsf->Release();
			return false;
		}

		lpsf->Release();
		return true;
	}
	else
		return false;
}

////////////////////////////////////////////////////////////////////////////
// finds the item that is nearest to a given path - returns true if find
// even a part of a path
bool CDirTreeCtrl::SetPath(LPCTSTR lpszPath)
{
	ASSERT(lpszPath);
	if(!lpszPath)
		return false;

	// if path is empty
	if (_tcscmp(lpszPath, _T("")) == 0)
		return false;

	// type of path
	bool bNetwork=_tcsncmp(lpszPath, _T("\\\\"), 2) == 0;

	if (!bNetwork)
		return SetLocalPath(lpszPath);
	else
	{
		// we don't look in net neighbourhood for speed reasons
		EnsureVisible(m_hNetwork);
//		SelectItem(m_hNetwork);
//		ExpandItem(m_hNetwork, TVE_EXPAND);
		return true;
	}
}


///////////////////////////////////////////////////////////////////////////
// sets the local path - not network
bool CDirTreeCtrl::SetLocalPath(LPCTSTR lpszPath)
{
	// expand desktop and my computer
	HTREEITEM hItem=GetRootItem();
	ExpandItem(hItem, TVE_EXPAND);
	ExpandItem(m_hDrives, TVE_EXPAND);
	
	HTREEITEM hFound=RegularSelect(m_hDrives, lpszPath);
	if (hFound)
	{
		EnsureVisible(hFound);
		SelectItem(hFound);
		ExpandItem(hFound, TVE_EXPAND);
	}
	
	return hFound != NULL;
}

///////////////////////////////////////////////////////////////////////////
// starts regular selecting - if finds the first part of a path - the next
// component is a child item
HTREEITEM CDirTreeCtrl::RegularSelect(HTREEITEM hStart, LPCTSTR lpszPath)
{
	// some interfaces
	_SHELLITEMDATA* psid;
	TCHAR szPath[_MAX_PATH];
	TVITEM tvi;
	
	// traverse the child items of my computer
	HTREEITEM hItem=GetChildItem(hStart), hLast=NULL;
	while (hItem != NULL)
	{
		tvi.mask=TVIF_HANDLE | TVIF_PARAM;
		tvi.hItem=hItem;
		if (GetItem(&tvi))
		{
			// getting info about item
			psid=reinterpret_cast<_SHELLITEMDATA*>(tvi.lParam);
			
			if (SHGetPathFromIDList(psid->lpiidl, szPath))
			{
				if (ComparePaths(lpszPath, szPath))
				{
					// it's contained - expand
					ExpandItem(hItem, TVE_EXPAND);
					hLast=hItem;	// remember last that matches path
					
					hItem=GetChildItem(hItem);	// another 'zoom'
					continue;
				}
			}
		}

		// next folder
		hItem=GetNextSiblingItem(hItem);
	}
	
	// return what has been found
	return hLast;
}
////////////////////////////////////////////////////////////////////////////
// helper - finds comp in a network that matches the path. Skips all the
// workgroups, network names, ...
/*
HTREEITEM CDirTreeCtrl::TraverseNetNode(HTREEITEM hItem, LPCTSTR lpszPath, LPTSTR lpszBuffer)
{
	// start with 1st chil item
	HTREEITEM hNext=GetChildItem(hItem);
	NETRESOURCE *pnet;

	while (hNext)
	{
		// get NETRESOURCE structure - is this a server ?
		if (GetItemShellData(hNext, SHGDFIL_NETRESOURCE, lpszBuffer, 2048))
		{
			// got NETRESOURCE
			pnet=(NETRESOURCE*)lpszBuffer;

			// is the path contained
			if (ComparePaths(lpszPath, pnet->lpRemoteName))
				return hNext;	// found what's needed

			if (pnet->dwDisplayType != RESOURCEDISPLAYTYPE_SERVER)
			{
				// expand
				if (!ExpandItem(hNext, TVE_EXPAND))
					return NULL;

				// recurse
				HTREEITEM hFound;
				if ( (hFound=TraverseNetNode(hNext, lpszPath, lpszBuffer)) != NULL)
					return hFound;	// if found - return, if not - continue search
			}
		}

		// next item to check.
		hNext=GetNextSiblingItem(hNext);
	}

	return NULL;	// nothing has been found
}
*/
////////////////////////////////////////////////////////////////////////////
// compares two paths - if one is contained in another (to the first '\\' or '/'
bool CDirTreeCtrl::ComparePaths(LPCTSTR lpszFull, LPCTSTR lpszPartial)
{
	CString strSrc=lpszFull, strFnd=lpszPartial;
	strFnd.MakeUpper();
	strFnd.TrimRight(_T("\\/"));
				
	// make uppercas the source string, cut before nearest \\ or / after strFnd.GetLength
	strSrc.MakeUpper();
				
	// find out the position of a nearest / lub '\\'
	int iLen=strFnd.GetLength(), iPos;
	if (strSrc.GetLength() >= iLen)
	{
		iPos=strSrc.Mid(iLen).FindOneOf(_T("\\/"));
		if (iPos != -1)
			strSrc=strSrc.Left(iPos+iLen);

		return strSrc == strFnd;
	}
	else
		return false;
}

////////////////////////////////////////////////////////////////////////////
// returns shell description for an item (like in explorer).
bool CDirTreeCtrl::GetItemInfoTip(HTREEITEM hItem, CString* pTip)
{
	_SHELLITEMDATA* psid=(_SHELLITEMDATA*)GetItemData(hItem);
	if (psid == NULL || psid->lpiidlRelative == NULL || psid->lpsfParent == NULL)
		return false;

	// get interface
	IQueryInfo *pqi;
	if (psid->lpsfParent->GetUIObjectOf(this->GetSafeHwnd(), 1, (const struct _ITEMIDLIST**)&psid->lpiidlRelative, IID_IQueryInfo, 0, (void**)&pqi) != NOERROR)
		return false;

	// get tip
	WCHAR *pszTip;
	if (pqi->GetInfoTip(0, &pszTip) != NOERROR)
	{
		pqi->Release();
		return false;
	}

	// copy with a conversion
	*pTip=(const WCHAR *)pszTip;

	// uwolnij pamiêæ skojarzon¹ z pszTip
	LPMALLOC lpm;
	if (SHGetMalloc(&lpm) == NOERROR)
	{
		lpm->Free(pszTip);
		lpm->Release();
	}

	pqi->Release();

	return true;
}

////////////////////////////////////////////////////////////////////////////
// better exchange for SHGetDataFromIDList
bool CDirTreeCtrl::GetItemShellData(HTREEITEM hItem, int nFormat, PVOID pBuffer, int iSize)
{
	PSHELLITEMDATA psid;
	if (!GetItemStruct(hItem, &psid) || psid->lpsfParent == NULL || psid->lpiidlRelative == NULL)
		return false;

	return SHGetDataFromIDList(psid->lpsfParent, psid->lpiidlRelative, nFormat, pBuffer, iSize) == NOERROR;
}

/////////////////////////////////////////////////////////////////////////////
// returns SHELLITEMDATA associated with an item
bool CDirTreeCtrl::GetItemStruct(HTREEITEM hItem, PSHELLITEMDATA *ppsid)
{
	ASSERT(ppsid);
	if(!ppsid)
		return false;
	*ppsid=(PSHELLITEMDATA)GetItemData(hItem);
	return *ppsid != NULL;
}

HTREEITEM CDirTreeCtrl::InsertNewFolder(HTREEITEM hParent, LPCTSTR /*lpszNewFolder*/)
{
	// check if HTREEITEM has an associated path
	TCHAR szPath[_MAX_PATH];
	if (!GetPath(hParent, szPath))
		return NULL;
	
	// focus
	SetFocus();

	// if has child items - enumerate
	TVITEM tvi;
	tvi.mask=TVIF_HANDLE | TVIF_STATE | TVIF_CHILDREN;
	tvi.hItem=hParent;
	tvi.stateMask=TVIS_EXPANDED;
	if (GetItem(&tvi))
	{
		if (!(tvi.state & TVIS_EXPANDED))
			TRACE("InsertNewFolder's expanditem returned %u\n", ExpandItem(hParent, TVE_EXPAND));
		TRACE("%lu child items\n", tvi.cChildren);
	}

	// if hParent doesn't have any chilren - add + to make it look like it hastak, aby wydawa³o siê, ¿e ma
	if (!ItemHasChildren(hParent))
	{
		tvi.mask=TVIF_HANDLE | TVIF_CHILDREN;
		tvi.hItem=hParent;
		tvi.cChildren=1;
		if (!SetItem(&tvi))
			TRACE("SetItem failed...\n");
	}

	// temp buffer for a name
	TCHAR *pszPath=new TCHAR[1];
	pszPath[0]=_T('\0');

	// insert new item with an empty lParam
	TVINSERTSTRUCT tvis;
	tvis.hParent=hParent;
	tvis.hInsertAfter=TVI_SORT;
	tvis.item.mask=TVIF_TEXT | TVIF_PARAM | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
	tvis.item.iImage=-1;
	tvis.item.iSelectedImage=-1;
	tvis.item.pszText=pszPath;
	tvis.item.cchTextMax=lstrlen(tvis.item.pszText);
	tvis.item.lParam=NULL;

	HTREEITEM hRes=InsertItem(&tvis);

	delete [] pszPath;

	// now make sure hParent is expanded
	Expand(hParent, TVE_EXPAND);

	// edit an item
	if (hRes)
	{
		m_iEditType=1;
		CEdit *pctlEdit=EditLabel(hRes);
		if (pctlEdit == NULL)
			return NULL;
	}

	return hRes;
}

void CDirTreeCtrl::OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NMTVDISPINFO* pdi = (NMTVDISPINFO*)pNMHDR;
	
	if (m_iEditType == 1)	// for a new folder
	{
		if (pdi->item.pszText && _tcslen(pdi->item.pszText) != 0)
		{
			// no success at beginning
			int iResult=0;

			// item - parent
			HTREEITEM hParent=GetParentItem(pdi->item.hItem);
			
			// get parent folder name
			TCHAR szPath[_MAX_PATH];
			if (!GetPath(hParent, szPath))
			{
				// there's no path - skip
				*pResult=0;
				m_iEditType=0;

				// delete item
				DeleteItem(pdi->item.hItem);
				GetParent()->SendMessage(WM_CREATEFOLDERRESULT, 0, iResult);
				return;
			}
			
			// try to create folder
			CString strPath=szPath;
			if (strPath.Right(1) != _T('\\'))
				strPath+=_T('\\');

			// full path to the new folder
			strPath+=pdi->item.pszText;

			// new folder
			if (CreateDirectory(strPath, NULL))
				iResult = 1;

			// refresh - delete all from hParent and fill node
			HTREEITEM hChild;
			while ((hChild=GetChildItem(hParent)) != NULL)
				DeleteItem(hChild);

			// now fillnode
			SHELLITEMDATA* psid;
			if (GetItemStruct(hParent, &psid))
			{
				if (FAILED(FillNode(hParent, psid->lpsf, psid->lpiidl)))
					TRACE("FillNode in EndEditLabel failed...\n");
			}

			hChild=GetChildItem(hParent);
			while(hChild)
			{
				if (!GetPath(hChild, szPath))
				{
					hChild=GetNextSiblingItem(hChild);
					continue;
				}

				if (_tcscmp(strPath, szPath) == 0)
				{
					// zaznacz
					ExpandItem(hChild, TVE_EXPAND);
					break;
				}

				hChild=GetNextSiblingItem(hChild);
			}
			

			// another members
			*pResult=1;
			m_iEditType=0;

			GetParent()->SendMessage(WM_CREATEFOLDERRESULT, 0, iResult);
		}
		else
		{
			// no path - skip
			*pResult=0;
			m_iEditType=0;

			// delete an item
			DeleteItem(pdi->item.hItem);
		}
	}
	
	*pResult = 0;
}

void CDirTreeCtrl::OnBeginLabelEdit(NMHDR* /*pNMHDR*/, LRESULT* pResult)
{
	if (m_iEditType == 1)
		*pResult=0;
	else
		*pResult=1;
}

BOOL CDirTreeCtrl::OnEraseBkgnd(CDC* /*pDC*/)
{
	return FALSE;
}

void CDirTreeCtrl::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	CMemDC memdc(&dc, &dc.m_ps.rcPaint);
	
	DefWindowProc(WM_PAINT, (WPARAM)memdc.GetSafeHdc(), 0);
}

void CDirTreeCtrl::SetIgnoreShellDialogs(bool bFlag)
{
	m_bIgnoreShellDialogs=bFlag;
}

bool CDirTreeCtrl::GetIgnoreShellDialogs()
{
	return m_bIgnoreShellDialogs;
}
