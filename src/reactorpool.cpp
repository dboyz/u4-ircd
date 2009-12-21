/*****************************************************************
 * Unreal Internet Relay Chat Daemon, Version 4
 * File         reactorpool.cpp
 * Description  An object representing a set of worker threads
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

#include "base.hpp"
#include "reactorpool.hpp"
#include <assert.h>
#include <boost/thread.hpp>
#include <iostream>

/**
 * UnrealReactorPool constructor.
 *
 * The ReactorPool allows us to use all CPU cores effectively for
 * multithreading.
 *
 * @param nthreads Number of threads to allocate
 * @param nr Number of event reactors to allocate
 */
UnrealReactorPool::UnrealReactorPool(size_t nthreads, size_t nr)
	: threads_size_(nthreads), next_reactor_(0)
{
	assert(nios > 0);

	/* allocate the IO services */
	for (size_t i = 0; i < nios; i++)
	{
		UnrealReactor react;
		reactors_ << react;
	}
}

/**
 * Returns an reactor from the list to be used.
 * A round-robin scheme distributes work on the reactors.
 *
 * @return Reactor reference
 */
UnrealReactor& UnrealReactorPool::getReactor()
{
	UnrealReactor& react = reactors_[next_reactor_];

	/* increment counter */
	next_reactor_++;

	if (next_reactor_ >= reactors_.size())
		next_reactor_ = 0;

	return react;
}

/**
 * Resize the number of reactors in pool.
 *
 * Please note: If `new_size' is less the current pool size, all current
 * IO services are stopped.
 *
 * @param nthreads New number of threads
 * @param nr New number of reactors
 */
void UnrealReactorPool::resize(size_t nthreads, size_t nr)
{
    if (nios > reactors_.size())
    {
        size_t diff = nr - reactors_.size();

        for (size_t i = 0; i < diff; i++)
        {
			UnrealReactor react;
			reactors_ << react;
        }
    }
    else if (nr < reactors_.size())
    {
        size_t diff = reactors_.size() - nr;

        /* remove the difference amount of reactors */
        for (size_t i = 0; i < diff; i++)
        {
            reactors_.removeFirst();
        }
    }

    if (nthreads != threads_size_)
    	threads_size_ = nthreads;
}

/**
 * Thread all reactors to work in background.
 */
void UnrealIOServicePool::run()
{
/*
	List<ThreadPtr> threads;
	size_t ios_pos = 0;

	try
	{
		for (size_t i = 0; i < threads_size_; i++)
		{
			ThreadPtr thread(new boost::thread(
				boost::bind(&UnrealIOService::run, io_services_[ios_pos])));

			threads << thread;

			ios_pos++;

			if (ios_pos >= io_services_.size())
				ios_pos = 0;
		}

		for (size_t i = 0; i < threads.size(); i++)
			threads[i]->join();

		if (threads_size_ == 0)
			io_services_[0]->run();
	}
	catch (std::exception& ex)
	{
		unreal->log.write(UnrealLog::Normal, "Exception: [%s] - %s",
				__PRETTY_FUNCTION__,
				ex.what());
	}
*/
	reactors_[0].run();
}

/**
 * Returns the current size of allocated reactors.
 *
 * @return Pool size
 */
size_t UnrealReactorPool::size()
{
	return reactors_.size();
}

/**
 * Stop all running reactors.
 */
void UnrealReactorPool::stop()
{
	for (size_t i = 0; i < reactors_.size(); i++)
	{
		reactors_[i].stop();
	}
}

/**
 * Returns the number of threads allocated and running.
 *
 * @return Amount of running threads
 */
size_t UnrealReactorPool::threads()
{
	return threads_size_;
}
