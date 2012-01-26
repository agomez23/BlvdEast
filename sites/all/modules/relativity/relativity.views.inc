<?php
// $Id: relativity.views.inc,v 1.1.2.4 2010/08/03 00:18:47 davea Exp $

function relativity_views_data() {
  //drupal_set_message('View hook called.');
  $data['relativity']['table']['group'] = t('Node Relativity');

  $data['relativity']['table']['base'] = array(
    'field' => 'nid',
    'title' => t('Node ID'),
    'help' => t('The Node ID of the child node.'),
  );

  $data['relativity']['table']['join'] = array(
    'node' => array(
      'left_field' => 'nid',
      'field' => 'nid',
    ),
  );

  $data['relativity']['nid'] = array(
    'title' => t('Child Node ID'),
    'help' => t('The Node ID of the child node.'),
    'field' => array(
      'handler' => 'views_handler_field_node',
      'click sortable' => TRUE,
    ),
  );

  $data['relativity']['parent_nid'] = array(
    'title' => t('Parent Node ID'),
    'help' => t('The Node ID of the parent node.'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'relationship' => array(
      'base' => 'node',
      'field' => 'nid',
      'handler' => 'views_handler_relationship',
      'label' => t('Node'),
    ),
  );

  return $data;
}
