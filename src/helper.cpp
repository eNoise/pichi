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

#include "helper.h"
#include <boost/algorithm/string/regex.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/foreach.hpp>

#include <dirent.h>
#include <sys/time.h>
#include <sys/stat.h>


namespace pichi
{

std::string Helper::pichiHeader(void )
{
	std::string header;
	header += "\n ______   __   ______      __  __   __    \n";
	header += "/\\  == \\ /\\ \\   /\\  ___\\     /\\ \\_\\ \\   /\\ \\   \n";
	header += "\\ \\   _-/   \\ \\ \\  \\ \\ \\____   \\ \\  __ \\  \\ \\ \\  \n";
	header += " \\ \\_\\       \\ \\_\\  \\ \\_____\\  \\ \\_\\ \\_\\  \\ \\_\\ \n";
	header += "  \\/_/        \\/_/   \\/_____/   \\/_/\\/_/   \\/_/ \n";
	return header;
}
  
std::string Helper::timeToString(time_t t, std::string format)
{
	struct tm * ptm = localtime(&t);
	char buf[100];
	strftime (buf, 100, format.c_str(),  ptm);
	std::string result(buf);
	//delete ptm;
	return result;
}

std::vector< std::string > Helper::explode(std::string seperator, std::string str)
{
	std::vector< std::string > ret;
	boost::split_regex( ret, str, boost::regex( seperator ) ) ;
	return ret;
}

std::string Helper::implode(std::string seperator, std::vector< std::string > strs)
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

template<typename compT> bool Helper::in_array(compT& val, std::vector<compT>& array)
{
	return (std::find(array.begin(), array.end(), val) != array.end());
}

template bool Helper::in_array<std::string>(std::string& val, std::vector<std::string>& array);

std::string Helper::stringTime(time_t time)
{
	return boost::lexical_cast<std::string>(time);
}

int Helper::atoi(std::string str)
{
	return boost::lexical_cast<int>( str );
}

std::string Helper::itoa(int num)
{
	return boost::lexical_cast<std::string>( num );
}

std::vector< std::string > Helper::splitBySize(const std::string& str, size_t size)
{
	std::vector< std::string > ret;
	for(size_t cursize = 0; cursize < str.size() && size > 0; cursize += size)
		ret.push_back(str.substr(cursize, size));
	
	if(ret.size() == 0)
		ret.push_back(str);
	
	return ret;
}

size_t Helper::atot(std::string str)
{
	return boost::lexical_cast<size_t>( str );
}

std::string Helper::ttoa(size_t t)
{
	return boost::lexical_cast<std::string>( t );
}

const std::string Helper::getFullPath(const std::string& dir)
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

bool Helper::fileExists(const std::string& file)
{
	struct stat st;
	return (stat(getFullPath(file).c_str(), &st ) == 0);
}

std::vector< std::string > Helper::getDirFiles(const std::string& path)
{
	std::vector< std::string > files;
	DIR* dp;
	struct dirent *dirp;
	if((dp = opendir(path.c_str())) == NULL)
		return files;
	while((dirp = readdir(dp)) != NULL)
		if(std::string(dirp->d_name) != "." && std::string(dirp->d_name) != "..")
			files.push_back(dirp->d_name);
	closedir(dp);
	return files;
}

double Helper::microtime(void )
{
	struct timeval tp;
	gettimeofday(&tp, NULL);
	double msec = (double) (tp.tv_usec / 1000000.0);
	double sec = tp.tv_sec;
	return (sec + msec);
}

}
