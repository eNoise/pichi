<?php

class SyntaxAnalizer
{
  
	protected $user_text;
	protected $send_text;
  
	public $db;
	public $log;
  
	//Settings
	public $limit_word = 2;
	public $from_world_coi = 3;

	public function __construct()
	{
		//$this->log->log("Init syntaxis analizator", PichiLog::LEVEL_DEBUG);
	}

	public function parseText($string)
	{
		$this->log->log("$string to lexems", PichiLog::LEVEL_DEBUG);
		$base = explode(" ", $string);
      
		if($base[0] != NULL)
		{
			$str = "#beg# " . $base[0];
			$this->addLexema($str);      
		}
      
		for($i = 0; $i < count($base) ; $i++)
		{
			$str = $base[$i] . (($base[$i+1]) ? " " . $base[$i+1] : " #end#");
			$this->addLexema($str);
		}
		
		$this->user_text = $string;
	}
	
	public function returnText()
	{
		return $this->send_text;
	}
	
	private function addLexema($str)
	{	
		if($str == NULL)
			return;
      
		$this->db->query("SELECT COUNT(*) FROM lexems WHERE lexeme = '" . $this->db->db->escapeString($str) . "';");
		if($this->db->fetchColumn() > 0)
			$this->db->query("UPDATE lexems SET count = count+1  WHERE lexeme = '".$this->db->db->escapeString($str)."';");
		else
			$this->db->query("INSERT INTO lexems (`lexeme`,`count`) VALUES ('" . $this->db->db->escapeString($str) . "','1');");
		$this->log->log("$str writed to lexems", PichiLog::LEVEL_VERBOSE);
	}
	
	private function choseLexem(& $answers)
	{
		$this->log->log("Rand world from lexems:", PichiLog::LEVEL_VERBOSE);
		$sum = 0;
		foreach($answers as $key=>$data)
		{
			$sum += (int)$data['count'];
			$this->log->log("World \"{$data['lexeme']}\" have {$data['count']} points", PichiLog::LEVEL_VERBOSE);
		}
		$rand = rand(0, $sum);
      
		$temp_sum = 0;
		$num = 0;
		while($temp_sum < $rand)
			$temp_sum += $answers[$num++]['count'];

		return $answers[(($num>0) ? $num-1 : $num)]['lexeme'];
	}
	
	public function generate()
	{
		if(rand(1, $this->from_world_coi) === 1)
		{
			$this->log->log("Full Random method", PichiLog::LEVEL_DEBUG);
			$this->send_text = $this->genFullRandom();
		}
		else
		{
			$this->log->log("Word method", PichiLog::LEVEL_DEBUG);
			$words = explode(" ", $this->user_text);
			$word = $this->choseWord($words);
			$this->send_text = $this->genFromCenterWord($word);
		}
	}
	
	private function choseWord(& $array)
	{
		if(count($array) < 1)
			return false;
		$good = array();
		//first
		foreach($array as $word)
		{
			if(strlen($word) > 2)
				$good[] = $word;
		}
		if(count($good) > 0)
		{
			$rand = rand(0, count($good)-1);
			$return = $good[$rand];
		}
		else
		{
			$rand = rand(0, count($array)-1);
			$return = $array[$rand];
		}
		$this->log->log("Choise word: $return", PichiLog::LEVEL_DEBUG);
		return $return;
	}


	private function genFullRandom($limit = 20)
	{
		$this->db->query("SELECT * FROM lexems WHERE lexeme LIKE '#beg# %';");
		if($this->db->numRows(true) == 0)
			return; //пусто
		$last = $this->choseLexem($this->buildArray());
		$last = explode(" ",$last);
		$genans = $last = $last[1];
		for($i=0; $i < $limit; $i++)
		{
			$this->db->query("SELECT * FROM lexems WHERE lexeme LIKE '" . $this->db->db->escapeString($last) . " %';");
			if($this->db->numRows(true) == 0)
				break; //больше нет совпадений
			$last = $this->choseLexem($this->buildArray());
			$last = explode(" ",$last);
			$last = $last[1];
	
			if($last == "#end#" || $last==NULL)
				break;
	
			$genans .= " " . $last;
		}
		return $genans;
	}
	
	private function genFromCenterWord($word, $limit = 10)
	{
		$answer = $word;
		//left
		for($i = 0; $i < $limit; $i++)
		{
			$this->db->query("SELECT * FROM lexems WHERE lexeme LIKE '% " . $this->db->db->escapeString(($i == 0) ? $word : $first) . "';");
			if($this->db->numRows(true) == 0)
				break; //больше нет совпадений
			$last = $this->choseLexem($this->buildArray());
			list($first, $second) = explode(" ", $last);
				
			if($first == "#beg#" || $first == NULL)
				break;
			$answer = $first . " " . $answer;
		}
		//right
		for($i = 0; $i < $limit; $i++)
		{
			$this->db->query("SELECT * FROM lexems WHERE lexeme LIKE '" . $this->db->db->escapeString(($i == 0) ? $word : $second) . " %';");
			if($this->db->numRows(true) == 0)
				break; //больше нет совпадений
			$last = $this->choseLexem($this->buildArray());
			list($first, $second) = explode(" ", $last);
				
			if($second == "#end#" || $second == NULL)
				break;
			$answer .= " " . $second;
		}
		return $answer;
	}
	
	private function buildArray()
	{
		$answers = array();
		$i = 0;
		while($data = $this->db->fetch_array())
		{
			$answers[$i]['lexeme'] = $data['lexeme'];
			$answers[$i]['count'] = $data['count'];
			$i++;
		}
		return $answers;
	}
}

?>
