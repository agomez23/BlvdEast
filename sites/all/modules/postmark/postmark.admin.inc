<?php
// $Id: postmark.admin.inc,v 1.1 2010/04/27 15:30:01 alliprice Exp $

/**
 * @file
 *
 * Administration include for Postmark.
 */

/**
 * Setting form for Postmark
 */
function postmark_settings_form() {
  // It makes sense to set the smtp_library value as
  // per PHPMailer as both modules may be in place
  // and so the settings could work together as opposed
  // to overlapping, which would mean the heaviest
  // (in weight) module wins
  $smtp_library = variable_get('smtp_library', '');
  if (variable_get('postmark_enabled', 0)) {
    if (!strpos($smtp_library, 'postmark')) {
      variable_set('smtp_library', drupal_get_filename('module', 'postmark'));
    }
  }
  else {
    if (strpos($smtp_library, 'postmark')) {
      variable_del('smtp_library');
    }
  }

  // Enable the Postmark library for sending emails
  $form['postmark_enabled'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable Postmark to send Drupal emails'),
    '#default_value' => variable_get('postmark_enabled', 0),
    '#description' => t('Use Postmark to act as your mail handler.'),
  );
  // The user's Postmark API key
  $form['postmark_api_key'] = array(
    '#type' => 'textfield',
    '#title' => t('Postmark API key'),
    '#default_value' => variable_get('postmark_api_key', ''),
    '#description' => t('You Postmark API key similar to : ed742D75-5a45-49b6-a0a1-5b9ec3dc9e5d (taken from the API page)'),
    '#required' => TRUE,
  );

  // Debug settings fieldset
  $form['debug'] = array(
    '#type' => 'fieldset',
    '#title' => t('Debugging'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['debug']['postmark_debug_mode'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable Postmark debugging'),
    '#default_value' => variable_get('postmark_debug_mode', 0),
    '#description' => t('Use Postmark debugging, just to display what is contained in the Drupal $message variable (this will be extended to include more debugging options)'),
  );
  $form['debug']['postmark_debug_email'] = array(
    '#type' => 'textfield',
    '#title' => t('Enable Postmark debugging email'),
    '#default_value' => variable_get('postmark_debug_email', variable_get('site_mail', '')),
    '#description' => t('Use a debugging email, so all system emails will go to this address. Debugging mode must be on for this to work'),
  );
  $form['debug']['postmark_debug_no_send'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable Postmark debugging to not send an email and therefore not use a credit'),
    '#default_value' => variable_get('postmark_debug_no_send', 0),
    '#description' => t('Disable credit usage when debugging'),
  );

  // If an address is submitted, send a test email message.
  $test_address = variable_get('test_address', '');
  if ($test_address != '') {
    drupal_set_message(t('A test e-mail has been sent to %email.  You may want to <a href="!watchdog">check the logs</a> for any error messages.', array('%email' => $test_address, '!watchdog' => url('admin/reports/dblog'))));
    drupal_mail('postmark', 'test', $test_address, NULL);
    variable_del('test_address');
  }
  $form['test'] = array(
    '#type' => 'fieldset',
    '#title' => t('Test configuration'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['test']['test_address'] = array(
    '#type' => 'textfield',
    '#title' => t('Recipient'),
    '#default_value' => '',
    '#description' => t('Enter a valid email address to send a test email.'),
  );
  return system_settings_form($form);
}

/**
 * Implementation of hook_mail().
 */
function postmark_mail($key, &$message, $params) {
  $message['subject'] = t('Postmark Test Run Email');
  $message['body'][] = t('Your site is properly configured to send emails using the Postmark library.');
}
