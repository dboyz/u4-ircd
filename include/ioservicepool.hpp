/*****************************************************************
 * Unreal Internet Relay Chat Daemon, Version 4
 * File         ioservicepool.hpp
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

#ifndef _UNREALIRCD_IOSERVICEPOOL_HPP
#define _UNREALIRCD_IOSERVICEPOOL_HPP

#include "ioservice.hpp"
#include "list.hpp"
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>

/** default amount of threads */
#define IOSERVICEPOOL_DEFAULT_THREADS	1

/** default amount of IO services */
#define IOSERVICEPOOL_DEFAULT_IOSSIZE	1

/**
 * The UnrealIOServicePool class provides an I/O service pool that dispenses
 * operations on Sockets, Timers, etc. to multiple threads and/or IO services.
 */
class UnrealIOServicePool
{
public:
	UnrealIOServicePool(size_t nthreads = IOSERVICEPOOL_DEFAULT_THREADS,
			size_t nios = IOSERVICEPOOL_DEFAULT_IOSSIZE);

	UnrealIOService& getIOService();
	void resize(size_t nthreads, size_t nios);
	void run();
	size_t size();
	void stop();
	size_t threads();

private:
	/** alias shared_ptr for UnrealIOService */
	typedef boost::shared_ptr<UnrealIOService> IOServicePtr;

	/** alias shared_ptr for UnrealIOService::Work */
	typedef boost::shared_ptr<UnrealIOService::Work> WorkPtr;

	/** alias shared_ptr for boost::thread */
	typedef boost::shared_ptr<boost::thread> ThreadPtr;

private:
	/** I/O service pool */
	List<IOServicePtr> io_services_;

	/** I/O service work */
	List<WorkPtr> works_;

	/** threads size */
	size_t threads_size_;

	/** next I/O service to be used */
	size_t next_io_service_;
};

#endif /* _UNREALIRCD_IOSERVICEPOOL_HPP */
