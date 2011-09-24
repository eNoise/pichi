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

#include "lexemebuilder.h"
#include "sqlite.h"
#include "helper.h"
#include <boost/foreach.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/regex.hpp>

namespace pichi
{

LexemeBuilder::LexemeBuilder(SQLite** s) : sql(s)
{
	message_size_limit = 0;
	word_size_limit = 500;
	query_limit = 10;
	is_answer_limit = false;
}

void LexemeBuilder::addNick(const std::string& nick)
{
	nicks.push_back(nick);
	nicks.unique();
}

void LexemeBuilder::parseText(std::string text)
{
	cleanString(text);
	//$this->log->log("$string to lexems", PichiLog::LEVEL_DEBUG);
	if(message_size_limit > 0 && text.size() > message_size_limit)
		return;
	
	std::vector<std::string> base = Helper::explode(" ", text);
	std::string beg("#beg#"), end("#end#");
	
	std::vector<std::string> tmp(3);
	
	std::string sqlData = "";
	
	if(base.size() <= word_size_limit)
	{
		for(int i = 0; i < INT_MAX && i < base.size(); i++)
		{
			tmp[0] = (i-1 >= 0) ? base[i-1] : beg;
			tmp[1] = base[i];
			tmp[2] = (i+1 <= base.size()-1) ? base[i+1] : end;
			sqlData += addLexema(tmp);
		}
		if(!sqlData.empty())
			(*sql)->exec(sqlData);
	}
	
	user_text = text;
}

std::string LexemeBuilder::addLexema(const std::vector<std::string>& lex)
{
	if(lex.size() != 3)
		return "";

	std::string sql_query = "INSERT OR IGNORE INTO lexems (`lexeme1`,`lexeme2`,`lexeme3`) VALUES('" + (*sql)->escapeString(lex[0]) + "', '" + (*sql)->escapeString(lex[1]) + "', '" + (*sql)->escapeString(lex[2]) + "');\n";
	sql_query += "UPDATE lexems SET count = count+1 WHERE lexeme1 = '" + (*sql)->escapeString(lex[0]) + "' AND lexeme2 = '" + (*sql)->escapeString(lex[1]) + "' AND lexeme3 = '" + (*sql)->escapeString(lex[2]) + "';\n";
	return sql_query;	
}

void LexemeBuilder::buildArray(void )
{
	if(!answers.empty())
		answers.clear();
	std::map<std::string, std::string> gt;
	while(!(gt = (*sql)->fetchArray()).empty())
	{
		answers.push_back( std::pair< int, std::vector<std::string> >( Helper::atoi(gt["count"]), std::vector<std::string>(3) ) );
		answers.back().second[0] = gt["lexeme1"];
		answers.back().second[1] = gt["lexeme2"];
		answers.back().second[2] = gt["lexeme3"];
	}
}

std::string LexemeBuilder::wToString(const std::vector< std::string >& msg, bool f, bool s, bool t)
{
	return ((f) ? msg[0] : "") + ((f && s) ? " " : "") + ((s) ? msg[1] : "") + (((f || s) && t) ? " " : "") + ((t) ? msg[2] : "");
}

std::vector<std::string> LexemeBuilder::choiseWords(void)
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



void LexemeBuilder::clean(void )
{
      answers.clear();
}

std::string LexemeBuilder::genFullRandom()
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
		(*sql)->query("SELECT * FROM lexems WHERE lexeme1 = '" + (*sql)->escapeString(lastx[1]) + "' AND lexeme2 = '" + (*sql)->escapeString(lastx[2]) + "' ORDER BY `count` DESC LIMIT 0," + Helper::ttoa(query_limit) + ";");
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

std::string LexemeBuilder::genFromWord(const std::string& word)
{
	int limit = 10;
	if(is_answer_limit)
		limit = 7 + rand() % 12;
	
	std::string answer;
	//$this->try_count++; //очередная попытка
	//left
	(*sql)->query("SELECT * FROM lexems WHERE lexeme1 = '" + (*sql)->escapeString(word) + "' OR lexeme2 = '" + (*sql)->escapeString(word) + "' OR lexeme3 = '" + (*sql)->escapeString(word) + "' ORDER BY RANDOM() LIMIT 0,1;");
	buildArray();
	std::vector<std::string> lastx, blastx;
	blastx = lastx = choiseWords();
	
	bool f = true, t = true;
	if(lastx[0] == "#beg#")
		f = false;
	if(lastx[2] == "#end#")
		t = false;
	answer = wToString(lastx, f, true, t);
	
	// left
	for(int i = 0; ((is_answer_limit) ? i < limit : true) && lastx[0] != "#beg#"; i++)
	{
		(*sql)->query("SELECT * FROM lexems WHERE lexeme2 = '" + (*sql)->escapeString(lastx[0]) + "' AND lexeme3 = '" + (*sql)->escapeString(lastx[1]) + "' ORDER BY `count` DESC LIMIT 0," + Helper::ttoa(query_limit) + ";");
		if((*sql)->numRows() == 0)
			break; //больше нет совпадений
		//if($i != $limit-1)
		buildArray();
		lastx = choiseWords();
		//else
		//	$last = $this->doubleLemem($this->buildArray(), "#end#");
		if(lastx[0] == "#beg#" || lastx[0] == "")
			break;
		answer = lastx[0] + " " + answer;
	}
	//right
	lastx = blastx; // возращаем исходную фразу
	for(int i = 0; ((is_answer_limit) ? i < limit : true) && lastx[2] != "#end#"; i++)
	{
		(*sql)->query("SELECT * FROM lexems WHERE lexeme1 = '" + (*sql)->escapeString(lastx[1]) + "' AND lexeme2 = '" + (*sql)->escapeString(lastx[2]) + "' ORDER BY `count` DESC LIMIT 0," + Helper::ttoa(query_limit) + ";");
		if((*sql)->numRows() == 0)
			break; //больше нет совпадений
		//if($i != $limit-1)
		buildArray();
		lastx = choiseWords();
		//else
		//	$last = $this->doubleLemem($this->buildArray(), "#end#");
		if(lastx[2] == "#end#" || lastx[2] == "")
			break;
		answer += " " + lastx[2];
	}
	//if($answer != $this->user_text)
	return answer;
	//else
	//	return $this->randFromLog(); //возращать тоже самое нехорошо, вернем что-нибудь из лога
}


void LexemeBuilder::cleanString(std::string& string)
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
