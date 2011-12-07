<?php
	
	class ue_feeds_adapter_base extends ue_feeds_adapter implements ue_feeds_adapter_output
	{
		
		public function feed(&$nodes)
		{
			$root = new SimpleXMLElement('<feed xmlns="http://www.w3.org/2005/Atom" xmlns:g="http://base.google.com/ns/1.0"></feed>');
			
			$this->addrWarnings = array();
			$this->descWarnings = array();
			
			foreach($nodes['properties'] as $i => $prop)
				if(is_array($prop->listings))
					foreach($prop->listings as $i => $listing)
						$this->feedListing($prop, $listing, $root);
			
			return $root->asXml();
		}
		
		private function feedListing(&$prop, &$listing, &$parent)
		{
			//skip all listings except brooklyner in brownstoner feed.
			if($this->feed->fid == 36 && !in_array($prop->id, array(759, 17249)))
				return;
			
			$node = $parent->addChild('entry');
			$node->title = substr($prop->name . ' : ' . $listing->name, 0, 80);
			
			if(isset($prop->contact_name)) $this->addGChild($node, 'agent', $prop->contact_name);
			
			if(isset($listing->sqft) && is_array($listing->sqft))
				$this->addGChild($node, 'area', $listing->sqft['max'] . ' square ft.');
			elseif(isset($listing->sqft) && floatval($listing->sqft) > 0)
				$this->addGChild($node, 'area', $listing->sqft . ' square ft.');
			
			$this->addGChild($node, 'bedrooms', $listing->bed);
			$this->addGChild($node, 'bathrooms', $listing->bath);
			
			$node->summary = '';
			
			if(isset($listing->desc))
				$node->summary = $this->stripHtml($listing->desc) . "\n\n";
			
			if(isset($prop->desc))
				$node->summary .= $this->stripHtml($prop->desc);
			
			// If there is no description for an item Google will invalidate the feed.
			if(strlen($node->summary) < 15)
			{
				//$this->logError("Listing has an invalid (or short) description and was not included in the feed.", $listing->id);
				unset($parent->entry[count($parent->entry)-1]);
				return;
			}
			
			$this->addGChild($node, 'expiration_date', date('Y-m-d', strtotime('+2 weeks')));
		
			if(isset($prop->amenities))
				foreach($prop->amenities as $i => $amen)
					$this->addGChild($node, 'feature', $amen);
			
			if(isset($prop->services))
				foreach($prop->services as $i => $service)
					$this->addGChild($node, 'feature', $service);
			
			$node->id = $listing->id;
			
			//Google base has a maximum of 10 images.
			$totalImages = 0;
			$maxImages = 10;
			
			if(isset($listing->floorplans))
				$totalImages += $this->feedAddFiles($listing->floorplans, 'floorplan_link', $node, $totalImages, $maxImages);
		
			if(isset($listing->photos))
				$totalImages += $this->feedAddFiles($listing->photos, 'image_link', $node, $totalImages, $maxImages);
		
			if(isset($prop->photos))
				$totalImages += $this->feedAddFiles($prop->photos, 'image_link', $node, $totalImages, $maxImages);
			
			//for brooklyner in brownstone feed, feed their link.
			if($this->feed->fid == 36 && in_array($prop->id, array(759, 17249)))
				$url = $prop->website;
			else
				$url = url('node/' . $listing->id, array('absolute' => true));
			
			$node->addChild('link')->addAttribute('href', $url);
			
			$this->addGChild($node, 'listing_type', 'for rent');
			
			$loc = $prop->addr1;
			if(isset($prop->addr2)) $loc .= ', ' . $prop->addr2;
			$loc .= ', ' . $prop->city;
			$loc .= ', ' . $prop->state;
			$loc .= ' ' . $prop->zip;
			
			//if(stripos($prop->addr1, '-') !== false || stripos($prop->addr1, '&') !== false)
			//	$this->logWarn("Property may be excluded because of an invalid address.", $prop->id);
			
			$this->addGChild($node, 'location', $this->encode($loc));
			
			if($listing->price) $this->addGChild($node, 'price', $listing->price);
			
			$this->addGChild($node, 'property_type', 'apartment');
			
			if(isset($prop->yearBuilt)) $this->addGChild($node, 'year', $prop->yearBuilt);
			
			$this->addGChild($node, 'model', $listing->name);
			
			if(isset($prop->phone)) $this->addGChild($node, 'agent_phone', $prop->phone);
			if(isset($prop->phone2)) $this->addGChild($node, 'secondary_contact_phone', $prop->phone2);
			
			if(isset($prop->lat)) $this->addGChild($node, 'latitude', $prop->lat);
			if(isset($prop->lng)) $this->addGChild($node, 'longitude', $prop->lng);
			
			if(isset($listing->condition) && in_arrayi('new', $listing->condition))
				$this->addGChild($node, 'condition', 'new');
			elseif(isset($listing->condition) && in_arrayi('renovated', $listing->condition))
				$this->addGChild($node, 'condition', 'refurbished');
			else
				$this->addGChild($node, 'condition', 'used');
			
			$this->addGChild($node, 'provider_class', 'aggregator');
		}
		
		// shortcut to adding gangsta nodes
		private function addGChild(&$node, $name, $val = '')
		{
			$child = $node->addChild('g:' . $name, $val, 'http://base.google.com/ns/1.0');
		}
		
		//totalImages is running count, maxImages is max per listing.
		private function feedAddFiles(&$files, $type, &$parent, $totalImages, $maxImages)
		{
			$num = $totalImages;
			foreach($files as $i => $file)
			{
				if($num < $maxImages)
				{
					$info = pathinfo($file);
					$this->addGChild($parent, 'image_link', $file);
				}
				$num++;
			}
			return $num;
		}
	}
	
	ue_feeds_register_adapter('Google Base', '', 'ue_feeds_adapter_base');
