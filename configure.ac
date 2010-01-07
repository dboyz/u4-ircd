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
AC_INIT([unrealircd-cpp], [4.0.0], [http://bugs.unrealircd.org/])
AC_CONFIG_SRCDIR([src/unreal.cpp])
AC_CONFIG_HEADERS([include/config.h])
AM_INIT_AUTOMAKE([1.11])

# VCS or includes/hgrev.hpp
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
AC_SUBST([LTDL_LIBS], [${LIBLTDL}])

# pthreads
ACX_PTHREAD([],
	[
		AC_MSG_ERROR([I need pthreads])
	])
AC_SUBST([CC],[${PTHREAD_CC}])

#
# epoll()/select() implementation choice
#

# epoll()
AC_ARG_ENABLE([epoll],
	[AS_HELP_STRING([--enable-epoll], [Choose epoll() for the server's socket ``reactor'' engine (default)])],
	[enable_epoll=$enableval],
	[enable_epoll=maybe])
AX_HAVE_EPOLL([have_epoll=yes],
	[have_epoll=no
	AS_IF([test "x$enable_epoll" = "xyes"], 
		AC_MSG_ERROR([epoll() ``reactor'' selected but not available]))	
	])

# foo()

AC_MSG_CHECKING([which socket ``reactor'' engine to use])
# choose impl
AS_IF([test "x$enable_foo" = "xyes" && test "x$have_foo" = "xyes"],
		[AC_DEFINE([EVREACT_FOO],[1], [Choose foo as a reactor engine])
			reactor="foo"],
	[test "x$enable_epoll" != "xno" && test "x$have_epoll" = "xyes"],
		[AC_DEFINE([EVREACT_EPOLL],[1], [Choose epoll() as a reactor engine])
			reactor="epoll()"],
	[AC_MSG_ERROR([All potential reactors are disabled. Try to enable a reactor such as with --enable-epoll.])])
AC_MSG_RESULT([$reactor])

# boost modules -- supposedly to be removed soon, possibly temporarily
AX_BOOST_BASE([1.39])
AX_BOOST_ASIO
AX_BOOST_SYSTEM

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL

# Checks for library functions.

AC_CONFIG_FILES([Makefile
src/cmd/Makefile])
AC_OUTPUT
