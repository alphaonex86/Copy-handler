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
#include "ThemedButton.h"
#include "MemDC.h"
#include "Theme helpers.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CThemedButton

// undefine this if use some internal windows files
#define TP_BUTTON		1
#define TS_NORMAL 1
#define TS_HOT 2
#define TS_PRESSED 3
#define TS_DISABLED 4
#define TS_CHECKED 5
#define TS_HOTCHECKED 6

CThemedButton::CThemedButton()
{
	m_bHovering=false;
	m_iIndex=-1;
	m_pilList=NULL;
}

CThemedButton::~CThemedButton()
{
}


BEGIN_MESSAGE_MAP(CThemedButton, CButton)
	//{{AFX_MSG_MAP(CThemedButton)
	ON_WM_MOUSEMOVE()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CThemedButton message handlers

void CThemedButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	CDC* pDC=CDC::FromHandle(lpDrawItemStruct->hDC);
	CMemDC memdc(pDC, &lpDrawItemStruct->rcItem);
	
	bool bPushed=(lpDrawItemStruct->itemState & ODS_SELECTED);
	CRect rcItem=lpDrawItemStruct->rcItem;
	
	// draw button's frame
	CUxThemeSupport uxTheme;
	if (uxTheme.IsThemeSupported() && uxTheme.IsAppThemed())
	{
		HTHEME ht=uxTheme.OpenThemeData(lpDrawItemStruct->hwndItem, L"TOOLBAR");
		
		uxTheme.DrawThemeParentBackground(lpDrawItemStruct->hwndItem, memdc.GetSafeHdc(), &rcItem);
		uxTheme.DrawThemeBackground(ht, memdc.GetSafeHdc(), TP_BUTTON, bPushed ? TS_PRESSED : (m_bHovering ? TS_HOT : TS_NORMAL), &rcItem, NULL);
		
		uxTheme.CloseThemeData(ht);
	}
	else
		DrawFrameControl(memdc.GetSafeHdc(), &rcItem, DFC_BUTTON, DFCS_ADJUSTRECT | DFCS_BUTTONPUSH | (bPushed ? DFCS_PUSHED : 0));

	ASSERT(m_pilList);	// make sure the image list exist
	if(m_pilList)
	{
		int cx=16, cy=16;
		ImageList_GetIconSize(m_pilList->m_hImageList, &cx, &cy);

		CRect rcBtn;
		GetClientRect(&rcBtn);
		m_pilList->Draw(&memdc, m_iIndex, CPoint( ((rcBtn.Width()-cx)/2)+(bPushed ? 1 : 0), (rcBtn.Height()-cy)/2+(bPushed ? 1 : 0)), ILD_TRANSPARENT);
	}
}

void CThemedButton::OnMouseMove(UINT nFlags, CPoint point) 
{
	if (!m_bHovering)
	{
		TRACKMOUSEEVENT tme;
		tme.cbSize=sizeof(TRACKMOUSEEVENT);
		tme.dwFlags=TME_LEAVE;
		tme.dwHoverTime=0;
		tme.hwndTrack=this->GetSafeHwnd();

		::_TrackMouseEvent(&tme);

		m_bHovering=true;
		Invalidate();
	}

	CButton::OnMouseMove(nFlags, point);
}

LRESULT CThemedButton::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	switch(message)
	{
	case WM_LBUTTONDBLCLK:
		message=WM_LBUTTONDOWN;
		break;
	case WM_MOUSELEAVE:
		m_bHovering=false;
		Invalidate();
		break;
	}

	return CButton::WindowProc(message, wParam, lParam);
}

BOOL CThemedButton::OnEraseBkgnd(CDC* /*pDC*/) 
{
	return FALSE;/*CButton::OnEraseBkgnd(pDC);*/
}

void CThemedButton::SetImage(CImageList *pImgList, int iIndex)
{
	m_pilList=pImgList;
	m_iIndex=iIndex;
}
