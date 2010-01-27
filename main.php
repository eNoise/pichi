<?php

$db_version = 10;

require_once("XMPP/XMPP.php");
require_once("command_handler.php");
include("config.php");

function debug($it)
{
	global $debug;
	if($debug)
		echo $it . "\n";
}

function isRoom($test)
{
	global $room_service, $server;
	return (strpos($test, "@") !== FALSE) && (strpos($test, "$room_service") !== FALSE) && (strpos($test, "$server") !== FALSE);
}

// init
debug("Start...");
$jabber = new XMPPHP_XMPP($server, 5222, $user, $pass, $resource, "$server", $printlog=false, $loglevel = (($debug) ? XMPPHP_Log::LEVEL_VERBOSE : XMPPHP_Log::LEVEL_INFO));
try
{
	$jabber->connect();
}

catch(XMPPHP_Exception $e)
{
	debug("connection fail wit error: " . $e);
	exit("exit with error...");
}

// connect
debug("connected...");

if(file_exists("pichi.db"))
	$db_exist = TRUE;
else
	$db_exist = FALSE; 

$command_handler = new commandHandler($jabber);
$command_handler->room = $room;
$command_handler->user = $user;
$command_handler->room_user = $room_user;
$command_handler->ignore[] = $user . "@" . $server;

if(!$db_exist)
{
	$command_handler->db->query("CREATE TABLE log (`from` TEXT, `time` TEXT, `type` TEXT, `message` TEXT);");
	$command_handler->db->query("CREATE TABLE lexems (`lexeme` TEXT, `count` INT);");
	$command_handler->db->query("CREATE TABLE wiki (`name` TEXT, `value` TEXT);");
	$command_handler->db->query("CREATE TABLE settings (`name` TEXT, `value` TEXT);");
	$command_handler->db->query("CREATE TABLE users (`jid` TEXT, `nick` TEXT, `room` TEXT, `time` TEXT, `status` TEXT);");
	$command_handler->db->query("CREATE TABLE stats (`name` TEXT, `value` TEXT);");
	$command_handler->db->query("CREATE TABLE db_version (`version` TEXT, `value` TEXT);");
  
	$command_handler->db->query("INSERT INTO db_version (`version`) VALUES ('$db_version');");
	$command_handler->db->query("INSERT INTO settings (`name`, `value`) VALUES ('answer_mode','1');"); // Отвечать после сообщений пользователей
	$command_handler->db->query("INSERT INTO settings (`name`, `value`) VALUES ('answer_random','0');"); // отвечать не всегда
	$command_handler->db->query("INSERT INTO settings (`name`, `value`) VALUES ('answer_remember','1');"); // запоминать и разбивать на лексемы
	$command_handler->db->query("INSERT INTO settings (`name`, `value`) VALUES ('log_enabled','1');"); // вести лог ?
	$command_handler->db->query("INSERT INTO settings (`name`, `value`) VALUES ('treatment_coincidence','3');"); // вставлять обращение, совпадения (3 из 1)
	$command_handler->db->query("INSERT INTO settings (`name`, `value`) VALUES ('rand_message','0');"); // случайны ответ когда скучно
	$command_handler->db->query("INSERT INTO settings (`name`, `value`) VALUES ('msg_limit','200');"); // лимит символов, после чего отправляет ответ в личку
}

$command_handler->db->query("SELECT * FROM db_version;");
$current_db_version = $command_handler->db->fetch_column(0);
if($current_db_version < $db_version)
{
	debug("DB version is $current_db_version, but Bot require $db_version");
	exit("Exit with db error");
}

$command_handler->parseOptions(); // загнать все опции
$time_message = $time_ping = time(); // сбрасываем

while(!$jabber->isDisconnected()) {
	$payloads = $jabber->processUntil(array('message', 'presence', 'end_stream', 'session_start'), 1);
	//wait for proccess
    
	foreach($payloads as $event) 
	{
		$data = $event[1];
		switch($event[0]) 
		{
			case 'message':
				$time_message = time();
				if($time_message - $time_session > 10)
				{
					$command_handler->do_if_message($data['body'], $data['from'], $data['type']);
					debug("---------------------------------------------------------------------------------");
					debug("Message From: $data[from]");
					debug("Body: $data[body]");
					debug("---------------------------------------------------------------------------------");
				}
				break;
			case 'presence':
				debug("Presence: {$data['from']} [{$data['show']}] {$data['status']}");
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
				debug("Session Start...");
				$command_handler->db->query("UPDATE users SET status = 'unavailable';");
				$jabber->getRoster();
				$jabber->presence($status);
				$jabber->presence($status,'available',$room."/".$room_user);
				//пичи онлайн в базе
				$command_handler->setUserInfo($user."@".$server, $room_user, $room, 'available');
				$command_handler->reconstatus++;
				if($command_handler->reconstatus > 1)
					$command_handler->need_reconnect = TRUE;
				$time_session = time();
				break;
		}
	}
    
	// вставляем случайное сообщение если скучно
	if(time() - $time_message > $command_handler->options['rand_message'] && $command_handler->options['rand_message'] > 10) // 10 минимум секунд
	{
		$command_handler->sendRandMessage();
	}
	
	// Пингуем чтобы не отключалось
	if(time() - $time_ping > 5 * 60)
	{
		$time_ping = time();
		$jabber->send("<iq from='$user@$server' to='$server' id='pingx' type='get'><ping xmlns='urn:xmpp:ping'/></iq>");
	}
}

?>