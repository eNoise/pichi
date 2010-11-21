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

namespace pichi
{

lexemes::lexemes(sqlite** s) : sql(s)
{
	lexeme_limit = 1000;
	query_limit = 10;
	is_answer_limit = false;
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

void lexemes::buildArray(void )
{
	if(!answers.empty())
		answers.clear();
	std::map<std::string, std::string> gt;
	while(!(gt = (*sql)->fetchArray()).empty())
		answers.push_back( std::pair< int, std::string >( system::atoi(gt["count"]), gt["lexeme"] ) );
}

std::string lexemes::choiseWord(void )
{
	if(answers.empty())
		return "";
	
	size_t sum = 0, rnd, tmps = 0;
	int n = 0;
	for(std::vector< std::pair< int, std::string > >::iterator it = answers.begin(); it != answers.end(); it++)
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
	(*sql)->query("SELECT * FROM lexems WHERE lexeme LIKE '#beg# %' ORDER BY RANDOM() LIMIT 0,1;");
	if((*sql)->numRows() == 0)
		return ""; //пусто
	buildArray();
	std::string last = choiseWord();
	std::vector<std::string> lastx = system::explode(" ", last);

	last = lastx[1] + " " + lastx[2];
	std::string genans = lastx[1] + ((lastx[2] != "#end#") ? " " + lastx[2] : "");
	if(lastx[2] == "#end#")
		return genans;

	for(int i=0; ((is_answer_limit) ? i < limit : true); i++)
	{
		(*sql)->query("SELECT * FROM lexems WHERE lexeme LIKE '" + (*sql)->escapeString(last) + " %' ORDER BY `count` DESC LIMIT 0," + system::itoa(query_limit) + ";");
		if((*sql)->numRows() == 0)
			break; //больше нет совпадений
		//if($i != $limit-1)
			buildArray();
			last = choiseWord();
		//else
		//	$last = $this->doubleLemem($this->buildArray(), "#end#");
		lastx = system::explode(" ", last);
		last = lastx[1] + " " + lastx[2];
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
		(*sql)->query("SELECT * FROM lexems WHERE lexeme LIKE '% " + (*sql)->escapeString((i == 0) ? word : first + " " + second) + "' ORDER BY `count` DESC LIMIT 0," + system::itoa(query_limit) + ";");
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
		(*sql)->query("SELECT * FROM lexems WHERE lexeme LIKE '" + (*sql)->escapeString((i == 0) ? word : second + " " + third) + " %' ORDER BY `count` DESC LIMIT 0," + system::itoa(query_limit) + ";");
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

}
