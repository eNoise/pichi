<?php


class PichiLang
{
	private static $language = array();
	private static $dir = "languages/";
	private static $lastlang;
	
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
		if(file_exists(self::$dir . $lang . '.xml'))
		{
			$xml = simplexml_load_file(self::$dir . $lang . '.xml');
			
			foreach($xml->language->children() as $l)
			{
				self::$language[(string)$l->attributes()->name] = (string)$l;
			}
			self::$lastlang = $lang;
		}
	}
	
	public static function clear()
	{
		self::$language = array();
	}

	public static function get($name, $param = array())
	{
		$string = self::$language[$name];
		if(count($param) > 0)
		{
			$string = str_replace('%', '%%', $string);
			$string = preg_replace('#\{([0-9]+)\}#sU', '%\\1$s', $string);
			$string = @call_user_func_array('sprintf', array_merge(array($string), $param));
		}
		return $string;
	}
	
	public static function set($name, $value, $lang = NULL)
	{
		if($lang == NULL || ($lang != NULL && self::$lastlang == $lang))
			self::$language[$name] = $value;
	}
}

?>
