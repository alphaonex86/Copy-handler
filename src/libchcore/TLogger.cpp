// ============================================================================
//  Copyright (C) 2001-2009 by Jozef Starosczyk
//  ixen@copyhandler.com
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU Library General Public License
//  (version 2) as published by the Free Software Foundation;
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU Library General Public
//  License along with this program; if not, write to the
//  Free Software Foundation, Inc.,
//  59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
// ============================================================================
/// @file TLogger.cpp
/// @date 2009/05/19
/// @brief Contains logger class implementation.
// ============================================================================
#include "stdafx.h"
#include "libchcore.h"
#include "TLogger.h"

BEGIN_CHCORE_NAMESPACE

TLogger TLogger::S_Logger;

// ============================================================================
/// chcore::TLogger::TLogger
/// @date 2009/05/23
///
/// @brief     Constructs the TLogger object.
// ============================================================================
TLogger::TLogger() :
	m_bEnabled(false)
{
}

// ============================================================================
/// chcore::TLogger::Acquire
/// @date 2009/05/20
///
/// @brief     Acquires logger object.
/// @return    Reference to the logger object.
// ============================================================================
TLogger& TLogger::Acquire()
{
	return S_Logger;
}

// ============================================================================
/// chcore::TLogger::LogDebug
/// @date 2009/05/20
///
/// @brief     Logs an information to file (debug level).
/// @param[in] pszText	Text to be logged.
// ============================================================================
void TLogger::LogDebug(const tchar_t* pszText)
{
	BOOST_ASSERT(pszText);
	if(!pszText)
		return;

	TLogger& rLogger = Acquire();
	if(rLogger.m_bEnabled && rLogger.is_initialized())
		rLogger.logd(pszText);
}

// ============================================================================
/// chcore::TLogger::LogInfo
/// @date 2009/05/20
///
/// @brief     Logs an information to the file (info level).
/// @param[in] pszText	Text to be logged.
// ============================================================================
void TLogger::LogInfo(const tchar_t* pszText)
{
	BOOST_ASSERT(pszText);
	if(!pszText)
		return;

	TLogger& rLogger = Acquire();
	if(rLogger.m_bEnabled && rLogger.is_initialized())
		rLogger.logi(pszText);
}

// ============================================================================
/// chcore::TLogger::LogWarning
/// @date 2009/05/20
///
/// @brief     Logs an information to the file (info level).
/// @param[in] pszText	Text to be logged.
// ============================================================================
void TLogger::LogWarning(const tchar_t* pszText)
{
	BOOST_ASSERT(pszText);
	if(!pszText)
		return;

	TLogger& rLogger = Acquire();
	if(rLogger.m_bEnabled && rLogger.is_initialized())
		rLogger.logw(pszText);
}

// ============================================================================
/// chcore::TLogger::LogError
/// @date 2009/05/20
///
/// @brief     Logs an information to the file (info level).
/// @param[in] pszText	Text to be logged.
// ============================================================================
void TLogger::LogError(const tchar_t* pszText)
{
	BOOST_ASSERT(pszText);
	if(!pszText)
		return;

	TLogger& rLogger = Acquire();
	if(rLogger.m_bEnabled && rLogger.is_initialized())
		rLogger.loge(pszText);
}

END_CHCORE_NAMESPACE
