<?php

/**
 * @file
 * Creates an image count handler for galleries.
 */

class node_gallery_views_handler_image_count extends views_handler_field_numeric {
  function query() {
    // No query here, move along please.
  }

  function render($values) {
    $node = node_load($values->nid);
    $gid = NULL;
    if (in_array($node->type, (array)node_gallery_get_types('gallery'))) {
      $gid = $node->nid;
    }
    elseif (in_array($node->type, (array)node_gallery_get_types('image'))) {
      $gid = $node->gid;
    }
    $value = $gid ? node_gallery_get_image_count($gid) : '';

    // The following code is copy/pasted from views_handler_field_numeric.inc
    if (!empty($this->options['set_precision'])) {
      $value = number_format($value, $this->options['precision'], $this->options['decimal'], $this->options['separator']);
    }
    else {
      $remainder = abs($value) - intval(abs($value));
      $value = $value > 0 ? floor($value) : ceil($value);
      $value = number_format($value, 0, '', $this->options['separator']);
      if ($remainder) {
        // The substr may not be locale safe.
        $value .= $this->options['decimal'] . substr($remainder, 2);
      }
    }

    // Check to see if hiding should happen before adding prefix and suffix.
    if ($this->options['hide_empty'] && empty($value) && ($value !== 0 || $this->options['empty_zero'])) {
      return '';
    }

    return check_plain($this->options['prefix'] . $value . $this->options['suffix']);

  }
}
