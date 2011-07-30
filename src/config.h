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


#ifndef CONFIG_H
#define CONFIG_H

#define PICHI_VERSION std::string(GIT_VERSION) + " (" + std::string(GIT_DESCRIBE) + ") [r" + std::string(GIT_REVISION) + "]"

#ifndef WIN32
#define PICHI_CONFIG_DIR "~/.pichi/"
#else
#define PICHI_CONFIG_DIR ""
#endif

#define PICHI_PID_FILE "/var/run/pichi.pid"
#define PICHI_LOG_FILE "~/.pichi/pichi.log"

#define PICHI_TWITTER_KEY "cVAYap0FSuCYsi2H5z4lA"
#define PICHI_TWITTER_SECRET "nR4o0ffJ3Wa30x8RLb9KF1QumbEISQ1xQ2h4cel5oNQ"

#endif // CONFIG_H
