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

#ifndef PICHICURL_H
#define PICHICURL_H

#include <curl/curl.h>
#include <string>

namespace pichi
{

class pichicurl
{
  public:
    pichicurl();
    ~pichicurl();
    std::string readurl(const std::string& u);
    std::string read(void);
    void setUrl(const std::string& u);
    void setReferer(const std::string& u);
    void setAuth(const std::string& user, const std::string& pass);
    void setPostArgs(const std::string& args);
    const std::string urlencode(const std::string& u) const;
    const std::string urldecode(const std::string& u) const;
  private:
	std::string* postargs;
	char errorBuffer[CURL_ERROR_SIZE];
	std::string buffer;
	CURL *curl;  
	CURLcode result;
	std::string url;
	static int writer(char *data, size_t size, size_t nmemb,  
                  std::string *buffer);
};

}

#endif // PICHICURL_H
