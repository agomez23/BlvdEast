<?php
	
	class ue_feeds_adapter_rentbits extends ue_feeds_adapter implements ue_feeds_adapter_output
	{
		
		public function feed(&$nodes)
		{
			global $base_url;
			
			$root = new SimpleXMLElement('<c:ads xmlns:c="http://rentbits.com/collector" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:schemaLocation="http://rentbits.com/collector collector.xsd "></c:ads>');
			$this->addCChild($root, 'cv', '1.2');
			$this->addCChild($root, 'cv', $base_url);
			$this->addCChild($root, 'method', 'O');
			$this->addrWarnings = array();
			$this->descWarnings = array();
			
			foreach($nodes['properties'] as $i => $prop)
				if(is_array($prop->listings))
					foreach($prop->listings as $i => $listing)
						$this->feedListing($prop, $listing, $root);
			
			return $root->asXml();
		}
		
		private function feedListing($prop, $listing, $parent)
		{
			$desc = '';
			
			if(isset($listing->desc))
				$desc = $this->stripHtml($listing->desc) . "\n\n";
			
			if(isset($prop->desc))
				$desc .= $this->stripHtml($prop->desc);
			
			// If there is no description for an item Google will invalidate the feed.
			if(strlen($desc) < 15)
			{
				$this->logError("Listing has an invalid (or short) description and was not included in the feed.", $listing->id);
				return;
			}
			
			$node = $this->addCChild($parent, 'ad');
			$this->addCChild($node, 'market', 'A');
			$this->addCChild($node, 'commName', $prop->name);
			
			$addrNode = $this->addCChild($node, 'address');
			$this->addCChild($addrNode, 'line1', $prop->addr1);
			if($prop->addr2) $this->addCChild($addrNode, 'line2', $prop->addr2);
			$this->addCChild($addrNode, 'city', $prop->city);
			$this->addCChild($addrNode, 'province', $prop->state);
			$this->addCChild($addrNode, 'postal', $prop->zip);
			$this->addCChild($addrNode, 'country', 'US');
			
			if($prop->lat) $this->addCChild($node, 'latitude', $prop->lat);
			if($prop->lng) $this->addCChild($node, 'longitude', $prop->lng);
			
			$this->addCChild($node, 'url', url('node/' . $listing->id, array('absolute' => true)));
			
			$this->addCChild($node, 'type', 'A');
			
			$this->addCChild($node, 'headline', 'A');
			
			$this->addCChild($node, 'description', $desc);
			
			$this->addCChild($node, 'floorPlan', $listing->name);
			
			$this->addCChild($node, 'price', is_array($listing->price) ? $listing->price['min'] : $listing->price);
			
			if(is_array($listing->price)) $this->addCChild($node, 'priceMax', $listing->price['max']);
			
			$this->addCChild($node, 'beds', $listing->bed);
			$this->addCChild($node, 'baths', $listing->bath);
			
			if(is_array($listing->sqft))
			{
				$this->addCChild($node, 'area', $listing->sqft['min']);
				$this->addCChild($node, 'areaMax', $listing->sqft['max']);
			}
			elseif(floatval($listing->sqft) > 0)
				$this->addCChild($node, 'area', $listing->sqft);
			
			/*
		
			if($prop->amenities)
				foreach($prop->amenities as $i => $amen)
					$this->addCChild($node, 'feature', $amen);
			
			if($prop->services)
				foreach($prop->services as $i => $service)
					$this->addCChild($node, 'feature', $service);
			
			$node->id = $listing->id;
			
			//Google base has a maximum of 10 images.
			$totalImages = 0;
			$maxImages = 10;
			
			if($listing->floorplans)
				$totalImages += $this->feedAddFiles($listing->floorplans, 'floorplan_link', $node, $totalImages, $maxImages);
		
			if($listing->photos)
				$totalImages += $this->feedAddFiles($listing->photos, 'image_link', $node, $totalImages, $maxImages);
		
			if($prop->photos)
				$totalImages += $this->feedAddFiles($prop->photos, 'image_link', $node, $totalImages, $maxImages);
			
			$this->addCChild($node, 'listing_type', 'for rent');
			
			if((stripos($prop->addr1, '-') !== false || stripos($prop->addr1, '&') !== false) && !$this->addrWarnings[$prop->id])
			{
				print "Warning: " . (string)$prop->name . " (node " . $prop->id . ") may be excluded because of an invalid address.\n";
				$this->addrWarnings[$prop->id] = true;
			}
			
			$this->addCChild($node, 'location', htmlspecialchars($loc));
			
			$this->addCChild($node, 'property_type', 'apartment');
			
			if($prop->yearBuilt) $this->addCChild($node, 'year', $prop->yearBuilt);
			
			$this->addCChild($node, 'model', $listing->name);
			
			if($prop->phone) $this->addCChild($node, 'agent_phone', $prop->phone);
			if($prop->phone2) $this->addCChild($node, 'secondary_contact_phone', $prop->phone2);
			
			if(in_arrayi('new', $listing->condition))
				$this->addCChild($node, 'condition', 'new');
			elseif(in_arrayi('renovated', $listing->condition))
				$this->addCChild($node, 'condition', 'refurbished');
			else
				$this->addCChild($node, 'condition', 'used');
			
			$this->addCChild($node, 'provider_class', 'aggregator');
			*/
		}
		
		private function addCChild($node, $name, $val = '')
		{
			return $node->addChild('c:' . $name, $val, 'http://rentbits.com/collector');
		}
		
		//totalImages is running count, maxImages is max per listing.
		private function feedAddFiles($files, $type, $parent, $totalImages, $maxImages)
		{
			$num = $totalImages;
			foreach($files as $i => $file)
			{
				if($num < $maxImages)
				{
					$info = pathinfo($file);
					$this->addCChild($parent, 'image_link', $file);
				}
				$num++;
			}
			return $num;
		}
	}
	
	ue_feeds_register_adapter('Rentbits', '', 'ue_feeds_adapter_rentbits');