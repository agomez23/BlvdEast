<?php
	
	class ue_feeds_adapter_mits_stream_realplus extends ue_feeds_adapter implements ue_feeds_adapter_output, ue_feeds_adapter_output_stream
	{
		
		protected $writer;
		protected $companyWriter;
		protected $propWriter;
		protected $fedCompanies = array();
		private $listingCount = 0;
		
		public function feed(&$nodes)
		{
			$this->writer = new XMLWriter();
			
			$this->writer->openMemory();
			$this->writer->startDocument('1.0', 'UTF-8');
			$this->writer->setIndent(true);
			$this->writer->setIndentString("\t");
			
			$this->writer->startElement('PhysicalProperty');
			
			$this->feedCompanies($nodes['properties']); //management companies
			
			foreach($nodes['properties'] as $i => $prop) $this->feedProperty($prop);
			
			$this->writer->endElement();
			$this->writer->endDocument();
			
			return $this->writer->outputMemory(true);
		}
		
		public function feedStream(&$nodes)
		{
			$this->writer = new XMLWriter();
			$this->companyWriter = new XMLWriter();
			$this->propWriter = new XMLWriter();
			
			$this->writer->openMemory();
			$this->companyWriter->openMemory();
			$this->propWriter->openMemory();
			
			$this->writer->setIndent(true);
			$this->writer->setIndentString("\t");
			$this->companyWriter->setIndent(true);
			$this->companyWriter->setIndentString("\t");
			$this->propWriter->setIndent(true);
			$this->propWriter->setIndentString("\t");
			
			$this->writer->startDocument('1.0', 'UTF-8');
			
			$this->writer->startElement('PhysicalProperty');
			
			//loop through property
			//load property
			//feed property
			//feed company if not fed already
			
			foreach($nodes['properties'] as $propID => $listings)
			{
				$prop = $this->feed->filter->getProperty($propID, $listings);
				$this->feedProperty($prop);
				unset($prop);
			}
			
			$this->writer->writeAttribute('listings', $this->listingCount);
			$this->writer->writeAttribute('generated', date('r', time()));
			
			//$this->writer->writeRaw($this->companyWriter->outputMemory(true));
			
			$this->writer->writeRaw($this->propWriter->outputMemory(true));
			
			$this->writer->endElement();
			$this->writer->endDocument();
			
			return $this->writer->outputMemory(true);
		}
		
		protected function feedCompanies(&$properties)
    {
			foreach($properties as $i => $prop)
			{
				if($prop->leasing_mgr) $companies[$prop->leasing_mgr->id] = $prop->leasing_mgr;
				
				if($prop->building_mgr) $companies[$prop->building_mgr->id] = $prop->building_mgr;
				
				if($prop->building_own) $companies[$prop->building_own->id] = $prop->building_own;
			}
			foreach($companies as $i => $comp) $this->feedCompany($comp); 
		}
		
		protected function feedCompany(&$company)
    {
			//if(!in_array($company->id, $this->fedCompanies))
			//{
				$this->companyWriter->startElement('Management');
				
				$this->companyWriter->writeAttribute('Id', $company->id); 
        $this->companyWriter->writeAttribute('companyType', $company->company_type); 
				
				if($company->name) $this->companyWriter->writeElement('Name', $company->name);
				
				$this->writePhone($this->companyWriter, $company->phone);
				
				if($company->email) $this->companyWriter->writeElement('Email', $company->email);
				
        $this->writeAddr($this->companyWriter, $company->addr1, $company->addr2, $company->city, $company->state, $company->zip);

				
        $this->companyWriter->endElement();
        $this->companyWriter->outputMemory(true);
				
        $this->fedCompanies[] = $company->id;

			//}
		}
		
		protected function feedProperty(&$prop)
		{
			//only feed property if it has listings.
			//if(!$prop->listings || count($prop->listings) == 0)
			//{
			//	$this->logError("No active listings, was not included in the feed.", $prop->id);
			//	return;
			//}
			$this->propWriter->startElement('Property');
			
			if($prop->leasing_mgr)
        $company = $prop->leasing_mgr;
        $_company[] = $prop->leasing_mgr;
			
			if($prop->building_mgr)
				$company = $prop->building_mgr;
        $_company[] = $prop->building_mgr;
			
			if($prop->building_own)
				$company = $prop->building_own;
        $_company[] = $prop->building_own;

    //print_r($company);  
			if($company)
      {
        //print_r($company);
       // $total_companies = sizeof($company);
       // for($i=0; $i < $total_companies; $i++) {
				  $this->propWriter->writeAttribute('UE_Building_ID', $prop->id);
          //feed property
          //foreach($company as $comp) {
            //$this->feedCompany($company);
          $this->feedCompany($company);
          foreach($_company as $_comp) {
            if(!empty($_comp->id)) {
              $this->propWriter->startElement('Management');
              $this->propWriter->writeAttribute('ManagementType', $_comp->company_type);
              $this->propWriter->writeElement('CompanyId', $_comp->id);
              $this->propWriter->writeElement('CompanyName', $_comp->name);

              if(!empty($_comp->company_op)) {
                $this->propWriter->writeElement('Company_Showing_Instructions', $_comp->company_op);
              }

              if(!empty($_comp->company_showing_instructions)) {
                $this->propWriter->writeElement('Company_General_Instructions', $_comp->company_showing_instructions);
              }


              $this->propWriter->endElement();
            }
          }
      }

			$this->propWriter->startElement('Identification');
				
      $this->propWriter->writeAttribute('type', 'apartment'); 

			
			//$this->propWriter->writeElement('PrimaryId', $prop->id); 
			$this->propWriter->writeElement('BuildingName', $prop->name);
			 
			$this->writePhone($this->propWriter, $prop->phone);
			
			if($prop->lat) $this->propWriter->writeElement('Latitude', $prop->lat);
			if($prop->lng) $this->propWriter->writeElement('Longitude', $prop->lng);
			if($prop->email) $this->propWriter->writeElement('Email', $prop->email);
			
      $this->writeAddr($this->propWriter, $prop->addr1, $prop->addr2, $prop->city, $prop->state, $prop->zip);

      if($prop->crossStreet) $this->propWriter->writeElement('Cross_Street_1', $prop->crossStreet);
      if($prop->crossStreet2) $this->propWriter->writeElement('Cross_Street_2', $prop->crossStreet2);
			
			//$this->propWriter->writeElement('WebSite', url('node/' . $prop->id, array('absolute' => true)));
			if(!empty($prop->website))
				$this->propWriter->writeElement('WebSite', $prop->website);
			
      $this->propWriter->endElement();

       // } //foreach
			//}
			
			$this->propWriter->startElement('Information');
			
			$this->propWriter->startElement('Info');
			$this->propWriter->writeElement('LongDescription', $this->stripHtml($prop->desc));
			$this->propWriter->endElement();
			
			if($prop->unitCount) $this->propWriter->writeElement('UnitCount', $prop->unitCount);
      if($prop->yearBuilt) $this->propWriter->writeElement('YearBuilt', $prop->yearBuilt);

			if($prop->super) $this->propWriter->writeElement('Super', $prop->super);
      if($prop->super_phone) $this->propWriter->writeElement('Super_Phone', $prop->super_phone);

      if($prop->fee_desc) $this->propWriter->writeElement('List_Describe_All_Fees', $prop->fee_desc);

      if($prop->refundable_fee) $this->propWriter->writeElement('Refundable_Movein_Fee', $prop->refundable_fee);
      if($prop->non_refundable_fee) $this->propWriter->writeElement('Non_Refundable_Movein_Fee', $prop->non_refundable_fee);
      
		 
			//Pets checkboxes.
			if($prop->pets->dogs || $prop->pets->cats)
			{
				$this->propWriter->startElement('Policy');
				
				if($prop->pets->dogs)
				{
					$this->propWriter->startElement('Pet');
					$this->propWriter->writeAttribute('type', 'dogs');
					$this->propWriter->writeAttribute('allowed', 'true');
					$this->propWriter->endElement();
				}
				if($prop->pets->cats)
				{
					$this->propWriter->startElement('Pet');
					$this->propWriter->writeAttribute('type', 'cats');
					$this->propWriter->writeAttribute('allowed', 'true');
					$this->propWriter->endElement();
				}
				
				$this->propWriter->endElement();
			}

      if($prop->building_access) {
				foreach($prop->building_access as $i => $building_access)	{
					$this->propWriter->startElement('Amenities');
					$this->propWriter->writeAttribute('type', 'Building_Access');
					$this->propWriter->writeElement('Feature', $building_access);
					$this->propWriter->endElement();
				}
      }

			if($prop->building_type) {
				foreach($prop->building_type as $i => $building_type)	{
					$this->propWriter->startElement('Amenities');
					$this->propWriter->writeAttribute('type', 'Building_Ownership');
					$this->propWriter->writeElement('Feature', $building_type);
					$this->propWriter->endElement();
				}
			}
			
			if($prop->amenities) {
				foreach($prop->amenities as $i => $amen)	{
					$this->propWriter->startElement('Amenities');
					$this->propWriter->writeAttribute('type', 'community');
					$this->propWriter->writeElement('Feature', $amen);
					$this->propWriter->endElement();
				}
			}
			
			if($prop->services)	{
				foreach($prop->services as $i => $service)	{
					$this->propWriter->startElement('Amenities');
					$this->propWriter->writeAttribute('type', 'Building_Service');
					$this->propWriter->writeElement('Feature', $service);
					$this->propWriter->endElement();
				}
      }
      //additional features
      if($prop->bldg_amenities) {
        foreach($prop->bldg_amenities as $i => $bldg_amenities) {
          $this->propWriter->startElement('Amenities');
          $this->propWriter->writeAttribute('type', 'Additional_Building_Amenities');
          $this->propWriter->writeElement('Feature', $bldg_amenities);
          $this->propWriter->endElement();
        }
      }

      if($prop->laundry) {
        foreach($prop->laundry as $i => $laundry) {
          $this->propWriter->startElement('Amenities');
          $this->propWriter->writeAttribute('type', 'Laundry_Facilities_Storage');
          $this->propWriter->writeElement('Feature', $laundry);
          $this->propWriter->endElement();
        }
      }
      
      
      if($prop->concierge) {
        foreach($prop->concierge as $i => $concierge) {
          $this->propWriter->startElement('Amenities');
          $this->propWriter->writeAttribute('type', 'Concierge_Other_Services');
          $this->propWriter->writeElement('Feature', $concierge);
          $this->propWriter->endElement();
        }
      }
      
      if($prop->healthclub) {
        foreach($prop->healthclub as $i => $healthclub) {
          $this->propWriter->startElement('Amenities');
          $this->propWriter->writeAttribute('type', 'Health_Club_Spa_Features');
          $this->propWriter->writeElement('Feature', $healthclub);
          $this->propWriter->endElement();
        }
      }
      
      if($prop->sports) {
        foreach($prop->sports as $i => $sports) {
          $this->propWriter->startElement('Amenities');
          $this->propWriter->writeAttribute('type', 'Sports_Facilities');
          $this->propWriter->writeElement('Feature', $sports);
          $this->propWriter->endElement();
        }
      }
      
      if($prop->green) {
        foreach($prop->green as $i => $green) {
          $this->propWriter->startElement('Amenities');
          $this->propWriter->writeAttribute('type', 'Green_Building_Detail');
          $this->propWriter->writeElement('Feature', $green);
          $this->propWriter->endElement();
        }
      }
      
      if($prop->outdoor) {
        foreach($prop->outdoor as $i => $outdoor) {
          $this->propWriter->startElement('Amenities');
          $this->propWriter->writeAttribute('type', 'Outdoor_Space');
          $this->propWriter->writeElement('Feature', $outdoor);
          $this->propWriter->endElement();
        }
      }
      
      if($prop->parking_opts) {
        foreach($prop->parking_opts as $i => $parking_opts) {
          $this->propWriter->startElement('Amenities');
          $this->propWriter->writeAttribute('type', 'Parking_Garage_Options');
          $this->propWriter->writeElement('Feature', $parking_opts);
          $this->propWriter->endElement();
        }
      }
      
      if($prop->security_maint) {
        foreach($prop->security_maint as $i => $security_maint) {
          $this->propWriter->startElement('Amenities');
          $this->propWriter->writeAttribute('type', 'Security_Maintenance');
          $this->propWriter->writeElement('Feature', $security_maint);
          $this->propWriter->endElement();
        }
      }
      
      if($prop->bldg_doorman) {
        foreach($prop->bldg_doorman as $i => $bldg_doorman) {
          $this->propWriter->startElement('Amenities');
          $this->propWriter->writeAttribute('type', 'Doorman_Attended_Lobby');
          $this->propWriter->writeElement('Feature', $bldg_doorman);
          $this->propWriter->endElement();
        }
      }

      if($prop->incentives) {
        foreach($prop->incentives as $i => $incentives) {
          $this->propWriter->startElement('Amenities');
          $this->propWriter->writeAttribute('type', 'Complex_Building_Wide_Incentives');
          $this->propWriter->writeElement('Feature', $incentives);
          $this->propWriter->endElement();
        }
      }

      if($prop->nbrhd_details) {
        foreach($prop->nbrhd_details as $i => $nbrhd_details) {
          $this->propWriter->startElement('Amenities');
          $this->propWriter->writeAttribute('type', 'Street_Features');
          $this->propWriter->writeElement('Feature', $nbrhd_details);
          $this->propWriter->endElement();
        }
      }

      if($prop->walking_distance)  {
        foreach($prop->walking_distance as $i => $walking_distance) {
          $this->propWriter->startElement('Amenities');
          $this->propWriter->writeAttribute('type', 'Walking_Distance');
          $this->propWriter->writeElement('Feature', $walking_distance);
          $this->propWriter->endElement();
        }
      }

      if($prop->nearby_trans) {
        foreach($prop->nearby_trans as $i => $nearby_trans) {
          $this->propWriter->startElement('Amenities');
          $this->propWriter->writeAttribute('type', 'Nearby_Transportation');
          $this->propWriter->writeElement('Feature', $nearby_trans);
          $this->propWriter->endElement();
        }
      }

      if($prop->trans_distance) {
        foreach($prop->trans_distance as $i => $trans_distance) {
          $this->propWriter->startElement('Amenities');
          $this->propWriter->writeAttribute('type', 'Walking_Distance_to_Transportation');
          $this->propWriter->writeElement('Feature', $trans_distance);
          $this->propWriter->endElement();
        }
      }
      
      if($prop->income_required_tenant)  {
        foreach($prop->income_required_tenant as $i => $income_required_tenant) {
          $this->propWriter->startElement('Amenities');
          $this->propWriter->writeAttribute('type', 'Income_Requirements_Tenants');
          $this->propWriter->writeElement('Feature', $income_required_tenant);
          $this->propWriter->endElement();
        }
      }

      if($prop->rental_requirements) {
        foreach($prop->rental_requirements as $i => $rental_requirements)  {
          $this->propWriter->startElement('Amenities');
          $this->propWriter->writeAttribute('type', 'Renter_Requirements');
          $this->propWriter->writeElement('Feature', $rental_requirements);
          $this->propWriter->endElement();
        }
      }

      if($prop->income_required_guar)  {
        foreach($prop->income_required_guar as $i => $income_required_guar)  {
          $this->propWriter->startElement('Amenities');
          $this->propWriter->writeAttribute('type', 'Income_Requirements_Guarantors_Cosigners');
          $this->propWriter->writeElement('Feature', $income_required_guar);
          $this->propWriter->endElement();
        }
      }

      if($prop->cosigner)  {
        foreach($prop->cosigner as $i => $cosigner)  {
          $this->propWriter->startElement('Amenities');
          $this->propWriter->writeAttribute('type', 'Guarantor_Cosigner_Information');
          $this->propWriter->writeElement('Feature', $cosigner);
          $this->propWriter->endElement();
        }
      }

      if($prop->deposit) {
        foreach($prop->deposit as $i => $deposit)  {
          $this->propWriter->startElement('Amenities');
          $this->propWriter->writeAttribute('type', 'Deposit_and_Lease_Signing_Information');
          $this->propWriter->writeElement('Feature', $deposit);
          $this->propWriter->endElement();
        }
      }
      
      if($prop->refundable_movein_fee) {
        foreach($prop->refundable_movein_fee as $i => $refundable_movein_fee)  {
          $this->propWriter->startElement('Amenities');
          $this->propWriter->writeAttribute('type', 'Refundable_Movein_Fee');
          $this->propWriter->writeElement('Feature', $refundable_movein_fee);
          $this->propWriter->endElement();
        }
      }

      if($prop->nonrefund_movein_fee) {
        foreach($prop->nonrefund_movein_fee as $i => $nonrefund_movein_fee)  {
          $this->propWriter->startElement('Amenities');
          $this->propWriter->writeAttribute('type', 'Non_Refundable_Movein_Fee');
          $this->propWriter->writeElement('Feature', $nonrefund_movein_fee);
          $this->propWriter->endElement();
        }
      }

      if($prop->movein_details) {
        foreach($prop->movein_details as $i => $movein_details) {
          $this->propWriter->startElement('Amenities');
          $this->propWriter->writeAttribute('type', 'Movein_Details');
          $this->propWriter->writeElement('Feature', $movein_details);
          $this->propWriter->endElement();
        }
      }
      
      if($prop->incentivesText) {
        $this->propWriter->startElement('Other_Incentives');
        $this->propWriter->writeElement('Feature', $this->stripHtml($prop->incentivesText));
        $this->propWriter->endElement();
      }

      

      //
			if($prop->photos)
				$this->feedAddFiles($this->propWriter, $prop->photos, 'photo');
			
			$this->propWriter->endElement();
				
			if($prop->showing_instructions)
				$this->propWriter->writeElement('ShowingInstructions', $prop->showing_instructions);
			
			if($prop->op)
        $this->propWriter->writeElement('OP', $prop->op);
     
     
			
			if($prop->furnished)
				$this->propWriter->writeElement('Furnished', $prop->furnished);
				
			$this->propWriter->writeElement('Neighborhood', $prop->hoodName);
			$this->propWriter->writeElement('NeighborhoodDescription', $prop->hoodDesc);
			
			if(is_array($prop->type))
			{
				foreach($prop->type as $type)
					$this->propWriter->writeElement('BuildingType', $type);
			}
			
			if(isset($prop->openhouses))
				$this->writeOpenHouses($this->propWriter, $prop->openhouses);
			
			if($prop->listings)
				$this->feedListings($prop->listings);
			
			$this->propWriter->endElement(); 
		}
		
		protected function feedListings(&$listings)
		{
			foreach($listings as $i => $listing)
      {
        //print_r($listing);
				$this->propWriter->startElement('Floorplan');
				
				$this->propWriter->writeAttribute('UE_Unit_Id', $listing->id);
				
				//realplus needs unit numbers fed to them.
				$this->propWriter->writeElement('Name', $listing->unit_num);
				
				$this->propWriter->startElement('Room');
				$this->propWriter->writeAttribute('type', 'bedroom');
				$this->propWriter->writeElement('Count', $listing->bed);
				$this->propWriter->writeElement('BedroomDescription', $listing->bed_desc);
				$this->propWriter->writeElement('Comment', 'Bedrooms');
				$this->propWriter->endElement();
				
				$this->propWriter->startElement('Room');
				$this->propWriter->writeAttribute('type', 'bathroom');
				$this->propWriter->writeElement('Count', $listing->bath);
				$this->propWriter->writeElement('Comment', 'Bathrooms');
        $this->propWriter->endElement();

        if($listing->num_closets) {
          $this->propWriter->startElement('Total_Closets');
          $this->propWriter->writeElement('Count', $listing->num_closets);
          $this->propWriter->endElement();
        }

        if($listing->walkin_closets) {
          $this->propWriter->startElement('Walkin_Closets');
          $this->propWriter->writeElement('value', $listing->walkin_closets);
          $this->propWriter->endElement(); 
        }


        /* ===== */
        if($listing->features) {
          foreach($listing->features as $i => $features) {
            $this->propWriter->startElement('Amenities');
            $this->propWriter->writeAttribute('type', 'Features');
            $this->propWriter->writeElement('Feature', $features);
            $this->propWriter->endElement();
          }
        }
      if($listing->windows) {
        foreach($listing->windows as $i => $windows) {
          $this->propWriter->startElement('Amenities');
          $this->propWriter->writeAttribute('type', 'Windows');
          $this->propWriter->writeElement('Feature', $windows);
          $this->propWriter->endElement();
        }
      }

      if($listing->closets) {
        foreach($listing->closets as $i => $closets) {
          $this->propWriter->startElement('Amenities');
          $this->propWriter->writeAttribute('type', 'Closets_Storage_Laundry');
          $this->propWriter->writeElement('Feature', $closets);
          $this->propWriter->endElement();
        }
      }

      if($listing->features) {
        foreach($listing->features as $i => $features) {
          $this->propWriter->startElement('Amenities');
          $this->propWriter->writeAttribute('type', 'More_Features');
          $this->propWriter->writeElement('Feature', $features);
          $this->propWriter->endElement();
        }
      }

      if($listing->floorceiling) {
        foreach($listing->floorceiling as $i => $floorceiling) {
          $this->propWriter->startElement('Amenities');
          $this->propWriter->writeAttribute('type', 'Floor_and_Ceiling');
          $this->propWriter->writeElement('Feature', $floorceiling);
          $this->propWriter->endElement();
        }
      }      

      if($listing->addl_rooms) {
        foreach($listing->addl_rooms as $i => $addl_rooms) {
          $this->propWriter->startElement('Amenities');
          $this->propWriter->writeAttribute('type', 'Additional_Rooms');
          $this->propWriter->writeElement('Feature', $addl_rooms);
          $this->propWriter->endElement();
        }
      }

      if($listing->views_adv) {
        foreach($listing->views_adv as $i => $views_adv) {
          $this->propWriter->startElement('Amenities');
          $this->propWriter->writeAttribute('type', 'Views_Exposures');
          $this->propWriter->writeElement('Feature', $views_adv);
          $this->propWriter->endElement();
        }
      }

      if($listing->utilities) {
        foreach($listing->utilities as $i => $utilities) {
          $this->propWriter->startElement('Amenities');
          $this->propWriter->writeAttribute('type', 'Utilities');
          $this->propWriter->writeElement('Feature', $utilities);
          $this->propWriter->endElement();
        }
      }

      if($listing->hvac) {
        foreach($listing->hvac as $i => $hvac) {
          $this->propWriter->startElement('Amenities');
          $this->propWriter->writeAttribute('type', 'Heating_Cooling_Systems');
          $this->propWriter->writeElement('Feature', $hvac);
          $this->propWriter->endElement();
        }
      }

      if($listing->archit_det) {
        foreach($listing->archit_det as $i => $archit_det) {
          $this->propWriter->startElement('Amenities');
          $this->propWriter->writeAttribute('type', 'Architectural_Details');
          $this->propWriter->writeElement('Feature', $archit_det);
          $this->propWriter->endElement();
        }
      }
      
      if($listing->lease_type_adv) {
        print_r($listing->lease_type_adv);
        foreach($listing->lease_type_adv as $i => $lease_type_adv) {
          $this->propWriter->startElement('Amenities');
          $this->propWriter->writeAttribute('type', 'Special_Lease_Type');
          $this->propWriter->writeElement('Feature', $lease_type_adv);
          $this->propWriter->endElement();
        }
      }      
 
       if($listing->accessible) {
        foreach($listing->accessible as $i => $accessible) {
          $this->propWriter->startElement('Amenities');
          $this->propWriter->writeAttribute('type', 'Accessible');
          $this->propWriter->writeElement('Feature', $accessible);
          $this->propWriter->endElement();
        }
      }      

       if($listing->kitchen_type) {
        foreach($listing->kitchen_type as $i => $kitchen_type) {
          $this->propWriter->startElement('Amenities');
          $this->propWriter->writeAttribute('type', 'Kitchen Type');
          $this->propWriter->writeElement('Feature', $kitchen_type);
          $this->propWriter->endElement();
        }
      }      

       if($listing->kitchen_features) {
        foreach($listing->kitchen_features as $i => $kitchen_features) {
          $this->propWriter->startElement('Amenities');
          $this->propWriter->writeAttribute('type', 'Additional_Kitchen_Feature');
          $this->propWriter->writeElement('Feature', $kitchen_features);
          $this->propWriter->endElement();
        }
      }      

       if($listing->cabinet) {
        foreach($listing->cabinet as $i => $cabinet) {
          $this->propWriter->startElement('Amenities');
          $this->propWriter->writeAttribute('type', 'Cabinet_Type');
          $this->propWriter->writeElement('Feature', $cabinet);
          $this->propWriter->endElement();
        }
      }      

       if($listing->countertops) {
        foreach($listing->countertops as $i => $countertops) {
          $this->propWriter->startElement('Amenities');
          $this->propWriter->writeAttribute('type', 'Countertop_Type');
          $this->propWriter->writeElement('Feature', $countertops);
          $this->propWriter->endElement();
        }
      }      

       if($listing->backsplash) {
        foreach($listing->backsplash as $i => $backsplash) {
          $this->propWriter->startElement('Amenities');
          $this->propWriter->writeAttribute('type', 'Backsplash_Type');
          $this->propWriter->writeElement('Feature', $backsplash);
          $this->propWriter->endElement();
        }
      }      

       if($listing->refridge) {
        foreach($listing->refridge as $i => $refridge) {
          $this->propWriter->startElement('Amenities');
          $this->propWriter->writeAttribute('type', 'Refrigerator');
          $this->propWriter->writeElement('Feature', $refridge);
          $this->propWriter->endElement();
        }
      }      

       if($listing->oven) {
        foreach($listing->oven as $i => $oven) {
          $this->propWriter->startElement('Amenities');
          $this->propWriter->writeAttribute('type', 'Stove_Oven');
          $this->propWriter->writeElement('Feature', $oven);
          $this->propWriter->endElement();
        }
      }      

       if($listing->sink) {
        foreach($listing->sink as $i => $sink) {
          $this->propWriter->startElement('Amenities');
          $this->propWriter->writeAttribute('type', 'Sink_Fixtures');
          $this->propWriter->writeElement('Feature', $sink);
          $this->propWriter->endElement();
        }
      }      

       if($listing->kitchen_floors) {
        foreach($listing->kitchen_floors as $i => $kitchen_floors) {
          $this->propWriter->startElement('Amenities');
          $this->propWriter->writeAttribute('type', 'Kitchen_Floor');
          $this->propWriter->writeElement('Feature', $kitchen_floors);
          $this->propWriter->endElement();
        }
      }      

       if($listing->spec_kitchen) {
        foreach($listing->spec_kitchen as $i => $spec_kitchen) {
          $this->propWriter->startElement('Amenities');
          $this->propWriter->writeAttribute('type', 'Speciality_Kitchen_Item');
          $this->propWriter->writeElement('Feature', $spec_kitchen);
          $this->propWriter->endElement();
        }
      }      

       if($listing->bath_features) {
        foreach($listing->bath_features as $i => $bath_features) {
          $this->propWriter->startElement('Amenities');
          $this->propWriter->writeAttribute('type', 'More_Bath_Features');
          $this->propWriter->writeElement('Feature', $bath_features);
          $this->propWriter->endElement();
        }
      }      

       if($listing->bath_floor) {
        foreach($listing->bath_floor as $i => $bath_floor) {
          $this->propWriter->startElement('Amenities');
          $this->propWriter->writeAttribute('type', 'Wall_Floor_Tiles');
          $this->propWriter->writeElement('Feature', $bath_floor);
          $this->propWriter->endElement();
        }
      }      

       if($listing->bath_sink) {
        foreach($listing->bath_sink as $i => $bath_sink) {
          $this->propWriter->startElement('Amenities');
          $this->propWriter->writeAttribute('type', 'Sink_Vanity_Types');
          $this->propWriter->writeElement('Feature', $bath_sink);
          $this->propWriter->endElement();
        }
      }      

       if($listing->luxe_features) {
        foreach($listing->luxe_features as $i => $luxe_features) {
          $this->propWriter->startElement('Amenities');
          $this->propWriter->writeAttribute('type', 'Luxury Features');
          $this->propWriter->writeElement('Feature', $luxe_features);
          $this->propWriter->endElement();
        }
      } 
      /*if($listing->furnished) {
        foreach($listing->furnished as $i => $furnished) {
          $this->propWriter->startElement('Amenities');
          $this->propWriter->writeAttribute('type', 'Furnished');
          $this->propWriter->writeElement('Feature', $furnished);
          $this->propWriter->endElement();
        }
      }*/

      if($listing->kitchen_condition) {
        foreach($listing->kitchen_condition as $i => $kitchen_condition) {
          $this->propWriter->startElement('Amenities');
          $this->propWriter->writeAttribute('type', 'Kitchen_Condition');
          $this->propWriter->writeElement('Feature', $kitchen_condition);
          $this->propWriter->endElement();
        }
      }
      if($listing->kitchen_amenities) {
        foreach($listing->kitchen_amenities as $i => $kitchen_amenities) {
          $this->propWriter->startElement('Amenities');
          $this->propWriter->writeAttribute('type', 'Kitchen_Features');
          $this->propWriter->writeElement('Feature', $kitchen_amenities);
          $this->propWriter->endElement();
        }
      }
      if($listing->kitchen_appliances) {
        foreach($listing->kitchen_appliances as $i => $kitchen_appliances) {
          $this->propWriter->startElement('Amenities');
          $this->propWriter->writeAttribute('type', 'Kitchen_Appliances');
          $this->propWriter->writeElement('Feature', $kitchen_appliances);
          $this->propWriter->endElement();
        }
      }
      if($listing->bathroom_condition) {
        foreach($listing->bathroom_condition as $i => $bathroom_condition) {
          $this->propWriter->startElement('Amenities');
          $this->propWriter->writeAttribute('type', 'Bathroom_Condition');
          $this->propWriter->writeElement('Feature', $bathroom_condition);
          $this->propWriter->endElement();
        }
      }
      if($listing->bathroom_features) {
        foreach($listing->bathroom_features as $i => $bathroom_features) {
          $this->propWriter->startElement('Amenities');
          $this->propWriter->writeAttribute('type', 'Bathroom_Features');
          $this->propWriter->writeElement('Feature', $bathroom_features);
          $this->propWriter->endElement();
        }
      }
      if($listing->apt_type) {
        foreach($listing->apt_type as $i => $apt_type) {
          $this->propWriter->startElement('Amenities');
          $this->propWriter->writeAttribute('type', 'Apt_Type_Style');
          $this->propWriter->writeElement('Feature', $apt_type);
          $this->propWriter->endElement();
        }
      }
      if($listing->unit_type) {
        foreach($listing->unit_type as $i => $unit_type) {
          $this->propWriter->startElement('Amenities');
          $this->propWriter->writeAttribute('type', 'Special_Apt_Type');
          $this->propWriter->writeElement('Feature', $unit_type);
          $this->propWriter->endElement();
        }
      }
      if($listing->outdoor) {
        foreach($listing->outdoor as $i => $outdoor) {
          $this->propWriter->startElement('Amenities');
          $this->propWriter->writeAttribute('type', 'Outdoor_Space');
          $this->propWriter->writeElement('Feature', $outdoor);
          $this->propWriter->endElement();
        }
      }
      if($listing->unit_views) {
        foreach($listing->unit_views as $i => $unit_views) {
          $this->propWriter->startElement('Amenities');
          $this->propWriter->writeAttribute('type', 'Views');
          $this->propWriter->writeElement('Feature', $unit_views);
          $this->propWriter->endElement();
        }
      }     
     /* ======= */
				
				if($listing->dateAvail)
				{
					$this->propWriter->startElement('DateAvailable');
					$this->propWriter->writeAttribute('month', date('n', $listing->dateAvail));
					$this->propWriter->writeAttribute('day', date('j', $listing->dateAvail));
					$this->propWriter->writeAttribute('year', date('Y', $listing->dateAvail));
					$this->propWriter->endElement(); 
				}
				
				if($listing->updated)
				{
					$this->propWriter->startElement('DateUpdated');
					$this->propWriter->writeAttribute('month', date('n', $listing->updated));
					$this->propWriter->writeAttribute('day', date('j', $listing->updated));
					$this->propWriter->writeAttribute('year', date('Y', $listing->updated));
					$this->propWriter->endElement(); 
				}
				
				if(isset($listing->desc))
					$this->propWriter->writeElement('UnitDescription', $this->stripHtml($listing->desc));
				
				if($listing->showing_instructions)
					$this->propWriter->writeElement('ShowingInstructions', $listing->showing_instructions);
				
				if($listing->op)
					$this->propWriter->writeElement('OP', $listing->op);
				
				if($listing->furnished)
					$this->propWriter->writeElement('Furnished', $listing->furnished);
				
				if($listing->lease_term)
					$this->propWriter->writeElement('LeaseTerm', $listing->lease_term);
			  //print_r($listing->lease_type);	
        if($listing->lease_type) {
          $this->propWriter->writeElement('LeaseType', $listing->lease_type);
        }
				else {
          $this->propWriter->writeElement('LeaseType', 'Market Rate');
        }
				
        if($listing->condition) {
          $this->propWriter->writeElement('Overall_Condition', $listing->condition[0]);
        }
				
				if($prop->furnished)
					$this->propWriter->writeElement('Furnished', $prop->furnished);
			
				if(isset($listing->openhouses))
					$this->writeOpenHouses($this->propWriter, $listing->openhouses);
				
				$this->propWriter->startElement('Rent');
				
				if(is_array($listing->price))
				{
					$this->propWriter->writeAttribute('min', $listing->price['min']);
					$this->propWriter->writeAttribute('max', $listing->price['max']);
				}
				elseif($listing->price > 0)
				{
					$this->propWriter->writeAttribute('min', $listing->price);
					$this->propWriter->writeAttribute('max', $listing->price);
				}
				
				if(isset($listing->starting_price) && $listing->starting_price)
				{
					$this->propWriter->writeAttribute('StartingPrice', $listing->starting_price);
				}
				
        $this->propWriter->endElement();

        $this->propWriter->startElement('Household_Income');
        $this->propWriter->writeElement('Min', $listing->incomerange_min);
        $this->propWriter->writeElement('Max', $listing->incomerange_max);
        $this->propWriter->endElement();
				
				if(is_array($listing->sqft))
				{
					$this->propWriter->startElement('SquareFeet');
					$this->propWriter->writeAttribute('Min', $listing->sqft['min']);
					$this->propWriter->writeAttribute('Max', $listing->sqft['max']);
					$this->propWriter->endElement();
				}
				elseif($listing->sqft > 0)
				{
					$this->propWriter->writeElement('SquareFeet', $listing->sqft);
				}
			
				if($listing->photos)
					$this->feedAddFiles($this->propWriter, $listing->photos, 'photo');
			
				if($listing->floorplans)
					$this->feedAddFiles($this->propWriter, $listing->floorplans, 'floorplan');
				
				$this->listingCount++;
				
				$this->propWriter->endElement(); 
			}
		}
		
		protected function writePhone(&$writer, $phone)
		{
			if($phone)
			{
				$writer->startElement('Phone');
				$writer->writeElement('Number', $phone);
				$writer->endElement(); 
			}
		}
		
		protected function writeAddr(&$writer, $addr1, $addr2, $city, $state, $zip)
		{
			if($addr1 || $addr2 || $city || $state || $zip)
			{
				$writer->startElement('Address');
				if($addr1) $writer->writeElement('Address1', $addr1);
				if($addr2) $writer->writeElement('Address2', $addr2);
				if($city) $writer->writeElement('City', $city);
				if($state) $writer->writeElement('State', $state);
				if($zip) $writer->writeElement('Zip', $zip);
				$writer->endElement(); 
			}
		}
		
		protected function writeOpenHouses(&$writer, &$openhouses)
		{
			foreach($openhouses as $openhouse) {
				$writer->startElement('OpenHouse');
				$writer->writeAttribute('StartTime', $openhouse['start']);
				$writer->writeAttribute('EndTime', $openhouse['end']);
				$writer->endElement();
			}
		}
		
		protected function feedAddFiles(&$writer, &$files, $type)
		{
			foreach($files as $i => $file)
			{
				$info = pathinfo($file);
				$writer->startElement('File');
				$writer->writeAttribute('id', $info['basename']);
				$writer->writeAttribute('active', 'true');
				$writer->writeElement('Type', $type);
				$writer->writeElement('Name', $info['basename']);
				$writer->writeElement('Format', $info['extension']);
				$writer->writeElement('Src', $file);
				$writer->endElement(); 
			}
		}
		
		protected function encode($str)
		{
			return mb_convert_encoding($str, 'UTF-8');
		}
	}
	
	ue_feeds_register_adapter('MITS - RealPlus', 'Mits adapter.', 'ue_feeds_adapter_mits_stream_realplus');
