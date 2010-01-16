/*****************************************************************
 * Unreal Internet Relay Chat Daemon, Version 4
 * File         module.cpp
 * Description  An object wrapping a loaded module
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

#include "base.hpp"
#include "module.hpp"
#include <assert.h>
#include <iostream>
#include <dlfcn.h>

/**
 * lt_dladvise
 */
lt_dladvise UnrealModule::dlflags_;

/**
 * UnrealModule constructor.
 *
 * @param fname Filename of module to load
 */
UnrealModule::UnrealModule(const String& fname)
	: filename_(fname), handle_(0), state_(SNone)
{
	if (!filename_.empty())
		load();
}

/**
 * UnrealModule destructor.
 */
UnrealModule::~UnrealModule()
{
	std::cout<<"~UnrealModule\n";
	unload();
}

/**
   Deinitializes dynamic linker.

   Must be called once as the program is exiting after all
   dynamically loaded objects are unloaded.
*/
int UnrealModule::deinit()
{
	String msg;
	int myerr;

	myerr = 0;
	if(lt_dlexit() != 0)
	{
		unreal->log.write(UnrealLog::Error, "Unable to deinitialize ltdl: %s",
			lt_dlerror());
		myerr ++;
	}

	/** see UnrealModule::init()
	    -- don't try to destroy a NULL dlflags_
	 */	
	if(!dlflags_)
		return myerr;

	if(lt_dladvise_destroy(&dlflags_) != 0)
	{
		unreal->log.write(UnrealLog::Error, "Unable to deinitialize ltdl's "
			"advice: %s", lt_dlerror());
		myerr ++;
	}
	return myerr;
}

/**
 * Returns the error string, if any.
 *
 * @return Error message, or empty string if no errors were encountered
 */
const String& UnrealModule::errorString()
{
	return error_str_;
}

/**
 * Returns the file name of the module.
 *
 * @return Module file name
 */
const String& UnrealModule::fileName()
{
	return filename_;
}

/**
 * Lookup a Module by filename.
 *
 * @param fname Filename of module to lookup
 * @return Pointer to UnrealModule, or 0 when not found
 */
UnrealModule* UnrealModule::find(const String& fname)
{
	for (List<UnrealModule*>::Iterator i = unreal->modules.begin();
			i != unreal->modules.end(); ++i)
	{
		if ((*i)->fileName() == fname)
			return *i;
	}

	return 0;
}

/**
 * Returns the library handle.
 */
lt_dlhandle UnrealModule::handle()
{
	return handle_;
}

/**
   Must be called before the UnrealModule class
   may be used. But must only be called once.
*/
int UnrealModule::init()
{
	String msg;

	if(lt_dlinit() != 0)
	{
		unreal->log.write(UnrealLog::Fatal, "Unable to initialize ltdl: %s",
			lt_dlerror());
	}
	if(lt_dladvise_init(&dlflags_) != 0)
	{
		dlflags_ = (lt_dladvise)NULL;

		unreal->log.write(UnrealLog::Fatal, "Unable to initialize ltdl's "
			"advice: %s", lt_dlerror());
	}
	/** Aim at the equivlients for dlopen()'s RTLD_NOW | RTLD_GLOBAL */
	if(lt_dladvise_global(&dlflags_) != 0)
	{
		/** There's a chance we can work without this */
		unreal->log.write(UnrealLog::Error, "Error setting RTDL_GLOBAL in "
			"ld_dladvise(): %s", lt_dlerror());

		return 1 + deinit();
	}

	return 0;
}

/**
 * Returns whether the module got an error.
 */
bool UnrealModule::isError()
{
	return (state_ == SError);
}

/**
 * Returns whether the module is loaded.
 */
bool UnrealModule::isLoaded()
{
	return (state_ == SLoaded);
}

/**
 * Tries to open the module.
 *
 * @return True when module was loaded successfully, otherwise false
 */
bool UnrealModule::load()
{
	if (filename_.empty())
		setState(SNone);
	else if (UnrealModule::find(filename_))
	{
		error_str_ = "Module already loaded";
		setState(SError);
	}
	else if ( !(handle_ = lt_dlopenadvise(filename_.c_str(), dlflags_)) )
	{
		error_str_ = lt_dlerror();
		setState(SError);
	}
	else
	{
		setState(SLoaded);

		InitFunc* initfn = (InitFunc*)resolve(MODULE_INIT_FN);

		if (!initfn)
		{
			error_str_.sprintf("Entry symbol \"%s\" not found", MODULE_INIT_FN);
			setState(SError);

			lt_dlclose(handle_);
		}
		else
		{
			if (initfn(this) != Success)
			{
				error_str_.sprintf("Init function did not succeed");
				state_ = SError;

				lt_dlclose(handle_);
			}
		}
	}

	return isLoaded();
}

/**
 * Returns the address of a specified symbol in the module file.
 *
 * @param name Name of the symbol to resolve address for
 * @return Address of the symbol, or 0 if the symbol cannot be located or the
 * module isn't loaded
 */
void *UnrealModule::resolve(const String& name)
{
	if (!isLoaded())
		return 0;
	else
	{
		void* sym = lt_dlsym(handle_, name.c_str());

		if (!sym)
			error_str_ = lt_dlerror();

		return sym;
	}
}

/**
 * Set the file name.
 *
 * @param fname New filename
 */
void UnrealModule::setFileName(const String& fname)
{
	filename_ = fname;
}

/**
 * Update the module state.
 *
 * @param state New state
 */
void UnrealModule::setState(ModuleState state)
{
	state_ = state;
}

/**
 * Returns the module state.
 *
 * - SNone   when the module is not loaded yet
 * - SError  when an error occured; use errorString() to get the error message
 * - SLoaded when the module is loaded
 *
 * @return ModuleState
 */
UnrealModule::ModuleState UnrealModule::state()
{
	return state_;
}

/**
 * Unloads the module.
 */
void UnrealModule::unload()
{
std::cout<<String::format("UnrealModule::unload(%p) [%s]\n", handle_,
	filename_.c_str());
	if (isLoaded())
	{
		CloseFunc* clfn = reinterpret_cast<CloseFunc*>(resolve(MODULE_CLOSE_FN));

		if (clfn)
		{
			if (clfn(this) != Success)
			{
				unreal->log.write(UnrealLog::Normal,
						"UnrealModule::unload(): "
						"Close function didn't return Success");
			}
		}

		lt_dlclose(handle_);
		setState(SNone);
	}
}

// ---- UnrealModuleInf class ----

/**
 * ModuleInf constructor.
 */
UnrealModuleInf::UnrealModuleInf()
	: api_version_(MODULE_API_VERSION)
{ }

/**
 * Returns the module's API version.
 *
 * @return API version
 */
uint8_t UnrealModuleInf::apiVersion()
{
	return api_version_;
}

/**
 * Returns the module's Author.
 *
 * @return Author
 */
const String& UnrealModuleInf::author()
{
	return author_;
}

/**
 * Return the module's descriptive Information.
 *
 * @return Description
 */
const String& UnrealModuleInf::description()
{
	return description_;
}

/**
 * Return the module's name.
 *
 * @return Name
 */
const String& UnrealModuleInf::name()
{
	return name_;
}

/**
 * Update the module's API version.
 *
 * @param api_v New API version
 */
void UnrealModuleInf::setAPIVersion(const uint8_t& api_v)
{
	api_version_ = api_v;
}

/**
 * Update the module's Author.
 *
 * @param author Author name
 */
void UnrealModuleInf::setAuthor(const String& author)
{
	author_ = author;
}

/**
 * Update the module's Description
 *
 * @param descr New description
 */
void UnrealModuleInf::setDescription(const String& descr)
{
	description_ = descr;
}

/**
 * Update the module's Name.
 *
 * @param name New name
 */
void UnrealModuleInf::setName(const String& name)
{
	name_ = name;
}

/**
 * Update the module's version.
 *
 * @param version Version string
 */
void UnrealModuleInf::setVersion(const String& version)
{
	version_ = version;
}

/**
 * Return the module's Version.
 *
 * @return Module version
 */
const String& UnrealModuleInf::version()
{
	return version_;
}
