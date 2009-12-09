/*****************************************************************
 * Unreal Internet Relay Chat Daemon, Version 4
 * File         limits.hpp
 * Description  Default limitations for various things
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

#ifndef _UNREALIRCD_LIMITS_HPP
#define _UNREALIRCD_LIMITS_HPP

#include "base.hpp"

/** nick length limit */
#define _U4_NICKLEN_	unreal->config.get("limits/nicklen", 18).toUInt()

/** user (ident) length limit */
#define _U4_USERLEN_	unreal->config.get("limits/userlen", 10).toUInt()

/** topic length limit */
#define _U4_TOPICLEN_	unreal->config.get("limits/topiclen", 250).toUInt()

/** away message length limit */
#define _U4_AWAYLEN_	unreal->config.get("limits/awaylen", 250).toUInt()

/** kick message length limit */
#define _U4_KICKLEN_	unreal->config.get("limits/kicklen", 250).toUInt()

/** channel name length limit */
#define _U4_CHANNELLEN_ unreal->config.get("limits/channellen", 200).toUInt()

#endif /* _UNREALIRCD_LIMITS_HPP */
