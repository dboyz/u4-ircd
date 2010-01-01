/*****************************************************************
 * Unreal Internet Relay Chat Daemon, Version 4
 * File         timer.cpp
 * Description  Asyncronous timing events
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

#include "timer.hpp"

/**
 * Execute callback function.
 */
void UnrealTimer::exec()
{
	static_cast<FuncBase*>(callback_)->exec();
}

/**
 * Returns the expire time.
 *
 * @return Expire time
 */
const UnrealTime& UnrealTimer::expiretime()
{
	return expiretime_;
}

/**
 * Return the interval.
 *
 * Note: If the interval is -1 (UINT_MAX), timed events should be removed
 * after they've been triggered.
 *
 * @return Timer interval
 */
uint32_t UnrealTimer::interval()
{
	return interval_;
}

/**
 * Update the timer interval.
 *
 * @param ival New interval
 */
void UnrealTimer::setInterval(const uint32_t& ival)
{
	interval_ = ival;
}

/**
 * Update expire time.
 *
 * @param ts New expiry time
 */
void UnrealTimer::setExpireTime(const UnrealTime& ts)
{
	expiretime_ = ts;
}
