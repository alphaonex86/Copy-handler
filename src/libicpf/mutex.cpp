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
#include "mutex.h"

BEGIN_ICPF_NAMESPACE

mutex::mutex()
{
#ifdef _WIN32
	::InitializeCriticalSection(&m_cs);
#else
	pthread_mutexattr_t mta;
	pthread_mutexattr_init(&mta);
//#warning Recursive mutexes are disabled; Make sure you use them the right way.
	pthread_mutexattr_settype(&mta, PTHREAD_MUTEX_RECURSIVE_NP);
	pthread_mutex_init(&m_mutex, &mta);

	pthread_mutexattr_destroy(&mta);
#endif
}

mutex::mutex(const char_t* /*pszStr*/)
{
#ifdef _WIN32
	::InitializeCriticalSection(&m_cs);
#else
	pthread_mutexattr_t mta;
	pthread_mutexattr_init(&mta);
//#warning Recursive mutexes are disabled; Make sure you use them the right way.
	pthread_mutexattr_settype(&mta, PTHREAD_MUTEX_RECURSIVE_NP);
	pthread_mutex_init(&m_mutex, &mta);

	pthread_mutexattr_destroy(&mta);
#endif
}

mutex::~mutex()
{
#ifdef _WIN32
	::DeleteCriticalSection(&m_cs);
#else
	pthread_mutex_destroy(&m_mutex);
#endif
}
	
#ifdef ENABLE_MUTEX_DEBUGGING
void mutex::lock(const char_t* pszFile, ulong_t ulLine, const char_t* pszFunction)
{
	lock();
}

void mutex::unlock(const char_t* pszFile, ulong_t ulLine, const char_t* pszFunction)
{
	unlock();
}
#endif

END_ICPF_NAMESPACE
