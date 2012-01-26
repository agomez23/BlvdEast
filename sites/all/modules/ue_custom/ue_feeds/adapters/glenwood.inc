<?php
	
	require_once 'streeteasy.inc';
	
	class ue_feeds_adapter_glenwood extends ue_feeds_adapter_streeteasy
	{
		protected function parseProperty($xml)
		{
			$prop = parent::parseProperty($xml);
			
			$prop->externalID = (string)$xml->location->address;
			
			return $prop;
		}
	}
	
	ue_feeds_register_adapter('Glenwood', '', 'ue_feeds_adapter_glenwood');