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
	CLanguageDialog template

	Files: LanguageDialog.h, LanguageDialog.cpp
	Author: Ixen Gerthannes
	Usage:
		Derive your class from CLanguageDialog instead of CDialog, change all
		calls from CDialog to CLanguageDialog, change call to base constructor
		so it can take language as parameter.
	Creating dialog class:
		- derive class from CLanguageDialog
		- change all occurences of CDialog to CLanguageDialog
		- change parameters list of your default constructor so it can take
			language as parameter (WORD wLang)
		- modify call to base class constructor by putting into it declared
			wLang from your constructor
	Displaying dialog box:
		- declare object as your dialog class
		- eventually set public member m_bAutoDelete to true if you're
			creating dialog with operator new, and it should be
			automatically deleted when closed
		- call DoModal/Create/CreateModeless member function for
			modal/modeless/mixed mode dialog
	Members:
		Constructors - as described in CDialog constructors - language
						specifies resource language to load
			CLanguageDialog();
			CLanguageDialog(PCTSTR lpszTemplateName, CWnd* pParent = NULL);
			CLanguageDialog(UINT uiIDTemplate, CWnd* pParent = NULL);
		Functions:
			int DoModal(); - like in CDialog
			BOOL Create(); - creates modeless dialog box; this class
				automatically handles DestroyWindow, and like
			void Cleanup(); - function cleans unused data - use only when 
				window object wasn't created yet (in Create() and earlier)
			WORD GetCurrentLanguage() const; - retrieves current language
				setting for this dialog
		Attributes:
			bool m_bAutoDelete; - specifies whether this dialog should be
				deleted (by 'delete this') when closed.
*************************************************************************/
#pragma once

#include "libictranslate.h"
#include "ResourceManager.h"

BEGIN_ICTRANSLATE_NAMESPACE

#pragma pack(push, 1)
struct DLGTEMPLATEEX
{
	WORD dlgVer;
	WORD signature;
	DWORD helpID;
	DWORD exStyle;
	DWORD style;
	WORD cDlgItems;
	short x;
	short y;
	short cx;
	short cy;
};

struct DLGITEMTEMPLATEEX
{
	DWORD helpID;
	DWORD exStyle;
	DWORD style;
	short x;
	short y;
	short cx;
	short cy;
	WORD id;
	WORD __DUMMY__;
};
#pragma pack(pop)

class CDlgTemplate
{
public:
	CDlgTemplate() { m_wMenu=(WORD)-1; m_pszMenu=NULL; m_wClass=(WORD)-1; m_pszClass=NULL, m_wTitle=(WORD)-1; m_pszTitle=NULL; m_wFontSize=0; m_wWeight=0; m_byItalic=0; m_byCharset=0; m_pszFace=NULL; };
	CDlgTemplate(const DLGTEMPLATE* pDlgTemplate);
	CDlgTemplate(const DLGTEMPLATEEX* pDlgTemplate);
	~CDlgTemplate();

	bool Open(const DLGTEMPLATE* pDlgTemplate);

protected:
	void ConvertItemToEx(const DLGITEMTEMPLATE* pSrc, DLGITEMTEMPLATEEX* pDst);		// converts DLGITEMTEMPLATE to DLGITEMTEMPLATEEX
	void ConvertDlgToEx(const DLGTEMPLATE* pSrc, DLGTEMPLATEEX* pDst);

	const BYTE* ReadCompoundData(const BYTE* pBuffer, WORD* pwData, PTSTR* ppszStr);

public:
	struct _ITEM
	{
		DLGITEMTEMPLATEEX m_itemTemplate;
		
		WORD m_wClass;
		TCHAR *m_pszClass;

		WORD m_wTitle;
		TCHAR *m_pszTitle;

		WORD m_wCreationDataSize;
		BYTE *m_pbyCreationData;
	};
	vector<_ITEM> m_vItems;

	DLGTEMPLATEEX m_dlgTemplate;

	WORD m_wMenu;
	TCHAR *m_pszMenu;

	WORD m_wClass;
	TCHAR *m_pszClass;

	WORD m_wTitle;		// always -1
	TCHAR *m_pszTitle;		
	
	// font
	WORD m_wFontSize;
	WORD m_wWeight;
	BYTE m_byItalic;
	BYTE m_byCharset;
	TCHAR *m_pszFace;
};

// class stores information about control initial position and offset and scaling factors
class CControlResizeInfo
{
public:
	CControlResizeInfo(int iCtrlID, double dXPosFactor, double dYPosFactor, double dXScaleFactor, double dYScaleFactor);

	void SetInitialPosition(const CRect& rcPos);
	void GetNewControlPlacement(const CRect& rcDlgInitial, const CRect& rcDlgCurrent, CRect& rcNewPlacement);

	void ResetInitState();
	bool IsInitialized() const;

	int GetControlID() const { return m_iControlID; }

protected:
	int m_iControlID;
	CRect m_rcInitialPosition;
	double m_dXOffsetFactor;
	double m_dYOffsetFactor;
	double m_dXScaleFactor;
	double m_dYScaleFactor;
};

/////////////////////////////////////////////////////////////////////////////
// CLanguageDialog dialog
#define LDF_NODIALOGSIZE 0x01
#define LDF_NODIALOGFONT 0x02

class LIBICTRANSLATE_API CLanguageDialog : public CDialog
{
public:
// Construction/destruction
	CLanguageDialog(bool* pLock=NULL);
	CLanguageDialog(PCTSTR lpszTemplateName, CWnd* pParent = NULL, bool* pLock=NULL);   // standard constructor
	CLanguageDialog(UINT uiIDTemplate, CWnd* pParent = NULL, bool* pLock=NULL);   // standard constructor
	
	~CLanguageDialog();

	// static members - initialize global pointer to a resource manager
	static void SetResManager(CResourceManager* prm) { m_prm=prm; };

	// creation
	virtual INT_PTR DoModal();
	virtual BOOL Create();

	void MapRect(RECT* pRect);
	CFont* GetFont() { return m_pFont ? m_pFont : ((CDialog*)this)->GetFont(); };

	BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	void OnContextMenu(CWnd* pWnd, CPoint point);
	void OnHelpButton();

	// Controls resize support
	void InitializeResizableControls();
	void ClearResizableControls();
	void AddResizableControl(int iCtrlID, double dXPosFactor, double dYPosFactor, double dXScaleFactor, double dYScaleFactor);
	void RepositionResizableControls();

protected:
	void UpdateLanguage();
	virtual UINT GetLanguageUpdateOptions() { return 0; };
	virtual void OnLanguageChanged() { };
	void Cleanup();

	virtual BOOL OnTooltipText(UINT /*uiID*/, TOOLTIPTEXT* /*pTip*/) { return FALSE; };
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	virtual void OnOK();
	virtual void PostNcDestroy();
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
private:
	void CalcBaseUnits(PCTSTR pszFacename, WORD wPointSize);

// Attributes
public:
	bool m_bAutoDelete;			// deletes this dialog when exiting
	bool m_bLockInstance;		// allows only one instance of this dialog if set

protected:
	static CResourceManager* m_prm;		// points to the resource manager instance

	bool *m_pbLock;				// dialog box instance lock system
	bool m_bLockChanged;		// if this dialog changed the lock
	PCTSTR m_pszResName;		// resource (string) name of the dialog template
	UINT m_uiResID;				// resource ID if any of the dialog template
	CWnd* m_pParent;			// parent window ptr
	char m_cType;				// type of this dialog box
	CFont* m_pFont;				// currently used font
	int m_iBaseX, m_iBaseY;

	// controls resizing capabilities
	CRect m_rcDialogInitialPosition;
	std::map<int, CControlResizeInfo> m_mapResizeInfo;
};

END_ICTRANSLATE_NAMESPACE
