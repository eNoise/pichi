/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) <2010>  <Alexey Kasyanchuk (deg@uruchie.org)>

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

int LOG::LEVEL = 3; //default

LOG::LOG(std::string lst, LOG::LogType type)
{  
	switch(type)
	{
	  case ERROR:
		if(LEVEL > 0)
			std::cout << "[" + system::timeToString(time(NULL), "%H:%M:%S") + "][ERROR] " << lst << std::endl;
		break;
	  case WARNING:
		if(LEVEL > 1)
			std::cout << "[" + system::timeToString(time(NULL), "%H:%M:%S") + "][WARNING] " << lst << std::endl;
		break;
	  case INFO:
		if(LEVEL > 2)
			std::cout << "[" + system::timeToString(time(NULL), "%H:%M:%S") + "][INFO] " << lst << std::endl;
		break;
	  case DEBUG:
		if(LEVEL > 3)
			std::cout << "[" + system::timeToString(time(NULL), "%H:%M:%S") + "][DEBUG] " << lst << std::endl;
		break;
	  case VERBOSE:
		if(LEVEL > 4)
			std::cout << "[" + system::timeToString(time(NULL), "%H:%M:%S") + "][VERBOSE] " << lst << std::endl;
		break;
	}
}