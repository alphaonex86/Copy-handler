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
/// @file UpdateChecker.cpp
/// @date 2009/04/18
/// @brief Contains an implementation of update checker class.
// ============================================================================
#include "stdafx.h"
#include "UpdateChecker.h"
#include <afxinet.h>
#include <atlconv.h>
#include "../common/version.h"
#include "../libicpf/cfg.h"
#include "../libicpf/exception.h"
#include "../libicpf/circ_buffer.h"

// timeout used with waiting for events (avoiding hangs)
#define FORCE_TIMEOUT 60000

// ============================================================================
/// CAsyncHttpFile::CAsyncHttpFile
/// @date 2009/04/18
///
/// @brief     Constructs the CAsyncHttpFile object.
// ============================================================================
CAsyncHttpFile::CAsyncHttpFile() :
	m_hInternet(NULL),
	m_hOpenUrl(NULL),
	m_dwExpectedState(0),
	m_hFinishedEvent(NULL),
	m_dwError(ERROR_SUCCESS)
{
	memset(&m_internetBuffers, 0, sizeof(INTERNET_BUFFERS));

	m_tOpenRequest.pHttpFile = this;
	m_tOpenRequest.eOperationType = CONTEXT_REQUEST::eInternetOpenUrl;

	m_tReadRequest.pHttpFile = this;
	m_tReadRequest.eOperationType = CONTEXT_REQUEST::eInternetReadFileEx;
}

// ============================================================================
/// CAsyncHttpFile::~CAsyncHttpFile
/// @date 2009/04/18
///
/// @brief     Destructs the CASyncHttpFile object.
// ============================================================================
CAsyncHttpFile::~CAsyncHttpFile()
{
	Close();
}

// ============================================================================
/// CAsyncHttpFile::Open
/// @date 2009/04/18
///
/// @brief     Opens the specified internet address (starts those operations).
/// @param[in] pszPath		Url to be opened (full path to file).
/// @return    S_OK if opened, S_FALSE if wait for result is needed, E_* for errors.
// ============================================================================
HRESULT CAsyncHttpFile::Open(const tchar_t* pszPath)
{
	if(!pszPath)
	{
		SetErrorCode(ERROR_INTERNAL_ERROR);
		return E_INVALIDARG;
	}

	if(m_hInternet || m_hFinishedEvent)
	{
		SetErrorCode(ERROR_INTERNAL_ERROR);
		return E_FAIL;
	}

	// reset error code
	SetErrorCode(ERROR_SUCCESS);

	// create event
	m_hFinishedEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);
	if(!m_hFinishedEvent)
	{
		SetErrorCode(ERROR_INTERNAL_ERROR);
		return E_FAIL;
	}

	m_hInternet = ::InternetOpen(_T(PRODUCT_NAME), INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, INTERNET_FLAG_ASYNC);
	if(!m_hInternet)
	{
		SetErrorCode(GetLastError());

		::CloseHandle(m_hFinishedEvent);
		m_hFinishedEvent = NULL;

		return E_FAIL;
	}

	if(::InternetSetStatusCallback(m_hInternet, (INTERNET_STATUS_CALLBACK)&CAsyncHttpFile::InternetStatusCallback) == INTERNET_INVALID_STATUS_CALLBACK)
	{
		SetErrorCode(GetLastError());

		::InternetCloseHandle(m_hInternet);
		::CloseHandle(m_hFinishedEvent);

		m_hFinishedEvent = NULL;
		return E_FAIL;
	}

	m_dwExpectedState = INTERNET_STATUS_REQUEST_COMPLETE;
	HINTERNET hOpenUrl = ::InternetOpenUrl(m_hInternet, pszPath, NULL, 0, INTERNET_FLAG_NO_COOKIES | INTERNET_FLAG_NO_UI | INTERNET_FLAG_PRAGMA_NOCACHE | INTERNET_FLAG_RELOAD, (DWORD_PTR)&m_tOpenRequest);
	if(!hOpenUrl)
	{
		SetErrorCode(::GetLastError());
		if(GetErrorCode() != ERROR_IO_PENDING)
		{
			::InternetSetStatusCallback(m_hInternet, NULL);
			::InternetCloseHandle(m_hInternet);
			::CloseHandle(m_hFinishedEvent);

			m_hInternet = NULL;
			m_hFinishedEvent = NULL;
			m_dwExpectedState = 0;

			return E_FAIL;
		}
	}
	else
	{
		m_dwExpectedState = 0;		// everything has been completed
		::SetEvent(m_hFinishedEvent);
	}

	return hOpenUrl ? S_OK : S_FALSE;
}

// ============================================================================
/// CAsyncHttpFile::GetFileSize
/// @date 2009/04/18
///
/// @brief     Retrieves the size of file opened with CAsyncHttpFile::Open()
//				(if such information exists in http headers).
/// @param[out] stSize  Receives the size of file (receives 65536 if http headers
///                     did not contain the information).
/// @return		Result of the operation.
// ============================================================================
HRESULT CAsyncHttpFile::GetFileSize(size_t& stSize)
{
	if(!m_hInternet || !m_hOpenUrl)
	{
		SetErrorCode(ERROR_INTERNAL_ERROR);
		return E_FAIL;
	}

	DWORD dwContentLengthSize = sizeof(DWORD);
	if(!HttpQueryInfo(m_hOpenUrl, HTTP_QUERY_CONTENT_LENGTH | HTTP_QUERY_FLAG_NUMBER, &stSize, &dwContentLengthSize, NULL) || stSize == 0 || stSize > 1*1024UL*1024UL)
	{
		stSize = 65536;		// safe fallback
		return S_FALSE;
	}

	return S_OK;
}

// ============================================================================
/// CAsyncHttpFile::RequestData
/// @date 2009/04/18
///
/// @brief     Requests the data from already opened url.
/// @param[in]	pBuffer  Buffer for the data.
/// @param[in]  stSize   Buffer size.
/// @return	   S_OK if completed, S_FALSE if needs waiting, E_* on error.
// ============================================================================
HRESULT CAsyncHttpFile::RequestData(void* pBuffer, size_t stSize)
{
	if(!pBuffer)
	{
		SetErrorCode(ERROR_INTERNAL_ERROR);
		return E_INVALIDARG;
	}
	if(!m_hInternet || !m_hOpenUrl || !m_hFinishedEvent)
	{
		SetErrorCode(ERROR_INTERNAL_ERROR);
		return E_FAIL;
	}

	SetErrorCode(ERROR_SUCCESS);

	if(!::ResetEvent(m_hFinishedEvent))
	{
		SetErrorCode(ERROR_INTERNAL_ERROR);
		return E_FAIL;
	}

	memset(&m_internetBuffers, 0, sizeof(INTERNET_BUFFERS));
	m_internetBuffers.dwStructSize = sizeof(INTERNET_BUFFERS);
	m_internetBuffers.dwBufferLength = stSize;
	m_internetBuffers.dwBufferTotal = stSize;
	m_internetBuffers.lpvBuffer = pBuffer;

	m_dwExpectedState = INTERNET_STATUS_REQUEST_COMPLETE;
	if(!::InternetReadFileEx(m_hOpenUrl, &m_internetBuffers, IRF_NO_WAIT, (DWORD_PTR)&m_tReadRequest))
	{
		SetErrorCode(::GetLastError());
		if(GetErrorCode() == ERROR_IO_PENDING)
			return S_FALSE;
		else
			return E_FAIL;
	}

	if(!::SetEvent(m_hFinishedEvent))
	{
		SetErrorCode(ERROR_INTERNAL_ERROR);
		return E_FAIL;
	}

	return S_OK;
}

// ============================================================================
/// CAsyncHttpFile::RetrieveRequestedData
/// @date 2009/04/18
///
/// @brief     Retrieves the size of data retrieved.
/// @param[out] stSize  Receives the size of data read from file.
/// @return    Result of the operation.
// ============================================================================
HRESULT CAsyncHttpFile::GetRetrievedDataSize(size_t& stSize)
{
	if(!m_hInternet)
	{
		SetErrorCode(ERROR_INTERNAL_ERROR);
		return E_FAIL;
	}

	stSize = m_internetBuffers.dwBufferLength;
	return S_OK;
}

// ============================================================================
/// CAsyncHttpFile::Close
/// @date 2009/04/18
///
/// @brief     Closes the file.
/// @return    Result of the operation.
// ============================================================================
HRESULT CAsyncHttpFile::Close()
{
	SetErrorCode(ERROR_SUCCESS);
	if(m_hOpenUrl)
	{
		m_dwExpectedState = INTERNET_STATUS_CLOSING_CONNECTION;
		if(!::InternetCloseHandle(m_hOpenUrl))
		{
			SetErrorCode(::GetLastError());
			if(GetErrorCode() == ERROR_IO_PENDING)
				return S_FALSE;
			else
			{
				SetErrorCode(ERROR_INTERNAL_ERROR);
				return E_FAIL;
			}
		}

		// if closing url handle succeeded, we close internet here, if not
		// then a separate call to close need to be performed.
		m_dwExpectedState = 0;
		SetUrlHandle(NULL);
		::InternetCloseHandle(m_hInternet);
	}

	if(m_hFinishedEvent)
	{
		::CloseHandle(m_hFinishedEvent);
		m_hFinishedEvent = NULL;
	}

	return S_OK;
}

// ============================================================================
/// CAsyncHttpFile::GetResult
/// @date 2009/04/18
///
/// @brief     Retrieves the last call result (blocking call).
/// @return    Result of the last call.
// ============================================================================
CAsyncHttpFile::EWaitResult CAsyncHttpFile::GetResult()
{
	HANDLE hHandles[] = { m_hFinishedEvent };
	DWORD dwEffect = WaitForMultipleObjects(1, hHandles, FALSE, 0);
	if(dwEffect == WAIT_OBJECT_0 + 0 || dwEffect == WAIT_ABANDONED_0 + 0)
		return GetErrorCode() == ERROR_SUCCESS ? CAsyncHttpFile::eFinished : CAsyncHttpFile::eError;
	else
		return CAsyncHttpFile::ePending;
}

// ============================================================================
/// CAsyncHttpFile::WaitForResult
/// @date 2009/04/18
///
/// @brief     Waits for the result with additional 'kill' event.
/// @param[in] hKillEvent  Event handle that would break waiting for result.
/// @return    Result of waiting.
// ============================================================================
CAsyncHttpFile::EWaitResult CAsyncHttpFile::WaitForResult(HANDLE hKillEvent)
{
	HANDLE hHandles[] = { hKillEvent, m_hFinishedEvent };
	DWORD dwEffect = WaitForMultipleObjects(2, hHandles, FALSE, FORCE_TIMEOUT);
	if(dwEffect == 0xffffffff)
	{
		SetErrorCode(::GetLastError());
		return CAsyncHttpFile::eError;
	}
	else if(dwEffect == WAIT_OBJECT_0 + 0 || dwEffect == WAIT_ABANDONED_0 + 0)
		return CAsyncHttpFile::eKilled;
	else if(dwEffect == WAIT_OBJECT_0 + 1 || dwEffect == WAIT_ABANDONED_0 + 1)
		return GetErrorCode() == ERROR_SUCCESS ? CAsyncHttpFile::eFinished : CAsyncHttpFile::eError;
	else
		return CAsyncHttpFile::eTimeout;
}

// ============================================================================
/// CAsyncHttpFile::InternetStatusCallback
/// @date 2009/04/18
///
/// @brief     Callback for use with internet API.
/// @param[in] hInternet				Internet handle.
/// @param[in] dwContext				Context value.
/// @param[in] dwInternetStatus			Internet status.
/// @param[in] lpvStatusInformation		Additional status information.
/// @param[in] dwStatusInformationLength Length of lpvStatusInformation.
// ============================================================================
void CALLBACK CAsyncHttpFile::InternetStatusCallback(HINTERNET hInternet, DWORD_PTR dwContext, DWORD dwInternetStatus, LPVOID lpvStatusInformation, DWORD dwStatusInformationLength)
{
	CONTEXT_REQUEST* pRequest = (CONTEXT_REQUEST*)dwContext;
	BOOST_ASSERT(pRequest && pRequest->pHttpFile);
	if(!pRequest || !pRequest->pHttpFile)
		return;

	CString strMsg;
	strMsg.Format(_T("[CAsyncHttpFile::InternetStatusCallback] hInternet: %p, dwContext: %lu (operation: %lu), dwInternetStatus: %lu, lpvStatusInformation: %p, dwStatusInformationLength: %lu\n"),
		hInternet, dwContext, pRequest ? pRequest->eOperationType : CONTEXT_REQUEST::eNone, dwInternetStatus, lpvStatusInformation, dwStatusInformationLength);
	LOG_DEBUG(strMsg);

	switch(dwInternetStatus)
	{
	case INTERNET_STATUS_HANDLE_CREATED:
		{
			INTERNET_ASYNC_RESULT* pRes = (INTERNET_ASYNC_RESULT*)lpvStatusInformation;
			pRequest->pHttpFile->SetUrlHandle((HINTERNET)(pRes->dwResult));
			break;
		}
	case INTERNET_STATUS_RESPONSE_RECEIVED:
		{
			ATLTRACE(_T("INTERNET_STATUS_RESPONSE_RECEIVED; received %lu bytes."), *(DWORD*)lpvStatusInformation);
			break;
		}
	case INTERNET_STATUS_REQUEST_COMPLETE:
		{
			INTERNET_ASYNC_RESULT* pResult = (INTERNET_ASYNC_RESULT*)lpvStatusInformation;
			pRequest->pHttpFile->SetErrorCode(pResult->dwError);
			break;
		}
	case INTERNET_STATUS_CLOSING_CONNECTION:
		{
			pRequest->pHttpFile->SetUrlHandle(NULL);
			break;
		}
	case INTERNET_STATUS_CONNECTION_CLOSED:
		{
			break;
		}
	default:
		TRACE(_T("[CAsyncHttpFile::InternetStatusCallback()] Unhandled status: %lu\n"), dwInternetStatus);
	}

	pRequest->pHttpFile->SetCompletionStatus(dwInternetStatus);
}


// ============================================================================
/// CAsyncHttpFile::SetUrlHandle
/// @date 2009/04/18
///
/// @brief     Sets the url handle.
/// @param[in] hOpenUrl  Handle to be set.
// ============================================================================
void CAsyncHttpFile::SetUrlHandle(HANDLE hOpenUrl)
{
	m_hOpenUrl = hOpenUrl;
}

// ============================================================================
/// CAsyncHttpFile::SetErrorCode
/// @date 2009/04/18
///
/// @brief     Sets the error code.
/// @param[in] dwError  Error code to be set.
// ============================================================================
void CAsyncHttpFile::SetErrorCode(DWORD dwError)
{
	m_dwError = dwError;
}

// ============================================================================
/// CAsyncHttpFile::SetCompletionStatus
/// @date 2009/04/18
///
/// @brief     Sets the completion status.
/// @param[in] dwCurrentState  State to be set.
/// @return    Result of the operation.
// ============================================================================
HRESULT CAsyncHttpFile::SetCompletionStatus(DWORD dwCurrentState)
{
	if(!m_hFinishedEvent)
		return E_FAIL;

	if(dwCurrentState == m_dwExpectedState || dwCurrentState == INTERNET_STATUS_CLOSING_CONNECTION)
		return ::SetEvent(m_hFinishedEvent) ? S_OK : E_FAIL;
	return S_FALSE;
}

// ============================================================================
/// CUpdateChecker::CUpdateChecker
/// @date 2009/04/18
///
/// @brief     Constructs the update checker object.
// ============================================================================
CUpdateChecker::CUpdateChecker() :
	m_hThread(NULL),
	m_hKillEvent(NULL),
	m_eResult(eResult_Undefined),
	m_bCheckForBeta(false)
{
	m_hKillEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);
	BOOST_ASSERT(m_hKillEvent);
	::InitializeCriticalSection(&m_cs);
}

// ============================================================================
/// CUpdateChecker::~CUpdateChecker
/// @date 2009/04/18
///
/// @brief     Destroys the update checker object.
// ============================================================================
CUpdateChecker::~CUpdateChecker()
{
	Cleanup();

	::DeleteCriticalSection(&m_cs);
}

// ============================================================================
/// CUpdateChecker::AsyncCheckForUpdates
/// @date 2009/04/18
///
/// @brief     Starts the asynchronous checking for updates.
/// @param[in] pszSite	    Site where to search for updates (without file name).
/// @param[in] bCheckBeta   States if we are interested in beta products.
/// @return    True if operation started, false otherwise.
// ============================================================================
bool CUpdateChecker::AsyncCheckForUpdates(const tchar_t* pszSite, bool bCheckBeta, bool bOnlyIfConnected)
{
	if(!pszSite)
		return false;

	DWORD dwConnectionFlags = 0;

	if(bOnlyIfConnected && !InternetGetConnectedState(&dwConnectionFlags, 0))
		return false;

	m_strSite = pszSite;
	m_eResult = eResult_Undefined;
	m_bCheckForBeta = bCheckBeta;

	::ResetEvent(m_hKillEvent);

	m_hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&CUpdateChecker::UpdateCheckThread, (void*)this, 0, NULL);
	if(!m_hThread)
	{
		m_strSite.Empty();
		m_eResult = eResult_Undefined;
		m_bCheckForBeta = false;
		return false;
	}

	return true;
}

// ============================================================================
/// CUpdateChecker::Cleanup
/// @date 2009/04/18
///
/// @brief     Stops scanning for updates and clears the object.
// ============================================================================
void CUpdateChecker::Cleanup()
{
	if(m_hThread)
	{
		if(m_hKillEvent)
			::SetEvent(m_hKillEvent);
		WaitForSingleObject(m_hThread, 5000);
		if(m_hKillEvent)
			::CloseHandle(m_hKillEvent);
	}

	m_httpFile.Close();

	::EnterCriticalSection(&m_cs);
	m_hThread = NULL;
	m_hKillEvent = NULL;
	m_strSite.Empty();
	m_bCheckForBeta = false;
	m_strLastError.Empty();
	m_strNumericVersion.Empty();
	m_strReadableVersion.Empty();
	m_strDownloadAddress.Empty();
	m_eResult = CUpdateChecker::eResult_Undefined;
	::LeaveCriticalSection(&m_cs);
}

// ============================================================================
/// CUpdateChecker::SetResult
/// @date 2009/04/18
///
/// @brief     Sets the result of checking.
/// @param[in] eCheckResult  Result to be set.
/// @param[in] dwError       Error code (if any).
// ============================================================================
void CUpdateChecker::SetResult(ECheckResult eCheckResult, DWORD dwError)
{
	CString strError;

	if(eCheckResult == eResult_Error && dwError != 0)
	{
		PTSTR pszBuffer = strError.GetBufferSetLength(_MAX_PATH);
		FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, dwError, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), pszBuffer, _MAX_PATH, NULL);
		strError.ReleaseBuffer();

		if(strError.IsEmpty())
		{
			pszBuffer = strError.GetBufferSetLength(_MAX_PATH);
			FormatMessage(FORMAT_MESSAGE_FROM_HMODULE, GetModuleHandle(_T("wininet.dll")), dwError, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), pszBuffer, _MAX_PATH, NULL);
			strError.ReleaseBuffer();
		}

		if(strError.IsEmpty())
			strError.Format(_T("0x%lx"), dwError);
	}

	strError.TrimRight(_T("\r\n \t"));

	::EnterCriticalSection(&m_cs);
	
	m_eResult = eCheckResult;
	m_strLastError = strError;

	::LeaveCriticalSection(&m_cs);
}

// ============================================================================
/// CUpdateChecker::SetLastError
/// @date 2009/04/18
///
/// @brief     Sets last error.
/// @param[in] pszError  String containing the last error description.
// ============================================================================
void CUpdateChecker::SetLastError(PCTSTR pszError)
{
	::EnterCriticalSection(&m_cs);
	m_strLastError = pszError;
	::LeaveCriticalSection(&m_cs);
}

// ============================================================================
/// CUpdateChecker::SetVersionsAndAddress
/// @date 2009/04/18
///
/// @brief     Sets the download address and version information.
/// @param[in] pszAddress            Download address.
/// @param[in] pszNumericVersion     Numeric version number.
/// @param[in] pszReadableVersion    Human readable version number.
// ============================================================================
void CUpdateChecker::SetVersionsAndAddress(PCTSTR pszAddress, PCTSTR pszNumericVersion, PCTSTR pszReadableVersion)
{
	::EnterCriticalSection(&m_cs);
	m_strDownloadAddress = pszAddress;
	m_strNumericVersion = pszNumericVersion;
	m_strReadableVersion = pszReadableVersion;
	::LeaveCriticalSection(&m_cs);
}

// ============================================================================
/// CUpdateChecker::GetSiteAddress
/// @date 2009/04/18
///
/// @brief     Retrieves the address of a site to check the updates at.
/// @param[out] rstrAddress  Receives the address.
// ============================================================================
void CUpdateChecker::GetSiteAddress(CString& rstrAddress) const
{
	::EnterCriticalSection(&m_cs);
	rstrAddress = m_strSite;
	::LeaveCriticalSection(&m_cs);
}

// ============================================================================
/// CUpdateChecker::CheckForBeta
/// @date 2009/04/18
///
/// @brief     Returns information, if update should check for beta versions.
/// @return    True if beta versions should be processed, false otherwise.
// ============================================================================
bool CUpdateChecker::CheckForBeta()
{
	::EnterCriticalSection(&m_cs);
	bool bCheckForBeta = m_bCheckForBeta;
	::LeaveCriticalSection(&m_cs);

	return bCheckForBeta;
}

// ============================================================================
/// CUpdateChecker::GetResult
/// @date 2009/04/18
///
/// @brief     Retrieves the result of checking for updates.
/// @return    Check for updates result.
// ============================================================================
CUpdateChecker::ECheckResult CUpdateChecker::GetResult() const
{
	::EnterCriticalSection(&m_cs);
	ECheckResult eResult = m_eResult;
	::LeaveCriticalSection(&m_cs);
	return eResult;
}

// ============================================================================
/// CUpdateChecker::UpdateCheckThread
/// @date 2009/04/18
///
/// @brief     Main thread function.
/// @param[in] pParam  Pointer to the thread parameter (pointer to the CUpdateChecker object).
/// @return    Thread execution status.
// ============================================================================
DWORD CUpdateChecker::UpdateCheckThread(LPVOID pParam)
{
	CUpdateChecker* pUpdateChecker = (CUpdateChecker*)pParam;

	// mark as started
	pUpdateChecker->SetResult(eResult_Pending, 0);

	// get the real address of file to download
	CString strSite;
	pUpdateChecker->GetSiteAddress(strSite);
	strSite += _T("/chver.ini");

	CAsyncHttpFile::EWaitResult eWaitResult = CAsyncHttpFile::ePending;
	size_t stFileSize = 0;
	byte_t* pbyBuffer = NULL;
	icpf::circular_buffer circBuffer;

	// open the connection and try to get to the file
	HRESULT hResult = pUpdateChecker->m_httpFile.Open(strSite);
	if(SUCCEEDED(hResult))
	{
		eWaitResult = pUpdateChecker->m_httpFile.WaitForResult(pUpdateChecker->m_hKillEvent);
		switch(eWaitResult)
		{
		case CAsyncHttpFile::eFinished:
			break;
		case CAsyncHttpFile::eKilled:
			pUpdateChecker->SetResult(eResult_Killed, 0);
			return 1;
		case CAsyncHttpFile::eError:
			pUpdateChecker->SetResult(eResult_Error, pUpdateChecker->m_httpFile.GetErrorCode());
			return 1;
		case CAsyncHttpFile::eTimeout:
		case CAsyncHttpFile::ePending:
		default:
			pUpdateChecker->SetResult(eResult_Error, 0);
			return 1;
		}

		// get the file size
		hResult = pUpdateChecker->m_httpFile.GetFileSize(stFileSize);
	}

	if(SUCCEEDED(hResult))
	{
		bool bIsClosed = false;
		pbyBuffer = new byte_t[stFileSize];
		do 
		{
			hResult = pUpdateChecker->m_httpFile.RequestData(pbyBuffer, stFileSize);
			if(SUCCEEDED(hResult))
			{
				eWaitResult = pUpdateChecker->m_httpFile.WaitForResult(pUpdateChecker->m_hKillEvent);
				switch(eWaitResult)
				{
				case CAsyncHttpFile::eFinished:
					break;
				case CAsyncHttpFile::eKilled:
					pUpdateChecker->SetResult(eResult_Killed, 0);
					return 1;
					break;
				case CAsyncHttpFile::eError:
					pUpdateChecker->SetResult(eResult_Error, pUpdateChecker->m_httpFile.GetErrorCode());
					return 1;
				case CAsyncHttpFile::eTimeout:
				case CAsyncHttpFile::ePending:
				default:
					pUpdateChecker->SetResult(eResult_Error, 0);
					return 1;
				}
			}

			if(SUCCEEDED(hResult))
				hResult = pUpdateChecker->m_httpFile.GetRetrievedDataSize(stFileSize);

			if(SUCCEEDED(hResult) && stFileSize)
				circBuffer.push_data(pbyBuffer, stFileSize);

			bIsClosed = pUpdateChecker->m_httpFile.IsClosed();
		}
		while(stFileSize && !bIsClosed && SUCCEEDED(hResult));

		delete [] pbyBuffer;
	}

	if(FAILED(hResult))
	{
		pUpdateChecker->SetResult(eResult_Error, pUpdateChecker->m_httpFile.GetErrorCode());
		return 1;
	}

	pUpdateChecker->m_httpFile.Close();

	// convert text to unicode
	icpf::config cfg(icpf::config::eIni);
	const uint_t uiVersionNumeric = cfg.register_string(_t("Version/Numeric"), _t(""));
	const uint_t uiVersionReadable = cfg.register_string(_t("Version/Human Readable"), _t(""));
	const uint_t uiDownloadAddress = cfg.register_string(_t("Version/Download Address"), strSite);
	const uint_t uiBetaVersionNumeric = cfg.register_string(_t("Version/Numeric Beta"), _t(""));
	const uint_t uiBetaVersionReadable = cfg.register_string(_t("Version/Human Readable Beta"), _t(""));
	const uint_t uiBetaDownloadAddress = cfg.register_string(_t("Version/Download Address Beta"), strSite);
	try
	{
		cfg.read_from_buffer((TCHAR*)circBuffer.get_buffer(), (circBuffer.get_datasize() + 1) / 2);
	}
	catch(icpf::exception& e)
	{
		pUpdateChecker->SetResult(eResult_Error, 0);
		pUpdateChecker->SetLastError(e.get_desc());

		return 0xffffffff;
	}

	CString strVersionNumeric;
	bool bCheckForBeta = pUpdateChecker->CheckForBeta();
	if(bCheckForBeta)
	{
		strVersionNumeric = cfg.get_string(uiBetaVersionNumeric);
		pUpdateChecker->SetVersionsAndAddress(cfg.get_string(uiBetaDownloadAddress), strVersionNumeric, cfg.get_string(uiBetaVersionReadable));
	}

	if(!bCheckForBeta || strVersionNumeric.IsEmpty())
	{
		strVersionNumeric = cfg.get_string(uiVersionNumeric);
		pUpdateChecker->SetVersionsAndAddress(cfg.get_string(uiDownloadAddress), strVersionNumeric, cfg.get_string(uiVersionReadable));
	}

	// and compare to current version
	ushort_t usVer[4];
	if(_stscanf(strVersionNumeric, _t("%hu.%hu.%hu.%hu"), &usVer[0], &usVer[1], &usVer[2], &usVer[3]) != 4)
	{
		TRACE(_T("Error parsing retrieved version number."));
		pUpdateChecker->SetResult(eResult_Error, 0);
		return 0xffffffff;
	}

	ull_t ullCurrentVersion = ((ull_t)PRODUCT_VERSION1) << 48 | ((ull_t)PRODUCT_VERSION2) << 32 | ((ull_t)PRODUCT_VERSION3) << 16 | ((ull_t)PRODUCT_VERSION4);
	ull_t ullSiteVersion = ((ull_t)usVer[0]) << 48 | ((ull_t)usVer[1]) << 32 | ((ull_t)usVer[2]) << 16 | ((ull_t)usVer[3]);

	if(ullCurrentVersion < ullSiteVersion)
		pUpdateChecker->SetResult(eResult_RemoteVersionNewer, 0);
	else if(ullCurrentVersion == ullSiteVersion)
		pUpdateChecker->SetResult(eResult_VersionCurrent, 0);
	else
		pUpdateChecker->SetResult(eResult_RemoteVersionOlder, 0);

	return 0;
}
