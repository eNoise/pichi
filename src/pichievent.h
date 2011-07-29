/*
    Pichi XMPP (Jabber) Bot.
    Copyright (C) 2011  Alexey Kasyanchuk (deg@uruchie.org)

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


#ifndef PICHIEVENT_H
#define PICHIEVENT_H

#include <string>
#include <map>
#include "system.h"
#include <vector>

namespace pichi
{

class PichiCore;

class PichiEvent
{

public:
    PichiEvent(PichiCore*);
    void callEvent(std::string action, std::string coincidence = "");
    void deleteEvent(std::string action, std::string coincidence = "");
private:
    PichiCore* pichi;
    void doAction(std::string action, std::string doaction, std::string value, std::string option, std::string coincidence = "");
};

}

#endif // PICHIEVENT_H
