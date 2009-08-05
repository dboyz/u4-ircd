#   UnrealIRCd -- The Next Generation IRCd.
#
#   This program is free software; you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation; either version 1, or (at your option)
#   any later version.
#
#   This program is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.
#
#   You should have received a copy of the GNU General Public License
#   along with this program; if not, write to the Free Software
#   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
#

# The following is:
# somewhat based off of unieject-6

INCLUDES = -I$(top_srcdir)/include

bin_PROGRAMS = unrealircd

# this is pkginclude because these headers are needed
# by modules that will be compiled against UnrealIRCd-CPP
pkginclude_HEADERS = \
	include/config.h \
	include/internal.h \
	include/modules.h \
	include/numerics.h \
	include/socketengine.h \
	include/user.h

unrealircd_SOURCES = \
	src/channels.cpp \
	src/misc.cpp \
	src/modules.cpp \
	src/socketengine.cpp \
	src/users.cpp
