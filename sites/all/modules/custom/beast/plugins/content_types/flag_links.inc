<?php

  $plugin = array(
    'single' => TRUE,
    'no title override' => TRUE,
    'title' => t('Flag links'),
    'category' => t('Blvd East'),
    'required context' => new ctools_context_required(t('Node'), 'node'),
  );

/**
 * Render the search results.
 */
function beast_flag_links_content_type_render($subtype, $conf, $args, $context) {
  if (!isset($context->data)) {
    return;
  }
  $node = drupal_clone($context->data);
  
  
  // The build mode identifies the target for which the node is built.
  if (!isset($node->build_mode)) {
    $node->build_mode = NODE_BUILD_NORMAL;
  }

  $links = module_invoke_all('link', 'node', $node, $conf['teaser']);
  drupal_alter('link', $links, $node);
	
	$block = new stdClass();
	$block->module = 'beast';
	$block->delta = 'flag_links';
	
	$block->title = '';
	
	if($links['flag-favorites'])
		$block->content = theme('links', array($links['flag-favorites']));
	
	return $block;
}

/**
 * Return an edit form for the pane's settings.
 */
function beast_flag_links_content_type_edit_form(&$form, &$form_state) {
  $conf = $form_state['conf'];
}