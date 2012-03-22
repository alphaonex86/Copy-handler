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
#include "TrayIcon.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// CTrayIcon construction/creation/destruction

CTrayIcon::CTrayIcon()
{
	memset(&m_tnd, 0, sizeof(m_tnd));
	m_bHidden=false;
}

CTrayIcon::CTrayIcon(HWND hWnd, UINT uClbMsg, LPCTSTR szText, HICON hIcon, UINT uiID)
{
	CreateIcon(hWnd, uClbMsg, szText, hIcon, uiID);
	m_bHidden=false;
}

bool CTrayIcon::CreateIcon(HWND hWnd, UINT uClbMsg, LPCTSTR szText, HICON hIcon, UINT uiID)
{
	_ASSERT(hWnd);

	// load up the NOTIFYICONDATA structure
	m_tnd.cbSize=sizeof(NOTIFYICONDATA);
	m_tnd.hWnd=hWnd;
	m_tnd.uID=uiID;
	m_tnd.hIcon=hIcon;
	m_tnd.uFlags=NIF_MESSAGE | NIF_ICON | NIF_TIP;
	m_tnd.uCallbackMessage=uClbMsg;
	_tcsncpy(m_tnd.szTip, szText, 64);
	size_t tLen=_tcslen(szText);
	if (tLen < 64)
		m_tnd.szTip[tLen]=_T('\0');
	else
		m_tnd.szTip[63]=_T('\0');

	// Set the tray icon
	return Shell_NotifyIcon(NIM_ADD, &m_tnd) != FALSE;
}

CTrayIcon::~CTrayIcon()
{
	RemoveIcon();
}


/////////////////////////////////////////////////////////////////////////////
// CTrayIcon icon manipulation

void CTrayIcon::MoveToRight()
{
	HideIcon();
	ShowIcon();
}

void CTrayIcon::RemoveIcon()
{
	m_tnd.uFlags=0;
    Shell_NotifyIcon(NIM_DELETE, &m_tnd);
}

void CTrayIcon::HideIcon()
{
	if (!m_bHidden)
	{
		m_tnd.uFlags=NIF_ICON;
		Shell_NotifyIcon (NIM_DELETE, &m_tnd);
		m_bHidden=true;
	}
}

bool CTrayIcon::ShowIcon()
{
	if (m_bHidden)
	{
		m_tnd.uFlags=NIF_MESSAGE | NIF_ICON | NIF_TIP;
		m_bHidden=false;
		return Shell_NotifyIcon(NIM_ADD, &m_tnd) != FALSE;
	}
	return true;
}

bool CTrayIcon::SetIcon(HICON hIcon)
{
	m_tnd.uFlags=NIF_ICON;
	m_tnd.hIcon=hIcon;

	return Shell_NotifyIcon(NIM_MODIFY, &m_tnd) != 0;
}

bool CTrayIcon::SetStandardIcon(LPCTSTR lpIconName)
{
	HICON hIcon=::LoadIcon(NULL, lpIconName);
	return SetIcon(hIcon);
}
 
HICON CTrayIcon::GetIcon() const
{
	return m_tnd.hIcon;
}

/////////////////////////////////////////////////////////////////////////////
// CTrayIcon tooltip text manipulation

bool CTrayIcon::SetTooltipText(LPCTSTR pszTip)
{
	m_tnd.uFlags = NIF_TIP;
	_tcsncpy(m_tnd.szTip, pszTip, 64);
	size_t tLen=_tcslen(pszTip);
	if (tLen < 64)
		m_tnd.szTip[tLen]=_T('\0');
	else
		m_tnd.szTip[63]=_T('\0');

	return Shell_NotifyIcon(NIM_MODIFY, &m_tnd) != FALSE;
}

void CTrayIcon::GetTooltipText(LPTSTR pszText) const
{
	_tcscpy(pszText, m_tnd.szTip);
}

/////////////////////////////////////////////////////////////////////////////
// CTrayIcon notification window stuff

bool CTrayIcon::SetNotificationWnd(HWND hWnd)
{
	_ASSERT(hWnd);

	m_tnd.hWnd=hWnd;
	m_tnd.uFlags=0;

	return Shell_NotifyIcon(NIM_MODIFY, &m_tnd) != FALSE;
}

HWND CTrayIcon::GetNotificationWnd() const
{
	return m_tnd.hWnd;
}