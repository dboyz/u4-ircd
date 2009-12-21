/*****************************************************************
 * Unreal Internet Relay Chat Daemon, Version 4
 * File         reactorpool.hpp
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

#ifndef _UNREALIRCD_REACTORPOOL_HPP
#define _UNREALIRCD_REACTORPOOL_HPP

#include "list.hpp"
#include "reactor.hpp"
/*
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
*/

/** default amount of threads */
#define REACTORPOOL_DEFAULT_THREADS	1

/** default amount of IO services */
#define REACTORPOOL_DEFAULT_REASIZE	1

/**
 * The UnrealReactorPool class provides an event reactor pool that dispenses
 * operations on Sockets, Timers, etc. to multiple threads and/or reactors.
 */
class UnrealReactorPool
{
public:
	UnrealReactorPool(size_t nthreads = REACTORPOOL_DEFAULT_THREADS,
			size_t nr = REACTORPOOL_DEFAULT_REASIZE);

	UnrealReactor& getReactor();
	void resize(size_t nthreads, size_t nr);
	void run();
	size_t size();
	void stop();
	size_t threads();

private:
	/** reactor pool */
	List<UnrealReactor> reactors_;

	/** threads size */
	size_t threads_size_;

	/** next reactor to be used */
	size_t next_reactor_;
};

#endif /* _UNREALIRCD_REACTORPOOL_HPP */
