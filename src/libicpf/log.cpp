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
/** \file log.cpp
 *  \brief Contains the implamentation of a log class.
 */
#include "log.h"
#include <boost/assert.hpp>
#include "exception.h"
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <assert.h>
#include "macros.h"

#if defined(_WIN32) || defined(_WIN64)
	#include <stdlib.h>
	#include <fcntl.h>
	#include <windows.h>
#else	
	#include <unistd.h>
#endif

#if defined(_WIN32) || defined(_WIN64)
	#define ENDL _t("\r\n")
#else
	#define ENDL _t("\n")
#endif

BEGIN_ICPF_NAMESPACE

/// Table of strings representing the log message types
const tchar_t* __logtype_str[] = { _t("debug"), _t("info"), _t("warning"), _t("error") };

/** Constructs a log_file object.
 * \param[in] bGlobal - states if this should be treates as a global instance of the log_file.
 *						Only one global log_file instance could exist in the application.
 */
log_file::log_file() : 
	m_pszPath(NULL),
	m_iMaxSize(262144),
	m_bLogStd(false),
	m_iLogLevel(level_debug),
	m_lock()
{
#ifdef WIN32
	_fmode=_O_BINARY;
#endif
}

/** Standard destructor
 */
log_file::~log_file()
{
	delete [] m_pszPath;
}

/** Initializes the constructed log file.
 * \param[in] pszPath - path to a log file to write to
 * \param[in] iMaxSize - maximum size of a log file
 * \param[in] iLogLevel - minimum log level of the messages to log
 * \param[in] bLogStd - log the messages also to stdout/stderr
 * \param[in] bClean - cleans the log file upon opening
 */
void log_file::init(const tchar_t* pszPath, int_t iMaxSize, int_t iLogLevel, bool bLogStd, bool bClean)
{
	// store the path and other params
	delete [] m_pszPath;
	m_pszPath=new tchar_t[_tcslen(pszPath)+1];
	_tcscpy(m_pszPath, pszPath);
	
	m_iMaxSize=iMaxSize;
	m_bLogStd=bLogStd;
	m_iLogLevel=iLogLevel;
	
	// try to open a file
	FILE* pFile=_tfopen(pszPath, bClean ? _t("w") : _t("a"));
	if (pFile == NULL)
		THROW(exception::format(_t("[log_file::init()] Could not open the specified file (") TSTRFMT _t(")")), 0, 0, 0);
	
	fclose(pFile);
}

// ============================================================================
/// icpf::log_file::is_initialized
/// @date 2009/05/19
///
/// @brief     Checks is the log_file object has been initialized.
/// @return    True if it has been initialized, false otherwise.
// ============================================================================
bool log_file::is_initialized() const throw()
{
	return m_pszPath != 0;
}

// ============================================================================
/// icpf::log_file::set_log_level
/// @date 2009/05/23
///
/// @brief     Changes the log level for this class.
/// @param[in] iLogLevel      New log level.
// ============================================================================
void log_file::set_log_level(int_t iLogLevel) throw()
{
	m_iLogLevel = iLogLevel;
}

// ============================================================================
/// icpf::log_file::set_max_size
/// @date 2009/05/23
///
/// @brief     Sets the max size of the log file.
/// @param[in] iMaxSize	Max size of the log file.
// ============================================================================
void log_file::set_max_size(int_t iMaxSize) throw()
{
	BOOST_ASSERT(iMaxSize > 0);
	if(iMaxSize > 0)
		m_iMaxSize = iMaxSize;
}

/** Retrieves the current size of a log file.
 *  Quite slow function - have to access the file by opening and closing it.
 * \return Current file size.
 */
int_t log_file::size() const
{
	assert(m_pszPath);
	if(!m_pszPath)
		return -1;
	
	int_t iSize=-1;
	FILE* pFile=_tfopen(m_pszPath, _t("r"));
	if (pFile != NULL)
	{
		if (fseek(pFile, 0, SEEK_END) == 0)
			iSize=ftell(pFile);

		fclose(pFile);
	}
	
	return iSize;
}

/** Truncates the current log file content so when adding some new text the
 *  file size won't exceed the maximum size specified in init().
 * \param[in] iAdd - size of the new string to be added to the log file
 * \return True if truncate succeeded or false if not.
 */
bool log_file::truncate(int_t iAdd) const
{
	assert(m_pszPath);
	if(!m_pszPath)
		return false;

	// if we doesn't need to truncate anything
	if (m_iMaxSize <= 0)
		return true;
	
	// make some checks
	int_t iSize=size();
	if (iSize <= 0 || iSize+iAdd < m_iMaxSize)
		return false;
	
	// establish the new file size (1/3rd of the current size or max_size-add_size)
	int_t iNewSize=minval((int_t)(iSize*0.66), m_iMaxSize-iAdd) & ~1;
	
#ifdef _WIN32
	// win32 does not have the ftruncate function, so we have to make some API calls
	HANDLE hFile=CreateFile(m_pszPath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile != INVALID_HANDLE_VALUE)
	{
		// seek
		if (SetFilePointer(hFile, iSize-iNewSize, NULL, FILE_BEGIN) != INVALID_SET_FILE_POINTER)
		{
			// read the string to the eol
			DWORD dwRD;
			tchar_t szBuffer[4096/sizeof(tchar_t)];
			if (ReadFile(hFile, szBuffer, 4096, &dwRD, NULL))
			{
				dwRD/=sizeof(tchar_t);
				szBuffer[(dwRD > 0) ? dwRD-1 : 0]=_t('\0');

				// replace the /r and /n in the log to the \0
				for (DWORD i=0;i<dwRD;i++)
				{
					if (szBuffer[i] == _t('\r') || szBuffer[i] == _t('\n'))
					{
						szBuffer[i]=_t('\0');
						break;
					}
				}

				iNewSize-=(int_t)(_tcslen(szBuffer)+1)*sizeof(tchar_t);			// new size correction

				if (SetFilePointer(hFile, iSize-iNewSize, NULL, FILE_BEGIN) != INVALID_SET_FILE_POINTER)
				{
					long_t lSrc=(long_t)SetFilePointer(hFile, 0, NULL, FILE_CURRENT);
					long_t lDst=0;
					DWORD tRD, tWR;

					do
					{
						// seek to src
						SetFilePointer(hFile, lSrc, NULL, FILE_BEGIN);

						// read 4k chars from source offset
						if (ReadFile(hFile, szBuffer, 4096, &tRD, NULL))
						{
							// seek to the dst
							SetFilePointer(hFile, lDst, NULL, FILE_BEGIN);

							FlushFileBuffers(hFile);
							// write the buffer to the dest offset
							WriteFile(hFile, szBuffer, tRD, &tWR, NULL);
							lDst+=(long_t)tWR;
						}

						lSrc+=(long_t)tRD;
					}
					while(tRD != 0);

					// now truncate the file to the needed size
					SetEndOfFile(hFile);
				}
			}

			CloseHandle(hFile);
			return true;
		}
	}
#else
	FILE* pFile=fopen(m_pszPath, _t("r+"));
	if (pFile)
	{
		// seek
		if (fseek(pFile, iSize-iNewSize, SEEK_SET) == 0)
		{
			// read the string to the eol
			tchar_t szBuffer[4096];
			fgets(szBuffer, 4096, pFile);
			iNewSize-=_tcslen(szBuffer);			// new size correction

			int_t iSrc=ftell(pFile);
			int_t iDst=0;
			size_t tRD, tWR;
					
			do
			{
				// seek to src
				fseek(pFile, iSrc, SEEK_SET);

				// read 4k chars from source offset
				tRD=fread(szBuffer, 1, 4096, pFile);
				if (tRD > 0)
				{
					// seek to the dst
					fseek(pFile, iDst, SEEK_SET);

					fflush(pFile);
					// write the buffer to the dest offset
					tWR=fwrite(szBuffer, 1, tRD, pFile);
					iDst+=tWR;
				}

				iSrc+=tRD;
			}
			while(tRD != 0);

			// now truncate the file to the needed size
			ftruncate(fileno(pFile), iDst);

			fclose(pFile);
			return true;
		}

		fclose(pFile);
	}
#endif

	return false;
}

/** Logs a formatted message to a log file.
 * \param[in] iType - type of the log message (LT_*)
 * \param[in] bStd - log also to stdout/stderr if true
 * \param[in] pszStr - format string for the following parameters
 */
void log_file::log(int_t iType, bool bStd, const tchar_t* pszStr, ...)
{
	if (iType < m_iLogLevel)
		return;
	
	va_list va;
	va_start(va, pszStr);
	logv(iType, bStd, pszStr, va);
	va_end(va);
}

/** Logs a formatted message to a log file.
 * \param[in] iType - type of the log message (LT_*)
 * \param[in] bStd - log also to stdout/stderr if true
 * \param[in] pszStr - format string for the following parameters
 * \param[in] va - variable argument list
 */
void log_file::logv(int_t iType, bool bStd, const tchar_t* pszStr, va_list va)
{
	if (iType < m_iLogLevel)
		return;
	
	tchar_t szBuf1[2048];
	_vsntprintf(szBuf1, 2048, pszStr, va);		// user passed stuff
	
	logs(iType, bStd, szBuf1);
}
	
/** Logs an unformatted message to a log file.
 * \param[in] iType - type of the log message (LT_*)
 * \param[in] bStd - log also to stdout/stderr if true
 * \param[in] pszStr - message string
 */
void log_file::logs(int_t iType, bool bStd, const tchar_t* pszStr)
{
	assert(m_pszPath);
	if(!m_pszPath)
		return;

	if (iType < m_iLogLevel || iType < 0 || iType >= sizeof(__logtype_str))
		return;
	
	// log time
	time_t t=time(NULL);
	tchar_t szData[128];
	_tcscpy(szData, _tctime(&t));
	size_t tLen=_tcslen(szData)-1;
	while(szData[tLen] == _t('\n'))
		szData[tLen--]=_t('\0');

	m_lock.lock();
	
	// check the size constraints
	truncate((int_t)(_tcslen(pszStr)+1));
#if defined(UNICODE) && (defined(_WIN32) || defined(_WIN64))
	FILE* pFile=_tfopen(m_pszPath, _t("ab"));
#else
	FILE* pFile=_tfopen(m_pszPath, _t("at"));
#endif
	bool bFailed=false;
	if (pFile)
	{
		if (_ftprintf(pFile, _t("[") TSTRFMT _t("] [") TSTRFMT _t("] ") TSTRFMT ENDL, szData, __logtype_str[iType], pszStr) < 0)
			bFailed=true;
		fclose(pFile);
	}
	else
		bFailed=true;
	if (bFailed || (m_bLogStd && !bStd))
	{
		switch(iType)
		{
		case level_error:
			_ftprintf(stderr, _t("[") TSTRFMT _t("] [") TSTRFMT _t("] ") TSTRFMT ENDL, szData, __logtype_str[iType], pszStr);
			break;
		default:
			_ftprintf(stdout, _t("[") TSTRFMT _t("] [") TSTRFMT _t("] ") TSTRFMT ENDL, szData, __logtype_str[iType], pszStr);
		}
	}
	else if (bStd)
	{
		switch(iType)
		{
		case level_error:
			_ftprintf(stderr, TSTRFMT _t(": ") TSTRFMT ENDL, __logtype_str[iType], pszStr);
			break;
		case level_info:
			_ftprintf(stdout, TSTRFMT ENDL, pszStr);
			break;
		default:
			_ftprintf(stdout, TSTRFMT _t(": ") TSTRFMT ENDL, __logtype_str[iType], pszStr);
		}
	}

	m_lock.unlock();
}

#ifndef SKIP_LEVEL_DEBUG
/** Logs a formatted debug message to a log file.
 * \param[in] pszStr - format string for the given parameters
 */
void log_file::logd(const tchar_t* pszStr)
{
	if (m_iLogLevel > level_debug)
		return;
	
	logs(level_debug, false, pszStr);
}

/** Logs a formatted debug message to a log file.
* \param[in] pszStr - format string for the given parameters
*/
void log_file::logdv(const tchar_t* pszStr, ...)
{
	if (m_iLogLevel > level_debug)
		return;

	va_list va;
	va_start(va, pszStr);
	logv(level_debug, false, pszStr, va);
	va_end(va);
}

/** Logs a formatted debug message to a log file(also outputs to stdout).
 * \param[in] pszStr - format string for the given parameters
 */
void log_file::logds(const tchar_t* pszStr, ...)
{
	if (m_iLogLevel > level_debug)
		return;
	
	va_list va;
	va_start(va, pszStr);
	logv(level_debug, true, pszStr, va);
	va_end(va);
}

#else
void log_file::logd(const tchar_t* /*pszStr*/)
{
}

void log_file::logdv(const tchar_t* /*pszStr*/, ...)
{
}

void log_file::logds(const tchar_t* /*pszStr*/, ...)
{
}
#endif

#ifndef SKIP_LEVEL_INFO
/** Logs a formatted informational message to a log file.
 * \param[in] pszStr - format string for the given parameters
 */
void log_file::logi(const tchar_t* pszStr)
{
	if (m_iLogLevel > level_info)
		return;
	
	logs(level_info, false, pszStr);
}

/** Logs a formatted informational message to a log file.
* \param[in] pszStr - format string for the given parameters
*/
void log_file::logiv(const tchar_t* pszStr, ...)
{
	if (m_iLogLevel > level_info)
		return;

	va_list va;
	va_start(va, pszStr);
	logv(level_info, false, pszStr, va);
	va_end(va);
}

/** Logs a formatted informational message to a log file(also outputs to stdout).
 * \param[in] pszStr - format string for the given parameters
 */
void log_file::logis(const tchar_t* pszStr, ...)
{
	if (m_iLogLevel > level_info)
		return;
	
	va_list va;
	va_start(va, pszStr);
	logv(level_info, true, pszStr, va);
	va_end(va);
}
#else
void log_file::logi(const tchar_t* /*pszStr*/)
{
}

void log_file::logiv(const tchar_t* /*pszStr*/, ...)
{
}

void log_file::logis(const tchar_t* /*pszStr*/, ...)
{
}

#endif

#ifndef SKIP_LEVEL_WARNING
/** Logs a formatted warning message to a log file.
 * \param[in] pszStr - format string for the given parameters
 */
void log_file::logw(const tchar_t* pszStr)
{
	if (m_iLogLevel > level_warning)
		return;
	
	logs(level_warning, false, pszStr);
}

/** Logs a formatted warning message to a log file.
* \param[in] pszStr - format string for the given parameters
*/
void log_file::logwv(const tchar_t* pszStr, ...)
{
	if (m_iLogLevel > level_warning)
		return;

	va_list va;
	va_start(va, pszStr);
	logv(level_warning, false, pszStr, va);
	va_end(va);
}

/** Logs a formatted warning message to a log file(also outputs to stdout).
 * \param[in] pszStr - format string for the given parameters
 */
void log_file::logws(const tchar_t* pszStr, ...)
{
	if (m_iLogLevel > level_warning)
		return;
	va_list va;
	va_start(va, pszStr);
	logv(level_warning, true, pszStr, va);
	va_end(va);
}

#else
void log_file::logw(const tchar_t* /*pszStr*/)
{
}

void log_file::logwv(const tchar_t* /*pszStr*/, ...)
{
}

void log_file::logws(const tchar_t* /*pszStr*/, ...)
{
}

#endif

/** Logs a formatted error message to a log file.
 * \param[in] pszStr - format string for the given parameters
 */
void log_file::loge(const tchar_t* pszStr)
{
	logs(level_error, false, pszStr);
}

/** Logs a formatted error message to a log file.
* \param[in] pszStr - format string for the given parameters
*/
void log_file::logev(const tchar_t* pszStr, ...)
{
	va_list va;
	va_start(va, pszStr);
	logv(level_error, false, pszStr, va);
	va_end(va);
}

/** Logs a formatted error message to a log file(also outputs to stderr).
 * \param[in] pszStr - format string for the given parameters
 */
void log_file::loges(const tchar_t* pszStr, ...)
{
	va_list va;
	va_start(va, pszStr);
	logv(level_error, true, pszStr, va);
	va_end(va);
}

/** Logs a formatted error message to a log file(also outputs to stderr).
 *  As an addition the first string %err is replaced with a given error 
 *  followed by the error description (system-based).
 * \param[in] pszStr - format string for the given parameters
 * \param[in] iSysErr - system error to be shown
 */
void log_file::logerr(const tchar_t* pszStr, int_t iSysErr, ...)
{
	tchar_t szNewFmt[2048];
	if (prepare_fmt(pszStr, iSysErr, szNewFmt))
	{
		va_list va;
		va_start(va, iSysErr);
		logv(level_error, false, szNewFmt, va);
		va_end(va);
	}
	else
	{
		va_list va;
		va_start(va, iSysErr);
		logv(level_error, false, pszStr, va);
		va_end(va);
	}
}

/** Logs a formatted error message to a log file(also outputs to stderr).
 *  As an addition the first string %err is replaced with a given error 
 *  followed by the error description (system-based).
 *  This function differ from logerr() with logging the output string
 *  also to the stderr.
 * \param[in] pszStr - format string for the given parameters
 * \param[in] iSysErr - system error to be shown
 */
void log_file::logerrs(const tchar_t* pszStr, int_t iSysErr, ...)
{
	tchar_t szNewFmt[2048];
	if (prepare_fmt(pszStr, iSysErr, szNewFmt))
	{
		va_list va;
		va_start(va, iSysErr);
		logv(level_error, true, szNewFmt, va);
		va_end(va);
	}
	else
	{
		va_list va;
		va_start(va, iSysErr);
		logv(level_error, true, pszStr, va);
		va_end(va);
	}
}

/** Function prepares a format string with error number and an error message
 *  for use with logerr() and logerrs() functions.
 * \param[in] pszStr - input format string (%err will be replaced with a 0x%lx (error message)
 * \param[in] iSysError - system error to parse
 * \param[out] pszOut - pointer to a buffer that will receive the data (must be 2048 bytes in size)
 * \return If the %err string was found and replaced within a given format string.
 */
bool log_file::prepare_fmt(const tchar_t* pszStr, int_t iSysErr, tchar_t* pszOut) const
{
	// find the %err in pszStr
	const tchar_t* pszFnd=_tcsstr(pszStr, _t("%err"));
	if (pszFnd)
	{
		// find an error description for the error
		tchar_t* pszErrDesc=NULL;
#ifdef _WIN32
		tchar_t szErrDesc[512];
		FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, (DWORD)iSysErr, 0, szErrDesc, 512, NULL);
		pszErrDesc=szErrDesc;
#else
		pszErrDesc=strerror(iSysErr);
#endif

		// format a string with err no and desc
		tchar_t szError[1024];
		_sntprintf(szError, 1023, _t("0x%lx (%s)"), iSysErr, pszErrDesc);
		szError[1023] = _T('\0');

		// replace %err with the new data
		pszOut[0]=_t('\0');
		_tcsncat(pszOut, pszStr, (size_t)(pszFnd-pszStr));
		_tcscat(pszOut, szError);
		_tcscat(pszOut, pszFnd+4);

		return true;
	}
	else
		return false;
}

END_ICPF_NAMESPACE
