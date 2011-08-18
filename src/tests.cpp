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

namespace pichi {

std::map<std::string, Tests::TestFunction> Tests::testMap = std::map<std::string, Tests::TestFunction>();
  
void Tests::init()
{
	Tests::testMap["regexp_split"] = Tests::test_regexp_split;
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
	
}

bool Tests::test_helper_implode(const std::string& arg)
{

}


}

#endif