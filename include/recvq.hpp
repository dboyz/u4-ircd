/*****************************************************************
 * Unreal Internet Relay Chat Daemon, Version 4
 * File         recvq.hpp
 * Description  Incoming data queue
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

#ifndef _UNREALIRCD_RECVQ_HPP
#define _UNREALIRCD_RECVQ_HPP

#include <platform.hpp>
#include <string.hpp>

#define RQL_HARD_SIZE	1024
#define RQL_SOFT_SIZE	512

/**
 * Receive queue for user connections.
 */
class UnrealRecvQueue
{
public:
	enum Type { RQL_HARD = RQL_HARD_SIZE, RQL_SOFT = RQL_SOFT_SIZE };

public:
	UnrealRecvQueue();
	void add(const String& str);
	String getline();
	size_t length();
	const uint16_t& limit(Type type);
	void setLimit(Type type, const uint16_t& lim);
	size_t size();

private:
	/** actual queue contents */
	String queue_str_;

	/** hard limit, bytes */
	uint16_t hard_limit_;

	/** soft limit, bytes */
	uint16_t soft_limit_;
};

#endif /* _UNREALIRCD_RECVQ_HPP */
