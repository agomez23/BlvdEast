<?php
	
	class ue_feeds_adapter_nco extends ue_feeds_adapter implements ue_feeds_adapter_output
	{
		public function feed(&$nodes)
		{
			global $base_url;
			$root = new SimpleXMLExtended('<NCOPropertyFeed></NCOPropertyFeed>');
			
			$root->ReleaseVersion = '1.1';
			$root->SubmitterProfile->SubmitterID = 0;
			$root->SubmitterProfile->Submitter = variable_get('site_name', '');
			$root->SubmitterProfile->SubmitterEmail = 'listings@urbanedgeny.com';
			
			$parsed = parse_url($base_url);
			$this->domain = $parsed['host'];
			
			foreach($nodes['properties'] as $i => $prop)
				$this->feedProperty($prop, $root);
			
			$root->Summary->TotalProperties = count($root->Property);
			$root->Summary->FeedPrepareTimestamp = date('Y-m-d H:i:s');
			
			return $root->asXml();
		}
		
		protected function feedProperty($prop, $parent)
		{
			$node = $parent->addChild('Property');
			
			$node->PropertyID = $prop->id;
			$node->PropertyName = $prop->name;
			
			$node->PropertyAddress = $prop->addr1;
			
			$node->CityAreas->City->CityName = $prop->city;
			$node->CityAreas->City->AreaName = $prop->hoodName;
			
			$node->StateAbbr = $prop->state;
			$node->ZIPCode = $prop->zip;
			
			$node->PropertyEmail = $prop->email;
			
			$node->SalesOfficePhone = $prop->phone;
			
			if($prop->leasing_mgr)
				$builder = $prop->leasing_mgr;
			elseif($prop->building_mgr)
				$builder = $prop->building_mgr;
			elseif($prop->building_own)
				$builder = $prop->building_own;
			
			if($builder)
			{
				$node->BuilderProfile->BuilderName = $builder->name;
				$node->BuilderProfile->BuilderEmail = $builder->email;
				$node->BuilderProfile->BuilderWebsiteURL = $builder->website;
			}
			else
			{
				$node->BuilderProfile->BuilderName = '';
				$node->BuilderProfile->BuilderEmail = '';
				$node->BuilderProfile->BuilderWebsiteURL = '';
			}
			
			$node->CondoType = 3;
			
			$node->Floor = '';
			
			$node->Units = count($prop->listings); // in NCO units is available units, not building units.
			
			$maxBed = $maxBath = $maxArea = $maxPrice = NULL;
			$minBed = $minBath = $minArea = $minPrice = NULL;
			
			//calculate bed/bath/area/price from-to.
			foreach($prop->listings as $i => $listing)
			{
				if($listing->bed < $minBed || $minBed === NULL) $minBed = $listing->bed;
				if($listing->bed > $maxBed) $maxBed = $listing->bed;
				
				if($listing->bath < $minBath || $minBath === NULL) $minBath = $listing->bath;
				if($listing->bath > $maxBath) $maxBath = $listing->bath;
				
				if(is_array($listing->sqft))
				{
					if($listing->sqft['min'] < $minArea || $minArea === NULL) $minArea = $listing->sqft['min'];
					if($listing->sqft['max'] > $maxArea) $maxArea = $listing->sqft['max'];
				}
				elseif(floatval($listing->sqft) > 0)
				{
					if($listing->sqft < $minArea || $minArea === NULL) $minArea = $listing->sqft;
					if($listing->sqft > $maxArea) $maxArea = $listing->sqft;
				}
				
				if(is_array($listing->price))
				{
					if($listing->price['min'] < $minPrice || $minPrice === NULL) $minPrice = $listing->price['min'];
					if($listing->price['max'] > $maxPrice) $maxPrice = $listing->price['max'];
				}
				elseif(floatval($listing->price) > 0)
				{
					if($listing->price < $minPrice || $minPrice === NULL) $minPrice = $listing->price;
					if($listing->price > $maxPrice) $maxPrice = $listing->price;
				}
			}
			
			$node->BathFrom = $minBath;
			$node->BathTo = $maxBath;
			
			$node->BedroomFrom = $minBed;
			$node->BedroomTo = $maxBed;
			
			$node->SizeFrom = $minArea;
			$node->SizeTo = $maxArea;
			
			$node->PriceFrom = $minPrice;
			$node->PriceTo = $maxPrice;
			
			$node->PriceFromPrefix = 6;
			$node->PriceToPrefix = 5;
			
			//info we don't have. NCO requests we put empty nodes in to waste bandwidth :)
			$node->Phase = '';
			$node->CompletionDate->CompletionMonthPrefix = '';
			$node->CompletionDate->CompletionMonth = '';
			$node->CompletionDate->CompletionYear = '';
			$node->CompletionDate->CompletionMonthPrefix = '';
			
			$node->PropertyURL = url('node/' . $prop->id, array('absolute' => true)); // outgoing feeds link back to our site.
			$node->LeadFormURL = url('node/' . $prop->id, array('absolute' => true)); // outgoing feeds link back to our site.
			
			if($prop->photos)
			{
				$node->DefaultImageURL = array_shift($prop->photos);
				if(count($prop->photos) > 0)
				{
					$node->ImageGallery = '';
					$this->feedAddFiles($prop->photos, $node->ImageGallery);
				}
			}
			
			$node->SpecialIncentive = '';
			$node->FullDescription = $this->stripHtml($prop->desc);
			
			return $node;
		}
		
		protected function feedAddFiles($files, $parent)
		{
			foreach($files as $i => $file)
				$parent->addChild('ImageURL', $file);
		}
	}
	
	ue_feeds_register_adapter('Google Base', '', 'ue_feeds_adapter_nco');
