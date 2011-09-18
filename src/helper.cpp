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
#include <openssl/md5.h>

#include "config.h"
#include "pichiexception.h"

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

bool Helper::createDirectory(const std::string& path, const int mask)
{
#ifndef WIN32
	return mkdir(path.c_str(), mask) == 0;
#else
	return _mkdir(path.c_str()) == 0;
#endif
}

bool Helper::removeDirectory(const std::string& path)
{
	return rmdir(path.c_str()) == 0;
}

bool Helper::removeFile(const std::string& path)
{
	return remove(path.c_str()) == 0;
}

std::string Helper::getShareFile(const std::string& file, bool isThrowEx)
{
	if(Helper::fileExists(Helper::getFullPath(PICHI_CONFIG_DIR) + file))
		return Helper::getFullPath(PICHI_CONFIG_DIR) + file;
	else if(Helper::fileExists(Helper::getFullPath(PICHI_INSTALLED_DIR) + file))
		return Helper::getFullPath(PICHI_INSTALLED_DIR) + file;
	else if(Helper::fileExists(Helper::getFullPath(PICHI_SOURCE_DIR) + file))
		return Helper::getFullPath(PICHI_SOURCE_DIR) + file;
	else if(isThrowEx)
		throw PichiException("No file " + file + " founded...");
	return "";
}

double Helper::microtime(void )
{
	struct timeval tp;
	gettimeofday(&tp, NULL);
	double msec = (double) (tp.tv_usec / 1000000.0);
	double sec = tp.tv_sec;
	return (sec + msec);
}

std::string Helper::md5sum(const std::string& tomd5)
{
	unsigned char tmp_hash[MD5_DIGEST_LENGTH];
	MD5((const unsigned char*)tomd5.c_str(), tomd5.size(), tmp_hash);
	char buffer[10];
	std::string ret;
	for(int i=0; i <MD5_DIGEST_LENGTH; i++)
	{
		sprintf(buffer, "%02x", tmp_hash[i]);
		ret.append(buffer);
	}
	return ret;
}

bool Helper::decodeUnicodeEscapeSequence( const std::string& str, 
	                                     size_t current, 
	                                     size_t end, 
	                                     unsigned int &unicode )
{
	   if ( end - current < 4 )
	      return false;
	   unicode = 0;
	   for ( int index =0; index < 4; ++index )
	   {
	      char c = str[ current + index ];
	      unicode *= 16;
	      if ( c >= '0'  &&  c <= '9' )
	         unicode += c - '0';
	      else if ( c >= 'a'  &&  c <= 'f' )
	         unicode += c - 'a' + 10;
	      else if ( c >= 'A'  &&  c <= 'F' )
	         unicode += c - 'A' + 10;
	      else
	         return false;
	   }
	   return true;
}

/*
bool Helper::decodeUnicodeCodePoint( const std::string& str, 
	                                     size_t current, 
	                                     size_t end, 
	                                     unsigned int &unicode )
{
	   if ( !decodeUnicodeEscapeSequence( str, current, end, unicode ) )
	      return false;
	   if (unicode >= 0xD800 && unicode <= 0xDBFF)
	   {
	      // surrogate pairs
	      if (end - current < 6)
	         return false;
	      unsigned int surrogatePair;
	      if (*(current++) == '\\' && *(current++)== 'u')
	      {
	         if (decodeUnicodeEscapeSequence( token, current, end, surrogatePair ))
	         {
	            unicode = 0x10000 + ((unicode & 0x3FF) << 10) + (surrogatePair & 0x3FF);
	         } 
	         else
	            return false;
	      } 
	      else
	         return false;
	   }
	   return true;   
}
*/

std::string Helper::decodeUnicodeString( const std::string& str )
{
   std::string parsed;
   for(size_t i = 0; i < str.length(); ++i)
   {
      if ( str[i] == '\\' && i+1 < str.length() && str[i+1] == 'u')
      {
         unsigned int unicode;
         //if ( !decodeUnicodeCodePoint(str, i+2, str.length()-1, unicode ) )
	 if ( !decodeUnicodeEscapeSequence(str, i+2, str.length(), unicode ) )
             return "";
         parsed += codePointToUTF8(unicode);
	 i += 5;
      }
      else
      {
         parsed += str[i];
      }
   }
   return parsed;
}

std::string Helper::codePointToUTF8(unsigned int cp)
{
	  std::string result; 
	   // based on description from http://en.wikipedia.org/wiki/UTF-8
	
	   if (cp <= 0x7f) 
	   {
	      result.resize(1);
	      result[0] = static_cast<char>(cp);
	   } 
	   else if (cp <= 0x7FF) 
	   {
	      result.resize(2);
	      result[1] = static_cast<char>(0x80 | (0x3f & cp));
	      result[0] = static_cast<char>(0xC0 | (0x1f & (cp >> 6)));
	   } 
	   else if (cp <= 0xFFFF) 
	   {
	      result.resize(3);
	      result[2] = static_cast<char>(0x80 | (0x3f & cp));
	      result[1] = 0x80 | static_cast<char>((0x3f & (cp >> 6)));
	      result[0] = 0xE0 | static_cast<char>((0xf & (cp >> 12)));
	   }
	   else if (cp <= 0x10FFFF) 
	   {
	      result.resize(4);
	      result[3] = static_cast<char>(0x80 | (0x3f & cp));
	      result[2] = static_cast<char>(0x80 | (0x3f & (cp >> 6)));
	      result[1] = static_cast<char>(0x80 | (0x3f & (cp >> 12)));
	      result[0] = static_cast<char>(0xF0 | (0x7 & (cp >> 18)));
	   }
	
	   return result;
}

}
