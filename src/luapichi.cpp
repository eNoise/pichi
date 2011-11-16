/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) 2011  <copyright holder> <email>

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

#ifdef WITH_LUA

#include "luapichi.h"

#include <algorithm>
#include <boost/property_tree/json_parser.hpp>

#include "helper.h"
#include "log.h"
#include "pichiexception.h"
#include "pichicore.h"
#include "pichicurl.h"

namespace pichi
{

std::string LuaPichi::LuaModuleInfo::toString()
{
	return name + " " + version + " " + "(" + description + ")" + " [" + author + " <" + author_contact + "> " + "]";
}
  
LuaPichi::LuaPichi()
{
	loadLuaFiles();
	
	luaMap.push_back({"freeMessageEnv", PichiManager::freeMessageEnv, true});
	luaMap.push_back({"SendAnswer", PichiManager::sendAnswer, true});
	luaMap.push_back({"GetLastJID", PichiManager::getLastJID, true});
	luaMap.push_back({"RegisterModule", PichiManager::registerModule, true});
	luaMap.push_back({"md5sum", PichiManager::md5sum, true});
	luaMap.push_back({"ReadUrl", PichiManager::readUrl, true});
	luaMap.push_back({"Utf8Decode", PichiManager::utf8Decode, true});
	luaMap.push_back({"JsonDecode", PichiManager::jsonDecode, true});
	luaMap.push_back({"SetJIDinfo", PichiManager::setJIDinfo, true});
	luaMap.push_back({"GetJIDinfo", PichiManager::getJIDinfo, true});
	luaMap.push_back({"DelJIDinfo", PichiManager::delJIDinfo, true});
	luaMap.push_back({"PichiCoreToSQL", PichiManager::pichi2sql, true});
	luaMap.push_back({"SQLQuery", PichiManager::sqlQuery, true});
	luaMap.push_back({"SQLExec", PichiManager::sqlExec, true});
	luaMap.push_back({"SQLFinalize", PichiManager::sqlFinalize, true});
	luaMap.push_back({"SQLNumRows", PichiManager::sqlNumRows, true});
	luaMap.push_back({"SQLNumColumns", PichiManager::sqlNumColumns, true});
	luaMap.push_back({"SQLFetchArray", PichiManager::sqlFetchArray, true});
	luaMap.push_back({"SQLFetchColumn", PichiManager::sqlFetchColumn, true});
	
	registerLuaMap(); // регитрируем map
	
	// init call
	luaPush(this);
	callEvent("PichiLua", "init", 1);
}

void LuaPichi::reload()
{
	LuaMap::reload();
	//load files
	std::for_each(loadedLuaList.begin(), loadedLuaList.end(), [this](LuaFile& file){
		if(file.enabled || file.name == "pichi.lua") // Либо это pichi.lua (загружается всегда) либо включен
		{
			this->loadFile(file.path.c_str());
			if(this->loadFileStatus != 0)
				throw PichiException("Load lua file " + file.name + " problem...");
		}
	});
	
	// DONE!
	
	// someclear
	luaModules.clear();
	
	//reload
	luaPush(this);
	callEvent("PichiLua", "reload", 1);
	// init call
	luaPush(this);
	callEvent("PichiLua", "init", 1);
}

void LuaPichi::disable(const std::string& file)
{
	std::list<LuaFile>::iterator it = std::find_if(loadedLuaList.begin(), loadedLuaList.end(), [&file](LuaFile& testFile){
		return testFile.name == file;
	});
	if(it != loadedLuaList.end()) {
		if(it->name != "pichi.lua")
			it->enabled = false;
		reload();
	}
}

void LuaPichi::enable(const std::string& file)
{
	std::list<LuaFile>::iterator it = std::find_if(loadedLuaList.begin(), loadedLuaList.end(), [&file](LuaFile& testFile){
		return testFile.name == file;
	});
	if(it != loadedLuaList.end()) {
		if(it->name != "pichi.lua")
			it->enabled = true;
		reload();
	}
}

void LuaPichi::loadLuaFiles(void )
{
	std::string loadPath = Helper::getShareFile("lua/");
	std::vector< std::string > luaFiles = Helper::getDirFiles(loadPath);
	if(luaFiles.size() == 0)
	{
		Log("No lua plugins :(", Log::DEBUG);
		return;
	}
	
	std::string filePath = loadPath + "pichi.lua";
	this->loadFile(filePath.c_str());
	if(this->loadFileStatus != 0)
		throw PichiException("Load lua file pichi.lua problem...");
	else
		this->loadedLuaList.push_back({"pichi.lua", filePath, true});
	
	std::for_each(luaFiles.begin(), luaFiles.end(), [this,&filePath,&loadPath](const std::string& fileName){
		if(fileName == "pichi.lua")
			return; // уже загружен
		if(fileName.substr(fileName.size() - 4) != ".lua")
			return;
		filePath = loadPath + fileName;
		this->loadFile(filePath.c_str());
		if(this->loadFileStatus != 0)
			throw PichiException("Load lua file " + fileName + " problem...");
		else
			this->loadedLuaList.push_back({fileName, filePath, true});
	});
}

std::list<std::string> LuaPichi::getLuaFunctionsList(void)
{
     std::list<std::string> registerList;
     std::for_each(luaMap.begin(), luaMap.end(), [&registerList](LuaMap::LuaFunction& func){
		registerList.push_back(func.name);
     });
     return registerList;
}

int LuaPichi::callEvent(const std::string& table, const std::string& method, int args, int ret, bool async)
{
	if(std::find(luaHandlersList[table].begin(), luaHandlersList[table].end(), method) == luaHandlersList[table].end())
		luaHandlersList[table].push_back(method);
	return LuaManager::callEvent(table, method, args, ret, async);
}

void LuaPichi::appendModule(const pichi::LuaPichi::LuaModuleInfo& info)
{
	luaModules.push_back(info);
}


int PichiManager::freeMessageEnv(lua_State* L)
{
	if(lua_gettop(L) != 1)
		return 0;
	delete (PichiMessage*)lua_touserdata(L, -1);
	return 0;
}


int PichiManager::sendAnswer(lua_State* L)
{
	PichiCore* pichi;
	std::string toSend;
	PichiMessage* msgarg;
	switch(lua_gettop(L)) {
		case 2:
			pichi = (PichiCore*)lua_touserdata(L, -2);
			toSend = lua_tostring(L, -1);
			pichi->sendAnswer(toSend);
			break;
		case 3:
			pichi = (PichiCore*)lua_touserdata(L, -3);
			toSend = lua_tostring(L, -2);
			msgarg = (PichiMessage*)lua_touserdata(L, -1);
			pichi->sendAnswer(toSend, *msgarg);
			break;
	}
	return 0;
}

int PichiManager::getLastJID(lua_State* L)
{
	if(lua_gettop(L) != 1)
		return 0;
	PichiCore* pichi = (PichiCore*)lua_touserdata(L, -1);
	lua_pushstring(L, pichi->getJIDlast().c_str());
	return 1;
}

int PichiManager::registerModule(lua_State* L)
{
	LuaPichi* pichiLua = (LuaPichi*)lua_touserdata(L, -6);
	LuaPichi::LuaModuleInfo module = {
		lua_tostring(L, -5), //name;
		lua_tostring(L, -4), //description;
		lua_tostring(L, -3), //version;
		lua_tostring(L, -2), //author;
		lua_tostring(L, -1), //author_contact;
	};
	pichiLua->appendModule(module);
	return 0;
}

int PichiManager::delJIDinfo(lua_State* L)
{
	if(lua_gettop(L) < 2 || lua_gettop(L) > 4)
		return 0;
  
	PichiCore* pichi;
	std::string jid;
	std::string name;
	std::string group;
	
	switch(lua_gettop(L))
	{
		case 2:
			pichi = (PichiCore*)lua_touserdata(L, -2);
			jid = lua_tostring(L, -1);
			break;
		case 3:
			pichi = (PichiCore*)lua_touserdata(L, -3);
			jid = lua_tostring(L, -2);
			name = lua_tostring(L, -1);
			break;
		case 4:
			pichi = (PichiCore*)lua_touserdata(L, -4);
			jid = lua_tostring(L, -3);
			name = lua_tostring(L, -2);
			group = lua_tostring(L, -1);
			break;
	}
	
	pichi->delJIDinfo(jid, name, group);
	return 0;
}

int PichiManager::getJIDinfo(lua_State* L)
{
	if(lua_gettop(L) < 2 || lua_gettop(L) > 4)
		return 0;
	PichiCore* pichi;
	std::string jid;
	std::string name;
	std::string group;
	switch(lua_gettop(L))
	{
		case 2:
			pichi = (PichiCore*)lua_touserdata(L, -2);
			jid = lua_tostring(L, -1);
			break;
		case 3:
			pichi = (PichiCore*)lua_touserdata(L, -3);
			jid = lua_tostring(L, -2);
			name = lua_tostring(L, -1);
			break;
		case 4:
			pichi = (PichiCore*)lua_touserdata(L, -4);
			jid = lua_tostring(L, -3);
			name = lua_tostring(L, -2);
			group = lua_tostring(L, -1);
			break;
	}
	std::map<std::string, std::string> data = pichi->getJIDinfo(jid, name, group);
	lua_newtable(L);
	for(std::pair<std::string, std::string> row : data)
	{
		lua_pushstring(L, row.first.c_str());
		lua_pushstring(L, row.second.c_str());
		lua_settable(L,-3);
	}
	return 1;
}

int PichiManager::setJIDinfo(lua_State* L)
{
	if(lua_gettop(L) < 2 || lua_gettop(L) > 5)
		return 0;
	PichiCore* pichi;
	std::string jid;
	std::string name;
	std::string value;
	std::string group;
	switch(lua_gettop(L))
	{
		case 2:
			pichi = (PichiCore*)lua_touserdata(L, -2);
			jid = lua_tostring(L, -1);
			break;
		case 3:
			pichi = (PichiCore*)lua_touserdata(L, -3);
			jid = lua_tostring(L, -2);
			name = lua_tostring(L, -1);
			break;
		case 4:
			pichi = (PichiCore*)lua_touserdata(L, -4);
			jid = lua_tostring(L, -3);
			name = lua_tostring(L, -2);
			value = lua_tostring(L, -1);
			break;
		case 5:
			pichi = (PichiCore*)lua_touserdata(L, -5);
			jid = lua_tostring(L, -4);
			name = lua_tostring(L, -3);
			value = lua_tostring(L, -2);
			group = lua_tostring(L, -1);
			break;
	}
	pichi->setJIDinfo(jid, name, value, group);
	return 0;
}

int PichiManager::md5sum(lua_State* L)
{
	if(lua_gettop(L) != 1)
		return 0;
	lua_pushstring(L, Helper::md5sum(lua_tostring(L, -1)).c_str());
	return 1;
}

int PichiManager::readUrl(lua_State* L)
{
	if(lua_gettop(L) < 1 || lua_gettop(L) > 2)
		return 0;
	
	PichiCurl* curl = new PichiCurl();
	std::string url;
	std::map<std::string, std::string> table;
	
	switch(lua_gettop(L))
	{
		case 1:
			url = lua_tostring(L, -1);
			break;
		case 2:
			url = lua_tostring(L, -2);
			if(lua_istable(L, -1))
			{
				lua_pushnil(L);
				while(lua_next(L, -2) != 0)
				{
					table[lua_tostring(L, -2)] = lua_tostring(L, -1);
					lua_pop(L, 1);
				}
			}
			break;
	}
	
	if(table.size() > 0)
		curl->setPostArgs(table);
	lua_pushstring(L, curl->readurl(url).c_str());
	
	delete curl;
	return 1;
}

int PichiManager::utf8Decode(lua_State* L)
{
	if(lua_gettop(L) != 1)
		return 0;
	lua_pushstring(L, Helper::decodeUnicodeString(lua_tostring(L, -1)).c_str());
	return 1;
}

int PichiManager::jsonDecode(lua_State* L)
{
	if(lua_gettop(L) != 1)
		return 0;
	
	boost::property_tree::ptree ptree;
	std::stringstream stream(lua_tostring(L, -1));
	boost::property_tree::json_parser::read_json(stream, ptree);
	
	lua_newtable(L);
	jsonDecodeParse(L, ptree);
	
	return 1;
}

void PichiManager::jsonDecodeParse(lua_State* L, boost::property_tree::ptree& ptree)
{
	int i = 1;
	for(boost::property_tree::ptree::value_type &v : ptree)
	{
		if(v.first.empty())
			lua_pushnumber(L, i++);
		else
			lua_pushstring(L, v.first.c_str());
		if(v.second.size() > 0) {
			lua_newtable(L);
			jsonDecodeParse(L, v.second);
		} else {
			lua_pushstring(L, v.second.data().c_str());
		}
		lua_settable(L,-3);
	}
}

};

#endif