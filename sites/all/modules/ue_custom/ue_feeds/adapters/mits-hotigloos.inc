<?php
	require_once 'mits-omg.inc';
	
	class ue_feeds_adapter_mits_hotigloos extends ue_feeds_adapter_mits_omg
	{
		
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
				foreach($prop->amenities as $i => $amen)
					$node->Amenities = implode(', ', $prop->amenities);
			
			if($prop->services)
				foreach($prop->services as $i => $service)
					$node->Amenities = implode(', ', $prop->services);
			
			if($prop->photos)
				$node->Information->Photos = implode('|', $prop->photos);
				
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
		
		protected function feedListings(&$listings, &$parent)
		{
			foreach($listings as $i => $listing)
			{
				$node = $parent->addChild('Floorplan');
				$node->Name = $listing->name;
				$node->addAttribute('id', $listing->id);
				
				$node->Bedrooms = $listing->bed;
				$node->Bathrooms = $listing->bath;
				
				if($listing->price)
				{
					if(is_array($listing->price))
						$node->Price = $listing->price['min'];
					else
						$node->Price = $listing->price;
				}
				
				if(is_array($listing->sqft))
				{
					$sqft = $node->addChild('SquareFeet');
					$sqft->addAttribute('Min', $listing->sqft['min']);
					$sqft->addAttribute('Max', $listing->sqft['max']);
				}
				elseif(floatval($listing->sqft) > 0)
					$node->SquareFeet = $listing->sqft;
			
				if($listing->photos)
					$node->Photos = implode('|', $listing->photos);
			
				if($listing->floorplans)
					$node->Floorplans = implode('|', $listing->floorplans);
			}
		}
	}
	
	ue_feeds_register_adapter('MITS - Hot Igloos', 'MITS adapter to specify fields for hotigloos', 'ue_feeds_adapter_mits_hotigloos');