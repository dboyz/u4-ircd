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

#include "base.h"

/**
 * UnrealBase constructor.
 *
 * @param cnt Command line argument count
 * @param vec Command line argument vector
 */
UnrealBase::UnrealBase(int cnt, char** vec)
{
	for (int i = 0; i < cnt; i++)
		argv.push_back(std::string(vec));

	ParseArgv();

	// TODO: configuration reader

	// TODO: log file writer

	// TODO: load modules

	// TODO: setup listener (w/ threads ?!)
}

UnrealBase::~UnrealBase()
{
	//XXX: clean allocated stuff here
}

/**
 * Go through the argument vector passed to the program during startup
 * and check for options.
 */
void UnrealBase::ParseArgv()
{
	std::vector<std::string>::iterator ai = argv.begin();

	if (ai != argv.end())
		ai++; // skip the first entry here

	for (; ai != argv.end(); ai++)
	{
		if (*ai == "-h" || *ai == "--help")
			PrintUsage();
		else if (*ai == "-v" || *ai == "--version")
			PrintVersion();
		else
			std::cerr << *ai << ": unrecognized option" << std::endl;
	}
}

/**
 * Print the program usage.
 */
void UnrealBase::PrintUsage()
{
	std::cout << "Usage: " << argv.at(0) << " [ -hv [ arguments ] ]"
			  << std::endl << std::endl;
	std::cout << "Available arguments:" << std::endl;
	std::cout << "  -h, --help                 Print this overview"
			  << std::endl;
	std::cout << "  -v, --version              Print the program version"
			  << std::endl;
}

/**
 * Print the program version.
 */
void UnrealBase::PrintVersion()
{
	std::cout << "UnrealIRCd 4.0.0-alpha" << std::endl;
	std::cout << "Copyright (c) 2009 The UnrealIRCd Project" << std::endl;
	std::cout << "http://www.unrealircd.com" << std::endl << std::endl;

	std::cout << "This is free software: you are free to change and "
			  << "redistribute it." << std::endl
			  << "There is NO WARRANTY, to the extent permitted by law."
			  << std::endl;

}

/**
 * Run the main loop.
 */
void UnrealBase::Run()
{
	// TODO: enter the main loop here
}
