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

#include "core.h"

namespace pichi
{

/*
 *	Only on linux 
 */
void core::firstStart(void )
{
#ifndef WIN32
	LOG("First StartUp. Creating Structure...", LOG::WARNING);
	std::string dir = system::getFullPath(PICHI_CONFIG_DIR);
	
	mkdir(dir.c_str(), 0775);
	
	std::ifstream ifs("/usr/share/pichi/config/pichi.xml", std::ios::binary);
	std::ofstream ofs((dir + "pichi.xml").c_str(), std::ios::binary);
	ofs << ifs.rdbuf();
	
	LOG("Check ~/.pichi/pichi.xml config. Then start pichi again.", LOG::WARNING);
	throw PichiException("Check ~/.pichi/pichi.xml config. Then start pichi again.");
#endif
}
  
void core::botstart(void)
{
#ifdef WIN32
	UINT oldcodepage = GetConsoleOutputCP();
	SetConsoleOutputCP(65001);
#endif
	LOG("Start Pichi", LOG::INFO);
	if(pichi->getConfigOption("debug") == "1")
	{
		LOG::LEVEL = system::atoi(pichi->getConfigOption("debug_level"));
		LOG("Debug enabled at level: " + pichi->getConfigOption("debug_level"), LOG::INFO);
	}
	if(pichi->getConfigOption("port") != "5222")
		client = new Client( jid, password, system::atoi(pichi->getConfigOption("port")) );
	else
		client = new Client( jid, password );
	client->setResource(pichi->getConfigOption("resource"));
	client->logInstance().registerLogHandler(LogLevelDebug, LogAreaAll, this);
	client->registerConnectionListener( this );
	client->registerMessageHandler( this );
	client->registerPresenceHandler( this );
	client->registerSubscriptionHandler( this );
	roster = new RosterManager( client );
	BOOST_FOREACH( std::string room, system::explode(",", pichi->getConfigOption("room")) )
		enterRoom(JID(room + "/" + nick));
	times["white_ping"] = times["start"] = time(NULL);
	if(pthread_create(&thread, NULL, &core::cron, (void*)this) > 0)
		throw PichiException("Error in cron thread");
	client->connect();
	if(!was_connected)
		LOG("Pichi didn't connect to the xmpp server. Check connection settings.", LOG::WARNING);
#ifdef WIN32
	SetConsoleOutputCP(oldcodepage);
	::system("pause");
#endif
}

core::core(int argc, char** argv)
{
	// First start checks (create dir on linux)
#ifndef WIN32
	if(!system::fileExists(PICHI_CONFIG_DIR))
		firstStart();
#endif
	// Parse args
	if(!parseArgs(argc, argv))
	{
		// Null object for safe delete
		roster = NULL;
		client = NULL;
		pichi = NULL;
		return;
	}
		
	// Init pichi
	was_connected = false; // true if connect was successful
	pichi = new pichicore();
	pichi->jabber = this;
	// ----------
	initDBStruct();
	pichi->reloadSqlConfig();
  
	name = pichi->getConfigOption("user");
	nick = pichi->getConfigOption("room_user");
	password = pichi->getConfigOption("password");
	server = pichi->getConfigOption("server");
	jid = name + "@" + server;
	roomservice = pichi->getConfigOption("room_service");
#ifndef WIN32
	if(coptions.count("daemonize"))
	{
		if (daemon(true, false))
			std::cerr << "Pichi daemon failed" << std::endl;
		if(!coptions.count("no-pid"))
		{
			if(coptions["pid"].as<std::string>() != "")
				std::ofstream(coptions["pid"].as<std::string>().c_str()) << getpid() << std::endl;
			else
				std::ofstream(PID_FILE) << getpid() << std::endl;
		}
	}
#endif
	botstart();
}

core::~core() throw()
{
	for(std::list< std::pair<JID, MUCRoom*> >::iterator it=rooms.begin(); it!=rooms.end(); it++)
		delete (it->second);
	delete roster;
	delete client;
	delete pichi;
}


bool core::parseArgs(int argc, char** argv)
{
	namespace arg = boost::program_options;
	
	arg::options_description description("Allowed options");
	description.add_options()
		("help,h", "Help message")
		("pid,p", "PID file path")
		("no-pid,n", "Start pichi without pid file")
		("daemonize,d", "Run Pichi as a daemon");
	try 
	{
		/* разбор аргументов */
		arg::store(arg::parse_command_line(argc, argv, description), coptions);
	} 
	catch (const std::exception& e)
	{
		/* Неправильные аргументы */
		std::cout << "Command line error: " << e.what() << std::endl;
		return false;
	}
	
	if ( coptions.count("help") )
	{
		std::cout << description << std::endl;
		return false;
	}
	
	return true;
}

void core::sendMessage(JID jid, const std::string& message)
{
	Message::MessageType type = Message::Chat;
	for(std::list< std::pair<JID, MUCRoom*> >::iterator it=rooms.begin(); it!=rooms.end(); it++)
	{
		if(jid.full() == (*it).first.bare())
		{
			type = Message::Groupchat;
			break;
		}
	}
	Message m( type, jid, message );
	client->send( m );
}

void core::ban(const std::string& nick, JID room, std::string message)
{
	for(std::list< std::pair<JID, MUCRoom*> >::iterator it=rooms.begin(); it!=rooms.end(); it++)
	{
		if((*it).first.bare() == room.full())
		{
			it->second->ban(nick, message);
		}
	}
}

void core::unban(const std::string& nick, JID room, std::string message)
{
	for(std::list< std::pair<JID, MUCRoom*> >::iterator it=rooms.begin(); it!=rooms.end(); it++)
	{
		if((*it).first.bare() == room.full())
		{
			it->second->setAffiliation(nick, AffiliationNone, message);
		}
	}
}

void core::kick(const std::string& nick, JID room, std::string message)
{
	for(std::list< std::pair<JID, MUCRoom*> >::iterator it=rooms.begin(); it!=rooms.end(); it++)
	{
		if((*it).first.bare() == room.full())
		{
			it->second->kick(nick, message);
		}
	}
}


void core::enterRoom(JID room)
{
	MUCRoom* newroom = new MUCRoom(client, room, this, NULL);
	rooms.push_back(std::pair<JID, MUCRoom*>(room,newroom));
}

void core::leftRoom(JID room)
{
	for(std::list< std::pair<JID, MUCRoom*> >::iterator it=rooms.begin(); it!=rooms.end(); it++)
	{
		if(it->first.bareJID() == room || it->first == room)
		{
			it->second->leave();
			delete (it->second);
		}
	}
}

void core::onConnect()
{
	was_connected = true;
	for(std::list< std::pair<JID, MUCRoom*> >::iterator it=rooms.begin(); it!=rooms.end(); it++)
		it->second->join();
	times["wait"] = time(NULL);
	pichi->cleanUserInfo();
}

void core::handleMessage( const Message& msg, MessageSession* session = 0 )
{
	std::cout << "[" << system::timeToString(time(NULL), "%H:%M:%S") << "]" << " <" << msg.from().full() << "> " << msg.body() << std::endl;
	pichi->cronDo("message");
	pichi->reciveMessage(msg.body(), "chat", msg.from().full());
}

void core::handleMUCMessage (MUCRoom *thisroom, const Message &msg, bool priv )
{
	std::cout << "[" << system::timeToString(time(NULL), "%H:%M:%S") << "]" << " <" << msg.from().resource() << "> " << msg.body() << std::endl;
	pichi->cronDo("muc_message");
	pichi->reciveMessage(msg.body(), ((priv) ? "chatgroup" : "groupchat"), msg.from().full());
}

void core::handleMUCParticipantPresence (MUCRoom *thisroom, const MUCRoomParticipant  participant, const Presence &presence)
{
	
	std::string jid;
	std::string nick;
	std::string state;
	std::string room;
	std::string role;
	std::string status;
	
	if(participant.jid != NULL)
	{
		if(participant.jid->bare() != "")
		{
			jid = participant.jid->bare();
		}
		else
		{
			LOG("Can not recive JID from user (Check room settings, or pichi privilegy)", LOG::WARNING);
			return;
		}
	}
	else if(presence.presence() == Presence::Unavailable)
	{
		jid = pichi->getJID(participant.nick->resource());
	}
	else
	{
		LOG("Can not recive JID from user (Check room settings, or pichi privilegy)", LOG::WARNING);
		return; // what pichi must do ?
	}
	
	nick = participant.nick->resource();
	
	if(presence.presence() != Presence::Unavailable)
		state = "available";
	else
		state = "unavailable";
	
	room = participant.nick->bare();
	
	// get role
	if(participant.role == RoleModerator)
		role = "moderator";
	else
		role = "participant";
	
	status = participant.status;
	
	pichi->cronDo("muc_presence");
	pichi->setUserInfo(jid, nick, state, room, role, status);

}

void core::handlePresence(const Presence& presence)
{
	pichi->cronDo("presence");
  
	if(presence.from().bare() == "")
	{
		LOG("This is not user presence", LOG::DEBUG);
		return;
	}

	std::string state;
	if(presence.presence() != Presence::Unavailable)
		state = "available";
	else
		state = "unavailable";

	pichi->setUserInfo(presence.from().bare(), "", state, "", "participant", presence.status());
}


void core::handleSubscription(const Subscription& subscription)
{
	switch(subscription.subtype())
	{
		case Subscription::Subscribe: 
		{
			Subscription p1(Subscription::Subscribed, subscription.from().bareJID());
			client->send(p1);
			Subscription p2(Subscription::Subscribe, subscription.from().bareJID());
			client->send(p2);
			break;
		}
		case Subscription::Unsubscribe:
		{
		  	Subscription p1(Subscription::Unsubscribed, subscription.from().bareJID());
			client->send(p1);
			Subscription p2(Subscription::Unsubscribe, subscription.from().bareJID());
			client->send(p2);
			break;
		}
		case Subscription::Subscribed:
		{
			roster->handleSubscription(subscription);
			break;
		}
		case Subscription::Unsubscribed:
		{
			roster->handleSubscription(subscription);
			break;
		}
		default:
			break;
	}
}


void core::handleLog (LogLevel level, LogArea area, const std::string &message)
{
	LOG("[GLOOX]" + message, LOG::DEBUG);
}

void core::initDBStruct(void)
{
	if(!system::fileExists(PICHI_CONFIG_DIR + pichi->getConfigOption("db_file")))
	{
		pichi->sql = new sqlite(system::getFullPath(PICHI_CONFIG_DIR) + pichi->getConfigOption("db_file"));
		pichi->sql->exec("CREATE TABLE log (`from` TEXT, `time` TEXT, `type` TEXT, `message` TEXT);");
		pichi->sql->exec("CREATE TABLE lexems (`lexeme` TEXT, `count` INT);");
		pichi->sql->exec("CREATE TABLE wiki (`name` TEXT, `revision` INT, `value` TEXT);");
		pichi->sql->exec("CREATE TABLE settings (`name` TEXT, `value` TEXT, `description` TEXT);");
		pichi->sql->exec("CREATE TABLE users (`jid` TEXT, `nick` TEXT, `role` TEXT, `room` TEXT, `time` TEXT, `status` TEXT, `level` INT);");
		pichi->sql->exec("CREATE TABLE users_data (`jid` TEXT, `name` TEXT, `value` TEXT, `groupid` TEXT);");
		pichi->sql->exec("CREATE TABLE users_nick (`jid` TEXT, `nick` TEXT, `room` TEXT, `time` TEXT);");
		pichi->sql->exec("CREATE TABLE stats (`name` TEXT, `value` TEXT);");
		pichi->sql->exec("CREATE TABLE actions (`action` TEXT, `coincidence` TEXT, `do` TEXT, `option` TEXT, `value` TEXT);");
		pichi->sql->exec("CREATE TABLE db_version (`version` TEXT, `value` TEXT);");
  
		pichi->sql->exec("INSERT INTO db_version (`version`) VALUES ('19');");
		pichi->sql->exec("INSERT INTO settings (`name`, `value`, `description`) VALUES ('answer_mode','1','" + pichi->sql->escapeString((*pichi->lang)["db_configdesc_answer_mode"]) + "');"); // Отвечать после сообщений пользователей
		pichi->sql->exec("INSERT INTO settings (`name`, `value`, `description`) VALUES ('answer_random','0','" + pichi->sql->escapeString((*pichi->lang)["db_configdesc_answer_random"]) + "');"); // отвечать не всегда (0 - всегда)
		pichi->sql->exec("INSERT INTO settings (`name`, `value`, `description`) VALUES ('answer_remember','1','" + pichi->sql->escapeString((*pichi->lang)["db_configdesc_answer_remember"]) + "');"); // запоминать и разбивать на лексемы
		pichi->sql->exec("INSERT INTO settings (`name`, `value`, `description`) VALUES ('answer_word_limit','10','" + pichi->sql->escapeString((*pichi->lang)["db_configdesc_answer_word_limit"]) + "');"); // limit для запросов в лексемах
		pichi->sql->exec("INSERT INTO settings (`name`, `value`, `description`) VALUES ('log_enabled','1','" + pichi->sql->escapeString((*pichi->lang)["db_configdesc_log_enabled"]) + "');"); // вести лог ?
		pichi->sql->exec("INSERT INTO settings (`name`, `value`, `description`) VALUES ('treatment_coincidence','3','" + pichi->sql->escapeString((*pichi->lang)["db_configdesc_treatment_coincidence"]) + "');"); // вставлять обращение, совпадения (3 из 1)
		pichi->sql->exec("INSERT INTO settings (`name`, `value`, `description`) VALUES ('rand_message','0','" + pichi->sql->escapeString((*pichi->lang)["db_configdesc_rand_message"]) + "');"); // случайны ответ когда скучно
		pichi->sql->exec("INSERT INTO settings (`name`, `value`, `description`) VALUES ('msg_limit','500','" + pichi->sql->escapeString((*pichi->lang)["db_configdesc_msg_limit"]) + "');"); // лимит символов, после чего отправляет ответ в личку
		pichi->sql->exec("INSERT INTO settings (`name`, `value`, `description`) VALUES ('msg_max_limit','0','" + pichi->sql->escapeString((*pichi->lang)["db_configdesc_msg_max_limit"]) + "');"); // вверхний предел
	}
	else
	{
		pichi->sql = new sqlite(system::getFullPath(PICHI_CONFIG_DIR) + pichi->getConfigOption("db_file"));
		pichi->sql->exec("UPDATE settings SET description = '" + pichi->sql->escapeString((*pichi->lang)["db_configdesc_answer_mode"]) + "' WHERE name = 'answer_mode';");
		pichi->sql->exec("UPDATE settings SET description = '" + pichi->sql->escapeString((*pichi->lang)["db_configdesc_answer_random"]) + "' WHERE name = 'answer_random';");
		pichi->sql->exec("UPDATE settings SET description = '" + pichi->sql->escapeString((*pichi->lang)["db_configdesc_answer_remember"]) + "' WHERE name = 'answer_remember';");
		pichi->sql->exec("UPDATE settings SET description = '" + pichi->sql->escapeString((*pichi->lang)["db_configdesc_answer_word_limit"]) + "' WHERE name = 'answer_word_limit';");
		pichi->sql->exec("UPDATE settings SET description = '" + pichi->sql->escapeString((*pichi->lang)["db_configdesc_log_enabled"]) + "' WHERE name = 'log_enabled';");
		pichi->sql->exec("UPDATE settings SET description = '" + pichi->sql->escapeString((*pichi->lang)["db_configdesc_treatment_coincidence"]) + "' WHERE name = 'treatment_coincidence';");
		pichi->sql->exec("UPDATE settings SET description = '" + pichi->sql->escapeString((*pichi->lang)["db_configdesc_rand_message"]) + "' WHERE name = 'rand_message';");
		pichi->sql->exec("UPDATE settings SET description = '" + pichi->sql->escapeString((*pichi->lang)["db_configdesc_msg_limit"]) + "' WHERE name = 'msg_limit';");
		pichi->sql->exec("UPDATE settings SET description = '" + pichi->sql->escapeString((*pichi->lang)["db_configdesc_msg_max_limit"]) + "' WHERE name = 'msg_max_limit';");
	}
}

void core::handleEvent(const Event& event)
{
	if(event.eventType() == Event::PingPong)
		pichi->pingRecive(event.stanza()->from().bare());
}

gloox::JID& core::getMyJID(void )
{
	return jid;
}

void *core::cron(void *context)
{
	while(true)
	{
		if(((core *)context)->was_connected)
		{
			if(time(NULL) - ((core *)context)->times["white_ping"] > 5 * 60)
			{
				((core *)context)->times["white_ping"] = time(NULL);
				LOG("[CRON] Ping of live", LOG::VERBOSE);
		
				((core *)context)->client->whitespacePing();
			}
			((core *)context)->pichi->cronDo("cron");
		}
#ifndef WIN32
		sleep(1);
#else
		Sleep(1);
#endif
	}
	pthread_exit(context);
	return context;
}

}