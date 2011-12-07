<?php
	
	class ue_feeds_adapter_mits extends ue_feeds_adapter implements ue_feeds_adapter_input, ue_feeds_adapter_output
	{
		//fetch a company node.
		protected $parsed = NULL;
		
		protected $root;
		
		public function parse($reset = false)
		{
			if(!$this->parsed || $reset)
			{
				$xml = simplexml_load_string($this->feed->read(), null, LIBXML_NOCDATA);
				$this->parsed['companies'] = $this->parseCompanies($xml->Management);
				$this->parsed['properties'] = $this->parseProperties($xml->Property);
			}
			
			return $this->parsed;
		}
		
		public function feed(&$nodes)
		{
			$this->root = new SimpleXMLElement('<PhysicalProperty></PhysicalProperty>');
			
			$this->feedCompanies($nodes['properties'], $this->root);
			
			foreach($nodes['properties'] as $i => $prop) $this->feedProperty($prop, $this->root);
			
			return $this->root->asXml();
		}
		
		protected function feedCompanies(&$properties, &$parent)
		{
			foreach($properties as $i => $prop)
			{
				if($prop->leasing_mgr) $companies[$prop->leasing_mgr->id] = $prop->leasing_mgr;
				
				if($prop->building_mgr) $companies[$prop->building_mgr->id] = $prop->building_mgr;
				
				if($prop->building_own) $companies[$prop->building_own->id] = $prop->building_own;
			}
			
			foreach($companies as $i => $comp) $this->feedCompany($comp, $parent);
		}
		
		protected function feedAddFiles(&$files, $type, &$parent)
		{
			foreach($files as $i => $file)
			{
				$info = pathinfo($file);
				$node = $parent->addChild('File');
				$node->Type = $type;
				$node->Name = $info['basename'];
				$node->Format = $info['extension'];
				$node->Src = $file;
				$node->addAttribute('id', $info['basename']);
				$node->addAttribute('active', 'true');
			}
		}
		
		protected function feedProperty(&$prop, &$parent)
		{
			//only feed property if it has listings.
			if(!$prop->listings || count($prop->listings) == 0)
			{
				$this->logError("No active listings, was not included in the feed.", $prop->id);
				return;
			}
			$node = $parent->addChild('Property');
			if($prop->id) $node->Identification->PrimaryId = $prop->id;
			if($prop->name) $node->Identification->MarketingName = $prop->name;
			if($prop->phone) $node->Identification->Phone->Number = $prop->phone;
			if($prop->lat) $node->Identification->Latitude = $prop->lat;
			if($prop->lng) $node->Identification->Longitude = $prop->lng;
			if($prop->email) $node->Identification->Email = $prop->email;
			if($prop->addr1) $node->Identification->Address->Address1 = $prop->addr1;
			if($prop->addr2) $node->Identification->Address->Address2 = $prop->addr2;
			if($prop->city) $node->Identification->Address->City = $prop->city;
			if($prop->state) $node->Identification->Address->State = $prop->state;
			if($prop->zip) $node->Identification->Address->Zip = $prop->zip;
			
			$node->Identification->WebSite = url('node/' . $prop->id, array('absolute' => true)); // outgoing feeds link back to our site. $prop->website
			if($prop->desc) $node->Information->Info->LongDescription = $this->stripHtml($prop->desc);
			
			if($prop->unitCount) $node->Information->UnitCount = $prop->unitCount;
			if($prop->yearBuilt) $node->Information->YearBuilt = $prop->yearBuilt;
		
			//Pets checkboxes.
			if($prop->pets->dogs && $prop->pets->cats)
			{
				if(!$node->Information)
					$node->addChild('Information');
				
				$dogs = $node->Information->addChild('Policy')->addChild('Pet');
				$dogs->addAttribute('type', 'dogs');
				$dogs->addAttribute('allowed', 'true');
				
				$cats = $node->Information->Policy->addChild('Pet');
				$cats->addAttribute('type', 'cats');
				$cats->addAttribute('allowed', 'true');
			}
			elseif($prop->pets->dogs)
				$node->Information->Policy = '<Pet type="dogs" allowed="true"></Pet>';
			elseif($prop->pets->cats)
				$node->Information->Policy = '<Pet type="cats" allowed="true"></Pet>';
			
			if($prop->amenities)
			{
				foreach($prop->amenities as $i => $amen)
				{
					if(!$node->Information) $node->addChild('Information');
					$amenNode = $node->Information->addChild('Amenities');
					$amenNode->addAttribute('type', 'community');
					$amenNode->Feature = $amen;
				}
			}
			
			if($prop->services)
			{
				foreach($prop->services as $i => $service)
				{
					if(!$node->Information) $node->addChild('Information');
					$amenNode = $node->Information->addChild('Amenities');
					$amenNode->addAttribute('type', 'community');
					$amenNode->Feature = $service;
				}
			}
			
			if($prop->photos)
				$this->feedAddFiles($prop->photos, 'photo', $node->Information);
				
			if($prop->listings)
				$this->feedListings($prop->listings, $node);
			
			$companyId = 0;
			
			if($prop->leasing_mgr)
			{
				$this->feedCompanies[$prop->leasing_mgr->id] = $prop->leasing_mgr;
				$companyId = $prop->leasing_mgr->id;
			}
			
			if($prop->building_mgr)
			{
				$this->feedCompanies[$prop->building_mgr->id] = $prop->building_mgr;
				$companyId = $prop->building_mgr->id;
			}
			
			if($prop->building_own)
			{
				$this->feedCompanies[$prop->building_own->id] = $prop->building_own;
				$companyId = $prop->building_own->id;
			}
			
			if($companyId > 0)
				$node->addAttribute('managementID', $companyId);
			
			/*
			$prop->externalID = (int)$xml->Identification->SecondaryID;
			//$prop['managementID'] = (string)$attr['managementID'];
			$prop->logo = (string)$xml->Identification->Logo;
			
			$prop->photos[] = (string)$xml->Identification->MainPic;
			
			*/
			return $node;
		}
		
		protected function feedCompany(&$company, &$parent)
		{
			$node = $parent->addChild('Management');
			$node->addAttribute('Id', $company->id);
			if($company->name) $node->Name = $company->name;
			if($company->phone) $node->Phone->Number = $company->phone;
			if($company->email) $node->Email = $company->email;
			if($company->addr1) $node->Address->Address1 = $company->addr1;
			if($company->addr2) $node->Address->Address2 = $company->addr2;
			if($company->city) $node->Address->City = $company->city;
			if($company->state) $node->Address->State = $company->state;
			if($company->zip) $node->Address->Zip = $company->zip;
			return $node;
		}
		
		protected function feedListings(&$listings, &$parent)
		{
			foreach($listings as $i => $listing)
			{
				$node = $parent->addChild('Floorplan');
				$node->Name = $listing->name;
				$node->addAttribute('id', $listing->id);
				
				$bed = $node->addChild('Room');
				$bed->addAttribute('type', 'bedroom');
				$bed->Count = $listing->bed;
				$bed->Comment = 'Bedrooms';
			
				$bath = $node->addChild('Room');
				$bath->addAttribute('type', 'bathroom');
				$bath->Count = $listing->bath;
				$bath->Comment = 'Bathrooms';
				
				if($listing->price)
				{
					$price = $node->addChild('MarketRent', '');
					if(is_array($listing->price))
					{
						$price->addAttribute('min', $listing->price['min']);
						$price->addAttribute('max', $listing->price['max']);
					}
					else
					{
						$price->addAttribute('min', $listing->price);
						$price->addAttribute('max', $listing->price);
					}
				}
				
				if(is_array($listing->sqft))
				{
					$sqft = $node->addChild('SquareFeet');
					$sqft->addAttribute('Min', $listing->sqft['min']);
					$sqft->addAttribute('Max', $listing->sqft['max']);
				}
				elseif(floatval($listing->sqft) > 0)
					$node->SquareFeet = $listing->sqft;
			
				//if($listing->dateAvail)
			
				if($listing->photos)
					$this->feedAddFiles($listing->photos, 'photo', $node);
			
				if($listing->floorplans)
					$this->feedAddFiles($listing->floorplans, 'floorplan', $node);
			}
		}
		
		protected function parseCompanies($xml)
		{
			foreach($xml as $company) $companies[] = $this->parseCompany($company);
			return $companies;
		}
		
		protected function parseCompany($xml)
		{
			$attr = $xml->attributes();
			$company = new ue_feeds_company();
			$company->externalID = (string)$attr['id'];
			$company->name = (string)$xml->Name;
			$company->phone = (string)$xml->Phone->Number;
			$company->fax = (string)$xml->Fax->Number;
			return $company;
		}
		
		protected function parseProperties($xml)
		{
			foreach($xml as $property)
			{
				if(strtolower($property->Identification[type]) != 'sales')
				{
					$prop = $this->parseProperty($property);
					if($prop->hood) $properties[] = $prop;
				}
			}
			return $properties;
		}
		
		protected function parseProperty($xml)
		{
			$attr = $xml->attributes();
			$prop = new ue_feeds_property();
			$prop->externalID = (string)$xml->Identification->PrimaryID;
			
			$prop->name = (string)$xml->Identification->MarketingName;
			if(!$this->feed->options->preservephone)
				$prop->phone = (string)$xml->Identification->Phone->Number;
			//do not import lat/lng for now, it seems like our geolocate is a little more accurate than any feeds we are currently pulling.
			//if($xml->Identification->Latitude) $prop->lat = (float)$xml->Identification->Latitude;
			//if($xml->Identification->Longitude) $prop->lng = (float)$xml->Identification->Longitude;
			$prop->email = (string)$xml->Identification->Email;
			$prop->addr1 = (string)$xml->Identification->Address->Address1;
			$prop->addr2 = (string)$xml->Identification->Address->Address2;
			$prop->city = (string)$xml->Identification->Address->City;
			$prop->state = (string)$xml->Identification->Address->State;
			$prop->zip = (string)$xml->Identification->Address->Zip;
			
			$hood = ue_feeds_get_neighborhoods($prop->zip);
			if($hood) $prop->hood = $hood->tid;
			
			$prop->website = (string)$xml->Identification->WebSite;
			$prop->logo = (string)$xml->Identification->Logo;
			$prop->desc = (string)$xml->Information->Info->LongDescription;
			
			if($xml->Identification->MainPic)
				$prop->photos[] = (string)$xml->Identification->MainPic;
			
			if($prop->photos && $xml->Information->File)
				$prop->photos = array_merge($prop->photos, $this->parseFiles($xml->Information->File));
			elseif($xml->Information->File)
				$prop->photos = $this->parseFiles($xml->Information->File);
			
			if($xml->Information->UnitCount) $prop->unitCount = (int)$xml->Information->UnitCount;
			if($xml->Information->YearBuilt) $prop->yearBuilt = (int)$xml->Information->YearBuilt;
			
			if($xml->Information->Policy->Pet)
			{
				foreach($xml->Information->Policy->Pet as $pet)
				{
					$petAttr = $pet->attributes();
					if(stripos($petAttr['type'], 'dog') !== false && (bool)$petAttr['allowed']) $prop->pets->dogs = true;
					if(stripos($petAttr['type'], 'cat') !== false && (bool)$petAttr['allowed']) $prop->pets->cats = true;
					
					$comment = trim((string)$pet->Comment);
					if(strlen($comment))
						$prop->pets->comments[] = $comment;
				}
			}
			
			if($xml->Information->Amenities)
			{
				foreach($xml->Information->Amenities as $amen)
				{
					$amenAttr = $amen->attributes();
					
					if(stripos($amen->Feature, 'fitness center') !== false) $prop->amenities[] = 'Fitness Center';
					if(stripos($amen->Feature, 'garage') !== false) $prop->amenities[] = 'On-site Garage';
					
					if(stripos($amen->Feature, 'valet') !== false)
					{
						$prop->amenities[] = 'Valet Parking';
						$prop->services[] = 'Valet';
					}
					
					if(stripos($amen->Feature, 'wireless') !== false || stripos($amen->Feature, 'wifi') !== false)
					{
						$prop->amenities[] = 'WiFi Access';
						$prop->moreAmen[] = 'WiFi Access';
					}
					
					if(stripos($amen->Feature, 'concierge') !== false) $prop->services[] = 'Concierge';
					if(stripos($amen->Feature, 'elevator') !== false) $prop->access[] = 'Elevator';
					
					if(stripos($amen->Feature, 'laundry') !== false) $prop->services[] = 'Laundry in Building';
					
					if(stripos($amen->Feature, 'rooftop') !== false && stripos($amen->Feature, 'deck') !== false) $prop->amenities[] = 'Rooftop Deck';
					
					if((stripos($amen->Feature, 'rooftop') !== false || stripos($amen->Feature, 'outdoor') !== false) && stripos($amen->Feature, 'pool') !== false) $prop->amenities[] = 'Outdoor Pool';
					
					if(stripos($amen->Feature, '') !== false) $prop->moreAmen[] = 'Screening Room';
					
					if(stripos($amen->Feature, 'billiards') !== false) $prop->moreAmen[] = 'Billiards';
					if(stripos($amen->Feature, 'screening room') !== false) $prop->moreAmen[] = 'Screening Room';
					if(stripos($amen->Feature, 'clubhouse') !== false || stripos($amen->Feature, 'clubroom') !== false) $prop->moreAmen[] = 'Clubhouse';
					
					//lounge or clubhouse w/ fireplace
					if((stripos($amen->Feature, 'clubhouse') !== false || stripos($amen->Feature, 'lounge') !== false) && stripos($amen->Feature, 'fire') !== false) $prop->moreAmen[] = 'Tenant Lounge with Fireplace';
					
					//lounge or clubhouse w/ tv
					if((stripos($amen->Feature, 'clubhouse') !== false || stripos($amen->Feature, 'lounge') !== false) && stripos($amen->Feature, 'tv') !== false) $prop->moreAmen[] = 'TV Lounge';
					
					//library
					if(stripos($amen->Feature, 'library') !== false) $prop->moreAmen[] = 'Tenants Library';
					
					//library
					if(stripos($amen->Feature, 'bicycle storage') !== false) $prop->laundryStorage[] = 'Bicycle Storage';
					
					//library
					if(stripos($amen->Feature, 'landscaped roof') !== false) $prop->outdoorSpace[] = 'Landscaped Roof Deck';
				}
			}
			
			$idAttr = $xml->Identification->attributes();
			
			if($xml->Floorplan)
			{
				foreach($xml->Floorplan as $fpXml)
				{
					$listing = $this->parseFloorplan($fpXml);
					if($listing->numAvailable > 0 && ((is_array($listing->price) && $listing->price['min'] > 0) || $listing->price > 0)) $listings[] = $listing;
				}
			}
			
			$prop->listings = $listings;
			
			/* 
				Property->Type->attr['type']------------
				<xs:enumeration value="apartment"/> 
				<xs:enumeration value="condo"/> 
				<xs:enumeration value="duplex"/> 
				<xs:enumeration value="triplex"/> 
				<xs:enumeration value="4plex"/> 
				<xs:enumeration value="senior"/> 
				<xs:enumeration value="assisted living"/> 
				<xs:enumeration value="subsidized"/> 
				<xs:enumeration value="high rise"/> 
				<xs:enumeration value="garden style"/> 
				<xs:enumeration value="mid rise"/> 
				<xs:enumeration value="mixed use"/> 
				<xs:enumeration value="townhouse"/> 
				<xs:enumeration value="vacation"/> 
				<xs:enumeration value="campus"/> 
				<xs:enumeration value="military"/> 
				<xs:enumeration value="house for rent"/> 
				<xs:enumeration value="corporate"/> 
				<xs:enumeration value="unspecified"/>
			*/
			
			//$prop->xml = $xml;
			return $prop;
		}
		
		protected function parseFloorplan($xml)
		{
			$listing = new ue_feeds_listing();
			
			$listing->externalID = (string)$xml['id'];
			
			$listing->name = (string)$xml->Name;
			
			foreach($xml->Room as $room)
			{
				$roomAttr = $room->attributes();
				switch(isset($roomAttr['type']) ? strtolower($roomAttr['type']) : strtolower($roomAttr['RoomType']))
				{
					case 'bedroom':
						$listing->bed += (float)$room->Count;
					break;
					case 'bathroom':
						$listing->bath += (float)$room->Count;
					break;
				}
			}
			
			$listing->photos = $this->parseFiles($xml->File);
			$listing->floorplans = $this->parseFiles($xml->File, 'floorplan');
			
			$minsqft = (float)trim($xml->SquareFeet['min']);
			$maxsqft = (float)trim($xml->SquareFeet['max']);
			
			if($maxsqft != 0 && $minsqft == $maxsqft)
				$listing->sqft = $minsqft;
			else if($maxsqft != 0)
				$listing->sqft = array('min' => $minsqft, 'max' => $maxsqft);
			
			//market rent
			if($xml->MarketRent)
			{
				//added to support non-standard feeds with variation in capitalization.
				$mktrent = (array)$xml->MarketRent->attributes();
				$mktrent = array_change_key_case($mktrent['@attributes']);
				
				$minrent = (float)preg_replace('/[^\d\.]/', '', trim($mktrent['min']));
				$maxrent = (float)preg_replace('/[^\d\.]/', '', trim($mktrent['max']));
				
				if($maxrent != 0 && $minrent == $maxrent)
					$listing->price = $minrent;
				else if($maxrent != 0)
					$listing->price = array('min' => $minrent, 'max' => $maxrent);
			}
			
			//effective rent - overrides market rent.
			if($xml->EffectiveRent)
			{
				//added to support non-standard feeds with variation in capitalization.
				$effrent = (array)$xml->EffectiveRent->attributes();
				$effrent = array_change_key_case($effrent['@attributes']);
				
				$minrent = (float)preg_replace('/[^\d\.]/', '', trim($effrent['min']));
				$maxrent = (float)preg_replace('/[^\d\.]/', '', trim($effrent['max']));
				
				if($maxrent != 0 && $minrent == $maxrent)
					$listing->price = $minrent;
				else if($maxrent != 0)
					$listing->price = array('min' => $minrent, 'max' => $maxrent);
			}
			
			//Probably only avalon feeds have this property, but include it for now.
			if($xml->FloorplanSpecials) $listing->desc = (string)$xml->FloorplanSpecials;
			
			//Defaults date available for 60 day availabilities.
			if((int)$xml->UnitsAvailable60Days > 0)
			{
				$listing->numAvailable += (int)$xml->UnitsAvailable60Days;
				$listing->dateAvail = strtotime('+60 days');
			}
			
			//Defaults date available for 30 day availabilities.
			if((int)$xml->UnitsAvailable30Days > 0)
			{
				$listing->numAvailable += (int)$xml->UnitsAvailable30Days;
				$listing->dateAvail = strtotime('+30 days');
			}
			
			//Defaults date available for immediate availabilities
			if((int)$xml->NumberofUnitsAvailable > 0)
			{
				$listing->numAvailable = (int)$xml->NumberofUnitsAvailable;
				unset($listing->dateAvail);
			}
			
			if(strtotime((string)$xml->Availability) > time())
				$listing->dateAvail = strtotime((string)$xml->Availability);
			
			return $listing;
		}
		
		//pass file array, will return array of all photo urls.
		//if $active is true, will only use active photos.
		protected function parseFiles($xml, $type = 'photo', $active = true)
		{
			$photos = array();
			$urlregex = "((https?|ftp)\:\/\/([a-z0-9+!*(),;?&=\$_.-]+(\:[a-z0-9+!*(),;?&=\$_.-]+)?@)?[a-z0-9+\$_-]+(\.[a-z0-9+\$_-]+)*(\:[0-9]{2,5})?(\/([a-z0-9+\$_-]\.?)+)*\/?(\?[a-z+&\$_.-][a-z0-9;:@/&%=+\$_.-]*)?(#[a-z_.-][a-z0-9+\$_.-]*)?)";
			
			foreach($xml as $file)
			{
				$attr = $file->attributes();
				if(strtolower($file->Type) == strtolower($type) && (!$active || (bool)$attr['active']))
				{
					//some feeds do weird stuff and use <img> elements inside the src tag
					if($file->Src->img)
					{
						$attr = $file->Src->img->attributes();
						$photos[] = trim((string)$attr['src']);
					}
					else
						$photos[] = trim((string)$file->Src);
				}
			}
			
			return $photos;
		}
	}
	
	ue_feeds_register_adapter('MITS', 'Mits adapter.', 'ue_feeds_adapter_mits');