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

#include "pichicore.h"

#include <gloox/client.h>
#include <gloox/jid.h>

#include <vector>
#include <boost/lexical_cast.hpp>

#include "pichi.h"
#include "sqlite.h"
#include "helper.h"
#include "commandbase.h"
#include "translation.h"
#include "lexemebuilder.h"
#include "pichievent.h"


namespace pichi
{

PichiCore::PichiCore() : PichiOptions(&sql)
{
	enabled = true;
	// init commander
	commander = new commandbase(this);
	//translater
	lang = new Translation(config["language"]);
	lex = new LexemeBuilder(&sql);
	event = new PichiEvent(this);
	
	//timers
	crons["bans"]["last"] = time(NULL);
	crons["bans"]["interval"] = 10;
}

bool PichiCore::isEnabled(void)
{
	return enabled;
}

void PichiCore::off(void )
{
	enabled = false;
}

void PichiCore::on(void )
{
	enabled = true;
}

void PichiCore::setUserClient(const JID& jid, const std::string& client, const std::string& version, const std::string& os)
{
	sql->exec("UPDATE users SET client_name = '" + sql->escapeString(client) + "', client_version = '" + sql->escapeString(version) + "', client_os = '" + sql->escapeString(os) + "' WHERE jid = '" + sql->escapeString(jid.full()) + "' OR jid = '" + sql->escapeString(jid.bare()) + "';");
	
	if(userClientAnswer.size() > 0) {
		std::list<std::string>::iterator it = std::find_if(userClientAnswer.begin(), userClientAnswer.end(), [&jid](const std::string& str){ return jid.full() == str; });
		if(it != userClientAnswer.end()) {
			userClientAnswer.erase(it);
			if(client.size() > 0 || version.size() > 0 || os.size() > 0) {
				Log("UserInfo answer", Log::DEBUG);
				sendAnswer(client + " " + version + "\n" + os);
			}
		}
	}
}

void PichiCore::setUserInfo(std::string jid, std::string nick, std::string state, std::string room, std::string role, std::string status, const std::string& resource)
{
	int level = 1; // defaul access level
	
	sql->query("SELECT `status` FROM users WHERE jid = '" + sql->escapeString(jid) + "' AND room = '" + sql->escapeString(room) + "';");
	std::string old_state = sql->fetchColumn(0);
	
	if(config["global_admins"] == "1")
		if(role == "moderator")
			level = 2;

	std::vector< std::string > admins, ignore;
	
	admins = Helper::explode(",", config["admins"]);
	if(admins.size() > 0)
		if(Helper::in_array(jid, admins))
			level = 3;
	
	ignore = Helper::explode(",", config["ignore"]);
	if(ignore.size() > 0)
		if(Helper::in_array(jid, ignore))
			level = 0;

	if(state == "available" && old_state == "unavailable")
	{
		//event->callEvent("user_join_room", "room=" + room + ",jid=" + jid);
		// autokick
		SQLite::q* qu = sql->squery("SELECT nick FROM kicklist WHERE jid = '" + sql->escapeString(jid) + "' AND room = '" + sql->escapeString(room) + "';");
		if(sql->numRows(qu) == 1)
		{
			SQLite::SQLRow kickinfo = sql->fetchArray(qu);
			jabber->kick(kickinfo["nick"], room, "Auto-kick");
		}
		delete qu;
	}
	
	//($hook = PichiPlugin::fetch_hook('pichicore_status_set')) ? eval($hook) : false;
	if(room != "")
		if(state == "unavailable")
			Log("[PRESENCE_ROOM][OFF] " + jid + "{" + Helper::itoa(level) + "} (" + nick + ")[" + status + "] in " + room, Log::DEBUG);
		else
			Log("[PRESENCE_ROOM][ON] " + jid + "{" + Helper::itoa(level) + "} (" + nick + ")[" + status + "] in " + room, Log::DEBUG);
	else
		if(state == "unavailable")
			Log("[PRESENCE][OFF] " + jid + "{" + Helper::itoa(level) + "} [" + status + "]", Log::DEBUG);
		else
			Log("[PRESENCE][ON] " + jid + "{" + Helper::itoa(level) + "} [" + status + "]", Log::DEBUG);
	
	
	//get system info
	if(state == "available" && old_state != "available" && options["onstart_client_detection"] == "1")
	{
		if(resource.size() > 0) // пришел полный джид не из комнаты, отправляем с ресурсом
			jabber->sendClientDetection(jid + "/" + resource);
		else
			jabber->sendClientDetection(jid);
	}
		
	sql->query("SELECT COUNT(*) FROM users WHERE jid = '" + sql->escapeString(jid) + "' AND room = '" + sql->escapeString(room) + "';");
	if(Helper::atoi(sql->fetchColumn(0)) > 0)
	{
		sql->exec("UPDATE users SET nick = '" + sql->escapeString(nick)
				      + "', resource = '" + sql->escapeString(resource)
				      + "', time = '" + Helper::stringTime(time(NULL)) 
				      + "', status = '" + state 
				      + "', role = '" + sql->escapeString(role) 
				      + "', level = '" + Helper::itoa(level) 
				      + "' WHERE jid = '" + sql->escapeString(jid) 
					+ "' AND room = '" + sql->escapeString(room) 
					+ "';");
		//($hook = PichiPlugin::fetch_hook('pichicore_status_update')) ? eval($hook) : false;
	}
	else
	{
		sql->exec("INSERT INTO users (`jid`,`resource`,`nick`,`role`,`room`,`time`,`status`,`level`) VALUES ('" 
					    + sql->escapeString(jid) + "','" 
					    + sql->escapeString(resource) + "','"
					    + sql->escapeString(nick) + "','" 
					    + sql->escapeString(role) + "','" 
					    + sql->escapeString(room) + "','" 
					    + Helper::stringTime(time(NULL)) + "','" 
					    + state + "','" 
					    + Helper::itoa(level) + "');");
		sql->query("SELECT COUNT(*) FROM users WHERE jid = '" + sql->escapeString(jid) + "';");
		//if($this->db->fetchColumn() == 0)
		//		($hook = PichiPlugin::fetch_hook('pichicore_status_create')) ? eval($hook) : false;
	}
	
	// Insert Nick info
	if(nick != "")
	{
		sql->query("SELECT COUNT(*) FROM users_nick WHERE jid = '" + sql->escapeString(jid) + "' AND nick = '" + sql->escapeString(nick) + "' AND room = '" + sql->escapeString(room) + "';");
		if(Helper::atoi(sql->fetchColumn(0)) == 0)
			sql->exec("INSERT INTO users_nick (`jid`,`nick`,`room`,`time`) VALUES ('" + sql->escapeString(jid) + "','" + sql->escapeString(nick) + "','" + sql->escapeString(room) + "','" + Helper::stringTime(time(NULL)) + "');");
		
		// push to lexems nicks
		lex->addNick(nick);
	}
}

void PichiCore::cleanUserInfo(void)
{
	sql->exec("UPDATE users SET status = 'unavailable';");
}

bool PichiCore::isJID(const std::string& jid)
{
	return (std::find(jid.begin(), jid.end(), '@') != jid.end());
}

bool PichiCore::isBareJID(const std::string& jid)
{
	return ( isJID(jid) && (std::find(jid.begin(), jid.end(), '/') == jid.end()) );
}


std::string PichiCore::getJIDpart(const std::string& jid, unsigned int part)
{
	std::vector< std::string > exp = Helper::explode("/", jid);
	part--;
	if(part < exp.size())
		return exp[part];
	return "";
}

std::string PichiCore::getJIDfromNick(const std::string& nick, std::string room, bool all_rooms, int like_room, bool full_jid)
{
	Log("[JID] From nick " + nick, Log::VERBOSE);
	
	std::string likeQuery = "";
	if(like_room != 0)
	{
		std::list< std::pair<JID, MUCRoom*> >::iterator last_it = jabber->rooms.end();
		last_it--;
		for(std::list< std::pair<JID, MUCRoom*> >::iterator it=jabber->rooms.begin(); it!=jabber->rooms.end(); it++)
		{
			if(like_room > 0)
			{
				if(it == jabber->rooms.begin())
					likeQuery += " AND (";
				if(it != jabber->rooms.begin())
					likeQuery += " OR";
				likeQuery += " jid LIKE '" + it->first.bare() + "%'";
				if(it == last_it)
					likeQuery += ")";
			}
			else
			{
				likeQuery += " AND jid NOT LIKE '" + it->first.bare() + "%'";
			}
		}
	}
	
	SQLite::q* qu = sql->squery("SELECT `jid`,`resource` FROM users WHERE nick = '" + sql->escapeString(nick) + "'" + ((!all_rooms) ? " AND room = '" + sql->escapeString(room) + "'" : "" ) + likeQuery + ";");
	std::string jid = sql->fetchColumn(qu, 0);
	if(full_jid && !jid.empty()) {
		std::string resource = sql->fetchColumn(qu, 1, true);
		if(!resource.empty())
			jid += "/" + resource; // добавляем ресурс
	}
	delete qu;
	return jid;
}

std::string PichiCore::getJIDfromNicks(const std::string& nick, std::string room, bool all_rooms, int like_room)
{
	Log("[JID] From nick's " + nick, Log::VERBOSE);
	
	std::string likeQuery = "";
	if(like_room != 0)
	{
		std::list< std::pair<JID, MUCRoom*> >::iterator last_it = jabber->rooms.end();
		last_it--;
		for(std::list< std::pair<JID, MUCRoom*> >::iterator it=jabber->rooms.begin(); it!=jabber->rooms.end(); it++)
		{
			if(like_room > 0)
			{
				if(it == jabber->rooms.begin())
					likeQuery += " AND (";
				if(it != jabber->rooms.begin())
					likeQuery += " OR";
				likeQuery += " jid LIKE '" + it->first.bare() + "%'";
				if(it == last_it)
					likeQuery += ")";
			}
			else
			{
				likeQuery += " AND jid NOT LIKE '" + it->first.bare() + "%'";
			}
		}
	}
	
	SQLite::q* qu = sql->squery("SELECT `jid` FROM users_nick WHERE nick = '" + sql->escapeString(nick) + "'" + ((!all_rooms) ? " AND room = '" + sql->escapeString(room) + "'" : "" ) + likeQuery + " ORDER BY `time` ASC;");
	std::string jid = sql->fetchColumn(qu, 0);
	delete qu;
	return jid;
}


std::string PichiCore::getArgJID(const std::string& arg, bool withResource)
{
	std::string jid;
	if(isBareJID(arg) && !withResource)
		return arg; // Обычный jid, все впорядке
	if(!isBareJID(arg) && isJID(arg))
		if(!withResource)
			return getJIDpart(arg, 1); // длинный JID, получаем 1 часть
		else
			return arg;
	// Скорей всего ник (пробуем найти среди настоящих jid'ов)
	jid = getJIDfromNick(arg, last_room, false, -1, withResource); // Ищет среди последней комнаты (или лички, если сообщение не в комнате)
	if(jid != "")
		return jid;
	// Скорей всего ник (пробуем найти среди jid'ов из комнаты)
	jid = getJIDfromNick(arg, last_room, false, 1, withResource); // Ищет среди последней комнаты (или лички, если сообщение не в комнате)
	if(jid != "")
		return jid;
	// Скорей всего из другой комнаты (пробуем найти среди настоящих jid'ов из других комнат)
	jid = getJIDfromNick(arg, "", true, -1, withResource); // Ищет среди всех комнат и личек
	if(jid != "")
		return jid;
	// Скорей всего из другой комнаты (пробуем найти среди jid'ов из комнаты в виде комнатного-jid'a)
	jid = getJIDfromNick(arg, "", true, 1, withResource); // Ищет среди всех комнат и личек
	if(jid != "")
		return jid;
	// значит среди старых ников (преобразум в настоящий jid)
	if(!withResource) { // не имеет смысла искать ресурс среди старых jid'в (использовать неактуально)
		jid = getJIDfromNicks(arg, "", true, -1); // Ищет среди всех встречающихся ников, по всем комнатам или личкам
		if(jid != "")
			return jid;
		// значит среди старых ников (преобразум в комнатный jid)
		jid = getJIDfromNicks(arg, "", true, 1); // Ищет среди всех встречающихся ников, по всем комнатам или личкам
		if(jid != "")
			return jid;
	}
	return "";
}


std::string PichiCore::getDefaultRoom(void)
{
	std::list< std::pair<JID, MUCRoom*> >::iterator first_room;
	first_room = jabber->rooms.begin();
	return (*first_room).first.bare();
}

std::string PichiCore::getNickFromJID(const std::string& jid, const std::string& room, bool all_rooms)
{
	Log("[NICK] From JID " + jid, Log::VERBOSE);
	SQLite::q* qu = sql->squery("SELECT `nick` FROM users WHERE jid = '" + sql->escapeString(jid) + "'" + ((!all_rooms) ? " AND room = '" + sql->escapeString(room) + "'" : "" ) + ";");
	std::string rtn = sql->fetchColumn(qu, 0);
	delete qu;
	return rtn;
}

std::string PichiCore::getArgNick(const std::string& arg)
{
	if(!isJID(arg))
		return arg;
	if(isBareJID(arg))
		return getNickFromJID(arg, last_room);
	std::string barenick = getJIDpart(arg, 2);
	return barenick;
}


bool PichiCore::isAccess(std::string jid, std::string room, int level)
{
	if(jid == "")
		jid = last_jid;
	
	if(jid == "")
		return false;
	
	SQLite::q* qu = sql->squery("SELECT `level` FROM users WHERE jid = '" + sql->escapeString(jid) + "' AND room = '" + sql->escapeString(room) + "';");
	std::string tempresult = sql->fetchColumn(qu, 0);
	delete qu;
	
	if(tempresult == "")
		return false;
	
	int dblevel;
	try
	{
		dblevel = Helper::atoi(tempresult);
	}
	catch(boost::bad_lexical_cast e)
	{
		return false;
	}
	
	Log("Test access to " + jid + ": " + tempresult + " >= " + Helper::itoa(level), Log::VERBOSE);
	
	return (dblevel >= level);
}

bool PichiCore::isAccess(int level)
{
	return isAccess(last_jid, last_room, level);
}


bool PichiCore::reciveMessage(const std::string& message, const std::string& type, const std::string& from)
{ 
	if(message == "" || from == "" || type == "")
	{
		Log("Some off room values null ... ignore...", Log::WARNING);
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
	{
		last_jid = getJIDfromNick(getJIDpart(last_from, 2), last_room, false, -1);
		if(last_jid == "")
			last_jid = getJIDfromNick(getJIDpart(last_from, 2), last_room, false, 1);
	}
	else if(last_type == "chatgroup")
	{
		last_jid = getJIDfromNick(getJIDpart(last_from, 2), last_room, false, -1);
		if(last_jid == "")
			last_jid = getJIDfromNick(getJIDpart(last_from, 2), last_room, false, 1);	
	}
	else
		last_jid = getJIDpart(last_from, 1);
	
	// Защита от спама
	double micronow = Helper::microtime();
	if(usermsg_times[last_jid] <= 0 || usermsg_times[last_jid] > micronow) // 2 бред ))
		usermsg_times[last_jid] = 0; // если пустой контейнер
	if(micronow - usermsg_times[last_jid] < 0.25) // константа выяснена методом тяжелых проб и ошибок))
	{
		Log("It's may be spam. Ignoring this...", Log::VERBOSE);
		ignore_this = true;
	}
	usermsg_times[last_jid] = micronow;
	
	if(!isAccess(last_jid, last_room, 1))
	{
		Log("Acess denied at message reciver", Log::DEBUG);
		return false;
	}
		
	if(enabled && !isCommand(last_message) && options["log_enabled"] == "1")
		sql->exec("INSERT INTO log (`jid`,`room`,`from`,`time`,`type`,`message`) VALUES ('" + sql->escapeString(last_jid) + "','" + sql->escapeString(last_room) + "','" + sql->escapeString(last_from) + "','" + sql->escapeString(Helper::stringTime(time(NULL))) + "','" + sql->escapeString(last_type) + "','" + sql->escapeString(last_message) + "');");
	
	// В лог записали, остальное считает как спам
	if(ignore_this)
		return false;
	
	//ME breaker
	if(std::find(jabber->pichiIgnore.begin(), jabber->pichiIgnore.end(), last_jid) != jabber->pichiIgnore.end())
	{
		Log("Pichi message break", Log::DEBUG);
		return true;
	}
	
	//to lexems massges
	if(enabled && !isCommand(last_message) && options["answer_remember"] == "1")
		lex->parseText(last_message);
		
	event->callEvent("message_recive_completed", "jid=" + last_jid);
		
	if(isCommand(last_message))
	{
		std::string commandReal = last_message;
		if(jabber->getMyNick().size() > 0 && commandReal.substr(0,jabber->getMyNick().size()) == jabber->getMyNick()) {
			commandReal = commandReal.substr(jabber->getMyNick().size() + 2, commandReal.size() - (jabber->getMyNick().size() + 2));
		}
		commander->fetchCommand(commandReal);
	}
	
	if(enabled && !isCommand(last_message) && options["answer_mode"] == "1")
	{
		std::string answer;
		if(options["answer_random"] == "0" || (1 + rand() % Helper::atoi(options["answer_random"])) == 1)
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
			if( 	options["answer_only_nick"] == "0" ||
				last_type != "groupchat" || // в личках отвечает всегда
				(
					options["answer_only_nick"] == "1" && 
					( 
						last_message.substr(0,jabber->getMyNick().size() + 2) == jabber->getMyNick() + ": "
						|| last_message.substr(0,jabber->getMyNick().size() + 2) == jabber->getMyNick() + ", "
					)
				) 
			)
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

void PichiCore::sendAnswer(const std::string& message)
{
	std::string to;
	if(last_type == "groupchat")
		to = last_room;
	else
		to = last_from;
	
	if(message.size() > Helper::atot(options["msg_limit"]) && Helper::atot(options["msg_limit"]) > 1 && last_type == "groupchat")
	{
		jabber->sendMessage(JID(last_room), (*lang)("message_to_private_chat"));
		to = last_jid;
	}

	if(to == "")
		return;

	jabber->sendMessage(JID(to), message);
}

void PichiCore::sendAnswer(const std::string& message, const pichi::PichiMessage& msg)
{
	std::string to;
	if(msg.last_type == "groupchat")
		to = msg.last_room;
	else
		to = msg.last_from;
	
	if(message.size() > Helper::atot(options["msg_limit"]) && Helper::atot(options["msg_limit"]) > 1 && msg.last_type == "groupchat")
	{
		jabber->sendMessage(JID(msg.last_room), (*lang)("message_to_private_chat"));
		to = msg.last_jid;
	}

	if(to == "")
		return;

	jabber->sendMessage(JID(to), message);
}

bool PichiCore::isCommand(std::string& str)
{
	if(str.substr(0,1) == "!" && str.size() > 1)
		return true;
	return str.size() > jabber->getMyNick().size() 
		&& (str.substr(0,jabber->getMyNick().size()) == jabber->getMyNick())
		&& str.substr(jabber->getMyNick().size(), 3) == ": !";
}

bool PichiCore::setOption(std::string option, std::string value)
{

	sql->query("SELECT COUNT(*) FROM settings WHERE name = '" + sql->escapeString(option) + "';");
	if(Helper::atoi(sql->fetchColumn(0)) > 0)
	{
		setSqlOption(option, value);
		Log("Updated option '" + option + "' = " + value, Log::DEBUG);
		return true;
	}
	else
	{
		Log("Can't set '" + option + "'. There is no such option.", Log::DEBUG);
		return false;
	}
	
    return true; //stub
}

bool PichiCore::isOnline(std::string user, std::string room)
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

bool PichiCore::isOnline(std::string jid)
{
	sql->query("SELECT COUNT(*) FROM users WHERE status = 'available' AND jid = '" + sql->escapeString(jid) + "';");
	int count;
	try
	{
		count = Helper::atoi(sql->fetchColumn(0));
	}
	catch(boost::bad_lexical_cast e)
	{
		return false;
	}
	return (count > 0);
}

void PichiCore::ping(std::string jid)
{
	//timer = boost::timer();
	reciver["ping_" + jid] = boost::lexical_cast<std::string>(Helper::microtime());
	jabber->client->xmppPing(JID(jid), jabber);  	
}

void PichiCore::pingRecive(std::string jid)
{
	double diff =  Helper::microtime() - boost::lexical_cast<double>(reciver["ping_" + jid]);
	reciver.erase("ping_" + jid);
	std::string str = boost::lexical_cast<std::string>(diff * 1000.0);
	str = str.substr(0, str.find(".") + 3);
	sendAnswer( (*lang)("command_ping_pong", str.c_str()) );
}

void PichiCore::setJIDinfo(const std::string& jid, const std::string& name, const std::string& value, const std::string& groupid)
{
	SQLite::q* qu = sql->squery("SELECT COUNT(*) FROM users_data WHERE jid = '" + sql->escapeString(jid) + "' AND name = '" + sql->escapeString(name) + "'" + ((!groupid.empty()) ? " AND groupid = '" + sql->escapeString(groupid) + "'" : "") + ";");
	if(Helper::atoi(sql->fetchColumn(qu, 0)) > 0)
		sql->exec("UPDATE users_data SET value = '" + sql->escapeString(value) + "'  WHERE jid = '" + sql->escapeString(jid) + "' AND name = '" + sql->escapeString(name) + "'" + ((!groupid.empty()) ? " AND groupid = '" + sql->escapeString(groupid) + "'" : "") + ";");
	else
		sql->exec("INSERT INTO users_data (`jid`,`name`,`value`,`groupid`) VALUES ('" + sql->escapeString(jid) + "','" + sql->escapeString(name) + "','" + sql->escapeString(value) + "','" + ((!groupid.empty()) ? sql->escapeString(groupid) : "") + "');");
	delete qu;
}

std::map<std::string, std::string> PichiCore::getJIDinfo(const std::string& jid, const std::string& name, const std::string& groupid)
{
	SQLite::SQLRow retmap, data;
	SQLite::q* qu = sql->squery("SELECT * FROM users_data WHERE jid = '" + sql->escapeString(jid) + "'" + ((!name.empty()) ? " AND name = '" + sql->escapeString(name) + "'" : "") + ((!groupid.empty()) ? " AND groupid = '" + sql->escapeString(groupid) + "'" : "") + ";");
	while(!(data = sql->fetchArray(qu)).empty())
		retmap[data["name"]] = data["value"];
	delete qu;
	return retmap;
}

void PichiCore::delJIDinfo(const std::string& jid, const std::string& name, const std::string& groupid)
{
	sql->exec("DELETE FROM users_data WHERE jid = '" + sql->escapeString(jid) + "'" + ((!name.empty()) ? " AND name = '" + sql->escapeString(name) + "'" : "") + ((!groupid.empty()) ? " AND groupid = '" + sql->escapeString(groupid) + "'" : "") + ";");
}

void PichiCore::ban(const std::string& jid, const std::string& time, const std::string& reason, std::string room)
{
	if(jid.empty())
		return;
	if(room.empty())
		room = last_room;
	std::string nick = getNickFromJID(jid, room);
	jabber->ban(nick, room, reason);
	if(!time.empty())
	{
		time_t tm = convertTime(time);
		sql->query("INSERT INTO banlist (`jid`,`time`,`starttime`,`reason`, `room`, `nick`) VALUES ('" 
												+ sql->escapeString(jid) + "','" 
												+ Helper::stringTime(tm + ::time(NULL)) + "','"
												+ Helper::stringTime(::time(NULL)) + "','"
												+ sql->escapeString(reason) + "','"
												+ sql->escapeString(room) + "','"
												+ sql->escapeString(nick) + "');");
	}
}
        
void PichiCore::unban(const std::string& jid, const std::string& reason, std::string room)
{
	if(jid.empty())
		return;
	if(room.empty())
		room = last_room; // main room
	//std::string nick = getNickFromJID(jid, room);
	SQLite::q* qu = sql->squery("SELECT * FROM banlist WHERE jid = '" + sql->escapeString(jid) + "' AND room = '" + sql->escapeString(room) + "';");
	if(sql->numRows(qu) > 0)
	{
		SQLite::SQLRow baninfo = sql->fetchArray(qu);
		jabber->unban(baninfo["nick"], room, reason);
		sql->exec("DELETE FROM banlist WHERE jid = '" + sql->escapeString(jid) + "' AND room = '" + sql->escapeString(room) + "';");
	}
	delete qu;
}
        
void PichiCore::kick(const std::string& jid, const std::string& time, const std::string& reason, std::string room)
{
  	if(jid.empty())
		return;
	if(room.empty())
		room = getLastRoom(); // main room
	std::string nick = getNickFromJID(jid, room);
	jabber->kick(nick, room, reason);
	if(!time.empty())
	{
		time_t tm = convertTime(time);
		sql->query("INSERT INTO kicklist (`jid`,`time`,`starttime`,`reason`, `room`, `nick`) VALUES ('" 
												+ sql->escapeString(jid) + "','" 
												+ Helper::stringTime(tm + ::time(NULL)) + "','"
												+ Helper::stringTime(::time(NULL)) + "','"
												+ sql->escapeString(reason) + "','"
												+ sql->escapeString(room) + "','"
												+ sql->escapeString(nick) + "');");
	}
}
        
void PichiCore::unkick(std::string jid, std::string room)
{
  	if(jid.empty())
		return;
	if(room.empty())
		room = last_room; // main room
	sql->exec("DELETE FROM kicklist WHERE jid = '" + sql->escapeString(jid) + "' AND room = '" + sql->escapeString(room) + "';");
}

time_t PichiCore::convertTime(std::string time)
{
	if(time == "")
		return 0;
        
	time_t realtime;
	
	switch(boost::lexical_cast<char>(time.substr(time.length()-1).c_str()))
	{
		case 'm':
			realtime = Helper::atot(time.substr(0,time.length()-1));
			return realtime * 60;
			break;
		case 'h':
			realtime = Helper::atot(time.substr(0,time.length()-1));
			return realtime * 60 * 60;
			break;
		case 'd':
			realtime = Helper::atot(time.substr(0,time.length()-1));
			return realtime * 60 * 60 * 24;
			break;
		case 'w':
			realtime = Helper::atot(time.substr(0,time.length()-1));
			return realtime * 60 * 60 * 24 * 7;
			break;
		case 'M':
			realtime = Helper::atot(time.substr(0,time.length()-1));
			return realtime * 60 * 60 * 24 * 30;
			break;
		case 'Y':
			realtime = Helper::atot(time.substr(0,time.length()-1));
			return realtime * 60 * 60 * 24 * 30 * 12;
				break;
		default:
			return Helper::atot(time);
			break;
	}
}

// Действие по крону
void PichiCore::cronDo(std::string eventer)
{
    if(canCron("bans"))
    {
	Log("[CRON] Bans", Log::VERBOSE);
	//Check bans and kicks
	SQLite::q* qu = sql->squery("SELECT 'ban' AS type,* FROM banlist UNION ALL SELECT 'kick' AS type,* FROM kicklist;");
	SQLite::SQLRow bans;
	while(!(bans = sql->fetchArray(qu)).empty())
	{
		if(boost::lexical_cast<time_t>(bans["time"]) <= time(NULL))
		{
			if(bans["type"] == "ban")
				unban(bans["jid"], "Auto-unban", bans["room"]);
			else if(bans["type"] == "kick")
				unkick(bans["jid"], bans["room"]);
		}
	}
	delete qu;
	crons["bans"]["last"] = time(NULL);
    }
}

bool PichiCore::canCron(std::string crn)
{
	return (time(NULL) - crons[crn]["last"] > crons[crn]["interval"]);
}

PichiMessage& PichiCore::operator=(const pichi::PichiCore& lst)
{
	return PichiMessage::operator=(lst);
}


}
