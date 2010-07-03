<?php

require_once 'Console/CommandLine.php'; // Command Line options
$args = new Console_CommandLine(array(
    'description' => 'Pichi bot',
    'version'     => $config['pichi_version']
));

$args->addOption('database', array(
    'short_name'  => '-b',
    'long_name'   => '--db',
    'action'      => 'StoreString',
    'description' => 'Database name',
    'default'     => "{$config['db_file']}" // a default value
));

$args->addOption('daemon', array(
    'short_name'  => '-d',
    'long_name'   => '--daemon',
    'action'      => 'StoreTrue',
    'description' => 'Run pichi as daemon (Only Linux)'
));

$args->addOption('user', array(
    'short_name'  => '-u',
    'long_name'   => '--user',
    'action'      => 'StoreInt',
    'description' => 'As userid of system'
));

$args->addOption('group', array(
    'short_name'  => '-g',
    'long_name'   => '--group',
    'action'      => 'StoreInt',
    'description' => 'As usergroup of system'
));

$args->addOption('log', array(
    'short_name'  => '-l',
    'long_name'   => '--log',
    'action'      => 'StoreString',
    'description' => 'Log file',
    'default'     => dirname(__FILE__) . "/pichi.log"
));

$args->addOption('pid', array(
    'short_name'  => '-p',
    'long_name'   => '--pid',
    'action'      => 'StoreString',
    'description' => 'Pid file',
    'default'     => dirname(__FILE__) . "/pichi-bot/pichi.pid"
));

$args->addOption('database', array(
    'short_name'  => '-b',
    'long_name'   => '--db',
    'action'      => 'StoreString',
    'description' => 'Database name',
    'default'     => "{$config['db_file']}" // a default value
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
	$config['db_file'] = $arg->options['database'];
	if($arg->options['verbose'])
	{
		$config['debug'] = TRUE;
	}
	if($arg->options['daemon'] && RUN_OS == "Nix")
	{
		System_Daemon::setOption("appName", "pichi-bot");
		System_Daemon::setOption("appDir", dirname(__FILE__));
		System_Daemon::setOption("logLocation", $arg->options['log']);
		System_Daemon::setOption("appPidLocation", $arg->options['pid']);
		if($arg->options['group'])
			System_Daemon::setOption("appRunAsGID", $arg->options['group']);
		if($arg->options['user'])
			System_Daemon::setOption("appRunAsUID", $arg->options['user']);
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

// leeks
unset($args);
unset($arg);

?>
