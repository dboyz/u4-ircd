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
#include <windows.h>
#include "windows.hpp"
#define OS_WINDOWS
#if defined(_WIN64)
#define OS_WIN64
#define OS_NAME "Win64"
#else
#define OS_WIN32
#define OS_NAME "Win32"
#endif

#elif defined(__linux__) || defined(__linux)
#define OS_LINUX
#define OS_NAME "Linux"
#elif defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__)
#if defined(__FreeBSD__)
#define OS_FREEBSD
#define OS_NAME "FreeBSD"
#elif defined(__NetBSD__)
#define OS_NETBSD
#define OS_NAME "NetBSD"
#elif defined(__OpenBSD__)
#define OS_OPENBSD
#define OS_NAME "OpenBSD"
#endif
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

#endif /* _UNREALIRCD_PLATFORM_HPP */
