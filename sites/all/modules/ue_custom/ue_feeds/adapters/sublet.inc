<?php
	
	class ue_feeds_adapter_sublet extends ue_feeds_adapter implements ue_feeds_adapter_output
	{
		protected $root;
		
		protected $numListings;
		
		public function feed(&$nodes)
		{
			$this->feed->options->listingcap = intval($this->feed->options->listingcap);
			
			$this->root = new SimpleXMLElement('<feed></feed>');
			$this->root->updated = date('Y-m-d');
			$this->root->name = 'OMG';
			$this->root->Advertiser = '24';
			
			$this->numListings = 0;
			
			foreach($nodes['properties'] as $i => $prop)
			{
				$this->feedProperty($prop, $this->root);
				if($this->feed->options->listingcap > 0 && $this->numListings >= $this->feed->options->listingcap) break;
			}
			
			return $this->root->asXml();
		}
		
		protected function feedProperty($prop, $parent)
		{
			if($prop->listings)
			{
				//check author
				$author = user_load($prop->uid);
				
				if(!$prop->email && !$prop->building_mgr)
				{
					$this->logError("No management company assigned to property.", $prop->id);
					return;
				}
				else if(!$prop->email && !$prop->building_mgr->email && !$prop->building_mgr->syndication_email)
				{
					$this->logError("No email assigned to company.", $prop->building_mgr->id);
					return;
				}
				
				foreach($prop->listings as $i => $listing)
				{
					$node = $parent->addChild('entry');
					//$node->name = $prop->name . ' : ' . $listing->name;
					$node->SupplyId = $listing->id;
					
					$node->Description = '';
					
					if(isset($listing->desc))
						$node->Description = $this->stripHtml($listing->desc) . "\n\n";
					
					if(isset($prop->desc))
						$node->Description .= $this->stripHtml($prop->desc);
					
					$node->Description = trim($node->Description);
					
					$node->Action = 'update';
					$node->premium = $this->feed->options->premium ? 1 : 10;
					
					//Try to fill first/last name
					if($prop->contact_name)
					{
						$name = explode(' ', $prop->contact_name);
						$node->contact->Fname = array_shift($name);
						$node->contact->Lname = implode(' ', $name);
					}
					
					//If no contact name on property or one of first/last is blank, just set to property manager.
					//Sublet doesn't like blanks here.
					if(!$node->contact->Fname || !$node->contact->Lname)
					{
						$node->contact->Fname = 'Property';
						$node->contact->Lname = 'Manager';
					}
					
					if($prop->building_mgr->syndication_email)
						$node->contact->Email = $prop->building_mgr->syndication_email;
					else if($prop->building_mgr->email)
						$node->contact->Email = $prop->building_mgr->email;
					else if($prop->email)
						$node->contact->Email = $prop->email;
					
					$node->contact->SuplrId = $prop->building_mgr->id;
					
					$node->contact->Phone1 = $prop->building_mgr->phone; 
					//$node->contact->Email = $author->mail; // use account email.
					
					$node->contact->Password = 'subletomg';
					$this->logInfo("Added: " . $prop->name . " : " . $listing->name . " for SuplrId " . $prop->building_mgr->id . " account: " . $node->contact->Email . ":" . $node->contact->Password, $listing->id);
					//$node->contact->Phone1 = $prop->phone ? $prop->phone : '';
					//if($prop->phone2) $node->contact->Phone2 = $prop->phone2;
					//$node->contact->Phone3 = '';
					
					$node->contact->URL = url('node/' . $listing->id, array('absolute' => true));
					
					$node->contact->IpAddress = '';
					//$node->contact->IPfwd1 = '';
					//$node->contact->IPfwd2 = '';
					$node->contact->Relation = 3;
					
					$addrparts = explode(' ', $prop->addr1);
					$node->Address->HouseNum = $prop->addr1 ? array_shift($addrparts) : '';
					$node->Address->Unit = $prop->addr1 ? substr($listing->name, 0, 10) : '';
					$node->Address->StreetName = $prop->addr1 ? implode(' ', $addrparts) : '';
					$node->Address->IntersectingStreet = $prop->crossStreet ? $prop->crossStreet : '';
					$node->Address->City = $prop->city;
					$node->Address->StateCode = $prop->state;
					$node->Address->ZipCode = $prop->zip;
					$node->Address->CountryCode = 'USA';
					
					$node->AvailableDate = $listing->dateAvail ? date('Y-m-d', $listing->dateAvail) : date('Y-m-d');
					
					$node->Term = 'Min1';
					
					if($prop->size != 'townhouse')
						$node->PropertyType = 'Apartment';
					else
						$node->PropertyType = 'Townhouse';
					
					$node->Bedrooms = ceil($listing->bed);
					
					$node->Bathrooms = ceil($listing->bath);
					
					if(is_array($listing->price))
						$node->PriceMonth = $listing->price['min'];
					else
						$node->PriceMonth = $listing->price;
					
					$node->SecurityDeposit = '1 month';
					$node->BrokerFee = 'no';
					$node->Furnished = 'no';
					$node->parking = (in_arrayi('garage', $prop->services) || in_arrayi('parking', $prop->amenities)) ? 'yes' : 'no';
		
					//Pets checkboxes.
					if($prop->pets->dogs || $prop->pets->cats)
						$node->Pets = 'yes';
					else
						$node->Pets = 'no';
					
					$node->internet = 'no';
					$node->laundry = in_arrayi('laundry', $prop->services) ? 'yes' : 'no';
					$node->ac = 'yes';
					$node->smokerok = 'yes';
					$node->utilities = in_arrayi('all utilities included', $prop->utilities) ? 'yes' : 'no';
					$node->furnishedroom = 'no';
					$node->doorman = in_arrayi('doorman', $prop->services) ? 'yes' : 'no';
					$node->cable = 'yes';
					$node->elevator = in_arrayi('elevator', $prop->access) ? 'yes' : 'no';
					$node->creditapplication = 'yes';
					
					$node->tenantrenewlease = 'no';
					$node->summerrental = 'no';
					$node->holidayrental = 'no';
					
					if(is_array($listing->sqft))
						$node->sqft = $listing->sqft['max'];
					elseif(floatval($listing->sqft) > 0)
						$node->sqft = $listing->sqft;
					
					$node->postdate = date('Y-m-d', $listing->updated);
					
					$totalImages = 1;
					
					if($listing->floorplans)
					{
						$i = 0;
						while($totalImages <= 10 && $i < count($listing->floorplans))
						{
							$imgName = 'Image' . $totalImages;
							$node->$imgName = str_replace('http://www.urbanedgeny.com/feeds/images/', '', $listing->floorplans[$i]);
							$i++;
							$totalImages++;
						}
					}
				
					if($listing->photos)
					{
						$i = 0;
						while($totalImages <= 10 && $i < count($listing->photos))
						{
							$imgName = 'Image' . $totalImages;
							$node->$imgName = str_replace('http://www.urbanedgeny.com/feeds/images/', '', $listing->photos[$i]);
							$i++;
							$totalImages++;
						}
					}
				
					if($prop->photos)
					{
						$i = 0;
						while($totalImages <= 10 && $i < count($prop->photos))
						{
							$imgName = 'Image' . $totalImages;
							$node->$imgName = str_replace('http://www.urbanedgeny.com/feeds/images/', '', $prop->photos[$i]);
							$i++;
							$totalImages++;
						}
					}
					
					$this->numListings++;
					
					if($this->feed->options->listingcap > 0 && $this->numListings >= $this->feed->options->listingcap)
						return;
				}
			}
		}
		
		public function getForm()
		{
			$form['premium'] = array(
				'#type' => 'checkbox',
				'#title' => t('Premium'),
				'#default_value' => $this->feed->options->premium,
				'#maxlength' => 20,
				'#size' => 10,
				'#description' => t('If checked, all listings will be marked premium.'),
				'#required' => TRUE,
			);
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
			$this->feed->options->premium = $form_state['values']['premium'];
			$this->feed->options->listingcap = $form_state['values']['listingcap'];
		}
	}
	
	ue_feeds_register_adapter('Sublet', '', 'ue_feeds_adapter_sublet');