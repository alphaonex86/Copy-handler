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
// File was originally based on FileInfo.h by Antonio Tejada Lacaci.
// Almost everything has changed since then.

#ifndef __FILEINFO_H__
#define __FILEINFO_H__

#include "DestPath.h"

void FindFreeSubstituteName(CString strSrcPath, CString strDstPath, CString* pstrResult);
extern void GetDriveData(LPCTSTR lpszPath, int *piDrvNum, UINT *puiDrvType);

// CFileInfo flags
// flag stating that file has been processed (used to determine if file can be deleted at the end of copying)     
#define FIF_PROCESSED		0x00000001

class CFiltersArray;
/////////////////////////////////////////////////////////////////////////////
// CClipboardEntry
class CClipboardEntry
{
public:
	CClipboardEntry();
	CClipboardEntry(const CClipboardEntry& rEntry);

	void SetPath(const CString& strPath);
	void CalcBufferIndex(const CDestPath& dpDestPath);
	const CString& GetPath() const { return m_strPath; }

	void SetMove(bool bValue) { m_bMove=bValue; }
	bool GetMove() { return m_bMove; }

	int GetDriveNumber() const { return m_iDriveNumber; }
	UINT GetDriveType() const { return m_uiDriveType; }

	int GetBufferIndex() const { return m_iBufferIndex; }

	void Serialize(icpf::archive& ar, bool bData);

	void AddDestinationPath(const CString& strPath);
	size_t GetDestinationPathsCount() const;
	CString GetDestinationPath(size_t stIndex);

private:
	CString m_strPath;				// path (ie. c:\\windows\\) - always with ending '\\'
	bool m_bMove;					// specifies if we can use MoveFile (if will be moved)

	int m_iDriveNumber;		// disk number (-1 - none)
	UINT m_uiDriveType;		// path type

	int m_iBufferIndex;		// buffer number, with which we'll copy this data

	std::vector<CString> m_vDstPaths;	// dest paths table for this group of paths
};

//////////////////////////////////////////////////////////////////////////
// CClipboardArray

class CClipboardArray
{
public:
	~CClipboardArray();
	
	void Serialize(icpf::archive& ar, bool bData);

	CClipboardEntry* GetAt(int iPos);

	int GetSize() const;
	void Add(CClipboardEntry* pEntry);
	void SetAt(int nIndex, CClipboardEntry* pEntry);
	void RemoveAt(int nIndex, int nCount = 1);
	void RemoveAll();

protected:
	std::vector<CClipboardEntry*> m_vEntries;
};

class CFileInfo
{  
public:
	CFileInfo();
	CFileInfo(const CFileInfo& finf);
	~CFileInfo();

	// static member
	static bool Exist(CString strPath);	// check for file or folder existence
	
	void Create(const WIN32_FIND_DATA* pwfd, LPCTSTR pszFilePath, int iSrcIndex);
	bool Create(CString strFilePath, int iSrcIndex);
	
	ULONGLONG GetLength64() const { return m_uhFileSize; }
	void SetLength64(ULONGLONG uhSize) { m_uhFileSize=uhSize; }

	// disk - path and disk number (-1 if none - ie. net disk)
	CString GetFileDrive(void) const;		// returns string with src disk
	int GetDriveNumber() const;				// disk number A - 0, b-1, c-2, ...
	UINT GetDriveType() const;				// drive type
	
	CString GetFileDir() const;	// @rdesc Returns \WINDOWS\ for C:\WINDOWS\WIN.INI 
	CString GetFileTitle() const;	// @cmember returns WIN for C:\WINDOWS\WIN.INI
	CString GetFileExt() const;		/** @cmember returns INI for C:\WINDOWS\WIN.INI */
	CString GetFileRoot() const;	/** @cmember returns C:\WINDOWS\ for C:\WINDOWS\WIN.INI */
	CString GetFileName() const;	/** @cmember returns WIN.INI for C:\WINDOWS\WIN.INI */
	
	const CString& GetFilePath(void) const { return m_strFilePath; }	// returns path with m_strFilePath (probably not full)
	CString GetFullFilePath() const;		/** @cmember returns C:\WINDOWS\WIN.INI for C:\WINDOWS\WIN.INI */
	void SetFilePath(LPCTSTR lpszPath) { m_strFilePath=lpszPath; };
	
	/* Get File times info (equivalent to CFindFile members) */
	const COleDateTime& GetCreationTime() const { return m_timCreation; };
	const COleDateTime& GetLastAccessTime() const { return m_timLastAccess; };
	const COleDateTime& GetLastWriteTime() const { return m_timLastWrite; };
	
	/* Get File attributes info (equivalent to CFindFile members) */
	DWORD GetAttributes(void) const { return m_dwAttributes; }
	bool IsDirectory(void) const { return (m_dwAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0; }
	bool IsArchived(void) const { return (m_dwAttributes & FILE_ATTRIBUTE_ARCHIVE) != 0; }
	bool IsReadOnly(void) const { return (m_dwAttributes & FILE_ATTRIBUTE_READONLY) != 0; }
	bool IsCompressed(void) const { return (m_dwAttributes & FILE_ATTRIBUTE_COMPRESSED) != 0; }
	bool IsSystem(void) const { return (m_dwAttributes & FILE_ATTRIBUTE_SYSTEM) != 0; }
	bool IsHidden(void) const { return (m_dwAttributes & FILE_ATTRIBUTE_HIDDEN) != 0; }
	bool IsTemporary(void) const { return (m_dwAttributes & FILE_ATTRIBUTE_TEMPORARY) != 0; }
	bool IsNormal(void) const { return m_dwAttributes == 0; }

	uint_t GetFlags() const { return m_uiFlags; }
	void SetFlags(uint_t uiFlags, uint_t uiMask = 0xffffffff) { m_uiFlags = (m_uiFlags & ~(uiFlags & uiMask)) | (uiFlags & uiMask); }

	// operations
	void SetClipboard(CClipboardArray *pClipboard) { m_pClipboard=pClipboard; };
	CString GetDestinationPath(CString strPath, unsigned char ucCopyNumber, int iFlags);

	void SetSrcIndex(int iIndex) { m_iSrcIndex=iIndex; };
	int GetSrcIndex() const { return m_iSrcIndex; };

	bool GetMove() { if (m_iSrcIndex != -1) return m_pClipboard->GetAt(m_iSrcIndex)->GetMove(); else return true; };

	int GetBufferIndex() const;

	// operators
	bool operator==(const CFileInfo& rInfo);
	
	// (re)/store data
	void Store(icpf::archive& ar);
	void Load(icpf::archive& ar);

private:
	CString m_strFilePath;	// contains relative path (first path is in CClipboardArray)
	int m_iSrcIndex;		// index in CClipboardArray table (which contains the first part of the path)
	
	DWORD m_dwAttributes;	// attributes
	ULONGLONG m_uhFileSize;
	COleDateTime m_timCreation;
	COleDateTime m_timLastAccess;
	COleDateTime m_timLastWrite;

	uint_t m_uiFlags;
	// ptrs to elements providing data
	CClipboardArray* m_pClipboard;
}; 

/**
* @class Allows to retrieve <c CFileInfo>s from files/directories in a directory
*/
class CFileInfoArray
{
public:
	CFileInfoArray(CClipboardArray& A_rClipboardArray) :
		m_rClipboard(A_rClipboardArray)
	{
	}

	void AddDir(CString strDirName, const CFiltersArray* pFilters, int iSrcIndex,
		const bool bRecurse, const bool bIncludeDirs, const volatile bool* pbAbort=NULL);
	
	void AddFile(CString strFilePath, int iSrcIndex);
	
	void AddFileInfo(const CFileInfo& rFileInfo);

	void AppendArray(const CFileInfoArray& arrFiles);

	size_t GetSize() const;
	CFileInfo& GetAt(size_t stIndex);

	void Clear();

	// store/restore
	void Store(icpf::archive& ar, bool bOnlyFlags);

	void Load(icpf::archive& ar, bool bOnlyFlags);

protected:
	CClipboardArray& m_rClipboard;
	std::vector<CFileInfo> m_vFiles;
};
						  
#endif
