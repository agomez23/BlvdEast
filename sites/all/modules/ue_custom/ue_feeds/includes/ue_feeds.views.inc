<?php
/**
 * Implementation of hook_views_data()
 */
function ue_feeds_views_data() {

  $data = array();

    $data['node']['node_feed'] = array(
      'title' => t('UE Feeds: feed'),
      'help' => t('Filters nodes that are updated via feed.'),
      'filter' => array(
        'handler' => 'ue_feeds_handler_filter_feed',
      ),
    );

  return $data;
}


/**
 * Implementation of hook_views_handlers() to register all of the basic handlers
 * views uses.
 */
function ue_feeds_views_handlers() {
  return array(
    'info' => array(
      'path' => drupal_get_path('module', 'ue_feeds') . '/includes',
    ),
    'handlers' => array(
      // The name of my handler
      'ue_feeds_handler_filter_feed' => array(
        // The name of the handler we are extending.
        'parent' => 'views_handler_filter',
      ),
    ),
  );
}