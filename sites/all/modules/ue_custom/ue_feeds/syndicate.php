<?
	$start = microtime(true);
	chdir('../../../../../');
	include_once './includes/bootstrap.inc';
	
	$base_url = 'http://www.urbanedgeny.com';
	
	drupal_bootstrap(DRUPAL_BOOTSTRAP_FULL);
	header('Content-Type: text/plain');
	
	$fid = $_GET['fid'];
	
	$feed = ue_feeds_load($fid);
	
	if($feed->dir == 1)
	{
		
		$data = ue_feeds_feed($feed);
			
		$file = file_save_data($data, file_directory_path() . '/feeds/' . basename($feed->uri), FILE_EXISTS_REPLACE);
		
		db_query('UPDATE ue_feeds SET lastrun = NOW() WHERE fid = %d', $fid);
		
		//print '<a href="/' . $file . '">' . $file . '</a>';
		//print $data;
	}
	
	$end = microtime(true);
	
	$time = round($end - $start, 3);
	print 'Exported feed ' . $fid . ' (' . basename($feed->uri) . ') at ' . date('m/d/y h:i:s', $start) . ' in ' . $time . " seconds.\n";
?>