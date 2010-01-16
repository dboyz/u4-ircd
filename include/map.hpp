/*****************************************************************
 * Unreal Internet Relay Chat Daemon, Version 4
 * File         map.hpp
 * Description  An object mapping object onto eachother
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

#ifndef _UNREALIRCD_MAP_HPP
#define _UNREALIRCD_MAP_HPP

#include <list.hpp>
#include <map>

/** STL container for the map */
#define __CONTAINER		std::map

/**
 * Provides a container for storing key/value pairs.
 * Duplicate entries with the same key are not allowed.
 * Use the `Hash' class for that purpose.
 *
 * This class is header-only.
 */
template<typename _KeyType, typename _ElementType,
	class _Cmp = std::less<_KeyType> >
class Map
	: public __CONTAINER<_KeyType, _ElementType, _Cmp>
{
public:
	/** Alias the original iterator */
	typedef typename __CONTAINER<_KeyType, _ElementType, _Cmp>::iterator
		Iterator;

public:
	/**
	 * Add key/value to the map.
	 */
	void add(const _KeyType& key, const _ElementType& el)
	{
		this->insert(std::pair<_KeyType, _ElementType>(key, el));
	}

	/**
	 * Returns whether the map contains an Element with the specified Key.
	 *
	 * @param key Key to find
	 * @return true when found, otherwise false
	 */
	bool contains(const _KeyType& key) const
	{
		return this->find(key) != this->end();
	}

	/**
	 * Returns whether the map contains the specified Element.
	 *
	 * @param el Element to find
	 * @return true when found, otherwise false
	 */
	bool containsElement(const _ElementType& el)
	{
		for (Iterator i = this->begin(); i != this->end(); ++i)
		{
			if (i->second == el)
			{
				return true;
			}
		}

		return false;
	}

	/**
	 * Remove the Element with the specified Key from the map
	 * and free it's allocated memory.
	 * Just use this for pointer elements!
	 *
	 * @param key Key of Element to be removed
	 */
	void free(const _KeyType& key)
	{
		Iterator i = this->find(key);

		if (i != this->end())
		{
			delete i->second;
			this->erase(i);
		}
	}

	/**
	 * Return the Key of the specified Element.
	 *
	 * @param el Element to find
	 * @return Key of specified Element, or _KeyType(0) when not found
	 */
	_KeyType key(const _ElementType& el)
	{
		for (Iterator i = this->begin(); i != this->end(); ++i)
		{
			if (i->second == el)
			{
				return i->first;
			}
		}

		return _KeyType(0);
	}

	/**
	 * Return a list with all Keys in the map.
	 *
	 * @return List with the Keys
	 */
	List<_KeyType> keys()
	{
		List<_KeyType> kl;

		for (Iterator i = this->begin(); i != this->end(); ++i)
			kl << i->first;

		return kl;
	}

	/**
	 * Remove the Element with the specified Key from the map.
	 *
	 * @param key Key of Element to be removed
	 */
	void remove(const _KeyType& key)
	{
		Iterator i = this->find(key);

		if (i != this->end())
			this->erase(i);
	}

	/**
	 * Returns the Element that is associated with the specified Key.
	 *
	 * @param key Key of Element
	 * @return Element, or _ElementType(0) when not found
	 */
	_ElementType value(const _KeyType& key)
	{
		Iterator i = this->find(key);

		if (i != this->end())
			return i->second;
		else
			return _ElementType(0);
	}

	/**
	 * Returns a list with all Elements of the map.
	 *
	 * @return List with Elements
	 */
	List<_ElementType> values()
	{
		List<_ElementType> el;

		for (Iterator i = this->begin(); i != this->end(); ++i)
			el << i->second;

		return el;
	}

	/**
	 * Stream operator for inserting new pairs to the map.
	 *
	 * @param pr Pair to be added
	 * @return Reference to the map
	 */
	Map& operator<<(const std::pair<_KeyType, _ElementType>& pr)
	{
		this->insert(pr);
		return *this;
	}
};

#endif /* _UNREALIRCD_MAP_HPP */
