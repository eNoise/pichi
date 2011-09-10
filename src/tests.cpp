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

#ifdef WITH_TESTS

#include "tests.h"

#include <boost/algorithm/string/regex.hpp>
#include "helper.h"
#include "pichicore.h"
#include "pichidbpatcher.h"
#include "languages.h"
#include "config.h"
#include "sqlite.h"
#ifdef WITH_LUA
#include "luapichi.h"
#endif
#include <fstream>

namespace pichi {

std::map<std::string, Tests::TestFunction> Tests::testMap = std::map<std::string, Tests::TestFunction>();
  
void Tests::init()
{
	Tests::testMap["regexp_split"] = Tests::test_regexp_split;
	Tests::testMap["helper_explode"] = Tests::test_helper_explode;
	Tests::testMap["helper_implode"] = Tests::test_helper_implode;
	Tests::testMap["helper_splitbysize"] = Tests::test_helper_splitbysize;
	Tests::testMap["helper_file_exist"] = Tests::test_helper_file_exist;
	Tests::testMap["helper_microtime"] = Tests::test_helper_microtime;
	Tests::testMap["helper_createdirectory"] = Tests::test_helper_createdirectory;
	Tests::testMap["helper_removedirectory"] = Tests::test_helper_removedirectory;
	Tests::testMap["helper_md5sum"] = Tests::test_helper_md5sum;
	Tests::testMap["sqlite_open"] = Tests::test_sqlite_open;
	Tests::testMap["sqlite_query"] = Tests::test_sqlite_query;
	Tests::testMap["sqlite_query_async"] = Tests::test_sqlite_query_async;
	Tests::testMap["sqlite_escape_string"] = Tests::test_sqlite_escape_string;
#ifdef WITH_LUA
	Tests::testMap["lua_fileload"] = Tests::test_lua_fileload;
	Tests::testMap["lua_fileload_wrongsyntax"] = Tests::test_lua_fileload_wrongsyntax;
	Tests::testMap["lua_handler_pushpop"] = Tests::test_lua_handler_pushpop;
	Tests::testMap["lua_pichilua_core_listener"] = Tests::test_lua_pichilua_core_listener;
	Tests::testMap["lua_functions_userdata"] = Tests::test_lua_functions_userdata;
	Tests::testMap["lua_functions_jsondecode"] = Tests::test_lua_functions_jsondecode;
#endif
}


int Tests::runTest(const std::string& test, const std::string& arg)
{
	if(std::find_if(Tests::testMap.begin(), Tests::testMap.end(), [&test](std::pair<const std::string, Tests::TestFunction>& p){ return p.first == test; }) != Tests::testMap.end())
		return ((*Tests::testMap[test])(arg)) ? 0 : 1;
	else
		return 1; //error
}

// -------------------------------------------------------------------------
  
bool Tests::test_regexp_split(const std::string& arg)
{
	std::vector< std::string > ret;
	boost::split_regex( ret, "one two three", boost::regex( " " ) );
	return ret == std::vector< std::string >({"one", "two", "three"});
}

bool Tests::test_helper_explode(const std::string& arg)
{
	// простой тест
	bool test1 = (Helper::explode(" ", "one two three") == std::vector< std::string >({"one", "two", "three"}));
	// с другим разделителем
	bool test2 = (Helper::explode("::", "one::two::three") == std::vector< std::string >({"one", "two", "three"}));
	// проверим по бокам
	bool test3 = (Helper::explode("::", "::one::two::three::") == std::vector< std::string >({"", "one", "two", "three",""}));
	// проверим на нули
	bool test4 = true;
	//if(!arg.empty())
	//	test4 = 
	
	
	return test1 && test2 && test3 && test4;
}

bool Tests::test_helper_implode(const std::string& arg)
{
	// простой тест
	bool test1 = (Helper::implode(" ", {"one", "two", "three"}) == "one two three");
	// с пустыми по бокам
	bool test2 = (Helper::implode(" ", {"", "one", "two", "three", std::string()}) == " one two three ");
	
	return test1 && test2;
}

bool Tests::test_helper_splitbysize(const std::string& arg)
{
	// простой тест
	bool test1 = (Helper::splitBySize("1234",2) == std::vector<std::string>({"12", "34"}));
	bool test2 = (Helper::splitBySize("1234",0) == std::vector<std::string>({"1234"}));
	
	return test1 && test2;
}

bool Tests::test_helper_file_exist(const std::string& arg)
{
	return Helper::fileExists(arg);
}

bool Tests::test_helper_microtime(const std::string& arg)
{
	return Helper::microtime() > 1000000;
}

bool Tests::test_helper_createdirectory(const std::string& arg)
{
	bool test;
	if(Helper::fileExists(Helper::getFullPath(PICHI_CONFIG_DIR))) {
		test = Helper::createDirectory(Helper::getFullPath(PICHI_CONFIG_DIR) + "tests");
		Helper::removeDirectory(Helper::getFullPath(PICHI_CONFIG_DIR) + "tests");
	} else {
		test = Helper::createDirectory(Helper::getFullPath(PICHI_CONFIG_DIR));
	}
	return test;
}

bool Tests::test_helper_removedirectory(const std::string& arg)
{
	bool test;
	if(Helper::fileExists(Helper::getFullPath(PICHI_CONFIG_DIR))) {
		if(Helper::fileExists(Helper::getFullPath(PICHI_CONFIG_DIR) + "tests")) {
			test = Helper::removeDirectory(Helper::getFullPath(PICHI_CONFIG_DIR) + "tests");
		} else {
			Helper::createDirectory(Helper::getFullPath(PICHI_CONFIG_DIR) + "tests");
			test = Helper::removeDirectory(Helper::getFullPath(PICHI_CONFIG_DIR) + "tests");
		}
	} else {
		Helper::createDirectory(Helper::getFullPath(PICHI_CONFIG_DIR));
		Helper::createDirectory(Helper::getFullPath(PICHI_CONFIG_DIR) + "tests");
		test = Helper::removeDirectory(Helper::getFullPath(PICHI_CONFIG_DIR) + "tests");
	}
	return test;
}

bool Tests::test_helper_md5sum(const std::string& arg)
{
	bool test1 = Helper::md5sum("") == "d41d8cd98f00b204e9800998ecf8427e";
	bool test2 = Helper::md5sum("md5") == "1bc29b36f623ba82aaf6724fd3b16718";
	bool test3 = Helper::md5sum("рус") == "d31757c0969e08011ecd9cbc0ecf7ae7";
	return test1 && test2 && test3;
}

bool Tests::test_sqlite_open(const std::string& arg)
{
	std::string path = Helper::getFullPath(PICHI_CONFIG_DIR) + "test.db";
	SQLite* sql = new SQLite(path);
	delete sql;
	bool test = Helper::fileExists(path);
	Helper::removeFile(path);
	return test;
}

bool Tests::test_sqlite_query(const std::string& arg)
{
	std::string path = Helper::getFullPath(PICHI_CONFIG_DIR) + "test.db";
	SQLite* sql = new SQLite(path);
	
	sql->exec("CREATE TABLE `testing` (`test` TEXT);");
	sql->exec("INSERT INTO `testing` VALUES ('testx');");
	sql->query("SELECT * FROM `testing`;");
	std::string xxx = sql->fetchColumn(0);
	
	bool test = (xxx == "testx");
	
	delete sql;
	Helper::removeFile(path);
	return test;
}

bool Tests::test_sqlite_query_async(const std::string& arg)
{
	std::string path = Helper::getFullPath(PICHI_CONFIG_DIR) + "test.db";
	SQLite* sql = new SQLite(path);
	
	sql->exec("CREATE TABLE `testing` (`test` TEXT, `somewhere` TEXT);");
	sql->exec("INSERT INTO `testing` VALUES ('testx', '1');");
	sql->exec("INSERT INTO `testing` VALUES ('testy', '2');");
	sql->exec("INSERT INTO `testing` VALUES ('testz', '3');");
	
	sql->query("SELECT * FROM `testing` WHERE somewhere = '1';");
	SQLite::q* qu1 = sql->squery("SELECT * FROM `testing` WHERE somewhere = '2';");
	SQLite::q* qu2 = sql->squery("SELECT * FROM `testing` WHERE somewhere = '3';");
	
	std::map<std::string, std::string> data;
	std::pair<std::string, std::string> map1, map2;
	
	if(sql->numRows() == 1) {
		while(!(data = sql->fetchArray()).empty()){
		      map1.first = data["test"];
		      map1.second = data["somewhere"];
		}
	}
	std::string y1 = sql->fetchColumn(qu1, 0);
	std::string y2 = sql->fetchColumn(qu1, 1, true);
	if(sql->numRows(qu2) == 1) {
		while(!(data = sql->fetchArray(qu2)).empty()){
		      map2.first = data["test"];
		      map2.second = data["somewhere"];
		}
	}
	bool test = (map1.first == "testx" && map1.second == "1") && (y1 == "testy" && y2 == "2") && (map2.first == "testz" && map2.second == "3");
	
	delete sql, qu1, qu2;
	Helper::removeFile(path);
	return test;
}

bool Tests::test_sqlite_escape_string(const std::string& arg)
{
	std::string path = Helper::getFullPath(PICHI_CONFIG_DIR) + "test.db";
	SQLite* sql = new SQLite(path);
	
	std::string esc = sql->escapeString("'x");
	bool test = (esc == "''x");
	
	delete sql;
	Helper::removeFile(path);
	return test;
}

#ifdef WITH_LUA

bool Tests::test_lua_fileload(const std::string& arg)
{
	LuaPichi* lua = new LuaPichi();
	lua->loadFile(arg);
	bool test = lua->getFileStatus() == 0;
	delete lua;
	return test;
}

bool Tests::test_lua_fileload_wrongsyntax(const std::string& arg)
{
	std::string path = Helper::getFullPath(PICHI_CONFIG_DIR) + "test.lua";
	std::ofstream file(path);
	
	file << "function test()\n"
	     << " io.write(\"hahaha\")\n"
	     << "endf\n"; // no end f
	
	file.close();
	
	LuaPichi* lua = new LuaPichi();
	lua->loadFile(path);
	bool test = lua->getFileStatus() != 0;
	delete lua;
	
	Helper::removeFile(path);
	
	return test;
}

bool Tests::test_lua_handler_pushpop(const std::string& arg)
{
	std::string path = Helper::getFullPath(PICHI_CONFIG_DIR) + "test.lua";
	std::ofstream file(path);
	
	file << "testarea = {}\n"
	     << "function testarea.test(one, two)\n"
	     << " if one == 10 then\n"
	     << "   return two\n"
	     << " end\n"
	     << "end\n"; // no end f
	
	file.close();
	
	LuaPichi* lua = new LuaPichi();
	lua->loadFile(path);
	bool test = lua->getFileStatus() == 0; // test 1
	
	lua->luaPush(10);
	lua->luaPush("tt");
	lua->callEvent("testarea", "test", 2, 1);
	std::string two = lua->luaPopString();
	delete lua;
	
	test &= two == "tt";
	
	Helper::removeFile(path); // test 2
	
	return test;
}

bool Tests::test_lua_pichilua_core_listener(const std::string& pichilua)
{
	std::string path = Helper::getFullPath(PICHI_CONFIG_DIR) + "test.lua";
	std::ofstream file(path);
	
	file << "testarea = {}\n"
	     << "function testarea.test()\n"
	     << "  for k,f in pairs(pichi:getListeners(\"runtest\")) do\n"
	     << "    f()\n"
	     << "  end\n"
	     << "end\n"
	     << "pichi:setListener( \"runtest\", \"realtest1\","
	     << "  function( pichiobject )"
	     << "    io.write(\"goodtest1\")"
	     << "  end\n" 
	     << ")\n"
	     << "pichi:setListener( \"runtest\", \"realtest2\","
	     << "  function( pichiobject )"
	     << "    io.write(\"goodtest2\")"
	     << "  end\n" 
	     << ")\n";
	
	file.close();
	
	LuaPichi* lua = new LuaPichi();
	lua->loadFile(pichilua);
	if(lua->getFileStatus() != 0)
		return false;
	lua->loadFile(path);
	if(lua->getFileStatus() != 0)
		return false;
	lua->callEvent("testarea", "test", 0, 0);
	delete lua;
	
	Helper::removeFile(path); // test 2
	
	return true;
}

bool Tests::test_lua_functions_userdata(const std::string& pichilua)
{
	std::string sqlpath = Helper::getFullPath(PICHI_CONFIG_DIR) + "test.db";
	SQLite* sql = new SQLite(sqlpath);
	languages* lang = new languages("en");
	PichiDbPather patch(sql, lang);
	patch.initDbStruct();
    
	PichiCore* pichi = new PichiCore();
	pichi->sql = sql;
	
	std::string path = Helper::getFullPath(PICHI_CONFIG_DIR) + "test.lua";
	std::ofstream file(path);
	
	file << "testarea = {}\n"
	     << "function testarea.test( pichiObject )\n"
	     << " SetJIDinfo( pichiObject, 'deg1@j.ru', 'test', 'badtest' )\n"
	     << " SetJIDinfo( pichiObject, 'deg2@j.ru', 'test', 'badtest' )\n"
	     << " SetJIDinfo( pichiObject, 'deg1@j.ru', 'test', 'goodtest' )\n"
	     << " DelJIDinfo( pichiObject, 'deg2@j.ru', 'test' )\n"
	     << " io.write( GetJIDinfo( pichiObject, 'deg1@j.ru', 'test' ).test )\n"
	     << " io.write( GetJIDinfo( pichiObject, 'deg2@j.ru', 'test' ).test )\n"
	     << "end\n";
	
	file.close();
	
	LuaPichi* lua = new LuaPichi();
	lua->loadFile(pichilua);
	if(lua->getFileStatus() != 0)
		return false;
	lua->loadFile(path);
	if(lua->getFileStatus() != 0)
		return false;
	lua->luaPush(pichi);
	lua->callEvent("testarea", "test", 1, 0);
	delete lua;
	
	Helper::removeFile(path);
	Helper::removeFile(sqlpath);

	delete sql;
	delete lang;
	delete pichi;
	
	return true;
}

bool Tests::test_lua_functions_jsondecode(const std::string& pichilua)
{
	std::string path = Helper::getFullPath(PICHI_CONFIG_DIR) + "test.lua";
	std::ofstream file(path);
	
	file << "testarea = {}\n"
	     << "function testarea.test()\n"
	     << " realtest = JsonDecode([[\n"
	     << "  {\n"
	     << "   \"first\": \"test1\",\n"
	     << "   \"second\": \"test1\",\n"
	     << "   \"age\": 25,\n"
	     << "   \"some1\" : { \"some1st\": \"xxx\" },\n"
	     << "   \"some2\": [\n"
	     << "    {\n"
	     << "     \"t\": \"x\"\n"
	     << "    },\n"
	     << "    {\n"
	     << "     \"t\": \"y\"\n"
	     << "    }\n"
	     << "   ]\n"
	     << "  }\n"
	     << " ]])\n"
	     << " if realtest.second == \"test1\" then\n"
	     << "  io.write(\"goodtest1\")\n"
	     << " end\n"
	     << " if realtest.some1.some1st == \"xxx\" then\n"
	     << "  io.write(\"goodtest2\")\n"
	     << " end\n"
	     << " if realtest.some2[1].t == \"x\" then\n"
	     << "  io.write(\"goodtest3\")\n"
	     << " end\n"
	     << "end\n";
	
	file.close();
	
	LuaPichi* lua = new LuaPichi();
	lua->loadFile(pichilua);
	if(lua->getFileStatus() != 0)
		return false;
	lua->loadFile(path);
	if(lua->getFileStatus() != 0)
		return false;
	lua->callEvent("testarea", "test", 0, 0);
	delete lua;
	
	Helper::removeFile(path); // test 2
	
	return true;
}


#endif

}

#endif