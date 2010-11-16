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

#include "pichiconfig.h"


std::string pichiconfig::getConfigOption(std::string name)
{
	return config[name];
}

std::string pichiconfig::setConfigOption(std::string name, std::string value)
{
	config[name] = value;
}

bool pichiconfig::loadXmlConfig(std::string file)
{
	if(!xmlsimple::loadXmlConfig(file))
		throw PichiException("Config file read error. Check this.");
  
	TiXmlElement *xmllevel = 0;
	xmllevel = xmlfile->FirstChildElement("PichiConfig");
	
	xmllevel = xmllevel->FirstChildElement("version");
	version = atoi(xmllevel->GetText()); // get version
	xmllevel = xmlfile->FirstChildElement("PichiConfig");
	xmllevel = xmllevel->FirstChildElement("config");
	xmllevel = xmllevel->FirstChildElement("option");
	
	while(xmllevel != NULL)
	{
		if(xmllevel->GetText() != NULL)
			config[ xmllevel->Attribute("Name") ] = static_cast<std::string>(xmllevel->GetText());
		xmllevel = xmllevel->NextSiblingElement("option");
	} 
	
	delete xmllevel;
	return true;
}


pichiconfig::pichiconfig()
{
	version = 0;
	loadXmlConfig("pichi.xml");
}

std::string pichiconfig::operator[](std::string str)
{
	return config[str];
}

