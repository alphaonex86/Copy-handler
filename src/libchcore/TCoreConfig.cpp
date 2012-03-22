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
#include "../libicpf/cfg.h"
#include "TCoreConfig.h"
#include "../libicpf/exception.h"

BEGIN_CHCORE_NAMESPACE

TCoreConfig TCoreConfig::S_Config;

// ============================================================================
/// chcore::TCoreConfig::TCoreConfig
/// @date 2009/11/30
///
/// @brief     Constructs the core configuration object.
// ============================================================================
TCoreConfig::TCoreConfig() :
	m_config(icpf::config::eIni)
{
}

// ============================================================================
/// chcore::TCoreConfig::~TCoreConfig
/// @date 2009/11/30
///
/// @brief     Destructs the core configuration object.
// ============================================================================
TCoreConfig::~TCoreConfig()
{
}

// ============================================================================
/// chcore::TCoreConfig::Acquire
/// @date 2009/11/30
///
/// @brief     Acquires reference to core config.
/// @return    Reference to core config.
// ============================================================================
TCoreConfig& TCoreConfig::Acquire()
{
	return S_Config;
}

END_CHCORE_NAMESPACE
