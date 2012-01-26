<?php

  $plugin = array(
    'single' => TRUE,
    'no title override' => TRUE,
    'title' => t('Street View'),
    'category' => t('Blvd East'),
    'required context' => new ctools_context_required(t('Node'), 'node'),
  );

/**
 * Render the search results.
 */
function beast_street_view_content_type_render($subtype, $conf, $args, $context) {
  if (!isset($context->data)) {
    return;
  }
  $node = drupal_clone($context->data);
	
	$block = new stdClass();
	$block->module = 'beast';
	$block->delta = 'street_view';
	
	$output = 'street view';
	
	$block->title = t('Street View');
	$block->content = '<div id="streetview_' . $node->nid . '" class="streetview"></div>';
	
	drupal_add_js('
		var pano' . $node->nid . ';
		$(function(){
		  var nodelatlng = new GLatLng(' . $node->location['latitude'] . ',' . $node->location['longitude'] . ');
		  panoramaOptions = { latlng:nodelatlng };
		  myPano = new GStreetviewPanorama(document.getElementById("streetview_' . $node->nid . '"), panoramaOptions);
		});', 'inline');
	
	return $block;
}

/**
 * Return an edit form for the pane's settings.
 */
function beast_street_view_content_type_edit_form(&$form, &$form_state) {
  $conf = $form_state['conf'];
}