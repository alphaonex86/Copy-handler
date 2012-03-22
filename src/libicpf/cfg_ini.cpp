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
#include "cfg_ini.h"
#include "exception.h"
#include <string>
#include <map>
#include <assert.h>
#include "str_help.h"

BEGIN_ICPF_NAMESPACE

/// Buffer size for reading xml data from a file
#define INI_BUFFER	65536

// definition of line ending - system dependent
#if defined(_WIN32) || defined(_WIN64)
#define ENDL _t("\r\n")
#else
#define ENDL _t("\n")
#endif

#ifdef _UNICODE
	#define TEOF WEOF
#else
	#define TEOF EOF
#endif

/// String storage (key(s)=>value(s))
typedef std::multimap<tstring_t, tstring_t> attr_storage;
/// Ini node storage
typedef std::map<tstring_t, attr_storage> ini_storage;

/** Xml find handle structure - used for searching.
*/
struct INIFINDHANDLE
{
	attr_storage::iterator itAttr;			///< Iterator of currently retrieved string
	attr_storage::iterator itAttrEnd;		///< Iterator of a last string matching the criteria

	ini_storage::iterator itSection;	///< Section iterator
	ini_storage::iterator itSectionEnd;	///< End of section enumeration

	bool bOnlyAttributes;				///< Enumeration type - true = only attributes (does not use section iterators), false = sections + all attributes inside
	bool bSection;						///< Is section to be enumerated first ?
};

/// Macro for faster access to the xml storage
#define m_pMainNode ((ini_storage*)m_hMainNode)

/** Constructs the ini_cfg object.
*/
ini_cfg::ini_cfg() :
	m_hMainNode((ptr_t)new ini_storage)
{

}

/** Destructs the xml config object.
*/
ini_cfg::~ini_cfg()
{
	delete m_pMainNode;
}

/** Function reads the contents of the xml file, parses itAttr using expat parser
*  and then creates xml nodes in memory that could be read using find functions.
*
* \param[in] pszPath - path to the file to be read
*/
void ini_cfg::read(const tchar_t* pszPath)
{
	// clear current contents
	clear();

	// read the data from file
#if defined(_UNICODE) && (defined(_WIN32) || defined(_WIN64))
	FILE* pFile=_tfopen(pszPath, _t("rb"));
#else
	FILE* pFile=_tfopen(pszPath, _t("rt"));
#endif

	if(pFile == NULL)
		THROW(icpf::exception::format(_t("Cannot open the file ") TSTRFMT _t(" for reading."), pszPath), 0, errno, 0);

	// prepare buffer for data
	tchar_t* pszBuffer = new tchar_t[INI_BUFFER];
	tchar_t* pszLine = NULL;
	bool bFirstLine = true;

	while((pszLine = _fgetts(pszBuffer, INI_BUFFER, pFile)) != NULL)
	{
		if(bFirstLine)
		{
			bFirstLine = false;
			// check BOM
			if(pszBuffer[0] != _t('\0') && *(ushort_t*)pszBuffer == 0xfeff)
				parse_line(pszBuffer + 1);
			else
				parse_line(pszBuffer);
		}
		else
			parse_line(pszBuffer);
	}

	delete [] pszBuffer;

	// check if that was eof or error
	if(feof(pFile) == 0)
	{
		fclose(pFile);
		// error while reading file
		THROW(_T("Error while reading ini file."), 0, errno, 0);
	}

	// close the file
	fclose(pFile);
}

/// Processes the data from a given buffer
void ini_cfg::read_from_buffer(const tchar_t* pszBuffer, size_t stLen)
{
	// clear current contents
	clear();

	tchar_t* pszLine = new tchar_t[INI_BUFFER];
	size_t stLineLen = 0;
	const tchar_t* pszCurrent = pszBuffer;
	const tchar_t* pszLast = pszBuffer;
	bool bFirstLine = true;
	while(stLen--)
	{
		if(*pszCurrent == _t('\n'))
		{
			// there is a line [pszLast, pszCurrent)
			stLineLen = pszCurrent - pszLast;
			if(stLineLen)
			{
				if(stLineLen >= INI_BUFFER)
					stLineLen = INI_BUFFER - 1;
				_tcsncpy(pszLine, pszLast, stLineLen);
				pszLine[stLineLen] = _t('\0');

				if(bFirstLine)
				{
					bFirstLine = false;
					// check BOM
					if(pszLine[0] != _t('\0') && *(ushort_t*)pszLine == 0xfeff)
						parse_line(pszLine + 1);
					else
						parse_line(pszLine);
				}
				else
				{
					// process the line
					parse_line(pszLine);
				}
			}
			pszLast = pszCurrent + 1;
		}
		++pszCurrent;
	}
	if(pszCurrent != pszLast)
	{
		// there is a line [pszLast, pszCurrent)
		stLineLen = pszCurrent - pszLast;
		if(stLineLen)
		{
			if(stLineLen >= INI_BUFFER)
				stLineLen = INI_BUFFER - 1;

			_tcsncpy(pszLine, pszLast, stLineLen);
			pszLine[stLineLen] = _t('\0');

			// process the line
			parse_line(pszLine);
		}
	}

	delete [] pszLine;
}

/** Saves the internal xml nodes to the specified xml file.
*
* \param[in] pszPath - path to the file the data should be written to
*
* \note Function overwrites the contents of a file
*/
void ini_cfg::save(const tchar_t* pszPath)
{
	FILE* pFile=_tfopen(pszPath, _t("wb"));
	if(pFile == NULL)
		THROW(icpf::exception::format(_t("Cannot open the file ") TSTRFMT _t(" for writing."), pszPath), 0, errno, 0);

	// put BOM into the file

#if(defined(_WIN32) || defined(_WIN64))
	// utf-16le
	const uint_t uiBOM=0x0000feff;
	const uint_t uiCount=2;
#else
	// utf-8
	const uint_t uiBOM=0x00bfbbef;
	const uint_t uiCount=3;
#endif


	try
	{
		// write bom, check if itAttr succeeded
		if(fwrite(&uiBOM, 1, uiCount, pFile) != uiCount)
			THROW(_t("Cannot write the BOM to the file '") TSTRFMT _t("'"), 0, errno, 0);

		// and write
		tstring_t strLine;
		for(ini_storage::iterator iterSections = m_pMainNode->begin(); iterSections != m_pMainNode->end(); iterSections++)
		{
			strLine = _t("[") + (*iterSections).first + _t("]") + ENDL;
			if(_fputts(strLine.c_str(), pFile) == TEOF)
				THROW(_t("Cannot put section name"), 0, errno, 0);
			for(attr_storage::iterator iterAttribute = (*iterSections).second.begin(); iterAttribute != (*iterSections).second.end(); iterAttribute++)
			{
				strLine = (*iterAttribute).first + _t("=") + (*iterAttribute).second + ENDL;
				if(_fputts(strLine.c_str(), pFile) == TEOF)
					THROW(_t("Cannot put attribute"), 0, errno, 0);
			}

			if(_fputts(ENDL, pFile) == TEOF)
				THROW(_t("Cannot put end-of-line marker into the file"), 0, errno, 0);
		}
	}
	catch(...)
	{
		fclose(pFile);
		throw;
	}

	// close the file
	fclose(pFile);
}

/** Function starts a search operation. Given the name of the property
*  to be searched for(ie. "ch/program/startup"), funtion searches for
*  itAttr and returns a handle that can be used by subsequent calls to the
*  find_next(). Free the handle using find_close() after finish.
*
* \param[in] pszName - name of the property to search for(in the form of
*						"ch/program/startup"
* \return Handle to the search (NULL if not found).
*/
ptr_t ini_cfg::find(const tchar_t* pszName)
{
	if(pszName == NULL || pszName[0] == _t('*'))
	{
		INIFINDHANDLE* pHandle = new INIFINDHANDLE;
		pHandle->bOnlyAttributes = false;
		pHandle->bSection = true;
		pHandle->itSection = m_pMainNode->begin();
		pHandle->itSectionEnd = m_pMainNode->end();

		return pHandle;
	}
	else
	{
		// parse the path
		tstring_t strSection;
		tstring_t strAttr;
		if(!parse_property_name(pszName, strSection, strAttr))
			return NULL;

		ini_storage::iterator iterSection = m_pMainNode->find(strSection);
		if(iterSection == m_pMainNode->end())
			return NULL;

		std::pair<attr_storage::iterator, attr_storage::iterator> pairRange;
		if(strAttr == _t("*"))
		{
			pairRange.first = (*iterSection).second.begin();
			pairRange.second = (*iterSection).second.end();
		}
		else
			pairRange = (*iterSection).second.equal_range(strAttr);
		if(pairRange.first != (*iterSection).second.end() && pairRange.first != pairRange.second)
		{
			INIFINDHANDLE* pHandle = new INIFINDHANDLE;
			pHandle->bSection = false;
			pHandle->bOnlyAttributes = true;
			pHandle->itAttr = pairRange.first;
			pHandle->itAttrEnd = pairRange.second;

			return pHandle;
		}
	}

	return NULL;
}

/** Finds the next string that belong to a specific key (as defined in
*  a call to find() function.
*
* \param[in] pFindHandle - handle to the search (as returned from find())
* \return Pointer to a next string found, NULL if none.
*/
bool ini_cfg::find_next(ptr_t pFindHandle, PROPINFO& pi)
{
	assert(pFindHandle);
	if(!pFindHandle)
		return NULL;
	INIFINDHANDLE* pfh=(INIFINDHANDLE*)pFindHandle;

	if(pfh->bOnlyAttributes)
	{
		if(pfh->itAttr != pfh->itAttrEnd)
		{
			pi.pszName = (*pfh->itAttr).first.c_str();
			pi.pszValue = (*pfh->itAttr).second.c_str();
			pi.bGroup = false;
			pfh->itAttr++;
			return true;
		}
		else
			return false;
	}
	else
	{
		if(pfh->bSection)
		{
			if(pfh->itSection == pfh->itSectionEnd)
				return false;
			pfh->bSection = false;
			pfh->itAttr = (*pfh->itSection).second.begin();
			pfh->itAttrEnd = (*pfh->itSection).second.end();

			// section name
			pi.bGroup = true;
			pi.pszName = (*pfh->itSection++).first.c_str();
			pi.pszValue = NULL;
			return true;
		}
		else
		{
			if(pfh->itAttr != pfh->itAttrEnd)
			{
				pi.bGroup = false;
				pi.pszName = (*pfh->itAttr).first.c_str();
				pi.pszValue = (*pfh->itAttr).second.c_str();

				pfh->itAttr++;
				if(pfh->itAttr == pfh->itAttrEnd)
					pfh->bSection = true;
				return true;

			}
			else
			{
				// should not happen
				assert(false);
				return false;
			}
		}
	}
}

/** Closes the find handle.
*
* \param[in] pFindHandle - handle to the search (as returned from find())
*/
void ini_cfg::find_close(ptr_t pFindHandle)
{
	delete ((INIFINDHANDLE*)pFindHandle);
}

/** Sets the specified value in the given key name. Value can be either added to
*  the current ones (multi-string support) or replace them completely.
*
* \param[in] pszName - key name for which the string should be set at
* \param[in] pszValue - value to set
* \param[in] a - action to take while setting
*/
void ini_cfg::set_value(const tchar_t* pszName, const tchar_t* pszValue, actions a)
{
	// parse the path
	tstring_t strSection;
	tstring_t strAttr;
	if(!parse_property_name(pszName, strSection, strAttr))
		THROW(_t("Property not found"), 0, 0, 0);

	if(strAttr == _t("*"))
		THROW(_t("Wildcards not available in set_value mode"), 0, 0, 0);

	// search
	ini_storage::iterator iterSection = m_pMainNode->find(strSection.c_str());
	if(iterSection == m_pMainNode->end())
	{
		std::pair<ini_storage::iterator, bool> pairSection = m_pMainNode->insert(ini_storage::value_type(strSection, attr_storage()));
		iterSection = pairSection.first;
		if(iterSection == m_pMainNode->end())
			THROW(_t("Problem with creating section"), 0, 0, 0);
	}

	attr_storage& rAttrs = (*iterSection).second;

	// clear if we're replacing
	switch(a)
	{
	case config_base::action_replace:
		{
			std::pair<attr_storage::iterator, attr_storage::iterator> pairRange = (*iterSection).second.equal_range(strAttr);
			rAttrs.erase(pairRange.first, pairRange.second);
			// do not break here - we are about to insert the data
		}
	case config_base::action_add:
		{
			rAttrs.insert(attr_storage::value_type(strAttr, pszValue ? pszValue : tstring_t(_t(""))));
			break;
		}
	default:
		assert(false);
	}
}

/** Clears the contents of this class
*
* \param[in] pszName - name of the property to clear the values for
*/
void ini_cfg::clear()
{
	m_pMainNode->clear();
}

/** Recursive clear function - searches recursively for a proper node
*  and finally clears the string map.
*
* \param[in] pNodePtr - pointer to a node to be processed
* \param[in] pszName - name of the property to search for in the given node
*/
void ini_cfg::clear(const tchar_t* pszName)
{
	if(pszName == NULL || pszName[0] == _t('*'))
		m_pMainNode->clear();
	else
	{
		tstring_t strSection;
		tstring_t strAttr;
		if(!parse_property_name(pszName, strSection, strAttr))
			THROW(_t("Invalid name"), 0, 0, 0);

		ini_storage::iterator iterSection = m_pMainNode->find(strSection);
		if(iterSection != m_pMainNode->end())
		{
			attr_storage& rAttrs = (*iterSection).second;
			std::pair<attr_storage::iterator, attr_storage::iterator> pairRange;

			if(strAttr == _t("*"))
			{
				pairRange.first = rAttrs.begin();
				pairRange.second = rAttrs.end();
			}
			else
				pairRange = (*iterSection).second.equal_range(strAttr);
			rAttrs.erase(pairRange.first, pairRange.second);
		}
	}
}

void ini_cfg::parse_line(const tchar_t* pszLine)
{
	assert(pszLine);
	if(!pszLine)
		THROW(_t("Invalid parameter"), 0, 0, 0);

	tstring_t strLine = pszLine;

	// trim whitespaces on the left
	while(strLine.begin() != strLine.end() && string_tool::is_whitespace(*strLine.begin()))
	{
		strLine.erase(strLine.begin());
	}

	while(strLine.rbegin() != strLine.rend() && (*strLine.rbegin() == _t('\r') || *strLine.rbegin() == _t('\n')))
	{
		strLine.erase(strLine.end() - 1);
	}


	// detect line type
	if(strLine.begin() == strLine.end())			// empty line
		return;
	if(strLine[0] == _t('#') || strLine[0] == _t(';'))	// comment
		return;
	if(strLine[0] == _t('['))
	{
		// trim whitespaces and ']' on the right
		while(strLine.rbegin() != strLine.rend() && (string_tool::is_whitespace(*strLine.rbegin()) || *strLine.rbegin() == _t(']')))
		{
			strLine.erase(strLine.end() - 1);
		}
		// trim [
		strLine.erase(strLine.begin());

		// a new section
		m_strCurrentSection = strLine;
		m_pMainNode->insert(ini_storage::value_type(strLine, attr_storage()));
	}
	else
	{
		// do not trim whitespaces on the right - the spaces may be meaningful
		// key=value
		tstring_t::size_type stPos = strLine.find_first_of(_t('='));
		if(stPos != tstring_t::npos)
		{
			ini_storage::iterator iterSection = m_pMainNode->find(m_strCurrentSection);
			if(iterSection == m_pMainNode->end())
				THROW(_t("Internal processing error. Section should already be included."), 0, 0, 0);
			tstring_t strLeft, strRight;
			strLeft.insert(strLeft.begin(), strLine.begin(), strLine.begin() + stPos);
			strRight.insert(strRight.begin(), strLine.begin() + stPos + 1, strLine.end());
			(*iterSection).second.insert(attr_storage::value_type(strLeft, strRight));
		}
	}
}

bool ini_cfg::parse_property_name(const tchar_t* pszName, tstring_t& rstrSection, tstring_t& rstrName)
{
	// parse the path
	tstring_t strPath = pszName;
	tstring_t::size_type stPos = strPath.find_first_of(_t('/'));
	if(stPos == tstring_t::npos)
		return false;
	tstring_t::size_type stPos2 = strPath.find_first_of(_t('/'), stPos + 1);
	if(stPos2 != tstring_t::npos && stPos2 != stPos)
		return false;											// paths with two or more '/' are not supported

	rstrName.clear();
	rstrName.clear();
	rstrSection.insert(rstrSection.begin(), strPath.begin(), strPath.begin() + stPos);
	rstrName.insert(rstrName.begin(), strPath.begin() + stPos + 1, strPath.end());

	return true;
}

END_ICPF_NAMESPACE

