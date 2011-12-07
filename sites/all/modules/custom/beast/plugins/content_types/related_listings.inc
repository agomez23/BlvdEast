<?php

  $plugin = array(
    'single' => TRUE,
    'no title override' => TRUE,
    //'title' => t('Related Listings'),
	'title' => t('Similar Listings'),
    'category' => t('Blvd East'),
    'required context' => new ctools_context_required(t('Node'), 'node'),
  );

/**
 * Render the search results.
 */
function beast_related_listings_content_type_render($subtype, $conf, $args, $context) {
	if (!isset($context->data)) {
		return;
	}
	
	$node = drupal_clone($context->data);
	
	$block = new stdClass();
	$block->module = 'beast';
	$block->delta = 'related_listings';
	
	//$block->title = t('Related Listings');
	$block->title = t('Similar Listings');
	
	require_once(drupal_get_path('module', 'apachesolr') .'/apachesolr.admin.inc');
	
	$settings = apachesolr_mlt_block_defaults();
	
	$settings = apachesolr_mlt_load_block('mlt-001');
	
	$settings['mlt_type_filters'] = array($node->type);
	
	//add weight to filters so listings in the same property or town appear at top of list.
	$settings['mlt_custom_filters'] = '(' . implode(' OR ', array(
		'tid:' . $node->field_bedroom[0]['value'] . '^3',
		//'tid:' . $node->field_town[0]['value'] . '^4',
		'is_cck_field_propertyid:' . $node->field_propertyid[0]['nid'] . '^8',
	)) . ')';
	
	$docs = apachesolr_mlt_suggestions($settings, apachesolr_document_id($node->nid));
	$nids = implode(',', beast_solr_nids($docs));
	
	$block->content = views_embed_view('availabilities_mini', 'panel_pane_3', $nids);
	
	return $block;
}

/**
 * Return an edit form for the pane's settings.
 */
function beast_related_listings_content_type_edit_form(&$form, &$form_state) {
  $conf = $form_state['conf'];
}