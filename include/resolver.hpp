/*****************************************************************
 * Unreal Internet Relay Chat Daemon, Version 4
 * File         resolver.hpp
 * Description  Asyncronous DNS resolver
 *
 * Copyright(C) 2009, 2010
 * The UnrealIRCd development team and contributors
 * http://www.unrealircd.com
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

#ifndef _UNREALIRCD_RESOLVER_HPP
#define _UNREALIRCD_RESOLVER_HPP

#include <map.hpp>
#include <reactor.hpp>
#include <string.hpp>

#include <boost/asio.hpp>
#include <boost/signal.hpp>

using namespace boost::asio::ip;

/**
 * Resolver class.
 */
class UnrealResolver
	: public tcp::resolver
{
public:
	/** error code type */
	typedef boost::system::error_code ErrorCode;
	
	/** result iterator */
	typedef tcp::resolver::iterator Iterator;
	
	/** endpoint type */
	typedef tcp::resolver::endpoint_type Endpoint;
	
	/** query type */
	typedef tcp::resolver::query Query;

public:
	UnrealResolver();
	void query(Endpoint& ep);
	void query(const String& hostname, const uint16_t& port);

public:
	boost::signal<void(const ErrorCode&, Iterator)> onResolve;

private:
	void handleResult(const ErrorCode& ec, Iterator ep_iter);
};

#endif /* _UNREALIRCD_RESOLVER_HPP */
