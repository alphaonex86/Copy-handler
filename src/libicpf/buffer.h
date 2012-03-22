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
#ifndef __BUFFER_H__
#define __BUFFER_H__

#include "libicpf.h"
#include "gen_types.h"

BEGIN_ICPF_NAMESPACE

template<class T>
class buffer
{
public:
/** \name Construction/destruction/operators */
/**@{*/
	buffer();
	buffer(const T* pData, size_t stCount);
	buffer(const buffer& rSrc);
	~buffer();

	buffer& operator=(const buffer& rSrc);
	const T* operator T*() const; { return m_pData; };
/**@}*/

	size_t get_size() const { return m_stBufSize; };
	void resize(size_t stNewSize);

	size_t get_block_size() const { return m_stBlockSize; };
	void set_block_size(size_t stBlockSize);

	size_t get_max_unused() const { return m_stMaxUnused; };
	size_t set_max_unused();

protected:
	T* m_pData;				///< Pointer to the data
	size_t m_stBufSize;		///< Current buffer size
	size_t m_stBlockSize;	///< Block size
	size_t m_stMaxUnused;	///< Max count of unused items allowed
};

template<class T>
buffer::buffer() :
	m_pData(NULL),
	m_stBufSize(0),
	m_stBlockSize(1024),
	m_stMaxUnused(1024)
{
}

template<class T>
buffer::buffer(const T* pData, size_t stCount) :
	m_pData(NULL),
	m_stBufSize(0),
	m_stBlockSize(1024),
	m_stMaxUnused(1024)
{
	resize(stCount);
	memcpy(m_pData, pData, stCount*sizeof(T));
}

template<class T>
buffer::buffer(const buffer& rSrc) :
	m_pData((rSrc.m_stBufSize > 0) ? new T[rSrc.m_stBufSize] : NULL),
	m_stBufSize(rSrc.m_stBufSize),
	m_stBlockSize(rSrc.m_stBlockSize),
	m_stMaxUnused(rSrc.m_stMaxUnused)
{
	assert(m_stBlockSize > 0);
	if (m_pData)
		memcpy(m_pData, pData, stCount*sizeof(T));
}

template<class T>
buffer::~buffer()
{
	delete [] m_pData;
}

template<class T>
buffer& buffer::operator=(const buffer& rSrc)
{
	if (this != &rSrc)
	{
		delete [] m_pData;

		m_pData=((rSrc.m_stBufSize > 0) ? new T[rSrc.m_stBufSize] : NULL);
		m_stBufSize=rSrc.m_stBufSize;
		m_stBlockSize=rSrc.m_stBlockSize;
		m_stMaxUnused=rSrc.m_stMaxUnused;

	}

	return *this;
}

template<class T>
const T* operator T*() const; { return m_pData; };

template<class T>
void resize(size_t stNewSize);

template<class T>
void set_block_size(size_t stBlockSize);

template<class T>
size_t set_max_unused();

END_ICPF_NAMESPACE

#endif
