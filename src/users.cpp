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
#include "modules.h"
#include <iostream>
#include <cstdarg>
#include <cstdio>

User::User()
{ }

User::~User()
{ }

int User::Quit(const std::string& reason)
{
	if (reason.empty())
	{
		return -1;
	}
	this->SendRaw(":%s!%s@%s QUIT :%s", this->nick.c_str(), this->ident.c_str(), this->host.c_str(), reason.c_str());
	MODULARIZE_FUNCTION(I_OnUserQuit, OnUserQuit(reason.c_str());
}

int User::ChangeNick(const std::string& newnick)
{
	if (newnick.empty())
	{
		return -1;
	}

	if (!this->isValidNick(newnick))
	{
		this->SendRaw(ERR_ERRONEUSNICKNAME, conf->ServerName.c_str(), newnick.c_str());
		MODULARIZE_FUNCTION(I_OnBadNickChange, OnBadNickChange(newnick.c_str());
		return -1;
	}
	this->SendRaw(":%s!%s@%s NICK :%s", this->nick.c_str(), this->ident.c_str(), this->host.c_str(), newnick.c_str());
	MODULARIZE_FUNCTION(I_OnNickChange, OnNickChange(newnick.c_str());

	/* TODO: Add the NICK propagations (nick changes to channels, etc)
	*  --David Kingston
	*/
	return 0;
}


bool User::isValidNick(const std::string& nick)
{
	if (nick.empty())
	{
		return false;
	}
	MODULARIZE_FUNCTION(I_OnValidNickCheck, OnValidNickCheck(nick.c_str());
	return true;
}

int User::SendRaw(const std::string& text, ...)
{
	if (text.empty())
	{
		return -1;
	}
	va_list args;
	static char buf[BUFSIZE];
	size_t desired_length;

	va_start(args, text);
	desired_length = vsnprintf(buf, BUFSIZE - 1, text.c_str(), args);
	va_end(args);

	/*
	 * desired_length excludes the NULL terminator that snprintf places at the
	 * end of the text it copies.
	 */
	if(desired_length > BUFSIZE - 2)
	{
		std::cerr << __FILE__ << ":" << __LINE__ << "Sending truncated message" << std::endl;
		desired_length = BUFSIZE - 2;
	}


	/*
	 * The last two bytes of buf that are sent will be '\0' and then an uninitialized char.
	 * We reset it to "\r\n". We also place a NULL after "\r\n"
	 */
	buf[desired_length] = '\r';
	buf[desired_length + 1] = '\n';

	/*
	 * now desired_length refers to the number of bytes to send to
	 * the client.
	 */
	desired_length += 2;
	/*
	 * This hook may post-process the message. Currently, it is limited to
	 * editing buf[0 through BUFSIZE]. It must edit desired_length and deal
	 * with "\n\r" if it changes the length of the message.
	 */
	MODULARIZE_FUNCTION(I_OnPreSendRaw, OnPreSendRaw(buf, &desired_length));
	/*
	 * This hook must actually send the message (without
	 * sending the NULL terminator). (NULL terminator allows the size of
	 * buf to be determined).
	 */
	MODULARIZE_FUNCTION(I_OnSendRaw, OnSendRaw(buf, desired_length));

	return 0;
}

int User::SendMOTD(void)
{
	if (!conf->MOTDFile.size())
	{
		this->SendRaw(ERR_NOMOTD, conf->ServerName.c_str(), this->nick.c_str());
		MODULARIZE_FUNCTION(I_OnFailedMOTD, OnFailedMOTD());
		return -1;
	}
	this->SendRaw(RPL_MOTDSTART, conf->ServerName.c_str(), this->nick.c_str());
	// We need to fix the DATE and TIME in this. -- Stealth
	this->SendRaw(RPL_MOTD, conf->ServerName.c_str(), this->nick.c_str(), __DATE__, __TIME__);

	for (files::const_iterator i = conf->MOTDFile.begin(); i != conf->MOTDFile.end(); i++)
	{
		this->SendRaw(RPL_MOTD, conf->ServerName.c_str(), this->nick.c_str(), i->c_str());
		this->SendRaw(RPL_ENDOFMOTD, conf->ServerName.c_str(), this->nick.c_str());
		MODULARIZE_FUNCTION(I_OnMOTDSend, OnMOTDSend());
	}
	return 0;
}

void User::SendAdminInfo(void)
{
	MODULARIZE_FUNCTION(I_OnPreSendAdminInfo, OnPreSendAdminInfo());

	this->SendRaw(RPL_ADMINME, conf->ServerName.c_str(), this->nick.c_str(), conf->ServerName.c_str());
	this->SendRaw(RPL_ADMINNAME, conf->ServerName.c_str(), this->nick.c_str(), conf->AdminName.c_str());
	this->SendRaw(RPL_ADMINNICK, conf->ServerName.c_str(), this->nick.c_str(), conf->AdminNick.c_str());
	this->SendRaw(RPL_ADMINEMAIL, conf->ServerName.c_str(), this->nick.c_str(), conf->AdminEmail.c_str());

	MODULARIZE_FUNCTION(I_OnSendAdminInfo, OnSendAdminInfo());

	return;
}
