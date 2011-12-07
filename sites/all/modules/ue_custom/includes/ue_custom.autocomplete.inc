<?php
	/* various autocomplete callbacks. */
	
	function ue_custom_autocomplete_node($type, $name)
	{
		$result = db_query_range("SELECT nid, title FROM {node} WHERE status=1 AND type = '%s' AND LOWER(title) LIKE LOWER('%%%s%%') ORDER BY title", $type, $name, 0, 10);
		
		while($row = db_fetch_array($result)) $nodes[$row['nid']] = $row['title'];
		
		print drupal_json($nodes);
		
		exit();
	}

?>