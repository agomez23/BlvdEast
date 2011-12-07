<?php

// $Id: flvmediaplayer_wizard.inc,v 1.1.2.1 2008/11/23 22:49:12 arthuregg Exp $


/**
 * @FILE  This file contains all the functions to run the install wizard
 *        for the flvmediaplayer
 */


/**
 * Controller for the configuration wizard
 *
 * @param array $form_values
 * @return array
 */
function flvmediaplayer_admin_settings_wizard_form($form_values = null) {

  if (! isset($form_values['flvmediaplayer_configuration_state'])) {
    $state = 1;
  }
  else {
    // we need to be sure that we can pass the profile name and id through the
    // formset
    $form['name'] = array('#type' => 'value', '#value' => $form_values['name']);
    $form['pid'] = array('#type' => 'value', '#value' => $form_values['pid']);
    $state = $form_values['flvmediaplayer_configuration_state'] + 1;
  }

  // what form function should be called?
  $function = 'flvmediaplayer_admin_settings_wizard_form_step_'. $state;
  if (function_exists($function)) {
    // get the requested form
    $form[] = $function($form_values);
  }

  $form['flvmediaplayer_configuration_state'] = array(
    '#type' => 'hidden',
    '#value' => $state,
  );

  // This part is important!
  $form['#multistep'] = TRUE;
  $form['#redirect'] = FALSE;

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Submit'),
  );
  return $form;
}



function flvmediaplayer_admin_settings_wizard_form_validate($form_id, $form_values) {
  switch ($form_values['flvmediaplayer_configuration_state']) {
    // player configuration, no validation needed
    case 1:
      //
    break;
    // profile validation
    case 2:
      flvmediaplayer_profile_create_form_validate($form_id, $form_values);
    break;

  }

}


/**
 * handles the submission of the helper forms
 *
 * @param string $form_id
 * @param array $form_values
 */
function flvmediaplayer_admin_settings_wizard_form_submit($form_id, $form_values) {
  switch ($form_values['flvmediaplayer_configuration_state']) {
    case 1:
      // player configuration need to set system values
      variable_set('flvmediaplayer_player', $form_values['flvmediaplayer_player']);
      variable_set('flvmediaplayer_path', $form_values['flvmediaplayer_path']);
    break;

    case 2:
      // create the profile name
      flvmediaplayer_profile_create_form_submit($form_id, $form_values);
    break;

    case 3:
      // submit the profile data to be saved
      flvmediaplayer_profile_edit_form_submit($form_id, $form_values);
    break;

    case 4:
      // this could alter the number of content type that were enabled, but we
      // are going to trust that the admin chose the content types that they
      // wanted to be enabled
      variable_set('flvmediaplayer_types', $form_values['flvmediaplayer_types']);

      // now set the profile that was generated already for these content types
      foreach ($form_values['flvmediaplayer_types'] as $type ) {
        variable_set('flvmp_'. $type .'_profile', $form_values['pid']);
      }
      drupal_set_message(t('Congratulations, you have configured your FLV player. You can edit settings here if you need to change any of your settings'));
      variable_set('flvmediaplayer_configuration_state', 4);
      drupal_goto('admin/settings/flvmediaplayer');
    break;
  }
}


/**
 * Creates the page for the configuration overview
 * @return array
 *   drupal form array
 */
function flvmediaplayer_admin_settings_wizard_form_step_0($form_values) {
  drupal_set_title(t('Hi!'));
  $text[] = '<h2>'. t('It looks like you need to configure FLV Media Player') . '</h2>';
  $text[] = '<p>'. t('This wizard will guide you through these steps to configure your FLV player'). '</p>';
  $text[] = theme('flvmediaplayer_admin_settings_wizard_steps');
  $text[] = '<p><b>'. l(t('Click here'), 'admin/settings/flvmediaplayer/wizard') . t(' to get started!') .'</b></p>';
  $text[] = '<p>'. t('You can also !expert the helper and configure yourself', array('!expert' => l(t('turn off'), 'admin/settings/flvmediaplayer/expert')));

  $form['text'] = array(
    '#type' => 'markup',
    '#value' => implode("\n", $text),
  );
  return $form;
}


/**
 * Creates the form for the player configuration
 *
 */
function flvmediaplayer_admin_settings_wizard_form_step_1($form_values) {
  drupal_set_title(t('Step 1: Choose your player'));
  $text[] = theme('flvmediaplayer_admin_settings_wizard_steps', 0);
  $text[] = t('You need to choose which flash player you would like to use and where it is located on your server. If you have not uploaded a flash player to you server, you need to do so now. A common player is the !player', array('!player' => l(t('JW FLV flash player'), 'http://www.jeroenwijering.com/?item=JW_FLV_Player')));

  $form['text'] = array(
    '#type' => 'markup',
    '#value' => implode("\n", $text),
  );

  // define default player and profiles per content type
  $form['settings']['flvmediaplayer_player'] = array(
    '#title' => t('Choose your player configuration'),
    '#type' => 'select',
    '#options' => flvmediaplayer_player_list(),
    '#default_value' => variable_get('flvmediaplayer_player', 'jw_player_4.x.inc'),
    '#description' => t('Select the type player you would like to use. This defines what kinds of options your player has. If you do not see your player on the list, this module may not support it'),
  );

  $form['settings']['flvmediaplayer_path' ] = array(
    '#title' => t('Set the URL to your player'),
    '#type' => 'textfield',
    '#default_value' => variable_get('flvmediaplayer_path', $base_path . drupal_get_path('module', 'flvmediaplayer') .'/mediaplayer.swf'),
    '#description' => t('URL to your flash player: http://yoursite.com/path/@file  Generally you want to put the @file into this module directory.', array('@file' => 'mediaplayer.swf')),
  );
  return $form;
}


/**
 * Creates the create profile form
 * @return array
 *   drupal form array
 *
 */
function flvmediaplayer_admin_settings_wizard_form_step_2($form_values) {
  drupal_set_title(t('Step 2: create a profile'));
  $text[] = theme('flvmediaplayer_admin_settings_wizard_steps', 1);
  $text[] = t('Now that you have choosen your player, you need to create a profile for your player to use.');

  $form['text'] = array(
    '#type' => 'markup',
    '#value' => implode("\n", $text),
  );

  $form['create'] = array(
    '#type' => 'fieldset',
    '#title' => t('Create new profile'),
  );
  $form['create']['name'] = array(
    '#type' => 'textfield',
    '#title' => t('Name'),
    '#description' => t('Give your profile a distinct name. Use _ for spaces if you need them.'),
  );
  return $form;
}


/**
 * Creates the profile configuration form
 * @return array
 *   drupal form array
 */
function flvmediaplayer_admin_settings_wizard_form_step_3($form_values) {
  drupal_set_title(t('Step 2: create a profile'));
  $text[] = theme('flvmediaplayer_admin_settings_wizard_steps', 2);
  $text[] = t('Now that you have created your profile, you need to configure it.');

  $form['text'] = array(
    '#type' => 'markup',
    '#value' => implode("\n", $text),
  );
  // get the profile
  $configuration = flvmediaplayer_profile_get_config($form_values['name']);
  // get the player configuration form
  $form = array_merge($form, flvmdiaplayer_get_player_form($configuration));
  // hide the player field
  $form['flvmp_player_path']['#type'] = 'hidden';

  // add the configuration name to the form
  $form['name'] = array(
    '#value' => $configuration['name'],
    '#type' => 'hidden',
  );
  // add the configuration id to the form
  $form['pid'] = array(
    '#value' => $configuration['pid'],
    '#type' => 'hidden',
  );

  return $form;
}


function flvmediaplayer_admin_settings_wizard_form_step_4($form_values) {
  drupal_set_title(t('Step 3: choose content types'));
  $text[] = theme('flvmediaplayer_admin_settings_wizard_steps', 3);
  $text[] = t('Now that you have created your profile, you need to choose which content types will use it.');

  $form['text'] = array(
    '#type' => 'markup',
    '#value' => implode("\n", $text),
  );

  // get a listing of all the content types
  $types = _node_types_build();
  $enabled_types = variable_get('flvmediaplayer_types', array());
  $form['flvmediaplayer_types'] = array(
    '#type' => 'select',
    '#multiple' => true,
    '#title' => t('Player enabled'),
    '#description' => t('Choose which content types you want to be able to use video players on.'),
    '#options' => $types[1],
    '#default_value' => $enabled_types,
  );

  return $form;
}




/**
 * Creates a list of steps for the wizard process, returns
 * a bulleted list of items, highlighting the current step
 *
 * @param int $step
 * @return string
 */
function theme_flvmediaplayer_admin_settings_wizard_steps($step = null) {
  $items = array(
    t('Choose and set the path to your player'),
    t('Create a profile to use with your player'),
    t('Configure your profile'),
    t('Set which content types will automatically use your profile'),
  );

  foreach ($items as $id => $item) {
    if ($step == $id) {
      $items[$id] = '<b>'. $items[$id] . '</b>';
    }
  }
  return theme('item_list', $items);
}


/**
 * sets the configuration status to finished so that admins can avoid the helper
 *
 */
function flvmediaplayer_admin_expert() {
  variable_set('flvmediaplayer_configuration_state', 5);
  drupal_set_message(t('The configuration helper has been turned off, you are free to configure as you please'));
  drupal_goto('admin/settings/flvmediaplayer');
}
