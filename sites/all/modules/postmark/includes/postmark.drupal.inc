<?php
// $Id: postmark.drupal.inc,v 1.1 2010/04/27 15:30:01 alliprice Exp $

/**
 * @file
 * Implements postmark support on behalf of Drupal core.
 */
define('POSTMARKAPP_API_KEY', variable_get('postmark_api_key', ''));

if (module_load_include('php', 'postmark', 'includes/Postmark')) {
  drupal_set_message('You must include the Postmark PHP5 library to use this module. See README.txt for more details.', 'error');
}

/**
 * Send out an e-mail.
 *
 * @param $message
 *   Message array structure.
 */
function postmark_send($message) {
  static $mail;

  if (!isset($mail)) {
    $mail = new Mail_Postmark();
  }

  // Parse 'From' e-mail address.
  $address = postmark_parse_address($message['from']);

  $mail->from($address[0]['mail']);
  if ($address[0]['name'] != '') {
    $mail->fromName($address[0]['name']);
  }
  unset($message['headers']['From']);

  // Set up our debugging mode vars
  $debug_mode = variable_get('postmark_debug_mode', 0);
  $debug_email = variable_get('postmark_debug_email', '');
  if (!$debug_mode) {
    // Set recipients.
    foreach (postmark_parse_address($message['to']) as $address) {
      $mail->to($address['mail'], $address['name']);
    }
  }
  else {
    // Reroute to debug e-mail address.
    $mail->to($debug_email);
  }

  $mail->subject($message['subject']);
  if (strpos('text/plain', $message['headers']['Content-Type']) !== FALSE) {
    $mail->messagePlain($message['body']);
  }
  else {
    $mail->messageHtml($message['body']);
  }

  if ($debug_mode) {
    drupal_set_message(print_r($message, true));
  }

  if (variable_get('postmark_debug_no_send', 0)) {
    return TRUE;
  }
  else {
    if (!($result = $mail->send())) {
      watchdog('postmark', $mail->ErrorInfo, NULL, WATCHDOG_ERROR);
    }
  }
  return $result;
}

