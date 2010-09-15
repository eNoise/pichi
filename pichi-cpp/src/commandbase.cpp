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

#include "commandbase.h"
#include "pichicore.h"

commandbase::commandbase(pichicore* p): commandhandler(p)
{
	//base command array
	commands["version"] = &commandbase::command_version;
	commands["enable"] = &commandbase::command_enable;
	commands["disable"] = &commandbase::command_disable;
	commands["log"] = &commandbase::command_log;
	commands["wtf"] = &commandbase::command_wtf;
	commands["wtfcount"] = &commandbase::command_wtfcount;
	commands["wtfrand"] = &commandbase::command_wtfrand;
	commands["wtfrev"] = &commandbase::command_wtfrev;
	commands["wtfull"] = &commandbase::command_wtfull;
	commands["wtfset"] = &commandbase::command_wtfset;
	commands["top"] = &commandbase::command_top;
	commands["talkers"] = &commandbase::command_talkers;
}

void commandbase::fetchCommand(std::string command)
{
	commandhandler::fetchCommand(command);
	if(commands.find(last_command) != commands.end())
		(this->*(commands[last_command]))(last_args);
}


/*
	// Begin commands
	protected function command_help()
	{
		$help = "\n";
		$help .= "-------------------------\n";
		$help .= "        Pichi Bot        \n";
		$help .= "-------------------------\n";
		$help .= "--------------------------------------------\n";
		$help .= PichiLang::get('help_syntaxis_info') . "\n";
		$help .= "--------------------------------------------\n";
		
		$help .= "=====  " . PichiLang::get('help_main_commands') . "  =====\n";
		$help .= "!set ".PichiLang::get('help_command_usage_variable')."=".PichiLang::get('help_command_usage_value')." - ".PichiLang::get('help_command_description_set')."\n";
		$help .= "!gc [".PichiLang::get('help_command_usage_variable')."] - ".PichiLang::get('help_command_description_gc')."\n";
		$help .= "!log ".PichiLang::get('help_command_usage_param')." - ".PichiLang::get('help_command_description_log')."\n";
		$help .= "!users [".PichiLang::get('help_command_usage_nick')."|".PichiLang::get('help_command_usage_jid')."] - ".PichiLang::get('help_command_description_users')."\n";		
		$help .= "!msg [".PichiLang::get('help_command_usage_nick')."|".PichiLang::get('help_command_usage_jid')."|".PichiLang::get('help_command_usage_room')."] [type] message - ".PichiLang::get('help_command_description_msg')."\n";
		$help .= "!ping [".PichiLang::get('help_command_usage_nick')."|".PichiLang::get('help_command_usage_jid')."] - ".PichiLang::get('help_command_description_ping')."\n";
		$help .= "!join ".PichiLang::get('help_command_usage_room')." ".PichiLang::get('help_command_usage_nick')." [".PichiLang::get('help_command_usage_status')."] - ".PichiLang::get('help_command_description_join')."\n";
		$help .= "!left ".PichiLang::get('help_command_usage_room')." ".PichiLang::get('help_command_usage_nick')." [".PichiLang::get('help_command_usage_status')."] - ".PichiLang::get('help_command_description_left')."\n";
		$help .= "!greet ".PichiLang::get('help_command_usage_jid')." ".PichiLang::get('help_command_usage_room')." ".PichiLang::get('help_command_usage_message')." - ".PichiLang::get('help_command_description_greet')."\n";
		$help .= "!farewell ".PichiLang::get('help_command_usage_jid')." ".PichiLang::get('help_command_usage_room')." ".PichiLang::get('help_command_usage_message')." - ".PichiLang::get('help_command_description_farewell')."\n";
		$help .= "!idle ".PichiLang::get('help_command_usage_nick')." - ".PichiLang::get('help_command_description_idle')."\n";
		$help .= "!on - ".PichiLang::get('help_command_description_on')."\n";
		$help .= "!off - ".PichiLang::get('help_command_description_off')."\n";
		$help .= "!quit - ".PichiLang::get('help_command_description_quit')."\n";
		$help .= "!version - ".PichiLang::get('help_command_description_version')."\n";
		
		$help .= "=====  " . PichiLang::get('help_admin_commands') . "  =====\n";
		$help .= "!topic ".PichiLang::get('help_command_usage_param')." - ".PichiLang::get('help_command_description_topic')."\n";
		$help .= "!ban ".PichiLang::get('help_command_usage_jid')."|".PichiLang::get('help_command_usage_nick')." [".PichiLang::get('help_command_usage_time')."] [".PichiLang::get('help_command_usage_reason')."] - ".PichiLang::get('help_command_description_ban')."\n";
		$help .= "!unban ".PichiLang::get('help_command_usage_jid')." - ".PichiLang::get('help_command_description_jid')."\n";
		$help .= "!banlist - ".PichiLang::get('help_command_description_banlist')."\n";
		$help .= "!kick ".PichiLang::get('help_command_usage_nick')."|".PichiLang::get('help_command_usage_jid')." [".PichiLang::get('help_command_usage_time')."] [".PichiLang::get('help_command_usage_reason')."] - ".PichiLang::get('help_command_description_kick')."\n";
		$help .= "!unkick ".PichiLang::get('help_command_usage_jid')." - ".PichiLang::get('help_command_description_unkick')."\n";
		
		$help .= "=====  " . PichiLang::get('help_plugins') . "  =====\n";
		$help .= "!plugins - ".PichiLang::get('help_command_description_plugins')."\n";
		$help .= "!enable ".PichiLang::get('help_command_usage_param')." - ".PichiLang::get('help_command_description_enable')."\n";
		$help .= "!disable ".PichiLang::get('help_command_usage_param')." - ".PichiLang::get('help_command_description_disable')."\n";
		$help .= "!reload - ".PichiLang::get('help_command_description_reload')."\n";

		$help .= "=====  " . PichiLang::get('help_wiki_inline') . "  =====\n";
		$help .= "!dfn ".PichiLang::get('help_command_usage_param')."=".PichiLang::get('help_command_usage_value')." - ".PichiLang::get('help_command_description_dfn')."\n";
		$help .= "!wtf ".PichiLang::get('help_command_usage_param')." - ".PichiLang::get('help_command_description_wtf')."\n";
		$help .= "!wtfcount - ".PichiLang::get('help_command_description_wtfcount')."\n";
		$help .= "!wtfrand - ".PichiLang::get('help_command_description_wtfrand')."\n";
		$help .= "!wtfrev ".PichiLang::get('help_command_usage_param')." - ".PichiLang::get('help_command_description_wtfrev')."\n";
		$help .= "!wtfull ".PichiLang::get('help_command_usage_param')." - ".PichiLang::get('help_command_description_wtffull')."\n";
		$help .= "!wtfset ".PichiLang::get('help_command_usage_param')." - ".PichiLang::get('help_command_description_wtfset')."\n";

		$help .= "=====  " . PichiLang::get('help_lexems') . "  =====\n";
		$help .= "!top - ".PichiLang::get('help_command_description_top')."\n";
		$help .= "!count - ".PichiLang::get('help_command_description_count')."\n";
		$help .= "!talkers - ".PichiLang::get('help_command_description_talkers')."\n";
		$help .= "!nicks ".PichiLang::get('help_command_usage_param')." - ".PichiLang::get('help_command_description_nicks')."\n";

		$help .= "=====  " . PichiLang::get('help_other_commands') . "  =====\n";
		($hook = PichiPlugin::fetch_hook('commands_show_help')) ? eval($hook) : false;
		
		$this->sendAnswer($help);
	}
*/

void commandbase::command_version(std::string null)
{
	//global $config;
	pichi->sendAnswer("Pichi Bot v.C++");
	//($hook = PichiPlugin::fetch_hook('commands_show_version')) ? eval($hook) : false;
	//$this->sendAnswer("".PichiLang::get('command_version_plugins').":\n" . PichiPlugin::show_plugin_list());
}

void commandbase::command_enable(std::string arg)
{
	if(!pichi->isAccess())
		return;
	
	//PichiPlugin::enable(system::atoi(arg));
}

void commandbase::command_disable(std::string arg)
{
	if(!pichi->isAccess())
		return;
	
	//PichiPlugin::disable(system::atoi(arg));
}

/*
	protected function command_reload()
	{
		if(!$this->isAccess())
			return;
		
		PichiPlugin::reload();
	}
	
	protected function command_plugins()
	{
		$this->sendAnswer("".PichiLang::get('command_version_plugins').":\n" . PichiPlugin::show_plugin_list());
	}
*/

/*
	protected function command_kick()
	{
		if(!$this->isAccess())
			return;
		
		$w = $this->seperate($this->last_message, 3);
		$this->kick($w[1], $w[2], $w[3]);
	}
	
	protected function command_unkick()
	{
		if(!$this->isAccess())
			return;
		
		$w = $this->seperate($this->last_message);
		$this->unkick($w[1]);
	}
*/

/*

	protected function command_ban()
	{
		if(!$this->isAccess())
			return;
		
		$w = $this->seperate($this->last_message, 3);
		$this->ban($w[1], $w[2], $w[3]);
	}
	
	protected function command_unban()
	{
		if(!$this->isAccess())
			return;
		
		$w = $this->seperate($this->last_message, 2);
		$this->unban($w[1], $w[2]);
	}

*/

/*
	protected function command_banlist()
	{
		$this->db->query("SELECT `jid`,`value` FROM users_data WHERE name = 'ban';");
		$banlist = "";
		while($bans = $this->db->fetchArray())
		{
			$banlist .= $bans['jid'] . " " . date("d.m.y \в H:i:s", $bans['value']) . "\n";
		}
		$this->sendAnswer("".PichiLang::get('command_banlist_lock').":\n" . $banlist);
	}
*/

void commandbase::command_log(std::string arg)
{
	 	pichi->sql->query("SELECT * FROM log ORDER BY time;");
		int qu_i = pichi->sql->numRows();
		int n = (arg != "") ? (system::atoi(arg)) : 20;
		int i = 0;
		std::string log = "\n-----------------------------------------------------------------------\n";
		std::map<std::string, std::string> data;
		while(!(data = pichi->sql->fetchArray()).empty())
		{
			if(i < n && i < 50 && qu_i <= n)
			{
				log += "[" + system::timeToString(system::atot(data["time"]), "%H:%M:%S") + "]<" + pichi->getName(data["from"]) + "> " + data["message"] + "\n";
				i++;
			}
			qu_i--;
		}
		log += "-----------------------------------------------------------------------";
		//$this->log->log("Request log:\n$log", PichiLog::LEVEL_VERBOSE);
		pichi->sendAnswer(log); 
}


void commandbase::command_wtf(std::string arg)
{
	pichi->sql->query("SELECT `value` FROM wiki WHERE name='" + pichi->sql->escapeString(arg) + "' ORDER BY revision DESC LIMIT 0,1;");
	std::string answer = pichi->sql->fetchColumn(0);
	pichi->sendAnswer(answer);
	//$this->log->log("User request wiki page \"$w[1]\" = $answer", PichiLog::LEVEL_DEBUG);
}

void commandbase::command_wtfcount(std::string arg)
{
	pichi->sql->query("SELECT name FROM wiki;");
	if(pichi->sql->numRows() > 0)
	{
		std::vector<std::string>tmp_ar;
		std::map<std::string, std::string> tmp;
		while(!(tmp = pichi->sql->fetchArray()).empty())
			tmp_ar.push_back(tmp["name"]);
		std::unique(tmp_ar.begin(),tmp_ar.end());
		size_t wtfnum = tmp_ar.size();
		//pichi->sendAnswer("".PichiLang::get('command_wiki_count').": $wtfnum");
		pichi->sendAnswer(system::ttoa(wtfnum));
	}
	else
	{
		//pichi->sendAnswer(PichiLang::get('command_wiki_nodef'));
	}
}

void commandbase::command_wtfrand(std::string arg)
{
	pichi->sql->query("SELECT name FROM wiki ORDER BY RANDOM() LIMIT 0,1;");
	if(pichi->sql->numRows() > 0)
	{
		pichi->sql->query("SELECT name,value FROM wiki WHERE name = '" + pichi->sql->escapeString(pichi->sql->fetchColumn(0)) + "' ORDER BY revision DESC LIMIT 0,1;");
		std::string wtfword = pichi->sql->fetchColumn(0);
		std::string wtfdef = pichi->sql->fetchColumn(1, true);
		if(wtfword != "" && wtfdef != "")
			pichi->sendAnswer(wtfword + " = " + wtfdef);
	}
	else
	{
		//$this->sendAnswer(PichiLang::get('command_wiki_nodef'));
		pichi->sendAnswer("Пустышка");
	} 
}

void commandbase::command_wtfrev(std::string arg)
{
	pichi->sql->query("SELECT revision FROM wiki WHERE name = '" + pichi->sql->escapeString(arg) + "' ORDER BY revision DESC LIMIT 0,1;");
	if(pichi->sql->numRows() > 0)
		//pichi->sendAnswer("".PichiLang::get('command_wiki_revision').": " . $this->db->fetchColumn(0));
		pichi->sendAnswer(pichi->sql->fetchColumn(0));
	else
		//pichi->sendAnswer(PichiLang::get('command_wiki_nodef'));
		pichi->sendAnswer("не установлено");
}

void commandbase::command_wtfull(std::string arg)
{
	pichi->sql->query("SELECT * FROM wiki WHERE name = '" + pichi->sql->escapeString(arg) + "' ORDER BY revision DESC;");
	std::string list_rev;
	std::map<std::string, std::string> tmp;
	while(!(tmp = pichi->sql->fetchArray()).empty())
		//list_rev += "\n------- ".PichiLang::get('command_wiki_revision')." {$tmp['revision']} ({$tmp['name']}) -------\n{$tmp['value']}\n---------------------";
		list_rev += "\n------- " + tmp["revision"] + "(" + tmp["name"] + ")" + "-------\n" + tmp["value"] + "\n---------------------";
	if(list_rev != "")
		pichi->sendAnswer(list_rev);
	else
		//pichi->sendAnswer(PichiLang::get('command_wiki_nodef'));
		pichi->sendAnswer("отсуствует");
}

void commandbase::command_wtfset(std::string arg)
{
	std::vector< std::string > w = seperate(arg, 2);
	pichi->sql->query("SELECT name,revision,value FROM wiki WHERE name = '" + pichi->sql->escapeString(w[0]) + "' AND revision='" + pichi->sql->escapeString(w[1]) + "' LIMIT 0,1;");
	if(pichi->sql->numRows() > 0)
	{
		std::string name = pichi->sql->fetchColumn(0);
		std::string rev = pichi->sql->fetchColumn(1,true);
		std::string val = pichi->sql->fetchColumn(2,true);
		pichi->sql->query("SELECT revision FROM wiki WHERE name = '" + pichi->sql->escapeString(name) + "' ORDER BY revision DESC LIMIT 0,1;");
		int newrev = (system::atoi(pichi->sql->fetchColumn(0))) + 1;
		pichi->sql->query("INSERT INTO wiki (`name`,`revision`,`value`) VALUES ('" + pichi->sql->escapeString(name) + "','" + pichi->sql->escapeString(system::itoa(newrev)) + "','" + pichi->sql->escapeString(val) + "');");
		//pichi->sendAnswer(PichiLang::get('command_wiki_revision_set', array($rev)));
		pichi->sendAnswer(rev);
	}
	else
	{
		//pichi->sendAnswer(PichiLang::get('command_wiki_nodef'));
	} 
}

void commandbase::command_top(std::string arg)
{
	pichi->sql->query("SELECT `lexeme`,`count` FROM lexems ORDER BY count DESC LIMIT 0,10;");
	//pichi->sendAnswer(PichiLang::get('command_top10'));
	pichi->sendAnswer("топ:");
	std::string ans;
	int ix = 0;
	std::map<std::string, std::string> lex;
	std::vector<std::string> tmp;
	while(!(lex = pichi->sql->fetchArray()).empty())
	{
		ix++;
		tmp = system::explode(" ", lex["lexeme"]);
		if(tmp[0] == "#beg#")
			//tmp[0] = "(".PichiLang::get('command_top10_begin').")";
			tmp[0] = "(начало)";
		if(tmp[2] == "#end#")
			//$tmp[2] = "(".PichiLang::get('command_top10_end').")";
			tmp[2] = "(конец)";
		ans += system::itoa(ix) + ". " + system::implode(" ", tmp) + " [" + lex["count"] + "]" + "\n";
	}
	pichi->sendAnswer(ans);
}

void commandbase::command_talkers(std::string arg)
{  
	sqlite::q* qqr = pichi->sql->squery("SELECT `from`, COUNT(*) AS `counter` FROM log GROUP BY `from` ORDER BY `counter` DESC;");
	//pichi->sendAnswer(PichiLang::get('command_talkers'));
	pichi->sendAnswer("топ говорливых:");
	std::string ans;
	std::map<std::string, std::string> fr;
	typedef std::map< std::string, std::pair<std::string, size_t> > p_t;
	typedef std::map< size_t, std::pair<std::string, size_t> > p_t2;
	p_t tmp;
	int i = 0;
	while(!(fr = pichi->sql->fetchArray(qqr)).empty() && i < 10)
	{
		std::string from = pichi->getJID(pichi->getName(fr["from"]), "", true);
		if(from == "")
			continue;
		if(tmp[from] == std::pair<std::string, size_t>())
		{
			tmp[from].second = 0;
			i++;
		}
		tmp[from].first = from;
		tmp[from].second += system::atot(fr["counter"]);
	}
	i = 0;
	
	delete qqr;
	p_t2 tmp2;
	for(p_t::iterator it = tmp.begin(); it != tmp.end(); it++)
		tmp2[it->second.second] = it->second;
	
	 BOOST_REVERSE_FOREACH(p_t2::value_type &p, tmp2)
	 {
		//ans += PichiLang::get('command_talkers_list', array(++$i, $this->getName($key), $val)) . "\n";
		ans += system::itoa(++i) + ". " + pichi->getName(p.second.first) + " (" + system::ttoa(p.second.second) + ")\n";
	 }
	pichi->sendAnswer(ans);
}
