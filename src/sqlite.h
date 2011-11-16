/*
    Pichi XMPP (Jabber) Bot.
    Copyright (C) 2011  Alexey Kasyanchuk (deg@uruchie.org)

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
#include "pichiexception.h"
#include "log.h"

namespace pichi
{

class SQLite
{
	public:
		typedef std::map<std::string, std::string> SQLRow;
		struct q // query struct
		{
			sqlite3_stmt *statement;
			bool is_statement;
			std::string query_string;
			int query_status;
			int result_status;
			int rows_count;
			q();
			~q();
			void finalize(void);
			const int numColumns() const;
			const int numRows() const;
		};
		SQLite* clone() const;
		SQLite(const std::string& f);
		SQLite(const SQLite& sqlcp);
		bool query(const std::string&);
		q* squery(const std::string&);
		bool exec(const std::string&);
		SQLRow fetchArray();
		static SQLRow fetchArray(q* state);
		std::string fetchColumn(const int num, bool stay = false);
		static std::string fetchColumn(q* state, const int num, bool stay = false);
		const int numColumns() const;
		const int numColumns(q* state) const;
		const int numRows() const;
		const int numRows(q* state) const;
		const std::string escapeString(const std::string&);
		bool reset();
		void finalize();
		~SQLite();
	private:
		std::string dbfile;
		sqlite3 *db;

		q mainquery;
};

}

#endif // SQLITE_H
