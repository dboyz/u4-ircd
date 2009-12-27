/*****************************************************************
 * Unreal Internet Relay Chat Daemon, Version 4
 * File         wthread.hpp
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

#ifndef _UNREALIRCD_WTHREAD_HPP
#define _UNREALIRCD_WTHREAD_HPP

#include "platform.hpp"
#include "string.hpp"
#include <memory>
#include <pthread.h>

extern "C" void* unreal_windows_thread_function(void* arg);

/**
 * Windows mutal exclusions, critical sections
 */
class UnrealWinMutex
{
public:
	UnrealWinMutex();
	~UnrealWinMutex();
	void lock();
	void unlock();

private:
	/** mutex handle */
	HANDLE mutex_;
};

/**
 * Windows(R) threads.
 */
class UnrealWinThread
{
public:
	template<typename FnType> UnrealWinThread(FnType fn)
		: closed_(false)
	{
		std::auto_ptr<FuncBase> fu(new Func<FnType>(fn));

		// Thread creation
		native_ = CreateThread(
				0,								//< security attributes
				0,								//< initial size of stack
				unreal_windows_thread_function,	//< function call
				fu.get(),						//< callback function pointer
				0,								//< initial thread state
				&thread_id_);					//< thread id storage

		if (native_ == NULL)
		{
			String msg;
			msg.sprintf("UnrealWinThread: CreateThread() failed: %s",
					strerror_w(GetLastError()));

			throw new UnrealThreadException(msg,
					UnrealThreadException::Error::CreationFailed);
		}

		// Release memory
		fu.release();
	}

	~UnrealWinThread();
	void join();
	static String type();

public:
	friend void* unreal_windows_thread_function(void* arg);

	class FuncBase
	{
	public:
		virtual ~FuncBase() {}
		virtual void exec() = 0;
	};

	template<typename FnType> class Func : public FuncBase
	{
	public:
		Func(FnType fn) : fn_(fn) {}
		void exec() { fn_(); }
	private:
		FnType fn_;
	};

private:
	/** native thread handle */
	HANDLE native_;

	/** thread ID */
	DWORD thread_id_;

	/** already closed? */
	bool closed_;
};

/**
 * Call function for ResumeThread().
 *
 * @param arg Generic pointer for the callback function
 * @return NULL
 */
inline void* unreal_windows_thread_function(void* arg)
{
	std::auto_ptr<UnrealWinThread::FuncBase> fu(
	    static_cast<UnrealWinThread::FuncBase*>(arg));

	fu->exec();

	return NULL;
}

/** global typedef */
typedef UnrealWinMutex UnrealMutex;
typedef UnrealWinThread UnrealThread;

#endif /* _UNREALIRCD_WTHREAD_HPP */
