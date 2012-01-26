<?php
	
	//include nytimes base feed for listing codes.
	require_once 'nytimes.inc';
	
	class ue_feeds_adapter_nytimes_xml extends ue_feeds_adapter implements ue_feeds_adapter_input
	{
		//fetch a company node.
		protected $parsed = NULL;
		
		const eidType_ID = 0;
		const eidType_Addr = 1;
		
		public function parse($reset = false)
		{
			if(!$this->parsed || $reset)
			{
				$xml = simplexml_load_string($this->feed->read(), null, LIBXML_NOCDATA);
				if(count($xml->listing))
					$this->parsed['properties'] = $this->parseProperties($xml->listing);
				elseif(count($xml->property))
					$this->parsed['properties'] = $this->parseProperties($xml->property);
			}
			
			return $this->parsed;
		}
		
		protected function parseProperties(&$xml)
		{
			foreach($xml as $propXml)
			{
				if($this->feed->options->eidType == self::eidType_Addr)
					$eid = trim($propXml->AddrZip) . '-' . trim($propXml->Address);
				else
					$eid = intval($propXml->IdWeb);
				
				if(!$properties[$eid])
				{
					$prop = new ue_feeds_property();
					$prop->externalID = $eid;
					
					$prop->name = trim($propXml->BuildingName);
					if(!strlen($prop->name)) $prop->name = trim($propXml->Address);
					
					$prop->phone = trim($propXml->Agent1PhonePrimary);
					
					$prop->email = trim($propXml->Email);
					$prop->addr1 = trim($propXml->Address);
					$prop->city = trim($propXml->AddrCity);
					$prop->state = trim($propXml->AddrState);
					$prop->zip = trim($propXml->AddrZip);
					
					$hood = ue_feeds_get_neighborhoods($prop->zip);
					if($hood) $prop->hood = $hood->tid;
					
					$prop->website = trim($propXml->AdvertiserListingURL->a['href']);
					$prop->logo = trim($propXml->AdvertiserLogo);
					$prop->desc = trim($propXml->CommentsLong);
					
					if($propXml->BuiltYear) $prop->yearBuilt = (int)$propXml->BuiltYear;
					
					if($propXml->CodePets == NYTimes::CodePets_Allowed)
						$prop->pets->dogs = $prop->pets->cats = true;
					elseif($propXml->CodePets == NYTimes::CodePets_Cats)
						$prop->pets->cats = true;
					
					foreach(explode(',', (string)$propXml->CodePopularFeature) as $pop)
					{
						switch($pop)
						{
							case NYTIMES::CodePopFeature_Doorman:
								$prop->services[] = 'Full-time Doorman';
								break;
							case NYTIMES::CodePopFeature_WashDryBldg:
								$prop->services[] = 'Laundry in Building';
								break;
							case NYTIMES::CodePopFeature_Green:
								$prop->type[] = 'Green Building';
								break;
							case NYTIMES::CodePopFeature_PreWar:
								$prop->type[] = 'Pre-war Apartment Building';
								break;
							case NYTIMES::CodePopFeature_FitnessCtr:
								$prop->amenities[] = 'Fitness Center';
								break;
						}
					}
					
					$prop->listings[] = $this->parseListing($propXml);
				
					if($prop->hood) $properties[$eid] = $prop;
				}
				else
				{
					$prop = $properties[$eid];
					$prop->listings[] = $this->parseListing($propXml);
				}
			}
			return array_values($properties);
		}
		
		protected function parseListing(&$xml)
		{
			$listing = new ue_feeds_listing();
			
			$listing->externalID = ltrim(strstr($xml->IdWeb, '-', false), '-');
			
			$listing->name = trim($xml->UnitAptId);
			$listing->bed = (float)$xml->Bedroom;
			$listing->bath = (float)$xml->BathsFull;
			
			$listing->price = (float)$xml->Price;
			
			$listing->photos = $this->parseFiles($xml);
			
			return $listing;
		}
		
		protected function parseFiles(&$xml)
		{
			for($i = 1; $i < 10; $i++)
			{
				$photo = 'Photo' . $i;
				if(strlen($xml->$photo))
					$photos[] = trim($xml->$photo);
			}
			
			return $photos;
		}
		
		public function getForm()
		{
			$form['eidType'] = array(
				'#type' => 'select',
				'#title' => t('External ID type'),
				'#default_value' => $this->feed->options->eidType,
				'#options' => array('ID', 'Address'),
				'#description' => t('Allows feed to reference nodes by ID or address.'),
				'#required' => TRUE,
			);
			return $form;
		}
		
		public function processForm($form_state)
		{
			$this->feed->options->eidType = (int)$form_state['values']['eidType'];
		}
	}
	
	ue_feeds_register_adapter('NYTimes - XML', '', 'ue_feeds_adapter_nytimes_xml');
