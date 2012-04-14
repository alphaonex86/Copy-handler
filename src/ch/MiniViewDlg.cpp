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
#include "MiniViewDlg.h"
#include "ch.h"
#include <assert.h>
#include "MemDC.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define WM_INITDATA				WM_USER+5

static const int sg_iMargin=7;

#define ROUND(val) ( ( (val)-static_cast<int>(val) > 0.5 ) ? static_cast<int>(val)+1 : static_cast<int>(val) )
#undef ROUNDUP	// from other module
#define ROUNDUP(val, to) ( (static_cast<int>((val)/(to))+1 )*(to) )

bool CMiniViewDlg::m_bLock=false;

/////////////////////////////////////////////////////////////////////////////
// CMiniViewDlg dialog

CMiniViewDlg::CMiniViewDlg(CTaskArray* pArray, bool *pbHide, CWnd* pParent /*=NULL*/)
	:ictranslate::CLanguageDialog(IDD_MINIVIEW_DIALOG, pParent, &m_bLock)
{
	//{{AFX_DATA_INIT(CMiniViewDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_brBackground.CreateSolidBrush(GetSysColor(COLOR_3DFACE));
	m_iLastHeight=0;
	m_bShown=false;
	m_pTasks=pArray;
	m_bActive=false;
	m_iIndex=-1;
	m_pbHide=pbHide;
}


void CMiniViewDlg::DoDataExchange(CDataExchange* pDX)
{
	CLanguageDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMiniViewDlg)
	DDX_Control(pDX, IDC_PROGRESS_LIST, m_ctlStatus);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CMiniViewDlg,ictranslate::CLanguageDialog)
	//{{AFX_MSG_MAP(CMiniViewDlg)
	ON_WM_CTLCOLOR()
	ON_WM_TIMER()
	ON_LBN_SELCHANGE(IDC_PROGRESS_LIST, OnSelchangeProgressList)
	ON_WM_NCLBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_NCPAINT()
	ON_WM_NCACTIVATE()
	ON_LBN_SETFOCUS(IDC_PROGRESS_LIST, OnSetfocusProgressList)
	ON_LBN_SELCANCEL(IDC_PROGRESS_LIST, OnSelcancelProgressList)
	ON_WM_MOUSEMOVE()
	ON_WM_SETTINGCHANGE()
	ON_LBN_DBLCLK(IDC_PROGRESS_LIST, OnDblclkProgressList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMiniViewDlg message handlers

HBRUSH CMiniViewDlg::OnCtlColor(CDC*, CWnd*, UINT) 
{
	return m_brBackground;
}

BOOL CMiniViewDlg::OnInitDialog() 
{
	CLanguageDialog::OnInitDialog();

	// fill the buttons' structure
	m_bdButtons[0].pfnCallbackFunc=&OnPause;
	m_bdButtons[0].iPosition=4;
	m_bdButtons[0].bPressed=false;
	m_bdButtons[0].bEnabled=false;
	m_bdButtons[1].pfnCallbackFunc=&OnResume;
	m_bdButtons[1].iPosition=3;
	m_bdButtons[1].bPressed=false;
	m_bdButtons[1].bEnabled=false;
	m_bdButtons[2].pfnCallbackFunc=&OnCancelBtn;
	m_bdButtons[2].iPosition=2;
	m_bdButtons[2].bPressed=false;
	m_bdButtons[2].bEnabled=false;
	m_bdButtons[3].pfnCallbackFunc=&OnRestartBtn;
	m_bdButtons[3].iPosition=1;
	m_bdButtons[3].bPressed=false;
	m_bdButtons[3].bEnabled=false;
	m_bdButtons[4].pfnCallbackFunc=&OnCloseBtn;
	m_bdButtons[4].iPosition=0;
	m_bdButtons[4].bPressed=false;
	m_bdButtons[4].bEnabled=true;

	ResizeDialog();
	PostMessage(WM_INITDATA);

	return TRUE;
}

void CMiniViewDlg::OnTimer(UINT_PTR nIDEvent) 
{
	if (nIDEvent == 9843)
	{
		KillTimer(9843);

		RefreshStatus();

		SetTimer(9843, (UINT)GetConfig().get_signed_num(PP_MVREFRESHINTERVAL), NULL);
	}

	CLanguageDialog::OnTimer(nIDEvent);
}

void CMiniViewDlg::RecalcSize(int nHeight, bool bInitial)
{
	// set listbox size
	CRect rcList;
	m_ctlStatus.GetClientRect(&rcList);

	if (nHeight == 0)
		nHeight=rcList.Height();
	
	// don't do anything if height doesn't changed
	if (nHeight == m_iLastHeight && !bInitial)
		return;

	// remember height
	m_iLastHeight = nHeight;

	// size of a dialog and screen
	CRect rCLanguageDialog, rcScreen;
	GetWindowRect(&rCLanguageDialog);
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rcScreen, 0);

	// place listbox in the best place
	m_ctlStatus.SetWindowPos(NULL, sg_iMargin, 0/*sg_iMargin*/, 0, 0, SWP_NOZORDER | SWP_NOSIZE);

	int iWidth=rcList.Width()+2*sg_iMargin+2*GetSystemMetrics(SM_CXDLGFRAME);
	int iHeight=rcList.Height()+1*sg_iMargin+2*GetSystemMetrics(SM_CYDLGFRAME)+GetSystemMetrics(SM_CYSMCAPTION);

	if (bInitial || (rCLanguageDialog.left == rcScreen.right-rCLanguageDialog.Width()
						&& rCLanguageDialog.top == rcScreen.bottom-rCLanguageDialog.Height()) )
	{
		SetWindowPos(&wndTopMost, rcScreen.right-iWidth, rcScreen.bottom-iHeight, iWidth, iHeight,
			0);
	}
	else
		SetWindowPos(&wndTopMost, 0, 0, iWidth, iHeight, SWP_NOMOVE);
}

void CMiniViewDlg::RefreshStatus()
{
	int index=0;
	_PROGRESSITEM_* pItem=NULL;

	if (GetConfig().get_bool(PP_MVSHOWSINGLETASKS))
	{
		for (int i=0;i<m_pTasks->GetSize();i++)
		{
			CTask* pTask=m_pTasks->GetAt(i);
			pTask->GetMiniSnapshot(&dd);

			if ((dd.m_uiStatus & ST_STEP_MASK) != ST_FINISHED && (dd.m_uiStatus & ST_STEP_MASK) != ST_CANCELLED)
			{
				pItem=m_ctlStatus.GetItemAddress(index++);

				// load
				if ((dd.m_uiStatus & ST_WORKING_MASK) == ST_ERROR)
					pItem->m_crColor=RGB(255, 0, 0);
				else if ((dd.m_uiStatus & ST_WORKING_MASK) == ST_PAUSED)
					pItem->m_crColor=RGB(255, 255, 0);
				else if ((dd.m_uiStatus & ST_WAITING_MASK) == ST_WAITING)
					pItem->m_crColor=RGB(50, 50, 50);
				else
					pItem->m_crColor=RGB(0, 255, 0);
				
				pItem->m_strText=dd.m_fi.GetFileName();
				pItem->m_uiPos=dd.m_nPercent;
				pItem->m_pTask=pTask;
			}
		}
	}

	// should we show ?
	bool bInitial=false;
	if (index == 0)
	{
		if (m_bShown)
		{
			if (GetConfig().get_bool(PP_MVAUTOHIDEWHENEMPTY) || *m_pbHide)
				HideWindow();
		}
		else if (!GetConfig().get_bool(PP_MVAUTOHIDEWHENEMPTY) && !(*m_pbHide))
		{
			// need to be visible
			ShowWindow();
			bInitial=true;
		}
	}
	else
	{
		if (!m_bShown)
		{
			if (!(*m_pbHide))
			{
				ShowWindow();
				bInitial=true;
			}
		}
		else
		{
			if (*m_pbHide)
				HideWindow();
		}
	}

	// add all state
	pItem=m_ctlStatus.GetItemAddress(index++);
	pItem->m_crColor=GetSysColor(COLOR_HIGHLIGHT);
	pItem->m_strText=GetResManager().LoadString(IDS_MINIVIEWALL_STRING);
	pItem->m_uiPos=m_pTasks->GetPercent();
	pItem->m_pTask=NULL;

	// get rid of the rest
	m_ctlStatus.SetSmoothProgress(GetConfig().get_bool(PP_MVUSESMOOTHPROGRESS));
	m_ctlStatus.UpdateItems(index, true);
	
	m_ctlStatus.SetShowCaptions(GetConfig().get_bool(PP_MVSHOWFILENAMES));

	// calc size
	RecalcSize(0, bInitial);
}

LRESULT CMiniViewDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	if (message == WM_INITDATA)
	{
		// listbox with progress pseudocontrols
		m_ctlStatus.Init();
		
		// refresh
		RefreshStatus();
		
		// set refresh timer
		SetTimer(9843, (UINT)GetConfig().get_signed_num(PP_MVREFRESHINTERVAL), NULL);

		return static_cast<LRESULT>(0);
	}

	return ictranslate::CLanguageDialog::WindowProc(message, wParam, lParam);
}

void CMiniViewDlg::OnNcPaint() 
{
	int iCXBorder=GetSystemMetrics(SM_CXBORDER);
	int iCYBorder=GetSystemMetrics(SM_CYBORDER);
	int iWidth=GetSystemMetrics(SM_CXSMSIZE);
	int iHeight=GetSystemMetrics(SM_CYSMSIZE);
	int iFrameHeight=GetSystemMetrics(SM_CYDLGFRAME);
	int iFrameWidth=GetSystemMetrics(SM_CXDLGFRAME);
	bool bEnabled=(m_ctlStatus.GetCurSel() != LB_ERR);

	// NC coordinates
	CRect rcWindow;
	GetWindowRect(&rcWindow);
	rcWindow.OffsetRect(-rcWindow.left, -rcWindow.top);

	// device context
	CWindowDC ncdc(this);

	// frame
	ncdc.DrawEdge(&rcWindow, EDGE_RAISED, BF_RECT);
	rcWindow.DeflateRect(iFrameWidth-iCXBorder, iFrameHeight-iCYBorder, iFrameWidth-iCXBorder, iFrameHeight-iCYBorder);

	CPen pen, pen2;
	pen.CreatePen(PS_SOLID, iCXBorder, GetSysColor(COLOR_3DFACE));
	pen2.CreatePen(PS_SOLID, 1, GetSysColor(COLOR_3DFACE));

	ncdc.SelectObject(&pen);
	ncdc.SelectStockObject(NULL_BRUSH);
	
	ncdc.Rectangle(&rcWindow);

	// caption bar
	rcWindow.DeflateRect(iCXBorder, iCXBorder, iCXBorder, 0);
	rcWindow.bottom=rcWindow.top+iHeight;	// caption pos

	// additional horz bar
	ncdc.SelectObject(&pen2);
	ncdc.MoveTo(rcWindow.left, rcWindow.bottom);
	ncdc.LineTo(rcWindow.right, rcWindow.bottom);

	// memdc
	CMemDC dc(&ncdc, &rcWindow);

	COLORREF crLeft=GetSysColor(m_bActive ? COLOR_ACTIVECAPTION : COLOR_INACTIVECAPTION);
	dc.FillSolidRect(&rcWindow, crLeft);

	// caption text
	CString strWindow;
	GetWindowText(strWindow);
//	TRACE("DRAWING TEXT: "+strWindow+"\n");

	rcWindow.DeflateRect(5, 0, BTN_COUNT*iWidth+iFrameWidth+5, 0);
	
	// caption font
	NONCLIENTMETRICS ncm;
	memset(&ncm, 0, sizeof(NONCLIENTMETRICS));
	ncm.cbSize=sizeof(NONCLIENTMETRICS);
	SystemParametersInfo(SPI_GETNONCLIENTMETRICS, ncm.cbSize, &ncm, 0);
	
	CFont font;
	font.CreateFontIndirect(&ncm.lfSmCaptionFont);
	dc.SelectObject(&font);
	
	dc.SetTextColor(GetSysColor(COLOR_CAPTIONTEXT));
	dc.SetBkMode(TRANSPARENT);
	dc.DrawText(strWindow, &rcWindow, DT_END_ELLIPSIS | DT_VCENTER | DT_LEFT | DT_NOCLIP | DT_SINGLELINE);
	// button drawing
	GetClientRect(&rcWindow);
	
	for (int i=0;i<BTN_COUNT;i++)
	{
		if (m_bdButtons[i].iPosition == 0)
		{
			m_bdButtons[i].rcButton.left=rcWindow.right-iWidth+2;
			m_bdButtons[i].bEnabled=true;
		}
		else
		{
			m_bdButtons[i].rcButton.left=rcWindow.right-(m_bdButtons[i].iPosition+1)*iWidth-iFrameWidth;
			m_bdButtons[i].bEnabled=bEnabled;
		}

		m_bdButtons[i].rcButton.top=iFrameHeight;
		m_bdButtons[i].rcButton.right=m_bdButtons[i].rcButton.left+iWidth;
		m_bdButtons[i].rcButton.bottom=m_bdButtons[i].rcButton.top+iHeight;
		
		m_bdButtons[i].pfnCallbackFunc(this, MSG_DRAWBUTTON, &m_bdButtons[i], &dc);
	}
}

void OnPause(CMiniViewDlg* pDlg, UINT uiMsg, CMiniViewDlg::_BTNDATA_* pData, CDC* pDC)
{
	switch (uiMsg)
	{
	case MSG_DRAWBUTTON:
		{
			CRect rcCopy=pData->rcButton;
			rcCopy.DeflateRect(2,2,2,2);
			
			// frame drawing
			if (!pData->bPressed || pDlg->m_ctlStatus.GetCurSel() == LB_ERR)
				pDC->Draw3dRect(&rcCopy, GetSysColor(COLOR_BTNHIGHLIGHT), GetSysColor(COLOR_BTNSHADOW));
			else
				pDC->Draw3dRect(&rcCopy, GetSysColor(COLOR_BTNSHADOW), GetSysColor(COLOR_BTNHIGHLIGHT));
			
			// fill the background
			rcCopy.DeflateRect(1, 1, 1, 1);
			pDC->FillSolidRect(&rcCopy, GetSysColor(COLOR_3DFACE));
			
			// pause
			CPen pen;
			int iPenWidth=rcCopy.Width()/10+1;
			pen.CreatePen(PS_SOLID, iPenWidth, pData->bEnabled ? GetSysColor(COLOR_BTNTEXT) : GetSysColor(COLOR_BTNSHADOW));
			CPen* pOld=pDC->SelectObject(&pen);

			int iOffset=rcCopy.Width()/3;
			pDC->MoveTo(rcCopy.left+iOffset-ROUND(0.66*iPenWidth)+pData->bPressed, rcCopy.top+1*iPenWidth+pData->bPressed);
			pDC->LineTo(rcCopy.left+iOffset-ROUND(0.66*iPenWidth)+pData->bPressed, rcCopy.bottom-ROUND(1.5*iPenWidth)+pData->bPressed);
			pDC->MoveTo(rcCopy.right-iOffset-ROUND(0.66*iPenWidth)+pData->bPressed, rcCopy.top+1*iPenWidth+pData->bPressed);
			pDC->LineTo(rcCopy.right-iOffset-ROUND(0.66*iPenWidth)+pData->bPressed, rcCopy.bottom-ROUND(1.5*iPenWidth)+pData->bPressed);

			pDC->SelectObject(pOld);
			break;
		}
	case MSG_ONCLICK:
		{
			int iSel=pDlg->m_ctlStatus.GetCurSel();
			if (iSel == LB_ERR || (size_t)iSel >= pDlg->m_ctlStatus.m_vItems.size())
				return;

			CTask* pTask;
			if ( (pTask=pDlg->m_ctlStatus.m_vItems.at(iSel)->m_pTask) != NULL)
				pTask->PauseProcessing();
			else
				pDlg->m_pTasks->TasksPauseProcessing();

			break;
		}
	}
}

void OnCloseBtn(CMiniViewDlg* pDlg, UINT uiMsg, CMiniViewDlg::_BTNDATA_* pData, CDC* pDC)
{
	switch (uiMsg)
	{
	case MSG_DRAWBUTTON:
		{
			CRect rcCopy=pData->rcButton;
			rcCopy.DeflateRect(2,2,2,2);
			
			// frame
			if (!pData->bPressed)
				pDC->Draw3dRect(&rcCopy, GetSysColor(COLOR_BTNHIGHLIGHT), GetSysColor(COLOR_BTNSHADOW));
			else
				pDC->Draw3dRect(&rcCopy, GetSysColor(COLOR_BTNSHADOW), GetSysColor(COLOR_BTNHIGHLIGHT));

			// background
			rcCopy.DeflateRect(1, 1, 1, 1);
			pDC->FillSolidRect(&rcCopy, GetSysColor(COLOR_3DFACE));
			
			// close
			CPen pen;
			int iPenSize=rcCopy.Width()/10+1;
			pen.CreatePen(PS_SOLID | PS_INSIDEFRAME, iPenSize, GetSysColor(COLOR_BTNTEXT));
			CPen* pOld=pDC->SelectObject(&pen);

			switch (iPenSize)
			{
			case 1:
				pDC->MoveTo(rcCopy.left+pData->bPressed+ROUND(1.4*iPenSize), rcCopy.top+pData->bPressed+ROUND(1.4*iPenSize));
				pDC->LineTo(rcCopy.right+pData->bPressed-ROUND(1.4*iPenSize), rcCopy.bottom+pData->bPressed-ROUND(1.6*iPenSize));
				pDC->MoveTo(rcCopy.left+pData->bPressed+ROUND(1.4*iPenSize), rcCopy.bottom+pData->bPressed-ROUND(2.6*iPenSize));
				pDC->LineTo(rcCopy.right+pData->bPressed-ROUND(1.4*iPenSize), rcCopy.top+pData->bPressed+ROUND(0.4*iPenSize));
				break;
			default:
				pDC->MoveTo(rcCopy.left+pData->bPressed+ROUND(1.4*iPenSize), rcCopy.top+pData->bPressed+ROUND(1.4*iPenSize));
				pDC->LineTo(rcCopy.right+pData->bPressed-ROUND(2.0*iPenSize), rcCopy.bottom+pData->bPressed-ROUND(2.0*iPenSize));
				pDC->MoveTo(rcCopy.left+pData->bPressed+ROUND(1.4*iPenSize), rcCopy.bottom+pData->bPressed-ROUND(2.0*iPenSize));
				pDC->LineTo(rcCopy.right+pData->bPressed-ROUND(2.0*iPenSize), rcCopy.top+pData->bPressed+ROUND(1.4*iPenSize));
				break;
			}

			pDC->SelectObject(pOld);
			break;
		}
	case MSG_ONCLICK:
		pDlg->SendMessage(WM_CLOSE, 0, 0);
		break;
	}
}

void OnResume(CMiniViewDlg* pDlg, UINT uiMsg, CMiniViewDlg::_BTNDATA_* pData, CDC* pDC)
{
	switch (uiMsg)
	{
	case MSG_DRAWBUTTON:
		{
			CRect rcCopy=pData->rcButton;
			rcCopy.DeflateRect(2,2,2,2);
			
			// frame
			if (!pData->bPressed || pDlg->m_ctlStatus.GetCurSel() == LB_ERR)
				pDC->Draw3dRect(&rcCopy, GetSysColor(COLOR_BTNHIGHLIGHT), GetSysColor(COLOR_BTNSHADOW));
			else
				pDC->Draw3dRect(&rcCopy, GetSysColor(COLOR_BTNSHADOW), GetSysColor(COLOR_BTNHIGHLIGHT));
			
			// bkgnd
			rcCopy.DeflateRect(1, 1, 1, 1);
			pDC->FillSolidRect(&rcCopy, GetSysColor(COLOR_3DFACE));

			// triangle
			int iOffset=rcCopy.Width()/4;
			int iHeight=rcCopy.Width()/10+1;
			POINT pt[3]={ {rcCopy.left+iOffset-1+pData->bPressed, rcCopy.top+1*iHeight+pData->bPressed}, {rcCopy.left+iOffset-1+pData->bPressed, rcCopy.bottom-ROUND(1.625*iHeight)+pData->bPressed},
							{rcCopy.right-iOffset-1+pData->bPressed, rcCopy.top+1*iHeight+pData->bPressed+(rcCopy.Height()-3*iHeight+1)/2} };

			CBrush brs;
			brs.CreateSolidBrush(pData->bEnabled ? GetSysColor(COLOR_BTNTEXT) : GetSysColor(COLOR_BTNSHADOW));
			CBrush* pOld=pDC->SelectObject(&brs);

			CPen pen;
			pen.CreatePen(PS_SOLID, 1, pData->bEnabled ? GetSysColor(COLOR_BTNTEXT) : GetSysColor(COLOR_BTNSHADOW));
			CPen *pOldPen=pDC->SelectObject(&pen);
			pDC->SetPolyFillMode(WINDING);

			pDC->Polygon(pt, 3);

			pDC->SelectObject(pOld);
			pDC->SelectObject(pOldPen);

			break;
		}
	case MSG_ONCLICK:
		{
			int iSel=pDlg->m_ctlStatus.GetCurSel();
			if (iSel == LB_ERR || (size_t)iSel >= pDlg->m_ctlStatus.m_vItems.size())
				return;
			CTask* pTask;
			if ( (pTask=pDlg->m_ctlStatus.m_vItems.at(iSel)->m_pTask) != NULL)
			{
				if (pTask->GetStatus(ST_WAITING_MASK) & ST_WAITING)
					pTask->SetForceFlag(true);
				else
					pTask->ResumeProcessing();
			}
			else
				pDlg->m_pTasks->TasksResumeProcessing();
			break;
		}
	}
}

void OnCancelBtn(CMiniViewDlg* pDlg, UINT uiMsg, CMiniViewDlg::_BTNDATA_* pData, CDC* pDC)
{
	switch (uiMsg)
	{
	case MSG_DRAWBUTTON:
		{
			CRect rcCopy=pData->rcButton;
			rcCopy.DeflateRect(2,2,2,2);
			
			// frame
			if (!pData->bPressed || pDlg->m_ctlStatus.GetCurSel() == LB_ERR)
				pDC->Draw3dRect(&rcCopy, GetSysColor(COLOR_BTNHIGHLIGHT), GetSysColor(COLOR_BTNSHADOW));
			else
				pDC->Draw3dRect(&rcCopy, GetSysColor(COLOR_BTNSHADOW), GetSysColor(COLOR_BTNHIGHLIGHT));
			
			// bkgnd
			rcCopy.DeflateRect(1, 1, 1, 1);
			pDC->FillSolidRect(&rcCopy, GetSysColor(COLOR_3DFACE));

			// square
			int iWidth=rcCopy.Width()/10+1;
			rcCopy.DeflateRect(1*iWidth+pData->bPressed, 1*iWidth+pData->bPressed, ROUND(1.6*iWidth)-pData->bPressed, 1*iWidth-pData->bPressed);
			pDC->FillSolidRect(&rcCopy, pData->bEnabled ? GetSysColor(COLOR_BTNTEXT) : GetSysColor(COLOR_BTNSHADOW));
			break;
		}
	case MSG_ONCLICK:
		int iSel=pDlg->m_ctlStatus.GetCurSel();
		if (iSel == LB_ERR || (size_t)iSel >= pDlg->m_ctlStatus.m_vItems.size())
			return;
		CTask* pTask;
		if ( (pTask=pDlg->m_ctlStatus.m_vItems.at(iSel)->m_pTask) != NULL)
			pTask->CancelProcessing();
		else
			pDlg->m_pTasks->TasksCancelProcessing();
		break;
	}
}

void OnRestartBtn(CMiniViewDlg* pDlg, UINT uiMsg, CMiniViewDlg::_BTNDATA_* pData, CDC* pDC)
{
	switch (uiMsg)
	{
	case MSG_DRAWBUTTON:
		{
			CRect rcCopy=pData->rcButton;
			rcCopy.DeflateRect(2,2,2,2);
			
			// frame
			if (!pData->bPressed || pDlg->m_ctlStatus.GetCurSel() == LB_ERR)
				pDC->Draw3dRect(&rcCopy, GetSysColor(COLOR_BTNHIGHLIGHT), GetSysColor(COLOR_BTNSHADOW));
			else
				pDC->Draw3dRect(&rcCopy, GetSysColor(COLOR_BTNSHADOW), GetSysColor(COLOR_BTNHIGHLIGHT));
			
			// bkgnd
			rcCopy.DeflateRect(1, 1, 1, 1);
			pDC->FillSolidRect(&rcCopy, GetSysColor(COLOR_3DFACE));

			// triangle in a square
			int iOffset=rcCopy.Width()/4;
			int iHeight=rcCopy.Width()/10+1;
			POINT pt[3]={ {rcCopy.left+iOffset-1+pData->bPressed, rcCopy.top+1*iHeight+pData->bPressed}, {rcCopy.left+iOffset-1+pData->bPressed, rcCopy.bottom-ROUND(1.625*iHeight)+pData->bPressed},
							{rcCopy.right-iOffset-1+pData->bPressed, rcCopy.top+1*iHeight+pData->bPressed+(rcCopy.Height()-3*iHeight+1)/2} };

			CBrush brs;
			brs.CreateSolidBrush(pData->bEnabled ? RGB(255, 0, 0) : GetSysColor(COLOR_BTNSHADOW));
			CBrush* pOld=pDC->SelectObject(&brs);

			CPen pen;
			pen.CreatePen(PS_SOLID, 1, pData->bEnabled ? RGB(255, 0, 0) : GetSysColor(COLOR_BTNSHADOW));
			CPen *pOldPen=pDC->SelectObject(&pen);

			pDC->SetPolyFillMode(WINDING);
			pDC->Polygon(pt, 3);
			pDC->SelectObject(pOld);
			pDC->SelectObject(pOldPen);

			break;
		}
	case MSG_ONCLICK:
		{
			int iSel=pDlg->m_ctlStatus.GetCurSel();
			if (iSel == LB_ERR || (size_t)iSel >= pDlg->m_ctlStatus.m_vItems.size())
				return;
			CTask* pTask;
			if ( (pTask=pDlg->m_ctlStatus.m_vItems.at(iSel)->m_pTask) != NULL)
				pTask->RestartProcessing();
			else
				pDlg->m_pTasks->TasksRestartProcessing();
			break;
		}
	}
}

void CMiniViewDlg::OnSelchangeProgressList() 
{
	RefreshStatus();
	RedrawWindow(NULL, NULL, RDW_INVALIDATE | RDW_FRAME);
//	PostMessage(WM_NCPAINT);
}

void CMiniViewDlg::OnNcLButtonDown(UINT nHitTest, CPoint point) 
{
	bool bEnabled=false;
	CRect rcBtn;
	int iNCHeight=GetSystemMetrics(SM_CYSMCAPTION)+GetSystemMetrics(SM_CYDLGFRAME);
		
	// has been button pressed ?
	for (int i=0;i<BTN_COUNT;i++)
	{
		// translate coordinates
		rcBtn=m_bdButtons[i].rcButton;
		ClientToScreen(rcBtn);
		rcBtn.top-=iNCHeight;
		rcBtn.bottom-=iNCHeight;
			
		// check
		if (rcBtn.PtInRect(point))
		{
			bEnabled=true;
			if (m_bdButtons[i].bEnabled)
			{
				m_iIndex=i;
				m_bdButtons[i].bPressed=true;
				SetCapture();
				RedrawWindow(NULL, NULL, RDW_INVALIDATE | RDW_FRAME);
//				PostMessage(WM_NCPAINT, NULL, NULL);
				return;
			}
		}
	}

	if (!bEnabled)
		CLanguageDialog::OnNcLButtonDown(nHitTest, point);
}

void CMiniViewDlg::OnLButtonUp(UINT nFlags, CPoint point) 
{
	ReleaseCapture();
	bool bProcessed=false;

	if (m_iIndex != -1 && m_bdButtons[m_iIndex].bPressed)
	{
		m_bdButtons[m_iIndex].bPressed=false;
		RedrawWindow(NULL, NULL, RDW_INVALIDATE | RDW_FRAME);
//		PostMessage(WM_NCPAINT, NULL, NULL);
		m_bdButtons[m_iIndex].pfnCallbackFunc(this, MSG_ONCLICK, &m_bdButtons[m_iIndex], NULL);
		bProcessed=true;
	}

	if (!bProcessed)
		m_ctlStatus.SetCurSel(-1);

	m_iIndex=-1;

	CLanguageDialog::OnLButtonUp(nFlags, point);
}

BOOL CMiniViewDlg::OnNcActivate(BOOL bActive) 
{
	m_bActive=bActive != 0;
	RedrawWindow(NULL, NULL, RDW_INVALIDATE | RDW_FRAME);
//	PostMessage(WM_NCPAINT);	
	return TRUE/*bResult*/;
}

void CMiniViewDlg::OnSetfocusProgressList() 
{
	RedrawWindow(NULL, NULL, RDW_INVALIDATE | RDW_FRAME);
//	PostMessage(WM_NCPAINT);	
}

void CMiniViewDlg::OnSelcancelProgressList() 
{
	RefreshStatus();
	RedrawWindow(NULL, NULL, RDW_INVALIDATE | RDW_FRAME);
//	PostMessage(WM_NCPAINT);	
}

void CMiniViewDlg::OnMouseMove(UINT nFlags, CPoint point) 
{
//	int iNCHeight=GetSystemMetrics(SM_CYSMCAPTION)+GetSystemMetrics(SM_CYDLGFRAME);

	if (m_iIndex != -1)
	{
		// popraw wspó³rzêdne punktu
		point.x+=GetSystemMetrics(SM_CYDLGFRAME);
		point.y+=GetSystemMetrics(SM_CYSMCAPTION)+GetSystemMetrics(SM_CYDLGFRAME);
		if (m_bdButtons[m_iIndex].rcButton.PtInRect(point))
		{
			if (!m_bdButtons[m_iIndex].bPressed)
			{
				m_bdButtons[m_iIndex].bPressed=true;
				SendMessage(WM_NCPAINT, NULL, NULL);
			}
		}
		else
		{
			if (m_bdButtons[m_iIndex].bPressed)
			{
				m_bdButtons[m_iIndex].bPressed=false;
				SendMessage(WM_NCPAINT, NULL, NULL);
			}
		}
	}
	
	CLanguageDialog::OnMouseMove(nFlags, point);
}

void CMiniViewDlg::OnSettingChange(UINT uFlags, LPCTSTR lpszSection) 
{
	CLanguageDialog::OnSettingChange(uFlags, lpszSection);

	if (uFlags == SPI_SETNONCLIENTMETRICS)
		ResizeDialog();
}

void CMiniViewDlg::ResizeDialog()
{
	if(!IsWindowVisible())
		return;

	// remember pos of listbox
	CRect rcList, rcWindow, rcClient;
	m_ctlStatus.GetWindowRect(&rcList);
	ScreenToClient(&rcList);
	GetWindowRect(&rcWindow);
	GetClientRect(&rcClient);

	// change window size
	CString strTitle;
	GetWindowText(strTitle);
	
	CClientDC dc(this);

	NONCLIENTMETRICS ncm;
	memset(&ncm, 0, sizeof(NONCLIENTMETRICS));
	ncm.cbSize=sizeof(NONCLIENTMETRICS);
	if(!SystemParametersInfo(SPI_GETNONCLIENTMETRICS, ncm.cbSize, &ncm, 0))
	{
		BOOST_ASSERT(false);		// function call failed
		return;
	}
	
	CFont font;
	font.CreateFontIndirect(&ncm.lfSmCaptionFont);
	dc.SelectObject(&font);

	CSize sSize=dc.GetOutputTextExtent(strTitle);

	int iEdgeWidth=1;
	int iBoxWidth=static_cast<int>(static_cast<double>(((9+2)-2*iEdgeWidth))*(2.0/3.0))+1;
	int iWidth=BTN_COUNT*(GetSystemMetrics(SM_CYSMCAPTION))+sSize.cx+2*GetSystemMetrics(SM_CXDLGFRAME)+18;
	
	// change pos of listbox
	m_ctlStatus.SetWindowPos(NULL, 0, 0, ROUNDUP(iWidth-2*sg_iMargin, iBoxWidth)+2*iEdgeWidth, rcList.Height(), SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOMOVE);

	RecalcSize(0, true);
}

void CMiniViewDlg::HideWindow()
{
	static_cast<CLanguageDialog*>(this)->ShowWindow(SW_HIDE);
	m_bShown=false;
}

void CMiniViewDlg::ShowWindow()
{
	static_cast<CLanguageDialog*>(this)->ShowWindow(SW_SHOW);
	m_bShown=true;
}

void CMiniViewDlg::OnDblclkProgressList() 
{
	int iSel=m_ctlStatus.GetCurSel();
	if (iSel == LB_ERR || (size_t)iSel >= m_ctlStatus.m_vItems.size())
		return;
	CTask* pTask;
	pTask=m_ctlStatus.m_vItems.at(iSel)->m_pTask;

	GetParent()->PostMessage(WM_MINIVIEWDBLCLK, 0, (LPARAM)pTask);
}

void CMiniViewDlg::OnLanguageChanged()
{
	ResizeDialog();
}
