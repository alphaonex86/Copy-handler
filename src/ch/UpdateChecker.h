// ============================================================================
//  Copyright (C) 2001-2009 by Jozef Starosczyk
//  ixen@copyhandler.com
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU Library General Public License
//  (version 2) as published by the Free Software Foundation;
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU Library General Public
//  License along with this program; if not, write to the
//  Free Software Foundation, Inc.,
//  59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
// ============================================================================
/// @file UpdateChecker.h
/// @date 2009/04/18
/// @brief Contains declaration of update checker class.
// ============================================================================
#ifndef __UPDATECHECKER_H__
#define __UPDATECHECKER_H__

class CAsyncHttpFile;

struct CONTEXT_REQUEST
{
	CAsyncHttpFile* pHttpFile;
	enum EOperation
	{
		eNone,
		eInternetOpenUrl = 1,
		eInternetReadFileEx = 2,
	} eOperationType;
};

class CAsyncHttpFile
{
public:
	enum EWaitResult
	{
		eKilled,
		eFinished,
		eTimeout,
		ePending,
		eError
	};

public:
	CAsyncHttpFile();
	~CAsyncHttpFile();

	HRESULT Open(const tchar_t* pszPath);
	HRESULT GetFileSize(size_t& stSize);

	HRESULT RequestData(void* pBuffer, size_t stSize);
	HRESULT GetRetrievedDataSize(size_t& stSize);

	HRESULT Close();

	EWaitResult GetResult();
	DWORD GetErrorCode() { return m_dwError; }

	EWaitResult WaitForResult(HANDLE hKillEvent);

	bool IsClosed() const { return m_hOpenUrl == NULL; }

protected:
	static void CALLBACK InternetStatusCallback(HINTERNET hInternet, DWORD_PTR dwContext, DWORD dwInternetStatus, LPVOID lpvStatusInformation, DWORD dwStatusInformationLength);

	void SetUrlHandle(HANDLE hOpenUrl);
	void SetErrorCode(DWORD dwError);

	/// Sets the completion event
	HRESULT SetCompletionStatus(DWORD dwCurrentState);

protected:
	HINTERNET m_hInternet;
	HINTERNET m_hOpenUrl;

	DWORD m_dwExpectedState;		///< State we are expecting
	HANDLE m_hFinishedEvent;

	INTERNET_BUFFERS m_internetBuffers;
	CONTEXT_REQUEST m_tOpenRequest;
	CONTEXT_REQUEST m_tReadRequest;


	DWORD m_dwError;
};

class CUpdateChecker : protected CInternetSession
{
public:
	enum ECheckResult
	{
		eResult_Undefined,
		eResult_Pending,
		eResult_Killed,
		eResult_Error,
		eResult_RemoteVersionOlder,
		eResult_VersionCurrent,
		eResult_RemoteVersionNewer
	};

public:
	/// Constructs the update checker object
	CUpdateChecker();
	/// Destructs the update checker object
	~CUpdateChecker();

	/// Starts the 'check for updates' thread
	bool AsyncCheckForUpdates(const tchar_t* pszSite, bool bCheckBeta, bool bOnlyIfConnected);

	/// Stops checking and cleanups the object
	void Cleanup();

	/// Retrieves the update result
	ECheckResult GetResult() const;

	// methods for retrieving state
	const tchar_t* GetNumericVersion() const { return (const tchar_t*)m_strNumericVersion; }
	const tchar_t* GetReadableVersion() const { return (const tchar_t*)m_strReadableVersion; }
	const tchar_t* GetLastError() const { return (const tchar_t*)m_strLastError; }
	const tchar_t* GetDownloadAddress() const { return m_strDownloadAddress; }

protected:
	/// Thread function (handles most of the internet connection operation)
	static DWORD WINAPI UpdateCheckThread(LPVOID pParam);

	/// Sets the result in mt-safe way
	void SetResult(ECheckResult eCheckResult, DWORD dwError);
	/// Sets the last error
	void SetLastError(PCTSTR pszError);
	/// Sets the versions and download address
	void SetVersionsAndAddress(PCTSTR pszAddress, PCTSTR pszNumericVersion, PCTSTR pszReadableVersion);
	/// Retrieves the site address
	void GetSiteAddress(CString& rstrAddress) const;

	/// Returns information if we're interested in beta versions
	bool CheckForBeta();

protected:
	CString m_strSite;
	bool m_bCheckForBeta;
	CString m_strLastError;
	CString m_strNumericVersion;
	CString m_strReadableVersion;
	CString m_strDownloadAddress;
	
	ECheckResult m_eResult;

	CAsyncHttpFile m_httpFile;
	HANDLE m_hThread;
	HANDLE m_hKillEvent;
	mutable CRITICAL_SECTION m_cs;
};

#endif
