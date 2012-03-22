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
#ifndef __CALLBACK_H__
#define __CALLBACK_H__

/** \file callback.h
 *  \brief Provides callback classes
 */

#include "libicpf.h"
#include "gen_types.h"
#include <list>
#include "mutex.h"

BEGIN_ICPF_NAMESPACE

/// General function definition
#ifdef _WIN32
    typedef void(__stdcall *PFNFUNC)(void);
#else
    typedef void(*PFNFUNC)(void);
#endif 

/// Helper structure for callback class
struct LIBICPF_API CLBDATA
{
	PFNFUNC pfn;			///< General function definition
	ptr_t param;
};

class LIBICPF_API callback_list
{
public:
/** \name Construction/destruction */
/**@{*/
	callback_list();
	callback_list(const callback_list& rSrc);
	virtual ~callback_list();
/**@}*/

	void add(PFNFUNC pfn, ptr_t param);
	bool remove(PFNFUNC pfn);
	void clear();
	size_t size();
	CLBDATA* at(size_t tIndex);

	void lock();
	void unlock();

	const callback_list& operator=(const callback_list& rSrc);

protected:
	icpf::mutex m_lock;		///< A locking mechanism for the storage area

private:
	void* m_pStorage;		///< A pointer to a storage struct (a std::list probably), but must be inaccessible from outside
};

/** \brief Callback class with one parameter.
 *
 *  Class provides a simple interface for user to call a specific callback
 *  function(s) registered by the user. Good for notifying user that something
 *  had happened.
 */
template<class R, class P1>
class /*LIBICPF_API*/ callback1 : public callback_list
{
protected:
	/// Callback1-type callback function
#ifdef _WIN32
	typedef R(__stdcall *PFNCALLBACKPROC1)(ptr_t, P1);
#else
	typedef R(*PFNCALLBACKPROC1)(ptr_t, P1);
#endif

public:
/** \name Construction/destruction */
/**@{*/
	callback1()  : callback_list() { };	///< Standard constructor
	virtual ~callback1() { };	///< Standard destructor
/**@}*/

/** \name User interface */
/**@{*/
	/** Executes a callback list associated with this object.
	* \param[in] data - parameter that will be passed to a user callback function
	*/
	void exec(P1 data)
	{
		m_lock.lock();
		CLBDATA* pData;
		for (size_t i=0;i != size();i++)
		{
			pData=at(i);
			if (pData)
				(*(PFNCALLBACKPROC1)(pData->pfn))(pData->param, data);
		}
		m_lock.unlock();
	}
	
	/** Connects a user callback function to this object.
	* \param[in] pfn - user callback function address
	* \param[in] appParam - user parameter to pass to the callback function when executing
	*/
	void connect(PFNCALLBACKPROC1 pfn, ptr_t appParam)
	{
		add((PFNFUNC)pfn, appParam);
	}

	/** Disconnects the user callback function if connected earlier.
	* \param[in] pfn - address of a function to remove
	*/
	void disconnect(PFNCALLBACKPROC1 pfn)
	{
		remove((PFNFUNC)pfn);
	}
/**@}*/
};

/** \brief Callback class with two parameters.
 *
 *  Class provides a simple interface for user to call a specific callback
 *  function(s) registered by the user. Good for notifying user that something
 *  had happened.
 */
template<class R, class P1, class P2>
class /*LIBICPF_API*/ callback2 : public callback_list
{
protected:
	/// Callback2-type callback function
#ifdef _WIN32
	typedef R(__stdcall *PFNCALLBACKPROC2)(ptr_t, P1, P2);
#else
	typedef R(*PFNCALLBACKPROC2)(ptr_t, P1, P2);
#endif

public:
/** \name Construction/destruction */
/**@{*/
	callback2() : callback_list() { };	///< Standard constructor
	virtual ~callback2() { };	///< Standard destructor
/**@}*/

/** \name User interface */
/**@{*/
	/** Executes a callback list associated with this object.
	* \param[in] data1 - parameter that will be passed to a user callback function
	* \param[in] data2 - parameter that will be passed to a user callback function
	*/
	void exec(P1 data1, P2 data2)
	{
		m_lock.lock();
		CLBDATA* pData;
		for (size_t i=0;i != size();i++)
		{
			pData=at(i);
			if (pData)
				(*(PFNCALLBACKPROC2)(pData->pfn))(pData->param, data1, data2);
		}
		m_lock.unlock();
	}
	
	/** Connects a user callback function to this object.
	* \param[in] pfn - user callback function address
	* \param[in] appParam - user parameter to pass to the callback function when executing
	*/
	void connect(PFNCALLBACKPROC2 pfn, ptr_t appParam)
	{
		add((PFNFUNC)pfn, appParam);
	}

	/** Disconnects the user callback function if connected earlier.
	* \param[in] pfn - address of a function to remove
	*/
	void disconnect(PFNCALLBACKPROC2 pfn)
	{
		remove((PFNFUNC)pfn);
	}
/**@}*/
};

/** \brief Callback class with three parameters.
 *
 *  Class provides a simple interface for user to call a specific callback
 *  function(s) registered by the user. Good for notifying user that something
 *  had happened.
 */
template<class R, class P1, class P2, class P3>
class /*LIBICPF_API*/ callback3 : public callback_list
{
protected:
	/// Callback2-type callback function
#ifdef _WIN32
	typedef R(__stdcall *PFNCALLBACKPROC3)(ptr_t, P1, P2, P3);
#else
	typedef R(*PFNCALLBACKPROC3)(ptr_t, P1, P2, P3);
#endif

public:
/** \name Construction/destruction */
/**@{*/
	callback3() : callback_list() { };	///< Standard constructor
	virtual ~callback3() { };	///< Standard destructor
/**@}*/

/** \name User interface */
/**@{*/
	/** Executes a callback list associated with this object.
	* \param[in] data1 - parameter that will be passed to a user callback function
	* \param[in] data2 - parameter that will be passed to a user callback function
	* \param[in] data3 - parameter that will be passed to a user callback function
	*/
	void exec(P1 data1, P2 data2, P3 data3)
	{
		m_lock.lock();
		CLBDATA* pData;
		for (size_t i=0;i != size();i++)
		{
			pData=at(i);
			if (pData)
				(*(PFNCALLBACKPROC3)(pData->pfn))(pData->param, data1, data2, data3);
		}
		m_lock.unlock();
	}
	
	/** Connects a user callback function to this object.
	* \param[in] pfn - user callback function address
	* \param[in] appParam - user parameter to pass to the callback function when executing
	*/
	void connect(PFNCALLBACKPROC3 pfn, ptr_t appParam)
	{
		add((PFNFUNC)pfn, appParam);
	}

	/** Disconnects the user callback function if connected earlier.
	* \param[in] pfn - address of a function to remove
	*/
	void disconnect(PFNCALLBACKPROC3 pfn)
	{
		remove((PFNFUNC)pfn);
	}
/**@}*/
};

END_ICPF_NAMESPACE

#endif
