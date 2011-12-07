<?php

/**
 * Check if mid/path is present in the menu.
 *
 * @param $in
 *   Numeric input is treated as a menu-id, strings as src-paths.
 * @return
 *   An existing mid, or 0 if none found.
 */
function token_menu_get_mid($in) {
  global $_menu;

  if (!is_numeric($in)) {
    if (isset($_menu['path index'][$in])) {
      $mid = $_menu['path index'][$in];
    }
    else {
      $mid = 0;
    }
  }
  else if (!isset($_menu['visible'][$in])) {
    $mid = 0;
  }

  // Temporary paths would break much of this module.
  if ($mid < 0) {
    $mid = 0;
  }

  return $mid;
}
