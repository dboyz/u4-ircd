/*****************************************************************
 * Unreal Internet Relay Chat Daemon, Version 4
 * File         string.hpp
 * Description  A fatter string object
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

#ifndef _UNREALIRCD_STRING_HPP
#define _UNREALIRCD_STRING_HPP

#include "platform.hpp"
#include <string>

/** define the String::sprintf() buffer size */
#define SPRINTF_BUFFER_SIZE		4096

#define foreach_str(x, y) \
	foreach(String::Iterator, x, y)

class StringList;

/**
 * Improved standard string class.
 */
class String
	: public std::string
{
public:
	/** iterator */
	typedef std::string::iterator Iterator;
	
	/** const_iterator */
	typedef std::string::const_iterator ConstIterator;

public:
	String() {}
	String(const char* str);
	String(const std::string& str);
	String(double n);
	String(int8_t n);
	String(int16_t n);
	String(int32_t n);
	String(int64_t n);
	String(uint8_t n);
	String(uint16_t n);
	String(uint32_t n);
	String(uint64_t n);

	bool contains(const String& str) const;
	static String format(const char* fmt, ...);
	String left(size_t count);

	/**
 	 * Simple wildcard pattern matcher.
 	 *
 	 * @param pattern Pattern string to check against
 	 * @return True if matching, otherwise false
 	 */
	inline bool match(const String& pattern)
	{
		ConstIterator m = pattern.begin();
		ConstIterator n = begin();
		ConstIterator ma = end();
		ConstIterator na = begin();

		if (pattern.empty())
			return false;

		for (;;)
		{
			if (*m == '*')
			{
				while (*m == '*')
					++m;
		
				ma = m;
				na = n;
			}
		
			if (m == pattern.end())
			{
				if (n == end())
					return true;
				if (ma == end())
					return false;
			
				for (--m; (m > pattern.begin()) && (*m == '?'); --m);
			
				if (*m == '*')
					return true;
			
				m = ma;
				n = ++na;
			}
			else if (n == end())
			{
				while (*m == '*')
					++m;
			
				return (m == pattern.end());
			}
		
			if ((String::toLower(*m) != String::toLower(*n))
					&& !((*m == '_') && (*n == ' ')) && (*m != '?'))
			{
				if (ma == end())
					return false;

				m = ma;
				n = ++na;
			}
			else
			{
				++m;
				++n;
			}
		}
	
		return false;
	}

	String mid(size_t start, size_t count = npos);
	inline String& prepend(const char* str)
		{ this->insert(0, str); return *this; }
	inline String& prepend(const String& str)
		{ this->insert(0, str); return *this; }
	static String repeat(char ch, size_t count);
	String right(size_t count);
	StringList split(const String& separator);
	String& sprintf(const char* fmt, ...);
	bool toBool();
	char toChar();
	int32_t toInt();
	int8_t toInt8();
	int16_t toInt16();
	int32_t toInt32();
	int64_t toInt64();
	String toLower();

	/**
 	 * Convert an character to lowercase.
 	 *
 	 * @param ch Input character
 	 * @return Lowercase version of ch
 	 */
	inline static char toLower(char ch)
	{
		if (ch >= 'A' && ch <= 'Z')
			ch += 32;
		
		return ch;
	}

	size_t toSize();
	uint32_t toUInt();
	uint8_t toUInt8();
	uint16_t toUInt16();
	uint32_t toUInt32();
	uint64_t toUInt64();
	String toUpper();

	/**
 	 * Convert an character to uppercase.
 	 *
 	 * @param ch Input character
 	 * @return Uppercase version of ch
 	 */
	inline static char toUpper(char ch)
	{
		if (ch >= 'a' && ch <= 'z')
			ch -= 32;
		
		return ch;
	}

	String trimmed();

	String& operator<<(const String& str) { append(str); return *this; }
	String& operator<<(const std::string& str) { append(str); return *this; }
	String& operator<<(const char* str) { append(str); return *this; }
	String& operator<<(int8_t n) { String s(n); append(s); return *this; }
	String& operator<<(int16_t n) { String s(n); append(s); return *this; }
	String& operator<<(int32_t n) { String s(n); append(s); return *this; }
	String& operator<<(int64_t n) { String s(n); append(s); return *this; }
	String& operator<<(uint8_t n) { String s(n); append(s); return *this; }
	String& operator<<(uint16_t n) { String s(n); append(s); return *this; }
	String& operator<<(uint32_t n) { String s(n); append(s); return *this; }
	String& operator<<(uint64_t n) { String s(n); append(s); return *this; }
	String& operator>>(char& ch) { ch = toChar(); return *this; }

private:
	template<typename _Ty> _Ty toGenericType();
};

#endif /* _UNREALIRCD_STRING_HPP */
