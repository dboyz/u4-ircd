/*****************************************************************
 * Unreal Internet Relay Chat Daemon, Version 4
 * File         platform.hpp
 * Description  Abstractions around system-specific code
 *
 * All parts of this program are Copyright(C) 2009 by their
 * respective authors and the UnrealIRCd development team.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 3 of
 * the License, or (at your option) any later version.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program; if not, write to the Free
 * Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 ******************************************************************/

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

	typedef __int8 int8;
	typedef __int16 int16;
	typedef __int32 int32;
	typedef __int64 int64;

	typedef unsigned __int8 uint8;
	typedef unsigned __int16 uint16;
	typedef unsigned __int32 uint32;
	typedef unsigned __int64 uint64;

#elif defined(__GNUC__)

	#define COMPILER_GCC

	#include <inttypes.h>

	typedef int8_t    int8;
	typedef int16_t   int16;
	typedef int32_t   int32;
	typedef int64_t   int64;

	typedef uint8_t   uint8;
	typedef uint16_t  uint16;
	typedef uint32_t  uint32;
	typedef uint64_t  uint64;

#endif

#endif /* _UNREALIRCD_PLATFORM_HPP */
