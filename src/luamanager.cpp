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

namespace pichi
{
 
void LuaManager::loadLuaLibs(void )
{
	luaopen_base(L);
	luaopen_table(L);
	luaopen_string(L);
	luaopen_math(L);
}

int LuaManager::exec(const std::string& table, const std::string& method, int args, int ret)
{
	lua_getglobal(L, table.c_str()); // args + 1
	lua_pushstring(L, method.c_str()); // args + 2
	if(lua_istable(L, -2)) // есть такая таблица
	{
		lua_gettable(L, -2); // args + 2
		lua_remove(L, -2); // args + 1
		lua_insert(L, 1); // args + 1 (смещаем на 1 вниз)
		status = lua_pcall(L, args, ret, 0);
		reportError();
	}
	return status;
}

void LuaManager::registerFunction(const char* name, lua_CFunction func)
{
	lua_register(L, name, func);
}

void LuaManager::loadFile(const char* filename)
{
	loadFileStatus = luaL_loadfile(L, filename);
}

void LuaManager::reportError(void )
{
	if(status != 0) {
		Log(std::string("[LUA][ERROR]") + lua_tostring(L, -1), Log::WARNING);
		lua_pop(L, 1);
	}
}


}


#endif