/*
    <Pichi jabber bot>
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

#include "pichicore.h"
#include "core.h"

pichicore::pichicore()
{
	enabled = true;
	wait_time = system::atoi(config["wait_time"]);
	sql_options = & sql;
	// init commander
	commander = new commandbase(this);
	//translater
	lang = new languages(config["language"]);
	lex = new lexemes(&sql);
	event = new PichiEvent(this);
	
	//timers
	crons["bans"]["last"] = time(NULL);
	crons["bans"]["interval"] = 5;
}

pichicore::~pichicore()
{
	delete commander;
	delete sql;
	delete lang;
	delete lex;
	delete event;
}

bool pichicore::isEnabled(void)
{
	return enabled;
}

void pichicore::off(void )
{
	enabled = false;
}

void pichicore::on(void )
{
	enabled = true;
}

void pichicore::setUserInfo(std::string jid, std::string nick, std::string state, std::string room, std::string role, std::string status)
{
	int level = 1; // defaul access level
	
	sql->query("SELECT `status` FROM users WHERE jid = '" + sql->escapeString(jid) + "' AND room = '" + sql->escapeString(room) + "';");
	std::string old_state = sql->fetchColumn(0);
	
	if(config["global_admins"] == "1")
		if(role == "moderator")
			level = 2;

	std::vector< std::string > admins, ignore;
	
	admins = system::explode(",", config["admins"]);
	if(admins.size() > 0)
		if(system::in_array(jid, admins))
			level = 3;
	
	ignore = system::explode(",", config["ignore"]);
	if(ignore.size() > 0)
		if(system::in_array(jid, ignore))
			level = 0;

	if(time(NULL) - wait > wait_time)
	{
		if(state == "available" && old_state == "unavailable")
		{
			event->callEvent("user_join_room", "room=" + room + ",jid=" + jid);
			// autokick
				sql->query("SELECT COUNT(*) FROM users_data WHERE jid = '" + sql->escapeString(jid) + "' AND name = 'kick' AND groupid = '" + sql->escapeString(room) + "';");
				if(system::atoi(sql->fetchColumn(0)) > 0)
					jabber->kick(getName(getJID(jid, room), room), JID(room), "Auto-kick");
			}
		else if(state == "unavailable" && old_state == "available")
		{
			event->callEvent("user_left_room", "room=" + room + ",jid=" + jid);
		}
	}
	
	//($hook = PichiPlugin::fetch_hook('pichicore_status_set')) ? eval($hook) : false;
	LOG("Updating user " + nick + "(" + jid + ")[" + status + "] in " + room, LOG::DEBUG);
	
	sql->query("SELECT COUNT(*) FROM users WHERE jid = '" + sql->escapeString(jid) + "' AND room = '" + sql->escapeString(room) + "';");
	if(system::atoi(sql->fetchColumn(0)) > 0)
	{
		sql->exec("UPDATE users SET nick = '" + sql->escapeString(nick) + "', time = '" + system::stringTime(time(NULL)) + "', status = '" + state + "', role = '" + sql->escapeString(role) + "', level = '" + system::itoa(level) + "' WHERE jid = '" + sql->escapeString(jid) + "' AND room = '" + sql->escapeString(room) + "';");
		//($hook = PichiPlugin::fetch_hook('pichicore_status_update')) ? eval($hook) : false;
	}
	else
	{
		sql->exec("INSERT INTO users (`jid`,`nick`,`role`,`room`,`time`,`status`,`level`) VALUES ('" + sql->escapeString(jid) + "','" + sql->escapeString(nick) + "','" + sql->escapeString(role) + "','" + sql->escapeString(room) + "','" + system::stringTime(time(NULL)) + "','" + state + "', '" + system::itoa(level) + "');");
		sql->query("SELECT COUNT(*) FROM users WHERE jid = '" + sql->escapeString(jid) + "';");
		//if($this->db->fetchColumn() == 0)
		//		($hook = PichiPlugin::fetch_hook('pichicore_status_create')) ? eval($hook) : false;
	}
		
		sql->query("SELECT COUNT(*) FROM users_nick WHERE jid = '" + sql->escapeString(jid) + "' AND nick = '" + sql->escapeString(nick) + "' AND room = '" + sql->escapeString(room) + "';");
		if(system::atoi(sql->fetchColumn(0)) == 0)
			sql->exec("INSERT INTO users_nick (`jid`,`nick`,`room`,`time`) VALUES ('" + sql->escapeString(jid) + "','" + sql->escapeString(nick) + "','" + sql->escapeString(room) + "','" + system::stringTime(time(NULL)) + "');");
}

void pichicore::cleanUserInfo(void)
{
	sql->exec("UPDATE users SET status = 'unavailable';");
}

bool pichicore::isJID(const std::string& jid)
{
	return (std::find(jid.begin(), jid.end(), '@') != jid.end());
}


std::string pichicore::getJID(const std::string& nick, std::string room, bool full_search)
{
	LOG("Get JID from " + nick, LOG::VERBOSE);
	if(isJID(nick))
	{
		std::vector< std::string > exp = system::explode("/", nick);
		return exp[0];
	}
		
	if(room == std::string())
		room = getDefaultRoom(); // main room
	
	sql->query("SELECT `jid` FROM users WHERE nick = '" + sql->escapeString(nick) + "' AND room = '" + sql->escapeString(room) + "';");
	std::string jid = sql->fetchColumn(0);
	
	if(jid != std::string())
	{
		return jid;
	}
	else
	{
		if(full_search)
		{
			sql->query("SELECT `jid` FROM users_nick WHERE nick = '" + sql->escapeString(nick) + "' AND room = '" + sql->escapeString(room) + "' ORDER BY `time` ASC;");
			jid = sql->fetchColumn(0);
			if(jid != std::string())
				return jid;
		}
		return std::string();
	}
}

std::string pichicore::getDefaultRoom(void)
{
	std::list< std::pair<JID, MUCRoom*> >::iterator first_room;
	first_room = jabber->rooms.begin();
	return (*first_room).first.bare();
}

std::string pichicore::getName(const std::string& jid, std::string room)
{
	LOG("Get Nick from JID " + jid, LOG::VERBOSE);
	if(!isJID(jid))
		return jid;
	
	std::vector< std::string > exp = system::explode("/", jid);
	if(exp.size() != 2)
	{
		if(room == std::string())
			room = getDefaultRoom(); // main room
		sql->query("SELECT `nick` FROM users WHERE jid = '" + sql->escapeString(jid) + "' AND room = '" + sql->escapeString(room) + "';");
		return (sql->fetchColumn(0));
	}
	else
	{
		return exp[1];
	}
}

bool pichicore::isAccess(int level, std::string jid, std::string room, bool room_hook)
{
	if(jid == "")
		jid = last_jid;
	
	if(jid == "")
		return false;
	
	if(room == std::string() && !room_hook)
		room = getDefaultRoom(); // main room
	
	jid = getJID(getName(jid, room), room); // current access
	sql->query("SELECT `level` FROM users WHERE jid = '" + sql->escapeString(jid) + "' AND room = '" + sql->escapeString(room) + "';");
	std::string tempresult = sql->fetchColumn(0);
	if(tempresult == "")
		return false;
	int dblevel = system::atoi(tempresult);
	
	LOG("Test access to " + jid + ": " + tempresult + " >= " + system::itoa(level), LOG::VERBOSE);
	
	if(dblevel >= level)
		return true;
	else
		return false;
}

bool pichicore::reciveMessage(std::string message, std::string type, std::string from, std::string jid, std::string room, int level)
{
	if(time(NULL) - wait < wait_time)
	{
		LOG("Ignore Message", LOG::DEBUG);
		return false;
	}
  
	if(message == "" || from == "" || type == "")
		return false;
	
	last_message = message;
	last_from = from;
	last_type = type;
	
	if(room == "")
		if(last_type == "groupchat")
			last_room = getJID(last_from);
		else
			last_room = "";
	else
		last_room = room;
	
	if(jid == "")
		if(last_type == "groupchat")
			last_jid = getJID(getName(last_from), last_room);
		else
			last_jid = getJID(last_from);
	else
		last_jid = jid;
		
	if(!isAccess(1, last_jid, last_room, true))
	{
		LOG("Acess denied", LOG::DEBUG);
		return false;
	}
	
	//($hook = PichiPlugin::fetch_hook('pichicore_message_recive_begin')) ? eval($hook) : false;
	//$this->log->log("Call message method", PichiLog::LEVEL_DEBUG);
	
	if(enabled && !isCommand(last_message) && options["log_enabled"] == "1")
		sql->exec("INSERT INTO log (`from`,`time`,`type`,`message`) VALUES ('" + sql->escapeString(last_from) + "','" + sql->escapeString(system::stringTime(time(NULL))) + "','" + sql->escapeString(last_type) + "','" + sql->escapeString(last_message) + "');");
	
	//ME breaker
	if(last_jid == jabber->getMyJID().bare())
		return true;
	
	//to lexems massges
	if(enabled && !isCommand(last_message) && options["answer_remember"] == "1")
		lex->parseText(last_message);
		
	//($hook = PichiPlugin::fetch_hook('pichicore_message_recive_complete')) ? eval($hook) : false;
		
	if(isCommand(last_message))
		commander->fetchCommand(last_message);
	
	if(enabled && !isCommand(last_message) && options["answer_mode"] == "1")
	{
		if(options["answer_random"] == "0" || (1 + rand() % system::atoi(options["answer_random"])) == 1)
		{
		  /*
			$this->syntax->generate();
			if(rand(1, (int)$this->options['treatment_coincidence']) === 1 && $this->options['treatment_coincidence'] > 0)
			{
				switch(rand(1,2))
				{
					case 1:
						$this->sendAnswer($this->getName($this->last_from) . ": " . $this->syntax->returnText());
						break;
					case 2:
						$this->sendAnswer($this->getName($this->last_from) . ", " . $this->syntax->returnText());
						break;		
				}
			}
			else
			{
				$this->sendAnswer($this->syntax->returnText());
			}
		    */
		    sendAnswer(lex->genFullRandom());
		}
	}
		
	//($hook = PichiPlugin::fetch_hook('commands_message_recive_complete')) ? eval($hook) : false;
	
	return true;
}

void pichicore::sendAnswer(std::string message)
{
	std::string to;
	if(last_type == "groupchat")
		to = last_room;
	else
		to = last_from;
	
	if(message.size() > system::atoi(options["msg_limit"]) && system::atoi(options["msg_limit"]) > 1 && last_type == "groupchat")
	{
		jabber->sendMessage(JID(last_room), (*lang)("message_to_private_chat"));
		to = last_jid;
	}
	//($hook = PichiPlugin::fetch_hook('pichicore_answer_send')) ? eval($hook) : false;
	
	if(to == "")
		return;
	
	//if(system::atot(options["msg_max_limit"]) > 0)
	//	foreach(self::str_split_unicode($message, (int)$this->options['msg_max_limit']) as $msg)
	//		$this->jabber->message($to, $msg, $type);
	//else
		jabber->sendMessage(JID(to), message);
	
	//$this->log->log("Send answer to $to:\n$message", PichiLog::LEVEL_VERBOSE);
}

bool pichicore::isCommand(std::string& str)
{
	return (str.substr(0,1) == "!");
}

bool pichicore::setOption(std::string option, std::string value)
{

	sql->query("SELECT COUNT(*) FROM settings WHERE name = '" + sql->escapeString(option) + "';");
	if(system::atoi(sql->fetchColumn(0)) > 0)
	{
		setSqlOption(option, value);
		LOG("Updated option '" + option + "' = " + value, LOG::DEBUG);
		return true;
	}
	else
	{
		LOG("Can't set '" + option + "'. There is no such option.", LOG::DEBUG);
		return false;
	}
	
    return true; //stub
}

bool pichicore::isOnline(std::string user, std::string room)
{
	sql->query("SELECT `jid`,`nick` FROM users WHERE status = 'available'" + ((room != "") ? " AND room = '" + sql->escapeString(room) + "'" : "") + ";");
	std::map<std::string, std::string> users;
	while(!(users = sql->fetchArray()).empty())
	{
		if(users["nick"] == user || users["jid"] == user)
			return true;
	}
	return false;
}

void pichicore::ping(std::string jid)
{
	//timer = boost::timer();
	//reciver["ping_" + jid] = boost::lexical_cast<std::string>(clock());
	jabber->client->xmppPing(JID(jid), jabber);  	
}

void pichicore::pingRecive(std::string jid)
{
	//sendAnswer(boost::lexical_cast<std::string>(timer.elapsed()));
	//sendAnswer( boost::lexical_cast<std::string>( ( clock() - (boost::lexical_cast<clock_t>(reciver["ping_" + jid])) ) / CLOCKS_PER_SEC ) );
	//sendAnswer( boost::lexical_cast<std::string>( std::difftime(clock(), boost::lexical_cast<clock_t>(reciver["ping_" + jid])) ) );
	//reciver.erase("ping_" + jid);
	sendAnswer("Понг. А время считай сам.");
}

// устанавливает информацию о jid
void pichicore::setJIDinfo(std::string jid, std::string name, std::string value, std::string groupid, sqlite* db)
{
    	if(db == NULL)
		db = sql;
	db->query("SELECT COUNT(*) FROM users_data WHERE jid = '" + db->escapeString(jid) + "' AND name = '" + db->escapeString(name) + "'" + ((groupid != "") ? " AND groupid = '" + db->escapeString(groupid) + "'" : "") + ";");
	if(system::atoi(db->fetchColumn(0)) > 0)
		db->exec("UPDATE users_data SET value = '" + db->escapeString(value) + "'  WHERE jid = '" + db->escapeString(jid) + "' AND name = '" + db->escapeString(name) + "'" + ((groupid != "") ? " AND groupid = '" + db->escapeString(groupid) + "'" : "") + ";");
	else
		db->exec("INSERT INTO users_data (`jid`,`name`,`value`,`groupid`) VALUES ('" + db->escapeString(jid) + "','" + db->escapeString(name) + "','" + db->escapeString(value) + "','" + ((groupid != "") ? db->escapeString(groupid) : "") + "');");
}

// а теперь получить инфу
std::map<std::string, std::string> pichicore::getJIDinfo(std::string jid, std::string name, std::string groupid, sqlite* db)
{
    	if(db == NULL)
		db = sql;
	std::map<std::string, std::string> retmap, data;
	db->query("SELECT * FROM users_data WHERE jid = '" + db->escapeString(jid) + "'" + ((name != "") ? " AND name = '" + db->escapeString(name) + "'" : "") + ((groupid != "") ? " AND groupid = '" + db->escapeString(groupid) + "'" : "") + ";");
	while(!(data = db->fetchArray()).empty())
		retmap[data["name"]] = data["value"];
	return retmap;
}

// ну и удалить
void pichicore::delJIDinfo(std::string jid, std::string name, std::string groupid, sqlite* db)
{
    	if(db == NULL)
		db = sql;
	db->exec("DELETE FROM users_data WHERE jid = '" + db->escapeString(jid) + "'" + ((name != "") ? " AND name = '" + db->escapeString(name) + "'" : "") + ((groupid != "") ? " AND groupid = '" + db->escapeString(groupid) + "'" : "") + ";");
}

std::string pichicore::getJIDlast(void )
{
	return last_jid;
}

void pichicore::ban(std::string jid, std::string time, std::string reason, std::string room)
{
	if(room == "")
		room = getDefaultRoom(); // main room
	jabber->ban((jid = getJID(jid, room)), JID(room), reason);
	if(time != "")
	{
		time_t tm = convertTime(time);
		setJIDinfo(jid, "ban", system::stringTime(tm + ::time(NULL)), room);
		setJIDinfo(jid, "ban_reason", reason, room);
		setJIDinfo(jid, "ban_room", room, room);
	}
}
        
void pichicore::unban(std::string jid, std::string reason, std::string room, sqlite* db)
{
	if(db == NULL)
		db = sql;
	if(room == "")
		room = getDefaultRoom(); // main room
	jid = getJID(jid, room);
	jabber->unban(jid, JID(room), reason);
	delJIDinfo(jid, "ban", room, db);
	delJIDinfo(jid, "ban_reason", room, db);
	delJIDinfo(jid, "ban_room", room, db);
}
        
void pichicore::kick(std::string jid, std::string time, std::string reason, std::string room)
{
	if(room == "")
		room = getDefaultRoom(); // main room
	jabber->kick(getName(jid = getJID(jid, room), room), JID(room), reason);
	if(time != "")
	{
		time_t tm = convertTime(time);
		setJIDinfo(jid, "kick", system::stringTime(tm + ::time(NULL)), room);
		setJIDinfo(jid, "kick_reason", reason, room);
		setJIDinfo(jid, "kick_room", room, room);
	}
}
        
void pichicore::unkick(std::string jid, std::string room, sqlite* db)
{
  	if(db == NULL)
		db = sql;
	if(room == "")
		room = getDefaultRoom(); // main room
	jid = getJID(jid, room);
	delJIDinfo(jid, "kick", room, db);
	delJIDinfo(jid, "kick_reason", room, db);
	delJIDinfo(jid, "kick_room", room, db);
}

time_t pichicore::convertTime(std::string time)
{
	if(time == "")
		return 0;
        
	time_t realtime;
	
	switch(boost::lexical_cast<char>(time.substr(time.length()-1).c_str()))
	{
		case 'm':
			realtime = system::atot(time.substr(0,time.length()-1));
			return realtime * 60;
			break;
		case 'h':
			realtime = system::atot(time.substr(0,time.length()-1));
			return realtime * 60 * 60;
			break;
		case 'd':
			realtime = system::atot(time.substr(0,time.length()-1));
			return realtime * 60 * 60 * 24;
			break;
		case 'w':
			realtime = system::atot(time.substr(0,time.length()-1));
			return realtime * 60 * 60 * 24 * 7;
			break;
		case 'M':
			realtime = system::atot(time.substr(0,time.length()-1));
			return realtime * 60 * 60 * 24 * 30;
			break;
		case 'Y':
			realtime = system::atot(time.substr(0,time.length()-1));
			return realtime * 60 * 60 * 24 * 30 * 12;
				break;
		default:
			return system::atot(time);
			break;
	}
}

// Действие по крону
void pichicore::cronDo(std::string eventer)
{
    if(canCron("bans"))
    {
	LOG("[CRON] Bans", LOG::INFO);
	//Check bans and kicks
	cronsql->query("SELECT `jid`,`value`,`name`,`groupid` FROM users_data WHERE name = 'ban' OR name = 'kick';");
	std::map<std::string, std::string> bans;
	while(!(bans = cronsql->fetchArray()).empty())
	{
		if(system::atot(bans["value"]) <= time(NULL))
		{
			if(bans["name"] == "ban")
				unban(bans["jid"], "Auto-unban", bans["groupid"], cronsql);
			else if(bans["name"] == "kick")
				unkick(bans["jid"], bans["groupid"], cronsql);
		}
	}
	
	crons["bans"]["last"] = time(NULL); 
    }
}

bool pichicore::canCron(std::string crn)
{
	return (time(NULL) - crons[crn]["last"] > crons[crn]["interval"]);
}

