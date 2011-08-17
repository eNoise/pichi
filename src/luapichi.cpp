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

#include "helper.h"
#include "log.h"
#include "pichiexception.h"
#include "pichicore.h"

namespace pichi
{

std::string LuaPichi::LuaModuleInfo::toString()
{
	return name + " " + version + " " + "(" + description + ")" + " [" + author + " <" + author_contact + "> " + "]";
}
  
LuaPichi::LuaPichi()
{
	loadLuaFiles();
	
	luaMap.push_back({"SendAnswer", PichiManager::sendAnswer, true});
	luaMap.push_back({"RegisterModule", PichiManager::registerModule, true});
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
		if(file.enabled)
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
		it->enabled = true;
		reload();
	}
}

void LuaPichi::loadLuaFiles(void )
{
	std::vector< std::string > luaFiles = Helper::getDirFiles(PICHI_INSTALLED_DIR + std::string("lua/"));
	if(luaFiles.size() == 0)
	{
		Log("No lua plugins :(", Log::DEBUG);
		return;
	}
	
	std::for_each(luaFiles.begin(), luaFiles.end(), [this](const std::string& fileName){
		std::string filePath = PICHI_INSTALLED_DIR + std::string("lua/") + fileName;
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

int LuaPichi::callEvent(const std::string& table, const std::string& method, int args, int ret)
{
	if(std::find(luaHandlersList[table].begin(), luaHandlersList[table].end(), method) == luaHandlersList[table].end())
		luaHandlersList[table].push_back(method);
	return LuaManager::callEvent(table, method, args, ret);
}

void LuaPichi::appendModule(const pichi::LuaPichi::LuaModuleInfo& info)
{
	luaModules.push_back(info);
}


int PichiManager::sendAnswer(lua_State* L)
{
	PichiCore* pichi = (PichiCore*)lua_touserdata(L, -2);
	std::string toSend = lua_tostring(L, -1);
	pichi->sendAnswer(toSend);
	return 0;
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


};

#endif