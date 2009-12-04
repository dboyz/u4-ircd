/*****************************************************************
 * Unreal Internet Relay Chat Daemon, Version 4
 * File         config.cpp
 * Description  An object for configuration storage and loading
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

#include "config.hpp"
#include <fstream>
#include <iostream>
#include <cstdlib>

/**
 * Full documentation of the configuration system can be found at
 * http://wiki.commx.ws/wiki/Neonbox/Config
 */

/**
 * Config constructor.
 */
UnrealConfig::UnrealConfig()
	: filename_(CONFIG_DEFAULT_FILE), warnings_(0)
{ }

/**
 * Returns the initial configuration file name.
 *
 * @return Configuration filename
 */
const String& UnrealConfig::fileName()
{
	return filename_;
}

/**
 * Returns a configuration item.
 *
 * @param key Configuration key to lookup
 * @param def Default value to return if the configuration key cannot be found
 * @return Value of the config item, otherwise the default value
 */
String UnrealConfig::get(const String& key, const String& def)
{
	if (entries_.contains(key))
		return entries_[key];
	else
		return def;
}

/**
 * Returns the last index ID for the specified sequence key.
 *
 * @param key Sequence key to look up.
 * @return Last Index ID
 */
size_t UnrealConfig::getLastIndex(const String& key)
{
	size_t index = 1;
	String strKey = key + "_#SEQ-" + String(index) + "#";

	for (Map<String, String>::Iterator i = entries_.begin();
			i != entries_.end(); i++)
	{
		String current = i->first;

		if (current.left(strKey.length()) == strKey)
		{
			index++;
			strKey = key + "_#SEQ-" + String(index) + "#";
		}
	}

	return (index - 1);
}

/**
 * Runs through a string that may contain quotes. That quotes are removed
 * in the final string. Semicolons are treated as the end of the line.
 *
 * @param inp Input string
 * @param val Output string
 * @return Returns true if all opened quotes are closed, otherwise false
 */
bool UnrealConfig::getQuotedContent(String& inp, String& val)
{
	bool has_quote = false;

	val.clear();

	for (String::Iterator siter = inp.begin(); siter != inp.end(); siter++)
	{
		if (*siter == '\\')
		{
			/* don't remove special characters when escaped */
			if (*(siter + 1) == '"' || *(siter + 1) == ';'
					|| *(siter + 1) == '\\')
			{
				val.append(1, *(++siter));
				continue;
			}
		}
		else if (*siter == '"')
		{
			if (siter != inp.begin() && *(siter - 1) == '\\')
				continue;

			has_quote = !has_quote;
		}
		else if (*siter == ';' && !has_quote)
		{
			if (siter != inp.begin() && *(siter - 1) == '\\')
				continue;

			break; // marks EOL
		}
		else
		{
			val.append(1, *siter);
		}
	}

	return !has_quote;
}

/**
 * Returns the content of a specified sequenced configuration item.
 *
 * @param key Block to look up
 * @param index Sequence entry ID
 * @param name Name of entry to fetch
 * @param def Default value to return when not found
 * @return The actual sequenced configuration item, or an empty string when not
 * found
 */
String UnrealConfig::getSeqVal(const String& key, int index, const String& name,
	const String& def)
{
	String vr = key + "_#SEQ-" + String(index) + "#/" + name;
	return get(vr, def);
}

/**
 * Returns the content of the specied variable item.
 *
 * @param key Variable key to look up
 * @param category Category name to look up
 * @return Acutal content of variable or an empty string when the variable
 * cannot be found
 */
String UnrealConfig::getVarContent(const String& key, const String& category)
{
	String vname = category + "." + key;

	if (entries_.contains(vname))
		return entries_[vname];
	else if (entries_.contains(key))
		return entries_[key];
	else
		return String();
}

/**
 * Returns a copy of the local hash table.
 *
 * @return Hash table
 */
Map<String, String> UnrealConfig::map()
{
	return entries_;
}

/**
 * Returns a copy of the local module list.
 *
 * @return Module list
 */
StringList UnrealConfig::moduleList()
{
	return modules_;
}

/**
 * Reads and parses the content of the configuration files recursively.
 *
 * @param file File name to read
 * @return Returns true when the file has been parsed properly, or false when an
 * error occured
 */
bool UnrealConfig::read(const String& file)
{
	std::ifstream fst(file.c_str(), std::ifstream::in);
	size_t pos, lineno = 0, cpos = String::npos, cpos2 = String::npos;
	String category;

	if (!fst)
		return false;
	else if (!fst.good())
		return false;

	while (!fst.eof() && fst.good())
	{
		char buf[CONFIG_LINE_LENGHT_LIMIT];
		fst.getline(buf, CONFIG_LINE_LENGHT_LIMIT);

		String line = String(buf).trimmed();
		lineno++;

		if (line.left(1) == "#" || line.left(2) == "//" || line.size() == 0)
			continue;
		/* maybe we have an C-style comment? */
		else if (cpos != String::npos)
		{
			if ((cpos2 = line.find("*/", cpos)) != String::npos)
			{
				line = line.mid(cpos2 + 2);
				cpos = String::npos;

				if (line.size() == 0)
					continue;
			}
			else
				continue;
		}
		else if ((cpos = line.find("/*")) != String::npos)
		{
			if ((cpos2 = line.find("*/", cpos)) != String::npos)
			{
				// they end in the same line
				line = line.mid(cpos2 + 2);
				cpos = String::npos;

				if (line.size() == 0)
					continue;
			}
			else
				continue;
		}

		// do actual file parsing
		if ((pos = line.find(" {")) != String::npos)
		{
			String tmp_cat = line.left(pos).trimmed();

			/* check if we have a nested block */
			if (!category.empty())
				category << "/" << line.mid(0, pos).trimmed();

			/* or an sequenced block */
			else if (line.size() > 2 && (line.mid(pos - 2, 2) == "++"
					|| sequences_.contains(tmp_cat)))
			{
				if (line.mid(pos - 2, 2) == "++")
					tmp_cat = line.left(pos - 2).trimmed();

				// build sequence category name
				category.sprintf("%s_#SEQ-%d#",
						tmp_cat.c_str(),
						getLastIndex(tmp_cat) + 1);
			}
			else
			{
				category = line.mid(0, pos).trimmed();
			}
		}
		else if (line.contains("};")) //< end of block
		{
			/* on nested blocks, remove the last category item */
			if (category.contains("/"))
			{
				StringList sl = category.split("/");
				sl.removeLast();
				category = sl.join("/");
			}
			else
				category.clear();
		}
		else if (line.left(8) == "Include ") //< Include directive
		{
			String r;
			String s = line.mid(8);

			replaceVars(s, r, "");

			r.clear();
			if (!getQuotedContent(s, r))
			{
				std::cout << "Warning: "
						  << file
						  << ":"
						  << lineno
						  << " --"
						  << " Value is missing closing quote"
						  << std::endl;
				warnings_++;

				continue;
			}

			files_ << r;
		}
		else if (line.left(11) == "LoadModule ") //< LoadModule directive
		{
			String r;
			String s = line.mid(11);

			replaceVars(s, r, "");

			r.clear();
			if (!getQuotedContent(s, r))
			{
				std::cout << "Warning: "
						  << file
						  << ":"
						  << lineno
						  << " --"
						  << " Value is missing closing quote"
						  << std::endl;
				warnings_++;

				continue;
			}

			modules_ << r;
		}
		else if (line.left(9) == "Sequence ") //< Sequence directive
		{
			String r;
			String s = line.mid(9);

			replaceVars(s, r, "");

			r.clear();
			if (!getQuotedContent(s, r))
			{
				std::cout << "Warning: "
						  << file
						  << ":"
						  << lineno
						  << " --"
						  << " Value is missing closing quote"
						  << std::endl;
				warnings_++;

				continue;
			}

			sequences_ << r;
		}
		else
		{
			if (category.empty())
			{
				std::cout << "Warning: " << file << ":" << lineno << " --"
						  << " Assignment outside of a block: "
						  << line.trimmed()
						  << std::endl;
				warnings_++;
				continue;
			}

			while (line.contains(";"))
			{
				/* get end position of key */
				size_t fi = line.find(' ');

				/* get position of ';' */
				size_t sc = line.find(';');

				// get the key
				String val = line.left(fi).trimmed();
				String key = category + "/" + val;

				// content
				String tmp = line.mid(fi + 1, sc);

				// lookup variables to be replaced
				String ret;
				if (!replaceVars(tmp, ret, category))
				{
					std::cout << "Warning: "
							  << file
							  << ":"
							  << lineno
							  << " -- Can't resolve content of variable \""
							  << ret
							  << "\" in block \""
							  << category
							  << "\"\n";
					warnings_++;
				}

				// clear the value since we have to fill it up with proper content
				val.clear();

				if (!getQuotedContent(tmp, val))
				{
					std::cout << "Warning: "
							  << file
							  << ":"
							  << lineno
							  << " --"
							  << " Value is missing closing quote"
							  << std::endl;
					warnings_++;

					continue;
				}

				entries_.add(key, val.trimmed());

				line = line.mid(sc + 1).trimmed();
			}
		}
		/*
		else
		{
			std::cout << "Warning: "
					  << file
					  << ":"
					  << lineno
					  << " -- "
					  << "Invalid directive: "
					  << line.trimmed()
					  << std::endl;

			warnings_++;
		}
		*/
	}

	if (cpos != String::npos)
	{
		std::cout << "Warning: "
				  << file
				  << ":"
				  << lineno
				  << " -- "
				  << "Unclosed comment till EOF"
				  << std::endl;
		warnings_++;
	}
	else if (!category.empty())
	{
		std::cout << "Warning:"
				  << file
				  << ":"
				  << lineno
				  << " -- "
				  << "Missing close bracked for block \""
				  << category
				  << "\""
				  << std::endl;
		warnings_++;
	}

	fst.close();

	// include other files recursively, if any
	while (files_.size() > 0)
	{
		String fnam = files_.takeFirst();
		bool is_required = false;

		if (fnam.left(9) == "required:")
		{
			is_required = true;
			fnam = fnam.mid(9);
		}

		if (!read(fnam))
		{
			std::cout << "Warning: Reading configuration file \""
					  << fnam
					  << "\" failed."
					  << std::endl;

			if (is_required)
				std::exit(1);
		}
	}

	return true;
}

/**
 * Replace occurences of variables with their value.
 *
 * @param str String to work with
 * @param ret Variable name storage if replacement fails
 * @param category Category to locate scope variable names
 */
bool UnrealConfig::replaceVars(String& str, String& ret, const String& category)
{
	// lookup variables to be replaced
	size_t pos, lastpos = 0;

	while ((pos = str.find("$(", lastpos)) != String::npos)
	{
		char var_name[1024];

		/* ignore escaped vars */
		if (pos > 0 && str[pos - 1] == '\\')
		{
			pos += 3;
			continue;
		}
		else if (std::sscanf(str.c_str() + pos, "$(%[^)])", var_name) > 0)
		{
			String vr = var_name;
			size_t vrlen = vr.length();
			String repl = getVarContent(vr, category);

			if (repl.empty())
			{
				ret = vr;
				return false;
			}

			str.replace(pos, (size_t)(vrlen + 3), repl);
			lastpos = pos + 2;
		}
	}

	return true;
}

/**
 * Returns the number of sequence blocks found for the specified block name.
 *
 * @param blk Name of block to lookup
 * @return Number of sequence blocks found
 */
size_t UnrealConfig::sequenceCount(const String& blk)
{
	size_t highest = 0;
	String blknam = blk + "_#SEQ-";

	for (Map<String, String>::Iterator siter = entries_.begin();
			siter != entries_.end(); siter++)
	{
		String key = siter->first;

		if (key.length() >= blknam.length()
				&& key.left(blknam.length()) == blknam)
		{
			char tm[1024];
			int tm2;

			if (std::sscanf(key.c_str(), "%[^SEQ-]SEQ-%d#", tm, &tm2) >= 2)
			{
				if ((size_t)tm2 > highest)
					highest = tm2;
			}
			else if (key.contains("_#SEQ-"))
			{
				std::cout << "Config: Can't extract sequence ID out of string:"
						  << " ["
						  << key
						  << "]"
						  << std::endl;
			}
		}
	}

	return highest;
}

/**
 * Sets the main configuration filename.
 *
 * @param file Name of file to set
 */
void UnrealConfig::setFileName(const String& file)
{
	filename_ = file;
}

/**
 * Start reading the initial configuration file. When the configuration file
 * cannot be opened or read for any reason, the execution of the program is
 * terminated immediately.
 */
void UnrealConfig::startRead()
{
	if (!read(filename_))
	{
		std::cout << "Error: Can't read configuration file \""
				  << filename_
				  << "\", exiting."
				  << std::endl;
		std::exit(1);
	}
}


/**
 * Returns the number of warnings encountered during configuration file parsing
 * process.
 *
 * @return Number of warnings occurred
 */
uint32 UnrealConfig::warnings()
{
	return warnings_;
}
