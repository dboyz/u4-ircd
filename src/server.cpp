/*****************************************************************
 * Unreal Internet Relay Chat Daemon, Version 4
 * File         server.cpp
 * Description  Server representation
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
#include "server.hpp"

/**
 * UnrealServer constructor.
 *
 * @param sname Server name
 */
UnrealServer::UnrealServer(const String& sname)
	: name_(sname), numeric_(0), uplink_(0)
{ }

/**
 * Returns the server boot timestamp.
 */
UnrealTime UnrealServer::bootTime()
{
	return boot_time_;
}

/**
 * Returns the server link timestamp.
 */
UnrealTime UnrealServer::linkTime()
{
	return link_time_;
}

/**
 * Returns the server name.
 */
const String& UnrealServer::name()
{
	return name_;
}

/**
 * Returns the server numeric.
 */
const uint32_t& UnrealServer::numeric()
{
	return numeric_;
}

/**
 * Returns the server uplink server.
 */
UnrealServer* UnrealServer::uplink()
{
	return uplink_;
}

/**
 * Update the server boot timestamp.
 *
 * @param ts New boot timestamp
 */
void UnrealServer::setBootTime(const UnrealTime& ts)
{
	boot_time_ = ts;
}

/**
 * Update the server link timestamp.
 *
 * @param ts New link timestamp
 */
void UnrealServer::setLinkTime(const UnrealTime& ts)
{
	link_time_ = ts;
}

/**
 * Update the server name.
 *
 * @param sname New server name
 */
void UnrealServer::setName(const String& sname)
{
	name_ = sname;
}

/**
 * Update the server numeric.
 *
 * @param num New server numeric
 */
void UnrealServer::setNumeric(const uint32_t& num)
{
	numeric_ = num;
}

/**
 * Update the server's uplink server.
 *
 * @param ulptr Pointer to uplink server entry
 */
void UnrealServer::setUplink(UnrealServer* ulptr)
{
	uplink_ = ulptr;
}
