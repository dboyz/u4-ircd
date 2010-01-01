/*****************************************************************
 * Unreal Internet Relay Chat Daemon, Version 4
 * File         version.hpp
 * Description  Server version identifiers
 *
 * Copyright(C) 2009, 2010
 * The UnrealIRCd development team and contributors
 * http://www.unrealircd.com
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 3 of
 * the License, or (at your option) any later version.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program; if not, write to the Free
 * Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 ******************************************************************/

#ifndef _UNREALIRCD_VERSION_HPP
#define _UNREALIRCD_VERSION_HPP

/**
   Most definitions that would be found in this file
   are defined in configure.ac. The defined variables include:
   
   Integer macros testable by modules:
   PACKAGE_VERSION_MAJOR
   PACKAGE_VERSION_MINOR
   PACKAGE_VERSINO_RELEASE

   Generic, possibly useful macros:
   PACKAGE the name of this package
   PACKAGE_VERSION the version as a string
 */

/**
   Do not confuse config.h with config.hpp.
   This config.h is the information that ./configure
   transfers to us.
*/
extern "C"
{
#include "config.h"
}

/**
   The PACKAGE_CHANGESET macro is available if the HAVE_PACKAGE_CHANGESET
   macro is defined. You must confirm that HAVE_PACKAGE_CHANGESET is set
   before just using PACKAGE_CHANGESET because releases will not need or
   use the PACKAGE_CHANGESET macro because they are...  releases!
 */
#ifdef HAVE_PACKAGE_CHANGESET
#include "hgrev.hpp"
#endif

/**
   A replacement for PACKAGE_STRING which contains no spaces
 */
#define PACKAGE_VERSTR 		PACKAGE_TARNAME "-" PACKAGE_VERSION
#define PACKAGE_PATCHLEVEL	"+alpha_hgrev-tip"

#endif /* _UNREALIRCD_VERSION_HPP */
