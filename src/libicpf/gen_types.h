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
/** \file gen_types.h
 *  \brief Contains some compile-time settings for the whole engine.
 */
#ifndef __GENTYPES_H__
#define __GENTYPES_H__

#if defined(HAVE_CONFIG_H)
	#include "config.h"
#endif

#ifdef HAVE_INTTYPES_H
	#include <inttypes.h>
#else
	#include <stddef.h>
#endif

#if defined (_WIN32) || defined (_WIN64)
	#include <tchar.h>
#endif

#include <string>

// standard types and formats used throughout the library
// exactly 1 byte
/// Byte type (8bit unsigned int)
typedef unsigned char 	byte_t;

// chars
/// 8bit signed char
typedef char			char_t;
/// 8bit unsigned char
typedef unsigned char	uchar_t;

// system/configuration dependent chars
#if (defined(_WIN32) || defined(_WIN64)) && defined(_UNICODE)
	/// System/configuration dependent character (either wide char or normal one)
	typedef wchar_t		tchar_t;
	typedef std::wstring tstring_t;
	/// Macro to be appended to each text in code to be either composed of wide characters or normal ones
	#define __t(text)	L##text
	#define _t(text)	__t(text)
	/// String formatting string - depending on configuration could display wide char string or normal one.
	#define TSTRFMT		WSTRFMT
	#define TCHRFMT		CHRFMT
#else
	// description as above
	typedef char_t		tchar_t;
	typedef std::string tstring_t;
	#define _t(text)	text
	#define TSTRFMT		STRFMT
	#define TCHRFMT		CHRFMT
#endif

// 16-bit integers
/// 16bit short integer
typedef short			short_t;
/// 16bit unsigned short integer
typedef unsigned short	ushort_t;

// 32-bit integers
#ifdef _WIN32
	#ifdef _WIN64
		/// 32bit integer
		typedef int			int_t;
		/// 32bit integer
		typedef long			long_t;
		/// 32bit unsigned integer
		typedef unsigned int	uint_t;
		/// 32bit unsigned long
		typedef unsigned long	ulong_t;
	#else
		/// 32bit integer
		typedef int				int_t;
		/// 32bit integer
		typedef long			long_t;
		/// 32bit unsigned integer
		typedef unsigned int	uint_t;
		/// 32bit unsigned integer
		typedef unsigned long	ulong_t;
	#endif
#else
	/// 32bit integer
	typedef int					int_t;
	/// 32bit integer
	typedef int					long_t;
	/// 32bit unsigned integer
	typedef unsigned int		uint_t;
	/// 32bit unsigned integer
	typedef unsigned int		ulong_t;
#endif

// 64-bit integers;
/// 64bit;
typedef long long				longlong_t;
/// 64bit unsigned long long
typedef unsigned long long		ulonglong_t;
/// 64bit long long
typedef  longlong_t				ll_t;
/// 64bit unsigned long long
typedef  ulonglong_t			ull_t;

// double and float
/// Float type
typedef float					float_t;
/// Double type
typedef double					double_t;

// platform dependent integers (32-bit on 32-bit platforms, 64-bit on 64-bit platforms)
#ifdef _WIN32
	/// platform-dependent size signed integer (32-bit on 32-bit platforms, 64-bit on 64-bit platforms)
//	typedef int				intptr_t;
	/// platform-dependent size signed integer (32-bit on 32-bit platforms, 64-bit on 64-bit platforms)
	typedef intptr_t		longptr_t;
	/// platform-dependent size unsigned integer (32-bit on 32-bit platforms, 64-bit on 64-bit platforms)
//	typedef unsigned int	uintptr_t;
	/// platform-dependent size unsigned integer (32-bit on 32-bit platforms, 64-bit on 64-bit platforms)
	typedef uintptr_t		ulongptr_t;
#else
	// linux and other
	/// platform-dependent size signed integer (32-bit on 32-bit platforms, 64-bit on 64-bit platforms)
//	typedef long				intptr_t;
	/// platform-dependent size signed integer (32-bit on 32-bit platforms, 64-bit on 64-bit platforms)
	typedef intptr_t		longptr_t;
	/// platform-dependent size unsigned integer (32-bit on 32-bit platforms, 64-bit on 64-bit platforms)
//	typedef unsigned long		uintptr_t;
	/// platform-dependent size unsigned integer (32-bit on 32-bit platforms, 64-bit on 64-bit platforms)
	typedef uintptr_t		ulongptr_t;
#endif

// pointer
/// untyped pointer
typedef void*				ptr_t;

/// std::string or std::wstring dependent on _UNICODE macro
#ifdef _UNICODE
	#define tstring std::wstring
#else
	#define tstring std::string
#endif

// formatting-related macros
// chars
/// Printf-style format string for displaying char_t value (as char)
#define CHARFMT		_t("%c")
/// Printf-style format string for displaying uchar_t value (as char)
#define UCHARFMT	CHARFMT

// char related numbers (workaround for (u)chars - values are(should be) converted to (u)short_t)
/// Printf-style format string for displaying char_t as a number (the number has to be converted to short_t)
#define CFMT		_t("%hd")
/// Printf-style format string for displaying char_t as a hexadecimal number (the number has to be converted to short_t)
#define CXFMT		_t("0x%.2hx")
/// Printf-style format string for displaying uchar_t as a number (the number has to be converted to ushort_t)
#define UCFMT		_t("%hu")
/// Printf-style format string for displaying uchar_t as a hexadecimal number (the number has to be converted to ushort_t)
#define UCXFMT		CXFMT

// numbers
// 16-bit
/// Printf-style format string for displaying short_t as a number
#define SFMT		_t("%hd")
/// Printf-style format string for displaying short_t as a hex number
#define SXFMT		_t("0x%.4hx")
/// Printf-style format string for displaying ushort_t as a number
#define USFMT		_t("%hu")
/// Printf-style format string for displaying ushort_t as a hex number
#define USXFMT		SXFMT

// 32-bit
#ifdef _WIN32
	/// Printf-style format string for displaying long_t
	#define LFMT		_t("%ld")
	/// Printf-style format string for displaying long_t as a hex number
	#define LXFMT		_t("0x%.8lx")
	/// Printf-style format string for displaying ulong_t
	#define ULFMT		_t("%lu")
#else
	/// Printf-style format string for displaying long_t
	#define LFMT		_t("%d")
	/// Printf-style format string for displaying long_t as a hex number
	#define LXFMT		_t("0x%.8x")
	/// Printf-style format string for displaying ulong_t
	#define ULFMT		_t("%u")
#endif

/// Printf-style format string for displaying int_t
#define IFMT		LFMT
/// Printf-style format string for displaying int_t as a hex number
#define IXFMT		LXFMT
/// Printf-style format string for displaying uint_t
#define UIFMT		ULFMT
/// Printf-style format string for displaying ulong_t as a hex number
#define ULXFMT		LXFMT
/// Printf-style format string for displaying uint_t as a hex number
#define UIXFMT		ULXFMT

// 64-bit & system dependent
#ifdef _WIN32
	/// Printf-style format string for displaying ulonglong_t as a number
	#define ULLFMT		_t("%I64u")
	/// Printf-style format string for displaying ulonglong_t as a hex number
	#define ULLXFMT		_t("0x%.16I64x")
	/// Printf-style format string for displaying longlong_t
	#define LLFMT		_t("%I64d")
	/// Printf-style format string for displaying longlong_t as a hex number
	#define LLXFMT		ULLXFMT
	
	#ifdef _WIN64
		/// Printf-style format string for displaying intptr_t
		#define IPTRFMT		LLFMT
		/// Printf-style format string for displaying longptr_t
		#define LPTRFMT		LLFMT
		/// Printf-style format string for displaying intptr_t as a hex number
		#define IPTRXFMT	LLXFMT
		/// Printf-style format string for displaying longptr_t as a hex number
		#define LPTRXFMT	LLXFMT
		/// Printf-style format string for displaying uintptr_t
		#define UIPTRFMT	ULLFMT
		/// Printf-style format string for displaying ulongptr_t
		#define ULPTRFMT	ULLFMT
		/// Printf-style format string for displaying uintptr_t as a hex number
		#define UIPTRXFMT	ULLXFMT
		/// Printf-style format string for displaying ulongptr_t as a hex number
		#define ULPTRXFMT	ULLXFMT
	#else
		/// Printf-style format string for displaying intptr_t
		#define IPTRFMT		LFMT
		/// Printf-style format string for displaying longptr_t
		#define LPTRFMT		LFMT
		/// Printf-style format string for displaying intptr_t as a hex number
		#define IPTRXFMT	LXFMT
		/// Printf-style format string for displaying longptr_t as a hex number
		#define LPTRXFMT	LXFMT
		/// Printf-style format string for displaying uintptr_t
		#define UIPTRFMT	ULFMT
		/// Printf-style format string for displaying ulongptr_t
		#define ULPTRFMT	ULFMT
		/// Printf-style format string for displaying uintptr_t as a hex number
		#define UIPTRXFMT	ULXFMT
		/// Printf-style format string for displaying ulongptr_t as a hex number
		#define ULPTRXFMT	ULXFMT
	#endif
#else
	/// Printf-style format string for displaying ulonglong_t as a number
	#define ULLFMT		_t("%llu")
	/// Printf-style format string for displaying ulonglong_t as a hex number
	#define ULLXFMT		_t("0x%.16llx")
	/// Printf-style format string for displaying longlong_t
	#define LLFMT		_t("%lld")
	/// Printf-style format string for displaying longlong_t as a hex number
	#define LLXFMT		ULLXFMT
	
	// FIXME: distinguish between linux 32-bit architecture and 64-bit architecture here
	/// Printf-style format string for displaying intptr_t
	#define IPTRFMT		_t("%ld")
	/// Printf-style format string for displaying longptr_t
	#define LPTRFMT		IPTRFMT
	/// Printf-style format string for displaying intptr_t as a hex number
	#define IPTRXFMT	_t("0x%.8lx")
	/// Printf-style format string for displaying longptr_t as a hex number
	#define LPTRXFMT	IPTRXFMT
	/// Printf-style format string for displaying uintptr_t
	#define UIPTRFMT	_t("%lu")
	/// Printf-style format string for displaying ulongptr_t
	#define ULPTRFMT	UIPTRFMT
	/// Printf-style format string for displaying uintptr_t as a hex number
	#define UIPTRXFMT	_t("0x%.8lx")
	/// Printf-style format string for displaying ulongptr_t as a hex number
	#define ULPTRXFMT	UIPTRXFMT
#endif

// double (not checked for linux)
/// Printf-style format string for displaying float_t
#define FLOATFMT	_t("%.2f")
/// Printf-style format string for displaying double_t
#define DOUBLEFMT	_t("%.2f")

// strings

// NOTE: below are the specifications for strings, however win32/64 specified the
// formatting strings to be dependent on the function used - %s used in printf
// formats an ascii string, while the same %s used in wprintf gives a wide string.
// So, basically, those macros should be modified in some way - either by making
// a dependence on _UNICODE define or by creating additional set of macros to be used
// with wprintf() and use the current ones for printf().
#if defined(_WIN32) || defined(_WIN64)
	#ifdef _UNICODE
		/// Printf-style format string for displaying ansi strings (char_t based strings)
		#define STRFMT		_t("%S")
		/// Printf-style format string for displaying ascii char
		#define CHRFMT		_t("%C")
		/// Printf-style format string for displaying wide strings (wchar_t based strings)
		#define WSTRFMT		_t("%s")
		/// Printf-style format string for displaying wide char
		#define WCHRFMT		_t("%c")
	#else
		/// Printf-style format string for displaying ansi strings (char_t based strings)
		#define STRFMT		_t("%s")
		/// Printf-style format string for displaying ascii char
		#define CHRFMT		_t("%c")
		/// Printf-style format string for displaying wide strings (wchar_t based strings)
		#define WSTRFMT		_t("%S")
		/// Printf-style format string for displaying wide char
		#define WCHRFMT		_t("%C")
	#endif
#else
	/// Printf-style format string for displaying ansi strings (char_t based strings)
	#define STRFMT		_t("%s")
	/// Printf-style format string for displaying ascii char
	#define CHRFMT		_t("%c")
	/// Printf-style format string for displaying wide strings (wchar_t based strings)
	#define WSTRFMT		_t("%ls")
	/// Printf-style format string for displaying wide char (WARNING: untested)
	#define WCHRFMT		_t("%lc")
#endif

// pointer
/// Printf-style format string for displaying pointers
#define PTRFMT		_t("%p")
/// Printf-style format string for displaying pointers (with 0x prefix)
#define PTRXFMT		_t("0x%p")

#endif
