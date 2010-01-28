<?php

if(function_exists("date_default_timezone_set") and function_exists("date_default_timezone_get"))
	@date_default_timezone_set(@date_default_timezone_get()); //disable timezone errors

$config['db_version'] = 10; // Work only parram

require_once("XMPP/XMPP.php");
require_once("command_handler.php");
require_once("Log_pichi.php");
include("config.php");

$log = new PichiLog($config['debug'], 5);

function php_extension_load($ext)
{
	if (!extension_loaded($ext))
	{
		if (strtoupper(substr(PHP_OS, 0, 3)) === 'WIN')
		{
			if(!dl("php_$ext.dll"))
			{
				$log->log("Extension $ext not loaded. Enable it.",PichiLog::LEVEL_ERROR);
				exit();
			}
		}
		else
		{
			if(!dl("$ext.so"))
			{
				$log->log("Extension $ext not loaded. Enable it.",PichiLog::LEVEL_ERROR);
				exit();
			}
		}
	}
}

php_extension_load("sqlite3");
php_extension_load("mbstring");
php_extension_load("openssl");

function isRoom($test)
{
	global $config;
	return (strpos($test, "@") !== FALSE) && (strpos($test, $config['room_service']) !== FALSE) && (strpos($test, $config['server']) !== FALSE);
}

// init
$log->log("Start Pichi",PichiLog::LEVEL_INFO);
$jabber = new XMPPHP_XMPP($config['server'], $config['port'], $config['user'], $config['password'], $config['resource'], $config['server'], $printlog=false, $loglevel = (($config['debug']) ? XMPPHP_Log::LEVEL_VERBOSE : XMPPHP_Log::LEVEL_INFO));
try
{
	$log->log("Try to connect...",PichiLog::LEVEL_VERBOSE);
	$jabber->connect();
}

catch(XMPPHP_Exception $e)
{
	$log->log("Connection fail with error: $e",PichiLog::LEVEL_ERROR);
	exit();
}

// connect
$log->log("Connection success",PichiLog::LEVEL_VERBOSE);

if(file_exists($config['db_file']))
{
	$log->log("There is no db file.",PichiLog::LEVEL_DEBUG);
	$db_exist = TRUE;
}
else
{
	$log->log("Db file founded",PichiLog::LEVEL_DEBUG);
	$db_exist = FALSE; 
}
$log->log("Creating CommandHandler",PichiLog::LEVEL_VERBOSE);
$command_handler = new commandHandler($jabber);
$command_handler->log = $log;
$command_handler->room = $config['room'];
$command_handler->user = $config['user'];
$command_handler->server = $config['server'];
$command_handler->room_service = $config['room_service'];
$command_handler->room_user = $config['room_user'];
$command_handler->ignore[] = $config['user'] . "@" . $config['server'];
$log->log("done!",PichiLog::LEVEL_VERBOSE);

if(!$db_exist)
{
	$log->log("Creating database structure",PichiLog::LEVEL_DEBUG);
	$command_handler->db->query("CREATE TABLE log (`from` TEXT, `time` TEXT, `type` TEXT, `message` TEXT);");
	$command_handler->db->query("CREATE TABLE lexems (`lexeme` TEXT, `count` INT);");
	$command_handler->db->query("CREATE TABLE wiki (`name` TEXT, `value` TEXT);");
	$command_handler->db->query("CREATE TABLE settings (`name` TEXT, `value` TEXT);");
	$command_handler->db->query("CREATE TABLE users (`jid` TEXT, `nick` TEXT, `room` TEXT, `time` TEXT, `status` TEXT);");
	$command_handler->db->query("CREATE TABLE stats (`name` TEXT, `value` TEXT);");
	$command_handler->db->query("CREATE TABLE db_version (`version` TEXT, `value` TEXT);");
  
	$command_handler->db->query("INSERT INTO db_version (`version`) VALUES ('" . $config['db_version'] . "');");
	$command_handler->db->query("INSERT INTO settings (`name`, `value`) VALUES ('answer_mode','1');"); // Отвечать после сообщений пользователей
	$command_handler->db->query("INSERT INTO settings (`name`, `value`) VALUES ('answer_random','0');"); // отвечать не всегда
	$command_handler->db->query("INSERT INTO settings (`name`, `value`) VALUES ('answer_remember','1');"); // запоминать и разбивать на лексемы
	$command_handler->db->query("INSERT INTO settings (`name`, `value`) VALUES ('log_enabled','1');"); // вести лог ?
	$command_handler->db->query("INSERT INTO settings (`name`, `value`) VALUES ('treatment_coincidence','3');"); // вставлять обращение, совпадения (3 из 1)
	$command_handler->db->query("INSERT INTO settings (`name`, `value`) VALUES ('rand_message','0');"); // случайны ответ когда скучно
	$command_handler->db->query("INSERT INTO settings (`name`, `value`) VALUES ('msg_limit','500');"); // лимит символов, после чего отправляет ответ в личку
	$log->log("done",PichiLog::LEVEL_DEBUG);
}

$command_handler->db->query("SELECT * FROM db_version;");
$current_db_version = $command_handler->db->fetchColumn(0);
if($current_db_version < $config['db_version'])
{
	$log->log("Old database!",PichiLog::LEVEL_ERROR);
	$log->log("Your database version: $current_db_version. But needed: ". $config['db_version'],PichiLog::LEVEL_ERROR);
	exit();
}

$command_handler->parseOptions(); // загнать все опции
$log->log("Sync Timers",PichiLog::LEVEL_VERBOSE);
$time_message = $time_ping = time(); // сбрасываем
$log->log("Begin Session",PichiLog::LEVEL_VERBOSE);
while(!$jabber->isDisconnected()) {
	$payloads = $jabber->processUntil(array('message', 'presence', 'end_stream', 'session_start', 'ping'), 1);
	//wait for proccess
    
	foreach($payloads as $event) 
	{
		$data = $event[1];
		switch($event[0]) 
		{
			case 'message':
				$time_message = time();
				$log->log("Recive MESSAGE Handler From $data[body]($data[type]):\nMessage: $data[body]",PichiLog::LEVEL_DEBUG);
				$command_handler->do_if_message($data['body'], $data['from'], $data['type']);
				break;
			case 'ping':
				$command_handler->do_if_ping($data['id']);
				break;
			case 'presence':
				$log->log("Recive PRESENCE Handler from: {$data['from']} [{$data['show']}] {$data['status']}",PichiLog::LEVEL_DEBUG);
				if($data['show'] == 'available' || $data['show'] == 'unavailable')
				{
					unset($jid);
					$nick = $command_handler->getName($data['from']);
					foreach($data['xml']->subs as $x)
						if($x->name == 'x' && $x->subs[0]->attrs['jid'])
							$jid = $command_handler->getJID($x->subs[0]->attrs['jid']);
					if(!$jid)
						break;

					$room = $command_handler->getJID($data['from']);
					if(isRoom($room))
						$command_handler->setUserInfo($jid, $nick, $room, $data['show']);
					else
						$command_handler->setUserInfo($jid, $nick, NULL, $data['show']);
				}
				break;
			case 'session_start':
				$log->log("Recive SESSION_START Handler",PichiLog::LEVEL_DEBUG);
				$command_handler->db->query("UPDATE users SET status = 'unavailable';");
				$jabber->getRoster();
				$jabber->presence($config['status']);
				$command_handler->joinRoom($config['room'], $config['room_user'], "BotWorld!");
				$command_handler->wait = $time_session = time();
				break;
		}
	}
    
	// вставляем случайное сообщение если скучно
	if(time() - $time_message > $command_handler->options['rand_message'] && $command_handler->options['rand_message'] > $config['wait_time']) // 10 минимум секунд
	{
		$log->log("Send randome message",PichiLog::LEVEL_DEBUG);
		$command_handler->sendRandMessage();
	}
	
	// Пингуем чтобы не отключалось
	if(time() - $time_ping > $config['ping_time'] * 60)
	{
		$time_ping = time();
		$jabber->send("<iq from='$config[user]@$config[server]' to='$config[server]' id='pingx' type='get'><ping xmlns='urn:xmpp:ping'/></iq>");
		$log->log("Send ping query",PichiLog::LEVEL_DEBUG);
	}
}

?>