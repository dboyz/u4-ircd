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
AC_INIT([unrealircd-cpp], [0.1], [http://bugs.unrealircd.org/])
AC_CONFIG_SRCDIR([src/unreal.cpp])
AC_CONFIG_HEADERS([include/config.h])
AM_INIT_AUTOMAKE

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
  [AC_CHECK_LIB([ltdl], [lt_dladdsearchdir],
    [LIBLTDL=-lltdl], [LIBLTDL=])],
  [LIBLTDL=])
AC_SUBST([LTDL_LIBS], [${LIBLTDL}])

# boost modules
AX_BOOST_BASE([1.39])
AX_BOOST_ASIO
AX_BOOST_SYSTEM
AX_BOOST_THREAD

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL

# Checks for library functions.

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
