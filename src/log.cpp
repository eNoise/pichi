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

#include "log.h"

#include <iostream>
#include "helper.h"
#include "config.h"
#include <fstream>
#include <boost/algorithm/string/erase.hpp>

namespace pichi
{

int Log::LEVEL = 3; //default
bool Log::file_log = false;
std::string Log::log_file = Helper::getFullPath(PICHI_LOG_FILE);

void Log::showLog(std::string lst, LogType type)
{
std::string logthis;
	switch(type)
	{
	  case ERROR:
		if(LEVEL > 0)
			logthis += ((file_log) ? "[" + Helper::timeToString(time(NULL), "%d.%m.%y") + "]" : "") + "[" + Helper::timeToString(time(NULL), "%H:%M:%S") + "][ERROR] " + lst + "\n";
		break;
	  case WARNING:
		if(LEVEL > 1)
			logthis += ((file_log) ? "[" + Helper::timeToString(time(NULL), "%d.%m.%y") + "]" : "") + "[" + Helper::timeToString(time(NULL), "%H:%M:%S") + "][WARNING] " + lst + "\n";
		break;
	  case INFO:
		if(LEVEL > 2)
			logthis += ((file_log) ? "[" + Helper::timeToString(time(NULL), "%d.%m.%y") + "]" : "") + "[" + Helper::timeToString(time(NULL), "%H:%M:%S") + "][INFO] " + lst + "\n";
		break;
	  case DEBUG:
		if(LEVEL > 3)
			logthis += ((file_log) ? "[" + Helper::timeToString(time(NULL), "%d.%m.%y") + "]" : "") + "[" + Helper::timeToString(time(NULL), "%H:%M:%S") + "][DEBUG] " + lst + "\n";
		break;
	  case VERBOSE:
		if(LEVEL > 4)
			logthis += ((file_log) ? "[" + Helper::timeToString(time(NULL), "%d.%m.%y") + "]" : "") + "[" + Helper::timeToString(time(NULL), "%H:%M:%S") + "][VERBOSE] " + lst + "\n";
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
#ifndef WITH_TESTS
		boost::erase_all(logthis, "\n");
		std::cout << logthis << std::endl;
#endif
	}
}


Log::Log(std::string lst)
{
	showLog(lst, INFO);
}

Log::Log(std::string lst, LogType type)
{  
	showLog(lst, type);
}

}
