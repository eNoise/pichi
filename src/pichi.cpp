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

#include "pichi.h"

#include <gloox/client.h>
#include <gloox/rostermanager.h>
#include <gloox/jid.h>
#include <gloox/message.h>
#include <gloox/mucroom.h>
#include <gloox/presence.h>
#include <gloox/subscription.h>

#ifdef WIN32
#include <direct.h>
#endif
#include <sys/stat.h>
#include <list>
#include <fstream>
#include <boost/program_options.hpp>
#include <boost/foreach.hpp>
#include <iostream>

#include "pichicore.h"
#include "helper.h"
#include "log.h"
#include "config.h"
#include "pichidbpatcher.h"
#ifdef WITH_TESTS
#include "tests.h"
#endif

namespace pichi
{

/*
 *	Only on linux 
 */
void Pichi::firstStart(void)
{
	Log("First Start.", Log::WARNING);
#ifndef WIN32
	// Creating ~
	std::string dir = Helper::getFullPath(PICHI_CONFIG_DIR);
	
	// construcor checked dir exist... all good
	Helper::createDirectory(dir);
	
	std::string cfgFile;
	if(Helper::fileExists(std::string(PICHI_INSTALLED_DIR) + "pichi.xml"))
		cfgFile = std::string(PICHI_INSTALLED_DIR) + "pichi.xml";
	else if(Helper::fileExists("/usr/share/pichi/config/pichi.xml"))
		cfgFile = "/usr/share/pichi/config/pichi.xml";
	else if(RUN_DIR.substr(RUN_DIR.size()-4) == "/bin" && Helper::fileExists(RUN_DIR.substr(0, RUN_DIR.size()-3) + "share/pichi/config/pichi.xml"))
		cfgFile = RUN_DIR.substr(0, RUN_DIR.size()-3) + "share/pichi/config/pichi.xml";
	else
		throw PichiException("Error: no config file founded... you must delete ~/.pichi and start bot again");
	
	std::ifstream ifs(cfgFile.c_str(), std::ios::binary);
	std::ofstream ofs((dir + "pichi.xml").c_str(), std::ios::binary);
	ofs << ifs.rdbuf();
#endif
}
  
void Pichi::botstart(void)
{
#ifdef WIN32
	UINT oldcodepage = GetConsoleOutputCP();
	SetConsoleOutputCP(65001);
#endif
	Log("Start Pichi", Log::INFO);
	if(pichi->getCfgOption("debug") == "1")
	{
		Log::LEVEL = Helper::atoi(pichi->getCfgOption("debug_level"));
		Log("Debug enabled at level: " + pichi->getCfgOption("debug_level"), Log::INFO);
	}
	if(pichi->getCfgOption("port") != "5222")
		client = new Client( jid, password, Helper::atoi(pichi->getCfgOption("port")) );
	else
		client = new Client( jid, password );
	// set my info
	client->disco()->setVersion("Pichi", PICHI_VERSION);
	client->setResource(pichi->getCfgOption("resource"));
	client->logInstance().registerLogHandler(LogLevelDebug, LogAreaAll, this);
	client->registerConnectionListener( this );
	client->registerMessageHandler( this );
	client->registerPresenceHandler( this );
	client->registerSubscriptionHandler( this );
	client->registerIqHandler( this, ExtVersion);
	roster = new RosterManager( client );
	BOOST_FOREACH( std::string room, Helper::explode(",", pichi->getCfgOption("room")) )
		enterRoom(JID(room + "/" + nick));
	times["white_ping"] = times["start"] = time(NULL);
	if(pthread_create(&thread, NULL, &Pichi::cron, (void*)this) > 0)
		throw PichiException("Error in cron thread");
	client->connect();
	if(!isConnected)
		Log("Pichi didn't connect to the xmpp server. Check connection settings.", Log::WARNING);
#ifdef WIN32
	SetConsoleOutputCP(oldcodepage);
	::system("pause");
#endif
	Log("Stop Pichi", Log::INFO);
}

int Pichi::start(int argc, char** argv)
{
	// Parsing args
	if(!parseArgs(argc, argv))
	{
		// Close Session
		return 1;
	}

#ifdef WITH_TESTS
	// Starting tests
	if(coptions.count("test"))
	{
		Tests::init();
		return Tests::runTest(
				      coptions["test"].as<std::string>(), 
				      ((coptions.count("testargs")) ? Helper::implode(" ", coptions["testargs"].as< std::vector< std::string > >()) : "") 
				     );
	}
#endif
  
	// Runtime dir
	char pathbuf[1024];
#ifndef WIN32
	if(getcwd(pathbuf, 1024) == NULL)
#else
	if(_getcwd(pathbuf, 1024) == NULL)
#endif
		throw PichiException("Start dir not founded");
	RUN_DIR = pathbuf;

	// Detect first start
	isFirstStart = false;
#ifndef WIN32
	// First start checks (create dir on linux)
	if(!Helper::fileExists(PICHI_CONFIG_DIR))
		isFirstStart = true;
#else
	if(!Helper::fileExists(std::string(PICHI_CONFIG_DIR) + "pichi.db"))
		isFirstStart = true;
#endif
	// First start
	if(isFirstStart)
		firstStart();
		
	// Init pichi
	isConnected = false; // true if connect was successful
	// Connect to Pichi core
	pichi = new PichiCore();
	pichi->jabber = this;
	// ----------
	// Staring DB structure
	initDBStruct();
	pichi->reloadSqlConfig();
  
	name = pichi->getCfgOption("user");
	nick = pichi->getCfgOption("room_user");
	password = pichi->getCfgOption("password");
	server = pichi->getCfgOption("server");
	jid = name + "@" + server;
	roomservice = pichi->getCfgOption("room_service");
	
	// игнорировать свой jid
	pichiIgnore.push_back(jid.full());
	
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
	
	return 0;
}

Pichi::~Pichi() throw()
{
	for(std::list< std::pair<JID, MUCRoom*> >::iterator it=rooms.begin(); it!=rooms.end(); it++)
		delete (it->second);
}


bool Pichi::parseArgs(int argc, char** argv)
{
	namespace arg = boost::program_options;
	
	arg::options_description description("Allowed options");
	description.add_options()
		("help,h", "Help message")
		("log,l", arg::value< std::string >()->default_value(Helper::getFullPath(PICHI_LOG_FILE)), "Log file path")
		("pid,p", arg::value< std::string >()->default_value(Helper::getFullPath(PICHI_PID_FILE)), "PID file path")
		("no-pid,n", "Start pichi without pid file")
#ifdef WITH_TESTS
		("test,t", arg::value< std::string >(), "Start test module")
		("testargs,a", arg::value< std::vector< std::string > >(), "Testings args")
#endif
		("daemonize,d", "Run Pichi as a daemon");
#ifdef WITH_TESTS
	arg::positional_options_description pdesc;
	pdesc.add("testargs", -1);
#endif
	try 
	{
		/* разбор аргументов */
#ifdef WITH_TESTS
		arg::store(arg::command_line_parser(argc, argv).options(description).positional(pdesc).run(), coptions);
#else
		arg::store(arg::parse_command_line(argc, argv, description), coptions);
#endif
	} 
	catch (const std::exception& e)
	{
		/* Неправильные аргументы */
		throw new PichiException(std::string("Command line error: ") + e.what());
	}
	
	if ( coptions.count("help") )
	{
		std::cout << description << std::endl;
		return false;
	}
	
	return true;
}

void Pichi::sendMessage(JID jid, const std::string& message)
{
	Message::MessageType type = Message::Chat;
	for(std::list< std::pair<JID, MUCRoom*> >::iterator it=rooms.begin(); it!=rooms.end(); it++)
	{
		if(jid.full() == it->first.bare())
		{
			type = Message::Groupchat;
			break;
		}
	}
	Message m( type, jid, message );
	client->send( m );
}

void Pichi::ban(const std::string& nick, JID room, std::string message)
{
	for(std::list< std::pair<JID, MUCRoom*> >::iterator it=rooms.begin(); it!=rooms.end(); it++)
	{
		if(it->first.bare() == room.full())
		{
			it->second->ban(nick, message);
		}
	}
}

void Pichi::unban(const std::string& nick, JID room, std::string message)
{
	for(std::list< std::pair<JID, MUCRoom*> >::iterator it=rooms.begin(); it!=rooms.end(); it++)
	{
		if(it->first.bare() == room.full())
		{
			it->second->setAffiliation(nick, AffiliationNone, message);
		}
	}
}

void Pichi::kick(const std::string& nick, JID room, std::string message)
{
	for(std::list< std::pair<JID, MUCRoom*> >::iterator it=rooms.begin(); it!=rooms.end(); it++)
	{
		if(it->first.bare() == room.full())
		{
			it->second->kick(nick, message);
		}
	}
}

// С ником
MUCRoom* Pichi::enterRoom(JID room)
{
	MUCRoom* newroom = new MUCRoom(client, room, this, NULL);
	rooms.push_back(std::pair<JID, MUCRoom*>(room.full(), newroom));
	
	// игнорировать свой jid
	pichiIgnore.push_back(room.full());
	// в список ников
	pichiNicks[room.bare()] = room.resource();
	
	return newroom;
}

void Pichi::leftRoom(JID room)
{
	for(std::list< std::pair<JID, MUCRoom*> >::iterator it=rooms.begin(); it!=rooms.end(); it++)
	{
		if(it->first.bareJID() == room || it->first.bareJID() == room.bare())
		{
			  // игнорировать свой jid
			pichiIgnore.remove(it->first.full());
		  
			// в список ников
			pichiNicks.erase(it->first.bare());
			
			it->second->leave();
			delete (it->second);
			rooms.erase(it);
			break;
		}
	}
}

bool pichi::Pichi::isRoom(const gloox::JID& room) const
{
	return std::find_if(rooms.begin(), rooms.end(), [&room](const std::pair<JID, MUCRoom*>& r){ return r.first.bare() == room.full(); }) != rooms.end();
}

void Pichi::onConnect()
{
	isConnected = true;
	for(std::list< std::pair<JID, MUCRoom*> >::iterator it=rooms.begin(); it!=rooms.end(); it++)
		it->second->join();
	times["wait"] = time(NULL);
	pichi->cleanUserInfo();
}

void Pichi::sendClientDetection(const gloox::JID& jid)
{
	IQ clientDetect(IQ::Get, jid);
	Tag* root = clientDetect.tag();
	Tag* query = new Tag("query");
	query->addAttribute("xmlns", XMLNS_VERSION);
	root->addChild(query);
	client->send(root);
}

bool Pichi::handleIq(const gloox::IQ& iq)
{
	// get version
	if(iq.findExtension(gloox::ExtVersion))
	{
		Tag* tag = iq.tag()->findChild("query");
		if(tag->hasChild("name") && tag->hasChild("version"))
		{
			  std::string client = tag->findChild("name")->cdata();
			  std::string version = tag->findChild("version")->cdata();
			  std::string os = "";
			  if(tag->hasChild("os"))
				os = tag->findChild("os")->cdata();
			  
			  // update ingnores for pichi
			  if(client == "Pichi" && std::find(pichiIgnore.begin(), pichiIgnore.end(), iq.from().full()) == pichiIgnore.end())
			  {
				Log("Other pichi detected... ignore: " + iq.from().full(), Log::DEBUG);
				pichiIgnore.push_back(iq.from().full());
			  }
			  
			  pichi->setUserClient(iq.from(), client, version, os);
		}
	}
	return true;
}

void Pichi::handleMessage( const Message& msg, MessageSession* session = 0 )
{
	if(!msg.tag()->hasChild("delay"))
	{
		pichi->cronDo("message");
		pichi->reciveMessage(msg.body(), "chat", msg.from().full());
	}
	else
	{
		Log("Ignore message with delay", Log::DEBUG);
	}
}

void Pichi::handleMUCMessage (MUCRoom *thisroom, const Message &msg, bool priv )
{
	if(!msg.tag()->hasChild("delay"))
	{
		pichi->cronDo("muc_message");
		pichi->reciveMessage(msg.body(), ((priv) ? "chatgroup" : "groupchat"), msg.from().full());
	}
	else
	{
		Log("Ignore message with delay", Log::DEBUG);
	}
}

void Pichi::handleMUCParticipantPresence (MUCRoom *thisroom, const MUCRoomParticipant  participant, const Presence &presence)
{
	
	std::string jid = "";
	std::string resource = "";
	std::string nick;
	std::string state;
	std::string room;
	std::string role;
	std::string status;
	
	if(participant.jid != NULL)
	{
		jid = participant.jid->bare();
		resource = participant.jid->resource();
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
	pichi->setUserInfo(jid, nick, state, room, role, status, resource);

}

void Pichi::handlePresence(const Presence& presence)
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

	pichi->setUserInfo(presence.from().bare(), "", state, "", "participant", presence.status(), presence.from().resource());
}


void Pichi::handleSubscription(const Subscription& subscription)
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


void Pichi::handleLog (LogLevel level, LogArea area, const std::string &message)
{
	Log("[GLOOX]" + message, Log::DEBUG);
}

void Pichi::initDBStruct(void)
{
	if(!Helper::fileExists(PICHI_CONFIG_DIR + pichi->getCfgOption("db_file")))
	{
		pichi->sql = new SQLite(Helper::getFullPath(PICHI_CONFIG_DIR) + pichi->getCfgOption("db_file"));
		PichiDbPather patch(pichi->sql, pichi->lang);
		patch.initDbStruct();
	}
	else
	{
		pichi->sql = new SQLite(Helper::getFullPath(PICHI_CONFIG_DIR) + pichi->getCfgOption("db_file"));
		PichiDbPather patch(pichi->sql, pichi->lang);
		patch.checkDbStruct();
	}
}

void Pichi::handleEvent(const Event& event)
{
	if(event.eventType() == Event::PingPong)
		pichi->pingRecive(event.stanza()->from().full());
}

gloox::JID& Pichi::getMyJID(void )
{
	return jid;
}

std::string Pichi::getMyNick(const std::string& room)
{
	if(room == "")
		return pichiNicks[pichi->getLastRoom()];
	else
		return pichiNicks[room];
}

void *Pichi::cron(void *context)
{
	while(true)
	{
		if(((Pichi *)context)->isConnected)
		{
			if(time(NULL) - ((Pichi *)context)->times["white_ping"] > 5 * 60)
			{
				((Pichi *)context)->times["white_ping"] = time(NULL);
				Log("[CRON] Ping of live", Log::VERBOSE);
		
				((Pichi *)context)->client->whitespacePing();
			}
			((Pichi *)context)->pichi->cronDo("cron");
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
