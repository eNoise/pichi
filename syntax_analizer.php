<?php

class SyntaxAnalizer
{
  
	protected $user_text;
	protected $send_text;
	
	public $db;
	public $log;
  
	//Settings
	public $limit_word = 2;
	public $query_limit = 10;
	public $from_world_coi = 3;

	private $try_count = 0; //попытки построить выражения
	public $try_limit = 3; //лимит попыток
	
	public function __construct()
	{
		//$this->log->log("Init syntaxis analizator", PichiLog::LEVEL_DEBUG);
	}

	public function parseText($string)
	{
		$this->testText($string);
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
		$this->try_count = 0; // сбрасываем попытки
	  
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


	private function genFullRandom($limit = NULL)
	{
		if($limit == NULL)
			$limit = rand(5,20);
		$this->try_count++; //очередная попытка
		$this->db->query("SELECT * FROM lexems WHERE lexeme LIKE '#beg# %' ORDER BY `count` DESC;");
		if($this->db->numRows(true) == 0)
			return; //пусто
		$last = $this->choseLexem($this->buildArray());
		$last = explode(" ",$last);
		$genans = $last = $last[1];
		for($i=0; $i < $limit; $i++)
		{
			$this->db->query("SELECT * FROM lexems WHERE lexeme LIKE '" . $this->db->db->escapeString($last) . " " . (($i == $limit-1) ? "#end#" : "%") . "' ORDER BY `count` DESC LIMIT 0," . $this->query_limit . ";");
			if($this->db->numRows(true) == 0)
				break; //больше нет совпадений
			$last = $this->choseLexem($this->buildArray());
			$last = explode(" ",$last);
			$last = $last[1];
	
			if($last == "#end#" || $last==NULL)
				break;
	
			$genans .= " " . $last;
		}
		if($genans != $this->user_text)
			return $genans;
		else
			return $this->randFromLog(); //возращать тоже самое нехорошо, вернем что-нибудь из лога
	}
	
	private function genFromCenterWord($word, $limit = NULL)
	{
		if($limit == NULL)
			$limit = rand(7,12);
		$answer = $word;
		$this->try_count++; //очередная попытка
		//left
		for($i = 0; $i < $limit; $i++)
		{
			$this->db->query("SELECT * FROM lexems WHERE lexeme LIKE '" . (($i == $limit-1) ? "#beg#" : "%") . " " . $this->db->db->escapeString(($i == 0) ? $word : $first) . "' ORDER BY `count` DESC LIMIT 0," . $this->query_limit . ";");
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
			$this->db->query("SELECT * FROM lexems WHERE lexeme LIKE '" . $this->db->db->escapeString(($i == 0) ? $word : $second) . " " . (($i == $limit-1) ? "#end#" : "%") . "' ORDER BY `count` DESC LIMIT 0," . $this->query_limit . ";");
			if($this->db->numRows(true) == 0)
				break; //больше нет совпадений
			$last = $this->choseLexem($this->buildArray());
			list($first, $second) = explode(" ", $last);
				
			if($second == "#end#" || $second == NULL)
				break;
			$answer .= " " . $second;
		}
		if($answer != $this->user_text)
			return $answer;
		else
			return $this->randFromLog(); //возращать тоже самое нехорошо, вернем что-нибудь из лога
	}
	
	//Использовать случайную фразу из лога
	// 3 слова как лимит
	// смайл если невозможно найти подходящей фразы
	private function randFromLog($words_limit = 3, $dafault = ":(")
	{
		$this->try_count++; //очередная попытка
		$this->db->query("SELECT message FROM log ORDER BY RANDOM() LIMIT 0,10;");
		while($text = $this->db->fetch_array())
			if(count(explode(" ", $text['message'])) <= $words_limit)
				return $text['message'];
		return $dafault;
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

	private function testText(& $text)
	{
		$this->log->log("$text before test to lexems", PichiLog::LEVEL_VERBOSE);
		$text = str_replace("\n", " ", $text);
		$test1 = explode(",",$text);
		$test2 = explode(":",$text);
		$ignore_nick = array();
		$this->db->query("SELECT nick FROM users WHERE status='available';");
		while($u = $this->db->fetch_array())
			$ignore_nick[] = $u['nick'];
		if(in_array($test1[0], $ignore_nick))
		{
			unset($test1[0]);
			$text = implode(",",$test1);
			ltrim($text);
		}
		if(in_array($test2[0], $ignore_nick))
		{
			unset($test2[0]);
			$text = implode(":",$test2);
			ltrim($text);
		}
		$this->log->log("$text after test to lexems", PichiLog::LEVEL_VERBOSE);
	}
}

?>
