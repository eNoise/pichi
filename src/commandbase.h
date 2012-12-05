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

#ifndef COMMANDBASE_H
#define COMMANDBASE_H

#include "commandhandler.h"
#include <map>
#include <string>
#include <algorithm>
#include "pichicurl.h"
#include "config.h"
#include "pichidbpatcher.h"

#include "json_parser_read.hpp"
#include <boost/property_tree/json_parser.hpp>
#include <boost/algorithm/string/erase.hpp>

#define TR(x) (*(pichi->lang))(x)
#define TR2(x,y) (*(pichi->lang))(x,y)
#define TR3(x,y,z) (*(pichi->lang))(x,y,z)
#define TR4(x,y,z,v) (*(pichi->lang))(x,y,z,v)
#define TR5(x,y,z,v,n) (*(pichi->lang))(x,y,z,v,n)

namespace pichi
{

class PichiCore;

class commandbase : public commandhandler
{
  public:
    commandbase(PichiCore* p);
    void fetchCommand(std::string command);
        
    protected:
	  std::map<std::string, void (commandbase::*)(std::string)> commands;
	  
	  void command_version(std::string);
	  void command_info(std::string arg);
	  void command_help(std::string arg);
	  void command_enable(std::string arg);
	  void command_disable(std::string arg);
	  void command_reload(std::string arg);
	  void command_plugins(std::string arg);
	  void command_ban(std::string arg);
	  void command_unban(std::string arg);
	  void command_kick(std::string arg);
	  void command_unkick(std::string arg);
	  void command_banlist(std::string arg);
	  void command_kicklist(std::string arg);
	  void command_log(std::string arg);
	  void command_wtf(std::string arg);
	  void command_wtfcount(std::string arg);
	  void command_wtfrand(std::string arg);
	  void command_wtfrev(std::string arg);
	  void command_wtfull(std::string arg);
	  void command_wtfset(std::string arg);
	  void command_top(std::string arg);
	  void command_talkers(std::string arg);
	  void command_count(std::string arg);
	  void command_dfn(std::string arg);
	  void command_set(std::string arg);
	  void command_msg(std::string arg);
	  void command_gc(std::string arg);
	  void command_users(std::string arg);
	  void command_join(std::string arg);
	  void command_left(std::string arg);
	  void command_ping(std::string arg);
	  void command_topic(std::string arg);
	  void command_nicks(std::string arg);
	  void command_idle(std::string arg);
	  void command_q(std::string arg);
	  void command_greet(std::string arg);
	  void command_farewell(std::string arg);
	  void command_quit(std::string arg);
	  void command_on(std::string arg);
	  void command_off(std::string arg);
	  void command_uptime(std::string arg);
	  
	  void command_lastfm(std::string arg);
	  static void *thread_lastfm(void *context);
	  void command_lastfm_user(std::string arg);
	  
	  static void *thread_googletranslate(void *context);
	  void command_translate(std::string arg);
	  void command_tr(std::string arg);
	  void command_translate_language(std::string arg);
	  void command_google(std::string arg);
	  static void *thread_google(void *context);
	  
	  void command_urlshort(std::string arg);
	  static void *thread_urlshort(void *context);
};

}
#endif // COMMANDBASE_H
