<?php

global $config;

$xml_config = simplexml_load_file("pichi.xml");

$config['version'] = (int)$xml_config->version; // init version

foreach($xml_config->config->children() as $cfg)
{
	$name = $cfg->attributes()->Name;
	switch($cfg->attributes()->type)
	{
		case "string":
			$config["{$name}"] = (string)$cfg;
			break;
		case "int":
			$config["{$name}"] = (int)$cfg;
			break;
		case "bool":
			$config["{$name}"] = (bool)(int)$cfg;
			break;
		case "array":
			$config["{$name}"] = explode($cfg->attributes()->sepperator, $cfg);
			break;
	}
	
}

unset($xml_config);
?>
