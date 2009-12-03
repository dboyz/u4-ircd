/*
 *  UnrealIRCd, the next generation IRCd.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef _UNREALIRCD_IOSERVICEPOOL_HPP
#define _UNREALIRCD_IOSERVICEPOOL_HPP

#include "list.hpp"
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>

/** default I/O service pool size */
#define IOSERVICEPOOL_DEFAULT_SIZE	1

/**
 * The UnrealIOServicePool class provides an I/O service pool that dispenses
 * operations on Sockets, Timers, etc. to multiple threads.
 */
class UnrealIOServicePool
{
public:
	UnrealIOServicePool(size_t poolsize = IOSERVICEPOOL_DEFAULT_SIZE);

	boost::asio::io_service& getIOService();
	void resize(size_t new_size);
	void run();
	size_t size();
	void stop();

private:
	/** alias shared_ptr for asio::io_service */
	typedef boost::shared_ptr<boost::asio::io_service> IOServicePtr;

	/** alias shared_ptr for asio::io_service::work */
	typedef boost::shared_ptr<boost::asio::io_service::work> WorkPtr;

private:
	/** I/O service pool */
	List<IOServicePtr> io_services_;

	/** I/O service work */
	List<WorkPtr> works_;

	/** next I/O service to be used */
	size_t next_io_service_;
};

#endif /* _UNREALIRCD_IOSERVICEPOOL_HPP */
