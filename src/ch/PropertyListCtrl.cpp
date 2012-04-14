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
#include "ch.h"
#include "PropertyListCtrl.h"
#include "dialogs.h"
#include "memdc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CComboButton
CComboButton::CComboButton()
{
}

CComboButton::~CComboButton()
{
	// Delete the objects created
	delete m_pBkBrush;
	delete m_pBlackBrush;
//	delete m_pGrayPen;
	delete m_pBkPen;
}

BEGIN_MESSAGE_MAP(CComboButton, CButton)
	//{{AFX_MSG_MAP(CComboButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CComboButton message handlers
BOOL CComboButton::Create( CRect Rect, CWnd* pParent, UINT uID)
{
	// Create the Brushes and Pens
	m_pBkBrush		= new CBrush( GetSysColor(COLOR_BTNFACE));
	m_pBkPen		= new CPen( PS_SOLID, 1, GetSysColor(COLOR_BTNFACE));
//	m_pGrayPen		= new CPen( PS_SOLID, 1, RGB(128,128,128));
	m_pBlackBrush	= new CBrush(GetSysColor(COLOR_BTNTEXT)); 

    // Create the CButton
	if( !CButton::Create(_T(""), WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON|BS_OWNERDRAW, Rect, pParent, uID ))
		return FALSE;
		
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// Draw the Button
void CComboButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct )
{
	CDC*	prDC			= CDC::FromHandle(lpDrawItemStruct->hDC);
	CRect 	ButtonRect  = lpDrawItemStruct->rcItem;
	CMemDC dc(prDC, ButtonRect);
	CMemDC *pDC=&dc;
    
	// Fill the Background
	CBrush* pOldBrush = (CBrush*)pDC->SelectObject( m_pBkBrush );
	CPen* pOldPen = (CPen*)pDC->SelectObject(m_pBkPen);
	pDC->Rectangle(ButtonRect);
		
	// Draw the Correct Border
	if(lpDrawItemStruct->itemState & ODS_SELECTED)
	{
		pDC->DrawEdge(ButtonRect, EDGE_SUNKEN, BF_RECT);
		ButtonRect.left++;
		ButtonRect.right++;
		ButtonRect.bottom++;
		ButtonRect.top++;
	}
	else
		pDC->DrawEdge(ButtonRect, EDGE_RAISED, BF_RECT);
	
	// Draw the Triangle
	ButtonRect.left		+= 3;
	ButtonRect.right	-= 4;
	ButtonRect.top		+= 5;
	ButtonRect.bottom	-= 5;
	DrawTriangle(pDC, ButtonRect);

	// Return what was used
	pDC->SelectObject( pOldPen );
	pDC->SelectObject( pOldBrush );
}

void CComboButton::DrawTriangle(CDC* pDC, CRect Rect)
{
	POINT 	ptArray[3];	

	// Figure out the Top left
	ptArray[0].x = Rect.left;
	ptArray[0].y = Rect.top;
	ptArray[1].x = Rect.right;
	ptArray[1].y = Rect.top;
	ptArray[2].x = Rect.right - (Rect.Width() / 2);
	ptArray[2].y = Rect.bottom;

	// Select the Brush and Draw the triangle
	/*CBrush* pOldBrush = (CBrush*)*/pDC->SelectObject(m_pBlackBrush);
	pDC->Polygon(ptArray, 3 );
}
void CComboButton::MeasureItem(LPMEASUREITEMSTRUCT/* lpMeasureItemStruct*/)
{
}

/////////////////////////////////////////////////////////////////////////////
// CPropertyListCtrl
CPropertyListCtrl::CPropertyListCtrl()
{
	m_nWidestItem			= 0;
	m_bDeleteFont			= TRUE;
	m_bBoldSelection		= TRUE;

	m_pBkBrush				= NULL;
	m_pBkPropertyBrush		= NULL;
	m_pEditWnd				= NULL;
	m_pFontButton			= NULL;
	m_pPathButton			= NULL;
	m_pDirButton=NULL;
	m_pCustomButton=NULL;
	m_pComboButton			= NULL;
	m_pListBox				= NULL;
	m_pBkHighlightBrush		= NULL;
	m_pSelectedFont			= NULL;
	m_pBorderPen			= NULL;
	m_pCurItem				= NULL;
	m_pCurFont				= NULL;
	m_pCurDrawItem			= NULL;
	m_pTextFont				= NULL;
	m_pSelectedFont			= NULL;
	m_pBorderPen			= NULL;

	m_crBorderColor			= RGB(192,192,192);
	m_crBkColor				= GetSysColor(COLOR_WINDOW);
	m_crPropertyBkColor		= m_crBkColor;
	m_crTextColor			= GetSysColor(COLOR_WINDOWTEXT);
	m_crPropertyTextColor	= m_crTextColor;
	m_crHighlightColor		= GetSysColor(COLOR_HIGHLIGHT);
	m_crTextHighlightColor	= GetSysColor(COLOR_HIGHLIGHTTEXT);
}

CPropertyListCtrl::~CPropertyListCtrl()
{
	if(m_bDeleteFont)		delete m_pTextFont;

	if(m_pEditWnd)			delete m_pEditWnd;
	if(m_pFontButton)		delete m_pFontButton;
	if(m_pPathButton)		delete m_pPathButton;
	if (m_pDirButton) delete m_pDirButton;
	if (m_pCustomButton)
		delete m_pCustomButton;
	if(m_pListBox)			delete m_pListBox;
	if(m_pComboButton)		delete m_pComboButton;

	if(m_pBkBrush)			delete m_pBkBrush;
	if(m_pBkPropertyBrush)	delete m_pBkPropertyBrush;
	if(m_pBkHighlightBrush) delete m_pBkHighlightBrush;
	if(m_pSelectedFont)		delete m_pSelectedFont;
	if(m_pBorderPen)		delete m_pBorderPen;

	// Clear items
	Reset();
}

void CPropertyListCtrl::Reinit()
{
	ResetContent();

	// Clean up
	if(m_bDeleteFont)		delete m_pTextFont;

	if(m_pEditWnd)			delete m_pEditWnd;
	if(m_pFontButton)		delete m_pFontButton;
	if(m_pPathButton)		delete m_pPathButton;
	if (m_pDirButton) delete m_pDirButton;
	if (m_pCustomButton)
		delete m_pCustomButton;
	if(m_pListBox)			delete m_pListBox;
	if(m_pComboButton)		delete m_pComboButton;

	if(m_pBkBrush)			delete m_pBkBrush;
	if(m_pBkPropertyBrush)	delete m_pBkPropertyBrush;
	if(m_pBkHighlightBrush) delete m_pBkHighlightBrush;
	if(m_pSelectedFont)		delete m_pSelectedFont;
	if(m_pBorderPen)		delete m_pBorderPen;

	// Clear items
	Reset();

	m_nWidestItem			= 0;
	m_bDeleteFont			= TRUE;
	m_bBoldSelection		= TRUE;

	m_pBkBrush				= NULL;
	m_pBkPropertyBrush		= NULL;
	m_pEditWnd				= NULL;
	m_pFontButton			= NULL;
	m_pPathButton			= NULL;
	m_pDirButton=NULL;
	m_pCustomButton=NULL;
	m_pComboButton			= NULL;
	m_pListBox				= NULL;
	m_pBkHighlightBrush		= NULL;
	m_pSelectedFont			= NULL;
	m_pBorderPen			= NULL;
	m_pCurItem				= NULL;
	m_pCurFont				= NULL;
	m_pCurDrawItem			= NULL;
	m_pTextFont				= NULL;
	m_pSelectedFont			= NULL;
	m_pBorderPen			= NULL;

	m_crBorderColor			= RGB(192,192,192);
	m_crBkColor				= GetSysColor(COLOR_WINDOW);
	m_crPropertyBkColor		= m_crBkColor;
	m_crTextColor			= GetSysColor(COLOR_WINDOWTEXT);
	m_crPropertyTextColor	= m_crTextColor;
	m_crHighlightColor		= GetSysColor(COLOR_HIGHLIGHT);
	m_crTextHighlightColor	= GetSysColor(COLOR_HIGHLIGHTTEXT);

	Init();
}

void CPropertyListCtrl::Reset()
{	
	// Clear the List
	POSITION Pos = m_Items.GetHeadPosition();
	while(Pos)
	{
		m_pCurItem = (PROPERTYITEM*)m_Items.GetNext(Pos);
		if(m_pCurItem->pBrush)
			delete m_pCurItem->pBrush;
		delete m_pCurItem;
	}
	m_Items.RemoveAll();
}
	
BEGIN_MESSAGE_MAP(CPropertyListCtrl, CListBox)
	//{{AFX_MSG_MAP(CPropertyListCtrl)
	ON_WM_CREATE()
	ON_WM_CTLCOLOR_REFLECT()
	ON_CONTROL_REFLECT(LBN_SELCHANGE, OnSelchange)
	ON_WM_CTLCOLOR()
	ON_CONTROL_REFLECT(LBN_DBLCLK, OnDblclk)
	ON_EN_KILLFOCUS( ID_PROPERTY_TEXT, OnEditLostFocus )
	ON_EN_CHANGE( ID_PROPERTY_TEXT, OnEditChange )
	ON_BN_CLICKED( ID_PROPERTY_FONT, OnFontPropertyClick )
	ON_BN_CLICKED( ID_PROPERTY_PATH, OnPathPropertyClick )
	ON_BN_CLICKED( ID_PROPERTY_DIR, OnDirPropertyClick )
	ON_BN_CLICKED( ID_PROPERTY_CUSTOM, OnCustomPropertyClick )
	ON_BN_CLICKED( ID_PROPERTY_COMBO_BTN, OnComboBoxClick )
	ON_LBN_SELCHANGE(ID_PROPERTY_COMBO_LIST, OnSelChange)
	ON_LBN_KILLFOCUS(ID_PROPERTY_COMBO_LIST, OnListboxLostFocus)
	ON_WM_LBUTTONDOWN()
	ON_WM_VSCROLL()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPropertyListCtrl message handlers

HBRUSH CPropertyListCtrl::CtlColor(CDC* /*pDC*/, UINT/* nCtlColor*/) 
{	
	return (HBRUSH)m_pBkBrush->GetSafeHandle();
}

HBRUSH CPropertyListCtrl::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CListBox::OnCtlColor(pDC, pWnd, nCtlColor);
	
/*	if( nCtlColor == CTLCOLOR_EDIT) 
	{
		pDC->SetBkColor(m_crPropertyBkColor);
		pDC->SetTextColor(m_crPropertyTextColor);
	}*/
	pDC->SetBkColor(m_crPropertyBkColor);
	pDC->SetTextColor(m_crPropertyTextColor);

	if(m_pBkPropertyBrush)
		return (HBRUSH)(m_pBkPropertyBrush->GetSafeHandle() );
	else
		return hbr;
}
void CPropertyListCtrl::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	// Make sure its a valid item
	if( lpDrawItemStruct->itemID == LB_ERR )
		return;
     
  	// Obtain the text for this item
	m_csText.Empty();
	GetText(lpDrawItemStruct->itemID, m_csText);

	// Get the drawing DC
	CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
	
	// Set the Current member we are drawing
    m_pCurDrawItem = (PROPERTYITEM*)m_Items.GetAt(m_Items.FindIndex(lpDrawItemStruct->itemID));
	
	// Obtain the Item Rect
	CRect ItemRect(lpDrawItemStruct->rcItem);
	
    // Draw This item
	DrawItem( pDC, ItemRect, lpDrawItemStruct->itemState & ODS_SELECTED);
}
void CPropertyListCtrl::MeasureItem(LPMEASUREITEMSTRUCT /*lpMeasureItemStruct*/) 
{
}
void CPropertyListCtrl::OnDblclk() 
{
	// Get the Course Position
	POINT pPoint;
	GetCursorPos(&pPoint);
	
	// Convert this rect to coordinates of the desktop	
	CRect TempRect = m_CurRect;
	MapWindowPoints(GetDesktopWindow(), TempRect);

	// Display the Correct Control
	switch(m_pCurItem->nType)
	{
		case	ID_PROPERTY_BOOL:	
				// Is the Mouse in this area
				if(TempRect.PtInRect(pPoint))
				{
					// Reverse the Selection
					m_pCurItem->nPropertySelected = !m_pCurItem->nPropertySelected;
					
					// Redraw this item
					RedrawWindow();

					// Send the message that a property has changed
					GetParent()->PostMessage(ID_PROPERTY_CHANGED, GetCurSel(), m_pCurItem->nType);
				}
				break;

		case	ID_PROPERTY_FONT:
				m_pFontButton->SetFocus();		
				OnFontPropertyClick();
				break;

		case	ID_PROPERTY_PATH:
				m_pPathButton->SetFocus();		
				OnPathPropertyClick();
				break;

		case ID_PROPERTY_DIR:
				m_pDirButton->SetFocus();
				OnDirPropertyClick();
				break;

		case ID_PROPERTY_CUSTOM:
				m_pCustomButton->SetFocus();
				OnCustomPropertyClick();
				break;

		case	ID_PROPERTY_COLOR:
				LOGBRUSH lb;
				m_pCurItem->pBrush->GetLogBrush(&lb);
				CColorDialog ColorDialog(lb.lbColor, 0, GetParent());
				if(ColorDialog.DoModal() != IDOK)
					return;

				// Destroy the Brush and create a new one
				if(m_pCurItem->pBrush) delete m_pCurItem->pBrush;
				m_pCurItem->pBrush = new CBrush(ColorDialog.GetColor());

				// Redraw the Widow (Theres probably a better way)
				RedrawWindow();

				// Send the message that a property has changed
				GetParent()->PostMessage(ID_PROPERTY_CHANGED, GetCurSel(), m_pCurItem->nType);
				break;
	}
	
}
void CPropertyListCtrl::OnSelchange() 
{		
	HideControls();

	// Display the Correct Control
	CRect TempRect = m_CurRect;
	TempRect.InflateRect(-1,-1);
	switch(m_pCurItem->nType)
	{
		case	ID_PROPERTY_TEXT:
				TempRect.left += 1;
				m_pEditWnd->SetWindowText(m_pCurItem->csProperties.GetAt(0));
				m_pEditWnd->MoveWindow(TempRect);
				m_pEditWnd->ShowWindow(SW_SHOWNORMAL);
//				m_pEditWnd->SetFocus();
//				m_pEditWnd->SetSel(0,-1);
				break;

		case	ID_PROPERTY_FONT:
				TempRect.left = TempRect.right - 17;
				m_pFontButton->MoveWindow(TempRect);
				m_pFontButton->ShowWindow(SW_SHOWNORMAL);
				break;

		case	ID_PROPERTY_PATH:
				TempRect.left = TempRect.right - 17;
				m_pPathButton->MoveWindow(TempRect);
				m_pPathButton->ShowWindow(SW_SHOWNORMAL);
				break;

		case ID_PROPERTY_DIR:
				TempRect.left = TempRect.right - 17;
				m_pDirButton->MoveWindow(TempRect);
				m_pDirButton->ShowWindow(SW_SHOWNORMAL);
				break;

		case ID_PROPERTY_CUSTOM:
				TempRect.left = TempRect.right - 17;
				m_pCustomButton->MoveWindow(TempRect);
				m_pCustomButton->ShowWindow(SW_SHOWNORMAL);
				break;

		case	ID_PROPERTY_COMBO_LIST:
				TempRect.left = TempRect.right - 17;
				m_pComboButton->MoveWindow(TempRect);
				m_pComboButton->ShowWindow(SW_SHOWNORMAL);

				TempRect.left = m_CurRect.left + 2;
				TempRect.right -= 17;
				if(m_pCurItem->bComboEditable)
				{
					m_pEditWnd->SetWindowText(m_pCurItem->csProperties.GetAt(m_pCurItem->nPropertySelected));
//					m_pEditWnd->SetFocus();
//					m_pEditWnd->SetSel(0,-1);
					m_pEditWnd->MoveWindow(TempRect);
					m_pEditWnd->ShowWindow(SW_SHOWNORMAL);
				}

				// Move the Lsit box
//				TempRect.left--;
				TempRect.right += 18;
				TempRect.top = TempRect.bottom;

				// Set the Bottom Height
				if(m_pCurItem->csProperties.GetSize() > 5)
					TempRect.bottom += GetItemHeight(0) * 5;
				else
					TempRect.bottom += GetItemHeight(0) * (int)m_pCurItem->csProperties.GetSize();
				
				// pobierz wspó³rzêdne tej kontrolki w stosunku do okna parenta
//				CRect rcThisParent;
//				GetWindowRect(&rcThisParent);
//				GetParent()->ScreenToClient(&rcThisParent);

//				TempRect.OffsetRect(rcThisParent.left+2, rcThisParent.top+2);
				m_pListBox->MoveWindow(TempRect);

				// Force the Expansion
				OnComboBoxClick();
				break;

	}
}
void CPropertyListCtrl::OnEditLostFocus()
{
	// Get the text
	CString csText;
	m_pEditWnd->GetWindowText(csText);

	// Is the current item a text item
	if(m_pCurItem->nType == ID_PROPERTY_TEXT)
	{
		// Did the text change
		if(!m_bChanged)
			return;

		m_pCurItem->csProperties.SetAt(0, csText);
		
		// Send the message that a property has changed
		GetParent()->PostMessage(ID_PROPERTY_CHANGED, GetCurSel(), m_pCurItem->nType);
	}
	else
	{
		// Get the window that has the focus now
		if(GetFocus() == m_pComboButton || !m_pListBox->GetCount())
			return;
		
		// Did the text change
		if(!m_bChanged)
			return;

		// Send the message that a property has changed
		GetParent()->PostMessage(ID_PROPERTY_CHANGED, GetCurSel(), m_pCurItem->nType);

		// Look for this text
		m_bChanged = FALSE;
		if( m_pListBox->FindStringExact(-1,csText) != LB_ERR)
			return;

		// Add it and select it
		m_pCurItem->nPropertySelected = (int)m_pCurItem->csProperties.Add(csText);
	}
}
void CPropertyListCtrl::OnEditChange()
{
	m_bChanged = TRUE;
}
void CPropertyListCtrl::OnFontPropertyClick()
{
	// Show the Dialog
	CFontDialog FontDialog(&m_pCurItem->LogFont);
	if(FontDialog.DoModal() != IDOK)
		return;
	
	// Set the Font data 
	FontDialog.GetCurrentFont(&m_pCurItem->LogFont);

	// Redraw
	RedrawWindow();

	// Send the message that a property has changed
	GetParent()->PostMessage(ID_PROPERTY_CHANGED, GetCurSel(), m_pCurItem->nType);
}
void CPropertyListCtrl::OnPathPropertyClick()
{
	// Look for a ending tag
	CString csExt = "*";
	CString csPath = m_pCurItem->csProperties.GetAt(0);
	int nPos = csPath.ReverseFind('.');
	if(nPos)
		csExt = csPath.Right(csPath.GetLength() - nPos - 1);

	// Show the Dialog
	CFileDialog QuizFileDlg(TRUE, _T("*"), _T("*.") + csExt, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, m_pCurItem->csProperties.GetAt(1) );
	QuizFileDlg.m_ofn.lpstrInitialDir = csPath;
	if(QuizFileDlg.DoModal() != IDOK)
		return;
	
	// Obtain the Path they selected
    m_pCurItem->csProperties.SetAt(0, QuizFileDlg.GetPathName());

	// Redraw
	RedrawWindow();

	// Send the message that a property has changed
	GetParent()->PostMessage(ID_PROPERTY_CHANGED, GetCurSel(), m_pCurItem->nType);
}

void CPropertyListCtrl::OnDirPropertyClick()
{
	CString strPath;
	if (BrowseForFolder(m_pCurItem->csProperties.GetAt(1), &strPath))
	{
		m_pCurItem->csProperties.SetAt(0, strPath);
		RedrawWindow();
	
		// Send the message that a property has changed
		GetParent()->PostMessage(ID_PROPERTY_CHANGED, GetCurSel(), m_pCurItem->nType);
	}
}

void CPropertyListCtrl::OnCustomPropertyClick()
{
	m_pCurItem->pfnCallback(m_pCurItem->lpParam, m_pCurItem->iParam, &m_Items, GetCurSel());
	RedrawWindow();
}

void CPropertyListCtrl::OnComboBoxClick()
{
	// Add the items
	m_pListBox->ResetContent();

	// Loop for all items
	for( int nItem = 0; nItem < m_pCurItem->csProperties.GetSize(); nItem++)
		m_pListBox->AddString(m_pCurItem->csProperties.GetAt(nItem));

	// Select the correct item
	m_pListBox->SetCurSel(m_pCurItem->nPropertySelected);
	m_pListBox->SetTopIndex(m_pCurItem->nPropertySelected);
	
	// Show the List box
	m_pListBox->ShowWindow(SW_NORMAL);	
}
void CPropertyListCtrl::OnSelChange()
{
	// Set the new current item
	m_pCurItem->nPropertySelected = m_pListBox->GetCurSel();

	// Hide the Windows
	m_pListBox->ShowWindow(SW_HIDE);

	if(m_pCurItem->bComboEditable)
		m_pEditWnd->SetWindowText(m_pCurItem->csProperties.GetAt(m_pCurItem->nPropertySelected));
	else
		RedrawWindow();

	// Send the message that a property has changed
	GetParent()->PostMessage(ID_PROPERTY_CHANGED, GetCurSel(), m_pCurItem->nType);
	m_pComboButton->SetFocus();
}
void CPropertyListCtrl::OnListboxLostFocus()
{
	m_pListBox->ShowWindow(SW_HIDE);
}
void CPropertyListCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// is there an item at this point
	BOOL bOutside;
	/*UINT uItem =*/ ItemFromPoint(point, bOutside);

	// Is this outside the client
	if(bOutside)
		HideControls();
	
	CListBox::OnLButtonDown(nFlags, point);
}
void CPropertyListCtrl::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// Hide the Controls
	HideControls();

	CListBox::OnVScroll(nSBCode, nPos, pScrollBar);
}

/////////////////////////////////////////////////////////////////////////////
// GUI User Functions
/////////////////////////////////////////////////////////////////////////////

void CPropertyListCtrl::Init()
{
	// utwórz czcionkê bazowan¹ na czcionce okna
	LOGFONT lf;
	GetFont()->GetLogFont(&lf);

	if(m_pTextFont)		delete m_pTextFont;
	if(m_pSelectedFont) delete m_pSelectedFont;
	
	m_pTextFont=new CFont();
	m_pTextFont->CreateFontIndirect(&lf);

	lf.lfWeight=FW_BOLD;
	m_pSelectedFont=new CFont();
	m_pSelectedFont->CreateFontIndirect(&lf);

	m_bDeleteFont=TRUE;

	// Create the Border Pen
	m_pBorderPen = new CPen(PS_SOLID, 1, m_crBorderColor);
	
	// Create the Selected Background brush
	m_pBkHighlightBrush = new CBrush(m_crHighlightColor);
	m_pBkBrush			= new CBrush(m_crBkColor);

	// Set the row height - read text height
	CClientDC dc(this);
	dc.SelectObject(&m_pSelectedFont);
	TEXTMETRIC tm;
	dc.GetTextMetrics(&tm);

	SetItemHeight(-1, MulDiv(tm.tmHeight+tm.tmExternalLeading, dc.GetDeviceCaps(LOGPIXELSY), tm.tmDigitizedAspectY) );
}

void CPropertyListCtrl::SetFont(CFont* pFont)
{
	// Delete our font and set our font to theirs
	if(m_pTextFont)		delete m_pTextFont;
	if(m_pSelectedFont) delete m_pSelectedFont;
	m_pTextFont = pFont;
	m_bDeleteFont = FALSE;
	
	// Figure out the text size
	LOGFONT lpLogFont;
	m_pTextFont->GetLogFont(&lpLogFont);

	// Set the font and redraw
	CWnd::SetFont(m_pTextFont, FALSE);

	// Create the heading font with the bold attribute
	lpLogFont.lfWeight = FW_BOLD;
	m_pSelectedFont = new CFont();
	m_pSelectedFont->CreateFontIndirect(&lpLogFont);

	// Set the Row height
	CClientDC dc(this);
	dc.SelectObject(&m_pSelectedFont);
	TEXTMETRIC tm;
	dc.GetTextMetrics(&tm);

	SetItemHeight(-1, MulDiv(tm.tmHeight+tm.tmExternalLeading, dc.GetDeviceCaps(LOGPIXELSY), tm.tmDigitizedAspectY) );

	// ** IMPLEMENT LATER ?? **
	// Recalculate the Width Position
}

void CPropertyListCtrl::SetLineStyle(COLORREF crColor, int nStyle)
{
	// Delete the old Pen
	if(m_pBorderPen) delete m_pBorderPen;
	
	// Create the brush
	m_pBorderPen = new CPen(nStyle, 1, crColor);
	m_crBorderColor = crColor;
}
void CPropertyListCtrl::SetBkColor(COLORREF crColor)
{
	// Delete the old brush
	if(m_pBkBrush) delete m_pBkBrush;
	
	// Create the brush
	m_pBkBrush = new CBrush(crColor);
	m_crBkColor = crColor;
}
void CPropertyListCtrl::SetPropertyBkColor(COLORREF crColor)
{
	// Delete the old brush
	if(m_pBkPropertyBrush) delete m_pBkPropertyBrush;
	
	// Create the brush
	m_pBkPropertyBrush = new CBrush(crColor);
	m_crPropertyBkColor = crColor;
}

void CPropertyListCtrl::SetHighlightColor(COLORREF crColor)
{
	// Delete the old brush
	if(m_pBkHighlightBrush) delete m_pBkHighlightBrush;
	
	// Create the brush
	m_pBkHighlightBrush = new CBrush(crColor);
	m_crHighlightColor = crColor;
}

/////////////////////////////////////////////////////////////////////////////
// Add Properties Functions
/////////////////////////////////////////////////////////////////////////////
BOOL CPropertyListCtrl::AddString(CString csText)
{
	// Call our function (assume its a text Item)
	return AddString(csText, ID_PROPERTY_STATIC, _T(""));
}

BOOL CPropertyListCtrl::AddString(UINT nIDString)
{
	return AddString(GetResManager().LoadString(nIDString));
}

BOOL CPropertyListCtrl::AddString(UINT nIDString, int nType, CString csData, void (*pfnCallback)(LPVOID, int, CPtrList*, int), LPVOID lpParam, int iParam, int nPropertySelected, int nAlignment, BOOL bComboEditable)
{
	// Is this a valid Control type
	if(nType > ID_PROPERTY_COMBO_LIST)
		return FALSE;
	
	// load string
	const TCHAR *pszText=GetResManager().LoadString(nIDString);

	// Create a new Structure to hold it
	PROPERTYITEM*  pNewItem = new PROPERTYITEM;
	pNewItem->nType = nType;
	pNewItem->nAlignment = nAlignment;
	pNewItem->pBrush = NULL;
	pNewItem->bComboEditable = bComboEditable;
	pNewItem->pfnCallback=pfnCallback;
	pNewItem->iParam=iParam;
	pNewItem->lpParam=lpParam;

	// Calculate the Width of the string based on the font set
	CDC* pDC = GetDC();
	pDC->SelectObject(m_pSelectedFont);
	CSize Size = pDC->GetTextExtent(pszText);
	if(Size.cx + 10 > m_nWidestItem)
		m_nWidestItem = Size.cx + 10;
	ReleaseDC(pDC);
	pNewItem->nWidth = Size.cx;
	pNewItem->nPropertySelected = nPropertySelected;

	// Set Property
	if(!SetProperty(pNewItem, nType, csData))
	{
		delete pNewItem;
		return FALSE;
	}

	// Is the item set bigger than the number of properties
	if(pNewItem->nPropertySelected > pNewItem->csProperties.GetSize())
	{
		delete pNewItem;
		return FALSE;
	}

	// Add to the list
	m_Items.AddTail(pNewItem);

	// Add the string to the list box
	/*int nPos =*/ CListBox::AddString(pszText);

	// Create the Control if Needed
	CreateControl(nType);
	return TRUE;
}

BOOL CPropertyListCtrl::AddString(CString csText, int nType, CString csData, int nPropertySelected, int nAlignment, BOOL bComboEditable)
{
	// Is this a valid Control type
	if(nType > ID_PROPERTY_COMBO_LIST)
		return FALSE;
		
	// Create a new Structure to hold it
	PROPERTYITEM*  pNewItem = new PROPERTYITEM;
	pNewItem->nType = nType;
	pNewItem->nAlignment = nAlignment;
	pNewItem->pBrush = NULL;
	pNewItem->bComboEditable = bComboEditable;

	// Calculate the Width of the string based on the font set
	CDC* pDC = GetDC();
	pDC->SelectObject(m_pSelectedFont);
	CSize Size = pDC->GetTextExtent(csText);
	if(Size.cx + 10 > m_nWidestItem)
		m_nWidestItem = Size.cx + 10;
	ReleaseDC(pDC);
	pNewItem->nWidth = Size.cx;
	pNewItem->nPropertySelected = nPropertySelected;

	// Set Property
	if(!SetProperty(pNewItem, nType, csData))
	{
		delete pNewItem;
		return FALSE;
	}

	// Is the item set bigger than the number of properties
	if(pNewItem->nPropertySelected > pNewItem->csProperties.GetSize())
	{
		delete pNewItem;
		return FALSE;
	}

	// Add to the list
	m_Items.AddTail(pNewItem);

	// Add the string to the list box
	/*int nPos =*/ CListBox::AddString(csText);

	// Create the Control if Needed
	CreateControl(nType);
	return TRUE;
}

BOOL CPropertyListCtrl::AddString(UINT nIDString, int nType, UINT nIDData, int nPropertySelected, int nAlignment, BOOL bComboEditable)
{
	return AddString(GetResManager().LoadString(nIDString), nType, GetResManager().LoadString(nIDData), nPropertySelected, nAlignment, bComboEditable);
}

BOOL CPropertyListCtrl::AddString(UINT nIDString, int nType, CString csData, int nPropertySelected, int nAlignment, BOOL bComboEditable)
{
	return AddString(GetResManager().LoadString(nIDString), nType, csData, nPropertySelected, nAlignment, bComboEditable);
}

BOOL CPropertyListCtrl::AddString(CString csText, COLORREF crColor, int nAlignment)
{
	// Create a new brush based on this color
	m_pCurBrush = new CBrush(crColor);
	
	// Call the other functions
	return AddString(csText, ID_PROPERTY_COLOR, _T(""), 0, nAlignment);
}

BOOL CPropertyListCtrl::AddString(UINT nIDString, COLORREF crColor, int nAlignment)
{
	return AddString(GetResManager().LoadString(nIDString), crColor, nAlignment);
}

BOOL CPropertyListCtrl::AddString(CString csText, CFont* pFont, int nAlignment)
{
	// Safe the Font
	m_pCurFont = pFont;
	
	// Call the other functions
	return AddString(csText, ID_PROPERTY_FONT, _T(""), 0, nAlignment);
}

BOOL CPropertyListCtrl::AddString(UINT nIDString, CFont* pFont, int nAlignment)
{
	return AddString(GetResManager().LoadString(nIDString), pFont, nAlignment);
}
		
/////////////////////////////////////////////////////////////////////////////
// Helper Functions
/////////////////////////////////////////////////////////////////////////////
void CPropertyListCtrl::DrawItem(CDC* pDC, CRect ItemRect, BOOL bSelected)
{
	if (m_pCurDrawItem->nType != ID_PROPERTY_STATIC)
	{
		/////////////////////////////////////////
		// Paint the Background rectangle (Property Value)
		if(m_pCurDrawItem->nType == ID_PROPERTY_COLOR)
			pDC->SelectObject(m_pCurDrawItem->pBrush);
		else
			pDC->SelectObject(m_pBkBrush);
		pDC->SelectObject(m_pBorderPen);
		
		// Draw the Rectangle
		ItemRect.left = m_nWidestItem - 1;
		ItemRect.top--;
		ItemRect.right++;
		pDC->Rectangle(ItemRect);
		CRect OrginalRect = ItemRect;
		
		/////////////////////////////////////////
		// Draw the Property Text
		pDC->SetBkMode(TRANSPARENT);
		pDC->SelectObject(m_pBkBrush);
		pDC->SelectObject(m_pTextFont);
		pDC->SetTextColor(m_crTextColor);
		DrawPropertyText(pDC, ItemRect);
		
		/////////////////////////////////////////
		// Paint the Background rectangle (Property Name)
		if( bSelected )
			pDC->SelectObject(m_pBkHighlightBrush);
		
		// Draw the Rectangle
		ItemRect.right = m_nWidestItem;
		ItemRect.left = -1;
		pDC->Rectangle(ItemRect);
		
		/////////////////////////////////////////
		// Paint the Property name Text
		// Is this item selected?
		if( bSelected )
		{		
			if(m_bBoldSelection) pDC->SelectObject(m_pSelectedFont);
			pDC->SetTextColor(m_crTextHighlightColor);
			m_pCurItem = m_pCurDrawItem;
			m_CurRect = OrginalRect;
		}
		
		// Draw the Text
		ItemRect.left += 6;
		ItemRect.right -= 5;
		pDC->DrawText( m_csText, m_csText.GetLength(), ItemRect, DT_SINGLELINE|DT_VCENTER|DT_NOPREFIX|m_pCurDrawItem->nAlignment);
	}
	else
	{
		/////////////////////////////////////////
		// Paint the Background rectangle (Property Value)
		pDC->SelectObject(m_pBkHighlightBrush);
		pDC->SelectObject(m_pBorderPen);
		pDC->SelectObject(m_pSelectedFont);
		
		// Draw the Rectangle
		pDC->Rectangle(ItemRect);
		CRect OrginalRect = ItemRect;
		
		/////////////////////////////////////////
		// Draw
		pDC->SetBkMode(TRANSPARENT);
		pDC->SetTextColor(m_crTextHighlightColor);
		if (bSelected)
		{
			m_pCurItem = m_pCurDrawItem;
			m_CurRect = OrginalRect;
		}
		
		// Draw the Text
		pDC->DrawText( m_csText, m_csText.GetLength(), ItemRect, DT_NOPREFIX|DT_SINGLELINE|DT_VCENTER|DT_CENTER);
	}
}

void CPropertyListCtrl::DrawPropertyText(CDC* pDC, CRect ItemRect)
{
	ItemRect.left += 5;
	switch(m_pCurDrawItem->nType)
	{
		case	ID_PROPERTY_BOOL:
		case	ID_PROPERTY_COMBO_LIST:
			{
				CString strText;
				INT_PTR iCount = m_pCurDrawItem->csProperties.GetCount();
				if(m_pCurDrawItem->nPropertySelected >= 0 && m_pCurDrawItem->nPropertySelected < iCount)
					strText = m_pCurDrawItem->csProperties.GetAt(m_pCurDrawItem->nPropertySelected);
				
				pDC->DrawText( strText, strText.GetLength(), ItemRect, DT_SINGLELINE|DT_VCENTER|DT_LEFT|DT_NOPREFIX);
				break;
			}
		
		case	ID_PROPERTY_TEXT:
		case	ID_PROPERTY_PATH:
		case ID_PROPERTY_DIR:
		case ID_PROPERTY_CUSTOM:
		case	ID_PROPERTY_STATIC:
			{
				CString strText;
				INT_PTR iCount = m_pCurDrawItem->csProperties.GetCount();
				if(iCount > 0)
					strText = m_pCurDrawItem->csProperties.GetAt(0);

				pDC->DrawText( strText, strText.GetLength(), ItemRect, DT_SINGLELINE|DT_VCENTER|DT_LEFT|DT_NOPREFIX);
				break;
			}

		case	ID_PROPERTY_FONT:
				if(m_pCurDrawItem->LogFont.lfHeight)
					pDC->DrawText( m_pCurDrawItem->LogFont.lfFaceName, (int)_tcslen(m_pCurDrawItem->LogFont.lfFaceName), ItemRect, DT_SINGLELINE|DT_VCENTER|DT_LEFT|DT_NOPREFIX);
				break;
	}
}
	
void CPropertyListCtrl::CreateControl(int nType)
{
	switch(nType)
	{
		// Edit Window
		case	ID_PROPERTY_TEXT:
				if(!m_pEditWnd)
				{
					m_pEditWnd = new CEdit();
					m_pEditWnd->Create(WS_CHILD|ES_AUTOHSCROLL|ES_LEFT, CRect(0,0,100,100), this, ID_PROPERTY_TEXT);
					m_pEditWnd->SetFont(m_pTextFont);
				}
				break;

		// Font Button
		case	ID_PROPERTY_FONT:
				if(!m_pFontButton)
				{
					m_pFontButton = new CButton();
					m_pFontButton->Create(_T("..."), WS_CHILD|BS_PUSHBUTTON, CRect(0,0,100,100), this, ID_PROPERTY_FONT);
					m_pFontButton->SetFont(m_pTextFont);
				}
				break;

		case	ID_PROPERTY_PATH:
				if(!m_pPathButton)
				{
					m_pPathButton = new CButton();
					m_pPathButton->Create(_T("..."), WS_CHILD|BS_PUSHBUTTON, CRect(0,0,100,100), this, ID_PROPERTY_PATH);
					m_pPathButton->SetFont(m_pTextFont);
				}
				break;

		case ID_PROPERTY_DIR:
				if(!m_pDirButton)
				{
					m_pDirButton = new CButton();
					m_pDirButton->Create(_T("..."), WS_CHILD|BS_PUSHBUTTON, CRect(0,0,100,100), this, ID_PROPERTY_DIR);
					m_pDirButton->SetFont(m_pTextFont);
				}
				break;

		case ID_PROPERTY_CUSTOM:
				if(!m_pCustomButton)
				{
					m_pCustomButton = new CButton();
					m_pCustomButton->Create(_T("..."), WS_CHILD|BS_PUSHBUTTON, CRect(0,0,100,100), this, ID_PROPERTY_CUSTOM);
					m_pCustomButton->SetFont(m_pTextFont);
				}
				break;

		case	ID_PROPERTY_COMBO_LIST:
				if(!m_pEditWnd)
				{
					m_pEditWnd = new CEdit();
					m_pEditWnd->Create(WS_CHILD|ES_AUTOHSCROLL|ES_LEFT, CRect(0,0,100,100), this, ID_PROPERTY_TEXT);
					m_pEditWnd->SetFont(m_pTextFont);
				}
				if(!m_pListBox)
				{
					m_pListBox = new CListBox();
					m_pListBox->Create(WS_CHILD|WS_BORDER|LBS_NOTIFY|WS_VSCROLL|LBS_HASSTRINGS, CRect(0,0,100,100), this, ID_PROPERTY_COMBO_LIST);
					m_pListBox->SetFont(m_pTextFont);

					m_pComboButton = new CComboButton();
					m_pComboButton->Create(CRect(0,0,0,0), this, ID_PROPERTY_COMBO_BTN ); 
				}
				break;
	}
}

BOOL CPropertyListCtrl::SetProperty(PROPERTYITEM* pPropertyItem, int nType, CString csData)
{
	switch(nType)
	{
		case	ID_PROPERTY_BOOL:
		case	ID_PROPERTY_PATH:
		case ID_PROPERTY_DIR:
				ParseProperties(pPropertyItem, csData);

				// Is the item selected more than items in the array?
				if(pPropertyItem->csProperties.GetSize() != 2 )
					return FALSE;
				break;

		case	ID_PROPERTY_TEXT:
		case	ID_PROPERTY_STATIC:
		case ID_PROPERTY_CUSTOM:
				pPropertyItem->csProperties.Add(csData);
				break;

		case	ID_PROPERTY_FONT:
				memset(&pPropertyItem->LogFont, 0, sizeof(pPropertyItem->LogFont));
				if(m_pCurFont)
				{
					m_pCurFont->GetLogFont(&pPropertyItem->LogFont);			
					m_pCurFont = NULL;
				}
				break;

		case	ID_PROPERTY_COLOR:
				pPropertyItem->pBrush = m_pCurBrush;
				break;	

		case	ID_PROPERTY_COMBO_LIST:
				ParseProperties(pPropertyItem, csData);
				break;

	}

	return TRUE;
}

void CPropertyListCtrl::ParseProperties(PROPERTYITEM* pPropertyItem, CString csData)
{
	// Parse the Items
	TCHAR* pText = csData.GetBuffer( csData.GetLength() );
	TCHAR* pWord;	
	TCHAR  Separations[]   = _T("!");

	// Establish string and get the first token:
	pWord = _tcstok( pText, Separations);
	while( pWord != NULL )
	{
		// Add this to the Array
		pPropertyItem->csProperties.Add(pWord);
		
		// Get next token
		pWord = _tcstok( NULL, Separations );
	}
	
	// Release the buffer
	csData.ReleaseBuffer();
}

void CPropertyListCtrl::HideControls()
{
	// Hide the controls
	if(m_pEditWnd)		m_pEditWnd->ShowWindow(SW_HIDE);
	if(m_pFontButton)	m_pFontButton->ShowWindow(SW_HIDE);
	if(m_pPathButton)	m_pPathButton->ShowWindow(SW_HIDE);
	if (m_pDirButton) m_pDirButton->ShowWindow(SW_HIDE);
	if (m_pCustomButton)
		m_pCustomButton->ShowWindow(SW_HIDE);
	if(m_pListBox)		m_pListBox->ShowWindow(SW_HIDE);
	if(m_pComboButton)	m_pComboButton->ShowWindow(SW_HIDE);	
}

/////////////////////////////////////////////////////////////////////////////
// Get Properties Functions
/////////////////////////////////////////////////////////////////////////////
bool CPropertyListCtrl::GetProperty(int nItem, CString* pText)
{
	// is the item to high
	if(nItem + 1 > GetCount())
		return false;

	// Make sure this item is the correct type
	PROPERTYITEM* pItem;
	pItem = (PROPERTYITEM*)m_Items.GetAt(m_Items.FindIndex(nItem));
	if(pItem->nType != ID_PROPERTY_TEXT && pItem->nType != ID_PROPERTY_PATH && pItem->nType != ID_PROPERTY_DIR && pItem->nType != ID_PROPERTY_STATIC && pItem->nType != ID_PROPERTY_CUSTOM)
		return false;

	// Copy the item
	*pText = pItem->csProperties.GetAt(0);
	return true;
}
bool CPropertyListCtrl::GetProperty(int nItem, bool* bValue)
{
	// is the item to high
	if(nItem + 1 > GetCount())
		return false;

	// Make sure this item is the correct type
	PROPERTYITEM* pItem;
	pItem = (PROPERTYITEM*)m_Items.GetAt(m_Items.FindIndex(nItem));
	if(pItem->nType != ID_PROPERTY_BOOL)
		return false;

	// Copy the item
	*bValue = pItem->nPropertySelected != 0;
	return true;
}
bool CPropertyListCtrl::GetProperty(int nItem, COLORREF* crColor)
{
	// is the item to high
	if(nItem + 1 > GetCount())
		return false;

	// Make sure this item is the correct type
	PROPERTYITEM* pItem;
	pItem = (PROPERTYITEM*)m_Items.GetAt(m_Items.FindIndex(nItem));
	if(pItem->nType != ID_PROPERTY_COLOR)
		return false;

	// Copy the item
	LOGBRUSH LogBrush;
	pItem->pBrush->GetLogBrush(&LogBrush);
	*crColor = LogBrush.lbColor;
	return true;
}
bool CPropertyListCtrl::GetProperty(int nItem, LOGFONT* LogFont)
{
	// is the item to high
	if(nItem + 1 > GetCount())
		return false;

	// Make sure this item is the correct type
	PROPERTYITEM* pItem;
	pItem = (PROPERTYITEM*)m_Items.GetAt(m_Items.FindIndex(nItem));
	if(pItem->nType != ID_PROPERTY_FONT)
		return false;

	// Copy the item
	*LogFont = pItem->LogFont;
	return true;
}
bool CPropertyListCtrl::GetProperty(int nItem, CStringArray* pArray, int* /*SelectedItem*/)
{
	// is the item to high
	if(nItem + 1 > GetCount())
		return false;

	// Make sure this item is the correct type
	PROPERTYITEM* pItem;
	pItem = (PROPERTYITEM*)m_Items.GetAt(m_Items.FindIndex(nItem));
	if(pItem->nType != ID_PROPERTY_COMBO_LIST)
		return false;

	// I do NOT want to send them a pointer to my array so I loop through and copy the item to thiers
	for( int nString = 0; nString < pItem->csProperties.GetSize(); nString++)
		pArray->Add(pItem->csProperties.GetAt(nString));
	return true;
}
bool CPropertyListCtrl::GetProperty(int nItem, int* SelectedItem, CString* pText)
{
	// is the item to high
	if(nItem + 1 > GetCount())
		return false;

	// Make sure this item is the correct type
	PROPERTYITEM* pItem;
	pItem = (PROPERTYITEM*)m_Items.GetAt(m_Items.FindIndex(nItem));

	// Copy the item
	*SelectedItem = pItem->nPropertySelected;

	// Do they want the text
	if(pText != NULL && pItem->nType == ID_PROPERTY_COMBO_LIST)
		*pText = pItem->csProperties.GetAt(pItem->nPropertySelected);
	return true;
}
