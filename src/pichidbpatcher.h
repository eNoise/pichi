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

#ifndef PICHIDBPATCHER_H
#define PICHIDBPATCHER_H

#define PICHI_DB_VERSION_ACTUAL 25

namespace pichi
{
  class SQLite;
  class languages;
  
class PichiDbPather
{ 
public:
  PichiDbPather(SQLite* sql, languages* lang);
  void checkDbStruct(void);
  void initDbStruct(void);
  int getDbVersion(void);
private:
  SQLite* sql;
  languages* lang;
  int db_version;
  void patch(void);
  void upVersion(const int v);
};
  
}

#endif // PICHIDBPATCHER_H
