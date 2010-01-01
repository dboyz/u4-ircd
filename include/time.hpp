/*****************************************************************
 * Unreal Internet Relay Chat Daemon, Version 4
 * File         time.hpp
 * Description  Object for time storage and math
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

#ifndef _UNREALIRCD_TIME_HPP
#define _UNREALIRCD_TIME_HPP

#include "string.hpp"
#include <ctime>

/**
 * UnrealTime class.
 */
class UnrealTime
{
public:
	UnrealTime(const std::time_t& ts = 0);

	UnrealTime& addSeconds(const std::time_t& sec);
	static UnrealTime now();
	void setTS(const std::time_t& ts);
	void sync();
	std::time_t toTS();
	String toString(const String& fmt);

	inline bool operator<(const UnrealTime& other)
	{ return timestamp_ < const_cast<UnrealTime&>(other).toTS(); }

	inline bool operator>(const UnrealTime& other)
	{ return timestamp_ > const_cast<UnrealTime&>(other).toTS(); }

	inline bool operator==(const UnrealTime& other)
	{ return timestamp_ == const_cast<UnrealTime&>(other).toTS(); }

	inline bool operator!=(const UnrealTime& other)
	{ return timestamp_ != const_cast<UnrealTime&>(other).toTS(); }

	inline bool operator<=(const UnrealTime& other)
	{ return timestamp_ <= const_cast<UnrealTime&>(other).toTS(); }

	inline bool operator>=(const UnrealTime& other)
	{ return timestamp_ >= const_cast<UnrealTime&>(other).toTS(); }

private:
	std::time_t timestamp_;
};

#endif /* _UNREALIRCD_TIME_HPP */
