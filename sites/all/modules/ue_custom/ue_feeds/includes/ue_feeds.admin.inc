<?php

	function ue_feeds_admin_list()
	{
		drupal_set_title('Feeds');
		
		$feeds = ue_feeds_get_feeds();
		
		$output = drupal_get_form('ue_feeds_admin_list_form', $feeds);
		
		return $output;
	}
	
	function ue_feeds_admin_list_form(&$form_state, $feeds)
	{
		global $ue_feeds_adapters;
		
		$header = array(
			array('data' => t('Feed'), 'width' => '30%'),
			array('data' => t('Feed type'), 'width' => '15%'),
			array('data' => t('Last run'), 'width' => '15%'),
			array('data' => t('Operations'), 'colspan' => 7)
		);
		
		foreach ($feeds as $feed)
		{
			if($feed->dir == 0)
				$feedUrl = $feed->uri;
			else
				$feedUrl = 'feeds/' . $feed->uri;
			
			$row = array(
				$feed->name,
				$ue_feeds_adapters[$feed->dir == 0 ? 'input' : 'output'][$feed->adapterclass]['name'],
				$feed->lastrun ? format_date($feed->lastrun, 'small') : 'Never',
			);
			
			$ops = array();
			
			if(user_access(UE_FEEDS_PERM_MANAGE_FEEDS)) $ops[] = l('edit', 'admin/ue-feeds/edit/'. $feed->fid);
			if(user_access(UE_FEEDS_PERM_MANAGE_FEEDS)) $ops[] = l('delete', 'admin/ue-feeds/delete/'. $feed->fid);
			if(intval($feed->dir) == 0 && user_access(UE_FEEDS_PERM_MANAGE_FEEDS)) $ops[] = l('test', 'admin/ue-feeds/test/'. $feed->fid);
			$ops[] = l('view', $feedUrl, array('query' => time()));
			if(user_access(UE_FEEDS_PERM_MANAGE_FEEDS)) l('run', 'admin/ue-feeds/run/' . $feed->fid);
			if(stripos(url($feedUrl, array('absolute' => true)), 'http://') === 0 && user_access(UE_FEEDS_PERM_MANAGE_FEEDS)) $ops[] = l('validate XML', 'http://validator.w3.org/check?uri=' . url($feedUrl, array('absolute' => true)));
			
			$row = array_merge($row, $ops);
			
			if($feed->dir == 0)
				$inputRows[] = $row;
			else
				$outputRows[] = $row;
		}
		
		if(!$inputRows)
			$inputRows[] = array(array('data' => t('No feeds are currently defined.'), 'colspan' => count($header)));
		
		if(!$outputRows)
			$outputRows[] = array(array('data' => t('No feeds are currently defined.'), 'colspan' => count($header)));
		
		$form['inputfeeds'] = array(
			'#type' => 'fieldset',
			'#title' => t('Input feeds'),
			'#collapsible' => TRUE,
			'#tree' => TRUE,
			'feeds' => array('#value' => '<div>' . theme('table', $header, $inputRows,  array('width'=>'100%')) . '</div>'),
		);
		
		$form['outputfeeds'] = array(
			'#type' => 'fieldset',
			'#title' => t('Output feeds'),
			'#collapsible' => TRUE,
			'#tree' => TRUE,
			'feeds' => array('#value' => '<div>' . theme('table', $header, $outputRows, array('width'=>'100%')) . '</div>'),
		);
		
		$form['stats'] = array(
			'#type' => 'fieldset',
			'#title' => t('Stats'),
			'#collapsible' => TRUE,
		);
		
		$result = db_query('SELECT COUNT(*) FROM ue_feeds_files');
		
		$form['stats']['pending files'] = array(
			'#value' => l(format_plural(db_result($result), '1 pending file download.', '@count pending file downloads.'), 'admin/ue-feeds/files')
		);
		
		if(user_access(UE_FEEDS_PERM_MANAGE_FEEDS))
		{
			$form['buttons']['add'] = array(
				'#type' => 'button',
				'#value' => t('Create new feed'),
				'#attributes' => array('onClick' => 'location.href = "/admin/ue-feeds/add"; return false;')
			);
		}
		
		return $form;
	}

	function ue_feeds_admin_adapters()
	{
		global $ue_feeds_adapters;
		
		ue_feeds_load_includes();
		
		drupal_set_title('Adapters');
		
		$output = drupal_get_form('ue_feeds_admin_adapters_form', $ue_feeds_adapters);
		
		return $output;
	}
	
	function ue_feeds_admin_adapters_form(&$form_state, $adapters)
	{
		$header = array(
			array('data' => t('Adapter'), 'width' => '30%'),
			'',
		);
		
		foreach ($adapters['input'] as $adapter)
			$rows[] = array(t($adapter['name']), t($adapter['description']));
		
		if(!$rows)
			$rows[] = array('data' => t('No adapters are currently defined.'), 'colspan' => 2);
		
		$form['inputadapters'] = array(
			'#type' => 'fieldset',
			'#title' => t('Input adapters'),
			'#collapsible' => TRUE,
			'#tree' => TRUE,
			'adapters' => array('#value' => '<div>' . theme('table', $header, $rows, array('width'=>'100%')) . '</div>'),
		);
		
		$rows = array();
		
		foreach ($adapters['output'] as $adapter)
			$rows[] = array(t($adapter['name']), t($adapter['description']));
		
		if(!count($rows))
			$rows[] = array('data' => t('No adapters are currently defined.'), 'colspan' => 2);
		
		$form['outputadapters'] = array(
			'#type' => 'fieldset',
			'#title' => t('Output adapters'),
			'#collapsible' => TRUE,
			'#tree' => TRUE,
			'adapters' => array('#value' => '<div>' . theme('table', $header, $rows, array('width'=>'100%')) . '</div>'),
		);
		
		return $form;
	}
	
	function ue_feeds_admin_add()
	{
		return drupal_get_form('ue_feeds_admin_edit_form');
	}
	
	function ue_feeds_admin_edit($fid = 0)
	{
		if ($fid)
		{
			$alias = ue_feeds_load($fid);
			drupal_set_title(check_plain($alias->name));
			$output = drupal_get_form('ue_feeds_admin_edit_form', $alias);
		}
		return $output;
	}
	
	/**
	* Edit feed page.
	*/
	function ue_feeds_admin_edit_form(&$form_state, $edit = NULL)
	{
		global $ue_feeds_filters;
		if($form_state['feed']) $edit = $form_state['feed'];
		
		if(!$edit)
		{
			$edit = (object)$form_state['post'];
			
			foreach (ue_feeds_get_adapters() as $aid => $info)
				$adapters[$aid] = $info['name'];
				
			$form['adapterclass'] = array(
				'#type' => 'select',
				'#title' => t('Feed type'),
				'#default_value' => $edit->adapterclass,
				'#description' => t('The type of feed to parse.'),
				'#required' => TRUE,
				'#options' => $adapters,
			);
				
			$form['dir'] = array(
				'#type' => 'select',
				'#title' => t('Feed direction'),
				'#default_value' => 0,
				'#required' => TRUE,
				'#options' => array(t('Input'), t('Output')),
			);
			
			$form['submit'] = array('#type' => 'submit', '#value' => t('Create new feed'));
		}
		else
		{
			if($edit->dir == 1 && strlen($edit->uri) == 0)
				$edit->uri = substr(md5(rand()), 0, 10) . '.xml'; //default filename for syndication.
			
			foreach ($ue_feeds_filters as $filter => $info)
				$filters[$filter] = $info['name'];
			
			$form['#alias'] = $edit;
			
			$form['adapterclass'] = array('#type' => 'hidden', '#value' => $edit->adapterclass);
			$form['dir'] = array('#type' => 'hidden', '#value' => $edit->dir);
			
			$form['details'] = array(
				'#type' => 'fieldset', 
				'#title' => t('Feed settings'), 
				'#collapsible' => TRUE, 
			);
			
			$form['details']['name'] = array(
				'#type' => 'textfield',
				'#title' => t('Feed name'),
				'#default_value' => $edit->name,
				'#maxlength' => 60,
				'#size' => 30,
				'#description' => t('Feed name.'),
				'#required' => TRUE,
			);
			
			$form['details']['uri'] = array(
				'#type' => 'textfield',
				'#title' => t('Feed URI'),
				'#default_value' => $edit->uri,
				'#maxlength' => 1024,
				'#size' => 60,
				'#description' => t('URI for feed (can be http:// https:// or ftp://)'),
				'#required' => TRUE,
			);
			
			if($edit->dir == 1)
			{
				$form['details']['filterclass'] = array(
					'#type' => 'select', 
					'#title' => t('Feed Filter'), 
					'#default_value' => $edit->filterclass,
					'#options' => $filters,
				);
			}
			else
			{
				$form['details']['authorname'] = array(
					'#type' => 'textfield',
					'#title' => t('Default author'),
					'#maxlength' => 60,
					'#size' => 30,
					'#autocomplete_path' => 'user/autocomplete',
					'#default_value' => $edit->authorname,
				);
				
				$form['details']['companyid'] = array(
					'#type' => 'textfield',
					'#title' => t('Default company'),
					'#maxlength' => 60,
					'#size' => 30,
					'#autocomplete_path' => 'ue/autocomplete/node/company',
					'#default_value' => $edit->companyid,
				);
			}
			
			//merge adapter options into form.
			$options = $edit->adapter->getForm();
			
			$form['options'] = array(
				'#type' => 'fieldset', 
				'#title' => t('Feed options'), 
				'#collapsible' => TRUE, 
			);
			
			$form['options'] = array_merge($form['options'], $options);
			
			if($edit->dir == 1)
			{
				$form['options']['appendcontact'] = array(
					'#type' => 'checkbox',
					'#title' => t('Append contact info into listing descriptions.'),
					'#default_value' => $edit->options->appendcontact,
				);
			}
			else
			{
				$form['options']['hideunit'] = array(
					'#type' => 'checkbox',
					'#title' => t('Hide unit # in listings.'),
					'#default_value' => $edit->options->hideunit,
				);
				$form['options']['preservephone'] = array(
					'#type' => 'checkbox',
					'#title' => t('Preserve phone #.'),
					'#default_value' => $edit->options->preservephone,
				);
			}
			
			$form['targets'] = array(
				'#type' => 'fieldset', 
				'#title' => t('Feed targets'), 
				'#collapsible' => TRUE, 
				'#description' => t($edit->dir == 0 ? 'Select default targets for imported listings.' : 'Select targets for feed. When feeds are checked below, any listing must have at least the same feeds checked to be placed in this feed.'),
				'targets' => ue_feeds_targets_form('checkboxes', array('#default_value' => $edit->targets))
			);
				
			//$form['targets']['freeTargets'] = ue_feeds_admin_edit_form_targets('field_syndication_free', $edit->freeTargets, 'Free Sites', NULL, 'radios');
			//$form['targets']['paidTargets'] = ue_feeds_admin_edit_form_targets('field_syndication_paid', $edit->paidTargets, 'Paid Sites');
			//$form['targets']['premiumTargets'] = ue_feeds_admin_edit_form_targets('field_syndication_premium', $edit->premiumTargets, 'Premium Sites');
			
			if ($edit->fid)
			{
				$form['fid'] = array('#type' => 'hidden', '#value' => $edit->fid);
				$form['submit'] = array('#type' => 'submit', '#value' => t('Update feed'));
			}
			else
				$form['submit'] = array('#type' => 'submit', '#value' => t('Create new feed'));
		}
		
		return $form;
	}
	
	function ue_feeds_admin_edit_form_validate($form, &$form_state)
	{
		$name = $form_state['values']['name'];
		$uri = $form_state['values']['uri'];
		$adapter = $form_state['values']['adapterclass'];
		$fid = isset($form_state['values']['fid']) ? $form_state['values']['fid'] : 0;
	}
	
	function ue_feeds_admin_edit_form_submit($form, &$form_state)
	{
		ue_feeds_load_includes();
		
		if(empty($form_state['values']['name']))
		{
			$feed = ue_feeds_feed::factory_by_array($form_state['values']);
			
			$form_state['feed'] = $feed;
			$form_state['rebuild'] = true;
		}
		else
		{
			/*
			$targets = array_flip($form_state['values']['paidTargets']);
			unset($targets[0]);
			$form_state['values']['paidTargets'] = array_values($targets);
			
			$targets = array_flip($form_state['values']['premiumTargets']);
			unset($targets[0]);
			$form_state['values']['premiumTargets'] = array_values($targets);
			*/
			$form_state['values']['targets'] = array_keys($form_state['values']['targets']);
			
			if(strlen($form_state['values']['authorname']) > 0)
			{
				$author = user_load(array('name' => $form_state['values']['authorname']));
				$form_state['values']['authorid'] = $author->uid;
			}
			
			$feed = ue_feeds_feed::factory_by_array($form_state['values']);
			$feed->adapter->processForm($form_state);
			
			if($feed->dir == 1)
				$feed->options->appendcontact = $form_state['values']['appendcontact'];
			else
			{
				$feed->options->hideunit = $form_state['values']['hideunit'];
				$feed->options->preservephone = $form_state['values']['preservephone'];
			}
			
			$feed->save();
			
			drupal_set_message(t('The feed has been saved.'));
			$form_state['redirect'] = 'admin/ue-feeds';
		}
	}
	
	/**
	* Menu callback; confirms deleting an URL alias
	*/
	function ue_feeds_admin_delete_confirm($form_state, $fid)
	{
		$feed = ue_feeds_load($fid);
		if (user_access('administer ue feeds'))
		{
			$form['fid'] = array('#type' => 'value', '#value' => $fid);
			$output = confirm_form($form, t('Are you sure you want to delete feed %title?', array('%title' => $feed->name)),
			isset($_GET['destination']) ? $_GET['destination'] : 'admin/ue-feeds/');
		}
		return $output;
	}
	
	/**
	* Execute URL alias deletion
	*/
	function ue_feeds_admin_delete_confirm_submit($form, &$form_state)
	{
		if ($form_state['values']['confirm'])
		{
			ue_feeds_delete($form_state['values']['fid']);
			drupal_set_message(t('The feed has been deleted.'));
			$form_state['redirect'] = 'admin/ue-feeds/';
			return;
		}
	}
	
	function ue_feeds_admin_zip()
	{
		if(is_array($_POST['tid']))
		{
			db_query('DELETE FROM {ue_feeds_zips}');
			
			$sql .= 'INSERT INTO {ue_feeds_zips} (tid, zip) VALUES';
			
			foreach($_POST['tid'] as $tid => $zips)
			{
				if(strlen(trim($zips)))
					foreach(explode(',', $zips) as $i => $zip)
						$values[] = sprintf(" (%d, '%s')", $tid, trim($zip));
			}
			
			$sql .= implode(',', $values);
			
			db_query($sql);
		}
		
		$output .= '<p>' . t('This page lists all currently existing neighborhoods so we can filter on zipcodes to not add national properties. Will also provide a rough neighborhood selection when more accurate data is not available.');
		
		$result = db_query('SELECT t.*, z.zip FROM {ue_feeds_zips} z INNER JOIN {term_data} t ON t.tid = z.tid ORDER BY t.tid');
			
		while ($row = db_fetch_object($result))
		{
			if(is_array($zips[$row->tid]))
				array_push($zips[$row->tid], $row->zip);
			else
				$zips[$row->tid] = array($row->zip);
		}
		
		$neighborhoods = taxonomy_get_tree(1,0,1);
		
		$header = array(t('Neighborhood'), t('Zipcodes'));
		
		foreach ($neighborhoods as $hood)
		{
			if(is_array($zips[$hood->tid]))
				$rowZips = implode(', ', $zips[$hood->tid]);
			else
				$rowZips = '';
			
			$rows[] = array(
				'<span style="margin-left:' . (($hood->depth - 2) * 15) . 'px">' . $hood->name . '</span>',
				'<input type="text" name="tid[' . $hood->tid . ']" value="' . $rowZips . '" size="40" />'
			);
		}
		
		if(!$neighborhoods)
			$rows[] = array(array('data' => t('No feeds are currently defined.'), 'colspan' => count($header)));
		
		$output .= '<form action="' . $_SERVER['REQUEST_URI'] . '" method="POST">';
		
		$output .= theme('table', $header, $rows, array('width'=>600));
		
		$output .= '<input type="submit" value="Update table" /></form>';
		
		return $output;
	}
	
	function ordinal($n) {                                                                                                                                                   
		return $n . gmdate("S", (((abs($n) + 9) % 10) + ((abs($n / 10) % 10) == 1) * 10) * 86400);                                                                           
	}

	function ue_feeds_admin_downloads_list()
	{
		drupal_set_title('Pending Feed Files');
		
		$output = drupal_get_form('ue_feeds_admin_downloads_list_form');
		
		return $output;
	}
	
	function ue_feeds_admin_downloads_list_form(&$form_state)
	{
		$result = pager_query('SELECT ff.nid, ff.field, ff.url, f.* FROM {ue_feeds_files} ff INNER JOIN {files} f ON f.fid = ff.fid ORDER BY ff.nid, ff.field, f.timestamp DESC', 100, 0);
		
		$header = array(
			'Node',
			'Field',
			'File',
		);
		
		while($file = db_fetch_object($result))
		{
			$node = node_load($file->nid);
			
			$rows[] = array(
				l($node->title, $node->nid),
				$file->field,
				l($file->url, $file->url),
			);
		}
		
		$form['files'] = array('#value' => theme('table', $header, $rows,  array('width'=>'100%')));
		$form['pager']['#value'] = theme('pager');
		
		return $form;
	}

?>