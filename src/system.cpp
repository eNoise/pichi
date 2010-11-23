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

#include "system.h"

namespace pichi
{

std::string system::timeToString(time_t t, std::string format)
{
	struct tm * ptm = localtime(&t);
	char buf[100];
	strftime (buf, 100, format.c_str(),  ptm);
	std::string result(buf);
	//delete ptm;
	return result;
}

std::vector< std::string > system::explode(std::string seperator, std::string str)
{
	std::vector< std::string > ret;
	boost::split_regex( ret, str, boost::regex( seperator ) ) ;
	return ret;
}

std::string system::implode(std::string seperator, std::vector< std::string > strs)
{
	  std::string ret;

	  for(std::vector< std::string >::iterator it = strs.begin(), tempit = strs.begin(); it != strs.end(); it++, tempit = it)
	  {
		tempit++;
		if(tempit == strs.end())
			ret += (*it);
		else
			ret += (*it) + seperator;
	  }
	 return ret;
}

template<typename compT> bool system::in_array(compT& val, std::vector<compT>& array)
{
	return (std::find(array.begin(), array.end(), val) != array.end());
}

template bool system::in_array<std::string>(std::string& val, std::vector<std::string>& array);

std::string system::stringTime(time_t time)
{
	return boost::lexical_cast<std::string>(time);
}

int system::atoi(std::string str)
{
	return boost::lexical_cast<int>( str );
}

std::string system::itoa(int num)
{
	return boost::lexical_cast<std::string>( num );
}

std::vector< std::string > system::splitBySize(std::string& str, size_t size)
{
	std::vector< std::string > ret;
	for(size_t cursize = 0; cursize < str.size(); cursize += size)
		ret.push_back(str.substr(cursize, size));
	
	return ret;
}

size_t system::atot(std::string str)
{
	return boost::lexical_cast<size_t>( str );
}

std::string system::ttoa(size_t t)
{
	return boost::lexical_cast<std::string>( t );
}

const std::string system::getFullPath(const std::string& dir)
{
	if (dir[0] != '~')
		return dir;
#ifndef WIN32
	std::string home = getenv("HOME");
	return (home + dir.substr(1));
#else
	return dir;
#endif
}

bool system::fileExists(const std::string& file)
{
	struct stat st;
	return (stat(getFullPath(file).c_str(), &st ) == 0);
}

}
