<?php
	
	class ue_feeds_adapter_zillow extends ue_feeds_adapter implements ue_feeds_adapter_output
	{
		public function feed(&$nodes)
		{
			global $base_url;
			$root = new SimpleXMLExtended('<Listings></Listings>');
			
			$parsed = parse_url($base_url);
			$this->domain = $parsed['host'];
			
			foreach($nodes['properties'] as $i => $prop)
				foreach($prop->listings as $i => $listing)
					$this->feedListing($prop, $listing, $root);
			
			return $root->asXml();
		}
		
		private function feedListing($prop, $listing, $parent)
		{
			$node = $parent->addChild('Listing');
			
			$node->BasicDetails->Title = $prop->name . ' : ' . $listing->name;
			//$node->BasicDetails->addChild('provider-listingid', $listing->id);
			
			//for rent
			$node->ListingDetails->Status = 'For Rent';
					
			$node->BasicDetails->Description = '';
			
			if(isset($listing->desc))
				$node->BasicDetails->Description = $this->stripHtml($listing->desc) . "\n\n";
			
			if(isset($prop->desc))
				$node->BasicDetails->Description .= $this->stripHtml($prop->desc);
			
			$node->BasicDetails->Description = trim($node->BasicDetails->Description);
			
			// If there is no description for an item Google will invalidate the feed.
			if(strlen($node->BasicDetails->Description) < 15)
			{
				$this->logError("Listing has an invalid (or short) description and was not included in the feed.", $listing->id);
				unset($parent->Listing[count($parent->Listing)-1]);
				return;
			}
			
			$node->BasicDetails->Bedrooms = $listing->bed;
			
			$node->BasicDetails->Bathrooms = $listing->bath;
			if($listing->fullBath)
				$node->BasicDetails->FullBathrooms = $listing->fullBath;
			if($listing->halfBath)
				$node->BasicDetails->HalfBathrooms = $listing->halfBath;
			
			if(is_array($listing->sqft))
				$node->BasicDetails->LivingArea = $listing->sqft['max'];
			elseif(floatval($listing->sqft) > 0)
				$node->BasicDetails->LivingArea = $listing->sqft;
			
			if($prop->yearBuilt) $node->BasicDetails->YearBuilt = $prop->yearBuilt;
			
			$node->ListingDetails->ListingUrl = url('node/' . $listing->id, array('absolute' => true));
			
			//Zillow has a maximum of 50 images.
			$totalImages = 0;
			$maxImages = 50;
		
			if($listing->photos)
				$totalImages += $this->feedAddFiles($listing->photos, $node, $totalImages, $maxImages);
		
			if($prop->photos)
				$totalImages += $this->feedAddFiles($prop->photos, $node, $totalImages, $maxImages);
			
			if($listing->floorplans)
				$totalImages += $this->feedAddFiles($listing->floorplans, $node, $totalImages, $maxImages, 'Floorplan');
			
			//add building info.
			//$this->addBuilding($node, $prop);
			
			$node->Location->UnitNumber = $listing->name;
			$node->Location->StreetAddress = $prop->addr1;
			$node->Location->City = $prop->city;
			$node->Location->State = $prop->state;
			$node->Location->Zip = $prop->zip;
			if($prop->crossStreet) $node->Location->StreetIntersection = $prop->crossStreet;
			if($prop->lat) $node->Location->Lat = $prop->lat;
			if($prop->lng) $node->Location->Long = $prop->lng;
			
			$node->Neighborhood->Name = $prop->hoodName;
			if(strlen($this->stripHtml($prop->hoodDesc)) > 0) $node->Neighborhood->Description = $this->stripHtml($prop->hoodDesc);
			
			if($prop->phone) $node->Agent->OfficeLineNumber = $prop->phone;
			if($prop->fax) $node->Agent->FaxLineNumber = $prop->fax;
			if($prop->email) $node->Agent->EmailAddress = $prop->email;
			if($prop->website) $node->Office->BrokerWebsite = $prop->website;
			
			if($listing->price > 0)
				$node->ListingDetails->Price = $listing->price;
			else
			{
				print "Warning: " . $prop->name . ' : ' . $listing->name . " (node " . $listing->id . ") has a price of 0 and was not included in the feed.\n";
				unset($parent->Listing[count($parent->Listing)-1]);
				return;
			}
			
			$node->BasicDetails->PropertyType = 'apartment';
			
			$detailed = $node->addChild('RichDetails');
			
			if($prop->unitCount) $detailed->BuildingUnitCount = $prop->unitCount;
			
			if(in_arrayi('pool', $prop->amenities)) $detailed->Pool = 'yes';
			if(in_arrayi('doorman', $prop->services)) $detailed->Doorman = 'yes';
			if(in_arrayi('elevator', $prop->access)) $detailed->Elevator ='yes';
			$detailed->PropertyName = $prop->name;
		}
		
		private function addBuilding($parent, $prop)
		{
			
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
		private function feedAddFiles($files, $parent, $totalImages, $maxImages, $caption = NULL)
		{
			if(count($files) && !$parent->Pictures) $parent->addChild('Pictures');
			
			$num = $totalImages;
			foreach($files as $i => $file)
			{
				if($num < $maxImages)
				{
					$picture = $parent->Pictures->addChild('Picture');
					$picture->PictureUrl = $file;
					if($caption) $picture->Caption = $caption;
				}
				
				$num++;
			}
			return $num;
		}
	}
	
	ue_feeds_register_adapter('Zillow', '', 'ue_feeds_adapter_zillow');