/*  UnrealIRCd -- The Next Generation IRCd.
*
*   This program is free software; you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation; either version 1, or (at your option)
*   any later version.
*
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with this program; if not, write to the Free Software
*   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#include "user.h"
#include "numerics.h"
#include "internal.h"
#include <iostream>
#include <cstdarg>
#include <cstdio>

void User::Quit(const std::string& reason)
{ }

int User::ChangeNick(const std::string& newnick)
{
	if (newnick.c_str() == NULL)
	{
		return -1;
	}
	else
	{
		if (!this->isValidNick(newnick.c_str()))
		{
			this->SendNumeric(ERR_ERRONEUSNICKNAME, "%s :Erroneous nickname", newnick.c_str());
			return -1;
		}
		else
		{
			this->SendRaw(":%s!%s@%s NICK :%s", this->nick.c_str(), this->ident.c_str(), this->host.c_str(), newnick.c_str());

			/* TODO: Add the NICK propagations (nick changes to channels, etc)
			*  --David Kingston
			*/
		}
	}
	return 0;
}

int User::SendNumeric(int numeric, const char* text, ...)
{

	if (text == NULL || !numeric)
	{
		return -1;
	}
	else
	{
		va_list args;
		static char buf[BUFSIZE];
		va_start(args, text);
		vsnprintf(buf, sizeof(buf), text, args);
		va_end(args);

		this->SendRaw(":%s!%s@%s %d %s :%s", this->nick.c_str(), this->ident.c_str(), this->host.c_str(), numeric, this->nick.c_str(), buf);
		return 0;
	}
}

bool User::isValidNick(const std::string& nick)
{
	return true;
}

int User::SendRaw(const char* text, ...)
{
	va_list args;
	static char buf[BUFSIZE];
	va_start(args, text);
	vsnprintf(buf, sizeof(buf), text, args);
	va_end(args);

	return 0;
}
