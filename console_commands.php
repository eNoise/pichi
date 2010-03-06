<?php

require_once 'Console/CommandLine.php'; // Command Line options
$args = new Console_CommandLine(array(
    'description' => 'Pichi bot',
    'version'     => $config['pichi_version']
));

$args->addOption('config', array(
    'short_name'  => '-c',
    'long_name'   => '--config',
    'action'      => 'StoreString',
    'description' => 'Config name',
    'default'     => "{$config['db_file']}" // a default value
));

// add a global option to make the program verbose
$args->addOption('daemon', array(
    'short_name'  => '-d',
    'long_name'   => '--daemon',
    'action'      => 'StoreTrue',
    'description' => 'Run pichi as daemon (Only Linux)'
));


// add a global option to make the program verbose
$args->addOption('verbose', array(
    'short_name'  => '-v',
    'long_name'   => '--verbose',
    'action'      => 'StoreTrue',
    'description' => 'turn on verbose output (debug mode)'
));

try 
{
	$arg = $args->parse();
	$config['db_file'] = $arg->options['config'];
	if($arg->options['verbose'])
	{
		$config['debug'] = TRUE;
	}
	if($arg->options['daemon'] && RUN_OS == "Nix")
	{
		System_Daemon::setOption("appName", "pichi-bot");
		System_Daemon::setOption("appDir", dirname(__FILE__));
		System_Daemon::setOption("logLocation", dirname(__FILE__) . "/pichi.log");
		System_Daemon::setOption("appPidLocation", dirname(__FILE__) . "/" . System_Daemon::getOption("appName") . "/pichi.pid");
		System_Daemon::start();
		$config['daemon_mode'] = TRUE;
	}
	else
	{
		$config['daemon_mode'] = FALSE;
	}
} catch (Exception $exc) {
	$args->displayError($exc->getMessage());
}


?>
