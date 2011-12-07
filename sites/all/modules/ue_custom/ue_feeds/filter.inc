<?php
	
	abstract class ue_feeds_filter
	{
		protected $feed = NULL;
		
		function __construct(ue_feeds_feed $feed = NULL)
		{
			$this->feed = $feed;
		}
		
		public function getProperty($propID, $listings)
		{
			$prop = new ue_feeds_property(node_load($propID), true);
			
			if($this->feed->options->appendcontact && $prop->desc)
			{
				$contactLine = array();
				if(isset($prop->phone)) $contactLine[] = t('call !phone', array('!phone' => $prop->phone));
				if(isset($prop->email)) $contactLine[] = t('email <a href="mailto:!email">!email</a>', array('!email' => $prop->email));
				if(isset($prop->website)) $contactLine[] = t('visit <a href="!url">!url</a>', array('!url' => $prop->website));
				
				if(count($contactLine))
				{
					$contactLine[count($contactLine)-1] = 'or ' . $contactLine[count($contactLine)-1];
					$prop->desc .= t("\nPlease !contact today for more information!", array('!contact' => implode(count($contactLine) > 2 ? ', ' : ' ', $contactLine)));
				}
			}
			
			//foreach($listings as $listingID)
			//	$prop->listings[] = new ue_feeds_listing(node_load($listingID, true));
			
			return $prop;
		}
		
		public function getProperties()
		{
			$propIDs = $this->getPropertyIds();
			
			drush_print('Loading ' . count($propIDs) . ' properties.');
			
			foreach($propIDs as $propID => $listings)
			{
				$properties[$propID] = $this->getProperty($propID, $listings);
			}
			
			return array_values($properties);
		}
		
		//even though new filters will be based off listings, the syndication structure is based off properties as a container for listings.
		//filters will return an array of node IDs, likely from a standard mysql query - loadProperties will take care of loading the actual data.
		abstract protected function getPropertyIds();
	}
	
?>