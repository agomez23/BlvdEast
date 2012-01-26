<?php
	
	class ue_feeds_adapter_forrent extends ue_feeds_adapter implements ue_feeds_adapter_output
	{
		const PROPTYPE_APARTMENT			= 1;
		const PROPTYPE_HOUSE				= 2;
		const PROPTYPE_ROOM					= 3;
		const PROPTYPE_DUPLEX				= 4;
		const PROPTYPE_TRIPLEX				= 5;
		const PROPTYPE_EFFICIENCY_STUDIO	= 6;
		const PROPTYPE_TOWNHOUSE			= 7;
		const PROPTYPE_CONDO				= 8;
		const PROPTYPE_VACA_TIMESHARE		= 9;
		const PROPTYPE_LOFT					= 10;
		const PROPTYPE_SINGLE_UNIT			= 11;
		const PROPTYPE_OTHER				= 12;
		
		public function feed(&$nodes)
		{
			$PMCID = 7370;
			
			$this->root = new SimpleXMLExtended("<PhysicalProperty PMCID=\"$PMCID\"></PhysicalProperty>");
			
			//we need to build a list of all neighborhoods in the following regions to filter forrent for.
			$whitelist = $this->getTids(taxonomy_get_tree(1, 90));								//manhattan
			$whitelist = array_merge($whitelist, $this->getTids(taxonomy_get_tree(1, 390)));	//brooklyn
			$whitelist = array_merge($whitelist, $this->getTids(taxonomy_get_tree(1, 459)));	//bronx
			$whitelist = array_merge($whitelist, $this->getTids(taxonomy_get_tree(1, 153)));	//queens
			$whitelist = array_merge($whitelist, $this->getTids(taxonomy_get_tree(1, 522)));	//staten island
			
			//block post luminaria, post toscana, queenswood, 10 hanover square, trump place
			$blacklist_props = array(660, 661, 659, 6523, 13800);
			
			//blacklist goldfarb
			$blacklist_companies = array(487);
			
			foreach($nodes['properties'] as $i => $prop)
			{
				if(
					in_array($prop->hoodId, $whitelist)
					&& !in_array($prop->id, $blacklist_props)
					&& !in_array($prop->building_mgr->id, $blacklist_companies)
					&& !in_array($prop->building_own->id, $blacklist_companies)
					&& !in_array($prop->leasing_mgr->id, $blacklist_companies)
				)
				{
					$this->feedProperty($prop, $this->root);
				}
			}
			
			return $this->root->asXml();
		}
		
		protected function getTids(&$tree)
		{
			foreach($tree as $term)
				$tid[] = $term->tid;
			return $tid;
		}
		
		protected function feedProperty(&$prop, &$parent)
		{
			$node = $parent->addChild('Property');
			$node->addAttribute('SiteID', $prop->id);
			
			$node->Identification->PropertyName = $prop->name;
			$node->Identification->URL = url('node/' . $prop->id, array('absolute' => true)); // outgoing feeds link back to our site. $prop->website
			
			$node->Identification->PropertyType = self::PROPTYPE_APARTMENT;
			
			$node->Identification->PropertyLAT = $prop->lat;
			$node->Identification->PropertyLONG = $prop->lng;
			
			$node->Identification->Email = $prop->email;
			$node->Identification->Address->Address1 = $prop->addr1;
			$node->Identification->Address->Address2 = $prop->addr2;
			$node->Identification->Address->City = $prop->city;
			$node->Identification->Address->State = $prop->state;
			$node->Identification->Address->Zip = $prop->zip;
			
			$node->Information->Verbiage = '';
			
			//$node->Information->Verbiage->addCData('LongDescription', $this->stripHtml($prop->desc));
			$node->Information->Verbiage->LongDescription = $this->stripHtml($prop->desc);
			
			$node->Information->Verbiage->Headline = $prop->tagline;
			
			if($prop->photos)
				$this->feedAddFiles($prop->photos, $node->Information, 0, 50);
			
			$node->Information->PropertyPhoneNumber = $prop->phone;
			$node->Information->YearBuilt = $prop->yearBuilt;
		
			//Pets checkboxes.
			if($prop->pets->comments)
				$node->Information->Verbiage->PetPolicy = $this->stripHtml($prop->pets->comments);
			elseif($prop->pets->dogs && $prop->pets->cats)
				$node->Information->Verbiage->PetPolicy = 'Pets OK';
			elseif($prop->pets->dogs)
				$node->Information->Verbiage->PetPolicy = 'Dogs OK';
			elseif($prop->pets->cats)
				$node->Information->Verbiage->PetPolicy = 'Cats OK';
			
			if($prop->incentives)
				$node->Information->Verbiage->PromoText = implode($prop->incentives, ', ');
			else
				$node->Information->Verbiage->PromoText = $this->stripHtml($prop->incentivesText);
		
			if($prop->amenities)
				foreach($prop->amenities as $i => $amen)
					$node->Information->addChild('CommunityAmenity', $amen);
			
			if($prop->services)
				foreach($prop->services as $i => $service)
					$node->Information->addChild('CommunityAmenity', $service);
			
			/*
			if($prop->unitCount) $node->Information->UnitCount = $prop->unitCount;
			
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
			*/
			
			$minPrice = NULL;
			
			if($prop->listings)
			{
				foreach($prop->listings as $i => $listing)
				{
					if(is_array($listing->price))
					{
						if($listing->price['min'] < $minPrice || $minPrice === NULL) $minPrice = $listing->price['min'];
					}
					elseif(floatval($listing->price) > 0)
					{
						if($listing->price < $minPrice || $minPrice === NULL) $minPrice = $listing->price;
					}
					
					$this->feedListing($prop, $listing, $node);
				}
			}
			
			$node->Information->RentStarts = $minPrice;
			
			return $node;
		}
		
		protected function feedListing(&$prop, &$listing, &$parent)
		{
			$node = $parent->addChild('Floorplan');
			$node->addAttribute('floorplanId', $listing->id);
			
			$node->FloorplanName = $listing->name;
			
			$node->FloorplanURL = url('node/' . $listing->id, array('absolute' => true)); // outgoing feeds link back to our site. $prop->website
			
			$node->FloorplanBeds = $listing->bed;
			$node->FloorplanBaths = $listing->bath;
			
			if(is_array($listing->price))
			{
				$node->FloorplanPriceLow = $listing->price['min'];
				$node->FloorplanPriceHigh = $listing->price['max'];
			}
			else
			{
				$node->FloorplanPriceLow = $listing->price;
				$node->FloorplanPriceHigh = $listing->price;
			}
			
			if(is_array($listing->sqft))
			{
				$sqft = $node->addChild('SquareFeet');
				$node->FloorplanMinSQFeet = $listing->sqft['min'];
				$node->FloorplanMaxSQFeet = $listing->sqft['max'];
			}
			elseif(floatval($listing->sqft) > 0)
			{
				$node->FloorplanMinSQFeet = $listing->sqft;
				$node->FloorplanMaxSQFeet = $listing->sqft;
			}
			
			//include floorplan if available (non-PNG)
			if($listing->floorplans && substr($listing->floorplans[0], strlen($listing->floorplans[0]) - 4) != '.png')
				$node->FloorplanImage = $listing->floorplans[0];
			
			if($prop->deposit)
				$node->FloorplanDeposit = implode($prop->deposit, ', ');
			
			//$node->addCData('FloorplanDescription', $this->stripHtml($listing->desc));
			$node->FloorplanDescription = $this->stripHtml($listing->desc);
			
			$node->BrokerFee = 0;
		
			if($listing->features)
				foreach($listing->features as $i => $feature)
					$node->addChild('Amenity', $feature);
		}
		
		//totalImages is running count, maxImages is max per listing.
		protected function feedAddFiles(&$files, &$parent, $totalImages, $maxImages)
		{
			$num = $totalImages;
			foreach($files as $i => $file)
			{
				//remove all PNG images from forrent feed.
				if(substr($file, strlen($file) - 4) != '.png')
				{
					if($num < $maxImages)
						$parent->addChild('PropertyPhoto', $file);
					else
						return $num;
					
					$num++;
				}
			}
			return $num;
		}
	}
	
	ue_feeds_register_adapter('ForRent', '', 'ue_feeds_adapter_forrent');
