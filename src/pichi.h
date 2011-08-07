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

#ifndef PICHI_H
#define PICHI_H

#include <string>
#include <pthread.h>
#include <boost/program_options/variables_map.hpp>

#include <gloox/messagehandler.h>
#include <gloox/presencehandler.h>
#include <gloox/connectionlistener.h>
#include <gloox/loghandler.h>
#include <gloox/eventhandler.h>
#include <gloox/subscriptionhandler.h>
#include <gloox/mucroomhandler.h>

namespace gloox {
    class Client;
    class RosterManager;  
};

namespace pichi
{

static std::string RUN_DIR; // Runtime Directory

class PichiCore;

using namespace gloox;

class Pichi : 	public MessageHandler, public MUCRoomHandler, public LogHandler,
		public ConnectionListener, public PresenceHandler, public EventHandler,
		public SubscriptionHandler
{
	private:
		bool isConnected;
		bool isFirstStart;
	  
		std::string name;
		std::string nick;
		std::string password;
		std::string server;
		std::string port;
                std::string roomservice;
		JID jid;
		
		
		RosterManager* roster;
	protected:
		PichiCore* pichi;
	  
		pthread_t thread;
		void botstart(void);
		void initDBStruct(void);
		static void *cron(void *context);
		void firstStart(void);
	public:
		Pichi(int argc, char** argv);
                ~Pichi() throw();
		bool parseArgs(int argc, char** argv);
		
		boost::program_options::variables_map coptions;
		
		Client* client;
		std::list< std::pair<JID, MUCRoom*> > rooms;
		std::map< std::string, time_t > times;
		std::list< std::string > pichiIgnore; // игорировать JID'ы пичи
		std::map< std::string, std::string > pichiNicks;
		
		void sendMessage(JID jid, const std::string& message);
		MUCRoom* enterRoom(JID room);
		void leftRoom(JID room);
		JID& getMyJID(void);
		std::string getMyNick(const std::string& room = "");
		void kick(const std::string &nick, JID room, std::string message = "");
		void ban(const std::string &nick, JID room, std::string message = "");
		void unban(const std::string &nick, JID room, std::string message = "");
		
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
		
		void handleSubscription( const Subscription& subscription );
		
		void handleEvent (const Event &event);

};

}

#endif // PICHI_H
