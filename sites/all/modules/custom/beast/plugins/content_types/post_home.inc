<?php

if (module_exists('apachesolr_search')) {
  $plugin = array(
    'single' => TRUE,
    'no title override' => TRUE,
    'title' => t('Post Home'),
    'category' => t('Blvd East'),
  );
}

/**
 * Render the search results.
 */
function beast_post_home_content_type_render($subtype, $conf, $args, $context) {
	global $user;
	
	$block = new stdClass();
	$block->module = 'beast';
	$block->delta = 'post_home';
	
	$type = 'home';
	
	$node = array(
		'uid' => $user->uid,
		'name' => (isset($user->name) ? $user->name : ''),
		'type' => $type,
		'language' => '',
		'field_propertyid' => array(array('value' => 1)),
	);
	
	module_load_include('inc', 'node', 'node.pages');  
	
	$output = drupal_get_form($type . '_node_form', $node);
	
	$block->title = t('<none>');
	$block->content = $output;
	
	return $block;
}

/**
 * Return an edit form for the pane's settings.
 */
function beast_post_home_content_type_edit_form(&$form, &$form_state) {
  $conf = $form_state['conf'];
}