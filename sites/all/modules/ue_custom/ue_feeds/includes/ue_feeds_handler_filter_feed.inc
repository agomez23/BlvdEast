<?php

class ue_feeds_handler_filter_feed extends views_handler_filter {
  function admin_summary() { }
  function operator_form() { }
  function query() {
    $table = $this->ensure_my_table();
    $join = new views_join();
    $join->construct('ue_feeds_nodes', $this->table_alias, 'nid', 'nid', array(), 'INNER');
    $this->query->ensure_table('ue_feeds_nodes', $this->relationship, $join);
  }
}