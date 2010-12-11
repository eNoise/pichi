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

#ifndef PICHIDBPATCHER_H
#define PICHIDBPATCHER_H

#include "sqlite.h"
#include "languages.h"
#include "log.h"
#include "pichiexception.h"

#define PICHI_DB_VERSION_ACTUAL 20

namespace pichi
{
  
class PichiDbPather
{
public:
  PichiDbPather(sqlite* sql, languages* lang);
  void checkDbStruct(void);
  void initDbStruct(void);
  int getDbVersion(void);
private:
  sqlite* sql;
  languages* lang;
  int db_version;
  void patch(void);
  void upVersion(int v);
};
  
}

#endif // PICHIDBPATCHER_H