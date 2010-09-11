/*
    <Pichi jabber bot>
    Copyright (C) <2010>  <Alexey Kasyanchuk (deg@uruchie.org)>

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

#ifndef SQLITE_H
#define SQLITE_H

#include <sqlite3.h>
#include <iostream>
#include <map>

class sqlite
{
	private:
		std::string dbfile;
		
		std::string last_query_string;
		int last_query_status;
		int rows_count;
		int last_result_status;
		
		sqlite3 *db;
		sqlite3_stmt *statement;
	public:
		sqlite(std::string f);
		bool query(std::string);
		bool exec(std::string);
		std::map<std::string, std::string> fetchArray(void);
		std::string fetchColumn(int num, bool stay = false);
		const int numColumns() const;
		const int numRows() const;
		const std::string escapeString(std::string);
		bool reset();
		void finalize();
		~sqlite();
};

#endif // SQLITE_H
