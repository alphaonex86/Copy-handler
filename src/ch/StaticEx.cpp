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
#include "StaticEx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define STATICEX_CLASS _T("STATICEX")

LRESULT CALLBACK StaticExWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	STATICEXSETTINGS* pSettings=(STATICEXSETTINGS*)GetWindowLongPtr(hwnd, 0);
	switch (uMsg)
	{
	case WM_NCCREATE:
		{
			STATICEXSETTINGS* pSett=new STATICEXSETTINGS;
			pSett->hFontNormal=NULL;
			pSett->hFontUnderline=NULL;
			pSett->pszLink=NULL;
			pSett->pszText=NULL;
			pSett->bActive=false;
			pSett->bDown=false;
			pSett->hLink=NULL;
			pSett->hNormal=NULL;
			pSett->rcText.left=0;
			pSett->rcText.right=0;
			pSett->rcText.top=0;
			pSett->rcText.bottom=0;
			::SetWindowLongPtr(hwnd, 0, (LONG_PTR)pSett);

			// create cursors
			pSett->hNormal=::LoadCursor(NULL, IDC_ARROW);
			pSett->hLink=::LoadCursor(NULL, IDC_HAND);

			break;
		}
	case WM_CREATE:
		{
			CREATESTRUCT* pcs=(CREATESTRUCT*)lParam;

			const TCHAR* pSep=_tcsrchr(pcs->lpszName, _T('|'));

			if (!(pcs->style & SES_LINK) || pSep == NULL || pSep-pcs->lpszName < 0)
			{
				pSettings->pszText=new TCHAR[_tcslen(pcs->lpszName)+1];
				_tcscpy(pSettings->pszText, pcs->lpszName);
				pSettings->pszLink=NULL;
			}
			else
			{
				pSettings->pszText=new TCHAR[pSep-pcs->lpszName+1];
				_tcsncpy(pSettings->pszText, pcs->lpszName, pSep-pcs->lpszName);
				pSettings->pszText[pSep-pcs->lpszName]=_T('\0');
				pSep++;
				pSettings->pszLink=new TCHAR[_tcslen(pSep)+1];
				_tcscpy(pSettings->pszLink, pSep);
			}

			break;
		}
	case WM_NCDESTROY:
		{
			if (pSettings->hFontNormal)
				DeleteObject(pSettings->hFontNormal);
			if (pSettings->hFontUnderline)
				DeleteObject(pSettings->hFontUnderline);
			if (pSettings->hLink)
				DeleteObject(pSettings->hLink);
			if (pSettings->hNormal)
				DeleteObject(pSettings->hNormal);
			delete [] pSettings->pszLink;
			delete [] pSettings->pszText;

			delete pSettings;
			break;
		}
	case WM_SETFONT:
		{
			// delete old fonts
			if (pSettings->hFontNormal)
				DeleteObject(pSettings->hFontNormal);
			if (pSettings->hFontUnderline)
				DeleteObject(pSettings->hFontUnderline);

			// new font - create a font based on it (the normal and the underlined one)
			HFONT hfont=(HFONT)wParam;
			LOGFONT lf;
			if (GetObject(hfont, sizeof(LOGFONT), &lf) != 0)
			{
				// size
				if (::GetWindowLong(hwnd, GWL_STYLE) & SES_LARGE)
					lf.lfHeight=(long)(lf.lfHeight*1.25);

				// create a font
				if (::GetWindowLong(hwnd, GWL_STYLE) & SES_BOLD)
					lf.lfWeight=FW_BOLD;
				pSettings->hFontNormal=CreateFontIndirect(&lf);
				lf.lfUnderline=TRUE;
				pSettings->hFontUnderline=CreateFontIndirect(&lf);
			}
			else
			{
				pSettings->hFontNormal=NULL;
				pSettings->hFontUnderline=NULL;
			}

			break;
		}
	case WM_SETTEXT:
		{
			// delete the old font
			delete [] pSettings->pszText;
			delete [] pSettings->pszLink;

			// style
			LONG lStyle=::GetWindowLong(hwnd, GWL_STYLE);

			LPCTSTR psz=(LPCTSTR)lParam;
			const TCHAR* pSep=_tcsrchr(psz, _T('|'));

			if (!(lStyle & SES_LINK) || pSep == NULL || pSep-psz < 0)
			{
				pSettings->pszText=new TCHAR[_tcslen(psz)+1];
				_tcscpy(pSettings->pszText, psz);
				pSettings->pszLink=NULL;
			}
			else
			{
				pSettings->pszText=new TCHAR[pSep-psz+1];
				_tcsncpy(pSettings->pszText, psz, pSep-psz);
				pSettings->pszText[pSep-psz]=_T('\0');
				pSep++;
				pSettings->pszLink=new TCHAR[_tcslen(pSep)+1];
				_tcscpy(pSettings->pszLink, pSep);
			}

			::RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE);
			break;
		}
	case WM_ERASEBKGND:
		{
			return (LRESULT)FALSE;
			break;
		}
	case WM_PAINT:
		{
			// draw anything
			PAINTSTRUCT ps;
			HDC hDC=BeginPaint(hwnd, &ps);

			// flicker-free drawing
			HDC hdc=::CreateCompatibleDC(hDC);
			HBITMAP hBmp=::CreateCompatibleBitmap(hDC, ps.rcPaint.right-ps.rcPaint.left+1, ps.rcPaint.bottom-ps.rcPaint.top+1);
			HBITMAP hOldBitmap=(HBITMAP)::SelectObject(hdc, hBmp);
			::SetWindowOrgEx(hdc, ps.rcPaint.left, ps.rcPaint.top, NULL);

			// paint the background
			::FillRect(hdc, &ps.rcPaint, ::GetSysColorBrush(COLOR_BTNFACE));

			// size of the all control
			RECT rcCtl;
			::GetClientRect(hwnd, &rcCtl);

			// draw text
			DWORD dwFlags=DT_LEFT | DT_VCENTER | (::GetWindowLong(hwnd, GWL_STYLE) & SES_PATHELLIPSIS ? DT_PATH_ELLIPSIS : 0)
				| (::GetWindowLong(hwnd, GWL_STYLE) & SES_ELLIPSIS ? DT_END_ELLIPSIS : 0)
				| (::GetWindowLong(hwnd, GWL_STYLE) & SES_WORDBREAK ? DT_WORDBREAK : 0);

			pSettings->rcText=rcCtl;
			if (::GetWindowLong(hwnd, GWL_STYLE) & SES_LINK)
			{
				HFONT hOld=(HFONT)::SelectObject(hdc, pSettings->hFontUnderline);
				::SetBkMode(hdc, TRANSPARENT);

				COLORREF crColor=(pSettings->bActive ? (RGB(255, 0, 0)) : (RGB(0, 0, 255)));
				::SetTextColor(hdc, crColor);

				if (pSettings->pszText)
				{
					DrawText(hdc, pSettings->pszText, -1, &pSettings->rcText, dwFlags | DT_CALCRECT);
					DrawText(hdc, pSettings->pszText, -1, &rcCtl, dwFlags);
				}
				else
				{
					pSettings->rcText.left=0;
					pSettings->rcText.right=0;
					pSettings->rcText.top=0;
					pSettings->rcText.bottom=0;
				}

				::SelectObject(hdc, hOld);
			}
			else
			{
				// aesthetics
				rcCtl.left+=3;
				rcCtl.right-=3;

				// draw
				HFONT hOld=(HFONT)::SelectObject(hdc, pSettings->hFontNormal);
				::SetBkMode(hdc, TRANSPARENT);
				::SetTextColor(hdc, ::GetSysColor(COLOR_BTNTEXT));

				if (pSettings->pszText)
				{
					DWORD dwMod=(::GetWindowLong(hwnd, GWL_STYLE) & SES_RALIGN) ? DT_RIGHT : 0;
					DrawText(hdc, pSettings->pszText, -1, &pSettings->rcText, dwFlags | DT_CALCRECT | dwMod);
					DrawText(hdc, pSettings->pszText, -1, &rcCtl, dwFlags | dwMod);
				}
				else
				{
					pSettings->rcText.left=0;
					pSettings->rcText.right=0;
					pSettings->rcText.top=0;
					pSettings->rcText.bottom=0;
				}

				::SelectObject(hdc, hOld);
			}

			// free the compatible dc
			::BitBlt(ps.hdc, ps.rcPaint.left, ps.rcPaint.top, ps.rcPaint.right-ps.rcPaint.left+1, ps.rcPaint.bottom-ps.rcPaint.top+1, 
				hdc, ps.rcPaint.left, ps.rcPaint.top, SRCCOPY);
			::SelectObject(hdc, hOldBitmap);
			::DeleteObject(hBmp);
			::DeleteDC(hdc);

			EndPaint(hwnd, &ps);

			break;
		}
	case WM_MOUSEMOVE:
		{
			if (::GetWindowLong(hwnd, GWL_STYLE) & SES_LINK)
			{
				POINT pt = { LOWORD(lParam), HIWORD(lParam) };
				
				if (pSettings->bActive)
				{
					if (!::PtInRect(&pSettings->rcText, pt))
					{
						pSettings->bActive=false;
						::ReleaseCapture();
						::RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE);
						
						::SetCursor(pSettings->hNormal);
					}
				}
				else
				{
					if (::PtInRect(&pSettings->rcText, pt))
					{
						pSettings->bActive=true;
						::RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE);
						::SetCapture(hwnd);
						::SetCursor(pSettings->hLink);
					}
				}
			}
			break;
		}
	case WM_LBUTTONDOWN:
		{
			pSettings->bDown=true;
			break;
		}
	case WM_LBUTTONUP:
		{
			POINT pt={ LOWORD(lParam), HIWORD(lParam) };
			if (pSettings->bDown && ::GetWindowLong(hwnd, GWL_STYLE) & SES_LINK && ::PtInRect(&pSettings->rcText, pt))
			{
				if (::GetWindowLong(hwnd, GWL_STYLE) & SES_NOTIFY)
				{
					::SendMessage((HWND)::GetWindowLong(hwnd, GWLP_HWNDPARENT), WM_COMMAND, (WPARAM)(SEN_CLICKED << 16 | ::GetWindowLong(hwnd, GWL_ID)), (LPARAM)hwnd);
				}
				else
				{
					
					TRACE("Executing %s...\n", pSettings->pszLink);
					ShellExecute(NULL, _T("open"), pSettings->pszLink, NULL, NULL, SW_SHOWNORMAL);
				}
			}
			pSettings->bDown=false;

			break;
		}
	case WM_CANCELMODE:
		{
			pSettings->bActive=false;
			pSettings->bDown=false;
			break;
		}
	case SEM_GETLINK:
		{
			// wParam - count
			// lParam - addr of a buffer
			if (pSettings->pszLink)
				_tcsncpy((PTSTR)lParam, pSettings->pszLink, (int)wParam);
			else
				_tcscpy((PTSTR)lParam, _T(""));

			return (LRESULT)TRUE;
			break;
		}
	}

	return ::DefWindowProc(hwnd, uMsg, wParam, lParam);
}

bool RegisterStaticExControl(HINSTANCE hInstance)
{
    WNDCLASS wndcls;

    if (!(::GetClassInfo(hInstance, STATICEX_CLASS, &wndcls)))
    {
        // need to register a new class
        wndcls.style = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW;
		wndcls.lpfnWndProc = ::StaticExWndProc;
        wndcls.cbClsExtra = 0;
		wndcls.cbWndExtra = sizeof(STATICEXSETTINGS*);
        wndcls.hInstance = hInstance;
        wndcls.hIcon = NULL;
		wndcls.hCursor = NULL;				// will load each time needed
        wndcls.hbrBackground = NULL;
        wndcls.lpszMenuName = NULL;
        wndcls.lpszClassName = STATICEX_CLASS;

        if (!RegisterClass(&wndcls))
            return false;
    }

    return true;
}
