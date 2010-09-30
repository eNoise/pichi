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
#include "core.h"

commandbase::commandbase(pichicore* p): commandhandler(p)
{
	//base command array
	commands["help"] = &commandbase::command_help;
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
	commands["count"] = &commandbase::command_count;
	commands["dfn"] = &commandbase::command_dfn;
	commands["set"] = &commandbase::command_set;
	commands["msg"] = &commandbase::command_msg;
	commands["gc"] = &commandbase::command_gc;
	commands["ping"] = &commandbase::command_ping;
	commands["topic"] = &commandbase::command_topic;
	commands["nicks"] = &commandbase::command_nicks;
	commands["idle"] = &commandbase::command_idle;
}

void commandbase::fetchCommand(std::string command)
{
	commandhandler::fetchCommand(command);
	if(commands.find(last_command) != commands.end())
		(this->*(commands[last_command]))(last_args);
}

void commandbase::command_help(std::string arg)
{
	std::string help = "\n";
	help += "-------------------------\n";
	help += "        Pichi Bot        \n";
	help += "-------------------------\n";
	help += "--------------------------------------------\n";
	help += TR("help_syntaxis_info")  + "\n";
	help += "--------------------------------------------\n";
	
	help += "=====  " +  TR("help_main_commands")  + "  =====\n";
	help += "!set " + TR("help_command_usage_variable") + "=" + TR("help_command_usage_value") + " - " + TR("help_command_description_set") + "\n";
	help += "!gc [" + TR("help_command_usage_variable") + "] - " + TR("help_command_description_gc") + "\n";
	help += "!log " + TR("help_command_usage_param") + " - " + TR("help_command_description_log") + "\n";
	help += "!users [" + TR("help_command_usage_nick") + "|" + TR("help_command_usage_jid") + "] - " + TR("help_command_description_users") + "\n";		
	help += "!msg [" + TR("help_command_usage_nick") + "|" + TR("help_command_usage_jid") + "|" + TR("help_command_usage_room") + "] [type] message - " + TR("help_command_description_msg") + "\n";
	help += "!ping [" + TR("help_command_usage_nick") + "|" + TR("help_command_usage_jid") + "] - " + TR("help_command_description_ping") + "\n";
	help += "!join " + TR("help_command_usage_room") + " " + TR("help_command_usage_nick") + " [" + TR("help_command_usage_status") + "] - " + TR("help_command_description_join") + "\n";
	help += "!left " + TR("help_command_usage_room") + " " + TR("help_command_usage_nick") + " [" + TR("help_command_usage_status") + "] - " + TR("help_command_description_left") + "\n";
	help += "!greet " + TR("help_command_usage_jid") + " " + TR("help_command_usage_room") + " " + TR("help_command_usage_message") + " - " + TR("help_command_description_greet") + "\n";
	help += "!farewell " + TR("help_command_usage_jid") + " " + TR("help_command_usage_room") + " " + TR("help_command_usage_message") + " - " + TR("help_command_description_farewell") + "\n";
	help += "!idle " + TR("help_command_usage_nick") + " - " + TR("help_command_description_idle") + "\n";
	help += "!on - " + TR("help_command_description_on") + "\n";
	help += "!off - " + TR("help_command_description_off") + "\n";
	help += "!quit - " + TR("help_command_description_quit") + "\n";
	help += "!version - " + TR("help_command_description_version") + "\n";
		
	help += "=====  " +  TR("help_admin_commands")  + "  =====\n";
	help += "!topic " + TR("help_command_usage_param") + " - " + TR("help_command_description_topic") + "\n";
	help += "!ban " + TR("help_command_usage_jid") + "|" + TR("help_command_usage_nick") + " [" + TR("help_command_usage_time") + "] [" + TR("help_command_usage_reason") + "] - " + TR("help_command_description_ban") + "\n";
	help += "!unban " + TR("help_command_usage_jid") + " - " + TR("help_command_description_jid") + "\n";
	help += "!banlist - " + TR("help_command_description_banlist") + "\n";
	help += "!kick " + TR("help_command_usage_nick") + "|" + TR("help_command_usage_jid") + " [" + TR("help_command_usage_time") + "] [" + TR("help_command_usage_reason") + "] - " + TR("help_command_description_kick") + "\n";
	help += "!unkick " + TR("help_command_usage_jid") + " - " + TR("help_command_description_unkick") + "\n";
	
	help += "=====  " +  TR("help_plugins")  + "  =====\n";
	help += "!plugins - " + TR("help_command_description_plugins") + "\n";
	help += "!enable " + TR("help_command_usage_param") + " - " + TR("help_command_description_enable") + "\n";
	help += "!disable " + TR("help_command_usage_param") + " - " + TR("help_command_description_disable") + "\n";
	help += "!reload - " + TR("help_command_description_reload") + "\n";

	help += "=====  " +  TR("help_wiki_inline")  + "  =====\n";
	help += "!dfn " + TR("help_command_usage_param") + "=" + TR("help_command_usage_value") + " - " + TR("help_command_description_dfn") + "\n";
	help += "!wtf " + TR("help_command_usage_param") + " - " + TR("help_command_description_wtf") + "\n";
	help += "!wtfcount - " + TR("help_command_description_wtfcount") + "\n";
	help += "!wtfrand - " + TR("help_command_description_wtfrand") + "\n";
	help += "!wtfrev " + TR("help_command_usage_param") + " - " + TR("help_command_description_wtfrev") + "\n";
	help += "!wtfull " + TR("help_command_usage_param") + " - " + TR("help_command_description_wtffull") + "\n";
	help += "!wtfset " + TR("help_command_usage_param") + " - " + TR("help_command_description_wtfset") + "\n";

	help += "=====  " +  TR("help_lexems")  + "  =====\n";
	help += "!top - " + TR("help_command_description_top") + "\n";
	help += "!count - " + TR("help_command_description_count") + "\n";
	help += "!talkers - " + TR("help_command_description_talkers") + "\n";
	help += "!nicks " + TR("help_command_usage_param") + " - " + TR("help_command_description_nicks") + "\n";

	help += "=====  " +  TR("help_other_commands")  + "  =====\n";
	//($hook = PichiPlugin::fetch_hook("commands_show_help")) ? eval($hook) : false;
		
	pichi->sendAnswer(help);
}

void commandbase::command_version(std::string null)
{
	//global $config;
	pichi->sendAnswer("Pichi Bot v.C++");
	//($hook = PichiPlugin::fetch_hook("commands_show_version")) ? eval($hook) : false;
	//pichi->sendAnswer(""+TR("command_version_plugins")+":\n" + PichiPlugin::show_plugin_list());
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
		pichi->sendAnswer(""+TR("command_version_plugins")+":\n" + PichiPlugin::show_plugin_list());
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
			$banlist .= $bans['jid'] + " " + date("d.m.y \в H:i:s", $bans['value']) + "\n";
		}
		pichi->sendAnswer(""+TR("command_banlist_lock")+":\n" + $banlist);
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
		pichi->sendAnswer("" + TR("command_wiki_count") + ": " + system::ttoa(wtfnum));
	}
	else
	{
		pichi->sendAnswer(TR("command_wiki_nodef"));
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
		pichi->sendAnswer(TR("command_wiki_nodef"));
	} 
}

void commandbase::command_wtfrev(std::string arg)
{
	pichi->sql->query("SELECT revision FROM wiki WHERE name = '" + pichi->sql->escapeString(arg) + "' ORDER BY revision DESC LIMIT 0,1;");
	if(pichi->sql->numRows() > 0)
		pichi->sendAnswer("" + TR("command_wiki_revision") + ": " + pichi->sql->fetchColumn(0));
	else
		pichi->sendAnswer(TR("command_wiki_nodef"));
}

void commandbase::command_wtfull(std::string arg)
{
	pichi->sql->query("SELECT * FROM wiki WHERE name = '" + pichi->sql->escapeString(arg) + "' ORDER BY revision DESC;");
	std::string list_rev;
	std::map<std::string, std::string> tmp;
	while(!(tmp = pichi->sql->fetchArray()).empty())
		list_rev += "\n------- " + TR("command_wiki_revision") + tmp["revision"] + "(" + tmp["name"] + ")" + "-------\n" + tmp["value"] + "\n---------------------";
	if(list_rev != "")
		pichi->sendAnswer(list_rev);
	else
		pichi->sendAnswer(TR("command_wiki_nodef"));
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
		pichi->sendAnswer(TR2("command_wiki_revision_set", rev.c_str()));
	}
	else
	{
		pichi->sendAnswer(TR("command_wiki_nodef"));
	} 
}

void commandbase::command_top(std::string arg)
{
	pichi->sql->query("SELECT `lexeme`,`count` FROM lexems ORDER BY count DESC LIMIT 0,10;");
	pichi->sendAnswer(TR("command_top10"));
	std::string ans;
	int ix = 0;
	std::map<std::string, std::string> lex;
	std::vector<std::string> tmp;
	while(!(lex = pichi->sql->fetchArray()).empty())
	{
		ix++;
		tmp = system::explode(" ", lex["lexeme"]);
		if(tmp[0] == "#beg#")
			tmp[0] = "("+TR("command_top10_begin")+")";
		if(tmp[2] == "#end#")
			tmp[2] = "("+TR("command_top10_end")+")";
		ans += system::itoa(ix) + "+ " + system::implode(" ", tmp) + " [" + lex["count"] + "]" + "\n";
	}
	pichi->sendAnswer(ans);
}

void commandbase::command_talkers(std::string arg)
{  
	sqlite::q* qqr = pichi->sql->squery("SELECT `from`, COUNT(*) AS `counter` FROM log GROUP BY `from` ORDER BY `counter` DESC;");
	pichi->sendAnswer(TR("command_talkers"));
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
	
	qqr->finalize();
	delete qqr;
	p_t2 tmp2;
	for(p_t::iterator it = tmp.begin(); it != tmp.end(); it++)
		tmp2[it->second.second] = it->second;
	
	 BOOST_REVERSE_FOREACH(p_t2::value_type &p, tmp2)
	 {
		ans += TR4("command_talkers_list", system::itoa(++i).c_str(), pichi->getName(p.second.first).c_str(), system::ttoa(p.second.second).c_str()) + "\n";
	 }
	pichi->sendAnswer(ans);
}

void commandbase::command_count(std::string arg)
{
	pichi->sql->query("SELECT COUNT(*) FROM lexems;");
	size_t lexnum = system::atot(pichi->sql->fetchColumn(0));
	pichi->sendAnswer(TR2("command_count", system::ttoa(lexnum).c_str()));
}

void commandbase::command_dfn(std::string arg)
{
	std::vector< std::string > w = seperate(arg, 2);

	pichi->sql->query("SELECT revision FROM wiki WHERE name = '" + pichi->sql->escapeString(w[0]) + "' ORDER BY revision DESC LIMIT 0,1;");
	size_t rev;
	if(pichi->sql->numRows() > 0)
	{
		rev = system::atot(pichi->sql->fetchColumn(0));
		pichi->sql->query("INSERT INTO wiki (`name`,`revision`,`value`) VALUES ('" + pichi->sql->escapeString(w[0]) + "','" + pichi->sql->escapeString(system::ttoa(rev + 1)) + "','" + pichi->sql->escapeString(w[1]) + "');");
	}
	else
	{
		pichi->sql->query("INSERT INTO wiki (`name`,`revision`,`value`) VALUES ('" + pichi->sql->escapeString(w[0]) + "','1','" + pichi->sql->escapeString(w[1]) + "');");
	}
	
	//$this->log->log("User set wiki page $w[1] = $w[2]", PichiLog::LEVEL_DEBUG);
	pichi->sendAnswer(TR("command_dfn"));
}

void commandbase::command_set(std::string arg)
{
	if(!pichi->isAccess())
		return;
     
	std::vector< std::string > w = seperate(arg, 2);
	if(pichi->setOption(w[0], w[1]))
		pichi->sendAnswer(TR("command_set"));
	else	
		pichi->sendAnswer(TR("command_nosuch"));
}

void commandbase::command_msg(std::string arg)
{
	if(!pichi->isAccess())
		return;
     
	std::vector< std::string > w = seperate(arg, 1);

	pichi->jabber->sendMessage(JID(pichi->getJID(w[0])), w[1]);
	//$this->log->log("Send message to $user: $message", PichiLog::LEVEL_DEBUG);
}


void commandbase::command_gc(std::string arg)
{
	if(!pichi->isAccess())
		return;
     
	pichi->sql->query("SELECT * FROM settings" + ((arg != "") ? " WHERE name='" + pichi->sql->escapeString(arg) + "'" : "") + ";");
	std::map<std::string, std::string> data;
	std::string show = "\n";
	while(!(data = pichi->sql->fetchArray()).empty())
	{
		show += data["name"] + " = " + data["value"] + " // " + data["description"] + "\n";
		//$this->log->log("User request setting: {$data['name']} = {$data['value']}", PichiLog::LEVEL_VERBOSE);
	}
	pichi->sendAnswer(show);
}

/*
	
	protected function command_users()
	{
		$w = $this->seperate($this->last_message);
      
		$this->db->query("SELECT * FROM users;");
      
		if($w[1] == NULL)
		{
			$this->log->log("Begin creting user list", PichiLog::LEVEL_DEBUG);
			$userlist = ""+TR("command_users_list_seen")+":\n";
			$online = $offline = "";
			$n = $f = 0;
			while($data = $this->db->fetchArray())
			{
				if($data['room'] == NULL)
					continue;
				$roomname = explode("@", $data['room']);
				$roomname = $roomname[0];
				if($data['status'] == 'available")
				{
					$n++;
					$online .= TR("command_users_online_seen',array($n, $data['nick'], $roomname)) + "\n";
					$this->log->log("User $data[nick]: online", PichiLog::LEVEL_VERBOSE);
				}
				else
				{
					$f++;
					$offline .= TR("command_users_offline_seen',array($f, $data['nick'], date("d.m.y \в H:i:s", $data['time']), $roomname)) + "\n";
					$this->log->log("User $data[nick]: offline", PichiLog::LEVEL_VERBOSE);
				}
			}
			$userlist .= ""+TR("command_users_online")+":\n" + $online;
			$userlist .= ""+TR("command_users_offline")+":\n" + $offline;
			pichi->sendAnswer($userlist);
		}
		else
		{
			while($data = $this->db->fetchArray())
			{
				if($data['nick'] == $w[1] || $data['jid'] == $w[1])
				{
					$this->log->log("User {$data['nick']} founded!", PichiLog::LEVEL_VERBOSE);
					pichi->sendAnswer(TR("command_status', array($data['nick'], (($data['status'] == 'available") ? TR("command_status_online") : TR("command_status_offline")))));
				}
			}
		}
	}

	protected function command_join()
	{
		if(!$this->isAccess())
			return;
		$w = $this->seperate($this->last_message, 3);
		$this->joinRoom($w[1], $w[2], $w[3]);
	}
	
	protected function command_left()
	{
		if(!$this->isAccess())
			return;
		$w = $this->seperate($this->last_message, 3);
		$this->leftRoom($w[1], $w[2], $w[3]);
	}
	
*/

void commandbase::command_ping(std::string arg)
{
	if(pichi->isOnline(arg))
		pichi->ping(pichi->getJID(pichi->getName(arg)));
	else
		pichi->sendAnswer(TR("command_ping_nouser"));
}

void commandbase::command_topic(std::string arg)
{
  /*
	if(arg == ")
		room = pichi->getDefaultRoom();
	$w = $this->seperate($this->last_message);
	$this->jabber->setTopic($room, $w[1]);
  */
}

void commandbase::command_nicks(std::string arg)
{
	std::string ans;
	pichi->sql->query("SELECT `nick` FROM users_nick WHERE `jid` = '" + pichi->sql->escapeString(pichi->getJID(pichi->getName(arg))) + "' GROUP BY `nick`;");
	std::map<std::string, std::string> data;
	while(!(data = pichi->sql->fetchArray()).empty())
		ans += data["nick"] + "\n";
	if(ans == "")
		return;
	pichi->sendAnswer(TR("command_nicks") + "\n" + ans);
}

void commandbase::command_idle(std::string arg)
{
	pichi->sql->query("SELECT `time` FROM log WHERE `from` = '" + pichi->sql->escapeString(arg) + "' OR `from` LIKE '%/" + pichi->sql->escapeString(pichi->getName(arg)) + "' ORDER BY time DESC;");
	time_t date = boost::lexical_cast<time_t>(pichi->sql->fetchColumn(0));
	if(date > 0)
		pichi->sendAnswer(system::timeToString(date, "%d.%m.%Y в %H:%M:%S"));
}

/*

	protected function command_greet()
	{
		$w = $this->seperate($this->last_message, 3);
		if(!$this->isAccess())
			return;
		$action = ($w[0] == "!greet") ? "user_join_room" : "user_left_room";
		$this->db->query("SELECT COUNT(*) FROM actions WHERE action = '$action' AND coincidence='room=" + $this->db->db->escapeString($w[2]) + ",jid=" + $this->db->db->escapeString($w[1]) + "';");
		if($this->db->fetchColumn() > 0)
			$this->db->query("UPDATE actions SET value = '"+$this->db->db->escapeString($w[3])+"'  WHERE action = '$action' AND coincidence='room=" + $this->db->db->escapeString($w[2]) + ",jid=" + $this->db->db->escapeString($w[1]) + "';");
		else
			$this->db->query("INSERT INTO actions (`action`,`coincidence`,`do`,`option`,`value`) VALUES ('$action', 'room=" + $this->db->db->escapeString($w[2]) + ",jid=" + $this->db->db->escapeString($w[1]) + "', 'send_message', '', '"+$this->db->db->escapeString($w[3])+"');");
		pichi->sendAnswer("Updated!");
	}
	
	protected function command_quit()
	{
		if(!$this->isAccess(3))
			return;
		$this->doExit();
	}
	
	protected function command_on()
	{
		if(!$this->isAccess(3))
			return;
		$this->enabled = TRUE;
		pichi->sendAnswer("Updated!");
	}
	
	protected function command_off()
	{
		if(!$this->isAccess(3))
			return;
		$this->enabled = FALSE;
		pichi->sendAnswer("Updated!");
	}

*/