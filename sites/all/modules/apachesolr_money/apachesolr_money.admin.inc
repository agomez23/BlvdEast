<?php

/**
 * @file
 * Provides the ApacheSolr Money administrative settings.
 */

/**
 * Form callback; administrative settings for Amazon.
 */
function apachesolr_money_admin_settings() {
  $form = array();
  $form['apachesolr_money_currency'] = array(
    '#type' => 'textfield',
    '#title' => t('Default currency symbol'),
    '#description' => t('Enter your site main currency symbol, like $, used for the price slider'),
    '#default_value' => variable_get('apachesolr_money_currency', '$'),
    '#maxlength' => 40,
    '#required' => TRUE,
  );
  $form['apachesolr_money_currency_code'] = array(
    '#type' => 'textfield',
    '#title' => t('Default currency code'),
    '#default_value' => variable_get('apachesolr_money_currency_code', 'USD'),
    '#maxlength' => 40,
    '#required' => TRUE,
  );
  return system_settings_form($form);
}