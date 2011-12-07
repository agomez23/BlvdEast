<?php
	
	//filter to feed all listings through for checked targets.
	class ue_feeds_filter_none extends ue_feeds_filter
	{
		public function getPropertyIds()
		{
			$args = array();
			$qry = "SELECT DISTINCT pn.nid AS propid, l.nid FROM {content_type_listing} l
			INNER JOIN {node} ln ON ln.nid = l.nid AND ln.status = 1
			INNER JOIN {relativity} r ON r.nid = ln.nid
			INNER JOIN {node} pn ON pn.nid = r.parent_nid AND pn.status = 1\n";
			
			$qry .= "WHERE l.field_unit_availability_value = 'On The Market'\n";
			
			if(count($this->feed->targets))
			{
				$qry .= " AND l.nid IN (SELECT nid FROM {ue_feeds_targets} t WHERE t.tid IN (" . db_placeholders($this->feed->targets) . ")) ";
				$args = array_merge($args, $this->feed->targets);
			}
			
			$qry .= "ORDER BY pn.nid";
			
			$result = db_query($qry, $args);
			
			while($row = db_fetch_array($result))
				$props[$row['propid']][] = $row['nid'];
			
			return $props;
		}
	}
	
	ue_feeds_register_filter('No Filter', '', 'ue_feeds_filter_none');