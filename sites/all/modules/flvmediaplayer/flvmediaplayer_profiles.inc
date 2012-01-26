<?php

// $Id: flvmediaplayer_profiles.inc,v 1.1.2.2.2.11 2009/05/14 17:52:29 arthuregg Exp $

/* ************************************************************** */
/* PROFILE FUNCTIONS */
/* ************************************************************** */


/**
 * handles the profile config save/delete
 * @param $form_state
 * @param $pid
 */
function flvmediaplayer_profile_edit_form(&$form_state, $pid) {
  // load the profile
  $profile = flvmediaplayer_profile_get_config($pid);
  $form = array();

  // grab the profile form
  $form['profiles']['data'] = flvmdiaplayer_get_player_form($profile);

  // set the pid
  $form['profiles']['flvmp_pid'] = array(
    '#type' => 'value',
    '#value' => $pid,
  );

  $form['profiles']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );

  $form['profiles']['delete'] = array(
    '#type' => 'submit',
    '#value' => t('Delete'),
    '#submit' => array('flvmediaplayer_profile_delete_submit'),
  );
  return $form;
}


/**
 * handles the submit of an individual profile form
 */
function flvmediaplayer_profile_edit_form_submit($form, &$form_state) {
  // parse the data out of the form
  $data = flvmediaplayer_profile_parse_profile_form_data($form_state['values']);
  // update the name, so use the pid
  db_query('UPDATE {flvmediaplayer_profiles} SET name = "%s", config = "%s" WHERE pid = %d', $data['flvmp_name'], serialize($data), $data['flvmp_pid']);
  // clear cached nodes
  cache_clear_all('*', 'cache_flvmediaplayer', true);
  drupal_set_message(t('Cleared configuration cache'));
  drupal_set_message(t('Saved your player profile'));
}


/**
 * Delete the requested profile
 * @param $form
 * @param $form_state
 */
function flvmediaplayer_profile_delete_submit($form, &$form_state) {
  cache_clear_all('flvmp_%'. $form_state['values']['name'], 'cache_flvmediaplayer', true);
  db_query('DELETE FROM {flvmediaplayer_profiles} WHERE pid = %d', $form_state['values']['pid']);
  drupal_set_message(t('Deleted your player profile'));
  $form_state['redirect'] = 'admin/settings/flvmediaplayer';
}