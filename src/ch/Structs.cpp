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
#include "structs.h"
#include "resource.h"
#include "..\common\FileSupport.h"
#include "StringHelpers.h"
#include "ch.h"
#include "task.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// global
int PriorityToIndex(int nPriority)
{
	switch(nPriority)
	{
	case THREAD_PRIORITY_TIME_CRITICAL:
		return 0;
	case THREAD_PRIORITY_HIGHEST:
		return 1;
	case THREAD_PRIORITY_ABOVE_NORMAL:
		return 2;
	case THREAD_PRIORITY_NORMAL:
		return 3;
	case THREAD_PRIORITY_BELOW_NORMAL:
		return 4;
	case THREAD_PRIORITY_LOWEST:
		return 5;
	case THREAD_PRIORITY_IDLE:
		return 6;
	default:
		return 3;
	}
}

int IndexToPriority(int nIndex)
{
	switch(nIndex)
	{
	case 0:
		return THREAD_PRIORITY_TIME_CRITICAL;
	case 1:
		return THREAD_PRIORITY_HIGHEST;
	case 2:
		return THREAD_PRIORITY_ABOVE_NORMAL;
	case 3:
		return THREAD_PRIORITY_NORMAL;
	case 4:
		return THREAD_PRIORITY_BELOW_NORMAL;
	case 5:
		return THREAD_PRIORITY_LOWEST;
	case 6:
		return THREAD_PRIORITY_IDLE;
	default:
		return THREAD_PRIORITY_NORMAL;
	}
}

int IndexToPriorityClass(int iIndex)
{
	switch(iIndex)
	{
	case 0:
		return IDLE_PRIORITY_CLASS;
	case 1:
		return NORMAL_PRIORITY_CLASS;
	case 2:
		return HIGH_PRIORITY_CLASS;
	case 3:
		return REALTIME_PRIORITY_CLASS;
	default:
		return NORMAL_PRIORITY_CLASS;
	}
}

int PriorityClassToIndex(int iPriority)
{
	switch(iPriority)
	{
	case IDLE_PRIORITY_CLASS:
		return 0;
	case NORMAL_PRIORITY_CLASS:
		return 1;
	case HIGH_PRIORITY_CLASS:
		return 2;
	case REALTIME_PRIORITY_CLASS:
		return 3;
	default:
		return 1;
	}
}
