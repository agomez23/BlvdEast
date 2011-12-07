<?
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
?>