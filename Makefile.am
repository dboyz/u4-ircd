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

INCLUDES = -I$(top_srcdir)/include -I$(top_srcdir)/src/cmd/include

AM_DEFAULT_SOURCE_EXT = .cpp
AM_LDFLAGS = $(BOOST_LDFLAGS)

# catch all for needed files:
EXTRA_DIST = src/version.cpp.in \
	$(top_srcdir)/tools/versionblast.sh

# config file:
dist_sysconf_DATA = etc/unrealircd4.conf

# docs:
dist_doc_DATA = docs/codingstyle.txt \
	docs/configuration.txt

#
# Binaries:
#

bin_PROGRAMS = unrealircd4

# this is pkginclude because these headers are needed
# by modules that will be compiled against UnrealIRCd-CPP
pkginclude_HEADERS = \
	include/base.hpp \
	include/bitmask.hpp \
	include/channel.hpp \
	include/command.hpp \
	include/config.hpp \
	include/exception.hpp \
	include/isupport.hpp \
	include/hash.hpp \
	include/limits.hpp \
	include/list.hpp \
	include/listener.hpp \
	include/log.hpp \
	include/map.hpp \
	include/mode.hpp \
	include/modebuf.hpp \
	include/module.hpp \
	include/numeric.hpp \
	include/platform.hpp \
	include/reactor.hpp \
	include/recvq.hpp \
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

cmdpkgincludedir = $(pkgincludedir)/cmd
cmdpkginclude_HEADERS = \
	include/cmd/admin.hpp \
	include/cmd/away.hpp \
	include/cmd/help.hpp \
	include/cmd/info.hpp \
	include/cmd/insmod.hpp \
	include/cmd/invite.hpp \
	include/cmd/ison.hpp \
	include/cmd/join.hpp \
	include/cmd/kick.hpp \
	include/cmd/kill.hpp \
	include/cmd/list.hpp \
	include/cmd/lsmod.hpp \
	include/cmd/lusers.hpp \
	include/cmd/mode.hpp \
	include/cmd/motd.hpp \
	include/cmd/names.hpp \
	include/cmd/nick.hpp \
	include/cmd/notice.hpp \
	include/cmd/oper.hpp \
	include/cmd/part.hpp \
	include/cmd/ping.hpp \
	include/cmd/pong.hpp \
	include/cmd/privmsg.hpp \
	include/cmd/quit.hpp \
	include/cmd/rehash.hpp \
	include/cmd/restart.hpp \
	include/cmd/rmmod.hpp \
	include/cmd/topic.hpp \
	include/cmd/user.hpp \
	include/cmd/userhost.hpp \
	include/cmd/version.hpp \
	include/cmd/who.hpp \
	include/cmd/whois.hpp \
	include/cmd/whowas.hpp

unrealircd4_SOURCES = \
	src/base.cpp \
	src/channel.cpp \
	src/command.cpp \
	src/config.cpp \
	src/hash.cpp \
	src/listener.cpp \
	src/log.cpp \
	src/module.cpp \
	src/recvq.cpp \
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
nodist_unrealircd4_SOURCES = $(top_builddir)/src/version.cpp

# we need to _not_ have AM_LDFLAGS added to the binary because it has libtool
# flags which don't work with binaries. Thus, we set this variable which
# _overrides_ AM_LDFLAGS. It's a good thing that this doesn't just supplement
# them  ;-)
#
# supposedly we need -export-dynamic (eaten by libtool)
# so that modules can link to the IRCd's module object
#  when loaded
unrealircd4_LDFLAGS = $(AM_LDFLAGS) -export-dynamic

#
# src/version.cpp:
#
NEED_VERSION_CPP = $(top_srcdir)/src/version.cpp.in $(top_srcdir)/tools/versionblast.sh
DOCP =
if VCS
NEED_VERSION_CPP += $(top_srcdir)/.hg/dirstate
else
DOCP += cp $(top_srcdir)/src/version.cpp.in $(top_builddir)/src/version.cpp
endif

$(top_builddir)/src/version.cpp: $(NEED_VERSION_CPP)
	MKDIR_P="$(MKDIR_P)" SED="$(SED)" top_builddir="$(top_builddir)" top_srcdir="$(top_srcdir)" DOCP="$(DOCP)" $(top_srcdir)/tools/versionblast.sh
