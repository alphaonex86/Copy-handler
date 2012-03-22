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
#include "TPath.h"
#include <boost/algorithm/string.hpp>

BEGIN_CHCORE_NAMESPACE

// ============================================================================
/// TPath::TPath
/// @date 2009/11/29
///
/// @brief     Constructs the TPath object.
// ============================================================================
TPath::TPath() :
	m_strPath(),
	m_lRefCount(1)
{
}

// ============================================================================
/// TPath::~TPath
/// @date 2009/11/29
///
/// @brief     Destructs the TPath object.
// ============================================================================
TPath::~TPath()
{
}

// ============================================================================
/// TPath::Release
/// @date 2009/11/29
///
/// @brief     Releases a reference to this object. Deletes the object if no reference exists.
/// @return    Current reference count.
// ============================================================================
long TPath::Release()
{
	if(--m_lRefCount == 0)
	{
		delete this;
		return 0;
	}
	return m_lRefCount;
}

// ============================================================================
/// TPath::New
/// @date 2009/11/29
///
/// @brief     Allocates a new, empty TPath object.
/// @return    Pointer to the newly allocated object.
// ============================================================================
TPath* TPath::New()
{
	return new TPath();
}

// ============================================================================
/// TPath::Delete
/// @date 2009/11/29
///
/// @brief     Deletes the TPath object 
/// @param[in] pPath - pointer to the object to delete.
// ============================================================================
void TPath::Delete(TPath* pPath)
{
	delete pPath;
}

// ============================================================================
/// TSmartPath::TSmartPath
/// @date 2009/11/29
///
/// @brief     Constructs an empty path.
// ============================================================================
TSmartPath::TSmartPath() :
	m_pPath(NULL)
{
}

// ============================================================================
/// TSmartPath::TSmartPath
/// @date 2009/11/29
///
/// @brief     Constructs path from stl string object.
/// @param[in] strPath - string containing a path.
// ============================================================================
TSmartPath::TSmartPath(const tstring_t& strPath) :
	m_pPath(TPath::New())
{
	if(m_pPath)
		m_pPath->m_strPath = strPath;
}

// ============================================================================
/// TSmartPath::TSmartPath
/// @date 2009/11/29
///
/// @brief     Constructs a path object from string.
/// @param[in] pszPath - string with path.
// ============================================================================
TSmartPath::TSmartPath(const tchar_t* pszPath) :
	m_pPath(TPath::New())
{
	if(m_pPath)
		m_pPath->m_strPath = pszPath;
}

// ============================================================================
/// TSmartPath::TSmartPath
/// @date 2009/11/29
///
/// @brief     Constructs path object from another path object.
/// @param[in] spPath - reference to another path object.
// ============================================================================
TSmartPath::TSmartPath(const TSmartPath& spPath) :
	m_pPath(spPath.m_pPath)
{
}

// ============================================================================
/// TSmartPath::~TSmartPath
/// @date 2009/11/29
///
/// @brief     
/// @return    
// ============================================================================
TSmartPath::~TSmartPath()
{
	Clear();
}

// ============================================================================
/// TSmartPath::Clear
/// @date 2009/11/29
///
/// @brief     Clears this object.
// ============================================================================
void TSmartPath::Clear() throw()
{
	if(m_pPath)
	{
		m_pPath->Release();		// Release will delete object if unused anymore
		m_pPath = NULL;
	}
}

// ============================================================================
/// TSmartPath::operator=
/// @date 2009/11/29
///
/// @brief     Assigns a path from string.
/// @param[in] strPath - string containing a path.
/// @return    Reference to this object.
// ============================================================================
TSmartPath& TSmartPath::operator=(const tstring_t& strPath)
{
	// can we get exclusive access to the member?
	// if not, clear this object
	if(m_pPath && m_pPath->IsShared())
		Clear();

	// create new internal path if does not exist
	if(!m_pPath)
		m_pPath = TPath::New();

	m_pPath->m_strPath = strPath;

	return *this;
}

// ============================================================================
/// TSmartPath::operator=
/// @date 2009/11/29
///
/// @brief     Assigns a path from string.
/// @param[in] strPath - string containing a path.
/// @return    Reference to this object.
// ============================================================================
TSmartPath& TSmartPath::operator=(const tchar_t* pszPath)
{
	// can we get exclusive access to the member?
	// if not, clear this object
	if(m_pPath && m_pPath->IsShared())
		Clear();

	// create new internal path if does not exist
	if(!m_pPath)
		m_pPath = TPath::New();

	m_pPath->m_strPath = pszPath;

	return *this;
}

// ============================================================================
/// TSmartPath::operator=
/// @date 2009/11/29
///
/// @brief     Assigns a path from other path object.
/// @param[in] spPath - path object from which we want to get path.
/// @return    Reference to this object.
// ============================================================================
TSmartPath& TSmartPath::operator=(const TSmartPath& spPath)
{
	if(this != &spPath && m_pPath != spPath.m_pPath)
	{
		Clear();
		m_pPath = spPath.m_pPath;
		m_pPath->AddRef();
	}

	return *this;
}

// ============================================================================
/// TSmartPath::operator==
/// @date 2009/11/29
///
/// @brief     Compares paths (case sensitive).
/// @param[in] rPath - path to compare this object to.
/// @return    True if equal, false otherwise.
// ============================================================================
bool TSmartPath::operator==(const TSmartPath& rPath) const
{
	if(m_pPath == rPath.m_pPath)
		return true;
	else if(m_pPath == NULL || rPath.m_pPath == NULL)
		return false;
	else
		return m_pPath->m_strPath == rPath.m_pPath->m_strPath;
}

// ============================================================================
/// TSmartPath::operator<
/// @date 2009/11/29
///
/// @brief     Compares paths (case sensitive).
/// @param[in] rPath - input path to compare.
/// @return    True if this object is less than rPath, false otherwise.
// ============================================================================
bool TSmartPath::operator<(const TSmartPath& rPath) const
{
	if(m_pPath == rPath.m_pPath)
		return false;
	else if(m_pPath == NULL || rPath.m_pPath == NULL)
		return m_pPath < rPath.m_pPath;
	else
		return m_pPath->m_strPath < rPath.m_pPath->m_strPath;
}

// ============================================================================
/// TSmartPath::operator>
/// @date 2009/11/29
///
/// @brief     Compares paths (case sensitive).
/// @param[in] rPath - input path to compare.
/// @return    True if this object is less than rPath, false otherwise.
// ============================================================================
bool TSmartPath::operator>(const TSmartPath& rPath) const
{
	if(m_pPath == rPath.m_pPath)
		return false;
	else if(m_pPath == NULL || rPath.m_pPath == NULL)
		return m_pPath > rPath.m_pPath;
	else
		return m_pPath->m_strPath > rPath.m_pPath->m_strPath;
}

// ============================================================================
/// TSmartPath::operator+
/// @date 2009/11/29
///
/// @brief     Concatenates two paths, returns the result.
/// @param[in] rPath - path to concatenate.
/// @return    New path object with the results of concatenation.
// ============================================================================
TSmartPath TSmartPath::operator+(const TSmartPath& rPath) const
{
	TSmartPath spNewPath(*this);
	if(rPath.m_pPath)
		spNewPath += rPath.m_pPath->m_strPath;

	return spNewPath;
}

// ============================================================================
/// TSmartPath::operator+=
/// @date 2009/11/29
///
/// @brief     Concatenates provided path to our own.
/// @param[in] rPath - path to concatenate.
/// @return    Reference to this object.
// ============================================================================
TSmartPath& TSmartPath::operator+=(const TSmartPath& rPath)
{
	// if there is no path inside rPath, then there is no point in doing anything
	if(rPath.m_pPath)
	{
		// can we use this object exclusively?
		if(m_pPath && m_pPath->IsShared())
			Clear();

		if(!m_pPath)
			m_pPath = TPath::New();

		m_pPath->m_strPath += rPath.m_pPath->m_strPath;
	}

	return *this;
}

// ============================================================================
/// TSmartPath::operator tstring_t
/// @date 2009/11/29
///
/// @brief     
/// @return    
// ============================================================================
TSmartPath::operator tstring_t() const
{
	tstring_t strPath;
	if(m_pPath)
		strPath = m_pPath->m_strPath;

	return strPath;
}

// ============================================================================
/// TSmartPath::Compare
/// @date 2009/11/29
///
/// @brief     Compares paths.
/// @param[in] rPath - path to compare to.
/// @return    Result of the comparison.
// ============================================================================
bool TSmartPath::Compare(const TSmartPath& rPath, bool bCaseSensitive) const
{
	if(m_pPath == rPath.m_pPath)
		return true;
	else if(m_pPath == NULL || rPath.m_pPath == NULL)
		return m_pPath == rPath.m_pPath;
	else
	{
		if(bCaseSensitive)
			return boost::equals(m_pPath->m_strPath, rPath.m_pPath->m_strPath);
		else
			return boost::iequals(m_pPath->m_strPath, rPath.m_pPath->m_strPath);
	}
}

// ============================================================================
/// TSmartPath::IsChildOf
/// @date 2009/11/29
///
/// @brief     Checks if this path starts with the path specified as parameter.
/// @param[in] rPath - path to check this one against.
/// @return    True if this path starts with the provided one, false otherwise.
// ============================================================================
bool TSmartPath::IsChildOf(const TSmartPath& rPath, bool bCaseSensitive) const
{
	if(!m_pPath || !rPath.m_pPath)
		return false;

	if(bCaseSensitive)
		return boost::starts_with(m_pPath->m_strPath, rPath.m_pPath->m_strPath);
	else
		return boost::istarts_with(m_pPath->m_strPath, rPath.m_pPath->m_strPath);
}

// ============================================================================
/// TPathContainer::TPathContainer
/// @date 2009/11/30
///
/// @brief     Constructs an empty path container object.
// ============================================================================
TPathContainer::TPathContainer() :
	m_vPaths()
{
}

// ============================================================================
/// TPathContainer::TPathContainer
/// @date 2009/11/30
///
/// @brief     Constructs the path container object from another path container.
/// @param[in] rSrcContainer - path container to copy paths from.
// ============================================================================
TPathContainer::TPathContainer(const TPathContainer& rSrcContainer) :
	m_vPaths(rSrcContainer.m_vPaths)
{
}

// ============================================================================
/// TPathContainer::~TPathContainer
/// @date 2009/11/30
///
/// @brief     Destructs this path container object.
// ============================================================================
TPathContainer::~TPathContainer()
{

}

// ============================================================================
/// TPathContainer::operator=
/// @date 2009/11/30
///
/// @brief     Assigns another path container object to this one.
/// @param[in] rSrcContainer - container with paths to copy from.
/// @return    Reference to this object.
// ============================================================================
TPathContainer& TPathContainer::operator=(const TPathContainer& rSrcContainer)
{
	if(this != &rSrcContainer)
		m_vPaths = rSrcContainer.m_vPaths;

	return *this;
}

// ============================================================================
/// TPathContainer::Add
/// @date 2009/11/30
///
/// @brief     Adds a path to the end of list.
/// @param[in] spPath - path to be added.
// ============================================================================
void TPathContainer::Add(const TSmartPath& spPath)
{
	m_vPaths.push_back(spPath);
}

// ============================================================================
/// TPathContainer::GetAt
/// @date 2009/11/30
///
/// @brief     Retrieves path at specified index.
/// @param[in] stIndex - index at which to retrieve item.
/// @return    Reference to the path object.
// ============================================================================
const TSmartPath& TPathContainer::GetAt(size_t stIndex) const
{
	if(stIndex > m_vPaths.size())
		THROW_CORE_EXCEPTION(eBoundsExceeded);

	return m_vPaths.at(stIndex);
}

// ============================================================================
/// TPathContainer::GetAt
/// @date 2009/11/30
///
/// @brief     Retrieves path at specified index.
/// @param[in] stIndex - index at which to retrieve item.
/// @return    Reference to the path object.
// ============================================================================
TSmartPath& TPathContainer::GetAt(size_t stIndex)
{
	if(stIndex > m_vPaths.size())
		THROW_CORE_EXCEPTION(eBoundsExceeded);

	return m_vPaths.at(stIndex);
}

// ============================================================================
/// chcore::TPathContainer::SetAt
/// @date 2009/11/30
///
/// @brief     Sets a path at a specified index.
/// @param[in] stIndex - index at which to set the path.
/// @param[in] spPath -  path to be set.
// ============================================================================
void TPathContainer::SetAt(size_t stIndex, const TSmartPath& spPath)
{
	if(stIndex > m_vPaths.size())
		THROW_CORE_EXCEPTION(eBoundsExceeded);
	
	m_vPaths[stIndex] = spPath;
}

// ============================================================================
/// chcore::TPathContainer::DeleteAt
/// @date 2009/11/30
///
/// @brief     Removes a path from container at specified index.
/// @param[in] stIndex - index at which to delete.
// ============================================================================
void TPathContainer::DeleteAt(size_t stIndex)
{
	if(stIndex > m_vPaths.size())
		THROW_CORE_EXCEPTION(eBoundsExceeded);

	m_vPaths.erase(m_vPaths.begin() + stIndex);
}

// ============================================================================
/// chcore::TPathContainer::Clear
/// @date 2009/11/30
///
/// @brief     Removes all paths from this container.
// ============================================================================
void TPathContainer::Clear()
{
	m_vPaths.clear();
}

// ============================================================================
/// chcore::TPathContainer::GetCount
/// @date 2009/11/30
///
/// @brief     Retrieves count of elements in the container.
/// @return    Count of elements.
// ============================================================================
size_t TPathContainer::GetCount() const
{
	return m_vPaths.size();
}

END_CHCORE_NAMESPACE
