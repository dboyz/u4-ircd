/*****************************************************************
 * Unreal Internet Relay Chat Daemon, Version 4
 * File         pthread.hpp
 * Description  Posix threads
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

#ifndef _UNREALIRCD_PTHREAD_HPP
#define _UNREALIRCD_PTHREAD_HPP

#include "string.hpp"
#include "thread.hpp"
#include <memory>
#include <pthread.h>

extern "C" void* unreal_posix_thread_function(void* arg);

/**
 * Posix mutal exclusions.
 */
class UnrealPosixMutex
{
public:
	UnrealPosixMutex();
	~UnrealPosixMutex();
	void lock();
	void unlock();

private:
	::pthread_mutex_t mutex_;
};

/**
 * Posix threads.
 */
class UnrealPosixThread
{
public:
	template<typename FnType> UnrealPosixThread(FnType fn)
		: joined_(false)
	{
		std::auto_ptr<FuncBase> fu(new Func<FnType>(fn));

		// Thread creation
		if (::pthread_create(
				&native_,						//< pthread object
				NULL,							//< thread attributes
				unreal_posix_thread_function,	//< function to call
				fu.get())						//< callback function pointer
				!= 0)
		{
			throw new UnrealThreadException("UnrealPosixThread: "
					"pthread_create() failed",
					ErrorCode::Thread::CreationFailed);
		}

		// Release memory
		fu.release();
	}

	~UnrealPosixThread();
	void join();
	static String type();

public:
	friend void* unreal_posix_thread_function(void* arg);

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
	/** native pthread object */
	::pthread_t native_;

	/** something to check if we joined the thread already */
	bool joined_;
};

/**
 * Call function for pthread_create().
 *
 * @param arg Generic pointer for the callback function
 * @return 0
 */
inline void* unreal_posix_thread_function(void* arg)
{
	std::auto_ptr<UnrealPosixThread::FuncBase> fu(
	    static_cast<UnrealPosixThread::FuncBase*>(arg));

	fu->exec();

	return 0;
}

/** global typedef */
typedef UnrealPosixMutex UnrealMutex;
typedef UnrealPosixThread UnrealThread;

#endif /* _UNREALIRCD_PTHREAD_HPP */
