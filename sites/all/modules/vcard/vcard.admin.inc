<?php
/* $Id: vcard.admin.inc,v 1.1 2008/07/17 19:49:43 sanduhrs Exp $ */

/**
 * vCard
 */

/**
 * Callback for administration page.
 */
function vcard_admin_settings() {
  if (!_vcard_init()) {
    drupal_set_message(t('The PEAR package Contact_Vcard_Build (required by vcard.module) has not been installed properly, please read INSTALL.txt.'), 'error');
  }

  $form['field_mappings'] = array(
    '#type' => 'fieldset',
    '#title' => t('Field Mappings'),
    '#description' => t('This section is only enabled if the "profiles" module is enabled. When enabled it will provide a dropdown selection box for each defined profile item of type textfield, textarea, or url.'),
  );
  $options = array('' => 'Select a property');
  $options = $options + _vcard_properties();
  foreach (_vcard_profile_fields() as $fid => $title) {
    $form['field_mappings']['vcard_profile_'. $fid] = array(
      '#type' => 'select',
      '#title' => t('Property for ') . $title,
      '#default_value' => variable_get('vcard_profile_'. $fid, ''),
      '#options' => $options,
    );
  }

  return system_settings_form($form);
}

/**
 *
 */
function _vcard_profile_fields() {
  $output = array();
  $result = db_query("SELECT fid, title FROM {profile_fields}");
  while ($row = db_fetch_object($result)) {
    $output[$row->fid] = $row->title;
  }
  return $output;
}