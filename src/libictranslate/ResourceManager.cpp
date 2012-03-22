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
#include "ResourceManager.h"
#include "../libicpf/exception.h"
#include "../libicpf/cfg.h"
#include "../libicpf/crc32.h"
#include <assert.h>
#include <sstream>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define TRANSLATION_FORMAT_VERSION _T("2")

BEGIN_ICTRANSLATE_NAMESPACE

#define EMPTY_STRING _t("")

CResourceManager CResourceManager::S_ResourceManager;

CFormat::CFormat(const tchar_t* pszFormat) :
	m_strText(pszFormat)
{
}

CFormat::~CFormat()
{
}

void CFormat::SetFormat(const tchar_t* pszFormat)
{
	m_strText = pszFormat;
}

CFormat& CFormat::SetParam(PCTSTR pszName, PCTSTR pszText)
{
	assert(pszName);
	if(!pszName)
		return *this;

	size_t stLen = _tcslen(pszName);
	tstring_t::size_type stPos = 0;
	while((stPos = m_strText.find(pszName)) != tstring_t::npos)
	{
		m_strText.replace(stPos, stLen, pszText);
	}

	return *this;
}

CFormat& CFormat::SetParam(PCTSTR pszName, ull_t ullData)
{
	tchar_t szBuffer[64];
	_sntprintf(szBuffer, 63, ULLFMT, ullData);
	szBuffer[63] = _t('\0');

	return SetParam(pszName, szBuffer);
}

CFormat& CFormat::SetParam(PCTSTR pszName, ll_t llData)
{
	tchar_t szBuffer[64];
	_sntprintf(szBuffer, 63, LLFMT, llData);
	szBuffer[63] = _t('\0');

	return SetParam(pszName, szBuffer);
}

CFormat& CFormat::SetParam(PCTSTR pszName, ulong_t ulData)
{
	tchar_t szBuffer[64];
	_sntprintf(szBuffer, 63, ULFMT, ulData);
	szBuffer[63] = _t('\0');

	return SetParam(pszName, szBuffer);
}

CFormat& CFormat::SetParam(PCTSTR pszName, uint_t uiData)
{
	tchar_t szBuffer[64];
	_sntprintf(szBuffer, 63, UIFMT, uiData);
	szBuffer[63] = _t('\0');

	return SetParam(pszName, szBuffer);
}

CFormat& CFormat::SetParam(PCTSTR pszName, int_t iData)
{
	tchar_t szBuffer[64];
	_sntprintf(szBuffer, 63, IFMT, iData);
	szBuffer[63] = _t('\0');

	return SetParam(pszName, szBuffer);
}

CFormat& CFormat::SetParam(PCTSTR pszName, bool bData)
{
	tchar_t szBuffer[64];
	_sntprintf(szBuffer, 63, USFMT, (ushort_t)bData);
	szBuffer[63] = _t('\0');

	return SetParam(pszName, szBuffer);
}

CTranslationItem::CTranslationItem() :
	m_pszText(NULL),
	m_uiChecksum(0),
	m_stTextLength(0)
{
}

CTranslationItem::CTranslationItem(const tchar_t* pszText, uint_t uiChecksum) :
	m_pszText(NULL),
	m_stTextLength(0),
	m_uiChecksum(uiChecksum)
{
	if(pszText)
	{
		m_stTextLength = _tcslen(pszText);
		if(m_stTextLength > 0)
		{
			m_pszText = new tchar_t[m_stTextLength + 1];
			_tcscpy(m_pszText, pszText);

			UnescapeString();
		}
	}
}

CTranslationItem::~CTranslationItem()
{
	Clear();
}

CTranslationItem& CTranslationItem::operator=(const CTranslationItem& rSrc)
{
	if(this != &rSrc)
	{
		Clear();
		if(rSrc.m_pszText)
		{
			m_stTextLength = rSrc.m_stTextLength;
			if(m_stTextLength > 0)
			{
				m_pszText = new tchar_t[rSrc.m_stTextLength + 1];
				_tcscpy(m_pszText, rSrc.m_pszText);
			}
		}
		m_uiChecksum = rSrc.m_uiChecksum;
	}

	return *this;
}

void CTranslationItem::Clear()
{
	delete [] m_pszText;
	m_pszText = NULL;
	m_stTextLength = 0;
	m_uiChecksum = 0;
}

void CTranslationItem::CalculateChecksum()
{
	if(m_pszText)
		m_uiChecksum = icpf::crc32((const byte_t*)m_pszText, m_stTextLength*sizeof(tchar_t));
	else
		m_uiChecksum = 0;
}

const tchar_t* CTranslationItem::GetText() const
{
	return m_pszText ? m_pszText : _t("");
}

void CTranslationItem::SetText(const tchar_t* pszText, bool bUnescapeString)
{
	delete [] m_pszText;
	if(pszText)
	{
		m_stTextLength = _tcslen(pszText);
		if(m_stTextLength > 0)
		{
			m_pszText = new tchar_t[m_stTextLength + 1];
			_tcscpy(m_pszText, pszText);
			if(bUnescapeString)
				UnescapeString();
			return;
		}
	}

	m_pszText = NULL;
	m_stTextLength = 0;
}

void CTranslationItem::UnescapeString()
{
	if(!m_pszText)
		return;

	const tchar_t* pszIn = m_pszText;
	tchar_t* pszOut = m_pszText;
	while (*pszIn != 0)
	{
		if (*pszIn == _T('\\'))
		{
			pszIn++;
			switch(*pszIn++)
			{
			case _T('t'):
				*pszOut++ = _T('\t');
				break;
			case _T('r'):
				*pszOut++ = _T('\r');
				break;
			case _T('n'):
				*pszOut++ = _T('\n');
				break;
			default:
				*pszOut++ = _T('\\');
			}
		}
		else
			*pszOut++ = *pszIn++;
	}
	*pszOut = _T('\0');
}

CTranslationItem::ECompareResult CTranslationItem::Compare(const CTranslationItem& rReferenceItem)
{
	if(!m_pszText || !rReferenceItem.m_pszText)
		return eResult_Invalid;

	if(m_uiChecksum != rReferenceItem.m_uiChecksum)
		return eResult_Invalid;

	// space check
	if(rReferenceItem.m_pszText[0] == _t(' ') && m_pszText[0] != _t(' '))
		return eResult_ContentWarning;
	
	size_t stReferenceLen = _tcslen(rReferenceItem.m_pszText);
	size_t stOwnLen = _tcslen(m_pszText);
	if(stReferenceLen > 0 && stOwnLen > 0 && rReferenceItem.m_pszText[stReferenceLen - 1] == _t(' ') && m_pszText[stOwnLen - 1] != _t(' '))
		return eResult_ContentWarning;

	// formatting strings check
	std::set<tstring_t> setRefFmt;
	if(!rReferenceItem.GetFormatStrings(setRefFmt))
		return eResult_ContentWarning;

	std::set<tstring_t> setThisFmt;
	if(!GetFormatStrings(setThisFmt))
		return eResult_ContentWarning;

	if(setRefFmt != setThisFmt)
		return eResult_ContentWarning;

	return eResult_Valid;
}

bool CTranslationItem::GetFormatStrings(std::set<tstring_t>& setFmtStrings) const
{
	setFmtStrings.clear();

	const tchar_t* pszData = m_pszText;
	const tchar_t* pszNext = NULL;
	const size_t stMaxFmt = 256;
	tchar_t szFmt[stMaxFmt];
	while((pszData = _tcschr(pszData, _t('%'))) != NULL)
	{
		pszData++;		// it works assuming the string is null-terminated

		// search the end of fmt string
		pszNext = pszData;
		while(*pszNext && isalpha(*pszNext))
			pszNext++;

		// if we have bigger buffer needs than is available
		if(pszNext - pszData >= stMaxFmt)
			return false;

		// copy data
		_tcsncpy(szFmt, pszData, pszNext - pszData);
		szFmt[pszNext - pszData] = _T('\0');

		setFmtStrings.insert(tstring_t(szFmt));
	}

	return true;
}

CLangData::CLangData() :
	m_pszFilename(NULL),
	m_pszLngName(NULL),
	m_pszFontFace(NULL),
	m_wPointSize(0),
	m_pszHelpName(NULL),
	m_pszAuthor(NULL),
	m_bRTL(false),
	m_uiSectionID(0),
	m_bUpdating(false),
	m_bModified(false)
{
}

CLangData::~CLangData()
{
	delete [] m_pszFilename;
	delete [] m_pszLngName;
	delete [] m_pszFontFace;
	delete [] m_pszHelpName;
	delete [] m_pszAuthor;
}

void CLangData::Clear()
{
	delete [] m_pszFilename;
	m_pszFilename = NULL;
	delete [] m_pszLngName;
	m_pszLngName = NULL;
	delete [] m_pszFontFace;
	m_pszFontFace = NULL;
	delete [] m_pszHelpName;
	m_pszHelpName = NULL;
	delete [] m_pszAuthor;
	m_pszAuthor = NULL;
	m_bModified = false;
	m_bRTL = false;
	m_bUpdating = false;
	m_uiSectionID = 0;
	m_wPointSize = 0;

	m_mapTranslation.clear();
}

CLangData::CLangData(const CLangData& ld) :
	m_pszFilename(NULL),
	m_pszLngName(NULL),
	m_pszFontFace(NULL),
	m_pszHelpName(NULL),
	m_pszAuthor(NULL),
	m_bRTL(ld.m_bRTL),
	m_bUpdating(ld.m_bUpdating),
	m_uiSectionID(ld.m_uiSectionID),
	m_wPointSize(ld.m_wPointSize),
	m_bModified(false)
{
	SetFilename(ld.GetFilename(true));
	SetLangName(ld.GetLangName());
	SetFontFace(ld.GetFontFace());
	SetPointSize(ld.GetPointSize());
	SetDirection(ld.GetDirection());
	SetHelpName(ld.GetHelpName());
	SetAuthor(ld.GetAuthor());

	m_mapTranslation.insert(ld.m_mapTranslation.begin(), ld.m_mapTranslation.end());
}

CLangData& CLangData::operator=(const CLangData& rSrc)
{
	if(this != &rSrc)
	{
		SetFilename(rSrc.GetFilename(true));
		SetLangName(rSrc.GetLangName());
		SetFontFace(rSrc.GetFontFace());
		SetPointSize(rSrc.GetPointSize());
		SetDirection(rSrc.GetDirection());
		SetHelpName(rSrc.GetHelpName());
		SetAuthor(rSrc.GetAuthor());
		m_bRTL = rSrc.m_bRTL;
		m_bUpdating = rSrc.m_bUpdating;
		m_uiSectionID = rSrc.m_uiSectionID;
		m_wPointSize = rSrc.m_wPointSize;
		m_bModified = false;

		m_mapTranslation.insert(rSrc.m_mapTranslation.begin(), rSrc.m_mapTranslation.end());
	}

	return *this;
}

bool CLangData::ReadInfo(PCTSTR pszFile)
{
	try
	{
		Clear();

		icpf::config cfg(icpf::config::eIni);
		const uint_t uiLangName = cfg.register_string(_T("Info/Lang Name"), _t(""));
		const uint_t uiFontFace = cfg.register_string(_T("Info/Font Face"), _T(""));
		const uint_t uiSize = cfg.register_signed_num(_T("Info/Size"), 0, 0, 0xffff);
		const uint_t uiRTL = cfg.register_bool(_T("Info/RTL reading order"), false);
		const uint_t uiHelpName = cfg.register_string(_T("Info/Help name"), _T(""));
		const uint_t uiAuthor = cfg.register_string(_T("Info/Author"), _T(""));
		const uint_t uiVersion = cfg.register_string(_T("Info/Format version"), _T("1"));

		cfg.read(pszFile);
		
		// we don't support old language versions
		const tchar_t* pszVersion = cfg.get_string(uiVersion);
		if(_tcscmp(pszVersion, TRANSLATION_FORMAT_VERSION) != 0)
			return false;

		const tchar_t* psz = cfg.get_string(uiLangName);
		if(!psz || psz[0] == _t('\0'))
			return false;
		SetLangName(psz);

		psz = cfg.get_string(uiFontFace);
		if(!psz || psz[0] == _t('\0'))
			return false;
		SetFontFace(psz);

		ll_t ll = cfg.get_signed_num(uiSize);
		if(ll == 0)
			return false;
		SetPointSize((WORD)ll);

		SetDirection(cfg.get_bool(uiRTL));

		psz = cfg.get_string(uiHelpName);
		if(!psz || psz[0] == _t('\0'))
			return false;
		SetHelpName(psz);

		psz = cfg.get_string(uiAuthor);
		if(!psz || psz[0] == _t('\0'))
			return false;
		SetAuthor(psz);

		SetFilename(pszFile);

		m_bModified = false;

		return true;
	}
	catch(...)
	{
		return false;
	}
}

void CLangData::EnumAttributesCallback(bool bGroup, const tchar_t* pszName, const tchar_t* pszValue, ptr_t pData)
{
	CLangData* pLangData = (CLangData*)pData;
	assert(pLangData);
	assert(pszName);
	if(!pLangData || !pszName)
		return;

	if(bGroup && _tcsicmp(pszName, _t("Info")) == 0)
		return;
	if(bGroup)
	{
		// new section - remember in member
		pLangData->m_uiSectionID = _ttoi(pszName);
	}
	else
	{
		uint_t uiID = 0;
		uint_t uiChecksum = 0;

		// parse the pszName to get both the string id and checksum
		const tchar_t* pszChecksum = _tcschr(pszName, _T('['));
		if(pszChecksum == NULL)
		{
			TRACE(_T("Warning! Old-style translation string %s.\n"), pszName);

			int iCount = _stscanf(pszName, UIFMT, &uiID);
			if(iCount != 1)
			{
				TRACE(_T("Warning! Problem retrieving id from string '%s'\n"), pszName);
				return;
			}
		}
		else
		{
			int iCount = _stscanf(pszName, UIFMT _T("[0x%lx]"), &uiID, &uiChecksum);
			if(iCount != 2)
			{
				TRACE(_T("Warning! Problem retrieving id/checksum from string '%s'\n"), pszName);
				return;
			}
		}

		uint_t uiKey = pLangData->m_uiSectionID << 16 | uiID;
		translation_map::iterator itTranslation = pLangData->m_mapTranslation.end();
		if(pLangData->m_bUpdating)
		{
			// check if the checksum exists and matches
			itTranslation = pLangData->m_mapTranslation.find(uiKey);
			if(itTranslation == pLangData->m_mapTranslation.end())
			{
				TRACE(_T("Warning! Superfluous entry %lu in processed language file\n"), uiKey);
				return;		// entry not found - probably superfluous entry in the language file
			}

			if((*itTranslation).second.GetChecksum() != uiChecksum)
			{
				TRACE(_T("Warning! Invalid checksum for string ID %lu in processed language file\n"), uiKey);
				return;		// entry has invalid checksum (older version of translation)
			}
		}
		else
		{
			std::pair<translation_map::iterator, bool> pairTranslation = pLangData->m_mapTranslation.insert(translation_map::value_type(uiKey, CTranslationItem()));
			itTranslation = pairTranslation.first;
		}

		assert(itTranslation != pLangData->m_mapTranslation.end());
		if(itTranslation != pLangData->m_mapTranslation.end())
		{
			(*itTranslation).second.SetText(pszValue, true);
			if(!pLangData->m_bUpdating)
				(*itTranslation).second.SetChecksum(uiChecksum);
		}
	}
}

void CLangData::UnescapeString(tchar_t* pszData)
{
	tchar_t* pszOut = pszData;
	while (*pszData != 0)
	{
		if (*pszData == _T('\\'))
		{
			pszData++;
			switch(*pszData++)
			{
			case _T('t'):
				*pszOut++ = _T('\t');
				break;
			case _T('r'):
				*pszOut++ = _T('\r');
				break;
			case _T('n'):
				*pszOut++ = _T('\n');
				break;
			default:
				*pszOut++ = _T('\\');
			}
		}
		else
			*pszOut++ = *pszData++;
	}
	*pszOut = _T('\0');

}

bool CLangData::ReadTranslation(PCTSTR pszFile, bool bUpdateTranslation, bool bIgnoreVersion)
{
	try
	{
		if(!bUpdateTranslation)
			Clear();

		// load data from file
		icpf::config cfg(icpf::config::eIni);
		const uint_t uiLangName = cfg.register_string(_T("Info/Lang Name"), _t(""));
		const uint_t uiFontFace = cfg.register_string(_T("Info/Font Face"), _T(""));
		const uint_t uiSize = cfg.register_signed_num(_T("Info/Size"), 0, 0, 0xffff);
		const uint_t uiRTL = cfg.register_bool(_T("Info/RTL reading order"), false);
		const uint_t uiHelpName = cfg.register_string(_T("Info/Help name"), _T(""));
		const uint_t uiAuthor = cfg.register_string(_T("Info/Author"), _T(""));
		const uint_t uiVersion = cfg.register_string(_T("Info/Format version"), _T("1"));

		cfg.read(pszFile);

		// we don't support old language versions unless requested specifically
		if(!bIgnoreVersion)
		{
			const tchar_t* pszVersion = cfg.get_string(uiVersion);
			if(_tcscmp(pszVersion, TRANSLATION_FORMAT_VERSION) != 0)
				return false;
		}

		const tchar_t* psz = cfg.get_string(uiLangName);
		if(!psz || psz[0] == _t('\0'))
			return false;
		SetLangName(psz);

		psz = cfg.get_string(uiFontFace);
		if(!psz || psz[0] == _t('\0'))
			return false;
		SetFontFace(psz);

		ll_t ll = cfg.get_signed_num(uiSize);
		if(ll == 0)
			return false;
		SetPointSize((WORD)ll);

		SetDirection(cfg.get_bool(uiRTL));

		psz = cfg.get_string(uiHelpName);
		if(!psz || psz[0] == _t('\0'))
			return false;
		SetHelpName(psz);

		psz = cfg.get_string(uiAuthor);
		if(!psz || psz[0] == _t('\0'))
			return false;
		SetAuthor(psz);

		m_bUpdating = bUpdateTranslation;
		m_uiSectionID = 0;
		if(!cfg.enum_properties(_t("*"), EnumAttributesCallback, this))
		{
			m_bUpdating = false;
			return false;
		}
		m_bUpdating = false;

		SetFilename(pszFile);

		m_bModified = false;

		return true;
	}
	catch(...)
	{
		return false;
	}
}

void CLangData::WriteTranslation(PCTSTR pszPath)
{
	if(!IsValidDescription())
		THROW(_t("Invalid translation information (author, name or point size)"), 0, 0, 0);

	// real writing
	const int iBufferSize = 256;
	tchar_t szTemp[iBufferSize];

	// load data from file
	icpf::config cfg(icpf::config::eIni);
	cfg.set_string(_t("Info/Lang Name"), m_pszLngName);
	cfg.set_string(_T("Info/Font Face"), m_pszFontFace);
	cfg.set_string(_T("Info/Size"), _itot(m_wPointSize, szTemp, 10));
	cfg.set_string(_T("Info/RTL reading order"), m_bRTL ? _T("1") : _T("0"));
	cfg.set_string(_T("Info/Help name"), m_pszHelpName);
	cfg.set_string(_T("Info/Author"), m_pszAuthor);
	cfg.set_string(_T("Info/Format version"), TRANSLATION_FORMAT_VERSION);

	tstring_t strText;
	for(translation_map::iterator it = m_mapTranslation.begin(); it != m_mapTranslation.end(); it++)
	{
		uint_t uiKey = (*it).first;
		_sntprintf(szTemp, iBufferSize - 1, UIFMT _T("/") UIFMT _T("[") UIXFMT _T("]"), (uiKey >> 16), uiKey & 0x0000ffff, (*it).second.GetChecksum());

		strText = (*it).second.GetText();
		tstring_t::size_type stPos;
		while((stPos = strText.find_first_of(_t("\r\n\t"))) != tstring_t::npos)
		{
			switch(strText[stPos])
			{
			case _t('\r'):
				strText.replace(stPos, 1, _t("\\r"));
				break;
			case _t('\n'):
				strText.replace(stPos, 1, _t("\\n"));
				break;
			case _t('\t'):
				strText.replace(stPos, 1, _t("\\t"));
				break;
			}
		}

		cfg.set_string(szTemp, strText.c_str());
	}

	if(pszPath == NULL)
		pszPath = m_pszFilename;
	else
		SetFilename(pszPath);
	cfg.write(pszPath);

	m_bModified = false;
}

PCTSTR CLangData::GetString(WORD wHiID, WORD wLoID)
{
	translation_map::const_iterator it=m_mapTranslation.find((wHiID << 16) | wLoID);
	if (it != m_mapTranslation.end())
		return (*it).second.GetText();
	else
		return EMPTY_STRING;
}

void CLangData::EnumStrings(PFNENUMCALLBACK pfnCallback, ptr_t pData)
{
	for(translation_map::const_iterator iterTranslation = m_mapTranslation.begin(); iterTranslation != m_mapTranslation.end(); ++iterTranslation)
	{
		(*pfnCallback)((*iterTranslation).first, &(*iterTranslation).second, pData);
	}
}

CTranslationItem* CLangData::GetTranslationItem(uint_t uiTranslationKey, bool bCreate)
{
	translation_map::iterator iterTranslation = m_mapTranslation.find(uiTranslationKey);
	if(iterTranslation != m_mapTranslation.end())
		return &(*iterTranslation).second;
	else
	{
		if(bCreate)
		{
			std::pair<translation_map::iterator, bool> pairTranslation = m_mapTranslation.insert(std::make_pair(uiTranslationKey, CTranslationItem()));
			if(pairTranslation.second)
			{
				m_bModified = true;
				return &(*pairTranslation.first).second;
			}
		}
	}

	return NULL;
}

bool CLangData::Exists(uint_t uiTranslationKey) const
{
	return m_mapTranslation.find(uiTranslationKey) != m_mapTranslation.end();
}

// removes strings that does not exist in the reference translation
void CLangData::CleanupTranslation(const CLangData& rReferenceTranslation)
{
	translation_map::iterator iterTranslation = m_mapTranslation.begin();
	while(iterTranslation != m_mapTranslation.end())
	{
		if(!rReferenceTranslation.Exists((*iterTranslation).first))
		{
			m_bModified = true;
			m_mapTranslation.erase(iterTranslation++);
		}
		else
			++iterTranslation;
	}
}

void CLangData::SetFilename(PCTSTR psz)
{
	if (m_pszFilename)
		delete [] m_pszFilename;

	// copy
	m_pszFilename=new TCHAR[_tcslen(psz)+1];
	_tcscpy(m_pszFilename, psz);

	m_bModified = true;
}

PCTSTR CLangData::GetFilename(bool bFullPath) const
{
	if (bFullPath)
		return m_pszFilename;
	else
	{
		if(m_pszFilename)
		{
			TCHAR *pszFnd=_tcsrchr(m_pszFilename, _T('\\'));
			if (pszFnd)
				return pszFnd+1;
		}

		return m_pszFilename;
	}
}

void CLangData::SetLangName(PCTSTR psz)
{
	if (m_pszLngName)
		delete [] m_pszLngName;
	m_pszLngName=new TCHAR[_tcslen(psz)+1];
	_tcscpy(m_pszLngName, psz);
	m_bModified = true;
}

void CLangData::SetFontFace(PCTSTR psz)
{
	if (m_pszFontFace)
		delete [] m_pszFontFace;
	m_pszFontFace=new TCHAR[_tcslen(psz)+1];
	_tcscpy(m_pszFontFace, psz);
	m_bModified = true;
}

void CLangData::SetHelpName(PCTSTR psz)
{
	SetFnameData(&m_pszHelpName, psz);
	m_bModified = true;
}

void CLangData::SetAuthor(PCTSTR psz)
{
	if (m_pszAuthor)
		delete [] m_pszAuthor;
	m_pszAuthor=new TCHAR[_tcslen(psz)+1];
	_tcscpy(m_pszAuthor, psz);
	m_bModified = true;
}

bool CLangData::IsValidDescription() const
{
	// basic sanity checks
	if(!m_pszAuthor || m_pszAuthor[0] == _t('\0') ||
		!m_pszLngName || m_pszLngName[0] == _t('\0') ||
		!m_pszFontFace || m_pszFontFace[0] == _t('\0') ||
		m_wPointSize == 0)
		return false;
	return true;
}

void CLangData::SetFnameData(PTSTR *ppszDst, PCTSTR pszSrc)
{
	if (*ppszDst)
		delete [] (*ppszDst);
	const TCHAR* pszLast=NULL;
	if ( (pszLast=_tcsrchr(pszSrc, _T('\\'))) != NULL)
		pszLast++;
	else
		pszLast=pszSrc;

	// copy
	*ppszDst=new TCHAR[_tcslen(pszLast)+1];
	_tcscpy(*ppszDst, pszLast);
}

CResourceManager::CResourceManager() :
	m_pfnCallback(NULL),
	m_hRes(NULL)
{
	InitializeCriticalSection(&m_cs);
}

CResourceManager::~CResourceManager()
{
	DeleteCriticalSection(&m_cs);
}

CResourceManager& CResourceManager::Acquire()
{
	return CResourceManager::S_ResourceManager;
}

void CResourceManager::Init(HMODULE hrc)
{
	m_hRes=hrc;
}

// requires the param with ending '\\'
void CResourceManager::Scan(LPCTSTR pszFolder, vector<CLangData>* pvData)
{
	assert(pszFolder);
	assert(pvData);
	if(!pszFolder || !pvData)
		return;

	TCHAR szPath[_MAX_PATH];
	_tcscpy(szPath, pszFolder);
	_tcscat(szPath, _T("*.lng"));
	
	WIN32_FIND_DATA wfd;
	HANDLE hFind=::FindFirstFile(szPath, &wfd);
	BOOL bFound=TRUE;
	CLangData ld;
	while (bFound && hFind != INVALID_HANDLE_VALUE)
	{
		if (!(wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
		{
			_tcscpy(szPath, pszFolder);
			_tcscat(szPath, wfd.cFileName);
			if (ld.ReadInfo(szPath))
				pvData->push_back(ld);
		}

		bFound=::FindNextFile(hFind, &wfd);
	}

	if (hFind != INVALID_HANDLE_VALUE)
		::FindClose(hFind);
}

bool CResourceManager::SetLanguage(PCTSTR pszPath)
{
	bool bRet = false;
	tchar_t szPath[_MAX_PATH];

	// parse the path to allow reading the english language first
	const tchar_t* pszBaseName = _t("english.lng");
	if(_tcsstr(pszPath, pszBaseName) != NULL)
	{
		_tcscpy(szPath, pszPath);
		pszPath = NULL;
	}
	else
	{
		const tchar_t* pszData = _tcsrchr(pszPath, _t('\\'));
		if(pszData != NULL)
		{
			memset(szPath, 0, _MAX_PATH*sizeof(tchar_t));
			_tcsncpy(szPath, pszPath, pszData - pszPath + 1);
			szPath[_MAX_PATH - 1] = _T('\0');
			_tcscat(szPath, pszBaseName);
		}
		else
			_tcscpy(szPath, pszPath);
	}

	// and load everything
	EnterCriticalSection(&m_cs);
	try
	{
		bRet = m_ld.ReadTranslation(szPath);		// base language
		if(bRet && pszPath)
			bRet=m_ld.ReadTranslation(pszPath, true);	// real language
	}
	catch(...)
	{
		LeaveCriticalSection(&m_cs);
		return false;
	}
	LeaveCriticalSection(&m_cs);
	if (!bRet)
		return false;
	
	// update registered dialog boxes
	list<CWnd*>::iterator it=m_lhDialogs.begin();
	while (it != m_lhDialogs.end())
	{
		if (::IsWindow((*it)->m_hWnd))
			(*it)->PostMessage(WM_RMNOTIFY, RMNT_LANGCHANGE, 0);
		it++;
	}
				
	// send the notification stuff to the others
	if (m_pfnCallback)
		(*m_pfnCallback)(RMNT_LANGCHANGE);

	return bRet;
}

HGLOBAL CResourceManager::LoadResource(LPCTSTR pszType, LPCTSTR pszName)
{
	EnterCriticalSection(&m_cs);

	// find resource
	HGLOBAL hRet=NULL;
	HRSRC hr=FindResource(m_hRes, pszName, pszType);
	if (hr)
		hRet=::LoadResource(m_hRes, hr);

	LeaveCriticalSection(&m_cs);
	return hRet;
}

HACCEL CResourceManager::LoadAccelerators(LPCTSTR pszName)
{
	return ::LoadAccelerators(m_hRes, pszName);
}

HBITMAP CResourceManager::LoadBitmap(LPCTSTR pszName)
{
	return ::LoadBitmap(m_hRes, pszName);
}

HCURSOR CResourceManager::LoadCursor(LPCTSTR pszName)
{
	return ::LoadCursor(m_hRes, pszName);
}

HICON CResourceManager::LoadIcon(LPCTSTR pszName)
{
	return ::LoadIcon(m_hRes, pszName);
}

void CResourceManager::UpdateMenu(HMENU hMenu, WORD wMenuID)
{
	// change the strings inside the menu to the one from txt res file
	int iCount=::GetMenuItemCount(hMenu);
	MENUITEMINFO mif;
	WORD wLoID;
	TCHAR szItem[1024];
	memset(szItem, 0, 1024);
	for (int i=0;i<iCount;i++)
	{
		memset(&mif, 0, sizeof(MENUITEMINFO));
		mif.cbSize=sizeof(MENUITEMINFO);
		mif.dwTypeData=szItem;
		mif.cch=1023;
		mif.fMask=MIIM_FTYPE | MIIM_SUBMENU | MIIM_ID | MIIM_DATA | MIIM_STRING;
		if (::GetMenuItemInfo(hMenu, i, TRUE, &mif))
		{
			// has sub items ?
			if (mif.hSubMenu)
				UpdateMenu(mif.hSubMenu, wMenuID);

			// the menu item contains a string to update
			if (mif.fType == MFT_STRING)
			{
				if (mif.hSubMenu)
				{
					if (mif.dwItemData != 0)
						wLoID=(WORD)mif.dwItemData;		// already updated data
					else
					{
						// fresh menu - try to update info from caption
						wLoID=(WORD)_ttoi((PCTSTR)mif.dwTypeData);

						// remember this info in item's private storage
						MENUITEMINFO ii;
						ii.cbSize=sizeof(MENUITEMINFO);
						ii.fMask=MIIM_DATA;
						::SetMenuItemInfo(hMenu, i, TRUE, &ii);
					}
				}
				else
					wLoID=(WORD)::GetMenuItemID(hMenu, i);

				mif.fMask=MIIM_STRING | MIIM_FTYPE;
				if (m_ld.GetDirection())
					mif.fType |= MFT_RIGHTORDER;
				else
					mif.fType &= ~MFT_RIGHTORDER;
				mif.dwTypeData=(LPTSTR)(m_ld.GetString(wMenuID, wLoID));
				::SetMenuItemInfo(hMenu, i, TRUE, &mif);
			}
		}
	}
}

HMENU CResourceManager::LoadMenu(LPCTSTR pszName)
{
	EnterCriticalSection(&m_cs);
	HMENU hMenu=::LoadMenu(m_hRes, pszName);

	if (hMenu && IS_INTRESOURCE(pszName))
		UpdateMenu(hMenu, (WORD)pszName);

	LeaveCriticalSection(&m_cs);
	return hMenu;
}

LPDLGTEMPLATE CResourceManager::LoadDialog(LPCTSTR pszName)
{
	HGLOBAL hgl=LoadResource(RT_DIALOG, pszName);
	DLGTEMPLATE *dlgt=(DLGTEMPLATE*)::LockResource(hgl);
	return dlgt;
}

PCTSTR CResourceManager::LoadString(UINT uiID)
{
	EnterCriticalSection(&m_cs);
	PCTSTR pszData=m_ld.GetString(0, (WORD)uiID);
	LeaveCriticalSection(&m_cs);
	
	return pszData;
}

PCTSTR CResourceManager::LoadString(WORD wGroup, WORD wID)
{
	EnterCriticalSection(&m_cs);
	PCTSTR pszData=m_ld.GetString(wGroup, wID);
	LeaveCriticalSection(&m_cs);
	return pszData;
}

PTSTR CResourceManager::LoadStringCopy(UINT uiID, PTSTR pszStr, UINT uiMax)
{
	EnterCriticalSection(&m_cs);
	_tcsncpy(pszStr, m_ld.GetString(0, (WORD)uiID), uiMax-1);
	pszStr[uiMax-1]=_T('\0');
	LeaveCriticalSection(&m_cs);
	return pszStr;
}

HANDLE CResourceManager::LoadImage(LPCTSTR lpszName, UINT uType, int cxDesired, int cyDesired, UINT fuLoad)
{
	EnterCriticalSection(&m_cs);

	HANDLE hImg=::LoadImage(m_hRes, lpszName, uType, cxDesired, cyDesired, fuLoad);

	LeaveCriticalSection(&m_cs);
	return hImg;
}

END_ICTRANSLATE_NAMESPACE
