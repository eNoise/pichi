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

#include "commandbase.h"
#include "pichicore.h"
#include "core.h"

namespace pichi
{

commandbase::commandbase(pichicore* p): commandhandler(p)
{
	//base command array
	commands["help"] = &commandbase::command_help;
	commands["version"] = &commandbase::command_version;
	commands["enable"] = &commandbase::command_enable;
	commands["disable"] = &commandbase::command_disable;
	commands["reload"] = &commandbase::command_reload;
	commands["plugins"] = &commandbase::command_plugins;
	commands["ban"] = &commandbase::command_ban;
	commands["unban"] = &commandbase::command_unban;
	commands["kick"] = &commandbase::command_kick;
	commands["unkick"] = &commandbase::command_unkick;
	commands["banlist"] = &commandbase::command_banlist;
	commands["kicklist"] = &commandbase::command_kicklist;
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
	commands["users"] = &commandbase::command_users;
	commands["join"] = &commandbase::command_join;
	commands["left"] = &commandbase::command_left;
	commands["ping"] = &commandbase::command_ping;
	commands["topic"] = &commandbase::command_topic;
	commands["nicks"] = &commandbase::command_nicks;
	commands["idle"] = &commandbase::command_idle;
	commands["q"] = &commandbase::command_q;
	commands["greet"] = &commandbase::command_greet;
	commands["farewell"] = &commandbase::command_farewell;
	commands["quit"] = &commandbase::command_quit;
	commands["on"] = &commandbase::command_on;
	commands["off"] = &commandbase::command_off;
	commands["uptime"] = &commandbase::command_uptime;
	
	commands["lastfm"] = &commandbase::command_lastfm;
	commands["lastfm_user"] = &commandbase::command_lastfm_user;
	
	commands["translate"] = &commandbase::command_translate;
	commands["tr"] = &commandbase::command_tr;
	commands["translate_language"] = &commandbase::command_translate_language;
	commands["google"] = &commandbase::command_google;
	
	commands["urlshort"] = &commandbase::command_urlshort;
}

void commandbase::fetchCommand(std::string command)
{
	commandhandler::fetchCommand(command);
	if(commands.find(last_command) != commands.end())
		(this->*(commands[last_command]))(last_args);
}

void commandbase::command_help(std::string arg)
{
	std::map< std::string, std::string > help;
	help["header"] = "-------------------------\n";
	help["header"] += "        Pichi Bot        \n";
	help["header"] += "-------------------------\n";
	help["header"] += "--------------------------------------------\n";
	help["header"] += TR("help_syntaxis_info")  + "\n";
	help["header"] += "--------------------------------------------\n";
	
	std::map < std::string, std::vector< std::string > > helpmap;
	helpmap["commands_main"].push_back("help");
	helpmap["commands_main"].push_back("set");
	helpmap["commands_main"].push_back("gc");
	helpmap["commands_main"].push_back("log");
	helpmap["commands_main"].push_back("users");
	helpmap["commands_main"].push_back("msg");
	helpmap["commands_main"].push_back("ping");
	helpmap["commands_main"].push_back("join");
	helpmap["commands_main"].push_back("left");
	helpmap["commands_main"].push_back("greet");
	helpmap["commands_main"].push_back("farewell");
	helpmap["commands_main"].push_back("idle");
	helpmap["commands_main"].push_back("uptime");
	helpmap["commands_main"].push_back("on");
	helpmap["commands_main"].push_back("off");
	helpmap["commands_main"].push_back("version");
	helpmap["commands_admin"].push_back("topic");
	helpmap["commands_admin"].push_back("ban");
	helpmap["commands_admin"].push_back("unban");
	helpmap["commands_admin"].push_back("banlist");
	helpmap["commands_admin"].push_back("kick");
	helpmap["commands_admin"].push_back("unkick");
	helpmap["commands_admin"].push_back("kicklist");
	helpmap["commands_plugins"].push_back("plugins");
	helpmap["commands_plugins"].push_back("disable");
	helpmap["commands_plugins"].push_back("enable");
	helpmap["commands_plugins"].push_back("reload");
	helpmap["commands_wiki"].push_back("dfn");
	helpmap["commands_wiki"].push_back("wtf");
	helpmap["commands_wiki"].push_back("wtfcount");
	helpmap["commands_wiki"].push_back("wtfrand");
	helpmap["commands_wiki"].push_back("wtfset");
	helpmap["commands_wiki"].push_back("wtfull");
	helpmap["commands_wiki"].push_back("wtfrev");
	helpmap["commands_lexems"].push_back("top");
	helpmap["commands_lexems"].push_back("count");
	helpmap["commands_lexems"].push_back("talkers");
	helpmap["commands_lexems"].push_back("nick");
	helpmap["commands_lexems"].push_back("q");
	helpmap["commands_other"].push_back("urlshort");
	helpmap["commands_other"].push_back("lastfm");
	helpmap["commands_other"].push_back("lastfm_user");
	helpmap["commands_other"].push_back("translate");
	helpmap["commands_other"].push_back("translate_language");
	helpmap["commands_other"].push_back("tr");
	helpmap["commands_other"].push_back("google");
	
	help["commands_main"] = "=====  " +  TR("help_main_commands")  + "  =====\n";
	help["help"] = "!help [" + TR("help_command_usage_param") + "] - " + TR("help_command_description_help") + "\n";
	help["set"] = "!set " + TR("help_command_usage_variable") + "=" + TR("help_command_usage_value") + " - " + TR("help_command_description_set") + "\n";
	help["gc"] = "!gc [" + TR("help_command_usage_variable") + "] - " + TR("help_command_description_gc") + "\n";
	help["log"] = "!log [" + TR("help_command_usage_param") + "] [" + TR("help_command_usage_param") + "] - " + TR("help_command_description_log") + "\n";
	help["users"] = "!users [" + TR("help_command_usage_nick") + "|" + TR("help_command_usage_jid") + "|" + TR("help_command_usage_number") + "] - " + TR("help_command_description_users") + "\n";		
	help["msg"] = "!msg " + TR("help_command_usage_nick") + "|" + TR("help_command_usage_jid") + "|" + TR("help_command_usage_room") + " " + TR("help_command_usage_message") + " - " + TR("help_command_description_msg") + "\n";
	help["ping"] = "!ping " + TR("help_command_usage_nick") + "|" + TR("help_command_usage_jid") + " - " + TR("help_command_description_ping") + "\n";
	help["join"] = "!join " + TR("help_command_usage_room") + " " + TR("help_command_usage_nick") + " [" + TR("help_command_usage_status") + "] - " + TR("help_command_description_join") + "\n";
	help["left"] = "!left " + TR("help_command_usage_room") + " " + TR("help_command_usage_nick") + " [" + TR("help_command_usage_status") + "] - " + TR("help_command_description_left") + "\n";
	help["greet"] = "!greet " + TR("help_command_usage_jid") + " " + TR("help_command_usage_room") + " " + TR("help_command_usage_message") + " - " + TR("help_command_description_greet") + "\n";
	help["farewell"] = "!farewell " + TR("help_command_usage_jid") + " " + TR("help_command_usage_room") + " " + TR("help_command_usage_message") + " - " + TR("help_command_description_farewell") + "\n";
	help["idle"] = "!idle " + TR("help_command_usage_nick") + " - " + TR("help_command_description_idle") + "\n";
	help["uptime"] = "!uptime - " + TR("help_command_description_uptime") + "\n";
	help["on"] = "!on - " + TR("help_command_description_on") + "\n";
	help["off"] = "!off - " + TR("help_command_description_off") + "\n";
	help["quit"] = "!quit - " + TR("help_command_description_quit") + "\n";
	help["version"] = "!version - " + TR("help_command_description_version") + "\n";
		
	help["commands_admin"] = "=====  " +  TR("help_admin_commands")  + "  =====\n";
	help["topic"] = "!topic " + TR("help_command_usage_param") + " - " + TR("help_command_description_topic") + "\n";
	help["ban"] = "!ban " + TR("help_command_usage_jid") + "|" + TR("help_command_usage_nick") + " " + TR("help_command_usage_time") + " " + TR("help_command_usage_reason") + " - " + TR("help_command_description_ban") + "\n";
	help["unban"] = "!unban " + TR("help_command_usage_jid") + " - " + TR("help_command_description_unban") + "\n";
	help["banlist"] = "!banlist - " + TR("help_command_description_banlist") + "\n";
	help["kicklist"] = "!kicklist - " + TR("help_command_description_kicklist") + "\n";
	help["kick"] = "!kick " + TR("help_command_usage_nick") + "|" + TR("help_command_usage_jid") + " " + TR("help_command_usage_time") + " " + TR("help_command_usage_reason") + " - " + TR("help_command_description_kick") + "\n";
	help["unkick"] = "!unkick " + TR("help_command_usage_jid") + " - " + TR("help_command_description_unkick") + "\n";
	
	help["commands_plugins"] = "=====  " +  TR("help_plugins")  + "  =====\n";
	help["plugins"] = "!plugins - " + TR("help_command_description_plugins") + "\n";
	help["enable"] = "!enable " + TR("help_command_usage_param") + " - " + TR("help_command_description_enable") + "\n";
	help["disable"] = "!disable " + TR("help_command_usage_param") + " - " + TR("help_command_description_disable") + "\n";
	help["reload"] = "!reload - " + TR("help_command_description_reload") + "\n";

	help["commands_wiki"] = "=====  " +  TR("help_wiki_inline")  + "  =====\n";
	help["dfn"] = "!dfn " + TR("help_command_usage_param") + "=" + TR("help_command_usage_value") + " - " + TR("help_command_description_dfn") + "\n";
	help["wtf"] = "!wtf " + TR("help_command_usage_param") + " - " + TR("help_command_description_wtf") + "\n";
	help["wtfcount"] = "!wtfcount - " + TR("help_command_description_wtfcount") + "\n";
	help["wtfrand"] = "!wtfrand - " + TR("help_command_description_wtfrand") + "\n";
	help["wtfrev"] = "!wtfrev " + TR("help_command_usage_param") + " - " + TR("help_command_description_wtfrev") + "\n";
	help["wtfull"] = "!wtfull " + TR("help_command_usage_param") + " - " + TR("help_command_description_wtffull") + "\n";
	help["wtfset"] = "!wtfset " + TR("help_command_usage_param") + " - " + TR("help_command_description_wtfset") + "\n";

	help["commands_lexems"] = "=====  " +  TR("help_lexems")  + "  =====\n";
	help["top"] = "!top - " + TR("help_command_description_top") + "\n";
	help["count"] = "!count - " + TR("help_command_description_count") + "\n";
	help["talkers"] = "!talkers - " + TR("help_command_description_talkers") + "\n";
	help["nicks"] = "!nicks " + TR("help_command_usage_param") + " - " + TR("help_command_description_nicks") + "\n";
	help["q"] = "!q " + TR("help_command_usage_param") + " - " + TR("help_command_description_q") + "\n";

	help["commands_other"] = "=====  " +  TR("help_other_commands")  + "  =====\n";
	help["urlshort"] = "!urlshort " + TR("help_command_usage_param") + " - " + TR("help_command_description_urlshort") + "\n";
	//($hook = PichiPlugin::fetch_hook("commands_show_help")) ? eval($hook) : false;
	
	help["subgroup_lastfm"] = "--- Last.fm ---";
	help["lastfm"] = "!lastfm - " + TR("help_command_description_lastfm") + "\n";
	help["lastfm_user"] = "!lastfm_user " + TR("help_command_usage_param") + " - " + TR("help_command_description_lastfm_user") + "\n";
	
	help["subgroup_google_translate"] = "--- Google Translate ---";
	help["translate"] = "!translate " + TR("help_command_usage_param") + " - " + TR("help_command_description_translate") + "\n";
	help["tr"] = "!tr lg2lg " + TR("help_command_usage_param") + " - " + TR("help_command_description_tr") + "\n";
	help["translate_language"] = "!translate_language " + TR("help_command_usage_param") + " - " + TR("help_command_description_translate_language") + "\n";
	
	help["subgroup_google"] = "--- Google Search ---";
	help["google"] = "!google " + TR("help_command_usage_param") + " - " + TR("help_command_description_google") + "\n";
	
	if(arg != "")
	{
		boost::erase_all(arg, "!");
		if(help[arg] != "")
		{
			boost::erase_all(help[arg], "\n");
			pichi->sendAnswer(help[arg]);
		}
		else
		{
			pichi->sendAnswer(TR("command_help_nosuchcommand"));
		}
		
		return;
	}
	
	std::string helpshow = "\n" + help["header"];
	
	for(std::map < std::string, std::vector< std::string > >::iterator it = helpmap.begin(); it != helpmap.end(); it++)
	{
		helpshow += help[it->first];
		for(std::vector< std::string >::iterator itx = it->second.begin(); itx != it->second.end(); itx++)
			helpshow += help[(*itx)];
		helpshow += "\n";
	}
	
	pichi->sendAnswer(helpshow);
}

void commandbase::command_version(std::string null)
{
	//global $config;
	pichi->sendAnswer(
		static_cast<std::string>("Pichi Bot v.") + PICHI_VERSION
		+ "\n" + TR("command_version_environment") + "\n" +
		+ "SQLite version: " + SQLITE_VERSION + "\n" +
		+ "CURL version: " + curl_version() + "\n" +
		+ "Boost version: " + BOOST_LIB_VERSION
	);
	//($hook = PichiPlugin::fetch_hook("commands_show_version")) ? eval($hook) : false;
	//pichi->sendAnswer(""+TR("command_version_plugins")+":\n" + PichiPlugin::show_plugin_list());
}

void commandbase::command_enable(std::string arg)
{
	if(!pichi->isAccess())
		return;
	
	pichi->sendAnswer(TR("command_plugins_no_sorry"));
	//PichiPlugin::enable(system::atoi(arg));
}

void commandbase::command_disable(std::string arg)
{
	if(!pichi->isAccess())
		return;
	
	pichi->sendAnswer(TR("command_plugins_no_sorry"));
	//PichiPlugin::disable(system::atoi(arg));
}

void commandbase::command_reload(std::string arg)
{
	if(!pichi->isAccess())
		return;
	
	pichi->sendAnswer(TR("command_plugins_no_sorry"));
	//PichiPlugin::reload();
}
	
void commandbase::command_plugins(std::string arg)
{
	pichi->sendAnswer(TR("command_plugins_no_sorry"));
}


void commandbase::command_kick(std::string arg)
{
	if(!pichi->isAccess())
		return;
	
	std::vector< std::string > w = seperate(arg, 3);
	if(!testArgs(w, 3))
	{
		pichi->sendAnswer(TR("bad_argument"));
		return;
	}
	pichi->kick(w[0], w[1], w[2]);
}

void commandbase::command_unkick(std::string arg)
{
	if(!pichi->isAccess())
		return;
	
	pichi->unkick(arg);
}

void commandbase::command_ban(std::string arg)
{
	if(!pichi->isAccess())
		return;
	
	std::vector< std::string > w = seperate(arg, 3);
	if(!testArgs(w, 3))
	{
		pichi->sendAnswer(TR("bad_argument"));
		return;
	}
	pichi->ban(w[0], w[1], w[2]);
}

void commandbase::command_unban(std::string arg)
{
	if(!pichi->isAccess())
		return;
	
	std::vector< std::string > w = seperate(arg, 2);
	if(!testArgs(w, 2))
	{
		pichi->sendAnswer(TR("bad_argument"));
		return;
	}
	pichi->unban(w[0], w[1]);
}

void commandbase::command_banlist(std::string arg)
{
	pichi->sql->query("SELECT `jid`,`value` FROM users_data WHERE name = 'ban';");
	std::string banlist = "";
	std::map<std::string, std::string> bans;
	while(!(bans = pichi->sql->fetchArray()).empty())
	{
		banlist += bans["jid"] + " " + system::timeToString(system::atot(bans["value"]), "%d.%m.%Y в %H:%M:%S") + "\n";
	}
	pichi->sendAnswer(TR("command_banlist_lock") + ":\n" + banlist);
}

void commandbase::command_kicklist(std::string arg)
{
	pichi->sql->query("SELECT `jid`,`value` FROM users_data WHERE name = 'kick';");
	std::string kicklist = "";
	std::map<std::string, std::string> kicks;
	while(!(kicks = pichi->sql->fetchArray()).empty())
	{
		kicklist += kicks["jid"] + " " + system::timeToString(system::atot(kicks["value"]), "%d.%m.%Y в %H:%M:%S") + "\n";
	}
	pichi->sendAnswer(TR("command_kicklist_lock") + ":\n" + kicklist);
}

void commandbase::command_log(std::string arg)
{
		std::vector< std::string > w = seperate(arg, 2);
		int limit = 20, skip = 0;
		try
		{
			limit = system::atoi(w[0]);
		}
		catch(boost::bad_lexical_cast e)
		{
			pichi->sendAnswer(TR("bad_argument"));
		}
		try
		{
			skip = system::atoi(w[1]);
		} catch(boost::bad_lexical_cast e) { }
		pichi->sql->query("SELECT * FROM log ORDER BY time DESC LIMIT " + system::itoa(skip) + "," + system::itoa(limit) + ";");
		std::map< std::string, std::string > data;
		typedef std::vector< std::map< std::string, std::string > > tp;
		tp msgs;
		while(!(data = pichi->sql->fetchArray()).empty())
			msgs.push_back(data);
		
		std::string log = "\n-----------------------------------------------------------------------\n";
		BOOST_REVERSE_FOREACH(tp::value_type &ms, msgs)
		{
			    log += "[" + system::timeToString(system::atot(ms["time"]), "%H:%M:%S") + "]<" + pichi->getName(ms["from"]) + "> " + ms["message"] + "\n";
		}
		log += "-----------------------------------------------------------------------";
		pichi->sendAnswer(log); 
}


void commandbase::command_wtf(std::string arg)
{
	pichi->sql->query("SELECT `value` FROM wiki WHERE name='" + pichi->sql->escapeString(arg) + "' ORDER BY revision DESC LIMIT 0,1;");
	std::string answer = pichi->sql->fetchColumn(0);
	if(answer != "")
		pichi->sendAnswer(answer);
	else
		pichi->sendAnswer(TR("command_wiki_nodef"));
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
		list_rev += "\n------- " + TR("command_wiki_revision") + " " + tmp["revision"] + " " + "(" + tmp["name"] + ")" + " -------\n" + tmp["value"] + "\n------------------------------------------";
	if(list_rev != "")
		pichi->sendAnswer(list_rev);
	else
		pichi->sendAnswer(TR("command_wiki_nodef"));
}

void commandbase::command_wtfset(std::string arg)
{
	std::vector< std::string > w = seperate(arg, 2);
	if(!testArgs(w, 2))
	{
		pichi->sendAnswer(TR("bad_argument"));
		return;
	}
	pichi->sql->query("SELECT name,revision,value FROM wiki WHERE name = '" + pichi->sql->escapeString(w[0]) + "' AND revision='" + pichi->sql->escapeString(w[1]) + "' LIMIT 0,1;");
	if(pichi->sql->numRows() > 0)
	{
		std::string name = pichi->sql->fetchColumn(0);
		std::string rev = pichi->sql->fetchColumn(1, true);
		std::string val = pichi->sql->fetchColumn(2, true);
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
	int ct = 10;
	if(arg != "" && arg.length() <= 2)
	{
		try
		{
			ct = system::atoi(arg);
		}
		catch(boost::bad_lexical_cast e)
		{
			pichi->sendAnswer(TR("bad_argument"));
		}
	}
	pichi->sql->query("SELECT `lexeme`,`count` FROM lexems ORDER BY count DESC LIMIT 0," + system::itoa(ct) + ";");
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
		ans += system::itoa(ix) + ". " + system::implode(" ", tmp) + " [" + lex["count"] + "]" + "\n";
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
	if(!testArgs(w, 2))
	{
		pichi->sendAnswer(TR("bad_argument"));
		return;
	}
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
	if(!testArgs(w, 2))
	{
		pichi->sendAnswer(TR("bad_argument"));
		return;
	}
	if(pichi->setOption(w[0], w[1]))
		pichi->sendAnswer(TR("command_set"));
	else	
		pichi->sendAnswer(TR("command_nosuch"));
}

void commandbase::command_msg(std::string arg)
{
	if(!pichi->isAccess())
		return;
     
	std::vector< std::string > w = seperate(arg, 2);

	if(!testArgs(w, 2))
	{
		pichi->sendAnswer(TR("bad_argument"));
		return;
	}
	
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

	
void commandbase::command_users(std::string arg)
{
  	int ct = 20;
	bool individual = false;
	if(arg != "")
	{
		if(arg.substr(0,1) != "!")
		{
			try
			{
				ct = system::atoi(arg);
			}
			catch(boost::bad_lexical_cast e)
			{
				individual = true;
			}
		}
		else
		{
			arg = arg.substr(1);
			individual = true;
		}
	}
  
	std::map<std::string, std::string> data;
  
	if(!individual)
	{
		pichi->sql->query("SELECT COUNT(*) FROM users WHERE status = 'available' AND room != '';");
		int avl = system::atoi(pichi->sql->fetchColumn(0));
		pichi->sql->query("SELECT COUNT(*) FROM users WHERE status = 'unavailable' AND room != '';");
		int navl = system::atoi(pichi->sql->fetchColumn(0));
	
		pichi->sql->query("SELECT * FROM users LIMIT 0," + system::itoa(ct) + ";");
		std::string userlist, online, offline;

		//$this->log->log("Begin creting user list", PichiLog::LEVEL_DEBUG);
		userlist = "" + TR("command_users_list_seen") + ":\n";
		int n=0, f=0;
		while(!(data = pichi->sql->fetchArray()).empty())
		{
			if(data["room"] == "")
				continue;
			std::string roomname = system::explode("@", data["room"]).at(0);
			if(data["status"] == "available")
			{
				n++;
				online += TR4("command_users_online_seen", system::itoa(n).c_str(), data["nick"].c_str(), roomname.c_str()) + "\n";
				//$this->log->log("User $data[nick]: online", PichiLog::LEVEL_VERBOSE);
			}
			else
			{
				f++;
				offline += TR5("command_users_offline_seen", system::itoa(f).c_str(), data["nick"].c_str(), system::timeToString(boost::lexical_cast<time_t>(data["time"]), "%d.%m.%Y в %H:%M:%S").c_str(), roomname.c_str()) + "\n";
				//$this->log->log("User $data[nick]: offline", PichiLog::LEVEL_VERBOSE);
			}
		}
		online += TR3("command_users_limitshow", system::itoa(n).c_str(), system::itoa(avl).c_str()) + "\n";
		offline += TR3("command_users_limitshow", system::itoa(f).c_str(), system::itoa(navl).c_str()) + "\n";
		userlist += "" + TR("command_users_online") + ":\n" + online;
		userlist += "" + TR("command_users_offline") + ":\n" + offline;
		pichi->sendAnswer(userlist);
	}
	else
	{
		pichi->sql->query("SELECT * FROM users WHERE (nick = '" + pichi->sql->escapeString(arg) + "' OR jid = '" + pichi->sql->escapeString(arg) + "') AND room != '';");
		if(pichi->sql->numRows() > 0)
		{
			data = pichi->sql->fetchArray();
			pichi->sendAnswer(TR3("command_status", data["nick"].c_str(), ((data["status"] == "available") ? TR("command_status_online").c_str() : TR("command_status_offline").c_str())));
		}
		else
			pichi->sendAnswer(TR2("command_users_nouser", arg.c_str()));
	}
}


void commandbase::command_join(std::string arg)
{
	if(!pichi->isAccess())
		return;
	pichi->jabber->enterRoom(JID(arg));
}

void commandbase::command_left(std::string arg)
{
	if(!pichi->isAccess())
		return;
	pichi->jabber->leftRoom(JID(arg));
}

void commandbase::command_ping(std::string arg)
{
	if(pichi->isOnline(arg))
		pichi->ping(pichi->getJID(pichi->getName(arg)));
	else
		pichi->sendAnswer(TR("command_ping_nouser"));
}

void commandbase::command_topic(std::string arg)
{
	for(std::list< std::pair<JID, MUCRoom*> >::iterator it = pichi->jabber->rooms.begin(); it != pichi->jabber->rooms.end(); it++)
		if(it->first == JID(pichi->getLastRoom()))
			it->second->setSubject(arg);
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
	time_t date;
	try
	{
		date = boost::lexical_cast<time_t>(pichi->sql->fetchColumn(0));
	}
	catch(boost::bad_lexical_cast e)
	{
		pichi->sendAnswer(TR("bad_argument"));
		return;
	}
	if(date > 0)
		pichi->sendAnswer(system::timeToString(date, "%d.%m.%Y в %H:%M:%S"));
	else
		pichi->sendAnswer(TR("bad_argument"));
}

void commandbase::command_q(std::string arg)
{
	pichi->sendAnswer(pichi->lex->genFromWord(arg));
}

void commandbase::command_greet(std::string arg)
{
	std::vector< std::string > w = seperate(arg, 3);
	if(!pichi->isAccess())
		return;
	if(!testArgs(w, 3))
	{
		pichi->sendAnswer(TR("bad_argument"));
		return;
	}
	pichi->sql->query("SELECT COUNT(*) FROM actions WHERE action = 'user_join_room' AND coincidence='room=" + pichi->sql->escapeString(w[1]) + ",jid=" + pichi->sql->escapeString(w[0]) + "';");
	if(system::atoi(pichi->sql->fetchColumn(0)) > 0)
		pichi->sql->exec("UPDATE actions SET value = '" + pichi->sql->escapeString(w[2]) + "'  WHERE action = 'user_join_room' AND coincidence='room=" + pichi->sql->escapeString(w[1]) + ",jid=" + pichi->sql->escapeString(w[0]) + "';");
	else
		pichi->sql->exec("INSERT INTO actions (`action`,`coincidence`,`do`,`option`,`value`) VALUES ('user_join_room', 'room=" + pichi->sql->escapeString(w[1]) + ",jid=" + pichi->sql->escapeString(w[0]) + "', 'send_message', '', '" + pichi->sql->escapeString(w[2]) + "');");
	pichi->sendAnswer(TR("updated"));
}

void commandbase::command_farewell(std::string arg)
{
	std::vector< std::string > w = seperate(arg, 3);
	if(!pichi->isAccess())
		return;
	if(!testArgs(w, 3))
	{
		pichi->sendAnswer(TR("bad_argument"));
		return;
	}
	pichi->sql->query("SELECT COUNT(*) FROM actions WHERE action = 'user_left_room' AND coincidence='room=" + pichi->sql->escapeString(w[1]) + ",jid=" + pichi->sql->escapeString(w[0]) + "';");
	if(system::atoi(pichi->sql->fetchColumn(0)) > 0)
		pichi->sql->exec("UPDATE actions SET value = '" + pichi->sql->escapeString(w[2]) + "'  WHERE action = 'user_left_room' AND coincidence='room=" + pichi->sql->escapeString(w[1]) + ",jid=" + pichi->sql->escapeString(w[0]) + "';");
	else
		pichi->sql->exec("INSERT INTO actions (`action`,`coincidence`,`do`,`option`,`value`) VALUES ('user_left_room', 'room=" + pichi->sql->escapeString(w[1]) + ",jid=" + pichi->sql->escapeString(w[0]) + "', 'send_message', '', '" + pichi->sql->escapeString(w[2]) + "');");
	pichi->sendAnswer(TR("updated"));
}

void commandbase::command_quit(std::string arg)
{
	if(!pichi->isAccess(3))
		return;
	pichi->jabber->client->disconnect(); // call destructor
}
	
void commandbase::command_on(std::string arg)
{
	if(!pichi->isAccess(3))
		return;
	pichi->on();
}
	
void commandbase::command_off(std::string arg)
{
	if(!pichi->isAccess(3))
		return;
	pichi->off();
}

// ---------------------------------------------------------------


void commandbase::command_lastfm(std::string arg)
{
	updateThreadVars();
	if(pthread_create(&remotethread, NULL, &commandbase::thread_lastfm, (void*)this) > 0)
		throw PichiException("Error in last.fm thread");
}

void* commandbase::thread_lastfm(void* context)
{
	lastmessage last = ((commandbase*)context)->thread_args["lastfm"];
	std::map<std::string, std::string> user = ((commandbase*)context)->pichi->getJIDinfo( last.getJIDlast() , "lastfm_user" );
	if(user["lastfm_user"] != "")
	{
		pichicurl* curl = new pichicurl();
		std::string data = curl->readurl("http://ws.audioscrobbler.com/1.0/user/" + user["lastfm_user"] + "/recenttracks.txt");
		if(data != "")
			((commandbase*)context)->pichi->sendAnswer( (*((commandbase*)context)->pichi->lang)("command_lastfm_listen", ((commandbase*)context)->pichi->getName( last.getJIDlast() ).c_str(), (system::explode("," , (system::explode("\n", data).at(0)))).at(1).c_str()), last );
		else
			LOG("Music read from last.fm failed. Check internet connection.", LOG::WARNING);
		delete curl;
	}
	pthread_exit(context);
}


void commandbase::command_lastfm_user(std::string arg)
{
	pichi->setJIDinfo(pichi->getJIDlast(), "lastfm_user", arg);
	pichi->sendAnswer(TR2("command_lastfm_setuser", arg.c_str()));
}


void* commandbase::thread_googletranslate(void* context)
{
	lastmessage last = ((commandbase*)context)->thread_args["googletranslate"];
  
	std::string text;
	std::string from;
	std::string to;
	std::string server = "http://google.com";
  
	if( ((commandbase*)context)->last_command == "translate" )
	{
		std::map<std::string, std::string> t_from = ((commandbase*)context)->pichi->getJIDinfo(last.getJIDlast(), "translate_from");
		std::map<std::string, std::string> t_to = ((commandbase*)context)->pichi->getJIDinfo(last.getJIDlast(), "translate_to");
		
		text = last.getArg();
		from = t_from["translate_from"];
		to = t_to["translate_to"];
	}
	else if(((commandbase*)context)->last_command == "tr")
	{
		std::vector< std::string > w = ((commandbase*)context)->seperate(last.getArg(), 2);
		if(!((commandbase*)context)->testArgs(w, 2))
		{
			((commandbase*)context)->pichi->sendAnswer( (*((commandbase*)context)->pichi->lang)("bad_argument"), last );
			pthread_exit(context);
		}
		std::vector< std::string > langs = system::explode("2", w[0]);
		if(!((commandbase*)context)->testArgs(langs, 2))
		{
			((commandbase*)context)->pichi->sendAnswer( (*((commandbase*)context)->pichi->lang)("bad_argument"), last );
			pthread_exit(context);
		}
		text = w[1];
		from = langs[0];
		to = langs[1];
	}
	else
	{
		LOG("Слишком медленный асинхронный вызов, невозможно определить тип функции. Выход.", LOG::WARNING);
		pthread_exit(context);
	}
  
	if(text == "" || from == "" || to == "" || server == "")
		pthread_exit(context);
  
	pichicurl* curl = new pichicurl();
	curl->setUrl("http://ajax.googleapis.com/ajax/services/language/translate?v=1.0&q=" + curl->urlencode(text) + "&langpair=" + curl->urlencode(from + "|" + to));
	curl->setReferer(server);
	std::string ret = curl->read();
	delete curl;
	
	if(ret == "" || ret.substr(0,1) != "{")
	{
		LOG("Google translate can failed.", LOG::DEBUG);
		pthread_exit(context);
	}
	
	boost::property_tree::ptree ptree;
	std::stringstream stream(ret);
	boost::property_tree::json_parser::read_json(stream, ptree);

	if(ptree.get("responseStatus", "") == "200")
		((commandbase*)context)->pichi->sendAnswer( ptree.get_child("responseData").get("translatedText", ""), last );
	
	pthread_exit(context);
}


void commandbase::command_translate(std::string arg)
{
  	updateThreadVars("googletranslate");
	if(pthread_create(&remotethread, NULL, &commandbase::thread_googletranslate, (void*)this) > 0)
		throw PichiException("Error in !translate thread");
}

void commandbase::command_tr(std::string arg)
{
  	updateThreadVars("googletranslate");
	if(pthread_create(&remotethread, NULL, &commandbase::thread_googletranslate, (void*)this) > 0)
		throw PichiException("Error in !tr thread");
}

void commandbase::command_translate_language(std::string arg)
{
	std::vector< std::string > w = seperate(arg, 2);
	if(!testArgs(w, 2))
	{
		pichi->sendAnswer(TR("bad_argument"));
		return;
	}
	std::string jid = pichi->getJIDlast();
	pichi->setJIDinfo(jid, "translate_from", w[0]);
	pichi->setJIDinfo(jid, "translate_to", w[1]);
}

void commandbase::command_google(std::string arg)
{
    	updateThreadVars();
	if(pthread_create(&remotethread, NULL, &commandbase::thread_google, (void*)this) > 0)
		throw PichiException("Error in !google thread");
}

void* commandbase::thread_google(void* context)
{
	lastmessage last = ((commandbase*)context)->thread_args["google"];
  
	pichicurl* curl = new pichicurl();
	curl->setUrl( "http://ajax.googleapis.com/ajax/services/search/web?v=1.0&q=" + curl->urlencode( last.getArg() ) );
	curl->setReferer("http://google.com");
	std::string ret = curl->read();
	delete curl;
	
	if(ret == "" || ret.substr(0,1) != "{")
	{
		LOG("Google search can failed.", LOG::DEBUG);
		pthread_exit(context);
	}
	
	boost::property_tree::ptree ptree, result;
	std::stringstream stream(ret);
	boost::property_tree::json_parser::read_json(stream, ptree);

	std::string ans = "";
	
	if(ptree.get("responseStatus", "") == "200")
	{
		result = ptree.get_child("responseData").get_child("results");
		BOOST_FOREACH(boost::property_tree::ptree::value_type &ptree, result)
		{
		      ans += "\n" + ptree.second.get("url","") + " - " 
			+ ptree.second.get("titleNoFormatting","") + "\n" 
			+ "(" + ptree.second.get("content","") + ")\n";
		}
		((commandbase*)context)->pichi->sendAnswer( ans, last );
	}
	pthread_exit(context);
}


void commandbase::command_urlshort(std::string arg)
{
  	updateThreadVars();
	if(pthread_create(&remotethread, NULL, &commandbase::thread_urlshort, (void*)this) > 0)
		throw PichiException("Error in !urlshort thread");
}

void* commandbase::thread_urlshort(void* context)
{
	lastmessage last = ((commandbase*)context)->thread_args["urlshort"];
  
	pichicurl* curl = new pichicurl();
	curl->setUrl("http://ur.ly/new.json?href=" + curl->urlencode( last.getArg() ) );
	std::string ret = curl->read();
	delete curl;
	
	if(ret == "" || ret.substr(0,1) != "{")
	{
		LOG("UL.ly failed.", LOG::DEBUG);
		((commandbase*)context)->pichi->sendAnswer((*((commandbase*)context)->pichi->lang)("command_urlshort_incorrect_service"), last);
		pthread_exit(context);
	}
	
	boost::property_tree::ptree ptree;
	std::stringstream stream(ret);
	boost::property_tree::json_parser::read_json(stream, ptree);
	
	((commandbase*)context)->pichi->sendAnswer("http://ur.ly/" + ptree.get("code",""), last);
	pthread_exit(context);
}


void commandbase::command_uptime(std::string arg)
{
	time_t diff = time(NULL) - pichi->jabber->times["start"];
	std::vector<time_t> undiff;
	int ar[6] = {60,60,24,30,12,5000};
	std::vector<std::string> whatis;
	whatis.push_back(TR("time_n_seconds"));
	whatis.push_back(TR("time_n_minutes"));
	whatis.push_back(TR("time_n_hours"));
	whatis.push_back(TR("time_n_days"));
	whatis.push_back(TR("time_n_months"));
	whatis.push_back(TR("time_n_years"));
	int i = 0;
	while(diff > ar[i])
	{
		undiff.push_back(diff % ar[i]);
		diff /= ar[i];
		i++;
	}
	std::string ans = system::ttoa(diff) + " " + whatis[i];
	for(; i>0; i--)
		ans += " " + system::ttoa(undiff[i-1]) + " " + whatis[i-1];
	pichi->sendAnswer("\n" + TR2("command_uptime_start", system::timeToString(pichi->jabber->times["start"],"%d.%m.%Y в %H:%M:%S").c_str()) 
			+ "\n" + TR2("command_uptime_uptime", ans.c_str()));
}

}