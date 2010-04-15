<?php


class PichiPlugin
{
	private static $pluginlist = array(
					'main_init_plugin_system',
					'main_jabber_connected',
					'main_creating_db',
					'main_session_start',
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
				$pluglist .= "$i. {$plugin['name']} ({$plugin['description']}) v{$plugin['version']}";
		return $pluglist;
	}
	
	public static function fetch_hook($hookname = false)
	{
		$code = NULL;
		if(in_array($hookname, self::$pluginlist) && self::$plugins != NULL)
		{
			foreach(self::$plugins as $key=>$plugin)
			{
				if(@$plugin['code'][$hookname] != NULL)
					$code .= $plugin['code'][$hookname];
			}
		}
		return $code;
	}
	
}

?>
