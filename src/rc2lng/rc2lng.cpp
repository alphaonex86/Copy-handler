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
// rc2lng.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "rc2lng.h"
#include "conio.h"
#include "rc.h"
#include "../libicpf/exception.h"

#pragma warning(disable : 4786)

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

int _tmain(int argc, TCHAR* argv[], TCHAR* /*envp*/[])
{
	// initialize MFC and print and error on failure
	HMODULE hModule = ::GetModuleHandle(NULL);
	if(!hModule)
		return -1;
	if (!AfxWinInit(hModule, NULL, ::GetCommandLine(), 0))
	{
		cerr << _T("Fatal Error: MFC initialization failed") << endl;
		return 1;
	}

	// usage - rc2lng infile.rc resource.h inheader.lng outfile.rc outfile.lng
	if (argc < 6)
	{
		wcerr << _T("Fatal Error: Incorrect numer of params") << endl;
		wcerr << _T("Usage: infile.rc inheader.lng outfile.rc outfile.lng resource.h resource2.h") << endl;
		return -1;
	}

	CRCFile rcFile;

	try
	{
		for (int i=5;i<argc;i++)
		{
			rcFile.ReadResourceIDs(argv[i]);
		}
		
		rcFile.ReadRC(argv[1]);

		rcFile.WriteRC(argv[3]);
		rcFile.WriteLang(argv[4], argv[2]);
	}
	catch(icpf::exception& e)
	{
		wcerr << e.get_desc() << endl;
		return -1;
	}
	return 0;
}
