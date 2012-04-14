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
#ifndef __DATABUFFER_H__
#define __DATABUFFER_H__

#include "../libicpf/file.h"

#define DEFAULT_SIZE	65536

#define ROUNDTODS(number)\
	((number + DEFAULT_SIZE - 1) & ~(DEFAULT_SIZE-1))

#define ROUNDUP(number, to)\
	((number + to - 1) & ~(to-1))

#define ROUNDDOWN(number, to)\
	(number & ~(to-1))

#define BI_DEFAULT		0
#define BI_ONEDISK		1
#define BI_TWODISKS		2
#define BI_CD			3
#define BI_LAN			4

#pragma warning (disable: 4201) 
struct BUFFERSIZES
{
	void Serialize(icpf::archive& ar);
	bool operator==(const BUFFERSIZES& bsSizes) const;
	union
	{
		struct
		{
			UINT m_uiDefaultSize;		// default buffer size
			UINT m_uiOneDiskSize;		// inside one disk boundary
			UINT m_uiTwoDisksSize;		// two disks
			UINT m_uiCDSize;			// CD<->anything
			UINT m_uiLANSize;			// LAN<->anything
		};
		UINT m_auiSizes[5];
	};
	bool m_bOnlyDefault;
};
#pragma warning (default: 4201)

class CDataBuffer
{
public:
	CDataBuffer() { m_pBuffer=NULL; m_uiRealSize=0; m_bsSizes.m_uiDefaultSize=0; m_bsSizes.m_uiOneDiskSize=0; m_bsSizes.m_uiTwoDisksSize=0; m_bsSizes.m_uiCDSize=0; m_bsSizes.m_uiLANSize=0; m_bsSizes.m_bOnlyDefault=false; };
	~CDataBuffer() { Delete(); };

	const BUFFERSIZES* Create(const BUFFERSIZES* pbsSizes);	// (re)allocates the buffer; if there's an error - restores previous buffer size
	void Delete();				// deletes buffer

	UINT GetRealSize() { return m_uiRealSize; };
	UINT GetDefaultSize() { return m_bsSizes.m_uiDefaultSize; };
	UINT GetOneDiskSize() { return m_bsSizes.m_uiOneDiskSize; };
	UINT GetTwoDisksSize() { return m_bsSizes.m_uiTwoDisksSize; };
	UINT GetCDSize() { return m_bsSizes.m_uiCDSize; };
	UINT GetLANSize() { return m_bsSizes.m_uiLANSize; };
	const BUFFERSIZES* GetSizes() { return &m_bsSizes; }; 

	// operators
	operator unsigned char*() { return m_pBuffer; };
protected:
	unsigned char *m_pBuffer;	// buffer address
	UINT m_uiRealSize;			// real buffer size
	BUFFERSIZES m_bsSizes;
};

#endif