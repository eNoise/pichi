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


#ifndef TESTS_H
#define TESTS_H

#ifdef WITH_TESTS

#include <map>
#include <string>

namespace pichi {

class Tests
{
public:
    typedef bool (*TestFunction)(const std::string&);
    static std::map<std::string, TestFunction> testMap;
    static void init();
    static int runTest(const std::string& test, const std::string& arg);
    
    // tests
    static bool test_regexp_split(const std::string& arg);
    static bool test_helper_explode(const std::string& arg);
    static bool test_helper_implode(const std::string& arg);
    static bool test_helper_splitbysize(const std::string& arg);
    static bool test_helper_file_exist(const std::string& arg);
    static bool test_helper_microtime(const std::string& arg);
    static bool test_helper_createdirectory(const std::string& arg);
    static bool test_helper_removedirectory(const std::string& arg);
    static bool test_helper_md5sum(const std::string& arg);
    static bool test_sqlite_open(const std::string& arg);
    static bool test_sqlite_query(const std::string& arg);
    static bool test_sqlite_query_async(const std::string& arg);
    static bool test_sqlite_escape_string(const std::string& arg);
#ifdef WITH_LUA
    static bool test_lua_fileload(const std::string& arg);
    static bool test_lua_fileload_wrongsyntax(const std::string& arg);
    static bool test_lua_handler_pushpop(const std::string& arg);
    static bool test_lua_pichilua_core_listener(const std::string& pichilua);
    static bool test_lua_functions_userdata(const std::string& pichilua);
#endif
};

};

#endif

#endif // TESTS_H
