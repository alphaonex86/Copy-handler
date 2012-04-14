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
#include "FileInfo.h"
#include "FileFilter.h"

////////////////////////////////////////////////////////////////////////////
bool _tcicmp(TCHAR c1, TCHAR c2)
{
	TCHAR ch1[2]={c1, 0}, ch2[2]={c2, 0};
	return (_tcsicmp(ch1, ch2) == 0);
}

CFileFilter::CFileFilter()
{
	// files mask
	m_bUseMask=false;
	m_astrMask.RemoveAll();

	m_bUseExcludeMask=false;
	m_astrExcludeMask.RemoveAll();

	// size filtering
	m_bUseSize=false;
	m_iSizeType1=GT;
	m_ullSize1=0;
	m_bUseSize2=false;
	m_iSizeType2=LT;
	m_ullSize2=0;

	// date filtering
	m_bUseDate=false;
	m_iDateType=DATE_CREATED;
	m_iDateType1=GT;
	m_bDate1=false;
	m_tDate1=CTime::GetCurrentTime();
	m_bTime1=false;
	m_tTime1=CTime::GetCurrentTime();

	m_bUseDate2=false;
	m_iDateType2=LT;
	m_bDate2=false;
	m_tDate2=CTime::GetCurrentTime();
	m_bTime2=false;
	m_tTime2=CTime::GetCurrentTime();

	// attribute filtering
	m_bUseAttributes=false;
	m_iArchive=2;
	m_iReadOnly=2;
	m_iHidden=2;
	m_iSystem=2;
	m_iDirectory=2;
}

CFileFilter::CFileFilter(const CFileFilter& rFilter)
{
	*this=rFilter;
}

CFileFilter& CFileFilter::operator=(const CFileFilter& rFilter)
{
	// files mask
	m_bUseMask=rFilter.m_bUseMask;
	m_astrMask.Copy(rFilter.m_astrMask);

	m_bUseExcludeMask=rFilter.m_bUseExcludeMask;
	m_astrExcludeMask.Copy(rFilter.m_astrExcludeMask);

	// size filtering
	m_bUseSize=rFilter.m_bUseSize;
	m_iSizeType1=rFilter.m_iSizeType1;
	m_ullSize1=rFilter.m_ullSize1;
	m_bUseSize2=rFilter.m_bUseSize2;
	m_iSizeType2=rFilter.m_iSizeType2;
	m_ullSize2=rFilter.m_ullSize2;

	// date filtering
	m_bUseDate=rFilter.m_bUseDate;
	m_iDateType=rFilter.m_iDateType;
	m_iDateType1=rFilter.m_iDateType1;
	m_bDate1=rFilter.m_bDate1;
	m_tDate1=rFilter.m_tDate1;
	m_bTime1=rFilter.m_bTime1;
	m_tTime1=rFilter.m_tTime1;

	m_bUseDate2=rFilter.m_bUseDate2;
	m_iDateType2=rFilter.m_iDateType2;
	m_bDate2=rFilter.m_bDate2;
	m_tDate2=rFilter.m_tDate2;
	m_bTime2=rFilter.m_bTime2;
	m_tTime2=rFilter.m_tTime2;

	// attribute filtering
	m_bUseAttributes=rFilter.m_bUseAttributes;
	m_iArchive=rFilter.m_iArchive;
	m_iReadOnly=rFilter.m_iReadOnly;
	m_iHidden=rFilter.m_iHidden;
	m_iSystem=rFilter.m_iSystem;
	m_iDirectory=rFilter.m_iDirectory;

	return *this;
}

void CFileFilter::Serialize(icpf::archive& ar)
{
	ULARGE_INTEGER li;
	if (ar.is_storing())
	{
		// store
		// files mask
		ar<<static_cast<unsigned char>(m_bUseMask);
		ar<<m_astrMask;

		ar<<static_cast<unsigned char>(m_bUseExcludeMask);
		//		ar<<m_astrExcludeMask;

		// size filtering
		ar<<static_cast<unsigned char>(m_bUseSize);
		ar<<m_iSizeType1;
		li.QuadPart=m_ullSize1;
		ar<<li.LowPart;
		ar<<li.HighPart;
		ar<<static_cast<unsigned char>(m_bUseSize2);
		ar<<m_iSizeType2;
		li.QuadPart=m_ullSize2;
		ar<<li.LowPart;
		ar<<li.HighPart;

		// date
		ar<<static_cast<unsigned char>(m_bUseDate);
		ar<<m_iDateType;
		ar<<m_iDateType1;
		ar<<static_cast<unsigned char>(m_bDate1);
		ar<<m_tDate1;
		ar<<static_cast<unsigned char>(m_bTime1);
		ar<<m_tTime1;

		ar<<static_cast<unsigned char>(m_bUseDate2);
		ar<<m_iDateType2;
		ar<<static_cast<unsigned char>(m_bDate2);
		ar<<m_tDate2;
		ar<<static_cast<unsigned char>(m_bTime2);
		ar<<m_tTime2;

		// attributes
		ar<<static_cast<unsigned char>(m_bUseAttributes);
		ar<<m_iArchive;
		ar<<m_iReadOnly;
		ar<<m_iHidden;
		ar<<m_iSystem;
		ar<<m_iDirectory;
	}
	else
	{
		// read
		unsigned char tmp;

		// files mask
		ar>>tmp;
		m_bUseMask=(tmp != 0);
		ar>>m_astrMask;

		ar>>tmp;
		m_bUseExcludeMask=(tmp != 0);
		ar>>m_astrExcludeMask;

		// size
		ar>>tmp;
		m_bUseSize=(tmp != 0);
		ar>>m_iSizeType1;
		ar>>li.LowPart;
		ar>>li.HighPart;
		m_ullSize1=li.QuadPart;
		ar>>tmp;
		m_bUseSize2=(tmp != 0);
		ar>>m_iSizeType2;
		ar>>li.LowPart;
		ar>>li.HighPart;
		m_ullSize2=li.QuadPart;

		// date
		ar>>tmp;
		m_bUseDate=(tmp != 0);
		ar>>m_iDateType;
		ar>>m_iDateType1;
		ar>>tmp;
		m_bDate1=(tmp != 0);
		ar>>m_tDate1;
		ar>>tmp;
		m_bTime1=(tmp != 0);
		ar>>m_tTime1;

		ar>>tmp;
		m_bUseDate2=(tmp != 0);
		ar>>m_iDateType2;
		ar>>tmp;
		m_bDate2=(tmp != 0);
		ar>>m_tDate2;
		ar>>tmp;
		m_bTime2=(tmp != 0);
		ar>>m_tTime2;

		// attributes
		ar>>tmp;
		m_bUseAttributes=(tmp != 0);
		ar>>m_iArchive;
		ar>>m_iReadOnly;
		ar>>m_iHidden;
		ar>>m_iSystem;
		ar>>m_iDirectory;
	}
}

CString& CFileFilter::GetCombinedMask(CString& pMask) const
{
	pMask.Empty();
	if (m_astrMask.GetSize() > 0)
	{
		pMask=m_astrMask.GetAt(0);
		for (int i=1;i<m_astrMask.GetSize();i++)
			pMask+=_T("|")+m_astrMask.GetAt(i);
	}

	return pMask;
}

void CFileFilter::SetCombinedMask(const CString& pMask)
{
	m_astrMask.RemoveAll();

	TCHAR *pszData=new TCHAR[pMask.GetLength()+1];
	_tcscpy(pszData, pMask);

	TCHAR *szToken=_tcstok(pszData, _T("|"));
	while (szToken != NULL)
	{
		// add token to a table
		m_astrMask.Add(szToken);

		// search for next
		szToken=_tcstok(NULL, _T("|"));
	}

	delete [] pszData;
}

CString& CFileFilter::GetCombinedExcludeMask(CString& pMask) const
{
	pMask.Empty();
	if (m_astrExcludeMask.GetSize() > 0)
	{
		pMask=m_astrExcludeMask.GetAt(0);
		for (int i=1;i<m_astrExcludeMask.GetSize();i++)
			pMask+=_T("|")+m_astrExcludeMask.GetAt(i);
	}

	return pMask;
}

void CFileFilter::SetCombinedExcludeMask(const CString& pMask)
{
	m_astrExcludeMask.RemoveAll();

	TCHAR *pszData=new TCHAR[pMask.GetLength()+1];
	_tcscpy(pszData, pMask);

	TCHAR *szToken=_tcstok(pszData, _T("|"));
	while (szToken != NULL)
	{
		// add token
		m_astrExcludeMask.Add(szToken);

		// find next
		szToken=_tcstok(NULL, _T("|"));
	}

	delete [] pszData;
}

bool CFileFilter::Match(const CFileInfo& rInfo) const
{
	// check by mask
	if (m_bUseMask)
	{
		bool bRes=false;
		for (int i=0;i<m_astrMask.GetSize();i++)
		{
			if (MatchMask(m_astrMask.GetAt(i), rInfo.GetFileName()))
				bRes=true;
		}
		if (!bRes)
			return false;
	}

	// excluding mask
	if (m_bUseExcludeMask)
	{
		for (int i=0;i<m_astrExcludeMask.GetSize();i++)
		{
			if (MatchMask(m_astrExcludeMask.GetAt(i), rInfo.GetFileName()))
				return false;
		}
	}

	// by size
	if (m_bUseSize)
	{
		switch (m_iSizeType1)
		{
		case LT:
			if (m_ullSize1 <= rInfo.GetLength64())
				return false;
			break;
		case LE:
			if (m_ullSize1 < rInfo.GetLength64())
				return false;
			break;
		case EQ:
			if (m_ullSize1 != rInfo.GetLength64())
				return false;
			break;
		case GE:
			if (m_ullSize1 > rInfo.GetLength64())
				return false;
			break;
		case GT:
			if (m_ullSize1 >= rInfo.GetLength64())
				return false;
			break;
		}

		// second part
		if (m_bUseSize2)
		{
			switch (m_iSizeType2)
			{
			case LT:
				if (m_ullSize2 <= rInfo.GetLength64())
					return false;
				break;
			case LE:
				if (m_ullSize2 < rInfo.GetLength64())
					return false;
				break;
			case EQ:
				if (m_ullSize2 != rInfo.GetLength64())
					return false;
				break;
			case GE:
				if (m_ullSize2 > rInfo.GetLength64())
					return false;
				break;
			case GT:
				if (m_ullSize2 >= rInfo.GetLength64())
					return false;
				break;
			}
		}
	}

	// date - get the time from rInfo
	if (m_bUseDate)
	{
		COleDateTime tm;
		switch (m_iDateType)
		{
		case DATE_CREATED:
			tm=rInfo.GetCreationTime();
			break;
		case DATE_MODIFIED:
			tm=rInfo.GetLastWriteTime();
			break;
		case DATE_LASTACCESSED:
			tm=rInfo.GetLastAccessTime();
			break;
		}

		// counting...
		unsigned long ulInfo=0, ulCheck=0;
		if (m_bDate1)
		{
			ulInfo=(tm.GetYear()-1970)*32140800+tm.GetMonth()*2678400+tm.GetDay()*86400;
			ulCheck=(m_tDate1.GetYear()-1970)*32140800+m_tDate1.GetMonth()*2678400+m_tDate1.GetDay()*86400;
		}

		if (m_bTime1)
		{
			ulInfo+=tm.GetHour()*3600+tm.GetMinute()*60+tm.GetSecond();
			ulCheck+=m_tTime1.GetHour()*3600+m_tTime1.GetMinute()*60+m_tTime1.GetSecond();
		}

		// ... and comparing
		switch (m_iDateType1)
		{
		case LT:
			if (ulInfo >= ulCheck)
				return false;
			break;
		case LE:
			if (ulInfo > ulCheck)
				return false;
			break;
		case EQ:
			if (ulInfo != ulCheck)
				return false;
			break;
		case GE:
			if (ulInfo < ulCheck)
				return false;
			break;
		case GT:
			if (ulInfo <= ulCheck)
				return false;
			break;
		}

		if (m_bUseDate2)
		{
			// counting...
			ulInfo=0, ulCheck=0;
			if (m_bDate2)
			{
				ulInfo=(tm.GetYear()-1970)*32140800+tm.GetMonth()*2678400+tm.GetDay()*86400;
				ulCheck=(m_tDate2.GetYear()-1970)*32140800+m_tDate2.GetMonth()*2678400+m_tDate2.GetDay()*86400;
			}

			if (m_bTime2)
			{
				ulInfo+=tm.GetHour()*3600+tm.GetMinute()*60+tm.GetSecond();
				ulCheck+=m_tTime2.GetHour()*3600+m_tTime2.GetMinute()*60+m_tTime2.GetSecond();
			}

			// ... comparing
			switch (m_iDateType2)
			{
			case LT:
				if (ulInfo >= ulCheck)
					return false;
				break;
			case LE:
				if (ulInfo > ulCheck)
					return false;
				break;
			case EQ:
				if (ulInfo != ulCheck)
					return false;
				break;
			case GE:
				if (ulInfo < ulCheck)
					return false;
				break;
			case GT:
				if (ulInfo <= ulCheck)
					return false;
				break;
			}
		}
	} // of m_bUseDate

	// attributes
	if (m_bUseAttributes)
	{
		if ( (m_iArchive == 1 && !rInfo.IsArchived()) || (m_iArchive == 0 && rInfo.IsArchived()))
			return false;
		if ( (m_iReadOnly == 1 && !rInfo.IsReadOnly()) || (m_iReadOnly == 0 && rInfo.IsReadOnly()))
			return false;
		if ( (m_iHidden == 1 && !rInfo.IsHidden()) || (m_iHidden == 0 && rInfo.IsHidden()))
			return false;
		if ( (m_iSystem == 1 && !rInfo.IsSystem()) || (m_iSystem == 0 && rInfo.IsSystem()))
			return false;
		if ( (m_iDirectory == 1 && !rInfo.IsDirectory()) || (m_iDirectory == 0 && rInfo.IsDirectory()))
			return false;
	}

	return true;
}

bool CFileFilter::MatchMask(LPCTSTR lpszMask, LPCTSTR lpszString) const
{
	bool bMatch = 1;

	//iterate and delete '?' and '*' one by one
	while(*lpszMask != _T('\0') && bMatch && *lpszString != _T('\0'))
	{
		if (*lpszMask == _T('?')) lpszString++;
		else if (*lpszMask == _T('*'))
		{
			bMatch = Scan(lpszMask, lpszString);
			lpszMask--;
		}
		else
		{
			bMatch = _tcicmp(*lpszMask, *lpszString);
			lpszString++;
		}
		lpszMask++;
	}
	while (*lpszMask == _T('*') && bMatch) lpszMask++;

	return bMatch && *lpszString == _T('\0') && *lpszMask == _T('\0');
}

// scan '?' and '*'
bool CFileFilter::Scan(LPCTSTR& lpszMask, LPCTSTR& lpszString) const
{
	// remove the '?' and '*'
	for(lpszMask++; *lpszString != _T('\0') && (*lpszMask == _T('?') || *lpszMask == _T('*')); lpszMask++)
		if (*lpszMask == _T('?')) lpszString++;
	while ( *lpszMask == _T('*')) lpszMask++;

	// if lpszString is empty and lpszMask has more characters or,
	// lpszMask is empty, return 
	if (*lpszString == _T('\0') && *lpszMask != _T('\0')) return false;
	if (*lpszString == _T('\0') && *lpszMask == _T('\0')) return true; 
	// else search substring
	else
	{
		LPCTSTR wdsCopy = lpszMask;
		LPCTSTR lpszStringCopy = lpszString;
		bool bMatch = true;
		do 
		{
			if (!MatchMask(lpszMask, lpszString)) lpszStringCopy++;
			lpszMask = wdsCopy;
			lpszString = lpszStringCopy;
			while (!(_tcicmp(*lpszMask, *lpszString)) && (*lpszString != '\0')) lpszString++;
			wdsCopy = lpszMask;
			lpszStringCopy = lpszString;
		}
		while ((*lpszString != _T('\0')) ? !MatchMask(lpszMask, lpszString) : (bMatch = false) != false);

		if (*lpszString == _T('\0') && *lpszMask == _T('\0')) return true;

		return bMatch;
	}
}

CFiltersArray& CFiltersArray::operator=(const CFiltersArray& rSrc)
{
	if(this != &rSrc)
	{
		m_vFilters = rSrc.m_vFilters;
	}

	return *this;
}

bool CFiltersArray::Match(const CFileInfo& rInfo) const
{
	if(m_vFilters.empty())
		return true;

	// if only one of the filters matches - return true
	for(std::vector<CFileFilter>::const_iterator iterFilter = m_vFilters.begin(); iterFilter != m_vFilters.end(); iterFilter++)
	{
		if((*iterFilter).Match(rInfo))
			return true;
	}

	return false;
}

void CFiltersArray::Serialize(icpf::archive& ar)
{
	if (ar.is_storing())
	{
		ar<< m_vFilters.size();
		for(std::vector<CFileFilter>::iterator iterFilter = m_vFilters.begin(); iterFilter != m_vFilters.end(); iterFilter++)
		{
			(*iterFilter).Serialize(ar);
		}
	}
	else
	{
		m_vFilters.clear();

		size_t stSize;
		CFileFilter ff;

		ar >> stSize;
		m_vFilters.reserve(stSize);
		while(stSize--)
		{
			ff.Serialize(ar);
			m_vFilters.push_back(ff);
		}
	}
}

bool CFiltersArray::IsEmpty() const
{
	return m_vFilters.empty();
}

void CFiltersArray::Add(const CFileFilter& rFilter)
{
	m_vFilters.push_back(rFilter);
}

bool CFiltersArray::SetAt(size_t stIndex, const CFileFilter& rNewFilter)
{
	BOOST_ASSERT(stIndex < m_vFilters.size());
	if(stIndex < m_vFilters.size())
	{
		CFileFilter& rFilter = m_vFilters.at(stIndex);
		rFilter = rNewFilter;
		return true;
	}
	else
		return false;
}

const CFileFilter* CFiltersArray::GetAt(size_t stIndex) const
{
	BOOST_ASSERT(stIndex < m_vFilters.size());
	if(stIndex < m_vFilters.size())
		return &m_vFilters.at(stIndex);
	else
		return NULL;
}

bool CFiltersArray::RemoveAt(size_t stIndex)
{
	BOOST_ASSERT(stIndex < m_vFilters.size());
	if(stIndex < m_vFilters.size())
	{
		m_vFilters.erase(m_vFilters.begin() + stIndex);
		return true;
	}
	else
		return false;
}

size_t CFiltersArray::GetSize() const
{
	return m_vFilters.size();
}
