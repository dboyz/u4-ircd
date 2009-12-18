 #################################################################
 # Unreal Internet Relay Chat Daemon, Version 4
 # File         Makefile.am
 # Description  Description of source files to build and install
 #
 # All parts of this program are Copyright(C) 2009 by their
 # respective authors and the UnrealIRCd development team.
 #
 # This program is free software; you can redistribute it and/or
 # modify it under the terms of the GNU General Public License as
 # published by the Free Software Foundation; either version 3 of
 # the License, or (at your option) any later version.
 #
 # You should have received a copy of the GNU General Public
 # License along with this program; if not, write to the Free
 # Software Foundation, Inc., 59 Temple Place - Suite 330,
 # Boston, MA 02111-1307, USA.
 #
 # This program is distributed in the hope that it will be useful,
 # but WITHOUT ANY WARRANTY; without even the implied warranty of
 # MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 # GNU General Public License for more details.
 ##################################################################

# The following is:
# somewhat based off of unieject-6

SUBDIRS = . src/cmd

INCLUDES = -I$(top_srcdir)/include

AM_DEFAULT_SOURCE_EXT = .cpp
AM_CPPFLAGS = @BOOST_CPPFLAGS@
AM_CXXFLAGS = -Wall
#AM_CXXFLAGS = -std=c++0x -Wall

# http://www.flameeyes.eu/autotools-mythbuster/libtool/index.html#libtool.plugins.dlopen
#AM_LDFLAGS = @BOOST_LDFLAGS@ -module -avoid-version -shared
LIBS = @LTDL_LIBS@ @BOOST_ASIO_LIB@ @BOOST_SYSTEM_LIB@ @BOOST_THREAD_LIB@ -ldl

#
# Binaries:
#

bin_PROGRAMS = unrealircd

# this is pkginclude because these headers are needed
# by modules that will be compiled against UnrealIRCd-CPP
pkginclude_HEADERS = \
	include/base.hpp \
	include/bitmask.hpp \
	include/channel.hpp \
	include/cmdmap.hpp \
	include/command.hpp \
	include/config.hpp \
	include/ioservicepool.hpp \
	include/isupport.hpp \
	include/list.hpp \
	include/listener.hpp \
	include/log.hpp \
	include/map.hpp \
	include/mode.hpp \
	include/modebuf.hpp \
	include/module.hpp \
	include/numeric.hpp \
	include/platform.hpp \
	include/resolver.hpp \
	include/server.hpp \
	include/socket.hpp \
	include/stats.hpp \
	include/string.hpp \
	include/stringlist.hpp \
	include/time.hpp \
	include/timer.hpp \
	include/user.hpp \
	include/version.hpp

unrealircd_SOURCES = \
	src/base.cpp \
	src/channel.cpp \
	src/command.cpp \
	src/config.cpp \
	src/ioservicepool.cpp \
	src/listener.cpp \
	src/log.cpp \
	src/module.cpp \
	src/resolver.cpp \
	src/server.cpp \
	src/socket.cpp \
	src/string.cpp \
	src/stringlist.cpp \
	src/time.cpp \
	src/timer.cpp \
	src/unreal.cpp \
	src/user.cpp \
	$(pkginclude_HEADERS)

# we need to _not_ have AM_LDFLAGS added to the binary because it has libtool
# flags which don't work with binaries. Thus, we set this variable which
# _overrides_ AM_LDFLAGS. It's a good thing that this doesn't just supplement
# them  ;-)
#
# supposedly we need --export-dynamic so that modules can link to the IRCd when loaded...?
unrealircd_LDFLAGS = @BOOST_LDFLAGS@ -Wl,--export-dynamic

#
# include/hgrev.hpp:
#

# only define repo vars if needed
NEED_HGREV_HPP = 
if VCS
NEED_HGREV_HPP += include/hgrev.hpp
endif

include/hgrev.hpp: .hg/dirstate
	cd $(top_srcdir)
	echo \#define\ PACKAGE_CHANGESET\ \"$(hg -R $(top_srcdir) id -i)\" > $(top_srcdir)/include/hgrev.hpp


# there must be something cleaner than this.
# there must be some type of Makefile-time
# file generation hook. If anything, this is
# where parallel building will break, especially
# with SUBDIRS :-/
$(unrealircd_SOURCES): $(NEED_HGREV_HPP)
