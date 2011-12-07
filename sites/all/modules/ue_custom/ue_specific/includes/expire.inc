<?
	function ue_custom_cron_expire()
	{
		$sql = "
		SELECT n.nid FROM {node} n
		INNER JOIN {content_type_listing} l ON l.nid = n.nid AND field_unit_availability_value = 'On the Market'
		INNER JOIN relativity r ON r.nid = n.nid
		WHERE type = 'listing' AND changed < %d
		ORDER BY r.parent_nid";
		
		$nodes = db_query($sql, strtotime('-2 weeks'));
		
		$header = array(t('Unit #'), t('Owner'));
		
		while($nid = db_result($nodes))
		{
			$listing = node_load($nid, NULL);
			$prop = node_load($listing->parent_node, NULL);
			
			if($prop->field_contact_build_owner[0]['nid'])
				$company = node_load($prop->field_contact_build_owner[0]['nid']);
			elseif($prop->field_contact_build_mgr[0]['nid'])
				$company = node_load($prop->field_contact_build_mgr[0]['nid']);
			elseif($prop->field_contact_lease_mgr[0]['nid'])
				$company = node_load($prop->field_contact_lease_mgr[0]['nid']);
			
			$rows["$prop->nid"] = array(
				array('data' => l($prop->title, 'node/' . $prop->nid), 'style' => 'font-weight:bold'),
				l($company->title, 'node/' . $company->nid)
			);
			
			$rows[$prop->nid . '-' . $listing->nid] = array(
				l($listing->title, 'node/' . $listing->nid),
				''
			);
		}
		
		$output .= '<h1>Listings Expired on ' . variable_get('site_name', '') . '</h1>';
		
		$output .= theme('table', $header, $rows, array('width'=>'100%'));
		
		mimemail(variable_get('site_name') . ' <' . variable_get('site_mail') . '>', 'darrin@urbanedgeny.com', 'expired', $output);
		
		return $output;
	}
?>