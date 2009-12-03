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
 * Returns the log file name
 *
 * @return Log file name
 */
const String& UnrealLog::fileName()
{
	return filename_;
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

	if (level > level_)
		return;

	va_start(vl, fmt);
	vsnprintf(buffer, sizeof(buffer), fmt, vl);
	va_end(vl);

	// time string
	Time now = Time::now();
	String timeFmt = now.toString("%Y-%m-%dT%H:%M:%S %Z");

	/* print to file if opened */
	if (stream_.good())
	{
		stream_ << timeFmt
				<< "  "
				<< buffer
				<< std::endl;
	}

	/* just print to stdout if not deamonized */
	if (unreal->fstate() != UnrealBase::Daemonized)
	{
		std::cout << timeFmt
				  << "  "
				  << buffer
				  << std::endl;
	}
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
