/*  UnrealIRCd -- The Next Generation IRCd.
*
*   This program is free software; you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation; either version 1, or (at your option)
*   any later version.
*
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with this program; if not, write to the Free Software
*   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

class User
{
 public:
	/* The nick of the IRC user. */
	std::string nick;

	/* The ident of the IRC user. */
	std::string ident;

	/* The host of the IRC user. */
	std::string host;

	/* The GECOS (Real Name) of the IRC user. */
	std::string gecos;

	/* The IP of the IRC user. */
	std::string ip;

	/* A doubly linked list of the channels the IRC user is currently
	*  in.
	*/
	std::list in_channels;

	/* Is the user an oper, not taking into consideration any other
	*  privliges?
	*/
	bool isOper = false;

	/* Quit's a user with the specified kill message.
	*  Parameters:
	*  reason - The reason to quit the user.
	*/
	void Quit(const std::string& reason);

	/* Changes the user's nick with the specified parameter.
	*  Parameters:
	*  newnick - The new nick.
	*/
	int ChangeNick(const std::string& newnick);

	/* Sends a numeric message to the user.
	*  Parameters:
	*  numeric - The numeric to send.
	*  text - The text to send in conjunction with the numeric.
	*  ... - Any number of parameters.
	*/
	int SendNumeric(int numeric, const std::string& text, ...);
};
