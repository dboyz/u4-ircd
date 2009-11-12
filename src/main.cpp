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

#include "ircd.h"
#include "config.h"

#include <iostream>

extern "C"
{
#include <errno.h>
#include <fcntl.h>
#include <ltdl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
}

int main(int argc, char *argv[])
{
	char optstring[] = "hf:";
	char curopt;

	std::string config_path = "unreal++.conf";
	int config_fd;

	IRCd *IRCd;
	
	std::cerr << PACKAGE " " << VERSION << std::endl;

	while( (curopt = getopt(argc, argv, optstring)) != (char)-1)
		switch(curopt)
		{
		case 'h':
			std::cerr << "Usage: " << argv[0] << " [-h] [-f unreal++.conf]" << std::endl
				  << std::endl
				  << "\t-h\tShow this help" << std::endl
				  << "\t-f file\tSpecify path to unreal++.conf" << std::endl
				  << std::endl
				  << PACKAGE " will not be started because it was invoked with -h" << std::endl;
			return 0;
			break;

		case 'f':
			config_path = std::string(optarg);
			break;

		case '?':
			std::cerr << "Command line argument ``-" << (char)optopt << "'' does not exist" << std::endl
				    << "Exiting" << std::endl;
			
			return 1;
			break;

		case ':':
			std::cerr << "Command line argument ``-" << (char)optopt << "'' requires argument" << std::endl
				    << "Exiting" << std::endl;
			return 1;
			break;
		}

	if(!lt_dlinit())
	{
		std::cerr << "Unable to initialize ltdl: " << lt_dlerror() << std::endl;
		return 1;
	}

	errno = 0;
	config_fd = open(config_path.c_str(), O_RDONLY);
	if(config_fd == -1)
	{
		perror(config_path.c_str());
		std::cerr << "Unable to open config file" << std::endl
			  << "Exiting" << std::endl;
		return 1;
	}
	close(config_fd);

	std::cerr << "Using config file: ``" << config_path << "''" << std::endl;

	if(!lt_dlexit())
	{
		std::cerr << "Unable to deinitialize ltdl: " << lt_dlerror() << std::endl;
		return 1;
	}
	return 0;
}
