<?php
	
	class beast_block_search_apartments extends ue_custom_block
	{
		public function view()
		{
			$content = drupal_get_form('beast_block_search_apartments_form');
			return array(
				'subject' => 'Apartments',
				'content' => $content,
			);
		}
	}
	
	function beast_block_search_apartments_form($form_state)
	{
		$form['#method'] = 'GET';
		$form['#action'] = url('search/apartments');
		$form['#attributes']['class'] = 'blvd-search blvd-search-apartments';
		
		$settings['keywordDefault'] = t('Enter Complex Name, Address, or Web ID');
		
		drupal_add_js(drupal_get_path('module', 'beast') . '/js/search.js');
		drupal_add_js(array('blvdSearch' => array('apartments' => $settings)), 'setting');
		
		$bedterms = taxonomy_get_tree(variable_get('beast_taxonomy_bed_tid', 7));
		$bathterms = taxonomy_get_tree(variable_get('beast_taxonomy_bath_tid', 14));
		$townterms = taxonomy_get_tree(variable_get('beast_taxonomy_town_tid', 1));
		$petterms = taxonomy_get_tree(variable_get('beast_taxonomy_pet_tid', 12));
		$viewterms = taxonomy_get_tree(variable_get('beast_taxonomy_views_tid', 8));
		$parkingterms = taxonomy_get_tree(variable_get('beast_taxonomy_parking_tid', 11));
		$outdoorterms = taxonomy_get_tree(variable_get('beast_taxonomy_outdoor_tid', 20));
		
		$form['keywords'] = array(
			'#type' => 'textfield',
			'#title' => '',
			'#default_value' => $settings['keywordDefault'],
			'#attributes' => array('class' => 'keyword'),
		);
		//print_r($form['keywords']);
		
		$form['col1'] = array('#prefix' => '<div class="form-col form-left">', '#suffix' => '<div class="clear"></div></div>');
		$form['col2'] = array('#prefix' => '<div class="form-col form-right">', '#suffix' => '<div class="clear"></div></div><div class="clear"></div>');
		
		$form['col1']['bldg'] = array(
			'#type' => 'select',
			'#title' => t('Building'),
			'#options' => beast_building_options(true),
		);
		
		$form['col2']['price'] = array(
			'#prefix' => '<div class="price">',
			'#suffix' => '</div>'
		);
		
		$form['col2']['price']['min'] = array(
			'#title' => 'Price',
			'#type' => 'select',
			'#options' => array('' => 'Min'),
		);
		
		$form['col2']['price']['max'] = array(
			'#type' => 'select',
			'#options' => array('' => 'Max'),
		);
		
		for($i = 1000; $i < 10000; $i += ($i < 3000 ? 100 : 500))
			$form['col2']['price']['min']['#options'][$i] = $form['col2']['price']['max']['#options'][$i] = '$' . number_format($i, 0);
		
		$form['col1']['bedrooms'] = array(
			'#type' => 'select',
			'#title' => t('Bedrooms'),
			'#options' => beast_terms_to_options($bedterms),
		);
		
		$form['col1']['bathrooms'] = array(
			'#type' => 'select',
			'#title' => t('Bathrooms'),
			'#options' => beast_terms_to_options($bathterms),
		);
		
		$form['col2']['town'] = array(
			'#type' => 'select',
			'#title' => t('Town'),
			'#options' => beast_terms_to_options($townterms),
		);
		
		$petoptions = beast_terms_to_options($petterms);
		unset($petoptions[91], $petoptions[90]);
		
		/*
		$form['col2']['pets'] = array(
			'#type' => 'select',
			'#title' => t('Pets'),
			'#options' => $petoptions,
		);
		*/
		
		/*
		$form['col1']['parking'] = array(
			'#type' => 'checkboxes',
			'#title' => t('Parking'),
			'#options' => beast_terms_to_options($parkingterms),
			'#attributes' => array('class' => 'parking-opts'),
		);
		print_r($form['col1']['parking']['#options']); exit;
		$form['col1']['parking']['#options'][0] = 'Available';
		*/
		
		$form['col1']['parking'] = array(
			'#type' => 'checkboxes',
			'#title' => t('Parking'),
			'#options' => array(''=>'&nbsp;'),
			'#attributes' => array('class' => 'parking-opts'),
		);
		
		$form['col2']['views'] = array(
			'#type' => 'select',
			'#title' => t('Views'),
			'#options' => beast_terms_to_options($viewterms),
		);
		
		/*
		$form['col2']['outdoor'] = array(
			'#type' => 'select',
			'#title' => t('Outdoor Space'),
			'#options' => beast_terms_to_options($outdoorterms),
		);
		*/
		
		$form['col2']['pets'] = array(
			'#type' => 'checkboxes',
			'#title' => t('Pets'),
			'#options' => array(''=>'&nbsp;'),
			'#attributes' => array('class' => 'pet-opts'),
		);
		//print_r($form['col2']['pets']);
		
		$form['col2']['search'] = array(
			'#type' => 'submit',
			'#value' => t('Search'),
			'#attributes' => array('class' => 'form-button-red'),
		);
		
		return $form;
	}