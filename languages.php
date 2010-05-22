<?php


class PichiLang
{
	private static $language = array();
	private static $dir = "/languages/";
	
	// Constructor
	function PichiLang()
	{
	}
	
	public static function init()
	{
		self::load('en'); // Language by default
	}

	public static function load($lang)
	{
		if(file_exists(self::$dir . $lang . '.xml')
		{
			$xml = simplexml_load_file(self::$dir . $lang . '.xml');
			
			foreach($xml_config->language->children() as $l)
			{
				self::$language[$l->attributes()->name] = (string)$l;
			}
		}
	}
	
	public static function clear()
	{
		self::$language = array();
	}

	public static function get($name)
	{
		return self::$language[$name];
	}
}

?>
