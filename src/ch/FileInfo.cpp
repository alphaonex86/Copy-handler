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
/*************************************************************************
	FileInfo.cpp: implementation of the CFileInfo class.
	(c) Codeguru & friends
	Coded by Antonio Tejada Lacaci. 1999, modified by Ixen Gerthannes
	atejada@espanet.com
*************************************************************************/

#include "stdafx.h"
#include "FileInfo.h"
#include "FileFilter.h"
#include "resource.h"
#include "DataBuffer.h"
#include "Device IO.h"
#include "imagehlp.h"
#include "ch.h"
#include "../libicpf/exception.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////////////
// CClipboardArray

CClipboardArray::~CClipboardArray()
{
	RemoveAll();
}

void CClipboardArray::Serialize(icpf::archive& ar, bool bData)
{
	if (ar.is_storing())
	{
		// write data
		int iSize = m_vEntries.size();
		ar<<iSize;
		for (int i=0;i<iSize;i++)
			m_vEntries.at(i)->Serialize(ar, bData);
	}
	else
	{
		int iSize;
		ar>>iSize;
		
		m_vEntries.reserve(iSize);
		CClipboardEntry* pEntry;
		for (int i=0;i<iSize;i++)
		{
			if (i < m_vEntries.size())
				pEntry = m_vEntries.at(i);
			else
			{
				pEntry = new CClipboardEntry;
				m_vEntries.push_back(pEntry);
			}

			pEntry->Serialize(ar, bData);
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
// CClipboardEntry

CClipboardEntry::CClipboardEntry() :
	m_bMove(true),
	m_iDriveNumber(-1),
	m_uiDriveType(static_cast<UINT>(-1)),
	m_iBufferIndex(0)
{
}

CClipboardEntry::CClipboardEntry(const CClipboardEntry& rEntry) :
	m_strPath(rEntry.m_strPath),
	m_bMove(rEntry.m_bMove),
	m_iDriveNumber(rEntry.m_iDriveNumber),
	m_uiDriveType(rEntry.m_uiDriveType),
	m_vDstPaths(rEntry.m_vDstPaths)
{
}

CClipboardEntry* CClipboardArray::GetAt(int iPos)
{
	return m_vEntries.at(iPos);
}

void CClipboardArray::SetAt(int nIndex, CClipboardEntry* pEntry)
{
	delete [] m_vEntries.at(nIndex);
	m_vEntries[nIndex] = pEntry;
}

void CClipboardArray::Add(CClipboardEntry* pEntry)
{
	m_vEntries.push_back(pEntry);
}

void CClipboardArray::RemoveAt(int nIndex, int nCount)
{
	while (nCount--)
	{
		delete m_vEntries.at(nIndex);
	}
	m_vEntries.erase(m_vEntries.begin() + nIndex, m_vEntries.begin() + nIndex + nCount);
}

void CClipboardArray::RemoveAll()
{
	for(std::vector<CClipboardEntry*>::iterator iterEntry = m_vEntries.begin(); iterEntry != m_vEntries.end(); ++iterEntry)
	{
		delete *iterEntry;
	}
	m_vEntries.clear();
}

int CClipboardArray::GetSize() const
{
	return m_vEntries.size();
}

/////////////////////////////////////////////////////////////////////////////
// CClipboardEntry

void CClipboardEntry::SetPath(const CString& strPath)
{
	m_strPath=strPath;			// guaranteed without ending '\\' 
	if (m_strPath.Right(1) == _T('\\'))
		m_strPath=m_strPath.Left(m_strPath.GetLength()-1);

	GetDriveData(m_strPath, &m_iDriveNumber, &m_uiDriveType);
}

void CClipboardEntry::CalcBufferIndex(const CDestPath& dpDestPath)
{
	// what kind of buffer
	if (m_uiDriveType == DRIVE_REMOTE || dpDestPath.GetDriveType() == DRIVE_REMOTE)
		m_iBufferIndex=BI_LAN;
	else if (m_uiDriveType == DRIVE_CDROM || dpDestPath.GetDriveType() == DRIVE_CDROM)
		m_iBufferIndex=BI_CD;
	else if (m_uiDriveType == DRIVE_FIXED && dpDestPath.GetDriveType() == DRIVE_FIXED)
	{
		// two hdd's - is this the same physical disk ?
		if (m_iDriveNumber == dpDestPath.GetDriveNumber() || IsSamePhysicalDisk(m_iDriveNumber, dpDestPath.GetDriveNumber()))
			m_iBufferIndex=BI_ONEDISK;
		else
			m_iBufferIndex=BI_TWODISKS;
	}
	else
		m_iBufferIndex=BI_DEFAULT;
}

void CClipboardEntry::Serialize(icpf::archive& ar, bool bData)
{
	if (bData)
	{
		if(ar.is_storing())
		{
			ar<<m_strPath;
			ar<<static_cast<unsigned char>(m_bMove);
			ar<<m_iDriveNumber;
			ar<<m_uiDriveType;
			ar<<m_iBufferIndex;
		}
		else
		{
			ar>>m_strPath;
			unsigned char ucData;
			ar>>ucData;
			m_bMove=ucData != 0;
			ar>>m_iDriveNumber;
			ar>>m_uiDriveType;
			ar>>m_iBufferIndex;
		}
	}
	else
	{
		if(ar.is_storing())
			ar << m_vDstPaths;
		else
			ar >> m_vDstPaths;
	}
}

void CClipboardEntry::AddDestinationPath(const CString& strPath)
{
	m_vDstPaths.push_back(strPath);
}

size_t CClipboardEntry::GetDestinationPathsCount() const
{
	return m_vDstPaths.size();
}

CString CClipboardEntry::GetDestinationPath(size_t stIndex)
{
	return m_vDstPaths.at(stIndex);
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
// finds another name for a copy of src file(folder) in dest location
void FindFreeSubstituteName(CString strSrcPath, CString strDstPath, CString* pstrResult)
{
	// get the name from srcpath
	if (strSrcPath.Right(1) == _T("\\"))
		strSrcPath=strSrcPath.Left(strSrcPath.GetLength()-1);

	int iBarPos=strSrcPath.ReverseFind(_T('\\'));
	CString strFolderName;
	if (iBarPos != -1)
		strFolderName=strSrcPath.Mid(iBarPos+1);
	else
		strFolderName=strSrcPath;	// it shouldn't happen at all

	if (strDstPath.Right(1) != _T("\\"))
		strDstPath+=_T("\\");

	// set the dest path
	CString strCheckPath;
	ictranslate::CFormat fmt(GetResManager().LoadString(IDS_FIRSTCOPY_STRING));
	fmt.SetParam(_t("%name"), strFolderName);
	strCheckPath = fmt;
	if (strCheckPath.GetLength() > _MAX_PATH)
		strCheckPath=strCheckPath.Left(_MAX_PATH);	// max - 260 chars

	// when adding to strDstPath check if the path already exists - if so - try again
	int iCounter=1;
	CString strFmt = GetResManager().LoadString(IDS_NEXTCOPY_STRING);
	while (CFileInfo::Exist(strDstPath+strCheckPath))
	{
		fmt.SetFormat(strFmt);
		fmt.SetParam(_t("%name"), strFolderName);
		fmt.SetParam(_t("%count"), ++iCounter);
		strCheckPath = fmt;
	}

	*pstrResult=strCheckPath;
}

////////////////////////////////////////////////////////////////////////////
CFileInfo::CFileInfo() :
	m_pClipboard(NULL)
{
}

CFileInfo::CFileInfo(const CFileInfo& finf) :
	m_strFilePath(finf.m_strFilePath),
	m_iSrcIndex(finf.m_iSrcIndex),
	m_dwAttributes(finf.m_dwAttributes),
	m_uhFileSize(finf.m_uhFileSize),
	m_timCreation(finf.m_timCreation),
	m_timLastAccess(finf.m_timLastAccess),
	m_timLastWrite(finf.m_timLastWrite),
	m_uiFlags(finf.m_uiFlags),
	m_pClipboard(finf.m_pClipboard)
{
}

CFileInfo::~CFileInfo()
{
}

bool CFileInfo::Exist(CString strPath)
{
	WIN32_FIND_DATA fd;
	
	// search by exact name
	HANDLE hFind = FindFirstFile(strPath, &fd);
	if (hFind != INVALID_HANDLE_VALUE)
		return true;

	// another try (add '\\' if needed and '*' for marking that we look for ie. c:\*
	// instead of c:\, which would never be found prev. way)
	if (strPath.Right(1) != _T("\\"))
		strPath+=_T("\\*");
	else
		strPath+=_T("*");

	hFind = FindFirstFile(strPath, &fd);
	if (hFind != INVALID_HANDLE_VALUE)
		return true;
	else
		return false;
}

void CFileInfo::Create(const WIN32_FIND_DATA* pwfd, const LPCTSTR &pszFilePath,const int &iSrcIndex)
{
	BOOST_ASSERT(iSrcIndex == -1 || m_pClipboard);
	if(iSrcIndex != -1 && !m_pClipboard)
		THROW(_t("Internal error: pointer not initialized."), 0, 0, 0);

	// copy data from W32_F_D
	m_strFilePath = CString(pszFilePath) + pwfd->cFileName;
	
	// if proper index has been passed - reduce the path
	if(m_pClipboard && iSrcIndex >= 0)
		m_strFilePath=m_strFilePath.Mid(m_pClipboard->GetAt(iSrcIndex)->GetPath().GetLength());	// wytnij œcie¿kê z clipboarda

	m_iSrcIndex=iSrcIndex;
	m_dwAttributes = pwfd->dwFileAttributes;
	m_uhFileSize = (((ULONGLONG) pwfd->nFileSizeHigh) << 32) + pwfd->nFileSizeLow;
	m_timCreation = pwfd->ftCreationTime;
	m_timLastAccess = pwfd->ftLastAccessTime;
	m_timLastWrite = pwfd->ftLastWriteTime;
	m_uiFlags = 0;
}

bool CFileInfo::Create(const CString &strFilePath,const int &iSrcIndex)
{
	WIN32_FIND_DATA wfd;
	HANDLE hFind;
	int nBarPos;
	
	hFind = FindFirstFile(strFilePath, &wfd);
	if (hFind != INVALID_HANDLE_VALUE)
	{
		FindClose(hFind);

		// add data to members
		nBarPos = strFilePath.ReverseFind(TCHAR('\\'));
		Create(&wfd, strFilePath.Left(nBarPos+1), iSrcIndex);
		
		return true;
	}
	else
	{
		m_strFilePath=GetResManager().LoadString(IDS_NOTFOUND_STRING);
		m_iSrcIndex=-1;
		m_dwAttributes = (DWORD)-1;
		m_uhFileSize = (unsigned __int64)-1;
		m_timCreation.SetDateTime(1900, 1, 1, 0, 0, 0);
		m_timLastAccess.SetDateTime(1900, 1, 1, 0, 0, 0);
		m_timLastWrite.SetDateTime(1900, 1, 1, 0, 0, 0);
		m_uiFlags = 0;
		return false;
	}
}

CString CFileInfo::GetFileDrive(void) const
{
	ASSERT(m_pClipboard);

	CString strPath=(m_iSrcIndex != -1) ? m_pClipboard->GetAt(m_iSrcIndex)->GetPath()+m_strFilePath : m_strFilePath;
	TCHAR szDrive[_MAX_DRIVE];
	_tsplitpath(strPath, szDrive, NULL, NULL, NULL);
	return CString(szDrive);
}

int CFileInfo::GetDriveNumber() const
{
	ASSERT(m_pClipboard);

	if (m_iSrcIndex != -1)
	{
		// read data stored in CClipboardEntry
		return m_pClipboard->GetAt(m_iSrcIndex)->GetDriveNumber();
	}
	else
	{
		// manually
		int iNum;
		GetDriveData(m_strFilePath, &iNum, NULL);
		return iNum;
	}
}

UINT CFileInfo::GetDriveType() const
{
	ASSERT(m_pClipboard);

	if (m_iSrcIndex != -1)
	{
		// read data contained in CClipboardEntry
		return m_pClipboard->GetAt(m_iSrcIndex)->GetDriveType();
	}
	else
	{
		// manually
		UINT uiType;
		GetDriveData(m_strFilePath, NULL, &uiType);
		return uiType;
	}
}

CString CFileInfo::GetFileDir(void) const
{ 
	ASSERT(m_pClipboard);

	CString strPath=(m_iSrcIndex != -1) ? m_pClipboard->GetAt(m_iSrcIndex)->GetPath()+m_strFilePath : m_strFilePath;
	TCHAR szDir[_MAX_DIR];
	_tsplitpath(strPath, NULL, szDir,NULL, NULL);
	return CString(szDir);
}

CString CFileInfo::GetFileTitle(void) const
{
	ASSERT(m_pClipboard);

	CString strPath=(m_iSrcIndex != -1) ? m_pClipboard->GetAt(m_iSrcIndex)->GetPath()+m_strFilePath : m_strFilePath;
	TCHAR szName[_MAX_FNAME];
	_tsplitpath(strPath, NULL, NULL, szName, NULL);
	return CString(szName);
}

CString CFileInfo::GetFileExt(void) const
{
	ASSERT(m_pClipboard);

	CString strPath=(m_iSrcIndex != -1) ? m_pClipboard->GetAt(m_iSrcIndex)->GetPath()+m_strFilePath : m_strFilePath;
	TCHAR szExt[_MAX_EXT];
	_tsplitpath(strPath, NULL, NULL, NULL, szExt);
	return CString(szExt);
}

CString CFileInfo::GetFileRoot(void) const
{
	ASSERT(m_pClipboard);

	CString strPath=(m_iSrcIndex != -1) ? m_pClipboard->GetAt(m_iSrcIndex)->GetPath()+m_strFilePath : m_strFilePath;

	TCHAR szDrive[_MAX_DRIVE];
	TCHAR szDir[_MAX_DIR];
	_tsplitpath(strPath, szDrive, szDir, NULL, NULL);
	return CString(szDrive)+szDir;
}

CString CFileInfo::GetFileName(void) const
{
	ASSERT(m_pClipboard || m_iSrcIndex == -1);

	CString strPath;
	if(m_pClipboard && m_iSrcIndex != -1)
		strPath = m_pClipboard->GetAt(m_iSrcIndex)->GetPath() + m_strFilePath;
	else
		strPath = m_strFilePath;

	TCHAR szName[_MAX_FNAME];
	TCHAR szExt[_MAX_EXT];
	_tsplitpath(strPath, NULL, NULL, szName, szExt);
	return CString(szName)+szExt;
}

void CFileInfo::Store(icpf::archive& ar)
{
	ar<<m_strFilePath;
	ar<<m_iSrcIndex;
	ar<<m_dwAttributes;
	ar<<static_cast<unsigned long>((m_uhFileSize & 0xFFFFFFFF00000000) >> 32);
	ar<<static_cast<unsigned long>(m_uhFileSize & 0x00000000FFFFFFFF);
	ar<<m_timCreation;
	ar<<m_timLastAccess;
	ar<<m_timLastWrite;
}

void CFileInfo::Load(icpf::archive& ar)
{
	ar>>m_strFilePath;
	ar>>m_iSrcIndex;
	ar>>m_dwAttributes;
	unsigned long part;
	ar>>part;
	m_uhFileSize=(static_cast<unsigned __int64>(part) << 32);
	ar>>part;
	m_uhFileSize+=part;
	ar>>m_timCreation;
	ar>>m_timLastAccess;
	ar>>m_timLastWrite;
	m_uiFlags = 0;
}

bool CFileInfo::operator==(const CFileInfo& rInfo)
{
	return (rInfo.m_dwAttributes == m_dwAttributes && rInfo.m_timCreation == m_timCreation
		&& rInfo.m_timLastWrite == m_timLastWrite && rInfo.m_uhFileSize == m_uhFileSize);
}

CString CFileInfo::GetDestinationPath(CString strPath,const unsigned char &ucCopyNumber,const int &iFlags)
{
	// add '\\'
	if (strPath.Right(1) != _T("\\"))
		strPath+=_T("\\");

	// iFlags: bit 0-ignore folders; bit 1-force creating directories
	if (iFlags & 0x02)
	{
		// force create directories
		TCHAR dir[_MAX_DIR], fname[_MAX_FNAME], ext[_MAX_EXT];
		_tsplitpath(GetFullFilePath(), NULL, dir, fname, ext);
		
		CString str=dir;
		str.TrimLeft(_T("\\"));

		// force create directory
//		AfxMessageBox("Created multiple level of paths for %s"+strPath+str);
		SHCreateDirectoryEx(NULL, strPath+str, NULL);
//		MakeSureDirectoryPathExists(strPath+str);

//		AfxMessageBox(strPath+str+fname+CString(ext));
		return strPath+str+fname+CString(ext);
	}
	else
	{
		if (!(iFlags & 0x01) && m_iSrcIndex != -1)
		{
			// generate new dest name
			while (ucCopyNumber >= m_pClipboard->GetAt(m_iSrcIndex)->GetDestinationPathsCount())
			{
				CString strNewPath;
				FindFreeSubstituteName(GetFullFilePath(), strPath, &strNewPath);
				m_pClipboard->GetAt(m_iSrcIndex)->AddDestinationPath(strNewPath);
			}
			
			return strPath+m_pClipboard->GetAt(m_iSrcIndex)->GetDestinationPath(ucCopyNumber)+m_strFilePath;
		}
		else
			return strPath+GetFileName();
	}
}

CString CFileInfo::GetFullFilePath() const
{
	CString strPath;
	if (m_iSrcIndex >= 0)
	{
		ASSERT(m_pClipboard);
		strPath+=m_pClipboard->GetAt(m_iSrcIndex)->GetPath();
	}
	strPath+=m_strFilePath;

	return strPath;
}

int CFileInfo::GetBufferIndex() const
{
	if (m_iSrcIndex != -1)
		return m_pClipboard->GetAt(m_iSrcIndex)->GetBufferIndex();
	else
		return BI_DEFAULT;
}

///////////////////////////////////////////////////////////////////////
// Array

void CFileInfoArray::AddDir(CString strDirName, const CFiltersArray* pFilters, int iSrcIndex,
							const bool bRecurse, const bool bIncludeDirs,
							const volatile bool* pbAbort)
{ 
	WIN32_FIND_DATA wfd;
	CString strText;
	
	// init CFileInfo
	CFileInfo finf;
	finf.SetClipboard(&m_rClipboard);	// this is the link table (CClipboardArray)
	
	// append '\\' at the end of path if needed
	if (strDirName.Right(1) != _T("\\"))
		strDirName+=_T("\\");
	
	strText = strDirName + _T("*");
	// Iterate through dirs & files
	HANDLE hFind = FindFirstFile(strText, &wfd);
	if (hFind != INVALID_HANDLE_VALUE)
	{
		do
		{
			if ( !(wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) )
			{
				finf.Create(&wfd, strDirName, iSrcIndex);
				if (pFilters->Match(finf))
					m_vFiles.push_back(finf);
			}
			else if ( _tcscmp(wfd.cFileName, _T(".")) != 0 && _tcscmp(wfd.cFileName, _T("..")) != 0)
			{
				if (bIncludeDirs)
				{
					// Add directory itself
					finf.Create(&wfd, strDirName, iSrcIndex);
					m_vFiles.push_back(finf);
				}
				if (bRecurse)
				{
					strText = strDirName + wfd.cFileName+_T("\\");
					// Recurse Dirs
					AddDir(strText, pFilters, iSrcIndex, bRecurse, bIncludeDirs, pbAbort);
				}
			}
		}
		while (((pbAbort == NULL) || (!*pbAbort)) && (FindNextFile(hFind, &wfd)));
		
		FindClose(hFind);
	}
}

void CFileInfoArray::AddFile(CString strFilePath,const int &iSrcIndex)
{
   CFileInfo finf;

   // CUSTOMIZATION3 - cut '\\' at the end of strFilePath, set relative path
   if (strFilePath.Right(1) == _T("\\"))
		strFilePath=strFilePath.Left(strFilePath.GetLength()-1);

   finf.Create(strFilePath, iSrcIndex);
   return m_vFiles.push_back(finf);
}

void CFileInfoArray::AddFileInfo(const CFileInfo& rFileInfo)
{
	m_vFiles.push_back(rFileInfo);
}

void CFileInfoArray::AppendArray(const CFileInfoArray& arrFiles)
{
	m_vFiles.insert(m_vFiles.end(), arrFiles.m_vFiles.begin(), arrFiles.m_vFiles.end());
}

size_t CFileInfoArray::GetSize() const
{
	return m_vFiles.size();
}

CFileInfo& CFileInfoArray::GetAt(const size_t &stIndex)
{
	return m_vFiles.at(stIndex);
}

void CFileInfoArray::Clear()
{
	m_vFiles.clear();
}

void CFileInfoArray::Store(icpf::archive& ar, bool bOnlyFlags)
{
	INT_PTR iSize = m_vFiles.size();
	ar << iSize;
	for (std::vector<CFileInfo>::iterator iterFile = m_vFiles.begin(); iterFile != m_vFiles.end(); ++iterFile)
	{
		if(bOnlyFlags)
			ar << (*iterFile).GetFlags();
		else
			(*iterFile).Store(ar);
	}
}

void CFileInfoArray::Load(icpf::archive& ar, bool bOnlyFlags)
{
	INT_PTR iSize;
	ar>>iSize;

	// workaround for a problem, where '0' was stored as int instead of INT_PTR;
	// in this case on x86_64 iSize could have some enormous size (because we read
	// someone else's data following the int value.
	// Try to avoid reading later some invalid data (since we have stolen 4 bytes on x86_64).
	if(iSize > INT_MAX)
		THROW(_T("[CFileInfoArray::Load()] Corrupted task data (bug [#sf:2905339]"), 0, 0, 0);

	if(!bOnlyFlags)
	{
		m_vFiles.clear();
		m_vFiles.reserve(iSize);
	}

	CFileInfo fi;
	fi.SetClipboard(&m_rClipboard);
	uint_t uiFlags = 0;
	for (INT_PTR i = 0; i < iSize; i++)
	{
		if(bOnlyFlags)
		{
			CFileInfo& rInfo = m_vFiles.at(i);
			ar >> uiFlags;
			rInfo.SetFlags(uiFlags);
		}
		else
		{
			fi.Load(ar);
			m_vFiles.push_back(fi);
		}
	}
}
