<?php

require_once("Color.php");

class PichiLog {
	
	const LEVEL_ERROR   = 0;
	const LEVEL_WARNING = 1;
	const LEVEL_INFO	= 2;
	const LEVEL_DEBUG   = 3;
	const LEVEL_VERBOSE = 4;
	
	protected $data = array();
	protected $names = array('ERROR', 'WARNING', 'INFO', 'DEBUG', 'VERBOSE');
	protected $colors = array('%r', '%y', '%c', '%g', '%w');
	protected $runlevel;
	protected $printout;

	public function __construct($printout = false, $runlevel = self::LEVEL_INFO)
	{
		$this->printout = (boolean)$printout;
		$this->runlevel = (int)$runlevel;
	}

	public function log($msg, $runlevel = self::LEVEL_INFO)
	{
		$time = time();
		#$this->data[] = array($this->runlevel, $msg, $time);
		if($this->printout and $runlevel <= $this->runlevel)
		{
			$this->writeLine($msg, $runlevel, $time);
		}
	}
	
	protected function writeLine($msg, $runlevel, $time)
	{
		global $config;
		if(RUN_OS == "Nix")
			if($config['daemon_mode'])
				System_Daemon::log(System_Daemon::LOG_INFO, "[".$this->names[$runlevel]."]: " . $msg);
			else
				echo Console_Color::convert($this->colors[$runlevel] . "(" . date('Y-m-d H:i:s', $time).") [".$this->names[$runlevel]."]: ".$msg."%n\n");
		else if(RUN_OS == "Windows")
			echo iconv("UTF-8","cp866","(" . date('Y-m-d H:i:s', $time).") [".$this->names[$runlevel]."]: ".$msg."\n");
		else
			echo "(" . date('Y-m-d H:i:s', $time).") [".$this->names[$runlevel]."]: ".$msg."\n";
		flush();
	}
}
