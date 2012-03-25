#include "PluginInterface_Listener.h"

#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <shlobj.h>
#include <stdio.h>

PluginInterface_Listener::PluginInterface_Listener(PluginInterface_Listener_Return * returnObject)
{
	this->returnObject=returnObject;
}

void PluginInterface_Listener_Return::newCopy(unsigned int orderId,std::vector<std::string> sources)
{
	std::string empty("");
	std::string folder=getDirectory();
	if(folder.compare(empty)!=0)
		newCopy(orderId,sources,folder);
}

void PluginInterface_Listener_Return::newMove(unsigned int orderId,std::vector<std::string> sources)
{
	std::string empty("");
	std::string folder=getDirectory();
	if(folder.compare(empty)!=0)
		newMove(orderId,sources,folder);
}

std::string PluginInterface_Listener_Return::getDirectory()
{
	bool f_selected = false;
	WCHAR szDir[MAX_PATH];
	BROWSEINFO bi;
	LPITEMIDLIST pidl;
	LPMALLOC pMalloc;
	if (SUCCEEDED (::SHGetMalloc (&pMalloc)))
	{
	     ::ZeroMemory (&bi,sizeof(bi));

	     bi.lpszTitle = L"Go ahead, select a directory:";
	     bi.hwndOwner = NULL;//this->GetSafeHwnd()
	     bi.pszDisplayName = 0;
	     bi.pidlRoot = 0;
	     bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_STATUSTEXT;
	     bi.lpfn = NULL;      //no customization function
	     bi.lParam = NULL;    //no parameters to the customization function

	     pidl = ::SHBrowseForFolder(&bi);
	     if (pidl)
	     {
		    if (::SHGetPathFromIDList (pidl, szDir))
		    {
			  f_selected = true;
		    }

		    pMalloc -> Free(pidl);
		    pMalloc -> Release();
	     }
	}

	if (f_selected)
	{
		char ch[MAX_PATH];
		char DefChar=' ';
		WideCharToMultiByte(CP_ACP,0,szDir,-1,ch,MAX_PATH,&DefChar,NULL);
		std::string returnedString(ch);
		return returnedString;
	}
	std::string returnedString;
	return returnedString;
}
