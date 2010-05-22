<?php

require_once("command_handler.php");

class Pichi extends CommandHandler
{
  
	public function __construct(&$jabber)
	{
		parent::__construct($jabber);
	}
  
	// Begin commands
	protected function command_help()
	{
		$help = "\n";
		$help .= "-------------------------\n";
		$help .= "        Pichi Bot        \n";
		$help .= "-------------------------\n";
		$help .= "--------------------------------------------\n";
		$help .= PichiLang::get('help_syntaxis_info') . "\n";
		$help .= "--------------------------------------------\n";
		
		$help .= "=====  " . PichiLang::get('help_main_commands') . "  =====\n";
		$help .= "!set ".PichiLang::get('help_command_usage_param')."=".PichiLang::get('help_command_usage_value')." - ".PichiLang::get('help_command_description_set')."\n";
		$help .= "!gc [".PichiLang::get('help_command_usage_param')."] - ".PichiLang::get('help_command_description_gc')."\n";
		$help .= "!log ".PichiLang::get('help_command_usage_param')." - ".PichiLang::get('help_command_description_log')."\n";
		$help .= "!users [".PichiLang::get('help_command_usage_nick')."|".PichiLang::get('help_command_usage_jid')."] - ".PichiLang::get('help_command_description_users')."\n";		
		$help .= "!msg [".PichiLang::get('help_command_usage_nick')."|".PichiLang::get('help_command_usage_jid')."|".PichiLang::get('help_command_usage_room')."] [type] message - ".PichiLang::get('help_command_description_msg')."\n";
		$help .= "!ping [".PichiLang::get('help_command_usage_nick')."|".PichiLang::get('help_command_usage_jid')."] - ".PichiLang::get('help_command_description_ping')."\n";
		$help .= "!join ".PichiLang::get('help_command_usage_room')." ".PichiLang::get('help_command_usage_nick')." [".PichiLang::get('help_command_usage_status')."] - ".PichiLang::get('help_command_description_join')."\n";
		$help .= "!left ".PichiLang::get('help_command_usage_room')." ".PichiLang::get('help_command_usage_nick')." [".PichiLang::get('help_command_usage_status')."] - ".PichiLang::get('help_command_description_left')."\n";
		$help .= "!greet ".PichiLang::get('help_command_usage_jid')." ".PichiLang::get('help_command_usage_room')." ".PichiLang::get('help_command_usage_message')." - ".PichiLang::get('help_command_description_greet')."\n";
		$help .= "!farewell ".PichiLang::get('help_command_usage_jid')." ".PichiLang::get('help_command_usage_room')." ".PichiLang::get('help_command_usage_message')." - ".PichiLang::get('help_command_description_farewell')."\n";
		$help .= "!idle ".PichiLang::get('help_command_usage_nick')." - ".PichiLang::get('help_command_description_idle')."\n";
		$help .= "!quit - ".PichiLang::get('help_command_description_quit')."\n";
		$help .= "!version - ".PichiLang::get('help_command_description_version')."\n";
		
		$help .= "=====  " . PichiLang::get('help_admin_commands') . "  =====\n";
		$help .= "!topic ".PichiLang::get('help_command_usage_param')." - ".PichiLang::get('help_command_description_topic')."\n";
		$help .= "!ban ".PichiLang::get('help_command_usage_jid')."|".PichiLang::get('help_command_usage_nick')." [".PichiLang::get('help_command_usage_time')."] [".PichiLang::get('help_command_usage_reason')."] - ".PichiLang::get('help_command_description_ban')."\n";
		$help .= "!unban ".PichiLang::get('help_command_usage_jid')." - ".PichiLang::get('help_command_description_jid')."\n";
		$help .= "!banlist - ".PichiLang::get('help_command_description_banlist')."\n";
		$help .= "!kick ".PichiLang::get('help_command_usage_nick')."|".PichiLang::get('help_command_usage_jid')." [".PichiLang::get('help_command_usage_time')."] [".PichiLang::get('help_command_usage_reason')."] - ".PichiLang::get('help_command_description_kick')."\n";
		$help .= "!unkick ".PichiLang::get('help_command_usage_jid')." - ".PichiLang::get('help_command_description_unkick')."\n";
		
		$help .= "=====  " . PichiLang::get('help_plugins') . "  =====\n";
		$help .= "!plugins - ".PichiLang::get('help_command_description_plugins')."\n";
		$help .= "!enable ".PichiLang::get('help_command_usage_param')." - ".PichiLang::get('help_command_description_enable')."\n";
		$help .= "!disable ".PichiLang::get('help_command_usage_param')." - ".PichiLang::get('help_command_description_disable')."\n";
		$help .= "!reload - ".PichiLang::get('help_command_description_reload')."\n";

		$help .= "=====  " . PichiLang::get('help_wiki_inline') . "  =====\n";
		$help .= "!dfn ".PichiLang::get('help_command_usage_param')."=".PichiLang::get('help_command_usage_value')." - ".PichiLang::get('help_command_description_dfn')."\n";
		$help .= "!wtf ".PichiLang::get('help_command_usage_param')." - ".PichiLang::get('help_command_description_wtf')."\n";
		$help .= "!wtfcount - ".PichiLang::get('help_command_description_wtfcount')."\n";
		$help .= "!wtfrand - ".PichiLang::get('help_command_description_wtfrand')."\n";
		$help .= "!wtfrev ".PichiLang::get('help_command_usage_param')." - ".PichiLang::get('help_command_description_wtfrev')."\n";
		$help .= "!wtfull ".PichiLang::get('help_command_usage_param')." - ".PichiLang::get('help_command_description_wtffull')."\n";
		$help .= "!wtfset ".PichiLang::get('help_command_usage_param')." - ".PichiLang::get('help_command_description_wtfset')."\n";

		$help .= "=====  " . PichiLang::get('help_lexems') . "  =====\n";
		$help .= "!top - ".PichiLang::get('help_command_description_top')."\n";
		$help .= "!count - ".PichiLang::get('help_command_description_count')."\n";
		$help .= "!talkers - ".PichiLang::get('help_command_description_talkers')."\n";

		$help .= "=====  " . PichiLang::get('help_other_commands') . "  =====\n";
		($hook = PichiPlugin::fetch_hook('commands_show_help')) ? eval($hook) : false;
		
		$this->sendAnswer($help);
	}
	
	protected function command_version()
	{
		global $config;
		$this->sendAnswer("Pichi Bot v.{$config['pichi_version']}");
		($hook = PichiPlugin::fetch_hook('commands_show_version')) ? eval($hook) : false;
		$this->sendAnswer("Плагины:\n" . PichiPlugin::show_plugin_list());
	}
	
	protected function command_enable()
	{
		if(!$this->isAccess())
			return;
		
		$w = $this->seperate($this->last_message);
		PichiPlugin::enable((int)$w[1]);
	}
	
	protected function command_disable()
	{
		if(!$this->isAccess())
			return;
		
		$w = $this->seperate($this->last_message);
		PichiPlugin::disable((int)$w[1]);
	}
	
	protected function command_reload()
	{
		if(!$this->isAccess())
			return;
		
		PichiPlugin::reload();
	}
	
	protected function command_plugins()
	{
		$this->sendAnswer("Плагины:\n" . PichiPlugin::show_plugin_list());
	}
	
	protected function command_kick()
	{
		if(!$this->isAccess())
			return;
		
		$w = $this->seperate($this->last_message, 3);
		$this->kick($w[1], $w[2], $w[3]);
	}
	
	protected function command_unkick()
	{
		if(!$this->isAccess())
			return;
		
		$w = $this->seperate($this->last_message);
		$this->unkick($w[1]);
	}
	
	protected function command_ban()
	{
		if(!$this->isAccess())
			return;
		
		$w = $this->seperate($this->last_message, 3);
		$this->ban($w[1], $w[2], $w[3]);
	}
	
	protected function command_unban()
	{
		if(!$this->isAccess())
			return;
		
		$w = $this->seperate($this->last_message, 2);
		$this->unban($w[1], $w[2]);
	}
	
	protected function command_banlist()
	{
		$this->db->query("SELECT `jid`,`value` FROM users_data WHERE name = 'ban';");
		$banlist = "";
		while($bans = $this->db->fetch_array())
		{
			$banlist .= $bans['jid'] . " " . date("d.m.y \в H:i:s", $bans['value']) . "\n";
		}
		$this->sendAnswer("Список заблокированных в пичи:\n" . $banlist);
	}
	
	protected function command_log()
	{
	 	$this->db->query("SELECT * FROM log ORDER BY time;");
		$qu_i = $this->db->numRows(true);
		$n = explode(" ", $this->last_message);
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
	
	protected function command_wtf()
	{
	  	$w = $this->seperate($this->last_message);
		$this->db->query("SELECT `value` FROM wiki WHERE name='".$this->db->db->escapeString($w[1])."' ORDER BY revision DESC LIMIT 0,1;");
		$answer = $this->db->fetchColumn(0);
		$this->sendAnswer($answer);
		$this->log->log("User request wiki page \"$w[1]\" = $answer", PichiLog::LEVEL_DEBUG);
	}
	
	protected function command_wtfcount()
	{
		$this->db->query("SELECT name FROM wiki;");
		if($this->db->numRows(true) > 0)
		{
			$tmp_ar = array();
			while($tmp = $this->db->fetch_array())
				$tmp_ar[] = $tmp['name'];
			$tmp_ar = array_unique($tmp_ar);
			$wtfnum = count($tmp_ar);
			$this->sendAnswer("Количество определений в базе: $wtfnum");
		}
		else
		{
			$this->sendAnswer("В базе нет поредлений =(");
		}
	}
	
	protected function command_wtfrand()
	{
		$this->db->query("SELECT name FROM wiki ORDER BY RANDOM() LIMIT 0,1;");
		if($this->db->numRows(true) > 0)
		{
			$this->db->query("SELECT name,value FROM wiki WHERE name = '" . $this->db->db->escapeString($this->db->fetchColumn(0)) . "' ORDER BY revision DESC LIMIT 0,1;");
			$wtfword = $this->db->fetchColumn(0);
			$wtfdef = $this->db->fetchColumn(1,true);
			if($wtfword != NULL && $wtfdef != NULL)
				$this->sendAnswer($wtfword . " = " . $wtfdef);
		}
		else
		{
			$this->sendAnswer("В базе нет поредлений =(");
		} 
	}
	
	protected function command_wtfrev()
	{
		$w = $this->seperate($this->last_message);
		$this->db->query("SELECT revision FROM wiki WHERE name = '" . $this->db->db->escapeString($w[1]) . "' ORDER BY revision DESC LIMIT 0,1;");
		if($this->db->numRows(true) > 0)
			$this->sendAnswer("Ревизия: " . $this->db->fetchColumn(0));
		else
			$this->sendAnswer("Такого определения нету в базе");
		break;
	}
	
	protected function command_wtfull()
	{
		$w = $this->seperate($this->last_message);
		$this->db->query("SELECT * FROM wiki WHERE name = '" . $this->db->db->escapeString($w[1]) . "' ORDER BY revision DESC;");
		$list_rev = NULL;
		while($tmp = $this->db->fetch_array())
			$list_rev .= "\n------- Ревизия {$tmp['revision']} ({$tmp['name']}) -------\n{$tmp['value']}\n---------------------";
		if($list_rev != NULL)
			$this->sendAnswer($list_rev);
		else
			$this->sendAnswer("Такого определения нету в базе");
		break;
	}
	
	protected function command_wtfset()
	{
		$w = $this->seperate($this->last_message, 3);
		$this->db->query("SELECT name,revision,value FROM wiki WHERE name = '" . $this->db->db->escapeString($w[1]) . "' AND revision='" . $this->db->db->escapeString($w[2]) . "' LIMIT 0,1;");
		if($this->db->numRows(true) > 0)
		{
			$name = $this->db->fetchColumn(0);
			$rev = $this->db->fetchColumn(1,true);
			$val = $this->db->fetchColumn(2,true);
			$this->db->query("SELECT revision FROM wiki WHERE name = '" . $this->db->db->escapeString($name) . "' ORDER BY revision DESC LIMIT 0,1;");
			$newrev = ((int)$this->db->fetchColumn(0))+1;
			$this->db->query("INSERT INTO wiki (`name`,`revision`,`value`) VALUES ('" . $this->db->db->escapeString($name) . "','" . $this->db->db->escapeString($newrev) . "','".$this->db->db->escapeString($val)."');");
			$this->sendAnswer("Set to revision {$rev}.");
		}
		else
		{
			$this->sendAnswer("Такого определения нету в базе");
		} 
	}
	
  	protected function command_top()
	{
		$this->db->query("SELECT `lexeme`,`count` FROM lexems ORDER BY count DESC LIMIT 0,10;");
		$this->sendAnswer("10 самых популярных связок слов:");
		$ans = "";
		$ix = 0;
		while($lex = $this->db->fetch_array())
		{
			$ix++;
			$tmp = explode(" ", $lex['lexeme']);
			if($tmp[0] == "#beg#")
				$tmp[0] = "(начало)";
			if($tmp[2] == "#end#")
				$tmp[2] = "(конец)";
			$ans .= $ix . ". " . implode(" ", $tmp) . " [{$lex['count']}]" . "\n";
		}
		$this->sendAnswer($ans);
	}

  	protected function command_talkers()
	{
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
			$ans .= $i . ". " . $this->getName($key) . " с " . $val . " фразами.\n";
			if($i>=10)
				break;
		}
		$this->sendAnswer($ans);
	}
	
	protected function command_count()
	{
		$this->db->query("SELECT COUNT(*) FROM lexems;");
		$lexnum = (int)$this->db->fetchColumn(0);
		$this->sendAnswer("Количество слов-связок в базе: $lexnum");
	}
	
	protected function command_dfn()
	{
		$w = $this->seperate($this->last_message);
      
		$this->db->query("SELECT revision FROM wiki WHERE name = '" . $this->db->db->escapeString($w[1]) . "' ORDER BY revision DESC LIMIT 0,1;");
		if($this->db->numRows(true) > 0)
		{
			$rev = (int)$this->db->fetchColumn(0);
			$this->db->query("INSERT INTO wiki (`name`,`revision`,`value`) VALUES ('" . $this->db->db->escapeString($w[1]) . "','" . $this->db->db->escapeString($rev+1) . "','".$this->db->db->escapeString($w[2])."');");
		}
		else
		{
			$this->db->query("INSERT INTO wiki (`name`,`revision`,`value`) VALUES ('" . $this->db->db->escapeString($w[1]) . "','1','".$this->db->db->escapeString($w[2])."');");
		}
		
		$this->log->log("User set wiki page $w[1] = $w[2]", PichiLog::LEVEL_DEBUG);
		$this->sendAnswer("Value Updated!");
	}
	
	protected function command_set()
	{
		if(!$this->isAccess())
			return;
      
		$w = $this->seperate($this->last_message);
		if($this->setOption($w[1], $w[2]))
			$this->sendAnswer("Option updated!");
		else	
			$this->sendAnswer("Нету такой опции =(");
	}
	
	protected function command_msg()
	{
		if(!$this->isAccess())
			return;
      
		$w = $this->seperate($this->last_message, 3);

		$user = $this->getJID($w[1]);
		$message = $w[3];
		if($w[2] != "chat" && $w[2] != "groupchat")
			$w[2] = "chat";
		
		$this->jabber->message($user, $message, $w[2]);
		$this->log->log("Send message to $user: $message", PichiLog::LEVEL_DEBUG);
	}
	
	protected function command_gc()
	{
		if(!$this->isAccess())
			return;
      
		$w = $this->seperate($this->last_message);
		$this->db->query("SELECT * FROM settings" . (($w[1] != NULL) ? " WHERE name='".$this->db->db->escapeString($w[1])."'" : "") . ";");
		while($data = $this->db->fetch_array())
		{
			$this->sendAnswer($data['name'] . " = " . $data['value'] . " //{$data['description']}");
			$this->log->log("User request setting: {$data['name']} = {$data['value']}", PichiLog::LEVEL_VERBOSE);
		}
	}
	
	protected function command_users()
	{
		$w = $this->seperate($this->last_message);
      
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

	protected function command_join()
	{
		if(!$this->isAccess())
			return;
		$w = $this->seperate($this->last_message, 3);
		$this->joinRoom($w[1], $w[2], $w[3]);
	}
	
	protected function command_left()
	{
		if(!$this->isAccess())
			return;
		$w = $this->seperate($this->last_message, 3);
		$this->leftRoom($w[1], $w[2], $w[3]);
	}
	
	protected function command_ping()
	{
		$w = $this->seperate($this->last_message);
		if($this->isOnline($w[1]))
			$this->ping($w[1]);
		else
			$this->sendAnswer("Такого пользователя нет =(");
	}
	
	protected function command_topic()
	{
		$w = $this->seperate($this->last_message);
		$this->jabber->setTopic($this->room, $w[1]);
	}
	
	protected function command_idle()
	{
		$w = $this->seperate($this->last_message);
		$this->db->query("SELECT `time` FROM log WHERE `from` = '" . $this->db->db->escapeString($w[1]) . "' OR `from` LIKE '%/" . $this->db->db->escapeString($this->getName($w[1])) . "' ORDER BY time DESC;");
		$date = (int)$this->db->fetchColumn(0);
		if($date > 0)
			$this->sendAnswer(date("d.m.y \в H:i:s", $date));
	}
	
	protected function command_greet()
	{
		$w = $this->seperate($this->last_message, 3);
		if(!$this->isAccess())
			return;
		$action = ($w[0] == "!greet") ? "user_join_room" : "user_left_room";
		$this->db->query("SELECT COUNT(*) FROM actions WHERE action = '$action' AND coincidence='room=" . $this->db->db->escapeString($w[2]) . ",jid=" . $this->db->db->escapeString($w[1]) . "';");
		if($this->db->fetchColumn() > 0)
			$this->db->query("UPDATE actions SET value = '".$this->db->db->escapeString($w[3])."'  WHERE action = '$action' AND coincidence='room=" . $this->db->db->escapeString($w[2]) . ",jid=" . $this->db->db->escapeString($w[1]) . "';");
		else
			$this->db->query("INSERT INTO actions (`action`,`coincidence`,`do`,`option`,`value`) VALUES ('$action', 'room=" . $this->db->db->escapeString($w[2]) . ",jid=" . $this->db->db->escapeString($w[1]) . "', 'send_message', '', '".$this->db->db->escapeString($w[3])."');");
		$this->sendAnswer("Updated!");
	}
	
	protected function command_quit()
	{
		$this->doExit();
	}
}



?>
