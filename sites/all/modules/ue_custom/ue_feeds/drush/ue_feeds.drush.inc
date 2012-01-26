<?php
	/**
	 * @file
	 *   drush integration for ue_feeds.
	 */
	
	/**
	 * Implementation of hook_drush_command().
	 *
	 * In this hook, you specify which commands your
	 * drush module makes available, what it does and
	 * description.
	 *
	 * Notice how this structure closely resembles how
	 * you define menu hooks.
	 *
	 * @See drush_parse_command() for a list of recognized keys.
	 *
	 * @return
	 *   An associative array describing your command(s).
	 */
	function ue_feeds_drush_command() {
		$items = array();
		
		// the key in the $items array is the name of the command.
		$items['ue-feeds-run'] = array(
			'callback' => 'drush_ue_feeds_run',
			'description' => dt("Runs a feed."),
			'arguments' => array(
				'fid' => dt('Feed ID.'),
			),
		);
		$items['ue-feeds-images'] = array(
			'callback' => 'drush_ue_feeds_images',
			'description' => dt("Downloads feed images."),
		);
		
		return $items;
	}
	
	/**
	 * Implementation of hook_drush_help().
	 *
	 * This function is called whenever a drush user calls
	 * 'drush help <name-of-your-command>'
	 *
	 * @param
	 *   A string with the help section (prepend with 'drush:')
	 *
	 * @return
	 *   A string with the help text for your command.
	 */
	function ue_feeds_drush_help($section) {
		switch ($section) {
			case 'drush:ue-feed-run':
			return dt("Runs an Urban Edge feed.");
		}
	}
	
	/**
	 * Example drush command callback.
	 *
	 * This is where the action takes place.
	 *
	 * In this function, all of Drupals API is (usually) available, including
	 * any functions you have added in your own modules/themes.
	 *
	 * To print something to the terminal window, use drush_print().
	 *
	 */
	function drush_ue_feeds_run($fid) {
		ue_feeds_run($fid, true);
	}
	
	/**
	 * Example drush command callback.
	 *
	 * This is where the action takes place.
	 *
	 * In this function, all of Drupals API is (usually) available, including
	 * any functions you have added in your own modules/themes.
	 *
	 * To print something to the terminal window, use drush_print().
	 *
	 */
	function drush_ue_feeds_images() {
		$start = microtime(true);
		ue_feeds_download_files();
		
		$end = microtime(true);
		
		$time = round($end - $start, 3);
		print 'Downloaded files at ' . date('m/d/y h:i:s', $start) . ' in ' . $time . " seconds.\n";
	}