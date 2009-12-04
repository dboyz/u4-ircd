/*****************************************************************
 * Unreal Internet Relay Chat Daemon, Version 4
 * File         list.hpp
 * Description  std::vector extension
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

#ifndef _UNREALIRCD_LIST_HPP
#define _UNREALIRCD_LIST_HPP

#include <assert.h>
#include <vector>

/**
 * Provides an container for elements of the same type, derived from std::vector.
 *
 * This class is header-only.
 */
template<typename _ElementType>
class List
	: public std::vector<_ElementType>
{
public:
	/** Alias the original iterator */
	typedef typename std::vector<_ElementType>::iterator Iterator;

public:
	/**
	 * Adds an element to the end of the list.
	 *
	 * @param el Element to append
	 */
	void add(const _ElementType& el)
	{
		this->push_back(el);
	}

	/**
	 * Returns whether the specified Element is present in the list.
	 *
	 * @param el Element to find
	 * @return true when the element has been found, otherwise false
	 */
	bool contains(const _ElementType& el)
	{
		return this->indexOf(el) != (size_t)-1;
	}

	/**
	 * Returns the position of the specified Element.
	 *
	 * @param el Element to find
	 * @return Position index (>= 0) if found, otherwise (size_t) -1
	 */
	size_t indexOf(const _ElementType& el)
	{
		for (size_t i = 0; i < this->size(); i++)
		{
			if (this->at(i) == el)
			{
				return i;
			}
		}

		/* not found */
		return -1;
	}

	/**
	 * Remove an element from the list.
	 *
	 * @param el Element to be removed
	 */
	void remove(const _ElementType& el)
	{
		for (Iterator i = this->begin(); i != this->end(); i++)
		{
			if (i == el)
			{
				this->erase(i);
				return;
			}
		}
	}

	/**
	 * Remove the Element at the specified position.
	 *
	 * @param index List index of Element
	 */
	void removeAt(size_t index)
	{
		assert(index < this->size());

		/* remove it */
		this->erase(this->begin() + index);
	}

	/**
	 * Remove the first Element of the list.
	 */
	void removeFirst()
	{
		if (this->size() > 0)
		{
			this->erase(this->begin());
		}
	}

	/**
	 * Remove the last Element of the list.
	 */
	void removeLast()
	{
		if (this->size() > 0)
		{
			this->erase(this->end() - 1);
		}
	}

	/**
	 * Remove a specified Element from the list and return a copy of it.
	 *
	 * @param index Index of Element
	 * @return A copy of the element
	 */
	_ElementType takeAt(size_t index)
	{
		assert(index < this->size());

		/* copy & remove it */
		_ElementType el = this->at(index);
		this->erase(this->begin() + index);

		return el;
	}

	/**
	 * Remove the first Element from the list and return a copy of it.
	 *
	 * @return A copy of the element
	 */
	_ElementType takeFirst()
	{
		return this->takeAt(0);
	}

	/**
	 * Remove the last Element from the list and return a copy of it.
	 *
	 * @return A copy of the element
	 */
	_ElementType takeLast()
	{
		return this->takeAt(this->size() - 1);
	}

	/**
	 * Stream operator for adding Element to the end of the list.
	 *
	 * @param el Element to add
	 * @return Reference to the List
	 */
	List& operator<<(const _ElementType& el)
	{
		this->push_back(el);
		return *this;
	}

	/**
	 * Stream operator extracting Elements from the list.
	 *
	 * @param el Address of element to copy to
	 * @return Reference to the List
	 */
	List& operator>>(_ElementType& el)
	{
		el = this->takeFirst();
		return *this;
	}
};

#endif /* _UNREALIRCD_LIST_HPP */
