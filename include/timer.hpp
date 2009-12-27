/*****************************************************************
 * Unreal Internet Relay Chat Daemon, Version 4
 * File         timer.hpp
 * Description  Asyncronous timing events
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

#ifndef _UNREALIRCD_TIMER_HPP
#define _UNREALIRCD_TIMER_HPP

#include "reactor.hpp"
#include "time.hpp"

/**
 * UnrealTimer class.
 *
 * Timers are observed from event reactors. If no interval has been set, the
 * timer is being removed (it's interpreted to have single-shot only).
 */
class UnrealTimer
{
public:
	template<typename FnType>
	UnrealTimer(const UnrealTime& ts, FnType cb, const uint32_t ival = -1)
		: expiretime_(ts), interval_(ival),
		callback_(static_cast<void*>(new Func<FnType>(cb)))
	{ }

	void exec();
	const UnrealTime& expiretime();
	uint32_t interval();
	void setInterval(const uint32_t& ival);
	void setExpireTime(const UnrealTime& ts);

private:
	class FuncBase
	{
	public:
		virtual ~FuncBase() {}
		virtual void exec() = 0;
	};

	template<typename FnType> class Func : public FuncBase
	{
	public:
		Func(FnType fn) : fn_(fn) {}
		void exec() { fn_(); }
	private:
		FnType fn_;
	};

private:
	/** expire time */
	UnrealTime expiretime_;

	/** interval */
	uint32_t interval_;

	/** callback function, sure not the cleanest way */
	void* callback_;
};

#endif /* _UNREALIRCD_TIMER_HPP */
