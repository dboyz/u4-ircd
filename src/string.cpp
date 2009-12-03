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

#include "string.hpp"
#include "stringlist.hpp"
#include <cstdarg>
#include <sstream>

/**
 * Construct a string, copying the contents of a zero-terminated char pointer.
 *
 * @param str Pointer to string to copy
 */
String::String(const char* str)
{
	assign(str);
}

/**
 * Construct a string, copying the contents of a std::string
 *
 * @param str String to copy
 */
String::String(const std::string& str)
{
	assign(str);
}

/**
 * Construct a string, inserting a floating-point number as string.
 *
 * @param n Number to insert
 */
String::String(double n)
{
	std::stringstream ss;
	ss << n;
	assign(ss.str());
}

/**
 * Construct a string, inserting a 8bit integer as string.
 *
 * @param n Number to insert
 */
String::String(int8 n)
{
	std::stringstream ss;
	ss << n;
	assign(ss.str());
}

/**
 * Construct a string, inserting a 16bit integer as string.
 *
 * @param n Number to insert
 */
String::String(int16 n)
{
	std::stringstream ss;
	ss << n;
	assign(ss.str());
}

/**
 * Construct a string, inserting a 32bit integer as string.
 *
 * @param n Number to insert
 */
String::String(int32 n)
{
	std::stringstream ss;
	ss << n;
	assign(ss.str());
}

/**
 * Construct a string, inserting a 64bit integer as string.
 *
 * @param n Number to insert
 */
String::String(int64 n)
{
	std::stringstream ss;
	ss << n;
	assign(ss.str());
}

/**
 * Construct a string, inserting a 8bit unsigned integer as string.
 *
 * @param n Number to insert
 */
String::String(uint8 n)
{
	std::stringstream ss;
	ss << n;
	assign(ss.str());
}

/**
 * Construct a string, inserting a 16bit unsigned integer as string.
 *
 * @param n Number to insert
 */
String::String(uint16 n)
{
	std::stringstream ss;
	ss << n;
	assign(ss.str());
}

/**
 * Construct a string, inserting a 32bit unsigned integer as string.
 *
 * @param n Number to insert
 */
String::String(uint32 n)
{
	std::stringstream ss;
	ss << n;
	assign(ss.str());
}

/**
 * Construct a string, inserting a 64bit unsigned integer as string.
 *
 * @param n Number to insert
 */
String::String(uint64 n)
{
	std::stringstream ss;
	ss << n;
	assign(ss.str());
}

/**
 * Returns whether the string contains `str'.
 *
 * @param str Substring to search for
 * @return true when found, false when not
 */
bool String::contains(const String& str) const
{
	return this->find(str) != npos;
}

/**
 * Returns `count' characters, start counting from the left side.
 *
 * @param count Number of characters to return
 */
String String::left(size_t count)
{
	return this->substr(0, count);
}

/**
 * Returns `count' characters, start counting from `start'.
 *
 * @param start Position of first character to return
 * @param count Number of characters to return
 */
String String::mid(size_t start, size_t count)
{
	return this->substr(start, count);
}

/**
 * Repeat `ch' for `count' times and return it as string.
 *
 * @param ch Character to repeat
 * @param count Count
 */
String String::repeat(char ch, size_t count)
{
	String result;

	result.append(count, ch);

	return result;
}

/**
 * Returns `count' characters, start counting from the right side.
 *
 * @param count Number of characters to return
 */
String String::right(size_t count)
{
	return this->substr(this->size() - count - 1, count);
}

/**
 * Split the string into pieces which are separated by an specified separator.
 *
 * @param separator Separator to use for splitting
 * @return stringlist with the splitted pieces
 */
StringList String::split(const String& separator)
{
	StringList sl;
	String tmp;
	size_t pos = 0, last = 0;

	do
	{
		tmp = mid(pos);

		if ((last = tmp.find(separator)) != npos)
		{
			sl << tmp.left(last);
			pos += last + 1;
		}
		else
		{
			if (sl.size() > 0)
				sl << tmp;

			break;
		}
	} while (true);

	return sl;
}

/**
 * Format string.
 *
 * @param fmt printf-style string format
 * @return Reference to actual string.
 */
String& String::sprintf(const char* fmt, ...)
{
	static char buffer[SPRINTF_BUFFER_SIZE];
	va_list vl;

	va_start(vl, fmt);
	vsnprintf(buffer, SPRINTF_BUFFER_SIZE, fmt, vl);
	va_end(vl);

	assign(buffer);

	return *this;
}

/**
 * Convert string to an boolean value.
 */
bool String::toBool()
{
	if (contains("true") || contains("t") || contains("yes") || contains("y")
			|| contains("1"))
		return true;

	return false;
}

/**
 * Convert string to character.
 */
char String::toChar()
{
	if (this->empty())
		return 0;
	else
		return this->c_str()[0];
}

/**
 * Provides a generic way to convert the string into target formats
 * that can be convered using std::stringstream.
 *
 * @return The converted value in the templated type
 */
template<typename _Ty>
_Ty String::toGenericType()
{
	_Ty val;
	std::stringstream ss(c_str());

	ss >> val;

	return val;
}

/**
 * Convert string to 32bit integer.
 */
int32 String::toInt()
{
	return toGenericType<int32>();
}

/**
 * Convert string to 8bit integer.
 */
int8 String::toInt8()
{
	return toGenericType<int8>();
}

/**
 * Convert string to 16bit integer.
 */
int16 String::toInt16()
{
	return toGenericType<int16>();
}

/**
 * Convert string to 32bit integer.
 */
int32 String::toInt32()
{
	return toGenericType<int32>();
}

/**
 * Convert string to 64bit integer.
 */
int64 String::toInt64()
{
	return toGenericType<int64>();
}

/**
 * Convert all occurences of uppercase letters to lowercase letters.
 *
 * @return Lowercase version of the string
 */
String String::toLower()
{
	String buf(c_str());

	for (char* s = (char*)buf.c_str(); *s; s++)
		if (*s >= 'A' && *s <= 'Z')
			*s = *s + 32;

	return buf;
}

/**
 * Convert string to 32bit unsigned integer.
 */
uint32 String::toUInt()
{
	return toGenericType<uint32>();
}

/**
 * Convert string to 8bit unsigned integer.
 */
uint8 String::toUInt8()
{
	return toGenericType<uint8>();
}

/**
 * Convert string to 16bit unsigned integer.
 */
uint16 String::toUInt16()
{
	return toGenericType<uint16>();
}

/**
 * Convert string to 32bit unsigned integer.
 */
uint32 String::toUInt32()
{
	return toGenericType<uint32>();
}

/**
 * Convert string to 64bit unsigned integer.
 */
uint64 String::toUInt64()
{
	return toGenericType<uint64>();
}

/**
 * Convert all occurences of lowercase letters to uppercase letters.
 *
 * @return Uppercase version of the string
 */
String String::toUpper()
{
	String buf(c_str());

	for (char* s = (char*)buf.c_str(); *s; s++)
		if (*s >= 'a' && *s <= 'z')
			*s = *s - 32;

	return buf;
}

/**
 * Clean whitespace from the string, including CRLF, TABs and trailing spaces.
 *
 * @return Clean version of the string
 */
String String::trimmed()
{
	String result(c_str());

	size_t s = result.find_first_not_of(" \n\r\t");
	if (s != npos)
		result.erase(0, s);
	else
		result.erase(result.begin(), result.end());

	size_t e = result.find_last_not_of(" \n\r\t");
	if (e != npos)
		result.erase(e + 1);
	else
		result.erase(result.begin(), result.end());

	return result;
}
