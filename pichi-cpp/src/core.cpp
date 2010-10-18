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

#include "core.h"

void core::botstart(void)
{
#ifdef WIN32
	std::setlocale(LC_ALL, ".ACP");
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
	BOOST_FOREACH( std::string room, system::explode(",", pichi->getConfigOption("room")) )
		enterRoom(JID(room + "/" + nick));
	client->connect();
}

core::core()
{
  	// Init pichi
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

	botstart();
}

core::~core()
{
	for(std::list< std::pair<JID, MUCRoom*> >::iterator it=rooms.begin(); it!=rooms.end(); it++)
		delete (it->second);
	delete client;
	delete pichi;
}


void core::sendMessage(JID jid, std::string message)
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

void core::enterRoom(JID room)
{
	MUCRoom* newroom = new MUCRoom(client, room, this, NULL);
	rooms.push_back(std::pair<JID, MUCRoom*>(room,newroom));
}

void core::leftRoom(JID room)
{
	for(std::list< std::pair<JID, MUCRoom*> >::iterator it=rooms.begin(); it!=rooms.end(); it++)
	{
		if(it->first == room)
		{
			it->second->leave();
			delete (it->second);
		}
	}
}

void core::onConnect()
{
	for(std::list< std::pair<JID, MUCRoom*> >::iterator it=rooms.begin(); it!=rooms.end(); it++)
		it->second->join();
	pichi->wait = time(NULL);
	pichi->cleanUserInfo();
}

void core::handleMessage( const Message& msg, MessageSession* session = 0 )
{
	std::cout << msg.body()  << std::endl;
	pichi->reciveMessage(msg.body(), "chat", msg.from().full());
}

void core::handleMUCMessage (MUCRoom *thisroom, const Message &msg, bool priv )
{
	std::cout << msg.body()  << std::endl;
	pichi->reciveMessage(msg.body(), "groupchat", msg.from().full());
}

void core::handleMUCParticipantPresence (MUCRoom *thisroom, const MUCRoomParticipant  participant, const Presence &presence)
{
	std::string role;
	if(participant.role == 3)
		role = "moderator";
	else
		role = "participant";
	
	if(presence.presence() != Presence::Unavailable)
	{
		LOG("Пользователь в комнате онлайн: " + participant.jid->full(), LOG::INFO);
		pichi->setUserInfo(participant.jid->bare(), participant.nick->resource(), "available", participant.nick->bare(), role, participant.status);
	}
	else
	{
		LOG("Пользователь вышел из комнаты: " + participant.jid->full(), LOG::INFO);
		pichi->setUserInfo(participant.jid->bare(), participant.nick->resource(), "unavailable", participant.nick->bare(), role, participant.status);
	}
}

void core::handlePresence(const Presence& presence)
{
	if(presence.presence() != Presence::Unavailable)
	{
		LOG("Пользователь онлайн: " + presence.from().bare(), LOG::INFO);
		pichi->setUserInfo(presence.from().bare(), "", "available", "", "participant", presence.status());
	}
	else
	{
		LOG("Пользователь вышел: " + presence.from().bare(), LOG::INFO);
		pichi->setUserInfo(presence.from().bare(), "", "unavailable", "", "participant", presence.status());
	}
}


void core::handleLog (LogLevel level, LogArea area, const std::string &message)
{
	LOG("[GLOOX]" + message, LOG::DEBUG);
}

void core::initDBStruct(void)
{
	if(!system::fileExists(pichi->getConfigOption("db_file")))
	{
		pichi->sql = new sqlite(pichi->getConfigOption("db_file"));
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
		pichi->sql->exec("INSERT INTO settings (`name`, `value`, `description`) VALUES ('answer_mode','1','Режим ответа на сообщения. [0 - выключить; 1 - включить][По умолчанию: 1]');"); // Отвечать после сообщений пользователей
		pichi->sql->exec("INSERT INTO settings (`name`, `value`, `description`) VALUES ('answer_random','0','Не всегда ответь при получении сообщения. [0 - всегда отвечать; >100 фактически всегда молчать][По умолчанию: 0]');"); // отвечать не всегда (0 - всегда)
		pichi->sql->exec("INSERT INTO settings (`name`, `value`, `description`) VALUES ('answer_remember','1','Разбивать на связки слов. [0 - выключить; 1 - включить][По умолчанию: 1]');"); // запоминать и разбивать на лексемы
		pichi->sql->exec("INSERT INTO settings (`name`, `value`, `description`) VALUES ('answer_word_limit','10','Максимальное количество связок слов в расмотрении. Влияет на алгоритм построения фраз, так же как и на нагрузку. [Рекомендуется >3 и <50][По умолчанию: 10]');"); // limit для запросов в лексемах
		pichi->sql->exec("INSERT INTO settings (`name`, `value`, `description`) VALUES ('log_enabled','1','Вести лог? [0 - выключить; 1 - включить][По умолчанию: 1]');"); // вести лог ?
		pichi->sql->exec("INSERT INTO settings (`name`, `value`, `description`) VALUES ('treatment_coincidence','3','Вероятность вставки обращений. [1 - всегда; >100 фактически никогда][По умолчанию: 3]');"); // вставлять обращение, совпадения (3 из 1)
		pichi->sql->exec("INSERT INTO settings (`name`, `value`, `description`) VALUES ('rand_message','0','Переодически отправлять случайные фразы в главный чат. [0 - выключить; 1 - включить][По умолчанию: 0]');"); // случайны ответ когда скучно
		pichi->sql->exec("INSERT INTO settings (`name`, `value`, `description`) VALUES ('msg_limit','500','Максимальное количество символов, допустимое в главном чате (в противном случае пишет в личку) [По умолчанию: 500]');"); // лимит символов, после чего отправляет ответ в личку
		pichi->sql->exec("INSERT INTO settings (`name`, `value`, `description`) VALUES ('msg_max_limit','0','Верхний предел для сообщения, после которого сообщение разобьет на несколько [По умолчанию: 0]');"); // вверхний предел
	}
	else
	{
		pichi->sql = new sqlite(pichi->getConfigOption("db_file"));
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
