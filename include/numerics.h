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

/*
 * Numeric Constants
 * KEEP THESE IN ORDER it'll make things easier later.
 */

#define RPL_WELCOME			":%s 001 %s :Welcome to the %s IRC Network %s!%s@%s"
#define RPL_YOURHOST			":%s 002 %s :Your host is %s, running version %s"
#define RPL_CREATED			":%s 003 %s :This server was created %s"
#define RPL_MYINFO			":%s 004 %s %s %s %s %s"
#define RPL_ISUPPORT			":%s 005 %s %s :are supported by this server"
#define RPL_MAP				":%s 006 %s :%s%-*s(%ld)  %s"
#define RPL_MAPEND			":%s 007 %s :End of /MAP"
#define RPL_SNOMASK			":%s 008 %s :Server notice mask (%s)"
#define RPL_REDIR			":%s 010 %s %s %d :Please use this Server/Port instead"
#define RPL_REMOTEISUPPORT		":%s 105 %s %s :are supported by this server"
#define RPL_TRACELINK 			":%s 200 %s Link %s%s %s %s"
#define RPL_TRACECONNECTING		":%s 201 %s Attempt %s %s"
#define RPL_TRACEHANDSHAKE 		":%s 202 %s Handshaking %s %s"
#define RPL_TRACEUNKNOWN 		":%s 203 %s ???? %s %s"
#define RPL_TRACEOPERATOR 		":%s 204 %s Operator %s %s [%s] %ld"
#define RPL_TRACEUSER 			":%s 205 %s User %s %s [%s] %ld"
#define RPL_TRACESERVER 		":%s 206 %s Server %s %dS %dC %s %s!%s@%s %ld"
#define RPL_TRACESERVICE 		":%s 207 %s Service %s %s"
#define RPL_TRACENEWTYPE 		":%s 208 %s %s 0 %s"
#define RPL_TRACECLASS 			":%s 209 %s Class %s %d"
#define RPL_STATSHELP 			":%s 210 %s :%s"
#define RPL_STATSCOMMANDS 		":%s 212 %s %s %u %lu"
#define RPL_STATSCLINE 			":%s 213 %s %c %s * %s %d %d %s"
#define RPL_STATSOLDNLINE 		":%s 214 %s %c %s * %s %d %d %s"
#define RPL_STATSILINE 			":%s 215 %s I %s * %s %d %s %s %d"
#define RPL_STATSKLINE 			":%s 216 %s %s %s %s"
#define RPL_STATSQLINE 			":%s 217 %s %c %s %ld %ld %s :%s"
#define RPL_STATSYLINE 			":%s 218 %s Y %s %d %d %d %d %d"
#define RPL_ENDOFSTATS 			":%s 219 %s %c :End of /STATS report"
#define RPL_STATSBLINE 			":%s 220 %s %c %s %s %s %d %d"
#define RPL_UMODEIS 			":%s 221 %s %s"
#define RPL_SQLINE_NICK 		":%s 222 %s %s :%s"
#define RPL_STATSGLINE 			":%s 223 %s %c %s@%s %li %li %s :%s"
#define RPL_STATSTLINE 			":%s 224 %s T %s %s %s"
#define RPL_STATSELINE 			":%s 225 %s e %s"
#define RPL_STATSNLINE 			":%s 226 %s n %s %s"
#define RPL_STATSVLINE			":%s 227 %s v %s %s %s"
#define RPL_STATSBANVER 		":%s 228 %s %s %s"
#define RPL_STATSSPAMF 			":%s 229 %s %c %s %s %li %li %li %s %s :%s"
#define RPL_STATSEXCEPTTKL 		":%s 230 %s %c %s"
#define RPL_RULES 			":%s 232 %s :- %s"
#define RPL_STATSLLINE 			":%s 241 %s %c %s * %s %d %d"
#define RPL_STATSUPTIME 		":%s 242 %s :Server Up %ld days, %ld:%02ld:%02ld"
#define RPL_STATSOLINE 			":%s 243 %s %c %s * %s %s %s"
#define RPL_STATSHLINE 			":%s 244 %s %c %s * %s %d %d"
#define RPL_STATSSLINE 			":%s 245 %s %c %s * %s %d %d"
#define RPL_STATSXLINE 			":%s 247 %s X %s %d"
#define RPL_STATSULINE 			":%s 248 %s U %s"
#define RPL_STATSCONN 			":%s 250 %s :Highest connection count: %d (%d clients)"
#define RPL_LUSERCLIENT 		":%s 251 %s :There are %d users and %d invisible on %d servers"
#define RPL_LUSEROP 			":%s 252 %s %d :operator(s) online"
#define RPL_LUSERUNKNOWN 		":%s 253 %s %d :unknown connection(s)"
#define RPL_LUSERCHANNELS 		":%s 254 %s %d :channels formed"
#define RPL_LUSERME 			":%s 255 %s :I have %d clients and %d servers"
#define RPL_ADMINME			":%s 256 %s :Administrative info about %s"
#define RPL_ADMINLOC1 			":%s 257 %s :%s"
#define RPL_ADMINLOC2 			":%s 258 %s :%s"
#define RPL_ADMINEMAIL 			":%s 259 %s :%s"
#define RPL_TRACELOG 			":%s 261 %s File %s %d"
#define RPL_LOCALUSERS 			":%s 265 %s :Current Local Users: %d  Max: %d"
#define RPL_GLOBALUSERS 		":%s 266 %s :Current Global Users: %d  Max: %d"
#define RPL_SILELIST 			":%s 271 %s %s %s"
#define RPL_ENDOFSILELIST 		":%s 272 %s :End of Silence List"
#define RPL_STATSDLINE 			":%s 275 %s %c %s %s"
#define RPL_HELPFWD 			":%s 294 %s :Your help-request has been forwarded to Help Operators"
#define RPL_HELPIGN 			":%s 295 %s :Your address has been ignored from forwarding"
#define RPL_AWAY 			":%s 301 %s %s :%s"
#define RPL_USERHOST 			":%s 302 %s :%s %s %s %s %s"
#define RPL_ISON 			":%s 303 %s :"
#define RPL_UNAWAY 			":%s 305 %s :You are no longer marked as being away"
#define RPL_NOWAWAY 			":%s 306 %s :You have been marked as being away"
#define RPL_WHOISREGNICK 		":%s 307 %s %s :is a registered nick"
#define RPL_RULESSTART 			":%s 308 %s :- %s Server Rules - "
#define RPL_ENDOFRULES 			":%s 309 %s :End of RULES command."
#define RPL_WHOISHELPOP 		":%s 310 %s %s :is available for help."
#define RPL_WHOISUSER 			":%s 311 %s %s %s %s * :%s"
#define RPL_WHOISSERVER 		":%s 312 %s %s %s :%s"
#define RPL_WHOISOPERATOR 		":%s 313 %s %s :is %s"
#define RPL_WHOWASUSER 			":%s 314 %s %s %s %s * :%s"
#define RPL_ENDOFWHO 			":%s 315 %s %s :End of /WHO list."
#define RPL_WHOISIDLE 			":%s 317 %s %s %ld %ld :seconds idle, signon time"
#define RPL_ENDOFWHOIS 			":%s 318 %s %s :End of /WHOIS list."
#define RPL_WHOISCHANNELS 		":%s 319 %s %s :%s"
#define RPL_WHOISSPECIAL 		":%s 320 %s %s :%s"
#define RPL_LISTSTART 			":%s 321 %s Channel :Users  Name"
#define RPL_LISTM 			":%s 322 %s %s %d :%s"
#define RPL_LIST 			":%s 322 %s %s %d :%s %s"
#define RPL_LISTEND 			":%s 323 %s :End of /LIST"
#define RPL_CHANNELMODEIS 		":%s 324 %s %s %s %s"
#define RPL_CREATIONTIME 		":%s 329 %s %s %lu"
#define RPL_NOTOPIC 			":%s 331 %s %s :No topic is set."
#define RPL_TOPIC 			":%s 332 %s %s :%s"
#define RPL_TOPICWHOTIME 		":%s 333 %s %s %s %lu"
#define RPL_LISTSYNTAX 			":%s 334 %s :%s"
#define RPL_WHOISBOT 			":%s 335 %s %s :is a \2Bot\2 on %s"
#define RPL_INVITELIST 			":%s 336 %s :%s"
#define RPL_ENDOFINVITELIST		":%s 337 %s :End of /INVITE list."
#define RPL_USERIP 			":%s 340 %s :%s %s %s %s %s"
#define RPL_INVITING 			":%s 341 %s %s %s"
#define RPL_SUMMONING 			":%s 342 %s %s :User summoned to irc"
#define RPL_INVEXLIST 			":%s 346 %s %s %s %s %lu"
#define RPL_ENDOFINVEXLIST 		":%s 347 %s %s :End of Channel Invite List"
#define RPL_EXLIST 			":%s 348 %s %s %s %s %lu"
#define RPL_ENDOFEXLIST 		":%s 349 %s %s :End of Channel Exception List"
#define RPL_VERSION 			":%s 351 %s %s.%s %s :%s%s%s [%s=%d]"
#define RPL_WHOREPLY 			":%s 352 %s %s %s %s %s %s %s :%d %s"
#define RPL_NAMREPLY 			":%s 353 %s %s"
#define RPL_CLOSING 			":%s 362 %s %s :Closed. Status = %d"
#define RPL_CLOSEEND 			":%s 363 %s %d: Connections Closed"
#define RPL_LINKS 			":%s 364 %s %s %s :%d %s"
#define RPL_ENDOFLINKS 			":%s 365 %s %s :End of /LINKS list."
#define RPL_ENDOFNAMES 			":%s 366 %s %s :End of /NAMES list."
#define RPL_BANLIST 			":%s 367 %s %s %s %s %lu"
#define RPL_ENDOFBANLIST  		":%s 368 %s %s :End of Channel Ban List"
#define RPL_ENDOFWHOWAS 		":%s 369 %s %s :End of WHOWAS"
#define RPL_INFO 			":%s 371 %s :%s"
#define RPL_MOTD 			":%s 372 %s :- %s"
#define RPL_INFOSTART 			":%s 373 %s :Server INFO"
#define RPL_ENDOFINFO 			":%s 374 %s :End of /INFO list."
#define RPL_MOTDSTART 			":%s 375 %s :- %s Message of the Day - "
#define RPL_ENDOFMOTD 			":%s 376 %s :End of /MOTD command."
#define RPL_WHOISHOST 			":%s 378 %s %s :is connecting from %s@%s %s"
#define RPL_WHOISMODES 			":%s 379 %s %s :is using modes %s %s"
#define RPL_YOUREOPER 			":%s 381 %s :You are now an IRC Operator"
#define RPL_REHASHING 			":%s 382 %s %s :Rehashing"
#define RPL_MYPORTIS 			":%s 384 %s %d :Port to local server is\r\n"
#define RPL_QLIST 			":%s 386 %s %s %s"
#define RPL_ENDOFQLIST 			":%s 387 %s %s :End of Channel Owner List"
#define RPL_ALIST 			":%s 388 %s %s %s"
#define RPL_ENDOFALIST 			":%s 389 %s %s :End of Protected User List"
#define RPL_TIME 			":%s 391 %s %s :%s"
#define ERR_NOSUCHNICK 			":%s 401 %s %s :No such nick/channel"
#define ERR_NOSUCHSERVER 		":%s 402 %s %s :No such server"
#define ERR_NOSUCHCHANNEL 		":%s 403 %s %s :No such channel"
#define ERR_CANNOTSENDTOCHAN		":%s 404 %s %s :%s (%s)"
#define ERR_TOOMANYCHANNELS 		":%s 405 %s %s :You have joined too many channels"
#define ERR_WASNOSUCHNICK 		":%s 406 %s %s :There was no such nickname"
#define ERR_TOOMANYTARGETS 		":%s 407 %s %s :Duplicate recipients. No message delivered"
#define ERR_NOORIGIN 			":%s 409 %s :No origin specified"
#define ERR_NORECIPIENT 		":%s 411 %s :No recipient given (%s)"
#define ERR_NOTEXTTOSEND 		":%s 412 %s :No text to send"
#define ERR_NOTOPLEVEL 			":%s 413 %s %s :No toplevel domain specified"
#define ERR_WILDTOPLEVEL 		":%s 414 %s %s :Wildcard in toplevel Domain"
#define ERR_UNKNOWNCOMMAND 		":%s 421 %s %s :Unknown command"
#define ERR_NOMOTD 			":%s 422 %s :MOTD File is missing"
#define ERR_NOADMININFO 		":%s 423 %s %s :No administrative info available"
#define ERR_FILEERROR 			":%s 424 %s :File error doing %s on %s"
#define ERR_NOOPERMOTD 			":%s 425 %s :OPERMOTD File is missing"
#define ERR_TOOMANYAWAY 		":%s 429 %s :Too Many aways - Flood Protection activated"
#define ERR_NONICKNAMEGIVEN 		":%s 431 %s :No nickname given"
#define ERR_ERRONEUSNICKNAME 		":%s 432 %s %s :Erroneous Nickname: %s"
#define ERR_NICKNAMEINUSE 		":%s 433 %s %s :Nickname is already in use."
#define ERR_NORULES 			":%s 434 %s :RULES File is missing"
#define ERR_NICKCOLLISION 		":%s 436 %s %s :Nickname collision KILL"
#define ERR_BANNICKCHANGE 		":%s 437 %s %s :Cannot change nickname while banned on channel"
#define ERR_NCHANGETOOFAST 		":%s 438 %s %s :Nick change too fast. Please wait %d seconds"
#define ERR_TARGETTOOFAST 		":%s 439 %s %s :Message target change too fast. Please wait %ld seconds"
#define ERR_SERVICESDOWN 		":%s 440 %s %s :Services are currently down. Please try again later."
#define ERR_USERNOTINCHANNEL		":%s 441 %s %s %s :They aren't on that channel"
#define ERR_NOTONCHANNEL 		":%s 442 %s %s :You're not on that channel"
#define ERR_USERONCHANNEL 		":%s 443 %s %s %s :is already on channel"
#define ERR_NOLOGIN 			":%s 444 %s %s :User not logged in"
#define ERR_SUMMONDISABLED 		":%s 445 %s :SUMMON has been disabled"
#define ERR_USERSDISABLED 		":%s 446 %s :USERS has been disabled"
#define ERR_NONICKCHANGE 		":%s 447 %s :Can not change nickname while on %s (+N)"
#define ERR_NOTREGISTERED 		":%s 451 %s :You have not registered"
#define ERR_HOSTILENAME 		":%s 455 %s :Your username %s contained the invalid character(s) %s and has been changed to %s. Please use only the characters 0-9 a-z A-Z _ - or . in your username. Your username is the part before the @ in your email address."
#define ERR_NOHIDING 			":%s 459 %s %s :Cannot join channel (+H)"
#define ERR_NOTFORHALFOPS 		":%s 460 %s :Halfops cannot set mode %c"
#define ERR_NEEDMOREPARAMS 		":%s 461 %s %s :Not enough parameters"
#define ERR_ALREADYREGISTRED 		":%s 462 %s :You may not reregister"
#define ERR_NOPERMFORHOST 		":%s 463 %s :Your host isn't among the privileged"
#define ERR_PASSWDMISMATCH 		":%s 464 %s :Password Incorrect"
#define ERR_YOUREBANNEDCREEP 		":%s 465 %s :You are banned from this server.  Mail %s for more information"
#define ERR_KEYSET 			":%s 467 %s %s :Channel key already set"
#define ERR_ONLYSERVERSCANCHANGE 	":%s 468 %s %s :Only servers can change that mode"
#define ERR_LINKSET 			":%s 469 %s %s :Channel link already set"
#define ERR_LINKCHANNEL 		":%s 470 %s [Link] %s has become full, so you are automatically being transferred to the linked channel %s"
#define ERR_CHANNELISFULL 		":%s 471 %s %s :Cannot join channel (+l)"
#define ERR_UNKNOWNMODE 		":%s 472 %s %c :is unknown mode char to me"
#define ERR_INVITEONLYCHAN 		":%s 473 %s %s :Cannot join channel (+i)"
#define ERR_BANNEDFROMCHAN 		":%s 474 %s %s :Cannot join channel (+b)"
#define ERR_BADCHANNELKEY 		":%s 475 %s %s :Cannot join channel (+k)"
#define ERR_BADCHANMASK 		":%s 476 %s %s :Bad Channel Mask"
#define ERR_NEEDREGGEDNICK 		":%s 477 %s %s :You need a registered nick to join that channel."
#define ERR_BANLISTFULL 		":%s 478 %s %s %s :Channel ban/ignore list is full"
#define ERR_LINKFAIL 			":%s 479 %s %s :Sorry, the channel has an invalid channel link set."
#define ERR_CANNOTKNOCK 		":%s 480 %s :Cannot knock on %s (%s)"
#define ERR_NOPRIVILEGES 		":%s 481 %s :Permission Denied- You do not have the correct IRC operator privileges"
#define ERR_CHANOPRIVSNEEDED 		":%s 482 %s %s :You're not channel operator"
#define ERR_CANTKILLSERVER 		":%s 483 %s :You cant kill a server!"
#define ERR_ATTACKDENY			":%s 484 %s %s :Cannot kick protected user %s."
#define ERR_KILLDENY			":%s 485 %s :Cannot kill protected user %s."
#define ERR_NONONREG			":%s 486 %s :You must identify to a registered nick to private message %s"
#define ERR_NOTFORUSERS			":%s 487 %s :%s is a server only command"
#define ERR_HTMDISABLED			":%s 488 %s :%s is currently disabled, please try again later."
#define ERR_SECUREONLYCHAN		":%s 489 %s %s :Cannot join channel (SSL is required)"
#define ERR_NOSWEAR			":%s 490 %s :%s does not accept private messages containing swearing."
#define ERR_NOOPERHOST			":%s 491 %s :No O-lines for your host"
#define ERR_NOCTCP			":%s 492 %s :%s does not accept CTCPs"
#define ERR_CHANOWNPRIVNEEDED		":%s 499 %s %s :You're not a channel owner"
#define ERR_TOOMANYJOINS 		":%s 500 %s %s :Too many join requests. Please wait a while and try again."
#define ERR_UMODEUNKNOWNFLAG 		":%s 501 %s :Unknown MODE flag"
#define ERR_USERSDONTMATCH 		":%s 502 %s :Cant change mode for other users"
#define ERR_SILELISTFULL 		":%s 511 %s %s :Your silence list is full"
#define ERR_TOOMANYWATCH 		":%s 512 %s %s :Maximum size for WATCH-list is 128 entries"
#define ERR_NEEDPONG 			":%s 513 %s :To connect, type /QUOTE PONG %lX"
#define ERR_TOOMANYDCC 			":%s 514 %s %s :Your dcc allow list is full. Maximum size is %d entries"
#define ERR_DISABLED			":%s 517 %s %s :%s"
#define ERR_CANTINVITE			":%s 518 %s :Cannot invite (+V) at channel %s",
#define ERR_ADMINONLY			":%s 519 %s :Cannot join channel %s (Admin only)",
#define ERR_OPERONLY			":%s 520 %s :Cannot join channel %s (IRCops only)",
#define ERR_LISTSYNTAX			":%s 521 %s :Bad list syntax, type /quote list ? or /raw list ?"
#define ERR_WHOSYNTAX			":%s 522 %s :/WHO Syntax incorrect, use /who ? for help"
#define ERR_WHOLIMEXCEED		":%s 523 %s :Error, /who limit of %d exceeded. Please narrow your search down and try again"
#define ERR_OPERSPVERIFY		":%s 524 %s :Trying to join +s or +p channel as an oper. Please invite yourself first."
#define RPL_LOGON			":%s 600 %s %s %s %s %d :logged online"
#define RPL_LOGOFF			":%s 601 %s %s %s %s %d :logged offline"
#define RPL_WATCHOFF			":%s 602 %s %s %s %s %d :stopped watching"
#define RPL_WATCHSTAT			":%s 603 %s :You have %d and are on %d WATCH entries"
#define RPL_NOWON			":%s 604 %s %s %s %s %ld :is online"
#define RPL_NOWOFF			":%s 605 %s %s %s %s %ld :is offline"
#define RPL_WATCHLIST			":%s 606 %s :%s"
#define RPL_ENDOFWATCHLIST		":%s 607 %s :End of WATCH %c"
#define RPL_MAPMORE			":%s 610 %s :%s%-*s --> *more*"
#define RPL_DCCSTATUS			":%s 617 %s :%s has been %s your DCC allow list"
#define RPL_DCCLIST			":%s 618 %s :%s"
#define RPL_ENDOFDCCLIST		":%s 619 %s :End of DCCALLOW %s"
#define RPL_DCCINFO			":%s 620 %s :%s"
#define RPL_SPAMCMDFWD			":%s 659 %s %s :Command processed, but a copy has been sent to ircops for evaluation (anti-spam) purposes. [%s]"
#define RPL_WHOISSECURE			":%s 671 %s %s :%s"
#define ERR_CANNOTDOCOMMAND		":%s 972 %s %s :%s"
#define ERR_CANNOTCHANGECHANMODE	":%s 974 %s %c :%s"
#define ERR_NUMERICERR			":%s 999 %s Numeric error!"
