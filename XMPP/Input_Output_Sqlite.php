<?php

require_once dirname(__FILE__) . "/Input_Output_Abstract.php";

class XMPP_InputOutput_Sqlite extends XMPP_InputOutput_Abstract
{	
	protected $reciver_time_interval = 5;
	protected $reciver_time = NULL;
	protected $reciver_name = '';
	
	public function __construct($host, $port, $user, $password, $resource, $server = null, $printlog = false, $loglevel = null, $name = 'xmppdata.db', $time = 5)
	{
		parent::__construct($host, $port, $user, $password, $resource, $server, $printlog, $loglevel);
		$this->reciver_time_interval= $time;
		$this->reciver_name = $name;
	}
	
	protected function createReciver()
	{
		if(!file_exists($this->reciver_name))
		{
			$this->reciver = new SQLite3($this->reciver_name);
			$this->reciver->query('CREATE TABLE input (`time` TEXT, `data` TEXT);');
			$this->reciver->query('CREATE TABLE output (`time` TEXT, `data` TEXT);');
		}
		else
		{
			$this->reciver = new SQLite3($this->reciver_name);
			$this->reciver->query('DELETE FROM input;');
			$this->reciver->query('DELETE FROM output;');
		}
	}
	
	protected function getReciver()
	{
		$this->reciver->query("INSERT INTO input(`time`,`data`) VALUES(`" . time() ."`, `" . $this->reciver->escapeString($this->reciver_read) . "`);");
	}

	
	protected function runReciverTest()
	{
		if(time() - $this->reciver_time > $this->reciver_time_interval && is_object($this->reciver))
		{
			$query = $this->reciver->query("SELECT * FROM output;");
			$this->reciver_write = '';
			while($data = $query->fetchArray())
				$this->reciver_write .= $data['data'];
			$this->sendReciver();
			$this->reciver->query('DELETE FROM output;');
			$this->reciver_time = time();
		}
	}
	
	protected function __post_process()
	{
		parent::__post_process();
		$this->runReciverTest();
	}
}

?>
