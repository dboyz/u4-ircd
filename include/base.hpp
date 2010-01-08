/*****************************************************************
 * Unreal Internet Relay Chat Daemon, Version 4
 * File         base.hpp
 * Description  An object representing the unreal IRCd server
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

#ifndef _UNREALIRCD_BASE_H
#define _UNREALIRCD_BASE_H

#include "channel.hpp"
#include "command.hpp"
#include "config.hpp"
#include "isupport.hpp"
#include "list.hpp"
#include "listener.hpp"
#include "log.hpp"
#include "map.hpp"
#include "module.hpp"
#include "reactor.hpp"
#include "server.hpp"
#include "stats.hpp"
#include "string.hpp"
#include "stringlist.hpp"
#include "time.hpp"
#include "user.hpp"
#include "version.hpp"

/** generic foreach */
#define foreach(x, y, z) \
	for (x y=z.begin(); y!=z.end();++y)

/**
 * UnrealBase acts as the global ircd instance.
 */
class UnrealBase
{
public:
	enum FState
	{
		Daemon,			//< server is to be forked
		Daemonized,		//< server is daemonized
		Interactive		//< server should not be daemonized
	};

public:
	UnrealBase(int cnt, char** vec);
	~UnrealBase();

	void exit(int code = 0);
	FState fstate();
	void run();

public:
	/** command line argument vector */
	StringList argv;

	/** configuration */
	UnrealConfig config;

	/** log system */
	UnrealLog log;

	/** list with modules */
	List<UnrealModule*> modules;

	/** Reactor */
	UnrealReactor reactor;

	/** list with listeners */
	List<UnrealListener*> listeners;

	/** user mapping */
	Map<UnrealSocket*, UnrealUser*> users;

	/** nick mapping */
	Map<String, UnrealUser*> nicks;

	/** channel mapping */
	Map<String, UnrealChannel*> channels;

	/** user command mapping */
	Map<String, UnrealUserCommand*> user_commands;

	/** server mapping */
	Map<uint32_t, UnrealServer*> servers;

	/** iSupport features */
	UnrealISupport isupport;

	/** server statistics */
	UnrealLocalStat stats;

	/** local server entry */
	UnrealServer me;

private:
	void checkConfig();
	void checkPermissions();
	void finish();
	void initLog();
	void initModes();
	void initModules();
	void parseArgv();
	void printConfig();
	void printUsage();
	void printVersion();
	void setupISupport();
	void setupListener();
	void setupRlimit();
	void setupServer();

private:
	/** fork() state */
	FState fork_state_;
};

extern UnrealBase* unreal;

#endif /* _UNREALIRCD_BASE_H */
