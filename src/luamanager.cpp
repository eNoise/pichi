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

#include "luamanager.h"
#include "log.h"
#include "string.h"

namespace pichi
{

int LuaManager::status;
  
void LuaManager::loadLuaLibs(void )
{
	Log("[LUA][INIT] Initialization", Log::DEBUG);
	/*
	luaopen_io(L);
	
	luaopen_base(L);
	luaopen_table(L);
	luaopen_string(L);
	luaopen_math(L);
	*/
	luaL_openlibs(L);
}

void* LuaManager::call_thread(void* context)
{
	LuaManager* th = (LuaManager*)context;
	Log(std::string("[LUA][CALL]") + th->callArgs.table + ":" + th->callArgs.method, Log::VERBOSE);
	lua_getglobal(th->L, th->callArgs.table.c_str()); // args + 1
	lua_pushstring(th->L, th->callArgs.method.c_str()); // args + 2
	if(lua_istable(th->L, -2)) // есть такая таблица
	{
		lua_gettable(th->L, -2); // args + 2
		lua_remove(th->L, -2); // args + 1
		lua_insert(th->L, 1); // args + 1 (смещаем на 1 вниз)
		LuaManager::status = lua_pcall(th->L, th->callArgs.args, th->callArgs.ret, 0);
		reportError(th->L);
	}
	else 
	{
		lua_settop(th->L, 0);
	}
	
	pthread_exit(context);
	return context;
}

int LuaManager::callEvent(const std::string& table, const std::string& method, int args, int ret, bool async)
{
	callArgs = {table, method, args, ret, async};
	if(pthread_create(&callThread[table + method], NULL, &LuaManager::call_thread, (void*)this) != 0)
		return 1;
	if(!async)
		if(pthread_join(callThread[table + method], NULL) != 0)
			return 1;
	return 0;
}

void LuaManager::registerFunction(const char* name, lua_CFunction func)
{
	Log(std::string("[LUA][REGISTER]") + name, Log::DEBUG);
	lua_register(L, name, func);
}

void LuaManager::loadFile(const char* filename)
{
	Log(std::string("[LUA][LOAD]") + filename, Log::DEBUG);
	//loadFileStatus = luaL_loadfile(L, filename);
	loadFileStatus = luaL_dofile(L, filename);
}

void LuaManager::reload()
{
	// Full reload
	lua_close(L);
	L = lua_open();
	loadLuaLibs();
}

void LuaManager::reportError(lua_State *L)
{
	if(LuaManager::status != 0) {
		Log(std::string("[LUA][ERROR]") + lua_tostring(L, -1), Log::WARNING);
		lua_pop(L, 1);
	}
}


void LuaManager::luaPush(int i)
{
	lua_pushnumber(L, i);
}

void LuaManager::luaPush(bool b)
{
	lua_pushboolean(L, b);
}

void LuaManager::luaPush(const std::string& str)
{
	lua_pushlstring(L, str.data(), str.size());
}

void LuaManager::luaPush(const char* str)
{
	lua_pushlstring(L, str, strlen(str));
}

void LuaManager::luaPush(void* func)
{
	lua_pushlightuserdata(L, func);
}

void LuaManager::luaPush(const std::map< std::string, std::string >& map)
{
	lua_newtable(L);
	for(std::pair<std::string, std::string> row : map)
	{
		lua_pushstring(L, row.first.c_str());
		lua_pushstring(L, row.second.c_str());
		lua_settable(L,-3);
	}
}

double LuaManager::luaPopNumber(void )
{
	double back;
	if(lua_isnumber(L, -1))
		back = lua_tonumber(L, -1);
	lua_pop(L, 1);
	return back;
}

std::string LuaManager::luaPopString(void )
{
	std::string back;
	if(lua_isstring(L, -1))
		back = lua_tostring(L, -1);
	lua_pop(L, 1);
	return back;
}

std::map< std::string, std::string > LuaManager::luaPopTable(void )
{
	std::map< std::string, std::string > table;
	if(lua_istable(L, -1))
	{
		lua_pushnil(L);
		while(lua_next(L, -2) != 0)
		{
			table[lua_tostring(L, -2)] = lua_tostring(L, -1);
			lua_pop(L, 1);
		}
		lua_pop(L, 1); // remove key
		lua_pop(L, 1); // remove table
	}
	return table;
}

}


#endif