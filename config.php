<?php

// --------------------------

$config['server'] = "jabber.uruchie.org"; // Jabber server
$config['port'] = 5222; //With TLS
$config['room_service'] = "conference"; // Jabber service for rooms
$config['room'] = "main"."@".$room_service.".".$server; // Room to autojoin
$config['db_file'] = "pichi.db"; // Room to autojoin
$config['debug'] = TRUE; // Debug mode

$config['user'] = "pichi";
$config['room_user'] = "pichi";
$config['password'] = "iampichi";
$config['resource'] = "BotWorld!";
$config['status'] = "From hell!";

### DO NOT MODIFY! ###
$config['ping_time'] = 5; //If reconnect problem (in openfire)
$config['wait_time'] = 8; //Secs to waiting after connect
######################

?>