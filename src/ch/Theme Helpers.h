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
#ifndef __THEME_SUPPORT__
#define __THEME_SUPPORT__

// definicja HTHEME - podobna do tej z UxTheme...h
#ifndef HTHEME
#define HTHEME HANDLE
#endif

typedef HTHEME(_stdcall *PFNOPENTHEMEDATA)(HWND hwnd, LPCWSTR pszClassList);
typedef HRESULT(_stdcall *PFNCLOSETHEMEDATA)(HTHEME hTheme);
typedef HRESULT(_stdcall *PFNDRAWTHEMEEDGE)(HTHEME hTheme, HDC hdc, int iPartId, int iStateId, const RECT* pDestRect, UINT uEdge, UINT uFlags, RECT* pContentRect);
typedef HRESULT(_stdcall *PFNDRAWTHEMEBACKGROUND)(HTHEME hTheme, HDC hdc, int iPartId, int iStateId, const RECT* pRect, const RECT* pClipRect);
typedef HRESULT(_stdcall *PFNDRAWTHEMEPARENTBACKGROUND)(HWND hwnd, HDC hdc, RECT* prc);
typedef BOOL(_stdcall *PFNISAPPTHEMED)();

class CUxThemeSupport
{
public:
	CUxThemeSupport();
	~CUxThemeSupport();

	HTHEME OpenThemeData(HWND hwnd, LPCWSTR pszClassList);
	HRESULT CloseThemeData(HTHEME hTheme);

	bool IsThemeSupported() { return m_hThemesDll != NULL; };
	BOOL IsAppThemed();

	HRESULT DrawThemeEdge(HTHEME hTheme, HDC hdc, int iPartId, int iStateId, const RECT* pDestRect, UINT uEdge, UINT uFlags, RECT* pContentRect);

	HRESULT DrawThemeBackground(HTHEME hTheme, HDC hdc, int iPartId, int iStateId, const RECT *pRect, OPTIONAL const RECT *pClipRect);
	HRESULT DrawThemeParentBackground(HWND hwnd, HDC hdc, RECT* prc);

/*	HRESULT DrawThemeText(HTHEME hTheme, HDC hdc, int iPartId, int iStateId, LPCWSTR pszText, int iCharCount, DWORD dwTextFlags, DWORD dwTextFlags2, const RECT *pRect);

THEMEAPI GetThemeBackgroundContentRect(HTHEME hTheme, OPTIONAL HDC hdc, 
    int iPartId, int iStateId,  const RECT *pBoundingRect, 
    OUT RECT *pContentRect);

THEMEAPI GetThemeBackgroundExtent(HTHEME hTheme, OPTIONAL HDC hdc,
    int iPartId, int iStateId, const RECT *pContentRect, 
    OUT RECT *pExtentRect);

THEMEAPI GetThemeTextMetrics(HTHEME hTheme, OPTIONAL HDC hdc, 
    int iPartId, int iStateId, OUT TEXTMETRIC* ptm);

THEMEAPI GetThemeBackgroundRegion(HTHEME hTheme, OPTIONAL HDC hdc,  
    int iPartId, int iStateId, const RECT *pRect, OUT HRGN *pRegion);

THEMEAPI HitTestThemeBackground(HTHEME hTheme, OPTIONAL HDC hdc, int iPartId, 
    int iStateId, DWORD dwOptions, const RECT *pRect, OPTIONAL HRGN hrgn, 
    POINT ptTest, OUT WORD *pwHitTestCode);

THEMEAPI DrawThemeEdge(HTHEME hTheme, HDC hdc, int iPartId, int iStateId, 
                       const RECT *pDestRect, UINT uEdge, UINT uFlags, OPTIONAL OUT RECT *pContentRect);

THEMEAPI DrawThemeIcon(HTHEME hTheme, HDC hdc, int iPartId, 
    int iStateId, const RECT *pRect, HIMAGELIST himl, int iImageIndex);

THEMEAPI_(BOOL) IsThemePartDefined(HTHEME hTheme, int iPartId, 
    int iStateId);

THEMEAPI_(BOOL) IsThemeBackgroundPartiallyTransparent(HTHEME hTheme, 
    int iPartId, int iStateId);

THEMEAPI GetThemeColor(HTHEME hTheme, int iPartId, 
    int iStateId, int iPropId, OUT COLORREF *pColor);

THEMEAPI GetThemeMetric(HTHEME hTheme, OPTIONAL HDC hdc, int iPartId, 
    int iStateId, int iPropId, OUT int *piVal);

THEMEAPI GetThemeString(HTHEME hTheme, int iPartId, 
    int iStateId, int iPropId, OUT LPWSTR pszBuff, int cchMaxBuffChars);

THEMEAPI GetThemeBool(HTHEME hTheme, int iPartId, 
    int iStateId, int iPropId, OUT BOOL *pfVal);

THEMEAPI GetThemeInt(HTHEME hTheme, int iPartId, 
    int iStateId, int iPropId, OUT int *piVal);

THEMEAPI GetThemeEnumValue(HTHEME hTheme, int iPartId, 
    int iStateId, int iPropId, OUT int *piVal);

THEMEAPI GetThemePosition(HTHEME hTheme, int iPartId, 
    int iStateId, int iPropId, OUT POINT *pPoint);

THEMEAPI GetThemeFont(HTHEME hTheme, OPTIONAL HDC hdc, int iPartId, 
    int iStateId, int iPropId, OUT LOGFONT *pFont);

THEMEAPI GetThemeRect(HTHEME hTheme, int iPartId, 
    int iStateId, int iPropId, OUT RECT *pRect);

THEMEAPI GetThemeMargins(HTHEME hTheme, OPTIONAL HDC hdc, int iPartId, 
    int iStateId, int iPropId, OPTIONAL RECT *prc, OUT MARGINS *pMargins);

THEMEAPI GetThemeIntList(HTHEME hTheme, int iPartId, 
    int iStateId, int iPropId, OUT INTLIST *pIntList);

THEMEAPI GetThemePropertyOrigin(HTHEME hTheme, int iPartId, 
    int iStateId, int iPropId, OUT enum PROPERTYORIGIN *pOrigin);

THEMEAPI SetWindowTheme(HWND hwnd, LPCWSTR pszSubAppName, 
    LPCWSTR pszSubIdList);

THEMEAPI GetThemeFilename(HTHEME hTheme, int iPartId, 
    int iStateId, int iPropId, OUT LPWSTR pszThemeFileName, int cchMaxBuffChars);

THEMEAPI_(COLORREF) GetThemeSysColor(HTHEME hTheme, int iColorId);

THEMEAPI_(HBRUSH) GetThemeSysColorBrush(HTHEME hTheme, int iColorId);

THEMEAPI_(BOOL) GetThemeSysBool(HTHEME hTheme, int iBoolId);

THEMEAPI_(int) GetThemeSysSize(HTHEME hTheme, int iSizeId);

THEMEAPI GetThemeSysFont(HTHEME hTheme, int iFontId, OUT LOGFONT *plf);

THEMEAPI GetThemeSysString(HTHEME hTheme, int iStringId, 
    OUT LPWSTR pszStringBuff, int cchMaxStringChars);

THEMEAPI GetThemeSysInt(HTHEME hTheme, int iIntId, int *piValue);

THEMEAPI_(BOOL) IsThemeActive();


THEMEAPI_(HTHEME) GetWindowTheme(HWND hwnd);


THEMEAPI EnableThemeDialogTexture(HWND hwnd, DWORD dwFlags);


THEMEAPI_(BOOL) IsThemeDialogTextureEnabled(HWND hwnd);

THEMEAPI_(DWORD) GetThemeAppProperties();

THEMEAPI_(void) SetThemeAppProperties(DWORD dwFlags);

THEMEAPI GetCurrentThemeName(
    OUT LPWSTR pszThemeFileName, int cchMaxNameChars, 
    OUT OPTIONAL LPWSTR pszColorBuff, int cchMaxColorChars,
    OUT OPTIONAL LPWSTR pszSizeBuff, int cchMaxSizeChars);

THEMEAPI GetThemeDocumentationProperty(LPCWSTR pszThemeName,
    LPCWSTR pszPropertyName, OUT LPWSTR pszValueBuff, int cchMaxValChars);

THEMEAPI DrawThemeParentBackground(HWND hwnd, HDC hdc, OPTIONAL RECT* prc);

THEMEAPI EnableTheming(BOOL fEnable);
*/
protected:

	HMODULE m_hThemesDll;
};

#endif
