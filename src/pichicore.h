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

#ifndef PICHICORE_H
#define PICHICORE_H

#include "sqlite.h"
#include "pichiconfig.h"
#include "pichioptions.h"
#include "lastmessage.h"
#include "system.h"
#include <string>
#include <vector>
#include <time.h>
#include <gloox/jid.h>
#include "commandbase.h"
#include "languages.h"
#include "lexemes.h"
#include "pichievent.h"
//#include <boost/timer.hpp>

namespace pichi
{

class core;

class pichicore : public pichiconfig, 
		  public lastmessage, 
		  public pichioptions
{
	public:
		sqlite *sql;
		core *jabber;
		commandbase *commander;
		languages *lang;
		lexemes *lex;
		PichiEvent *event;
		
		time_t wait_time;
		std::map<std::string, std::string> reciver;
		//boost::timer timer;
		
		void on(void);
		void off(void);
		
		void cronDo(std::string eventer = "");
		bool canCron(std::string crn);
		std::map< std::string, std::map< std::string, time_t > > crons;
		
		bool isEnabled(void);
		void setUserInfo(std::string, std::string, std::string, std::string, std::string, std::string);
		void cleanUserInfo(void);
		bool reciveMessage(const std::string& message, const std::string& type, const std::string& from);
		void sendAnswer(const std::string& message);
		void sendAnswer(const std::string& message, const lastmessage& msg);
		
		bool isJID(const std::string& jid);
		bool isBareJID(const std::string& jid);
		std::string getJIDfromNick(const std::string& nick, std::string room, bool all_rooms = false);
		std::string getJIDfromNicks(const std::string& nick, std::string room, bool all_rooms = false);
		std::string getNickFromJID(const std::string& jid, std::string room, bool all_rooms = false);
		std::string getJIDpart(const std::string& jid, unsigned int part);
		std::string getArgJID(const std::string& arg);
		std::string getArgNick(const std::string& arg);
		
		std::string getDefaultRoom(void);
		bool isCommand(std::string& str);
		bool isAccess(std::string jid, std::string room, int level = 2);
		bool isAccess(int level = 2);
		bool setOption(std::string option, std::string value);
		bool isOnline(std::string user, std::string room);
		bool isOnline(std::string jid);
		void ping(std::string jid);
		void pingRecive(std::string id);
		
		void setJIDinfo(std::string jid, std::string name, std::string value, std::string groupid = "");
		std::map<std::string, std::string> getJIDinfo(std::string jid, std::string name = "", std::string groupid = "");
		void delJIDinfo(std::string jid, std::string name = "", std::string groupid = "");
		
		void ban(std::string jid, std::string time = "", std::string reason = "", std::string room = "");
		void unban(std::string jid, std::string reason = "", std::string room = "");
		void kick(std::string jid, std::string time = "", std::string reason = "", std::string room = "");
		void unkick(std::string jid, std::string room = "");
		
		time_t convertTime(std::string time);
		
		lastmessage& operator=(const pichicore& lst);
		
		pichicore();
		~pichicore();
	protected:
		bool enabled;
	private:
};

}

#endif // PICHICORE_H
