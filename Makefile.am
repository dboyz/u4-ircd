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
	include/socket.hpp \
	include/stats.hpp \
	include/string.hpp \
	include/stringlist.hpp \
	include/time.hpp \
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
	src/socket.cpp \
	src/string.cpp \
	src/stringlist.cpp \
	src/time.cpp \
	src/unreal.cpp \
	src/user.cpp \
	$(pkginclude_HEADERS)

unrealircd_CPPFLAGS = @BOOST_CPPFLAGS@
unrealircd_CXXFLAGS = -Wall

unrealircd_LDFLAGS = @BOOST_LDFLAGS@ -Wl,--export-dynamic
unrealircd_LDADD = @LTDL_LIBS@ @BOOST_ASIO_LIB@ @BOOST_SYSTEM_LIB@ @BOOST_THREAD_LIB@ -ldl
#unrealircd_CXXFLAGS = @BOOST_CPPFLAGS@ -std=c++0x -Wall

# http://www.flameeyes.eu/autotools-mythbuster/libtool/index.html#libtool.plugins.dlopen

MODULEDIR=src/modules
pkglib_LTLIBRARIES = \
	m_admin.la \
	m_away.la \
	m_help.la \
	m_info.la \
	m_insmod.la \
	m_ison.la \
	m_join.la \
	m_kick.la \
	m_list.la \
	m_lsmod.la \
	m_lusers.la \
	m_mode.la \
	m_motd.la \
	m_names.la \
	m_nick.la \
	m_notice.la \
	m_part.la \
	m_ping.la \
	m_pong.la \
	m_privmsg.la \
	m_quit.la \
	m_rmmod.la \
	m_topic.la \
	m_user.la \
	m_userhost.la \
	m_version.la \
	m_whois.la

#m_null_la_SOURCES = $(MODULEDIR)/m_null.cpp
#m_null_la_LIBADD = $(unrealircd_LDADD)
#m_null_la_LDFLAGS = -module -avoid-version -shared

m_admin_la_SOURCES = src/cmd/admin.cpp
m_admin_la_CPPFLAGS = $(unrealircd_CPPFLAGS)
m_admin_la_LIBADD = $(unrealircd_LDADD)
m_admin_la_LDFLAGS = -module -avoid-version -shared

m_away_la_SOURCES = src/cmd/away.cpp
m_away_la_CPPFLAGS = $(unrealircd_CPPFLAGS)
m_away_la_LIBADD = $(unrealircd_LDADD)
m_away_la_LDFLAGS = -module -avoid-version -shared

m_help_la_SOURCES = src/cmd/help.cpp
m_help_la_CPPFLAGS = $(unrealircd_CPPFLAGS)
m_help_la_LIBADD = $(unrealircd_LDADD)
m_help_la_LDFLAGS = -module -avoid-version -shared

m_info_la_SOURCES = src/cmd/info.cpp
m_info_la_CPPFLAGS = $(unrealircd_CPPFLAGS)
m_info_la_LIBADD = $(unrealircd_LDADD)
m_info_la_LDFLAGS = -module -avoid-version -shared

m_insmod_la_SOURCES = src/cmd/insmod.cpp
m_insmod_la_CPPFLAGS = $(unrealircd_CPPFLAGS)
m_insmod_la_LIBADD = $(unrealircd_LDADD)
m_insmod_la_LDFLAGS = -module -avoid-version -shared

m_ison_la_SOURCES = src/cmd/ison.cpp
m_ison_la_CPPFLAGS = $(unrealircd_CPPFLAGS)
m_ison_la_LIBADD = $(unrealircd_LDADD)
m_ison_la_LDFLAGS = -module -avoid-version -shared

m_join_la_SOURCES = src/cmd/join.cpp
m_join_la_CPPFLAGS = $(unrealircd_CPPFLAGS)
m_join_la_LIBADD = $(unrealircd_LDADD)
m_join_la_LDFLAGS = -module -avoid-version -shared

m_kick_la_SOURCES = src/cmd/kick.cpp
m_kick_la_CPPFLAGS = $(unrealircd_CPPFLAGS)
m_kick_la_LIBADD = $(unrealircd_LDADD)
m_kick_la_LDFLAGS = -module -avoid-version -shared

m_list_la_SOURCES = src/cmd/list.cpp
m_list_la_CPPFLAGS = $(unrealircd_CPPFLAGS)
m_list_la_LIBADD = $(unrealircd_LDADD)
m_list_la_LDFLAGS = -module -avoid-version -shared

m_lsmod_la_SOURCES = src/cmd/lsmod.cpp
m_lsmod_la_CPPFLAGS = $(unrealircd_CPPFLAGS)
m_lsmod_la_LIBADD = $(unrealircd_LDADD)
m_lsmod_la_LDFLAGS = -module -avoid-version -shared

m_lusers_la_SOURCES = src/cmd/lusers.cpp
m_lusers_la_CPPFLAGS = $(unrealircd_CPPFLAGS)
m_lusers_la_LIBADD = $(unrealircd_LDADD)
m_lusers_la_LDFLAGS = -module -avoid-version -shared

m_mode_la_SOURCES = src/cmd/mode.cpp
m_mode_la_CPPFLAGS = $(unrealircd_CPPFLAGS)
m_mode_la_LIBADD = $(unrealircd_LDADD)
m_mode_la_LDFLAGS = -module -avoid-version -shared

m_motd_la_SOURCES = src/cmd/motd.cpp
m_motd_la_CPPFLAGS = $(unrealircd_CPPFLAGS)
m_motd_la_LIBADD = $(unrealircd_LDADD)
m_motd_la_LDFLAGS = -module -avoid-version -shared

m_names_la_SOURCES = src/cmd/names.cpp
m_names_la_CPPFLAGS = $(unrealircd_CPPFLAGS)
m_names_la_LIBADD = $(unrealircd_LDADD)
m_names_la_LDFLAGS = -module -avoid-version -shared

m_nick_la_SOURCES = src/cmd/nick.cpp
m_nick_la_CPPFLAGS = $(unrealircd_CPPFLAGS)
m_nick_la_LIBADD = $(unrealircd_LDADD)
m_nick_la_LDFLAGS = -module -avoid-version -shared

m_notice_la_SOURCES = src/cmd/notice.cpp
m_notice_la_CPPFLAGS = $(unrealircd_CPPFLAGS)
m_notice_la_LIBADD = $(unrealircd_LDADD)
m_notice_la_LDFLAGS = -module -avoid-version -shared

m_part_la_SOURCES = src/cmd/part.cpp
m_part_la_CPPFLAGS = $(unrealircd_CPPFLAGS)
m_part_la_LIBADD = $(unrealircd_LDADD)
m_part_la_LDFLAGS = -module -avoid-version -shared

m_ping_la_SOURCES = src/cmd/ping.cpp
m_ping_la_CPPFLAGS = $(unrealircd_CPPFLAGS)
m_ping_la_LIBADD = $(unrealircd_LDADD)
m_ping_la_LDFLAGS = -module -avoid-version -shared

m_pong_la_SOURCES = src/cmd/pong.cpp
m_pong_la_CPPFLAGS = $(unrealircd_CPPFLAGS)
m_pong_la_LIBADD = $(unrealircd_LDADD)
m_pong_la_LDFLAGS = -module -avoid-version -shared

m_privmsg_la_SOURCES = src/cmd/privmsg.cpp
m_privmsg_la_CPPFLAGS = $(unrealircd_CPPFLAGS)
m_privmsg_la_LIBADD = $(unrealircd_LDADD)
m_privmsg_la_LDFLAGS = -module -avoid-version -shared

m_quit_la_SOURCES = src/cmd/quit.cpp
m_quit_la_CPPFLAGS = $(unrealircd_CPPFLAGS)
m_quit_la_LIBADD = $(unrealircd_LDADD)
m_quit_la_LDFLAGS = -module -avoid-version -shared

m_rmmod_la_SOURCES = src/cmd/rmmod.cpp
m_rmmod_la_CPPFLAGS = $(unrealircd_CPPFLAGS)
m_rmmod_la_LIBADD = $(unrealircd_LDADD)
m_rmmod_la_LDFLAGS = -module -avoid-version -shared

m_topic_la_SOURCES = src/cmd/topic.cpp
m_topic_la_CPPFLAGS = $(unrealircd_CPPFLAGS)
m_topic_la_LIBADD = $(unrealircd_LDADD)
m_topic_la_LDFLAGS = -module -avoid-version -shared

m_user_la_SOURCES = src/cmd/user.cpp
m_user_la_CPPFLAGS = $(unrealircd_CPPFLAGS)
m_user_la_LIBADD = $(unrealircd_LDADD)
m_user_la_LDFLAGS = -module -avoid-version -shared

m_userhost_la_SOURCES = src/cmd/userhost.cpp
m_userhost_la_CPPFLAGS = $(unrealircd_CPPFLAGS)
m_userhost_la_LIBADD = $(unrealircd_LDADD)
m_userhost_la_LDFLAGS = -module -avoid-version -shared

m_version_la_SOURCES = src/cmd/version.cpp
m_version_la_CPPFLAGS = $(unrealircd_CPPFLAGS)
m_version_la_LIBADD = $(unrealircd_LDADD)
m_version_la_LDFLAGS = -module -avoid-version -shared

m_whois_la_SOURCES = src/cmd/whois.cpp
m_whois_la_CPPFLAGS = $(unrealircd_CPPFLAGS)
m_whois_la_LIBADD = $(unrealircd_LDADD)
m_whois_la_LDFLAGS = -module -avoid-version -shared
