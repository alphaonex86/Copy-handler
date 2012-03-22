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
#include "cfg_xml.h"
//#include <expat.h>
#include "exception.h"
#include <string>
#include <map>
#include <assert.h>

//BEGIN_ICPF_NAMESPACE
//
///// Buffer size for reading xml data from a file
//#define XML_BUFFER	65536
//
//// definition of line ending - system dependent
//#if defined(_WIN32) || defined(_WIN64)
//	#define ENDL _t("\r\n")
//#else
//	#define ENDL _t("\n")
//#endif
//
//// forward declaration
//class xml_node;
//
///// Xml node storage
//typedef std::map<tstring_t, xml_node> xml_storage;
///// String storage (key(s)=>value(s))
//typedef std::multimap<tstring_t, tstring_t> attr_storage;
//
///** Class manages a single xml node.
// */
//class xml_node
//{
//public:
///** \name Construction/destruction */
///**@{*/
//	/// Standard constructor
//	xml_node() : m_mNodes(), m_mAttr(), m_pParentNode(NULL) { };
//	/// Constructor defining the parent node
//	xml_node(xml_node* pParentNode) : m_mNodes(), m_mAttr(), m_pParentNode(pParentNode) { };
///**@}*/
//
//	/// Clears the node
//	void clear(bool bClearParent = false)
//	{
//		m_mNodes.clear();
//		m_mAttr.clear();
//		if(bClearParent)
//			m_pParentNode = NULL;
//	}
//
//public:
//	xml_storage m_mNodes;		///< Additional nodes inside of this one
//	attr_storage m_mAttr;		///< String pairs belonging to this node
//	xml_node* m_pParentNode;	///< Parent node
//};
//
///** State structure - used by expat notifications.
// */
//struct XMLSTATE
//{
//	xml_cfg* pCfg;
//	xml_node* pNode;
//};
//
///** Xml find handle structure - used for searching.
// */
//struct XMLFINDHANDLE
//{
//	attr_storage::iterator it;			///< Iterator of currently retrieved string
//	attr_storage::iterator itEnd;		///< Iterator of a last string matching the criteria
//};
//
///// Macro for faster access to the xml storage
//#define m_pMainNode ((xml_node*)m_hMainNode)
//
///** Constructs the xml_cfg object.
// */
//xml_cfg::xml_cfg() :
//	m_hMainNode((ptr_t)new xml_node)
//{
//	
//}
//
///** Destructs the xml config object.
// */
//xml_cfg::~xml_cfg()
//{
//	delete m_pMainNode;
//}
//
///** Expat start element handler.
// *
// * \param[in] userData - pointer to user defined parameters
// * \param[in] name - name of the tag being processed
// * \param[in] attrs - array of pointers to strings with attributes and their values
// */
//void xml_cfg::element_start(void *userData, const tchar_t *name, const tchar_t **attrs)
//{
//	XMLSTATE* pState=(XMLSTATE*)userData;
//	assert(pState);
//	assert(pState->pNode);
//
//	// temp
//	tchar_t szData[512];
//	_sntprintf(szData, 512, _t("Opening Name: %s\n"), name);
//	OutputDebugString(szData);
//	// /temp
//
//	// parse node attributes
//	for(size_t t=0;attrs[t] != NULL;t+=2)
//	{
//		if(_tcscmp(attrs[t], _t("value")) == 0)
//		{
//			// this is the value type tag
//			pState->pNode->m_mAttr.insert(attr_storage::value_type(tstring_t(name), tstring_t(attrs[t+1])));
//		}
//	}
//
//	std::pair<xml_storage::iterator, bool> pr;
//	pr=pState->pNode->m_mNodes.insert(xml_storage::value_type(tstring_t(name), xml_node(pState->pNode)));
//	pState->pNode=&((*pr.first).second);
//}
//
///** Expat handler for closing tag.
// *
// * \param[in] userData - user defined parameter
// * \param[in] name - name of the tag being closed
// */
//void xml_cfg::element_end(void *userData, const tchar_t* name)
//{
//	XMLSTATE* pState=(XMLSTATE*)userData;
//	assert(pState);
//
//	// temp
//	tchar_t szData[512];
//	_sntprintf(szData, 512, _t("Closing Name: %s\n"), name);
//	OutputDebugString(szData);
//	// /temp
//
//	// go up one level
//	if(pState->pNode)
//		pState->pNode=pState->pNode->m_pParentNode;
//	else
//		THROW(_t("Trying to close non-existent tag."), 0, 0, 0);
//}
//
///*void XMLCALL element_content(void *userData, const XML_Char *s, int len)
//{
//	XMLSTATE* pState=(XMLSTATE*)userData;
//
//}*/
//
///** Function reads the contents of the xml file, parses it using expat parser
// *  and then creates xml nodes in memory that could be read using find functions.
// *
// * \param[in] pszPath - path to the file to be read
// */
//void xml_cfg::read(const tchar_t* pszPath)
//{
//	// clear current contents
//	clear();
//
//	// read the data from file in 64kB portions and feed it to the expat xml parser
//	FILE* pFile=_tfopen(pszPath, _t("rb"));
//	if(pFile == NULL)
//		THROW(icpf::exception::format(_t("Cannot open the file ") TSTRFMT _t(" for reading."), pszPath), 0, errno, 0);
//
//	// create the parser
//	XML_Parser parser=XML_ParserCreate(NULL);
//	XML_SetElementHandler(parser, element_start, element_end);
////	XML_SetCharacterDataHandler(parser, element_content);
//
//	XMLSTATE xs = { this, m_pMainNode };
//	XML_SetUserData(parser, &xs);
//
//	for(;;)
//	{
//		bool bLast=false;
//
//		// get xml buffer
//		void* pBuffer=XML_GetBuffer(parser, XML_BUFFER);
//
//		// read some data to it
//		size_t tSize=fread(pBuffer, 1, XML_BUFFER, pFile);
//		if(tSize < XML_BUFFER)
//		{
//			// check for errors
//			int iErr=0;
//			if( (iErr=ferror(pFile)) != 0)
//				THROW(icpf::exception::format(_t("Error reading from the file ") TSTRFMT _t("."), pszPath), 0, iErr, 0);
//			else
//				bLast=true;
//		}
//
//		// parse
//		if(!XML_ParseBuffer(parser, (int)tSize, bLast))
//		{
//			// parser error
//			THROW(icpf::exception::format(_t("Error encountered while parsing the xml file ") STRFMT _t(" - ") STRFMT _t("."), pszPath, XML_ErrorString(XML_GetErrorCode(parser))), 0, 0, 0);
//		}
//
//		// end of processing ?
//		if(bLast)
//			break;
//	}
//
//	// free parser
//	XML_ParserFree(parser);
//
//	// close the file
//	fclose(pFile);
//}
//
///** Saves the internal xml nodes to the specified xml file.
// *
// * \param[in] pszPath - path to the file the data should be written to
// *
// * \note Function overwrites the contents of a file
// */
//void xml_cfg::save(const tchar_t* pszPath)
//{
//	FILE* pFile=_tfopen(pszPath, _t("wb"));
//	if(pFile == NULL)
//		THROW(icpf::exception::format(_t("Cannot open the file ") TSTRFMT _t(" for writing."), pszPath), 0, errno, 0);
//
//	// put BOM into the file
//#if(defined(_WIN32) || defined(_WIN64))
//	// utf-16le
//	const uint_t uiBOM=0x0000feff;
//	const uint_t uiCount=2;
//#else
//	// utf-8
//	const uint_t uiBOM=0x00bfbbef;
//	const uint_t uiCount=3;
//#endif
//
//	try
//	{
//		// write bom, check if it succeeded
//		if(fwrite(&uiBOM, 1, uiCount, pFile) != uiCount)
//			THROW(_t("Cannot write the BOM to the file '") TSTRFMT _t("'"), 0, errno, 0);
//
//		// and write
//		save_node(pFile, m_pMainNode);
//	}
//	catch(...)
//	{
//		fclose(pFile);
//		throw;
//	}
//
//	// close the file
//	fclose(pFile);
//}
//
//void xml_cfg::save_node(FILE* pFile, ptr_t pNodePtr)
//{
//	xml_node* pNode=(xml_node*)pNodePtr;
//
//	// attributes first
//	const tchar_t *pszFmt = _t("<") TSTRFMT _t(" value=\"") TSTRFMT _t("\"/>") ENDL;
//	for(attr_storage::iterator it=pNode->m_mAttr.begin();it != pNode->m_mAttr.end();it++)
//	{
//		fprintf_encoded(pFile, pszFmt, (*it).first.c_str(), (*it).second.c_str());
//	}
//
//	// sub-nodes
//	for(xml_storage::iterator it=pNode->m_mNodes.begin();it != pNode->m_mNodes.end();it++)
//	{
//		xml_node& rNode = (*it).second;
//		if(!rNode.m_mNodes.empty() || !rNode.m_mAttr.empty())
//		{
//			// opening tag
//			fprintf_encoded(pFile, _t("<") TSTRFMT _t(">") ENDL, (*it).first.c_str());
//
//			save_node(pFile, &(*it).second);
//
//			// closing tag
//			fprintf_encoded(pFile, _t("</") TSTRFMT _t(">") ENDL, (*it).first.c_str());
//		}
//	}
//}
//
//void xml_cfg::fprintf_encoded(FILE* pFile, const tchar_t* pszFmt, ...)
//{
//	va_list va;
//	va_start(va, pszFmt);
//
//	// get count of characters in the string
//	int_t iCount=_vsctprintf(pszFmt, va);
//	tchar_t* pszFormatted=new tchar_t[iCount+1];
//
//	// make a formatted string
//	va_start(va, pszFmt);
//	_vsntprintf(pszFormatted, iCount + 1, pszFmt, va);
//
//#if(!defined(UNICODE) && (defined(_WIN32) || defined(_WIN64)))
//	// convert to unicode
//	iCount = lstrlen(pszFormatted);
//	int iWideCount = MultiByteToWideChar(CP_ACP, 0, pszFormatted, iCount, NULL, 0);
//	if(iWideCount)
//	{
//		wchar_t* pszWideString = new wchar_t[iWideCount];
//		iWideCount = MultiByteToWideChar(CP_ACP, 0, pszFormatted, iCount, pszWideString, iWideCount);
//		fwrite(pszWideString, 1, iWideCount*sizeof(wchar_t), pFile);
//
//		delete [] pszWideString;
//	}
//	else
//		THROW(_t("Cannot convert string to wide characters."), 0, GetLastError(), 0);
//#else
//	fwrite(pszFormatted, sizeof(tchar_t), iCount, pFile);
//#endif
//
//	delete [] pszFormatted;
//
//	va_end(va);
//}
//
///** Function starts a search operation. Given the name of the property
// *  to be searched for(ie. "ch/program/startup"), funtion searches for
// *  it and returns a handle that can be used by subsequent calls to the
// *  find_next(). Free the handle using find_close() after finish.
// *
// * \param[in] pszName - name of the property to search for(in the form of
// *						"ch/program/startup" for xml such as this:
// *
// *						<ch>
// *							<program>
// *								<startup value="1"/>
// *							</program>
// *						</ch>
// * \return Handle to the search (NULL if not found).
// */
//ptr_t xml_cfg::find(const tchar_t* pszName)
//{
//	return find(m_pMainNode, pszName);
//}
//
///** A find() helper function - recursively searches a specific node
// *  for a given name.
// *
// * \param[in] pNodePtr - pointer to a node to search in
// * \param[in] pszName - name of the property to search for
// * \return Handle to the node or NULL if none.
// */
//ptr_t xml_cfg::find(ptr_t pNodePtr, const tchar_t* pszName)
//{
//	xml_node* pNode=(xml_node*)pNodePtr;
//
//	// parse the name
//	const tchar_t* pSign=_tcschr(pszName, _t('/'));
//	if(pSign)
//	{
//		// locate the xml_node associated with the name
//		xml_storage::iterator it=pNode->m_mNodes.find(tstring_t(pszName, pSign-pszName));
//		if(it != pNode->m_mNodes.end())
//			return find(&(*it).second, pSign+1);
//		else
//			return NULL;
//	}
//	else
//	{
//		std::pair<attr_storage::iterator, attr_storage::iterator> pr=pNode->m_mAttr.equal_range(pszName);
//		if(pr.first != pNode->m_mAttr.end() && pr.second != pNode->m_mAttr.end())
//		{
//			XMLFINDHANDLE* pfh=new XMLFINDHANDLE;
//			pfh->it=pr.first;
//			pfh->itEnd=pr.second;
//
//			return pfh;
//		}
//		else
//			return NULL;
//	}
//}
//
///** Finds the next string that belong to a specific key (as defined in
// *  a call to find() function.
// *
// * \param[in] pFindHandle - handle to the search (as returned from find())
// * \return Pointer to a next string found, NULL if none.
// */
//const tchar_t* xml_cfg::find_next(ptr_t pFindHandle)
//{
//	XMLFINDHANDLE* pfh=(XMLFINDHANDLE*)pFindHandle;
//	if(pfh->it != pfh->itEnd)
//		return (*pfh->it++).second.c_str();
//	else
//		return NULL;
//}
//
///** Closes the find handle.
// *
// * \param[in] pFindHandle - handle to the search (as returned from find())
// */
//void xml_cfg::find_close(ptr_t pFindHandle)
//{
//	delete ((XMLFINDHANDLE*)pFindHandle);
//}
//
///** Sets the specified value in the given key name. Value can be either added to
// *  the current ones (multi-string support) or replace them completely.
// *
// * \param[in] pszName - key name for which the string should be set at
// * \param[in] pszValue - value to set
// * \param[in] a - action to take while setting
// */
//void xml_cfg::set_value(const tchar_t* pszName, const tchar_t* pszValue, actions a)
//{
//	// traverse the current tag tree
//	set_value(m_pMainNode, pszName, pszValue, a);
//}
//
///** Sets the specified value in the given key name - recursive helper function.
// *
// * \param[in] pNodePtr - pointer to the xml node to process
// * \param[in] pszName - key name for which the string should be set at
// * \param[in] pszValue - value to set
// * \param[in] a - action to take while setting
// */
//void xml_cfg::set_value(ptr_t pNodePtr, const tchar_t* pszName, const tchar_t* pszValue, actions a)
//{
//	xml_node* pNode=(xml_node*)pNodePtr;
//
//	const tchar_t* pszSign=_tcschr(pszName, _t('/'));
//	if(pszSign != NULL)
//	{
//		xml_storage::iterator it=pNode->m_mNodes.find(tstring_t(pszName, pszSign-pszName));
//		if(it != pNode->m_mNodes.end())
//			set_value(&(*it).second, pszSign+1, pszValue, a);
//		else
//		{
//			std::pair<xml_storage::iterator, bool> pr=pNode->m_mNodes.insert(xml_storage::value_type(tstring_t(pszName, pszSign-pszName), xml_node(pNode)));
//			set_value(&(*pr.first).second, pszSign+1, pszValue, a);
//		}
//	}
//	else
//	{
//		// clear if we're replacing
//		switch(a)
//		{
//		case config_base::action_replace:
//			pNode->m_mAttr.clear();
//		case config_base::action_add:
//			pNode->m_mAttr.insert(attr_storage::value_type(tstring_t(pszName), tstring_t(pszValue)));
//			break;
//		default:
//			assert(false);
//		}
//	}
//}
//
///** Clear values for a given property name.
// *
// * \param[in] pszName - name of the property to clear the values for
// */
//void xml_cfg::clear(const tchar_t* pszName)
//{
//	clear(m_pMainNode, pszName);
//}
//
///** Clears the contents of this class
//*
//* \param[in] pszName - name of the property to clear the values for
//*/
//void xml_cfg::clear()
//{
//	m_pMainNode->clear(true);
//}
//
///** Recursive clear function - searches recursively for a proper node
// *  and finally clears the string map.
// *
// * \param[in] pNodePtr - pointer to a node to be processed
// * \param[in] pszName - name of the property to search for in the given node
// */
//void xml_cfg::clear(ptr_t pNodePtr, const tchar_t* pszName)
//{
//	xml_node* pNode=(xml_node*)pNodePtr;
//
//	// parse the name
//	const tchar_t* pSign=_tcschr(pszName, _t('/'));
//	if(pSign)
//	{
//		// locate the xml_node associated with the name
//		xml_storage::iterator it=pNode->m_mNodes.find(tstring_t(pszName, pSign-pszName));
//		if(it != pNode->m_mNodes.end())
//			clear(&(*it).second, pSign+1);
//	}
//	else
//	{
//		std::pair<attr_storage::iterator, attr_storage::iterator> pr=pNode->m_mAttr.equal_range(tstring_t(pszName));
//		pNode->m_mAttr.erase(pr.first, pr.second);
//	}
//}
//
//END_ICPF_NAMESPACE
