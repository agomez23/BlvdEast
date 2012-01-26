<?
	
	/* run some maintenance actions on cron */
	function ue_custom_cronplus_daily()
	{
		ue_custom_cron_geocode(); // geocode properties that haven't already been.
	}
	
	function ue_custom_cronplus_hourly()
	{
		ue_custom_cron_cleanopenhouse(); // remove open houses as the end date (start date if null) 
		ue_custom_cron_cleandateavail(); // blank out date available when the date falls before today
	}
	
	function ue_custom_cron_geocode()
	{
		module_load_include('inc', 'location', 'location');
		module_load_include('inc', 'location', 'geocoding/google');
		$view = views_get_view('view_ungeocoded');
		$view->execute();
		foreach($view->result as $index=>$result)
		{
			$node = node_load($result->nid);
			$location = google_geocode_location(array('street'=>$node->field_address_line_1[0]['value'], 'city'=>$node->field_city[0]['value'], 'state'=>$node->field_province[0]['value'], 'postal_code'=>$node->field_zipcode[0]['value'], 'country'=>'us'));
			$node->field_latitude[0]['value'] = $location['lat'];
			$node->field_longitude[0]['value'] = $location['lon'];
			node_save($node);
		}
	}
	
	function ue_custom_cron_cleanopenhouse()
	{
		$sql = 'SELECT DISTINCT nid FROM content_field_openhousedate WHERE ';
		$sql .= '(field_openhousedate_value < NOW() AND field_openhousedate_value2 IS NULL) OR ';
		$sql .= 'field_openhousedate_value2 < NOW() LIMIT 0,20';
		
		$nodes = db_query($sql);
		
		while($nid = db_result($nodes))
		{
			$node = node_load($nid);
			$starttime = strtotime($node->field_openhousedate[0]['value']);
			$endtime = strtotime($node->field_openhousedate[0]['value2']);
			if(empty($endtime) && $startime < time())
				node_delete($node->nid);
			else if($endtime < time())
				node_delete($node->nid);
		}
	}
	
	function ue_custom_cron_cleandateavail()
	{	
		/* field_date moved back to property table, for now at least.
		$sql .= 'SELECT DISTINCT content_type_listing.nid FROM content_field_date ';
		$sql .= 'INNER JOIN content_type_listing ON content_type_listing.vid = content_field_date.vid ';
		$sql .= 'WHERE field_date_value < NOW() LIMIT 0,20;';
		*/
		
		$sql .= 'SELECT DISTINCT content_type_listing.nid FROM content_type_listing WHERE field_date_value < NOW() LIMIT 0,20;';
		
		$nodes = db_query($sql);
		
		while($nid = db_result($nodes))
		{
			$node = node_load($nid);
			$dateavail = strtotime($node->field_date[0]['value']);
			if($dateavail < time())
			{
				unset($node->field_date[0]['value']);
				content_update($node);
			}
		}
	}
?>