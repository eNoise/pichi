/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) 2011  Alexey Kasyanchuk <deg@uruchie.org>

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
#include "pichicore.h"
#include "sqlite.h"

namespace pichi
{

int PichiManager::pichi2sql(lua_State* L)
{
	if(lua_gettop(L) != 1)
		return 0;
	lua_pushlightuserdata(L, ((PichiCore*)lua_touserdata(L, -1))->sql);
	return 1;
}
  
int PichiManager::sqlQuery(lua_State* L)
{
	if(lua_gettop(L) != 2)
		return 0;
	
	SQLite* sql = (SQLite*)lua_touserdata(L, -2);
	std::string query = lua_tostring(L, -1);
	
	lua_pushlightuserdata(L, sql->squery(query));
	return 1;
}

int PichiManager::sqlExec(lua_State* L)
{
	if(lua_gettop(L) != 2)
		return 0;
	
	SQLite* sql = (SQLite*)lua_touserdata(L, -2);
	std::string query = lua_tostring(L, -1);
	
	sql->exec(query);
	return 0;
}

int PichiManager::sqlNumRows(lua_State* L)
{
	if(lua_gettop(L) != 1)
		return 0;
	
	lua_pushnumber(L, ((SQLite::q*)lua_touserdata(L, -1))->numRows());
	
	return 1;
}

int PichiManager::sqlNumColumns(lua_State* L)
{
	if(lua_gettop(L) != 1)
		return 0;
	
	lua_pushnumber(L, ((SQLite::q*)lua_touserdata(L, -1))->numColumns());
	
	return 1;
}

int PichiManager::sqlFetchArray(lua_State* L)
{
	if(lua_gettop(L) != 1)
		return 0;
	
	SQLite::q* sqlQuery = (SQLite::q*)lua_touserdata(L, -1);
	SQLite::SQLRow data = SQLite::fetchArray(sqlQuery);
	
	lua_newtable(L);
	for(std::pair<std::string, std::string> row : data)
	{
		lua_pushstring(L, row.first.c_str());
		lua_pushstring(L, row.second.c_str());
		lua_settable(L,-3);
	}
	return 1;
}

int PichiManager::sqlFetchColumn(lua_State* L)
{
	if(lua_gettop(L) != 3)
		return 0;
	
	SQLite::q* sqlQuery = (SQLite::q*)lua_touserdata(L, -3);
	int num = lua_tonumber(L, -2);
	bool is_move = lua_toboolean(L, -1);
	std::string data = SQLite::fetchColumn(sqlQuery, num, is_move);
	
	lua_pushstring(L, data.c_str());
	return 1;
}

int PichiManager::sqlFinalize(lua_State* L)
{
	if(lua_gettop(L) != 1)
		return 0;
	
	delete (SQLite::q*)lua_touserdata(L, -1);
	
	return 0;
}

};

#endif