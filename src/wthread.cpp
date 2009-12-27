/*****************************************************************
 * Unreal Internet Relay Chat Daemon, Version 4
 * File         wthread.cpp
 * Description  Windows(R) threads
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

#include "wthread.hpp"

/**
 * Mutex initialization.
 */
UnrealWinMutex::UnrealWinMutex()
{
	mutex_ = CreateMutex(
			NULL,
			FALSE,
			0);

	if (mutex_ == NULL)
	{
		String msg;
		msg.sprintf("UnrealWinMutex: Initialization failed: %s",
				strerror_w(GetLastError()));

		throw new UnrealMutexException(msg,
				UnrealMutexException::Error::InitFailed);
	}
}

/**
 * Mutex destruction.
 */
UnrealWinMutex::~UnrealWinMutex()
{
	if (!CloseHandle(mutex_))
	{
		String msg;
		msg.sprintf("UnrealWinMutex: Destruction failed: %s",
				strerror_w(GetLastError()));

		throw new UnrealMutexException(msg,
				UnrealMutexException::Error::DestructFailed);
	}
}

/**
 * Tries to gain exclusive ownership for the mutex.
 */
void UnrealWinMutex::lock()
{
	if (WaitForSingleObject(&mutex_, INFINITE) == WAIT_FAILED)
	{
		String msg;
		msg.sprintf("UnrealWinMutex: Lock failed: %s",
				strerror_w(GetLastError()));

		throw new UnrealMutexException(msg,
				UnrealMutexException::Error::LockFailed);
	}
}

/**
 * Release mutex ownership.
 */
void UnrealWinMutex::unlock()
{
	if (!ReleaseMutex(&mutex_))
	{
		String msg;
		msg.sprintf("UnrealWinMutex: Unlock failed: %s",
				strerror_w(GetLastError()));

		throw new UnrealMutexException(msg,
				UnrealMutexException::Error::UnlockFailed);
	}
}

/**
 * Thread initialization.
 */
UnrealWinThread::~UnrealWinThread()
{
	if (!closed_ && !CloseHandle(native_))
	{
		String msg;
		msg.sprintf("UnrealWinThread: Destruction of thread failed: "
				"%s", strerror_w(GetLastError()));

		throw new UnrealThreadException(msg,
				UnrealThreadException::Error::DestructFailed);
	}
}

/**
 * Wait for thread termination.
 */
void UnrealWinThread::join()
{
	if (WaitForSingleObject(native_, INFINITE) == WAIT_FAILED)
	{
		String msg;
		msg.sprintf("UnrealWinThread: Waiting for thread termination failed: "
				"%s", strerror_w(GetLastError()));

		throw new UnrealThreadException(msg,
				UnrealThreadException::Error::WaitFailed);
	}
}

/**
 * Returns the name of threading library.
 */
String UnrealWinThread::type()
{
	return "windows";
}
