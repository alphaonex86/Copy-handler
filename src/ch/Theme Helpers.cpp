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
#include "Theme Helpers.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CUxThemeSupport::CUxThemeSupport()
{
	m_hThemesDll=LoadLibrary(_T("UxTheme.dll"));
}

CUxThemeSupport::~CUxThemeSupport()
{
	if (m_hThemesDll)
		FreeLibrary(m_hThemesDll);
}

HTHEME CUxThemeSupport::OpenThemeData(HWND hwnd, LPCWSTR pszClassList)
{
	ASSERT(m_hThemesDll);
	if(!m_hThemesDll)
		return NULL;

	PFNOPENTHEMEDATA pfnProc=(PFNOPENTHEMEDATA)GetProcAddress(m_hThemesDll, "OpenThemeData");

	if (pfnProc)
		return (*pfnProc)(hwnd, pszClassList);
	else
		return NULL;
}

HRESULT CUxThemeSupport::CloseThemeData(HTHEME hTheme)
{
	ASSERT(m_hThemesDll);
	if(!m_hThemesDll)
		return E_FAIL;

	PFNCLOSETHEMEDATA pfnProc=(PFNCLOSETHEMEDATA)GetProcAddress(m_hThemesDll, "CloseThemeData");

	if (pfnProc)
		return (*pfnProc)(hTheme);
	else
		return E_UNEXPECTED;
}

HRESULT CUxThemeSupport::DrawThemeEdge(HTHEME hTheme, HDC hdc, int iPartId, int iStateId, const RECT* pDestRect, UINT uEdge, UINT uFlags, RECT* pContentRect)
{
	ASSERT(m_hThemesDll);
	if(!m_hThemesDll)
		return E_FAIL;

	PFNDRAWTHEMEEDGE pfnProc=(PFNDRAWTHEMEEDGE)GetProcAddress(m_hThemesDll, "DrawThemeEdge");

	if (pfnProc)
		return (*pfnProc)(hTheme, hdc, iPartId, iStateId, pDestRect, uEdge, uFlags, pContentRect);
	else
		return E_UNEXPECTED;
}

HRESULT CUxThemeSupport::DrawThemeBackground(HTHEME hTheme, HDC hdc, int iPartId, int iStateId, const RECT *pRect, OPTIONAL const RECT *pClipRect)
{
	ASSERT(m_hThemesDll);
	if(!m_hThemesDll)
		return E_FAIL;

	PFNDRAWTHEMEBACKGROUND pfnProc=(PFNDRAWTHEMEBACKGROUND)GetProcAddress(m_hThemesDll, "DrawThemeBackground");

	if (pfnProc)
		return (*pfnProc)(hTheme, hdc, iPartId, iStateId, pRect, pClipRect);
	else
		return E_UNEXPECTED;
}

HRESULT CUxThemeSupport::DrawThemeParentBackground(HWND hwnd, HDC hdc, RECT* prc)
{
	ASSERT(m_hThemesDll);
	if(!m_hThemesDll)
		return E_FAIL;

	PFNDRAWTHEMEPARENTBACKGROUND pfnProc=(PFNDRAWTHEMEPARENTBACKGROUND)GetProcAddress(m_hThemesDll, "DrawThemeParentBackground");

	if (pfnProc)
		return (*pfnProc)(hwnd, hdc, prc);
	else
		return E_UNEXPECTED;
}

BOOL CUxThemeSupport::IsAppThemed()
{
	ASSERT(m_hThemesDll);
	if(!m_hThemesDll)
		return FALSE;

	PFNISAPPTHEMED pfnProc=(PFNISAPPTHEMED)GetProcAddress(m_hThemesDll, "IsAppThemed");

	if (pfnProc)
		return (*pfnProc)();
	else
		return FALSE;
}
