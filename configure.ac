 #################################################################
 # Unreal Internet Relay Chat Daemon, Version 4
 # File         configure.ac
 # Description  Build-system setup and configuration meta-script
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

AC_PREREQ([2.13])
AC_INIT([unrealircd4], [4.0.0], [http://bugs.unrealircd.org/])
AC_CONFIG_SRCDIR([src/unreal.cpp])
AC_CONFIG_HEADERS([include/config.h])
AM_INIT_AUTOMAKE([1.11])

#
# VCS/UnrealVersion::package_changeset support:
# Users building from the repo must keep Mercurial
# installed so that their builds reveal the
# changeset identifier they are derived from.
#
AM_CONDITIONAL([VCS],[test -e .hg])
AS_IF([test -e .hg],[
	AC_DEFINE([HAVE_PACKAGE_CHANGESET], [1], [Define if PACKAGE_CHANGESET should be used])
])

# versioning
AC_DEFINE([PACKAGE_VERSION_MAJOR], [4], [Major version number])
AC_DEFINE([PACKAGE_VERSION_MINOR], [0], [Minor version number])
AC_DEFINE([PACKAGE_VERSION_RELEASE], [0], [Release number. i.e., bugfix release number])

# Checks for programs.
AC_PROG_CXX
AC_PROG_LIBTOOL
# needed for src/version.cpp / tools/verionblaster.sh
AC_PROG_MKDIR_P
AC_PROG_SED

# Checks for libraries/headers.

# ltdl, one portable way to provide dlopen()
# for now, this just assumes that the OS will provide
# a copy of ltdl
# 
# copied from http://flameeyes.eu/autotools-mythbuster/libtool/#libtool.plugins.ltdl
AC_CHECK_HEADER([ltdl.h],
  [AC_CHECK_LIB([ltdl], [lt_dladvise_init],
    [LIBLTDL=-lltdl], [LIBLTDL=])],
  [LIBLTDL=])
AS_IF([test -z "${LIBLTDL}"],
  [AC_MSG_ERROR([I need ltdl (provided by libtool)])])
AC_SUBST([LTDL_LIBS], [${LIBLTDL}])

# pthreads
# Must be checked early because crypto++ requires pthreads (and doesn't yet use pkg-config)
ACX_PTHREAD([],
	[
		AC_MSG_ERROR([I need pthreads])
	])
AC_SUBST([CC],[${PTHREAD_CC}])

# crypto++ ( http://cryptopp.org/ )
# https://sourceforge.net/tracker/?func=detail&aid=2929583&group_id=6152&atid=356152
# Use "cryptopp" instead of "crypto++", as the "++" thing is something invented by
# the Debian project.
AC_MSG_CHECKING([crypto++])
AC_LANG_PUSH([C++])
SAVED_LIBS="${LIBS}"
LIBS="${LIBS} -lcryptopp ${PTHREAD_LIBS}"
SAVED_CXXFLAGS="${CXXFLAGS}"
CXXFLAGS="${CXXFLAGS} ${PTHREAD_CFLAGS}"
AC_LINK_IFELSE(
  [AC_LANG_PROGRAM([[#include <cryptopp/hex.h>]], [[CryptoPP::HexEncoder he; return 0;]])],
  [AC_MSG_RESULT([yes])
	CRYPTOPP_LIBS="-lcryptopp"],
  [AC_MSG_RESULT([no])
	AC_MSG_ERROR([crypto++ not found. Get it at http://www.cryptopp.com])])
	
LIBS="${SAVED_LIBS}"
CXXFLAGS="${SAVED_CXXFLAGS}"
AC_LANG_POP([C++])

# boost modules
AX_BOOST_BASE([1.35])
AX_BOOST_ASIO
AX_BOOST_SIGNALS
AX_BOOST_SYSTEM

# c-ares
# before 1.5.*, there were API changes. Syzop consideres that
# 1.6.0 is a good minimum.
PKG_CHECK_MODULES([CARES], libcares >= 1.6.0)

# Common Makefile.am substitutions:
LIBS="${BOOST_ASIO_LIB} ${BOOST_SYSTEM_LIB} ${BOOST_SIGNALS_LIB} ${CRYPTOPP_LIBS} ${LTDL_LIBS} ${PTHREAD_LIBS} ${CARES_LIBS}"
AC_SUBST([AM_CPPFLAGS], ["${BOOST_CPPFLAGS} -DSYSCONFDIR='\"\$(sysconfdir)\"' -DPKGLIBDIR='\"\$(pkglibdir)\"'"])
AC_SUBST([AM_CXXFLAGS], ["${PTHREAD_CFLAGS} ${CARES_CFLAGS} -Wall -Wextra -Wno-unused"])

AC_CONFIG_FILES([Makefile
src/cmd/Makefile])
AC_OUTPUT
