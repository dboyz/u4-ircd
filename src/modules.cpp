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

#include "module.h"

/* unportable */
export "C"
{
#include <dlfcn.h>
}

/*
 * ModularObjectInstantiater
 */
ModularObjectInstantiater::ModularObjectInstantiater(std::string name)
{
	this->name = name;
}

/* Encapsulation -- or no encapsulation? */
std::string
ModularObjectInstantiater::getName()
{
	return name;
}

/*
 * Module
 */

bool
Module::exportObject(ModularObjectInstantiator loader)
{
	for(std::vector<ModularObjectInstantiator>::iterator i = instantiators.begin; i != instantiators.end; i ++)
		if(loader.getName() == i->getName())
		{
			std:err << "Module" << name << "Attempted to export different version of " << (i->getName()) << std::endl;
			return false;
		}
	instantiaters.push_back(loader);
	return true;
}

Module&
load(std::string moduleSpec)
{
	/*
	 * TODO: expand this to support guessing filename suffixes.
	 * Mabye we should force a filenameing convention on users :-D
	 *
	 * No lazy linking: we want to die with an undefined symbol
	 * as early as possible.
	 */
	void *dl_handle = dlopen(moduleSpec, RTLD_NOW);
	if(!dl_handle)
		(*(int *)0) ++;
	/*
	 * ...
	 * I shouldn't implement any more of my module interface until 
	 * it gets approval and is argued about in #unreal-devel
	 */
}

/*
 * dl_handle is initialized by load()
 */
Module
Module::Module()
	: dl_handle(NULL)
{
}
