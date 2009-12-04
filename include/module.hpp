/*****************************************************************
 * Unreal Internet Relay Chat Daemon, Version 4
 * File         module.hpp
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

#ifndef _UNREALIRCD_MODULE_HPP
#define _UNREALIRCD_MODULE_HPP

#include "platform.hpp"
#include "string.hpp"

#ifdef COMPILER_MSVC
#	define UNREAL_DLL extern "C" __declspec(dllexport)
#else
#	include <dlfcn.h>
#	define UNREAL_DLL extern "C"
#endif

/** module initialization symbol name */
#define MODULE_INIT_FN		"unrInit"

/** module close symbol name */
#define MODULE_CLOSE_FN		"unrClose"

/**
 * This class holds informations that are updated by the modules itself
 * on initialization.
 */
struct UnrealModuleInf
{
	/** module name info */
	String name;

	/** module version info */
	String version;

	/** module author info */
	String author;
};

/**
 * Module class.
 */
class UnrealModule
{
public:
	/** enumerate module state */
	enum ModuleState { SNone, SError, SLoaded };

	/** alias the Inf class */
	typedef UnrealModuleInf Info;

	/** generic handle type */
#ifdef WIN32
	typedef HMODULE Handle;
#else
	typedef void Handle;
#endif

	/** module initialization symbol type */
	typedef void (InitFunc)(UnrealModule&);

	/** module close symbol type */
	typedef void (CloseFunc)(UnrealModule&);

public:
	UnrealModule(const String& fname);
	~UnrealModule();

	const String& errorString();
	const String& fileName();
	Handle* handle();
	bool isError();
	bool isLoaded();
	bool load();
	Handle* resolve(const String& name);
	void setFileName(const String& fname);
	ModuleState state();
	void unload();

public:
	Info info;

private:
	/** error string */
	String error_str_;

	/** file name */
	String filename_;

	/** library handle */
	Handle* handle_;

	/** module state */
	ModuleState state_;
};

#endif /* _UNREALIRCD_MODULE_HPP */