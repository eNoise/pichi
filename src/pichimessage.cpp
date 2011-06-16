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

#include "pichimessage.h"

namespace pichi
{

PichiMessage::PichiMessage()
{

}


bool PichiMessage::reciveMessage(const std::string& message, const std::string& type, const std::string& from)
{
    return false;
}

std::string PichiMessage::getLastRoom(void )
{
    return last_room;
}

PichiMessage::PichiMessage(const pichi::PichiMessage& lst)
{
    (*this) = lst;
}

PichiMessage& PichiMessage::operator=(const pichi::PichiMessage& lst)
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

std::string PichiMessage::getJIDlast(void )
{
    return last_jid;
}

std::string PichiMessage::getFromlast(void )
{
    return last_from;
}

std::string PichiMessage::getArg(void )
{
    return last_args;
}

std::string PichiMessage::getMsgLast(void )
{
    return last_message;
}

}
