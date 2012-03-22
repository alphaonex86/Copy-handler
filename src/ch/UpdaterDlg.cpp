// UpdaterDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ch.h"
#include "UpdaterDlg.h"
#include "UpdateChecker.h"
#include "../common/version.h"

#define UPDATER_TIMER 639

BEGIN_MESSAGE_MAP(CUpdaterDlg, ictranslate::CLanguageDialog)
	ON_BN_CLICKED(IDC_OPEN_WEBPAGE_BUTTON, &CUpdaterDlg::OnBnClickedOpenWebpageButton)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CUpdaterDlg dialog

IMPLEMENT_DYNAMIC(CUpdaterDlg, ictranslate::CLanguageDialog)

CUpdaterDlg::CUpdaterDlg(bool bBackgroundMode, CWnd* pParent /*=NULL*/) :
	ictranslate::CLanguageDialog(CUpdaterDlg::IDD, pParent),
	m_eLastState(CUpdateChecker::eResult_Undefined),
	m_bBackgroundMode(bBackgroundMode)
{
}

CUpdaterDlg::~CUpdaterDlg()
{
	m_ucChecker.Cleanup();
}

void CUpdaterDlg::DoDataExchange(CDataExchange* pDX)
{
	ictranslate::CLanguageDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_INFO_STATIC, m_ctlText);
}

BOOL CUpdaterDlg::OnInitDialog()
{
	ictranslate::CLanguageDialog::OnInitDialog();

	ictranslate::CFormat fmt(GetResManager().LoadString(IDS_UPDATER_WAITING_STRING));
	fmt.SetParam(_t("%site"), _T(PRODUCT_SITE));
	m_ctlText.SetWindowText(fmt);

	// disable button initially
	CWnd* pWnd = GetDlgItem(IDC_OPEN_WEBPAGE_BUTTON);
	if(pWnd)
		pWnd->EnableWindow(FALSE);

	if(!m_bBackgroundMode)
		ShowWindow(SW_SHOW);

	// start the updater
	m_ucChecker.AsyncCheckForUpdates(_T(PRODUCT_SITE), GetConfig().get_bool(PP_PUPDATE_CHECK_FOR_BETA), m_bBackgroundMode);

	// start a timer to display progress
	SetTimer(UPDATER_TIMER, 10, NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CUpdaterDlg::OnBnClickedOpenWebpageButton()
{
	CString strDownloadAddr = m_ucChecker.GetDownloadAddress();
	if(!strDownloadAddr.IsEmpty())
	{
		CString str;
		str.Format(_T("Opening a browser with address %s..."), (PCTSTR)strDownloadAddr);
		LOG_DEBUG(str);

		str.Format(_T("url.dll,FileProtocolHandler %s"), (PCTSTR)strDownloadAddr);
		ulong_t ulRes = (ulong_t)ShellExecute(NULL, _T("open"), _T("rundll32.exe"), str, NULL, SW_SHOW);

		str.Format(_T("ShellExecute returned %lu"), ulRes);
		LOG_DEBUG(str);

		// close the dialog if succeeded; 32 is some arbitrary value from ms docs
		if(ulRes > 32)
			CUpdaterDlg::OnOK();
	}
}

void CUpdaterDlg::OnTimer(UINT_PTR nIDEvent)
{
	if(nIDEvent == UPDATER_TIMER)
	{
		ictranslate::CResourceManager& rResManager = GetResManager();
		ictranslate::CFormat fmt;
		CUpdateChecker::ECheckResult eResult = m_ucChecker.GetResult();
		CString strFmt;
		EBkModeResult eBkMode = eRes_None;
		bool bEnableButton = false;

		if(eResult != m_eLastState)
		{
			switch(m_ucChecker.GetResult())
			{
			case CUpdateChecker::eResult_Undefined:
				TRACE(_T("CUpdateChecker::eResult_Undefined\n"));
				eBkMode = eRes_Exit;
				strFmt = rResManager.LoadString(IDS_UPDATER_WAITING_STRING);
				break;
			case CUpdateChecker::eResult_Pending:
				TRACE(_T("CUpdateChecker::eResult_Pending\n"));
				strFmt = rResManager.LoadString(IDS_UPDATER_WAITING_STRING);
				break;
			case CUpdateChecker::eResult_Killed:
				TRACE(_T("CUpdateChecker::eResult_Killed\n"));
				eBkMode = eRes_Exit;
				strFmt = rResManager.LoadString(IDS_UPDATER_ERROR_STRING);
				break;
			case CUpdateChecker::eResult_Error:
				TRACE(_T("CUpdateChecker::eResult_Error\n"));
				eBkMode = eRes_Exit;
				strFmt = rResManager.LoadString(IDS_UPDATER_ERROR_STRING);
				break;
			case CUpdateChecker::eResult_RemoteVersionOlder:
				TRACE(_T("CUpdateChecker::eResult_RemoteVersionOlder\n"));
				eBkMode = eRes_Exit;
				bEnableButton = true;
//				eBkMode = eRes_Show;		// for debugging purposes only
				strFmt = rResManager.LoadString(IDS_UPDATER_OLD_VERSION_STRING);
				break;
			case CUpdateChecker::eResult_VersionCurrent:
				TRACE(_T("CUpdateChecker::eResult_VersionCurrent\n"));
				eBkMode = eRes_Exit;
				bEnableButton = true;
				strFmt = rResManager.LoadString(IDS_UPDATER_EQUAL_VERSION_STRING);
				break;
			case CUpdateChecker::eResult_RemoteVersionNewer:
				TRACE(_T("CUpdateChecker::eResult_RemoteVersionNewer\n"));
				eBkMode = eRes_Show;
				bEnableButton = true;
				strFmt = rResManager.LoadString(IDS_UPDATER_NEW_VERSION_STRING);
				break;
			default:
				_ASSERTE(FALSE);
				eBkMode = eRes_Exit;
				return;
			}

			fmt.SetFormat(strFmt);
			fmt.SetParam(_t("%site"), _t(PRODUCT_SITE));
			fmt.SetParam(_t("%errdesc"), m_ucChecker.GetLastError());
			fmt.SetParam(_t("%thisver"), _T(PRODUCT_VERSION));
			fmt.SetParam(_t("%officialver"), m_ucChecker.GetReadableVersion());

			m_ctlText.SetWindowText(fmt);

			// Update button state
			CWnd* pWnd = GetDlgItem(IDC_OPEN_WEBPAGE_BUTTON);
			if(pWnd)
				pWnd->EnableWindow(bEnableButton);

			m_eLastState = eResult;

			// handle background mode
			if(m_bBackgroundMode)
			{
				switch(eBkMode)
				{
				case eRes_None:
					break;
				case eRes_Exit:
					KillTimer(UPDATER_TIMER);
					EndDialog(IDCANCEL);
					return;
				case eRes_Show:
					ShowWindow(SW_SHOW);
					break;
				default:
					BOOST_ASSERT(FALSE);
				}
			}
		}
	}

	CLanguageDialog::OnTimer(nIDEvent);
}
