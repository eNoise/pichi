/*
    Pichi XMPP (Jabber) Bot.
    Copyright (C) 2010  Alexey Kasyanchuk (deg@uruchie.org)

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

*/

#include "lastmessage.h"

namespace pichi
{

lastmessage::lastmessage()
{

}

  
bool lastmessage::reciveMessage(const std::string& message, const std::string& type, const std::string& from)
{
	return false;
}
  
std::string lastmessage::getLastRoom(void )
{
	return last_room;
}

lastmessage::lastmessage(const pichi::lastmessage& lst)
{
	(*this) = lst;
}

lastmessage& lastmessage::operator=(const pichi::lastmessage& lst)
{
	last_message = lst.last_message;
	last_from = lst.last_from;
	last_jid = lst.last_jid;
	last_room = lst.last_room;
	last_type = lst.last_type;
	last_level = lst.last_level;
	last_time = lst.last_time;
	
	last_args = lst.last_args;
	last_command = lst.last_command;
	
	return (*this);
}

std::string lastmessage::getJIDlast(void )
{
	return last_jid;
}

std::string lastmessage::getFromlast(void )
{
	return last_from;
}

std::string lastmessage::getArg(void )
{
	return last_args;
}

std::string lastmessage::getMsgLast(void )
{
	return last_message;
}

}