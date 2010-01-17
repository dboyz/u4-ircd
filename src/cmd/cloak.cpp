/*****************************************************************
 * Unreal Internet Relay Chat Daemon, Version 4
 * File         cloak.cpp
 * Description  CLOAK command handler
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

#include <base.hpp>
#include <cmdmap.hpp>
#include <command.hpp>
#include <module.hpp>

#include <crypto++/md5.h>


/** Module informations */
UnrealModule::Info modinf =
{
	/** Module name */
	"Unreal3.x style host cloaking support",

	/** Module version */
	"1.0",

	/** Module author */
	"(Packaged)"
};

/** Command Instance */
UnrealUserCommand* ucmd_cloak = NULL;
UnrealUserCommand* ucmd_uncloak = NULL;

UnrealUserMode HostCloaking('x');

namespace HostCloak 
{
	String keys[3];
	String checksum;
	String hostPrefix;
	CryptoPP::MD5 hash;
	
	bool isBadRandomness(const String& s)
	{
		bool gotLowCase = false, gotUpCase = false, gotDigit = false;
		unsigned n = 0;
		
		for (n = 0; n < s.length(); ++n)
		{
			if (islower(s.at(n)))
				gotLowCase = true;
				
			else if (isupper(s.at(n)))
				gotUpCase = true;
				
			else if (isdigit(s.at(n)))
				gotDigit = true;
		}
		
		if (gotLowCase && gotUpCase && gotDigit)
			return false;
			
		return true;
	}
	
	
	inline bool isBadLength(const String &s)
	{
		if (s.length() < 5 || s.length() > 100)
			return true;
		
		return false;
	}
	
	void setKeys() 
	{
		keys[0] = unreal->config.get("Cloak::Key1");
		keys[1] = unreal->config.get("Cloak::Key2");
		keys[2] = unreal->config.get("Cloak::Key3");
		
		for (unsigned n = 0; n < 3; ++n) {
			if (keys[n].empty())
				unreal->log.write(UnrealLog::Fatal, "Cloak::Key%d is missing", n+1);
				
			else if (isBadLength(keys[n]))
				unreal->log.write(UnrealLog::Fatal, "Cloak::Key%d: Each key should be at least 5 and less than 100 characters", n+1);
				
			else if (isBadRandomness(keys[n]))
				unreal->log.write(UnrealLog::Fatal, "Cloak::Key%d: Keys should be mixed a-zA-Z0-9, like \"a2JO6fh3Q6w4oN3s7\"", n+1);
		}
		
		if (keys[0] == keys[1] || keys[1] == keys[2])
			unreal->log.write(UnrealLog::Fatal, "All your 3 keys should be RANDOM, they should not be equal");
			
		hostPrefix = unreal->config.get("Cloak::Prefix", unreal->config.get("Me::Network", "hidden"));
	}
	
	
	void run()
	{
		byte result[CryptoPP::MD5::DIGESTSIZE];
		
		String fullKey = keys[0] + ':' + keys[1] + ':' + keys[2];
		
		hash.CalculateDigest(result, (const byte*) fullKey.c_str(), fullKey.length());
		
		checksum.sprintf("MD5:%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x",
			(u_int)(result[0] & 0xf), (u_int)(result[0] >> 4),
			(u_int)(result[1] & 0xf), (u_int)(result[1] >> 4),
			(u_int)(result[2] & 0xf), (u_int)(result[2] >> 4),
			(u_int)(result[3] & 0xf), (u_int)(result[3] >> 4),
			(u_int)(result[4] & 0xf), (u_int)(result[4] >> 4),
			(u_int)(result[5] & 0xf), (u_int)(result[5] >> 4),
			(u_int)(result[6] & 0xf), (u_int)(result[6] >> 4),
			(u_int)(result[7] & 0xf), (u_int)(result[7] >> 4),
			(u_int)(result[8] & 0xf), (u_int)(result[8] >> 4),
			(u_int)(result[9] & 0xf), (u_int)(result[9] >> 4),
			(u_int)(result[10] & 0xf), (u_int)(result[10] >> 4),
			(u_int)(result[11] & 0xf), (u_int)(result[11] >> 4),
			(u_int)(result[12] & 0xf), (u_int)(result[12] >> 4),
			(u_int)(result[13] & 0xf), (u_int)(result[13] >> 4),
			(u_int)(result[14] & 0xf), (u_int)(result[14] >> 4),
			(u_int)(result[15] & 0xf), (u_int)(result[15] >> 4));
	}
	
	
	const String& cloakSum()
	{ 
		return checksum;
	}
	
	inline unsigned int downSample(char *i)
	{
	
		char r[4];

		r[0] = i[0] ^ i[1] ^ i[2] ^ i[3];
		r[1] = i[4] ^ i[5] ^ i[6] ^ i[7];
		r[2] = i[8] ^ i[9] ^ i[10] ^ i[11];
		r[3] = i[12] ^ i[13] ^ i[14] ^ i[15];
		
		return ( ((unsigned int)r[0] << 24) +
				 ((unsigned int)r[1] << 16) +
				 ((unsigned int)r[2] << 8) +
				 (unsigned int)r[3]);
	}
	
	
	/* Stolen from UnrealIRCd3 code -- cloak.c */
	String hideHost_ipv4(const char *host)
	{
		unsigned int a, b, c, d;
		static char res[512];
		unsigned long n;
		unsigned int alpha, beta, gamma;
		String buf;

		/* 
		 * Output: ALPHA.BETA.GAMMA.IP
		 * ALPHA is unique for a.b.c.d
		 * BETA  is unique for a.b.c.*
		 * GAMMA is unique for a.b.*
		 * We cloak like this:
		 * ALPHA = downsample(md5(md5("keys[1].c_str():A.B.C.D:keys[2].c_str()")+"keys[0].c_str()"));
		 * BETA  = downsample(md5(md5("keys[2].c_str():A.B.C:keys[0].c_str()")+"keys[1].c_str()"));
		 * GAMMA = downsample(md5(md5("keys[0].c_str():A.B:keys[1].c_str()")+"keys[2].c_str()"));
		 */
		 
		sscanf(host, "%u.%u.%u.%u", &a, &b, &c, &d);

		/* ALPHA... */
		buf = keys[1] + ':' + String(host) + ':' + keys[2];
		hash.CalculateDigest((byte*) res, (const byte*) buf.c_str(), buf.length());
		strcpy(res + 16, keys[0].c_str()); /* first 16 bytes are filled, append our key.. */
		n = strlen(res + 16) + 16;
		hash.CalculateDigest((byte*) res, (const byte*) res, n);
		alpha = downSample(res);

		/* BETA... */
		buf = keys[2] + ':' + String(a) + '.' + String(b) + '.' + String(c) + ':' + keys[0];
		hash.CalculateDigest((byte*) res, (const byte*) buf.c_str(), buf.length());
		strcpy(res+16, keys[1].c_str()); /* first 16 bytes are filled, append our key.. */
		n = strlen(res+16) + 16;
		hash.CalculateDigest((byte*) res, (const byte*) res, n);
		beta = downSample(res);

		/* GAMMA... */
		buf = keys[0] + ':' + String(a) + '.' + String(b) + ':' + keys[1];
		hash.CalculateDigest((byte*) res, (const byte*) buf.c_str(), buf.length());
		strcpy(res+16, keys[2].c_str()); /* first 16 bytes are filled, append our key.. */
		n = strlen(res+16) + 16;
		hash.CalculateDigest((byte*) res, (const byte*) res, n);
		gamma = downSample(res);

		buf.sprintf("%X.%X.%X.IP", alpha, beta, gamma);
		
		return buf;
	}
	
	
	/* Stolen from UnrealIRCd3 code -- cloak.c */
	String hideHost_normalHost(const char *host)
	{
		const char *p;
		static char res[512];
		unsigned int alpha, n;
		String buf;

		buf = keys[0] + ':' + String(host) + ':' + keys[1];
		hash.CalculateDigest((byte*) res, (const byte*) buf.c_str(), buf.length());
		
		strcpy(res+16, keys[2].c_str()); /* first 16 bytes are filled, append our key.. */
		n = strlen(res+16) + 16;
		hash.CalculateDigest((byte*) res, (const byte*) res, n);
		
		alpha = downSample(res);

		for (p = host; *p; ++p)
			if (*p == '.')
				if (isalpha(*(p + 1)))
					break;

		if (*p)
		{
			++p;
			buf.sprintf("%s-%X.%s", hostPrefix.c_str(), alpha, p);

		} else
			buf.sprintf("%s-%X", hostPrefix.c_str(), alpha);

		return buf;
	}
	

	const String hideHost(const String& s)
	{
		if (s.find(':') != String::npos)
			return s; // Ipv6 -- not supported yet; return unchanged hostname
		
		unsigned n;
		
		for (n = 0; n < s.length(); ++n)
			if (!isdigit(s.at(n)) && s.at(n) != '.')
				break;
				
		if (n == s.length())
			return hideHost_ipv4(s.c_str());
			
		return hideHost_normalHost(s.c_str());
	}

	
}
	
				
	

/**
 * CLOAK command handler for User connections.
 *
 * Usage:
 * CLOAK
 *
 * Message example:
 * CLOAK
 *
 * @param uptr Originating user
 * @param argv Argument list
 */
void uc_cloak(UnrealUser* uptr, StringList* argv)
{

		/** Check if +x is already set **/
		
		if (UnrealUserProperties::ModeTable.contains(HostCloaking) 
				&& uptr->modes().isset(UnrealUserProperties::ModeTable.value(HostCloaking)))
			return;


		uptr->setHostname(HostCloak::hideHost(uptr->realHostname()));
		
		uptr->modes().add(UnrealUserProperties::ModeTable.value(HostCloaking));

		uptr->sendreply(396, 
			String::format("%s :is now your visible host", uptr->hostname().c_str())
		);
		
		uptr->sendlocalreply(CMD_MODE, 
			String::format("%s +x", uptr->nick().c_str())
		);
}


/**
 * UNCLOAK command handler for User connections.
 *
 * Usage:
 * UNCLOAK
 *
 * Message example:
 * UNCLOAK
 *
 * @param uptr Originating user
 * @param argv Argument list
 */
void uc_uncloak(UnrealUser* uptr, StringList* argv)
{
		/** Return if +x is not set **/
		
		if (UnrealUserProperties::ModeTable.contains(HostCloaking) 
				&& !uptr->modes().isset(UnrealUserProperties::ModeTable.value(HostCloaking)))
			return;


		uptr->modes().revoke(UnrealUserProperties::ModeTable.value(HostCloaking));
		uptr->setHostname(uptr->realHostname());
		
		
		uptr->sendreply(396, 
			String::format("%s :is now your visible host", uptr->hostname().c_str())
		);
		
		uptr->sendlocalreply(CMD_MODE, 
			String::format("%s -x", uptr->nick().c_str())
		);
}

/**
 * Module initialization function.
 * Called when the Module is loaded.
 *
 * @param module Reference to Module
 */
UNREAL_DLL UnrealModule::Result unrInit(UnrealModule& module)
{
	/* update module info */
	module.info = modinf;

	/* register command */
	
	HostCloak::setKeys();
	HostCloak::run();
	
	UnrealUserProperties::ModeTable.registerMode(HostCloaking);
	
	ucmd_cloak = new UnrealUserCommand("CLOAK", &uc_cloak);
	ucmd_uncloak = new UnrealUserCommand("UNCLOAK", &uc_uncloak);

	return UnrealModule::Success;
}

/**
 * Module close function.
 * It's called before the Module is unloaded.
 */
UNREAL_DLL UnrealModule::Result unrClose(UnrealModule& module)
{
	UnrealUserProperties::ModeTable.deregisterMode(HostCloaking);
	
	delete ucmd_cloak;
	delete ucmd_uncloak;

	return UnrealModule::Success;
}
