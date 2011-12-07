<?php
	require_once 'mits.inc';
	class ue_feeds_adapter_mits_nounits extends ue_feeds_adapter_mits
	{
		protected function parseFloorplan($xml)
		{
			$listing = parent::parseFloorplan($xml);
			
			//Set unit count to 1 for feeds that don't include it.
			$listing->numAvailable = 1;
			
			return $listing;
		}
	}
	
	ue_feeds_register_adapter('MITS - No units', 'MITS adapter to parse feeds with no \'numAvailable\' in listing.', 'ue_feeds_adapter_mits_nounits');