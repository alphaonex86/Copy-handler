#pragma once
#include "UpdateChecker.h"

// CUpdaterDlg dialog

class CUpdaterDlg : public ictranslate::CLanguageDialog
{
	DECLARE_DYNAMIC(CUpdaterDlg)

	enum EBkModeResult
	{
		eRes_None,
		eRes_Exit,
		eRes_Show
	};
public:
	CUpdaterDlg(bool bBackgroundMode, CWnd* pParent = NULL);   // standard constructor
	virtual ~CUpdaterDlg();

	// Dialog Data
	enum { IDD = IDD_UPDATER_DIALOG };

	virtual BOOL OnInitDialog();

	afx_msg void OnBnClickedOpenWebpageButton();
	afx_msg void OnTimer(UINT_PTR nIDEvent);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

protected:
	CStatic m_ctlText;
	CUpdateChecker m_ucChecker;
	CUpdateChecker::ECheckResult m_eLastState;
	bool m_bBackgroundMode;		///< Do we operate in standard mode (false), or in background mode (true)
};
