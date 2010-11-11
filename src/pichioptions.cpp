/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) <year>  <name of author>

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

bool pichioptions::reloadSqlConfig(void )
{
	(*sql_options)->query("SELECT * FROM settings;");
	//$this->log->log("Parse Settings", PichiLog::LEVEL_DEBUG);
	std::map<std::string, std::string> data;
	while((data = (*sql_options)->fetchArray()).size() > 0)
	{
		options[ data["name"] ] = data["value"];
		//$this->log->log("$data[name] = $data[value]", PichiLog::LEVEL_VERBOSE);
	}
	//postParseOptions();
}

void pichioptions::setSqlOption(std::string name, std::string value)
{
	if(name == "")
		return;
	
	if(options[ name ] != "")
		options[ name ] = value;
	else
		return;
		
	(*sql_options)->exec("UPDATE settings SET value = '" + (*sql_options)->escapeString(value) + "' WHERE name = '" + (*sql_options)->escapeString(name) + "';");
}

std::string pichioptions::getSqlOption(std::string name)
{
	return (options[ name ]);
}
