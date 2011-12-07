<?php
	define('UE_FEEDS_MANAGE_PROPERTIES_PER_PG',		20);
	define('UE_FEEDS_MANAGE_LISTING_BATCH_SIZE',	50);

	function ue_feeds_admin_manage()
	{
		
		drupal_set_title('Syndication Manager');
		
		$output = drupal_get_form('ue_feeds_admin_manage_form');
		
		return $output;
		
		
	}
	
	/* TODO: MOVE TO INSTALL */
	function ue_feeds_create_taxonomy()
	{
		$vid = variable_get('ue_feeds_target_taxonomy', 0);
		$vocabularies = taxonomy_get_vocabularies();
		if (!isset($vocabularies[$vid])) {
		
			$vocabulary = array(
				'name' => t('Feed Targets'), 
				'multiple' => true, 
				'required' => false,
				'module' => 'ue_feeds', 
			);
			taxonomy_save_vocabulary($vocabulary);
			
			variable_set('ue_feeds_target_taxonomy', $vocabulary['vid']);
		}
	}
	
	/**
	* Theme function for nodeaccess_grants_form.
	*/
	function theme_ue_feeds_admin_manage_form($form)
	{
		$output .= drupal_render($form);
		
		return $output;
	}
	
	function ue_feeds_admin_manage_form(&$form_state)
	{
		drupal_add_css(drupal_get_path('module', 'ue_feeds') .'/ue_feeds.css', 'module');
		
		$form['filters'] = ue_feeds_manage_form_filters($form_state);
		
		$form['results'] = ue_feeds_manage_form_results($form_state);
		
		$form['update'] = array(
			'#type' => 'fieldset',
			'#title' => t('Update action'),
			'#tree' => TRUE,
			'targets' => ue_feeds_targets_form('select', array('#prefix' => '<div style="float:left">', '#suffix' => '</div>')),
		);
		
		$form['update']['operation'] = array(
			'#type' => 'select',
			'#title' => t('Update action'),
			'#options' => array(
				'set-syndication' => t('Overwrite syndication targets to new selection'),
				'append-syndication' => t('Enable selected syndication targets'),
				'remove-syndication' => t('Disable selected syndication targets'),
				'clear-syndication' => t('Clear all syndication targets')
			),
			'#prefix' => '<div style="clear:left"></div>',
		);
			
		$form['update']['buttons']['update'] = array(
			'#type'  => 'submit',
			'#value' => t('Update'),
			'#submit' => array('ue_feeds_admin_manage_form_update'),
		);
		
		$form['#redirect'] = false;
		
		return $form;
	}
	
	// generates filter fieldset for syndication manager.
	function ue_feeds_manage_form_filters(&$form_state)
	{
		$form = array(
			'#type' => 'fieldset',
			'#title' => t('Filter Results'),
			'#collapsible' => TRUE,
			'#collapsed' => $form_state['clicked_button']['#value'] == t('Filter'),
			'#tree' => TRUE,
		);
		
		ue_feeds_create_taxonomy();
		
		$companies = ue_custom_get_companies();
		$companyIDs = $_SESSION['ue_feeds_manage_filter_company'];
		$companyList = array();
		
		foreach($companies as $company)
			$companyList[$company->nid] = $company->title;
		
		$form['company'] = array(
			'#type' => 'select',
			'#title' => t('Select company'),
			'#options' => $companyList,
			'#multiple' => TRUE,
			'#description' => t('Filter by company.'),
			'#default_value' => $companyIDs,
			'#prefix' => '<div style="float:left">',
			'#suffix' => '</div>',
			'#size' => 6,
		);
		
		$properties = ue_custom_get_properties('n.title', $companyIDs);
		foreach ($properties as $property)
			$propertyList[$property->nid] = $property->title;
		
		$form['property'] = array(
			'#type' => 'select',
			'#title' => t('Select property'),
			'#options' => $propertyList,
			'#multiple' => TRUE,
			'#description' => t('Filter by property.'),
			'#default_value' => $_SESSION['ue_feeds_manage_filter_property'],
			'#prefix' => '<div style="float:left">',
			'#suffix' => '</div>',
			'#size' => 6,
		);
		
		$form['target'] = ue_feeds_targets_form('select', array('#prefix' => '<div style="float:left">', '#suffix' => '</div>', '#default_value' => $_SESSION['ue_feeds_manage_filter_target']));
		
		$form['buttons']['#prefix'] = '<div style="clear:both"></div>';
			
		$form['properties_per_pg'] = array(
			'#type' => 'textfield',
			'#title' => t('Properties per page'),
			'#default_value' => isset($_SESSION['ue_feeds_manage_properties_per_pg']) ? $_SESSION['ue_feeds_manage_properties_per_pg'] : UE_FEEDS_MANAGE_PROPERTIES_PER_PG,
			'#maxlength' => 5,
			'#size' => 5,
			'#required' => TRUE,
		);
			
		$form['buttons']['filter'] = array(
			'#type'  => 'submit',
			'#value' => t('Filter'),
			'#submit' => array('ue_feeds_admin_manage_form_refine'),
		);
		$form['buttons']['reset'] = array(
			'#type' => 'submit',
			'#value' => t('Reset'),
			'#submit' => array('ue_feeds_admin_manage_form_reset'),
		);
		
		return $form;
	}
	
	function reorder($list, $selected)
	{
		foreach($list as $key => $title)
		{
			if(in_array($key, $selected))
			{
				$rtn["$key"] = $title;
				unset($list[$key]);
			}
		}
		
		foreach($list as $key => $title)
			$rtn["$key"] = $title;
		
		return $rtn;
	}
	
	// generates results fieldset for syndication manager.
	function ue_feeds_manage_form_results(&$form_state)
	{
		$result = ue_feeds_manage_form_results_properties($form_state);
			
		$form = array(
			'#type' => 'fieldset',
			'#title' => t('Results'),
			'#tree' => TRUE,
			'#attributes' => array('class' => 'manage-results')
		);
		
		$form['header'] = array(
			'#value' => '<a href="#" onclick="jQuery(\'.manage-results :checkbox\').attr({checked: true}); return false;">' . t('Check All') . '</a> - <a href="#" onclick="jQuery(\'.manage-results :checkbox\').attr({checked: false}); return false;">' . t('Check None') . '</a>'
		);
		
		$listingtotal = 0;
		
		while($row = db_fetch_object($result))
		{
			if(!isset($form['company'][$row->compid]))
				$form['company'][$row->compid] = array('#value' => '<h3>' . check_plain($row->comptitle) . '</h3>', '#prefix' => '<div class="company">', '#suffix' => '</div>');
			
			$listings = ue_feeds_manage_form_results_listings($row->propid, $form_state);
			
			$listingoptions = array();
			
			while($listingrow = db_fetch_object($listings))
				$listingoptions[$listingrow->nid] = $listingrow->title . '<br>' . ue_feeds_manage_target_tip($listingrow->nid);
			
			$form['company'][$row->companyid]['properties'][$row->propid] = array(
					'#value' => '<h4>' . l($row->proptitle, "node/$row->propid") . '</h4>' . ue_feeds_manage_target_tip($row->propid),
					'#prefix' => '<div class="building">',
					'#suffix' => '</div>',
					'listings' => array(
						'#type' => 'fieldset',
						'#collapsible' => true,
						'#collapsed' => true,
						'#title' => t('!listingcount listing(s) on the market', array('!listingcount' => count($listingoptions))),
						'listings' => array(
							'#type' => 'checkboxes',
							'#options' => $listingoptions,
						),
					),	
			);
			
			$listingtotal += count($listingoptions);
		}
		
		$form['total']['#value'] = format_plural($listingtotal, 'One listing on this page.', '@count listings on this page.');
		
		$form['pager']['#value'] = theme('pager');
		
		return $form;
	}
				
	function ue_feeds_manage_target_tip($nid)
	{
		static $js_added;
		
		$node = node_load($nid);
		$targets = ue_feeds_targets();
		
		foreach($targets as $group => $terms)
		{
			$grouptotal = 0;
			foreach($terms as $tid => $name)
			{
				if(in_array($tid, $node->targets))
				{
					$terms[$tid] = array('data' => $name, 'class' => 'active');
					$grouptotal++;
				}
			}
			$tip .= '<span class="targettip">' . $group . ' - ' . $grouptotal . '/' . format_plural(count($terms), '1 site', '@count sites') . '<span class="targets">' . theme_item_list($terms) . '</span></span>';
		}
		
		if(!$js_added)
			$js_added = drupal_add_js('jQuery(function() {jQuery(".targettip").each(function(i, el){jQuery(this).bt(jQuery(".targets", el).html());jQuery(".targets", el).hide()})});', 'inline');
		
		return $tip;
	}
	
	function ue_feeds_manage_form_results_properties(&$form_state)
	{
		if(is_array($_SESSION['ue_feeds_manage_filter_company']))
			$companyIDs = $_SESSION['ue_feeds_manage_filter_company'];
		if(is_array($_SESSION['ue_feeds_manage_filter_property']))
			$propIDs = $_SESSION['ue_feeds_manage_filter_property'];
		if(is_array($_SESSION['ue_feeds_manage_filter_target']))
			$targetIDs = $_SESSION['ue_feeds_manage_filter_target'];
		
		$select = "SELECT DISTINCT
				cnode.nid AS companyid,
				cnode.title AS companytitle,
				pnode.nid AS propid,
				pnode.title AS proptitle ";
		
		$from = "FROM {content_type_property} p
			INNER JOIN {node} pnode ON
				pnode.nid = p.nid
			INNER JOIN {node} cnode ON
				cnode.nid IN (p.field_contact_build_mgr_nid, p.field_contact_build_owner_nid, p.field_contact_lease_mgr_nid) ";
		
		$where = 'WHERE 1 = 1 ';
		
		$args = array();
		
		if(is_array($propIDs) && count($propIDs))
		{
			$where .= 'AND p.nid IN (' . db_placeholders($propIDs) . ') ';
			$args = $propIDs;
		}
		elseif(is_array($companyIDs) && count($companyIDs))
		{
			$where .= 'AND (p.field_contact_build_mgr_nid IN (' . db_placeholders($companyIDs) . ') ';
			$where .= 'OR p.field_contact_build_owner_nid IN (' . db_placeholders($companyIDs) . ') ';
			$where .= 'OR p.field_contact_lease_mgr_nid IN (' . db_placeholders($companyIDs) . ')) ';
			$args = array_merge($companyIDs, $companyIDs, $companyIDs);
		}
		
		if(is_array($targetIDs) && count($targetIDs))
		{
		
			$where .= "AND EXISTS(SELECT l.nid FROM {relativity} r INNER JOIN {content_type_listing} l ON l.nid = r.nid AND l.field_unit_availability_value = '%s' INNER JOIN {ue_feeds_targets} t ON t.nid = l.nid AND t.tid IN (" . db_placeholders($targetIDs) . ") WHERE r.parent_nid = pnode.nid) ";
			$args[] = 'On the Market';
			$args = array_merge($args, $targetIDs);
		}
		else
		{
			$where .= "AND EXISTS(SELECT l.nid FROM {relativity} r INNER JOIN {content_type_listing} l ON l.nid = r.nid AND l.field_unit_availability_value = '%s' WHERE r.parent_nid = pnode.nid) ";
			$args[] = 'On the Market';
		}
		
		$where .= 'ORDER BY cnode.title, pnode.title';
		
		$per_pg = isset($_SESSION['ue_feeds_manage_properties_per_pg']) ? $_SESSION['ue_feeds_manage_properties_per_pg'] : UE_FEEDS_MANAGE_PROPERTIES_PER_PG;
		
		return pager_query($select . $from . $where, $per_pg, 0, 'SELECT DISTINCT COUNT(*) ' . $from . $where, $args);
	}
	
	function ue_feeds_manage_form_results_listings($propID, &$form_state)
	{
		if(is_array($_SESSION['ue_feeds_manage_filter_target']))
			$targetIDs = $_SESSION['ue_feeds_manage_filter_target'];
		
		$query = "
			SELECT DISTINCT
				n.nid,
				n.title
			FROM
				relativity r
			INNER JOIN {node} n ON
				n.nid = r.nid
				AND n.type = '%s'
			INNER JOIN {content_type_listing} l ON
				l.nid = n.nid
				AND l.field_unit_availability_value = '%s'
			WHERE
				1 = 1 ";
		
		$args[] = 'listing';
		$args[] = 'On the Market';
		
		if(is_array($targetIDs) && count($targetIDs))
		{
			$query .= "AND EXISTS(SELECT t.nid FROM {ue_feeds_targets} t WHERE t.nid = r.nid AND t.tid IN (" . db_placeholders($targetIDs) . ")) ";
			$args = array_merge($args, $targetIDs);
		}
		
		$query .= "AND r.parent_nid = %d";
		
		$args[] = $propID;
		
		$result = db_query($query, $args);
		
		return $result;
	}
	
	function ue_feeds_admin_manage_form_update($form, &$form_state)
	{
		//we need to move listing IDs up the structure somehow, so we don't have to loop three levels deep to get a checked listing.
		foreach($form_state['values']['results']['company'] as $companyID => $company)
		{
			foreach($company['properties'] as $propertyID => $property)
			{
				foreach($property['listings']['listings'] as $listing => $listingID)
					if($listingID) $listingIDs[] = $listingID;
			}
		}
		
		if(is_array($form_state['values']['update']['targets']))
			$targetIDs = array_values($form_state['values']['update']['targets']);
		
		//for more than 20 invoke the batch API for good measure.
		if(count($listingIDs) > UE_FEEDS_MANAGE_LISTING_BATCH_SIZE)
		{
			//setup batch
			$batch = array(
				'finished' => 'ue_feeds_admin_manage_update_finish',
				'title' => t('Updating Syndication Targets'),
				'file' => drupal_get_path('module', 'ue_feeds') . '/includes/ue_feeds.manage.inc',
			);
			
			//add all the operations
			while(count($listingIDs))
				$batch['operations'][] = array('ue_feeds_admin_manage_update_process', array(array_splice($listingIDs, 0, UE_FEEDS_MANAGE_LISTING_BATCH_SIZE), $form_state['values']['update']['operation'], $targetIDs));
		
			batch_set($batch);
		}
		else
		{
			foreach($listingIDs as $listingID)
				_ue_feeds_admin_manage_update_helper($listingID, $form_state['values']['update']['operation'], $targetIDs);
			
			ue_feeds_admin_manage_update_finish(true, $listingIDs);
		}
		$form_state['rebuild'] = true;
	}
	
	function ue_feeds_admin_manage_update_process($listingIDs, $action, $targets, &$context)
	{
		foreach($listingIDs as $listingID)
		{
			if(_ue_feeds_admin_manage_update_helper($listingID, $action, $targets));
			{
				$context['results'][] = $listingID;
			}
		}
	}
	
	function _ue_feeds_admin_manage_update_helper($nid, $action, &$targets)
	{
		$node = node_load($nid, NULL, TRUE);
		
		switch($action)
		{
			case 'set-syndication':
				//overwrite all $node->targets.
				$node->targets = $targets;
				break;
			case 'append-syndication':
				$node->targets = array_unique(array_merge($node->targets, $targets));
				break;
			case 'remove-syndication':
				//remove all $targets from $node->targets.
				$node->targets = array_diff($node->targets, $targets);
				break;
			case 'clear-syndication':
				//clear all $node->targets.
				$node->targets = array();
				break;
		}
		
		node_save($node);
		
		return true;
	}
	
	function ue_feeds_admin_manage_update_finish($success, $results, $operations) {
		if ($success) {
			$message = format_plural(count($results), 'Set syndication targets for 1 listing.', 'Set syndication targets for @count listings.');
		}
		else
		{
			$error_operation = reset($operations);
			$message = 'An error occurred while processing ' . $error_operation[0] . ' with arguments :' . print_r($error_operation[0], TRUE);
		}
		drupal_set_message($message);
	}
	
	function ue_feeds_admin_manage_form_refine($form, &$form_state)
	{
		$form_state['rebuild'] = true;
		$form['filters']['#collapsed'] = true;
		$_SESSION['ue_feeds_manage_filter_company'] = array_values($form_state['values']['filters']['company']);
		$_SESSION['ue_feeds_manage_filter_property'] = array_values($form_state['values']['filters']['property']);
		$_SESSION['ue_feeds_manage_filter_target'] = array_values($form_state['values']['filters']['target']);
		$_SESSION['ue_feeds_manage_properties_per_pg'] = intval($form_state['values']['filters']['properties_per_pg']);
	}
	
	function ue_feeds_admin_manage_form_reset($form, &$form_state)
	{
		$form_state['rebuild'] = true;
		unset($_SESSION['ue_feeds_manage_properties_per_pg']);
		unset($_SESSION['ue_feeds_manage_filter_company']);
		unset($_SESSION['ue_feeds_manage_filter_property']);
		unset($_SESSION['ue_feeds_manage_filter_target']);
	}