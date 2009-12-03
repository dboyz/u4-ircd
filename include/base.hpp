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

#ifndef _UNREALIRCD_BASE_H
#define _UNREALIRCD_BASE_H

#include "config.hpp"
#include "log.hpp"
#include "string.hpp"
#include "stringlist.hpp"

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

private:
	void checkConfig();
	void checkPermissions();
	void finish();
	void initLog();
	void parseArgv();
	void printConfig();
	void printUsage();
	void printVersion();

private:
	/** fork() state */
	FState fork_state_;
};

extern UnrealBase* unreal;

#endif /* _UNREALIRCD_BASE_H */
