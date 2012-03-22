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
#ifndef __LIBICTRANSLATE_H__
#define __LIBICTRANSLATE_H__

// import/export macros
#if defined(_WIN32) || defined(_WIN64)
	#ifdef LIBICTRANSLATE_EXPORTS
		/** \brief Import/export macros
		*
		*  These macros are being used throughout the whole code. They are meant to
		*  export symbols (if the LIBICTRANSLATE_EXPORTS is defined) from this library
		*  (also for importing (when LIBICTRANSLATE_EXPORTS macro is undefined) in other apps).
		*/
		#define LIBICTRANSLATE_API __declspec(dllexport)
	#else
		/** \brief Import/export macros
		*
		*  These macros are being used throughout the whole code. They are meant to
		*  export symbols (if the LIBICTRANSLATE_EXPORTS is defined) from this library
		*  (also for importing (when LIBICTRANSLATE_EXPORTS macro is undefined) in other apps).
		*/
		#define LIBICTRANSLATE_API __declspec(dllimport)
	#endif
#else
	/** \brief Import/export macros
	*
	*  These macros are being used throughout the whole code. They are meant to
	*  export symbols (if the LIBICTRANSLATE_EXPORTS is defined) from this library
	*  (also for importing (when LIBICTRANSLATE_EXPORTS macro is undefined) in other apps).
	*/
	#define LIBICTRANSLATE_API
#endif

/// Begins ch namespace
#define BEGIN_ICTRANSLATE_NAMESPACE namespace ictranslate {
/// Ends ch namespace
#define END_ICTRANSLATE_NAMESPACE }

#endif
