<?php
	
	class ue_feeds_adapter_hotpads extends ue_feeds_adapter implements ue_feeds_adapter_output
	{
		protected $root;
		
		public function feed(&$nodes)
		{
			$this->root = new SimpleXMLElement('<hotPadsItems version="2.1"></hotPadsItems>');
			
			foreach($nodes['properties'] as $i => $prop) $this->feedProperty($prop, $this->root);
			
			return $this->root->asXml();
		}
		
		protected function feedProperty($prop, $parent)
		{
			//only feed property if it has listings.
			if(!$prop->listings || count($prop->listings) == 0)
			{
				print "Warning: " . $prop->name . " (node " . $prop->id . ") has no active listings and was not included in the feed.\n";
				return;
			}
			$node = $parent->addChild('Listing');
			if($prop->id) $node['id'] = $prop->id;
			if($prop->name) $node->name = $prop->name;
			if($prop->phone) $node->contactPhone = $prop->phone;
			if($prop->lat) $node->latitude = $prop->lat;
			if($prop->lng) $node->longitude = $prop->lng;
			if($prop->email) $node->contactEmail = $prop->email;
			if($prop->addr1) $node->street = $prop->addr1;
			if($prop->city) $node->city = $prop->city;
			if($prop->state) $node->state = $prop->state;
			if($prop->zip) $node->zip = $prop->zip;
			
			$node->website = url('node/' . $prop->id, array('absolute' => true));
			if($prop->desc) $node->description = $this->stripHtml($prop->desc);
			
			if($prop->unitCount) $node->numUnits = $prop->unitCount;
			
			if($prop->size == 'highrise')
				$node['propertyType'] = 'LARGE';
			elseif($prop->size == 'townhouse')
				$node['propertyType'] = 'TOWNHOUSE';
			else
				$node['propertyType'] = 'MEDIUM';
		
			//Pets checkboxes.
			if($prop->pets->dogs)
			{
				$tag = $node->addChild('ListingTag');
				$tag->addAttribute('type', 'DOGS_ALLOWED');
				$tag->tag = 'true';
			}
			
			if($prop->pets->cats)
			{
				$tag = $node->addChild('ListingTag');
				$tag->addAttribute('type', 'CATS_ALLOWED');
				$tag->tag = 'true';
			}
			
			if($prop->amenities)
			{
				foreach($prop->amenities as $i => $amen)
				{
					$amenNode = $node->addChild('ListingTag');
					$amenNode->addAttribute('type', 'PROPERTY_AMENITY');
					$amenNode->tag = $amen;
				}
			}
			
			if($prop->services)
			{
				foreach($prop->services as $i => $service)
				{
					$amenNode = $node->addChild('ListingTag');
					$amenNode->addAttribute('type', 'PROPERTY_AMENITY');
					$amenNode->tag = $service;
				}
			}
			
			$node['type'] = 'RENTAL';
			
			if($prop->photos)
				$this->feedAddFiles($prop->photos, 'ListingPhoto', $node);
				
			if($prop->listings)
				$this->feedListings($prop->listings, $node);
			
			return $node;
		}
		
		protected function feedListings($listings, $parent)
		{
			foreach($listings as $i => $listing)
			{
				$node = $parent->addChild('Model');
				$node->name = $listing->name;
				$node['id'] = $listing->id;
				$node['searchable'] = 'true';
				
				$node->numBedrooms = $listing->bed;
				
				if($listing->fullBath)
					$node->numFullBaths = $listing->fullBath;
				if($listing->halfBath)
					$node->numHalfBaths = $listing->halfBath;
				
				if($listing->price)
				{
					if(is_array($listing->price))
					{
						$node->lowPrice = $listing->price['min'];
						$node->highPrice = $listing->price['max'];
						$node['pricingType'] = 'RANGE';
					}
					else
					{
						$node->lowPrice = $node->highPrice = $listing->price;
						$node['pricingType'] = 'FLAT';
					}
					$node->pricingFrequency = 'MONTH';
				}
				
				if(is_array($listing->sqft))
					$node->squareFeet = $listing->sqft['max'];
				elseif(floatval($listing->sqft) > 0)
					$node->squareFeet = $listing->sqft;
				
				if($listing->desc)
					$node->description = $this->stripHtml($listing->desc);
			
				if($listing->photos)
					$this->feedAddFiles($listing->photos, 'ModelPhoto', $node);
			
				if($listing->floorplans)
					$this->feedAddFiles($listing->floorplans, 'ModelLayout', $node);
			}
		}
		
		protected function feedAddFiles($files, $type, $parent)
		{
			foreach($files as $i => $file)
			{
				$info = pathinfo($file);
				$node = $parent->addChild($type);
				$node->addAttribute('source', $file);
			}
		}
	}
	
	ue_feeds_register_adapter('Hotpads', '', 'ue_feeds_adapter_hotpads');