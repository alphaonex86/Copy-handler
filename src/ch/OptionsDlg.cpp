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
#include "resource.h"
#include "OptionsDlg.h"
#include "BufferSizeDlg.h"
#include "ShortcutsDlg.h"
#include "RecentDlg.h"
#include <assert.h>
#include "structs.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

bool COptionsDlg::m_bLock=false;

/////////////////////////////////////////////////////////////////////////////
// COptionsDlg dialog

COptionsDlg::COptionsDlg(CWnd* pParent /*=NULL*/)
	:ictranslate::CLanguageDialog(COptionsDlg::IDD, pParent, &m_bLock)
{
	//{{AFX_DATA_INIT(COptionsDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void COptionsDlg::DoDataExchange(CDataExchange* pDX)
{
	CLanguageDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COptionsDlg)
	DDX_Control(pDX, IDC_PROPERTIES_LIST, m_ctlProperties);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(COptionsDlg,ictranslate::CLanguageDialog)
	//{{AFX_MSG_MAP(COptionsDlg)
	ON_BN_CLICKED(IDC_APPLY_BUTTON, OnApplyButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COptionsDlg message handlers

// properties handling macros
#define PROP_SEPARATOR(text)\
	m_ctlProperties.AddString(text)

#define PROP_BOOL(text, value)\
	m_ctlProperties.AddString(text, ID_PROPERTY_COMBO_LIST, IDS_BOOLTEXT_STRING, (value))

#define PROP_UINT(text, value)\
	m_ctlProperties.AddString(text, ID_PROPERTY_TEXT, _itot((int)(value), m_szBuffer, 10), 0)

#define PROP_COMBO(text, prop_text, value)\
	m_ctlProperties.AddString(text, ID_PROPERTY_COMBO_LIST, prop_text, (int)((value)))

#define PROP_DIR(text, prop_text, value)\
	m_ctlProperties.AddString(text, ID_PROPERTY_DIR, (value)+CString(GetResManager().LoadString(prop_text)), 0)

#define PROP_PATH(text, prop_text, value)\
	m_ctlProperties.AddString(text, ID_PROPERTY_PATH, (value)+CString(GetResManager().LoadString(prop_text)), 0)

#define PROP_CUSTOM_UINT(text, value, callback, param)\
	m_ctlProperties.AddString(text, ID_PROPERTY_CUSTOM, CString(_itot((int)(value), m_szBuffer, 10)), callback, this, param, 0)

#define SKIP_SEPARATOR(pos)\
	pos++

BOOL COptionsDlg::OnInitDialog() 
{
	CLanguageDialog::OnInitDialog();

	AddResizableControl(IDC_PROPERTIES_LIST, 0.0, 0.0, 1.0, 1.0);
	AddResizableControl(IDOK, 1.0, 1.0, 0.0, 0.0);
	AddResizableControl(IDCANCEL, 1.0, 1.0, 0.0, 0.0);
	AddResizableControl(IDC_APPLY_BUTTON, 1.0, 1.0, 0.0, 0.0);
	AddResizableControl(IDHELP, 1.0, 1.0, 0.0, 0.0);

	InitializeResizableControls();

	m_ctlProperties.Init();

	// copy shortcut and recent paths
	icpf::config& rConfig = GetConfig();

	m_cvRecent.clear(true);
	size_t stSize = rConfig.get_value_count(PP_RECENTPATHS);
	const tchar_t* pszPath = NULL;
	for(size_t stIndex = 0; stIndex < stSize; stIndex++)
	{
		pszPath = rConfig.get_string(PP_RECENTPATHS, stIndex);
		if(pszPath)
			m_cvRecent.push_back(pszPath);
	}

	m_cvShortcuts.clear(true);
	stSize = rConfig.get_value_count(PP_SHORTCUTS);
	for(size_t stIndex = 0; stIndex < stSize; stIndex++)
	{
		pszPath = rConfig.get_string(PP_SHORTCUTS, stIndex);
		if(pszPath)
			m_cvShortcuts.push_back(pszPath);
	}

	_tcscpy(m_szLangPath, _T("<PROGRAM>\\Langs\\"));
	GetApp().ExpandPath(m_szLangPath);

	GetResManager().Scan(m_szLangPath, &m_vld);

	// some attributes
	m_ctlProperties.SetBkColor(RGB(255, 255, 255));
	m_ctlProperties.SetTextColor(RGB(80, 80, 80));
	m_ctlProperties.SetTextHighlightColor(RGB(80,80,80));
	m_ctlProperties.SetHighlightColor(RGB(200, 200, 200));
	m_ctlProperties.SetPropertyBkColor(RGB(255,255,255));
	m_ctlProperties.SetPropertyTextColor(RGB(0,0,0));
	m_ctlProperties.SetLineStyle(RGB(74,109,132), PS_SOLID);

	FillPropertyList();

	return TRUE;
}

void CustomPropertyCallbackProc(LPVOID lpParam, int iParam, CPtrList* pList, int iIndex)
{
	COptionsDlg* pDlg=static_cast<COptionsDlg*>(lpParam);
	CBufferSizeDlg dlg;

	dlg.m_bsSizes.m_bOnlyDefault=pDlg->GetBoolProp(iIndex-iParam-1);
	dlg.m_bsSizes.m_uiDefaultSize=pDlg->GetUintProp(iIndex-iParam);
	dlg.m_bsSizes.m_uiOneDiskSize=pDlg->GetUintProp(iIndex-iParam+1);
	dlg.m_bsSizes.m_uiTwoDisksSize=pDlg->GetUintProp(iIndex-iParam+2);
	dlg.m_bsSizes.m_uiCDSize=pDlg->GetUintProp(iIndex-iParam+3);
	dlg.m_bsSizes.m_uiLANSize=pDlg->GetUintProp(iIndex-iParam+4);
	dlg.m_iActiveIndex=iParam;	// selected buffer for editing

	if (dlg.DoModal() == IDOK)
	{
		PROPERTYITEM* pItem;
		TCHAR xx[32];

		pItem = (PROPERTYITEM*)pList->GetAt(pList->FindIndex(iIndex-iParam-1));
		pItem->nPropertySelected=(dlg.m_bsSizes.m_bOnlyDefault ? 1 : 0);
		pItem = (PROPERTYITEM*)pList->GetAt(pList->FindIndex(iIndex-iParam));
		pItem->csProperties.SetAt(0, _itot(dlg.m_bsSizes.m_uiDefaultSize, xx, 10));
		pItem = (PROPERTYITEM*)pList->GetAt(pList->FindIndex(iIndex-iParam+1));
		pItem->csProperties.SetAt(0, _itot(dlg.m_bsSizes.m_uiOneDiskSize, xx, 10));
		pItem = (PROPERTYITEM*)pList->GetAt(pList->FindIndex(iIndex-iParam+2));
		pItem->csProperties.SetAt(0, _itot(dlg.m_bsSizes.m_uiTwoDisksSize, xx, 10));
		pItem = (PROPERTYITEM*)pList->GetAt(pList->FindIndex(iIndex-iParam+3));
		pItem->csProperties.SetAt(0, _itot(dlg.m_bsSizes.m_uiCDSize, xx, 10));
		pItem = (PROPERTYITEM*)pList->GetAt(pList->FindIndex(iIndex-iParam+4));
		pItem->csProperties.SetAt(0, _itot(dlg.m_bsSizes.m_uiLANSize, xx, 10));
	}
}

void ShortcutsPropertyCallbackProc(LPVOID lpParam, int /*iParam*/, CPtrList* pList, int iIndex)
{
	COptionsDlg* pDlg=static_cast<COptionsDlg*>(lpParam);

	CShortcutsDlg dlg;
	dlg.m_cvShortcuts.assign(pDlg->m_cvShortcuts.begin(), pDlg->m_cvShortcuts.end(), true, true);
	dlg.m_pcvRecent=&pDlg->m_cvRecent;
	if (dlg.DoModal() == IDOK)
	{
		// restore shortcuts to pDlg->cvShortcuts
		pDlg->m_cvShortcuts.assign(dlg.m_cvShortcuts.begin(), dlg.m_cvShortcuts.end(), true, false);
		dlg.m_cvShortcuts.erase(dlg.m_cvShortcuts.begin(), dlg.m_cvShortcuts.end(), false);

		// property list
		TCHAR szBuf[32];
		PROPERTYITEM* pItem;
		pItem = (PROPERTYITEM*)pList->GetAt(pList->FindIndex(iIndex));
		pItem->csProperties.SetAt(0, _ui64tot(pDlg->m_cvShortcuts.size(), szBuf, 10));
	}
}

void RecentPropertyCallbackProc(LPVOID lpParam, int /*iParam*/, CPtrList* pList, int iIndex)
{
	COptionsDlg* pDlg=static_cast<COptionsDlg*>(lpParam);

	CRecentDlg dlg;
	dlg.m_cvRecent.assign(pDlg->m_cvRecent.begin(), pDlg->m_cvRecent.end(), true, true);
	if (dlg.DoModal() == IDOK)
	{
		// restore
		pDlg->m_cvRecent.assign(dlg.m_cvRecent.begin(), dlg.m_cvRecent.end(), true, false);
		dlg.m_cvRecent.erase(dlg.m_cvRecent.begin(), dlg.m_cvRecent.end(), false);

		// property list
		TCHAR szBuf[32];
		PROPERTYITEM* pItem;
		pItem = (PROPERTYITEM*)pList->GetAt(pList->FindIndex(iIndex));
		pItem->csProperties.SetAt(0, _ui64tot(pDlg->m_cvRecent.size(), szBuf, 10));
	}
}

void COptionsDlg::OnOK() 
{
	// kill focuses
	m_ctlProperties.HideControls();

	ApplyProperties();

	SendClosingNotify();
	CLanguageDialog::OnOK();
}

void COptionsDlg::FillPropertyList()
{
	CString strPath;

	// load settings
	PROP_SEPARATOR(IDS_PROGRAM_STRING);
	PROP_BOOL(IDS_CLIPBOARDMONITORING_STRING, GetConfig().get_bool(PP_PCLIPBOARDMONITORING));
	PROP_UINT(IDS_CLIPBOARDINTERVAL_STRING, GetConfig().get_signed_num(PP_PMONITORSCANINTERVAL));
	PROP_BOOL(IDS_AUTORUNPROGRAM_STRING, GetConfig().get_bool(PP_PRELOADAFTERRESTART));
	PROP_COMBO(IDS_CFG_CHECK_FOR_UPDATES_FREQUENCY, IDS_UPDATE_FREQUENCIES, GetConfig().get_unsigned_num(PP_PCHECK_FOR_UPDATES_FREQUENCY));
	PROP_BOOL(IDS_CFG_UPDATE_CHECK_FOR_BETA, GetConfig().get_bool(PP_PUPDATE_CHECK_FOR_BETA));
	PROP_BOOL(IDS_AUTOSHUTDOWN_STRING, GetConfig().get_bool(PP_PSHUTDOWNAFTREFINISHED));
	PROP_UINT(IDS_SHUTDOWNTIME_STRING, GetConfig().get_signed_num(PP_PTIMEBEFORESHUTDOWN));
	PROP_COMBO(IDS_FORCESHUTDOWN_STRING, IDS_FORCESHUTDOWNVALUES_STRING, GetConfig().get_bool(PP_PFORCESHUTDOWN));
	PROP_UINT(IDS_AUTOSAVEINTERVAL_STRING, GetConfig().get_signed_num(PP_PAUTOSAVEINTERVAL));
	PROP_COMBO(IDS_CFGPRIORITYCLASS_STRING, IDS_CFGPRIORITYCLASSITEMS_STRING, PriorityClassToIndex((int)GetConfig().get_signed_num(PP_PPROCESSPRIORITYCLASS)));
	PROP_DIR(IDS_TEMPFOLDER_STRING, IDS_TEMPFOLDERCHOOSE_STRING, strPath);

	// lang
	CString strLangs;
	size_t stIndex=0;
	for (vector<ictranslate::CLangData>::iterator it=m_vld.begin();it != m_vld.end();it++)
	{
		strLangs+=(*it).GetLangName();
		strLangs+=_T("!");
		if (_tcsicmp((*it).GetFilename(true), GetResManager().m_ld.GetFilename(true)) == 0)
			stIndex = it - m_vld.begin();
	}
	strLangs.TrimRight(_T('!'));

	PROP_COMBO(IDS_LANGUAGE_STRING, strLangs, stIndex);

	/////////////////
	PROP_SEPARATOR(IDS_STATUSWINDOW_STRING);
	PROP_UINT(IDS_REFRESHSTATUSINTERVAL_STRING, GetConfig().get_signed_num(PP_STATUSREFRESHINTERVAL));
	PROP_BOOL(IDS_STATUSSHOWDETAILS_STRING, GetConfig().get_bool(PP_STATUSSHOWDETAILS));
	PROP_BOOL(IDS_STATUSAUTOREMOVE_STRING, GetConfig().get_bool(PP_STATUSAUTOREMOVEFINISHED));

	PROP_SEPARATOR(IDS_MINIVIEW_STRING);
	PROP_BOOL(IDS_SHOWFILENAMES_STRING, GetConfig().get_bool(PP_MVSHOWFILENAMES));
	PROP_BOOL(IDS_SHOWSINGLETASKS_STRING, GetConfig().get_bool(PP_MVSHOWSINGLETASKS));
	PROP_UINT(IDS_MINIVIEWREFRESHINTERVAL_STRING, GetConfig().get_signed_num(PP_MVREFRESHINTERVAL));
	PROP_BOOL(IDS_MINIVIEWSHOWAFTERSTART_STRING, GetConfig().get_bool(PP_MVAUTOSHOWWHENRUN));
	PROP_BOOL(IDS_MINIVIEWAUTOHIDE_STRING, GetConfig().get_bool(PP_MVAUTOHIDEWHENEMPTY));
	PROP_BOOL(IDS_MINIVIEWSMOOTHPROGRESS_STRING, GetConfig().get_bool(PP_MVUSESMOOTHPROGRESS));

	PROP_SEPARATOR(IDS_CFGFOLDERDIALOG_STRING);
	PROP_BOOL(IDS_CFGFDEXTVIEW_STRING, GetConfig().get_bool(PP_FDEXTENDEDVIEW));
	PROP_UINT(IDS_CFGFDWIDTH_STRING, GetConfig().get_signed_num(PP_FDWIDTH));
	PROP_UINT(IDS_CFGFDHEIGHT_STRING, GetConfig().get_signed_num(PP_FDHEIGHT));
	PROP_COMBO(IDS_CFGFDSHORTCUTS_STRING, IDS_CFGFDSHORTCUTSSTYLES_STRING, GetConfig().get_signed_num(PP_FDSHORTCUTLISTSTYLE));
	PROP_BOOL(IDS_CFGFDIGNOREDIALOGS_STRING, GetConfig().get_bool(PP_FDIGNORESHELLDIALOGS));

	PROP_SEPARATOR(IDS_CFGSHELL_STRING);
	PROP_BOOL(IDS_CFGSHCOPY_STRING, GetConfig().get_bool(PP_SHSHOWCOPY));
	PROP_BOOL(IDS_CFGSHMOVE_STRING, GetConfig().get_bool(PP_SHSHOWMOVE));
	PROP_BOOL(IDS_CFGSHCMSPECIAL_STRING, GetConfig().get_bool(PP_SHSHOWCOPYMOVE));
	PROP_BOOL(IDS_CFGSHPASTE_STRING, GetConfig().get_bool(PP_SHSHOWPASTE));
	PROP_BOOL(IDS_CFGSHPASTESPECIAL_STRING, GetConfig().get_bool(PP_SHSHOWPASTESPECIAL));
	PROP_BOOL(IDS_CFGSHCOPYTO_STRING, GetConfig().get_bool(PP_SHSHOWCOPYTO));
	PROP_BOOL(IDS_CFGSHMOVETO_STRING, GetConfig().get_bool(PP_SHSHOWMOVETO));
	PROP_BOOL(IDS_CFGSHCMTOSPECIAL_STRING, GetConfig().get_bool(PP_SHSHOWCOPYMOVETO));
	PROP_BOOL(IDS_CFGSHSHOWFREESPACE_STRING, GetConfig().get_bool(PP_SHSHOWFREESPACE));
	PROP_BOOL(IDS_CFGSHSHOWICONS_STRING, GetConfig().get_bool(PP_SHSHOWSHELLICONS));
	PROP_BOOL(IDS_CFGSHINTERCEPTDRAG_STRING, GetConfig().get_bool(PP_SHINTERCEPTDRAGDROP));
	PROP_BOOL(IDS_CFGINTERCEPTKEYACTION_STRING, GetConfig().get_bool(PP_SHINTERCEPTKEYACTIONS));
	PROP_BOOL(IDS_CFGINTERCEPTCONTEXTMENU_STRING, GetConfig().get_bool(PP_SHINTERCEPTCTXMENUACTIONS));

	PROP_SEPARATOR(IDS_PROCESSINGTHREAD_STRING);
	PROP_BOOL(IDS_AUTOCOPYREST_STRING, GetConfig().get_bool(PP_CMUSEAUTOCOMPLETEFILES));
	PROP_BOOL(IDS_SETDESTATTRIB_STRING, GetConfig().get_bool(PP_CMSETDESTATTRIBUTES));
	PROP_BOOL(IDS_SETDESTTIME_STRING, GetConfig().get_bool(PP_CMSETDESTDATE));
	PROP_BOOL(IDS_PROTECTROFILES_STRING, GetConfig().get_bool(PP_CMPROTECTROFILES));
	PROP_UINT(IDS_LIMITOPERATIONS_STRING, GetConfig().get_signed_num(PP_CMLIMITMAXOPERATIONS));
	PROP_BOOL(IDS_READSIZEBEFOREBLOCK_STRING, GetConfig().get_bool(PP_CMREADSIZEBEFOREBLOCKING));
	PROP_COMBO(IDS_SHOWVISUALFEEDBACK_STRING, IDS_FEEDBACKTYPE_STRING, GetConfig().get_signed_num(PP_CMSHOWVISUALFEEDBACK));
	PROP_BOOL(IDS_USETIMEDDIALOGS_STRING, GetConfig().get_bool(PP_CMUSETIMEDFEEDBACK));
	PROP_UINT(IDS_TIMEDDIALOGINTERVAL_STRING, GetConfig().get_signed_num(PP_CMFEEDBACKTIME));
	PROP_BOOL(IDS_AUTORETRYONERROR_STRING, GetConfig().get_bool(PP_CMAUTORETRYONERROR));
	PROP_UINT(IDS_AUTORETRYINTERVAL_STRING, GetConfig().get_signed_num(PP_CMAUTORETRYINTERVAL));
	PROP_COMBO(IDS_DEFAULTPRIORITY_STRING, MakeCompoundString(IDS_PRIORITY0_STRING, 7, _T("!")), PriorityToIndex((int)GetConfig().get_signed_num(PP_CMDEFAULTPRIORITY)));
	PROP_BOOL(IDS_CFGDISABLEPRIORITYBOOST_STRING, GetConfig().get_bool(PP_CMDISABLEPRIORITYBOOST));
	PROP_BOOL(IDS_DELETEAFTERFINISHED_STRING, GetConfig().get_bool(PP_CMDELETEAFTERFINISHED));
	PROP_BOOL(IDS_CREATELOGFILES_STRING, GetConfig().get_bool(PP_CMCREATELOG));

	// Buffer
	PROP_SEPARATOR(IDS_OPTIONSBUFFER_STRING);
	PROP_BOOL(IDS_AUTODETECTBUFFERSIZE_STRING, GetConfig().get_bool(PP_BFUSEONLYDEFAULT));
	PROP_CUSTOM_UINT(IDS_DEFAULTBUFFERSIZE_STRING, GetConfig().get_signed_num(PP_BFDEFAULT), &CustomPropertyCallbackProc, 0);
	PROP_CUSTOM_UINT(IDS_ONEDISKBUFFERSIZE_STRING, GetConfig().get_signed_num(PP_BFONEDISK), &CustomPropertyCallbackProc, 1);
	PROP_CUSTOM_UINT(IDS_TWODISKSBUFFERSIZE_STRING, GetConfig().get_signed_num(PP_BFTWODISKS), &CustomPropertyCallbackProc, 2);
	PROP_CUSTOM_UINT(IDS_CDBUFFERSIZE_STRING, GetConfig().get_signed_num(PP_BFCD), &CustomPropertyCallbackProc, 3);
	PROP_CUSTOM_UINT(IDS_LANBUFFERSIZE_STRING, GetConfig().get_signed_num(PP_BFLAN), &CustomPropertyCallbackProc, 4);
	PROP_BOOL(IDS_USENOBUFFERING_STRING, GetConfig().get_bool(PP_BFUSENOBUFFERING));
	PROP_UINT(IDS_LARGEFILESMINSIZE_STRING, GetConfig().get_signed_num(PP_BFBOUNDARYLIMIT));

	PROP_SEPARATOR(IDS_CFGLOGFILE_STRING);
	PROP_BOOL(IDS_CFGENABLELOGGING_STRING, GetConfig().get_bool(PP_LOGENABLELOGGING));
	PROP_UINT(IDS_CFGMAXLIMIT_STRING, GetConfig().get_signed_num(PP_LOGMAXSIZE));
	PROP_COMBO(IDS_CFGLOGLEVEL, IDS_CFGLOGLEVEL_VALUES, GetConfig().get_unsigned_num(PP_LOGLEVEL));

	// Sounds
	PROP_SEPARATOR(IDS_SOUNDS_STRING);
	PROP_BOOL(IDS_PLAYSOUNDS_STRING, GetConfig().get_bool(PP_SNDPLAYSOUNDS));
	GetConfig().get_string(PP_SNDERRORSOUNDPATH, strPath.GetBuffer(_MAX_PATH), _MAX_PATH);
	strPath.ReleaseBuffer();
	PROP_PATH(IDS_SOUNDONERROR_STRING, IDS_SOUNDSWAVFILTER_STRING, strPath);
	GetConfig().get_string(PP_SNDFINISHEDSOUNDPATH, strPath.GetBuffer(_MAX_PATH), _MAX_PATH);
	strPath.ReleaseBuffer();
	PROP_PATH(IDS_SOUNDONFINISH_STRING, IDS_SOUNDSWAVFILTER_STRING, strPath);

	PROP_SEPARATOR(IDS_CFGSHORTCUTS_STRING);
	PROP_CUSTOM_UINT(IDS_CFGSCCOUNT_STRING, m_cvShortcuts.size(), &ShortcutsPropertyCallbackProc, 0);

	PROP_SEPARATOR(IDS_CFGRECENT_STRING);
	PROP_CUSTOM_UINT(IDS_CFGRPCOUNT_STRING, m_cvRecent.size(), &RecentPropertyCallbackProc, 0);
}

void COptionsDlg::ApplyProperties()
{
	// counter
	int iPosition=0;

	icpf::config& rConfig = GetConfig();

	SKIP_SEPARATOR(iPosition);
	rConfig.set_bool(PP_PCLIPBOARDMONITORING, GetBoolProp(iPosition++));
	rConfig.set_signed_num(PP_PMONITORSCANINTERVAL, GetUintProp(iPosition++));
	rConfig.set_bool(PP_PRELOADAFTERRESTART, GetBoolProp(iPosition++));
	rConfig.set_unsigned_num(PP_PCHECK_FOR_UPDATES_FREQUENCY, GetIndexProp(iPosition++));
	rConfig.set_bool(PP_PUPDATE_CHECK_FOR_BETA, GetBoolProp(iPosition++));
	rConfig.set_bool(PP_PSHUTDOWNAFTREFINISHED, GetBoolProp(iPosition++));
	rConfig.set_signed_num(PP_PTIMEBEFORESHUTDOWN, GetUintProp(iPosition++));
	rConfig.set_bool(PP_PFORCESHUTDOWN, GetBoolProp(iPosition++));
	rConfig.set_signed_num(PP_PAUTOSAVEINTERVAL, GetUintProp(iPosition++));
	rConfig.set_signed_num(PP_PPROCESSPRIORITYCLASS, IndexToPriorityClass(GetIndexProp(iPosition++)));
	// language
	PCTSTR pszSrc=m_vld.at(GetIndexProp(iPosition++)).GetFilename(true);
	if (_tcsnicmp(pszSrc, GetApp().GetProgramPath(), _tcslen(GetApp().GetProgramPath())) == 0)
	{
		// replace the first part of path with <PROGRAM>
		TCHAR szData[_MAX_PATH];
		_sntprintf(szData, _MAX_PATH, _T("<PROGRAM>%s"), pszSrc+_tcslen(GetApp().GetProgramPath()));
		rConfig.set_string(PP_PLANGUAGE, szData);
	}
	else
		rConfig.set_string(PP_PLANGUAGE, pszSrc);

	SKIP_SEPARATOR(iPosition);
	rConfig.set_signed_num(PP_STATUSREFRESHINTERVAL, GetUintProp(iPosition++));
	rConfig.set_bool(PP_STATUSSHOWDETAILS, GetBoolProp(iPosition++));
	rConfig.set_bool(PP_STATUSAUTOREMOVEFINISHED, GetBoolProp(iPosition++));

	SKIP_SEPARATOR(iPosition);
	rConfig.set_bool(PP_MVSHOWFILENAMES, GetBoolProp(iPosition++));
	rConfig.set_bool(PP_MVSHOWSINGLETASKS, GetBoolProp(iPosition++));
	rConfig.set_signed_num(PP_MVREFRESHINTERVAL, GetUintProp(iPosition++));
	rConfig.set_bool(PP_MVAUTOSHOWWHENRUN, GetBoolProp(iPosition++));
	rConfig.set_bool(PP_MVAUTOHIDEWHENEMPTY, GetBoolProp(iPosition++));
	rConfig.set_bool(PP_MVUSESMOOTHPROGRESS, GetBoolProp(iPosition++));

	SKIP_SEPARATOR(iPosition);
	rConfig.set_bool(PP_FDEXTENDEDVIEW, GetBoolProp(iPosition++));
	rConfig.set_signed_num(PP_FDWIDTH, GetUintProp(iPosition++));
	rConfig.set_signed_num(PP_FDHEIGHT, GetUintProp(iPosition++));
	rConfig.set_signed_num(PP_FDSHORTCUTLISTSTYLE, GetIndexProp(iPosition++));
	rConfig.set_bool(PP_FDIGNORESHELLDIALOGS, GetBoolProp(iPosition++));

	SKIP_SEPARATOR(iPosition);
	rConfig.set_bool(PP_SHSHOWCOPY, GetBoolProp(iPosition++));
	rConfig.set_bool(PP_SHSHOWMOVE, GetBoolProp(iPosition++));
	rConfig.set_bool(PP_SHSHOWCOPYMOVE, GetBoolProp(iPosition++));
	rConfig.set_bool(PP_SHSHOWPASTE, GetBoolProp(iPosition++));
	rConfig.set_bool(PP_SHSHOWPASTESPECIAL, GetBoolProp(iPosition++));
	rConfig.set_bool(PP_SHSHOWCOPYTO, GetBoolProp(iPosition++));
	rConfig.set_bool(PP_SHSHOWMOVETO, GetBoolProp(iPosition++));
	rConfig.set_bool(PP_SHSHOWCOPYMOVETO, GetBoolProp(iPosition++));
	rConfig.set_bool(PP_SHSHOWFREESPACE, GetBoolProp(iPosition++));
	rConfig.set_bool(PP_SHSHOWSHELLICONS, GetBoolProp(iPosition++));
	rConfig.set_bool(PP_SHINTERCEPTDRAGDROP, GetBoolProp(iPosition++));
	rConfig.set_bool(PP_SHINTERCEPTKEYACTIONS, GetBoolProp(iPosition++));
	rConfig.set_bool(PP_SHINTERCEPTCTXMENUACTIONS, GetBoolProp(iPosition++));

	SKIP_SEPARATOR(iPosition);
	rConfig.set_bool(PP_CMUSEAUTOCOMPLETEFILES, GetBoolProp(iPosition++));
	rConfig.set_bool(PP_CMSETDESTATTRIBUTES, GetBoolProp(iPosition++));
	rConfig.set_bool(PP_CMSETDESTDATE, GetBoolProp(iPosition++));
	rConfig.set_bool(PP_CMPROTECTROFILES, GetBoolProp(iPosition++));
	rConfig.set_signed_num(PP_CMLIMITMAXOPERATIONS, GetUintProp(iPosition++));
	rConfig.set_bool(PP_CMREADSIZEBEFOREBLOCKING, GetBoolProp(iPosition++));
	rConfig.set_signed_num(PP_CMSHOWVISUALFEEDBACK, GetIndexProp(iPosition++));
	rConfig.set_bool(PP_CMUSETIMEDFEEDBACK, GetBoolProp(iPosition++));
	rConfig.set_signed_num(PP_CMFEEDBACKTIME, GetUintProp(iPosition++));
	rConfig.set_bool(PP_CMAUTORETRYONERROR, GetBoolProp(iPosition++));
	rConfig.set_signed_num(PP_CMAUTORETRYINTERVAL, GetUintProp(iPosition++));
	rConfig.set_signed_num(PP_CMDEFAULTPRIORITY, IndexToPriority(GetIndexProp(iPosition++)));
	rConfig.set_bool(PP_CMDISABLEPRIORITYBOOST, GetBoolProp(iPosition++));
	rConfig.set_bool(PP_CMDELETEAFTERFINISHED, GetBoolProp(iPosition++));
	rConfig.set_bool(PP_CMCREATELOG, GetBoolProp(iPosition++));

	// Buffer
	SKIP_SEPARATOR(iPosition);
	rConfig.set_bool(PP_BFUSEONLYDEFAULT, GetBoolProp(iPosition++));
	rConfig.set_signed_num(PP_BFDEFAULT, GetUintProp(iPosition++));
	rConfig.set_signed_num(PP_BFONEDISK, GetUintProp(iPosition++));
	rConfig.set_signed_num(PP_BFTWODISKS, GetUintProp(iPosition++));
	rConfig.set_signed_num(PP_BFCD, GetUintProp(iPosition++));
	rConfig.set_signed_num(PP_BFLAN, GetUintProp(iPosition++));
	rConfig.set_bool(PP_BFUSENOBUFFERING, GetBoolProp(iPosition++));
	rConfig.set_signed_num(PP_BFBOUNDARYLIMIT, GetUintProp(iPosition++));

	// log file
	SKIP_SEPARATOR(iPosition);
	rConfig.set_bool(PP_LOGENABLELOGGING, GetBoolProp(iPosition++));
	rConfig.set_signed_num(PP_LOGMAXSIZE, GetUintProp(iPosition++));
	rConfig.set_unsigned_num(PP_LOGLEVEL, GetIndexProp(iPosition++));

	// Sounds
	SKIP_SEPARATOR(iPosition);
	rConfig.set_bool(PP_SNDPLAYSOUNDS, GetBoolProp(iPosition++));
	rConfig.set_string(PP_SNDERRORSOUNDPATH, GetStringProp(iPosition++));
	rConfig.set_string(PP_SNDFINISHEDSOUNDPATH, GetStringProp(iPosition++));

	// shortcuts & recent paths
	SKIP_SEPARATOR(iPosition);
	rConfig.clear_array_values(PP_SHORTCUTS);
	for(char_vector::iterator it = m_cvShortcuts.begin(); it != m_cvShortcuts.end(); it++)
	{
		rConfig.set_string(PP_SHORTCUTS, (*it), icpf::property::action_add);
	}
	
	SKIP_SEPARATOR(iPosition);
	rConfig.clear_array_values(PP_RECENTPATHS);
	for(char_vector::iterator it = m_cvRecent.begin(); it != m_cvRecent.end(); it++)
	{
		rConfig.set_string(PP_RECENTPATHS, (*it), icpf::property::action_add);
	}

	rConfig.write(NULL);
}

void COptionsDlg::OnCancel() 
{
	SendClosingNotify();
	CLanguageDialog::OnCancel();
}

void COptionsDlg::SendClosingNotify()
{
	GetParent()->PostMessage(WM_CONFIGNOTIFY);
}

CString COptionsDlg::MakeCompoundString(UINT uiBase, int iCount, LPCTSTR lpszSeparator)
{
	assert(lpszSeparator);
	if(!lpszSeparator)
		return _T("");
	_tcscpy(m_szBuffer, GetResManager().LoadString(uiBase+0));
	for (int i=1;i<iCount;i++)
	{
		_tcscat(m_szBuffer, lpszSeparator);
		_tcscat(m_szBuffer, GetResManager().LoadString(uiBase+i));
	}

	return CString((PCTSTR)m_szBuffer);
}

bool COptionsDlg::GetBoolProp(int iPosition)
{
	m_ctlProperties.GetProperty(iPosition, &m_iSel);
	return m_iSel != 0;
}

UINT COptionsDlg::GetUintProp(int iPosition)
{
	m_ctlProperties.GetProperty(iPosition, &m_strTemp);
	return _ttoi(m_strTemp);
}

CString COptionsDlg::GetStringProp(int iPosition)
{
	m_ctlProperties.GetProperty(iPosition, &m_strTemp);
	return m_strTemp;
}

int COptionsDlg::GetIndexProp(int iPosition)
{
	m_ctlProperties.GetProperty(iPosition, &m_iSel);
	return m_iSel;
}

void COptionsDlg::OnApplyButton() 
{
	// kill focuses
	m_ctlProperties.HideControls();

	ApplyProperties();
}

void COptionsDlg::OnLanguageChanged()
{
	m_ctlProperties.Reinit();

	// set attributes
	m_ctlProperties.SetBkColor(RGB(255, 255, 255));
	m_ctlProperties.SetTextColor(RGB(80, 80, 80));
	m_ctlProperties.SetTextHighlightColor(RGB(80,80,80));
	m_ctlProperties.SetHighlightColor(RGB(200, 200, 200));
	m_ctlProperties.SetPropertyBkColor(RGB(255,255,255));
	m_ctlProperties.SetPropertyTextColor(RGB(0,0,0));
	m_ctlProperties.SetLineStyle(RGB(74,109,132), PS_SOLID);

	FillPropertyList();
}
