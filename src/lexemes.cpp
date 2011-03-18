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

#include "lexemes.h"

namespace pichi
{

lexemes::lexemes(sqlite** s) : sql(s)
{
	message_size_limit = 0;
	word_size_limit = 500;
	query_limit = 10;
	is_answer_limit = false;
}

void lexemes::addNick(const std::string& nick)
{
	nicks.push_back(nick);
	nicks.unique();
}

void lexemes::parseText(std::string text)
{
	cleanString(text);
	//$this->log->log("$string to lexems", PichiLog::LEVEL_DEBUG);
	if(message_size_limit > 0 && text.size() > message_size_limit)
		return;
	
	std::vector<std::string> base = system::explode(" ", text);
	std::string beg("#beg#"), end("#end#");
	
	std::vector<std::string> tmp(3);
	
	if(base.size() <= word_size_limit)
	{
		for(int i = 0; i < INT_MAX && i < base.size(); i++)
		{
			tmp[0] = (i-1 >= 0) ? base[i-1] : beg;
			tmp[1] = base[i];
			tmp[2] = (i+1 <= base.size()-1) ? base[i+1] : end;
			addLexema(tmp);
		}
	}
	
	user_text = text;
}

void lexemes::addLexema(const std::vector<std::string>& lex)
{
	if(lex.size() != 3)
		return;

	std::string sql_query;
	sql_query = "INSERT OR IGNORE INTO lexems (`lexeme1`,`lexeme2`,`lexeme3`) VALUES('" + (*sql)->escapeString(lex[0]) + "', '" + (*sql)->escapeString(lex[1]) + "', '" + (*sql)->escapeString(lex[2]) + "');\n";
	sql_query += "UPDATE lexems SET count = count+1 WHERE lexeme1 = '" + (*sql)->escapeString(lex[0]) + "' AND lexeme2 = '" + (*sql)->escapeString(lex[1]) + "' AND lexeme3 = '" + (*sql)->escapeString(lex[2]) + "';";
	(*sql)->exec(sql_query);	
}

void lexemes::buildArray(void )
{
	if(!answers.empty())
		answers.clear();
	std::map<std::string, std::string> gt;
	while(!(gt = (*sql)->fetchArray()).empty())
	{
		answers.push_back( std::pair< int, std::vector<std::string> >( system::atoi(gt["count"]), std::vector<std::string>(3) ) );
		answers.back().second[0] = gt["lexeme1"];
		answers.back().second[1] = gt["lexeme2"];
		answers.back().second[2] = gt["lexeme3"];
	}
}

std::string lexemes::wToString(const std::vector< std::string >& msg, bool f, bool s, bool t)
{
	return ((f) ? msg[0] : "") + ((f && s) ? " " : "") + ((s) ? msg[1] : "") + (((f || s) && t) ? " " : "") + ((t) ? msg[2] : "");
}

std::vector<std::string> lexemes::choiseWords(void)
{
	std::vector<std::string> empt(3);
	if(answers.empty())
		return empt;
	
	size_t sum = 0, rnd, tmps = 0;
	int n = 0;
	for(std::vector< std::pair< int, std::vector<std::string> > >::iterator it = answers.begin(); it != answers.end(); it++)
		sum += it->first;
	
	srand ( time(NULL) );
	rnd = rand() % sum + 1;
	while(tmps < rnd)
		tmps += answers[n++].first;
	
	return answers[((n > 0) ? n-1 : n)].second;
}



void lexemes::clean(void )
{
      answers.clear();
}

std::string lexemes::genFullRandom()
{
	int limit = 10;
	if(is_answer_limit)
		limit = 5 + rand() % 20;
	//$this->try_count++; //очередная попытка
	(*sql)->query("SELECT * FROM lexems WHERE lexeme1 = '#beg#' ORDER BY RANDOM() LIMIT 0,1;");
	if((*sql)->numRows() == 0)
		return ""; //пусто
	buildArray();
	std::vector<std::string> lastx = choiseWords();
	
	std::string genans = wToString(lastx, false, true, true);
	
	if(lastx[2] == "#end#")
		return wToString(lastx, false, true, false);
	
	for(int i=0; ((is_answer_limit) ? i < limit : true); i++)
	{
		(*sql)->query("SELECT * FROM lexems WHERE lexeme1 = '" + (*sql)->escapeString(lastx[1]) + "' AND lexeme2 = '" + (*sql)->escapeString(lastx[2]) + "' ORDER BY `count` DESC LIMIT 0," + system::ttoa(query_limit) + ";");
		if((*sql)->numRows() == 0)
			break; //больше нет совпадений
		//if($i != $limit-1)
		buildArray();
		lastx = choiseWords();
		//else
		//	$last = $this->doubleLemem($this->buildArray(), "#end#");
		if(lastx[2] == "#end#" || lastx[2] == "")
			break;
		genans += " " + lastx[2];
	}
	//if($genans != $this->user_text)
	clean();
	return genans;
	//else
	//	return $this->randFromLog(); //возращать тоже самое нехорошо, вернем что-нибудь из лога
}
/*
std::string lexemes::genFromWord(std::string word)
{
	int limit = 10;
	if(is_answer_limit)
		limit = 7 + rand() % 12;
	std::string answer = word, first, last, third, second;
	std::vector<std::string> words;
	//$this->try_count++; //очередная попытка
	//left
	for(int i = 0; ((is_answer_limit) ? i < limit : true); i++)
	{
		(*sql)->query("SELECT * FROM lexems WHERE lexeme LIKE '% " + (*sql)->escapeString((i == 0) ? word : first + " " + second) + "' ORDER BY `count` DESC LIMIT 0," + system::ttoa(query_limit) + ";");
		if((*sql)->numRows() == 0)
			break; //больше нет совпадений
		//if(i != $limit-1)
		buildArray();
		last = choiseWord();
		//else
		//	$last = $this->doubleLemem($this->buildArray(), "#beg#", true);
		words = system::explode(" ", last);
		first = words[0];
		second = words[1];
		third = words[2];
			
		if(first == "#beg#" || first == "")
			break;
		answer = ((i==0) ? first + " " + second : first) + " " + answer;
	}
	//right
	for(int i = 0; ((is_answer_limit) ? i < limit : true); i++)
	{
		(*sql)->query("SELECT * FROM lexems WHERE lexeme LIKE '" + (*sql)->escapeString((i == 0) ? word : second + " " + third) + " %' ORDER BY `count` DESC LIMIT 0," + system::ttoa(query_limit) + ";");
		if((*sql)->numRows() == 0)
			break; //больше нет совпадений
		//if($i != $limit-1)
		buildArray();
		last = choiseWord();
		//else
		//	$last = $this->doubleLemem($this->buildArray(), "#end#");
		words = system::explode(" ", last);
		first = words[0];
		second = words[1];
		third = words[2];
			
		if(third == "#end#" || third == "")
			break;
		answer = answer + " " + ((i==0) ? second + " " + third : third);
	}
	//if($answer != $this->user_text)
		return answer;
	//else
	//	return $this->randFromLog(); //возращать тоже самое нехорошо, вернем что-нибудь из лога
}
*/
void lexemes::cleanString(std::string& string)
{
	// Очищаем от лишних пробелов
	while(string.find("  ") != std::string::npos)
		boost::replace_all(string, "  ", " ");
	// убираем обращения
	//if(boost::regex_match(my_nick, boost::regex("^[\\w]+$")))
	//	string = boost::regex_replace(string, boost::regex("^" + my_nick + "(,|:)\\s"), "");
	BOOST_FOREACH(std::string &n, nicks)
	{
		if( string.substr(0, n.size()) == n && (string.substr(n.size(), 2) == ", " || string.substr(n.size(), 2) == ": ") )
			string = string.substr(n.size()+2);
	}
}

}
