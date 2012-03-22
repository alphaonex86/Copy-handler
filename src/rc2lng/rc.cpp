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
#include "stdafx.h"
#include <vector>
#include <map>
#include <functional>
#include <algorithm>
#include "rc.h"
#include "../libicpf/exception.h"
#include "../libicpf/crc32.h"

#define MAX_LINE 65536

CRCFile::CRCFile() :
	m_pszBuffer(new TCHAR[MAX_LINE])
{
	AddInitialValues();
}

CRCFile::~CRCFile()
{
	delete [] m_pszBuffer;
}

void CRCFile::ReadRC(PCTSTR pszFilename)
{
	// read file
	CRCFile::ReadFile(pszFilename, m_vInRCFile, false);

	// and process it
	ProcessRCFile();
}

void CRCFile::ReadResourceIDs(PCTSTR pszFile)
{
	_ASSERTE(pszFile);
	std::vector<CString> vLines;
	CRCFile::ReadFile(pszFile, vLines, false);

	long lLineNo = 0;
	CString str, str2;
	for(std::vector<CString>::iterator itLine = vLines.begin(); itLine != vLines.end(); ++itLine)
	{
		str = (*itLine);
		str.TrimLeft(_T(" \t"));
		if (str.Left(7) == _T("#define"))
		{
			str=str.Mid(8);
			int iPos=str.FindOneOf(_T(" \t"));
			str2=str.Left(iPos);
			str=str.Mid(iPos);
			str.TrimLeft(_T(" \t"));
			str.TrimRight(_T(" \t\r\n"));

			int iID;
			if(str.Find(_T("x")) != -1)
			{
				// hex2dec
				if(_stscanf(str, _T("%lx"), &iID) != 1)
					THROW(icpf::exception::format(TSTRFMT _T("(%ld) : Error: Cannot parse hex number in line:\n") TSTRFMT, pszFile, lLineNo, m_pszBuffer), 0, 0, 0);
			}
			else
				iID=_ttoi(str);

			m_mapNameToID.insert(std::map<CString, UINT>::value_type(str2, iID));
		}

		++lLineNo;
	}
}

void CRCFile::WriteRC(PCTSTR pszFilename)
{
	CRCFile::WriteFile(pszFilename, m_vOutRCFile, false);
}

void CRCFile::WriteLang(PCTSTR pszFilename, PCTSTR pszHeaderFile)
{
	// write header file to the output file
	std::vector<CString> vData;
	CRCFile::ReadFile(pszHeaderFile, vData, true);

	// append the translation
	CString str;
	for(group_map::iterator itGroup = m_mapOutputLanguage.begin(); itGroup != m_mapOutputLanguage.end(); ++itGroup)
	{
		// write section header
		AddTranslationLine(eLine_Group, (*itGroup).first, NULL, vData);

		// and key=value assignment
		element_map& rElementMap = (*itGroup).second;
		for(element_map::iterator itElement = rElementMap.begin(); itElement != rElementMap.end(); ++itElement)
		{
			AddTranslationLine(eLine_Translation, (*itElement).first, (*itElement).second, vData);
		}
	}

	CRCFile::WriteFile(pszFilename, vData, true);
}

void CRCFile::Clear()
{
	m_vInRCFile.clear();
	m_vOutRCFile.clear();
	m_mapNameToID.clear();
	m_mapOutputLanguage.clear();

	AddInitialValues();
}

void CRCFile::AddInitialValues()
{
	m_mapNameToID.insert(std::map<CString, UINT>::value_type(CString(_T("IDOK")), 1));
	m_mapNameToID.insert(std::map<CString, UINT>::value_type(CString(_T("IDCANCEL")), 2));
	m_mapNameToID.insert(std::map<CString, UINT>::value_type(CString(_T("IDABORT")), 3));
	m_mapNameToID.insert(std::map<CString, UINT>::value_type(CString(_T("IDRETRY")), 4));
	m_mapNameToID.insert(std::map<CString, UINT>::value_type(CString(_T("IDIGNORE")), 5));
	m_mapNameToID.insert(std::map<CString, UINT>::value_type(CString(_T("IDYES")), 6));
	m_mapNameToID.insert(std::map<CString, UINT>::value_type(CString(_T("IDNO")), 7));
	m_mapNameToID.insert(std::map<CString, UINT>::value_type(CString(_T("IDCLOSE")), 8));
	m_mapNameToID.insert(std::map<CString, UINT>::value_type(CString(_T("IDHELP")), 9));
	m_mapNameToID.insert(std::map<CString, UINT>::value_type(CString(_T("IDTRYAGAIN")), 10));
	m_mapNameToID.insert(std::map<CString, UINT>::value_type(CString(_T("IDCONTINUE")), 11));
}

int CRCFile::GetCommasCount(const CString& str)
{
	int cnt=0;
	bool bInside=false;
	for (int i=0;i<str.GetLength();i++)
	{
		if (str[i] == _T('\"'))
			bInside=!bInside;

		if (!bInside && str[i] == _T(','))
			cnt++;
	}

	return cnt;
}

void CRCFile::AddTranslationLine(ELineType eLineType, UINT uiID, PCTSTR pszText, std::vector<CString>& vLines)
{
	switch(eLineType)
	{
	case eLine_Group:
		{
			CString str;
			str.Format(_T("[") UIFMT _T("]"), uiID);
			vLines.push_back(_T(""));
			vLines.push_back(str);
			break;
		}
	case eLine_Translation:
		{
			if(!pszText)
				THROW(_T("Error: Invalid string."), 0, 0, 0);
			if(pszText[0] != _T('\0'))
			{
				CString str;
				str.Format(UIFMT _T("[") UIXFMT _T("]=") TSTRFMT, uiID, icpf::crc32((const byte_t*)pszText, _tcslen(pszText)*sizeof(TCHAR)), pszText);
				vLines.push_back(str);
			}
			break;
		}
	default:
		{
			_ASSERTE(FALSE);
			THROW(_T("Error: Unknown line type."), 0, 0, 0);
		}
	}
}

void CRCFile::ReadFile(PCTSTR pszFile, std::vector<CString>& rLines, bool bUnicode)
{
	// load file
	FILE* pFile = _tfopen(pszFile, bUnicode ? _T("rb") : _T("rt"));
	if(!pFile)
		THROW(icpf::exception::format(_T("Error: Cannot open file: ") TSTRFMT, pszFile), 0, errno, 0);

	CString str;
	while(_fgetts(str.GetBufferSetLength(MAX_LINE), MAX_LINE, pFile))
	{
		str.ReleaseBuffer();
		str.TrimRight(_T("\r\n"));
		rLines.push_back(str);
	}
	str.ReleaseBuffer();

	fclose(pFile);
}

void CRCFile::WriteFile(PCTSTR pszFile, const std::vector<CString>& rLines, bool bUnicode)
{
	FILE* pFile = _tfopen(pszFile, bUnicode ? _T("wb") : _T("wt"));
	if(!pFile)
		THROW(icpf::exception::format(_T("Error: Cannot open file: ") TSTRFMT _T(" for writing."), pszFile), 0, errno, 0);

	for (std::vector<CString>::const_iterator it=rLines.begin();it != rLines.end();it++)
	{
		CString str = (*it);
		str += _T("\r\n");
		if(_fputts((PCTSTR)str, pFile) < 0)
			THROW(icpf::exception::format(_T("Cannot write data to file ") TSTRFMT, pszFile), 0, errno, 0);
	}

	fclose(pFile);
}

void CRCFile::ProcessMenu(UINT uiMenuID, std::vector<CString>::iterator *init)
{
	element_map* pElementMap = GetElementMap(uiMenuID);
	if(!pElementMap)
		return;

	CString str;
	for (;(*init) != m_vInRCFile.end();(*init)++)
	{
		str=**init;
		str.TrimLeft(_T(" "));
		str.TrimRight(_T(" "));

		// check for exit
		if ( str == _T("END") )
		{
			// add the line to the outrc wo changes
			m_vOutRCFile.push_back(**init);
			return;
		}
		else if (str.Left(5) == _T("POPUP")) // if that is the popup string - call the function once more
		{
			// add the line to the outrc with changes - replace string inside "" with P
			str=**init;

			// processing menuitem - find the text
			int iPos=str.Find(_T("\""), 0);
			CString strText;
			if (iPos != -1)
			{
				strText=str.Mid(iPos+1);
				int iPos2=strText.Find(_T("\""));
				if (iPos2 != -1)
					strText=strText.Left(iPos2);
			}

			// now find the | that separates the text from the pseudo-ID
			int iBar=strText.ReverseFind(_T('|'));
			if (iBar != -1)
			{
				// there is a text with an ID
				CString strID=strText.Mid(iBar+1);
				strText=strText.Left(iBar);

				// put the id and text in the translation file
				// find the equiv for the id
				UINT uiID = GetResourceID(strID);
				pElementMap->insert(std::make_pair(uiID, strText));

				// put the found ID as output text
				CString out;
				out.Format(_T("\"%lu\""), uiID);
				str=str.Left(iPos)+out;
			}
			else
			{
				// no pseudoID in menu name
				str=str.Left(iPos)+_T("\"P\"");
			}

			m_vOutRCFile.push_back(str);

			(*init)++;
			ProcessMenu(uiMenuID, init);
		}
		else
		{
			// if the line has MENUITEM
			if (str.Left(8) == _T("MENUITEM") && str.Right(9) != _T("SEPARATOR"))
			{
				// restore original
				str=**init;

				// check if there is any text after the comma
				int iPos=str.Find(_T(","), 0);
				CString strTest=str.Mid(iPos);
				strTest.TrimLeft(_T(" ,\t\r\n"));
				if (strTest.IsEmpty())
				{
					(*init)++;

					CString tmp=**init;
					tmp.Trim(_T(" ,\t\r\n"));
					str+=tmp;
				}

				// processing menuitem - find the text
				iPos=str.Find(_T("\""), 0);
				CString strText;
				if (iPos != -1)
				{
					strText=str.Mid(iPos+1);
					int iPos2=strText.Find(_T("\""));
					if (iPos2 != -1)
						strText=strText.Left(iPos2);
				}

				// find the ID
				iPos=str.Find(_T(","), 0);
				CString strID;
				if (iPos != -1)
				{
					strID=str.Mid(iPos+1);
					int iPos2=strID.Find(_T(","), 0);
					if (iPos2 != -1)
						strID=strID.Left(iPos2);
				}
				strID.TrimLeft(_T(" \t"));
				strID.TrimRight(_T(" \t"));

				// find the equiv for the id
				UINT uiID = GetResourceID(strID);
				pElementMap->insert(std::make_pair(uiID, strText));
				CString out = str;
				//				out=**init;
				out.Replace(_T("\"")+strText+_T("\""), _T("\"i\""));
				m_vOutRCFile.push_back(out);
			}
			else
				m_vOutRCFile.push_back(**init);
		}
	}
}

void CRCFile::ProcessDialog(UINT uiDialogID, std::vector<CString>::iterator *init)
{
	element_map* pElementMap = GetElementMap(uiDialogID);
	if(!pElementMap)
		return;

	CString str;
	for (;(*init) != m_vInRCFile.end();(*init)++)
	{
		str=**init;
		str.TrimLeft(_T(" "));
		str.TrimRight(_T(" "));

		// check for exit
		if ( str == _T("END") )
		{
			// add the line to the outrc wo changes
			m_vOutRCFile.push_back(**init);
			return;
		}
		else if ( str.Left(7) == _T("CAPTION") )
		{
			// read the caption
			CString strText=str.Mid(7);
			strText.TrimLeft(_T(" \t\""));
			strText.TrimRight(_T(" \t\""));

			pElementMap->insert(std::make_pair(0, strText));

			// save to rc wo title
			str=**init;
			str.Replace(_T("\"")+strText+_T("\""), _T("\"\""));
			m_vOutRCFile.push_back(str);
		}
		else if ( str.Left(5) == _T("LTEXT") || str.Left(5) == _T("CTEXT") || str.Left(5) == _T("RTEXT") || str.Left(13) == _T("DEFPUSHBUTTON") || str.Left(10) == _T("PUSHBUTTON") || str.Left(7) == _T("CONTROL") || str.Left(8) == _T("GROUPBOX") )
		{
			// needed only 2 commas (outside the '\"')
			if ( GetCommasCount(str) < 3 )
				str+=*((*init)+1);

			// the first thing after LTEXT(and other) is the caption
			CString strText;
			bool bControl = false;

			if (str.Left(5) == _T("LTEXT") || str.Left(5) == _T("CTEXT") || str.Left(5) == _T("RTEXT"))
				str=str.Mid(5);
			else if (str.Left(13) == _T("DEFPUSHBUTTON"))
				str=str.Mid(13);
			else if (str.Left(10) == _T("PUSHBUTTON"))
				str=str.Mid(10);
			else if (str.Left(7) == _T("CONTROL"))
			{
				bControl = true;
				str=str.Mid(7);
			}
			else if (str.Left(8) == _T("GROUPBOX"))
				str=str.Mid(8);

			str=str.Mid(str.Find(_T("\""))+1);
			int iPos=str.Find(_T("\""), 0);
			if (iPos != -1)
				strText=str.Left(iPos);
			else
				THROW(_T("Error: cannot find a comma in processed text"), 0, 0, 0);

			str = str.Mid(iPos+1);

			// after the first comma there is an ID
			iPos=str.Find(_T(","), 0);
			CString strID;
			if (iPos != -1)
			{
				str=str.Mid(iPos+1);
				iPos=str.Find(_T(","), 0);
				if (iPos != -1)
					strID=str.Left(iPos);
				else
					THROW(_T("Error: cannot find a comma in processed text"), 0, 0, 0);

				strID.TrimLeft(_T(" \t"));
				strID.TrimRight(_T(" \t"));
			}
			else
				THROW(_T("Error: cannot find a comma in processed text"), 0, 0, 0);

			bool bSkip = false;
			if(bControl)
			{
				str = str.Mid(iPos+1);
				iPos = str.Find(_T(","), 0);
				if(iPos == -1)
					THROW(_T("Error: cannot find a comma in processed text"), 0, 0, 0);

				CString strType = str.Left(iPos);
				strType.Trim(_T("\""));
				if(strType == _T("SysListView32") || strType == _T("msctls_progress32") ||
					strType == _T("ComboBoxEx32") || strType == _T("msctls_updown32") ||
					strType == _T("SysDateTimePick32"))
				{
					bSkip = true;
				}
			}

			if(!bSkip)
			{
				// find id
				UINT uiID = GetResourceID(strID);
				CString out;
				pElementMap->insert(std::make_pair(uiID, strText));
			}

			// now add the data to output rc
			str=**init;
			str.Replace(_T("\"")+strText+_T("\""), _T("\"\""));

			m_vOutRCFile.push_back(str);
		}
		else
			m_vOutRCFile.push_back(**init);
	}
}

void CRCFile::ProcessStringTable(UINT uiStringGroupID, std::vector<CString>::iterator *init)
{
	element_map* pElementMap = GetElementMap(uiStringGroupID);
	if(!pElementMap)
		return;

	CString str;
	for (;(*init) != m_vInRCFile.end();(*init)++)
	{
		str=**init;
		str.TrimLeft(_T(" \t"));
		str.TrimRight(_T(" \t"));

		if ( str == _T("END") )
			return;
		else if ( str != _T("BEGIN") )
		{
			// the first stuff is ID, the second is text
			int iPos=str.Find(_T("\""), 0);
			if (iPos == -1)
			{
				(*init)++;
				str+=**init;
				iPos=str.Find(_T("\""), 0);
			}

			if (iPos != -1)
			{
				CString strID=str.Left(iPos);
				strID.TrimRight(_T(" \"\t\n\r"));

				CString strText=str.Mid(iPos+1);
				strText.Replace(_T("\"\""), _T("\""));
				strText=strText.Left(strText.ReverseFind(_T('\"')));

				UINT uiID = GetResourceID(strID);
				pElementMap->insert(std::make_pair(uiID, strText));

				str=**init;
				str.Replace(_T("\"")+strText+_T("\""), _T("\"\""));
			}
		}
	}
}

bool CRCFile::ProcessRCFile()
{
	int iPos;
	CString strData;
	std::vector<CString> vStrTable;
	for (std::vector<CString>::iterator it=m_vInRCFile.begin();it != m_vInRCFile.end();it++)
	{
		if ( (iPos=it->Find(_T(" MENU "))) != -1 )
		{
			// add the line to the output rc with no change
			m_vOutRCFile.push_back(*it);

			UINT uiID = GetResourceID(it->Left(iPos));

			// begin enumerating items
			it++;

			// process the menu
			ProcessMenu(uiID, &it);
		}
		else if ( (iPos=it->Find(_T(" DIALOGEX "))) != -1)
		{
			// add the line to the output rc with no change
			m_vOutRCFile.push_back(*it);

			UINT uiID = GetResourceID(it->Left(iPos));
			// begin processing dialog template
			it++;
			ProcessDialog(uiID, &it);
		}
		else if ( (iPos=it->Find(_T("STRINGTABLE "))) != -1)
		{
			// begin of the string table
			it++;
			ProcessStringTable(0, &it);
		}
		else
			m_vOutRCFile.push_back(*it);
	}

	return true;
}

CRCFile::element_map* CRCFile::GetElementMap(UINT uiGroup)
{
	// insert menu if does not exist
	element_map* pElementMap = NULL;
	group_map::iterator itGroup = m_mapOutputLanguage.find(uiGroup);
	if(itGroup == m_mapOutputLanguage.end())
	{
		std::pair<group_map::iterator, bool> pairGroup = m_mapOutputLanguage.insert(std::make_pair(uiGroup, element_map()));
		if(!pairGroup.second)
		{
			_ASSERTE(false);
			return NULL;
		}

		pElementMap = &(*(pairGroup.first)).second;
	}
	else
		pElementMap = &((*itGroup).second);

	if(!pElementMap)
	{
		_ASSERTE(false);
		return NULL;
	}

	return pElementMap;
}

UINT CRCFile::GetResourceID(PCTSTR pszID)
{
	std::map<CString, UINT>::iterator mit = m_mapNameToID.find(pszID);
	if (mit != m_mapNameToID.end())
		return (*mit).second;
	else
	{
		_ASSERTE(false);
		THROW(icpf::exception::format(_T("Error: Cannot find resource identifier ") TSTRFMT, pszID), 0, 0, 0);
	}
}
