/*
    <one line to give the program's name and a brief idea of what it does.>
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

#include "pichicurl.h"

pichicurl::pichicurl()
{
	curl = curl_easy_init();

	curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errorBuffer);  
	curl_easy_setopt(curl, CURLOPT_HEADER, 0);  
	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);  
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, pichicurl::writer);  
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);
}

pichicurl::~pichicurl()
{
	curl_easy_cleanup(curl);
}

int pichicurl::writer(char* data, size_t size, size_t nmemb, std::string* buf)
{
	int result = 0;  
    
	buf->clear();
	buf->append(data, size * nmemb);  
	result = size * nmemb;  
  
	return result;
}

std::string pichicurl::readurl(std::string u)
{
	setUrl(u);
	return read();
}

std::string pichicurl::read(void )
{
	curl_easy_perform(curl);
	if (result == CURLE_OK)
		return buffer;
	else
		return std::string();
}

void pichicurl::setUrl(std::string u)
{
	url = u;
	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
}

void pichicurl::setReferer(std::string u)
{
	curl_easy_setopt(curl, CURLOPT_REFERER, u.c_str());
}

std::string pichicurl::urlencode(std::string u)
{
	return curl_easy_escape(curl, u.c_str(), u.length());
}

std::string pichicurl::urldecode(std::string u)
{
	int size;
	return curl_easy_unescape(curl, u.c_str(), u.length(), &size);
}
