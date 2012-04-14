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
#ifndef __PROGRESSLISTBOX_H__
#define __PROGRESSLISTBOX_H__

class CTask;

/////////////////////////////////////////////////////////////////////////////
// CProgressListBox window
struct _PROGRESSITEM_
{
	CString m_strText;

	UINT m_uiPos;
	UINT m_uiRange;

	COLORREF m_crColor;

	CTask* m_pTask;
};

class CProgressListBox : public CListBox
{
// Construction
public:
	CProgressListBox();

// Attributes
public:

// Operations
public:
	std::vector<_PROGRESSITEM_*> m_vItems;

protected:
	bool m_bShowCaptions;
	bool m_bSmoothProgress;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProgressListBox)
public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	//}}AFX_VIRTUAL

// Implementation
public:
	void SetSmoothProgress(bool bSmoothProgress);
	int SetCurSel( int nSelect );
	void Init();

	void UpdateItems(int nLimit, bool bUpdateSize);		// updates items in listbox
	void RecalcHeight();	// sets size of a listbox by counting szie of the items

	_PROGRESSITEM_* GetItemAddress(int iIndex);

	void SetShowCaptions(bool bShow=true);
	bool GetShowCaptions();

	virtual ~CProgressListBox();

	// Generated message map functions
protected:
	//{{AFX_MSG(CProgressListBox)
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC*);
	afx_msg void OnKillfocus();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif
