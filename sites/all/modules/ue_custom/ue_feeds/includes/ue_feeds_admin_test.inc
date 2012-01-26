<?php

	function ue_feeds_admin_test($fid)
	{
		ini_set('max_execution_time', '60'); //bump timeout up to 5 mins.
		
		$feed = ue_feeds_load($fid);
		$output = drupal_get_form('ue_feeds_admin_test_form', $feed);
		
		drupal_set_title($feed->name); //reset our title.
		
		return $output;
	}

	//Menu callback. Draws the grant tab.
	function ue_feeds_admin_test_form(&$form_state, $feed)
	{
		$form_values =& $form_state['values'];
		
		if(!$form_values) $form_values = array();
		
		$parsed = $feed->adapter->parse();
		
		$properties = $parsed['properties'];
		
		$form['fid'] = array('#type' => 'value', '#value' => $feed->fid);
		$form['eid'] = array('#tree' => TRUE);
		
		foreach ($properties as $prop)
		{
			$hood = taxonomy_get_term($prop->hood);
			
			$row = array(
				$prop->name,
				$prop->addr1 . ', ' . $prop->city . ', ' . $prop->state . ' ' . $prop->zip,
				$hood->name
			);
			
			$form['eid'][$prop->externalID]['#value'] = $prop->name;
			$form['addr'][$prop->externalID]['#value'] = $prop->addr1 . ', ' . $prop->city . ', ' . $prop->state . ' ' . $prop->zip;
			$form['hood'][$prop->externalID]['#value'] = $hood->name;
			
			$form['nid']['nid-' . $prop->externalID] = array('#type' => 'textfield', '#autocomplete_path' => 'autocomplete/ue-feeds/test');
			
			if($feed->nodes[$prop->externalID])
			{
				$propNode = node_load($feed->nodes[$prop->externalID]);
				//$form['nid'][$prop->externalID] = array('#type' => 'hidden', '#value' => $propNode->id);
				$form['nid']['nid-' . $prop->externalID]['#default_value'] = $propNode->title . ' [' . $propNode->nid . ']';
				$form['eid'][$prop->externalID]['#nodes'][] = $propNode;
				$form['match'][$prop->externalID]['#value'] = theme_image(drupal_get_path('theme', 'ue') . '/images/icons/accept.png');
			}
			else
			{
				
				
				//this builds a search phrase and adds in an OR with the ordinal (st, th, etc) to the street if appropriate.
				//ie "307 East 81 Street" becomes "307 East 81 OR 81st Street"
				$searchPhrase = explode(' ', $prop->name);
				//if(intval($searchPhrase[2]) > 0 && strlen($searchPhrase[2]) == strlen(intval($searchPhrase[2])))
				//	array_splice($searchPhrase, 3, 0, array('OR', ue_feeds_ordinal($searchPhrase[2])));
				
				$searchPhrase = implode(' ', $searchPhrase);
				
				$vMatch = views_get_view('property_match');
				
				$vMatch->set_exposed_input(array('keys' => $searchPhrase));
				$vMatch->execute();
				
				if(count($vMatch->result))
				{
					$form['nid']['nid-' . $prop->externalID]['#default_value'] = $vMatch->result[0]->node_title . ' [' . $vMatch->result[0]->nid . ']';
					$form['match'][$prop->externalID]['#value'] = theme_image(drupal_get_path('theme', 'ue') . '/images/icons/error.png');
				}
				else
					$form['match'][$prop->externalID]['#value'] = theme_image(drupal_get_path('theme', 'ue') . '/images/icons/cross.png');
			}
		}
		
		
		if(count($parsed['properties']) > 0)
		{
			
			$props = array_values($parsed['properties']);
			$sampleProp = $props[rand(0, count($parsed['properties'])-1)];
			$sampleNode = $sampleProp->toNode();
			
			$sampleHtml .= '<p>Successfully loaded ' . count($parsed['properties']) . ' properties.</p>';
			$sampleHtml .= '<h3>Sample Property:</h3>';
			$sampleHtml .= '<div style="width:660px;overflow:auto;border:solid 2px #999;padding:10px">';
			$sampleHtml .= '<div class="marker" style="text-align:center;margin: 0 0 .5em">(photos, management companies, and listings are not shown in this view)</div>';
			$sampleHtml .= '<h1 class="title">' . $sampleNode->title . '</h1><div class="clear"></div>';
			$sampleHtml .= node_view($sampleNode, FALSE, TRUE, FALSE) . '</div>';
			$sampleHtml .= '<textarea rows="10" cols="40">' . print_r($sampleProp, true) . '</textarea>';
			$sampleHtml .= '<textarea rows="10" cols="40">' . print_r($sampleNode, true) . '</textarea>';
			
			$form['sample'] = array('#type' => 'fieldset', '#collapsible' => true, '#collapsed' => true, '#title' => 'Sample Property', '#value' => $sampleHtml);
		}
		
		$form['save'] = array('#type' => 'submit', '#value' => t('Save'));
		
		return $form;
	}
	
	/**
	* Theme function for nodeaccess_grants_form.
	*/
	function theme_ue_feeds_admin_test_form($form)
	{
		$header = array(array('data' => t('Building Name'), 'style' => 'width:16%'), array('data' => t('Address'), 'style' => 'width:16%'), array('data' => t('Neighborhood'), 'style' => 'width:16%'), array('data' => t('Matched'), 'style' => 'text-align:center'), array('data' => t('Matched Name'), 'style' => 'width:16%')); //, array('data' => t('Address'), 'style' => 'width:16%'), array('data' => t('Neighborhood'), 'style' => 'width:16%')

		$properties = element_children($form['eid']);
		
		if(count($properties) > 0)
		{
			foreach ($properties as $key)
			{
				$row = array(
					drupal_render($form['eid'][$key]),
					drupal_render($form['addr'][$key]),
					drupal_render($form['hood'][$key]),
				);
				
				$row[] = array('data' => drupal_render($form['match'][$key]), 'style' => 'text-align:center');
				
				$row[] = drupal_render($form['nid']['nid-' . $key]);
				$rows[] = $row;
			}
		}
		else
			$rows[] = array(array('data' => t('No properties were found.'), 'colspan' => count($header)));
		
		$output .= theme('table', $header, $rows, array('width'=>'100%'));
		
		$output .= drupal_render($form);
		
		return $output;
	}

	function ue_feeds_admin_test_form_submit($form, &$form_values)
	{
		$feed = ue_feeds_load($form_values['values']['fid']);
		
		foreach($form_values['values'] as $key => $value)
		{
			if(strpos($key, 'nid-') === 0)
			{
				$nid = intval(substr($value, strrpos($value, '[') + 1));
				if($nid > 0)
					$mapping[substr($key, 4)] = $nid;
			}
		}
		
		$feed->nodes = $mapping;
		
		$feed->save();
	}
	
	function ue_feeds_admin_test_autocomplete($name)
	{
		$nodes = array();
		
		$vMatch = views_get_view('property_match');
		
		$vMatch->set_exposed_input(array('keys' => $name));
		$vMatch->execute();
		
		foreach($vMatch->result as $match)
			$nodes[$match->node_title . ' [' . $match->nid . ']'] = $match->node_title;
		
		drupal_json($nodes);
	}
