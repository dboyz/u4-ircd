/*
 *  UnrealIRCd, the next generation IRCd.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

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
#ifdef WIN32
	else if (!(handle_ = LoadLibrary(filename_.c_str())))
	{
		DWORD lastErr = GetLastError();

		/* TODO: someone has to convert lastErr to a readable string using
		 * FormatString(). I'm not familiar with MSVC stuff --commx
		 */
		error_str_ = "Unknown error";
		state_ = SError;
	}
#else
	else if (!(handle_ = dlopen(filename_.c_str(), RTLD_NOW | RTLD_GLOBAL)))
	{
		error_str_ = dlerror();
		state_ = SError;
	}
#endif
	else
	{
		InitFunc* init = reinterpret_cast<InitFunc*>(resolve(MODULE_INIT_FN));

		if (!init)
		{
			error_str_.sprintf("Entry symbol \"%s\" not found", MODULE_INIT_FN);
			state_ = SError;
		}
		else
		{
			init(*this);
			state_ = SLoaded;
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
#ifdef WIN32
		void* sym = reinterpret_cast<void*>(GetProcAddress(handle_, name.c_str()));

		if (!sym)
		{
			DWORD lastErr = GetLastError();

			/* TODO: as written above, someone has to do this. */
			error_str_ = "Unknown error";
		}
#else
		void* sym = dlsym(handle_, name.c_str());

		if (!sym)
			error_str_ = dlerror();
#endif

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
			clfn(*this);

		dlclose(handle_);
		handle_ = 0;
		state_ = SNone;
	}
}
