<?php
	class NYTimes
	{
		const CodeListingType_Sales = 'S';
		const CodeListingType_Rentals = 'R';
		
		const CodeAddlListingType_New = 1;
		const CodeAddlListingType_Luxury = 2;
		const CodeAddlListingType_Vaca = 4;
		const CodeAddlListingType_Retirement = 8;
		const CodeAddlListingType_OpenHouse = 16;
		
		const CodePets_Allowed = 1;
		const CodePets_Cats = 2;
		const CodePets_None = 3;
		const CodePets_SmDogs = 4;
		
		const	CodePopFeature_Fireplace	=	1;
		const	CodePopFeature_Pool	=	2;
		const	CodePopFeature_Deck	=	3;
		const	CodePopFeature_DiningRm	=	4;
		const	CodePopFeature_Balcony	=	5;
		const	CodePopFeature_Doorman	=	6;
		const	CodePopFeature_Elevator	=	7;
		const	CodePopFeature_FitnessCtr	=	8;
		const	CodePopFeature_Furnished	=	9;
		const	CodePopFeature_GardenPatio	=	10;
		const	CodePopFeature_LiveWork	=	11;
		const	CodePopFeature_Loft	=	12;
		const	CodePopFeature_MultiLvl	=	13;
		const	CodePopFeature_PreWar	=	14;
		const	CodePopFeature_Terrace	=	15;
		const	CodePopFeature_WashDryBldg	=	16;
		const	CodePopFeature_WashDryUnit	=	17;
		const	CodePopFeature_Alarm	=	18;
		const	CodePopFeature_Cable	=	19;
		const	CodePopFeature_Internet	=	20;
		const	CodePopFeature_Green	=	21;
		const	CodePopFeature_Carpeted	=	22;
		const	CodePopFeature_Dishwasher	=	23;
		const	CodePopFeature_GarbageDisposal	=	24;
		const	CodePopFeature_HardwoodFloors	=	25;
		const	CodePopFeature_Wheelchair	=	26;
		const	CodePopFeature_SpaJacuzzi	=	27;
		const	CodePopFeature_NoFee	=	28;
		const	CodePopFeature_OutdoorSpace	=	29;
		const	CodePopFeature_New	=	30;
		const	CodePopFeature_Investment	=	31;

	}
	class ue_feeds_adapter_nytimes extends ue_feeds_adapter implements ue_feeds_adapter_output
	{
		protected $sitename;
		public function feed(&$nodes)
		{
			$colnames = array('IdWeb', 'IdAccount', 'Address', 'AddrDisplay', 'AddrCrossStreet', 'AddrCity', 'AddrState', 'AddrZip', 'AddrAptId', 'Price', 'Bedroom', 'BathsFull', 'BathsPartial', 'BuiltYear', 'SqFeetInterior', 'BuildingName', 'BuildingUnitsTotal', 'CommentsLong', 'AdvertiserHomePageURL', 'AdvertiserListingURL', 'AdvertiserName', 'Agent1Name', 'Agent1Email', 'CodeListingType', 'CodePets', 'CodePopularFeature', 'CodeParking');//, 'AdvertiserLogo'
			
			for($i = 1; $i <= 20; $i++) $colnames[] = 'Photo' . $i;
			for($i = 1; $i <= 5; $i++) $colnames[] = 'Floorplan' . $i;
			
			$this->sitename = variable_get('site_name', 'Urban Edge');
			
			$this->addrWarnings = array();
			$this->descWarnings = array();
			
			foreach($nodes['properties'] as $i => $prop)
				if(is_array($prop->listings))
					foreach($prop->listings as $i => $listing)
						$rows[] = $this->feedListing($prop, $listing, $root);
			
			$rtn = $this->arrayToRow($colnames);
			
			foreach($rows as $row)
				$rtn .= $this->arrayToRow($row);
			
			return $rtn;
		}
		
		private function arrayToRow($array)
		{
			return implode('|', $array) . "\n";
		}
		
		private function feedListing($prop, $listing, $parent)
		{
			global $base_url;
			
			$row[] = $listing->id; //IdWeb
			$row[] = $this->feed->options->accountID ? $this->feed->options->accountID : ''; //IdAccount
			$row[] = $prop->addr1; //Address
			$row[] = $prop->addr1; //AddrDisplay
			$row[] = $prop->crossStreet ? $prop->crossStreet : ''; //AddrCrossStreet
			$row[] = $prop->city; //AddrCity
			$row[] = $prop->state; //AddrState
			$row[] = $prop->zip; //AddrZip
			$row[] = $listing->name; //AddrAptId
			$row[] = is_array($listing->price) ? $listing->price['min'] : $listing->price; //Price
			$row[] = intval($listing->bed); //Bedroom
			$row[] = intval( $listing->fullBath); //BathsFull
			$row[] = intval($listing->halfBath); //BathsPartial
			$row[] = $prop->yearBuilt ? $prop->yearBuilt : ''; //BuiltYear
			if(is_array($listing->sqft))
				$row[] = $listing->sqft['max']; //SqFeetInterior
			elseif($listing->sqft)
				$row[] = $listing->sqft; //SqFeetInterior
			else
				$row[] = ''; //SqFeetInterior
			$row[] = $prop->name; //BuildingName
			$row[] = $prop->unitCount ? $prop->unitCount : ''; //BuildingUnitsTotal
			
			$desc = '';
			
			if(isset($listing->desc) && strlen($listing->desc) > 15)
			{
				$desc = $listing->desc;
				
				if(strlen($listing->desc) > 15)
					$desc .= '<br><hr><br>';
			}
			
			$desc .= $prop->desc;
			
			$row[] = preg_replace('[\r\n]', '', $desc);
			
			$row[] = $base_url;
			$row[] = url('node/' . $listing->id, array('absolute' => true)); //AdvertiserListingURL
			//$row[] = 'AdvertiserLogo';
			$row[] = $this->sitename;
			$row[] = $prop->contact_name; //Agent1Name
			$row[] = $prop->email; //Agent1Email
			$row[] = 'R'; //CodeListingType
		
			if($prop->pets->dogs && $prop->pets->cats)
				$row[] = 1; //CodePets
			elseif($prop->pets->dogs)
				$row[] = 4; //CodePets
			elseif($prop->pets->cats)
				$row[] = 2; //CodePets
			else
				$row[] = '';
			
			$features[] = 28; //no fee
			
			if(in_arrayi('fireplace', $listing->features)) $features[] = 1;
			if(in_arrayi('pool', $prop->amenities)) $features[] = 2;
			if(in_arrayi('deck', $prop->amenities)) $features[] = 3;
			if(in_arrayi('doorman', $prop->services)) $features[] = 6;
			if(in_arrayi('elevator', $prop->access)) $features[] = 7;
			if(in_arrayi('fitness center', $prop->amenities)) $features[] = 8;
			if(in_arrayi('garden', $prop->amenities)) $features[] = 10;
			if(in_arrayi('laundry', $prop->services)) $features[] = 16;
			if(in_arrayi('washer/dryer in unit', $listing->features)) $features[] = 17;
			if(in_arrayi('green building', $prop->type)) $features[] = 21;
			if(in_arrayi('carpeting', $listing->features)) $features[] = 22;
			if(in_arrayi('hardwood floors', $listing->features)) $features[] = 25;
			
			$row[] = implode(',', $features); //CodePopularFeature
			
			$row[] = in_arrayi('garage', $prop->services) || in_arrayi('garage', $prop->parking) ? 1 : ''; //CodeParking
			
			for($i = 0; $i < min(array(20, count($listing->photos))); $i++)
				$row[] = $listing->photos[$i] ? $listing->photos[$i] : '';
				
			for($j = 0; $j < 20 - $i; $j++)
				$row[] = $prop->photos[$j] ? $prop->photos[$j] : '';
			
			for($i = 0; $i < 5; $i++)
				$row[] = $listing->floorplans[$i] ? $listing->floorplans[$i] : '';
			
			return $row;
		}
		
		//totalImages is running count, maxImages is max per listing.
		private function feedAddFiles($files, $type, $parent, $totalImages, $maxImages)
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
		
		public function getForm()
		{
			$form['accountID'] = array(
				'#type' => 'textfield',
				'#title' => t('Account ID'),
				'#default_value' => $this->feed->options->accountID,
				'#maxlength' => 20,
				'#size' => 10,
				'#description' => t('Account ID given by NYTimes.'),
				'#required' => TRUE,
			);
			return $form;
		}
		
		public function processForm($form_state)
		{
			$this->feed->options->accountID = $form_state['values']['accountID'];
		}
	}
	
	ue_feeds_register_adapter('NYTimes', 'NYTimes adapter.', 'ue_feeds_adapter_nytimes');