/*****************************************************************
 * Unreal Internet Relay Chat Daemon, Version 4
 * File         epoll.hpp
 * Description  epoll() event reactor
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

#include "base.hpp"
#include "epoll.hpp"
#include <sys/epoll.h>
#include <iostream>
#include <cstdlib>

/**
 * UnrealEpollReactor constructor.
 * Allocates an epoll instance for file descriptor observations.
 */
UnrealEpollReactor::UnrealEpollReactor()
		: active_(true)
{
	int max_connections = unreal->config.get("Me/MaxConnections", "1024")
		.toInt();

	if ((epoll_fd_ = epoll_create(max_connections)) < 0)
	{
		unreal->log.write(UnrealLog::Fatal, "Couldn't create epoll instance "
		    "for %d connections: %s",
		    max_connections,
		    strerror(errno));
	}

	/** allocate event entries */
	max_evpl_ = unreal->config.get("Limits/MaxEventsPerLoop", "35").toInt();
	events_ = new struct epoll_event[max_evpl_];
}

/**
 * UnrealEpollReactor destructor.
 */
UnrealEpollReactor::~UnrealEpollReactor()
{
	stop();
	close(epoll_fd_);
	delete[] events_;
}

/**
 * Dispatch events in a single run.
 */
void UnrealEpollReactor::dispatch()
{
	struct epoll_event* ev;
	int num_events;

	num_events = epoll_wait(epoll_fd_, events_, max_evpl_, -1);

	for (int i = 0; i < num_events; i++)
	{
		ev = &events_[i];

		/* notify callback */
		//(*callbacks_[ev->data.fd])(1, 12);
		callbacks_[ev->data.fd](ev->data.fd, ev->events);
	}
}

/**
 * Observe the specified file descriptor for events.
 *
 * @param fd File descriptor to observe
 * @param cbptr Callback pointer
 * @param events Generic event mask with flags to check for
 * @return True if the event has been registered successfully, otherwise false
 */
bool UnrealEpollReactor::observe(int fd, CallbackType cbptr, uint32_t events)
{
	struct epoll_event evs;
	int ep_result;

	evs.events = translateEvents(Native, events);

	ep_result = epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, fd, &evs);

	if (ep_result == 0)
		callbacks_.add(fd, cbptr);

	return (ep_result == 0);
}

/**
 * Run the event loop.
 */
void UnrealEpollReactor::run()
{
	while (active_)
		dispatch();
}

/**
 * Stop the event loop.
 */
void UnrealEpollReactor::stop()
{
	active_ = false;
}

/**
 * Stop monitoring the specified file descriptor.
 *
 * @param fd File descriptor that is currently being observed
 * @return True if the operation succeed, otherwise false
 */
bool UnrealEpollReactor::stop(int fd)
{
	int ep_result;

	ep_result = epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, fd, NULL);
	callbacks_.remove(fd);

	return (ep_result == 0);
}

/**
 * Translate generic Socket Engine event flags to epoll compatible
 * event flags and vice-versa.
 *
 * @param ot Output type, either `Generic' for the generic flags, or
 * `native' for the native flags (e.g. epoll flags)
 * @param ev Input event flags
 * @return Translated version of the events
 */
uint32_t UnrealEpollReactor::translateEvents(TranslationFlag ot, uint32_t ev)
{
	uint32_t result = 0;

	if (ot == Generic)
	{
		if (ev & EPOLLERR)
			result |= EventError;
		if (ev & EPOLLHUP)
			result |= EventHup;
		if (ev & EPOLLIN)
			result |= EventIn;
		if (ev & EPOLLOUT)
			result |= EventOut;
	}
	else if (ot == Native)
	{
		if (ev & EventError)
			result |= EPOLLERR;
		if (ev & EventHup)
			result |= EPOLLHUP;
		if (ev & EventIn)
			result |= EPOLLIN;
		if (ev & EventOut)
			result |= EPOLLOUT;
	}

	return result;
}

/**
 * Returns the reactor name.
 *
 * @return "epoll"
 */
String UnrealEpollReactor::type()
{
	return "epoll";
}
