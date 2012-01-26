<?php

  $plugin = array(
    'single' => TRUE,
    'no title override' => TRUE,
	'title' => t('Transportation Info'),
    'category' => t('Blvd East'),
    'description' => t('Given a node with a field_town tid attached, will pull the transportation info from town node.'),
    'required context' => new ctools_context_required(t('Node'), 'node'),
  );

/**
 * Render the search results.
 */
function beast_trans_info_content_type_render($subtype, $conf, $args, $context) {
	if (!isset($context->data)) {
		return;
	}
	
	$node = drupal_clone($context->data);
	
	$town_nid = db_result(db_query('SELECT tt.nid FROM content_type_town tt INNER JOIN content_field_town ft ON ft.nid = tt.nid AND ft.field_town_value = %d', $node->field_town[0]['value']));
	
	if(!$town_nid)
		return;
	
	$town_node = node_load($town_nid);
	ctools_include('context');
	$panel_mini = panels_mini_load('community_transportation');
	
	$context = new StdClass();
	$context->type = 'node';
	$context->data = $town_node;
	
	$context = ctools_context_match_required_contexts($panel_mini->requiredcontexts, array($context));
	$panel_mini->context = $panel_mini->display->context = ctools_context_load_contexts($panel_mini, FALSE, $context);
	
	
	$block = new stdClass();
	$block->module = 'beast';
	$block->delta = 'trans_info';
	$block->title = t('Transportation');
	
	$block->content = panels_render_display($panel_mini->display);
	
	return $block;
}

/**
 * Return an edit form for the pane's settings.
 */
function beast_trans_info_content_type_edit_form(&$form, &$form_state) {
  $conf = $form_state['conf'];
}