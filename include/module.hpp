/*****************************************************************
 * Unreal Internet Relay Chat Daemon, Version 4
 * File         module.hpp
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

#ifndef _UNREALIRCD_MODULE_HPP
#define _UNREALIRCD_MODULE_HPP

#include <platform.hpp>
#include <string.hpp>
#include <iostream>
#include <ltdl.h>

#define UNREAL_DLL			extern "C"

/** module initialization symbol name */
#define MODULE_INIT_FN		"unrInit"

/** module close symbol name */
#define MODULE_CLOSE_FN		"unrClose"

/** default module API version */
#define MODULE_API_VERSION	4

/**
 * This class holds informations that are updated by the modules itself
 * on initialization.
 */
class UnrealModuleInf
{
public:
	UnrealModuleInf();
	uint8_t apiVersion();
	const String& author();
	const String& description();
	const String& name();
	void setAPIVersion(const uint8_t& api_v);
	void setAuthor(const String& author);
	void setDescription(const String& descr);
	void setName(const String& name);
	void setVersion(const String& version);
	const String& version();

private:
	/** API version number */
	uint8_t api_version_;

	/** Module Author; desired format: Name <email@host.xx> */
	String author_;

	/** Module Description */
	String description_;

	/** Module Name */
	String name_;

	/** Module Version String */
	String version_;
};

/**
 * Module class.
 */
class UnrealModule
{
public:
	/** enumerate module state */
	enum ModuleState { SNone, SError, SLoaded };

	/** enumerate module result */
	enum Result { Success, Failed };

	/** module initialization symbol type */
	typedef Result (InitFunc)(UnrealModule*);

	/** module close symbol type */
	typedef Result (CloseFunc)(UnrealModule*);

public:
	UnrealModule(const String& fname);
	~UnrealModule();

	const String& errorString();
	static int deinit();
	const String& fileName();
	static UnrealModule* find(const String& fname);
	lt_dlhandle handle();
	static int init();
	bool isError();
	bool isLoaded();
	bool load();
	void *resolve(const String& name);
	void setFileName(const String& fname);
	void setState(ModuleState state);
	ModuleState state();
	void unload();

public:
	UnrealModuleInf inf;

private:
	/** ltdl module loading ``advice'' */
	static lt_dladvise dlflags_;

	/** error string */
	String error_str_;

	/** file name */
	String filename_;

	/** library handle */
	lt_dlhandle handle_;

	/** module state */
	ModuleState state_;
};

#endif /* _UNREALIRCD_MODULE_HPP */
