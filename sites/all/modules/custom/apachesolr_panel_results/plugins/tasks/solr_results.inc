<?php
// $Id: search_results.inc,v 1.1.2.1 2010/06/11 22:59:45 gundalo Exp $

/**
 * @file
 * Handle the 'node teaser' override task.
 *
 * This plugin overrides node teasers.
 */

/**
 * Specialized implementation of hook_page_manager_task_tasks(). See api-task.html for
 * more information.
 */
function apachesolr_panel_results_solr_results_page_manager_tasks() {
  return array(
    // This is a 'page' task and will fall under the page admin UI
    'task type' => 'page',

    'title' => t('Solr results'),

    'admin title' => t('Solr results'),
    'admin description' => t('When enabled, this overrides the default Drupal behavior for displaying search results.  If you add variants, you may use selection criteria such as node type or language or user access to provide different views of nodes. If no variant is selected, the default Drupal search result view will be used. '),

    // This is task uses 'context' handlers and must implement these to give the
    // handler data it needs.
    'handler type' => 'context',
    'get arguments' => 'apachesolr_panel_results_solr_results_get_arguments',
    'get context placeholders' => 'apachesolr_panel_results_solr_results_get_contexts',

    // Allow this to be enabled or disabled:
    'disabled' => variable_get('apachesolr_panel_results_solr_results_disabled', TRUE),
    'enable callback' => 'apachesolr_panel_results_solr_results_enable',

    // Enable other drupal hooks for this plugin.
    'use hooks' => TRUE,
  );
}


/**
 * Callback to get arguments provided by this task handler.
 *
 * Since this is the node view and there is no UI on the arguments, we
 * create dummy arguments that contain the needed data.
 */
function apachesolr_panel_results_solr_results_get_arguments($task, $subtask_id) {
  return array(
    array(
      'keyword' => 'node',
      'identifier' => t('Node being viewed'),
      'id' => 1,
      'name' => 'nid',
      'settings' => array(),
    ),
  );
}

/**
 * Callback to get context placeholders provided by this handler.
 */
function apachesolr_panel_results_solr_results_get_contexts($task, $subtask_id) {
  return ctools_context_get_placeholders_from_argument(apachesolr_panel_results_solr_results_get_arguments($task, $subtask_id));
}

/**
 * Callback to enable/disable the page from the UI.
 */
function apachesolr_panel_results_solr_results_enable($cache, $status) {
  variable_set('apachesolr_panel_results_solr_results_disabled', $status);

  // Set a global flag so that the menu routine knows it needs
  // to set a message if enabling cannot be done.
  if (!$status) {
    $GLOBALS['page_manager_enabling_node_view'] = TRUE;
  }
}
