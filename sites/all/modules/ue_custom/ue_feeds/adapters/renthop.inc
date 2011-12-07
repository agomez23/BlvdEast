<?php
	
	class ue_feeds_adapter_renthop extends ue_feeds_adapter implements ue_feeds_adapter_output
	{
		public function feed(&$nodes)
		{
			global $base_url;
			$root = new SimpleXMLExtended('<feed></feed>');
			
			foreach($nodes['properties'] as $i => $prop)
				foreach($prop->listings as $i => $listing)
					$this->feedListing($prop, $listing, $root);
			
			return $root->asXml();
		}
		
		private function feedListing($prop, $listing, $parent)
		{
			$node = $parent->addChild('listing');
			
			$node->address = "$prop->addr1, $prop->city, $prop->state $prop->zip";
			
			if($listing->price > 0)
				$node->price = $listing->price;
			else
			{
				$this->logError("Price is zero.", $listing->id);
				unset($parent->listing[count($parent->listing)-1]);
				return;
			}
			
			$node->bedrooms = $listing->bed;
			$node->bathrooms = $listing->bath;
			
			if($listing->dateAvail)
				$node->availability_date = date('m/d/Y', $listing->dateAvail);
			
			if(is_array($listing->sqft))
				$node->square_feet = $listing->sqft['max'];
			elseif(floatval($listing->sqft) > 0)
				$node->square_feet = $listing->sqft;
			
			$node->description = '';
			
			if(isset($listing->desc))
				$node->description = $this->stripHtml($listing->desc) . "\n\n";
			
			if(isset($prop->desc))
				$node->description .= $this->stripHtml($prop->desc);
			
			// If there is no description for an item Google will invalidate the feed.
			if(strlen($node->description) < 15)
			{
				$this->logError("Listing has an invalid (or short) description and was not included in the feed.", $listing->id);
				unset($parent->listing[count($parent->listing)-1]);
				return;
			}
			
			if($prop->contact_name)
				$node->contact_name = $prop->contact_name;
			if($prop->leasing_mgr)
				$node->contact_name = $prop->leasing_mgr->title;
			elseif($prop->building_mgr)
				$node->contact_name = $prop->building_mgr->title;
			elseif($prop->building_own)
				$node->contact_name = $prop->building_own->title;
			else
			{
				$this->logError("No contact name or company (required for renthop feeds).", $prop->id);
				unset($parent->listing[count($parent->listing)-1]);
				return;
			}
				
			if($prop->email) $node->email = $prop->email;
			if($prop->phone) $node->phone_number = $prop->phone;
			
			$node->id = $listing->id;
			
			$node->website = url('node/' . $listing->id, array('absolute' => true));
		
			$photos = array();
			
			if($listing->photos)
				$photos = array_merge($photos, $listing->photos);
		
			if($prop->photos)
				$photos = array_merge($photos, $prop->photos);
			
			if(count($photos))
			{
				$node->photos = '';
				foreach($photos as $url)
					$node->photos->addChild('url', $url);
			}
			
			$amen = array();
			
			if($prop->amenities)
				$amen = array_merge($amen, $prop->amenities);
			
			if($prop->services)
				$amen = array_merge($amen, $prop->services);
			
			if(count($amen))
			{
				$node->features = '';
				foreach($amen as $feature)
					$node->features->addChild('feature', $feature);
			}
		}
	}
	
	ue_feeds_register_adapter('Renthop', '', 'ue_feeds_adapter_renthop');