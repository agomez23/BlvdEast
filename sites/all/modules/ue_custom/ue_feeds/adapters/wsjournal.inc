<?php
	
	class ue_feeds_adapter_wsjournal extends ue_feeds_adapter implements ue_feeds_adapter_output
	{
		
		public function feed(&$nodes)
		{
			$root = new SimpleXMLElement('<Properties></Properties>');
			
			foreach($nodes['properties'] as $i => $prop)
				if(is_array($prop->listings))
					foreach($prop->listings as $i => $listing)
						$this->feedListing($prop, $listing, $root);
			
			return $root->asXml();
		}
		
		private function feedListing(&$prop, &$listing, &$parent)
		{
			$node = $parent->addChild('Property');
			$node->Clientid = '';
			$node->Adid = $listing->id;
			
			$node->Address1 = $prop->addr1;
			if(isset($prop->addr2)) $node->Address2 = $prop->addr2;
			$node->City = $prop->city;
			$node->State = $prop->state;
			$node->Zip = $prop->zip;
			if(isset($prop->lat)) $node->Latitude = $prop->lat;
			if(isset($prop->lng)) $node->Longitude = $prop->lng;
					
			//Try to fill first/last name
			if($prop->contact_name)
			{
				$name = explode(' ', $prop->contact_name);
				$node->ContactData->ContactFirstName = array_shift($name);
				$node->ContactData->ContactLastName = implode(' ', $name);
			}
			
			if(isset($prop->building_mgr)) $node->ContactData->ContactAgencyName = $prop->building_mgr->name;
			if(isset($prop->phone)) $node->ContactData->ContactPhone = $prop->phone;
			if(isset($prop->email)) $node->ContactData->ContactEmail = $prop->email;
			
			$node->ContactData->ContactUrl = url('node/' . $listing->id, array('absolute' => true));
			
			//$node->PostDate = format_date($listing->updated, 'custom', 'Y-m-d');
			//$node->ExpireDate = format_date(strtotime('+2 weeks'), 'custom', 'Y-m-d');
			
			$node->PropertyType = 'Apartment';
			
			$node->Terms = 'rent';
			
			$node->MinPrice = $listing->price;
			
			$node->BathroomNumber = $listing->bath;
			$node->BedroomNumber = $listing->bed;
			
			$node->DescriptionHtml = '';
			
			if(isset($listing->desc) && strlen($listing->desc) > 15)
			{
				$node->DescriptionHtml = $listing->desc;
				$node->DescriptionHtml .= '<br><hr><br>';
			}
			$node->DescriptionHtml .= $prop->desc;
			
			if(isset($listing->sqft) && is_array($listing->sqft))
				$node->SqFt = $listing->sqft['max'];
			elseif(isset($listing->sqft) && floatval($listing->sqft) > 0)
				$node->SqFt = $listing->sqft;
			
			$node->AvailableDate = $listing->dateAvail ? date('Y-m-d', $listing->dateAvail) : date('Y-m-d');
			
			/*
			
			$node->title = substr($prop->name . ' : ' . $listing->name, 0, 80);
		
			if(isset($prop->amenities))
				foreach($prop->amenities as $i => $amen)
					$this->addGChild($node, 'feature', $amen);
			
			if(isset($prop->services))
				foreach($prop->services as $i => $service)
					$this->addGChild($node, 'feature', $service);
			
			$node->id = $listing->id;
			
			*/
			
			//WSJ has a maximum of 6 images.
			$totalImages = 0;
			$maxImages = 6;
		
			if(isset($listing->photos))
				$totalImages += $this->feedAddFiles($listing->photos, $node, $totalImages, $maxImages);
		
			if(isset($prop->photos))
				$totalImages += $this->feedAddFiles($prop->photos, $node, $totalImages, $maxImages);
			
		}
		
		//totalImages is running count, maxImages is max per listing.
		private function feedAddFiles(&$files, &$parent, $totalImages, $maxImages)
		{
			$num = $totalImages;
			foreach($files as $i => $file)
			{
				if($num < $maxImages)
				{
					if(!$parent->Images) $parent->Images = '';
					
					$parent->Images->addChild('Image', $file);
				}
				$num++;
			}
			return $num;
		}
	}
	
	ue_feeds_register_adapter('Wall Street Journal', '', 'ue_feeds_adapter_wsjournal');
