<?php

### Some settings ###
$config['db_version'] = 13; // Work only parram
$config['min_version'] = 6; // Min version of config
$config['pichi_version'] = "0.4.1 (dev)"; //Pichi version

### Begin basic settings end checks ###
if (strtoupper(substr(PHP_OS, 0, 3)) === 'WIN')
       @define(RUN_OS, "Windows");
else
       @define(RUN_OS, "Nix");

if(function_exists("date_default_timezone_set") and function_exists("date_default_timezone_get"))
	@date_default_timezone_set(@date_default_timezone_get()); //disable timezone errors

include("parse_xml_config.php"); // init config
require_once("XMPP/XMPP.php");
require_once("command_handler.php");
require_once("Log_pichi.php");
require_once("System/Daemon.php");
include("console_commands.php"); // parse command line

if($config['debug'] && $config['debug_level'] == 5)
	$config['xmpp_log'] = TRUE;
else
	$config['xmpp_log'] = FALSE;

if(!$config['debug'] && $config['debug_level'] > 2)
	$config['debug_level'] = 2;
if($config['debug_level'] > 4)
	$config['debug_level'] = 4;
if($config['debug_level'] < 3)
	error_reporting(E_ALL & ~E_NOTICE); //disable notices

$log = new PichiLog(true, $config['debug_level']);

function php_extension_load($ext)
{
	if (!extension_loaded($ext))
	{
		if (RUN_OS == "Windows")
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

function isRoom($test)
{
	global $config;
	return (strpos($test, "@") !== FALSE) && (strpos($test, $config['room_service']) !== FALSE) && (strpos($test, $config['server']) !== FALSE);
}

php_extension_load("sqlite3");
php_extension_load("mbstring");
php_extension_load("openssl");
php_extension_load("xml");

if($config['version'] < $config['min_version'])
{
	$log->log("Old config version",PichiLog::LEVEL_ERROR);
	$log->log("You version {$config['version']}. But >{$config['min_version']} required.",PichiLog::LEVEL_ERROR);
	exit();
}

if(RUN_OS == "Nix")
{
	echo Console_Color::convert("%b");
	echo Console_Color::convert("----------------------------------\n");
	echo Console_Color::convert("      Pichi bot v.{$config['pichi_version']}\n");
	echo Console_Color::convert("----------------------------------%n\n");
}
else
{
	echo "----------------------------------\n";
	echo "      Pichi bot v.{$config['pichi_version']}\n";
	echo "----------------------------------\n";
}

### Begin ###
$log->log("Start Pichi",PichiLog::LEVEL_INFO);
$jabber = new XMPPHP_XMPP($config['server'], $config['port'], $config['user'], $config['password'], $config['resource'], $config['server'], $config['xmpp_log'], $loglevel = (($config['debug']) ? XMPPHP_Log::LEVEL_VERBOSE : XMPPHP_Log::LEVEL_INFO));
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
$log->log("Connection success", PichiLog::LEVEL_VERBOSE);

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
$command_handler->admins = $config['admins'];
$command_handler->wait_time = $config['wait_time'];
$log->log("done!",PichiLog::LEVEL_VERBOSE);

if(!$db_exist)
{
	$log->log("Creating database structure",PichiLog::LEVEL_DEBUG);
	$command_handler->db->query("CREATE TABLE log (`from` TEXT, `time` TEXT, `type` TEXT, `message` TEXT);");
	$command_handler->db->query("CREATE TABLE lexems (`lexeme` TEXT, `count` INT);");
	$command_handler->db->query("CREATE TABLE wiki (`name` TEXT, `value` TEXT);");
	$command_handler->db->query("CREATE TABLE settings (`name` TEXT, `value` TEXT, `description` TEXT);");
	$command_handler->db->query("CREATE TABLE users (`jid` TEXT, `nick` TEXT, `role` TEXT, `room` TEXT, `time` TEXT, `status` TEXT);");
	$command_handler->db->query("CREATE TABLE stats (`name` TEXT, `value` TEXT);");
	$command_handler->db->query("CREATE TABLE actions (`action` TEXT, `coincidence` TEXT, `do` TEXT, `option` TEXT, `value` TEXT);");
	$command_handler->db->query("CREATE TABLE db_version (`version` TEXT, `value` TEXT);");
  
	$command_handler->db->query("INSERT INTO db_version (`version`) VALUES ('" . $config['db_version'] . "');");
	$command_handler->db->query("INSERT INTO settings (`name`, `value`, `description`) VALUES ('answer_mode','1','Режим ответа на сообщения. [0 - выключить; 1 - включить][По умолчанию: 1]');"); // Отвечать после сообщений пользователей
	$command_handler->db->query("INSERT INTO settings (`name`, `value`, `description`) VALUES ('answer_random','0','Не всегда ответь при получении сообщения. [0 - всегда отвечать; >100 фактически всегда молчать][По умолчанию: 0]');"); // отвечать не всегда (0 - всегда)
	$command_handler->db->query("INSERT INTO settings (`name`, `value`, `description`) VALUES ('answer_remember','1','Разбивать на связки слов. [0 - выключить; 1 - включить][По умолчанию: 1]');"); // запоминать и разбивать на лексемы
	$command_handler->db->query("INSERT INTO settings (`name`, `value`, `description`) VALUES ('answer_word_limit','10','Максимальное количество связок слов в расмотрении. Влияет на алгоритм построения фраз, так же как и на нагрузку. [Рекомендуется >3 и <50][По умолчанию: 10]');"); // limit для запросов в лексемах
	$command_handler->db->query("INSERT INTO settings (`name`, `value`, `description`) VALUES ('log_enabled','1','Вести лог? [0 - выключить; 1 - включить][По умолчанию: 1]');"); // вести лог ?
	$command_handler->db->query("INSERT INTO settings (`name`, `value`, `description`) VALUES ('treatment_coincidence','3','Вероятность вставки обращений. [1 - всегда; >100 фактически никогда][По умолчанию: 3]');"); // вставлять обращение, совпадения (3 из 1)
	$command_handler->db->query("INSERT INTO settings (`name`, `value`, `description`) VALUES ('rand_message','0','Переодически отправлять случайные фразы в главный чат. [0 - выключить; 1 - включить][По умолчанию: 0]');"); // случайны ответ когда скучно
	$command_handler->db->query("INSERT INTO settings (`name`, `value`, `description`) VALUES ('msg_limit','500','Максимальное количество символов, допустимое в главном чате (в противном случае пишет в личку) [По умолчанию: 500]');"); // лимит символов, после чего отправляет ответ в личку
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
	if(count($payloads) == 2)
	{
		$data = $payloads[1];
		switch($payloads[0]) 
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
					{
						if($x->name == 'x' && $x->subs[0]->attrs['jid'])
						{
							$jid = $command_handler->getJID($x->subs[0]->attrs['jid']);
							$role = $x->subs[0]->attrs['role'];
						}
					}
					if(!$jid)
						break;

					$room = $command_handler->getJID($data['from']);
					if(isRoom($room))
						$command_handler->setUserInfo($jid, $nick, $role, $room, $data['show']);
					else
						$command_handler->setUserInfo($jid, $nick, $role, NULL, $data['show']);
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
	if(time() - $time_message > $command_handler->options['rand_message'] && $command_handler->options['rand_message'] > 10) // 10 минимум секунд
	{
		$log->log("Send randome message",PichiLog::LEVEL_DEBUG);
		$command_handler->sendRandMessage();
	}
}

?>