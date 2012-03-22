#include "stdafx.h"
#include "resource.h"
#include "chext.h"

#include "dllmain.h"

#include "MenuExt.h"
#include "DropMenuExt.h"
#include "ShellExtControl.h"
#include "..\Common\ipcstructs.h"

CCHExtModule _AtlModule;

// common memory - exactly 64kB
CSharedConfigStruct* g_pscsShared;
static HANDLE hMapObject=NULL;

OBJECT_ENTRY_AUTO(CLSID_MenuExt, CMenuExt)
OBJECT_ENTRY_AUTO(CLSID_DropMenuExt, CDropMenuExt)
OBJECT_ENTRY_AUTO(CLSID_CShellExtControl, CShellExtControl)


extern "C"
BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		DisableThreadLibraryCalls(hInstance);

		// memory mapped file
		hMapObject = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(CSharedConfigStruct), _T("CHLMFile"));    // name of map object
		if (hMapObject == NULL) 
			return FALSE; 

		// Get a pointer to the file-mapped shared memory.
		g_pscsShared = (CSharedConfigStruct*)MapViewOfFile(hMapObject, FILE_MAP_WRITE, 0, 0, 0);
		if (g_pscsShared == NULL) 
			return FALSE; 
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		// Unmap shared memory from the process's address space.
		UnmapViewOfFile((LPVOID)g_pscsShared); 

		// Close the process's handle to the file-mapping object.
		CloseHandle(hMapObject); 
	}

	return _AtlModule.DllMain(dwReason, lpReserved);
}