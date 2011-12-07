<?
	set_time_limit(120);
	$start = microtime(true);
	chdir('../../../../../');
	include_once './includes/bootstrap.inc';
	
	$base_url = 'http://www.urbanedgeny.com';
	
	drupal_bootstrap(DRUPAL_BOOTSTRAP_FULL);
	
	$clients = views_get_view('tmpOMGClients');
	$clients->set_items_per_page(0);
	$clients->execute();
	
	//$paidSites = array('CZ', 'EN', 'FR', 'HP', 'NC', 'OX', 'RE', 'RH', 'SE', 'TR', 'TA', 'VA', 'YA', 'ZI');
	//$premSites = array();
	
	//foreach($paidSites as $i => $key) $paid[] = array('value' => $key);
	//foreach($premSites as $i => $key) $prem[] = array('value' => $key);
	
	//if(!count($paid)) $paid = array(array('value' => ''));
	//if(!$prem) $prem = array(array('value' => ''));
	
	foreach($clients->result as $i => $row)
	{
		$node = node_load($row->nid, NULL, true);
		if(is_array($node->field_syndication_paid))
			$node->field_syndication_paid[] = array('value' => 'WS');
			
		//$node->field_syndication_paid = $paid;
		//$node->field_syndication_premium = $prem;
		//$node->field_syndication_free = array(array('value' => 1));
		content_update($node);
		unset($node);
	}
?>