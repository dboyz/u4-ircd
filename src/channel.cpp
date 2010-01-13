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

#include "base.hpp"
#include "channel.hpp"
#include "cmdmap.hpp"
#include <iostream>

/* cast */
#define FLAG_TO_UINT(x) \
	static_cast<uint16_t>(x)

/** channel mode definitions */
namespace UnrealChannelProperties
{
	/** channel ban instruction */
	UnrealChannelMode Ban('b', 1);

	/** channel halfop instruction */
	UnrealChannelMode HalfOp('h', 1);

	/** channel is invite-only */
	UnrealChannelMode InviteOnly('i');

	/** channel requires a key to join */
	UnrealChannelMode Key('k', 1);

	/** channel has a user limit set */
	UnrealChannelMode Limit('l', 1);

	/** only users with flags may talk */
	UnrealChannelMode Moderated('m');

	/** only permit messages from users in the channel */
	UnrealChannelMode NoExternalMsg('n');

	/** channel operator instruction */
	UnrealChannelMode ChanOp('o', 1);

	/** channel is private */
	UnrealChannelMode Private('p');

	/** channel is secret */
	UnrealChannelMode Secret('s');

	/** only permit channel operators to change the topic */
	UnrealChannelMode TopicOpsOnly('t');

	/** channel voice instruction */
	UnrealChannelMode Voice('v');

	/** channel mode table */
	UnrealChannelModeTable ModeTable;
}

/**
 * UnrealChannel constructor.
 *
 * @param name Initial channel name
 */
UnrealChannel::UnrealChannel(const String& name)
	: creation_time_(UnrealTime::now()), limit_(0)
{
	setName(name);
}

/**
 * UnrealChannel destructor.
 */
UnrealChannel::~UnrealChannel()
{
	foreach(MemberIterator, cmi, members)
	{
		/* free all member entries */
		Member* cmptr = cmi->second;
		delete cmptr;
	}

	/* remove channel from channel list */
	if (!name_.empty())
	{
		String ln = lowerName();

		if (unreal->channels.contains(ln))
			unreal->channels.remove(ln);
	}
}

/**
 * Add a ban to the channel.
 *
 * @param mask Ban mask
 * @param origin Mask of ban originator
 * @return Returns the new ban entry, or an existing one if
 * the ban already exists
 */
UnrealChannel::Ban* UnrealChannel::addBan(const String& mask,
		const String& origin)
{
	Ban* cbptr = findBan(mask);

	if (!cbptr)
	{
		cbptr = new Ban();
		cbptr->mask = mask;
		cbptr->originator = mask;
		cbptr->lastmod = UnrealTime::now();

		/* add it into the ban list */
		banlist << cbptr;
	}

	return cbptr;
}

/**
 * Add a user to the channel.
 *
 * @param uptr User to add
 * @param flags Initial flags, if any
 * @return Returns the new member entry, or an existing one if
 * the member already exists
 */
UnrealChannel::Member* UnrealChannel::addMember(UnrealUser* uptr,
		const uint8_t& flags)
{
	Member* cmptr = findMember(uptr);

	if (!cmptr)
	{
		cmptr = new Member();
		cmptr->user = uptr;
		cmptr->flags.reset(flags);
		cmptr->joined = UnrealTime::now();

		/* add member to channel */
		members.add(uptr, cmptr);

		/* add channel into user's channel list so we can access the channels
		 * a bit faster
		 */
		uptr->channels << this;
	}

	return cmptr;
}

/**
 * Returns whether the specified user can send to the channel.
 *
 * @param uptr User entry pointer
 * @param text Text to send
 * @return true when user is able to send, otherwise false
 */
bool UnrealChannel::canSend(UnrealUser* uptr, String& text)
{
	if (uptr->isOper())
		return true; /* opers can always send to channels */
	else if (isModerated())
	{
		Member* cmptr = findMember(uptr);

		/* permit only when it's having necessary flags */
		if (cmptr && (!cmptr->isChanOp() && !cmptr->isHalfOp()
				&& !cmptr->isVoiced()))
			return false;
	}
	else if (isNoExternalMsg() && !findMember(uptr))
		return false; /* no external messages */

	return !isBanned(uptr);
}

/**
 * Returns the channel creation timestamp.
 *
 * @return UnrealTime
 */
UnrealTime UnrealChannel::creationTime()
{
	return creation_time_;
}

/**
 * Lookup a channel entry.
 *
 * @param chname Channel name
 */
UnrealChannel* UnrealChannel::find(const String& chname)
{
	String ln = chname;

	/* get a lowercase version of the name so we can find it */
	ln = ln.toLower();

	if (unreal->channels.contains(ln))
		return unreal->channels[ln];
	else
		return 0;
}

/**
 * Locate a channel ban.
 *
 * @param mask Ban mask
 * @return Pointer to channel ban entry or 0 if not found
 */
UnrealChannel::Ban* UnrealChannel::findBan(const String& mask)
{
	foreach (BanIterator, cbi, banlist)
	{
		Ban* cbptr = *cbi;

		if (cbptr->mask == mask)
			return cbptr;
	}

	return 0;
}

/**
 * Locate a channel member.
 *
 * @param uptr User entry pointer
 * @return Pointer to member entry or 0 if not found
 */
UnrealChannel::Member* UnrealChannel::findMember(UnrealUser* uptr)
{
	if (members.contains(uptr))
		return members[uptr];
	else
		return 0;
}

/**
 * Returns whether the specified user is banned from the channel.
 *
 * @param uptr User entry
 * @return True if banned, otherwise false
 */
bool UnrealChannel::isBanned(UnrealUser* uptr)
{
	String mask;

	/* build the mask */
	mask.sprintf("%s!%s@%s",
		uptr->nick().c_str(),
		uptr->ident().c_str(),
		uptr->hostname().c_str());

	/* iterate throug the banlist and look if some ban match */
	foreach (BanIterator, cbi, banlist)
	{
		Ban* cbptr = *cbi;

		if (mask.match(cbptr->mask))
			return true;
	}

	return false;
}

/**
 * Returns whether the channel is invite-only.
 *
 * @return true when invite-only, otherwise false
 */
bool UnrealChannel::isInviteOnly()
{
	using namespace UnrealChannelProperties;

	if (ModeTable.contains(InviteOnly)
			&& modes_.isset(ModeTable.value(InviteOnly)))
		return true;
	else
		return false;
}

/**
 * Returns whether the channel requires a key to enter it.
 *
 * @return true when a key is required, otherwise false
 */
bool UnrealChannel::isKey()
{
	using namespace UnrealChannelProperties;

	if (ModeTable.contains(Key)
			&& modes_.isset(ModeTable.value(Key)))
		return true;
	else
		return false;
}

/**
 * Returns whether the channel has a limit set.
 *
 * @return true when a limit is set, otherwise false
 */
bool UnrealChannel::isLimit()
{
	using namespace UnrealChannelProperties;

	if (ModeTable.contains(Limit)
			&& modes_.isset(ModeTable.value(Limit)))
		return true;
	else
		return false;
}

/**
 * Returns whether the channel is moderated.
 *
 * @return true when moderated mode is set, otherwise false
 */
bool UnrealChannel::isModerated()
{
	using namespace UnrealChannelProperties;

	if (ModeTable.contains(Moderated)
			&& modes_.isset(ModeTable.value(Moderated)))
		return true;
	else
		return false;
}

/**
 * Returns whether external users are allowed to send to the channel.
 *
 * @return true when external messages are not permitted, otherwise false
 */
bool UnrealChannel::isNoExternalMsg()
{
	using namespace UnrealChannelProperties;

	if (ModeTable.contains(NoExternalMsg)
			&& modes_.isset(ModeTable.value(NoExternalMsg)))
		return true;
	else
		return false;
}

/**
 * Returns whether the channel is private.
 *
 * @return true when private, otherwise false
 */
bool UnrealChannel::isPrivate()
{
	using namespace UnrealChannelProperties;

	if (ModeTable.contains(Private)
			&& modes_.isset(ModeTable.value(Private)))
		return true;
	else
		return false;
}

/**
 * Returns whether the channel is secret.
 *
 * @return true when secret, otherwise false
 */
bool UnrealChannel::isSecret()
{
	using namespace UnrealChannelProperties;

	if (ModeTable.contains(Secret)
			&& modes_.isset(ModeTable.value(Secret)))
		return true;
	else
		return false;
}

/**
 * Returns whether users require channel operator privileges to
 * change the channel topic.
 *
 * @return true when operator privileges are required, otherwise false
 */
bool UnrealChannel::isTopicOpsOnly()
{
	using namespace UnrealChannelProperties;

	if (ModeTable.contains(TopicOpsOnly)
			&& modes_.isset(ModeTable.value(TopicOpsOnly)))
		return true;
	else
		return false;
}

/**
 * Returns the channel key. If no key has been set, this returns
 * an empty string.
 *
 * @return Channel key
 */
const String& UnrealChannel::key()
{
	return key_;
}

/**
 * Returns the timestamp of the last join tracked on the channel.
 *
 * @return UnrealTime
 */
UnrealTime UnrealChannel::lastJoinTime()
{
	return last_join_time_;
}

/**
 * Returns the channel limit. If no limit has been set, this returns zero.
 *
 * @return Channel limit
 */
const uint32_t& UnrealChannel::limit()
{
	return limit_;
}

/**
 * Returns a lowercase version of the channel name, used for channel hashing.
 *
 * @return Lowercase channel name
 */
String UnrealChannel::lowerName()
{
	return name_.toLower();
}

/**
 * Send modes.
 *
 * @param uptr User entry pointer
 * @param mblist Mode buffer list
 */
void UnrealChannel::modebufSend(UnrealUser* uptr, List<ModeBuf>& mblist)
{
	ModeBuf mbuf;
	StringList argv;
	String mflags;
	ModeBuf::StateType last_state = ModeBuf::None;
	size_t count = 0;

	foreach (List<ModeBuf>::Iterator, mbi, mblist)
	{
		mbuf = *mbi;

		if (last_state != mbuf.state)
		{
			last_state = mbuf.state;
			mflags += mbuf.statestr();
		}

		if (mbuf.uptr)
			argv << mbuf.uptr->nick();
		else if (!mbuf.args.empty())
			argv << mbuf.args;
		else if (mbuf.mode == UnrealChannelProperties::Key)
			argv << key_;
		else if (mbuf.mode == UnrealChannelProperties::Limit
				&& last_state == ModeBuf::Add)
			argv << String(limit_);

		mflags.append(1, mbuf.mode.mode_char);
		count++;

		if (count >= 6)
		{
			String repl = mflags;

			if (argv.size() > 0)
				repl += " " + argv.join(" ");

			sendlocalreply(uptr, CMD_MODE, repl);

			argv.clear();
			mflags.clear();

			last_state = ModeBuf::None;
			count = 0;
		}
	}

	if (count > 0)
	{
		String repl = mflags;

		if (argv.size() > 0)
			repl += " " + argv.join(" ");

		sendlocalreply(uptr, CMD_MODE, repl);
	}
}

/**
 * Returns the current channel mode bitmask.
 *
 * @return Mode Bitmask
 */
Bitmask<uint32_t>& UnrealChannel::modes()
{
	return modes_;
}

/**
 * Returns a readable version of the channel modes.
 *
 * @return Channel mode string w/ params
 */
String UnrealChannel::modestr()
{
	String result;

	/* channel mode table */
	UnrealChannelModeTable& modetab = UnrealChannelProperties::ModeTable;

	foreach (UnrealChannelModeTable::Iterator, cmi, modetab)
	{
		UnrealChannelMode mo = cmi->first;

		if (modes_.isset(cmi->second))
			result.append(1, mo.mode_char);
	}

	/* append additional parameters if required */
	if (isKey())
		result += " " + key_;

	if (isLimit())
		result += " " + String(limit_);

	return result;
}

/**
 * Returns the channel name.
 *
 * @return Channel name
 */
const String& UnrealChannel::name()
{
	return name_;
}

/**
 * Parse channel mode changes.
 *
 * @param uptr User entry pointer
 * @param argv Arguments
 */
void UnrealChannel::parseModeChange(UnrealUser* uptr, StringList* argv)
{
	ModeBuf::StateType state = ModeBuf::Add;
	size_t par = 1;
	String flagset = argv->at(0);
	List<ModeBuf> mbuf_changeset;
	Member* cmptr = findMember(uptr);

	if (!cmptr)
		return;

	/* mode table */
	UnrealChannelModeTable& modetab = UnrealChannelProperties::ModeTable;

	foreach_str(ch, flagset)
	{
		if (*ch == '+' || *ch == '-')
			state = (*ch == '+') ? ModeBuf::Add : ModeBuf::Remove;
		else
		{
			if (!modetab.hasFlag(*ch))
			{
				uptr->sendreply(ERR_UNKNOWNMODE,
					String::format(MSG_UNKNOWNMODE,
						*ch));
				continue;
			}

			/* lookup mode */
			UnrealChannelMode cmo = modetab.lookup(*ch);
			uint32_t fl = modetab.value(cmo);

			if (state == ModeBuf::Add)
			{
				if (cmo == UnrealChannelProperties::Ban && argv->size()
						< (par + 1))
				{
					sendBanList(uptr);
					continue;
				}
				
				/* check for privileges */
				else if (((cmo == UnrealChannelProperties::Voice
						&& !cmptr->isChanOp() && !cmptr->isHalfOp())
					|| (cmo != UnrealChannelProperties::Voice
						&& !cmptr->isChanOp())) && !uptr->isOper())
				{
					sendreply(uptr, ERR_CHANOPRIVSNEEDED, MSG_CHANOPRIVSNEEDED);
					continue;
				}

				/* check for modes that require additional parameters */
				else if (cmo.param_count > 0 && argv->size()
						< (par + cmo.param_count))
				{
					uptr->sendreply(ERR_NEEDMOREPARAMS,
						String::format(MSG_NEEDMOREPARAMS,
							CMD_MODE));
					continue;
				}

				/* apply mode flag; dont apply the flag on member modes */
				if (!modes_.isset(fl)
						&& cmo != UnrealChannelProperties::Ban
						&& cmo != UnrealChannelProperties::HalfOp
						&& cmo != UnrealChannelProperties::ChanOp
						&& cmo != UnrealChannelProperties::Voice)
				{
					ModeBuf mbuf;

					mbuf.uptr = 0;
					mbuf.mode = cmo;
					mbuf.state = state;

					mbuf_changeset << mbuf;

					modes_ << fl;
				}

				if (cmo == UnrealChannelProperties::Key)
					setKey(argv->at(par++));
				else if (cmo == UnrealChannelProperties::Limit)
					setLimit(argv->at(par++).toUInt());
				else if (cmo == UnrealChannelProperties::Ban)
				{
					String mask = argv->at(par++);

					if (!findBan(mask))
					{
						ModeBuf mbuf;

						mbuf.uptr  = 0;
						mbuf.args  = mask;
						mbuf.mode  = cmo;
						mbuf.state = state;

						mbuf_changeset << mbuf;

						addBan(mask, uptr->nick());
					}
				}
				else if (cmo == UnrealChannelProperties::HalfOp
						|| cmo == UnrealChannelProperties::ChanOp
						|| cmo == UnrealChannelProperties::Voice)
				{
					UnrealUser* tuptr = UnrealUser::find(argv->at(par++));
					Member* tumptr = 0;

					if (!tuptr)
						uptr->sendreply(ERR_NOSUCHNICK,
							String::format(MSG_NOSUCHNICK,
								argv->at(par - 1).c_str()));
					else if (!(tumptr = findMember(tuptr)))
						sendreply(uptr, ERR_USERNOTINCHANNEL,
							String::format(MSG_USERNOTINCHANNEL,
								tuptr->nick().c_str()));
					else if ((cmo == UnrealChannelProperties::HalfOp
									&& !tumptr->isHalfOp())
							|| (cmo == UnrealChannelProperties::ChanOp
									&& !tumptr->isChanOp())
							|| (cmo == UnrealChannelProperties::Voice
									&& !tumptr->isVoiced()))
					{
						ModeBuf mbuf;

						mbuf.uptr = tuptr;
						mbuf.mode = cmo;
						mbuf.state = state;

						/* adding mode change to changeset */
						mbuf_changeset << mbuf;

						/* apply flag */
						if (cmo == UnrealChannelProperties::HalfOp)
							tumptr->flags << Member::HalfOp;
						else if (cmo == UnrealChannelProperties::ChanOp)
							tumptr->flags << Member::ChanOp;
						else
							tumptr->flags << Member::Voice;
					}
				}
			}
			else if (state == ModeBuf::Remove)
			{
				if (cmo == UnrealChannelProperties::Ban && argv->size()
						< (par + 1))
				{
					sendBanList(uptr);
					continue;
				}
				
				/* check for privileges */
				else if (((cmo == UnrealChannelProperties::Voice
						&& !cmptr->isChanOp() && !cmptr->isHalfOp())
					|| (cmo != UnrealChannelProperties::Voice
						&& !cmptr->isChanOp())) && !uptr->isOper())
				{
					sendreply(uptr, ERR_CHANOPRIVSNEEDED, MSG_CHANOPRIVSNEEDED);
					continue;
				}
				
				/* check if the mode change requires additional parameters;
				 * removing the channel limit usually doesn't require the limit
				 */
				else if (cmo.param_count > 0 && argv->size()
						< (par + cmo.param_count)
						&& cmo != UnrealChannelProperties::Limit)
				{
					uptr->sendreply(ERR_NEEDMOREPARAMS,
						String::format(MSG_NEEDMOREPARAMS,
							CMD_MODE));
					continue;
				}

				/* apply mode flag */
				if (modes_.isset(fl)
					&& cmo != UnrealChannelProperties::Ban
					&& cmo != UnrealChannelProperties::HalfOp
					&& cmo != UnrealChannelProperties::ChanOp
					&& cmo != UnrealChannelProperties::Voice)
				{
					ModeBuf mbuf;

					mbuf.uptr = 0;
					mbuf.mode = cmo;
					mbuf.state = state;

					mbuf_changeset << mbuf;

					modes_.revoke(fl);
				}

				if (cmo == UnrealChannelProperties::Key)
					setKey("");
				else if (cmo == UnrealChannelProperties::Limit)
					setLimit(0);
				else if (cmo == UnrealChannelProperties::Ban)
				{
					String mask = argv->at(par++);

					if (findBan(mask))
					{
						ModeBuf mbuf;

						mbuf.uptr = 0;
						mbuf.args = mask;
						mbuf.mode = cmo;
						mbuf.state = state;

						mbuf_changeset << mbuf;

						removeBan(mask);
					}
				}
				else if (cmo == UnrealChannelProperties::HalfOp
						|| cmo == UnrealChannelProperties::ChanOp
						|| cmo == UnrealChannelProperties::Voice)
				{
					UnrealUser* tuptr = UnrealUser::find(argv->at(par++));
					Member* tumptr = 0;

					if (!tuptr)
						uptr->sendreply(ERR_NOSUCHNICK,
							String::format(MSG_NOSUCHNICK,
								argv->at(par - 1).c_str()));
					else if (!(tumptr = findMember(tuptr)))
						sendreply(uptr, ERR_USERNOTINCHANNEL,
							String::format(MSG_USERNOTINCHANNEL,
								tuptr->nick().c_str()));
					else if ((cmo == UnrealChannelProperties::HalfOp
									&& tumptr->isHalfOp())
							|| (cmo == UnrealChannelProperties::ChanOp
									&& tumptr->isChanOp())
							|| (cmo == UnrealChannelProperties::Voice
									&& tumptr->isVoiced()))
					{
						ModeBuf mbuf;

						mbuf.uptr = tuptr;
						mbuf.mode = cmo;
						mbuf.state = state;

						/* adding mode change to changeset */
						mbuf_changeset << mbuf;

						/* apply flag */
						if (cmo == UnrealChannelProperties::HalfOp)
							tumptr->flags.revoke(Member::HalfOp);
						else if (cmo == UnrealChannelProperties::ChanOp)
							tumptr->flags.revoke(Member::ChanOp);
						else
							tumptr->flags.revoke(Member::Voice);
					}
				}
			}
		}
	}

	/* send mode changes */
	if (mbuf_changeset.size() > 0)
		modebufSend(uptr, mbuf_changeset);
}

/**
 * Remove a ban from the channel.
 *
 * @param mask Ban mask
 */
void UnrealChannel::removeBan(const String& mask)
{
	Ban* cbptr = findBan(mask);

	if (cbptr)
	{
		banlist.remove(cbptr);
		delete cbptr;
	}
}

/**
 * Remove a member from the channel.
 *
 * @param cptr Client pointer
 */
void UnrealChannel::removeMember(UnrealUser* uptr)
{
	Member* cmptr = findMember(uptr);

	if (cmptr)
	{
		uptr->channels.remove(this);
		members.remove(uptr);
		delete cmptr;
	}
}

/**
 * Send the channel banlist to the specified user.
 *
 * @param uptr User entry pointer
 */
void UnrealChannel::sendBanList(UnrealUser* uptr)
{
	for (BanIterator cbi = banlist.begin(); cbi != banlist.end(); cbi++)
	{
		Ban* cbptr = *cbi;

		sendreply(uptr, RPL_BANLIST,
			String::format(MSG_BANLIST,
				cbptr->mask.c_str(),
				cbptr->originator.c_str(),
				cbptr->lastmod.toTS()));
	}

	sendreply(uptr, RPL_ENDOFBANLIST, MSG_ENDOFBANLIST);
}

/**
 * Send a command to all users on the channel, originating from
 * a specified user hostmask.
 *
 * @param uptr User where message originates from
 * @param cmd Command
 * @param data Additional data
 * @param skip_sender If specified, don't send the reply to the
 * message originator
 */
void UnrealChannel::sendlocalreply(UnrealUser* uptr, const String& cmd,
	const String& data, bool skip_sender)
{
	String reply;

	reply.sprintf(":%s!%s@%s %s %s %s",
		uptr->nick().c_str(),
		uptr->ident().c_str(),
		uptr->hostname().c_str(),
		cmd.c_str(),
		name_.c_str(),
		data.c_str());

	/* send the message to all users on the channel */
	foreach (MemberIterator, cmi, members)
	{
		UnrealUser* tuptr = cmi->first;

		if (skip_sender && (tuptr == uptr))
			continue;

		tuptr->send(reply);
	}
}

/**
 * Send a numeric reply to a single user containing channel name, originating
 * from the server.
 *
 * @param uptr Pointer to user sending reply to
 * @param numeric Numeric
 * @param data Data block
 */
void UnrealChannel::sendreply(UnrealUser* uptr, IRCNumeric numeric,
		const String& data)
{
	int num = static_cast<int>(numeric);
	String reply;

	reply.sprintf(":%s %03d %s %s %s",
		unreal->me.name().c_str(),
		num,
		uptr->nick().c_str(),
		name_.c_str(),
		data.c_str());

	uptr->send(reply);
}

/**
 * Update the channel creation timestamp.
 *
 * @param ts New timestamp
 */
void UnrealChannel::setCreationTime(const UnrealTime& ts)
{
	creation_time_ = ts;
}

/**
 * Update the channel key.
 *
 * @param keystr New key
 */
void UnrealChannel::setKey(const String& keystr)
{
	key_ = keystr;
}

/**
 * Update the last join timestamp on the channel.
 *
 * @param ts New timestamp
 */
void UnrealChannel::setLastJoinTime(const UnrealTime& ts)
{
	last_join_time_ = ts;
}

/**
 * Update the channel limit.
 *
 * @param lim New limit
 */
void UnrealChannel::setLimit(const uint32_t& lim)
{
	limit_ = lim;
}

/**
 * Update the channel name and add it to the channel map.
 *
 * @param name New name
 */
void UnrealChannel::setName(const String& chname)
{
	/* remove old name from channel list*/
	if (!name_.empty())
	{
		String cur_lower = lowerName();

		if (unreal->channels.contains(cur_lower))
			unreal->channels.remove(cur_lower);
	}

	String lower_cn = chname;
	lower_cn = lower_cn.toLower();

	/* add channel to channel list */
	unreal->channels.add(lower_cn, this);

	name_ = chname;
}

/**
 * Update channel topic.
 *
 * @param msg New channel topic
 */
void UnrealChannel::setTopic(const String& msg)
{
	topic_ = msg;
}

/**
 * Update channel topic originator mask.
 *
 * @param mask Originator mask
 */
void UnrealChannel::setTopicMask(const String& mask)
{
	topic_mask_ = mask;
}

/**
 * Update channel topic timestamp.
 *
 * @param ts New timestamp
 */
void UnrealChannel::setTopicTime(const UnrealTime& ts)
{
	topic_time_ = ts;
}

/**
 * Returns the channel topic.
 *
 * @return Channel topic
 */
const String& UnrealChannel::topic()
{
	return topic_;
}

/**
 * Returns the mask of the channel topic originator (can be either
 * a valid hostmask, or a server name).
 *
 * @return Topic mask
 */
const String& UnrealChannel::topicMask()
{
	return topic_mask_;
}

/**
 * Returns the channel topic timestamp.
 *
 * @return UnrealTime
 */
UnrealTime UnrealChannel::topicTime()
{
	return topic_time_;
}
