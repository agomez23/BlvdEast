<?php
	
	//filter to select all properties and their listings marked for syndication that are not OMG clients
	class ue_feeds_filter_nonomg extends ue_feeds_filter
	{
		public function getPropertyIds()
		{
			$qry = "SELECT pn.nid AS propid, l.nid FROM content_type_listing l
			INNER JOIN node ln ON ln.nid = l.nid AND ln.status = 1
			INNER JOIN relativity r ON r.nid = ln.nid
			INNER JOIN node pn ON pn.nid = r.parent_nid AND pn.status = 1 AND pn.nid NOT IN (SELECT f.content_id FROM {flag_content} f WHERE f.fid = 3)";
			
			$targetNum = 1;
			foreach($this->feed->premiumTargets as $target)
			{
				$qry .= "INNER JOIN content_field_syndication_premium target$targetNum ON target$targetNum.nid = l.nid AND target$targetNum.field_syndication_premium_value = '$target'\n";
				$targetNum++;
			}
			foreach($this->feed->paidTargets as $target)
			{
				$qry .= "INNER JOIN content_field_syndication_paid target$targetNum ON target$targetNum.nid = l.nid AND target$targetNum.field_syndication_paid_value = '$target'\n";
				$targetNum++;
			}
			
			$qry .= "WHERE l.field_unit_availability_value = 'On The Market'\n";
			
			// only feed free listings if no paid targets are selected.
			//if(count($this->feed->paidTargets) + count($this->feed->premiumTargets) == 0) $qry .= "AND l.field_syndication_free_value = 1\n";
			
			$qry .= "ORDER BY pn.nid";
			
			$result = db_query($qry);
			
			while($row = db_fetch_array($result))
			{
				if(!$properties[$row['propid']])
					$properties[$row['propid']] = new ue_feeds_property(node_load($row['propid']), false);
				
				$properties[$row['propid']]->listings[] = new ue_feeds_listing(node_load($row['nid']));
			}
			
			while($row = db_fetch_array($result))
				$props[$row['propid']][] = $row['nid'];
			
			return $props;
		}
	}
	
	ue_feeds_register_filter('Non-OMG', '', 'ue_feeds_filter_nonomg');