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
/** \file file.cpp
 *  \brief Contains file/serializer class */

#include "file.h"
#include "crc32.h"
#include <string.h>
#include "macros.h"
#include "err_codes.h"

#ifndef _WIN32
	#include <fcntl.h>
	#include <errno.h>
	#include <unistd.h>
	#include <sys/stat.h>
#endif
	
BEGIN_ICPF_NAMESPACE

#include <assert.h>

/// Serialization buffer increment value
#define SERIALBUFFER_DELTA	4096UL

// for making life easier
#ifdef _WIN32
	/// Macro representing the current value of last encountered error
	#define CURRENT_LAST_ERROR	GetLastError()
	/// Null value for a file handle - platform compatibility helper
	#define FNULL	NULL
#else
	/// Windoze compatibility macro - value returned from erroneous call to ::open() system call
	#define INVALID_HANDLE_VALUE -1
	/// Macro representing the current value of last encountered error
	#define CURRENT_LAST_ERROR errno
	/// Null value for a file handle - platform compatibility helper
	#define FNULL	0
#endif


/** Standard constructor - nullifies all member variables.
 */
file::file() :
	m_hFile(FNULL),
	m_pszPath(NULL),
	m_uiFlags(0),
	m_bLastOperation(false),
	m_bBuffered(false),
    m_uiBufferSize(0),
	m_pbyBuffer(NULL),
	m_uiCurrentPos(0),
    m_uiDataCount(0),
	m_bRememberedState(false),
	m_bSerializing(0),
    m_pbySerialBuffer(NULL),
	m_uiSerialBufferSize(0),
	m_uiSerialBufferPos(0),
	m_uiDataBlockFlags(BF_NONE)
{
}

/** Standard destructor - tries to close a file, but catches any exception thrown
 *  and does not throw it again.
 */
file::~file()
{
	// close all the stuff, but make sure no exception is thrown
	try
	{
		close();
	}
	catch(exception&)
	{
	}
}

/** Opens a filesystem object (a file) with a given flags and (if needed) with
 *  some buffer size used in internal buffering. In case of error throws an icpf::exception
 *  with the error description.
 * \param[in] pszPath - path to a file to open
 * \param[in] uiFlags - flags that determine the type of access to a file (FA_*)
 * \param[in] uiBufSize - buffer size that will be used for internal buffering (if enabled)
 */
void file::open(const tchar_t* pszPath, uint_t uiFlags, uint_t uiBufSize)
{
	// check if this object is ready to open a file
	if (m_hFile)
		close();

	// check for the flags completion
#ifdef _WIN32
	uint_t mode=0, flags=OPEN_EXISTING;
#else
	int_t mode=0;
#endif

	// read flag
	if (uiFlags & FA_READ)
#ifdef _WIN32
		mode |= GENERIC_READ;
#else
		mode |= O_RDONLY;
#endif

	// write flag
	if (uiFlags & FA_WRITE)
#ifdef _WIN32
		mode |= GENERIC_WRITE;
#else
		mode |= O_WRONLY;
#endif

	// creation flag
	if (uiFlags & FA_CREATE)
#ifdef _WIN32
	{
		if (uiFlags & FA_TRUNCATE)
			flags = CREATE_ALWAYS;
		else
			flags = OPEN_ALWAYS;
	}
#else
		mode |= O_CREAT;
#endif

	// truncation
	if (uiFlags & FA_TRUNCATE)
#ifdef _WIN32
	{
		if (!(uiFlags & FA_CREATE))
			flags = TRUNCATE_EXISTING;
	}
#else
		mode |= O_TRUNC;
#endif

	// make a system call
#ifdef _WIN32
	m_hFile=::CreateFile(pszPath, mode, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, flags, FILE_ATTRIBUTE_NORMAL, NULL);
#else
	m_hFile=::open(pszPath, mode);
#endif
	
	if (m_hFile == INVALID_HANDLE_VALUE)
	{
		m_hFile=FNULL;
		THROW(exception::format(_t("[file] Cannot open the file ") STRFMT _t(" with flags ") ULXFMT, pszPath, uiFlags), FERR_OPEN, CURRENT_LAST_ERROR, 0);
	}
	else
	{
		// remember the path of this file
		m_pszPath=new tchar_t[_tcslen(pszPath)+1];
		_tcscpy(m_pszPath, pszPath);

		// remember the mode
		m_uiFlags=uiFlags;

		// is this buffered ?
        set_buffering((uiFlags & FA_BUFFERED) != 0, uiBufSize);
	}
}

/** Tries to save the data contained in internal buffer (if any) and frees all the
 *  resources used by this class. Can be used on closed files. In case of error the
 *  ch::exception is thrown.
 */
void file::close()
{
	// only if the file has been opened
	if (m_hFile != FNULL)
	{
		// flush all data
		flush();
		
		// free strings and other data (at first, because of the destruction call)
		delete [] m_pszPath;
		m_pszPath=NULL;
		delete [] m_pbyBuffer;
		m_pbyBuffer=NULL;

		delete [] m_pbySerialBuffer;
		m_pbySerialBuffer=NULL;

		m_uiFlags=0;
		m_bLastOperation=false;

		m_bBuffered=false;
		m_uiBufferSize=0;
		m_uiCurrentPos=0;
		m_uiDataCount=0;

		m_bSerializing=false;
		m_uiSerialBufferSize=0;
		m_uiSerialBufferPos=0;
		m_uiDataBlockFlags=0;
	
		m_bRememberedState=false;
		
		// close the file
#ifdef _WIN32
		if (!::CloseHandle(m_hFile))
#else
		if (::close(m_hFile) == -1)
#endif
			THROW(exception::format(_t("[file] Cannot close the handle associated with a file ") STRFMT, m_pszPath), FERR_CLOSE, CURRENT_LAST_ERROR, 0);
		else
			m_hFile=FNULL;
	}
}

/** Does anything only if the file is opened, the operation is being buffered and the data
 *  count contained in the internal buffer is >0. If the last operation performed was
 *  storing data then the function tries to store a packet of data to the file. If the data
 *  has been read lately then only the file pointer will be repositioned. Also the internal buffer
 *  will be made empty. Function can throw an ch::exception in case of error.
 */
void file::flush()
{
	if (m_hFile && m_bBuffered && m_uiDataCount > 0)
	{
		if (m_bLastOperation)
		{
			// last operation - storing data
			_write_packet();
		}
		else
		{
			// last - reading data
			// set file pointer to position current-(m_uiDataCount-m_uiCurrentPos)
			_seek(-(longlong_t)(m_uiDataCount-m_uiCurrentPos), FS_CURRENT);
			m_uiCurrentPos=0;
			m_uiDataCount=0;
		}
	}
}

/** Tries to read a specified count of bytes from a file to the specified buffer.
 *  If the is currently in buffered state the reading is being done with internal
 *  buffering. It could be helpful when reading small amounts of data.
 * \param[out] pBuffer - ptr to a buffer that is about to receive data
 * \param[in] iSize - count of bytes to read
 * \return Count of bytes that has been read (could be less than iSize)
 */
ulong_t file::read(ptr_t pBuffer, ulong_t ulSize)
{
	assert(m_hFile);		// forgot to open the file ?

	// flush if needed
	if (m_bLastOperation)
	{
		flush();
		m_bLastOperation=false;
	}

	if (!m_bBuffered)
	{
		// unbuffered operation (read what is needed)
#ifdef _WIN32
		DWORD rd=0;
		if (!ReadFile(m_hFile, pBuffer, ulSize, &rd, NULL))
#else
		int_t rd=0;
		if ((rd=::read(m_hFile, pBuffer, ulSize)) < 0)
#endif
			THROW(exception::format(_t("Cannot read data from file ") STRFMT, m_pszPath), FERR_READ, CURRENT_LAST_ERROR, 0);

		return rd;		// if 0 - eof (not treated as exception)
	}
	else
	{
		// reads must be done by packets
		uint_t uiCurrPos=0;			// position in external buffer
        while (uiCurrPos < ulSize)
		{
			// are there any data left ?
			if (m_uiDataCount == 0 || m_uiCurrentPos == m_uiDataCount)
			{
				if (_read_packet() == 0)
					return uiCurrPos;				// return what was read 'til now
			}

			// copy data into external buffer
			uint_t uiCount=minval(m_uiDataCount-m_uiCurrentPos, ulSize-uiCurrPos);
			memcpy(((byte_t*)pBuffer)+uiCurrPos, m_pbyBuffer+m_uiCurrentPos, uiCount);

			// update positions
            uiCurrPos+=uiCount;
			m_uiCurrentPos+=uiCount;
		}

		return uiCurrPos;
	}
}

/** Tries to write a specified amount of data from a buffer to a file. If the buffered
 *  operations are enabled then this operation could store data in the internal buffer
 *  instead of a file.
 * \param[in] pBuffer - ptr to a buffer with data to store
 * \param[in] iSize - count of data to store
 * \return Count of data that has been stored
 */
ulong_t file::write(ptr_t pBuffer, ulong_t ulSize)
{
	assert(m_hFile);

	if (!m_bLastOperation)
	{
		flush();
		m_bLastOperation=true;
	}

	if (!m_bBuffered)
	{
		// standard write
#ifdef _WIN32
		DWORD wr=0;
		if (!WriteFile(m_hFile, pBuffer, ulSize, &wr, NULL))
#else
		int_t wr;
		if ((wr=::write(m_hFile, pBuffer, ulSize) == -1))
#endif
			THROW(exception::format(_t("[file] Cannot write data to a file"), m_pszPath), FERR_WRITE, CURRENT_LAST_ERROR, 0);

		return (ulong_t)wr;
	}
	else
	{
		uint_t uiPos=0;

		while (uiPos < ulSize)
		{
			// check if buffer need storing
			if (m_uiCurrentPos == m_uiBufferSize)
				_write_packet();

			// now add to internal buffer some data
			uint_t uiCount=minval(m_uiBufferSize-m_uiCurrentPos, ulSize-uiPos);

			memcpy(m_pbyBuffer+m_uiCurrentPos, ((byte_t*)pBuffer)+uiPos, uiCount);

			// update
			m_uiCurrentPos+=uiCount;
			m_uiDataCount+=uiCount;
			uiPos+=uiCount;
		}

		return uiPos;
	}
}

/** Tries to read a string from a file. Uses a buffering to achieve it. If the
 *  current file mode is unbuffered then this function enables buffering, makes
 *  requested operation and disables buffering. If the buffer is too small to
 *  contain all the line read then the rest of line is lost.
 * \param[out] pszStr - ptr to a buffer to receive a string
 * \param[in] uiMaxLen - size of the string buffer
 * \return If the line has been succesfully read.
 */
bool file::read_line(tchar_t* pszStr, uint_t uiMaxLen)
{
	// for unbuffered operations enable buffering for this op
	if (m_bBuffered)
		return _read_string(pszStr, uiMaxLen);
	else
	{
		uint_t uiSize=m_uiBufferSize;
		set_buffering(true, 4096);

		bool bRet=_read_string(pszStr, uiMaxLen);

		set_buffering(false, uiSize);
		return bRet;
	}
}

/** Tries to write some text into a file. Function allocates a buffer for the string
 *  copy and appends a \\r\\n to this text.
 * \param[in] pszString - string to store
 */
void file::write_line(tchar_t* pszString)
{
	assert(m_hFile);

	if (!m_bLastOperation)
	{
		flush();
		m_bLastOperation=true;
	}

	// make new string with \r\n at the end - cannot use old buffer - unknown size
	// NOTE: \r\n added for windoze compat; when reading file function properly handles unix style line endings
	uint_t uiLen=(uint_t)_tcslen(pszString);

	tchar_t *pszData=new tchar_t[uiLen+3];
	_tcscpy(pszData, pszString);
	pszData[uiLen]=_t('\r');
	pszData[uiLen+1]=_t('\n');
	pszData[uiLen+2]=_t('\0');

	try
	{
		if (m_bBuffered)
		{
			uint_t uiStrPos=0;	// current index in pszString
			uint_t uiMin=0;		// helper
			uint_t uiSize=uiLen+2;

			// processing whole string
			while (uiStrPos < uiSize)
			{
				if (m_uiCurrentPos == m_uiBufferSize)
					_write_packet();

				// count of chars to be copied
				uiMin=minval(uiSize-uiStrPos, m_uiBufferSize-m_uiCurrentPos);

				// copy data from pszString into internal buffer (maybe part of it)
				memcpy(m_pbyBuffer+m_uiCurrentPos, pszData+uiStrPos, uiMin);

				// move offsets
				m_uiCurrentPos+=uiMin;
				m_uiDataCount+=uiMin;
				uiStrPos+=uiMin;
			}
		}
		else
		{
			// standard write
#ifdef _WIN32
			DWORD wr=0;
			if (!WriteFile(m_hFile, pszData, uiLen+2, &wr, NULL))
#else
			int_t wr;
			if ((wr=::write(m_hFile, pszData, uiLen+2)) == -1)
#endif
				THROW(exception::format(_t("Cannot write data to a file ") STRFMT, m_pszPath), FERR_WRITE, CURRENT_LAST_ERROR, 0);
		}
	}
	catch(...)
	{
		delete [] pszData;
		throw;
	}

	delete [] pszData;
}

/** Moves the file pointer to some place in the file. If the file is in buffered
 *  state, then the data is flushed and then the real seek takes place.
 * \param[in] llOffset - offset of the new file pointer position
 * \param[in] uiFrom - specifies the relative point (FS_*) from which the llOffset will be counted
 */
void file::seek(longlong_t llOffset, uint_t uiFrom)
{
	// flush buffer
	flush();

	// seek
	_seek(llOffset, uiFrom);
}

/** Retrieves the current file pointer position (corrected by the content of
 *  internal buffer if buffered mode enabled).
 * \return Current file position.
 */
longlong_t file::getpos()
{
	// return corrected by internal members current file position
	return _seek(0, FS_CURRENT)-m_uiDataCount+m_uiCurrentPos;
}

/** Tries to set end-of-file marker on the current file pointer position. The internal buffer
 *  is flushed before truncating (or extending).
 */
void file::seteof()
{
	assert(m_hFile);

	// flush da buffers
	flush();

	// now set the end of file
#ifdef _WIN32
	if (!::SetEndOfFile(m_hFile))
#else
	if (::ftruncate(m_hFile, getpos()) == -1)
#endif
		THROW(exception::format(_t("[file] Cannot truncate the file ") STRFMT, m_pszPath), FERR_SETEOF, CURRENT_LAST_ERROR, 0);
}

/** Returns a size of the file.
 * \return Size of the file.
 */
longlong_t file::get_size()
{
	flush();

#ifdef _WIN32
	ULARGE_INTEGER li;
	li.LowPart=GetFileSize(m_hFile, &li.HighPart);
	if (li.LowPart == INVALID_FILE_SIZE && GetLastError() != NO_ERROR)
#else
	// NOTE: a strange way to check the file size...
	struct stat st;
	if (fstat(m_hFile, &st) == -1)
#endif
		THROW(exception::format(_t("[file] Cannot get the size of the file ") STRFMT, m_pszPath), FERR_GETSIZE, CURRENT_LAST_ERROR, 0);

#ifdef _WIN32
	return li.QuadPart;
#else
	return st.st_size;
#endif
}

/** Changes the buffering state (or internal buffer length). The internal buffer
 *  is always flushed before making any changes.
 * \param[in] bEnable - specifies if the buffering is about to be enabled
 * \param[in] uiSize - the new size of the internal buffer
 */
void file::set_buffering(bool bEnable, uint_t uiSize)
{
	assert(uiSize > 0);	// couldn't use 0-sized internal buffer

	// flush
	flush();

	// delete old buffer
	if (m_bBuffered && (!bEnable || uiSize != m_uiBufferSize))
	{
		delete [] m_pbyBuffer;
		m_pbyBuffer=NULL;
	}

	// alloc new buffer if needed
	if (bEnable && (!m_bBuffered || uiSize != m_uiBufferSize))
		m_pbyBuffer=new byte_t[uiSize];

	m_bBuffered=bEnable;
	m_uiBufferSize=uiSize;
}

/** Switches access mode to unbuffered one and remembers the last state. If already
 *  in unbuffered mode the function does nothing.
 */
void file::switch_unbuffered()
{
	if (!m_bBuffered)
		return;			// it's already unbuffered - leave it as is
	else
	{
		m_bRememberedState=true;				// mark that we change state to a different one
		set_buffering(false, m_uiBufferSize);	// do no change internal buffer size
	}
}

/** Switches access mode to buffered one and remembers the last state. If already
 *  in buffered mode the function does nothing.
 */
void file::switch_buffered()
{
	if (m_bBuffered)
		return;			// already buffered
	else
	{
		m_bRememberedState=true;
		set_buffering(true, m_uiBufferSize);
	}
}

/** Restores the buffered/unbuffered access mode if stored with switch_* functions.
 */
void file::restore_state()
{
	// restore state only if changed
	if (m_bRememberedState)
	{
		set_buffering(!m_bBuffered, m_uiBufferSize);
		m_bRememberedState=false;
	}
}

/** Begins the serialization data block. Each block can have it's
 *  own flags. Each block have to be ended with datablock_end before
 *  beginning another one. If the access is read then function tries to read
 *  the data block from a file and checks the crc checksums. If writing - only
 *  the buffer is allocated and initialized.
 *  Blocks cannot be nested.
 *  \param[in] uiFlags - block flags - look at BF_* macros
 */
void file::datablock_begin(uint_t uiFlags)
{
	// do not call begin data block within another data block
	assert(!m_bSerializing && m_pbySerialBuffer == NULL);

	// alloc the new buffer and insert there a header (a few unsigned chars)
	m_pbySerialBuffer=new byte_t[SERIALBUFFER_DELTA];
	m_uiSerialBufferSize=SERIALBUFFER_DELTA;

	// check flags
	if ((m_uiFlags & FA_READ) && (m_uiFlags & FA_WRITE))
		THROW(exception::format(_t("[file] Tried to begin a data block with file ") STRFMT _t(" opened for both read and write."), m_pszPath), FERR_SERIALIZE, 0, 0);

	// action
	if (m_uiFlags & FA_WRITE)
	{
		// reserve some space for a header
		m_uiSerialBufferPos=sizeof(SERIALIZEINFOHEADER);
	}
	else
	{
		// we need to read the block from a file
		if (read(m_pbySerialBuffer, sizeof(SERIALIZEINFOHEADER)) != sizeof(SERIALIZEINFOHEADER))
		{
			_clear_serialization();
			THROW(exception::format(_t("[file] Cannot read the specified amount of data from a file (reading serialization header)."), m_pszPath), FERR_SERIALIZE, CURRENT_LAST_ERROR, 0);
		}

		// move forward
		m_uiSerialBufferPos=sizeof(SERIALIZEINFOHEADER);

		// determine the size of the remaining data in file
		SERIALIZEINFOHEADER* psih=(SERIALIZEINFOHEADER*)m_pbySerialBuffer;
		uint_t uiSize=(uint_t)(psih->iRealSize-sizeof(SERIALIZEINFOHEADER));

		// check the header crc
		uint_t uihc=crc32(m_pbySerialBuffer, sizeof(SERIALIZEINFOHEADER)-sizeof(uint_t));
		if (uihc != psih->uiHeaderCRC32)
		{
			_clear_serialization();
			THROW(exception::format(_t("[file] Block contained in file ") STRFMT _t(" is corrupted. Header CRC check failed."), m_pszPath), FERR_SERIALIZE, 0, 0);
		}

		// resize the buffer
		_sbuf_resize((uint_t)psih->iRealSize);

		// refresh the psih
		psih=(SERIALIZEINFOHEADER*)m_pbySerialBuffer;

		// read the remaining data
		if (read(m_pbySerialBuffer+m_uiSerialBufferPos, uiSize) != (int_t)uiSize)
		{
			_clear_serialization();
			THROW(exception::format(_t("Cannot read specified amount of data from a file ") STRFMT _t(" (reading the after-header data)."), m_pszPath), FERR_SERIALIZE, CURRENT_LAST_ERROR, 0);
		}

		// NOTE: do not update the position - we need ptr at the beginning of data
		// now we are almost ready to retrieve data - only the crc check for the data
		uint_t uiCRC=crc32(m_pbySerialBuffer+sizeof(SERIALIZEINFOHEADER), psih->iDataSize-sizeof(SERIALIZEINFOHEADER));
		if (psih->uiCRC32 != uiCRC)
		{
			_clear_serialization();
			THROW(exception::format(_t("CRC check of the data read from file ") STRFMT _t(" failed."), m_pszPath), FERR_SERIALIZE, 0, 0);
		}
	}

	// make a mark
	m_uiDataBlockFlags=uiFlags;
	m_bSerializing=true;
}

/** Ends the data block opened previously with datablock_begin. If the access is writing
 *  then the function updates the crc checksums in the buffer and tries to write the block
 *  of data to the file. If reading - only the serialization is cancelled.
 */
void file::datablock_end()
{
	// make sure everything is ok
	assert(m_bSerializing && m_pbySerialBuffer != NULL);
	if(!m_pbySerialBuffer)
		THROW(_t("Invalid argument"), GE_INVALIDARG, 0, 0);

	// check the operation type
	if ((m_uiFlags & FA_READ) && (m_uiFlags & FA_WRITE))
		THROW(exception::format(_t("[file] Tried to end a data block with file ") STRFMT _t(" opened for both read and write."), m_pszPath), FERR_SERIALIZE, 0, 0);

	// when writing - make a header, ...; when reading - do nothing important
	if (m_uiFlags & FA_WRITE)
	{
		// check if there is any data
		if (m_uiSerialBufferPos == sizeof(SERIALIZEINFOHEADER))
			return;												// no data has been serialized

		// fill the header (real data information)
		SERIALIZEINFOHEADER *psih=(SERIALIZEINFOHEADER*)m_pbySerialBuffer;
		psih->iDataSize=m_uiSerialBufferPos;
		psih->uiCRC32=crc32(m_pbySerialBuffer+sizeof(SERIALIZEINFOHEADER), m_uiSerialBufferPos-sizeof(SERIALIZEINFOHEADER));

		// the rest of header
		psih->iRealSize=m_uiSerialBufferPos;

		// calc the header crc
		psih->uiHeaderCRC32=crc32(m_pbySerialBuffer, sizeof(SERIALIZEINFOHEADER)-sizeof(uint_t));

		// write the buffer
		write(m_pbySerialBuffer, psih->iRealSize);
	}

	// remove all the traces of serializing
	_clear_serialization();
}

/** Writes some bytes of data to a serialization buffer opened
 *  with the datablock_begin. Causes the class to reallocate an internal
 *  buffer(if needed) to make all the data fit into the buffer.
 *  NOTE: do not use too large data blocks because of the buffer reallocation.
 *  \param[in] pData - buffer address with some data
 *  \param[in] uiSize - count of bytes to write
 */
void file::swrite(ptr_t pData, uint_t uiSize)
{
	_sbuf_append(pData, uiSize);
}

/** Reads some bytes of data from a serialization buffer opened by
 *  datablock_end call.
 *  \param[out] pData - buffer for the received data
 *  \param[in] uiSize - count of data to read; if user requests more data than available then an exception will be thrown
 */
void file::sread(ptr_t pData, uint_t uiSize)
{
	_sbuf_read(pData, uiSize);
}

/** Cancels the serialization process and removes all the traces of data being serialized
 *  (it includes deleting the serialization buffer).
 */
void file::_clear_serialization()
{
	// remove all the traces of serializing
    delete [] m_pbySerialBuffer;
	m_pbySerialBuffer=NULL;
	m_uiSerialBufferSize=0;
	m_uiSerialBufferPos=0;
	m_bSerializing=false;
}

/** Used to read the next packet of data into the internal buffer (used for
 *  buffering).
 *  \return Count of bytes that has been read from a file.
 */
uint_t file::_read_packet()
{
	assert(m_hFile);
	if(!m_hFile || !m_pbyBuffer)
		THROW(_t("Invalid argument"), GE_INVALIDARG, 0, 0);

	// read data
#ifdef _WIN32
	DWORD rd=0;
	if (!ReadFile(m_hFile, m_pbyBuffer, m_uiBufferSize, &rd, NULL))
#else
	int_t rd;
	if ((rd=::read(m_hFile, m_pbyBuffer, m_uiBufferSize)) == -1)
#endif
		THROW(exception::format(_t("[file] Cannot read data from a file ") STRFMT _t("."), m_pszPath), FERR_READ, CURRENT_LAST_ERROR, 0);

	// reset internal members
	m_uiDataCount=rd;
	m_uiCurrentPos=0;

	return rd;
}

/** Used to write the next packet of data from an internal buffer (buffering
 *  related) to a file.
 *  \return Count of bytes written.
 */
uint_t file::_write_packet()
{
	assert(m_hFile);
	if(!m_hFile || !m_pbyBuffer)
		THROW(_t("Invalid argument"), GE_INVALIDARG, 0, 0);

#ifdef _WIN32
	DWORD wr=0;
	if (!WriteFile(m_hFile, m_pbyBuffer, m_uiDataCount, &wr, NULL))
#else
	int_t wr;
	if ((wr=::write(m_hFile, m_pbyBuffer, m_uiDataCount)) == -1)
#endif
	{
		THROW(exception::format(_t("Cannot write data to a file ") STRFMT _t("."), m_pszPath), FERR_WRITE, CURRENT_LAST_ERROR, 0);
	}

	// reset internal members
	m_uiDataCount=0;
	m_uiCurrentPos=0;

	return wr;
}

/** Reads some data from the internal serialization buffer.
 * \param[out] pData - ptr to a buffer that is about to receive data
 * \param[in] uiLen - count of data to be read
 */
void file::_sbuf_read(ptr_t pData, uint_t uiLen)
{
	// check if we are reading
	assert(m_uiFlags &  FA_READ);

	// check the ranges
	if (m_uiSerialBufferPos+uiLen > m_uiSerialBufferSize)
	{
		// throw an exception - read beyond the data range in a given object
		THROW(exception::format(_t("[file] Trying to read the serialization data beyond the range (file ") TSTRFMT _t(")."), m_pszPath), FERR_MEMORY, CURRENT_LAST_ERROR, 0);
	}

	// read the data
	memcpy(pData, m_pbySerialBuffer+m_uiSerialBufferPos, uiLen);
	m_uiSerialBufferPos+=uiLen;
}

/** Appends some bytes to the internal serialization buffer (data block).
 *  \param[in] pData - ptr to a data buffer
 *  \param[in] uiCount - count of data to store
 */
void file::_sbuf_append(ptr_t pData, uint_t uiCount)
{
	// check if we are writing
	assert(m_uiFlags & FA_WRITE);

	// check serial buffer size (if there is enough room for the data)
	if (m_uiSerialBufferPos+uiCount > m_uiSerialBufferSize)
	{
		// we need a buffer reallocation
		uint_t uiDelta=((uiCount/SERIALBUFFER_DELTA)+1)*SERIALBUFFER_DELTA;

		// alloc the new buffer
		_sbuf_resize(m_uiSerialBufferSize+uiDelta);
	}

	assert(m_pbySerialBuffer);
	if(!m_pbySerialBuffer)
		THROW(_t("Invalid buffer."), 0, 0, 0);
	// real storage of the data
	if (uiCount > 0)
	{
		memcpy(m_pbySerialBuffer+m_uiSerialBufferPos, pData, uiCount);
		m_uiSerialBufferPos+=uiCount;
	}
}

/** Resizes the internal serialization buffer to the new length.
 *  Also does copy the old data to the new buffer.
 *  \param[in] uiNewLen - new buffer length
 */
void file::_sbuf_resize(uint_t uiNewLen)
{
	// alloc the new buffer
	byte_t* pbyNewBuffer=new byte_t[uiNewLen];

	// copy the old data into the new one
	uint_t uiCount=minval(m_uiSerialBufferPos, uiNewLen);
	if (m_uiSerialBufferPos > 0)
		memcpy(pbyNewBuffer, m_pbySerialBuffer, uiCount);

	// delete the old buffer
	delete [] m_pbySerialBuffer;

	// set the new buffer inplace and update the internal size
	m_pbySerialBuffer=pbyNewBuffer;
	m_uiSerialBufferSize=uiNewLen;
}


/** Reads a line of text from text file - only for buffered operations.
 *  Properly interpretes the windoze and unix line endings.
 * \param[out] pszStr - buffer for the string that is about to be read
 * \param[in] uiMaxLen - size of the string buffer
 * \return Bool value that states if the string has been read.
 */
bool file::_read_string(tchar_t* pszStr, uint_t uiMaxLen)
{
	assert(m_hFile);	// file wasn_t('t opened - error opening or you')ve forgotten to do so ?
	assert(m_pbyBuffer != NULL);

	// last time was writing - free buffer
	if (m_bLastOperation)
	{
		flush();
		m_bLastOperation=false;
	}

	// zero all the string
	memset(pszStr, 0, uiMaxLen*sizeof(tchar_t));

	// additional vars
	uint_t uiStrPos=0;	// current pos in external buffer
	bool bSecondPass=false;		// if there is need to check data for 0x0a tchar_t

	// copy each tchar_t into pszString
	for (;;)
	{
		// if buffer is empty - fill it
		if (m_uiDataCount == 0 || m_uiCurrentPos == m_uiDataCount)
		{
			if (_read_packet() == 0)
				return _tcslen(pszStr) != 0;
		}

		// skipping 0x0a in second pass
		if (bSecondPass)
		{
			if (m_pbyBuffer[m_uiCurrentPos] == 0x0a)
				m_uiCurrentPos++;
			return true;
		}

		// now process chars
		while (m_uiCurrentPos < m_uiDataCount)
		{
			if (m_pbyBuffer[m_uiCurrentPos] == 0x0d)
			{
				bSecondPass=true;
				m_uiCurrentPos++;
				break;
			}
			else if (m_pbyBuffer[m_uiCurrentPos] == 0x0a)
			{
				m_uiCurrentPos++;
				return true;
			}
			else
			{
				if (uiStrPos < uiMaxLen-1)
					pszStr[uiStrPos++]=m_pbyBuffer[m_uiCurrentPos];
				m_uiCurrentPos++;
			}
		}
	}
}

/** Sets file pointer in a file.
 * \param[in] llOffset - position to move the file pointer to
 * \param[in] uiFrom - the relative position of a base position (FS_*)
 * \return The new file pointer position.
 */
longlong_t file::_seek(longlong_t llOffset, uint_t uiFrom)
{
#ifdef _WIN32
	LARGE_INTEGER li;
	li.QuadPart = llOffset;
	li.LowPart = SetFilePointer (m_hFile, li.LowPart, &li.HighPart, uiFrom);

	if (li.LowPart == INVALID_SET_FILE_POINTER && GetLastError() != NO_ERROR)
#else
	int_t lRes;
	if ((lRes=lseek(m_hFile, llOffset, uiFrom)) == -1)
#endif
		THROW(exception::format(_t("Seek error in file ") STRFMT _t("."), m_pszPath), FERR_SEEK, CURRENT_LAST_ERROR, 0);

#ifdef _WIN32
	return li.QuadPart;
#else
	return lRes;
#endif
}

END_ICPF_NAMESPACE
