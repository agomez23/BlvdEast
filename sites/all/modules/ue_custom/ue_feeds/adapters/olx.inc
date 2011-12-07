<?php
	
	class ue_feeds_adapter_olx extends ue_feeds_adapter implements ue_feeds_adapter_output
	{
		public function feed(&$nodes)
		{
			$root = new SimpleXMLExtended('<?xml version="1.0" encoding="utf-8"?><ADS></ADS>');
			
			foreach($nodes['properties'] as $i => $prop)
				foreach($prop->listings as $i => $listing)
					$this->feedListing($prop, $listing, $root);
			
			return $root->asXml();
		}
		
		private function feedListing($prop, $listing, $parent)
		{
			$node = $parent->addChild('AD');
			$node->ID = $listing->id;
			$node->addCData('TITLE', $prop->name . ' : ' . $listing->name);
			
			if(isset($listing->desc) && strlen($listing->desc) > 15)
			{
				$desc = $listing->desc;
				
				if(strlen($listing->desc) > 15)
					$desc .= '<br><hr><br>';
			}
			
			$desc .= $prop->desc;
			
			// make sure description has more than 15 characters, and exclude the listing otherwise.
			if(strlen($desc) < 15)
			{
				$this->logError("Listing has an invalid (or short) description and was not included in the feed.", $listing->id);
				unset($parent->AD[count($parent->AD)-1]);
				return;
			}
			
			$node->addCData('DESCRIPTION', $desc);
			
			$node->DATE = date('d-m-Y H:i:s', $listing->updated);
			$node->EXPIRE_DATE = date('d-m-Y H:i:s', strtotime('+2 weeks', $listing->updated));
			$node->addCData('EMAIL', $prop->email ? $prop->email : 'listings@urbanedgeny.com');
			if($prop->phone) $node->PHONE = $prop->phone;
			$node->addCData('ADDRESS', $prop->addr1);
			$node->ZIP_CODE = $prop->zip;
			$node->LOCATION_COUNTRY = 'US';
			$node->LOCATION_STATE = $prop->state;
			$node->LOCATION_CITY = $prop->city;
			$node->BEDROOMS = $listing->bed;
			$node->BATHROOMS = $listing->bath;
			
			//Pets checkboxes.
			if($prop->pets->dogs || $prop->pets->cats)
				$node->PETS = 'yes';
			
			$node->BROKERFEE = 'no';
			
			if(is_array($listing->sqft))
				$node->SURFACE = $listing->sqft['max'] . ' square ft.';
			elseif(floatval($listing->sqft) > 0)
				$node->SURFACE = $listing->sqft . ' square ft.';
			
			$node->CATEGORY = 363;
			
			//OLX has a maximum of 20 images.
			$totalImages = 0;
			$maxImages = 20;
			
			if($listing->floorplans)
				$totalImages += $this->feedAddFiles($listing->floorplans, $node, $totalImages, $maxImages);
		
			if($listing->photos)
				$totalImages += $this->feedAddFiles($listing->photos, $node, $totalImages, $maxImages);
		
			if($prop->photos)
				$totalImages += $this->feedAddFiles($prop->photos, $node, $totalImages, $maxImages);
			
			$node->PRICE = $listing->price;
			$node->CURRENCY = 'USD';
		}
		
		//totalImages is running count, maxImages is max per listing.
		private function feedAddFiles($files, $parent, $totalImages, $maxImages)
		{
			$num = $totalImages;
			foreach($files as $i => $file)
			{
				if($num < $maxImages)
					$parent->addCData('IMAGE_URL', $file);
				else
					break;
				$num++;
			}
			return $num;
		}
	}
	
	ue_feeds_register_adapter('OLX', '', 'ue_feeds_adapter_olx');
