/*****************************************************************
 * Unreal Internet Relay Chat Daemon, Version 4
 * File         resolver.cpp
 * Description  Asyncronous DNS resolver
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
#include "resolver.hpp"

/**
 * UnrealResolver constructor.
 */
UnrealResolver::UnrealResolver()
	: tcp::resolver(unreal->ios_pool.getIOService())
{ }

/**
 * Callback for asyncronous resolver query.
 *
 * @param err boost error_code
 * @param ep Resolver query iterator
 */
void UnrealResolver::handleResult(const ErrorCode& ec, Iterator ep_iter)
{
	onResolve(ec, ep_iter);
}

/**
 * Initiate an DNS query using an endpoint specification
 */
void UnrealResolver::query(tcp::endpoint& endpoint)
{
	async_resolve(endpoint,
		boost::bind(&UnrealResolver::handleResult,
			this,
			boost::asio::placeholders::error,
			boost::asio::placeholders::iterator));
}

/**
 * Initiate an DNS query w/ host/port.
 */
void UnrealResolver::query(const String& hostname, const uint16_t& port)
{
	Query query(hostname, String(port));

	async_resolve(query,
		boost::bind(&UnrealResolver::handleResult,
			this,
			boost::asio::placeholders::error,
			boost::asio::placeholders::iterator));
}
