<?php
	
	class beast_block_filter_rent extends ue_custom_block
	{
		public function view()
		{
 			if (apachesolr_has_searched())
			{
				$query = apachesolr_current_query();
				$response = apachesolr_static_response_cache();
				$facets = apachesolr_get_enabled_facets('apachesolr_money');
				
				$delta = 'field_rent';
				
				// Check for valid response and available facets
				if (empty($response) || empty($facets[$delta])) {
					return;
				}
				
				
				/*
				// Check for block visibility
				if (!apachesolr_block_visibility($query, 'apachesolr_money', $delta)) {
					return;
				}
				*/
				if ($fields = apachesolr_cck_fields()) {
					// Build the facet slider blocks for filtering
					foreach ($fields as $name => $field) {
						if ($field['field_type'] == 'money') {
							$index_key = $facets[$delta];
							$callback = FALSE;
							
							// The Money CCK field is actually a dual Amount+Currency field,
							// but the jQuery slider is only needed for the Amount subfield.
							// So We need some complex code to handle the difference:
							// the facet name ending in "_currency" is always the Currency field (we check with astrpos())
							$type = (strpos($facets[$delta], '_currency') !== FALSE) ? t('Currency') : t('Amount');
							if(strpos($facets[$delta], '_currency') == FALSE)
							{
								$content = drupal_get_form('beast_block_filter_rent_form', $index_key, $delta);
							}
						}
					}
				}
			}
			return array(
				'subject' => 'Filter by Rent',
				'content' => $content,
			);
		}
	}
	
	function beast_block_filter_rent_form($form_state, $facet, $cck)
	{

		// Get the proper user search keys
		$apachesolr_has_searched = apachesolr_has_searched();
		if (strpos($_GET['q'], 'search/apachesolr_search') !== FALSE || strpos($_GET['q'], 'search/apachesolr_ajax') !== FALSE) {
			$keys = trim(search_get_keys());
		}

		// Check the Apache Solr query values from url
		$query_values = array();
		if ($apachesolr_has_searched) {
			$query = apachesolr_current_query();
			$query_values = $query->get_url_queryvalues();
		}
		
		// Extract the current price range filter
		if (isset($_GET['filters'])) {
			$filters = $_GET['filters'];
			$extracted = apachesolr_money_filter_extract($filters, $facet);
		}
		
		$min = 1000;
		$max = 10000;
		
		$start = (isset($extracted['0']['#start']) && $extracted['0']['#start'] >= $min) ? $extracted['0']['#start'] : '';
		$end = (isset($extracted['0']['#end']) && $extracted['0']['#end'] <= $max) ? $extracted['0']['#end'] : '';
		
		//$form['#method'] = 'GET';
		//$form['#action'] = url('search/apartments');
		
		$form['min'] = array(
			'#title' => 'Price',
			'#type' => 'select',
			'#options' => array('' => 'Min'),
			'#default_value' => $start,
		);
		
		$form['max'] = array(
			'#type' => 'select',
			'#options' => array('' => 'Max'),
			'#default_value' => $end,
		);
		
		$form['apply'] = array(
			'#type' => 'submit',
			'#value' => t('Apply'),
			'#attributes' => array('class' => 'form-button-red'),
		);
		
		for($i = $min; $i <= $max; $i += ($i < 3000 ? 100 : 500))
			$form['min']['#options'][$i] = $form['max']['#options'][$i] = '$' . number_format($i, 0);
			
			
  $form['module']['#value'] == 'apachesolr_search';
  $form['#submit'] = array('beast_filter_rent_form_submit');
			
  $form['apachesolr_money']['#tree'] = TRUE;
  $form['apachesolr_money']['queryvalues'] = array(
    '#type' => 'hidden',
    '#default_value' => serialize($query_values),
  );
  $form['apachesolr_money']['get'] = array(
    '#type' => 'hidden',
    '#default_value' => serialize(array_diff_key($_GET, array('q' => 1, 'page' => 1, 'filters' => 1, 'solrsort' => 1, 'retain-filters' => 1))),
  );
  $form['apachesolr_money']['range-field'] = array(
    '#type' => 'hidden',
    '#default_value' => $facet,
  );

		  // Add Drupal JS settings
		  drupal_add_js(
			array(
			  'apacheSolrMoneyMin' => $min,
			  'apacheSolrMoneyMax' => $max,
			  'apacheSolrMoneyFrom' => $start,
			  'apacheSolrMoneyTo' => $end
			),
			'setting'
		  );
		
		return $form;
	}