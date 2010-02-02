<?php
require_once("database.php");

class commandHandler
{
	public $db;
	protected $jabber;
	public $log;
    
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
	public $users;
	public $users_count = 0;
	
	public $wait;
	public $wait_time = 5;
    
	public $admins = array();
	public $ignore = array();
    
	public function __construct(&$jabber)
	{
		$this->db = new PichiDatabase();
		$this->jabber = &$jabber;
		$this->db->log = & $this->log;
	}

	public function joinRoom($room, $nick, $status = "BotWorld!")
	{
		if(strpos($room, "@") === FALSE)
			$room .= "@" . $this->room_service . "." . $this->server;
		$this->jabber->presence($status, 'available', $room."/".$nick);
		$this->setUserInfo($this->user."@".$this->server, $nick, NULL, $room, 'available');
		$this->wait = time();
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
    
	public function getName($nick)
	{
		$mes=explode("/",$nick);
		return (($mes[1]) ? $mes[1] : $mes[0]);
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
		$this->log->log("Do disconnect", PichiLog::LEVEL_INFO);
		if($this->isAccess())
			$this->jabber->disconnect();
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
				$help .= "!dfn name=val - установить определение\n";
				$help .= "!set name=val - установить опцию\n";
				$help .= "!gc [name] - показать значение опции\n";
				$help .= "!users [nick|jid] - список пользователей\n";
				$help .= "!msg [nick|jid] message - сообщение пользователю\n";
				$help .= "!ping [nick|jid] - ping запрос пользователю\n";
				$help .= "!join room nick [status] - войти в комнату (сменить ник)\n";
				$help .= "!left room nick [status] - выйти из комнаты\n";
				$help .= "!greet jid room@server greet - Сообщение при заходе\n";
				$help .= "!farewell jid room@server buy - Сообщение при выходе\n";
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
      
		$w = $this->seperate($command);

		$user = $this->getJID($w[1]);
		$message = $w[2];
		
		$this->jabber->message($user, $message, "chat");
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
		if(!in_array($jid, $this->admins) && $role == "moderator")
			$this->admins[] = $jid;
		
		if($status == 'available')
			$this->doAction("user_join_room", "room=$room,jid=$jid");
		else if($status == 'unavailable')
			$this->doAction("user_left_room", "room=$room,jid=$jid");
		
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
			$this->toLexems($this->last_message);
	
		//test message
		if(!$this->isIgnore() && !$this->isCommand($this->last_message) && $this->options['answer_mode'] == 1)
		{
			if(rand(1, $this->options['treatment_coincidence']) == 1 && $this->options['treatment_coincidence'] > 0)
			{
				switch(rand(1,2))
				{
					case 1:
						$this->sendAnswer($this->getName($this->last_from) . ": " . $this->genFromLexems());
						break;
					case 2:
						$this->sendAnswer($this->getName($this->last_from) . ", " . $this->genFromLexems());
						break;		
				}
			}
			else
			{
				$this->sendAnswer($this->genFromLexems());
			}
		}
	}
  
  
	private function toLexems($string)
	{
		$this->log->log("$string to lexems", PichiLog::LEVEL_DEBUG);
		$base = explode(" ", $string);
      
		if($base[0] != NULL)
		{
			$str = "#beg# " . $base[0];
			$this->addLexema($str);      
		}
      
		for($i = 0; $i < count($base) ; $i++)
		{
			$str = $base[$i] . (($base[$i+1]) ? " " . $base[$i+1] : " #end#");
			$this->addLexema($str);
		}
	}
    
	private function addLexema($str)
	{	
		if($str == NULL)
			return;
      
		$this->db->query("SELECT COUNT(*) FROM lexems WHERE lexeme = '" . $this->db->db->escapeString($str) . "';");
		if($this->db->fetchColumn() > 0)
			$this->db->query("UPDATE lexems SET count = count+1  WHERE lexeme = '".$this->db->db->escapeString($str)."';");
		else
			$this->db->query("INSERT INTO lexems (`lexeme`,`count`) VALUES ('" . $this->db->db->escapeString($str) . "','1');");
		$this->log->log("$str writed to lexems", PichiLog::LEVEL_VERBOSE);
	}
    
	private function genFromLexems() //сложность
	{
		$this->db->query("SELECT * FROM lexems WHERE lexeme LIKE '#beg# %';");
		if($this->db->numRows(true) == 0)
			break; //пусто
		$last = $this->randGenAnswer();
		$last = explode(" ",$last);
		$genans = $last = $last[1];
		for($i=0; $i < 20; $i++)
		{
			$this->db->query("SELECT * FROM lexems WHERE lexeme LIKE '" . $this->db->db->escapeString($last) . " %';");
			if($this->db->numRows(true) == 0)
				break; //больше нет совпадений
			$last = $this->randGenAnswer();
			$last = explode(" ",$last);
			$last = $last[1];
	
			if($last == "#end#" || $last==NULL)
				break;
	
			$genans .= " " . $last;
		}
      
		return $genans;
	}
  
	// используемс алгоритм лелика на вес
	private function randGenAnswer()
	{
		$this->log->log("Rand world from lexems:", PichiLog::LEVEL_VERBOSE);
		$answers = array();
		$i = 0;
		$sum = 0;
		while($data = $this->db->fetch_array())
		{
			$answers[$i]['lexeme'] = $data['lexeme'];
			$answers[$i]['count'] = $data['count'];	
			$sum += (int)$data['count'];
			$i++;
			$this->log->log("World \"{$data['lexeme']}\" have {$data['count']} points", PichiLog::LEVEL_VERBOSE);
		}
		$rand = rand(0, $sum);
      
		$temp_sum = 0;
		$num = 0;
		while($temp_sum < $rand)
			$temp_sum += $answers[$num++]['count'];

		return $answers[(($num>0) ? $num-1 : $num)]['lexeme'];
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
			switch(rand(1,2))
			{
				case 1:
					$this->jabber->message($this->room, $user[rand(0, $n-1)] . ": " . $this->genFromLexems(), "groupchat");
					break;
				case 2:
					$this->jabber->message($this->room, $user[rand(0, $n-1)] . ", " . $this->genFromLexems(), "groupchat");
					break;		
			}
		}
	}
	
	public function doAction($action, $coincidence = NULL)
	{
		$this->db->query("SELECT `do`, `option`,`value` FROM actions WHERE action = '$action' AND coincidence='" . (($coincidence != NULL) ? $this->db->db->escapeString($coincidence) : "" ) . "';");
		while($actions = $this->db->fetch_array())
		{
			switch($actions['do'])
			{
				case "send_message":
					if($actions['option'] == NULL)
						if($actions['value'] != NULL)
							$this->jabber->message($this->room, $actions['value'],"groupchat");
					else
						if($actions['value'] != NULL)
							$this->jabber->message($actions['option'], $actions['value'],"chat");
			}
		}
	}
  
}

?>