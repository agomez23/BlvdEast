<?php

// $Id: flvmediaplayer_io.inc,v 1.1.2.1.2.3 2009/05/19 19:21:51 arthuregg Exp $

/* ************************************************************** */
/* Import / Export  */
/* ************************************************************** */

/**
 * creates the form for the export process
 * @param int $pid
 * @return array $form drupal form array
 *
 */
function flvmediaplayer_export_form($drupal_form, $pid = null) {

  // if a $pid is incoming, display it
  if ($pid) {
    return flvmediaplayer_export_display($pid);
  }

  // get a list of all the profiles
  $profiles = flvmediaplayer_profile_get_configs();
  $form['flvmediaplayer_export'] = array(
    '#type' => 'radios',
    '#title' => t('Profile to export'),
    '#options' => $profiles,
    '#description' => t('Select the profile you wish to export'),
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Export'),
  );
  return $form;
}


/**
 * Send the user to the export display screen
 * @param $form   
 * @param $form_values
 * return string drupal goto
 */
function flvmediaplayer_export_form_submit($form, &$form_state) {
  $form_state['redirect'] = 'admin/settings/flvmediaplayer/export/'. $form_state['values']['flvmediaplayer_export'];
}


/**
 * displays the profile in code form
 * @param $pid
 *   int, profile id
 */
function flvmediaplayer_export_display($pid) {
  // get the profile
  $profile = db_fetch_array(db_query('SELECT * FROM {flvmediaplayer_profiles} WHERE pid = %d', $pid));
  // unserialize the profile
  $profile['config'] = unserialize($profile['config']);
  // format the profile
  $profile = flvmediaplayer_export_display_helper($profile);
  // prefix the profile
  $profile = '$profile = '. $profile .';';

  $form['output'] = array(
    '#type' => 'textarea',
    '#title' => t('Profile export'),
    '#default_value' => $profile,
    '#rows' => 20,
  );
  return $form;
}


/**
 * takes an array and converts it to text helper for the export function
 * @param $data
 *   array, profile data
 * @param $spaces
 *   int, number of white spaces needed in the array
 * @return string 
 *   textual representation of array
 *
 */
function flvmediaplayer_export_display_helper($data, $spaces = 0) {
  // set the number of spaces
  for($i=1; $i<=$spaces; $i++) {
    $whitespace .= ' ';
  }
  if (is_array($data)) {
    $output = "array(\n  ";
    foreach ($data as $key => $value) {
      if (is_array($data[$key])) {
        $items[] = "$whitespace'$key' => ". flvmediaplayer_export_display_helper($value, $spaces + 2 );
      }
      else {
        $items[] = "$whitespace'$key' => '$value'";
      }
    }
    $output .= implode(",\n  ", $items) ."\n$whitespace)";
    return $output;
  }
  else {
    return "'$data'";
  }
}


/**
 * creates the import form
 */
function flvmediaplayer_import_form() {
  $form['import'] = array(
    '#title' => t('Enter the profile import code'),
    '#type' => 'textarea',
    '#description' => t('Paste the exported code here. This will create a new profile. If the imported profile already exists on the system, it will update the existing one.')
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Import'),
  );
  return $form;
}

/**
 * accepts the import form submission
 * @param string $form_id
 * @param array $form_values
 * @return string
 */
function flvmediaplayer_import_form_submit($form, &$form_state) {
  // evaluate imported code
  ob_start();
  eval($form_state['values']['import']);
  ob_end_clean();

  // does this pid already exist? no, update the existing one
  $query = db_result(db_query('SELECT pid FROM {flvmediaplayer_profiles} WHERE pid = %d', $profile['pid']));
  if ($query) {
    db_query('UPDATE {flvmediaplayer_profiles} SET config = "%s" WHERE pid = %d', serialize($profile['config']), $profile['pid']);
    $pid = $profile['pid'];
    drupal_set_message(t('Successfully updated flvmediaplayer profile: !name .', array('!name' => $profile['name'])));
  }
  // create a new profile
  else {
    db_query('INSERT INTO {flvmediaplayer_profiles} (type, name, config) VALUES ("%s", "%s", "%s")', $profile['type'], $profile['name'], serialize($profile['config']));
    $pid = db_last_insert_id('flvmediaplayer_profiles', 'pid');
    drupal_set_message(t('Successfully imported flvmediaplayer profile: !name .', array('!name' => $profile['name'])));
  }
  $form_state['redirect'] = 'admin/settings/flvmediaplayer/profile/'. $pid;
}


