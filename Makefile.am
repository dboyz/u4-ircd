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
	include/channel.h \
	include/config.h \
	include/internal.h \
	include/ircd.h \
	include/misc.h \
	include/module.h \
	include/modules.h \
	include/numerics.h \
	include/socketengine.h \
	include/user.h

unrealircd_SOURCES = \
	src/channel.cpp \
	src/main.cpp \
	src/misc.cpp \
	src/module.cpp \
	src/modules.cpp \
	src/socketengine.cpp \
	src/user.cpp \
	$(pkginclude_HEADERS)
unrealircd_LDADD = @LTDL_LIBS@

# http://www.flameeyes.eu/autotools-mythbuster/libtool/index.html#libtool.plugins.dlopen

MODULEDIR=src/modules
pkglib_LTLIBRARIES = \
	m_null.la

m_null_la_SOURCES = $(MODULEDIR)/m_null
m_null_la_LIBADD = $(unrealircd_LDADD)
m_null_la_LDFLAGS = -module -avoid-version -shared
