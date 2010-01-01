/*****************************************************************
 * Unreal Internet Relay Chat Daemon, Version 4
 * File         resolver.cpp
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

#include "base.hpp"
#include "resolver.hpp"

/**
 * UnrealResolver constructor.
 */
UnrealResolver::UnrealResolver()
{ }

/**
 * Initiate an DNS query w/ host/port.
 */
void UnrealResolver::query(const String& hostname, const uint16_t& port)
{

}
