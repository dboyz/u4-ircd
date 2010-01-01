/*****************************************************************
 * Unreal Internet Relay Chat Daemon, Version 4
 * File         config.hpp
 * Description  An object for configuration storage and loading
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

#ifndef _UNREALIRCD_CONFIG_HPP
#define _UNREALIRCD_CONFIG_HPP

#include "map.hpp"
#include "string.hpp"
#include "stringlist.hpp"

/** max length per config line for buffer */
#define CONFIG_LINE_LENGHT_LIMIT	1024

/** default config file */
#define CONFIG_DEFAULT_FILE			"../etc/unreal.conf"
#define CONFIG_ITEM_SEPARATOR		"::"

class UnrealConfig
{
public:
	UnrealConfig();
	const String& fileName();
	String get(const String& key, const String& def = String());
	size_t getLastIndex(const String& key);
	bool getQuotedContent(String& inp, String& val);
	String getSeqVal(const String& key, int index, const String& name,
		const String& def);
	String getVarContent(const String& key, const String& category);
	void initDefaults();
	Map<String, String> map();
	StringList moduleList();
	bool read(const String& file);
	bool replaceVars(String& str, String& ret, const String& category);
	size_t sequenceCount(const String& blk);
	void set(const String& key, const String& value);
	void setFileName(const String& file);
	void startRead();
	uint32_t warnings();

private:
	/** config table entries */
	Map<String, String> entries_;

	/** additional config files */
	StringList files_;

	/** module file list */
	StringList modules_;

	/** sequence list */
	StringList sequences_;

	/** filename of initial config file */
	String filename_;

	/** number of warnings encountered on parsing */
	uint32_t warnings_;
};

#endif /* _UNREALIRCD_CONFIG_HPP */
