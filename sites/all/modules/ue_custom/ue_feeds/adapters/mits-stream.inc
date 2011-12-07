<?php
	
	class ue_feeds_adapter_mits_stream extends ue_feeds_adapter implements ue_feeds_adapter_output, ue_feeds_adapter_output_stream
	{
		
		protected $writer;
		protected $companyWriter;
		protected $propWriter;
		protected $fedCompanies = array();
		
		public function feed(&$nodes)
		{
			$this->writer = new XMLWriter();
			
			$this->writer->openMemory();
			$this->writer->startDocument('1.0', 'UTF-8');
			$this->writer->setIndent(true);
			$this->writer->setIndentString("\t");
			
			$this->writer->startElement('PhysicalProperty');
			
			$this->feedCompanies($nodes['properties']);
			
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
			
			$this->writer->writeRaw($this->companyWriter->outputMemory(true));
			
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
			if(!in_array($company->id, $this->fedCompanies))
			{
				$this->companyWriter->startElement('Management');
				
				$this->companyWriter->writeAttribute('Id', $company->id); 
				
				if($company->name) $this->companyWriter->writeElement('Name', $company->name);
				
				$this->writePhone($this->companyWriter, $company->phone);
				
				if($company->email) $this->companyWriter->writeElement('Email', $company->email);
				
				$this->writeAddr($this->companyWriter, $company->addr1, $company->addr2, $company->city, $company->state, $company->zip);
				
				$this->companyWriter->endElement();
				
				$this->fedCompanies[] = $company->id;
			}
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
			
			if($prop->building_mgr)
				$company = $prop->building_mgr;
			
			if($prop->building_own)
				$company = $prop->building_own;
			
			if($company)
			{
				$this->propWriter->writeAttribute('managementID', $company->id);
				//feed property
				$this->feedCompany($company);
			}
			
			$this->propWriter->startElement('Identification');
				
			$this->propWriter->writeAttribute('type', 'apartment'); 
			
			$this->propWriter->writeElement('PrimaryId', $prop->id); 
			$this->propWriter->writeElement('MarketingName', $prop->name);
			 
			$this->writePhone($this->propWriter, $prop->phone);
			
			if($prop->lat) $this->propWriter->writeElement('Latitude', $prop->lat);
			if($prop->lng) $this->propWriter->writeElement('Longitude', $prop->lng);
			if($prop->email) $this->propWriter->writeElement('Email', $prop->email);
			
			$this->writeAddr($this->propWriter, $prop->addr1, $prop->addr2, $prop->city, $prop->state, $prop->zip);
			
			$this->propWriter->writeElement('WebSite', url('node/' . $prop->id, array('absolute' => true)));
			
			$this->propWriter->endElement();
			
			$this->propWriter->startElement('Information');
			
			$this->propWriter->startElement('Info');
			$this->propWriter->writeElement('LongDescription', $this->stripHtml($prop->desc));
			$this->propWriter->endElement();
			
			if($prop->unitCount) $this->propWriter->writeElement('UnitCount', $prop->unitCount);
			if($prop->yearBuilt) $this->propWriter->writeElement('YearBuilt', $prop->yearBuilt);
			
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
			
			if($prop->amenities)
			{
				foreach($prop->amenities as $i => $amen)
				{
					$this->propWriter->startElement('Amenities');
					$this->propWriter->writeAttribute('type', 'community');
					$this->propWriter->writeElement('Feature', $amen);
					$this->propWriter->endElement();
				}
			}
			
			if($prop->services)
			{
				foreach($prop->services as $i => $service)
				{
					$this->propWriter->startElement('Amenities');
					$this->propWriter->writeAttribute('type', 'community');
					$this->propWriter->writeElement('Feature', $service);
					$this->propWriter->endElement();
				}
			}
			
			if($prop->photos)
				$this->feedAddFiles($this->propWriter, $prop->photos, 'photo');
			
			$this->propWriter->endElement();
			
			if($prop->listings)
				$this->feedListings($prop->listings);
			
			$this->propWriter->endElement(); 
		}
		
		protected function feedListings(&$listings)
		{
			foreach($listings as $i => $listing)
			{
				$this->propWriter->startElement('Floorplan');
				
				$this->propWriter->writeAttribute('id', $listing->id);
				
				$this->propWriter->writeElement('Name', $listing->name);
				
				$this->propWriter->startElement('Room');
				$this->propWriter->writeAttribute('type', 'bedroom');
				$this->propWriter->writeElement('Count', $listing->bed);
				$this->propWriter->writeElement('Comment', 'Bedrooms');
				$this->propWriter->endElement();
				
				$this->propWriter->startElement('Room');
				$this->propWriter->writeAttribute('type', 'bathroom');
				$this->propWriter->writeElement('Count', $listing->bath);
				$this->propWriter->writeElement('Comment', 'Bathrooms');
				$this->propWriter->endElement(); 
				
				$this->propWriter->startElement('MarketRent');
				
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
	
	ue_feeds_register_adapter('MITS - Stream', 'Mits adapter.', 'ue_feeds_adapter_mits_stream');