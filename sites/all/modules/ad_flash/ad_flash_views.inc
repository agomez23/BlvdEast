<?php
// $Id: ad_flash_views.inc,v 1.3 2010/02/02 14:24:44 alexandrascu Exp $

/**
 * @file
 * Adds views support for the ad_flash.module
 *
 *
 * Based on the ad_image.module by Jeremy Andrews
 *
 * Contributed by 
 * 
 * 2007
 *   Fabio Varesano <fvaresano at yahoo dot it> 
 *
 * 2005-2007.
 *   Jeremy Andrews <jeremy@kerneltrap.org>.
 * 
 * 2010 
 *    Alexandru Mihnea Andrascu | Visual Candy <office@visualcandy.eu>
 */

function ad_flash_views_tables() {
  return array(
    'ad_flash' => array(
      'name' => 'ad_flash',
      'join' => array(
        'left' => array(
          'table' => 'ads',
          'field' => 'aid',
        ),
        'right' => array(
          'field' => 'aid',
        ),
      ),
      'fields' => array(
        'url' => array(
          'name' => 'Flash Ad: Click-through URL',
          'handler' => 'ad_handler_field_url',
          'option' => array(
            '#type' => 'select',
            '#options' => array(
              'link' => t('As link'),
              'nolink' => t('Not as link'),
            ),
          ),
        ),
        'aid' => array(
          'name' => 'Flash Ad: Flash File',
          'handler' => 'ad_flash_handler_field_flash',
          'option' => array(
            '#type' => 'select',
            '#options' => array(
              'linkflash' => t('Linked Flash File'),
              'image' => t('Unlinked Flash File'),
              'path' => t('Flash file path'),
            ),
          ),
        ),
      ),
    ),
  );
}

function ad_flash_handler_field_flash($fieldinfo, $fielddata, $value, $data) {
  if ($fielddata['options'] == 'linkflash') {
    $ad = db_fetch_object(db_query("SELECT a.redirect, f.filepath FROM {ads} a JOIN {ad_flash} i JOIN {files} f ON a.aid = i.aid AND i.aid = f.nid AND i.fid = f.fid WHERE i.aid = %d AND a.adtype = 'flash'", $value));
    if ($ad) {
      ad_statistics_increment($value, 'view');
      return "<a href=\"$ad->redirect\"><img src=\"$ad->filepath\" /></a>";
    }
  }
  else if ($fielddata['options'] == 'flash') {
    $ad = db_fetch_object(db_query("SELECT f.filepath FROM {ad_flash} i JOIN {files} f ON i.aid = f.nid AND i.fid = f.fid WHERE i.aid = %d AND a.adtype = 'flash'", $value));
    if ($ad) {
      ad_statistics_increment($value, 'view');
      return "<img src=\"$ad->filepath\" />";
    }
  }
  else {
    $ad = db_fetch_object(db_query("SELECT f.filepath FROM {ad_flash} i JOIN {files} f ON i.aid = f.nid AND i.fid = f.fid AND i.aid = %d", $value));
    return $ad->filepath;
  }
}
