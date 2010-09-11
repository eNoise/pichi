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

#ifndef CORE_H
#define CORE_H

#include <gloox/client.h>
#include <gloox/messagehandler.h>
#include <gloox/presencehandler.h>
#include <gloox/message.h>
#include <gloox/mucroom.h>
#include <gloox/mucroomhandler.h>
#include <gloox/loghandler.h>
#include <gloox/connectionlistener.h>
#include <gloox/presence.h>
#include <iostream>
#include <list>
#include <time.h>

#include "pichicore.h"
#include "system.h"

using namespace gloox;

class core : public MessageHandler, MUCRoomHandler, LogHandler, ConnectionListener, PresenceHandler
{
	private:
		std::string name;
		std::string nick;
		std::string password;
		std::string server;
		std::string port;
		std::string roomname;
                std::string roomservice;
		JID jid;
		JID roomjid;
	protected:
		pichicore* pichi;
	  	Client* client;
		
		void botstart(void);
		void initDBStruct(void);
	public:
		core(std::string n, std::string p, std::string s);
                ~core();
		
		std::list< std::pair<JID, MUCRoom*> > rooms;
		
		void sendMessage(JID jid, std::string message);
		void enterRoom(JID room);
		void leftRoom(JID room);
		
		// stubs
                void onConnect();
                void onDisconnect  (ConnectionError e){}
                bool onTLSConnect  (const CertInfo  &info){return true;}

                void handleMessage( const Message& msg, MessageSession* session);
                void handleMUCMessage (MUCRoom *thisroom, const Message &msg, bool priv );

		// room enter
                void handleMUCParticipantPresence (MUCRoom *thisroom, const MUCRoomParticipant  participant, const Presence &presence);
                
                
                bool handleMUCRoomCreation  (MUCRoom  *thisroom){return true; }
                void handleMUCSubject (MUCRoom *thisroom, const std::string &nick, const std::string &subject){}
                void handleMUCInviteDecline (MUCRoom *thisroom, const JID &invitee, const std::string &reason){}
                void handleMUCError (MUCRoom *thisroom, StanzaError error){}
                void handleMUCInfo (MUCRoom *thisroom, int features, const std::string &name, const DataForm *infoForm){}
                void handleMUCItems (MUCRoom *thisroom, const Disco::ItemList &items){}

                void handleLog (LogLevel level, LogArea area, const std::string &message);
		
		void handlePresence(const Presence& presence );

};

#endif // CORE_H
