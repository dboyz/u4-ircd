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

#ifndef _UNREALIRCD_CONFIG_HPP
#define _UNREALIRCD_CONFIG_HPP

#include "map.hpp"
#include "string.hpp"
#include "stringlist.hpp"

/** max length per config line for buffer */
#define CONFIG_LINE_LENGHT_LIMIT	1024

/** default config file */
#define CONFIG_DEFAULT_FILE			"../etc/unreal.conf"

class UnrealConfig
{
public:
	UnrealConfig();
	String fileName();
	String get(const String& key, const String& def);
	size_t getLastIndex(const String& key);
	bool getQuotedContent(String& inp, String& val);
	String getSeqVal(const String& key, int index, const String& name,
		const String& def);
	String getVarContent(const String& key, const String& category);
	Map<String, String> map();
	StringList moduleList();
	bool read(const String& file);
	bool replaceVars(String& str, String& ret, const String& category);
	size_t sequenceCount(const String& blk);
	void setFileName(const String& file);
	void startRead();
	uint32 warnings();

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
	uint32 warnings_;
};

#endif /* _UNREALIRCD_CONFIG_HPP */
