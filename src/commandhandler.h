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

#ifndef COMMANDHANDLER_H
#define COMMANDHANDLER_H

#include <string>
#include <iostream>
#include <pthread.h>
#include "system.h"
#include <algorithm>
#include "pichimessage.h"

namespace pichi
{

class PichiCore;

class commandhandler
{
  public:
    commandhandler(PichiCore* p);
    ~commandhandler();
    void fetchCommand(std::string command);
    void operator () (std::string command);
  protected:
    PichiCore* pichi;
    pthread_t remotethread;
    std::map< std::string, PichiMessage > thread_args;
    void updateThreadVars(std::string part = "");
    std::string last_command;
    std::string last_args;
    std::vector< std::string > seperate(std::string str, int level = 2);
    bool testArgs(const std::vector< std::string >& args, int num);
};

}

#endif // COMMANDHANDLER_H
