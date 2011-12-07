<?php
	
	class ue_feeds_adapter_streeteasy extends ue_feeds_adapter implements ue_feeds_adapter_input, ue_feeds_adapter_output_stream
	{
		//fetch a company node.
		private $parsed = NULL;
		
		public function parse($reset = false)
		{
			if(!$this->parsed || $reset)
			{
				$xml = simplexml_load_string($this->feed->read(), null, LIBXML_NOCDATA);
				$this->parsed['properties'] = $this->parseProperties($xml->properties->property);
			}
			
			return $this->parsed;
		}
		
		protected function parseProperties($xml)
		{
			foreach($xml as $property)
			{
				$prop = $this->parseProperty($property);
				
				if($properties[$prop->externalID])
					$properties[$prop->externalID]->listings[] = $this->parseListing($property);
				else
					$properties[$prop->externalID] = $prop;
			}
			
			return array_values($properties);
		}
		
		protected function parseProperty($xml)
		{
			$attr = $xml->attributes();
			$prop = new ue_feeds_property();
			
			if(strlen($xml->location->zipcode) > 0)
				$prop->externalID = $xml->location->address . '-' . $xml->location->zipcode;
			else
				$prop->externalID = (string)$xml->location->address;
			
			$prop->externalID = str_replace(' ', '_', $prop->externalID);
			$prop->externalID = str_replace('.', '_', $prop->externalID);
			
			$prop->name = (string)$xml->location->address;
			$prop->phone = (string)$xml->agents->office;
			$prop->email = (string)$xml->agents->lead_email;
			$prop->addr1 = (string)$xml->location->address;
			$prop->city = (string)$xml->location->city;
			$prop->state = (string)$xml->location->state;
			$prop->zip = (string)$xml->location->zipcode;
			
			$hood = ue_feeds_get_neighborhoods($prop->zip);
			if($hood) $prop->hood = $hood->tid;
			
			//$prop->desc = (string)$xml->details->description;
			
			//if($attr['url'])
			//	$prop->website = (string)$attr['url'];
			
			$prop->listings[] = $this->parseListing($xml);
			
			return $prop;
		}
		
		protected function parseListing($xml)
		{
			$attr = $xml->attributes();
			$listing = new ue_feeds_listing();
			$listing->name = (string)$xml->location->apartment;
			$listing->desc = (string)$xml->details->description;
			
			$listing->bed = (int)$xml->details->bedrooms;
			$listing->bath = (int)$xml->details->bathrooms;
			
			if(strtotime((string)$xml->details->availableOn) > time())
				$listing->dateAvail = strtotime((string)$xml->details->availableOn);
			
			if($xml->details->price)
				$listing->price = (float)preg_replace('/[^\d.]/', '', $xml->details->price);
			
			if($xml->media->photo)
			{
				foreach($xml->media->photo as $photoXml)
				{
					$attr = $photoXml->attributes();
					$listing->photos[] = (string)$attr['url'];
				}
			}
			
			if($xml->media->floorplan)
			{
				foreach($xml->media->floorplan as $floorplanXml)
				{
					$attr = $floorplanXml->attributes();
					$listing->floorplans[] = (string)$attr['url'];
				}
			}
			
			return $listing;
		}
		
		public function getForm()
		{
			$form['companyID'] = array(
				'#type' => 'textfield',
				'#title' => t('Company ID'),
				'#default_value' => $this->feed->options->companyID,
				'#maxlength' => 60,
				'#size' => 45,
				'#description' => t('Company ID'),
				'#required' => TRUE,
			);
			return $form;
		}
		
		public function processForm($form_state)
		{
			$this->feed->options->companyID = $form_state['values']['companyID'];
		}
		
		public function feedStream(&$nodes)
		{
			$this->writer = new XMLWriter();
			
			$this->writer->openMemory();
			
			$this->writer->setIndent(true);
			$this->writer->setIndentString("\t");
			
			$this->writer->startDocument('1.0', 'UTF-8');
			
			$this->writer->startElement('streeteasy');
			
			$this->writer->writeAttribute('version', '1.6');
			
			foreach($nodes['properties'] as $propID => $listings)
			{
				$prop = $this->feed->filter->getProperty($propID, $listings);
				$this->feedProperty($prop);
				unset($prop);
			}
			
			$this->writer->endElement();
			$this->writer->endDocument();
			
			return $this->writer->outputMemory(true);
		}
		
		
		protected function feedProperty(&$prop)
		{
			if(!$prop->listings || !count($prop->listings))
				return;
			
			foreach($prop->listings as $i => $listing)
			{
				$this->writer->startElement('property');
				
					$this->writer->writeAttribute('type', $listing->sales ? 'sales' : 'rental');
					
					$this->writer->writeAttribute('status', 'active');
					
					$this->writer->writeAttribute('id', $listing->id);
					
					$this->writer->writeAttribute('url', $prop->website ? $prop->website : url('node/' . $listing->id, array('absolute' => true)));
					
					$this->writer->startElement('location');
					
						$this->writer->writeElement('address', $prop->addr1);
						
						$this->writer->writeElement('apartment', $listing->unit_num);
					
						$this->writer->writeElement('city', $prop->city);
					
						$this->writer->writeElement('state', $prop->state);
					
						$this->writer->writeElement('zipcode', $prop->zip);
					
						$this->writer->writeElement('neighborhood', $prop->hoodName);
					
					$this->writer->endElement(); // </location>
					
					$this->writer->startElement('details');
					
						$this->writer->writeElement('price', $listing->price);
						
						$this->writer->writeElement('noFee');
						
						$this->writer->writeElement('bedrooms', $listing->bed);
						
						$this->writer->writeElement('bathrooms', $listing->bath);
								
						if(is_array($listing->sqft))
						{
							$this->writer->writeElement('squareFeet', $listing->sqft['min']);
						}
						elseif($listing->sqft > 0)
						{
							$this->writer->writeElement('squareFeet', $listing->sqft);
						}
						
						$this->writer->writeElement('availableOn', date('m/d/Y', $listing->dateAvail > time() ? $listing->dateAvail : time()));
						
						$desc = '';
						
						if(isset($listing->desc))
							$desc = $this->stripHtml($listing->desc) . "\n\n";
						
						if(isset($prop->desc))
							$desc .= $this->stripHtml($prop->desc);
						
						$this->writer->writeElement('description', $desc);
						
						$this->writer->writeElement('built', $prop->yearBuilt);
						
						$amenities = array();
							
						if(in_arrayi('full-time doorman', $prop->services))
							$amenities[] = 'doorman';
						
						//$this->writer->writeElement('gym');
						
						if(in_arrayi('pool', $prop->services))
							$amenities[] = 'pool';
							
						if(in_arrayi('elevator', $prop->access))
							$amenities[] = 'elevator';
						
						//$this->writer->writeElement('garage');
						
						//$this->writer->writeElement('parking');
						
						//$this->writer->writeElement('balcony');
						
						//$this->writer->writeElement('storage');
						
						//$this->writer->writeElement('patio');
						
						//$this->writer->writeElement('fireplace');
						
						//$this->writer->writeElement('washerDryer');
						
						//$this->writer->writeElement('dishwasher');
						
						//$this->writer->writeElement('furnished');
						
						if($prop->pets->cats || $prop->pets->dogs)
							$amenities[] = 'pets';
						
						//comma-delimited list of other amenities
						//$this->writer->writeElement('other', '');
						
						if(count($amenities) > 0)
						{
							$this->writer->startElement('amenities');
							
							foreach($amenities as $amenity)
								$this->writer->writeElement($amenity);
							
							$this->writer->endElement(); // </amenities>
						}
					
					$this->writer->endElement(); // </details>
					
					if($prop->contact_name || $prop->email || $prop->phone)
					{
						$this->writer->startElement('agents');
							
							$this->writer->startElement('agent');
							
								//$this->writer->writeAttribute('id', '1');
								
								if($prop->contact_name)
									$this->writer->writeElement('name', $prop->contact_name);
								elseif($prop->leasing_mgr)
									$this->writer->writeElement('name', $prop->leasing_mgr->title);
								elseif($prop->building_mgr)
									$this->writer->writeElement('name', $prop->building_mgr->title);
								elseif($prop->building_own)
									$this->writer->writeElement('name', $prop->building_own->title);
									
								//$this->writer->writeElement('company');
								
								//$this->writer->writeElement('photo');
								
								//$this->writer->writeElement('url');
								
								if($prop->email)
									$this->writer->writeElement('email', $prop->email);
								
								//$this->writer->writeElement('lead_email');
								
								if($prop->phone)
								{
									$this->writer->startElement('phone_numbers');
										
										$this->writer->writeElement('main', $prop->phone);
										
										//$this->writer->writeElement('office');
										
										//$this->writer->writeElement('cell');
										
										//$this->writer->writeElement('fax');
									
									$this->writer->endElement(); // </phone_numbers>
								}
							
							$this->writer->endElement(); // </agent>
						
						$this->writer->endElement(); // </agents>
					}
					
					if(count($listing->photos) + count($listing->floorplans) + count($prop->photos) > 0)
					{
						
						$this->writer->startElement('media');
						
							if(count($listing->photos))
								$this->feedMedia($listing->photos);
						
							if(count($prop->photos))
								$this->feedMedia($prop->photos);
						
							if(count($listing->floorplans))
								$this->feedMedia($listing->floorplans, 'floorplan');
						
						$this->writer->endElement(); // </media>
					}
				
				$this->writer->endElement(); // </property>
			}
		}
		
		private function feedMedia(&$files, $type = 'photo')
		{	
			if(!count($files))
				return;
			
			foreach($files as $file)
			{
				$this->writer->startElement($type);
			
				$this->writer->writeAttribute('url', $file);
			
				$this->writer->endElement();
			}
		}
	}
	
	
	ue_feeds_register_adapter('Streeteasy', '', 'ue_feeds_adapter_streeteasy');