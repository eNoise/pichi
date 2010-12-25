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

#include "languages.h"

namespace pichi
{

const std::string languages::findLangDir(void )
{
#ifndef WIN32
	if(system::fileExists(system::getFullPath(PICHI_CONFIG_DIR) + "languages/"))
		return (system::getFullPath(PICHI_CONFIG_DIR) + "languages/");
	else if(system::fileExists("/usr/share/pichi/languages/"))
		return "/usr/share/pichi/languages/";
	else if(RUN_DIR.substr(RUN_DIR.size() - 4) == "/bin" && system::fileExists(RUN_DIR.substr(0, RUN_DIR.size() - 3) + "share/pichi/languages/"))
		return (RUN_DIR.substr(0, RUN_DIR.size() - 3) + "share/pichi/languages/");
	else
		throw PichiException("No language directory's founded...");
#else
	return system::getFullPath(PICHI_CONFIG_DIR) + "languages/";
#endif
	return "";
}  
  
languages::languages()
{
	xmlfile = 0;
	default_lang = "en";
	choise = default_lang; // default language
	std::string dir = findLangDir();
	loadLanguage(dir + choise + ".xml");
}

languages::languages(const std::string& lan): choise(lan)
{
	xmlfile = 0;
	default_lang = "en";
	std::string dir = findLangDir();
	loadLanguage(dir + default_lang + ".xml");
	loadLanguage(dir + choise + ".xml");	
}

void languages::loadLanguage(const std::string& file, bool reload)
{
	if(xmlfile)
		delete xmlfile;
  
	xmlsimple::loadXmlConfig(file);

	if(reload)
		language.clear();
	
	TiXmlElement *xmllevel = 0;
	xmllevel = xmlfile->FirstChildElement("PichiLanguage");
	xmllevel = xmllevel->FirstChildElement("language");

	xmllevel = xmllevel->FirstChildElement("option");
	
	while(xmllevel != NULL)
	{
		if(xmllevel->GetText() != NULL)
			language[ xmllevel->Attribute("name") ] = xmllevel->GetText();
		xmllevel = xmllevel->NextSiblingElement("option");
	} 
	
	delete xmllevel;
}

std::string languages::operator[](const std::string& name)
{
	return language[name];
}

std::string languages::operator()(const std::string& first, ...)
{
	va_list marker;
	va_start(marker, first);
	//char* param = va_arg( marker, char*);
	//std::vector<std::string> list_param;
	//int count = 0;
	//while( count < 3 )
	//{
	//	count++;
	//	list_param.push_back(static_cast<std::string>(param));
	//	param = va_arg( marker, char*);
	//}
	

	std::string text = language[first];
	//boost::replace_all(text, "%", "%%");
	//boost::regex reg("#\\{([0-9]+)\\}#sU", boost::match_perl|boost::format_perl);
	//text = boost::regex_replace(text, reg, "%\\\\1$s");
	// fuck!!! regex don't work as I needed, so we use not protected method =((
	std::vector<std::string> past = system::explode("\\{", text);
	size_t h;
	for(std::vector<std::string>::iterator it = past.begin(); it != past.end(); it++)
	{
		//boost::replace_all((*it), "}", "");
		h = it->find("}");
		if(h != std::string::npos)
			(*it) = it->substr(h+1);
	}
	//text = system::implode("%", past);
	text = system::implode("%s", past); // redo!
	
	char inbuf[65536], outbuf[65536];
	strcpy(inbuf, text.c_str());
	
	vsprintf(outbuf, inbuf, marker);
	text = outbuf;
	
	va_end(marker);
	return text;
}

}
