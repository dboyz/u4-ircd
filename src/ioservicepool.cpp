/*****************************************************************
 * Unreal Internet Relay Chat Daemon, Version 4
 * File         ioservicepool.cpp
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
#include "ioservicepool.hpp"
#include <assert.h>
#include <boost/thread.hpp>
#include <iostream>

/**
 * UnrealIOServicePool constructor.
 *
 * The IOServicePool allows us to use all CPU cores effectively for
 * multithreading.
 *
 * @param threads Number of threads to allocate
 * @param iossize Number of IO services to allocate
 */
UnrealIOServicePool::UnrealIOServicePool(size_t nthreads, size_t nios)
	: threads_size_(nthreads), next_io_service_(0)
{
	assert(nios > 0);

	/* allocate the IO services */
	for (size_t i = 0; i < nios; i++)
	{
		IOServicePtr iosptr(new UnrealIOService());
		io_services_ << iosptr;

		WorkPtr workptr(new UnrealIOService::Work(*iosptr));
		works_ << workptr;
	}
}

/**
 * Returns an IO service from the list to be used.
 * A round-robin scheme distributes work on the IO services.
 *
 * @return IO service reference
 */
UnrealIOService& UnrealIOServicePool::getIOService()
{
	UnrealIOService& ios = *io_services_[next_io_service_];

	/* increment counter */
	next_io_service_++;

	if (next_io_service_ >= io_services_.size())
		next_io_service_ = 0;

	return ios;
}

/**
 * Resize the number of IO services in pool.
 *
 * Please note: If `new_size' is less the current pool size, all current
 * IO services are stopped.
 *
 * @param new_size New pool size
 */
void UnrealIOServicePool::resize(size_t nthreads, size_t nios)
{
    if (nios > io_services_.size())
    {
        size_t diff = nios - io_services_.size();

        for (size_t i = 0; i < diff; i++)
        {
            IOServicePtr iosptr(new UnrealIOService());
            io_services_ << iosptr;

            WorkPtr workptr(new UnrealIOService::Work(*iosptr));
            works_ << workptr;
        }
    }
    else if (nios < io_services_.size())
    {
        /* stop all running IO services */
        stop();

        size_t diff = io_services_.size() - nios;

        /* remove the difference amount of IO services */
        for (size_t i = 0; i < diff; i++)
        {
            io_services_.removeFirst();
            works_.removeFirst();
        }
    }

    if (nthreads != threads_size_)
    	threads_size_ = nthreads;
}

/**
 * Thread all IO services to work in background.
 */
void UnrealIOServicePool::run()
{
	List<ThreadPtr> threads;
	size_t ios_pos = 0;

	/* allocate threads */
	try
	{
		for (size_t i = 0; i < threads_size_; i++)
		{
			ThreadPtr thread(new boost::thread(
				boost::bind(&UnrealIOService::run, io_services_[ios_pos])));

			/* append the thread */
			threads << thread;

			ios_pos++;

			if (ios_pos >= io_services_.size())
				ios_pos = 0;
		}

		/* wait for all threads in the pool to exit */
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
}

/**
 * Returns the current size of allocated IO services.
 *
 * @return Pool size
 */
size_t UnrealIOServicePool::size()
{
	return io_services_.size();
}

/**
 * Stop all running IO services.
 */
void UnrealIOServicePool::stop()
{
	for (size_t i = 0; i < io_services_.size(); i++)
	{
		io_services_[i]->stop();
	}
}

/**
 * Returns the number of threads allocated and running.
 *
 * @return Amount of running threads
 */
size_t UnrealIOServicePool::threads()
{
	return threads_size_;
}
