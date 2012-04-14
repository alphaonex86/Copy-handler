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
/* Code based on code written by Chris Maunder (Chris.Maunder@cbr.clw.csiro.au) */

#ifndef __TRAYICON_H__
#define __TRAYICON_H__

class CTrayIcon
{
public:
// construction/destruction
	CTrayIcon();
	CTrayIcon(HWND hWnd, UINT uClbMsg, LPCTSTR szText, HICON hIcon, UINT uiID);
	~CTrayIcon();

//creation
	bool CreateIcon(HWND hWnd, UINT uClbMsg, LPCTSTR szText, HICON hIcon, UINT uiID);

// ToolTip text handleing
	bool SetTooltipText(LPCTSTR pszTip);
	void GetTooltipText(LPTSTR pszText) const;

// Icon handling
	bool SetIcon(HICON hIcon);
	bool SetStandardIcon(LPCTSTR lpIconName);
	HICON GetIcon() const;
	void HideIcon();
	bool ShowIcon();
	void RemoveIcon();
	void MoveToRight();

// Notifications
	bool SetNotificationWnd(HWND hWnd);
	HWND GetNotificationWnd() const;

// Attribs
public:
	bool m_bHidden;	// Has the icon been hidden?
	NOTIFYICONDATA m_tnd;
};

#endif