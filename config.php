<?php

// --------------------------

$config['server'] = "jabber.uruchie.org"; // Jabber server
$config['port'] = 5222; //With TLS
$config['room_service'] = "conference"; // Jabber service for rooms
$config['room'] = "main"."@".$config['room_service'].".".$config['server']; // Room to autojoin
$config['db_file'] = "pichi.db"; // Room to autojoin
$config['debug'] = TRUE; // Debug mode
$config['debug_level'] = 4; // Debug mode levels (0-4) 4 - max debug info

$config['user'] = "pichi";
$config['room_user'] = "pichi";
$config['password'] = "iampichi";
$config['resource'] = "BotWorld!";
$config['status'] = "From hell!";

$config['global_admins'] = 1; // Use global admin setting from jabber
$config['admins'] = array("deg@jabber.uruchie.org"); // Enter addition admins

### DO NOT MODIFY! ###
$config['ping_time'] = 5; //If reconnect problem (in openfire)
$config['wait_time'] = 5; //Secs to waiting after connect
######################

$config['version'] = 2; //Version of config file (Modify this only if you sure that all new options sets)

?>