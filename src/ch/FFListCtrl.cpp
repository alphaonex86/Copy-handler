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
#include "FFListCtrl.h"
#include "MemDC.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFFListCtrl

CFFListCtrl::CFFListCtrl()
{
}

CFFListCtrl::~CFFListCtrl()
{
}


BEGIN_MESSAGE_MAP(CFFListCtrl, CListCtrl)
	//{{AFX_MSG_MAP(CFFListCtrl)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_NOTIFY_REFLECT(LVN_ITEMCHANGED, OnItemchanged)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFFListCtrl message handlers

BOOL CFFListCtrl::OnEraseBkgnd(CDC*) 
{
	return FALSE;/*CListCtrl::OnEraseBkgnd(pDC);*/
}

void CFFListCtrl::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	CRect headerRect;
	GetHeaderCtrl()->GetWindowRect(&headerRect);
	ScreenToClient(&headerRect);
	dc.ExcludeClipRect(&headerRect);

	CRect rect;
	GetClientRect(&rect);
	CMemDC memDC(&dc, rect);
	
	CRect clip;
	memDC.GetClipBox(&clip);
	memDC.FillSolidRect(clip, GetSysColor(COLOR_WINDOW));

	DefWindowProc(WM_PAINT, (WPARAM)memDC.m_hDC, (LPARAM)0);
}

void CFFListCtrl::LimitItems(int iLimit)
{
	if (GetItemCount() > iLimit)
	{
		while (GetItemCount() > iLimit)
			DeleteItem(iLimit);
	}
}

void CFFListCtrl::OnItemchanged(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	if ( /*((pNMListView->uOldState & LVIS_SELECTED) && !(pNMListView->uNewState & LVIS_SELECTED))
		||*/ (!(pNMListView->uOldState & LVIS_SELECTED) && (pNMListView->uNewState & LVIS_SELECTED)) )
		SendSelChangedToParent();
	
	*pResult = 0;
}

void CFFListCtrl::SendSelChangedToParent()
{
	NMHDR nmhdr;
	nmhdr.hwndFrom=m_hWnd;
	nmhdr.idFrom=GetDlgCtrlID();
	nmhdr.code=LVN_CHANGEDSELECTION;
	GetParent()->SendMessage(WM_NOTIFY, static_cast<WPARAM>(nmhdr.idFrom), reinterpret_cast<LPARAM>(&nmhdr));	
}
