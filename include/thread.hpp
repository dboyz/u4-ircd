/*****************************************************************
 * Unreal Internet Relay Chat Daemon, Version 4
 * File         thread.hpp
 * Description  Thread wrapper implementation
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

#ifndef _UNREALIRCD_THREAD_HPP
#define _UNREALIRCD_THREAD_HPP

#include "exception.hpp"
#include "platform.hpp"
#include "string.hpp"

#if defined(OS_WINDOWS)
#include "wthread.hpp"
#else
#include "pthread.hpp"
#endif

namespace ErrorCode {

/**
 * Thread-related error codes.
 */
enum Thread
{
	OK,                 //!< no error specified
	CreationFailed,     //!< thread creation failed
	DestructFailed,     //!< thread destruction failed
	WaitFailed,         //!< waiting for thread to destruct failed
};

/**
 * Mutex-related error codes.
 */
enum Mutex
{
	OK,					//!< no error specified
	InitFailed,			//!< mutex initialization failed
	DestroyFailed,		//!< mutex destruction failed
	LockFailed,			//!< mutex lock failed
	UnlockFailed		//!< mutex unlock failed
};

} // namespace ErrorCode

/**
 * Thread exception class.
 */
class UnrealThreadException
	: public UnrealException<ErrorCode::Thread>
{
public:
	typedef ErrorCode::Thread Error;
};

/**
 * Mutex exception class.
 */
class UnrealMutexException
	: public UnrealException<ErrorCode::Mutex>
{
public:
	typedef ErrorCode::Mutex Error;
};

#endif /* _UNREALIRCD_THREAD_HPP */
