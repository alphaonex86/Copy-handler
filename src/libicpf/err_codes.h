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
#ifndef __ERR_CODES_H__
#define __ERR_CODES_H__

////////////////////////////////////////////////////////////////
// file related
/// Base define for file related errors
#define FE_BASE                                 0x00010000

/// Unknown error
#define FERR_UNKNOWN                    (FE_BASE+0x0000)
/// Error while opening file
#define FERR_OPEN                               (FE_BASE+0x0001)
/// Error while closing file
#define FERR_CLOSE                              (FE_BASE+0x0002)
/// Error reading data from file
#define FERR_READ                               (FE_BASE+0x0003)
/// Error writing data to a file
#define FERR_WRITE                              (FE_BASE+0x0004)
/// Error setting file pointer position
#define FERR_SEEK                               (FE_BASE+0x0005)
/// eof encountered - currently unused
#define FERR_EOF                                (FE_BASE+0x0006)
/// error while setting an eof in file
#define FERR_SETEOF                             (FE_BASE+0x0007)
/// error getting file size
#define FERR_GETSIZE                    (FE_BASE+0x0008)
/// serialization error
#define FERR_SERIALIZE                  (FE_BASE+0x0009)
/// trying to read the data beyond the index
#define FERR_MEMORY                             (FE_BASE+0x000a)
/// encryption/decryption error
#define FERR_CRYPT                              (FE_BASE+0x000b)
/// search error
#define FERR_SEARCH                             (FE_BASE+0x000c)
/// set/get attrib error
#define FERR_FINFO                              (FE_BASE+0x000d)
/// fast-move/rename error
#define FERR_MOVE                               (FE_BASE+0x000e)
/// Unicode strings not supported in this build of library
#define FERR_UNICODE							(FE_BASE+0x000f)

////////////////////////////////////////////////////////////////////
// module-related
/// Base define for module related errors
#define PE_BASE                                 0x00000000

/// plugin found with the id equal to one of the loaded plugins
#define PE_DUPLICATEPLUG                (PE_BASE+0x0000)
/// the module does not implement a mandatory function (or other words - cannot load an export from a module)
#define PE_CALLNOTIMPLEMENTED   (PE_BASE+0x0001)
/// the search error
#define PE_SEARCHERROR                  (PE_BASE+0x0002)
/// Cannot load an external module
#define PE_CANNOTLOAD                   (PE_BASE+0x0003)
/// Cannot unload the external module
#define PE_CANNOTUNLOAD                 (PE_BASE+0x0004)
/// Module not found
#define PE_NOTFOUND                             (PE_BASE+0x0005)

/////////////////////////////////////////////////////////////////////
// encryption related

/// Base define for encryption related error codes
#define EE_BASE							0x00020000

/// Initialization of the cipher failed
#define EE_INIT							(EE_BASE+0x0000)
/// Encryption failed
#define EE_CRYPT						(EE_BASE+0x0001)
/// Decryption failed
#define EE_DECRYPT						(EE_BASE+0x0002)

/////////////////////////////////////////////////////////////////////
// conversion related
/// Base define for conversion errors
#define CE_BASE							0x00030000
/// Error in converting a hex string to the binary data
#define CE_HEX2BIN						(CE_BASE+0x0000)

////////////////////////////////////////////////////////////////////
// general
#define GE_BASE							0x00040000
/// Invalid argument passed to the function
#define GE_INVALIDARG					(GE_BASE + 0x0000)

#endif
