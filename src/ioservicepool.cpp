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

#include "ioservicepool.hpp"
#include <assert.h>
#include <boost/thread.hpp>

/**
 * UnrealIOServicePool constructor.
 *
 * The IOServicePool allows us to use all CPU cores effectively for
 * multithreading.
 *
 * @param poolsize Number of IO services to allocate
 */
UnrealIOServicePool::UnrealIOServicePool(size_t poolsize)
	: next_io_service_(0)
{
	assert(poolsize > 0);

	/* allocate the IO services */
	for (size_t i = 0; i < poolsize; i++)
	{
		IOServicePtr iosptr(new boost::asio::io_service);
		io_services_ << iosptr;

		WorkPtr workptr(new boost::asio::io_service::work(*iosptr));
		works_ << workptr;
	}
}

/**
 * Returns an IO service from the list to be used for networking.
 *
 * @return IO service reference
 */
boost::asio::io_service& UnrealIOServicePool::getIOService()
{
  boost::asio::io_service& ios = *io_services_[next_io_service_];

  /* increment counter */
  next_io_service_++;

  if (next_io_service_ == io_services_.size())
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
void UnrealIOServicePool::resize(size_t new_size)
{
	if (new_size > io_services_.size())
	{
		size_t diff = new_size - io_services_.size();

		for (size_t i = 0; i < diff; i++)
		{
			IOServicePtr iosptr(new boost::asio::io_service);
			io_services_ << iosptr;

			WorkPtr workptr(new boost::asio::io_service::work(*iosptr));
			works_ << workptr;
		}
	}
	else if (new_size < io_services_.size())
	{
		/* stop all running IO services */
		stop();

		size_t diff = io_services_.size() - new_size;

		/* remove the difference amount of IO services */
		for (size_t i = 0; i < diff; i++)
		{
			io_services_.removeFirst();
			works_.removeFirst();
		}
	}
}

/**
 * Thread all IO services to work in background.
 */
void UnrealIOServicePool::run()
{
	List<boost::shared_ptr<boost::thread> > threads;

	for (size_t i = 0; i < io_services_.size(); i++)
	{
		boost::shared_ptr<boost::thread> thread(new boost::thread(
			boost::bind(&boost::asio::io_service::run, io_services_[i])));

		/* append the thread */
		threads << thread;
	}

	/* wait for all threads in the pool to exit */
	for (size_t i = 0; i < threads.size(); i++)
		threads[i]->join();
}

/**
 * Returns the current size of the internal pool.
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
