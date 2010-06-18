<?php

require_once dirname(__FILE__) . "/XMPP.php";

abstract class XMPP_InputOutput_Abstract extends XMPPHP_XMPP
{
	protected $reciver; // Main recive var
	
	protected $reciver_read = null;
	protected $reciver_write = null;
	
	public function __construct($host, $port, $user, $password, $resource, $server = null, $printlog = false, $loglevel = null)
	{
		parent::__construct($host, $port, $user, $password, $resource, $server, $printlog, $loglevel);
		$this->createReciver();
		$this->runReciverTest();
	}
	
	abstract protected function createReciver();
	abstract protected function getReciver();
	protected function sendReciver()
	{
		$this->send($this->reciver_write);
		$this->reciver_write = NULL;
	}
	
	protected function reciveData($data)
	{
		$this->reciver_read = $data;
		$this->getReciver();
		$this->reciver_read = NULL;
	}
	
	abstract protected function runReciverTest();
}

?>
