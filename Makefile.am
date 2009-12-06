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

INCLUDES = -I$(top_srcdir)/include

bin_PROGRAMS = unrealircd

# this is pkginclude because these headers are needed
# by modules that will be compiled against UnrealIRCd-CPP
pkginclude_HEADERS = \
	include/base.hpp \
	include/bitmask.hpp \
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
	include/module.hpp \
	include/numeric.hpp \
	include/platform.hpp \
	include/resolver.hpp \
	include/socket.hpp \
	include/stats.hpp \
	include/string.hpp \
	include/stringlist.hpp \
	include/time.hpp \
	include/user.hpp

unrealircd_SOURCES = \
	src/base.cpp \
	src/command.cpp \
	src/config.cpp \
	src/ioservicepool.cpp \
	src/listener.cpp \
	src/log.cpp \
	src/module.cpp \
	src/resolver.cpp \
	src/socket.cpp \
	src/string.cpp \
	src/stringlist.cpp \
	src/time.cpp \
	src/unreal.cpp \
	src/user.cpp \
	$(pkginclude_HEADERS)

unrealircd_LDADD = @LTDL_LIBS@ @BOOST_LDFLAGS@ @BOOST_ASIO_LIB@ @BOOST_SYSTEM_LIB@ @BOOST_THREAD_LIB@ -ldl
#unrealircd_CXXFLAGS = @BOOST_CPPFLAGS@ -std=c++0x -Wall
unrealircd_CXXFLAGS = @BOOST_CPPFLAGS@ -Wall
unrealircd_LDFLAGS = -Wl,--export-dynamic

# http://www.flameeyes.eu/autotools-mythbuster/libtool/index.html#libtool.plugins.dlopen

MODULEDIR=src/modules
pkglib_LTLIBRARIES = \
	m_admin.la \
	m_away.la \
	m_lusers.la \
	m_nick.la \
	m_notice.la \
	m_pong.la \
	m_privmsg.la \
	m_quit.la \
	m_user.la \
	m_userhost.la \
	m_whois.la

#m_null_la_SOURCES = $(MODULEDIR)/m_null.cpp
#m_null_la_LIBADD = $(unrealircd_LDADD)
#m_null_la_LDFLAGS = -module -avoid-version -shared

m_admin_la_SOURCES = src/cmd/admin.cpp
m_admin_la_LIBADD = $(unrealircd_LDADD)
m_admin_la_LDFLAGS = -module -avoid-version -shared

m_away_la_SOURCES = src/cmd/away.cpp
m_away_la_LIBADD = $(unrealircd_LDADD)
m_away_la_LDFLAGS = -module -avoid-version -shared

m_lusers_la_SOURCES = src/cmd/lusers.cpp
m_lusers_la_LIBADD = $(unrealircd_LDADD)
m_lusers_la_LDFLAGS = -module -avoid-version -shared

m_nick_la_SOURCES = src/cmd/nick.cpp
m_nick_la_LIBADD = $(unrealircd_LDADD)
m_nick_la_LDFLAGS = -module -avoid-version -shared

m_notice_la_SOURCES = src/cmd/notice.cpp
m_notice_la_LIBADD = $(unrealircd_LDADD)
m_notice_la_LDFLAGS = -module -avoid-version -shared

m_pong_la_SOURCES = src/cmd/pong.cpp
m_pong_la_LIBADD = $(unrealircd_LDADD)
m_pong_la_LDFLAGS = -module -avoid-version -shared

m_privmsg_la_SOURCES = src/cmd/privmsg.cpp
m_privmsg_la_LIBADD = $(unrealircd_LDADD)
m_privmsg_la_LDFLAGS = -module -avoid-version -shared

m_quit_la_SOURCES = src/cmd/quit.cpp
m_quit_la_LIBADD = $(unrealircd_LDADD)
m_quit_la_LDFLAGS = -module -avoid-version -shared

m_user_la_SOURCES = src/cmd/user.cpp
m_user_la_LIBADD = $(unrealircd_LDADD)
m_user_la_LDFLAGS = -module -avoid-version -shared

m_userhost_la_SOURCES = src/cmd/userhost.cpp
m_userhost_la_LIBADD = $(unrealircd_LDADD)
m_userhost_la_LDFLAGS = -module -avoid-version -shared

m_whois_la_SOURCES = src/cmd/whois.cpp
m_whois_la_LIBADD = $(unrealircd_LDADD)
m_whois_la_LDFLAGS = -module -avoid-version -shared
