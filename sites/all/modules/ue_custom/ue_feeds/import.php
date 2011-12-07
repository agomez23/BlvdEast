<?php
	$start = microtime(true);
	
	set_time_limit(480);
	
	chdir('../../../../../');
	include_once './includes/bootstrap.inc';
	drupal_bootstrap(DRUPAL_BOOTSTRAP_FULL);
	
	//set logged-in user to syndication.
	$user = user_load(126);
	$fid = intval($_GET['fid']);
	ue_feeds_import($fid);
	
	$end = microtime(true);
	
	$time = round($end - $start, 3);
	print 'Imported feed ' . $fid . ' at ' . date('m/d/y h:i:s', $start) . ' in ' . $time . " seconds.\n";
?>
