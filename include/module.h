/*  UnrealIRCd -- The Next Generation IRCd.
*
*   This program is free software; you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation; either version 1, or (at your option)
*   any later version.
*
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with this program; if not, write to the Free Software
*   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

class Module;

#ifndef _UNREALIRCD_MODULE_H
#define _UNREALIRCD_MODULE_H

#include "ircd.h"

#include <stdexcept>
#include <string>

extern "C"
{
#include <ltdl.h>
}

/**
   parent class of all modules

   API for the loading and unloading of modules
 */
class Module
{
public:
	/*
	 * Loads module specified by moduleSpec. This prepares
	 * the configuration engine to feed the module, but it
	 * isn't started until start() is called.
	 *
	 * This may fail if the module is incompatible with this
	 * compilation of UnrealIRCD-CPP
	 */
	static Module* load(IRCd&, std::string moduleSpec) throw(std::runtime_error);

	/*
	 * This activates a module.
	 *
	 * This is where a module should install hooks. 
	 * This is where a module pushes itself into the IRCd --
	 * whether or not it uses hooks.
	 *
	 * This function may only be called once for the lifetime
	 * of a module; the caller is responsible for enforcing
	 * this constraint. (changes to this portion of the
	 * interface will have to be made to support rehashing
	 * without reloading all modules)
	 */
	virtual void run() = 0;

	/**
	 * unloads specified module
	 * @return true on success
	 */
	static bool unload(IRCd&, Module *);

protected:
	Module();
	virtual ~Module();
	
	/*
	 * This is for the module to set itself up. This is only to
	 * be called by Module::load().
	 *
	 * This is where the module registers itself with the configuration
	 * engine.
	 *
	 * must only throw std::runtime_error because is called from Module::load()
	 */
	virtual void initialize(Conf&) throw(std::runtime_error) = 0;

	/*
	 * Cleans up a module to prepare it for closing. This is 
	 * only to be called by Module::close(). Returns true if successful.
	 *
	 * This is where a module should unhook itself, extract references
	 * to itself from the IRCd object, and remove configuration parameters
	 *
	 * Unsuccessful cleanup()s may be caused by the module being
	 * used. i.e., an object may belong to a class of this module,
	 * or ... hopefully nothing
	 */
	virtual bool cleanup() = 0;

private:
	/*
	 * The name this module is referred to as in the loadmodule "" config command:
	 */
	std::string moduleSpec;
	
	/*
	 * The pointer returned by dlopen or LoadLibrary. For
	 * running dlclose();
	 * 
	 * Initialized by load();
	 */
	lt_dlhandle ltdl_handle;
};

#endif /* _UNREALIRCD_MODULE_H */
