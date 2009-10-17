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
#include <stdexcept>

#define UNREAL_MODULE_ABI_VERSION 1

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

/* Module enums for the MODULARIZE_FUNCTION macro.
*  You may add the I_* here if you make a new hook.
*/

enum ModuleHooks
{
	I_OnValidChannelCheck,
	I_OnMOTDSend,
	I_OnFailedMOTD,
	I_OnSendRaw
};

/*
 * Every module must have ths C-style function. This lets us probe it
 * without needing to deal with mangled functions, etc.
 */
extern "C" 
{
	/*
	  The data the probing function must provide.
	 */
	struct unreal_modinfo {
		unsigned int unreal_module_abi_version;

		/*
		 * This is a pointer to a function the must return a pointer to
		 * a Module object. This module must register all hooks and objects 
		 * it provides before returning from this function.
		 */
		void *instantiate();
	};

	/* 
	 * This function returns a string with enough information
	 * to tell us if it is compatible with this compilation of
	 * UnrealIRCD-CPP and to then instantiate its module class.
	 */
	typedef struct unreal_modinfo *unreal_mod_getinfo();
}

/*
 * The parent class of all ModularObjects that can be instantiated 
 * by ModularObjectInstantiator.
 * Any methods needed in here may be added later. Currently, this
 * is just because dynamic_cast<>() can't convert void* to Object*
 */
class ModularObject
{
private:
	/*
	 * An object apparently needs a virtual method
	 * before RTTI is generated for it by the compiler.
	 */
	virtual void forceRTTIGeneration();
};

/*
 * The parent class of all objects which are able to
 * instantiate an object for the use of Module::getObject().
 *
 * This is _not_ the parent class of (hopefully) _any_ of the 
 * object instantiated by Module::getObject(). This is just
 * to store the name and a function which returns a nice, shiny
 * (like KVIRC) object for us.
 */
class ModularObjectInstantiator
{
private:
	std::string name;
public:
	ModularObjectInstantiator(std::string name);
	
	/* Encapsulation -- or no encapsulation? */
	const std::string getName();
	virtual ModularObject *instantiateObject() = 0;
};

/*
 * The parent class of all modules.
 */
class Module
{
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
	void *dl_handle;

	/*
	 * for the use of getObject(); -- to store objectInstantiators
	 * to allow us to instantiate arbitrary objects
	 */
	std::vector<ModularObjectInstantiator*> instantiators;

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
	 * TODO: should an IRCd object be passed to initilize or will that
	 * be a global variable?
	 *
	 * must only throw std::runtime_error because is called from Module::load()
	 */
	virtual void initialize() throw(std::runtime_error) = 0;

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
	 * You must use new to instantiate the loader, we'll
	 * delete it for you.
	 *
	 * returns false on error
	 */
	bool exportObject(ModularObjectInstantiator* loader);

public:
	/*
	 * Loads module specified by moduleSpec. This prepares
	 * the configuration engine to feed the module, but it
	 * isn't started until start() is called.
	 *
	 * This may fail if the module is incompatible with this
	 * compilation of UnrealIRCD-CPP
	 */
	static Module& load(std::string moduleSpec) throw(std::runtime_error);

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
	ModularObject *getObject(std::string);
	
	/*
	 * Unloads a module. Will fail if the module is STATIC.
	 */
	void close();
};

#endif
