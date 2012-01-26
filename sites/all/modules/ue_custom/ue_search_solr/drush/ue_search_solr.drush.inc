<?php
// $Id: apachesolr.drush.inc,v 1.1.2.7 2010/03/24 16:40:08 robertDouglass Exp $

/**
 * @file
 *   drush integration for apachesolr.
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
function ue_search_solr_drush_command() {
  $items = array();

  // the key in the $items array is the name of the command.
  $items['ue-solr-delete-index'] = array(
    // the name of the function implementing your command.
    'callback' => 'ue_search_solr_drush_solr_delete_index',
    // a short description of your command
    'description' => dt('Deletes UE listings from solr index.'),
  );
  $items['ue-solr-index'] = array(
    // the name of the function implementing your command.
    'callback' => 'ue_search_solr_drush_solr_index',
    // a short description of your command
    'description' => dt('Reindexes listings marked for (re)indexing.'),
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
function ue_search_solr_drush_help($section) {
  switch ($section) {
    case 'drush:ue-solr-delete-index':
      return dt("Used without parameters, this command deletes the entire Solr index. Used with parameters for content type, it deletes just the content types that are specified. After the index has been deleted, all content will be indexed again on future cron runs.");
    case 'drush:ue-solr-index':
      return dt("Reindexes content marked for (re)indexing. If you want to reindex all content or content of a specific type, use solr-reindex first to mark that content.");
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
function ue_search_solr_drush_solr_delete_index() {
	$args = func_get_args();
	module_load_include('inc', 'apachesolr', 'apachesolr.admin');
	apachesolr_delete_index('listing');
}

function ue_search_solr_drush_solr_index() {
	module_load_include('inc', 'ue_search_solr', 'ue_search_solr.index');
	ue_search_solr_batch_index_remaining();
	drush_backend_batch_process();
}
