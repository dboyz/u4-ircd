/*****************************************************************
 * Unreal Internet Relay Chat Daemon, Version 4
 * File         log.hpp
 * Description  An object facilitating logging
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

#ifndef _UNREALIRCD_LOG_HPP
#define _UNREALIRCD_LOG_HPP

#include <string.hpp>
#include <fstream>

class UnrealLog
{
public:
	enum LogLevel { Fatal, Error, Normal, Debug };

public:
	UnrealLog(const String& filename = String());
	~UnrealLog();

	void close();
	const String& fileName();
	bool open();
	void setFileName(const String& filename);
	void setLevel(LogLevel ll);
	void write(LogLevel level, const char* fmt, ...);

	UnrealLog& operator<<(const String& str);
	UnrealLog& operator<<(LogLevel ll);

private:
	String levelToStr(LogLevel ll);

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
