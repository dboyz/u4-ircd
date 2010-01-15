/*****************************************************************
 * Unreal Internet Relay Chat Daemon, Version 4
 * File         numeric.hpp
 * Description  IRC protocol numerics
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

#ifndef _UNREALIRCD_NUMERIC_HPP
#define _UNREALIRCD_NUMERIC_HPP

/**
 * IRC numeric enumeration
 */
enum IRCNumeric
{
	RPL_WELCOME 				= 001,
	RPL_YOURHOST 				= 002,
	RPL_CREATED 				= 003,
	RPL_MYINFO					= 004,
	RPL_ISUPPORT				= 005,

	RPL_UMODEIS					= 221,
	RPL_LUSERS					= 251,
	RPL_LUSEROPS				= 252,
	RPL_LUSERUNKNOWN			= 253,
	RPL_LUSERCHANNELS			= 254,
	RPL_LUSERME					= 255,
	RPL_ADMINME					= 256,
	RPL_ADMINLOC1				= 257,
	RPL_ADMINLOC2				= 258,
	RPL_ADMINEMAIL				= 259,

	RPL_AWAY					= 301,
	RPL_USERHOST				= 302,
	RPL_ISON					= 303,
	RPL_UNAWAY					= 305,
	RPL_NOWAWAY					= 306,
	RPL_WHOIS_USER				= 311,
	RPL_WHOIS_SERVER			= 312,
	RPL_WHOIS_OPERATOR			= 313,
	RPL_ENDOFWHO				= 315,
	RPL_WHOIS_IDLE				= 317,
	RPL_ENDOFWHOIS				= 318,
	RPL_WHOISCHANNELS			= 319,
	RPL_LISTSTART				= 321,
	RPL_LIST					= 322,
	RPL_LISTEND					= 323,
	RPL_CHANNELMODEIS			= 324,
	RPL_CHANNELCREATED			= 329,
	RPL_NOTOPIC					= 331,
	RPL_TOPIC					= 332,
	RPL_TOPICLASTCHANGED		= 333,
	RPL_INVITING				= 341,
	RPL_VERSION					= 351,
	RPL_WHOREPLY				= 352,
	RPL_NAMREPLY				= 353,
	RPL_ENDOFNAMES				= 366,
	RPL_BANLIST					= 367,
	RPL_ENDOFBANLIST			= 368,
	RPL_INFO					= 371,
	RPL_MOTD					= 372,
	RPL_ENDOFINFO				= 374,
	RPL_MOTDSTART				= 375,
	RPL_ENDOFMOTD				= 376,
	RPL_YOUREOPER				= 381,
	RPL_REHASHING				= 382,

	ERR_NOSUCHNICK				= 401,
	ERR_NOSUCHSERVER			= 402,
	ERR_NOSUCHCHANNEL			= 403,
	ERR_CANNOTSENDTOCHAN		= 404,
	ERR_TOOMANYCHANNELS			= 405,
	ERR_NOTEXTTOSEND			= 412,
	ERR_UNKNOWNCOMMAND			= 421,
	ERR_NOMOTD					= 422,
	ERR_NOADMININFO				= 423,
	ERR_INVALIDNICK				= 432,
	ERR_NICKNAMEINUSE			= 433,
	ERR_USERNOTINCHANNEL		= 441,
	ERR_NOTONCHANNEL			= 442,
	ERR_USERONCHANNEL			= 443,
	ERR_NOTREGISTERED			= 451,
	ERR_NEEDMOREPARAMS 			= 461,
	ERR_ALREADYREGISTERED		= 462,
	ERR_PASSWDMISMATCH			= 464,
	ERR_CHANNELISFULL			= 471,
	ERR_UNKNOWNMODE				= 472,
	ERR_INVITEONLYCHAN			= 473,
	ERR_BANNEDFROMCHAN			= 474,
	ERR_BADCHANNELKEY			= 475,
	ERR_NOPRIVILEGES			= 481,
	ERR_CHANOPRIVSNEEDED		= 482,
	ERR_NOOPERHOST				= 491,

	ERR_UMODEUNKNOWNFLAG		= 501,
	ERR_USERSDONTMATCH			= 502
};

/** numeric messages */
#define MSG_WELCOME				":Welcome to the %s IRC network, %s"
#define MSG_YOURHOST			":Your host is %s, running version %s"
#define MSG_CREATED				":This server was created %s"
#define MSG_MYINFO				"%s %s %s %s"
#define MSG_ISUPPORT			"%s :are supported by this server"
#define MSG_LUSERS				":There are %d users and %d invisible on %d "\
								"servers"

#define MSG_UMODEIS				"+%s"
#define MSG_LUSEROPS			"%d :operator(s) online"
#define MSG_LUSERUNKNOWN		"%d :unknown connection(s)"
#define MSG_LUSERCHANNELS		"%d :channels formed"
#define MSG_LUSERME				":I have %d clients and %d servers"
#define MSG_ADMINME				":Administrative information about \"%s\""
#define MSG_LUSERHIGHEST		":Highest connection count: %d (%d clients, %d"\
								" total connections since server was started)"
#define MSG_AWAY				"%s :%s"
#define MSG_USERHOST			":%s%s=%s%s@%s"
#define MSG_ISON				":%s"
#define MSG_UNAWAY				":You are not longer marked as being away"
#define MSG_NOWAWAY				":You have been marked as being away (%s)"
#define MSG_WHOIS_USER			"%s %s %s * :%s"
#define MSG_WHOIS_SERVER		"%s %s :%s"
#define MSG_WHOIS_OPERATOR		"%s :is an IRC operator"
#define MSG_ENDOFWHO			"%s :End of /WHO list."
#define MSG_WHOIS_IDLE			"%s %d %d :seconds idle, signon time"
#define MSG_ENDOFWHOIS			"%s :End of /WHOIS list."
#define MSG_WHOISCHANNELS		"%s :%s"
#define MSG_LISTSTART			"Channel :Users  Name"
#define MSG_LIST				"%s %d :[+%s] %s"
#define MSG_LISTEND				":End of /LIST"
#define MSG_CHANNELMODEIS		"+%s"
#define MSG_CHANNELCREATED		"%d"
#define MSG_NOTOPIC				":No topic is set."
#define MSG_TOPIC				":%s"
#define MSG_TOPICLASTCHANGED	"%s %d"
#define MSG_VERSION				"%s%s %s :%s"
#define MSG_WHOREPLY			"%s %s %s %s %s %s :%d %s"
#define MSG_NAMREPLY			"%c %s :%s"
#define MSG_ENDOFNAMES			"%s :End of /NAMES list."
#define MSG_BANLIST				"%s %s %d"
#define MSG_ENDOFBANLIST		":End of channel ban list"
#define MSG_INFO				":%s"
#define MSG_MOTD				":- %s"
#define MSG_ENDOFINFO			":End of /INFO command."
#define MSG_MOTDSTART			":- %s Message of the Day - "
#define MSG_ENDOFMOTD			":End of /MOTD command."
#define MSG_YOUREOPER			":You're now an IRC operator"
#define MSG_REHASHING			"%s :Rehashing"

#define MSG_NOSUCHNICK			"%s :No such nick"
#define MSG_NOSUCHSERVER		"%s :No such server"
#define MSG_NOSUCHCHANNEL		"%s :No such channel"
#define MSG_CANNOTSENDTOCHAN	"%s :Cannot send to channel"
#define MSG_TOOMANYCHANNELS		"%s :You have joined too many channels"
#define MSG_NOTEXTTOSEND		":No text to send"
#define MSG_UNKNOWNCOMMAND		"%s :Unknown command"
#define MSG_NOMOTD				":No MOTD available"
#define MSG_NOADMININFO			"%s :No administrative info available"
#define MSG_INVALIDNICK			"%s :Invalid nickname"
#define MSG_NICKNAMEINUSE		"%s :Nickname is already in use"
#define MSG_USERNOTINCHANNEL	"%s :They aren't on that channel"
#define MSG_NOTONCHANNEL		":You're not on that channel"
#define MSG_USERONCHANNEL		"%s %s :is already on channel"
#define MSG_NOTREGISTERED		":You have not registered"
#define MSG_NEEDMOREPARAMS		"%s :Not enough parameters"
#define MSG_ALREADYREGISTERED	":You may not reregister"
#define MSG_PASSWDMISMATCH		":Password incorrect"
#define MSG_CHANNELISFULL		"%s :Cannot join channel (+l); Full."
#define MSG_UNKNOWNMODE			"%c :Unknown channel mode flag"
#define MSG_INVITEONLYCHAN		"%s :Cannot join channel (+i); Invite only."
#define MSG_BANNEDFROMCHAN		"%s :Cannot join channel (+b); You're banned."
#define MSG_BADCHANNELKEY		"%s :Cannot join channel (+k); You need the "\
								"correct key."
#define MSG_NOPRIVILEGES		":Permission denied: You're not an IRC operator"
#define MSG_CHANOPRIVSNEEDED	":You're not channel operator"
#define MSG_NOOPERHOST			":No Operator privileges for your host"
#define MSG_CMDNOTAVAILABLE		"%s is currently not available"

#define MSG_UMODEUNKNOWNFLAG	"%c :Unknown user mode flag"
#define MSG_USERSDONTMATCH		":Can't change MODE for other users"

#define MSG_INSMODFAILED		":Loading module failed: %s"
#define MSG_INSMODOK			":Module \"%s\", Version \"%s\" loaded"
#define MSG_RMMODNOTFOUND		":Unable to locate module: %s"
#define MSG_RMMODOK				":Unloading module \"%s\""
#define MSG_LSMODSTART			":---- Active server modules (%d)"
#define MSG_LSMOD				":%s: \2%s\2 (Version \2%s\2) (Author: \2%s\2)"
#define MSG_LSMODEND			":End of /LSMOD."

#endif /* _UNREALIRCD_NUMERIC_HPP */
