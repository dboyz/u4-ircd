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

#ifndef _UNREALIRCD_LOG_HPP
#define _UNREALIRCD_LOG_HPP

#include "string.hpp"
#include <fstream>

class UnrealLog
{
public:
	enum LogLevel { Normal, Debug };

public:
	UnrealLog(const String& filename = String());
	~UnrealLog();

	const String& fileName();
	bool open();
	void setFileName(const String& filename);
	void setLevel(LogLevel ll);
	void write(LogLevel level, const char* fmt, ...);

	UnrealLog& operator<<(const String& str);
	UnrealLog& operator<<(LogLevel ll);

private:
	/** log file name */
	String filename_;

	/** file stream */
	std::ofstream stream_;

	/** log level */
	LogLevel level_;

	/** last level flag */
	LogLevel last_level_flag_;
};

#endif /* _UNREALIRCD_LOG_HPP */
