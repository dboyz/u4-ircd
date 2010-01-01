/*****************************************************************
 * Unreal Internet Relay Chat Daemon, Version 4
 * File         pthread.cpp
 * Description  Posix threads
 *
 * Copyright(C) 2009, 2010
 * The UnrealIRCd development team and contributors
 * http://www.unrealircd.com
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

#include "pthread.hpp"
#include <errno.h>
#include <string.h>

/**
 * Initialize the mutex.
 */
UnrealPosixMutex::UnrealPosixMutex()
{
	if (::pthread_mutex_init(&mutex_, 0) != 0)
	{
		String msg;
		msg.sprintf("UnrealPosixMutex: Initialization failed: %s",
				::strerror(errno));

		throw new UnrealMutexException(msg,
				ErrorCode::Mutex::InitFailed);
	}
}

/**
 * Mutex destruction.
 */
UnrealPosixMutex::~UnrealPosixMutex()
{
	if (::pthread_mutex_destroy(&mutex_) != 0)
	{
		String msg;
		msg.sprintf("UnrealPosixMutex: Destruction of mutex failed: %s",
				::strerror(errno));

		throw new UnrealMutexException(msg,
				ErrorCode::Mutex::DestroyFailed);
	}
}

/**
 * Lock the mutex.
 */
void UnrealPosixMutex::lock()
{
	if (::pthread_mutex_lock(&mutex_) != 0)
	{
		String msg;
		msg.sprintf("UnrealPosixMutex: Lock failed: %s",
				::strerror(errno));

		throw new UnrealMutexException(msg,
				ErrorCode::Mutex::LockFailed);
	}
}

/**
 * Unlock the mutex.
 */
void UnrealPosixMutex::unlock()
{
	if (::pthread_mutex_unlock(&mutex_) != 0)
	{
		String msg;
		msg.sprintf("UnrealPosixMutex: Unlock failed: %s",
				::strerror(errno));

		throw new UnrealMutexException(msg,
				ErrorCode::Mutex::UnlockFailed);
	}
}

/**
 * UnrealPosixThread destructor.
 */
UnrealPosixThread::~UnrealPosixThread()
{
	if (!joined_)
	{
		if (::pthread_detach(native_) != 0)
		{
			String msg;
			msg.sprintf("UnrealPosixThread: Detaching failed: %s",
					::strerror(errno));

			throw new UnrealThreadException(msg,
					ErrorCode::Thread::DestructFailed);
		}
	}
}

/**
 * Wait for thread termination.
 */
void UnrealPosixThread::join()
{
	if (!joined_)
	{
		if (::pthread_join(native_, 0) != 0)
		{
			String msg;
			msg.sprintf("UnrealPosixThread: Waiting for thread to finish "
					"failed: %s", ::strerror(errno));

			throw new UnrealThreadException(msg,
					ErrorCode::Thread::WaitFailed);
		}
		else
			joined_ = true;
	}
}

/**
 * Returns the name of threading library.
 */
String UnrealPosixThread::type()
{
	return "pthreads";
}
