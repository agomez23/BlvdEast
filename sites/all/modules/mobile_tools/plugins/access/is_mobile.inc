<?php
// $Id: is_mobile.inc,v 1.1.2.1 2009/10/15 00:26:09 tdrycker Exp $

/**
 * @file
 * Plugin to provide access control based on evaluated PHP.
 */

/**
 * Implementation of specially named hook_ctools_access().
 */
function mobile_tools_is_mobile_ctools_access() {
  $args['is_mobile'] = array(
    'title' => t("Mobile Browser"),
    'description' => t('Control access based on mobile browser.'),
    'callback' => 'mobile_tools_is_mobile_ctools_access_check',
    'default' => array('description' => '', 'browser' => ''),
    'settings form' => 'mobile_tools_is_mobile_ctools_access_settings',
    'summary' => 'mobile_tools_is_mobile_ctools_acesss_summary',
    'all contexts' => TRUE,
  );

  return $args;
}

/**
 * Settings form for the 'by perm' access plugin
 *
 * @todo Need a way to provide a list of all available contexts to be used by
 *       the eval-ed PHP.
 */
function mobile_tools_is_mobile_ctools_access_settings(&$form, &$form_state, $conf) {
  $perms = array();

  $options = drupal_map_assoc(mobile_tools_device_groups());

  $form['settings']['description'] = array(
    '#type' => 'textfield',
    '#title' => t('Administrative desc'),
    '#default_value' => $conf['description'],
    '#description' => t('A description for this test for administrative purposes.'),
  );
  $form['settings']['mobile'] = array(
    '#type' => 'select',
    '#title' => t('User Agent'),
    '#default_value' => $conf['mobile'],
    '#options' => $options,
    '#multiple' => TRUE,
    '#description' =>  t('Access will be granted if the client browser matches.'),
  );
}

/**
 * Check for access.
 */
function mobile_tools_is_mobile_ctools_access_check($conf, $contexts) {
  $browser = mobile_tools_is_mobile_device();
  $access = in_array($browser['group'], $conf['mobile']);
  return $access;
}

/**
 * Provide a summary description based upon the checked roles.
 */
function mobile_tools_is_mobile_ctools_acesss_summary($conf, $contexts) {
  return check_plain($conf['description']);
}
