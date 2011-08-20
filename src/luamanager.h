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

class PichiCore;
  
class LuaManager
{
public:
	LuaManager() : L(lua_open()) {
		loadLuaLibs();
	};
	~LuaManager() {
		lua_close(L);
	};
	
	int callEvent(const std::string& table, const std::string& method, int args = 0, int ret = LUA_MULTRET);
	void registerFunction(const char *name, lua_CFunction func);
	void loadFile(const char* filename);
	void loadFile(const std::string& filename) { loadFile(filename.c_str()); };
	void reload();
	
	int getStatus(){ return status; };
	int getFileStatus(){ return loadFileStatus; };
	
	void luaPush(int i);
	void luaPush(bool b);
	void luaPush(const std::string& str);
	void luaPush(const char* str);
	void luaPush(void* func);
	template< typename T > void luaPush(T* p){ luaPush((void*)p); };
	std::string luaPopString(void);
	double luaPopNumber(void);
	template< typename T> T* luaPopData(void) {
		T* back;
		if(lua_isuserdata(L, -1))
			back = lua_touserdata(L, -1);
		lua_pop(L, 1);
		return back;
	};
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
