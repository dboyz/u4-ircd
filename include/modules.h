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

#ifndef _UNREALIRCD_MODULES_H
#define _UNREALIRCD_MODULES_H

#include <vector>
#include <string>

enum ModuleTypes
{
	/* If a module type is STATIC, it CANNOT be unloaded by ANY
	*  means. We should use this for core modules. --David Kingston
	*/
	STATIC,
	/* If a module type is NOTVENDOR, it is treated as any other
	*  module, but it is defined as 3rd party. (i.e not from us)
	*/
	NOTVENDOR,
	/* If a module type is VENDOR, it is treated as any other
	*  module, but it is defined as it came with this
	*  distribution.
	*/
	VENDOR
};

/*
 * Every module must have these C-style functions. The let us probe it
 * without needing to deal with mangled junk.
 */
extern "C" 
{
	/* 
	 * This function returns a string with enough information
	 * to tell us if it is compatible with this compilation of
	 * UnrealIRCD-CPP
	 */
	typedef char *unreal_mod_getinfo();
	/*
	 * This function returns a pointer to a Module object
	 * which must be cast to a Module object.
	 */
	typedef void *unreal_mod_instantiate();
}

/*
 * The parent class of all objects which are able to
 * instantiate an object for the use of Module::getObject().
 *
 * This is _not_ the parent class of (hopefully) _any_ of the 
 * object instantiated by Module::getObject(). This is just
 * to store the name and a function which returns a nice, shiny
 * (like KVIRC) object for us.
 */
class ModularObjectInstantiater
{
private:
	std::string name;
public:
	ModularObjectInstantiater(std::string name);
	
	/* Encapsulation -- or no encapsulation? */
	const std::string getName();
	virtual void *instantiateObject() = 0;
}

/*
 * The parent class of all modules.
 */
class Module
{
private:
	/*
	 * The name this module is referred to as:
	 */
	std::string name;
	
	/*
	 * The pointer returned by dlopen or LoadLibrary. For
	 * running dlclose();
	 * 
	 * Initialized by load();
	 */
	void *dl_handle;

	/*
	 * for the use of getObject(); -- to store objectInstantiators
	 * to allow us to instantiate arbitrary objects
	 */
	std::vector<ModularObjectInstantiater> instantiaters;

protected:
	/*
	 * This is for the module to set itself up. This is only to
	 * be called by Module::load().
	 *
	 * This is where the module registers itself with the configuration
	 * engine.
	 *
	 * TODO: should an IRCd object be passed to initilize or will that
	 * be a global variable?
	 */
	virtual void initialize() = 0;

	/*
	 * Cleans up a module to prepare it for closing. This is 
	 * only to be called by Module::close(). Returns true if successful.
	 *
	 * This is where a module should unhook itself
	 *
	 * Unsuccessful cleanup()s may be caused by the module being
	 * used. i.e., an object may belong to a class of this module,
	 * or ...
	 */
	virtual bool cleanup() = 0;

	/*
	 * Modules should call this function to export an object
	 * for use by the Module::getObject() method. Modules
	 * have no need to unexport objects.
	 *
	 * returns false on error
	 */
	bool exportObject(ModularObjectInstantiater& loader);

public:
	Module();
	/*
	 * Loads module specified by moduleSpec. This prepares
	 * the configuration engine to feed the module, but it
	 * isn't started until start() is called.
	 *
	 * This may fail if the module is incompatible with this
	 * compilation of UnrealIRCD-CPP
	 */
	static Module& load(std::string moduleSpec);

	/*
	 * This activates a module.
	 *
	 * This is where a module should install hooks. 
	 */
	virtual void run() = 0;

	/*
	 * Gets an object that this module provides. This is, for example
	 * used by SocketEngine::create()
	 */
	void *getObject(std::string);
	
	/*
	 * Unloads a module. Will fail if the module is STATIC.
	 */
	void close();
}

#endif
