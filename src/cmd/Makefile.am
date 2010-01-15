 #################################################################
 # Unreal Internet Relay Chat Daemon, Version 4
 # File         Makefile.am
 # Description  Description of dist modules to build and install
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

AM_DEFAULT_SOURCE_EXT = .cpp

# http://www.flameeyes.eu/autotools-mythbuster/libtool/index.html#libtool.plugins.dlopen
AM_LDFLAGS = -module -avoid-version -shared $(BOOST_LDFLAGS)
INCLUDES = -I$(top_srcdir)/include

pkglib_LTLIBRARIES = \
	admin.la \
	away.la \
	help.la \
	info.la \
	insmod.la \
	invite.la \
	ison.la \
	join.la \
	kick.la \
	kill.la \
	list.la \
	lsmod.la \
	lusers.la \
	mode.la \
	motd.la \
	names.la \
	nick.la \
	notice.la \
	oper.la \
	part.la \
	ping.la \
	pong.la \
	privmsg.la \
	quit.la \
	rehash.la \
	rmmod.la \
	topic.la \
	user.la \
	userhost.la \
	version.la \
	who.la \
	whois.la
