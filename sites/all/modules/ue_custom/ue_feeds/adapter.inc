<?php
	
	interface ue_feeds_adapter_input
	{
		public function parse($reset = false);
		
	}
	
	interface ue_feeds_adapter_output
	{
		public function feed(&$nodes);
	}
	
	interface ue_feeds_adapter_output_stream
	{
		public function feedStream(&$nodes);
	}
	
	abstract class ue_feeds_adapter
	{
		protected $feed = NULL;
		
		public $logMessages = array();
		
		const ADAPTER_LOGTYPE_INFO	= 1;
		const ADAPTER_LOGTYPE_WARN	= 2;
		const ADAPTER_LOGTYPE_ERROR	= 3;
		
		function __construct(ue_feeds_feed &$feed = NULL)
		{
			$this->feed = $feed;
		}
		
		public function feed()
		{
			return '';
		}
		
		public function parse($reset = false)
		{
			return array('companies' => array(), 'properties' => array());
		}
		
		// return an array if you want to add options to edit form.
		public function getForm()
		{
			return array();
		}
		
		public function processForm(&$form_state)
		{
		}
		
		// each feed may need to alter how we strip html because of differences in ways that entities are parsed.
		protected function stripHtml($html)
		{
			$trans = array_flip(get_html_translation_table(HTML_ENTITIES, ENT_QUOTES));
			$trans["\r"] = '';
			$trans['&#39;'] = "'";
			$trans['&nbsp;'] = ' ';
			$trans['&lsquo;'] = "'";
			$trans['&rsquo;'] = "'";
			$trans['&ldquo;'] = '"';
			$trans['&rdquo;'] = '"';
			$trans['&hellip;'] = '-';
			$trans['&bull;'] = '-';
			$trans['&mdash;'] = '-';
			$trans[chr(0x97)] = '-';
			$trans['&ndash;'] = '-';
			$trans['&ccedil;'] = 'c';
			$trans['&amp;'] = '&';
			$trans[chr(0xAE)] = '&reg;';
			$trans[chr(0xA0)] = ' ';
			$trans[chr(0xe2).chr(0x80).chr(0xa2)] = $trans[chr(0x96)] = $trans[chr(0xe2).chr(0x80).chr(0xa6)] = $trans[chr(0xe2).chr(0x80).chr(0x93)] = '-';
			$trans[chr(0xe2).chr(0x80).chr(0x98)] = $trans[chr(0xe2).chr(0x80).chr(0x99)] = "'";
			$trans[chr(0x91)] = $trans[chr(0x92)] = $trans[chr(0x93)] = $trans[chr(0x94)] = '"';
			$trans["\t"] = '';
			
			$output = trim(strip_tags(strtr($html, $trans)));
			$output = $this->encode($output);
			
			return $output;
		}
		
		protected function encode($str)
		{
			return mb_convert_encoding($str, 'ISO-8859-1');
		}
		
		protected function logMessage($message, $type = self::ADAPTER_LOGTYPE_INFO, $nid = NULL)
		{
			$msg['message'] = $message;
			$msg['type'] = $type;
			if($nid) $msg['nid'] = $nid;
			
			$this->logMessages[] = $msg;
		}
		
		protected function logInfo($message, $nid = NULL)
		{
			$this->logMessage($message, self::ADAPTER_LOGTYPE_INFO, $nid);
		}
		
		protected function logWarn($message, $nid = NULL)
		{
			$this->logMessage($message, self::ADAPTER_LOGTYPE_WARN, $nid);
		}
		
		protected function logError($message, $nid = NULL)
		{
			$this->logMessage($message, self::ADAPTER_LOGTYPE_ERROR, $nid);
		}
	}