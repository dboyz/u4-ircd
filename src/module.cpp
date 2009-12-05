/*****************************************************************
 * Unreal Internet Relay Chat Daemon, Version 4
 * File         module.cpp
 * Description  An object wrapping a loaded module
 *
 * All parts of this program are Copyright(C) 2009 by their
 * respective authors and the UnrealIRCd development team.
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
	unload();
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
			i != unreal->modules.end(); i++)
	{
		if ((*i)->fileName() == fname)
			return *i;
	}

	return 0;
}

/**
 * Returns the library handle.
 */
void* UnrealModule::handle()
{
	return handle_;
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
		state_ = SNone;
	else if (UnrealModule::find(filename_))
	{
		error_str_ = "Module already loaded";
		state_ = SError;
	}
	else if (!(handle_ = dlopen(filename_.c_str(), RTLD_NOW | RTLD_GLOBAL)))
	{
		error_str_ = dlerror();
		state_ = SError;
	}
	else
	{
		state_ = SLoaded;

		InitFunc* initfn = (InitFunc*)resolve(MODULE_INIT_FN);

		if (!initfn)
		{
			error_str_.sprintf("Entry symbol \"%s\" not found", MODULE_INIT_FN);
			state_ = SError;

			dlclose(handle_);
			handle_ = 0;
		}
		else
		{
			if (initfn(*this) != Success)
			{
				error_str_.sprintf("Init function did not succeed");
				state_ = SError;

				dlclose(handle_);
				handle_ = 0;
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
void* UnrealModule::resolve(const String& name)
{
	if (!isLoaded())
		return 0;
	else
	{
		void* sym = dlsym(handle_, name.c_str());

		if (!sym)
			error_str_ = dlerror();

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
	if (isLoaded() && handle_)
	{
		CloseFunc* clfn = reinterpret_cast<CloseFunc*>(resolve(MODULE_CLOSE_FN));

		if (clfn)
		{
			if (clfn(*this) != Success)
			{
				unreal->log.write(UnrealLog::Normal,
						"UnrealModule::unload(): "
						"Close function didn't return Success");
			}
		}

		dlclose(handle_);
		handle_ = 0;
		state_ = SNone;
	}
}
