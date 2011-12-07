<?php
	require_once 'mits.inc';
	
	class ue_feeds_adapter_mits_omg extends ue_feeds_adapter_mits
	{
		public function feed(&$nodes)
		{
			parent::feed($nodes);
			
			foreach($this->root->Property as $prop) $prop[managementID] = 1;
			
			return $this->root->asXml();
		}
		
		protected function feedCompanies(&$properties, &$parent)
		{
			$node = $parent->addChild('Management');
			$node->addAttribute('Id', 1);
			$node->Name = 'Urban Edge';
			$node->Phone->Number = '212-929-2780';
			$node->Fax->Number = '646-349-1279';
			$node->Email = 'support@urbanedgeny.com';
			$node->Address->Address1 = '12 W 23rd St';
			$node->Address->Address2 = '4th Floor';
			$node->Address->City = 'New York';
			$node->Address->State = 'NY';
			$node->Address->Zip = 10010;
		}
	}
	
	ue_feeds_register_adapter('MITS - Urban Edge', 'MITS adapter to override management to say Urban Edge.', 'ue_feeds_adapter_mits_omg');