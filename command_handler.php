<?php
require_once("database.php");
require_once("syntax_analizer.php");
require_once("eventHandler.php");

class commandHandler
{
	public $db;
	protected $jabber;
	public $log;
	protected $syntax;
	protected $event;
    
	public $server;
	public $room_service;
	public $room;
	public $user;
	public $room_user;
    
	public $options;
    
	private $last_massage;
	private $last_from;
	private $last_type;
	private $last_room;
	public $last_id;                                           
	public $last_data;
	
	public $wait;
	public $wait_time = 5;
    
	public $admins = array();
	public $ignore = array();
    
	public function __construct(&$jabber)
	{
		$this->db = new PichiDatabase();
		$this->jabber = &$jabber;
		$this->db->log = & $this->log;
		$this->syntax = new SyntaxAnalizer();
		$this->syntax->db = & $this->db;
		$this->syntax->log = & $this->log;
		$this->event = new eventHandler();
		$this->event->db = & $this->db;
		$this->event->log = & $this->log;
		$this->event->jabber = & $this->jabber;
	}

	public function joinRoom($room, $nick, $status = "BotWorld!")
	{
		if(strpos($room, "@") === FALSE)
			$room .= "@" . $this->room_service . "." . $this->server;
		$this->jabber->presence($status, 'available', $room."/".$nick);
		$this->wait = time();
		$this->setUserInfo($this->user."@".$this->server, $nick, NULL, $room, 'available');
		$this->log->log("Join to room $room as $nick", PichiLog::LEVEL_DEBUG);
	}
	
	public function leftRoom($room, $nick, $status)
	{
	  	if(strpos($room, "@") === FALSE)
			$room .= "@" . $this->room_service . "." . $this->server;
		$this->jabber->presence($status, 'unavailable', $room."/".$nick);
		$this->setUserInfo($this->user."@".$this->server, $nick, NULL, $room, 'unavailable');
		$this->log->log("Left room $room as $nick", PichiLog::LEVEL_DEBUG);
	}

	public function ping($jid, $id = "pichi_ping")
	{
		$jid = $this->getJID($jid); // На всякий случай
		$this->log->log("Send ping to $jid", PichiLog::LEVEL_DEBUG);
		$this->jabber->ping($jid, $id);
		$this->last_data['ping_time'] = microtime(true);                   
		$this->last_id['ping'] = $id;
	}
	
	public function do_if_ping($id)
	{
		if($id == $this->last_id['ping'])
		{
			$time = microtime(true) - $this->last_data['ping_time'];
			$time = number_format($time, 2); // до 2 символов
			$this->log->log("Recived ping if $time", PichiLog::LEVEL_DEBUG);
			$this->sendAnswer("Pong in $time sec.");
		}
	}

	public function getJID($nick)
	{
		$this->log->log("Get JID from $nick", PichiLog::LEVEL_VERBOSE);
		if(strpos($nick, "@") === FALSE)
		{
			$this->db->query("SELECT `jid` FROM users WHERE nick = '" . $this->db->db->escapeString($nick) . "';");
			return $this->db->fetchColumn(0);
		}
		else
		{
			$mes=explode("/",$nick);
			return $mes[0];
		}
	}
    
	public function getName($jid)
	{
	  
	  	$this->log->log("Get Nick from JID $jid", PichiLog::LEVEL_VERBOSE);
		if(strpos($jid, "@") === FALSE)
			return $jid;
		if(strpos($jid, "/") === FALSE)
		{
			$this->db->query("SELECT `nick` FROM users WHERE jid = '" . $this->db->db->escapeString($jid) . "';");
			return $this->db->fetchColumn(0);
		}
		else
		{
			$mes=explode("/", $jid);
			return $mes[1];
		}
	}
    
	private function isCommand($command)
	{
		return (substr($command,0,1) == "!");
	}
    
	private function getCommand($command)
	{
		if($this->isCommand($command))
		{
			$get = explode(" ", $command);
			return $get[0];
		}
	}

	private function seperate($command, $level = 2)
	{
		$array = array();
		if(strpos($command, "=") !== FALSE)
		{
			$w = explode(" ",$command);
			$array[0] = $w[0];
			unset($w[0]);
			$w = implode(" ",$w);
			$w = explode("=",$w);
			$array[1] = $w[0];
			$array[2] = $w[1];
			return $array;
		}
		else if(strpos($command, "\"") !== FALSE)
		{
			$w = explode("\"", $command);
			$array[0] = trim($w[0]);
			for($i = $g = 1; $i < count($w); $i = $i+2, $g++)
				$array[$g] = $w[$i];
			$array[$g] = trim($w[--$i]);
			return $array;
		}
		else // default
		{
			$w = explode(" ",$command);
			for($i = 0; $i < $level; $i++)
			{
				$array[$i] = $w[$i];
				unset($w[$i]);
			}
			$array[$i] = implode(" ", $w);
			return $array;
		}
	}

	// Отправляет ответ тому, от кого пришло сообщение
	private function sendAnswer($message)
	{
		if($this->last_type == "groupchat")
			$to = $this->last_room;
		else
			$to = $this->last_from;
		$type = $this->last_type;
		if(strlen($message) < $this->options['msg_limit'] || $this->options['msg_limit'] < 1 || $this->last_type != "groupchat")
			$this->jabber->message($to, $message, $type);
		else
			$this->jabber->message($this->getJID($this->getName($this->last_from), true), $message, "chat");
		$this->log->log("Send answer to $to:\n$message", PichiLog::LEVEL_VERBOSE);
	}

	//Вспомогательная функция с поиском пользователя в списке
	//допустемые значения: jid, ник
	private function inUser(&$array_users, $user)
	{
		if(in_array($this->getJID($user), $array_users))
		{
			return true;
		}
		else
		{	  
			$this->db->query("SELECT `jid`,`nick` FROM users WHERE status = 'available';");
			while($users = $this->db->fetch_array())
			{
				if($users['nick'] == $this->getName($user))
					if(in_array($users['jid'], $array_users))
						return true;
			}
		}
		return false;
	}

	private function isAccess()
	{
		$this->log->log("Test access ". $this->last_from . " in admins", PichiLog::LEVEL_VERBOSE);
		return $this->inUser($this->admins, $this->last_from);
	}
	
	private function isIgnore()
	{
		$this->log->log("Test ignore ". $this->last_from . " in ignorelist", PichiLog::LEVEL_VERBOSE);
		return $this->inUser($this->ignore, $this->last_from);
	}
    
	private function doExit()
	{
		global $config;
		$this->log->log("Do disconnect", PichiLog::LEVEL_INFO);
		if($this->isAccess())
			$this->jabber->disconnect();
		if($config['daemon_mode'])
			System_Daemon::stop();
	}
    
	public function parseOptions()
	{
		$this->db->query("SELECT * FROM settings;");
		$this->log->log("Parse Settings", PichiLog::LEVEL_DEBUG);
		while($data = $this->db->fetch_array())
		{
			$this->options[$data['name']] = $data['value'];
			$this->log->log("$data[name] = $data[value]", PichiLog::LEVEL_VERBOSE);
		}
	}
    
	protected function fetch_commands($command, $from, $type)
	{
		global $config;
		$this->log->log("Command from " . $this->last_from . ": $command", PichiLog::LEVEL_DEBUG);
		switch($command)
		{
			case "!help":
				$help = "\n";
				$help .= "-------------------------\n";
				$help .= "        Pichi Bot        \n";
				$help .= "-------------------------\n";	  
				$help .= "!log n - показывает последний лог\n";
				$help .= "!wtf name - определение из базы\n";
				$help .= "!wtfcount - количество определений в базе\n";
				$help .= "!wtfrand - случайно определение\n";
				$help .= "!top - топ 10 слов в базе\n";
				$help .= "!count - количество лексем\n";
				$help .= "!talkers - 10 самых болтнивых пользователей\n";
				$help .= "!dfn name=val - установить определение\n";
				$help .= "!set name=val - установить опцию\n";
				$help .= "!gc [name] - показать значение опции\n";
				$help .= "!users [nick|jid] - список пользователей\n";
				$help .= "!msg [nick|jid|room] message [type] - сообщение пользователю\n";
				$help .= "!ping [nick|jid] - ping запрос пользователю\n";
				$help .= "!join room nick [status] - войти в комнату (сменить ник)\n";
				$help .= "!left room nick [status] - выйти из комнаты\n";
				$help .= "!greet jid room@server greet - Сообщение при заходе\n";
				$help .= "!farewell jid room@server buy - Сообщение при выходе\n";
				$help .= "!idle nick - сколько молчит указаный ник\n";
				$help .= "!quit - выход\n";
				$help .= "!version - версия бота\n";
				$this->sendAnswer($help);
				break;
			case "!version":
				$this->sendAnswer("Pichi Bot v.{$config['pichi_version']}");
				break;
			case ($this->getCommand($command) == "!log"):
				$this->show_log($command);
				break;
			case ($this->getCommand($command) == "!wtf"):
				$this->show_wtf($command);
				break;
			case ($this->getCommand($command) == "!wtfcount"):
				$this->db->query("SELECT COUNT(*) FROM wiki;");
				$wtfnum = (int)$this->db->fetchColumn(0);
				$this->sendAnswer("Количество определений в базе: $wtfnum");
				break;
			case ($this->getCommand($command) == "!wtfrand"):
				$this->db->query("SELECT * FROM wiki ORDER BY RANDOM() LIMIT 0,1;");
				$wtfword = $this->db->fetchColumn(0);
				$wtfdef = $this->db->fetchColumn(1,true);
				if($wtfword != NULL && $wtfdef != NULL)
					$this->sendAnswer($wtfword . " = " . $wtfdef);
				break;
			case ($this->getCommand($command) == "!top"):
				$this->db->query("SELECT lexeme FROM lexems ORDER BY count DESC LIMIT 0,10;");
				$this->sendAnswer("10 самых популярных связок слов:");
				$ans = "";
				$ix = 0;
				while($lex = $this->db->fetch_array())
				{
					$ix++;
					$tmp = explode(" ", $lex['lexeme']);
					if($tmp[0] == "#beg#")
						$tmp[0] = "(начало)";
					if($tmp[1] == "#end#")
						$tmp[1] = "(конец)";
					$ans .= $ix . ". " . implode(" ", $tmp) . "\n";
				}
				$this->sendAnswer($ans);
				break;
			case ($this->getCommand($command) == "!talkers"):
				$this->db->query("SELECT `from` FROM log;");
				$this->sendAnswer("10 самых болтливых пользователей:");
				$ans = "";
				$tmp = array();
				while($fr = $this->db->fetch_array())
				{
					if($tmp["{$fr['from']}"] == NULL)
						$tmp["{$fr['from']}"] = 0;
					$tmp["{$fr['from']}"]++;
				}
				arsort($tmp);
				$i = 0;
				foreach($tmp as $key=>$val)
				{
					$i++;
					$ans .= $i . ". " . $this->getName($key) . " с " . $val . " словами.\n";
					if($i>=10)
						break;
				}
				$this->sendAnswer($ans);
				break;
			case ($this->getCommand($command) == "!count"):
				$this->db->query("SELECT COUNT(*) FROM lexems;");
				$lexnum = (int)$this->db->fetchColumn(0);
				$this->sendAnswer("Количество определений в базе: $lexnum");
				break;
			case ($this->getCommand($command) == "!dfn"):
				$this->set_dfn($command);
				break;
			case ($this->getCommand($command) == "!set"):
				$this->set_setting($command);
				break;
			case ($this->getCommand($command) == "!msg"):
				$this->send_message($command);
				break;
			case ($this->getCommand($command) == "!gc"):
				$this->get_setting($command);
				break;
			case ($this->getCommand($command) == "!users"):
				$this->user_list($command);
				break;
			case ($this->getCommand($command) == "!join"):
				if(!$this->isAccess())
					return;
				$w = $this->seperate($command, 3);
				$this->joinRoom($w[1], $w[2], $w[3]);
				break;
			case ($this->getCommand($command) == "!left"):
				if(!$this->isAccess())
					return;
				$w = $this->seperate($command, 3);
				$this->leftRoom($w[1], $w[2], $w[3]);
				break;
			case ($this->getCommand($command) == "!ping"):
				$w = $this->seperate($command);
				$this->ping($w[1]);
				break;
			case ($this->getCommand($command) == "!idle"):
				$w = $this->seperate($command);
				$this->db->query("SELECT `time` FROM log WHERE `from` = '" . $this->db->db->escapeString($w[1]) . "' OR `from` LIKE '%/" . $this->db->db->escapeString($this->getName($w[1])) . "' ORDER BY time DESC;");
				$date = (int)$this->db->fetchColumn(0);
				if($date > 0)
					$this->sendAnswer(date("d.m.y \в H:i:s", $date));
				break;
			case ($this->getCommand($command) == "!greet" || $this->getCommand($command) == "!farewell"):
				$w = $this->seperate($command, 3);
				if(!$this->isAccess())
					return;
				$action = ($w[0] == "!greet") ? "user_join_room" : "user_left_room";
				$this->db->query("SELECT COUNT(*) FROM actions WHERE action = '$action' AND coincidence='room=" . $this->db->db->escapeString($w[2]) . ",jid=" . $this->db->db->escapeString($w[1]) . "';");
				if($this->db->fetchColumn() > 0)
					$this->db->query("UPDATE actions SET value = '".$this->db->db->escapeString($w[3])."'  WHERE action = '$action' AND coincidence='room=" . $this->db->db->escapeString($w[2]) . ",jid=" . $this->db->db->escapeString($w[1]) . "';");
				else
					$this->db->query("INSERT INTO actions (`action`,`coincidence`,`do`,`option`,`value`) VALUES ('$action', 'room=" . $this->db->db->escapeString($w[2]) . ",jid=" . $this->db->db->escapeString($w[1]) . "', 'send_message', '', '".$this->db->db->escapeString($w[3])."');");
				break;
			case ($this->getCommand($command) == "!quit" || $this->getCommand($command) == "!exit"):
				$this->doExit();
				break;
		}
	}
    
	private function get_setting($command)
	{
		if(!$this->isAccess())
			return;
      
		$w = $this->seperate($command);
		$this->db->query("SELECT * FROM settings" . (($w[1] != NULL) ? " WHERE name='".$this->db->db->escapeString($w[1])."'" : "") . ";");
		while($data = $this->db->fetch_array())
		{
			$this->sendAnswer($data['name'] . " = " . $data['value']);
			$this->log->log("User request setting: {$data['name']} = {$data['value']}", PichiLog::LEVEL_VERBOSE);
		}
	}
    
	private function show_wtf($command)
	{
		$w = $this->seperate($command);
		$this->db->query("SELECT `value` FROM wiki WHERE name='".$this->db->db->escapeString($w[1])."';");
		$answer = $this->db->fetchColumn(0);
		$this->sendAnswer($answer);
		$this->log->log("User request wiki page \"$w[1]\" = $answer", PichiLog::LEVEL_DEBUG);
	}
    
	private function set_dfn($command)
	{
		$w = $this->seperate($command);
      
		$this->db->query("SELECT COUNT(*) FROM wiki WHERE name = '" . $this->db->db->escapeString($w[1]) . "';");
		if($this->db->fetchColumn() > 0)
			$this->db->query("UPDATE wiki SET value = '".$this->db->db->escapeString($w[2])."'  WHERE name = '".$this->db->db->escapeString($w[1])."';");
		else
			$this->db->query("INSERT INTO wiki (`name`,`value`) VALUES ('" . $this->db->db->escapeString($w[1]) . "','".$this->db->db->escapeString($w[2])."');");
		
		$this->log->log("User set wiki page $w[1] = $w[2]", PichiLog::LEVEL_DEBUG);
		$this->sendAnswer("Value Updated!");
	}
    
	private function set_setting($command)
	{
		if(!$this->isAccess())
			return;
      
		$w = $this->seperate($command);
      
		$this->db->query("SELECT COUNT(*) FROM settings WHERE name = '" . $this->db->db->escapeString($w[1]) . "';");
		if($this->db->fetchColumn() > 0)
		{
			$this->db->query("UPDATE settings SET value = '".$this->db->db->escapeString($w[2])."'  WHERE name = '".$this->db->db->escapeString($w[1])."';");
			$this->sendAnswer("Updated!");
			$this->parseOptions();
			$this->log->log("Updated option $w[1] = $w[2]", PichiLog::LEVEL_DEBUG);
		}
		else
		{
			$this->log->log("Can't set $w[1]. There is no such option.", PichiLog::LEVEL_DEBUG);
			$this->sendAnswer("Нету такой опции =(");
		}
      
	}
	
	// Отправляет сообщение пользователю
	private function send_message($command)
	{
		if(!$this->isAccess())
			return;
      
		$w = $this->seperate($command, 3);

		$user = $this->getJID($w[1]);
		$message = $w[2];
		if($w[3] != "chat" && $w[3] != "groupchat")
			$w[3] = "chat";
		
		$this->jabber->message($user, $message, $w[3]);
		$this->log->log("Send message to $user: $message", PichiLog::LEVEL_DEBUG);
	}
	
    
	/*
	* Показывает информацию о известных пользователях
	*/
	private function user_list($command)
	{
		$w = $this->seperate($command);
      
		$this->db->query("SELECT * FROM users;");
      
		if($w[1] == NULL)
		{
			$this->log->log("Begin creting user list", PichiLog::LEVEL_DEBUG);
			$userlist = "Список пользователей, которых я видел:\n";
			$online = $offline = "";
			$n = $f = 0;
			while($data = $this->db->fetch_array())
			{
				$roomname = explode("@", $data['room']);
				$roomname = $roomname[0];
				if($data['status'] == 'available')
				{
					$n++;
					$online .= "$n. $data[nick] (в комнате $roomname)\n";
					$this->log->log("User $data[nick]: online", PichiLog::LEVEL_VERBOSE);
				}
				else
				{
					$f++;
					$offline .= "$f. $data[nick] (Последний раз видел ".date("d.m.y \в H:i:s", $data['time'])." в комнате $roomname)\n";
					$this->log->log("User $data[nick]: offline", PichiLog::LEVEL_VERBOSE);
				}
			}
			$userlist .= "Пользователи онлайн:\n" . $online;
			$userlist .= "Пользователи оффлайн:\n" . $offline;
			$this->sendAnswer($userlist);
		}
		else
		{
			while($data = $this->db->fetch_array())
			{
				if($data['nick'] == $w[1] || $data['jid'] == $w[1])
				{
					$this->log->log("User {$data['nick']} founded!", PichiLog::LEVEL_VERBOSE);
					$this->sendAnswer($data['nick'] . " сейчас " . (($data['status'] == 'available') ? "онлайн и смотрит на тебя 0_0" : "оффлайн, а жаль. Последний раз видел " . date("d.m.y \в H:i:s", $data['time'])));
				}
			}
		}
	}
    
	/*
	* Вставляет инофрмацию о пользователе
	* jid, nick, время
	*/
	public function setUserInfo($jid, $nick, $role, $room, $status)
	{      
		if($role == NULL)
			$role = "participant"; //Default permission
			
		//Admins add
		global $config;
		if($config['global_admins'])
		{
			if(!in_array($jid, $this->admins) && $role == "moderator")
				$this->admins[] = $jid;
		}
		
		if($status == 'available')
			$this->event->catchEvent("user_join_room", "room=$room,jid=$jid");
		else if($status == 'unavailable')
			$this->event->catchEvent("user_left_room", "room=$room,jid=$jid");
		
		$this->log->log("Updating user status for $nick($jid) in $room = $status", PichiLog::LEVEL_DEBUG);
		$this->db->query("SELECT COUNT(*) FROM users WHERE jid = '" . $this->db->db->escapeString($jid) . "' AND room = '" . $this->db->db->escapeString($room) . "';");
		if($this->db->fetchColumn() > 0)
			$this->db->query("UPDATE users SET nick = '".$this->db->db->escapeString($nick)."', time = '".time()."', status = '".$status."', role = '" . $this->db->db->escapeString($role) . "' WHERE jid = '".$this->db->db->escapeString($jid)."' AND room = '". $this->db->db->escapeString($room) ."';");
		else
			$this->db->query("INSERT INTO users (`jid`,`nick`,`role`,`room`,`time`,`status`) VALUES ('" . $this->db->db->escapeString($jid) . "','".$this->db->db->escapeString($nick)."','" . $this->db->db->escapeString($role) . "','". $this->db->db->escapeString($room) ."','".time()."','".$status."');");
	}
    
	public function do_if_message($message, $from, $type)
	{
		// No reaction while time off
		if(time() - $this->wait < $this->wait_time)
		{
			$this->log->log("Ignore Message: <$from> $message", PichiLog::LEVEL_DEBUG);
			return;
		}
			
		$this->last_message = $message;
		$this->last_from = $from;
		$this->last_type = $type;
		if($this->last_type == "groupchat")
			$this->last_room = $this->getJID($this->last_from);
		else
			$this->last_room = NULL;
	
		$this->log->log("Call message method", PichiLog::LEVEL_DEBUG);
		
		if(!$this->isIgnore())
			$this->fetch_commands($this->last_message, $this->last_from, $this->last_type); // проверяем на комманду

		if(!$this->isCommand($message) && $this->options['log_enabled'] == 1)
			$this->db->query("INSERT INTO log (`from`,`time`,`type`,`message`) VALUES ('".$this->db->db->escapeString($this->last_from)."','".$this->db->db->escapeString(time())."','".$this->db->db->escapeString($this->last_type)."','".$this->db->db->escapeString($this->last_message)."');");
		//Log
	
		//to lexems massges
		if(!$this->isIgnore() && !$this->isCommand($this->last_message) && $this->options['answer_remember'] == 1)
			$this->syntax->parseText($this->last_message);
	
		//test message
		if(!$this->isIgnore() && !$this->isCommand($this->last_message) && $this->options['answer_mode'] == 1)
		{
			if((int)$this->options['answer_random'] === 0 || rand(1, (int)$this->options['answer_random']) === 1)
			{
				$this->syntax->generate();
				if(rand(1, (int)$this->options['treatment_coincidence']) === 1 && $this->options['treatment_coincidence'] > 0)
				{
					switch(rand(1,2))
					{
						case 1:
							$this->sendAnswer($this->getName($this->last_from) . ": " . $this->syntax->returnText());
							break;
						case 2:
							$this->sendAnswer($this->getName($this->last_from) . ", " . $this->syntax->returnText());
							break;		
					}
				}
				else
				{
					$this->sendAnswer($this->syntax->returnText());
				}
			}
		}
	}
  
	public function show_log($command)
	{
		$this->db->query("SELECT * FROM log ORDER BY time;");
		$qu_i = $this->db->numRows(true);
		$n = explode(" ",$command);
		$n = ($n[1] != NULL) ? ((int)$n[1]) : 20;
		$i = 0;
		$log = "\n-----------------------------------------------------------------------\n";
		while($data = $this->db->fetch_array())
		{
			if($i < $n && $i < 50 && $qu_i <= $n)
			{
				$log .= "[".date("H:i:s",$data['time'])."]<".$this->getName($data['from'])."> $data[message]\n";
				$i++;
			}
			$qu_i--;
		}
		$log .= "-----------------------------------------------------------------------";
		$this->log->log("Request log:\n$log", PichiLog::LEVEL_VERBOSE);
		$this->sendAnswer($log);
	}
    
	public function sendRandMessage()
	{
		if($this->options['rand_message'] > 10) //10 вменяемый лимит секунд
		{
			$this->log->log("Rand message method", PichiLog::LEVEL_DEBUG);
			$this->db->query("SELECT `nick` FROM users WHERE status = 'available';");
			$user = array();
			$n = 0;
			while($users = $this->db->fetch_array())
			{
				$n++;
				$user[] = $users['nick'];
			}
			$this->syntax->generate();
			switch(rand(1,2))
			{
				case 1:
					$this->jabber->message($this->room, $user[rand(0, $n-1)] . ": " . $this->syntax->returnText(), "groupchat");
					break;
				case 2:
					$this->jabber->message($this->room, $user[rand(0, $n-1)] . ", " . $this->syntax->returnText(), "groupchat");
					break;		
			}
		}
	}

}

?>