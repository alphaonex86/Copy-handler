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
/** \file mutex.h
 *  \brief Contains mutex class for thread safe access.
 */
#ifndef __MUTEX_H__
#define __MUTEX_H__

#ifdef _WIN32
	#include <windows.h>
#else
	#include <pthread.h>
#endif

#include "libicpf.h"
#include "gen_types.h"

BEGIN_ICPF_NAMESPACE

#if defined(ENABLE_MUTEX_DEBUGGING) && defined(DEBUG_MUTEX)
	#define MLOCK(mutex) (mutex).lock(__FILE__, __LINE__, __FUNCTION__)
	#define MUNLOCK(mutex) (mutex).unlock(__FILE__, __LINE__, __FUNCTION__)
#else
	#define MLOCK(mutex) (mutex).lock()
	#define MUNLOCK(mutex) (mutex).unlock()
#endif

/** \brief Class provides the locking and unlocking capabilities for use with threads.
 *
 *  Class is a simple wrapper over the system related thread locking functions. In linux
 *  those functions are pthread_mutex_* and in windoze the functions related to CRITICAL_SECTION
 *  structure.
 */
class LIBICPF_API mutex
{
public:
/** \name Construction/destruction */
/**@{*/
	/** \brief Standard constructor
	 */
	mutex();
	explicit mutex(const char_t* pszStr);

	/** \brief Standard destructor
	 */
	~mutex();
/**@}*/
	
	// standard locking
/** \name Locking/unlocking */
/**@{*/

	/** \brief Locks access to some part of code for the current thread
	 *
	 * Locks access to some code using the platform specific functions.
	 * \return True if succeeded or false if not.
	 * \note The call under windows always return true.
	 */
	inline void lock()
	{
#ifdef _WIN32
		::EnterCriticalSection(&m_cs);
#else
		pthread_mutex_lock(&m_mutex);
#endif
	}


	/** \brief Unlock access to some locked part of code
	 *
	 * Unlocks access to some code using the platform specific functions.
	 * \return True if succeeded or false if not.
	 * \note The call under windows always return true.
	 */
	inline void unlock()
	{
#ifdef _WIN32
		::LeaveCriticalSection(&m_cs);
#else
		pthread_mutex_unlock(&m_mutex);		// return 0 on success
#endif
	}

/**@}*/

#ifdef ENABLE_MUTEX_DEBUGGING
	void lock(const char_t* pszFile, ulong_t ulLine, const char_t* pszFunction);
	void unlock(const char_t* pszFile, ulong_t ulLine, const char_t* pszFunction);
#endif
private:
#ifdef _WIN32
	/// Underlying windows locking structure
	CRITICAL_SECTION m_cs;
#else
	/// Underlying linux locking structure/handle
	pthread_mutex_t m_mutex;
#endif
};

END_ICPF_NAMESPACE

#endif
