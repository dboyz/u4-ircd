/*****************************************************************
 * Unreal Internet Relay Chat Daemon, Version 4
 * File         log.cpp
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

#include "base.hpp"
#include "log.hpp"
#include "time.hpp"
#include <cstdarg>
#include <iostream>

/**
 * UnrealLog constructor.
 * If `filename' has been specified, the log file is opened automatically.
 *
 * @param filename Initial log file name
 */
UnrealLog::UnrealLog(const String& filename)
	: filename_(filename), level_(Normal), last_level_flag_(Normal)
{
	if (!filename_.empty())
		open();
}

/**
 * UnrealLog destructor.
 */
UnrealLog::~UnrealLog()
{
	if (stream_.is_open())
	{
		stream_.close();
	}
}

/**
 * Close log file.
 */
void UnrealLog::close()
{
	stream_.close();
}

/**
 * Returns the log file name
 *
 * @return Log file name
 */
const String& UnrealLog::fileName()
{
	return filename_;
}

/**
 * Return a readable version of the loglevel.
 *
 * @param ll Loglevel
 * @return Readable string
 */
String UnrealLog::levelToStr(LogLevel ll)
{
	switch (ll)
	{
		case Fatal:
			return "FATAL";
		case Error:
			return "ERROR";
		case Normal:
			return "";
		case Debug:
			return "***";
		default:
			return "";
	}
}

/**
 * Open the log file.
 *
 * @return true if the file is opened successfully, otherwise false
 */
bool UnrealLog::open()
{
	stream_.open(filename_.c_str(), std::ios_base::out | std::ios_base::app);
	return stream_.good();
}

/**
 * Set the file name.
 *
 * @param filename New filename
 */
void UnrealLog::setFileName(const String& filename)
{
	filename_ = filename;
}

/**
 * Set the logging level.
 *
 * @param ll LogLevel
 */
void UnrealLog::setLevel(LogLevel ll)
{
	level_ = ll;
}

/**
 * Write a line to log file.
 */
void UnrealLog::write(LogLevel level, const char* fmt, ...)
{
	static char buffer[2048];
	va_list vl;

	/* always print error/fatal messages */
	if (level > level_ && (level != Error && level != Fatal))
		return;

	va_start(vl, fmt);
	vsnprintf(buffer, sizeof(buffer), fmt, vl);
	va_end(vl);

	// time string
	UnrealTime now = UnrealTime::now();
	String timeFmt = now.toString("%Y-%m-%dT%H:%M:%S %Z");

	/* print to file if opened */
	if (stream_.good())
	{
		stream_ << timeFmt
				<< " "
				<< levelToStr(level)
				<< " "
				<< buffer
				<< std::endl;
	}

	/* just print to stdout if not deamonized */
	if (unreal->fstate() != UnrealBase::Daemonized)
	{
		std::cout << timeFmt
				  << " "
				  << levelToStr(level)
				  << " "
				  << buffer
				  << std::endl;
	}

	/* Fatal errors always have to terminate the program execution */
	if (level == Fatal)
		unreal->exit(1);
}

/**
 * Stream operator for writing data into the log file.
 *
 * @param str String
 * @return Reference to Log object
 */
UnrealLog& UnrealLog::operator<<(const String& str)
{
	write(last_level_flag_, str.c_str());
	return *this;
}

/**
 * Stream operator to define level prefix for writing data to the log
 * file using the stream operator above.
 *
 * @param ll Log level
 * @return Reference to Log object
 */
UnrealLog& UnrealLog::operator<<(LogLevel ll)
{
	last_level_flag_ = ll;
	return *this;
}
