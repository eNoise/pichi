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

#include "pichicore.h"
#include "core.h"

namespace pichi
{

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

	if(time(NULL) - jabber->times["wait"] > wait_time)
	{
		if(state == "available" && old_state == "unavailable")
		{
			event->callEvent("user_join_room", "room=" + room + ",jid=" + jid);
			// autokick
				sql->query("SELECT COUNT(*) FROM users_data WHERE jid = '" + sql->escapeString(jid) + "' AND name = 'kick' AND groupid = '" + sql->escapeString(room) + "';");
				if(system::atoi(sql->fetchColumn(0)) > 0)
					jabber->kick(jid, JID(room), "Auto-kick");
			}
		else if(state == "unavailable" && old_state == "available")
		{
			event->callEvent("user_left_room", "room=" + room + ",jid=" + jid);
		}
	}
	
	//($hook = PichiPlugin::fetch_hook('pichicore_status_set')) ? eval($hook) : false;
	if(room != "")
		if(state == "unavailable")
			LOG("[PRESENCE_ROOM][OFF] " + jid + "{" + system::itoa(level) + "} (" + nick + ")[" + status + "] in " + room, LOG::DEBUG);
		else
			LOG("[PRESENCE_ROOM][ON] " + jid + "{" + system::itoa(level) + "} (" + nick + ")[" + status + "] in " + room, LOG::DEBUG);
	else
		if(state == "unavailable")
			LOG("[PRESENCE][OFF] " + jid + "{" + system::itoa(level) + "} [" + status + "]", LOG::DEBUG);
		else
			LOG("[PRESENCE][ON] " + jid + "{" + system::itoa(level) + "} [" + status + "]", LOG::DEBUG);
	
		
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
	
	// Insert Nick info
	if(nick != "")
	{
		sql->query("SELECT COUNT(*) FROM users_nick WHERE jid = '" + sql->escapeString(jid) + "' AND nick = '" + sql->escapeString(nick) + "' AND room = '" + sql->escapeString(room) + "';");
		if(system::atoi(sql->fetchColumn(0)) == 0)
			sql->exec("INSERT INTO users_nick (`jid`,`nick`,`room`,`time`) VALUES ('" + sql->escapeString(jid) + "','" + sql->escapeString(nick) + "','" + sql->escapeString(room) + "','" + system::stringTime(time(NULL)) + "');");
		
		// push to lexems nicks
		lex->addNick(nick);
	}
}

void pichicore::cleanUserInfo(void)
{
	sql->exec("UPDATE users SET status = 'unavailable';");
}

bool pichicore::isJID(const std::string& jid)
{
	return (std::find(jid.begin(), jid.end(), '@') != jid.end());
}

bool pichicore::isBareJID(const std::string& jid)
{
	return ( isJID(jid) && (std::find(jid.begin(), jid.end(), '/') == jid.end()) );
}


std::string pichicore::getJIDpart(const std::string& jid, unsigned int part)
{
	std::vector< std::string > exp = system::explode("/", jid);
	part--;
	if(part < exp.size())
		return exp[part];
	return "";
}

std::string pichicore::getJIDfromNick(const std::string& nick, std::string room, bool all_rooms, int like_room)
{
	LOG("[JID] From nick " + nick, LOG::VERBOSE);
	
	std::string likeQuery = "";
	if(like_room != 0)
	{
		for(std::list< std::pair<JID, MUCRoom*> >::iterator it=jabber->rooms.begin(); it!=jabber->rooms.end(); it++)
			likeQuery += std::string(" AND jid") + ((like_room > 0) ? "" : " NOT") + " LIKE '" + it->first.bare() + "%'";
	}
	
	sqlite::q* qu = sql->squery("SELECT `jid` FROM users WHERE nick = '" + sql->escapeString(nick) + "'" + ((!all_rooms) ? " AND room = '" + sql->escapeString(room) + "'" : "" ) + likeQuery + ";");
	std::string jid = sql->fetchColumn(qu, 0);
	delete qu;
	return jid;
}

std::string pichicore::getJIDfromNicks(const std::string& nick, std::string room, bool all_rooms, int like_room)
{
	LOG("[JID] From nick's " + nick, LOG::VERBOSE);
	
	std::string likeQuery = "";
	if(like_room != 0)
	{
		for(std::list< std::pair<JID, MUCRoom*> >::iterator it=jabber->rooms.begin(); it!=jabber->rooms.end(); it++)
			likeQuery += std::string(" AND jid") + ((like_room > 0) ? "" : " NOT") + " LIKE '" + it->first.bare() + "%'";
	}
	
	sqlite::q* qu = sql->squery("SELECT `jid` FROM users_nick WHERE nick = '" + sql->escapeString(nick) + "'" + ((!all_rooms) ? " AND room = '" + sql->escapeString(room) + "'" : "" ) + likeQuery + " ORDER BY `time` ASC;");
	std::string jid = sql->fetchColumn(qu, 0);
	delete qu;
	return jid;
}


std::string pichicore::getArgJID(const std::string& arg)
{
	std::string jid;
	if(isBareJID(arg))
		return arg; // Обычный jid, все впорядке
	if(!isBareJID(arg) && isJID(arg))
		return getJIDpart(arg, 1); // длинный JID, получаем 1 часть
	// Скорей всего ник (преобразум в настоящий jid)
	jid = getJIDfromNick(arg, last_room, false, -1); // Ищет среди последней комнаты (или лички, если сообщение не в комнате)
	if(jid != "")
		return jid;
	// Скорей всего ник (пробуем в комнатный jid)
	jid = getJIDfromNick(arg, last_room, false, 1); // Ищет среди последней комнаты (или лички, если сообщение не в комнате)
	if(jid != "")
		return jid;
	// Скорей всего из другой комнаты (преобразум в настоящий jid)
	jid = getJIDfromNick(arg, "", true, -1); // Ищет среди всех комнат и личек
	if(jid != "")
		return jid;
	// Скорей всего из другой комнаты (преобразум в комнатный jid)
	jid = getJIDfromNick(arg, "", true, 1); // Ищет среди всех комнат и личек
	if(jid != "")
		return jid;
	// значит среди старых ников (преобразум в настоящий jid)
	jid = getJIDfromNicks(arg, "", true, -1); // Ищет среди всех встречающихся ников, по всем комнатам или личкам
	if(jid != "")
		return jid;
	// значит среди старых ников (преобразум в комнатный jid)
	jid = getJIDfromNicks(arg, "", true, 1); // Ищет среди всех встречающихся ников, по всем комнатам или личкам
	if(jid != "")
		return jid;
	// Ну это уже вообще ппц тогда... нету такого
	return "";
}


std::string pichicore::getDefaultRoom(void)
{
	std::list< std::pair<JID, MUCRoom*> >::iterator first_room;
	first_room = jabber->rooms.begin();
	return (*first_room).first.bare();
}

std::string pichicore::getNickFromJID(const std::string& jid, std::string room, bool all_rooms)
{
	LOG("[NICK] From JID " + jid, LOG::VERBOSE);
	sqlite::q* qu = sql->squery("SELECT `nick` FROM users WHERE jid = '" + sql->escapeString(jid) + "'" + ((!all_rooms) ? " AND room = '" + sql->escapeString(room) + "'" : "" ) + ";");
	std::string rtn = sql->fetchColumn(qu, 0);
	delete qu;
	return rtn;
}

std::string pichicore::getArgNick(const std::string& arg)
{
	if(!isJID(arg))
		return arg;
	if(isBareJID(arg))
		return getNickFromJID(arg, last_room);
	std::string barenick = getJIDpart(arg, 2);
	return barenick;
}


bool pichicore::isAccess(std::string jid, std::string room, int level)
{
	if(jid == "")
		jid = last_jid;
	
	if(jid == "")
		return false;
	
	sqlite::q* qu = sql->squery("SELECT `level` FROM users WHERE jid = '" + sql->escapeString(jid) + "' AND room = '" + sql->escapeString(room) + "';");
	std::string tempresult = sql->fetchColumn(qu, 0);
	delete qu;
	
	if(tempresult == "")
		return false;
	
	int dblevel;
	try
	{
		dblevel = system::atoi(tempresult);
	}
	catch(boost::bad_lexical_cast e)
	{
		return false;
	}
	
	LOG("Test access to " + jid + ": " + tempresult + " >= " + system::itoa(level), LOG::VERBOSE);
	
	return (dblevel >= level);
}

bool pichicore::isAccess(int level)
{
	return isAccess(last_jid, last_room, level);
}


bool pichicore::reciveMessage(const std::string& message, const std::string& type, const std::string& from)
{
	if(time(NULL) - jabber->times["wait"] < wait_time)
	{
		LOG("Ignore Message", LOG::DEBUG);
		return false;
	}
  
	if(message == "" || from == "" || type == "")
	{
		LOG("Some off room values null ... ignore...", LOG::WARNING);
		return false;
	}
	
	bool ignore_this = false;
	
	last_message = message;
	last_from = from;
	last_type = type;
	
	if(last_type == "groupchat")
		last_room = getJIDpart(last_from, 1);
	else if(last_type == "chatgroup")
		last_room = getJIDpart(last_from, 1);
	else
		last_room = "";

	
	if(last_type == "groupchat")
		last_jid = getJIDfromNick(getJIDpart(last_from, 2), last_room);
	else if(last_type == "chatgroup")
		last_jid = getJIDfromNick(getJIDpart(last_from, 2), last_room);
	else
		last_jid = getJIDpart(last_from, 1);
	
	// Защита от спама
	double micronow = system::microtime();
	if(usermsg_times[last_jid] <= 0 || usermsg_times[last_jid] > micronow) // 2 бред ))
		usermsg_times[last_jid] = 0; // если пустой контейнер
	if(micronow - usermsg_times[last_jid] < 0.25) // константа выяснена методом тяжелых проб и ошибок))
	{
		LOG("It's may be spam. Ignoring this...", LOG::VERBOSE);
		ignore_this = true;
	}
	usermsg_times[last_jid] = micronow;
	
	if(!isAccess(last_jid, last_room, 1))
	{
		LOG("Acess denied at message reciver", LOG::DEBUG);
		return false;
	}
	
	//($hook = PichiPlugin::fetch_hook('pichicore_message_recive_begin')) ? eval($hook) : false;
	//$this->log->log("Call message method", PichiLog::LEVEL_DEBUG);
	
	if(enabled && !isCommand(last_message) && options["log_enabled"] == "1")
		sql->exec("INSERT INTO log (`jid`,`room`,`from`,`time`,`type`,`message`) VALUES ('" + sql->escapeString(last_jid) + "','" + sql->escapeString(last_room) + "','" + sql->escapeString(last_from) + "','" + sql->escapeString(system::stringTime(time(NULL))) + "','" + sql->escapeString(last_type) + "','" + sql->escapeString(last_message) + "');");
	
	// В лог записали, остальное считает как спам
	if(ignore_this)
		return false;
	
	//ME breaker
	if(last_jid == jabber->getMyJID().bare())
		return true;
	
	//to lexems massges
	if(enabled && !isCommand(last_message) && options["answer_remember"] == "1")
		lex->parseText(last_message);
		
	event->callEvent("message_recive_completed", "jid=" + last_jid);
		
	if(isCommand(last_message))
		commander->fetchCommand(last_message);
	
	if(enabled && !isCommand(last_message) && options["answer_mode"] == "1")
	{
		std::string answer;
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
			if( options["answer_only_nick"] == "0" ||
			  last_type != "groupchat" || // в личках отвечает всегда
			  (options["answer_only_nick"] == "1" && last_message.substr(0,jabber->getMyNick().size()) == jabber->getMyNick()) )
			{
				answer = (last_type == "groupchat" && options["answer_only_nick"] == "1") ? getArgNick(last_from) + ": " : "";
				answer += lex->genFullRandom();
				sendAnswer(answer);
			}
		}
	}
		
	//($hook = PichiPlugin::fetch_hook('commands_message_recive_complete')) ? eval($hook) : false;
	
	// Дошло до конца, записываем более актуальное время
	usermsg_times[last_jid] = micronow;
	
	return true;
}

void pichicore::sendAnswer(const std::string& message)
{
	std::string to;
	if(last_type == "groupchat")
		to = last_room;
	else
		to = last_from;
	
	if(message.size() > system::atot(options["msg_limit"]) && system::atot(options["msg_limit"]) > 1 && last_type == "groupchat")
	{
		jabber->sendMessage(JID(last_room), (*lang)("message_to_private_chat"));
		to = last_jid;
	}

	if(to == "")
		return;

	jabber->sendMessage(JID(to), message);
}

void pichicore::sendAnswer(const std::string& message, const pichi::lastmessage& msg)
{
	std::string to;
	if(msg.last_type == "groupchat")
		to = msg.last_room;
	else
		to = msg.last_from;
	
	if(message.size() > system::atot(options["msg_limit"]) && system::atot(options["msg_limit"]) > 1 && msg.last_type == "groupchat")
	{
		jabber->sendMessage(JID(msg.last_room), (*lang)("message_to_private_chat"));
		to = msg.last_jid;
	}

	if(to == "")
		return;

	jabber->sendMessage(JID(to), message);
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
	sql->query("SELECT `jid`,`nick` FROM users WHERE status = 'available' AND room = '" + sql->escapeString(room) + "';");
	std::map<std::string, std::string> users;
	while(!(users = sql->fetchArray()).empty())
	{
		if(users["nick"] == user || users["jid"] == user)
			return true;
	}
	return false;
}

bool pichicore::isOnline(std::string jid)
{
	sql->query("SELECT COUNT(*) FROM users WHERE status = 'available' AND jid = '" + sql->escapeString(jid) + "';");
	int count;
	try
	{
		count = system::atoi(sql->fetchColumn(0));
	}
	catch(boost::bad_lexical_cast e)
	{
		return false;
	}
	return (count > 0);
}

void pichicore::ping(std::string jid)
{
	//timer = boost::timer();
	reciver["ping_" + jid] = boost::lexical_cast<std::string>(system::microtime());
	jabber->client->xmppPing(JID(jid), jabber);  	
}

void pichicore::pingRecive(std::string jid)
{
	double diff =  system::microtime() - boost::lexical_cast<double>(reciver["ping_" + jid]);
	reciver.erase("ping_" + jid);
	std::string str = boost::lexical_cast<std::string>(diff * 1000.0);
	str = str.substr(0, str.find(".") + 3);
	sendAnswer( (*lang)("command_ping_pong", str.c_str()) );
}

// устанавливает информацию о jid
void pichicore::setJIDinfo(std::string jid, std::string name, std::string value, std::string groupid)
{
	sqlite::q* qu = sql->squery("SELECT COUNT(*) FROM users_data WHERE jid = '" + sql->escapeString(jid) + "' AND name = '" + sql->escapeString(name) + "'" + ((groupid != "") ? " AND groupid = '" + sql->escapeString(groupid) + "'" : "") + ";");
	if(system::atoi(sql->fetchColumn(qu, 0)) > 0)
		sql->exec("UPDATE users_data SET value = '" + sql->escapeString(value) + "'  WHERE jid = '" + sql->escapeString(jid) + "' AND name = '" + sql->escapeString(name) + "'" + ((groupid != "") ? " AND groupid = '" + sql->escapeString(groupid) + "'" : "") + ";");
	else
		sql->exec("INSERT INTO users_data (`jid`,`name`,`value`,`groupid`) VALUES ('" + sql->escapeString(jid) + "','" + sql->escapeString(name) + "','" + sql->escapeString(value) + "','" + ((groupid != "") ? sql->escapeString(groupid) : "") + "');");
	delete qu;
}

// а теперь получить инфу
std::map<std::string, std::string> pichicore::getJIDinfo(std::string jid, std::string name, std::string groupid)
{
	std::map<std::string, std::string> retmap, data;
	sqlite::q* qu = sql->squery("SELECT * FROM users_data WHERE jid = '" + sql->escapeString(jid) + "'" + ((name != "") ? " AND name = '" + sql->escapeString(name) + "'" : "") + ((groupid != "") ? " AND groupid = '" + sql->escapeString(groupid) + "'" : "") + ";");
	while(!(data = sql->fetchArray(qu)).empty())
		retmap[data["name"]] = data["value"];
	delete qu;
	return retmap;
}

// ну и удалить
void pichicore::delJIDinfo(std::string jid, std::string name, std::string groupid)
{
	sql->exec("DELETE FROM users_data WHERE jid = '" + sql->escapeString(jid) + "'" + ((name != "") ? " AND name = '" + sql->escapeString(name) + "'" : "") + ((groupid != "") ? " AND groupid = '" + sql->escapeString(groupid) + "'" : "") + ";");
}

void pichicore::ban(std::string jid, std::string time, std::string reason, std::string room)
{
	if(jid == "")
		return;
	if(room == "")
		room = getLastRoom(); // main room
	jabber->ban(jid, JID(room), reason);
	if(time != "")
	{
		time_t tm = convertTime(time);
		setJIDinfo(jid, "ban", system::stringTime(tm + ::time(NULL)), room);
		setJIDinfo(jid, "ban_reason", reason, room);
		setJIDinfo(jid, "ban_room", room, room);
	}
}
        
void pichicore::unban(std::string jid, std::string reason, std::string room)
{
  	if(jid == "")
		return;
	if(room == "")
		room = getLastRoom(); // main room
	jabber->unban(jid, JID(room), reason);
	delJIDinfo(jid, "ban", room);
	delJIDinfo(jid, "ban_reason", room);
	delJIDinfo(jid, "ban_room", room);
}
        
void pichicore::kick(std::string jid, std::string time, std::string reason, std::string room)
{
  	if(jid == "")
		return;
	if(room == "")
		room = getLastRoom(); // main room
	jabber->kick(getNickFromJID(jid, room), JID(room), reason);
	if(time != "")
	{
		time_t tm = convertTime(time);
		setJIDinfo(jid, "kick", system::stringTime(tm + ::time(NULL)), room);
		setJIDinfo(jid, "kick_reason", reason, room);
		setJIDinfo(jid, "kick_room", room, room);
	}
}
        
void pichicore::unkick(std::string jid, std::string room)
{
  	if(jid == "")
		return;
	if(room == "")
		room = getLastRoom(); // main room
	delJIDinfo(jid, "kick", room);
	delJIDinfo(jid, "kick_reason", room);
	delJIDinfo(jid, "kick_room", room);
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
	LOG("[CRON] Bans", LOG::VERBOSE);
	//Check bans and kicks
	sqlite::q* qu = sql->squery("SELECT `jid`,`value`,`name`,`groupid` FROM users_data WHERE name = 'ban' OR name = 'kick';");
	std::map<std::string, std::string> bans;
	while(!(bans = sql->fetchArray(qu)).empty())
	{
		if(boost::lexical_cast<time_t>(bans["value"]) <= time(NULL))
		{
			if(bans["name"] == "ban")
				unban(bans["jid"], "Auto-unban", bans["groupid"]);
			else if(bans["name"] == "kick")
				unkick(bans["jid"], bans["groupid"]);
		}
	}
	delete qu;
	crons["bans"]["last"] = time(NULL); 
    }
}

bool pichicore::canCron(std::string crn)
{
	return (time(NULL) - crons[crn]["last"] > crons[crn]["interval"]);
}

lastmessage& pichicore::operator=(const pichi::pichicore& lst)
{
	return lastmessage::operator=(lst);
}


}
