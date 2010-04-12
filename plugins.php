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
	
	private static $init_list = array();
	
	// Constructor
	function PichiPlugin()
	{
	}
	
	public static function init()
	{
		
	}
	
	public static function show_plugin_list()
	{
		
	}
	
	public static function fetch_hook($hookname = false)
	{
		return NULL;
	}
	
}

?>
