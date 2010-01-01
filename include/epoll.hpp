/*****************************************************************
 * Unreal Internet Relay Chat Daemon, Version 4
 * File         epoll.hpp
 * Description  epoll() event reactor
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

#ifndef _UNREALIRCD_EPOLL_HPP
#define _UNREALIRCD_EPOLL_HPP

#include "binder.hpp"
#include "map.hpp"
#include "platform.hpp"
#include "reactor.hpp"
#include "timer.hpp"

class UnrealTimer;

/**
 * epoll() reactor for asyncronous events
 */
class UnrealEpollReactor
	: public UnrealGenericReactor
{
public:
	/** callback type definition */
	typedef UnrealBinder2<
		UnrealEpollReactor,
		int,
		uint32_t,
		void> CallbackType;

public:
	UnrealEpollReactor();
	~UnrealEpollReactor();
	void dispatch();
	bool observe(int fd, CallbackType cb, 
	    uint32_t events = EventError | EventIn);
	bool observe(UnrealTimer* timer);
	void run();
	void stop();
	bool stop(int fd);
	bool stop(UnrealTimer* timer);
	uint32_t translateEvents(TranslationFlag ot, uint32_t ev);
	static String type();

private:
	/** file descriptor for epoll_create() */
	int epoll_fd_;

	/** amount of events to be handled per poll */
	int max_evpl_;

	/** events entries to be used for polling */
	struct epoll_event* events_;

	/** specified for an infinite loop break */
	bool active_;

	/** callback map */
	Map<int, CallbackType> callbacks_;

	/** timer map */
	List<UnrealTimer*> timers_;
};

/* typedef the reactor */
typedef UnrealEpollReactor UnrealReactor;


#endif /* _UNREALIRCD_EPOLL_HPP */
