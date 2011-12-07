<?php
	
	class ue_feeds_adapter_trovit extends ue_feeds_adapter implements ue_feeds_adapter_output
	{
		public function feed(&$nodes)
		{
			$root = new SimpleXMLExtended('<?xml version="1.0"?><trovit></trovit>');
			
			foreach($nodes['properties'] as $i => $prop)
				if(is_array($prop->listings))
					foreach($prop->listings as $i => $listing)
						$this->feedListing($prop, $listing, $root);
			
			$outputXML = $root->asXML();
			
			/*
			$dom = new DOMDocument('1.0', 'UTF-8');
			$dom->preserveWhiteSpace = false;
			$dom->formatOutput = true;
			$dom->loadXML($outputXML);
			$outputXML = $dom->saveXML();
			*/
			
			$outputXML = str_replace('<?xml version="1.0"?>', '<?xml version="1.0" encoding="UTF-8"?>', $outputXML);

			return $outputXML;
		}
		
		private function feedListing(&$prop, &$listing, &$parent)
		{
			$node = $parent->addChild('ad');
			
			$node->addCData('id', $listing->id);
			$node->addCData('url', url('node/' . $listing->id, array('absolute' => true)));
			
			$node->addCData('title', substr($prop->name . ' : ' . $listing->name, 0, 80));
			
			$node->addCData('type', isset($listing->sales) && $listing->sales ? 'For sale' : 'For rent');
			
			if(isset($listing->desc))
				$content = $this->stripHtml($listing->desc) . "\n\n";
			
			if(isset($prop->desc))
				$content .= $this->stripHtml($prop->desc);
			
			// minimum 30 characters
			if(strlen($content) < 30)
			{
				//$this->logError("Listing has an invalid (or short) description and was not included in the feed.", $listing->id);
				unset($parent->ad[count($parent->ad)-1]);
				return;
			}
			
			$node->addCData('content', $content);
			
			$node->addCData('price', $listing->price);
			
			$node->addCData('property_type', 'apartment');
			
			if(isset($listing->sqft) && is_array($listing->sqft))
				$node->addCData('floor_area', $listing->sqft['max']);
			elseif(isset($listing->sqft) && floatval($listing->sqft) > 0)
				$node->addCData('floor_area', $listing->sqft);
			
			$node->addCData('rooms', $listing->bed);
			$node->addCData('bathrooms', $listing->bath);
			
			$node->addCData('address', $prop->addr1);
			$node->addCData('city', $prop->city);
			$node->addCData('city_area', $prop->hoodName);
			$node->addCData('postcode', $prop->zip);
			$node->addCData('region', $prop->state);
			
			if(isset($prop->lat)) $node->addCData('latitude', $prop->lat);
			if(isset($prop->lng)) $node->addCData('longitude', $prop->lng);
			
			//No documented max for trovit?
			$totalImages = 0;
			$maxImages = 50;
		
			if($listing->photos)
				$totalImages += $this->feedAddFiles($listing->photos, $node, $totalImages, $maxImages);
		
			if($prop->photos)
				$totalImages += $this->feedAddFiles($prop->photos, $node, $totalImages, $maxImages);
			
			if(isset($prop->yearBuilt)) $node->addCData('year', $prop->yearBuilt);
			
		}
		
		// each feed may need to alter how we strip html because of differences in ways that entities are parsed.
		protected function stripHtml($html)
		{
			$output = trim(strip_tags($html));
			$output = $this->encode($output);
			
			return $output;
		}
		
		protected function encode($str)
		{
			return mb_convert_encoding($str, 'UTF-8');
		}
		
		//totalImages is running count, maxImages is max per listing.
		private function feedAddFiles($files, $parent, $totalImages, $maxImages, $caption = NULL)
		{
			if(count($files) && !$parent->pictures) $parent->addChild('pictures');
			
			$num = $totalImages;
			foreach($files as $i => $file)
			{
				if($num < $maxImages)
				{
					$picture = $parent->pictures->addChild('picture');
					$picture->addCData('picture_url', $file);
					if($caption) $picture->addCData('picture_title', $caption);
				}
				
				$num++;
			}
			return $num;
		}
	}
	
	ue_feeds_register_adapter('Trovit', 'Trovit adapter.', 'ue_feeds_adapter_trovit');