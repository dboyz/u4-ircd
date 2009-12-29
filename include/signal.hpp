/*****************************************************************
 * Unreal Internet Relay Chat Daemon, Version 4
 * File         signal.hpp
 * Description  A type-independent way to transmit signals
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

#ifndef _UNREALIRCD_SIGNAL_HPP
#define _UNREALIRCD_SIGNAL_HPP

#include "list.hpp"

/**
 * UnrealSignal w/o arguments.
 */
template<typename ConnectionFnType>
class UnrealSignal0
{
public:
	typedef List<ConnectionFnType> ConnectionList;
	typedef typename List<ConnectionFnType>::Iterator ConnectionIterator;

public:
	~UnrealSignal0()
	{
		disconnectAllSlots();
	}

	void connect(ConnectionFnType fn)
	{
		connections_ << fn;
	}

	size_t connectionCount()
	{
		return connections_.size();
	}

	void disconnect(ConnectionFnType fn)
	{
		connections_.remove(fn);
	}

	void disconnectAllSlots()
	{
		connections_.clear();
	}

	void operator()()
	{
		for (ConnectionIterator ci = connections_.begin();
		    	ci != connections_.end(); ++ci)
		{
			(*ci)();
		}
	}
	
private:
	ConnectionList connections_;
};

/**
 * UnrealSignal w/ one argument.
 */
template<typename ConnectionFnType, typename Arg1Type>
class UnrealSignal1
	: public UnrealSignal0<ConnectionFnType>
{
public:
	typedef List<ConnectionFnType> ConnectionList;
	typedef typename List<ConnectionFnType>::Iterator ConnectionIterator;

public:
	~UnrealSignal1()
	{
		disconnectAllSlots();
	}

	void connect(ConnectionFnType fn)
	{
		connections_ << fn;
	}

	size_t connectionCount()
	{
		return connections_.size();
	}

	void disconnect(ConnectionFnType fn)
	{
		connections_.remove(fn);
	}

	void disconnectAllSlots()
	{
		connections_.clear();
	}

	void operator()(Arg1Type arg1)
	{
		for (ConnectionIterator ci = connections_.begin();
		    	ci != connections_.end(); ++ci)
		{
			(*ci)(arg1);
		}
	}

private:
	ConnectionList connections_;
};

/**
 * UnrealSignal w/ two arguments.
 */
template<typename ConnectionFnType, typename Arg1Type, typename Arg2Type>
class UnrealSignal2
	: public UnrealSignal0<ConnectionFnType>
{
public:
	typedef List<ConnectionFnType> ConnectionList;
	typedef typename List<ConnectionFnType>::Iterator ConnectionIterator;

public:
	~UnrealSignal2()
	{
		disconnectAllSlots();
	}

	void connect(ConnectionFnType fn)
	{
		connections_ << fn;
	}

	size_t connectionCount()
	{
		return connections_.size();
	}

	void disconnect(ConnectionFnType fn)
	{
		connections_.remove(fn);
	}

	void disconnectAllSlots()
	{
		connections_.clear();
	}

	void operator()(Arg1Type arg1, Arg2Type arg2)
	{
		for (ConnectionIterator ci = connections_.begin();
		    	ci != connections_.end(); ++ci)
		{
			(*ci)(arg1, arg2);
		}
	}

private:
	ConnectionList connections_;
};

/**
 * UnrealSignal w/ three arguments.
 */
template<typename ConnectionFnType, typename Arg1Type, typename Arg2Type,
	typename Arg3Type>
class UnrealSignal3
	: public UnrealSignal0<ConnectionFnType>
{
public:
	typedef List<ConnectionFnType> ConnectionList;
	typedef typename List<ConnectionFnType>::Iterator ConnectionIterator;

public:
	~UnrealSignal3()
	{
		disconnectAllSlots();
	}

	void connect(ConnectionFnType fn)
	{
		connections_ << fn;
	}

	size_t connectionCount()
	{
		return connections_.size();
	}

	void disconnect(ConnectionFnType fn)
	{
		connections_.remove(fn);
	}

	void disconnectAllSlots()
	{
		connections_.clear();
	}

	void operator()(Arg1Type arg1, Arg2Type arg2, Arg3Type arg3)
	{
		for (ConnectionIterator ci = connections_.begin();
		    	ci != connections_.end(); ++ci)
		{
			(*ci)(arg1, arg2, arg3);
		}
	}

private:
	ConnectionList connections_;
};

#endif /* _UNREALIRCD_SIGNAL_HPP */
