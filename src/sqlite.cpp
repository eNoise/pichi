/*
    Pichi XMPP (Jabber) Bot.
    Copyright (C) 2010  Alexey Kasyanchuk (deg@uruchie.org)

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

namespace pichi
{
  
SQLite::SQLite(const std::string& f):dbfile(f)
{
	int rc;
	rc = sqlite3_open(dbfile.c_str(), &db);
	if( rc )
	{
		std::string err = sqlite3_errmsg(db);
		sqlite3_close(db);
		throw PichiException("Database error: " + err);
	}
}

SQLite::~SQLite()
{
	finalize();
	sqlite3_close(db);
}

bool SQLite::query(const std::string& sql)
{  
	if(sql != mainquery.query_string)
	{
		finalize();
		mainquery.query_string = sql;
		mainquery.query_status = sqlite3_prepare_v2(db, sql.c_str(), -1, &(mainquery.statement), 0);
		mainquery.is_statement = true;
		Log("[SQLITE] " + sql, Log::VERBOSE);
		// begin count rows
		if(mainquery.query_status != SQLITE_OK)
			return false;

		while(sqlite3_step(mainquery.statement) == SQLITE_ROW)
			mainquery.rows_count++;
		reset();
	}
		
	if(mainquery.query_status == SQLITE_OK)
		return true;
	else
		return false;
}


SQLite::q* SQLite::squery(const std::string& sql)
{
	q* ret = new q();
	ret->query_string = sql;
	ret->query_status = sqlite3_prepare_v2(db, sql.c_str(), -1, &(ret->statement), 0);
	ret->is_statement = true;
	Log("[SQLITE] " + sql, Log::VERBOSE);
	if(ret->query_status != SQLITE_OK)
	{
		delete ret;
		return NULL; // null pointer
	}
	ret->rows_count = 0;
	while(sqlite3_step(ret->statement) == SQLITE_ROW)
		ret->rows_count++;
	sqlite3_reset(ret->statement);
	
	return ret;
}


bool SQLite::exec(const std::string& sql)
{
	char *errtext = NULL;
  
	if (sqlite3_exec(db, sql.c_str(), NULL, NULL, &errtext) != SQLITE_OK)
	{
		Log(static_cast<std::string>("SQL exec error: ") + errtext, Log::WARNING);
		sqlite3_free(errtext);
		return false;
	}
	Log("[SQLITE] " + sql, Log::VERBOSE);
	return true;
}

std::map<std::string, std::string> SQLite::fetchArray(q* state)
{
	if(state == NULL)
		state = &mainquery;
  
	std::map<std::string, std::string> row;
	
	char* column_data;
	state->result_status = sqlite3_step(state->statement);
	if(state->result_status == SQLITE_ROW)
	{
		int cols = sqlite3_column_count(state->statement);
		for(int col = 0; col < cols; col++)
		{
			column_data = (char*)sqlite3_column_text(state->statement, col);
			row.insert(
				std::pair<std::string, std::string>(
					(char*)sqlite3_column_name(state->statement, col), 
					( (column_data != NULL) ? column_data : "" )
				)
			);
		}
	}
	
	return row;
}

std::string SQLite::fetchColumn(const int num, bool stay)
{
	if(!stay)
		mainquery.result_status = sqlite3_step(mainquery.statement);
	char* column_data;
	if(mainquery.result_status == SQLITE_ROW)
	{
		column_data = (char*)sqlite3_column_text(mainquery.statement, num);
		return ( (column_data != NULL) ? column_data : "" );
	}
	else
		return "";
}

std::string SQLite::fetchColumn(SQLite::q* state, const int num, bool stay)
{
	if(!stay)
		state->result_status = sqlite3_step(state->statement);
	char* column_data;
	if(state->result_status == SQLITE_ROW)
	{
		column_data = (char*)sqlite3_column_text(state->statement, num);
		return ( (column_data != NULL) ? column_data : "" );
	}
	else
		return "";
}

  
const int SQLite::numColumns() const
{
	return sqlite3_column_count(mainquery.statement);
}

const int SQLite::numColumns(SQLite::q* state) const
{
	return sqlite3_column_count(state->statement);
}

const int SQLite::numRows() const
{
	return mainquery.rows_count;
}

const int SQLite::numRows(SQLite::q* state) const
{
	return state->rows_count;
}

bool SQLite::reset()
{
	if (sqlite3_reset(mainquery.statement) == SQLITE_OK)
		return true;
	else
		return false;
}

void SQLite::finalize()
{
        if(mainquery.is_statement)
        {
               sqlite3_finalize(mainquery.statement);
               mainquery.is_statement = false;
        }
	mainquery.query_string = "";
	mainquery.rows_count = 0;
}

const std::string SQLite::escapeString(const std::string& sql)
{
	return static_cast<std::string>(sqlite3_mprintf("%q", sql.c_str())); 
}

SQLite::q::q()
{
       is_statement = false;
}

SQLite::q::~q()
{
	finalize();
}

void SQLite::q::finalize(void )
{
	if(is_statement)
	{
		sqlite3_finalize(statement);
		is_statement = false;
	}
}

SQLite* SQLite::clone() const
{
	return new SQLite( *this );
}

SQLite::SQLite(const SQLite& sqlcp)
{
	SQLite(sqlcp.dbfile);
}

}
