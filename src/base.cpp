/*****************************************************************
 * Unreal Internet Relay Chat Daemon, Version 4
 * File         base.cpp
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

#include "base.hpp"
#include "limits.hpp"
#include "reactor.hpp"
#include <cstdlib>
#include <iostream>
#include <boost/version.hpp>
#include <sys/resource.h>

/** global UnrealBase object */
UnrealBase* unreal = 0;

/**
 * UnrealBase constructor.
 *
 * @param cnt Command line argument count
 * @param vec Command line argument vector
 */
UnrealBase::UnrealBase(int cnt, char** vec)
	: fork_state_(Daemon)
{
	/* make this UnrealBase globally available */
	unreal = this;

	for (int i = 0; i < cnt; i++)
		argv << String(vec[i]);

	/* library initializations */
	UnrealModule::init();
	
	/* check the command line arguments */
	parseArgv();

	/* start reading the initial config file */
	config.startRead();

	/* open log file */
	initLog();

	/* load modules */
	initModules();

	/* initialize mode tables */
	initModes();

	/* fix resource limits */
	setupRlimit();
	
	/* setup Listeners */
	setupListener();

	/* build ISupport map */
	setupISupport();

	/* setup local server entry */
	setupServer();

	if (fork_state_ == Daemon)
	{
		pid_t fork_pid = fork();

		if (fork_pid == -1)
		{
			log << "Fatal: fork() failed.";
			exit(1);
		}
		else if (fork_pid != 0)
		{
			log.write(UnrealLog::Debug, "fork() succeed");

			/* update the fork state */
			fork_state_ = Daemonized;

			/* exit the parent process */
			exit();
		}
	}
}

/**
 * UnrealBase destructor.
 */
UnrealBase::~UnrealBase()
{
	finish();

	/* library deinitialization
	   (should only be done if modules are properly unloaded
	   first)
	 */
	UnrealModule::deinit();
}

/**
 * Configuration file validation check.
 */
void UnrealBase::checkConfig()
{
	std::cout << "Using file: "
			  << config.fileName()
			  << std::endl;

	config.startRead();

	if (config.warnings() > 0)
	{
		std::cout << "*** Detected "
				  << config.warnings()
				  << " warning(s)."
				  << std::endl;
		std::cout << "Please check the warning messages above to resolve "
				  << "errors in your configuration file and re-run this "
				  << "check."
				  << std::endl;
	}
	else
	{
		std::cout << "*** No warnings were detected when parsing your "
				  << "configuration file."
				  << std::endl;
	}

	exit();
}

/**
 * Check user permissions.
 */
void UnrealBase::checkPermissions()
{
	if (getuid() == 0 || getgid() == 0)
	{
		std::cerr << "Fatal: Operation as root UID/GID is not permitted."
				  << std::endl;
		exit(1);
	}
}

/**
 * Terminate program execution.
 *
 * @param code If specified, that code is returned to the OS
 */
void UnrealBase::exit(int code)
{
	finish();
	std::exit(code);
}

/**
 * Free allocated memory, close connections, etc.
 */
void UnrealBase::finish()
{
	for (List<UnrealListener*>::Iterator li = listeners.begin();
			li != listeners.end(); ++li)
	{
		delete *li;
	}
}

/**
 * Returns the fork() state.
 */
UnrealBase::FState UnrealBase::fstate()
{
	return fork_state_;
}

/**
 * Initialize the log system.
 */
void UnrealBase::initLog()
{
	String logfile = config.get("Log::File");
	String loglevel = config.get("Log::Level", "Normal");

	if (!logfile.empty())
	{
		log.setFileName(logfile);

		UnrealLog::LogLevel ll = UnrealLog::Normal;
		if (loglevel.toLower() == "debug")
			ll = UnrealLog::Debug;

		log.setLevel(ll);

		if (!log.open())
		{
			std::cerr << "Warning: Opening log file \""
					  << logfile
					  << "\" failed."
					  << std::endl;
		}
	}
}

/**
 * Load initial modes into the mode tables.
 */
void UnrealBase::initModes()
{
	/* user modes; using an own scope */
	{
		using namespace UnrealUserProperties;

		/* register standard modes into the mode table */
		ModeTable.registerMode(Deaf);
		ModeTable.registerMode(Invisible);
		ModeTable.registerMode(Operator);
		ModeTable.registerMode(Wallops);
	}

	/* channel modes */
	{
		using namespace UnrealChannelProperties;

		/* register standard modes into the mode table */
		ModeTable.registerMode(Ban);

		/* half op can be disabled */
		if (config.get("Features::EnableHalfOp", "false").toBool())
			ModeTable.registerMode(HalfOp);

		ModeTable.registerMode(InviteOnly);
		ModeTable.registerMode(Key);
		ModeTable.registerMode(Limit);
		ModeTable.registerMode(Moderated);
		ModeTable.registerMode(NoExternalMsg);
		ModeTable.registerMode(ChanOp);
		ModeTable.registerMode(Private);
		ModeTable.registerMode(Secret);
		ModeTable.registerMode(TopicOpsOnly);
		ModeTable.registerMode(Voice);
	}
}

/**
 * Load and initialize modules defined in the configuration files.
 */
void UnrealBase::initModules()
{
	StringList ml = config.moduleList();

	for (StringList::Iterator miter = ml.begin(); miter != ml.end(); ++miter)
	{
		UnrealModule* mptr = new UnrealModule(*miter);

		if (!mptr->isLoaded())
		{
			String errStr = mptr->errorString();

			log.write(UnrealLog::Normal, "Warning: Loading module failed: %s",
					errStr.c_str());

			delete mptr;
		}
		else
		{
			log.write(UnrealLog::Debug, "Loading Module \"%s\"",
			    (*miter).c_str());

			/* add to module list */
			modules << mptr;
		}
	}
}

/**
 * Go through the argument vector passed to the program during startup
 * and check for options.
 */
void UnrealBase::parseArgv()
{
	StringList::Iterator sli = argv.begin();

	if (sli != argv.end())
		sli++; // skip the first entry here

	for (; sli != argv.end(); ++sli)
	{
		if (*sli == "-c" || *sli == "--config-file")
		{
			if ((sli + 1) == argv.end())
			{
				std::cerr << *sli
						  << ": missing argument"
						  << std::endl;
				continue;
			}
			else
			{
				// update config file name
				config.setFileName(*++sli);
				continue;
			}
		}
		else if (*sli == "-C" || *sli == "--check-config")
		{
			checkConfig();
		}
		else if (*sli == "-h" || *sli == "--help")
		{
			printUsage();
		}
		else if (*sli == "-i" || *sli == "--interactive")
		{
			fork_state_ = Interactive;
		}
		else if (*sli == "-P" || *sli == "--print-config")
		{
			printConfig();
		}
		else if (*sli == "-v" || *sli == "--version")
		{
			printVersion();
		}
		else
		{
			std::cerr << *sli << ": unrecognized option" << std::endl;
		}
	}
}

/**
 * Print the program usage.
 */
void UnrealBase::printUsage()
{
	std::cout << "Usage: "
			  << argv.at(0)
			  << " [ -CchiPv [ arguments ] ]"
			  << std::endl
			  << std::endl;
	std::cout << "Available arguments:"
			  << std::endl;
	std::cout << "  -C, --check-config       Check your configuration file"
			  << std::endl;
	std::cout << "  -c, --config-file FILE   Specify an alternative config file"
			  << std::endl
			  << "                           Default: "
			  << CONFIG_DEFAULT_FILE
			  << std::endl;
	std::cout << "  -h, --help               Print this overview"
			  << std::endl;
	std::cout << "  -i, --interactive        Don't daemonize"
			  << std::endl;
	std::cout << "  -P, --print-config       Print config map contents"
			  << std::endl;
	std::cout << "  -v, --version            Print the program version"
			  << std::endl;

	exit();
}

/**
 * Print the config map contents.
 */
void UnrealBase::printConfig()
{
	config.startRead();

	Map<String, String> cmap = config.map();
	Map<String, String>::Iterator ci = cmap.begin();

	for (; ci != cmap.end(); ++ci)
	{
		std::cout << ci->first << "\n  => String("
		    << ci->second.length() << ") \"" << ci->second << "\"\n";
	}

	exit();
}

/**
 * Print the program version.
 */
void UnrealBase::printVersion()
{
	std::cout << "UnrealIRCd "
			  << VERSION
			  << PACKAGE_PATCHLEVEL
			  << ", Built "
			  << __DATE__
			  << " "
			  << __TIME__
			  << std::endl;
	std::cout << String::format("Using boost v%d.%d.%d",
					BOOST_VERSION / 100000,
					BOOST_VERSION / 100 % 1000,
					BOOST_VERSION % 100)
			  << std::endl;
	std::cout << "Copyright (c) 2009, 2010"
			  << std::endl;
	std::cout << "The UnrealIRCd team and contributors"
			  << std::endl;
	std::cout << "http://www.unrealircd.com"
			  << std::endl
			  << std::endl;

	std::cout << "This is free software: you are free to change and "
			  << "redistribute it."
			  << std::endl
			  << "There is NO WARRANTY, to the extent permitted by law."
			  << std::endl;

	exit();
}

/**
 * Returns the event reactor reference.
 */
UnrealReactor& UnrealBase::reactor()
{
	return reactor_;
}

/**
 * Run the main loop.
 * The reactor usually blocks until all attached I/O operations are done.
 * It usually never leaves this function unless all listeners are destroyed.
 */
void UnrealBase::run()
{
	reactor_.run();
}

/**
 * Setup basic iSupport items.
 */
void UnrealBase::setupISupport()
{
	if (isupport.size() > 0)
		isupport.clear();

	isupport.add("SILENCE", "0");
	isupport.add("MODES", "6");
	isupport.add("MAXCHANNELS", config.get("Limits::MaxChannelsPerUser", "20"));
	isupport.add("MAXBANS", config.get("Limits::MaxBansPerChannel", "30"));
	isupport.add("NICKLEN", config.get("Limits::Nicklen", "18"));
	isupport.add("TOPICLEN", config.get("Limits::Topiclen", "250"));
	isupport.add("AWAYLEN", config.get("Limits::Awaylen", "250"));
	isupport.add("KICKLEN", config.get("Limits::Kicklen", "250"));
	isupport.add("CHANNELLEN", config.get("Limits::Channellen", "200"));
	isupport.add("CHANMODES", "b,k,l,imnsp");
	isupport.add("CASEMAPPING", "rfc1459");
	isupport.add("NETWORK", config.get("Me::Network", "ExampleNet"));

	/* build PREFIX */
	String prefix = "(ov)@+";
	String statusmsg = "@+";
	String chantypes = "#";

	if (config.get("Features::EnableHalfOp", "false").toBool())
	{
		prefix = "(ohv)@%+";
		statusmsg = "@+";
	}
	
	if (config.get("Features::EnableLocalChannels", "true").toBool())
		chantypes = "&#";

	isupport.add("PREFIX", prefix);
	isupport.add("STATUSMSG", statusmsg);
	isupport.add("CHANTYPES", chantypes);
}

/**
 * Setup Listeners, that handle incoming connections.
 */
void UnrealBase::setupListener()
{
	for (size_t i = 1; i <= config.sequenceCount("Listener"); i++)
	{
		String addr = config.getSeqVal("Listener", i, "Address", "");

		if (addr.empty())
		{
			log.write(UnrealLog::Normal, "Warning: Omitting Listener #%d: "
					"No Address has been defined", i);

			continue;
		}

		String port = config.getSeqVal("Listener", i, "Port", "");

		if (port.empty())
		{
			log.write(UnrealLog::Normal, "Warning: Omitting Listener #%d: "
					"No Port has been defined", i);

			continue;
		}

		String type = config.getSeqVal("Listener", i, "Type", "");

		if (type.empty())
		{
			log.write(UnrealLog::Normal, "Warning: Omitting Listener #%d: "
					"No Type has been defined", i);

			continue;
		}

		/* ping frequency */
		uint32_t ping_freq = config.getSeqVal("Listener", i, "PingFreq",
			config.get("Limits::PingFreq", "120")).toUInt();

		/* max. connections allowed for this listener */
		uint32_t max_conns = config.getSeqVal("Listener", i, "MaxConnections",
			"1024").toUInt();

		/* Setup the Listener */
		UnrealListener* lptr = new UnrealListener(addr, port.toUInt16());
		UnrealListener::ListenerType ltype;

		if (type.toLower() == "server")
			ltype = UnrealListener::LServer;
		else
			ltype = UnrealListener::LClient;

		lptr->setMaxConnections(max_conns);
		lptr->setPingFrequency(ping_freq);
		lptr->setType(ltype);
		lptr->run();

		/* debug message */
		log.write(UnrealLog::Debug, "Adding listener \"%s:%s\"", addr.c_str(),
				port.c_str());

		/* append listener to list */
		listeners << lptr;
	}
}

/**
 * Check and fix resource limits as provided by the operating system.
 * That includes increasing the number of permitted open file descriptors
 * to the value of Me::MaxConnections, if specified in the configuration file.
 */
void UnrealBase::setupRlimit()
{
	struct rlimit rlnfiles;

	/* fetch the maximum number of permitted open files */
	if (getrlimit(RLIMIT_NOFILE, &rlnfiles) == -1)
		log.write(UnrealLog::Fatal, "Couldn't get file descriptor limit");
	else
	{
		size_t max_connections = config.get("Me::MaxConnections", "0").toSize();

		if (rlnfiles.rlim_max < max_connections)
		{
			log.write(UnrealLog::Fatal, "Me::MaxConnections (%d) exceeds the "
			    "resource limit on this system (%d). Please decrease the value "
			    "in your configuration file.",
			    max_connections,
			    rlnfiles.rlim_max);
		}

		if (rlnfiles.rlim_cur < max_connections)
		{
			/* try to increase */
			rlim_t old_lim = rlnfiles.rlim_cur;
			rlnfiles.rlim_cur = rlnfiles.rlim_max;

			if (setrlimit(RLIMIT_NOFILE, &rlnfiles) == -1)
			{
				log.write(UnrealLog::Fatal, "Couldn't update resource limit "
				    "for the number of open file descriptors to %d: %s",
				    rlnfiles.rlim_max,
				    strerror(errno));
			}
			else
			{
				log.write(UnrealLog::Debug, "Increased rlimit for open file "
				    "descriptors (%d) -> (%d)",
				    old_lim,
				    rlnfiles.rlim_cur);
			}
		}
	}
}

/**
 * Setup local server entry.
 */
void UnrealBase::setupServer()
{
	me.setName(config.get("Me::ServerName"));

	if (me.name().empty())
	{
		log.write(UnrealLog::Normal, "Fatal: Me::ServerName not specified.");
		exit(1);
	}

	me.setNumeric(config.get("Me::Numeric", "0").toUInt());

	if (me.numeric() == 0)
	{
		log.write(UnrealLog::Normal, "Fatal: Me::Numeric is an invalid server "
				"numeric. Please use a number between 1 and 4096.");
		exit(1);
	}

	me.setBootTime(UnrealTime::now());

	/* add into the server list */
	servers.add(me.numeric(), &me);
}
