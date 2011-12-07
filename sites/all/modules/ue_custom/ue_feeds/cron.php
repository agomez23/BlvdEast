<?
	$start = microtime(true);
	chdir('../../../../../');
	include_once './includes/bootstrap.inc';
	drupal_bootstrap(DRUPAL_BOOTSTRAP_FULL);
	
	ue_feeds_download_files();
	
	$end = microtime(true);
	
	$time = round($end - $start, 3);
	print 'Downloaded files at ' . date('m/d/y h:i:s', $start) . ' in ' . $time . " seconds.\n";
?>