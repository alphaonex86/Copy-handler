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
/** \file dmutex.cpp
 *  \brief Contains mutex class for thread safe access with debugging capabilities (implementation).
 *  \see The mutex class.
 */
#include "dmutex.h"

#ifdef ENABLE_MUTEX_DEBUGGING

#include <assert.h>
#include <stdio.h>

#ifdef _WIN32
	#include <windows.h>
#endif

BEGIN_ICPF_NAMESPACE

///////////////////////////////////////////////////////////////
// debuggable mutex

/** \brief Static dump context.
 *
 *  Must be initialized before using this class.
 */
dumpctx* d_mutex::m_pContext=NULL;

/** Constructs an unnamed mutex with a given dump context which will receive
 *  notifications about locking and unlocking of this mutex.
 */
d_mutex::d_mutex() :
	mutex(),
	m_ulLockCount(0)
{
	const char_t* psz="Unnamed";
	m_pszName=new char_t[strlen(psz)+1];
	strcpy(m_pszName, psz);

	m_ulLockCount=0;
}

/** Constructs a named mutex with a given dump context which will receive
 *  notifications about locking and unlocking of this mutex.
 *
 * \param[in] pszStr - name of this mutex (will be used for logging purposes)
 */
d_mutex::d_mutex(const char_t* pszStr) :
	mutex(pszStr),
	m_ulLockCount(0)
{
	m_pszName=new char_t[strlen(pszStr)+1];
	strcpy(m_pszName, pszStr);
}

/** Destructs the object
 */
d_mutex::~d_mutex()
{
	delete [] m_pszName;
}

/** Locks this mutex. Takes some parameters that should identify the place in code which
 *  at which the locking occurs.
 *
 * \param[in] pszFile - name of the source file in which the locking was requested
 * \param[in] ulLine - line of code in the file at which the locking was requested
 * \param[in] pszFunction - name of the function in which the locking was requested
 */
void d_mutex::lock(const char_t* pszFile, ulong_t ulLine, const char_t* pszFunction)
{
	assert(m_pContext);
	char_t sz[512];

	// pre-lock notification
	if (m_pContext)
	{
#ifdef _WIN32
		uint_t uiThread=GetCurrentThreadId();
		uint_t uiTime=GetTickCount();
#else
		// TODO: linux version of thread id must be put here sometime in the future
		assert(false);
		uint_t uiThread=0;
		uint_t uiTime=time(NULL);
#endif
		_snprintf(sz, 512, "[%lu][%lu][%s] Lock attempt (current lock count: %lu) in (%s - %lu: %s)", uiTime, uiThread, m_pszName, m_ulLockCount, pszFile, ulLine, pszFunction);
		m_pContext->open(sz);
		m_pContext->close();
	}

	// this is the real locking
	((mutex*)this)->lock();
	m_ulLockCount++;

	// post-lock information
	if (m_pContext)
	{
#ifdef _WIN32
		uint_t uiThread=GetCurrentThreadId();
		uint_t uiTime=GetTickCount();
#else
		// TODO: linux version of thread id must be put here sometime in the future
		assert(false);
		uint_t uiThread=0;
		uint_t uiTime=time(NULL);
#endif
		_snprintf(sz, 512, "[%lu][%lu][%s] LOCKED (current lock count: %lu) in (%s - %lu: %s)", uiTime, uiThread, m_pszName, m_ulLockCount, pszFile, ulLine, pszFunction);
		m_pContext->open(sz);
		m_pContext->close();
	}
}

/** Unlocks this mutex. Takes some parameters that should identify the place in code which
 *  at which the unlocking occurs.
 *
 * \param[in] pszFile - name of the source file in which the unlocking was requested
 * \param[in] ulLine - line of code in the file at which the unlocking was requested
 * \param[in] pszFunction - name of the function in which the unlocking was requested
 */
void d_mutex::unlock(const char_t* pszFile, ulong_t ulLine, const char_t* pszFunction)
{
	assert(m_pContext);
	char_t sz[512];

	// pre-lock notification
	if (m_pContext)
	{
#ifdef _WIN32
		uint_t uiThread=GetCurrentThreadId();
		uint_t uiTime=GetTickCount();
#else
		// TODO: linux version of thread id must be put here sometime in the future
		assert(false);
		uint_t uiThread=0;
		uint_t uiTime=time(NULL);
#endif
		_snprintf(sz, 512, "[%lu][%lu][%s] Unlock attempt (current lock count: %lu) in (%s - %lu: %s)", uiTime, uiThread, m_pszName, m_ulLockCount, pszFile, ulLine, pszFunction);
		m_pContext->open(sz);
		m_pContext->close();
	}

	// log the attempt and lock it
	m_ulLockCount--;
	((mutex*)this)->unlock();

	// post-unlock notification
	// NOTE: code here is quite dangerous - and could CRASH the application;
	// we have just unlocked the real mutex, so anyone has access to the object protected
	// by this d_mutex. If the object being protected is removed - the internal members
	// would be invalid. It especially have some meaning for the sprintf function
	// probably because of the m_pszName member
	if (m_pContext)
	{
#ifdef _WIN32
		uint_t uiThread=GetCurrentThreadId();
		uint_t uiTime=GetTickCount();
#else
		// TODO: linux version of thread id must be put here sometime in the future
		assert(false);
		uint_t uiThread=0;
		uint_t uiTime=time(NULL);
#endif
		_snprintf(sz, 512, "[%lu][%lu][%s] UNLOCKED (current lock count: %lu) in (%s - %lu: %s)", uiTime, uiThread, m_pszName, m_ulLockCount, pszFile, ulLine, pszFunction);
		m_pContext->open(sz);
		m_pContext->close();
	}
}

END_ICPF_NAMESPACE

#endif
