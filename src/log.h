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

#ifndef LOG_H
#define LOG_H

#include <string>

namespace pichi
{

// Mingw fix
#define TEMPERROR ERROR
#undef ERROR
  
class Log
{
  public:
	static int LEVEL;
	static std::string log_file;
	static bool file_log;
	enum LogType
	{
		ERROR,
		WARNING,
		INFO,
		DEBUG,
		VERBOSE
	};
	Log(std::string, LogType);
	Log(std::string);
	void showLog(std::string, LogType);
};

#define ERROR TEMPERROR

}

#endif // LOG_H
