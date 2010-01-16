/*****************************************************************
 * Unreal Internet Relay Chat Daemon, Version 4
 * File         channel.hpp
 * Description  Channel management
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

#ifndef _UNREALIRCD_CHANNEL_HPP
#define _UNREALIRCD_CHANNEL_HPP

#include <bitmask.hpp>
#include <list.hpp>
#include <map.hpp>
#include <mode.hpp>
#include <modebuf.hpp>
#include <numeric.hpp>
#include <string.hpp>
#include <time.hpp>
#include <user.hpp>

class UnrealUser;

/**
 * Channel ban representation.
 */
struct UnrealChannelBan
{
	/** mask of ban originator */
	String originator;

	/** ban mask */
	String mask;

	/** creation/last modification timestamp */
	UnrealTime lastmod;
};

/**
 * A channel member represents each user at a channel.
 */
struct UnrealChannelMember
{
	/**
	 * Defines the flags to be used for channel members.
	 * We use it that way, because the channel member modes are not gonna modified.
	 */
	enum FlagType
	{
		/** channel operator */
		ChanOp = 0x01,

		/** half operator */
		HalfOp = 0x02,

		/** voice */
		Voice  = 0x04
	};

	/**
	 * Returns whether the member is channel operator.
	 */
	bool isChanOp()
	{
		return flags.isset(ChanOp);
	}

	/**
	 * Returns whether the member is half operator.
	 */
	bool isHalfOp()
	{
		return flags.isset(HalfOp);
	}

	/**
	 * Returns whether the member is voiced.
	 */
	bool isVoiced()
	{
		return flags.isset(Voice);
	}

	/** user on the channel */
	UnrealUser* user;

	/** it's flags */
	Bitmask<uint8_t> flags;

	/** the time the user entered the channel */
	UnrealTime joined;
};

/**
 * The channel class.
 */
class UnrealChannel
{
public:
	/** alias the channel ban type */
	typedef UnrealChannelBan Ban;

	/** alias iterator for the ban list */
	typedef List<Ban*>::Iterator BanIterator;

	/** alias the channel member type */
	typedef UnrealChannelMember Member;

	/** alias iterator for the member map */
	typedef Map<UnrealUser*, Member*>::Iterator MemberIterator;

	/** alias the mode buffer type for channel modes */
	typedef UnrealModeBufferType<UnrealChannelMode> ModeBuf;

public:
	UnrealChannel(const String& name);
	~UnrealChannel();
	Ban* addBan(const String& mask, const String& origin);
	Member* addMember(UnrealUser* uptr, const uint8_t& flags);
	bool canSend(UnrealUser* uptr, String& text);
	UnrealTime creationTime();
	static UnrealChannel* find(const String& chname);
	Ban* findBan(const String& mask);
	Member* findMember(UnrealUser* uptr);
	bool isBanned(UnrealUser* uptr);
	bool isInvited(UnrealUser* uptr);
	bool isInviteOnly();
	bool isKey();
	bool isLimit();
	bool isModerated();
	bool isNoExternalMsg();
	bool isPrivate();
	bool isSecret();
	bool isTopicOpsOnly();
	const String& key();
	UnrealTime lastJoinTime();
	const uint32_t& limit();
	String lowerName();
	void modebufSend(UnrealUser* uptr, List<ModeBuf>& mblist);
	Bitmask<uint32_t>& modes();
	String modestr();
	const String& name();
	void parseModeChange(UnrealUser* uptr, StringList* argv);
	void removeBan(const String& mask);
	void removeMember(UnrealUser* uptr);
	void sendBanList(UnrealUser* uptr);
	void sendlocalreply(UnrealUser* uptr, const String& cmd,
			const String& data, bool skip_sender = false);
	void sendreply(UnrealUser* uptr, IRCNumeric numeric,
			const String& data);
	void sendreply(UnrealUser* uptr, const String& cmd,
			const String& data);
	void setCreationTime(const UnrealTime& ts);
	void setKey(const String& keystr);
	void setLastJoinTime(const UnrealTime& ts);
	void setLimit(const uint32_t& lim);
	void setName(const String& chname);
	void setTopic(const String& msg);
	void setTopicMask(const String& mask);
	void setTopicTime(const UnrealTime& ts);
	const String& topic();
	const String& topicMask();
	UnrealTime topicTime();

public:
	List<Ban*> banlist;
	List<UnrealUser*> invites;
	Map<UnrealUser*, Member*> members;

private:
	/** channel name */
	String name_;

	/** channel key */
	String key_;

	/** channel topic */
	String topic_;

	/** channel topic originator mask */
	String topic_mask_;

	/** channel creation timestamp */
	UnrealTime creation_time_;

	/** last user join timestamp */
	UnrealTime last_join_time_;

	/** channel topic timestamp */
	UnrealTime topic_time_;

	/** channel mode bitmask */
	Bitmask<uint32_t> modes_;

	/** channel limit */
	uint32_t limit_;
};

namespace UnrealChannelProperties
{
	extern UnrealChannelMode Ban;
	extern UnrealChannelMode HalfOp;
	extern UnrealChannelMode InviteOnly;
	extern UnrealChannelMode Key;
	extern UnrealChannelMode Limit;
	extern UnrealChannelMode Moderated;
	extern UnrealChannelMode NoExternalMsg;
	extern UnrealChannelMode ChanOp;
	extern UnrealChannelMode Private;
	extern UnrealChannelMode Secret;
	extern UnrealChannelMode TopicOpsOnly;
	extern UnrealChannelMode Voice;
	extern UnrealChannelModeTable ModeTable;
}

#endif /* _UNREALIRCD_CHANNEL_HPP */
