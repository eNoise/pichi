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

#ifndef SYSTEM_H
#define SYSTEM_H

#include <string>
#include <sys/stat.h>
#include <time.h>
#include <sys/time.h>
#include <vector>
#include <boost/algorithm/string/regex.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/foreach.hpp>
#include <stdlib.h>

namespace pichi
{

class system
{
  protected:
	
  public:
	system();
	~system();
	
	static std::vector< std::string > explode(std::string seperator, std::string str);
	static std::string implode(std::string, std::vector< std::string >);
	static std::string timeToString(time_t, std::string);
	static std::string stringTime(time_t);
	static std::string itoa(int);
	static std::vector< std::string > splitBySize(std::string& str, size_t size);
	static int atoi(std::string);
	static size_t atot(std::string);
	static std::string ttoa(size_t);
	static const std::string getFullPath(const std::string& dir);
	static bool fileExists(const std::string& file);
	template<typename compT> static bool in_array(compT&, std::vector<compT>&);
	static double microtime(void);
};

}

#endif // SYSTEM_H
