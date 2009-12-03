/*
 *  UnrealIRCd, the next generation IRCd
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

#include "base.hpp"
#include <cstdlib>
#include <iostream>

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

	// check the command line arguments
	parseArgv();

	// start reading the initial config file
	config.startRead();

	// log file stuff
	initLog();

	// TODO: load modules

	// TODO: setup listener (w/ threads ?!)

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
	if (getuid() == 0)
	{
		std::cerr << "Fatal: Operation as root is not permitted."
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
	//..
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
	String logfile = config.get("Log/File");
	String loglevel = config.get("Log/Level", "Normal");

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
 * Go through the argument vector passed to the program during startup
 * and check for options.
 */
void UnrealBase::parseArgv()
{
	StringList::Iterator sli = argv.begin();

	if (sli != argv.end())
		sli++; // skip the first entry here

	for (; sli != argv.end(); sli++)
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
			  << " [ -Cchv [ arguments ] ]"
			  << std::endl
			  << std::endl;
	std::cout << "Available arguments:"
			  << std::endl;
	std::cout << "  -C, --check-config         Check your configuration file"
			  << std::endl;
	std::cout << "  -c, --config-file FILE     Specify an alternative config file"
			  << std::endl
			  << "                             Default: "
			  << CONFIG_DEFAULT_FILE
			  << std::endl;
	std::cout << "  -h, --help                 Print this overview"
			  << std::endl;
	std::cout << "  -i, --interactive          Don't daemonize"
			  << std::endl;
	std::cout << "  -P, --print-config         Print config map contents"
			  << std::endl;
	std::cout << "  -v, --version              Print the program version"
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

	for (; ci != cmap.end(); ci++)
	{
		std::cout << ci->first << "\n  => \"" << ci->second << "\"\n";
	}

	exit();
}
/**
 * Print the program version.
 */
void UnrealBase::printVersion()
{
	std::cout << "UnrealIRCd 4.0.0-alpha" << std::endl;
	std::cout << "Copyright (c) 2009 The UnrealIRCd Project" << std::endl;
	std::cout << "http://www.unrealircd.com" << std::endl << std::endl;

	std::cout << "This is free software: you are free to change and "
			  << "redistribute it." << std::endl
			  << "There is NO WARRANTY, to the extent permitted by law."
			  << std::endl;

	exit();
}

/**
 * Run the main loop.
 */
void UnrealBase::run()
{
	// TODO: enter the main loop here
}
