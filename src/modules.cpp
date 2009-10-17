/*  UnrealIRCd -- The Next Generation IRCd.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 1, or (at your option)
 *  any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include "modules.h"
#include "misc.h"

#include <iostream>

extern "C"
{
	#include <dlfcn.h>
}

/*
 * ModularObjectInstantiater
 */
ModularObjectInstantiator::ModularObjectInstantiator(std::string name)
{
	this->name = name;
}

/* Encapsulation -- or no encapsulation? */
const std::string
ModularObjectInstantiator::getName()
{
	return name;
}

/*
 * ModularObject
 */
void
ModularObject::forceRTTIGeneration()
{
}



/*
 * Module
 */

bool
Module::exportObject(ModularObjectInstantiator* loader)
{
	for(std::vector<ModularObjectInstantiator*>::iterator i = instantiators.begin(); i != instantiators.end(); i ++)
		if(loader->getName() == (*i)->getName())
		{
			std::cerr << "Module " << moduleSpec << " attempted to export different version of " << (*i)->getName() << std::endl;
			return false;
		}
	instantiators.push_back(loader);
	return true;
}

Module&
Module::load(std::string moduleSpec) throw(std::runtime_error)
{
	void *dl_handle;
	unreal_mod_getinfo *infofunc;
	struct unreal_modinfo *modinfo;
	
	Module *newmodule;
	
	/*
	 * TODO: expand this to support guessing filename suffixes.
	 * Mabye we should force a filenameing convention on users :-D
	 *
	 * Stealth thinks maybe there should be a conf->DefaultModuleSuffix
	 * for modules. Users may then set .so or .dll and get lazy :D
	 *
	 * No lazy linking: we want to die with an undefined symbol
	 * as early as possible.
	 */
	dl_handle = dlopen(moduleSpec.c_str(), RTLD_NOW);
	if(!dl_handle)
		(*(int *)0) ++;

	infofunc = (unreal_mod_getinfo*) dlsym(dl_handle, "unreal_mod_getinfo");

	modinfo = infofunc();
	if(!modinfo)
		throw std::runtime_error("module ``" + moduleSpec + "'' returned NULL from unreal_mod_getinfo()");

	if(UNREAL_MODULE_ABI_VERSION != modinfo->unreal_module_abi_version)
		throw std::runtime_error("module ``" + moduleSpec + "'' has incompatible MODULE_ABI_VERSION; " 
					 + ConvertIntToString(UNREAL_MODULE_ABI_VERSION) + " != " 
					 + ConvertIntToString(modinfo->unreal_module_abi_version));

	newmodule = (Module *) modinfo->instantiate();
	
	/*
	  initialize our info about the module:
	 */
	newmodule->moduleSpec = moduleSpec;
	newmodule->dl_handle = dl_handle;

	newmodule->initialize();

	/*
	  TODO: after the module is successfully initialized, it is added to the list of loaded modules
	  so that this list can be enumerated later and to facilitate removal
	 */
	
}

/*
 * dl_handle is initialized by load()
 */
Module::Module()
	: dl_handle(NULL)
{
}
