/*****************************************************************
 * Unreal Internet Relay Chat Daemon, Version 4
 * File         time.cpp
 * Description  Object for time storage and math
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

#include "time.hpp"

/**
 * Time constructor.
 *
 * @param ts If set, the timestamp is assigned to the internal timestamp.
 * If ts is not specified, the internal timestamp is set to zero.
 */
Time::Time(const std::time_t& ts)
	: timestamp_(ts)
{ }

/**
 * Return a Time object with the current time.
 *
 * @return Time object
 */
Time Time::now()
{
	Time t;
	t.setTS(std::time(0));
	return t;
}

/**
 * Update the internal timestamp.
 *
 * @param ts New timestamp
 */
void Time::setTS(const std::time_t& ts)
{
	timestamp_ = ts;
}

/**
 * Return a formatted time string.
 * The format for `fmt' is described in the manpage for strftime().
 *
 * @param fmt Format according to strftime()
 * @return Formatted string
 */
String Time::toString(const String& fmt)
{
	static char buf[1024];
	struct tm* tmx = localtime(&timestamp_);

	if (std::strftime(buf, sizeof(buf), fmt.c_str(), tmx) == 0)
		return String("Invalid");
	else
		return String(buf);
}

/**
 * Return the time as time_t.
 *
 * @return Timestamp time_t
 */
std::time_t Time::toTS()
{
	return timestamp_;
}
