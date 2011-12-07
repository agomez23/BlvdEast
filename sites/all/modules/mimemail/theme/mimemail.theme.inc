<?php // $Id: mimemail.theme.inc,v 1.5 2010/04/21 23:26:42 jerdavis Exp $

function mimemail_theme_theme() {
  $path = drupal_get_path('module', 'mimemail') .'/theme';

  return array(
    'mimemail_message' => array(
      'arguments' => array('body' => NULL, 'mailkey' => NULL),
      'template' => 'mimemail-message',
      'pattern' => 'mimemail__',
      'file' => 'mimemail.theme.inc',
      'path' => $path,
    )
  );
}

/**
 * A preprocess function for theme('mimemail_message').
 *
 * The $variables array initially contains the following arguments:
 * - $body:  The message body
 * - $mailkey:  The mailkey associated with the message
 *
 * See includes/mimemail.tpl.php for additional variables
 */
function template_preprocess_mimemail_message(&$variables) {
  $theme = variable_get('theme_default', NULL);

  // Check for the existence of a mail.css file in the current theme folder
  if (!file_exists($styles)) {
    $styles = drupal_get_path('theme', $theme) .'/mail.css';
  }

  // If no mail.css was found, gather all style sheets
  if (!file_exists($styles)) {
    // embed a version of all style definitions
    $styles = preg_replace('|<link.*href="'. base_path()
                           .'([^"?]*)[?"].*|', '\1', drupal_get_css());
  }

  // Process each style sheet
  foreach (explode("\n", $styles) as $style) {
    if (file_exists($style)) $css .= file_get_contents($style);
  }

  // Perform some safe CSS optimizations. (derived from core CSS aggregation)
  $css = preg_replace('<
    \s*([@{}:;,]|\)\s|\s\()\s*[^\n\S] |  # Remove whitespace around separators, but keep space around parentheses and new lines between definitions.
    /\*([^*\\\\]|\*(?!/))+\*/ \s+    # Remove comments that are not CSS hacks.
    >x', '\1', $css);
  // Wordwrap to adhere to RFC821
  $css = wordwrap($css, 700);
  $variables['css'] = $css;

}
