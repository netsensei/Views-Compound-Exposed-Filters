<?php

class views_like_handler_filter_string extends views_handler_filter_string {

  function option_definition() {
    $options = parent::option_definition();    
    $options['expose']['contains']['vl_fields'] = array('default' => FALSE);
    return $options;
  }
  
  /**
   * Adds compound form element to the filter options. This way, we now what filters
   * we need to enable/disable in the query.
   * @param type $form
   * @param type $form_state 
   */
  function expose_form(&$form, &$form_state) {
    parent::expose_form($form, $form_state);

    $view = $this->view;
    $items = $this->view->get_items('filter', $view->current_display);    
    unset($items[$this->field]);
    
    if (!empty($items)) {
      $options = array();
      foreach ($items as $item) {        
        $field_name = substr($item['field'], 0, -6); 
        $fields = field_info_fields();
        switch ($fields[$field_name]['type']) {
          case 'text_long' :
          case 'text_with_summary' :          
            $options[$item['id']] = $item['id'];
          }
        }       
        $form['expose']['vl_fields'] = array(
          '#type' => 'checkboxes',
          '#title' => t('Compound fields'),
          '#description' => t('Select the compound fields. Any input from the exposed filter will also be passed to these.'),
          '#options' => $options,
          '#default_value' => $this->options['expose']['vl_fields'],
      );
    }
  }
  
  /**
   * Provide default options for exposed filters.
   */
  function expose_options() {
    parent::expose_options();
    $this->options['expose']['vl_fields'] = array();
  }

  /**
   * This is an extended version of the "Contains any word" operator. If a filter
   * Does not have any value assigned, it erroneously generates ((())) in the backend
   * An extra if(!empty($this->value)) prevents this from happening.
   * Does not interfere with normal working since the compound code does fill $this->value
   * at runtime. 
   * @param array $field
   * @return void if nothing was added 
   */
  function op_word($field) {
    $where = $this->operator == 'word' ? db_or() : db_and();
    
    preg_match_all('/ (-?)("[^"]+"|[^" ]+)/i', ' ' . $this->value, $matches, PREG_SET_ORDER);

    if (!empty($this->value)) {
      foreach ($matches as $match) {
        $phrase = false;
        // Strip off phrase quotes
        if ($match[2]{0} == '"') {
          $match[2] = substr($match[2], 1, -1);
          $phrase = true;
        }
        $words = trim($match[2], ',?!();:-');
        $words = $phrase ? array($words) : preg_split('/ /', $words, -1, PREG_SPLIT_NO_EMPTY);
        foreach ($words as $word) {
          $placeholder = $this->placeholder();
          $where->where("$field LIKE $placeholder", array($placeholder => '%' . db_like(trim($word, " ,!?")) . '%'));
        }
      }
      
      if (!$where) {
        return;
      }

      // previously this was a call_user_func_array but that's unnecessary
      // as views will unpack an array that is a single arg.
      $this->query->add_where($this->options['group'], $where);      
    }
  }
  
  /**
   * @todo other op's might bork as well when configured without input.
   */
  
}