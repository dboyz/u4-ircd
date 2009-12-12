/*****************************************************************
 * Unreal Internet Relay Chat Daemon, Version 4
 * File         resolver.hpp
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

#ifndef _UNREALIRCD_RESOLVER_HPP
#define _UNREALIRCD_RESOLVER_HPP

#include "map.hpp"
#include "string.hpp"
#include <boost/asio.hpp>
#include <boost/signals2.hpp>

using namespace boost::asio::ip;

class UnrealResolver
	: public tcp::resolver
{
public:
	/** alias errorcode class */
	typedef boost::system::error_code ErrorCode;

	/** alias the resolver iterator */
	typedef tcp::resolver::iterator Iterator;

	/** alias the query type */
	typedef tcp::resolver::query Query;

public:
	UnrealResolver();
	void query(tcp::endpoint& endpoint);
	void query(const String& hostname, const uint16_t& port);

public:
	/** emitted when we got an result, or an error occured */
	boost::signals2::signal<void(const ErrorCode&, Iterator)> onResolve;

private:
	void handleResult(const ErrorCode& ec, Iterator ep_iter);
};

#endif /* _UNREALIRCD_RESOLVER_HPP */
