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
#ifndef __TPATH_H__
#define __TPATH_H__

#include "libchcore.h"

BEGIN_CHCORE_NAMESPACE

class LIBCHCORE_API TPath
{
public:
	TPath();
	~TPath();

	long AddRef() { return ++m_lRefCount; }
	long Release();
	bool IsShared() const { return m_lRefCount > 1; }

protected:
	static TPath* New();
	static void Delete(TPath* pPath);

protected:
	tstring_t m_strPath;
	long m_lRefCount;

	friend class TSmartPath;
};

class LIBCHCORE_API TSmartPath
{
public:
	TSmartPath();
	TSmartPath(const tstring_t& strPath);
	TSmartPath(const tchar_t* pszPath);
	TSmartPath(const TSmartPath& spPath);

	~TSmartPath();

	void Clear() throw();

	TSmartPath& operator=(const tstring_t& strPath);
	TSmartPath& operator=(const TSmartPath& spPath);
	TSmartPath& operator=(const tchar_t* pszPath);

	bool operator==(const TSmartPath& rPath) const;
	bool operator<(const TSmartPath& rPath) const;
	bool operator>(const TSmartPath& rPath) const;

	TSmartPath operator+(const TSmartPath& rPath) const;
	TSmartPath& operator+=(const TSmartPath& rPath);

	operator tstring_t() const;

	bool Compare(const TSmartPath& rPath, bool bCaseSensitive) const;
	bool IsChildOf(const TSmartPath& rPath, bool bCaseSensitive) const;

protected:
	TPath* m_pPath;
};

class LIBCHCORE_API TPathContainer
{
public:
	TPathContainer();
	TPathContainer(const TPathContainer& rSrcContainer);
	~TPathContainer();

	TPathContainer& operator=(const TPathContainer& rSrcContainer);

	void Add(const TSmartPath& spPath);
	
	const TSmartPath& GetAt(size_t stIndex) const;
	TSmartPath& GetAt(size_t stIndex);

	void SetAt(size_t stIndex, const TSmartPath& spPath);

	void DeleteAt(size_t stIndex);
	void Clear();

	size_t GetCount() const;

private:
#pragma warning(push)
#pragma warning(disable: 4251)
	std::vector<TSmartPath> m_vPaths;
#pragma warning(pop)
};

END_CHCORE_NAMESPACE

#endif
