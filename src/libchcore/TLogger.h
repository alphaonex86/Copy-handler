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
/// @file TLogger.h
/// @date 2009/05/19
/// @brief File contains logger class declaration.
// ============================================================================
#ifndef __LOGGER_H__
#define __LOGGER_H__

#include "libchcore.h"
#include "../libicpf/log.h"

#define LOG_DEBUG(text)\
	chcore::TLogger::LogDebug(text)

#define LOG_INFO(text)\
	chcore::TLogger::LogInfo(text)

#define LOG_WARNING(text)\
	chcore::TLogger::LogWarning(text)

#define LOG_ERROR(text)\
	chcore::TLogger::LogError(text)

BEGIN_CHCORE_NAMESPACE

// Class manages logging of informations to a file.
class LIBCHCORE_API TLogger : public icpf::log_file
{
public:
	TLogger();

	// Retrieving global object instance
	static TLogger& Acquire();		///< Acquires the Logger object

	// Logging
	static void LogDebug(const tchar_t* pszText);
	static void LogInfo(const tchar_t* pszText);
	static void LogWarning(const tchar_t* pszText);
	static void LogError(const tchar_t* pszText);

	// Initialization/settings
	void Enable(bool bEnable) throw() { m_bEnabled = bEnable; }

protected:
	static TLogger S_Logger;
	bool m_bEnabled;		// logging enabled?
};

END_CHCORE_NAMESPACE

#endif
