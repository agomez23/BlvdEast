<?php

/**
 * Implementation of hook_token_list().
 */
function beast_token_list($type = 'all') {
  if ($type == 'user') {
    $tokens = array();

    $tokens['user']['first-name']  = t('User first name, or login name when unavailable');

    return $tokens;
  }
}

/**
 * Implementation of hook_token_values().
 */
function beast_token_values($type, $object = NULL) {
  if ($type == 'user') {
    $tokens = array();

    $tokens['first-name']  = $object->profile_firstname ? $object->profile_firstname : $object->name;

    return $tokens;
  }
}
