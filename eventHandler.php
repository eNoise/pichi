<?php

class eventHandler
{
	//class for catching events
	public $db;
	public $log;
	public $jabber;
  
	public function __construct()
	{
	}
  
	public function catchEvent($action, $coincidence = NULL)
	{
		$this->db->query("SELECT `do`, `option`,`value`,`coincidence` FROM actions WHERE action = '$action' AND coincidence='" . (($coincidence != NULL) ? $this->db->db->escapeString($coincidence) : "" ) . "';");
		while($actions = $this->db->fetch_array())
		{
			$this->doAction($actions['do'], $actions['value'], $actions['option'], $actions['coincidence']);
		}
	}
	
	private function doAction($action, $value, $option, $coincidence = NULL)
	{
		switch($action)
		{
			case "send_message":
				if($coincidence != NULL)
				{
					$room = explode(",", $coincidence);
					$room = explode("=", $room[0]);
					if($room[0] == "room")
					{
						$room = $room[1];
					}
					else
					{
						$this->log->log("Old send_message handler", PichiLog::LEVEL_WARNING);
						break;
					}
				}
				else
				{
					break;
				}
				if($option == NULL)
					if($value != NULL)
						$this->jabber->message($room, $value, "groupchat");
				else
					if($actions['value'] != NULL)
						$this->jabber->message($option, $value, "chat");
				$this->log->log("EVENT: {$action} (Send message {$value})", PichiLog::LEVEL_VERBOSE);
		}
	}
}


?>
