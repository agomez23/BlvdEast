<?php

	function ue_feeds_feed($feed)
	{
		
		$ref = new ReflectionClass($feed->adapter);
		
		if($ref->implementsInterface(ue_feeds_adapter_output_stream))
		{
			drush_print('using stream method');
			
			$properties = $feed->filter->getPropertyIDs();
			
			$nodes = array('properties'=>$properties);
		
			$data = $feed->adapter->feedStream($nodes);
			
			return $data;
		}
		else
		{
			drush_print('using classic method');
			
			$properties = $feed->filter->getProperties();
			
			$nodes = array('properties'=>$properties);
		
			$data = $feed->adapter->feed($nodes);
			
			return $data;
		}
	}