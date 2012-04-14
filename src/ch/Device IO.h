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
#ifndef __DEVICEIO_H__
#define __DEVICEIO_H__

// only NT
bool GetSignature(LPCTSTR lpszDrive, LPTSTR lpszBuffer, int iSize)
{
	std::auto_ptr<TCHAR> szMapping(new TCHAR[1024]);
	std::auto_ptr<TCHAR> szQuery(new TCHAR[16384]);
	std::auto_ptr<TCHAR> szSymbolic(new TCHAR[1024]);

	// read mappings
	if (QueryDosDevice(lpszDrive, szMapping.get(), 1024) == 0)
		return false;

	// search for all, to find out in which string is the signature
	int iCount, iCount2;
	if ((iCount=QueryDosDevice(NULL, szQuery.get(), 16384)) == 0)
	{
		TRACE("Encountered error #%lu @QueryDosDevice\n", GetLastError());
		return false;
	}

	size_t iOffset=0, iOffset2=0;
	TCHAR* pszSignature = NULL;
	TCHAR* pszOffset = NULL;
	while(iOffset < iCount)
	{
		if(_tcsncmp(szQuery.get() + iOffset, _T("STORAGE#Volume#"), _tcslen(_T("STORAGE#Volume#"))) == 0)
		{
			if((iCount2 = QueryDosDevice(szQuery.get() + iOffset, szSymbolic.get(), 1024)) == 0)
				return false;

			// now search for 'Signature' and extract (from szQuery+iOffset)
			pszSignature=_tcsstr(szQuery.get() + iOffset, _T("Signature"));
			if (pszSignature == NULL)
			{
				iOffset+=_tcslen(szQuery.get() + iOffset)+1;
				continue;
			}
			pszOffset=_tcsstr(pszSignature, _T("Offset"));
			if (pszOffset == NULL)
			{
				iOffset+=_tcslen(szQuery.get() + iOffset)+1;
				continue;
			}

			// for better string copying
			pszOffset[0]=_T('\0');

			// read values from szSymbolic and compare with szMapping
			iOffset2=0;
			while (iOffset2 < iCount2)
			{
				// compare szSymbolic+iOffset2 with szMapping
				if (_tcscmp(szMapping.get(), szSymbolic.get() + iOffset2) == 0)
				{
					// found Signature & Offset - copy
					int iCnt=reinterpret_cast<int>(pszOffset)-reinterpret_cast<int>(pszSignature)+1;
					_tcsncpy(lpszBuffer, pszSignature, (iCnt > iSize) ? iSize : iCnt);
					return true;
				}

				iOffset2+=_tcslen(szSymbolic.get())+1;
			}
		}

		iOffset+=_tcslen(szQuery.get() + iOffset)+1;
	}

	return false;
}

// at 9x function checks int13h devices and at NT within symbolic links
bool IsSamePhysicalDisk(int iDrvNum1, int iDrvNum2)
{
	OSVERSIONINFO osvi;
	osvi.dwOSVersionInfoSize=sizeof(OSVERSIONINFO);

	GetVersionEx(&osvi);
	if(osvi.dwPlatformId == VER_PLATFORM_WIN32_NT)
	{
		TCHAR drv1[3], drv2[3];
		
		drv1[0]=static_cast<TCHAR>(iDrvNum1+_T('A'));
		drv1[1]=_T(':');
		drv1[2]=_T('\0');
		drv2[0]=static_cast<TCHAR>(iDrvNum2+_T('A'));
		drv2[1]=_T(':');
		drv2[2]=_T('\0');
		
		TCHAR szSign1[512], szSign2[512];
		return (GetSignature(drv1, szSign1, 512) && GetSignature(drv2, szSign2, 512) && _tcscmp(szSign1, szSign2) == 0);
	}

	return false;
}

#endif
