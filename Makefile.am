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
	include/base.hpp \
	include/channel.hpp \
	include/config.h \
	include/list.hpp \
	include/log.hpp \
	include/map.hpp \
	include/module.hpp \
	include/numerics.hpp \
	include/platform.hpp \
	include/socketengine.hpp \
	include/string.hpp \
	include/stringlist.hpp \
	include/time.hpp \
	include/user.hpp

unrealircd_SOURCES = \
	src/base.cpp \
	src/channel.cpp \
	src/config.cpp \
	src/misc.cpp \
	src/module.cpp \
	src/string.cpp \
	src/stringlist.cpp \
	src/time.cpp \
	src/unreal.cpp \
	src/user.cpp \
	$(pkginclude_HEADERS)
unrealircd_LDADD = @LTDL_LIBS@ -lld
#unrealircd_CXXFLAGS = -std=c++0x -Wall
unrealircd_CXXFLAGS = -Wall

# http://www.flameeyes.eu/autotools-mythbuster/libtool/index.html#libtool.plugins.dlopen

MODULEDIR=src/modules
pkglib_LTLIBRARIES = \
	m_null.la

m_null_la_SOURCES = $(MODULEDIR)/m_null
m_null_la_LIBADD = $(unrealircd_LDADD)
m_null_la_LDFLAGS = -module -avoid-version -shared
