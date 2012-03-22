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
#include "DataBuffer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

bool BUFFERSIZES::operator==(const BUFFERSIZES& bsSizes) const
{
	return (m_uiDefaultSize == bsSizes.m_uiDefaultSize
			&& m_uiOneDiskSize == bsSizes.m_uiOneDiskSize
			&& m_uiTwoDisksSize == bsSizes.m_uiTwoDisksSize
			&& m_uiCDSize == bsSizes.m_uiCDSize
			&& m_uiLANSize == bsSizes.m_uiLANSize);
}

void BUFFERSIZES::Serialize(icpf::archive& ar)
{
	if (ar.is_storing())
	{
		ar<<m_uiDefaultSize;
		ar<<m_uiOneDiskSize;
		ar<<m_uiTwoDisksSize;
		ar<<m_uiCDSize;
		ar<<m_uiLANSize;
		ar<<static_cast<unsigned char>(m_bOnlyDefault);
	}
	else
	{
		ar>>m_uiDefaultSize;
		ar>>m_uiOneDiskSize;
		ar>>m_uiTwoDisksSize;
		ar>>m_uiCDSize;
		ar>>m_uiLANSize;
		unsigned char ucTemp;
		ar>>ucTemp;
		m_bOnlyDefault=(ucTemp != 0);
	}
}

const BUFFERSIZES* CDataBuffer::Create(const BUFFERSIZES* pbsSizes)
{
	// if trying to set 0-size buffer
	BUFFERSIZES bsSizes=*pbsSizes;	// copy - not to mix in the def. param

	if (bsSizes.m_uiDefaultSize == 0)
		bsSizes.m_uiDefaultSize=DEFAULT_SIZE;
	if (bsSizes.m_uiOneDiskSize == 0)
		bsSizes.m_uiOneDiskSize=DEFAULT_SIZE;
	if (bsSizes.m_uiTwoDisksSize == 0)
		bsSizes.m_uiTwoDisksSize=DEFAULT_SIZE;
	if (bsSizes.m_uiCDSize == 0)
		bsSizes.m_uiCDSize=DEFAULT_SIZE;
	if (bsSizes.m_uiLANSize == 0)
		bsSizes.m_uiLANSize=DEFAULT_SIZE;
	
	// max value from the all
	UINT uiLargest;
	if (bsSizes.m_bOnlyDefault)
		uiLargest=bsSizes.m_uiDefaultSize;
	else
	{
		uiLargest=(bsSizes.m_uiDefaultSize > bsSizes.m_uiOneDiskSize ? bsSizes.m_uiDefaultSize : bsSizes.m_uiOneDiskSize);
		if (uiLargest < bsSizes.m_uiTwoDisksSize)
			uiLargest=bsSizes.m_uiTwoDisksSize;
		if (uiLargest < bsSizes.m_uiCDSize)
			uiLargest=bsSizes.m_uiCDSize;
		if (uiLargest < bsSizes.m_uiLANSize)
			uiLargest=bsSizes.m_uiLANSize;
	}
	
	// modify buffer size to the next 64k boundary
	UINT uiRealSize=ROUNDTODS(uiLargest);
	TRACE("Size: %lu, rounded: %lu\n", uiLargest, uiRealSize);

	if (m_uiRealSize == uiRealSize)
	{
		// real buffersize doesn't changed
		m_bsSizes=bsSizes;
		
		return &m_bsSizes;
	}

	// try to allocate
	LPVOID pBuffer=VirtualAlloc(NULL, uiRealSize, MEM_COMMIT, PAGE_READWRITE);
	if (pBuffer == NULL)
	{
		if (m_pBuffer == NULL)
		{
			// try safe buffesize
			pBuffer=VirtualAlloc(NULL, DEFAULT_SIZE, MEM_COMMIT, PAGE_READWRITE);
			if (pBuffer == NULL)
				return &m_bsSizes;		// do not change anything
			
			// delete old buffer
			Delete();
			
			// store data
			m_pBuffer=static_cast<unsigned char*>(pBuffer);
			m_uiRealSize=DEFAULT_SIZE;
			m_bsSizes.m_bOnlyDefault=bsSizes.m_bOnlyDefault;
			m_bsSizes.m_uiDefaultSize=DEFAULT_SIZE;
			m_bsSizes.m_uiOneDiskSize=DEFAULT_SIZE;
			m_bsSizes.m_uiTwoDisksSize=DEFAULT_SIZE;
			m_bsSizes.m_uiCDSize=DEFAULT_SIZE;
			m_bsSizes.m_uiLANSize=DEFAULT_SIZE;
			
			return &m_bsSizes;
		}
		else
		{
			// no new buffer could be created - leave the old one
			return &m_bsSizes;
		}
	}
	else
	{
		// succeeded
		Delete();	// get rid of old buffer
		
		// store data
		m_pBuffer=static_cast<unsigned char*>(pBuffer);
		m_uiRealSize=uiRealSize;
		m_bsSizes=bsSizes;
		
		return &m_bsSizes;
	}
}

void CDataBuffer::Delete()
{
	if (m_pBuffer != NULL)
	{
		VirtualFree(static_cast<LPVOID>(m_pBuffer), 0, MEM_RELEASE);
		m_pBuffer=NULL;
		m_uiRealSize=0;
		m_bsSizes.m_uiDefaultSize=0;
		m_bsSizes.m_uiOneDiskSize=0;
		m_bsSizes.m_uiTwoDisksSize=0;
		m_bsSizes.m_uiCDSize=0;
		m_bsSizes.m_uiLANSize=0;
	}
}
