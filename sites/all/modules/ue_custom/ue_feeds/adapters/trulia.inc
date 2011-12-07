<?php
	
	class ue_feeds_adapter_trulia extends ue_feeds_adapter implements ue_feeds_adapter_output
	{
		public function feed(&$nodes)
		{
			global $base_url;
			$root = new SimpleXMLExtended('<properties></properties>');
			
			$parsed = parse_url($base_url);
			$this->domain = $parsed['host'];
			
			foreach($nodes['properties'] as $i => $prop)
				foreach($prop->listings as $i => $listing)
					$this->feedListing($prop, $listing, $root);
			
			return $root->asXml();
		}
		
		private function feedListing($prop, $listing, $parent)
		{
			$node = $parent->addChild('property');
			$node->details = '';
			
			$node->details->addChild('listing-title', $prop->name . ' : ' . $listing->name);
			$node->details->addChild('provider-listingid', $listing->id);
			
			// site name/url
			if($prop->building_mgr->id == 1820)
			{
				$node->addChild('site')->addChild('site-name', $prop->building_mgr->name);
				$node->site->addChild('site-url', 'www.brodskyorg.com');
				$node->addChild('landing-page')->addChild('lp-url', $prop->website);
			}
			else
			{
				$node->addChild('site')->addChild('site-name', variable_get('site_name', 'Urban Edge'));
				$node->site->addChild('site-url', $this->domain);
				$node->addChild('landing-page')->addChild('lp-url', url('node/' . $listing->id, array('absolute' => true)));
			}
			
			//for rent
			$node->status = 'For Rent';
			
			// Use whichever description has more than 15 characters, and exclude the listing otherwise. If there is no description for an item Google will invalidate the feed.
			if($listing->desc && strlen($listing->desc) > 15)
				$node->details->addText('description', $this->stripHtml($listing->desc));
			elseif($prop->desc && strlen($prop->desc) > 15)
				$node->details->addText('description', $this->stripHtml($prop->desc));
			else
			{
				$this->logError('Listing has an invalid (or short) description and was not included in the feed.', $listing->id);
				unset($parent->property[count($parent->property)-1]);
				return;
			}
			
			$node->details->addChild('num-bedrooms', $listing->bed);
			
			if($listing->fullBath)
				$node->details->addChild('num-full-bathrooms', $listing->fullBath);
			if($listing->halfBath)
				$node->details->addChild('num-half-bathrooms', $listing->halfBath);
			
			if(is_array($listing->sqft))
				$node->details->addChild('square-feet', $listing->sqft['max']);
			elseif(floatval($listing->sqft) > 0)
				$node->details->addChild('square-feet', $listing->sqft);
			
			//Trulia has a maximum of 1000 images.
			$totalImages = 0;
			$maxImages = 1000;
			
			if($listing->floorplans)
				$totalImages += $this->feedAddFiles($listing->floorplans, $node, $totalImages, $maxImages);
		
			if($listing->photos)
				$totalImages += $this->feedAddFiles($listing->photos, $node, $totalImages, $maxImages);
		
			if($prop->photos)
				$totalImages += $this->feedAddFiles($prop->photos, $node, $totalImages, $maxImages);
			
			//add building info.
			$this->addBuilding($node, $prop);
			
			$node->location = '';
			$node->location->addChild('unit-number', $listing->name);
			$node->location->addChild('street-address', $prop->addr1);
			$node->location->addChild('city-name', $prop->city);
			$node->location->addChild('state-code', $prop->state);
			$node->location->addChild('zipcode', $prop->zip);
			if($prop->crossStreet) $node->location->addChild('street-intersection', $prop->crossStreet);
			if($prop->lat) $node->location->latitude = $prop->lat;
			if($prop->lng) $node->location->longitude = $prop->lng;
			$node->location->addChild('neighborhood-name', $prop->hoodName);
			$node->location->addText('neighborhood-description', $this->stripHtml($prop->hoodDesc));
			
			$node->office = '';
			if($prop->phone) $node->office->addChild('office-phone', $prop->phone);
				
			if($prop->website) $node->office->addChild('office-website', $prop->website);
			
			$node->agent = '';
			if($prop->phone) $node->agent->addChild('agent-phone', $prop->phone);
			
			if($prop->email)
				$email = $prop->email;
			else if($prop->building_mgr->syndication_email)
				$email = $prop->building_mgr->syndication_email;
			else if($prop->building_mgr->email)
				$email = $prop->building_mgr->email;
			
			if($email)
			{
				$node->office->addChild('office-email', $email);
				$node->agent->addChild('agent-email', $email);
			}
			else
			{
				$this->logError('Property has no email and was not included in the feed.', $prop->id);
				unset($parent->property[count($parent->property)-1]);
				return;
			}
			
			if($listing->price > 0)
				$node->details->price = $listing->price;
			else
			{
				$this->logError('Listing has no price and was not included in the feed.', $listing->id);
				unset($parent->property[count($parent->property)-1]);
				return;
			}
			
			if($prop->yearBuilt) $node->details->addChild('year-built', $prop->yearBuilt);
			$node->details->addChild('property-type', 'apartment');
			
			$detailed = $node->addChild('detailed-characteristics');
			
			if($prop->unitCount) $detailed->addChild('building-unit-count', $prop->unitCount);
			
			if(in_arrayi('pool', $prop->amenities)) $detailed->addChild('has-pool', 'yes');
			if(in_arrayi('doorman', $prop->services)) $detailed->addChild('has-doorman', 'yes');
			if(in_arrayi('elevator', $prop->access)) $detailed->addChild('has-elevator', 'yes');
		}
		
		private function addBuilding($parent, $prop)
		{
			$node = $parent->addChild('community-building');
			$node->addChild('community-id', $prop->id);
			$node->addChild('community-name', $prop->name);
			if($prop->email) $node->addChild('community-email', $prop->email);
			if($prop->fax) $node->addChild('community-fax', $prop->fax);
			if($prop->desc) $node->addText('community-description', $this->stripHtml($prop->desc));
			
			$addr = $node->addChild('community-address');
			$addr->addChild('community-street-address', $prop->addr1);
			$addr->addChild('community-city-name', $prop->city);
			$addr->addChild('community-zipcode', $prop->zip);
			$addr->addChild('community-state-code', $prop->state);
			
			if($prop->amenities)
			{
				$amenNode = $node->addChild('building-amenities');
				foreach($prop->amenities as $i => $amen)
					$amenNode->addChild('building-amenity', $amen);
			}
			
			if($prop->services)
			{
				if(!$amenNode) $amenNode = $node->addChild('building-amenities');
				foreach($prop->services as $i => $service)
					$amenNode->addChild('building-amenity', $service);
			}
		}
		
		//totalImages is running count, maxImages is max per listing.
		private function feedAddFiles($files, $parent, $totalImages, $maxImages)
		{
			if(count($files) && !$parent->pictures) $parent->addChild('pictures');
			
			$num = $totalImages;
			foreach($files as $i => $file)
			{
				if($num < $maxImages)
					$parent->pictures->addChild('picture')->addChild('picture-url', $file);
				$num++;
			}
			return $num;
		}
	}
	
	ue_feeds_register_adapter('Trulia', '', 'ue_feeds_adapter_trulia');