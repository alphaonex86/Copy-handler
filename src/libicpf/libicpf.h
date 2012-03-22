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
#ifndef __LIBICPF_H__
#define __LIBICPF_H__

/** \brief Allows unicode handling throughout the files
 *
 * It means that the engine would process fs object's paths
 * in the better standard - if possible then with current ANSI code page (one-byte
 * chars) or with UNICODE strings (with 2-byte per char strings).
 * \note if this is disabled (#undef) then the data saved by ie. unicode windows
 * system may not be readable (because we won't use the iconv package on linux
 * systems nor use any of the unicode functions in windows systems).
 * Recommended setting is 'defined'.
 */
#define ALLOW_UNICODE

/** \brief Enables the mutex debugging code throughout this library.
 *
 *  Enables compiling the d_mutex class, so external or internal code could use mutex
 *  debugging.
 */
//#define ENABLE_MUTEX_DEBUGGING

/** \brief Enables mutex debugging/tracking in the internal and external code.
 */
//#define DEBUG_MUTEX

/** \brief Enables use of encryption throughout this library.
 *
 * Enabling this macro enables usage of the encryption in some modules.
 */
#define USE_ENCRYPTION

// import/export macros
#ifdef _WIN32
        #ifdef LIBICPF_EXPORTS
                /** \brief Import/export macros
                *
                *  These macros are being used throughout the whole code. They are meant to
                *  export symbols (if the LIBICPF_EXPORTS is defined) from this library
                *  (also for importing (when LIBICPF_EXPORTS macro is undefined) in other apps).
                */
                #define LIBICPF_API __declspec(dllexport)
				#define ICPFTEMPL_EXTERN
        #else
                /** \brief Import/export macros
                *
                *  These macros are being used throughout the whole code. They are meant to
                *  export symbols (if the LIBICPF_EXPORTS is defined) from this library
                *  (also for importing (when LIBICPF_EXPORTS macro is undefined) in other apps).
                */
                #define LIBICPF_API __declspec(dllimport)
				#define ICPFTEMPL_EXTERN extern
        #endif
#else
        /** \brief Import/export macros
        *
        *  These macros are being used throughout the whole code. They are meant to
        *  export symbols (if the LIBICPF_EXPORTS is defined) from this library
        *  (also for importing (when LIBICPF_EXPORTS macro is undefined) in other apps).
        */
        #define LIBICPF_API
#endif

/// Begins ch namespace
#define BEGIN_ICPF_NAMESPACE namespace icpf {
/// Ends ch namespace
#define END_ICPF_NAMESPACE }

#endif
