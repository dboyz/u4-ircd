/*****************************************************************
 * Unreal Internet Relay Chat Daemon, Version 4
 * File         windows.hpp
 * Description  Windows(R) specific helper functions
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

#ifndef _UNREALIRCD_WINDOWS_HPP
#define _UNREALIRCD_WINDOWS_HPP

/**
 * POSIX strerror() emulation for Windows.
 *
 * @param Error code as returned by GetLastError()
 * @return Pointer to error message buffer
 */
const char* strerror_w(DWORD ec)
{
	LPVOID msgbuf;
	DWORD ret;

	ret = FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER |
			FORMAT_MESSAGE_FROM_SYSTEM |
			FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			ec,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPTSTR)&msgbuf,
			0,
			NULL);

	if (ret == 0)
		return NULL;
	else
	{
		static String message;
		message.sprintf("%s (errcode: %d)", (char*)msgbuf, ec);

		// Free some memory
		LocalFree(msgbuf);

		return message.c_str();
	}
}

#endif /* WINDOWS_HPP_ */
