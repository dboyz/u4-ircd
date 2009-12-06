/*****************************************************************
 * Unreal Internet Relay Chat Daemon, Version 4
 * File         mode.hpp
 * Description  Dynamic mode flag class
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

#ifndef _UNREALIRCD_MODE_HPP
#define _UNREALIRCD_MODE_HPP

#include "map.hpp"
#include "user.hpp"
#include <limits>

class UnrealUser;

struct UnrealModeCompareIterator
{
	template<typename _IterA, typename _IterB>
	inline bool operator()(_IterA a, _IterB b) const
	{
		return (a).mode_char < (b).mode_char;
	}
};

template<typename TargetType>
class UnrealMode
{
public:
	/** validation hook result */
	enum Result { Allow, Deny };

	/** hook function definition */
	typedef Result (*HookFunc)(UnrealUser*, TargetType*);

	/** mode constructor */
	UnrealMode(char ch, size_t params = 0, HookFunc hookfn = 0)
		: mode_char(ch), param_count(params), hook(hookfn)
	{ }

	/** mode character */
	char mode_char;

	/** required parameter count */
	size_t param_count;

	/** validation hook */
	HookFunc hook;
};

/** global mode type declarations */
typedef UnrealMode<UnrealUser> UnrealUserMode;

/**
 * A mode table map, that is used to manage mode flags of a specified type
 * (channel or user modes).
 * The maximum amount of modes that can be stored depends on the
 * `_StorageSizeType'.
 */
template<typename _StorageSizeType, typename _TargetType>
class UnrealModeTable
	: public Map<_TargetType, _StorageSizeType, UnrealModeCompareIterator>
{
public:
	/** result type */
	enum Result { Success, Failed };

	/** alias iterator */
	typedef typename Map<_TargetType, _StorageSizeType,
		UnrealModeCompareIterator>::Iterator Iterator;

public:
	/**
	 * Deregister a mode entry.
	 *
	 * @param mo UnrealMode to deregister
	 * @return `Success' if anything went fine, otherwise `Failed'
	 */
	Result deregisterMode(const _TargetType& mo)
	{
		if (!contains(mo))
			return Failed;
		else
		{
			remove(mo);
			return Success;
		}
	}

	/**
	 * Register a mode entry.
	 *
	 * @param mo UnrealMode to register
	 * @return `Success' if anything went fine, otherwise `Failed'
	 */
	Result registerMode(const _TargetType& mo)
	{
		_StorageSizeType si = getFreeSlot();

		if (!si)
			return Failed; /* all slots in use */
		else if (hasFlag(mo.mode_char))
			return Failed; /* we've already such a flag registered */
		else
		{
			/* okay, finally add the mode */
			this->add(mo, si);

			return Success;
		}
	}

	/**
	 * Tries to get a free slot for another mode.
	 * The number of possible slots depend on the _StorageSizeType specified
	 * in the constructor.
	 *
	 * @return New mask for mode, or zero if all slots are in use
	 */
	_StorageSizeType getFreeSlot()
	{
		_StorageSizeType max = std::numeric_limits<_StorageSizeType>::max();

		for (_StorageSizeType i = 1; i <= max; i *= 2)
		{
			if (!containsElement(i)) /* search by element */
			{
				return i;
			}
		}

		return 0;
	}

	/**
	 * Returns whether the mode table contains a UnrealMode entry with
	 * the specified mode character.
	 *
	 * @param ch Mode character to search
	 * @return true when found, otherwise false
	 */
	bool hasFlag(char ch) const
	{
		for (Iterator i = this->begin(); i != this->end(); i++)
		{
			if ((i->first).mode_char == ch)
				return true;
		}

		return false;
	}

	/**
	 * Lookup a UnrealMode entry by it's mode character.
	 *
	 * @param ch Mode character
	 * @return UnrealMode entry
	 */
	_TargetType lookup(char ch)
	{
		for (Iterator i = this->begin(); i != this->end(); i++)
		{
			if ((i->first).mode_char == ch)
				return i->first;
		}

		return _TargetType(0);
	}
};

/** global mode table declarations */
typedef UnrealModeTable<uint16_t, UnrealUserMode> UnrealUserModeTable;

#endif /* _UNREALIRCD_MODE_HPP */
