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
/** \file dmutex.h
 *  \brief Contains mutex class for thread safe access with debugging capabilities.
 *  \see The mutex class.
 */
#ifndef __DMUTEX_H__
#define __DMUTEX_H__

#include "libicpf.h"
#include "gen_types.h"
#include "dumpctx.h"
#include "mutex.h"

#ifdef ENABLE_MUTEX_DEBUGGING

BEGIN_ICPF_NAMESPACE

/** \brief Class provides the locking and unlocking capabilities for use with threads.
 *
 *  Class is a simple wrapper over the system related thread locking functions. In linux
 *  those functions are pthread_mutex_* and in windoze the functions related to CRITICAL_SECTION
 *  structure.
 *  This class is very similar to the mutex class, with the difference that it allows logging
 *  of the locking/unlocking allowing easier debugging of the mutexes. Interface is almost
 *  out-of-the-box replaceable with standard mutex class.
 *  To use this class properly - the icpf::d_mutex::m_pContext static member has to be initialized
 *  to a pointer to a dumpctx class that will receive notifications.
 */
class LIBICPF_API d_mutex : public mutex
{
public:
/** \name Construction/destruction */
/**@{*/
	d_mutex();							///< Constructs an unnamed mutex
	d_mutex(const char_t* pszStr);		///< Constructs a named mutex
	virtual ~d_mutex();					///< Standard destructor
/**@}*/
	
	// standard locking
/** \name Locking/unlocking */
/**@{*/
	void lock(const char_t* pszFile, ulong_t ulLine, const char_t* pszFunction);		///< Locking with logging
	void unlock(const char_t* pszFile, ulong_t ulLine, const char_t* pszFunction);		///< Unlocking with logging
/**@}*/

public:
	static dumpctx* m_pContext;	///< Dump context that will receive informations about locking/unlocking

private:
	char* m_pszName;			///< Name of the mutex
	ulong_t m_ulLockCount;		///< Current lock count
};

END_ICPF_NAMESPACE

#endif

#endif
