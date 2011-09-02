<?php

/**
 * Implements hook_field_views_data_alter().
 * Alters the filter handler of 'text' fields. Instead of the default string, we use our
 * own slightly altered version
 * @param string $result
 * @param type $field
 * @param type $module 
 */
function views_like_field_views_data_alter(&$result, $field, $module) {
  if ($module == 'text') {
    foreach ($result as $table => $data) {
      $field_name = $field['field_name'];
      foreach ($data as $column => $value) {
        if (array_key_exists('filter', $value)) {
          $result[$table][$column]['filter']['handler'] = 'views_like_handler_filter_string';
        }
      }
    }
  }
}
