<?php
	
	class ue_feeds_adapter_rockrose extends ue_feeds_adapter implements ue_feeds_adapter_input
	{
		//fetch a company node.
		protected $parsed = NULL;
		
		public function parse($reset = false)
		{
			if(!$this->parsed || $reset)
			{
				$xml = simplexml_load_string($this->feed->read(), null, LIBXML_NOCDATA);
				
				foreach($xml as $propXml)
				{
					$prop = $this->parseProperty($propXml);
					if($prop->hood) $this->parsed['properties'][] = $prop;
				}
			}
			
			return $this->parsed;
		}
		
		protected function parseProperty(&$xml)
		{
			$eid = $xml->address->zip . '-' . $xml->address->address1;
			
			$prop = new ue_feeds_property();
			$prop->externalID = $eid;
			
			$prop->addr1 = trim($xml->address->address1);
			$prop->addr2 = trim($xml->address->address2);
			$prop->city = trim($xml->address->city);
			$prop->state = trim($xml->address->state);
			$prop->zip = trim($xml->address->zip);
			
			$prop->name = trim($xml->name);
			if(!strlen($prop->name)) $prop->name = $prop->addr1;
			
			$prop->desc = trim($xml->description);
			
			if(strlen(trim($xml->website)))
				$prop->website = trim($xml->website);

      // disabled property photos - they seem to be duplicates of the building photos.
			//$prop->photos = $this->parseFiles($xml->photos);
			
			$prop->contact_name = trim($xml->agent->name);
			$prop->phone = trim($xml->agent->phone);
			
			//fix for multiple emails - 4/4/11 dw
			$emails = explode(',', $xml->agent->emails->email);
			$prop->email = trim($emails[0]);
			
			$hood = ue_feeds_get_neighborhoods($prop->zip);
			if($hood) $prop->hood = $hood->tid;
			
			foreach($xml->units->unit as $unitXml)
				$prop->listings[] = $this->parseListing($unitXml);
			
			return $prop;
		}
		
		protected function parseListing(&$xml)
		{
			$listing = new ue_feeds_listing();
			
			$listing->name = trim($xml->number);
			
			$bed = 'num-bedrooms';
			$bath = 'num-bathrooms';
			
			$listing->bed = (float)$xml->$bed;
			$listing->bath = (float)$xml->$bath;
			
			$listing->price = (float)preg_replace('/[^\d]/', '', $xml->price);
			
			// parse their date available, seems to be either mm/dd or "immediate availability"
			$date = 'date-available';
			$listing->dateAvail = strtotime($xml->$date);
			
			// we unset it (default to immediate on our site) if it is anything other than a date in the future
			if($listing->dateAvail < time())
				unset($listing->dateAvail);
			
			if(strlen(trim($xml->details)))
				$listing->desc = trim($xml->details);
			
			// disabled unit photos - they seem to be duplicates of the building photos.
			//$listing->photos = $this->parseFiles($xml->photos);
			
			return $listing;
		}
		
		protected function parseFiles(&$xml)
		{
			foreach($xml->photo as $photo)
				$photos[] = trim($photo);
			
			return $photos;
		}
	}
	
	ue_feeds_register_adapter('Rockrose', '', 'ue_feeds_adapter_rockrose');
