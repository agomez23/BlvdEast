<?php
	require_once 'mits.inc';
	
	class ue_feeds_adapter_mits_rose extends ue_feeds_adapter_mits
	{
		protected $props = array();
		
		// unset unit count, invalid in their feed.
		protected function parseProperties($xml)
		{
			parent::parseProperties($xml);
			
			foreach($this->props as $property)
				unset($property->unitCount);
			
			return $this->props;
		}

		protected function parseProperty($xml)
		{
			$prop = parent::parseProperty($xml);
			
			// their MITS feed is really malformed and they have properties duplicated into multiple nodes, ID used to look something like this: (building id)-(listing id)
			//if(stripos($prop->externalID, '-') !== false)
			//	$prop->externalID = substr($prop->externalID, 0, stripos($prop->externalID, '-'));
			
			//now it looks like this: (company)-(building id)-(listing id)
			if(stripos($prop->externalID, '-') !== false)
				$prop->externalID = substr($prop->externalID, 0, strripos($prop->externalID, '-'));
			
			if($xml->File)
				$prop->photos = $this->parseFiles($xml->File);
			
			//gracefully handle duplicates.
			if($this->props[$prop->externalID])
			{
				$oldProp = $this->props[$prop->externalID];
				$oldProp->listings = array_merge($prop->listings, $oldProp->listings);
				$oldProp->photos = array_unique(array_merge($prop->photos, $oldProp->photos));
				$this->props[$prop->externalID] = $oldProp;
			}
			else
				$this->props[$prop->externalID] = $prop;
			
			return $prop;
		}
		
		protected function parseFloorplan($xml)
		{
			$listing = parent::parseFloorplan($xml);
			
			//Defaults date available for immediate availabilities
			if((int)$xml->UnitCount > 0)
				$listing->numAvailable = (int)$xml->UnitCount;
			
			//Rose feed unit IDs are unit numbers.
			$listing->name = $listing->externalID;
			
			return $listing;
		}
	}
	
	ue_feeds_register_adapter('MITS - Rose', 'MITS adapter for Rose feed.', 'ue_feeds_adapter_mits_rose');