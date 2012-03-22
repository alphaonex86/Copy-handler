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
/** \file dumpctx.h
 *  \brief Contain class dumpctx - a debug helper class.
 */
 
#ifndef __DUMPCONTEXT_H__
#define __DUMPCONTEXT_H__

#include "mutex.h"
#include "gen_types.h"
//#include "str.h"

BEGIN_ICPF_NAMESPACE

/// Maximum size of a single variable dump (ie. string)
//#define MAX_DUMP	4096

// dump type flags
/// Std out; pParam is NULL
/*#define DCX_STD			0x00000000
/// File object; pParam is const tchar_t* with file path
#define DCX_FILE		0x00000001
/// File object; pParam is FILE* - handle to a file opened in write mode
#define DCX_FILEHANDLE	0x00000002
/// Log file; pParam is log_file* with the given log (dump's will be logged as LT_DEBUG)
#define DCX_LOG			0x00000003
*/

/** \brief Class used as dump context (debugging purposes).
 *
 *  Class should be used to perform dump's of any object's internal state.
 *  Any class could (should?) use this class as a param to the dump member.
 *  Usage is quite simple - construct an object with needed flags and when
 *  need to dump an object - call open with an object name, dump what's needed
 *  and then close.
 *  Class is thread safe.
 */
class LIBICPF_API dumpctx
{
	/// Dump types available
	enum dump_types
	{
		type_std,			/// Reporting to the console
		type_file,			/// Reporting to the file
		type_filehandle,	/// Reporting to the file handle
		type_log			/// Reporting to a log file
	};

	/// Internal buffer size to use
	enum buf_sizes { max_dump=4096 };
public:
/** \name Construction/destruction */
/**@{*/
	explicit dumpctx(uint_t uiType, ptr_t pParam=NULL);	///< Standard constructor
	~dumpctx();											///< Standard destructor
/**@}*/	
	
/** \name Opening/closing dump process */
/**@{*/
	void open(const tchar_t* pszObject);	///< Begins the specified object dump
	void close();						///< Ends the object dump
/**@}*/
	
/** \name Dumping functions
 *  Operations to be executed between calls to open() and close()
 */
/**@{*/
	void dump(const tchar_t* pszName, const tchar_t* pszValue);		///< Ansi string dump
//	void dump(const tchar_t* pszName, const wchar_t* pszValue);		///< Unicode string dump
	void dump(const tchar_t* pszName, const tchar_t cValue);			///< tchar_t dump
	void dump(const tchar_t* pszName, const short_t sValue);			///< short_t dump
	void dump(const tchar_t* pszName, const int_t iValue);			///< int_t dump
	void dump(const tchar_t* pszName, const uchar_t ucValue);		///< uchar_t dump
	void dump(const tchar_t* pszName, const ushort_t usValue);		///< ushort_t dump
	void dump(const tchar_t* pszName, const uint_t uiValue);			///< uint_t dump
	void dump(const tchar_t* pszName, const longlong_t llValue);		///< longlong_t dump
	void dump(const tchar_t* pszName, const ulonglong_t ullValue);	///< ulonglong_t dump
	void dump(const tchar_t* pszName, const ptr_t pValue);			///< pointer dump
/**@}*/
protected:
	mutex m_lock;				///< A mutex or d_mutex class cast to void* because of the circular dependencies problem
//	string m_strBuffer;			///< String object that will gather information about dump
	ptr_t m_hBuffer;			///< Internal buffer handle
	tchar_t m_szBuffer[max_dump];	///< Buffer used in formatting output data
	uint_t m_uiType;			///< Type of dump (as passed to constructor)
	ptr_t m_pParam;				///< Parameter - the real type depends on the m_ulType field
};

END_ICPF_NAMESPACE

#endif
