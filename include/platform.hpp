/*
 *  UnrealIRCd, the next generation IRCd.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef _UNREALIRCD_PLATFORM_HPP
#define _UNREALIRCD_PLATFORM_HPP

/**
 * Platform-specific macros.
 */

// OS related
#if defined(_WIN32)

#	include <windows.h>

#	define OS_WINDOWS

#	if defined(_WIN64)
#		define OS_WIN64
#	else
#		define OS_WIN32
#	endif

#elif defined(__linux__) || defined(__linux)

	#define OS_LINUX

#endif

// compiler
#if defined(_MSC_VER)

	#define COMPILER_MSVC

	typedef __int8 int8_t;
	typedef __int16 int16_t;
	typedef __int32 int32_t;
	typedef __int64 int64_t;

	typedef unsigned __int8 uint8_t;
	typedef unsigned __int16 uint16_t;
	typedef unsigned __int32 uint32_t;
	typedef unsigned __int64 uint64_t;

#elif defined(__GNUC__)

	#define COMPILER_GCC

	#include <inttypes.h>

#endif

/** declare basic data types */
typedef int8_t    int8;
typedef int16_t   int16;
typedef int32_t   int32;
typedef int64_t   int64;

typedef uint8_t   uint8;
typedef uint16_t  uint16;
typedef uint32_t  uint32;
typedef uint64_t  uint64;

#endif /* _UNREALIRCD_PLATFORM_HPP */
