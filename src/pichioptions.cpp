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

#include "pichioptions.h"
#include "sqlite.h"

namespace pichi
{

PichiOptions::PichiOptions(SQLite** sql) : sql(sql)
{

}
  
void PichiOptions::reloadSqlConfig(void )
{
	(*sql)->query("SELECT * FROM settings;");
	std::map<std::string, std::string> data;
	while((data = (*sql)->fetchArray()).size() > 0)
	{
		options[ data["name"] ] = data["value"];
	}
}

void PichiOptions::setSqlOption(std::string name, std::string value)
{
	if(name == "")
		return;
	
	if(options[ name ] != "")
		options[ name ] = value;
	else
		return;
		
	(*sql)->exec("UPDATE settings SET value = '" + (*sql)->escapeString(value) + "' WHERE name = '" + (*sql)->escapeString(name) + "';");
}

std::string PichiOptions::getSqlOption(std::string name)
{
	return (options[ name ]);
}

}
