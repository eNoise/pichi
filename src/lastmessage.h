/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) <year>  <name of author>

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

#ifndef LASTMESSAGE_H
#define LASTMESSAGE_H

#include <string>
#include <time.h>

namespace pichi
{
  
class lastmessage
{
	public:
		virtual bool reciveMessage(std::string message, std::string type, std::string from, std::string jid = "", std::string room = "", int level = 2) = 0;
		std::string getLastRoom(void);
	protected:
		std::string last_message;
		std::string last_from;
		std::string last_type;
		std::string last_room;
		std::string last_jid;
		time_t last_time;
		int last_level;
};

}

#endif // LASTMESSAGE_H
