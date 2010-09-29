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

#ifndef LANGUAGES_H
#define LANGUAGES_H

#include <map>
#include <string>
#include "xmlsimple.h"
#include "system.h"
#include <vector>
#include <stdarg.h>
#include <boost/algorithm/string/replace.hpp>
//#include <boost/regex.hpp>
#include <stdio.h>

class languages : public xmlsimple
{  
  protected:
	 std::map<std::string, std::string> language;
	 std::string choise;
  public:
	 languages();
	 languages(std::string lan);
	 bool loadXmlConfig(std::string);
	 std::string operator[] (std::string);
         std::string operator() (std::string first, ...);
};

#endif // LANGUAGES_H
