<?php
	
	class ue_feeds_filter_allprops extends ue_feeds_filter
	{
		public function getPropertyIds()
		{
			$args = array();
			$qry = "SELECT DISTINCT
			p.nid AS propid,
			l.nid
			FROM {content_type_property} p
			INNER JOIN {node} pn ON pn.nid = p.nid AND pn.status = 1
			LEFT OUTER JOIN {relativity} r ON r.parent_nid = p.nid
			LEFT OUTER JOIN {content_type_listing} l ON
			l.nid = r.nid
			AND l.field_unit_availability_value = 'On The Market'
			WHERE 1=1 ";
			
			if(count($this->feed->targets)) 
			{
				$qry .= " AND l.nid IN (SELECT nid FROM {ue_feeds_targets} t WHERE t.tid IN (" . db_placeholders($this->feed->targets) . ")) ";
				$args = array_merge($args, $this->feed->targets);
			}
			
			$qry .= "ORDER BY pn.nid";
			
			//$qry .= " LIMIT 0,5000";
			
			$result = db_query($qry, $args);
			
			while($row = db_fetch_array($result))
			{
				if(!$props[$row['propid']])
					$props[$row['propid']] = array();
				
				if($row['nid'])
					$props[$row['propid']][] = $row['nid'];
			}
			
			return $props;
		}
	}
	
	ue_feeds_register_filter('All properties (incl. w/o listings)', '', 'ue_feeds_filter_allprops');