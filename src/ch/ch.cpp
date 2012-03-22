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

#include "CfgProperties.h"
#include "MainWnd.h"
#include "../common/ipcstructs.h"
#include <Dbghelp.h>
#include "CrashDlg.h"
#include "../common/version.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

icpf::config CCopyHandlerApp::m_config = icpf::config::eIni;

/////////////////////////////////////////////////////////////////////////////
// CCopyHandlerApp

BEGIN_MESSAGE_MAP(CCopyHandlerApp, CWinApp)
	//{{AFX_MSG_MAP(CCopyHandlerApp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CSharedConfigStruct* g_pscsShared;

int iCount=98;
unsigned short msg[]={	0x40d1, 0x4dcd, 0x8327, 0x6cdf, 0xb912, 0x017b, 0xac78, 0x1e04, 0x5637,
						0x1822, 0x0a69, 0x1b40, 0x4169, 0x504d, 0x80ff, 0x6c2f, 0xa612, 0x017e,
						0xac84, 0x1c8c, 0x552b, 0x16e2, 0x0a4b, 0x1dc0, 0x4179, 0x4d0d, 0x8337,
						0x6c4f, 0x6512, 0x0169, 0xac46, 0x1db4, 0x55cf, 0x1652, 0x0a0b, 0x1480,
						0x40fd, 0x470d, 0x822f, 0x6b8f, 0x6512, 0x013a, 0xac5a, 0x1d24, 0x5627,
						0x1762, 0x0a27, 0x1240, 0x40f5, 0x3f8d, 0x8187, 0x690f, 0x6e12, 0x011c,
						0xabc0, 0x1cc4, 0x567f, 0x1952, 0x0a51, 0x1cc0, 0x4175, 0x3ccd, 0x8377,
						0x6c5f, 0x6512, 0x0186, 0xac7c, 0x1e04, 0x5677, 0x1412, 0x0a61, 0x1d80,
						0x4169, 0x4e8d, 0x838f, 0x6c0f, 0xb212, 0x0132, 0xac7e, 0x1e54, 0x5593,
						0x1412, 0x0a15, 0x3dc0, 0x4195, 0x4e0d, 0x832f, 0x67ff, 0x9812, 0x0186,
						0xac6e, 0x1e4c, 0x5667, 0x1942, 0x0a47, 0x1f80, 0x4191, 0x4f8d };

int iOffCount=12;
unsigned char off[]={ 2, 6, 3, 4, 8, 0, 1, 3, 2, 4, 1, 6 };
unsigned short _hash[]={ 0x3fad, 0x34cd, 0x7fff, 0x65ff, 0x4512, 0x0112, 0xabac, 0x1abc, 0x54ab, 0x1212, 0x0981, 0x0100 };

/////////////////////////////////////////////////////////////////////////////
// The one and only CCopyHandlerApp object

CCopyHandlerApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CCopyHandlerApp construction

// main routing function - routes any message that comes from modules
void ResManCallback(uint_t uiMsg)
{
	theApp.OnResManNotify(uiMsg);
}

void ConfigPropertyChangedCallback(uint_t uiPropID, ptr_t /*pParam*/)
{
	theApp.OnConfigNotify(uiPropID);
}

CCopyHandlerApp::CCopyHandlerApp() :
	m_piShellExtControl(NULL),
	m_hMapObject(NULL),
	m_pMainWindow(NULL)
{
	// this is the one-instance application
	InitProtection();
}

CCopyHandlerApp::~CCopyHandlerApp()
{
	// Unmap shared memory from the process's address space.
	UnmapViewOfFile((LPVOID)g_pscsShared); 
	
	// Close the process's handle to the file-mapping object.
	CloseHandle(m_hMapObject);

	if (m_pMainWindow)
	{
		((CMainWnd*)m_pMainWindow)->DestroyWindow();
		delete m_pMainWindow;
		m_pMainWnd=NULL;
	}

	if(m_piShellExtControl)
	{
		m_piShellExtControl->Release();
		m_piShellExtControl = NULL;
	}
}

CCopyHandlerApp& GetApp()
{
	return theApp;
}

ictranslate::CResourceManager& GetResManager()
{
	return ictranslate::CResourceManager::Acquire();
}

icpf::config& GetConfig()
{
	return CCopyHandlerApp::m_config;
}

int MsgBox(UINT uiID, UINT nType, UINT nIDHelp)
{
	return AfxMessageBox(GetResManager().LoadString(uiID), nType, nIDHelp);
}

bool CCopyHandlerApp::UpdateHelpPaths()
{
	bool bChanged=false;		// flag that'll be returned - if the paths has changed

	// generate the current filename - uses language from config
	TCHAR szBuffer[_MAX_PATH];
	_tcscpy(szBuffer, _T("<PROGRAM>\\Help\\"));
	ExpandPath(szBuffer);
	_tcscat(szBuffer, GetResManager().m_ld.GetHelpName());
	if(_tcscmp(szBuffer, m_pszHelpFilePath) != 0)
	{
		free((void*)m_pszHelpFilePath);
		m_pszHelpFilePath = _tcsdup(szBuffer);
		bChanged=true;
	}

	return bChanged;
}

/////////////////////////////////////////////////////////////////////////////
// CCopyHandlerApp initialization
#include "charvect.h"

LONG WINAPI MyUnhandledExceptionFilter(struct _EXCEPTION_POINTERS* ExceptionInfo)
{
	// Step1 - should not fail - prepare some more unique crash name, create under the path where ch data exists
	TCHAR szPath[_MAX_PATH];
	HRESULT hResult = SHGetFolderPath(NULL, CSIDL_LOCAL_APPDATA, NULL, 0, szPath);
	if(FAILED(hResult))
		_tcscpy(szPath, _T("c:\\"));

	CString strPath(szPath);
	// make sure to create the required directories if they does not exist
	strPath += _T("\\Copy Handler");
	CreateDirectory(strPath, NULL);
	strPath += _T("\\Dumps");
	CreateDirectory(strPath, NULL);

	// current date
	SYSTEMTIME st;
	GetLocalTime(&st);
	
	TCHAR szName[_MAX_PATH];
	_sntprintf(szName, _MAX_PATH, _T("%s\\ch_crashdump-%s-%I64u-%s.dmp"), (PCTSTR)strPath, _T(PRODUCT_VERSION), (ull_t)_time64(NULL),
#ifdef _WIN64
		_T("64")
#else
		_T("32")
#endif
		);
	szPath[_MAX_PATH - 1] = _T('\0');

	// Step 2 - create the crash dump in case anything happens later
	bool bResult = false;
	HANDLE hFile = CreateFile(szName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hFile != INVALID_HANDLE_VALUE)
	{
		MINIDUMP_EXCEPTION_INFORMATION mei;
		mei.ThreadId = GetCurrentThreadId();
		mei.ExceptionPointers = ExceptionInfo;
		mei.ClientPointers = TRUE;

		if(MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hFile, MiniDumpWithProcessThreadData, &mei, NULL, NULL))
			bResult = true;
	}

	CloseHandle(hFile);

	CCrashDlg dlgCrash(bResult, szName);
	dlgCrash.DoModal();

	return EXCEPTION_EXECUTE_HANDLER;
}

BOOL CCopyHandlerApp::InitInstance()
{
	// ================================= Crash handling =======================================
	SetUnhandledExceptionFilter(&MyUnhandledExceptionFilter);

	// ================================= Configuration ========================================
	CString strPath;
	CString strCfgPath;
	CString strLogPath;

	// note that the GetProgramDataPath() below should create a directory; ExpandPath() could
	// depend on the directory to be created earlier
	if(!GetProgramDataPath(strPath))
	{
		AfxMessageBox(_T("Cannot initialize Copy Handler (data path cannot be established)."), MB_ICONERROR | MB_OK);
		return FALSE;
	}

	strCfgPath = strPath + _T("\\ch.ini");

	// initialize configuration file
	m_config.set_callback(ConfigPropertyChangedCallback, NULL);

	// read the configuration
	try
	{
		m_config.read(strCfgPath);
	}
	catch(...)
	{
	}

	// set working dir for the engine
	icpf::config& rConfig = GetConfig();

//	rConfig.SetBasePath(strPath);
	// register all properties
	RegisterProperties(&rConfig);

	// ================================= Logging ========================================
	// initialize the global log file if it is requested by configuration file
	strLogPath = strPath +  + _T("\\ch.log");

	chcore::TLogger& rLogger = chcore::TLogger::Acquire();
	try
	{
		rLogger.init(strLogPath, (int_t)m_config.get_signed_num(PP_LOGMAXSIZE), (int_t)rConfig.get_unsigned_num(PP_LOGLEVEL), false, false);
		rLogger.Enable(m_config.get_bool(PP_LOGENABLELOGGING));
	}
	catch(...)
	{
		BOOST_ASSERT(false);
	}

	LOG_INFO(_T("============================ Initializing Copy Handler ============================"));
	LOG_INFO(_T(""));

	// ================================= COM ========================================
	LOG_INFO(_T("Initializing COM"));

	HRESULT hResult = CoInitializeEx(NULL, COINIT_MULTITHREADED);
	if(FAILED(hResult))
	{
		CString strMsg;
		strMsg.Format(_T("Cannot initialize COM, the application will now exit (result = 0x%lx)"), hResult);

		LOG_ERROR(strMsg);
		AfxMessageBox(strMsg, MB_ICONERROR | MB_OK);
		return FALSE;
	}

	// ================================= Resource manager ========================================
	LOG_INFO(_T("Initializing resource manager..."));

	ictranslate::CResourceManager& rResManager = ictranslate::CResourceManager::Acquire();

	// set current language
	TCHAR szPath[_MAX_PATH];

	rResManager.Init(AfxGetInstanceHandle());
	rResManager.SetCallback(ResManCallback);
	rConfig.get_string(PP_PLANGUAGE, szPath, _MAX_PATH);
	TRACE(_T("Help path=%s\n"), szPath);
	if(!rResManager.SetLanguage(ExpandPath(szPath)))
	{
		TCHAR szData[2048];
		_sntprintf(szData, 2048, _T("Couldn't find the language file specified in configuration file:\n%s\nPlease correct this path to point the language file to use.\nProgram will now exit."), szPath);
		LOG_ERROR(szData);
		AfxMessageBox(szData, MB_ICONSTOP | MB_OK);
		return FALSE;
	}

	UpdateHelpPaths();

	// for dialogs
	ictranslate::CLanguageDialog::SetResManager(&rResManager);

	EnableHtmlHelp();

	// ================================= Checking for running instances of CH ========================================
	// check instance - return false if it's the second one
	LOG_INFO(_T("Checking for other running instances of Copy Handler"));
	if(!IsFirstInstance())
	{
		LOG_WARNING(_T("Other instance of Copy Handler is already running. Exiting."));
		MsgBox(IDS_ONECOPY_STRING);
		return FALSE;
	}

	// ================================= Common controls ========================================
	LOG_INFO(_T("Initializing GUI common controls"));

	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	// ================================= Shell extension ========================================
	LOG_INFO(_T("Initializing shared memory for communication with shell extension"));

	m_hMapObject = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(CSharedConfigStruct), _T("CHLMFile"));
	if (m_hMapObject == NULL)
		return FALSE; 
	
	// Get a pointer to the file-mapped shared memory.
	g_pscsShared=(CSharedConfigStruct*)MapViewOfFile(m_hMapObject, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0);
	if (g_pscsShared == NULL) 
		return FALSE; 

	LOG_INFO(_T("Checking shell extension compatibility"));

	// calculate ch version
	long lCHVersion = PRODUCT_VERSION1 << 24 | PRODUCT_VERSION2 << 16 | PRODUCT_VERSION3 << 8 | PRODUCT_VERSION4;

	// check the version of shell extension
	LONG lVersion = 0;
	BSTR bstrVersion = NULL;

	hResult = CoCreateInstance(CLSID_CShellExtControl, NULL, CLSCTX_ALL, IID_IShellExtControl, (void**)&m_piShellExtControl);
	if(SUCCEEDED(hResult) && !m_piShellExtControl)
		hResult = E_FAIL;
	if(SUCCEEDED(hResult))
		hResult = m_piShellExtControl->GetVersion(&lVersion, &bstrVersion);
	if(SUCCEEDED(hResult) && lVersion == lCHVersion)
		hResult = m_piShellExtControl->SetFlags(eShellExt_Enabled, eShellExt_Enabled);
	if(FAILED(hResult) || lCHVersion != lVersion)
	{
		CString strMsg;
		strMsg.Format(_T("Shell extension has different version (0x%lx) than Copy Handler (0x%lx). Shell extension will be disabled."), lVersion, lCHVersion);

		LOG_WARNING(strMsg);
		MsgBox(IDS_SHELL_EXTENSION_MISMATCH_STRING);

		if(m_piShellExtControl)
			m_piShellExtControl->SetFlags(0, eShellExt_Enabled);
	}

	if(bstrVersion)
		::SysFreeString(bstrVersion);

	// ================================= Initial settings ========================================
	LOG_INFO(_T("Applying initial settings"));

	// set this process priority class
	HANDLE hProcess=GetCurrentProcess();
	::SetPriorityClass(hProcess, (DWORD)rConfig.get_signed_num(PP_PPROCESSPRIORITYCLASS));

#ifndef _DEBUG		// for easier writing the program - doesn't collide with std CH
	// set "run with system" registry settings
	SetAutorun(rConfig.get_bool(PP_PRELOADAFTERRESTART));
#endif

	// ================================= Main window ========================================
	LOG_INFO(_T("Creating main application window"));
	// create main window
	m_pMainWindow=new CMainWnd;
	if (!((CMainWnd*)m_pMainWindow)->Create())
		return FALSE;				// will be deleted at destructor

	m_pMainWnd = m_pMainWindow;
	CWinApp::InitInstance();

	LOG_INFO(_T("Copy Handler initialized successfully"));

	return TRUE;
}

bool CCopyHandlerApp::IsShellExtEnabled() const
{
	if(m_piShellExtControl)
	{
		LONG lFlags = 0;
		HRESULT hResult = m_piShellExtControl->GetFlags(&lFlags);
		if(SUCCEEDED(hResult) && (lFlags & eShellExt_Enabled))
			return true;
	}
	return false;
}

void CCopyHandlerApp::OnConfigNotify(uint_t uiPropID)
{
	// is this language
	switch(uiPropID)
	{
	case PP_PLANGUAGE:
		{
			// update language in resource manager
			TCHAR szPath[_MAX_PATH];
			GetConfig().get_string(PP_PLANGUAGE, szPath, _MAX_PATH);
			GetResManager().SetLanguage(ExpandPath(szPath));
			break;
		}
	case PP_LOGENABLELOGGING:
		{
			chcore::TLogger& rLogger = chcore::TLogger::Acquire();

			rLogger.Enable(GetConfig().get_bool(PP_LOGENABLELOGGING));
			break;
		}
	case PP_LOGLEVEL:
		{
			chcore::TLogger& rLogger = chcore::TLogger::Acquire();

			rLogger.set_log_level((int_t)GetConfig().get_unsigned_num(PP_LOGLEVEL));
			break;
		}
	case PP_LOGMAXSIZE:
		{
			chcore::TLogger& rLogger = chcore::TLogger::Acquire();

			rLogger.set_max_size((int_t)GetConfig().get_signed_num(PP_LOGMAXSIZE));
			break;
		}
	}
}

void CCopyHandlerApp::OnResManNotify(UINT uiType)
{
	if (uiType == RMNT_LANGCHANGE)
	{
		// language has been changed - close the current help file
		if (UpdateHelpPaths())
			HtmlHelp(NULL, HH_CLOSE_ALL);
	}
}

HWND CCopyHandlerApp::HHelp(HWND hwndCaller, LPCTSTR pszFile, UINT uCommand, DWORD_PTR dwData)
{
	PCTSTR pszPath=NULL;
	WIN32_FIND_DATA wfd;
	HANDLE handle=::FindFirstFile(m_pszHelpFilePath, &wfd);
	if (handle != INVALID_HANDLE_VALUE)
	{
		pszPath=m_pszHelpFilePath;
		::FindClose(handle);
	}

	if (pszPath == NULL)
		return NULL;

	if (pszFile != NULL)
	{
		TCHAR szAdd[2*_MAX_PATH];
		_tcscpy(szAdd, pszPath);
		_tcscat(szAdd, pszFile);
		return ::HtmlHelp(hwndCaller, szAdd, uCommand, dwData);
	}
	else
		return ::HtmlHelp(hwndCaller, pszPath, uCommand, dwData);
}

void CCopyHandlerApp::HtmlHelp(DWORD_PTR dwData, UINT nCmd)
{
	switch (nCmd)
	{
	case HH_DISPLAY_TOPIC:
	case HH_HELP_CONTEXT:
		{
			HHelp(GetDesktopWindow(), NULL, nCmd, dwData);
			break;
		}
	case HH_CLOSE_ALL:
		::HtmlHelp(NULL, NULL, HH_CLOSE_ALL, NULL);
		break;
	case HH_DISPLAY_TEXT_POPUP:
		{
			HELPINFO* pHelp=(HELPINFO*)dwData;
			if ( pHelp->dwContextId == 0 || pHelp->iCtrlId == 0
				|| ::GetWindowContextHelpId((HWND)pHelp->hItemHandle) == 0)
				return;

			HH_POPUP hhp;
			hhp.cbStruct=sizeof(HH_POPUP);
			hhp.hinst=NULL;
			hhp.idString=(pHelp->dwContextId & 0xffff);
			hhp.pszText=NULL;
			hhp.pt=pHelp->MousePos;
			hhp.pt.y+=::GetSystemMetrics(SM_CYCURSOR)/2;
			hhp.clrForeground=(COLORREF)-1;
			hhp.clrBackground=(COLORREF)-1;
			hhp.rcMargins.left=-1;
			hhp.rcMargins.right=-1;
			hhp.rcMargins.top=-1;
			hhp.rcMargins.bottom=-1;
			hhp.pszFont=_T("Tahoma, 8, , ");

			TCHAR szPath[_MAX_PATH];
			_sntprintf(szPath, _MAX_PATH, _T("::/%lu.txt"), (pHelp->dwContextId >> 16) & 0x7fff);
			HHelp(GetDesktopWindow(), szPath, HH_DISPLAY_TEXT_POPUP, (DWORD)&hhp);

			break;
		}
	}
}

int CCopyHandlerApp::ExitInstance()
{
	LOG_INFO(_T("Pre-exit step - releasing shell extension"));
	if(m_piShellExtControl)
	{
		m_piShellExtControl->Release();
		m_piShellExtControl = NULL;
	}

	LOG_INFO(_T("Pre-exit step - uninitializing COM"));
	CoUninitialize();

	LOG_INFO(_T("============================ Leaving Copy Handler ============================"));

	return __super::ExitInstance();
}
