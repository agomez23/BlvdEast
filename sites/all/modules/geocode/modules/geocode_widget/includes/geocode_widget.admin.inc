<?php // $Id: geocode_widget.admin.inc,v 1.4 2009/12/22 19:43:37 jerdavis Exp $

/**
 * Implementation of hook_widget_settings().
 */
function geocode_widget_admin_widget_settings($op, $widget) {
  switch ($op) {
    case 'form':
      $form = array();

      // Select field(s) to geocode from, and associated handlers.
      $geo_type = str_replace('geocode_', '', $widget['type']);
      if ($fields = geocode_widget_available_fields($geo_type)) {
        $form['geocode_fields'] = array(
          '#tree' => TRUE,
          '#theme' => 'geocode_widget_settings_form',
        );
        foreach ($fields as $name => $field) {
          $form['geocode_fields'][$name] = array();
          $form['geocode_fields'][$name]['status'] = array(
            '#type' => 'checkbox',
            '#title' => $field['widget']['label'],
            '#default_value' => $widget['geocode_fields'][$name]['status'],
          );

          $handlers = $return = array();
          foreach ($field['geocode_handlers'] as $handler => $info) {
            $handlers[$handler] = isset($info['title']) ? $info['title'] : $handler;
           
            // TODO: Add magic to select the correct return type based on handler
            // $return[$handler] = $info['return types'][$geo_type];
            foreach ($info['return types']['geo'] as $type) $return[$type] = $type;
          }

          $form['geocode_fields'][$name]['handler'] = array(
            '#type' => 'select',
            '#title' => t('Geocode handler'),
            '#options' => $handlers,
            '#default_value' => $widget['geocode_fields'][$name]['handler'],
          );

          $form['geocode_fields'][$name]['return'] = array(
            '#type' => 'select',
            '#title' => t('Return value'),
            '#options' => $return,
            '#default_value' => $widget['geocode_fields'][$name]['return'],
          );
        }
      }
      else {
      }
      return $form;

    case 'save':
      return array('geocode_fields');
  }
}

function geocode_widget_available_fields($return_type) {
  static $fields;
  if (!isset($fields)) {
    $content_fields = $fields = array();

    foreach (content_fields() as $cf) {
      $content_fields[$cf['type']][$cf['field_name']] = $cf;
    }

    foreach (geocode_handler_info() as $name => $handler) {

      // Filter out handlers that don't have compatible return types.
      if (!isset($handler['return types'][$return_type])) continue;
      $handler['return types'] = array(
        $return_type => $handler['return types'][$return_type],
      );

      // Add applicable content fields to a list of candidates.
      foreach ($handler['field types'] as $type) {
        if (isset($content_fields[$type])) {
          foreach ($content_fields[$type] as $cf_name => $cf) {
            if (!isset($fields[$cf_name])) {
              $fields[$cf_name] = $cf;
            }
            $fields[$cf_name]['geocode_handlers'][$name] = $handler;
          }
        }
      }
    }

  }
  return $fields;
}

/**
 * Implementation of hook_form_FORM_ID_alter().
 */
function geocode_widget_form_content_field_edit_form_alter(&$form, $form_state) {
  if (isset($form['widget']['geocode_fields'])) {
    $info = _content_type_info();
    $fields = array_keys($info['content types'][$form['#field']['type_name']]['fields']);
    foreach(element_children($form['widget']['geocode_fields']) as $field) {
      if (!in_array($field, $fields)) {
        unset($form['widget']['geocode_fields'][$field]);
      }
    }
  }
}
