<?php
	
	class ue_feeds_adapter_olr extends ue_feeds_adapter implements ue_feeds_adapter_input
	{
		public function parse($reset = false)
		{
			if(!$this->parsed || $reset)
			{
				$rows = explode("\n", $this->feed->read());
				
				$header = $this->csv_explode(array_shift($rows), ';');
				foreach($rows as $row => $rowdata)
				{
					if(trim($rowdata))
					{
						$values = $this->csv_explode(trim($rowdata), ';');
						foreach($header as $col => $colname)
						{
							$data[strtolower($colname)] = trim($values[$col]);
						}
						$props[] = $data;
					}
				}
				$this->parsed['properties'] = $this->parseProperties($props);
			}
			
			return $this->parsed;
		}
		
		private function parseProperties(&$data)
		{
			foreach($data as $i => $row)
			{
				$externalID = intval($row['zip code']) . '-' . intval($row['house#']) . '-' . $row['street'];
				
				if($props[$externalID])
					$prop = $props[$externalID];
				else
				{
					$prop = new ue_feeds_property();
					
					$prop->externalID = $externalID;
					
					$prop->name = $prop->addr1 = $row['house#'] . ' ' . $row['street'];
					
					if(strlen($row['phone1'])) $prop->phone = $row['phone1'];
					if(strlen($row['email1'])) $prop->email = $row['email1'];
					if(strlen($row['zip code'])) $prop->zip = $row['zip code'];
					
					if(strlen($row['date built'])) $prop->yearBuilt = $row['date built'];
					if(strlen($row['units'])) $prop->unitCount = $row['units'];
					
					//i'm weary about putting full- or part-time doorman in here based on a spec like this, after the mansky lawsuit deal with the "no-fee" listings.
					//if(strtolower($row['doorman/concierge']) == 'y') $prop->amenities[] = 'Full-time Doorman';
			
					if(strtolower($row['elevator']) == 'y') $prop->access[] = 'Elevator';
					
					//all of their properties have garage set to Y, this can't possibly mean they all have on-site garages.
					//if(strtolower($row['garage']) == 'y') $prop->amenities[] = 'On-site Garage';
					
					//we also need context on health club.
					//if(strtolower($row['health club']) == 'y') $prop->amenities[] = 'Fitness Center';
					
					$floors = intval($row['floors']);
					if($floors > 0)
					{
						if($floors < 7)
							$prop->type[] = 'Low-rise (1-6 Floors)';
						elseif($floors < 26)
							$prop->type[] = 'Mid-rise (7-25 Floors)';
						else
							$prop->type[] = 'High-rise (26+ Floors)';
					}
					
					for($j = 1; $j < 10; $j++)
						if(strlen($row["photo$j"])) $prop->photos[] = $row["photo$j"];
				
					$hood = ue_feeds_get_neighborhoods($prop->zip);
					if($hood) $prop->hood = $hood->tid;
				
					if($prop->hood)
						$props[$prop->externalID] = $prop;
				}
				
				//Only rentals at this point.			
				if(strtolower($row['rental/sale']) == 'r')
					$prop->listings[] = $this->parseListing($row);
			}
			return $props;
		}
		
		//returns listing from listing data in each row.
		private function parseListing(&$row)
		{
			$listing = new ue_feeds_listing();
			
			$listing->externalID = $row['listing id'];
			
			$listing->name = $row['unit'];
			
			$listing->bed = (float)$row['bedrooms id'];
			
			$listing->bath = (float)$row['baths'];
			
			if(floatval($row['square foot']) > 0)
				$listing->sqft = floatval($row['square foot']);
			
			$maxPhotos = 10;
			
			for($i = 0; $i < $maxPhotos; $i++)
				if(strlen($row["photo$i"])) $listing->photos[] = $row["photo$i"];
			
			for($i = 0; $i < $maxPhotos; $i++)
				if(strlen($row["floorplan$i"])) $listing->floorplans[] = $row["floorplan$i"];
				
			if(floatval($row['price']) > 0)
				$listing->price = floatval($row['price']);
			
			if(strlen($row['comments']))
				$listing->desc = mb_convert_encoding($row['comments'], 'ISO-8859-1');
			
			if(strlen($row['date available']) && strtotime($row['date available']) > time())
				$listing->dateAvail = strtotime($row['date available']);
				
			if(strtolower($row['balcony']) == 'y') $listing->outdoorSpace[] = 'Balcony';
			
			if(strtolower($row['terrace']) == 'y') $listing->outdoorSpace[] = 'Terrace';
			
			return $listing;
		}
		
		private function csv_explode($str, $delim=',', $enclose='"', $preserve=false)
		{
			$resArr = array();
			$n = 0;
			$expEncArr = explode($enclose, $str);
			foreach($expEncArr as $EncItem)
			{
				if($n++%2)
					array_push($resArr, array_pop($resArr) . ($preserve?$enclose:'') . $EncItem . ($preserve?$enclose:''));
				else
				{
					$expDelArr = explode($delim, $EncItem);
					array_push($resArr, array_pop($resArr) . array_shift($expDelArr));
					$resArr = array_merge($resArr, $expDelArr);
				}
			}
			return $resArr;
		}
	}
	
	ue_feeds_register_adapter('OLR', '', 'ue_feeds_adapter_olr');
