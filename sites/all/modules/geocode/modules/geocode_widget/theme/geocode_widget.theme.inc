<?php // $Id: geocode_widget.theme.inc,v 1.1 2009/11/04 20:44:15 vauxia Exp $

function geocode_widget_theme_theme() {
  $path = drupal_get_path('module', 'geocode_widget') .'/theme';
  return array(
    'geocode_widget_settings_form' => array(
      'arguments' => array('form'),
      'file' => 'geocode_widget.theme.inc',
      'path' => $path,
    ),
  );
}

function theme_geocode_widget_settings_form(&$form) {
  $hdrs = array('', t('Geocode handler'), t('Return value'));
  foreach (element_children($form) as $key) {
    $row = array();
    $row[] = drupal_render($form[$key]['status']);

    unset($form[$key]['handler']['#title']);
    $row[] = drupal_render($form[$key]['handler']);

    unset($form[$key]['return']['#title']);
    $row[] = drupal_render($form[$key]['return']);

    $rows[] = $row;
  }
  return theme('table', $hdrs, $rows);
}
