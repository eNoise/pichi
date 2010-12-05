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

#include "log.h"

namespace pichi
{

int LOG::LEVEL = 3; //default
bool LOG::file_log = false;
std::string LOG::log_file = system::getFullPath(PICHI_LOG_FILE);

LOG::LOG(std::string lst, LOG::LogType type)
{  
	std::string logthis;
	switch(type)
	{
	  case ERROR:
		if(LEVEL > 0)
			logthis += "[" + system::timeToString(time(NULL), "%H:%M:%S") + "][ERROR] " + lst + "\n";
		break;
	  case WARNING:
		if(LEVEL > 1)
			logthis += "[" + system::timeToString(time(NULL), "%H:%M:%S") + "][WARNING] " + lst + "\n";
		break;
	  case INFO:
		if(LEVEL > 2)
			logthis += "[" + system::timeToString(time(NULL), "%H:%M:%S") + "][INFO] " + lst + "\n";
		break;
	  case DEBUG:
		if(LEVEL > 3)
			logthis += "[" + system::timeToString(time(NULL), "%H:%M:%S") + "][DEBUG] " + lst + "\n";
		break;
	  case VERBOSE:
		if(LEVEL > 4)
			logthis += "[" + system::timeToString(time(NULL), "%H:%M:%S") + "][VERBOSE] " + lst + "\n";
		break;
	}
	if(file_log)
	{
		std::ofstream file(log_file.c_str(), std::ios::out|std::ios::app);
		file << logthis;
		file.close();
	}
	else
	{
		boost::erase_all(logthis, "\n");
		std::cout << logthis << std::endl;
	}
}

}
