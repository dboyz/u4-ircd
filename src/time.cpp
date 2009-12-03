/*
 *  UnrealIRCd, the next generation IRCd.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

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
