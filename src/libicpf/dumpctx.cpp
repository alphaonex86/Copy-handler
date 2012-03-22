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
/** \file dumpctx.cpp
 *  \brief Contain implementation of class dumpctx - a debug helper class.
 */
#include "dumpctx.h"
#include <stdio.h>
#include "log.h"

BEGIN_ICPF_NAMESPACE

/// Helper definition for faster access
#define m_pBuffer ((tstring_t*)m_hBuffer)

/** Constructor stores the passed data in the internal members.
 * \param[in] uiType - type of dump (one of the DCX_*)
 * \param[in] pParam - additional param - the type of theis param depends on the ulType
 */
dumpctx::dumpctx(uint_t uiType, ptr_t pParam) :
	m_lock(),
	m_hBuffer((ptr_t)new tstring_t),
	m_uiType(uiType),
	m_pParam(pParam)
{
	m_uiType=uiType;
	if (uiType == type_file)
	{
		size_t tLen=_tcslen((const tchar_t*)pParam);
		m_pParam=(ptr_t)new tchar_t[tLen+1];
		_tcscpy((tchar_t*)m_pParam, (const tchar_t*)pParam);
	}
	else
		m_pParam=pParam;
	m_szBuffer[0]=_t('\0');
}

/** Destructor frees the internal data if needed
 */
dumpctx::~dumpctx()
{
	if (m_uiType == type_file)
		delete [] (tchar_t*)m_pParam;
	else
		m_pParam=NULL;		// we won't have a leak here, since we don't alloc memory for case m_uiType != type_file
}

/** Function opens the dump. It means initializing the internal string
 *  that will contain the dump result and locking the class (using mutex).
 *  \note Always call the close() member if you have opened the dump.
 */
void dumpctx::open(const tchar_t* pszObject)
{
	MLOCK(m_lock);
	*m_pBuffer=pszObject;
	*m_pBuffer+=_t("\n");
}

/** Closes the dump. Depending on the type specified in the constructor the
 *  function outputs the dump to the specified location. Also the internal 
 *  buffer is cleared.
 */
void dumpctx::close()
{
	// perform a dump - depending on the type of a dest object
	switch(m_uiType)
	{
		case type_std:
		{
			_tprintf(TSTRFMT, m_pBuffer->c_str());
			break;
		}
		case type_file:
		{
			FILE* pFile=_tfopen((const tchar_t*)m_pParam, _t("a"));
			if (pFile != NULL)
			{
				_ftprintf(pFile, TSTRFMT, m_pBuffer->c_str());
				fclose(pFile);
			}
			
			break;
		}
		case type_filehandle:
		{
			_ftprintf((FILE*)m_pParam, TSTRFMT, m_pBuffer->c_str());
			break;
		}
		case type_log:
		{
			((log_file*)m_pParam)->logdv(TSTRFMT, m_pBuffer->c_str());
			break;
		}
		default:
			break;
	}
	
	// clean the internal buffer
	m_pBuffer->clear();
	
	MUNLOCK(m_lock);
}

/** Function dumps (stores in the internal string object) the given ansi string.
 *  Strings longer than max_dump characters will be truncated.
 * \param[in] pszName - name of the member variable of the dumped object
 * \param[in] pszValue - an ansi string - the value of a given member
 */
void dumpctx::dump(const tchar_t* pszName, const tchar_t* pszValue)
{
	_sntprintf(m_szBuffer, max_dump, TSTRFMT _t(" (tstring):\n\t") PTRFMT _t(" (\"") TSTRFMT _t("\")\n"), pszName, pszValue, pszValue);
	m_szBuffer[max_dump-1]=_t('\0');
	MLOCK(m_lock);
	*m_pBuffer+=m_szBuffer;
	MUNLOCK(m_lock);
}

/** Function dumps (stores in the internal string object) the given unicode string.
 *  Strings longer than max_dump characters will be truncated.
 * \param[in] pszName - name of the member variable of the dumped object
 * \param[in] pszValue - an unicode string - the value of a given member
 */
/*void dumpctx::dump(const tchar_t* pszName, const wchar_t* pszValue)
{
	_sntprintf(m_szBuffer, max_dump, STRFMT _t(" (wide string):\n\t") PTRFMT _t(" (\"") WSTRFMT _t("\")\n"), pszName, pszValue, pszValue);
	m_szBuffer[max_dump-1]=_t('\0');
	MLOCK(m_lock);
	*m_pBuffer+=m_szBuffer;
	MUNLOCK(m_lock);
}*/

/** Function dumps (stores in the internal string object) the given character.
 * \param[in] pszName - name of the member variable of the dumped object
 * \param[in] cValue - a character (signed tchar_t) value
 */
void dumpctx::dump(const tchar_t* pszName, const tchar_t cValue)
{
	_sntprintf(m_szBuffer, max_dump, TSTRFMT _t(" (tchar_t):\n\t'") TCHRFMT _t("' (hex: ") CXFMT _t(" / dec: ") CFMT _t(")\n"), pszName, cValue, (short_t)cValue, (short_t)cValue);
	m_szBuffer[max_dump-1]=_t('\0');
	MLOCK(m_lock);
	*m_pBuffer+=m_szBuffer;
	MUNLOCK(m_lock);
}

/** Function dumps (stores in the internal string object) the given short_t.
 * \param[in] pszName - name of the member variable of the dumped object
 * \param[in] sValue - a short_t value to dump
 */
void dumpctx::dump(const tchar_t* pszName, const short_t sValue)
{
	_sntprintf(m_szBuffer, max_dump, TSTRFMT _t(" (short_t):\n\t") SFMT _t(" (hex: ") SXFMT _t(")\n"), pszName, sValue, sValue);
	m_szBuffer[max_dump-1]=_t('\0');
	MLOCK(m_lock);
	*m_pBuffer+=m_szBuffer;
	MUNLOCK(m_lock);
}

/** Function dumps (stores in the internal string object) the given int_t.
 * \param[in] pszName - name of the member variable of the dumped object
 * \param[in] iValue - a int_t value to dump
 */
void dumpctx::dump(const tchar_t* pszName, const int_t iValue)
{
	_sntprintf(m_szBuffer, max_dump, TSTRFMT _t(" (int_t):\n\t") LFMT _t(" (hex: ") LXFMT _t(")\n"), pszName, iValue, iValue);
	m_szBuffer[max_dump-1]=_t('\0');
	MLOCK(m_lock);
	*m_pBuffer+=m_szBuffer;
	MUNLOCK(m_lock);
}

/** Function dumps (stores in the internal string object) the given uchar_t.
 * \param[in] pszName - name of the member variable of the dumped object
 * \param[in] ucValue - an uchar_t value to dump
 */
void dumpctx::dump(const tchar_t* pszName, const uchar_t ucValue)
{
	_sntprintf(m_szBuffer, max_dump, TSTRFMT _t(" (uchar_t):\n\t'") UCHARFMT _t("' (hex: ") UCXFMT _t(" / dec: ") UCFMT _t(")\n"), pszName, ucValue, (ushort_t)ucValue, (ushort_t)ucValue);
	m_szBuffer[max_dump-1]=_t('\0');
	MLOCK(m_lock);
	*m_pBuffer+=m_szBuffer;
	MUNLOCK(m_lock);
}

/** Function dumps (stores in the internal string object) the given ushort_t.
 * \param[in] pszName - name of the member variable of the dumped object
 * \param[in] usValue - an ushort_t value to dump
 */
void dumpctx::dump(const tchar_t* pszName, const ushort_t usValue)
{
	_sntprintf(m_szBuffer, max_dump, TSTRFMT _t(" (ushort_t):\n\t") USFMT _t(" (hex: ") USXFMT _t(")\n"), pszName, usValue, usValue);
	m_szBuffer[max_dump-1]=_t('\0');
	MLOCK(m_lock);
	*m_pBuffer+=m_szBuffer;
	MUNLOCK(m_lock);
}

/** Function dumps (stores in the internal string object) the given uint_t.
 * \param[in] pszName - name of the member variable of the dumped object
 * \param[in] uiValue - an uint_t value to dump
 */
void dumpctx::dump(const tchar_t* pszName, const uint_t uiValue)
{
	_sntprintf(m_szBuffer, max_dump, TSTRFMT _t(" (uint_t):\n\t") ULFMT _t(" (hex: ") ULXFMT _t(")\n"), pszName, uiValue, uiValue);
	m_szBuffer[max_dump-1]=_t('\0');
	MLOCK(m_lock);
	*m_pBuffer+=m_szBuffer;
	MUNLOCK(m_lock);
}

/** Function dumps (stores in the internal string object) the longlong_t.
 * \param[in] pszName - name of the member variable of the dumped object
 * \param[in] llValue - a longlong_t value to dump
 */
void dumpctx::dump(const tchar_t* pszName, const longlong_t llValue)
{
	_sntprintf(m_szBuffer, max_dump, TSTRFMT _t(" (longlong_t):\n\t") LLFMT _t(" (hex: ") LLXFMT _t(")\n"), pszName, llValue, llValue);
	m_szBuffer[max_dump-1]=_t('\0');
	MLOCK(m_lock);
	*m_pBuffer+=m_szBuffer;
	MUNLOCK(m_lock);
}

/** Function dumps (stores in the internal string object) the ulonglong_t.
 * \param[in] pszName - name of the member variable of the dumped object
 * \param[in] ullValue - an ulonglong_t value to dump
 */
void dumpctx::dump(const tchar_t* pszName, const ulonglong_t ullValue)
{
	_sntprintf(m_szBuffer, max_dump, TSTRFMT _t(" (ulonglong_t):\n\t") ULLFMT _t(" (hex: ") ULLXFMT _t(")\n"), pszName, ullValue, ullValue);
	m_szBuffer[max_dump-1]=_t('\0');
	MLOCK(m_lock);
	*m_pBuffer+=m_szBuffer;
	MUNLOCK(m_lock);
}

/** Function dumps (stores in the internal string object) the untyped pointer.
 * \param[in] pszName - name of the member variable of the dumped object
 * \param[in] pValue - an untyped pointer value to dump
 */
void dumpctx::dump(const tchar_t* pszName, const ptr_t pValue)
{
	_sntprintf(m_szBuffer, max_dump, TSTRFMT _t(" (ptr_t):\n\t") PTRFMT _t("\n"), pszName, pValue);
	m_szBuffer[max_dump-1]=_t('\0');
	MLOCK(m_lock);
	*m_pBuffer+=m_szBuffer;
	MUNLOCK(m_lock);
}

END_ICPF_NAMESPACE
