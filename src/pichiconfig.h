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

#ifndef PICHICONFIG_H
#define PICHICONFIG_H

#include "xmlsimple.h"

#include <map>
#include <string>

namespace pichi
{

class PichiConfig : public xmlsimple
{
  public:
	 enum ConfigType {
		INT,
		STRING,
		ARRAY,
		BOOL,
		UNKNOWN
	 };
    
	 PichiConfig();
	 void loadXmlConfig(const std::string &);
	 void setCfgOption(const std::string &, const std::string &);
	 std::string getCfgOption(const std::string &);
	 std::string operator[] (const std::string &);
  private:
	 ConfigType cfg2type(const std::string& type);
  protected:
	 std::map<std::string, std::string> config;
	 std::map<std::string, ConfigType> configType;
	 std::map<std::string, std::string> configSeparator;
	 int version;
};

}

#endif // PICHICONFIG_H
