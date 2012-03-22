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
#include "circ_buffer.h"
#include <stddef.h>
#include <string.h>
#include <assert.h>
#include "err_codes.h"
#include "exception.h"

BEGIN_ICPF_NAMESPACE

// amount of bytes by which the buffer size will be incremented
// if there is one byte to be put in a full buffer
#define _BUFFER_INC			(size_t)512ULL

// if there is at least _BUFFER_DEC free space in the buffer then the buffer
// will be shrank (shrinked?)
#define _BUFFER_DEC			(size_t)4096ULL

// specifies if circular buffer should shrink the buffer size if there is too much free
// space in the buffer
#define _USE_SHRINKING		1

// if defined to 1 then function ForwardSeek will empty the whole
// internal buffer if it does not find the specified value inside of it
#define _FAILSEEK_TRUNCATES		1

circular_buffer::circular_buffer() :
	m_pbyBuffer(NULL),
	m_tSize(0),
	m_tDataSize(0),
	m_tBitsAtEndCount(0)
{
}

circular_buffer::circular_buffer(const circular_buffer& rSrc) :
	m_pbyBuffer(NULL),
	m_tSize(0),
	m_tDataSize(0),
	m_tBitsAtEndCount(0)
{
	copy_from(rSrc);
}

circular_buffer::~circular_buffer()
{
	try
	{
		destroy();
	}
	catch(...)
	{
	}
}

circular_buffer& circular_buffer::operator=(const circular_buffer& rSrc)
{
	if (this == &rSrc)
	{
		// delete the old stuff
		destroy();
		copy_from(rSrc);
	}
	
	return *this;
}
	
void circular_buffer::copy_from(const circular_buffer& rSrc)
{
	if (rSrc.m_pbyBuffer && rSrc.m_tSize > 0)
	{
		// copy the old stuff
		m_pbyBuffer=new byte_t[rSrc.m_tSize];
		memcpy(m_pbyBuffer, rSrc.m_pbyBuffer, rSrc.m_tDataSize);
		m_tSize=rSrc.m_tSize;
		m_tDataSize=rSrc.m_tDataSize;
	}
	else
	{
		m_pbyBuffer=NULL;
		m_tSize=0;
		m_tDataSize=0;
	}
}

void circular_buffer::destroy()
{
	delete [] m_pbyBuffer;
	m_pbyBuffer=NULL;
	m_tSize=0;
	m_tDataSize=0;
}

void circular_buffer::push_data(const byte_t* pbyBuffer, size_t tCount)
{
	// check if there is enough space
	if (m_tDataSize+tCount > m_tSize)
		resize_buffer(m_tDataSize+tCount);
	
	// check for buffer
	assert(m_pbyBuffer);

	// now there is enough space - fill it
	memcpy(m_pbyBuffer+m_tDataSize, pbyBuffer, tCount);
	
	// increase the counters
	m_tDataSize+=tCount;
}

void circular_buffer::push_data(circular_buffer& rcb)
{
	if (rcb.m_pbyBuffer && rcb.m_tDataSize)
		push_data(rcb.m_pbyBuffer, rcb.m_tDataSize);
}

// pushes length and a string
void circular_buffer::push_string(const char_t* pszString)
{
	if (pszString)
	{
		ulong_t ulLen=(ulong_t)(strlen(pszString)+1);
		push_ulong(ulLen);
		push_data((const uchar_t*)pszString, ulLen);
	}
	else
	{
		push_ulong(0);
	}
}

void circular_buffer::push_ulonglong(ull_t ull)
{
	push_data((uchar_t*)&ull, sizeof(ull_t));
}

// pushes an unsigned long value
void circular_buffer::push_ulong(ulong_t ulData)
{
	push_data((uchar_t*)&ulData, sizeof(ulong_t));
}

void circular_buffer::push_ushort(ushort_t wData)
{
	push_data((uchar_t*)&wData, sizeof(ushort_t));
}

void circular_buffer::push_uchar(uchar_t byData)
{
	push_data(&byData, 1);
}

size_t circular_buffer::pop_data(byte_t* pbyBuffer, size_t tCount)
{
	if (m_pbyBuffer == NULL || m_tDataSize == 0)
		return 0;
	
	// how much data we are going to spare
	size_t tRealCount=tCount;
	if (m_tDataSize < tRealCount)
		tRealCount=m_tDataSize;
	
	// copy the data
	memcpy(pbyBuffer, m_pbyBuffer, tRealCount);
	
	// now move the data to beginning
	skip_bytes(tRealCount);
#if _USE_SHRINKING == 1
	shrink_buffer();
#endif
	return tRealCount;
}

bool circular_buffer::pop_ulonglong(ull_t* pull)
{
	return (pop_data((byte_t*)pull, sizeof(ull_t)) == sizeof(ull_t));
}

bool circular_buffer::pop_ulong(ulong_t* pul)
{
	return (pop_data((byte_t*)pul, sizeof(ulong_t)) == sizeof(ulong_t));
}

bool circular_buffer::pop_ushort(ushort_t* pw)
{
	return (pop_data((byte_t*)pw, sizeof(ushort_t)) == sizeof(ushort_t));
}
	
bool circular_buffer::pop_uchar(uchar_t* pby)
{
	return (pop_data((byte_t*)pby, 1) == 1);
}

ulong_t circular_buffer::pop_string(char_t** pszString)
{
	ulong_t ul;
	if (!pop_ulong(&ul))
	{
		*pszString=NULL;
		return (ulong_t)-1;
	}
	
	if (ul == 0)
	{
		*pszString = NULL;
		return 0;
	}
	else
	{
		// check if there is enough data
		if (m_tDataSize < ul)
			return (ulong_t)-1;
		
		// alloc buffer for a string
		(*pszString)=new char_t[ul];
		if (pop_data((byte_t*)(*pszString), ul) != ul)
		{
			delete [] (*pszString);
			*pszString=NULL;
			return (ulong_t)-1;
		}
		else
		{
			(*pszString)[ul-1]='\0';		// just in case
			return ul-1;					// without the '\0'
		}
	}
}

void circular_buffer::free_string(char_t* pszString)
{
	delete [] pszString;
}

size_t circular_buffer::find(size_t tStartAt, ulong_t ulFnd) const
{
	assert(m_pbyBuffer);
//	printf("searching for %lu from %lu\n", ulFnd, ulStartAt);
//	printf("internal structures: buf: 0x%lx, data size: %lu, buf size: %lu\n", m_pbyBuffer, m_tDataSize, m_tSize);
	for (size_t i=tStartAt;i<m_tDataSize-3;i++)
	{
		if (((ulong_t*)(m_pbyBuffer+i))[0] == ulFnd)
		{
//			printf("found at %lu\n", i);
			return i;
		}
	}
	
	return (size_t)-1;	
}

// returns 0 if there is no value (but a part of it may exist), 1 if found, -1 if not found
int circular_buffer::forward_seek(ulong_t ulFnd)
{
	assert(m_pbyBuffer);
	if(!m_pbyBuffer)
		THROW(_t("Invalid member"), GE_INVALIDARG, 0, 0);

	if (m_tDataSize < sizeof(ulong_t))
		return FS_PARTIAL;		// cannot tell if there is such a value (may be a part of it)
	
	for (size_t i=0;i<m_tDataSize-3;i++)
	{
		if (((ulong_t*)(m_pbyBuffer+i))[0] == ulFnd)
		{
			// set this value at the beginning
			skip_bytes(i);
#if _USE_SHRINKING == 1
			shrink_buffer();
#endif
			return FS_FOUND;
		}
	}

#if _FAILSEEK_TRUNCATES == 1
	// throw off almost the entire buffer (leave only the 3 bytes because they may contain 3/4th of ulFnd)
	flush(sizeof(ulong_t)-1);
#if _USE_SHRINKING == 1
	shrink_buffer();
#endif
#endif
	return FS_NOTFOUND;
}

void circular_buffer::skip_bytes(size_t tCount)
{
	assert(m_pbyBuffer);
	if (tCount > m_tDataSize)
		m_tDataSize=0;
	else
	{
		memmove(m_pbyBuffer, m_pbyBuffer+tCount, m_tDataSize-tCount);
		m_tDataSize-=tCount;
	}
}

void circular_buffer::resize_buffer(size_t tNewSize)
{
	// modify the new length & alloc the new buffer
	tNewSize=(tNewSize & ~(_BUFFER_INC-1)) + _BUFFER_INC;
	if (tNewSize < m_tSize)
		return;
	
	byte_t *pszBuf=new byte_t[tNewSize];
	
	if (m_pbyBuffer && m_tDataSize > 0)
	{
		// copy the old buffer to the new one
		memcpy(pszBuf, m_pbyBuffer, m_tDataSize);
	}
	
	// destroy the old buffer
	delete [] m_pbyBuffer;

	// update data
	m_pbyBuffer=pszBuf;
	m_tSize=tNewSize;
}

void circular_buffer::shrink_buffer()
{
#if _USE_SHRINKING == 1
	assert(m_pbyBuffer);

	// check the current size of the data
	size_t tNewSize=(m_tDataSize & ~(_BUFFER_INC-1)) + _BUFFER_INC;
	if (m_tSize-tNewSize > _BUFFER_DEC)
	{
		// we must shrink the buffer
		byte_t *pszBuf=new byte_t[tNewSize];
		memcpy(pszBuf, m_pbyBuffer, m_tDataSize);
		delete [] m_pbyBuffer;
		
		m_pbyBuffer=pszBuf;
		m_tSize=tNewSize;
	}
#endif
}

void circular_buffer::flush(size_t tToLeave)
{
	if (m_tDataSize > tToLeave)
		skip_bytes(m_tDataSize-tToLeave);
}

void circular_buffer::clear()
{
	m_tDataSize=0;
}

size_t circular_buffer::get_datasize() const
{
	return m_tDataSize;
}

bool circular_buffer::is_empty() const
{
	return m_tDataSize == 0;
}

circular_buffer::operator const byte_t*() const
{
	return m_pbyBuffer;
}

const byte_t* circular_buffer::get_buffer() const
{
	return m_pbyBuffer;
}

void circular_buffer::push_bits(ulong_t ulBits, byte_t byCount)
{
//	assert(m_pbyBuffer);
	assert(byCount <= 32 && byCount >= 1);	// count of bits must be a sane value
	assert(m_tBitsAtEndCount <= 7);			// the internal bits count must be from the range [0..7]. For 8 bits in a buffer 
											// there is value of 0.
	
	do
	{
		// check if we have to add the bits to the last byte of a buffer
		if (m_tBitsAtEndCount != 0)
		{
			// count of bits to copy into the last byte of the internal buffer
			ulong_t ulCopy=(ulong_t)((byCount < 8-m_tBitsAtEndCount) ? byCount : 8-m_tBitsAtEndCount);
			
			// make some space for the incoming data
			m_pbyBuffer[m_tDataSize-1] >>= ulCopy;
			
			// get the full byte from the in
			byte_t uc=(byte_t)(ulBits & 0x000000ff);
			
			// we are getting from it only ulCopy lowest bits, so shift if a bit
			uc <<= (8-ulCopy);
			
			// and apply
			m_pbyBuffer[m_tDataSize-1] |= uc;
			
			// a bit of corrections
			ulBits >>= ulCopy;
			byCount-=(byte_t)ulCopy;
			m_tBitsAtEndCount+=ulCopy;
			if (m_tBitsAtEndCount == 8)
				m_tBitsAtEndCount = 0;
		}
		else
		{
			// now there is something to add at the beginning of a next byte
			// if there are some full bytes to add then simply add it through the
			// PushData.
			if (byCount >= 8)
			{
				// push the whole 8 bits as a byte into the buffer. Operation safe only
				// on the little endian machines.
				ulong_t ulCount=byCount/8;
				push_data(((const byte_t*)&ulBits), ulCount);
				
				// corrections
				ulBits >>= ulCount*8;
				byCount-=(byte_t)(ulCount*8);
			}
			else
			{
				// we are about to add <8 bits of data into the last byte of a buffer which does not exist yet
				// get the full byte from the input ulong
				byte_t uc=(byte_t)(ulBits & 0x000000ff);
			
				// shift it a bit
				uc <<= 8-byCount;
				
				// and add as a next byte
				push_data(&uc, 1);
				
//				Dump();
				
				// corrections
				m_tBitsAtEndCount = byCount;
				ulBits = 0;			// since there are no data left
				byCount = 0;	// no data left
			}
		}
	}
	while(byCount > 0);
}

// finished the operation of pushing bits, so we could use normal Push/PopData
/*void circular_buffer::PushBitsFinish()
{
	// check if there is unfinished byte at the end
	if (m_tBitsAtEndCount != 0)
	{
		m_pbyBuffer[m_tDataSize-1] >>= 8-m_tBitsAtEndCount;
		m_tBitsAtEndCount=0;
	}
}*/

// enumerates all the bit-packs that exists in a buffer. If there were any bits operations performed
// on a buffer - they must be finished by the PushBitsFinish.
void circular_buffer::enum_bit_packets(ulong_t ulBitsCount, PFNBITSCALLBACK pfn, void* pParam) const
{
	assert(m_pbyBuffer);
	assert(ulBitsCount >= 1 && ulBitsCount <=8);
	assert(pfn);
	if(!pfn || ! m_pbyBuffer || ulBitsCount < 1 || ulBitsCount > 8)
		THROW(_t("Invalid member or argument"), GE_INVALIDARG, 0, 0);

	ushort_t w=0;		// internal buffer for the next data from the class's buffer
	ulong_t ulBits=0;	// count of bits that was left in w
	
	size_t tIndex=0;	// current index in the class's buffer
	for (;;)
	{
		// make sure there is enough data in w so the next operation can succeed
		// if there is less data in w than requested
		if (ulBits < ulBitsCount)
		{
			// is there something left to read from the internal buffer
			if (tIndex < m_tDataSize)
			{
				// append some bits into the buffer.
				// NOTE: we are sure that there are at least 8 bits space in w
				if (tIndex == m_tDataSize && m_tBitsAtEndCount != 0)
				{
					// there are less than 8 bits left. add only the part that exists
					byte_t uc=(byte_t)(m_pbyBuffer[tIndex++] >> (8-m_tBitsAtEndCount));
					w |= (ushort_t)(uc) << ulBits;
				}
				else
				{
					w |= (ushort_t)(m_pbyBuffer[tIndex++]) << ulBits;
					ulBits+=8;
				}
			}
			else
			{
				// are there any bits left in the w ?
				if (ulBits > 0)
				{
					// there are some bits left, so we should add a bit or two to make sure nothing is lost
//					printf("$$$ Some (%lu) bits left. Current cache=%u\n", ulBits, w);
					ulBits=ulBitsCount;
				}
				else
				{
					// there are no data left in the internal buffer, so finish the operation
//					printf("&&& Leaving with %lu bits left\n", ulBits);
					return;
				}
			}
		}
		else
		{
			// call the callback function with the ucData as a param
			byte_t uc=(byte_t)((w & 0xff) << (8-ulBitsCount));
			uc >>= 8-ulBitsCount;
			
			(*pfn)(uc, pParam);
			
			// update variables
			ulBits-=ulBitsCount;
			w >>= ulBitsCount;
		}
	}
}

/*void circular_buffer::dump()
{
	printf("circular_buffer::Dump()\n\tsize of data: %lu\n\tsizeof the buffer: %lu\n\tbits at end: %lu", m_tDataSize, m_tSize, m_tBitsAtEndCount);
	for (unsigned long i=0;i<m_tDataSize;i++)
	{
		if ( (i % 16) == 0 )
			printf("\n\t");
		
		printf("0x%02lx,", (unsigned long)m_pbyBuffer[i]);
	}
	printf("\n");
}
*/

END_ICPF_NAMESPACE
