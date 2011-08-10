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

#include "pichiconfig.h"

#include "config.h"
#include "helper.h"
#include "pichiexception.h"
#include "log.h"

namespace pichi
{
  
std::string PichiConfig::getCfgOption(const std::string& name)
{
	return config[name];
}

void PichiConfig::setCfgOption(const std::string& name, const std::string& value)
{
	config[name] = value;
}

PichiConfig::ConfigType PichiConfig::cfg2type(const std::string& type)
{
	if(type == "int")
		return INT;
	else if(type == "string")
		return STRING;
	else if(type == "array")
		return ARRAY;
	else if(type == "bool")
		return BOOL;
	
	return UNKNOWN;
}

void PichiConfig::loadXmlConfig(const std::string& file)
{
	xmlsimple::loadXmlConfig(file);
  
	TiXmlElement *xmllevel = 0;
	xmllevel = xmlfile->FirstChildElement("PichiConfig");
	
	xmllevel = xmllevel->FirstChildElement("version");
	version = atoi(xmllevel->GetText()); // get version
	xmllevel = xmlfile->FirstChildElement("PichiConfig");
	xmllevel = xmllevel->FirstChildElement("config");
	xmllevel = xmllevel->FirstChildElement("option");
	
	ConfigType tmpType;
	
	while(xmllevel != NULL)
	{
		if(xmllevel->GetText() != NULL)
		{
			config[ xmllevel->Attribute("Name") ] = static_cast<std::string>(xmllevel->GetText());
			tmpType = cfg2type(xmllevel->Attribute("type"));
			if(tmpType == UNKNOWN)
			{
				Log("Unknown config option type.", Log::WARNING);
				throw new PichiException("Unknown config option type. Break.");
			}
			configType[ xmllevel->Attribute("Name") ] = tmpType;
			if(tmpType == ARRAY)
				configSeparator[ xmllevel->Attribute("Name") ] = xmllevel->Attribute("separator");
		}
		xmllevel = xmllevel->NextSiblingElement("option");
	} 
	
	delete xmllevel;
}


PichiConfig::PichiConfig()
{
	version = 0;
	loadXmlConfig(Helper::getFullPath(PICHI_CONFIG_DIR) + "pichi.xml");
}

std::string PichiConfig::operator[](const std::string& str)
{
	return getCfgOption(str);
}

}
