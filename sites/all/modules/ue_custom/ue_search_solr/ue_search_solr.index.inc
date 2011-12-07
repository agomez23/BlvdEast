<?php

/**
* Submit a batch job to index the remaining, unindexed content.
*/
function ue_search_solr_batch_index_remaining() {
  $batch = array(
    'operations' => array(
      array('ue_search_solr_batch_index_nodes', array()),
    ),
    'finished' => 'apachesolr_batch_index_finished',
    'title' => t('Indexing'),
    'init_message' => t('Preparing to submit content to Solr for indexing...'),
    'progress_message' => t('Submitting content to Solr...'),
    'error_message' => t('Solr indexing has encountered an error.'),
    'file' => drupal_get_path('module', 'ue_search_solr') . '/ue_search_solr.index.inc',
  );
  batch_set($batch);
}

/**
* Batch Operation Callback
*/
function ue_search_solr_batch_index_nodes(&$context) {
  if (empty($context['sandbox'])) {
    try {
      // Get the $solr object
      $solr = apachesolr_get_solr();
      // If there is no server available, don't continue.
      if (!$solr->ping()) {
        throw new Exception(t('No Solr instance available during indexing.'));
      }
    }
    catch (Exception $e) {
      watchdog('Apache Solr', $e->getMessage(), NULL, WATCHDOG_ERROR);
      return FALSE;
    }

	$status = ue_search_solr_index_status('ue_search_solr');
    $context['sandbox']['progress'] = 0;
    $context['sandbox']['max'] = $status['remaining'];
  }

  // We can safely process the apachesolr_cron_limit nodes at a time without a
  // timeout or out of memory error.
  $limit = variable_get('apachesolr_cron_limit', 50);

  // With each pass through the callback, retrieve the next group of nids.
  $rows = ue_search_solr_get_nodes_to_index('ue_search_solr', $limit);
  apachesolr_index_nodes($rows, 'ue_search_solr');

  $context['sandbox']['progress'] += count($rows);
  $context['message'] = t('Indexed @current of @total nodes', array('@current' => $context['sandbox']['progress'], '@total' => $context['sandbox']['max']));
  drush_print($context['message']);

  // Inform the batch engine that we are not finished, and provide an
  // estimation of the completion level we reached.
  $context['finished'] = empty($context['sandbox']['max']) ? 1 : $context['sandbox']['progress'] / $context['sandbox']['max'];

  // Put the total into the results section when we're finished so we can
  // show it to the admin.
  if ($context['finished']) {
    $context['results']['count'] = $context['sandbox']['progress'];
  }
}

/**
 * Helper function for modules implmenting hook_search's 'status' op.
 */
function ue_search_solr_index_status($namespace) {
  list($excluded_types, $args, $join_sql, $exclude_sql) = apachesolr_exclude_types($namespace);
 
  $total = db_result(db_query("SELECT COUNT(asn.nid) FROM {apachesolr_search_node} asn ". $join_sql ."INNER JOIN {content_type_listing} l ON l.nid = asn.nid AND l.field_unit_availability_value = 'On the Market' AND l.field_sales_value = 0 AND l.field_propertyid_nid > 1 WHERE asn.status = 1 " . $exclude_sql, $excluded_types));
  $remaining = db_result(db_query("SELECT COUNT(asn.nid) FROM {apachesolr_search_node} asn ". $join_sql ."INNER JOIN {content_type_listing} l ON l.nid = asn.nid AND l.field_unit_availability_value = 'On the Market' AND l.field_sales_value = 0 AND l.field_propertyid_nid > 1 WHERE (asn.changed > %d OR (asn.changed = %d AND asn.nid > %d)) AND asn.status = 1 " . $exclude_sql, $args));
  return array('remaining' => $remaining, 'total' => $total);
}

/**
 * Returns an array of rows from a query based on an indexing namespace.
 */
function ue_search_solr_get_nodes_to_index($namespace, $limit) {
  extract(apachesolr_get_last_index($namespace));
  $args = array($last_change, $last_change, $last_nid);
  $result = db_query_range("SELECT asn.nid, asn.changed FROM {apachesolr_search_node} asn INNER JOIN {content_type_listing} l ON l.nid = asn.nid AND l.field_unit_availability_value = 'On the Market' AND l.field_sales_value = 0 AND l.field_propertyid_nid > 1 WHERE (asn.changed > %d OR (asn.changed = %d AND asn.nid > %d)) AND asn.status = 1 ORDER BY asn.changed ASC, asn.nid ASC", $args, 0, $limit);
  while($row = db_fetch_object($result)) {
    $rows[] = $row;
  }
  return $rows;
}