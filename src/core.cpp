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
	Log("First StartUp. Creating Structure...", Log::WARNING);
	std::string dir = system::getFullPath(PICHI_CONFIG_DIR);
	
	mkdir(dir.c_str(), 0775);
	
	std::string conf_file;
	if(system::fileExists("/usr/share/pichi/config/pichi.xml"))
		conf_file = "/usr/share/pichi/config/pichi.xml";
	else if(RUN_DIR.substr(RUN_DIR.size()-4) == "/bin" && system::fileExists(RUN_DIR.substr(0, RUN_DIR.size()-3) + "share/pichi/config/pichi.xml"))
		conf_file = RUN_DIR.substr(0, RUN_DIR.size()-3) + "share/pichi/config/pichi.xml";
	else
		throw PichiException("Error: no config file founded... delete ~/.pichi and start bot again");
	
	std::ifstream ifs(conf_file.c_str(), std::ios::binary);
	std::ofstream ofs((dir + "pichi.xml").c_str(), std::ios::binary);
	ofs << ifs.rdbuf();
	
	Log("Check ~/.pichi/pichi.xml config. Then start pichi again.", Log::WARNING);
	throw PichiException("Check ~/.pichi/pichi.xml config. Then start pichi again.");
#endif
}
  
void core::botstart(void)
{
#ifdef WIN32
	UINT oldcodepage = GetConsoleOutputCP();
	SetConsoleOutputCP(65001);
#endif
	Log("Start Pichi", Log::INFO);
	if(pichi->getConfigOption("debug") == "1")
	{
		Log::LEVEL = system::atoi(pichi->getConfigOption("debug_level"));
		Log("Debug enabled at level: " + pichi->getConfigOption("debug_level"), Log::INFO);
	}
	if(pichi->getConfigOption("port") != "5222")
		client = new Client( jid, password, system::atoi(pichi->getConfigOption("port")) );
	else
		client = new Client( jid, password );
	// set my info
	client->disco()->setVersion("Pichi Bot", PICHI_VERSION);
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
		Log("Pichi didn't connect to the xmpp server. Check connection settings.", Log::WARNING);
#ifdef WIN32
	SetConsoleOutputCP(oldcodepage);
	::system("pause");
#endif
	Log("Stop Pichi", Log::INFO);
}

core::core(int argc, char** argv)
{
#ifndef WIN32
	char pathbuf[1024];
	if(getcwd(pathbuf, 1024) == NULL)
		throw PichiException("Start dir not founded");
	RUN_DIR = pathbuf;
	
	// First start checks (create dir on linux)
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
		Log::file_log = true;
		Log::log_file = coptions["log"].as<std::string>();
		if(!coptions.count("no-pid") && coptions["pid"].as<std::string>() != "")
			std::ofstream(coptions["pid"].as<std::string>().c_str()) << getpid() << std::endl;
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
		("log,l", arg::value< std::string >()->default_value(system::getFullPath(PICHI_LOG_FILE)), "Log file path")
		("pid,p", arg::value< std::string >()->default_value(system::getFullPath(PICHI_PID_FILE)), "PID file path")
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

// С ником
MUCRoom* core::enterRoom(JID room)
{
	MUCRoom* newroom = new MUCRoom(client, room, this, NULL);
	rooms.push_back(std::pair<JID, MUCRoom*>(room.bare(), newroom));
	return newroom;
}

void core::leftRoom(JID room)
{
	for(std::list< std::pair<JID, MUCRoom*> >::iterator it=rooms.begin(); it!=rooms.end(); it++)
	{
		if(it->first == room || it->first == room.bare())
		{
			it->second->leave();
			delete (it->second);
			rooms.erase(it);
			break;
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
	pichi->cronDo("message");
	pichi->reciveMessage(msg.body(), "chat", msg.from().full());
}

void core::handleMUCMessage (MUCRoom *thisroom, const Message &msg, bool priv )
{
	pichi->cronDo("muc_message");
	pichi->reciveMessage(msg.body(), ((priv) ? "chatgroup" : "groupchat"), msg.from().full());
}

void core::handleMUCParticipantPresence (MUCRoom *thisroom, const MUCRoomParticipant  participant, const Presence &presence)
{
	
	std::string jid = "";
	std::string nick;
	std::string state;
	std::string room;
	std::string role;
	std::string status;
	
	if(participant.jid != NULL)
	{
		jid = participant.jid->bare();
	}
	//else if(presence.presence() == Presence::Unavailable)
	//{
	//	if(participant.nick->resource() != "" && participant.nick->bare() != "")
	//		jid = pichi->getJIDfromNick(participant.nick->resource(), participant.nick->bare());
	//}

	// Uses room jid
	if(jid == "")
		jid = participant.nick->full();
	
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
		Log("This is not user presence", Log::DEBUG);
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
	Log("[GLOOX]" + message, Log::DEBUG);
}

void core::initDBStruct(void)
{
	
	if(!system::fileExists(PICHI_CONFIG_DIR + pichi->getConfigOption("db_file")))
	{
		pichi->sql = new sqlite(system::getFullPath(PICHI_CONFIG_DIR) + pichi->getConfigOption("db_file"));
		PichiDbPather patch(pichi->sql, pichi->lang);
		patch.initDbStruct();
	}
	else
	{
		pichi->sql = new sqlite(system::getFullPath(PICHI_CONFIG_DIR) + pichi->getConfigOption("db_file"));
		PichiDbPather patch(pichi->sql, pichi->lang);
		patch.checkDbStruct();
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

std::string core::getMyNick(void )
{
	return nick;
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
				Log("[CRON] Ping of live", Log::VERBOSE);
		
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
