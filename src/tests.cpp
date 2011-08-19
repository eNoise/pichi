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
#include "config.h"
#include "sqlite.h"

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
	Tests::testMap["sqlite_open"] = Tests::test_sqlite_open;
	Tests::testMap["sqlite_query"] = Tests::test_sqlite_query;
	Tests::testMap["sqlite_query_async"] = Tests::test_sqlite_query_async;
	Tests::testMap["sqlite_escape_string"] = Tests::test_sqlite_escape_string;
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

}

#endif