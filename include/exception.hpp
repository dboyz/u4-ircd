/*****************************************************************
 * Unreal Internet Relay Chat Daemon, Version 4
 * File         exception.hpp
 * Description  Generic exception interface
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

#ifndef _UNREALIRCD_EXCEPTION_HPP
#define _UNREALIRCD_EXCEPTION_HPP

#include <exception>
#include "string.hpp"

/**
 * Generic exception class.
 */
template<typename ErrorCodeType>
class UnrealException
	: public std::exception
{
public:
	UnrealException(const String& msg, ErrorCodeType c = OK)
		: message_(msg), code_(c)
	{}

	/**
	 * Returns the error code.
	 *
	 * @return Error code
	 */
	ErrorCodeType code()
	{
		return code_;
	}

	/**
	 * Returns the actual exception message.
	 *
	 * @return Exception message
	 */
	const char* what() const throw()
	{
		return message_.c_str();
	}

private:
	/** actual error message */
	String message_;

	/** error code number */
	ErrorCodeType code_;
};

#endif /* _UNREALIRCD_EXCEPTION_HPP */
