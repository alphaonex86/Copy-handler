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
#ifndef __PROPERTYTYPES_H__
#define __PROPERTYTYPES_H__

#pragma once

#include "../libicpf/cfg.h"

// properties definitions
enum CHProperties
{
	PP_PCLIPBOARDMONITORING = 0,
	PP_PMONITORSCANINTERVAL,
	PP_PRELOADAFTERRESTART,
	PP_PCHECK_FOR_UPDATES_FREQUENCY,
	PP_PUPDATE_CHECK_FOR_BETA,
	PP_PSHUTDOWNAFTREFINISHED,
	PP_PTIMEBEFORESHUTDOWN,
	PP_PFORCESHUTDOWN,
	PP_PAUTOSAVEINTERVAL,
	PP_PPROCESSPRIORITYCLASS,
//	PP_PAUTOSAVEDIRECTORY,
	PP_PLANGUAGE,

	PP_STATUSREFRESHINTERVAL,
	PP_STATUSSHOWDETAILS,
	PP_STATUSAUTOREMOVEFINISHED,

	PP_FDWIDTH,
	PP_FDHEIGHT,
	PP_FDSHORTCUTLISTSTYLE,
	PP_FDEXTENDEDVIEW,
	PP_FDIGNORESHELLDIALOGS,

	PP_MVSHOWFILENAMES,
	PP_MVSHOWSINGLETASKS,
	PP_MVREFRESHINTERVAL,
	PP_MVAUTOSHOWWHENRUN,
	PP_MVAUTOHIDEWHENEMPTY,
	PP_MVUSESMOOTHPROGRESS,

	PP_CMUSEAUTOCOMPLETEFILES,
	PP_CMSETDESTATTRIBUTES,
	PP_CMSETDESTDATE,
	PP_CMPROTECTROFILES,
	PP_CMLIMITMAXOPERATIONS,
	PP_CMREADSIZEBEFOREBLOCKING,
	PP_CMSHOWVISUALFEEDBACK,
	PP_CMUSETIMEDFEEDBACK,
	PP_CMFEEDBACKTIME,
	PP_CMAUTORETRYONERROR,
	PP_CMAUTORETRYINTERVAL,
	PP_CMDEFAULTPRIORITY,
	PP_CMDISABLEPRIORITYBOOST,
	PP_CMDELETEAFTERFINISHED,
	PP_CMCREATELOG,

	PP_SHSHOWCOPY,
	PP_SHSHOWMOVE,
	PP_SHSHOWCOPYMOVE,
	PP_SHSHOWPASTE,
	PP_SHSHOWPASTESPECIAL,
	PP_SHSHOWCOPYTO,
	PP_SHSHOWMOVETO,
	PP_SHSHOWCOPYMOVETO,
	PP_SHSHOWFREESPACE,
	PP_SHSHOWSHELLICONS,
	PP_SHINTERCEPTDRAGDROP,
	PP_SHINTERCEPTKEYACTIONS,
	PP_SHINTERCEPTCTXMENUACTIONS,

	PP_BFUSEONLYDEFAULT,
	PP_BFDEFAULT,
	PP_BFONEDISK,
	PP_BFTWODISKS,
	PP_BFCD,
	PP_BFLAN,
	PP_BFUSENOBUFFERING,
	PP_BFBOUNDARYLIMIT,

	PP_LOGENABLELOGGING,
	PP_LOGMAXSIZE,
	PP_LOGLEVEL,

	PP_SNDPLAYSOUNDS,
	PP_SNDERRORSOUNDPATH,
	PP_SNDFINISHEDSOUNDPATH,

	PP_SHORTCUTS,
	PP_RECENTPATHS,

	// invisible options
	PP_LAST_UPDATE_TIMESTAMP
};

enum EUpdatesFrequency
{
	eFreq_Never,
	eFreq_EveryStartup,
	eFreq_Daily,
	eFreq_Weekly,
	eFreq_OnceEvery2Weeks,
	eFreq_Monthly,
	eFreq_Quarterly,
	eFreq_Max
};

// register function
bool RegisterProperties(icpf::config* pManager);

#endif