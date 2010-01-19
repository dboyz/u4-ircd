/*****************************************************************
 * Unreal Internet Relay Chat Daemon, Version 4
 * File         hash.cpp
 * Description  Cryptographic hashing
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

#include "config.h"
#include "hash.hpp"

#ifndef HAVE_CRYPTOPP
#include <crypto++/hex.h>
#include <crypto++/sha.h>
#else
#include <cryptopp/hex.h>
#include <cryptopp/sha.h>
#endif

/**
 * Calculate the hash of the specified message and type and returns it
 * as the hexadecimal notation.
 *
 * @param message Input message
 * @param type Hash type
 * @return Result message
 */
String UnrealHash::calculate(const String& message, Type type)
{
	const size_t ds = digest_size(type);
	byte* input = (byte *)message.c_str();
	byte bin_buffer[ds];
	byte out_buffer[(ds * 2) + 1];
	CryptoPP::HexEncoder he;

	switch (type)
	{
		case SHA1:
		{
			CryptoPP::SHA1 sha;

			sha.CalculateDigest(bin_buffer, input, message.length());
			he.Put(bin_buffer, ds);

			break;
		}

		case SHA224:
		{
			CryptoPP::SHA224 sha;
			
			sha.CalculateDigest(bin_buffer, input, message.length());
			he.Put(bin_buffer, ds);

			break;
		}

		case SHA256:
		{
			CryptoPP::SHA256 sha;

			sha.CalculateDigest(bin_buffer, input, message.length());
			he.Put(bin_buffer, ds);

			break;
		}

		case SHA384:
		{
			CryptoPP::SHA384 sha;

			sha.CalculateDigest(bin_buffer, input, message.length());
			he.Put(bin_buffer, ds);

			break;
		}

		case SHA512:
		{
			CryptoPP::SHA512 sha;

			sha.CalculateDigest(bin_buffer, input, message.length());
			he.Put(bin_buffer, ds);

			break;
		}
		
		case Plain:
			return message;

		default:
			break;
	}

	/* now get the hexadecimal value of the hash */
	he.Get(out_buffer, ds * 2);
	out_buffer[ds * 2] = 0;

	/* and return it as string */
	return String((char *)out_buffer);
}

/**
 * Returns the digest size for the specified hash type.
 *
 * @param type Hash type
 * @return Digest size
 */
const size_t UnrealHash::digest_size(Type type)
{
	switch (type)
	{
		case SHA1:
			return CryptoPP::SHA1::DIGESTSIZE;
		case SHA224:
			return CryptoPP::SHA224::DIGESTSIZE;
		case SHA256:
			return CryptoPP::SHA256::DIGESTSIZE;
		case SHA384:
			return CryptoPP::SHA384::DIGESTSIZE;
		case SHA512:
			return CryptoPP::SHA512::DIGESTSIZE;
		default:
			return 0;
	}
}

/**
 * Returns the type identifier for the specified hash type string.
 *
 * @param str Hash type string
 * @return Hash type identifier
 */
UnrealHash::Type UnrealHash::strtotype(const String& str)
{
	if (str == "SHA" || str == "SHA-1")
		return SHA1;
	else if (str == "SHA-224")
		return SHA224;
	else if (str == "SHA-256")
		return SHA256;
	else if (str == "SHA-384")
		return SHA384;
	else if (str == "SHA-512")
		return SHA512;
	else if (str == "PLAIN")
		return Plain;
	else
		return Invalid;
}
