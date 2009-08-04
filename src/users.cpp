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

User::User()
{ }

User::~User()
{ }

void User::Quit(const std::string& reason)
{
	if (reason.c_str() == NULL)
	{
		this->SendRaw(":%s!%s@%s QUIT", this->nick.c_str(), this->ident.c_str(), this->host.c_str());
		MODULARIZE_FUNCTION(I_OnUserQuit, OnUserQuit());

		/* TODO: Add QUIT propagations to channels and such.
		*  -- David Kingston
		*/
	}
	else
	{
		this->SendRaw(":%s!%s@%s QUIT :%s", this->nick.c_str(), this->ident.c_str(), this->host.c_str(), reason.c_str());
		MODULARIZE_FUNCTION(I_OnUserQuit, OnUserQuit(reason.c_str());
	}
}

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
			this->SendRaw(ERR_ERRONEUSNICKNAME, conf->ServerName.c_str(), newnick.c_str());
			MODULARIZE_FUNCTION(I_OnBadNickChange, OnBadNickChange(newnick.c_str());
			return -1;
		}
		else
		{
			this->SendRaw(":%s!%s@%s NICK :%s", this->nick.c_str(), this->ident.c_str(), this->host.c_str(), newnick.c_str());
			MODULARIZE_FUNCTION(I_OnNickChange, OnNickChange(newnick.c_str());

			/* TODO: Add the NICK propagations (nick changes to channels, etc)
			*  --David Kingston
			*/
		}
	}
	return 0;
}

/*
 * Is this depreciated with all the numerics defined as string constants?
 * It also doesn't appear to have the correct numeric format...
 * Correct numeric format is :<server> <numeric> <target> <other params|:text>
 * - Stealth
 *
 * It can be modified to send the proper format. -- David Kingston
 */
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
		MODULARIZE_FUNCTION(I_OnSendNumeric, OnSendNumeric(numeric, buf));
		return 0;
	}
}

bool User::isValidNick(const std::string& nick)
{
	MODULARIZE_FUNCTION(I_OnValidNickCheck, OnValidNickCheck(nick.c_str());
	return true;
}

int User::SendRaw(const char* text, ...)
{
	va_list args;
	static char buf[BUFSIZE];
	va_start(args, text);
	vsnprintf(buf, sizeof(buf), text, args);
	va_end(args);

	MODULARIZE_FUNCTION(I_OnSendRaw, OnSendRaw(buf));

	return 0;
}

void User::SendMOTD(void)
{
	if (!conf->MOTDFile.size())
	{
		this->SendRaw(ERR_NOMOTD, conf->ServerName.c_str(), this->nick.cstr());
		MODULARIZE_FUNCTION(I_OnFailedMOTD, OnFailedMOTD());
		return;
	}
	else
	{
		this->SendRaw(RPL_MOTDSTART, conf->ServerName.c_str(), this->nick.c_str());
		this->SendRaw(RPL_MOTD, conf->ServerName.c_str(), this->nick.c_str(). __DATE__, __TIME__);

		for (files::iterator i = conf->MOTDFile.begin(); i != conf->MOTDFile.end(); i++)
		{
			this->SendRaw(RPL_MOTD, conf->ServerName.c_str(), this->nick.c_str(), i->c_str());
			this->SendRaw(RPL_ENDOFMOTD, conf->ServerName.c_str(), this->nick.c_str());

			MODULARIZE_FUNCTION(I_OnMOTDSend, OnMOTDSend());
		}
	}
	return;
}

/* CIA test. */
