/*
    <one line to give the program's name and a brief idea of what it does.>
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

#include "lexemes.h"

lexemes::lexemes(sqlite** s) : sql(s)
{
	lexeme_limit = 1000;
}

void lexemes::parseText(std::string text)
{
	//$this->testText($string); //режим все лишнее со строки
	//$this->log->log("$string to lexems", PichiLog::LEVEL_DEBUG);
	std::vector<std::string> base = system::explode(" ", text);
	std::string str, beg("#beg#"), end("#end#");
	
	if(base.size() <= lexeme_limit)
	{
		for(int i = 0; i < base.size(); i++)
		{
			str = ((i-1 >= 0) ? base[i-1] : beg ) + " " + base[i]  + " " + ((i+1 <= base.size()-1) ? base[i+1] : end);
			addLexema(str);
		}
	}
	
	user_text = text;
}

void lexemes::addLexema(std::string lex)
{
	if(lex == "")
		return;
      
	(*sql)->query("SELECT COUNT(*) FROM lexems WHERE lexeme = '" + (*sql)->escapeString(lex) + "';");
	if(system::atoi((*sql)->fetchColumn(0)) > 0)
		(*sql)->exec("UPDATE lexems SET count = count+1  WHERE lexeme = '" + (*sql)->escapeString(lex) + "';");
	else
		(*sql)->exec("INSERT INTO lexems (`lexeme`,`count`) VALUES ('" + (*sql)->escapeString(lex) + "','1');");
	//$this->log->log("$str writed to lexems", PichiLog::LEVEL_VERBOSE);
}
