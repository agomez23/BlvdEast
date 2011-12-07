<?php
	
	class ue_feeds_adapter_oodle extends ue_feeds_adapter implements ue_feeds_adapter_output
	{
		
		protected $numListings;
		
		public function feed(&$nodes)
		{
			global $base_url;
			$root = new SimpleXMLExtended('<listings></listings>');
			
			$this->numListings = 0;
			
			foreach($nodes['properties'] as $i => $prop)
			{
				foreach($prop->listings as $i => $listing)
				{
					$this->feedListing($prop, $listing, $root);
					
					$this->numListings++;
					
					if($this->feed->options->listingcap > 0 && $this->numListings >= $this->feed->options->listingcap)
						break;
				}
					
				if($this->feed->options->listingcap > 0 && $this->numListings >= $this->feed->options->listingcap)
					break;
			}
			
			return $root->asXml();
		}
		
		private function feedListing($prop, $listing, $parent)
		{
			$node = $parent->addChild('listing');
			
			$node->category = 'Apartments For Rent';
			
			$node->title = $prop->name . ' : ' . $listing->name;
			$node->id = $listing->id;
			
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
			
			$node->bedrooms = $listing->bed;
			$node->bathrooms = $listing->bath;
			
			if(is_array($listing->sqft))
				$node->square_feet = $listing->sqft['max'];
			elseif(floatval($listing->sqft) > 0)
				$node->square_feet = $listing->sqft;
			
			$node->url = url('node/' . $listing->id, array('absolute' => true));
			
			$amen = array();
			
			if($prop->amenities)
				$amen = array_merge($amen, $prop->amenities);
			
			if($prop->services)
				$amen = array_merge($amen, $prop->services);
			
			if(count($amen))
				$node->amenities = implode(', ', array_unique($amen));
		
			$photos = array();
			
			if($listing->photos)
				$photos = array_merge($photos, $listing->photos);
		
			if($prop->photos)
				$photos = array_merge($photos, $prop->photos);
			
			if(count($photos))
				$node->image_url = implode('|', $photos);
			
			$node->address = $prop->addr1;
			$node->city = $prop->city;
			$node->state = $prop->state;
			$node->zip_code = $prop->zip;
			$node->country = 'US';
			
			if($prop->lat) $node->latitude = $prop->lat;
			if($prop->lng) $node->longitude = $prop->lng;
			
			$node->neighborhood = $prop->hoodName;
			
			if($prop->phone) $node->seller_phone = $prop->phone;
			if($prop->email) $node->seller_email = $prop->email;
			if($prop->website) $node->seller_url = $prop->website;
			
			if($listing->price > 0)
				$node->price = $listing->price;
			else
			{
				$this->logError("Listing has a price of 0 and was not included in the feed.", $listing->id);
				unset($parent->listing[count($parent->listing)-1]);
				return;
			}
		}
		
		//totalImages is running count, maxImages is max per listing.
		private function feedAddFiles($files, $parent, $totalImages, $maxImages)
		{
			if(count($files) && !$parent->image_url) $parent->addChild('image_url');
			
			$num = $totalImages;
			foreach($files as $i => $file)
			{
				if($num < $maxImages)
					$parent->pictures->addChild('picture')->addChild('picture-url', $file);
				$num++;
			}
			return $num;
		}
		
		public function getForm()
		{
			$form['listingcap'] = array(
				'#type' => 'textfield',
				'#title' => t('Max # Listings'),
				'#default_value' => $this->feed->options->listingcap,
				'#maxlength' => 10,
				'#size' => 10,
				'#description' => t('Maximum # of listings to output. Leave blank to feed all.'),
			);
			return $form;
		}
		
		public function processForm($form_state)
		{
			$this->feed->options->listingcap = $form_state['values']['listingcap'];
		}
	}
	
	ue_feeds_register_adapter('Oodle', '', 'ue_feeds_adapter_oodle');
