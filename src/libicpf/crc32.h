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

/** \file crc32.h
 *  \brief Contain function counting crc32 checksum
 */

#ifndef __CRC32_H__
#define __CRC32_H__

#include "libicpf.h"
#include "gen_types.h"
#include <sys/types.h>

BEGIN_ICPF_NAMESPACE

/// Calculates crc32 checksum for a given data
LIBICPF_API uint_t crc32(const byte_t* pbyData, size_t tLen);

LIBICPF_API void crc32_begin(uint_t *puiValue);
LIBICPF_API void crc32_partial(uint_t *puiPrev, const byte_t *pbyData, size_t tLen);
LIBICPF_API void crc32_finish(const uint_t* puiValue);

END_ICPF_NAMESPACE

#endif
