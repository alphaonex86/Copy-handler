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
#ifndef __RESOURCEMANAGER_H__
#define __RESOURCEMANAGER_H__

#include "libictranslate.h"
#include <set>
#include <map>
#include <vector>
#include <list>
#include "../libicpf/gen_types.h"

using namespace std;

BEGIN_ICTRANSLATE_NAMESPACE

/////////////////////////////////////////////////////////////////////////
// types of notifications
#define WM_RMNOTIFY (WM_USER + 2)

// RMNT_LANGCHANGE, LPARAM - HIWORD - old language, LOWORD - new language
#define RMNT_LANGCHANGE		0x0001

typedef void(*PFNNOTIFYCALLBACK)(uint_t);

class LIBICTRANSLATE_API CFormat
{
public:
	CFormat() {};
	CFormat(const tchar_t* pszFormat);
	~CFormat();

	void SetFormat(const tchar_t* pszFormat);

	CFormat& SetParam(PCTSTR pszName, PCTSTR pszText);
	CFormat& SetParam(PCTSTR pszName, ull_t ullData);
	CFormat& SetParam(PCTSTR pszName, ll_t llData);
	CFormat& SetParam(PCTSTR pszName, ulong_t ulData);
	CFormat& SetParam(PCTSTR pszName, uint_t uiData);
	CFormat& SetParam(PCTSTR pszName, int_t iData);
	CFormat& SetParam(PCTSTR pszName, bool bData);

	operator const tchar_t*() const { return m_strText.c_str(); }

protected:
	tstring_t m_strText;
};

///////////////////////////////////////////////////////////
// language description structure
class LIBICTRANSLATE_API CTranslationItem
{
public:
	enum ECompareResult
	{
		eResult_Valid,		// valid translation
		eResult_Invalid,	// Invalid checksum or translation
		eResult_ContentWarning	// the translation is suspicious
	};
public:
	CTranslationItem();
	CTranslationItem(const tchar_t* pszText, uint_t uiChecksum);
	~CTranslationItem();

	CTranslationItem& operator=(const CTranslationItem& rSrc);

	void Clear();
	void CalculateChecksum();

	const tchar_t* GetText() const;
	void SetText(const tchar_t* pszText, bool bUnescapeString);
	uint_t GetChecksum() const { return m_uiChecksum; }
	void SetChecksum(uint_t uiChecksum) { m_uiChecksum = uiChecksum; }

	void UnescapeString();

	ECompareResult Compare(const CTranslationItem& rReferenceItem);

protected:
	bool GetFormatStrings(std::set<tstring_t>& setFmtStrings) const;

protected:
	tchar_t* m_pszText;
	size_t m_stTextLength;
	uint_t m_uiChecksum;
};

typedef void(*PFNENUMCALLBACK)(uint_t, const CTranslationItem*, ptr_t);
typedef std::map<uint_t, CTranslationItem> translation_map;

class LIBICTRANSLATE_API CLangData
{
public:
// construction/destruction
	CLangData();
	CLangData(const CLangData& ld);
	~CLangData();

	CLangData& operator=(const CLangData& rSrc);

	void Clear();
// operations
	bool ReadInfo(PCTSTR pszFile);
	bool ReadTranslation(PCTSTR pszFile, bool bReadBase = false, bool bIgnoreVersion = false);
	void WriteTranslation(PCTSTR pszPath);

// translation retrieving/setting
	const tchar_t* GetString(WORD wHiID, WORD wLoID);		// retrieves string using group id and string id
	void EnumStrings(PFNENUMCALLBACK pfnCallback, ptr_t pData);	// retrieves all translation items

	CTranslationItem* GetTranslationItem(uint_t uiTranslationKey, bool bCreate);	// retrieves pointer to the single translation item
	bool Exists(uint_t uiTranslationKey) const;
	void CleanupTranslation(const CLangData& rReferenceTranslation);

// attributes
	void SetFilename(PCTSTR psz);
	PCTSTR GetFilename(bool bFullPath) const;

	void SetLangName(PCTSTR psz);
	PCTSTR GetLangName() const { return m_pszLngName; };

	void SetFontFace(PCTSTR psz);
	PCTSTR GetFontFace() const { return m_pszFontFace; };

	void SetPointSize(WORD wSize) { m_wPointSize=wSize; m_bModified = true; };
	WORD GetPointSize() const { return m_wPointSize; };

	void SetDirection(bool brtl) { m_bRTL=brtl; m_bModified = true; };
	bool GetDirection() const { return m_bRTL; };

	void SetHelpName(PCTSTR psz);
	PCTSTR GetHelpName() const { return m_pszHelpName; };

	void SetAuthor(PCTSTR psz);
	PCTSTR GetAuthor() const { return m_pszAuthor; };

	bool IsModified() const { return m_bModified; }
	void SetModified() { m_bModified = true; }

	bool IsValidDescription() const;
protected:
	void SetFnameData(PTSTR *ppszDst, PCTSTR pszSrc);
	static void EnumAttributesCallback(bool bGroup, const tchar_t* pszName, const tchar_t* pszValue, ptr_t pData);
	static void WriteAttributesCallback(bool bGroup, const tchar_t* pszName, const tchar_t* pszValue, ptr_t pData);
	static void UnescapeString(tchar_t* pszData);

protected:
	TCHAR *m_pszFilename;		// file name of the language data (with path)
	TCHAR *m_pszLngName;		// name of the language (ie. Chinese (PRC))
	TCHAR *m_pszFontFace;		// face name of the font that will be used in dialogs
	WORD m_wPointSize;		// font point size
	TCHAR *m_pszHelpName;		// help name (wo the directory) for this language
	TCHAR *m_pszAuthor;		// author name
	bool m_bRTL;				// does the language require right-to-left reading order ?

	// strings (for controls in dialog boxes the ID contains hi:dlg ID, lo:ctrl ID, for strings hi part is 0)
	translation_map m_mapTranslation;		// maps string ID to the offset in pszStrings

private:
	uint_t m_uiSectionID;			///< ID of the currently processed section
	bool m_bUpdating;				///< Are we updating the language with base language ?
	bool m_bModified;				///< States if the translation has been modified
};

/////////////////////////////////////////////////////////////////////////////////////

class LIBICTRANSLATE_API CResourceManager
{
protected:
	CResourceManager();
	~CResourceManager();

public:
	static CResourceManager& Acquire();

	void Init(HMODULE hrc);

	void SetCallback(PFNNOTIFYCALLBACK pfn) { m_pfnCallback=pfn; };

	void Scan(LPCTSTR pszFolder, vector<CLangData>* pvData);
	bool SetLanguage(PCTSTR pszPath);

	// loading functions
	HGLOBAL LoadResource(LPCTSTR pszType, LPCTSTR pszName);
	HACCEL LoadAccelerators(LPCTSTR pszName);
	HBITMAP LoadBitmap(LPCTSTR pszName);
	HCURSOR LoadCursor(LPCTSTR pszName);
	HICON LoadIcon(LPCTSTR pszName);
	HANDLE LoadImage(LPCTSTR lpszName, UINT uType, int cxDesired, int cyDesired, UINT fuLoad);
	HMENU LoadMenu(LPCTSTR pszName);
	LPDLGTEMPLATE LoadDialog(LPCTSTR pszName);

	PCTSTR LoadString(UINT uiID);
	PCTSTR LoadString(WORD wGroup, WORD wID);
	PTSTR LoadStringCopy(UINT uiID, PTSTR pszStr, UINT uiMax);

	// res updating functions
	void UpdateMenu(HMENU hMenu, WORD wMenuID);

	const CLangData* GetLanguageData() const { return &m_ld; }

public:
	CLangData m_ld;				// current language data
	list<CWnd*> m_lhDialogs;	// currently displayed dialog boxes (even hidden)

	uint_t m_uiNotificationMsgID;	// window message to send to windows
	HMODULE m_hRes;
	PFNNOTIFYCALLBACK m_pfnCallback;
//	UINT m_uiMsg;
	CRITICAL_SECTION m_cs;

protected:
	static CResourceManager S_ResourceManager;
};

END_ICTRANSLATE_NAMESPACE

#endif