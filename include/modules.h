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

/**
   This file contains everything a module needs to be created.
   see src/modules/m_null.cpp for an example of how to make a module.
 */

#include "module.h"

#include <vector>
#include <string>

#define UNREAL_MODULE_ABI_VERSION 1

/*
  Stub to allow compilation. This macro's necessity is not decided upon.
 */
#define MODULARIZE_FUNCTION(hook,call)

enum ModuleTypes
{
	/**
	   Marks official modules
	 */
	OFFICIAL
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
	typedef void *instantiate_t();
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
		instantiate_t *instantiate;
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

public:
	virtual ~ModularObject();
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
	virtual ~ModularObjectInstantiator();

	ModularObjectInstantiator(std::string name);
	
	/* Encapsulation -- or no encapsulation? */
	const std::string getName();
	virtual ModularObject *instantiateObject() = 0;
};


/**
  macros for module files:
 */

/**
   a module file must call this so that n is the name of the module
   (without quotes)

   a ## b concatenates two strings in C Preprocessing language
 */
#define UNREALIRCD_MODULE(n)						\
struct unreal_modinfo modinfo_ ## n = {					\
	.unreal_module_abi_verson = UNREAL_MODULE_ABI_VERSION,		\
	.instantiate = &m_ ## n::					\
};									\
									\
extern "C" {								\
	struct unreal_modinfo *unreal_mod_getinfo()			\
	{								\
		return &modinfo ## n ;					\
	}								\
}

#endif
