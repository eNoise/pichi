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

#include "sqlite.h"

sqlite::sqlite(std::string f):dbfile(f)
{
	int rc;
	rc = sqlite3_open(dbfile.c_str(), &db);
	if( rc )
	{
		std::cout << static_cast<std::string>("Db file open failed with error: ") + sqlite3_errmsg(db) << std::endl;
		sqlite3_close(db);
		return;
	}
}

sqlite::~sqlite()
{
	finalize();
	sqlite3_close(db);
}

bool sqlite::query(std::string sql)
{  
	if(sql != last_query_string)
	{
		finalize();
		last_query_string = sql;
		last_query_status = sqlite3_prepare_v2(db, sql.c_str(), -1, &statement, 0);
		
		// begin count rows
		if(last_query_status != SQLITE_OK)
			return false;

		while(sqlite3_step(statement) == SQLITE_ROW)
			rows_count++;
		reset();
	}
		
	if(last_query_status == SQLITE_OK)
		return true;
	else
		return false;
}

bool sqlite::exec(std::string sql)
{
	char *errtext = NULL;
  
	if (sqlite3_exec(db, sql.c_str(), NULL, NULL, &errtext) != SQLITE_OK)
	{
		std::cout << static_cast<std::string>("SQL exec error: ") + errtext << std::endl;
		sqlite3_free(errtext);
		return false;
	}
	
	return true;
}

std::map<std::string, std::string> sqlite::fetchArray()
{
	std::map<std::string, std::string> row;
	
	last_result_status = sqlite3_step(statement);
	if(last_result_status == SQLITE_ROW)
	{
		int cols = sqlite3_column_count(statement);
		for(int col = 0; col < cols; col++)
			row.insert(std::pair<std::string, std::string>((char*)sqlite3_column_name(statement, col), (char*)sqlite3_column_text(statement, col)));
	}
	
	return row;
}

std::string sqlite::fetchColumn(int num, bool stay)
{
	if(!stay)
		last_result_status = sqlite3_step(statement);
	if(last_result_status == SQLITE_ROW)
		return static_cast<std::string>((char*)sqlite3_column_text(statement, num));
	else
		return std::string();
}
  
const int sqlite::numColumns() const
{
	return sqlite3_column_count(statement);
}

const int sqlite::numRows() const
{
	return rows_count;
}

bool sqlite::reset()
{
	if (sqlite3_reset(statement) == SQLITE_OK)
		return true;
	else
		return false;
}

void sqlite::finalize()
{
	sqlite3_finalize(statement);
	last_query_string = "";
	rows_count = 0;
}

const std::string sqlite::escapeString(std::string sql)
{
	return static_cast<std::string>(sqlite3_mprintf("%q", sql.c_str())); 
}
