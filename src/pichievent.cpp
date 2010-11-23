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


#include "pichievent.h"
#include "pichicore.h"
#include "core.h"

namespace pichi
{

PichiEvent::PichiEvent(pichicore* p)
{
	pichi = p;
}

void PichiEvent::callEvent(std::string action, std::string coincidence)
{
	pichi->sql->query("SELECT `do`, `option`,`value`,`coincidence` FROM actions WHERE action = '" + action + "' AND coincidence='" + ((coincidence != "") ? pichi->sql->escapeString(coincidence) : "" ) + "';");
	std::map<std::string, std::string> actions;
	while(!(actions = pichi->sql->fetchArray()).empty())
	{
		doAction(actions["do"], actions["value"], actions["option"], actions["coincidence"]);
	}
}

void PichiEvent::doAction(std::string action, std::string value, std::string option, std::string coincidence)
{
	std::vector<std::string> exploder;
	if(action == "send_message")
	{
		std::string room;
		if(coincidence != "")
		{
			exploder = system::explode(",", coincidence);
			exploder = system::explode("=", exploder[0]);
			if(exploder[0] == "room")
			{
				room = exploder[1];
			}
			else
			{
				//$this->log->log("Old send_message handler", PichiLog::LEVEL_WARNING);
				return;
			}
		}
		else
		{
			return;
		}
		if(option == "")
			if(value != "")
				pichi->jabber->sendMessage(JID(room), value);
		//$this->log->log("EVENT: {$action} (Send message {$value})", PichiLog::LEVEL_VERBOSE);
		return;
	}
	//($hook = PichiPlugin::fetch_hook('event_action')) ? eval($hook) : false;
}

}
