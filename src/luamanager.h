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


#ifndef LUAMANAGER_H
#define LUAMANAGER_H

#ifdef WITH_LUA

extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

#include <string>

namespace pichi
{

class LuaManager
{
public:
	LuaManager() : L(lua_open()) {
		loadLuaLibs();
	};
	~LuaManager() {
		lua_close(L);
	};
	
	int exec(const std::string& table, const std::string& method, int args = 0, int ret = LUA_MULTRET);
	void registerFunction(const char *name, lua_CFunction func);
	void loadFile(const char* filename);
protected:
	lua_State *L;
	int status;
	int loadFileStatus;
private:
	void loadLuaLibs(void);
	void reportError(void);
};

};

#endif //WITH_LUA

#endif // LUAMANAGER_H
