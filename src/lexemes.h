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

#ifndef LEXEMES_H
#define LEXEMES_H

#include "sqlite.h"
#include <string>
#include <vector>
#include "system.h"
#include <time.h>
#include <stdio.h>
#include <limits.h>

namespace pichi
{
  
class lexemes
{
  public:
	lexemes(sqlite** s);
	bool is_answer_limit;
	size_t word_size_limit;
	size_t message_size_limit;
	size_t query_limit;
	void parseText(std::string text);
	void addLexema(const std::string& lex);
	
	std::string genFullRandom();
	std::string genFromWord(std::string word);
  protected:
	sqlite **sql;
	std::string user_text;
	std::string send_text;
	
	void buildArray(void);
	std::string choiseWord(void);
	void clean(void);
	

  private:
	std::vector< std::pair< int, std::string > > answers;
	std::string sqlquery;
};

}

#endif // LEXEMES_H
