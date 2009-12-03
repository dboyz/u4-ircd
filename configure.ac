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

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL

# Checks for library functions.

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
