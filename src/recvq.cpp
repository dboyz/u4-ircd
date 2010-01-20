/*****************************************************************
 * Unreal Internet Relay Chat Daemon, Version 4
 * File         recvq.cpp
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

#include <recvq.hpp>
#include <stringlist.hpp>

/**
 * Receive Queue constructor.
 */
UnrealRecvQueue::UnrealRecvQueue()
	: hard_limit_(static_cast<uint16_t>(RQL_HARD)),
	  soft_limit_(static_cast<uint16_t>(RQL_SOFT))
{ }

/**
 * Add a line to the queue.
 * Note: The line to be added must NOT contain LF, as LF is being used as
 * internal separator for the lines.
 *
 * @param str String to be added
 */
void UnrealRecvQueue::add(const String& str)
{
	if (!queue_str_.empty())
		queue_str_.append(1, '\n');

	queue_str_.append(str);
}

/**
 * Get a line from the queue.
 *
 * @return A line from queue, or an empty string if the queue is empty
 */
String UnrealRecvQueue::getline()
{
	if (queue_str_.empty())
		return queue_str_; /* queue is empty */
	else if (queue_str_.find('\n') == String::npos)
	{
		String result = queue_str_;
		queue_str_.clear();
		
		return result;
	}
	else
	{
		StringList sq = queue_str_.split("\n");
		String result = sq.takeFirst();

		queue_str_ = sq.join("\n");

		return result;
	}
}

/**
 * Returns the number of bytes in queue.
 *
 * @return Number of bytes in queue
 */
size_t UnrealRecvQueue::length()
{
	size_t result = 0;

	for (String::Iterator ch = queue_str_.begin(); ch != queue_str_.end(); ++ch)
	{
		if (*ch != '\n')
			result++;
	}

	return result;
}

/**
 * Returns the queue length limit.
 *
 * @param type Specifies whether you want the hard- or soft limit.
 * @return Limit in bytes
 */
const uint16_t& UnrealRecvQueue::limit(Type type)
{
	if (type == RQL_HARD)
		return hard_limit_;
	else
		return soft_limit_;
}

/**
 * Update the queue length limit.
 *
 * @param type Limit type
 * @param lim New limit
 */
void UnrealRecvQueue::setLimit(Type type, const uint16_t& lim)
{
	if (type == RQL_HARD)
		hard_limit_ = lim;
	else
		soft_limit_ = lim;
}

/**
 * Returns the number of lines in queue.
 *
 * @return Number of lines in queue
 */
size_t UnrealRecvQueue::size()
{
	if (queue_str_.empty())
		return 0;
	else if (queue_str_.find('\n') == String::npos)
		return 1;
	else
	{
		StringList sq = queue_str_.split("\n");
		
		return sq.size();
	}
}
