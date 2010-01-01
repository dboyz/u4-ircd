/*****************************************************************
 * Unreal Internet Relay Chat Daemon, Version 4
 * File         bitmask.hpp
 * Description  Efficient way to work with bitwise flags
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

#ifndef _UNREALIRCD_BITMASK_HPP
#define _UNREALIRCD_BITMASK_HPP

/**
 * A bitmask is used to store flags (e.g. user/channel modes) bitwise.
 */
template<typename _StorageType>
class Bitmask
{
public:
	/**
	 * Constructs the Bitmask. The internal value is set to zero if `init_val'
	 * is not specified.
	 *
	 * @param init_val Optional, initial value for the whole data set
	 */
	Bitmask(const _StorageType& init_val = 0)
		: data_(init_val)
	{ }

	/**
	 * Add the specified flag to the Bitmask.
	 *
	 * @param flag Flag to add
	 */
	void add(const _StorageType& flag)
	{
		if (!isset(flag))
			data_ |= flag;
	}

	/**
	 * Sets the internal value to zero. That removes all flags that has been
	 * applied on the Bitmask.
	 */
	void clear()
	{
		data_ = 0;
	}

	/**
	 * Returns whether the specified flag `flag' is set.
	 *
	 * @param flag Flag to check for
	 * @return true when set, otherwise false
	 */
	inline bool isset(const _StorageType& flag)
	{
		return (data_ & flag);
	}

	/**
	 * Reset the internal value to `init_val'.
	 *
	 * @param init_val Optional, initial value
	 */
	void reset(const _StorageType& init_val = 0)
	{
		data_ = init_val;
	}

	/**
	 * Revoke the specified flag `flag' from the data set.
	 *
	 * @param flag Flag to revoke
	 */
	void revoke(const _StorageType& flag)
	{
		if (isset(flag))
			data_ &= ~flag;
	}

	/**
	 * Returns the internal data value of the Bitmask.
	 * It can be used to check if there are still flags set.
	 *
	 * @return Internal data value
	 */
	const _StorageType& value()
	{
		return data_;
	}

	/**
	 * Stream operator for adding flags to the Bitmask.
	 *
	 * @param flag Flag to add
	 * @return Reference to the Bitmask object
	 */
	inline Bitmask& operator<<(const _StorageType& flag)
	{
		add(flag);
		return *this;
	}

	/**
	 * Stream operator for revoking flags from the bitmask.
	 *
	 * @param flag Flag to revoke
	 * @return Reference to the Bitmask object
	 */
	inline Bitmask& operator>>(const _StorageType& flag)
	{
		revoke(flag);
		return *this;
	}

private:
	/** This is where the actual data is stored */
	_StorageType data_;
};

#endif /* _UNREALIRCD_BITMASK_HPP */
