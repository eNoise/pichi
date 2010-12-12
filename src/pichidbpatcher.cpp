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

#include "pichidbpatcher.h"

namespace pichi
{
  
PichiDbPather::PichiDbPather(pichi::sqlite* sql, languages* lang)
{
	this->sql = sql;
	this->lang = lang;
	db_version = 0; //defaut
}

void PichiDbPather::checkDbStruct(void )
{
	patch();
	sql->exec("UPDATE settings SET description = '" + sql->escapeString((*lang)["db_configdesc_answer_mode"]) + "' WHERE name = 'answer_mode';");
	sql->exec("UPDATE settings SET description = '" + sql->escapeString((*lang)["db_configdesc_answer_only_nick"]) + "' WHERE name = 'answer_only_nick';");
	sql->exec("UPDATE settings SET description = '" + sql->escapeString((*lang)["db_configdesc_answer_random"]) + "' WHERE name = 'answer_random';");
	sql->exec("UPDATE settings SET description = '" + sql->escapeString((*lang)["db_configdesc_answer_remember"]) + "' WHERE name = 'answer_remember';");
	sql->exec("UPDATE settings SET description = '" + sql->escapeString((*lang)["db_configdesc_answer_word_limit"]) + "' WHERE name = 'answer_word_limit';");
	sql->exec("UPDATE settings SET description = '" + sql->escapeString((*lang)["db_configdesc_log_enabled"]) + "' WHERE name = 'log_enabled';");
	sql->exec("UPDATE settings SET description = '" + sql->escapeString((*lang)["db_configdesc_treatment_coincidence"]) + "' WHERE name = 'treatment_coincidence';");
	sql->exec("UPDATE settings SET description = '" + sql->escapeString((*lang)["db_configdesc_rand_message"]) + "' WHERE name = 'rand_message';");
	sql->exec("UPDATE settings SET description = '" + sql->escapeString((*lang)["db_configdesc_msg_limit"]) + "' WHERE name = 'msg_limit';");
	sql->exec("UPDATE settings SET description = '" + sql->escapeString((*lang)["db_configdesc_msg_max_limit"]) + "' WHERE name = 'msg_max_limit';");
}

void PichiDbPather::initDbStruct(void )
{
	sql->exec("CREATE TABLE log (`jid` TEXT, `room` TEXT, `from` TEXT, `time` TEXT, `type` TEXT, `message` TEXT);");
	sql->exec("CREATE TABLE lexems (`lexeme` TEXT UNIQUE, `count` INT NOT NULL DEFAULT '0');");
	sql->exec("CREATE TABLE wiki (`name` TEXT UNIQUE, `revision` INT NOT NULL DEFAULT '0', `value` TEXT);");
	sql->exec("CREATE TABLE settings (`name` TEXT UNIQUE, `value` TEXT, `description` TEXT);");
	sql->exec("CREATE TABLE users (`jid` TEXT, `nick` TEXT, `role` TEXT, `room` TEXT, `time` TEXT, `status` TEXT, `level` INT NOT NULL DEFAULT '1');");
	sql->exec("CREATE TABLE users_data (`jid` TEXT, `name` TEXT, `value` TEXT, `groupid` TEXT);");
	sql->exec("CREATE TABLE users_nick (`jid` TEXT, `nick` TEXT, `room` TEXT, `time` TEXT);");
	sql->exec("CREATE TABLE stats (`name` TEXT UNIQUE, `value` TEXT);");
	sql->exec("CREATE TABLE actions (`action` TEXT, `coincidence` TEXT, `do` TEXT, `option` TEXT, `value` TEXT);");
	sql->exec("CREATE TABLE db_version (`version` TEXT, `value` TEXT);");
  
	sql->exec("INSERT INTO db_version (`version`) VALUES ('" + boost::lexical_cast<std::string>(PICHI_DB_VERSION_ACTUAL) + "');");
	sql->exec("INSERT INTO settings (`name`, `value`, `description`) VALUES ('answer_mode','1','" + sql->escapeString((*lang)["db_configdesc_answer_mode"]) + "');"); // Отвечать после сообщений пользователей
	sql->exec("INSERT INTO settings (`name`, `value`, `description`) VALUES ('answer_only_nick','0','" + sql->escapeString((*lang)["db_configdesc_answer_only_nick"]) + "');"); // отвечать только при нике в чате
	sql->exec("INSERT INTO settings (`name`, `value`, `description`) VALUES ('answer_random','0','" + sql->escapeString((*lang)["db_configdesc_answer_random"]) + "');"); // отвечать не всегда (0 - всегда)
	sql->exec("INSERT INTO settings (`name`, `value`, `description`) VALUES ('answer_remember','1','" + sql->escapeString((*lang)["db_configdesc_answer_remember"]) + "');"); // запоминать и разбивать на лексемы
	sql->exec("INSERT INTO settings (`name`, `value`, `description`) VALUES ('answer_word_limit','10','" + sql->escapeString((*lang)["db_configdesc_answer_word_limit"]) + "');"); // limit для запросов в лексемах
	sql->exec("INSERT INTO settings (`name`, `value`, `description`) VALUES ('log_enabled','1','" + sql->escapeString((*lang)["db_configdesc_log_enabled"]) + "');"); // вести лог ?
	sql->exec("INSERT INTO settings (`name`, `value`, `description`) VALUES ('treatment_coincidence','3','" + sql->escapeString((*lang)["db_configdesc_treatment_coincidence"]) + "');"); // вставлять обращение, совпадения (3 из 1)
	sql->exec("INSERT INTO settings (`name`, `value`, `description`) VALUES ('rand_message','0','" + sql->escapeString((*lang)["db_configdesc_rand_message"]) + "');"); // случайны ответ когда скучно
	sql->exec("INSERT INTO settings (`name`, `value`, `description`) VALUES ('msg_limit','500','" + sql->escapeString((*lang)["db_configdesc_msg_limit"]) + "');"); // лимит символов, после чего отправляет ответ в личку
	sql->exec("INSERT INTO settings (`name`, `value`, `description`) VALUES ('msg_max_limit','0','" + sql->escapeString((*lang)["db_configdesc_msg_max_limit"]) + "');"); // вверхний предел
}

void PichiDbPather::patch(void )
{
	// Begin patch
	switch(getDbVersion())
	{
		// Ниже 21 версии не патчит
	}
	
	if(getDbVersion() != PICHI_DB_VERSION_ACTUAL)
		throw PichiException("Pichi Database version discrepancy. Try to patch it manually");
}

void PichiDbPather::upVersion(int v)
{
	sql->exec("UPDATE db_version SET version = " + boost::lexical_cast<std::string>(v));
	db_version = v;
}

int PichiDbPather::getDbVersion(void )
{
	if(db_version > 0)
		return db_version;
	
	sql->query("SELECT version FROM db_version;");
	db_version = boost::lexical_cast<int>(sql->fetchColumn(0));
	return db_version;
}

  
}