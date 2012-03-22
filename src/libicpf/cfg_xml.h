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
#ifndef __CFGXML_H__
#define __CFGXML_H__

//#include "gen_types.h"
//#include "libicpf.h"
//#include "config_base.h"
//
//BEGIN_ICPF_NAMESPACE
//
///** Class provides the necessary base handlers for config class.
// *  It handles the xml data streams contained in the files, providing
// *  a way to set and retrieve data contained in the xml document.
// */
//class LIBICPF_API xml_cfg : public config_base
//{
//public:
///** \name Construction/destruction/operators */
///**@{*/
//	xml_cfg();							///< Standard constructor
//	xml_cfg(const xml_cfg& rSrc);		///< Copy constructor
//	virtual ~xml_cfg();					///< Standard destructor
///**@}*/
//
///** \name File operations */
///**@{*/
//	/// Reads the xml document from the specified file
//	virtual void read(const tchar_t* pszPath);
//	/// Saves the internal data to a specified file as the xml document
//	virtual void save(const tchar_t* pszPath);
///**@}*/
//
///** \name Key and value handling */
///**@{*/
//	/// Searches for a specified key (given all the path to a specific string)
//	virtual ptr_t find(const tchar_t* pszName);
//	/// Searches for the next string
//	virtual const tchar_t* find_next(ptr_t pFindHandle);
//	/// Closes the search operation
//	virtual void find_close(ptr_t pFindHandle);
//
//	/// Sets a value for a given key
//	virtual void set_value(const tchar_t* pszName, const tchar_t* pszValue, actions a=action_add);
//	/// Clear values for a given property name
//	virtual void clear(const tchar_t* pszName);
//	/// Clears all entries
//	virtual void clear();
///**@}*/
//
//private:
//	/// Find helper - recursively searches for a specific key node
//	ptr_t find(ptr_t pNodePtr, const tchar_t* pszName);
//	/// Set value helper - searches for a specific node and sets the value
//	void set_value(ptr_t pNodePtr, const tchar_t* pszName, const tchar_t* pszValue, actions a=action_add);
//	/// Clear helper - clears the appropriate attributes
//	void clear(ptr_t pNodePtr, const tchar_t* pszName);
//
//	/// Saves the specific node into the file
//	void save_node(FILE* pFile, ptr_t pNodePtr);
//
//	/// Stores the string to the file converted to utf8
//	void fprintf_encoded(FILE* pFile, const tchar_t* pszFmt, ...);
//
//	static void element_start(void *userData, const tchar_t *name, const tchar_t **attrs);
//	static void element_end(void *userData, const tchar_t* name);
//
//protected:
//	ptr_t m_hMainNode;		///< Handle to the internal xml storage
//};
//
//END_ICPF_NAMESPACE

#endif
