<?php


class PichiPlugin
{
	private static $pluginlist = array(
					'main_init_plugin_system',
					'main_jabber_connected',
					'main_creating_db',
					'main_session_start',
					'main_handle_cycle',
					'pichicore_room_join',
					'pichicore_room_left',
					'pichicore_answer_send',
					'pichicore_options_parse',
					'pichicore_status_update',
					'pichicore_message_recive_begin',
					'pichicore_message_recive_complete',
					'commands_show_help',
					'commands_show_version',
					'commands_fetch',
					'commands_message_recive_begin',
					'commands_message_recive_complete',
					'event_action'
					  );
	
	private static $plugins = array();
	private static $dir = "/plugins/";
	
	// Constructor
	function PichiPlugin()
	{
	}
	
	public static function init()
	{
		$opendir = getcwd() . self::$dir;
		$dir = opendir($opendir);
		while(($file = readdir($dir)) !== false)
		{
			if (!preg_match('#^(.*).xml$#i', $file, $matches))
			{
				continue;
			}

			$xml = simplexml_load_file($opendir . $file, 'SimpleXMLElement', LIBXML_NOCDATA);
			$index = (string)$xml->index;
			self::$plugins[$index]['name'] = (string)$xml->name;
			self::$plugins[$index]['description'] = (string)$xml->description;
			self::$plugins[$index]['version'] = (double)$xml->version;
			self::$plugins[$index]['enabled'] = (bool)(int)$xml->enabled;
			
			foreach($xml->code->children() as $hook)
			{
				$name = (string)$hook->attributes()->name;
				self::$plugins[$index]['code'][$name] = (string)$hook[0];
			}
		}
	}

	public static function show_plugin_list()
	{
		$pluglist = NULL;
		$i = 1;
		if(self::$plugins != NULL)
			foreach(self::$plugins as $name=>$plugin)
				$pluglist .= $i++ . ". {$plugin['name']} ({$plugin['description']}) v{$plugin['version']} - " . (($plugin['enabled']) ? "ON\n" : "OFF\n");
		return $pluglist;
	}
	
	public static function fetch_hook($hookname = false)
	{
		$code = NULL;
		if(in_array($hookname, self::$pluginlist) && self::$plugins != NULL)
		{
			foreach(self::$plugins as $key=>$plugin)
			{
				if(@$plugin['code'][$hookname] != NULL && $plugin['enabled'])
					$code .= $plugin['code'][$hookname];
			}
		}
		return $code;
	}
	
	private static function change($index, $enable = TRUE)
	{
		if(is_string($index))
		{
			self::$plugins[$index]['enabled'] = $enable;
		}
		else if(is_int($index) && $index > 0 && $index <= count(self::$plugins))
		{
			$plugs = self::$plugins; //copy!
			
			for($i = 0; $i < $index - 1; $i++)
				next($plugs);
			
			self::$plugins[key($plugs)]['enabled'] = $enable;
		}
	}
	
	public static function enable($index)
	{
		self::change($index, TRUE);
	}
	
	public static function disable($index)
	{
		self::change($index, FALSE);
	}
}

?>
