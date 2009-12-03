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

#ifndef _UNREALIRCD_STRING_HPP
#define _UNREALIRCD_STRING_HPP

#include "platform.hpp"
#include <string>

/** define the String::sprintf() buffer size */
#define SPRINTF_BUFFER_SIZE		4096

class StringList;

/**
 * Improved standard string class.
 */
class String
	: public std::string
{
public:
	/** Alias the Iterator */
	typedef std::string::iterator Iterator;

public:
	String() {}
	String(const char* str);
	String(const std::string& str);
	String(double n);
	String(int8 n);
	String(int16 n);
	String(int32 n);
	String(int64 n);
	String(uint8 n);
	String(uint16 n);
	String(uint32 n);
	String(uint64 n);

	bool contains(const String& str) const;
	String left(size_t count);
	String mid(size_t start, size_t count = npos);
	static String repeat(char ch, size_t count);
	String right(size_t count);
	StringList split(const String& separator);
	String& sprintf(const char* fmt, ...);
	bool toBool();
	char toChar();
	int32 toInt();
	int8 toInt8();
	int16 toInt16();
	int32 toInt32();
	int64 toInt64();
	String toLower();
	uint32 toUInt();
	uint8 toUInt8();
	uint16 toUInt16();
	uint32 toUInt32();
	uint64 toUInt64();
	String toUpper();
	String trimmed();

	String& operator<<(const String& str) { append(str); return *this; }
	String& operator<<(const std::string& str) { append(str); return *this; }
	String& operator<<(const char* str) { append(str); return *this; }
	String& operator<<(int8 n) { String s(n); append(s); return *this; }
	String& operator<<(int16 n) { String s(n); append(s); return *this; }
	String& operator<<(int32 n) { String s(n); append(s); return *this; }
	String& operator<<(int64 n) { String s(n); append(s); return *this; }
	String& operator<<(uint8 n) { String s(n); append(s); return *this; }
	String& operator<<(uint16 n) { String s(n); append(s); return *this; }
	String& operator<<(uint32 n) { String s(n); append(s); return *this; }
	String& operator<<(uint64 n) { String s(n); append(s); return *this; }
	String& operator>>(char& ch) { ch = toChar(); return *this; }

private:
	template<typename _Ty> _Ty toGenericType();
};

#endif /* _UNREALIRCD_STRING_HPP */
