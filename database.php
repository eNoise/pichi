<?php

class PichiDatabase
{
	public $db;
	private $name;
	private $type = "sqlite";
	public $last_query;
	public $last_query_text;
      
	public function __construct($name = "pichi.db", $prefix = "", $user = "", $pass = "", $host = "")
	{
		switch($this->type)
		{
			default:
				$this->db = new SQLite3($name);
				break;
		}
	}
      
	public function setTypeDB($type)
	{
		$this->type = $type;
	}
      
	public function query($query)
	{
		switch($this->type)
		{
			default:
				if($this->last_query_text != $query)
				{
					$this->last_query = $this->db->query($query);
					$this->last_query_text = $query;
				}
				$this->last_query->reset();
				return $this->last_query;
		}
	}

	public function fetch_array()
	{
		switch($this->type)
		{
			default:
				return $this->last_query->fetchArray();
		}
	}
      
	public function fetchColumn($column = 0)
	{
		switch($this->type)
		{
			default:
				$temp = $this->last_query->fetchArray(SQLITE3_NUM);
				return $temp[$column];
		}
	}

	public function numColumns()
	{
		switch($this->type)
		{
			default:
				return $this->last_query->numColumns();
		}
	}
	
	// $not_safe - быстро, но только вне циклов
	public function numRows($not_safe = false)
	{
		switch($this->type)
		{
			default:
				$n = 0;
				$this->last_query->reset();
				while($this->last_query->fetchArray())
					$n++;
				if($not_safe)
					$this->last_query->reset();
				else
					$this->last_query = $this->db->query($this->last_query_text);
				return $n;
		}
	}

}


?>